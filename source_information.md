# Task 1: Information about io_uring source
List in this section source and headers of io_uring. For each of the C source/header, you must put description what's the prime responsibily of the source. Take notes, description of the source should be slightly technical like the example given. 


## Source
There are 35 source listed in io_uring folder:


### advice.c
Store io_madvice & io_fadvice structures, both have the same exact attributes. Which make them basically the same thing. Except function body treat them as separate. Codes which make use of io_madvice are guarded by compilation macro, which make its relevant functions only active if the build flag is set. But functions that make use of io_fadvice are active all the time. The exact difference between io_madvice & io_fadvice will only known after exploring do_madvise function for io_madvice & vfs_fadvise function for io_fadvice. 


### alloc_chace.c
store the full function for organizing alloc chace which include init, new, and free. The function works as it's name implies.


### cancel.c
Contain cancelling procedure for io_uring. There is procedure cancelling with will run with matches criteria, and there are some helper function related to some function. This code is support async and sync operation paths. Including poll cancel, waitid cancel, futex cancel. furthermore, those function must be studied more deeply.


### epol.c
This file contain only used if the flag is set. Store io_epoll struct which initiatet with function io_kiocb_to_cmd. The function handle io_epoll_ctl.


### eventfd.c
Store io_ev_fd structure and an enum with value IO_EVENTFD_OP_SIGNAL_BIT. Control eventfd with io_eventfd_register to sets an eventfd and remove with io_eventfd_unregister. Send a signal with certain condition then call the rcu and free ev_fd after send.


### fdinfo.c
Active if there is flag set (CONFIG_PROC_FS). Showing cred and fdinfo another function is tracking the show of fdinfo. 


### filetable.c
The code is part of a system that manages files in the context of `io_uring`. It deals with allocating and managing file slots, tracking which files are used, and handling operations related to those files. The `io_file_bitmap_get` function searches for an available file slot, while `io_alloc_file_tables` and `io_free_file_tables` handle the allocation and freeing of file tables. The `io_install_fixed_file` and `__io_fixed_fd_install` functions are responsible for installing files into fixed slots, and `io_fixed_fd_remove` removes files from those slots. Finally, `io_register_file_alloc_range` allows for defining a range of file slots that can be allocated. These functions help manage file resources in `io_uring` efficiently, allowing users to interact with files in a non-blocking way while ensuring proper resource management and cleanup.


### fs.c
This code seems to handle file operations like renaming, deleting, making directories, and creating links, using io_uring. It defines some structures like io_rename, io_unlink, io_mkdir, and io_link, which help group the necessary information to perform these operations. Each of them keeps track of things like file descriptors, file names, and extra options like flags or permissions. There are functions to prepare each operation (like io_renameat_prep) that extract inputs and set things up, and others to actually do the operation (like io_renameat) using lower-level functions like do_renameat2. Cleanup functions (like io_renameat_cleanup) are used to free any memory or resources that were temporarily needed. The naming of the functions and structures helps tell you what part of the file system task they deal with, and most functions follow a similar pattern: prepare, act, then clean up.


### futex.c
The io_futex_cancel function is responsible for canceling in-flight futex-related I/O operations submitted via io_uring. Based on the name, it handles cleanup and notification for asynchronous futex operations that may still be pending in the kernel. It takes a pointer to the io_ring_ctx (the per-ring context structure managing io_uring state) and an io_cancel_data structure, which holds identifying information for the request to be canceled. It searches the ctx->futex_list, which is a hash list of all futex-related io_kiocb requests (the internal representation of an I/O request in io_uring), checking for a match based on user data or broader cancelation flags (IORING_ASYNC_CANCEL_ANY, IORING_ASYNC_CANCEL_ALL). Upon finding a matching request, it invokes __io_futex_cancel, which either dequeues the futex wait (futex_unqueue) or marks a futexv (vector wait) as claimed and completed. The cancelation completes the request with -ECANCELED and schedules any task work via io_req_task_work_add. Relevant structures include struct io_kiocb, struct io_futex, and struct io_futex_data (which embeds a struct futex_q representing the kernel futex waiter), showing how each async futex request is tracked, canceled, and cleaned up within io_uring's framework.


### io-wq.c
Implements the core logic for the io_uring worker thread pool, including worker creation, scheduling, cancellation, and affinity management. Handles the queuing and execution of asynchronous work items, manages worker lifecycle, and enforces concurrency and hashing constraints for safe parallel execution. Provides mechanisms for worker wakeup, sleep, and exit, as well as cancellation of pending and running work.


### io_uring.c
Implements the main io_uring kernel interface, managing submission and completion rings, request lifecycle, and system call entry points. Handles initialization, teardown, memory management, request submission, completion event posting, and cancellation. Coordinates with worker threads, manages task context, and enforces synchronization and memory ordering between user and kernel space for efficient asynchronous I/O operations.


### kbuf.c
Implements management of provided buffers for io_uring operations, including buffer selection, registration, removal, and recycling. Handles both classic and ring-mapped buffer groups, supports dynamic buffer allocation, and ensures safe concurrent access and cleanup. Provides mechanisms for user-space buffer registration, buffer lookup, and integration with the io_uring submission and completion paths.


### memmap.c
Implements memory mapping and region management for io_uring, including allocation, pinning, and freeing of memory regions used for submission/completion rings and user-provided buffers. Handles both kernel and user memory, supports safe mapping/unmapping, and provides architecture-specific logic for MMU and no-MMU systems. Ensures proper accounting, validation, and cleanup of memory regions associated with io_uring contexts.


### msg_ring.c
Implements support for io_uring's MSG_RING operations, enabling message passing and file descriptor transfer between io_uring instances. Handles preparation, validation, and execution of MSG_RING commands, including data delivery and file descriptor installation. Manages context locking, remote task work, and ensures safe resource cleanup and error handling for inter-ring communication.


### napi.c
Implements NAPI (New API) busy-polling integration for io_uring, enabling efficient polling of network receive queues within the io_uring context. Manages NAPI ID registration, tracking, and removal, supports both static and dynamic tracking modes, and provides mechanisms for busy-poll loop execution, timeout handling, and cleanup of stale entries. Integrates with io_uring's wait queues to optimize network I/O latency and responsiveness.


### net.c
Implements network-related operations for io_uring, including asynchronous send, receive, accept, connect, bind, listen, and socket creation. Handles preparation and execution of network syscalls, manages message headers and buffer selection, supports multishot and zero-copy modes, and integrates with io_uring's completion and notification mechanisms for efficient non-blocking network I/O.


### nop.c
Implements the no-operation (NOP) command for io_uring, providing a lightweight request that can be used for testing, benchmarking, or as a placeholder in submission queues. Handles preparation and execution of NOP requests, supports optional result injection, file and buffer association, and integrates with io_uring's completion and error handling mechanisms.


### notif.c
Implements notification and zero-copy completion handling for io_uring, including allocation and management of notification requests, user buffer accounting, and integration with network stack zero-copy mechanisms. Handles completion callbacks, notification chaining, and resource cleanup, ensuring correct synchronization and memory management for asynchronous notification events.


### opdef.c
Implements the operation definition tables and initialization for io_uring operations, containing two primary structures: `io_issue_defs[]` and `io_cold_defs[]`. The `io_issue_defs[]` array defines operation properties including preparation handlers, issue functions, flags (like `needs_file`, `iopoll`, `audit_skip`), and async operation sizes for each supported opcode. The `io_cold_defs[]` array provides metadata, cleanup handlers, and failure functions for operations. Handles operation validation through `io_uring_op_supported()` and initializes the operation tables during system startup via `io_uring_optable_init()`. Critical for maintaining the supported operation registry and enforcing operation-specific requirements and behaviors in the io_uring subsystem.


### openclose.c
Implements file opening and closing operations for io_uring, managing three key structures: `io_open`, `io_close`, and `io_fixed_install`. Handles both traditional file operations (`openat`, `openat2`) and fixed file descriptor operations specific to io_uring.


### poll.c
Implements polling mechanics for io_uring operations, providing infrastructure for monitoring file descriptors and events.


### register.c
Implements registration and management operations for io_uring's various resources and configurations


### rsrc.c
Implements resource management and buffer handling for io_uring operations, focusing on memory mapping, buffer registration, and resource accounting.


### rw.c
Implements read and write operations for io_uring


### splice.c
Implements splice and tee operations for io_uring.


### sqpoll.c
Implements submission queue polling functionality for io_uring, managing kernel-side polling threads that offload submission processing from applications.


### statx.c
Implements extended file status operations for io_uring.


### sync.c
Implements fsync, sync_file_range, and fallocate support for io_uring. Uses a shared io_sync struct to hold parameters like offset, length, and flags. Each operation parses and validates the SQE, then calls the corresponding VFS function. All operations require blocking context and are marked with REQ_F_FORCE_ASYNC.


### tctx.c
Manages task-specific io_uring context, including initialization, management, and cleanup. It allocates resources like work queues and reference counters for tasks and handles task-ring associations. The file also manages registered file descriptors to reduce overhead during io_uring operations, using synchronization mechanisms like mutexes and atomic counters for resource handling.


### timeout.c
This file handles timeout management for io_uring requests. It defines structures and functions to manage timeouts, including linked timeouts, multiple shot timeouts, and timeout cancellation. Key components include functions for starting, updating, completing, and flushing timeouts, as well as handling linked request failures. The hrtimer mechanisms are used for managing precise timeout events.


### truncate.c
This file handles file truncation in io_uring. io_ftruncate_prep prepares the truncation request, validating input and extracting the truncation length. io_ftruncate performs the truncation using the do_ftruncate system call, managing error checking and result setting. It integrates with the file management system and includes relevant kernel headers.


### uring_cmd.c
This file handles the execution and cleanup of io_uring commands, particularly asynchronous commands. It defines functions for preparing, executing, and completing commands with support for cancellation, handling socket options (GET/SET), and managing asynchronous data. Key operations include setting up command data, submitting commands, and handling deferred completions. It also provides functions for canceling and cleaning up commands, managing command-related resources, and interacting with sockets for network operations.


### waitid.c
This file manages asynchronous waitid notifications in io_uring. It handles request setup, cancellation, and completion for waitid, synchronizing with wait queues and ensuring safe resource cleanup. The file supports both regular and compatibility systems for signal information, and provides functions for reference management, process status changes, and handling cancellations.


### xattr.c
This file manages asynchronous file extended attribute (xattr) operations in io_uring. It prepares and executes getxattr and setxattr requests, handling both file-based and path-based operations. The file is responsible for setting up, cleaning up, and managing memory for xattr contexts, ensuring proper error handling and resource management during attribute retrieval or modification.


### zcrx.c
This file manages Zero-Copy Receive (ZCRX) operations in io_uring, including setting up receive buffers and interface queues. It handles registration and unregistration of ZCRX interfaces, and provides functions for receiving data through sockets. The file uses conditional compilation to enable ZCRX features based on kernel configuration.



## Headers
There are 37 headers:


### advise.h
- int io_madvise_prep(struct io_kiocb *req, const struct io_uring_sqe *sqe);
  - Purpose: Prepares a madvise operation for a given request (req) based on the provided submission queue entry (sqe).
  - Parameters:
	- struct io_kiocb *req: The I/O request structure.
	- const struct io_uring_sqe *sqe: The submission queue entry containing the operation details.
  - Return Value:
	Returns an integer indicating success (0) or an error code. 
- int io_madvise(struct io_kiocb *req, unsigned int issue_flags);
  - Purpose: Executes the madvise operation for the given request (req).
  - Parameters:
    - struct io_kiocb *req: The I/O request structure.
    - unsigned int issue_flags: Flags indicating how the operation should be issued.
  - Return Value: Returns an integer indicating success (0) or an error code.
- int io_fadvise_prep(struct io_kiocb *req, const struct io_uring_sqe *sqe);
  - Purpose: Prepares a fadvise operation for a given request (req) based on the provided submission queue entry (sqe).
  - Parameters:
    - struct io_kiocb *req: The I/O request structure.
    - const struct io_uring_sqe *sqe: The submission queue entry containing the operation details.
  - Return Value: Returns an integer indicating success (0) or an error code.
- int io_fadvise(struct io_kiocb *req, unsigned int issue_flags);
  - Purpose: Executes the fadvise operation for the given request (req).
  - Parameters:
    - struct io_kiocb *req: The I/O request structure.
    - unsigned int issue_flags: Flags indicating how the operation should be issued.
  - Return Value: Returns an integer indicating success (0) or an error code.


