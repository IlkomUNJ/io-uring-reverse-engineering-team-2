// SPDX-License-Identifier: GPL-2.0
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/mm.h>
#include <linux/slab.h>
#include <linux/eventfd.h>
#include <linux/eventpoll.h>
#include <linux/io_uring.h>
#include <linux/io_uring_types.h>

#include "io-wq.h"
#include "eventfd.h"

struct io_ev_fd {
	struct eventfd_ctx	*cq_ev_fd;
	unsigned int		eventfd_async;
	/* protected by ->completion_lock */
	unsigned		last_cq_tail;
	refcount_t		refs;
	atomic_t		ops;
	struct rcu_head		rcu;
};

enum {
	IO_EVENTFD_OP_SIGNAL_BIT,
};

/**
 * Called when the last reference to an io_ev_fd is dropped. Releases the
 * eventfd context and frees the io_ev_fd structure.
 */
static void io_eventfd_free(struct rcu_head *rcu)
{
	struct io_ev_fd *ev_fd = container_of(rcu, struct io_ev_fd, rcu);

	eventfd_ctx_put(ev_fd->cq_ev_fd);
	kfree(ev_fd);
}

/**
 * Decrements the reference count and schedules deferred freeing if this
 * was the last reference.
 */
static void io_eventfd_put(struct io_ev_fd *ev_fd)
{
	if (refcount_dec_and_test(&ev_fd->refs))
		call_rcu(&ev_fd->rcu, io_eventfd_free);
}

/**
 * Signals the eventfd with the appropriate mask and drops a reference
 * to the io_ev_fd structure.
 */
static void io_eventfd_do_signal(struct rcu_head *rcu)
{
	struct io_ev_fd *ev_fd = container_of(rcu, struct io_ev_fd, rcu);

	eventfd_signal_mask(ev_fd->cq_ev_fd, EPOLL_URING_WAKE);
	io_eventfd_put(ev_fd);
}

/**
 * Drops a reference to the io_ev_fd if requested and unlocks the RCU read lock.
 */
static void io_eventfd_release(struct io_ev_fd *ev_fd, bool put_ref)
{
	if (put_ref)
		io_eventfd_put(ev_fd);
	rcu_read_unlock();
}

/*
 * Returns true if the caller should put the ev_fd reference, false if not.
 */
/**
 * Signals the eventfd if allowed by the current context. If not, schedules
 * the signal to be performed later. Returns true if the caller should drop
 * the reference, false otherwise.
 */
static bool __io_eventfd_signal(struct io_ev_fd *ev_fd)
{
	if (eventfd_signal_allowed()) {
		eventfd_signal_mask(ev_fd->cq_ev_fd, EPOLL_URING_WAKE);
		return true;
	}
	if (!atomic_fetch_or(BIT(IO_EVENTFD_OP_SIGNAL_BIT), &ev_fd->ops)) {
		call_rcu_hurry(&ev_fd->rcu, io_eventfd_do_signal);
		return false;
	}
	return true;
}

/*
 * Trigger if eventfd_async isn't set, or if it's set and the caller is
 * an async worker. If ev_fd isn't valid, obviously return false.
 */
/**
 * Returns true if the eventfd should be triggered based on async state
 * and worker context.
 */
static bool io_eventfd_trigger(struct io_ev_fd *ev_fd)
{
	if (ev_fd)
		return !ev_fd->eventfd_async || io_wq_current_is_worker();
	return false;
}

/*
 * On success, returns with an ev_fd reference grabbed and the RCU read
 * lock held.
 */
/**
 * Returns a reference to the io_ev_fd structure if signaling is enabled
 * and the eventfd is valid. Acquires the RCU read lock.
 */
static struct io_ev_fd *io_eventfd_grab(struct io_ring_ctx *ctx)
{
	struct io_ev_fd *ev_fd;

	if (READ_ONCE(ctx->rings->cq_flags) & IORING_CQ_EVENTFD_DISABLED)
		return NULL;

	rcu_read_lock();

	/*
	 * rcu_dereference ctx->io_ev_fd once and use it for both for checking
	 * and eventfd_signal
	 */
	ev_fd = rcu_dereference(ctx->io_ev_fd);

