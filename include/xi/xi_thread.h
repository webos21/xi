/*
 * Copyright 2013 Cheolmin Jo (webos21@gmail.com)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef _XI_THREAD_H_
#define _XI_THREAD_H_

/**
 * @brief XI Thread API
 *
 * @file xi_thread.h
 * @date 2010-08-31
 * @author Cheolmin Jo (webos21@gmail.com)
 */

#include "xtype.h"

/**
 * Start Declaration
 */
_XI_EXTERN_C_BEGIN

/**
 * @defgroup xi_thread Thread API
 * @ingroup XI
 * @{
 */

/**
 * The type of thread handle
 */
typedef xuintptr  xi_thread_t;

/**
 * The type of thread-key handle
 */
typedef xuint32   xi_thread_key_t;

/**
 * The type of mutex handle
 */
typedef xuintptr  xi_thread_mutex_t;

/**
 * The type of conditional variable handle
 */
typedef xuintptr  xi_thread_cond_t;


/**
 * Return values of Thread Functions
 */
typedef enum _e_thr_rv {
	XI_THREAD_RV_OK        = 0,
	XI_THREAD_RV_ERR_ID    = -1,  ///< Bad thread id
	XI_THREAD_RV_ERR_COUNT = -2,  ///< No more thread
	XI_THREAD_RV_ERR_STACK = -3,  ///< Invalid attr
	XI_THREAD_RV_ERR_PERM  = -4,  ///< Permission denied
	XI_THREAD_RV_ERR_PRIOR = -5,  ///< Unsupported priority
	XI_THREAD_RV_ERR_NONEM = -6,  ///< Insufficient memory
	XI_THREAD_RV_ERR_ARGS  = -7,  ///< Invalid arguments
	XI_THREAD_RV_ERR_INTR  = -8,  ///< Interrupted
	XI_THREAD_RV_ERR_NOSUP = -9,  ///< Not support
	XI_THREAD_RV_ERR_DLOCK = -10  ///< Dead-Lock detected
} xi_thread_re;


/**
 * State values of Thread
 */
typedef enum _e_thr_state_e {
	XI_THREAD_STATE_CREATING       = 0,
	XI_THREAD_STATE_STARTED        = 1,
	XI_THREAD_STATE_RUNNING        = 2,
	XI_THREAD_STATE_WAITING        = 3,
	XI_THREAD_STATE_TIMED_WAITING  = 4,
	XI_THREAD_STATE_BLOCKED        = 5,
	XI_THREAD_STATE_SUSPENDED      = 6
} xi_thread_state_e;

/**
 * Suspend-block-state values of Thread
 */
typedef enum _e_thr_susblk_e {
	XI_THREAD_SUSBLK_NOBLOCK       = 0,
	XI_THREAD_SUSBLK_BLOCKING      = 1,
	XI_THREAD_SUSBLK_CRITICAL      = 2,
} xi_thread_susblk_e;


/**
 * Return values of Thread-Key Functions
 */
typedef enum _e_tkey_rv {
	XI_TKEY_RV_OK          = 0,
	XI_TKEY_RV_ERR_NOMORE  = -1,  ///< Cannot create any more key.
	XI_TKEY_RV_ERR_NOMEM   = -2,  ///< Insufficient memory.
	XI_TKEY_RV_ERR_ARGS    = -3   ///< Invalid arguments.
} xi_thread_key_re;


/**
 * Return values of Mutex Functions
 */
typedef enum _e_mutex_rv {
	XI_MUTEX_RV_OK         = 0,
	XI_MUTEX_RV_ERR_NOMORE = -1,  ///< Cannot create any more mutex.
	XI_MUTEX_RV_ERR_NOMEM  = -2,  ///< Insufficient memory.
	XI_MUTEX_RV_ERR_PERM   = -3,  ///< Insufficient privilege of Caller.
	XI_MUTEX_RV_ERR_BUSY   = -4,  ///< Already Locked or Initialized by other.
	XI_MUTEX_RV_ERR_AGAIN  = -5,  ///< Already Locked or Initialized by self.
	XI_MUTEX_RV_ERR_ARGS   = -6   ///< Invalid arguments.
} xi_thread_mutex_re;