### alloc_chace.h
- void io_alloc_cache_free(struct io_alloc_cache *cache,
			 void (*free)(const void *));
  - Purpose: Frees all cached objects in the allocation cache using the provided free function
  - Parameters:
    - struct io_alloc_cache *cache: The allocation cache to free
    - void (*free)(const void *): A function pointer to free individual objects in the cache
  - Return Value: None (void)
- bool io_alloc_cache_init(struct io_alloc_cache *cache,
			 unsigned max_nr, unsigned int size,
			 unsigned int init_bytes);
  - Purpose: Initializes the allocation cache with the specified parameters
  - Parameters:
    - struct io_alloc_cache *cache: The allocation cache to initialize
    - unsigned max_nr: The maximum number of objects the cache can hold
    - unsigned int size: The size of each object in the cache
    - unsigned int init_bytes: The number of bytes to initialize in each object
  - Return Value: Returns true if the cache was successfully initialized, false otherwise
- void *io_cache_alloc_new(struct io_alloc_cache *cache, gfp_t gfp);
  - Purpose: Allocates a new object when the cache is empty
  - Parameters:
    - struct io_alloc_cache *cache: The allocation cache to allocate from
    - gfp_t gfp: The GFP (Get Free Pages) flags for memory allocation
  - Return Value: A pointer to the newly allocated object, or NULL if allocation fails
- static inline void io_alloc_cache_kasan(struct iovec **iov, int *nr)
  - Purpose: Frees and nullifies cached objects when KASAN (Kernel Address Sanitizer) is enabled. Ensures memory safety by clearing potentially poisoned objects
  - Parameters:
    - struct iovec **iov: A pointer to the I/O vector to free
    - int *nr: A pointer to the number of cached objects
  - Return Value: None (void)
- static inline bool io_alloc_cache_put(struct io_alloc_cache *cache,
				      void *entry)
  - Purpose: Adds an object back to the cache if there is space available
  - Parameters:
    - struct io_alloc_cache *cache: The allocation cache to add the object to
    - void *entry: The object to add to the cache.
  - Return Value: Returns true if the object was successfully added to the cache, false otherwise
- static inline void *io_alloc_cache_get(struct io_alloc_cache *cache)
  - Purpose: Retrieves an object from the cache if available
  - Parameters:
    - struct io_alloc_cache *cache: The allocation cache to retrieve the object from
  - Return value: A pointer to the retrieved object, or NULL if the chace is empty
- static inline void *io_cache_alloc(struct io_alloc_cache *cache, gfp_t gfp)
  - Purpose: Allocates an object from the cache. If the cache is empty, it falls back to allocating a new object
  - Parameters:
    - struct io_alloc_cache *cache: The allocation cache to allocate from
    - gfp_t gfp: The GFP (Get Free Pages) flags for memory allocation
  - Return Value: A pointer to the allocated object, or NULL if allocation fails.


### cancel.h
- int io_async_cancel_prep(struct io_kiocb *req, const struct io_uring_sqe *sqe);
  - Purpose: Prepares an asynchronous cancel operation for a given request (req) based on the provided submission queue entry (sqe)
  - Parameters:
    - struct io_kiocb *req: The I/O request structure
    - const struct io_uring_sqe *sqe: The submission queue entry containing the operation details
  - Return Value: Returns an integer indicating success (0) or an error code
- int io_async_cancel(struct io_kiocb *req, unsigned int issue_flags);
  - Purpose: Executes an asynchronous cancel operation for the given request (req)
  - Parameters:
    - struct io_kiocb *req: The I/O request structure
    - unsigned int issue_flags: Flags indicating how the operation should be issued
  - Return Value: Returns an integer indicating success (0) or an error code
- int io_try_cancel(struct io_uring_task *tctx, struct io_cancel_data *cd,
		  unsigned int issue_flags);
  - Purpose: Attempts to cancel a request based on the provided cancel data (cd) and task context (tctx)
  - Parameters:
    - struct io_uring_task *tctx: The task context associated with the request
    - sturct io_cancel _data *cd: The cancel data containing details about the request to cancel
    - unsigned int issue flags: Flags indicatin how the operation should be issued
  - Return Value: Returns an integer indicating success (0) or an error code 
- int io_sync_cancel(struct io_ring_ctx *ctx, void __user *arg);
  - Purpose: Performs a sychronous cancel operation for a request in the given context (ctx)
  - Parameters:
    - struct io_ring_ctx *ctx: the I/O ring context associated with the request
    - void __user *arg: A user-space pointer containing the cancel arguments
  - Return Value: returns an integer indication success (0) or an error code
- bool io_cancel_req_match(struct io_kiocb *req, struct io_cancel_data *cd);
  - Purpose: checks if a given request (req) matches the cancel data (cd)
  - Parameters: struct io_kiocb *req: the I/O request structure
  - struct io_cancel_data *cd: the cancel data containing details about the request to match
  - Return Value: returns true if the request matches the cancel data, false otherwise
- static inline bool io_cancel_match_sequence(struct io_kiocb *req, int sequence)
  - Purpose: checks if a given request (req) mathces a specific cancel sequence. if not, it sets the sequence for the request
  - Parameters:
    - struct io_kiocb *req: the I/O request structure
    - int sequence: the cancel sequence to match
    - Return value: return true if then request matches the sequence, false otherwise


### epol.h
- int io_epoll_ctl_prep(struct io_kiocb *req, const struct io_uring_sqe *sqe);
  - purpose: prepares an epoll_ctl operation for a given request (req) based on the provided submission queue entry (sqe)
  - parameters:
    - struct io_kiocb *req: the i/o request structure
    - const struct io_uring_sqw *sqe: the submission queue entry containing the operation details
  - return value: returns an integer indication success (0) or an error code
- int io_epoll_ctl(struct io_kiocb *req, unsigned int issue_flags);
  - purpose: executes an epoll_ctl operation for the given request (req)
  - parameters:
    - struct io_kiocb *req: the i/o request structure
    - unsigned int issue_flags: flags indicating how the operation should be issued
  - return value: return an integer success (0) or an error code

function bellow not available at version v6.14
- int io_epoll_wait_prep(struct io_kiocb *req, const struct io_uring_sqe *sqe);
- int io_epoll_wait(struct io_kiocb *req, unsigned int issue_flags);


### eventfd.h
- int io_eventfd_register(struct io_ring_ctx *ctx, void __user *arg,
			unsigned int eventfd_async);
  - purpose: registers an eventfd with the given io_ring_ctx context
  - parameters:
    - struct io_ring ctx *ctx: the i/o ring context to associate with the eventfd
    - void __user *arg: a user-space pointer containing the eventfd arguments
    - unsigned int eventfd_async: a flag indicating wheter the eventfd should operate aysnchrounously
    - return value: returns an integer indicating success (0) or an error code
- int io_eventfd_unregister(struct io_ring_ctx *ctx);
  - purpose: unregisters an eventfd from the gicven io_ring_ctx context
  - parameters:
    -  struct io_ring_ctx *ctx: the i/o ring context from which the eventfd should be unregistered
 - return value: returns an integer indicating success (0) or an error code

- void io_eventfd_flush_signal(struct io_ring_ctx *ctx);
  - purpose: flushes and processes pending signals for the eventfd associated with the given io_ring_ctx context
  - parameters:
    - struct io_ring_ctx *ctx: the i/o ring context whose eventfd signals should be flushed
  - return value: none (void)
- void io_eventfd_signal(struct io_ring_ctx *ctx);
  - purpose: sends a signal to the eventfd associated with the given context
  - parameteres
    - struct io_ring_ctx *ctx: the o/o ring context whose eventfd should be signaled
  - return value: none (void)


### fdinfo.h
- void io_uring_show_fdinfo(struct seq_file *m, struct file *f);
  - purpose: displays information about a file descriptor associated with io_uring in a sequential file
  - parameters:
    - struct seq_file *m: the sequential file where the file descriptor information will be written
    - struct file *f: the file description whose information is to be displayed
  - return value: none (void)


### filetable.h
- bool io_alloc_file_tables(struct io_ring_ctx *ctx, struct io_file_table *table, unsigned nr_files);
  - purpose: Allocates file tables for the given io_ring_ctx context
  - parameters:
    - struct io_ring_ctx *ctx: The I/O ring context
    - struct io_file_table *table: The file table to allocate
    - unsigned nr_files: The number of files to allocate in the table
  - Return Value: Returns true if the allocation was successful, false otherwise.
- void io_free_file_tables(struct io_ring_ctx *ctx, struct io_file_table *table);
  - Purpose: Frees the file tables associated with the given io_ring_ctx context.
  - Parameters:
    - struct io_ring_ctx *ctx: The I/O ring context.
    - struct io_file_table *table: The file table to free.
  - Return Value: None (void).
- int io_fixed_fd_install(struct io_kiocb *req, unsigned int issue_flags,
			struct file *file, unsigned int file_slot);
  - Purpose: Installs a fixed file descriptor into the file table for the given request.
  - Parameters:
    - struct io_kiocb *req: The I/O request structure.
    - unsigned int issue_flags: Flags indicating how the operation should be issued.
    - struct file *file: The file to install.
    - unsigned int file_slot: The slot in the file table where the file should be installed.
  - Return Value: Returns an integer indicating success (0) or an error code.
- int __io_fixed_fd_install(struct io_ring_ctx *ctx, struct file *file,
				unsigned int file_slot);
  - Purpose: Installs a fixed file descriptor into the file table for the given context.
  - Parameters:
    - struct io_ring_ctx *ctx: The I/O ring context.
    - struct file *file: The file to install.
    - unsigned int file_slot: The slot in the file table where the file should be installed.
  - Return Value: Returns an integer indicating success (0) or an error code.
- int io_fixed_fd_remove(struct io_ring_ctx *ctx, unsigned int offset);
  - Purpose: Removes a fixed file descriptor from the file table for the given context.
  - Parameters:
    - struct io_ring_ctx *ctx: The I/O ring context.
    - unsigned int offset: The offset in the file table where the file should be removed.
  - Return Value: Returns an integer indicating success (0) or an error code.
- int io_register_file_alloc_range(struct io_ring_ctx *ctx,
				 struct io_uring_file_index_range __user *arg);
  - Purpose: Allocates a range of file slots in the file table for the given context.
  - Parameters:
    - struct io_ring_ctx *ctx: The I/O ring context.
    - struct io_uring_file_index_range __user *arg: A user-space pointer containing the range arguments.
  - Return Value: Returns an integer indicating success (0) or an error code.
- io_req_flags_t io_file_get_flags(struct file *file);
  - Purpose: Retrieves the flags associated with a given file.
  - Parameters:
    - struct file *file: The file whose flags are to be retrieved.
  - Return Value: Returns the flags as an io_req_flags_t value.
- static inline void io_file_bitmap_clear(struct io_file_table *table, int bit)
  - Purpose: Clears a specific bit in the file table's bitmap and updates the allocation hint.
  - Parameters:
    - struct io_file_table *table: The file table whose bitmap is to be modified.
    - int bit: The bit to clear.
  - Return Value: None (void).
- static inline void io_file_bitmap_set(struct io_file_table *table, int bit)
  - Purpose: Sets a specific bit in the file table's bitmap and updates the allocation hint.
  - Parameters:
    - struct io_file_table *table: The file table whose bitmap is to be modified.
    - int bit: The bit to set.
  - Return Value: None (void)
- static inline unsigned int io_slot_flags(struct io_rsrc_node *node)
  - Purpose: Retrieves the slot flags for a given resource node.
  - Parameters:
    - struct io_rsrc_node *node: The resource node whose slot flags are to be retrieved.
  - Return Value: Returns the slot flags as an unsigned integer.
- static inline struct file *io_slot_file(struct io_rsrc_node *node)
  - Purpose: Retrieves the file associated with a given resource node.
  - Parameters:
    - struct io_rsrc_node *node: The resource node whose file is to be retrieved.
  - Return Value: Returns a pointer to the file.
- static inline void io_fixed_file_set(struct io_rsrc_node *node,
				     struct file *file)
  - Purpose: Sets a file in a resource node and updates its flags.
  - Parameters:
    - struct io_rsrc_node *node: The resource node where the file is to be set.
    - struct file *file: The file to set.
  - Return Value: None (void).
