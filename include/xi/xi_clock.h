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

#ifndef _XI_CLOCK_H_
#define _XI_CLOCK_H_

/**
 * @brief XI Clock API
 *
 * @file xi_clock.h
 * @date 2010-08-31
 * @author Cheolmin Jo (webos21@gmail.com)
 */

#include "xtype.h"

/**
 * Start Declaration
 */
_XI_EXTERN_C_BEGIN

/**
 * @defgroup xi_clock Clock API
 * @ingroup XI
 * @{
 */

/**
 * Return values of Clock Functions
 */
typedef enum _e_clock_rv {
	XI_CLOCK_RV_OK       = 0,  ///< OK
	XI_CLOCK_RV_ERR_ARGS = -1  ///< Invalid arguments.
} xi_clock_re;


/**
 * Time Structure
 */
typedef struct _st_time {
	xint32 year;     ///< Year(1970 ~ 2050)
	xint32 mon;      ///< Month of Year(1 ~ 12)
	xint32 day;      ///< Day Of Month(1~31)
	xint32 hour;     ///< Hour of day(0 ~ 23)
	xint32 min;      ///< Minutes(0 ~ 59)
	xint32 sec;      ///< Seconds(0 ~ 59)
	xint32 msec;     ///< Milliseconds (0 ~ 999)
	xint64 utcmsec;  ///< Milliseconds of UTC based (like Epoch)
} xi_time_t;


/**
 * Get current time by Milliseconds
 *
 * @return	xint64 Milliseconds
 */
xint64      xi_clock_msec();


/**
 * Tick from the system-boot by Nanoseconds.
 *
 * @return	xint64 Nanoseconds
 */
xint64      xi_clock_ntick();


/**
 * Get Time-Zone
 *
 * @return	the time-zone value, -12 < timezone hour < 12 (ex: KST = +9)
 */
xint32      xi_clock_get_tz();


/**
 * Set Time-Zone
 *
 * @param  	tzhour the time-zone value, -12 < timezone hour < 12 (ex: KST = +9)
 * @return	xi_clock_re
 */
xi_clock_re xi_clock_set_tz(xint32 tzhour);


/**
 * Set System-Time
 *
 * @param  	newtime New System-Time
 * @return	xi_clock_re
 */
xi_clock_re xi_clock_settime(xi_time_t newtime);


/**
 * Get System-Time
 *
 * @param  	curtime Current System-Time
 * @return	xi_clock_re
 */
xi_clock_re xi_clock_gettime(xi_time_t *curtime);


/**
 * Convert the xi_time_t to UTC-Seconds
 *
 * @param  	utc variable for converted UTC-Seconds
 * @param  	ptime xi_time_t to be converted
 * @return	xi_clock_re
 */
xi_clock_re xi_clock_time2sec(xlong *utc, xi_time_t ptime);


/**
 * Convert the UTC-Seconds to xi_time_t
 *
 * @param  	ptime variable for converted xi_time_t
 * @param  	utc UTC-Seconds to be converted
 * @return	xi_clock_re
 */
xi_clock_re xi_clock_sec2time(xi_time_t *ptime, xlong utc);


/**
 * @}  // end of xi_clock
 */

/**
 * End Declaration
 */
_XI_EXTERN_C_END

#endif // _XI_CLOCK_H_
