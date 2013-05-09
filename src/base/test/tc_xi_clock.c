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
 * File : tc_xi_clock.c
 */

#include "xi/xi_clock.h"

#include "xi/xi_log.h"
#include "xi/xi_clock.h"

static void tc_info() {
	log_print(XDLOG, "\n\n");
	log_print(XDLOG, "====================================================\n");
	log_print(XDLOG, "                     xi_clock.h\n");
	log_print(XDLOG, "----------------------------------------------------\n");
	log_print(XDLOG, " * Functions)\n");
	log_print(XDLOG, "   - xi_clock_msec\n");
	log_print(XDLOG, "   - xi_clock_ntick\n");
	log_print(XDLOG, "   - xi_clock_gettime\n");
	log_print(XDLOG, "   - xi_clock_settime\n");
	log_print(XDLOG, "   - xi_clock_get_tz\n");
	log_print(XDLOG, "   - xi_clock_set_tz\n");
	log_print(XDLOG, "   - xi_clock_sec2time\n");
	log_print(XDLOG, "   - xi_clock_time2sec\n");
	log_print(XDLOG, "====================================================\n\n");
}

int tc_xi_clock() {
	xint32 t = 1;
	xchar *tcname = "xi_clock.h";

	xint32 ret;
	xlong secs = 0;

	xi_time_t t1, t2;
	xint64 tick1, tick2;

	tc_info();

	log_print(XDLOG, "[%s:%02d] xi_clock_msec ###############\n", tcname, t++);
	tick1 = xi_clock_msec();
	if (tick1 < 0) {
		log_print(XDLOG, "    - result : failed!!!\n\n");
		return -1;
	}
	log_print(XDLOG, "    - result : pass. (tick1=%lld)\n", tick1);
	ret = xi_clock_sec2time(&t1, (xlong) (tick1 / 1000));
	log_print(XDLOG, "                     (date : %04d/%02d/%02d %02d:%02d:%02d.%03d)\n\n",
			t1.year, t1.mon, t1.day, t1.hour, t1.min, t1.sec, t1.msec);

	log_print(XDLOG, "[%s:%02d] xi_clock_ntick ##############\n", tcname, t++);
	tick2 = xi_clock_ntick();
	if (tick2 < 0) {
		log_print(XDLOG, "    - result : failed!!!\n\n");
		return -1;
	}
	log_print(XDLOG, "    - result : pass. (tick2=%lld)\n\n", tick2);

	log_print(XDLOG, "[%s:%02d] xi_clock_gettime ############\n", tcname, t++);
	ret = xi_clock_gettime(&t1);
	if (ret != XI_CLOCK_RV_OK) {
		log_print(XDLOG, "    - result : failed!!! (ret=%d)\n", ret);
		return -1;
	}
	log_print(XDLOG, "    - result : pass. (date : %04d/%02d/%02d %02d:%02d:%02d.%03d)\n\n",
			t1.year, t1.mon, t1.day, t1.hour, t1.min, t1.sec, t1.msec);

	log_print(XDLOG, "[%s:%02d] xi_clock_set_tz(+9) #########\n", tcname, t++);
	ret = xi_clock_set_tz(9);
	if (ret != XI_CLOCK_RV_OK) {
		log_print(XDLOG, "    - result : failed!!! (ret=%d)\n", ret);
		return -1;
	}
	xi_clock_gettime(&t2);
	log_print(XDLOG, "    - result : pass. (date : %04d/%02d/%02d %02d:%02d:%02d.%03d)\n",
			t2.year, t2.mon, t2.day, t2.hour, t2.min, t2.sec, t2.msec);
	log_print(XDLOG, "                     (%lld)\n\n", xi_clock_msec());

	log_print(XDLOG, "[%s:%02d] xi_clock_get_tz() ###########\n", tcname, t++);
	ret = xi_clock_get_tz();
	if (ret != 9) {
		log_print(XDLOG, "    - result : failed!!! (ret=%d)\n", ret);
		return -1;
	}
	log_print(XDLOG, "    - result : pass. (tz=%+d:00)\n\n", ret);

	log_print(XDLOG, "[%s:%02d] xi_clock_settime(new) #######\n", tcname, t++);
	t2 = t1;
	t2.day = t2.day + 3;
	ret = xi_clock_settime(t2);
	if (ret != XI_CLOCK_RV_OK) {
		log_print(XDLOG, "    - result : failed!!! (ret=%d)\n", ret);
		return -1;
	}
	xi_clock_gettime(&t2);
	log_print(XDLOG, "    - result : pass. (date : %04d/%02d/%02d %02d:%02d:%02d.%03d)\n",
			t2.year, t2.mon, t2.day, t2.hour, t2.min, t2.sec, t2.msec);
	log_print(XDLOG, "                     (%lld)\n\n", xi_clock_msec());

	log_print(XDLOG, "[%s:%02d] xi_clock_settime(old) #######\n", tcname, t++);
	ret = xi_clock_settime(t1);
	if (ret != XI_CLOCK_RV_OK) {
		log_print(XDLOG, "    - result : failed!!! (ret=%d)\n", ret);
		return -1;
	}
	xi_clock_gettime(&t2);
	log_print(XDLOG, "    - result : pass. (date : %04d/%02d/%02d %02d:%02d:%02d.%03d)\n",
			t2.year, t2.mon, t2.day, t2.hour, t2.min, t2.sec, t2.msec);
	log_print(XDLOG, "                     (%lld)\n\n", xi_clock_msec());

	log_print(XDLOG, "[%s:%02d] xi_clock_sec2time() #########\n", tcname, t++);
	ret = xi_clock_sec2time(&t2, (xlong) (xi_clock_msec() / 1000));
	if (ret != XI_CLOCK_RV_OK) {
		log_print(XDLOG, "    - result : failed!!! (ret=%d)\n", ret);
		return -1;
	}
	log_print(XDLOG, "    - result : pass. (date : %04d/%02d/%02d %02d:%02d:%02d.%03d)\n",
			t2.year, t2.mon, t2.day, t2.hour, t2.min, t2.sec, t2.msec);
	log_print(XDLOG, "                     (%lld)\n\n", xi_clock_msec());

	log_print(XDLOG, "[%s:%02d] xi_clock_time2sec() #########\n", tcname, t++);
	ret = xi_clock_time2sec(&secs, t1);
	if (ret != XI_CLOCK_RV_OK) {
		log_print(XDLOG, "    - result : failed!!! (ret=%d)\n", ret);
		return -1;
	}
	log_print(XDLOG, "    - result : pass. (secs : %ld)\n", secs);
	log_print(XDLOG, "                     (date : %04d/%02d/%02d %02d:%02d:%02d.%03d)\n\n",
			t1.year, t1.mon, t1.day, t1.hour, t1.min, t1.sec, t1.msec);

	log_print(XDLOG, "================= DONE [xi_clock.h] ================\n\n");

	return 0;
}