/**
 * Return values of Cond Functions
 */
typedef enum _e_cond_rv {
	XI_COND_RV_OK          = 0,
	XI_COND_RV_ERR_NOMORE  = -1,  ///< Cannot create any more cond.
	XI_COND_RV_ERR_NOMEM   = -2,  ///< Insufficient memory.
	XI_COND_RV_ERR_PERM    = -3,  ///< Insufficient privilege of Caller.
	XI_COND_RV_ERR_BUSY    = -4,  ///< Already Initialized by other.
	XI_COND_RV_ERR_TIMEOUT = -5,  ///< Timeout occurred.
	XI_COND_RV_ERR_ARGS    = -6   ///< Invalid arguments.
} xi_thread_cond_re;

/**
 * The signature of thread function (function pointer)
 */
typedef xvoid *(*xi_thread_fn)(xvoid *arg);

/**
 * Create and initialize a new thread private address space
 *
 * @param key The thread private handle.
 */
xi_thread_key_re    xi_thread_key_create(xi_thread_key_t *key);


/**
 * Get a pointer to the thread private memory
 *
 * @param key The handle for the desired thread private memory
 * @return The data stored in private memory
 */
xvoid	           *xi_thread_key_get(xi_thread_key_t key);


/**
 * Set the data to be stored in thread private memory
 *
 * @param key The handle for the desired thread private memory
 * @param data The data to be stored in private memory
 */
xi_thread_key_re    xi_thread_key_set(xi_thread_key_t key, const xvoid *data);


/**
 * Destroy the thread private memory
 *
 * @param key The handle for the desired thread private memory
 */
xi_thread_key_re    xi_thread_key_destroy(xi_thread_key_t key);


/**
 * Create and initialize a mutex that can be used to synchronize threads.
 *
 * @param lock the memory address where the newly created mutex will be
 *        stored.
 * @param mname the name of the newly created mutex.
 */
xi_thread_mutex_re  xi_thread_mutex_create(xi_thread_mutex_t *lock, xchar *mname);


/**
 * Acquire the lock for the given mutex. If the mutex is already locked,
 * the current thread will be put to sleep until the lock becomes available.
 *
 * @param lock the mutex on which to acquire the lock.
 */
xi_thread_mutex_re  xi_thread_mutex_lock(xi_thread_mutex_t *lock);


/**
 * Attempt to acquire the lock for the given mutex. If the mutex has already
 * been acquired, the call returns immediately with XI_MUTEX_RV_ERR_BUSY.
 *
 * @param lock the mutex on which to acquire the lock.
 */
xi_thread_mutex_re  xi_thread_mutex_trylock(xi_thread_mutex_t *lock);


/**
 * Release the lock for the given mutex.
 *
 * @param lock the mutex from which to release the lock.
 */
xi_thread_mutex_re  xi_thread_mutex_unlock(xi_thread_mutex_t *lock);


/**
 * Destroy the mutex and free the memory associated with the lock.
 *
 * @param lock the mutex to destroy.
 */
xi_thread_mutex_re  xi_thread_mutex_destroy(xi_thread_mutex_t *lock);


/**
 * Create and initialize a condition variable that can be used to signal
 * and schedule threads in a single process.
 *
 * @param cond the memory address where the newly created condition variable
 *        will be stored.
 * @param cname the name of the newly created condition variable.
 */
xi_thread_cond_re   xi_thread_cond_create(xi_thread_cond_t *cond, xchar *cname);


/**
 * Put the active calling thread to sleep until signaled to wake up. Each
 * condition variable must be associated with a mutex, and that mutex must
 * be locked before  calling this function, or the behavior will be
 * undefined. As the calling thread is put to sleep, the given mutex
 * will be simultaneously released; and as this thread wakes up the lock
 * is again simultaneously acquired.
 *
 * @param cond the condition variable on which to block.
 * @param lock the mutex that must be locked upon entering this function,
 *        is released while the thread is asleep, and is again acquired before
 *        returning from this function.
 *
 * @remark Spurious wakeups may occur. Before and after every call to wait on
 * a condition variable, the caller should test whether the condition is already met.
 */
