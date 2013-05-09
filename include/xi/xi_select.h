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

#ifndef _XI_SELECT_H_
#define _XI_SELECT_H_

/**
 * @brief XI Select API
 *
 * @file xi_select.h
 * @date 2012-02-23
 * @author Cheolmin Jo (webos21@gmail.com)
 */

#include "xi_file.h"
#include "xi_socket.h"

/**
 * Start Declaration
 */
_XI_EXTERN_C_BEGIN

/**
 * @defgroup xi_select Select API
 * @ingroup XI
 * @{
 */

/**
 * Return values of Polling Functions
 */
typedef enum _e_select_rv {
	XI_SELECT_RV_OK           = 0,    ///< OK
	XI_SELECT_RV_TIMEOUT      = 0,    ///< Timed out
	XI_SELECT_RV_ERR_BADFD    = -1,   ///< Bad file-descriptor
	XI_SELECT_RV_ERR_INTR     = -2,   ///< Interrupted
	XI_SELECT_RV_ERR_NOMEM    = -3,   ///< Not enough memory
	XI_SELECT_RV_ERR_ARGS     = -4    ///< Invalid Arguments
} xi_select_re;


/**
 * Abstract handle of fd-set
 */
typedef struct _xi_fdset xi_fdset_t;

/**
 * Create a file-descriptor monitor
 *
 * @return the pointer of file-descriptor monitor. it must be destroyed after use.
 */
xi_fdset_t   *xi_sel_fdcreate();

/**
 * Reset a file-descriptor monitor
 *
 * @param fdset the pointer of file-descriptor monitor.
 */
xvoid         xi_sel_fdzero(xi_fdset_t *fdset);

/**
 * Clear a file-descriptor monitor on file-descriptor
 *
 * @param fd    the file-descriptor.
 * @param fdset the pointer of file-descriptor monitor.
 */
xvoid         xi_sel_fdclr(xint32 fd, xi_fdset_t *fdset);

/**
 * Check the setting of a file-descriptor monitor on file-descriptor
 *
 * @param fd    the file-descriptor.
 * @param fdset the pointer of file-descriptor monitor.
 */
xint32        xi_sel_fdisset(xint32 fd, xi_fdset_t *fdset);

/**
 * Set a file-descriptor monitor on file-descriptor
 *
 * @param fd    the file-descriptor.
 * @param fdset the pointer of file-descriptor monitor.
 */
xvoid         xi_sel_fdset(xint32 fd, xi_fdset_t *fdset);

/**
 * Destroy a file-descriptor monitor
 *
 * @param fdset the pointer of file-descriptor monitor.
 */
xvoid         xi_sel_fddestroy(xi_fdset_t *fdset);


/**
 * Monitor multiple file descriptors.
 *
 * @param nfds The highest-numbered file-descriptor in any of the three sets, plus 1.
 * @param rfds Optional pointer to a set of file-descriptor to be checked for readability.
 * @param wfds Optional pointer to a set of file-descriptor to be checked for readability.
 * @param efds Optional pointer to a set of file-descriptor to be checked for readability.
 * @param msec The amount of time in microseconds to wait.  This is a
 *             maximum, not a minimum.  If a descriptor is signaled, the
 *             function will return before this time.  If timeout is
 *             negative, the function will block until a descriptor is
 *             signaled.
 * @return The number of file descriptors contained in the three returned descriptor sets
 *         (that is, the total number of  bits that  are  set in readfds, writefds, exceptfds)
 *         which may be zero if the timeout expires before anything interesting happens.
 *         below 0 is error.
 */
xint32        xi_sel_select(xint32 nfds, xi_fdset_t *rfds, xi_fdset_t *wfds, xi_fdset_t *efds, xint32 msec);

/**
 * @}  // end of xi_select
 */

/**
 * End Declaration
 */
_XI_EXTERN_C_END

#endif // _XI_SELECT_H_