- static inline void io_file_table_set_alloc_range(struct io_ring_ctx *ctx,
						 unsigned off, unsigned len)
  - Purpose: Sets the allocation range for the file table in the given context.
  - Parameters:
    - struct io_ring_ctx *ctx: The I/O ring context.
    - unsigned off: The starting offset of the allocation range.
    - unsigned len: The length of the allocation range.
  - Return Value: None (void).


### fs.h
- int io_renameat_prep(struct io_kiocb *req, const struct io_uring_sqe *sqe);
  - Purpose: Prepares a renameat operation for a given request (req) based on the provided submission queue entry (sqe).
  - Parameters:
    - struct io_kiocb *req: The I/O request structure.
    - const struct io_uring_sqe *sqe: The submission queue entry containing the operation details.
  - Return Value: Returns an integer indicating success (0) or an error code.

- int io_renameat(struct io_kiocb *req, unsigned int issue_flags);
  - Purpose: Executes a renameat operation for the given request (req).
  - Parameters:
    - struct io_kiocb *req: The I/O request structure.
    - unsigned int issue_flags: Flags indicating how the operation should be issued.
  - Return Value: Returns an integer indicating success (0) or an error code.
- void io_renameat_cleanup(struct io_kiocb *req);
  - Purpose: Cleans up resources associated with a renameat operation.
  - Parameters:
    - struct io_kiocb *req: The I/O request structure.
  - Return Value: None (void)

- int io_unlinkat_prep(struct io_kiocb *req, const struct io_uring_sqe *sqe);
  - Purpose: Prepares an unlinkat operation for a given request (req) based on the provided submission queue entry (sqe).
  - Parameters:
    - struct io_kiocb *req: The I/O request structure.
    - const struct io_uring_sqe *sqe: The submission queue entry containing the operation details.
  - Return Value: Returns an integer indicating success (0) or an error code.
- int io_unlinkat(struct io_kiocb *req, unsigned int issue_flags);
  - Purpose: Executes an unlinkat operation for the given request (req).
  - Parameters:
    - struct io_kiocb *req: The I/O request structure.
    - unsigned int issue_flags: Flags indicating how the operation should be issued.
  - Return Value: Returns an integer indicating success (0) or an error code.
- void io_unlinkat_cleanup(struct io_kiocb *req);
  - Purpose: Cleans up resources associated with an unlinkat operation.
  - Parameters:
    - struct io_kiocb *req: The I/O request structure.
  - Return Value: None (void).

- int io_mkdirat_prep(struct io_kiocb *req, const struct io_uring_sqe *sqe);
  - Purpose: Prepares a mkdirat operation for a given request (req) based on the provided submission queue entry (sqe).
  - Parameters:
    - struct io_kiocb *req: The I/O request structure.
    - const struct io_uring_sqe *sqe: The submission queue entry containing the operation details.
  - Return Value: Returns an integer indicating success (0) or an error code.
- int io_mkdirat(struct io_kiocb *req, unsigned int issue_flags);
  - Purpose: Executes a mkdirat operation for the given request (req).
  - Parameters:
    - struct io_kiocb *req: The I/O request structure.
    - unsigned int issue_flags: Flags indicating how the operation should be issued.
  - Return Value: Returns an integer indicating success (0) or an error code.
- void io_mkdirat_cleanup(struct io_kiocb *req);
  - Purpose: Cleans up resources associated with a mkdirat operation.
  - Parameters:
    - struct io_kiocb *req: The I/O request structure.
  - Return Value: None (void).

- int io_symlinkat_prep(struct io_kiocb *req, const struct io_uring_sqe *sqe);
  - Purpose: Prepares a symlinkat operation for a given request (req) based on the provided submission queue entry (sqe).
  - Parameters:
    - struct io_kiocb *req: The I/O request structure.
    - const struct io_uring_sqe *sqe: The submission queue entry containing the operation details.
  - Return Value: Returns an integer indicating success (0) or an error code.
- int io_symlinkat(struct io_kiocb *req, unsigned int issue_flags);
  - Purpose: Executes a symlinkat operation for the given request (req).
  - Parameters:
    - struct io_kiocb *req: The I/O request structure.
    - unsigned int issue_flags: Flags indicating how the operation should be issued.
  - Return Value: Returns an integer indicating success (0) or an error code.

- int io_linkat_prep(struct io_kiocb *req, const struct io_uring_sqe *sqe);
  - Purpose: Prepares a linkat operation for a given request (req) based on the provided submission queue entry (sqe).
  - Parameters:
    - struct io_kiocb *req: The I/O request structure.
    - const struct io_uring_sqe *sqe: The submission queue entry containing the operation details.
  - Return Value: Returns an integer indicating success (0) or an error code.

- int io_linkat(struct io_kiocb *req, unsigned int issue_flags);
  - Purpose: Executes a linkat operation for the given request (req).
  - Parameters:
    - struct io_kiocb *req: The I/O request structure.
    - unsigned int issue_flags: Flags indicating how the operation should be issued.
  - Return Value: Returns an integer indicating success (0) or an error code.
- void io_link_cleanup(struct io_kiocb *req);
  - Purpose: Cleans up resources associated with a linkat operation.
  - Parameters:
    - struct io_kiocb *req: The I/O request structure.
  - Return Value: None (void).



### futex.h
- int io_futex_prep(struct io_kiocb *req, const struct io_uring_sqe *sqe);
  - Purpose: Prepares a futex operation for a given request (`req`) based on the provided submission queue entry (`sqe`).
  - Parameters:
    - `struct io_kiocb *req`: The I/O request structure.
    - `const struct io_uring_sqe *sqe`: The submission queue entry containing the operation details.
  - Return Value: Returns an integer indicating success (0) or an error code.

- int io_futexv_prep(struct io_kiocb *req, const struct io_uring_sqe *sqe);
  - Purpose: Prepares a vectorized futex operation for a given request (`req`) based on the provided submission queue entry (`sqe`).
  - Parameters:
    - `struct io_kiocb *req`: The I/O request structure.
    - `const struct io_uring_sqe *sqe`: The submission queue entry containing the operation details.
  - Return Value: Returns an integer indicating success (0) or an error code.

- int io_futex_wait(struct io_kiocb *req, unsigned int issue_flags);
  - Purpose: Executes a futex wait operation for the given request (`req`).
  - Parameters:
    - `struct io_kiocb *req`: The I/O request structure.
    - `unsigned int issue_flags`: Flags indicating how the operation should be issued.
  - Return Value: Returns an integer indicating success (0) or an error code.

- int io_futexv_wait(struct io_kiocb *req, unsigned int issue_flags);
  - Purpose: Executes a vectorized futex wait operation for the given request (`req`).
  - Parameters:
    - `struct io_kiocb *req`: The I/O request structure.
    - `unsigned int issue_flags`: Flags indicating how the operation should be issued.
  - Return Value: Returns an integer indicating success (0) or an error code.

- int io_futex_wake(struct io_kiocb *req, unsigned int issue_flags);
  - Purpose: Executes a futex wake operation for the given request (`req`).
  - Parameters:
    - `struct io_kiocb *req`: The I/O request structure.
    - `unsigned int issue_flags`: Flags indicating how the operation should be issued.
  - Return Value: Returns an integer indicating success (0) or an error code.

- int io_futex_cancel(struct io_ring_ctx *ctx, struct io_cancel_data *cd,
		    unsigned int issue_flags);
  - Purpose: Cancels a futex operation based on the provided cancel data (`cd`) and context (`ctx`).
  - Parameters:
    - `struct io_ring_ctx *ctx`: The I/O ring context.
    - `struct io_cancel_data *cd`: The cancel data containing details about the operation to cancel.
    - `unsigned int issue_flags`: Flags indicating how the operation should be issued.
  - Return Value: Returns an integer indicating success (0) or an error code.

- bool io_futex_remove_all(struct io_ring_ctx *ctx, struct io_uring_task *tctx,
			 bool cancel_all);
  - Purpose: Removes all futex operations associated with the given context (`ctx`) and task context (`tctx`).
  - Parameters:
    - `struct io_ring_ctx *ctx`: The I/O ring context.
    - `struct io_uring_task *tctx`: The task context associated with the operations.
    - `bool cancel_all`: Indicates whether all operations should be canceled.
  - Return Value: Returns `true` if operations were successfully removed, `false` otherwise.

- bool io_futex_cache_init(struct io_ring_ctx *ctx);
  - Purpose: Initializes the futex cache for the given context (`ctx`).
  - Parameters:
    - `struct io_ring_ctx *ctx`: The I/O ring context.
  - Return Value: Returns `true` if the cache was successfully initialized, `false` otherwise.

- void io_futex_cache_free(struct io_ring_ctx *ctx);
  - Purpose: Frees the futex cache associated with the given context (`ctx`).
  - Parameters:
    - `struct io_ring_ctx *ctx`: The I/O ring context.
  - Return Value: None (void).

- static inline int io_futex_cancel(struct io_ring_ctx *ctx,
				  struct io_cancel_data *cd,
				  unsigned int issue_flags)
  - Purpose: Provides a no-op implementation of `io_futex_cancel` when `CONFIG_FUTEX` is not enabled.
  - Parameters:
    - `struct io_ring_ctx *ctx`: The I/O ring context.
    - `struct io_cancel_data *cd`: The cancel data.
    - `unsigned int issue_flags`: Flags for the operation.
  - Return Value: Always returns 0.

- static inline bool io_futex_remove_all(struct io_ring_ctx *ctx,
				       struct io_uring_task *tctx, bool cancel_all)
  - Purpose: Provides a no-op implementation of `io_futex_remove_all` when `CONFIG_FUTEX` is not enabled.
  - Parameters:
    - `struct io_ring_ctx *ctx`: The I/O ring context.
    - `struct io_uring_task *tctx`: The task context.
    - `bool cancel_all`: Indicates whether all operations should be canceled.
  - Return Value: Always returns `false`.

- static inline bool io_futex_cache_init(struct io_ring_ctx *ctx)
  - Purpose: Provides a no-op implementation of `io_futex_cache_init` when `CONFIG_FUTEX` is not enabled.
  - Parameters:
    - `struct io_ring_ctx *ctx`: The I/O ring context.
  - Return Value: Always returns `false`.

- static inline void io_futex_cache_free(struct io_ring_ctx *ctx)
  - Purpose: Provides a no-op implementation of `io_futex_cache_free` when `CONFIG_FUTEX` is not enabled.
  - Parameters:
    - `struct io_ring_ctx *ctx`: The I/O ring context.
  - Return Value: None (void).



### io-wq.h
- void io_wq_worker_sleeping(struct task_struct *tsk);
  - purpose: notifies the io_uring worker infrastructure that a worker thread is entering a sleeping state (no-op if CONFIG_IO_WQ is not defined)
  - parameters:
    - struct task_struct *tsk: the worker thread that is sleeping
  - return value: none (void)

- void io_wq_worker_running(struct task_struct *tsk);
  - purpose: notifies the io_uring worker infrastructure that a worker thread is running (no-op if CONFIG_IO_WQ is not defined)
  - parameters:
    - struct task_struct *tsk: the worker thread that is running
  - return value: none (void)

- bool io_wq_current_is_worker(void);
  - purpose: checks if the current task is an io_uring worker thread
  - parameters: none
  - return value: true if the current task is an io_uring worker, false otherwise


### io_uring.h
- unsigned long rings_size(unsigned int flags, unsigned int sq_entries, unsigned int cq_entries, size_t *sq_offset);
  - purpose: calculates the total size required for io_uring submission and completion rings based on the provided parameters
  - parameters:
    - unsigned int flags: ring configuration flags
    - unsigned int sq_entries: number of submission queue entries
    - unsigned int cq_entries: number of completion queue entries
    - size_t *sq_offset: pointer to store the calculated SQ offset
  - return value: total size in bytes required for the rings

- int io_uring_fill_params(unsigned entries, struct io_uring_params *p);
  - purpose: fills the io_uring_params structure with ring configuration and offsets for a given number of entries
  - parameters:
    - unsigned entries: number of submission queue entries
    - struct io_uring_params *p: pointer to the io_uring_params structure to fill
  - return value: 0 on success, or a negative error code

- bool io_cqe_cache_refill(struct io_ring_ctx *ctx, bool overflow);
  - purpose: refills the completion queue event (CQE) cache for the given io_uring context
  - parameters:
    - struct io_ring_ctx *ctx: the io_uring context
    - bool overflow: indicates if the refill is due to an overflow condition
  - return value: true if the cache was refilled, false otherwise

