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

/**
 * File : xg_clock.c
 */

#include <errno.h>

#include <sys/time.h>
#include <time.h>

#include "xi/xi_clock.h"

#define DATE_MAX        2147483647L
#define TIME_MAX        0xffffffffUL
#define EPOCH_YR        1970
#define SECS_DAY        86400
#define YEAR0           1900
#define LEAPYEAR(year)  (!((year) % 4) && (((year) % 100) || !((year) % 400)))
#define YEARSIZE(year)  (LEAPYEAR(year) ? 366 : 365)

// ----------------------------------------------
// Global Variables
// ----------------------------------------------

static struct timeval _g_start = { -1, -1 };

static xint32 _g_clock_tzhour = 0;
static xlong _g_clock_gsdiff = 0;

static const xint32 _ytab[2][12] = { { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31,
		30, 31 }, { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 } };

// ----------------------------------------------
// Part Internal Functions
// ----------------------------------------------

static xi_time_t xg_gmtime(register const xlong ntt) {
	xi_time_t br_time;
	register xlong dayclock, dayno;
	xint32 year = EPOCH_YR;

	dayclock = ntt % SECS_DAY;
	dayno = ntt / SECS_DAY;

	br_time.sec = (xint32) (dayclock % 60);
	br_time.min = (xint32) ((dayclock % 3600) / 60);
	br_time.hour = (xint32) (dayclock / 3600);
	//br_time.wday = (dayno + 4) % 7;       /* day 0 was a thursday */
	while (dayno >= YEARSIZE(year)) {
		dayno -= YEARSIZE(year);
		year++;
	}
	br_time.year = year;
	//br_time.yday = dayno;
	br_time.mon = 0;
	while (dayno >= _ytab[LEAPYEAR(year)][br_time.mon]) {
		dayno -= _ytab[LEAPYEAR(year)][br_time.mon];
		br_time.mon++;
	}
	br_time.mon++;
	br_time.day = (xint32) (dayno + 1);

	br_time.msec = 0;
	br_time.utcmsec = (ntt * 1000);

	return br_time;
}

static xlong xg_mktime(register xi_time_t *stt) {
	register xint32 day, year;
	register xint32 tm_year;
	xint32 yday, month;
	register xlong seconds;
	xint32 overflow;

	stt->mon -= 1;
	stt->year -= YEAR0;

	stt->min += stt->sec / 60;
	stt->sec %= 60;
	if (stt->sec < 0) {
		stt->sec += 60;
		stt->min--;
	}
	stt->hour += stt->min / 60;
	stt->min = stt->min % 60;
	if (stt->min < 0) {
		stt->min += 60;
		stt->hour--;
	}
	day = stt->hour / 24;
	stt->hour = stt->hour % 24;
	if (stt->hour < 0) {
		stt->hour += 24;
		day--;
	}
	stt->year += stt->mon / 12;
	stt->mon %= 12;
	if (stt->mon < 0) {
		stt->mon += 12;
		stt->year--;
	}
	day += (stt->day - 1);
	while (day < 0) {
		if (--stt->mon < 0) {
			stt->year--;
			stt->mon = 11;
		}
		day += _ytab[LEAPYEAR(YEAR0 + stt->year)][stt->mon];
	}
	while (day >= _ytab[LEAPYEAR(YEAR0 + stt->year)][stt->mon]) {
		day -= _ytab[LEAPYEAR(YEAR0 + stt->year)][stt->mon];
		if (++(stt->mon) == 12) {
			stt->mon = 0;
			stt->year++;
		}
	}
	stt->day = day + 1;

	year = EPOCH_YR;
	if (stt->year < year - YEAR0)
		return (xint32) -1;
	seconds = 0;
	day = 0; /* means days since day 0 now */
	overflow = 0;

	tm_year = stt->year + YEAR0;

	if (DATE_MAX / 365 < tm_year - year)
		overflow++;
	day = (tm_year - year) * 365;
	if (DATE_MAX - day < (tm_year - year) / 4 + 1)
		overflow++;
	day += (tm_year - year) / 4 + ((tm_year % 4) && tm_year % 4 < year % 4);
	day -= (tm_year - year) / 100 + ((tm_year % 100) && tm_year % 100 < year
			% 100);
	day += (tm_year - year) / 400 + ((tm_year % 400) && tm_year % 400 < year
			% 400);

	yday = month = 0;
	while (month < stt->mon) {
		yday += _ytab[LEAPYEAR(tm_year)][month];
		month++;
	}
	yday += (stt->day - 1);
	if (day + yday < 0)
		overflow++;
	day += yday;

	seconds = (xlong)(((stt->hour * 60L) + stt->min) * 60L + stt->sec);

	if ((xint32) ((TIME_MAX - (xulong)seconds) / SECS_DAY) < day)
		overflow++;
	seconds += day * SECS_DAY;

	if (overflow)
		return (xint32) -1;

	if ((xint32) seconds != seconds)
		return (xint32) -1;
	return (xint32) seconds;
}

// ----------------------------------------------
// XI Functions
// ----------------------------------------------

xint64 xi_clock_msec() {
	struct timeval now;

	gettimeofday(&now, NULL);

	return (xint64) (((xint64) now.tv_sec + _g_clock_gsdiff + (_g_clock_tzhour
			* 3600)) * 1000) + (now.tv_usec / 1000);
}

xint64 xi_clock_ntick() {
	struct timeval now;

	if (_g_start.tv_sec == -1) {
		gettimeofday(&_g_start, NULL);
	}

	gettimeofday(&now, NULL);

	return (xint64) (((double)(now.tv_sec - _g_start.tv_sec) * 1E9)
			+ (((double)now.tv_usec) * 1E3));
}

xint32 xi_clock_get_tz() {
	return _g_clock_tzhour;
}

xi_clock_re xi_clock_set_tz(xint32 tzhour) {
	if (tzhour < -11 || tzhour > 11) {
		return XI_CLOCK_RV_ERR_ARGS;
	}

	_g_clock_tzhour = tzhour;

	return XI_CLOCK_RV_OK;
}

xi_clock_re xi_clock_settime(xi_time_t newtime) {
	xlong mk_utc;
	struct timeval now;

	gettimeofday(&now, NULL);

	mk_utc = xg_mktime(&newtime);

	_g_clock_gsdiff = mk_utc - now.tv_sec;

	return XI_CLOCK_RV_OK;
}

xi_clock_re xi_clock_gettime(xi_time_t *curtime) {
	xlong sys_utc;
	struct timeval now;

	gettimeofday(&now, NULL);

	sys_utc = now.tv_sec;
	sys_utc += _g_clock_gsdiff;
	sys_utc += (_g_clock_tzhour * 3600);

	*curtime = xg_gmtime(sys_utc);
	curtime->msec = (xint32)(now.tv_usec / 1000);
	curtime->utcmsec += curtime->msec;

	return XI_CLOCK_RV_OK;
}

xi_clock_re xi_clock_time2sec(xlong *utc, xi_time_t ptime) {
	*utc = xg_mktime(&ptime);
	if (*utc == -1) {
		return XI_CLOCK_RV_ERR_ARGS;
	}

	return XI_CLOCK_RV_OK;
}

xi_clock_re xi_clock_sec2time(xi_time_t *ptime, xlong utc) {
	*ptime = xg_gmtime(utc);
	return XI_CLOCK_RV_OK;
}

