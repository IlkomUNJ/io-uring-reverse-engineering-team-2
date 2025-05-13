// SPDX-License-Identifier: GPL-2.0
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/file.h>
#include <linux/mm.h>
#include <linux/slab.h>
#include <linux/namei.h>
#include <linux/io_uring.h>
#include <linux/fsnotify.h>

#include <uapi/linux/io_uring.h>

#include "io_uring.h"
#include "sync.h"

struct io_sync {
	struct file			*file;
	loff_t				len;
	loff_t				off;
	int				flags;
	int				mode;
};

/* Prepares a sync_file_range operation by extracting the offset, length, 
 * and sync_range_flags from the submission queue entry (SQE). Marks the 
 * request as requiring asynchronous execution. 
 * Returns -EINVAL if unsupported fields are set in the SQE.
*/
int io_sfr_prep(struct io_kiocb *req, const struct io_uring_sqe *sqe)
{
	struct io_sync *sync = io_kiocb_to_cmd(req, struct io_sync);

	if (unlikely(sqe->addr || sqe->buf_index || sqe->splice_fd_in))
		return -EINVAL;

	sync->off = READ_ONCE(sqe->off);
	sync->len = READ_ONCE(sqe->len);
	sync->flags = READ_ONCE(sqe->sync_range_flags);
	req->flags |= REQ_F_FORCE_ASYNC;

	return 0;
}

/* Performs the actual sync_file_range() system call, which synchronizes a file range with storage. 
 * This call must run in a blocking context. The result of the syscall is stored in the request.
 */
int io_sync_file_range(struct io_kiocb *req, unsigned int issue_flags)
{
	struct io_sync *sync = io_kiocb_to_cmd(req, struct io_sync);
	int ret;

	/* sync_file_range always requires a blocking context */
	WARN_ON_ONCE(issue_flags & IO_URING_F_NONBLOCK);

	ret = sync_file_range(req->file, sync->off, sync->len, sync->flags);
	io_req_set_res(req, ret, 0);
	return IOU_OK;
}

/*
 * Prepares an fsync operation by validating and extracting the fsync_flags, offset, and length from the SQE. 
 * Only IORING_FSYNC_DATASYNC is accepted as a valid flag. Marks the request as asynchronous.
 */
int io_fsync_prep(struct io_kiocb *req, const struct io_uring_sqe *sqe)
{
	struct io_sync *sync = io_kiocb_to_cmd(req, struct io_sync);

	if (unlikely(sqe->addr || sqe->buf_index || sqe->splice_fd_in))
		return -EINVAL;

	sync->flags = READ_ONCE(sqe->fsync_flags);
	if (unlikely(sync->flags & ~IORING_FSYNC_DATASYNC))
		return -EINVAL;

	sync->off = READ_ONCE(sqe->off);
	sync->len = READ_ONCE(sqe->len);
	req->flags |= REQ_F_FORCE_ASYNC;
	return 0;
}

/**
 * Executes an fsync or fdatasync operation via vfs_fsync_range() 
 * to flush file data and/or metadata to disk. Always runs in a blocking context. 
 * The flush range is based on off and len.
 */
int io_fsync(struct io_kiocb *req, unsigned int issue_flags)
{
	struct io_sync *sync = io_kiocb_to_cmd(req, struct io_sync);
	loff_t end = sync->off + sync->len;
	int ret;

	/* fsync always requires a blocking context */
	WARN_ON_ONCE(issue_flags & IO_URING_F_NONBLOCK);

	ret = vfs_fsync_range(req->file, sync->off, end > 0 ? end : LLONG_MAX,
				sync->flags & IORING_FSYNC_DATASYNC);
	io_req_set_res(req, ret, 0);
	return IOU_OK;
}

/**
 * Prepares a fallocate operation by validating and extracting offset, length, and mode from the SQE. 
 * Returns -EINVAL if invalid fields are set. Marks the request as asynchronous.
 */
int io_fallocate_prep(struct io_kiocb *req, const struct io_uring_sqe *sqe)
{
	struct io_sync *sync = io_kiocb_to_cmd(req, struct io_sync);

	if (sqe->buf_index || sqe->rw_flags || sqe->splice_fd_in)
		return -EINVAL;

	sync->off = READ_ONCE(sqe->off);
	sync->len = READ_ONCE(sqe->addr);
	sync->mode = READ_ONCE(sqe->len);
	req->flags |= REQ_F_FORCE_ASYNC;
	return 0;
}

/**
 * Performs the fallocate() system call to allocate space in a file without initializing data. 
 * It notifies the filesystem of file modification after successful allocation. 
 * Always requires blocking context.
 */
int io_fallocate(struct io_kiocb *req, unsigned int issue_flags)
{
	struct io_sync *sync = io_kiocb_to_cmd(req, struct io_sync);
	int ret;

	/* fallocate always requiring blocking context */
	WARN_ON_ONCE(issue_flags & IO_URING_F_NONBLOCK);

	ret = vfs_fallocate(req->file, sync->mode, sync->off, sync->len);
	if (ret >= 0)
		fsnotify_modify(req->file);
	io_req_set_res(req, ret, 0);
	return IOU_OK;
}