- int io_run_task_work_sig(struct io_ring_ctx *ctx);
  - purpose: runs pending task work for the given io_uring context, handling signal delivery if necessary
  - parameters:
    - struct io_ring_ctx *ctx: the io_uring context
  - return value: 0 on success, or a negative error code

- void io_req_defer_failed(struct io_kiocb *req, s32 res);
  - purpose: handles failure of a deferred request by setting the result and completing the request
  - parameters:
    - struct io_kiocb *req: the I/O request structure
    - s32 res: the result code to set
  - return value: none (void)

- bool io_post_aux_cqe(struct io_ring_ctx *ctx, u64 user_data, s32 res, u32 cflags);
  - purpose: posts an auxiliary completion queue event (CQE) to the io_uring context
  - parameters:
    - struct io_ring_ctx *ctx: the io_uring context
    - u64 user_data: user data to associate with the CQE
    - s32 res: result code for the CQE
    - u32 cflags: completion flags
  - return value: true if the CQE was posted, false otherwise

- void io_add_aux_cqe(struct io_ring_ctx *ctx, u64 user_data, s32 res, u32 cflags);
  - purpose: adds an auxiliary CQE to the io_uring context without immediate posting
  - parameters:
    - struct io_ring_ctx *ctx: the io_uring context
    - u64 user_data: user data to associate with the CQE
    - s32 res: result code for the CQE
    - u32 cflags: completion flags
  - return value: none (void)

- bool io_req_post_cqe(struct io_kiocb *req, s32 res, u32 cflags);
  - purpose: posts a completion queue event for the given request with the specified result and flags
  - parameters:
    - struct io_kiocb *req: the I/O request structure
    - s32 res: result code for the CQE
    - u32 cflags: completion flags
  - return value: true if the CQE was posted, false otherwise

- void __io_commit_cqring_flush(struct io_ring_ctx *ctx);
  - purpose: flushes the completion queue ring for the given io_uring context, ensuring all CQEs are visible to userspace
  - parameters:
    - struct io_ring_ctx *ctx: the io_uring context
  - return value: none (void)

- struct file *io_file_get_normal(struct io_kiocb *req, int fd);
  - purpose: retrieves a file pointer for a normal (non-fixed) file descriptor from the request
  - parameters:
    - struct io_kiocb *req: the I/O request structure
    - int fd: the file descriptor to retrieve
  - return value: pointer to the file structure, or NULL on failure

- struct file *io_file_get_fixed(struct io_kiocb *req, int fd, unsigned issue_flags);
  - purpose: retrieves a file pointer for a fixed file descriptor from the request, using the specified issue flags
  - parameters:
    - struct io_kiocb *req: the I/O request structure
    - int fd: the fixed file descriptor to retrieve
    - unsigned issue_flags: flags controlling retrieval behavior
  - return value: pointer to the file structure, or NULL on failure

- void __io_req_task_work_add(struct io_kiocb *req, unsigned flags);
  - purpose: adds a request to the task work queue with the specified flags
  - parameters:
    - struct io_kiocb *req: the I/O request structure
    - unsigned flags: flags controlling task work behavior
  - return value: none (void)

- void io_req_task_work_add_remote(struct io_kiocb *req, unsigned flags);
  - purpose: adds a request to a remote task's work queue with the specified flags
  - parameters:
    - struct io_kiocb *req: the I/O request structure
    - unsigned flags: flags controlling task work behavior
  - return value: none (void)

- void io_req_task_queue(struct io_kiocb *req);
  - purpose: queues a request for execution in the task work system
  - parameters:
    - struct io_kiocb *req: the I/O request structure
  - return value: none (void)

- void io_req_task_complete(struct io_kiocb *req, io_tw_token_t tw);
  - purpose: completes a request and performs any necessary task work cleanup
  - parameters:
    - struct io_kiocb *req: the I/O request structure
    - io_tw_token_t tw: task work token
  - return value: none (void)

- void io_req_task_queue_fail(struct io_kiocb *req, int ret);
  - purpose: handles failure of a request by queuing it for failure processing
  - parameters:
    - struct io_kiocb *req: the I/O request structure
    - int ret: error code to set for the request
  - return value: none (void)

- void io_req_task_submit(struct io_kiocb *req, io_tw_token_t tw);
  - purpose: submits a request for execution in the task work system
  - parameters:
    - struct io_kiocb *req: the I/O request structure
    - io_tw_token_t tw: task work token
  - return value: none (void)

- struct llist_node *io_handle_tw_list(struct llist_node *node, unsigned int *count, unsigned int max_entries);
  - purpose: processes a list of task work items, up to a maximum number of entries
  - parameters:
    - struct llist_node *node: pointer to the head of the task work list
    - unsigned int *count: pointer to store the number of processed entries
    - unsigned int max_entries: maximum number of entries to process
  - return value: pointer to the next unprocessed node, or NULL if all processed

- struct llist_node *tctx_task_work_run(struct io_uring_task *tctx, unsigned int max_entries, unsigned int *count);
  - purpose: runs task work for the specified io_uring task context, up to a maximum number of entries
  - parameters:
    - struct io_uring_task *tctx: the io_uring task context
    - unsigned int max_entries: maximum number of entries to process
    - unsigned int *count: pointer to store the number of processed entries
  - return value: pointer to the next unprocessed node, or NULL if all processed

- void tctx_task_work(struct callback_head *cb);
  - purpose: callback function to process task work for a given callback head
  - parameters:
    - struct callback_head *cb: pointer to the callback head structure
  - return value: none (void)

- __cold void io_uring_cancel_generic(bool cancel_all, struct io_sq_data *sqd);
  - purpose: cancels generic io_uring requests, optionally canceling all requests for the given submission queue data
  - parameters:
    - bool cancel_all: whether to cancel all requests
    - struct io_sq_data *sqd: pointer to the submission queue data
  - return value: none (void)

- int io_uring_alloc_task_context(struct task_struct *task, struct io_ring_ctx *ctx);
  - purpose: allocates and initializes an io_uring context for the specified task
  - parameters:
    - struct task_struct *task: the task to associate with the io_uring context
    - struct io_ring_ctx *ctx: the io_uring context to initialize for the task
  - return value: 0 on success, or a negative error code

- int io_ring_add_registered_file(struct io_uring_task *tctx, struct file *file, int start, int end);
  - purpose: adds a registered file to the io_uring task context within the specified range
  - parameters:
    - struct io_uring_task *tctx: the io_uring task context
    - struct file *file: the file to register
    - int start: starting index of the range
    - int end: ending index of the range
  - return value: 0 on success, or a negative error code

- void io_req_queue_iowq(struct io_kiocb *req);
  - purpose: queues a request for execution in the io_uring worker queue
  - parameters:
    - struct io_kiocb *req: the I/O request structure
  - return value: none (void)

- int io_poll_issue(struct io_kiocb *req, io_tw_token_t tw);
  - purpose: issues a poll operation for the given request using the specified task work token
  - parameters:
    - struct io_kiocb *req: the I/O request structure
    - io_tw_token_t tw: task work token
  - return value: 0 on success, or a negative error code

- int io_submit_sqes(struct io_ring_ctx *ctx, unsigned int nr);
  - purpose: submits a batch of submission queue entries (SQEs) for processing in the io_uring context
  - parameters:
    - struct io_ring_ctx *ctx: the io_uring context
    - unsigned int nr: number of SQEs to submit
  - return value: number of SQEs successfully submitted, or a negative error code

- int io_do_iopoll(struct io_ring_ctx *ctx, bool force_nonspin);
  - purpose: performs I/O polling for the io_uring context, optionally forcing non-spin behavior
  - parameters:
    - struct io_ring_ctx *ctx: the io_uring context
    - bool force_nonspin: whether to force non-spin polling
  - return value: 0 on success, or a negative error code

- void __io_submit_flush_completions(struct io_ring_ctx *ctx);
  - purpose: flushes all pending completions for the given io_uring context
  - parameters:
    - struct io_ring_ctx *ctx: the io_uring context
  - return value: none (void)

- struct io_wq_work *io_wq_free_work(struct io_wq_work *work);
  - purpose: frees a work item from the io_uring worker queue and returns the next work item
  - parameters:
    - struct io_wq_work *work: pointer to the work item to free
  - return value: pointer to the next work item, or NULL if none

- void io_wq_submit_work(struct io_wq_work *work);
  - purpose: submits a work item to the io_uring worker queue for execution
  - parameters:
    - struct io_wq_work *work: pointer to the work item to submit
  - return value: none (void)

- void io_free_req(struct io_kiocb *req);
  - purpose: frees the resources associated with an I/O request
  - parameters:
    - struct io_kiocb *req: the I/O request structure to free
  - return value: none (void)

- void io_queue_next(struct io_kiocb *req);
  - purpose: queues the next linked request for execution
  - parameters:
    - struct io_kiocb *req: the current I/O request structure
  - return value: none (void)

- void io_task_refs_refill(struct io_uring_task *tctx);
  - purpose: refills the reference counters for the io_uring task context
  - parameters:
    - struct io_uring_task *tctx: the io_uring task context
  - return value: none (void)

- bool __io_alloc_req_refill(struct io_ring_ctx *ctx);
  - purpose: refills the request allocation cache for the given io_uring context
  - parameters:
    - struct io_ring_ctx *ctx: the io_uring context
  - return value: true if the cache was refilled, false otherwise

- bool io_match_task_safe(struct io_kiocb *head, struct io_uring_task *tctx, bool cancel_all);
  - purpose: checks if the given request matches the specified task context, optionally matching all requests
  - parameters:
    - struct io_kiocb *head: the I/O request to check
    - struct io_uring_task *tctx: the task context to match
    - bool cancel_all: whether to match all requests
  - return value: true if the request matches, false otherwise

- void io_activate_pollwq(struct io_ring_ctx *ctx);
  - purpose: activates the poll wait queue for the given io_uring context
  - parameters:
    - struct io_ring_ctx *ctx: the io_uring context
  - return value: none (void)

- extern struct kmem_cache *req_cachep;
  - purpose: external reference to the kernel memory cache used for io_uring request allocations
  - parameters: none
  - return value: pointer to the kmem_cache structure


### memmap.h
- unsigned int io_uring_nommu_mmap_capabilities(struct file *file);
  - purpose: returns the mmap capabilities for a file when running on a system without an MMU
  - parameters:
    - struct file *file: the file to query for mmap capabilities
  - return value: bitmask of supported mmap capabilities

- unsigned long io_uring_get_unmapped_area(struct file *file, unsigned long addr, unsigned long len, unsigned long pgoff, unsigned long flags);
  - purpose: finds an unmapped memory area suitable for io_uring region mapping
  - parameters:
    - struct file *file: the file associated with the mapping
    - unsigned long addr: suggested start address
    - unsigned long len: length of the mapping
    - unsigned long pgoff: page offset
    - unsigned long flags: mapping flags
  - return value: start address of the unmapped area, or error code

- int io_uring_mmap(struct file *file, struct vm_area_struct *vma);
  - purpose: handles the mmap operation for io_uring, mapping kernel regions into user space
  - parameters:
    - struct file *file: the file to be mapped
    - struct vm_area_struct *vma: the virtual memory area to map into
  - return value: 0 on success, or a negative error code

- void io_free_region(struct io_ring_ctx *ctx, struct io_mapped_region *mr);
  - purpose: frees a previously allocated and mapped io_uring region
  - parameters:
    - struct io_ring_ctx *ctx: the io_uring context
    - struct io_mapped_region *mr: the mapped region to free
  - return value: none (void)

- int io_create_region(struct io_ring_ctx *ctx, struct io_mapped_region *mr, struct io_uring_region_desc *reg, unsigned long mmap_offset);
  - purpose: creates and initializes a new io_uring mapped region
  - parameters:
    - struct io_ring_ctx *ctx: the io_uring context
    - struct io_mapped_region *mr: the region structure to initialize
    - struct io_uring_region_desc *reg: region descriptor with mapping details
    - unsigned long mmap_offset: offset for the mapping
  - return value: 0 on success, or a negative error code

- int io_create_region_mmap_safe(struct io_ring_ctx *ctx, struct io_mapped_region *mr, struct io_uring_region_desc *reg, unsigned long mmap_offset);
  - purpose: safely creates and initializes a new io_uring mapped region, ensuring mmap safety
  - parameters:
    - struct io_ring_ctx *ctx: the io_uring context
    - struct io_mapped_region *mr: the region structure to initialize
    - struct io_uring_region_desc *reg: region descriptor with mapping details
    - unsigned long mmap_offset: offset for the mapping
  - return value: 0 on success, or a negative error code


