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

#include <windows.h>

#include <errno.h>
#include <time.h>

#include "xi/xi_clock.h"

#define CLOCK_TICK_WRAP (~(DWORD)0)

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

static LARGE_INTEGER _g_frequency;

static xint32 _g_clock_tzhour = 0;
static xlong _g_clock_gsdiff = 0;

static const xint32 _ytab[2][12] = { { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31,
		30, 31 }, { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 } };

// ----------------------------------------------
// Part Internal Functions
// ----------------------------------------------

static xi_time_t xg_gmtime(register const xlong ntt) {
	xi_time_t br_time;
	register xint32 dayclock, dayno;
	xint32 year = EPOCH_YR;

	dayclock = (xint32)(ntt % SECS_DAY);
	dayno = (xint32)(ntt / SECS_DAY);

	br_time.sec = dayclock % 60;
	br_time.min = (dayclock % 3600) / 60;
	br_time.hour = dayclock / 3600;
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
	br_time.day = dayno + 1;

	br_time.msec = 0;
	br_time.utcmsec = (ntt * 1000);

	return br_time;
}

static xlong xg_mktime(register xi_time_t *stt) {
	register xint32 day, year;
	register xint32 tm_year;
	xint32 yday, month;
	register xint32 seconds;
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

	seconds = ((stt->hour * 60L) + stt->min) * 60L + stt->sec;

	if ((xint32) ((TIME_MAX - seconds) / SECS_DAY) < day)
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
	xint64 ret;
	SYSTEMTIME now;

	GetSystemTime(&now);
	ret  = time(NULL);
	ret += _g_clock_gsdiff;
	ret += (_g_clock_tzhour * 3600);
	ret *= 1000;
	ret += now.wMilliseconds;
	return ret;
}

xint64 xi_clock_ntick() {
	static xbool init = FALSE;
	LARGE_INTEGER count;

	if (!init) {
		QueryPerformanceFrequency(&_g_frequency);
	}

	if (QueryPerformanceCounter(&count)) {
		return (xint64) ((double) count.QuadPart / _g_frequency.QuadPart * 1E9);
	} else {
		return (xint64) (GetTickCount() * 1E6);
	}
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
	xint32 sys_utc;
	xlong mk_utc;

	sys_utc = (xint32) time(NULL);
	mk_utc = xg_mktime(&newtime);

	_g_clock_gsdiff = mk_utc - sys_utc;

	return XI_CLOCK_RV_OK;
}

xi_clock_re xi_clock_gettime(xi_time_t *curtime) {
	xlong sys_utc;

	SYSTEMTIME now;

	sys_utc = (xlong) time(NULL);
	sys_utc += _g_clock_gsdiff;
	sys_utc += (_g_clock_tzhour * 3600);

	GetSystemTime(&now);

	*curtime = xg_gmtime(sys_utc);
	curtime->msec = now.wMilliseconds;
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
