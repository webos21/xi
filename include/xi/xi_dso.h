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

#ifndef _XI_DSO_H_
#define _XI_DSO_H_

/**
 * @brief XI DSO(Dynamic Shared Object) API
 *
 * @file xi_dso.h
 * @date 2010-08-31
 * @author Cheolmin Jo (webos21@gmail.com)
 */

#include "xtype.h"

/**
 * Start Declaration
 */
_XI_EXTERN_C_BEGIN

/**
 * @defgroup xi_dso DSO(Dynamic Shared Object) API
 * @ingroup XI
 * @{
 */

/**
 * Abstract handle of DSO
 */
typedef struct _xi_dso xi_dso_t;


/**
 * Load a DSO library.
 *
 * @param path Path to the DSO library
 * @return new handle for the DSO
 */
xi_dso_t    *xi_dso_load(const xchar *path);


/**
 * Get the function pointer from a DSO handle.
 *
 * @param dso handle to load the function pointer from.
 * @param funcname Name of the symbol to load.
 * @return function pointer
 */
xvoid       *xi_dso_get_func(xi_dso_t *dso, const xchar *funcname);


/**
 * Get the error string of DSO operation.
 *
 * @return string
 */
xchar       *xi_dso_error();


/**
 * Get the search-path of DSO.
 *
 * @param pathbuf character-array to receive the search-path (the length must be enough).
 * @param pbuflen the length of the @a pathbuf
 * @return length of the search-path; otherwise -1.
 */
xssize       xi_dso_get_searchpath(xchar *pathbuf, xsize pbuflen);


/**
 * Get the system-name of DSO.
 *
 * @param sysnamebuf character-array to receive the system-name (the length must be enough).
 * @param name name of the DSO.
 * @return length of the system-name of DSO.
 */
xssize       xi_dso_get_sysname(xchar *sysnamebuf, const xchar *name);


/**
 * Close a DSO library.
 *
 * @param dso handle to unload.
 * @return TRUE on success; otherwise FALSE.
 */
xbool        xi_dso_unload(xi_dso_t *dso);

/**
 * @}  // end of xi_dso
 */

/**
 * End Declaration
 */
_XI_EXTERN_C_END

#endif // _XI_DSO_H_