### msg_ring.h
- int io_uring_sync_msg_ring(struct io_uring_sqe *sqe);
  - purpose: synchronously handles a MSG_RING operation from a submission queue entry
  - parameters:
    - struct io_uring_sqe *sqe: the submission queue entry containing MSG_RING parameters
  - return value: 0 on success, or a negative error code

- int io_msg_ring_prep(struct io_kiocb *req, const struct io_uring_sqe *sqe);
  - purpose: prepares a MSG_RING operation by extracting parameters from the SQE and storing them in the request
  - parameters:
    - struct io_kiocb *req: the I/O request structure
    - const struct io_uring_sqe *sqe: the submission queue entry
  - return value: 0 on success, or a negative error code

- int io_msg_ring(struct io_kiocb *req, unsigned int issue_flags);
  - purpose: executes a MSG_RING operation, delivering a message or file descriptor to another io_uring instance
  - parameters:
    - struct io_kiocb *req: the I/O request structure
    - unsigned int issue_flags: flags controlling operation behavior
  - return value: 0 on success, or a negative error code

- void io_msg_ring_cleanup(struct io_kiocb *req);
  - purpose: cleans up resources associated with a MSG_RING request
  - parameters:
    - struct io_kiocb *req: the I/O request structure
  - return value: none (void)


### napi.h
- void io_napi_init(struct io_ring_ctx *ctx);
  - purpose: initializes NAPI tracking structures for the given io_uring context (no-op if CONFIG_NET_RX_BUSY_POLL is not defined)
  - parameters:
    - struct io_ring_ctx *ctx: the io_uring context
  - return value: none (void)

- void io_napi_free(struct io_ring_ctx *ctx);
  - purpose: frees NAPI tracking resources for the given io_uring context (no-op if CONFIG_NET_RX_BUSY_POLL is not defined)
  - parameters:
    - struct io_ring_ctx *ctx: the io_uring context
  - return value: none (void)

- int io_register_napi(struct io_ring_ctx *ctx, void __user *arg);
  - purpose: registers a NAPI ID for busy polling in the io_uring context (returns -EOPNOTSUPP if not supported)
  - parameters:
    - struct io_ring_ctx *ctx: the io_uring context
    - void __user *arg: user pointer to NAPI registration data
  - return value: 0 on success, or a negative error code

- int io_unregister_napi(struct io_ring_ctx *ctx, void __user *arg);
  - purpose: unregisters a NAPI ID from busy polling in the io_uring context (returns -EOPNOTSUPP if not supported)
  - parameters:
    - struct io_ring_ctx *ctx: the io_uring context
    - void __user *arg: user pointer to NAPI unregistration data
  - return value: 0 on success, or a negative error code

- int __io_napi_add_id(struct io_ring_ctx *ctx, unsigned int napi_id);
  - purpose: adds a NAPI ID to the busy poll list and hash table for the io_uring context
  - parameters:
    - struct io_ring_ctx *ctx: the io_uring context
    - unsigned int napi_id: the NAPI ID to add
  - return value: 0 on success, or a negative error code

- void __io_napi_busy_loop(struct io_ring_ctx *ctx, struct io_wait_queue *iowq);
  - purpose: performs a busy poll loop for NAPI tracking in the io_uring context
  - parameters:
    - struct io_ring_ctx *ctx: the io_uring context
    - struct io_wait_queue *iowq: the wait queue to poll
  - return value: none (void)

- int io_napi_sqpoll_busy_poll(struct io_ring_ctx *ctx);
  - purpose: performs busy polling for SQPOLL mode in the io_uring context
  - parameters:
    - struct io_ring_ctx *ctx: the io_uring context
  - return value: 0 on success, or a negative error code

- bool io_napi(struct io_ring_ctx *ctx);
  - purpose: checks if NAPI tracking is enabled and the NAPI list is not empty for the context
  - parameters:
    - struct io_ring_ctx *ctx: the io_uring context
  - return value: true if NAPI tracking is active, false otherwise

- void io_napi_add(struct io_kiocb *req);
  - purpose: adds the NAPI ID of the socket associated with the request to the busy poll list and hash table (no-op if not supported)
  - parameters:
    - struct io_kiocb *req: the I/O request structure
  - return value: none (void)

- void io_napi_busy_loop(struct io_ring_ctx *ctx, struct io_wait_queue *iowq);
  - purpose: performs a busy poll loop if NAPI tracking is active for the context (no-op if not supported)
  - parameters:
    - struct io_ring_ctx *ctx: the io_uring context
    - struct io_wait_queue *iowq: the wait queue to poll
  - return value: none (void)


### net.h
- int io_shutdown_prep(struct io_kiocb *req, const struct io_uring_sqe *sqe);
  - purpose: prepares a shutdown operation by extracting parameters from the SQE and storing them in the request
  - parameters:
    - struct io_kiocb *req: the I/O request structure
    - const struct io_uring_sqe *sqe: the submission queue entry
  - return value: 0 on success, or a negative error code

- int io_shutdown(struct io_kiocb *req, unsigned int issue_flags);
  - purpose: executes a shutdown operation on a socket using the parameters in the request
  - parameters:
    - struct io_kiocb *req: the I/O request structure
    - unsigned int issue_flags: flags controlling operation behavior
  - return value: 0 on success, or a negative error code

- void io_sendmsg_recvmsg_cleanup(struct io_kiocb *req);
  - purpose: cleans up resources allocated for sendmsg or recvmsg operations in the request
  - parameters:
    - struct io_kiocb *req: the I/O request structure
  - return value: none (void)

- int io_sendmsg_prep(struct io_kiocb *req, const struct io_uring_sqe *sqe);
  - purpose: prepares a sendmsg operation by extracting parameters from the SQE and storing them in the request
  - parameters:
    - struct io_kiocb *req: the I/O request structure
    - const struct io_uring_sqe *sqe: the submission queue entry
  - return value: 0 on success, or a negative error code

- int io_sendmsg(struct io_kiocb *req, unsigned int issue_flags);
  - purpose: executes a sendmsg operation using the parameters in the request
  - parameters:
    - struct io_kiocb *req: the I/O request structure
    - unsigned int issue_flags: flags controlling operation behavior
  - return value: number of bytes sent on success, or a negative error code

- int io_send(struct io_kiocb *req, unsigned int issue_flags);
  - purpose: executes a send operation using the parameters in the request
  - parameters:
    - struct io_kiocb *req: the I/O request structure
    - unsigned int issue_flags: flags controlling operation behavior
  - return value: number of bytes sent on success, or a negative error code

- int io_recvmsg_prep(struct io_kiocb *req, const struct io_uring_sqe *sqe);
  - purpose: prepares a recvmsg operation by extracting parameters from the SQE and storing them in the request
  - parameters:
    - struct io_kiocb *req: the I/O request structure
    - const struct io_uring_sqe *sqe: the submission queue entry
  - return value: 0 on success, or a negative error code

- int io_recvmsg(struct io_kiocb *req, unsigned int issue_flags);
  - purpose: executes a recvmsg operation using the parameters in the request
  - parameters:
    - struct io_kiocb *req: the I/O request structure
    - unsigned int issue_flags: flags controlling operation behavior
  - return value: number of bytes received on success, or a negative error code

- int io_recv(struct io_kiocb *req, unsigned int issue_flags);
  - purpose: executes a recv operation using the parameters in the request
  - parameters:
    - struct io_kiocb *req: the I/O request structure
    - unsigned int issue_flags: flags controlling operation behavior
  - return value: number of bytes received on success, or a negative error code

- void io_sendrecv_fail(struct io_kiocb *req);
  - purpose: handles failure for send or receive operations by setting the appropriate error state in the request
  - parameters:
    - struct io_kiocb *req: the I/O request structure
  - return value: none (void)

- int io_accept_prep(struct io_kiocb *req, const struct io_uring_sqe *sqe);
  - purpose: prepares an accept operation by extracting parameters from the SQE and storing them in the request
  - parameters:
    - struct io_kiocb *req: the I/O request structure
    - const struct io_uring_sqe *sqe: the submission queue entry
  - return value: 0 on success, or a negative error code

- int io_accept(struct io_kiocb *req, unsigned int issue_flags);
  - purpose: executes an accept operation using the parameters in the request
  - parameters:
    - struct io_kiocb *req: the I/O request structure
    - unsigned int issue_flags: flags controlling operation behavior
  - return value: new file descriptor on success, or a negative error code

- int io_socket_prep(struct io_kiocb *req, const struct io_uring_sqe *sqe);
  - purpose: prepares a socket creation operation by extracting parameters from the SQE and storing them in the request
  - parameters:
    - struct io_kiocb *req: the I/O request structure
    - const struct io_uring_sqe *sqe: the submission queue entry
  - return value: 0 on success, or a negative error code

- int io_socket(struct io_kiocb *req, unsigned int issue_flags);
  - purpose: executes a socket creation operation using the parameters in the request
  - parameters:
    - struct io_kiocb *req: the I/O request structure
    - unsigned int issue_flags: flags controlling operation behavior
  - return value: new file descriptor on success, or a negative error code

- int io_connect_prep(struct io_kiocb *req, const struct io_uring_sqe *sqe);
  - purpose: prepares a connect operation by extracting parameters from the SQE and storing them in the request
  - parameters:
    - struct io_kiocb *req: the I/O request structure
    - const struct io_uring_sqe *sqe: the submission queue entry
  - return value: 0 on success, or a negative error code

- int io_connect(struct io_kiocb *req, unsigned int issue_flags);
  - purpose: executes a connect operation using the parameters in the request
  - parameters:
    - struct io_kiocb *req: the I/O request structure
    - unsigned int issue_flags: flags controlling operation behavior
  - return value: 0 on success, or a negative error code

- int io_send_zc(struct io_kiocb *req, unsigned int issue_flags);
  - purpose: executes a zero-copy send operation using the parameters in the request
  - parameters:
    - struct io_kiocb *req: the I/O request structure
    - unsigned int issue_flags: flags controlling operation behavior
  - return value: number of bytes sent on success, or a negative error code

- int io_sendmsg_zc(struct io_kiocb *req, unsigned int issue_flags);
  - purpose: executes a zero-copy sendmsg operation using the parameters in the request
  - parameters:
    - struct io_kiocb *req: the I/O request structure
    - unsigned int issue_flags: flags controlling operation behavior
  - return value: number of bytes sent on success, or a negative error code

- int io_send_zc_prep(struct io_kiocb *req, const struct io_uring_sqe *sqe);
  - purpose: prepares a zero-copy send operation by extracting parameters from the SQE and storing them in the request
  - parameters:
    - struct io_kiocb *req: the I/O request structure
    - const struct io_uring_sqe *sqe: the submission queue entry
  - return value: 0 on success, or a negative error code

- void io_send_zc_cleanup(struct io_kiocb *req);
  - purpose: cleans up resources allocated for zero-copy send operations in the request
  - parameters:
    - struct io_kiocb *req: the I/O request structure
  - return value: none (void)

- int io_bind_prep(struct io_kiocb *req, const struct io_uring_sqe *sqe);
  - purpose: prepares a bind operation by extracting parameters from the SQE and storing them in the request
  - parameters:
    - struct io_kiocb *req: the I/O request structure
    - const struct io_uring_sqe *sqe: the submission queue entry
  - return value: 0 on success, or a negative error code

- int io_bind(struct io_kiocb *req, unsigned int issue_flags);
  - purpose: executes a bind operation using the parameters in the request
  - parameters:
    - struct io_kiocb *req: the I/O request structure
    - unsigned int issue_flags: flags controlling operation behavior
  - return value: 0 on success, or a negative error code

- int io_listen_prep(struct io_kiocb *req, const struct io_uring_sqe *sqe);
  - purpose: prepares a listen operation by extracting parameters from the SQE and storing them in the request
  - parameters:
    - struct io_kiocb *req: the I/O request structure
    - const struct io_uring_sqe *sqe: the submission queue entry
  - return value: 0 on success, or a negative error code

- int io_listen(struct io_kiocb *req, unsigned int issue_flags);
  - purpose: executes a listen operation using the parameters in the request
  - parameters:
    - struct io_kiocb *req: the I/O request structure
    - unsigned int issue_flags: flags controlling operation behavior
  - return value: 0 on success, or a negative error code

