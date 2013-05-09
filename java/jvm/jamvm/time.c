/*
 * Copyright (C) 2008 Robert Lougher <rob@jamvm.org.uk>.
 *
 * This file is part of JamVM.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2,
 * or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include "xi/xi_clock.h"

int getTimeoutAbsolute(long long millis, long long nanos) {
	int utc;
	long long tt;
	xi_time_t curtime;

	xi_clock_gettime(&curtime);
	xi_clock_time2sec(&utc, curtime);

	tt = millis - (utc * 1000);
	if (tt > 0) {
		tt += (nanos / 1000000);
		return (int) ((tt > XI_LONG_MAX) ? XI_LONG_MAX : tt);
	} else {
		return (nanos / 1000000);
	}
}

int getTimeoutRelative(long long millis, long long nanos) {
	if (millis > XI_LONG_MAX) {
		return XI_LONG_MAX;
	} else {
		long long tt = millis;
		tt += (nanos / 1000000);

		return (int) ((tt > XI_LONG_MAX) ? XI_LONG_MAX : tt);
	}
}
