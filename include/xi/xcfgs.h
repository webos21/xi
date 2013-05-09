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

#ifndef _XCFGS_H_
#define _XCFGS_H_

/**
 * @brief XConfigurations
 *
 * @file : xcfgs.h
 * @date : 2010-08-31
 * @author : Cheolmin Jo (webos21@gmail.com)
 */

/**
 * @defgroup xcfg XConfigurations
 * @ingroup XI
 * @{
 */

/************************************************
 * Global Configuration
 * ----------------------------------------------
 * Add values for configuration
 ************************************************/

#define XCFG_DRVNAME_MAX         4          ///< Maximum length of Win32 Drive-Name
#define XCFG_PATHNAME_MAX        256        ///< Maximum length of Path-Name

#define XCFG_HOSTNAME_MAX        256        ///< Maximum length of Host-Name

#define XCFG_ONAME_C08           8          ///< Object-Name : 8 bytes long
#define XCFG_ONAME_C16           16         ///< Object-Name : 16 bytes long
#define XCFG_ONAME_MAX           32         ///< Maximum length of Object-Name

#define XCFG_MUTEX_MAX           1024       ///< Maximum number of mutex
#define XCFG_COND_MAX            1024       ///< Maximum number of cond
#define XCFG_THREAD_MAX          128        ///< Maximum number of threads

#define XCFG_THREAD_PRIOR_MIN    1          ///< Has minimum priority
#define XCFG_THREAD_PRIOR_NORM   5          ///< Has normal priority
#define XCFG_THREAD_PRIOR_MAX    10         ///< Has maximum priority

#define XCFG_TIMER_SYSTEM_MAX    8          ///< Maximum number of timer instance

#define XCFG_FD_MAX              4096       ///< Maximum number of files and sockets

#define XCFG_GFXMEM_MAX          0x04000000 ///< Maximum size of GFX Memory (64M)

#define XCFG_DECODER_VOLUME_MIN  0          ///< Minimum value of Volume
#define XCFG_DECODER_VOLUME_MAX  50         ///< Maximum value of Volume

#define XCFG_FILTER_MAX          256        ///< Maximum number of filter instance
#define XCFG_FILTER_MASK_MAX     16         ///< Maximum length of filter mask

#define XCFG_CAS_SID_MAX         10         ///< Maximum length of CAS-SID

#define XCFG_DEBUG               1          ///< DEBUG Flag

/**
 * @} // end of xcfg
 */

#endif // _XCFGS_H_