- void io_netmsg_cache_free(const void *entry);
  - purpose: frees a cached network message entry used by io_uring networking operations
  - parameters:
    - const void *entry: pointer to the cached entry to free
  - return value: none (void)


### nop.h
- int io_nop_prep(struct io_kiocb *req, const struct io_uring_sqe *sqe);
  - purpose: prepares a no-operation (NOP) request by extracting parameters from the SQE and storing them in the request
  - parameters:
    - struct io_kiocb *req: the I/O request structure
    - const struct io_uring_sqe *sqe: the submission queue entry
  - return value: 0 on success, or a negative error code

- int io_nop(struct io_kiocb *req, unsigned int issue_flags);
  - purpose: executes a no-operation (NOP) request, optionally injecting a result if specified
  - parameters:
    - struct io_kiocb *req: the I/O request structure
    - unsigned int issue_flags: flags controlling operation behavior
  - return value: 0 on success, or a negative error code


### notif.h
- struct io_kiocb *io_alloc_notif(struct io_ring_ctx *ctx);
  - purpose: allocates and initializes a notification request for the given io_uring context
  - parameters:
    - struct io_ring_ctx *ctx: the io_uring context
  - return value: pointer to the allocated io_kiocb notification request, or NULL on failure

- void io_tx_ubuf_complete(struct sk_buff *skb, struct ubuf_info *uarg, bool success);
  - purpose: completes a user buffer (ubuf) transmission, updating state and freeing resources as needed
  - parameters:
    - struct sk_buff *skb: socket buffer associated with the transmission (can be NULL)
    - struct ubuf_info *uarg: user buffer info structure
    - bool success: indicates if the transmission was successful
  - return value: none (void)

- struct io_notif_data *io_notif_to_data(struct io_kiocb *notif);
  - purpose: converts an io_kiocb notification request to its associated io_notif_data structure
  - parameters:
    - struct io_kiocb *notif: the notification request
  - return value: pointer to the associated io_notif_data structure

- void io_notif_flush(struct io_kiocb *notif);
  - purpose: flushes the notification request, completing any outstanding user buffer operations
  - parameters:
    - struct io_kiocb *notif: the notification request
  - return value: none (void)

- int io_notif_account_mem(struct io_kiocb *notif, unsigned len);
  - purpose: accounts for memory usage for a notification request, updating user memory accounting
  - parameters:
    - struct io_kiocb *notif: the notification request
    - unsigned len: length of memory to account for
  - return value: 0 on success, or a negative error code


### opdef.h
- `bool io_uring_op_supported(u8 opcode)`
  - purpose: checks if an operation code is supported
  - parameters:
    - `opcode`: operation code to check
  - return value: true if operation is supported, false otherwise

- `void io_uring_optable_init(void)`
  - purpose: initializes operation tables during system startup
  - parameters: none
  - return value: none (void function)


### openclose.h
- `int __io_close_fixed(struct io_ring_ctx *ctx, unsigned int issue_flags, unsigned int offset)`
  - purpose: closes a file descriptor from the fixed file table
  - parameters:
    - `ctx`: io_uring context
    - `issue_flags`: operation flags
    - `offset`: index in fixed file table
  - return value: 0 on success, negative error code on failure

- `int io_openat_prep(struct io_kiocb *req, const struct io_uring_sqe *sqe)`
  - purpose: prepares an openat operation by parsing SQE parameters
  - parameters:
    - `req`: I/O request context
    - `sqe`: submission queue entry containing open parameters
  - return value: 0 on success, negative error code on failure

- `int io_openat(struct io_kiocb *req, unsigned int issue_flags)`
  - purpose: executes prepared openat operation
  - parameters:
    - `req`: prepared I/O request
    - `issue_flags`: operation flags
  - return value: new file descriptor on success, negative error code on failure

- `void io_open_cleanup(struct io_kiocb *req)`
  - purpose: cleans up resources associated with open operation
  - parameters:
    - `req`: I/O request to clean up
  - return value: none (void function)


### poll.h
- `static inline void io_poll_multishot_retry(struct io_kiocb *req)`
  - purpose: increments poll reference count for multishot operations
  - parameters:
    - `req`: request to increment
  - return value: none (void function)

- `int io_poll_add_prep(struct io_kiocb *req, const struct io_uring_sqe *sqe)`
  - purpose: prepares a poll add operation
  - parameters:
    - `req`: I/O request context
    - `sqe`: submission queue entry
  - return value: 0 on success, negative error code on failure

- `int io_poll_add(struct io_kiocb *req, unsigned int issue_flags)`
  - purpose: executes poll add operation
  - parameters:
    - `req`: prepared I/O request
    - `issue_flags`: operation flags
  - return value: 0 on success, negative error code on failure

- `int io_poll_remove_prep(struct io_kiocb *req, const struct io_uring_sqe *sqe)`
  - purpose: prepares poll removal operation
  - parameters: same as poll_add_prep
  - return value: 0 on success, negative error code on failure

- `int io_poll_remove(struct io_kiocb *req, unsigned int issue_flags)`
  - purpose: executes poll removal operation
  - parameters: same as poll_add
  - return value: 0 on success, negative error code on failure

- `int io_poll_cancel(struct io_ring_ctx *ctx, struct io_cancel_data *cd, unsigned issue_flags)`
  - purpose: cancels active polling operation
  - parameters:
    - `ctx`: ring context
    - `cd`: cancellation data
    - `issue_flags`: operation flags
  - return value: 0 on success, negative error code on failure

- `bool io_poll_remove_all(struct io_ring_ctx *ctx, struct io_uring_task *tctx, bool cancel_all)`
  - purpose: removes all poll operations for a context
  - parameters:
    - `ctx`: ring context
    - `tctx`: task context
    - `cancel_all`: whether to cancel all operations
  - return value: true if operations were removed


### refs.h
- `bool req_ref_inc_not_zero(struct io_kiocb *req)`
  - purpose: increments reference count if non-zero
  - parameters:
    - `req`: request to increment
  - return value: true if increment successful, false if count was zero

- `void req_ref_get(struct io_kiocb *req)`
  - purpose: unconditionally increments reference count
  - parameters:
    - `req`: request to increment
  - return value: none (void function)


### register.h
- `int io_eventfd_unregister(struct io_ring_ctx *ctx)`
  - purpose: unregisters eventfd from an io_uring context
  - parameters:
    - `struct io_ring_ctx *ctx`: io_uring context to unregister from
  - return value: returns 0 on success or negative error code on failure

- `int io_unregister_personality(struct io_ring_ctx *ctx, unsigned id)`
  - purpose: unregisters a personality (credential) from an io_uring context
  - parameters:
    - `struct io_ring_ctx *ctx`: io_uring context
    - `unsigned id`: personality ID to unregister
  - return value: returns 0 on success or negative error code on failure

- `struct file *io_uring_register_get_file(unsigned int fd, bool registered)`
  - purpose: retrieves file structure for a given file descriptor, handling both registered and unregistered cases
  - parameters:
    - `unsigned int fd`: file descriptor to look up
    - `bool registered`: whether the file descriptor is from registered set
  - return value: returns pointer to file structure or ERR_PTR on error


### rsrc.h
- `bool io_rsrc_cache_init(struct io_ring_ctx *ctx)`
  - purpose: initializes resource cache for an io_uring context
  - parameters:
    - `ctx`: io_uring context to initialize
  - return value: true on success, false on failure

- `int io_register_rsrc(struct io_ring_ctx *ctx, void __user *arg, unsigned int size, unsigned int type)`
  - purpose: registers new resources (files or buffers) with io_uring
  - parameters:
    - `ctx`: io_uring context
    - `arg`: user space argument containing resources
    - `size`: size of registration data
    - `type`: resource type (IORING_RSRC_FILE or IORING_RSRC_BUFFER)
  - return value: 0 on success, negative error code on failure

