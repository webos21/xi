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
 * File   : tc_xi_dso.c
 */

#include "xi/xi_dso.h"

#include "xi/xi_log.h"

static void tc_info() {
	log_print(XDLOG, "====================================================\n");
	log_print(XDLOG, "                      xi_dso.h\n");
	log_print(XDLOG, "----------------------------------------------------\n");
	log_print(XDLOG, " * Functions)\n");
	log_print(XDLOG, "   - xi_dso_get_searchpath\n");
	log_print(XDLOG, "   - xi_dso_get_sysname\n");
	log_print(XDLOG, "   - xi_dso_load\n");
	log_print(XDLOG, "   - xi_dso_error\n");
	log_print(XDLOG, "   - xi_dso_get_func\n");
	log_print(XDLOG, "   - xi_dso_unload\n");
	log_print(XDLOG, "====================================================\n\n");
}

int tc_xi_dso() {
	xint32 t = 1;
	xchar *tcname = "xi_dso.h";

	xssize ret;
	xchar sbuf[1024];

	xi_dso_t *dso;
	xint64 (*msec)();

	tc_info();

	log_print(XDLOG, "[%s:%02d] get_searchpath ##############\n", tcname, t++);
	ret = xi_dso_get_searchpath(sbuf, sizeof(sbuf));
	if (ret < 0) {
		log_print(XDLOG, "    - result : no-path!!!\n\n");
	} else {
		log_print(XDLOG, "    - result : pass. (path=%s)\n\n", sbuf);
	}

	log_print(XDLOG, "[%s:%02d] get_sysname #################\n", tcname, t++);
	ret = xi_dso_get_sysname(sbuf, "xg");
	if (ret < 0) {
		log_print(XDLOG, "    - result : failed!!!\n\n");
		return -1;
	}
	log_print(XDLOG, "    - result : pass. (name=%s)\n\n", sbuf);

	log_print(XDLOG, "[%s:%02d] load 'xxxx' for err! ##########\n", tcname, t++);
	dso = xi_dso_load("xxxx");
	if (dso != NULL) {
		log_print(XDLOG, "    - result : failed!!!\n\n");
		return -1;
	}
	log_print(XDLOG, "    - result : pass. (err-msg = %s)\n\n", xi_dso_error());

	log_print(XDLOG, "[%s:%02d] load '%s' ##############\n", tcname, t++, sbuf);
	dso = xi_dso_load(sbuf);
	if (dso == NULL) {
		log_print(XDLOG, "    - result : failed!!! (err-msg = %s)\n\n", xi_dso_error());
		return -1;
	}
	log_print(XDLOG, "    - result : pass. (dso=%p)\n\n", dso);

	log_print(XDLOG, "[%s:%02d] get func 'xi_clock_msec' ####\n", tcname, t++);
	msec = xi_dso_get_func(dso, "xi_clock_msec");
	if (msec == NULL) {
		log_print(XDLOG, "    - result : failed!!! (err-msg = %s)\n\n", xi_dso_error());
		return -1;
	}
	log_print(XDLOG, "    - result : pass. (msec=%p)\n\n", msec);

	log_print(XDLOG, "[%s:%02d] use 'xi_clock_msec' func ####\n", tcname, t++);
	log_print(XDLOG, "xi_clock_msec() = %lld\n\n", (*msec)());

	log_print(XDLOG, "[%s:%02d] unload '%s' ############\n", tcname, t++, sbuf);
	if (!xi_dso_unload(dso)) {
		log_print(XDLOG, "    - result : failed!!!\n\n");
		return -1;
	}
	log_print(XDLOG, "    - result : pass.\n\n");

	log_print(XDLOG, "================== DONE [xi_dso.h] =================\n\n");

	return 0;
}
