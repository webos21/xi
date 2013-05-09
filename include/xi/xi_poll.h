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

#ifndef _XI_POLL_H_
#define _XI_POLL_H_

/**
 * @brief XI Polling API
 *
 * @file xi_poll.h
 * @date 2010-08-31
 * @author Cheolmin Jo (webos21@gmail.com)
 */

#include "xi_file.h"
#include "xi_socket.h"

/**
 * Start Declaration
 */
_XI_EXTERN_C_BEGIN

/**
 * @defgroup xi_poll Polling API
 * @ingroup XI
 * @{
 */

/**
 * Return values of Polling Functions
 */
typedef enum _e_pollset_rv {
	XI_POLLSET_RV_OK           = 0,    ///< OK
	XI_POLLSET_RV_ERR_OP       = -1,   ///< Failed to operate native func
	XI_POLLSET_RV_ERR_OVER     = -2,   ///< Over the Size-Limit
	XI_POLLSET_RV_ERR_NF       = -3,   ///< Not Found
	XI_POLLSET_RV_ERR_TIMEOUT  = -4,   ///< Timed out
	XI_POLLSET_RV_ERR_INTR     = -5,   ///< Interrupted
	XI_POLLSET_RV_ERR_ARGS     = -6    ///< Invalid Arguments
} xi_pollset_re;


/**
 * Poll Option
 */
typedef enum _e_pollset_opt {
	XI_POLLSET_OPT_USELOCK  = 0x00000001, ///< Use the lock for Thread-Safe.
	XI_POLLSET_OPT_EPOLL    = 0x00000002  ///< Use EPOLL, if not set, use the normal POLL.
} xi_pollset_opt_e;


/**
 * Poll Events
 */
typedef enum _e_poll_event {
	XI_POLL_EVENT_IN   = 0x001, ///< There is data to read.
	XI_POLL_EVENT_PRI  = 0x002, ///< There is urgent data to read.
	XI_POLL_EVENT_OUT  = 0x004, ///< Writing now will not block.
	XI_POLL_EVENT_ERR  = 0x010, ///< Error condition (output only).
	XI_POLL_EVENT_HUP  = 0x020, ///< Hang up (output only).
	XI_POLL_EVENT_NVAL = 0x040  ///< Invalid request: fd not open (output only).
} xi_poll_event_e;


/**
 * file handle
 */
typedef xfd    xi_fd_t;


/**
 * The structure of poll descriptor
 */
typedef struct _st_pollfd {
	xint32           desc;     ///< file descriptor
	xint32           evts;     ///< xi_poll_event_e
	xvoid           *context;  ///< user-defined context
} xi_pollfd_t;


/**
 * Abstract handle of poll set
 */
typedef struct _xi_pollset xi_pollset_t;


/**
 * Create a pollset object
 *
 * @param size The maximum number of descriptors that this pollset can hold
 * @param opt Optional flags to modify the operation of the pollset.
 * @return The pointer in which to return the newly created object
 */
xi_pollset_t  *xi_pollset_create(xuint32 size, xint32 opt);


/**
 * Add a socket or file descriptor to a pollset
 *
 * @param pset The pollset to which to add the descriptor
 * @param fd The descriptor to add
 */
xi_pollset_re  xi_pollset_add(xi_pollset_t *pset, xi_pollfd_t fd);


/**
 * Remove a descriptor from a pollset
 *
 * @param pset The pollset from which to remove the descriptor
 * @param fd The descriptor to remove
 */
xi_pollset_re  xi_pollset_remove(xi_pollset_t *pset, xi_pollfd_t fd);


/**
 * Destroy a pollset object
 *
 * @param pset The pollset to destroy
 */
xi_pollset_re  xi_pollset_destroy(xi_pollset_t *pset);


/**
 * Block for activity on the descriptor(s) in a pollset
 *
 * @param pset The pollset to use
 * @param rfds Array of signaled descriptors (output parameter)
 * @param rlen Size of signaled descriptors - rfds
 * @param msecs The amount of time in microseconds to wait.  This is a
 *              maximum, not a minimum.  If a descriptor is signaled, the
 *              function will return before this time.  If timeout is
 *              negative, the function will block until a descriptor is
 *              signaled.
 * @return The number of signaled descriptors
 */
xint32        xi_pollset_poll(xi_pollset_t *pset, xi_pollfd_t  *rfds,
		        xint32 rlen, xint32 msecs);

/**
 * @}  // end of xi_poll
 */

/**
 * End Declaration
 */
_XI_EXTERN_C_END

#endif // _XI_POLL_H_
