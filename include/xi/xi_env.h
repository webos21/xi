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

#ifndef _XI_ENV_H_
#define _XI_ENV_H_

/**
 * @brief XI Environment Variables API
 * 
 * @file xi_env.h
 * @date 2010-08-31
 * @author Cheolmin Jo (webos21@gmail.com)
 */

#include "xtype.h"

/**
 * Start Declaration
 */
_XI_EXTERN_C_BEGIN

/**
 * @defgroup xi_env Environment Variables API
 * @ingroup XI
 * @{
 */

/**
 * Get the all environment variables.
 *  - You shoude give the sufficient size of char-array.
 *  ex) 
 *    char aenv[128];
 *    xi_mem_set(aenv, sizeof(aenv));
 *    xi_env_all(aenv, 128, 512);
 *
 * @param aenv  The multi-dimension character-array
 * @param num   The expected number of environment variables
 * @return The number of all environment variables; otherwise -1.
 */
xssize  xi_env_all(xchar * aenv[], xsize num);


/**
 * Get the value of an environment variable.
 *
 * @param key    the name of the environment variable
 * @param buf    a buffer to be filled
 * @param buflen the size of buffer
 * @return The lenth of value; otherwise 0(not-found).
 */
xssize  xi_env_get(const xchar *key, xchar *buf, xsize buflen);


/**
 * Set the value of an environment variable.
 *
 * @param key the name of the environment variable
 * @param val the value to set
 * @return true when 0; otherwise -1.
 */
xssize  xi_env_set(const xchar *key, const xchar *val);


/**
 * Delete a variable from the environment.
 *
 * @param key the name of the environment variable
 * @return true when 0; otherwise -1.
 */
xssize  xi_env_del(const xchar *key);

/**
 * @}  // end of xi_env
 */

/**
 * End Declaration
 */
_XI_EXTERN_C_END

#endif // _XI_ENV_H_