	/*
	 * Check again if ev_fd exists in case an io_eventfd_unregister call
	 * completed between the NULL check of ctx->io_ev_fd at the start of
	 * the function and rcu_read_lock.
	 */
	if (io_eventfd_trigger(ev_fd) && refcount_inc_not_zero(&ev_fd->refs))
		return ev_fd;

	rcu_read_unlock();
	return NULL;
}

/**
 * Signals the eventfd if it is enabled and valid for the given context.
 */
void io_eventfd_signal(struct io_ring_ctx *ctx)
{
	struct io_ev_fd *ev_fd;

	ev_fd = io_eventfd_grab(ctx);
	if (ev_fd)
		io_eventfd_release(ev_fd, __io_eventfd_signal(ev_fd));
}

/**
 * Signals the eventfd only if the completion queue tail has advanced since
 * the last signal, ensuring notifications are only sent for new events.
 */
void io_eventfd_flush_signal(struct io_ring_ctx *ctx)
{
	struct io_ev_fd *ev_fd;

	ev_fd = io_eventfd_grab(ctx);
	if (ev_fd) {
		bool skip, put_ref = true;

		/*
		 * Eventfd should only get triggered when at least one event
		 * has been posted. Some applications rely on the eventfd
		 * notification count only changing IFF a new CQE has been
		 * added to the CQ ring. There's no dependency on 1:1
		 * relationship between how many times this function is called
		 * (and hence the eventfd count) and number of CQEs posted to
		 * the CQ ring.
		 */
		spin_lock(&ctx->completion_lock);
		skip = ctx->cached_cq_tail == ev_fd->last_cq_tail;
		ev_fd->last_cq_tail = ctx->cached_cq_tail;
		spin_unlock(&ctx->completion_lock);

		if (!skip)
			put_ref = __io_eventfd_signal(ev_fd);

		io_eventfd_release(ev_fd, put_ref);
	}
}

/**
 * Registers an eventfd for completion queue notifications. Returns 0 on
 * success or a negative error code on failure.
 */
int io_eventfd_register(struct io_ring_ctx *ctx, void __user *arg,
			unsigned int eventfd_async)
{
	struct io_ev_fd *ev_fd;
	__s32 __user *fds = arg;
	int fd;

	ev_fd = rcu_dereference_protected(ctx->io_ev_fd,
					lockdep_is_held(&ctx->uring_lock));
	if (ev_fd)
		return -EBUSY;

	if (copy_from_user(&fd, fds, sizeof(*fds)))
		return -EFAULT;

	ev_fd = kmalloc(sizeof(*ev_fd), GFP_KERNEL);
	if (!ev_fd)
		return -ENOMEM;

	ev_fd->cq_ev_fd = eventfd_ctx_fdget(fd);
	if (IS_ERR(ev_fd->cq_ev_fd)) {
		int ret = PTR_ERR(ev_fd->cq_ev_fd);

		kfree(ev_fd);
		return ret;
	}

	spin_lock(&ctx->completion_lock);
	ev_fd->last_cq_tail = ctx->cached_cq_tail;
	spin_unlock(&ctx->completion_lock);

	ev_fd->eventfd_async = eventfd_async;
	ctx->has_evfd = true;
	refcount_set(&ev_fd->refs, 1);
	atomic_set(&ev_fd->ops, 0);
	rcu_assign_pointer(ctx->io_ev_fd, ev_fd);
	return 0;
}

/**
 * Unregisters the eventfd and releases its reference. Returns 0 on success
 * or -ENXIO if no eventfd was registered.
 */
int io_eventfd_unregister(struct io_ring_ctx *ctx)
{
	struct io_ev_fd *ev_fd;

	ev_fd = rcu_dereference_protected(ctx->io_ev_fd,
					lockdep_is_held(&ctx->uring_lock));
	if (ev_fd) {
		ctx->has_evfd = false;
		rcu_assign_pointer(ctx->io_ev_fd, NULL);
		io_eventfd_put(ev_fd);
		return 0;
	}

	return -ENXIO;
}
