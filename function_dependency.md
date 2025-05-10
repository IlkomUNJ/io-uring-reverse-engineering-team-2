# Task 2: Dependency Injection
For this assigment, we want a little clarity regarding what kind of functions being imported and used on each source. Do note, we record all function actually being used by the source including function defined by itself if actually used inside the file. For the sake of completion, it's better if you straight disregard include list on the source. Instead, trace each function being used to the declared source.

| Source        | Libary                               | Function utilized               | Time Used |
| ------------- | ------------------------------------ | ------------------------------- | --------- |
| alloc_cache.h | /include/linux/kasan.h               | kasan_mempool_unpoison_object   | 1         |
|               | /include/linux/kasan.h               | kasan_mempool_posion_object     | 1         |
|               | arch/x86/include/asm/string_64.h     | memset                          | 1         |
|               | alloc_cache.h                        | io_alloc_cache_get              | 1         |
|               | alloc_cache.h                        | io_cache_alloc_new              | 1         |
|               | alloc_cache.h                        | io_alloc_cache_put              | 1         |
|               | linux/mm/slub.c                      | kfree                           | 1         |
| filettable.h  | io_uring.c                           | io_file_get_flags               | 1         |
| io_uring.h    | /include/linux/lockdep.h             | lockdep_assert                  | 2         |
|               | /include/linux/lockdep.h             | lockdep_assert_held             | 7         |
|               | /include/linux/preempt.h             | in_task                         | 1         |
|               | io_uring.h                           | __io_req_task_work_add          | 2         |
|               | io_uring.h                           | __io_submit_flush_completions   | 1         |
|               | io_uring.h                           | io_lockdep_assert_cq_locked     | 1         |
|               | io_uring.h                           | io_cqe_cache_refill             | 1         |
|               | io_uring.h                           | io_get_cqe_overflow             | 1         |
|               | io_uring.h                           | io_get_cqe                      | 1         |
|               | arch/x86/include/asm/string_64.h     | memset                          | 1         |
|               | arch/x86/include/asm/string_64.h     | memcpy                          | 2         |
|               | trace/events/io_uring.h              | trace_io_uring_complete_enabled | 1         |
|               | trace/events/io_uring.h              | trace_io_uring_complete         | 1         |
|               | alloc_chace.h                        | io_cache_alloc                  | 1         |
|               | tools/testing/scatterlist/linux/mm.h | kmalloc                         | 1         |
|               | /fs/filetable.c                      | fput                            | 1         |
|               | kernel/locking/mutex.c               | mutex_unlock                    | 1         |
|               | kernel/locking/mutex.c               | mutex_lock                      | 1         |
|               | tools/arch/x86/include/asm/barrier.h | smp_store_release               | 1         |
|               | /include/linux/wait.h                | wq_has_sleeper                  | 2         |
|               | kernel/sched/wait.c                  | __wake_up                       | 2         |
|               | tools/arch/x86/include/asm/barrier.h | smp_load_acquire                | 1         |
|               | include/linux/sched                  | clear_notify_signal             | 1         |
|               | inlude/linux/thread_info.h           | test_thread_flag                | 2         |
|               | include/linux/resume_user_mod.h      | resume_user_mode_work           | 1         |
|               | io_uring.h                           | tctx_task_work_run              | 1         |
|               | io_uring.h                           | task_work_run                   | 1         |
|               | /include/linux/llist.h               | llist_empty                     | 2         |
|               | /include/linux/task_work.h           | task_work_pending               | 2         |
|               | io_uring.h                           | io_local_work_pending           | 2         |
|               | slist.h                              | wq_list_add_tail                | 1         |
|               | io_uring.h                           | __io_commit_cqring_flush        | 1         |
|               | io_uring.h                           | io_task_refs_refill             | 1         |
|               | slist.h                              | wq_stack_extract                | 1         |
|               | io_uring.h                           | io_req_cache_empty              | 1         |
|               | io_uring.h                           | __io_alloc_req_refill           | 1         |
|               | io_uring.h                           | io_extract_req                  | 1         |
|               | /include/linux/compiler.h            | likely                          | 2         |
|               | io_uring.h                           | io_req_set_res                  | 1         |
|               | io_uring.c                           | io_req_task_complete            | 1         |
|               | kernel/time/timekeeping.c            | ktime_get                       | 1         |
|               | kernel/time/timekeeping.c            | ktime_get_with_offset           | 1         |
|               |                                      |                                 |           |