- `struct io_rsrc_node *io_rsrc_node_alloc(struct io_ring_ctx *ctx, int type)`
  - purpose: allocates a new resource node
  - parameters:
    - `ctx: io_uring context
    - `type`: type of resource to allocate
  - return value: pointer to new node or NULL on failure

- `void io_free_rsrc_node(struct io_ring_ctx *ctx, struct io_rsrc_node *node)`
  - purpose: frees a resource node
  - parameters:
    - `ctx`: io_uring context
    - `node`: node to free
  - return value: none

- `int io_import_reg_buf(struct io_kiocb *req, struct iov_iter *iter, u64 buf_addr, size_t len, int ddir, unsigned issue_flags)`
  - purpose: imports registered buffer for I/O operation
  - parameters:
    - `req`: I/O request context
    - `iter`: I/O vector iterator
    - `buf_addr`: buffer address
    - `len`: buffer length
    - `ddir`: data direction
    - `issue_flags`: operation flags
  - return value: 0 on success, negative error code on failure


### rw.h
- `int io_prep_read_fixed(struct io_kiocb *req, const struct io_uring_sqe *sqe)`
  - purpose: prepares a read operation using pre-registered fixed buffers
  - parameters:
    - `req`: I/O request context
    - `sqe`: submission queue entry
  - return value: 0 on success, negative error code on failure

- `int io_prep_write_fixed(struct io_kiocb *req, const struct io_uring_sqe *sqe)`
  - purpose: prepares a write operation using pre-registered fixed buffers
  - parameters: same as above
  - return value: same as above

- `int io_prep_readv(struct io_kiocb *req, const struct io_uring_sqe *sqe)`
  - purpose: prepares a vector read operation
  - parameters: same as above
  - return value: same as above

- `int io_prep_writev(struct io_kiocb *req, const struct io_uring_sqe *sqe)`
  - purpose: prepares a vector write operation
  - parameters: same as above
  - return value: same as above

- `int io_read(struct io_kiocb *req, unsigned int issue_flags)`
  - purpose: executes a prepared read operation
  - parameters:
    - `req`: prepared I/O request
    - `issue_flags`: flags controlling operation behavior
  - return value: bytes read on success, negative error code on failure

- `int io_write(struct io_kiocb *req, unsigned int issue_flags)`
  - purpose: executes a prepared write operation
  - parameters: same as above
  - return value: bytes written on success, negative error code on failure


### slist.h
- `void wq_list_add_after(struct io_wq_work_node *node, struct io_wq_work_node *pos, struct io_wq_work_list *list)`
  - purpose: inserts a node after specified position in list
  - parameters:
    - `node`: node to insert
    - `pos`: position after which to insert
    - `list`: list to modify
  - return value: none (void function)
  
- `void wq_list_add_tail(struct io_wq_work_node *node, struct io_wq_work_list *list)`
  - purpose: adds node to end of list
  - parameters:
    - `node`: node to add
    - `list`: target list
  - return value: none (void function)
  
- `void wq_list_cut(struct io_wq_work_list *list, struct io_wq_work_node *last, struct io_wq_work_node *prev)`
  - purpose: cuts list at specified position
  - parameters:
    - `list`: list to modify
    - `last`: last node of cut section
    - `prev`: node before cut section
  - return value: none (void function)
  
- `struct io_wq_work *wq_next_work(struct io_wq_work *work)`
  - purpose: retrieves next work item in list
  - parameters:
    - `work`: current work item
  - return value: next work item or NULL if none exists


### splice.h
- `int io_tee_prep(struct io_kiocb *req, const struct io_uring_sqe *sqe)`
  - purpose: prepares a tee operation for copying data between two pipes without consuming it
  - parameters:
    - `struct io_kiocb *req`: I/O request context
    - `const struct io_uring_sqe *sqe`: submission queue entry containing operation parameters
  - return value: returns 0 on success or negative error code on failure
  
- `int io_tee(struct io_kiocb *req, unsigned int issue_flags)`
  - purpose: executes the prepared tee operation between pipes
  - parameters:
    - `struct io_kiocb *req`: prepared I/O request context
    - `unsigned int issue_flags`: flags controlling operation execution
  - return value: returns number of bytes copied on success or negative error code on failure
  
- `void io_splice_cleanup(struct io_kiocb *req)`
  - purpose: cleans up resources associated with a splice operation
  - parameters:
    - `struct io_kiocb *req`: I/O request context to clean up
  - return value: none (void function)
  
- `int io_splice_prep(struct io_kiocb *req, const struct io_uring_sqe *sqe)`
  - purpose: prepares a splice operation for moving data between file descriptors
  - parameters:
    - `struct io_kiocb *req`: I/O request context
    - `const struct io_uring_sqe *sqe`: submission queue entry containing operation parameters
  - return value: returns 0 on success or negative error code on failure
  
- `int io_splice(struct io_kiocb *req, unsigned int issue_flags)`
  - purpose: executes the prepared splice operation between file descriptors
  - parameters:
    - `struct io_kiocb *req`: prepared I/O request context
    - `unsigned int issue_flags`: flags controlling operation execution
  -return value: returns number of bytes moved on success or negative error code on failure


### sqpoll.h
- `int io_sq_offload_create(struct io_ring_ctx *ctx, struct io_uring_params *p)`
	- purpose: creates and initializes submission queue polling thread
	- parameters:
		- `struct io_ring_ctx *ctx`: io_uring context
		- `struct io_uring_params *p`: parameters for SQ thread creation
	- return value: returns 0 on success or negative error code on failure
  
- `void io_sq_thread_finish(struct io_ring_ctx *ctx)`
	- purpose: cleans up and finalizes SQ polling thread
	- parameters:
		- `struct io_ring_ctx *ctx`: io_uring context to finish
	- return value: none (void function)
  
- `void io_sq_thread_stop(struct io_sq_data *sqd)`
	- purpose: stops the SQ polling thread
	- parameters:
		- `struct io_sq_data *sqd`: SQ data structure containing thread info
	- return value: none (void function)
- `void io_sq_thread_park(struct io_sq_data *sqd)`
	- purpose: temporarily parks (suspends) the SQ polling thread
	- parameters:
		- `struct io_sq_data *sqd`: SQ data structure to park
	- return value: none (void function)
  
- `void io_sq_thread_unpark(struct io_sq_data *sqd)`
	- purpose: resumes a previously parked SQ polling thread
	- parameters:
		- `struct io_sq_data *sqd`: SQ data structure to unpark
	- return value: none (void function)
- `void io_put_sq_data(struct io_sq_data *sqd)`
	- purpose: decrements reference count and potentially frees SQ data
	- parameters:
		- `struct io_sq_data *sqd`: SQ data structure to put
	- return value: none (void function)
  
- `void io_sqpoll_wait_sq(struct io_ring_ctx *ctx)`
	- purpose: waits for SQ polling thread completion
	- parameters:
		- `struct io_ring_ctx *ctx`: io_uring context to wait on
	- return value: none (void function)
  
- `int io_sqpoll_wq_cpu_affinity(struct io_ring_ctx *ctx, cpumask_var_t mask)`
	- purpose: sets CPU affinity for SQ polling thread workqueue
	- parameters:
		- `struct io_ring_ctx *ctx`: io_uring context
		- `cpumask_var_t mask`: CPU mask defining allowed processors
	- return value: returns 0 on success or negative error code on failure


### statx.h
- `int io_statx_prep(struct io_kiocb *req, const struct io_uring_sqe *sqe)`
	- purpose: prepares an extended file status (statx) operation by extracting and validating parameters from the submission queue entry
	- parameters:
		- `struct io_kiocb *req`: the I/O request context containing operation state
		- `const struct io_uring_sqe *sqe`: the submission queue entry containing user parameters for the statx operation
	- return value: returns 0 on success or a negative error code (like -EINVAL) on parameter validation failure
  
- `int io_statx(struct io_kiocb *req, unsigned int issue_flags)`
	- purpose: executes the prepared statx operation to retrieve extended file status information
	- parameters:
		- `struct io_kiocb *req`: the prepared I/O request context
		- `unsigned int issue_flags`: flags controlling how the operation should be issued (e.g., blocking vs non-blocking)
	- return value: returns number of bytes written to user buffer on success or a negative error code on failure
  
- `void io_statx_cleanup(struct io_kiocb *req)`
	- purpose: cleans up resources associated with a statx operation after completion or cancellation
	- parameters:
		- `struct io_kiocb *req: the I/O request context to be cleaned up`
	- return value: none (void function)


### sync.h
- int io_sfr_prep(struct io_kiocb *req, const struct io_uring_sqe *sqe);
  - purpose: prepares a sync_file_range operation by parsing the submission queue entry (SQE)
  - parameters:
	- struct io_kiocb *req: the I/O request context
	- const struct io_uring_sqe *sqe: the submission queue entry containing user-provided operation parameters
  - return value: returns 0 on success or a negative error code on failure

- int io_sync_file_range(struct io_kiocb *req, unsigned int issue_flags);
  - purpose: executes a sync_file_range operation, flushing specified file ranges to disk
  - parameters:
   	- struct io_kiocb *req: the I/O request context
    - unsigned int issue_flags: additional flags influencing how the operation is issued
  - return value: returns 0 on success or a negative error code on failure

- int io_fsync_prep(struct io_kiocb *req, const struct io_uring_sqe *sqe);
  - purpose: prepares an fsync operation from a submission queue entry
  - parameters:
  	- struct io_kiocb *req: the I/O request context
    - const struct io_uring_sqe *sqe: the submission queue entry containing user-provided fsync parameters
  - return value: returns 0 on success or a negative error code on failure

- int io_fsync(struct io_kiocb *req, unsigned int issue_flags);
  - purpose: performs an fsync operation to flush all modified file data and metadata to disk
  - parameters:
    - struct io_kiocb *req: the I/O request context
    - unsigned int issue_flags: additional flags that may modify the behavior of the operation
  - return value: returns 0 on success or a negative error code on failure

- int io_fallocate(struct io_kiocb *req, unsigned int issue_flags);
  - purpose: performs a file space allocation operation (like preallocating disk space)
  - parameters:
    - struct io_kiocb *req: the I/O request context
    - unsigned int issue_flags: flags to control the behavior of the allocation
  - return value: returns 0 on success or a negative error code on failure

- int io_fallocate_prep(struct io_kiocb *req, const struct io_uring_sqe *sqe);
  - purpose: prepares a fallocate operation by parsing the submission queue entry
  - parameters:
    - struct io_kiocb *req: the I/O request context
    - const struct io_uring_sqe *sqe: the submission queue entry containing user-supplied fallocate parameters
  - return value: returns 0 on success or a negative error code on failure


### tctx.h
- int io_uring_alloc_task_context(struct task_struct *task, struct io_ring_ctx *ctx);
  - purpose: allocates and initializes an io_uring context for the specified task
  - parameters:
    - struct task_struct *task: the task to associate with the io_uring context
    - struct io_ring_ctx *ctx: the io_uring context to initialize for the task
  - return value: returns 0 on success or a negative error code on failure

- void io_uring_del_tctx_node(unsigned long index);
  - purpose: removes a task context node from the global task context array by index
  - parameters:
    - unsigned long index: the index of the task context node to remove
  - return value: none

- int __io_uring_add_tctx_node(struct io_ring_ctx *ctx);
  - purpose: internal function to associate a new io_uring context with the current task
  - parameters:
    - struct io_ring_ctx *ctx: the io_uring context to add to the current task
  - return value: returns 0 on success or a negative error code on failure

- int __io_uring_add_tctx_node_from_submit(struct io_ring_ctx *ctx);
  - purpose: internal variant used during submission to associate the context with the current task
  - parameters:
    - struct io_ring_ctx *ctx: the io_uring context to add
  - return value: returns 0 on success or a negative error code on failure

- void io_uring_clean_tctx(struct io_uring_task *tctx);
  - purpose: cleans up resources and state associated with an io_uring task context
  - parameters:
	- struct io_uring_task *tctx: the task-specific io_uring context to clean
  - return value: none

- void io_uring_unreg_ringfd(void);
  - purpose: unregisters any ring file descriptors associated with the current task
  - parameters: none
  - return value: none

- int io_ringfd_register(struct io_ring_ctx *ctx, void __user *__arg, unsigned nr_args);
  - purpose: registers one or more file descriptors to the specified io_uring context
  - parameters:
    - struct io_ring_ctx *ctx: the io_uring context for registration
    - void __user *__arg: user-space pointer to the file descriptor array
    - unsigned nr_args: number of file descriptors to register
  - return value: returns 0 on success or a negative error code on failure

- int io_ringfd_unregister(struct io_ring_ctx *ctx, void __user *__arg, unsigned nr_args);
  - purpose: unregisters one or more file descriptors from the specified io_uring context
  - parameters:
    - struct io_ring_ctx *ctx: the io_uring context to modify
    - void __user *__arg: user-space pointer to the file descriptor array
    - unsigned nr_args: number of file descriptors to unregister
  - return value: returns 0 on success or a negative error code on failure

- static inline int io_uring_add_tctx_node(struct io_ring_ctx *ctx);
  - purpose: conditionally associates an io_uring context with the current task, optimizing for reuse
  - parameters:
    - struct io_ring_ctx *ctx: the io_uring context to associate
  - return value: returns 0 if already associated or successfully added, otherwise a negative error code


### timeout.h
- static inline struct io_kiocb *io_disarm_linked_timeout(struct io_kiocb *req);
  - purpose: disarms a linked timeout request if the next operation is a LINK_TIMEOUT
  - parameters:
    - struct io_kiocb *req: the current I/O request
  - return value: returns the disarmed timeout request if applicable, or NULL if no action was taken

- __cold void io_flush_timeouts(struct io_ring_ctx *ctx);
  - purpose: flushes all pending timeouts in the specified io_uring context
  - parameters:
    - struct io_ring_ctx *ctx: the io_uring context whose timeouts should be flushed
  - return value: none

- int io_timeout_cancel(struct io_ring_ctx *ctx, struct io_cancel_data *cd);
  - purpose: cancels a timeout operation based on the provided cancelation data
  - parameters:
    - struct io_ring_ctx *ctx: the io_uring context associated with the timeout
    - struct io_cancel_data *cd: data structure specifying which timeout to cancel
  - return value: returns 0 on successful cancelation, or a negative error code on failure

- __cold bool io_kill_timeouts(struct io_ring_ctx *ctx, struct io_uring_task *tctx, bool cancel_all);
  - purpose: cancels all or selected timeouts associated with a task
  - parameters:
    - struct io_ring_ctx *ctx: the io_uring context in use
    - struct io_uring_task *tctx: the task whose timeouts may be canceled
    - bool cancel_all: whether to cancel all timeouts or only those associated with the task
  - return value: returns true if any timeouts were canceled, false otherwise

- void io_queue_linked_timeout(struct io_kiocb *req);
  - purpose: queues a linked timeout request for execution
  - parameters:
    - struct io_kiocb *req: the timeout request to be queued
  - return value: none

- void io_disarm_next(struct io_kiocb *req);
  - purpose: disarms the next timeout request in a chain if present
  - parameters:
    - struct io_kiocb *req: the current I/O request
  - return value: none

- int io_timeout_prep(struct io_kiocb *req, const struct io_uring_sqe *sqe);
  - purpose: prepares a timeout request based on user-submitted SQE data
  - parameters:
    - struct io_kiocb *req: the timeout request
    - const struct io_uring_sqe *sqe: the submission queue entry from userspace
  - return value: returns 0 on success or a negative error code on failure

- int io_link_timeout_prep(struct io_kiocb *req, const struct io_uring_sqe *sqe);
  - purpose: prepares a linked timeout operation from the provided SQE
  - parameters:
    - struct io_kiocb *req: the timeout request to prepare
    - const struct io_uring_sqe *sqe: submission queue entry containing the timeout parameters
  - return value: returns 0 on success or a negative error code on failure

- int io_timeout(struct io_kiocb *req, unsigned int issue_flags);
  - purpose: executes a timeout request, waiting for the specified time or cancelation
  - parameters:
    - struct io_kiocb *req: the timeout request
    - unsigned int issue_flags: flags modifying the behavior of the request
  - return value: returns 0 if the timeout completes successfully, or a negative error code

- int io_timeout_remove_prep(struct io_kiocb *req, const struct io_uring_sqe *sqe);
  - purpose: prepares a timeout removal request based on the submission queue entry
  - parameters:
    - struct io_kiocb *req: the request to prepare
    - const struct io_uring_sqe *sqe: user-provided submission queue entry
  - return value: returns 0 on success or a negative error code on failure

- int io_timeout_remove(struct io_kiocb *req, unsigned int issue_flags);
  - purpose: performs the removal of a timeout request
  - parameters:
    - struct io_kiocb *req: the removal request
    - unsigned int issue_flags: optional flags influencing removal behavior
  - return value: returns 0 on success or a negative error code on failure


### truncate.h
- int io_ftruncate_prep(struct io_kiocb *req, const struct io_uring_sqe *sqe);
  - purpose: prepares an ftruncate operation from the user-provided submission queue entry (SQE)
  - parameters:
    - struct io_kiocb *req: the I/O request to be prepared
    - const struct io_uring_sqe *sqe: the submission queue entry containing ftruncate parameters
  - return value: returns 0 on success or a negative error code if preparation fails

- int io_ftruncate(struct io_kiocb *req, unsigned int issue_flags);
  - purpose: executes an ftruncate operation to truncate the file to a specified length
  - parameters:
    - struct io_kiocb *req: the I/O request to execute
    - unsigned int issue_flags: optional flags to modify operation behavior
  - return value: returns 0 on success or a negative error code on failure


### uring_cmd.h
- int io_uring_cmd(struct io_kiocb *req, unsigned int issue_flags);
  - purpose: executes a generic io_uring_cmd request, allowing user-defined commands to be issued
  - parameters:
    - struct io_kiocb *req: the I/O request containing the uring_cmd
    - unsigned int issue_flags: flags modifying command behavior
  - return value: returns 0 on success or a negative error code on failure

- int io_uring_cmd_prep(struct io_kiocb *req, const struct io_uring_sqe *sqe);
  - purpose: prepares an io_uring_cmd request from the provided submission queue entry
  - parameters:
    - struct io_kiocb *req: the I/O request to prepare
    - const struct io_uring_sqe *sqe: submission queue entry from userspace
  - return value: returns 0 on success or a negative error code on failure

- void io_uring_cmd_cleanup(struct io_kiocb *req);
  - purpose: cleans up any resources associated with an io_uring_cmd request
  - parameters:
    - struct io_kiocb *req: the request to be cleaned up
  - return value: none

- bool io_uring_try_cancel_uring_cmd(struct io_ring_ctx *ctx, struct io_uring_task *tctx, bool cancel_all);
  - purpose: attempts to cancel one or all pending io_uring_cmd requests for a given task
  - parameters:
    - struct io_ring_ctx *ctx: the io_uring context in which to search for commands
    - struct io_uring_task *tctx: the task associated with the commands
    - bool cancel_all: flag to indicate whether all commands should be canceled or only one
  - return value: returns true if any command was canceled, false otherwise

- void io_cmd_cache_free(const void *entry);
  - purpose: frees memory used for caching io_uring_cmd entries
  - parameters:
    - const void *entry: the cached command entry to free
  - return value: none

- int io_uring_cmd_import_fixed_vec(struct io_uring_cmd *ioucmd, const struct iovec __user *uvec, size_t uvec_segs, int ddir, struct iov_iter *iter, unsigned issue_flags);
  - purpose: imports a user-provided I/O vector into kernel space for use with a uring_cmd
  - parameters:
    - struct io_uring_cmd *ioucmd: the io_uring_cmd structure that will use the I/O vector
    - const struct iovec __user *uvec: userspace pointer to the I/O vector
    - size_t uvec_segs: number of segments in the vector
  	- int ddir: direction of the data transfer (e.g., read or write)
  	- struct iov_iter *iter: destination kernel iterator to fill
  	- unsigned issue_flags: flags modifying import behavior
  - return value: returns 0 on success or a negative error code if import fails


### waitid.h
- int io_waitid_prep(struct io_kiocb *req, const struct io_uring_sqe *sqe);
  - purpose: prepares a waitid request based on the user-provided submission queue entry (SQE)
  - parameters:
  	- struct io_kiocb *req: the I/O request to be prepared
  	- const struct io_uring_sqe *sqe: the submission queue entry containing waitid parameters
  - return value: returns 0 on success or a negative error code on failure

- int io_waitid(struct io_kiocb *req, unsigned int issue_flags);
  - purpose: performs the waitid system call within the io_uring context to wait for process state changes
  - parameters:
  	- struct io_kiocb *req: the I/O request to execute
  	- unsigned int issue_flags: flags modifying how the operation is issued
  - return value: returns 0 on success or a negative error code on failur

- int io_waitid_cancel(struct io_ring_ctx *ctx, struct io_cancel_data *cd, unsigned int issue_flags);
  - purpose: attempts to cancel a pending waitid request from the context
  - parameters:
  	- struct io_ring_ctx *ctx: the io_uring context in which the request resides
  	- struct io_cancel_data *cd: cancellation data specifying which request to cancel
  	- unsigned int issue_flags: optional flags affecting cancellation behavior
  - return value: returns 0 on success or a negative error code if the request was not found or could not be canceled

- bool io_waitid_remove_all(struct io_ring_ctx *ctx, struct io_uring_task *tctx, bool cancel_all);
  - purpose: removes and optionally cancels all waitid requests associated with a given task
  - parameters:
  	- struct io_ring_ctx *ctx: the io_uring context to remove requests from
  	- struct io_uring_task *tctx: the task whose requests should be removed
  	- bool cancel_all: flag indicating whether to cancel all or just some matching requests
  - return value: returns true if any requests were removed, false otherwise


### xattr.h
- void io_xattr_cleanup(struct io_kiocb *req);
  - purpose: cleans up resources associated with an extended attribute (xattr) operation
  - parameters:
    - struct io_kiocb *req: the I/O request to clean up
  - return value: none (void)

- int io_fsetxattr_prep(struct io_kiocb *req, const struct io_uring_sqe *sqe);
  - purpose: prepares a fsetxattr request, which sets an extended attribute on an open file
  - parameters:
  	- struct io_kiocb *req: the I/O request to prepare
  	- const struct io_uring_sqe *sqe: the submission queue entry containing the xattr data
  - return value: 0 on success or a negative error code

- int io_fsetxattr(struct io_kiocb *req, unsigned int issue_flags);
  - purpose: executes the fsetxattr system call to set an extended attribute on an open file
  - parameters:
    - struct io_kiocb *req: the prepared I/O request
    - unsigned int issue_flags: flags affecting how the request is issued
  - return value: 0 on success or a negative error code

- int io_setxattr_prep(struct io_kiocb *req, const struct io_uring_sqe *sqe);
  - purpose: prepares a setxattr request, which sets an extended attribute on a file by path
  - parameters:
    - struct io_kiocb *req: the I/O request to prepare
    - const struct io_uring_sqe *sqe: the submission queue entry containing the xattr data
  - return value: 0 on success or a negative error code

- int io_setxattr(struct io_kiocb *req, unsigned int issue_flags);
  - purpose: executes the setxattr system call to set an extended attribute on a file by path
  - parameters:
  	- struct io_kiocb *req: the prepared I/O request
  	- unsigned int issue_flags: flags affecting how the request is issued
  - return value: 0 on success or a negative error code

- int io_fgetxattr_prep(struct io_kiocb *req, const struct io_uring_sqe *sqe);
  - purpose: prepares a fgetxattr request to retrieve an extended attribute from an open file
  - parameters:
  	- struct io_kiocb *req: the I/O request to prepare
  	- const struct io_uring_sqe *sqe: the submission queue entry containing xattr request info
  - return value: 0 on success or a negative error code

- int io_fgetxattr(struct io_kiocb *req, unsigned int issue_flags);
  - purpose: performs the fgetxattr system call to retrieve an extended attribute from an open file
  - parameters:
  	- struct io_kiocb *req: the I/O request
  	- unsigned int issue_flags: flags controlling request execution
  - return value: size of the attribute data retrieved or a negative error code

- int io_getxattr_prep(struct io_kiocb *req, const struct io_uring_sqe *sqe);
  - purpose: prepares a getxattr request to retrieve an extended attribute from a file by path
  - parameters:
    - struct io_kiocb *req: the I/O request to prepare
    - const struct io_uring_sqe *sqe: the submission queue entry with relevant data
  - return value: 0 on success or a negative error code

- int io_getxattr(struct io_kiocb *req, unsigned int issue_flags);
  - purpose: executes the getxattr system call to retrieve an extended attribute from a file by path
  - parameters:
    - struct io_kiocb *req: the prepared I/O request
    - unsigned int issue_flags: issue modifiers
  - return value: size of the attribute data retrieved or a negative error code


### zcrx.h
<!-- #if defined(CONFIG_IO_URING_ZCRX) -->
  - int io_register_zcrx_ifq(struct io_ring_ctx *ctx, struct io_uring_zcrx_ifq_reg __user *arg);
   - purpose: registers a Zero-Copy Receive (ZCRX) interface queue (IFQ) with the given io_uring context
   - parameters:
     - struct io_ring_ctx *ctx: the io_uring context
     - struct io_uring_zcrx_ifq_reg __user *arg: user-space pointer containing the IFQ registration data
   - return value: returns 0 on success, or a negative error code

  - void io_unregister_zcrx_ifqs(struct io_ring_ctx *ctx);
   - purpose: unregisters all ZCRX IFQs from the given context
   - parameters:
     - struct io_ring_ctx *ctx: the io_uring context
   - return value: none (void)

  - void io_shutdown_zcrx_ifqs(struct io_ring_ctx *ctx);
   - purpose: shuts down all active ZCRX IFQs associated with the context
   - parameters:
     - struct io_ring_ctx *ctx: the io_uring context
   - return value: none (void)

  - int io_zcrx_recv(struct io_kiocb *req, struct io_zcrx_ifq *ifq, struct socket *sock, unsigned int flags, unsigned issue_flags, unsigned int *len);
   - purpose: receives data using ZCRX from a socket through a given interface queue
   - parameters:
     - struct io_kiocb *req: the I/O request
     - struct io_zcrx_ifq *ifq: the interface queue to use
     - struct socket *sock: the socket to receive data from
     - unsigned int flags: flags for the receive operation
     - unsigned issue_flags: issue-specific flags
     - unsigned int *len: pointer to receive the length of the received data
   - return value: number of bytes received or a negative error code
<!-- #else -->
	- static inline int io_register_zcrx_ifq(struct io_ring_ctx *ctx, struct io_uring_zcrx_ifq_reg __user *arg)
	  - purpose: provides a stub implementation that returns -EOPNOTSUPP when ZCRX support is not compiled into the kernel.
	  - parameters:
  		- struct io_ring_ctx *ctx: the io_uring context associated with the request.
  		- struct io_uring_zcrx_ifq_reg __user *arg: user-space pointer containing ZCRX registration parameters.
	  - return value: always returns -EOPNOTSUPP to indicate the operation is not supported.

	- static inline void io_unregister_zcrx_ifqs(struct io_ring_ctx *ctx)
	  - purpose: provides a no-op fallback for unregistering ZCRX interfaces when ZCRX is not supported.
	  - parameters:
  		- struct io_ring_ctx *ctx: the io_uring context to clean up.
	  - return value: none (void).

	- static inline void io_shutdown_zcrx_ifqs(struct io_ring_ctx *ctx)
	  - purpose: provides a no-op fallback for shutting down ZCRX interfaces when ZCRX is not supported.
	  - parameters:
  		- struct io_ring_ctx *ctx: the io_uring context to shut down.
	  - return value: none (void).

	- static inline int io_zcrx_recv(struct io_kiocb *req, struct io_zcrx_ifq *ifq, struct socket *sock, unsigned int flags, unsigned issue_flags, unsigned int *len)
	  - purpose: provides a stub implementation of ZCRX receive operation that returns -EOPNOTSUPP when ZCRX is not supported.
	  - parameters:
  		- struct io_kiocb *req: the io_uring request structure.
  		- struct io_zcrx_ifq *ifq: the ZCRX interface queue structure.
  		- struct socket *sock: the socket to receive data from.
  		- unsigned int flags: additional receive flags.
  		- unsigned issue_flags: flags controlling request submission behavior.
  		- unsigned int *len: pointer to store the length of the received data.
	  - return value: always returns -EOPNOTSUPP to indicate the operation is not supported.
<!-- #endif -->

	- int io_recvzc(struct io_kiocb *req, unsigned int issue_flags);
	  - purpose: performs a zero-copy recv operation using io_uring
	  - parameters:
  		- struct io_kiocb *req: the I/O request
  		- unsigned int issue_flags: flags to control the operation
	  - return value: number of bytes received or a negative error code

	- int io_recvzc_prep(struct io_kiocb *req, const struct io_uring_sqe *sqe);
	  - purpose: prepares a zero-copy recv operation from an SQE
	  - parameters:
  		- struct io_kiocb *req: the I/O request to be prepared
  		- const struct io_uring_sqe *sqe: the submission queue entry
	  - return value: 0 on success, or a negative error code

<!-- #endif -->