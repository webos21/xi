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

#ifndef _XI_SYSINFO_H_
#define _XI_SYSINFO_H_

/**
 * @brief XI System Information API
 *
 * @file xi_sysinfo.h
 * @date 2010-08-31
 * @author Cheolmin Jo (webos21@gmail.com)
 */

#include "xtype.h"

/**
 * Start Declaration
 */
_XI_EXTERN_C_BEGIN

/**
 * @defgroup xi_sysinfo System Information API
 * @ingroup XI
 * @{
 *
 *         These APIs does not allocate memory in those.
 *         So, all out parameters must be inputed like this :
 *         <pre>
 *         xchar exepath[XCFG_PATHNAME_MAX];
 *
 *         if (xi_sysinfo_exec_path(exepath, sizeof(exepath)) {
 *             do something....
 *         } else {
 *             error process...
 *         }
 *         </pre>
 */



/**
 * Returns the number of processors in the system.
 */
xlong        xi_sysinfo_cpu_num();


/**
 * Returns the name of CPU architecture.
 */
const xchar *xi_sysinfo_cpu_arch();


/**
 * Provides the name of the host operating system.
 *
 * @param[out] nbuf the pointer to the OS name string
 * @param[in] nblen the length of the path buffer (for Windows, its value must be 32-bit long.)
 * @return if OK, return the string length, otherwise <code>FALSE(=0)</code>
 */
xssize       xi_sysinfo_os_name(xchar *nbuf, xsize nblen);


/**
 * Provides the version of the host operating system.
 *
 * @param[out] nbuf the pointer to the OS version string
 * @param[in] nblen the length of the path buffer (for Windows, its value must be 32-bit long.)
 * @return if OK, return the string length, otherwise <code>FALSE(=0)</code>
 */
xssize       xi_sysinfo_os_ver(xchar *nbuf, xsize nblen);


/**
 * Get the supported memory page sizes
 *
 * @return if OK, return the string length, otherwise <code>FALSE(=0)</code>
 */
xlong        xi_sysinfo_pagesize();


/**
 * Returns the name of the account under which the current process is executed.
 *
 * @param[out] nbuf the pointer to the requested name string
 * @param[in] nblen the length of the path buffer (for Windows, its value must be 32-bit long.)
 * @return if OK, return the string length, otherwise <code>FALSE(=0)</code>
 */
xssize       xi_sysinfo_user_name(xchar *nbuf, xsize nblen);


/**
 * Returns the home path of the account under which the process is executed.
 *
 * @param[out] nbuf the pointer to the requested path string
 * @param[in] nblen the length of the buffer (for Windows, its value must be 32-bit long.)
 * @return if OK, return the string length, otherwise <code>FALSE(=0)</code>
 */
xssize       xi_sysinfo_user_home(xchar *nbuf, xsize nblen);


/**
 * Returns the name of current system time zone. Time zone names are defined
 *
 * in the <i>tz</i> database, see ftp://elsie.nci.nih.gov/pub/.
 * @param[out] nbuf the pointer to the name string of time-zone
 * @param[in] nblen the length of the buffer (for Windows, its value must be 32-bit long.)
 * @return if OK, return the string length, otherwise <code>FALSE(=0)</code>
 */
xssize       xi_sysinfo_user_tz(xchar *nbuf, xsize nblen);


/**
 * Determines the absolute path of the executing process.
 *
 * @param[out] nbuf the pointer to the requested path string
 * @param[in] nblen the length of the path buffer (for Windows, its value must be 32-bit long.)
 * @return if OK, return the string length, otherwise <code>FALSE(=0)</code>
 *
 * @note The value returned can be freed by <code>STD_FREE</code> macro.
 */
xssize       xi_sysinfo_exec_path(xchar *nbuf, xsize nblen);


/**
 * @}  // end of xi_sysinfo
 */

/**
 * End Declaration
 */
_XI_EXTERN_C_END

#endif // _XI_SYSINFO_H_