xi_thread_cond_re   xi_thread_cond_wait(xi_thread_cond_t *cond, xi_thread_mutex_t *lock);


/**
 * Put the active calling thread to sleep until signaled to wake up or
 * the timeout is reached. Each condition variable must be associated
 * with a mutex, and that mutex must be locked before calling this
 * function, or the behavior will be undefined. As the calling thread
 * is put to sleep, the given mutex will be simultaneously released;
 * and as this thread wakes up the lock is again simultaneously acquired.
 *
 * @param cond the condition variable on which to block.
 * @param lock the mutex that must be locked upon entering this function,
 *        is released while the thread is asleep, and is again acquired before
 *        returning from this function.
 * @param msec The amount of time in milliseconds to wait. This is
 *        a maximum, not a minimum. If the condition is signaled, we
 *        will wake up before this time, otherwise the error TIMEUP
 *        is returned.
 */
xi_thread_cond_re   xi_thread_cond_timedwait(xi_thread_cond_t *cond, xi_thread_mutex_t *lock, xuint32 msec);


/**
 * Signals all threads blocking on the given condition variable.
 * Each thread that was signaled is then scheduled to wake up and acquire
 * the associated mutex. This will happen in a serialized manner.
 *
 * @param cond the condition variable on which to produce the broadcast.
 *
 * @remark If no threads are waiting on the condition variable, nothing happens.
 */
xi_thread_cond_re   xi_thread_cond_broadcast(xi_thread_cond_t *cond);


/**
 * Signals a single thread, if one exists, that is blocking on the given
 * condition variable. That thread is then scheduled to wake up and acquire
 * the associated mutex. Although it is not required, if predictable scheduling
 * is desired, that mutex must be locked while calling this function.
 *
 * @param cond the condition variable on which to produce the signal.
 *
 * @remark If no threads are waiting on the condition variable, nothing happens.
 */
xi_thread_cond_re   xi_thread_cond_signal(xi_thread_cond_t *cond);


/**
 * Destroy the condition variable and free the associated memory.
 *
 * @param cond the condition variable to destroy.
 */
xi_thread_cond_re   xi_thread_cond_destroy(xi_thread_cond_t *cond);


/**
 * Create a new thread of execution
 *
 * @param tid The newly created thread handle.
 * @param tname The name of newly created thread
 * @param func The function to start the new thread in
 * @param args Any arguments to be passed to the starting function
 * @param stack_size The stack-size of newly created thread
 * @param prior The priority of newly created thread
 *
 * @return a result value of thread function
 *
 * @note A thread be created that is already detached.
 */
xi_thread_re  xi_thread_create(xi_thread_t *tid, const xchar *tname,
		xi_thread_fn func, xvoid *args, xsize stack_size, xint32 prior);


/**
 * Pause current thread for given milliseconds
 *
 * @param msec The time of pause by milliseconds
 *
 * @return a result value of thread function
 */
xi_thread_re  xi_thread_sleep(xuint32 msec);


/**
 * Pause current thread for given microseconds
 *
 * @param usec The time of pause by microseconds
 *
 * @return a result value of thread function
 */
xi_thread_re  xi_thread_usleep(xuint32 usec);


/**
 *  Force the current thread to yield the processor
 */
xvoid         xi_thread_yield();


/**
 * Send a suspend request to a thread
 *
 * @param tid The thread to suspend
 *
 * @return a result value of thread function
 */
xi_thread_re  xi_thread_suspend(xi_thread_t tid);


/**
 * Send a suspend request to all threads
 *
 * @return a result value of thread function
 */
xi_thread_re  xi_thread_suspend_all();


/**
 * Send a resume request to a thread
 *
 * @param tid The thread to resume
 *
 * @return a result value of thread function
 */
xi_thread_re  xi_thread_resume(xi_thread_t tid);


