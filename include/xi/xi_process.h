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

#ifndef _XI_PROCESS_H_
#define _XI_PROCESS_H_

/**
 * @brief XI Process Handling API
 *
 * @file xi_process.h
 * @date 2010-08-31
 * @author Cheolmin Jo (webos21@gmail.com)
 */

#include "xtype.h"

/**
 * Start Declaration
 */
_XI_EXTERN_C_BEGIN

/**
 * @defgroup xi_process Process Handling API
 * @ingroup XI
 * @{
 */

/**
 @brief   xi_proc_mutex_re
*/
typedef enum _e_proc_mutex_rv {
	XI_PROC_MUTEX_RV_OK         = 0,
	XI_PROC_MUTEX_RV_ERR_ALLOC  = -1, ///< Cannot allocate memory
	XI_PROC_MUTEX_RV_ERR_EXIST  = -2, ///< Already exists
	XI_PROC_MUTEX_RV_ERR_CREATE = -3, ///< Cannot create
	XI_PROC_MUTEX_RV_ERR_LOCK   = -4, ///< Cannot lock/unlock
	XI_PROC_MUTEX_RV_ERR_ARGS   = -5  ///< Invalid arguments
} xi_proc_mutex_re;


/**
 * Opaque structure representing a process mutex
 */
typedef struct _xi_proc_mutex xi_proc_mutex_t;


/**
 * Create and initialize a mutex that can be used to synchronize processes.
 *
 * @param lock the memory address where the newly created mutex will be
 *        stored.
 * @param filepath A file name to use if the lock mechanism requires one.  This
 *        argument should always be provided.  The lock code itself will
 *        determine if it should be used.
 */
xi_proc_mutex_re  xi_proc_mutex_create(xi_proc_mutex_t **lock, const xchar *filepath);


/**
 * Open a mutex in a child process.
 *
 * @param lock The newly opened mutex structure.
 * @param filepath A file name to use if the mutex mechanism requires one.  This
 *              argument should always be provided.  The mutex code itself will
 *              determine if it should be used.  This filename should be the
 *              same one that was passed to xi_proc_mutex_create().
 */
xi_proc_mutex_re  xi_proc_mutex_open(xi_proc_mutex_t **lock, const xchar *filepath);


/**
 * Acquire the lock for the given mutex. If the mutex is already locked,
 * the current thread will be put to sleep until the lock becomes available.
 *
 * @param lock the mutex on which to acquire the lock.
 */
xi_proc_mutex_re  xi_proc_mutex_lock(xi_proc_mutex_t *lock);


/**
 * Release the lock for the given mutex.
 *
 * @param lock the mutex from which to release the lock.
 */
xi_proc_mutex_re  xi_proc_mutex_unlock(xi_proc_mutex_t *lock);


/**
 * Close a mutex in a child process.
 *
 * @param lock the mutex to close.
 */
xi_proc_mutex_re  xi_proc_mutex_close(xi_proc_mutex_t *lock);


/**
 * Destroy the mutex and free the memory associated with the lock.
 *
 * @param lock the mutex to destroy.
 */
xi_proc_mutex_re  xi_proc_mutex_destroy(xi_proc_mutex_t *lock);


/**
 * Create a new process and execute a new program within that process.
 *
 * @param cmdp    the arguments to pass to the new program.  The first
 *                one should be the program name.
 * @param cmdln   array size of cmdp.
 * @param envp    The new environment table for the new process.  This
 *                should be a list of NULL-terminated strings.
 *                It can be NULL.
 * @param envln   array size of envp.
 * @param workdir Working directory for this command.
 *                It can be NULL.
 * @return The resulting process handle.
 */
xintptr xi_proc_create(xchar * const cmdp[], xint32 cmdln, xchar * const envp[], xint32 envln,
			const xchar *workdir);


/**
 * Daemonize the process.
 * (be a SERVICE in win32)
 *
 * @return the process-id
 */
xintptr  xi_proc_daemonize();


/**
 * Get the pid of current process.
 *
 * @return the pid of current process
 */
xintptr  xi_proc_getpid();


/**
 * Wait for a child process to die
 *
 * @param pid The process handle that corresponds to the desired child process
 * @param status The process exit code pointer
 * @return The child process id, if it success, 0 is not changed, if it failed <0
 */
xintptr  xi_proc_waitpid(xintptr pid, xint32 *status);


/**
 * Terminate the target process forcely.
 *
 * @param pid target process-id
 * @return the value 0 if successful; otherwise it returns minus error value
 */
xint32   xi_proc_term(xintptr pid);


/**
 * Register a function to be called at normal process termination
 *
 * Note: Functions so registered are called in the reverse order
 *       of their registration.
 *       The same function may be registered multiple times,
 *       and it is called once for each registration.
 *
 * @param func the function to be called
 * @return the value 0 if successful; otherwise it returns a non-zero value.
 */
xint32   xi_proc_atexit(xvoid (*func)());


/**
 * Terminate current process normally
 *
 * @param status the status code of termination
 */
xvoid   xi_proc_exit(xint32 status);


/**
 * Terminate current process abnormally
 *
 * Note: SIGABRT will be occurred.
 *       The registered function by xi_proc_atexit() is not called.
 */
xvoid   xi_proc_abort();


/**
 * @}  // end of xi_process
 */

/**
 * End Declaration
 */
_XI_EXTERN_C_END

#endif // _XI_PROCESS_H_