/**
 * Send a resume request to all threads
 *
 * @return a result value of thread function
 */
xi_thread_re  xi_thread_resume_all();


/**
 * Enable the suspending on a thread
 *
 * @param tid The thread to enable suspending
 *
 * @return a result value of thread function
 */
xi_thread_re  xi_thread_enable_suspend(xi_thread_t tid);


/**
 * Fast enable the suspending on a thread
 *
 * @param tid The thread to enable suspending
 *
 * @return a result value of thread function
 */
xi_thread_re  xi_thread_enable_suspend_fast(xi_thread_t tid);


/**
 * Disable the suspending on a thread
 *
 * @param tid The thread to disable suspending.
 *
 * @return a result value of thread function
 */
xi_thread_re  xi_thread_disable_suspend(xi_thread_t tid);


/**
 * Fast disable the suspending on a thread
 *
 * @param tid The thread to disable suspending.
 *
 * @return a result value of thread function
 */
xi_thread_re  xi_thread_disable_suspend_fast(xi_thread_t tid);


/**
 * Get the handle of current thread
 *
 * @return the handle of the current thread
 */
xi_thread_t   xi_thread_self();


/**
 * Set the name of thread
 *
 * @param tid The thread to rename
 * @param tname the new name of thread
 *
 * @return a result value of thread function
 */
xi_thread_re  xi_thread_set_name(xi_thread_t tid, const xchar *tname);


/**
 * Arrange the priority of specific thread
 *
 * @param tid The thread to arrange the priority
 * @param new_prior New priority to set
 *
 * @return a result value of thread function
 */
xi_thread_re  xi_thread_set_prior(xi_thread_t tid, xint32 new_prior);


/**
 * Get the priority of specific thread
 *
 * @param tid The thread to get the priority
 * @param prior The pointer of priority value to be set
 *
 * @return a result value of thread function
 */
xi_thread_re  xi_thread_get_prior(xi_thread_t tid, xint32 *prior);


/**
 * Get the stack-base of a thread
 *
 * @param tid The thread to get the stack-base
 *
 * @return the pointer of stack-base
 */
xvoid        *xi_thread_get_stackbase(xi_thread_t tid);


/**
 * Get the stack-top of a thread
 *
 * @param tid The thread to get the stack-top
 *
 * @return the pointer of stack-top
 */
xvoid        *xi_thread_get_stacktop(xi_thread_t tid);


/**
 * Get the stack-size of a thread
 *
 * @param tid The thread to get the stack-size
 *
 * @return the size of stack
 */
xsize         xi_thread_get_stacksize(xi_thread_t tid);


/**
 * Get the state of thread
 *
 * @param tid The thread to get the state
 *
 * @return the value of suspending flag (xi_thread_state_e)
 */
xint32        xi_thread_get_state(xi_thread_t tid);


/**
 * Get the count of current threads
 *
 * @return the count of current threads
 */
xint32        xi_thread_get_threads_count();


/**
 * Get the peak count of threads
 *
 * @return the peak count of threads
 */
xint32        xi_thread_get_peak_count();


/**
 * Get the total-start count of threads
 *
 * @return the total-start count of threads
 */
xint64        xi_thread_get_total_starts();


/**
 * Get the peak count of current threads
 *
 * @return the value of suspending flag (xi_thread_state_e)
 */
xint32        xi_thread_get_peak_count();


/**
 * Can the thread be suspending
 *
 * @param tid The thread to get the suspending flag
 *
 * @return the value of suspending flag (xi_thread_susblk_e)
 */
xint32        xi_thread_is_suspendable(xi_thread_t tid);


/**
 * Get the thread-list
 */
xvoid        *xi_thread_list();


/**
 * Lock the thread-list
 */
xvoid         xi_thread_list_lock();


/**
 * Unlock the thread-list
 */
xvoid         xi_thread_list_unlock();



/**
 * @}  // end of xi_thread
 */

/**
 * End Declaration
 */
_XI_EXTERN_C_END

#endif // _XI_THREAD_H_
