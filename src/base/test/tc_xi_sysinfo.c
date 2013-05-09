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
 * File   : tc_xi_sysinfo.c
 */

#include "xi/xi_sysinfo.h"

#include "xi/xi_log.h"
#include "xi/xi_mem.h"
#include "xi/xi_thread.h"
#include "xi/xi_string.h"

static void tc_info() {
	log_print(XDLOG, "====================================================\n");
	log_print(XDLOG, "                    xi_sysinfo.h\n");
	log_print(XDLOG, "----------------------------------------------------\n");
	log_print(XDLOG, " * Functions)\n");
	log_print(XDLOG, "   - xi_sysinfo_exec_path\n");
	log_print(XDLOG, "   - xi_sysinfo_cpu_arch\n");
	log_print(XDLOG, "   - xi_sysinfo_cpu_num\n");
	log_print(XDLOG, "   - xi_sysinfo_os_name\n");
	log_print(XDLOG, "   - xi_sysinfo_os_ver\n");
	log_print(XDLOG, "   - xi_sysinfo_user_name\n");
	log_print(XDLOG, "   - xi_sysinfo_user_home\n");
	log_print(XDLOG, "   - xi_sysinfo_user_tz\n");
	log_print(XDLOG, "====================================================\n\n");
}

int tc_xi_sysinfo() {
	xint32 t = 1;
	xchar *tcname = "xi_sysinfo.h";

	xssize ret;
	xchar lbuf[1024];
	xchar sbuf[64];
	xchar *strptr = NULL;

	tc_info();

	log_print(XDLOG, "[%s:%02d] xi_sysinfo_exec_path ########\n", tcname, t++);
	ret = xi_sysinfo_exec_path(lbuf, sizeof(lbuf));
	if (ret < 0) {
		log_print(XDLOG, "    - result : failed!!!\n\n");
		return -1;
	}
	log_print(XDLOG, "    - result : pass. (strlen=%d/exec_path=%s)\n\n", ret, lbuf);

	log_print(XDLOG, "[%s:%02d] xi_sysinfo_cpu_arch #########\n", tcname, t++);
	strptr = (xchar*) xi_sysinfo_cpu_arch(lbuf, sizeof(lbuf));
	if (strptr == NULL) {
		log_print(XDLOG, "    - result : failed!!!\n\n");
		return -1;
	}
	log_print(XDLOG, "    - result : pass. (cpu_arch=%s)\n\n", strptr);

	log_print(XDLOG, "[%s:%02d] xi_sysinfo_cpu_num ##########\n", tcname, t++);
	ret = xi_sysinfo_cpu_num();
	if (ret < 0) {
		log_print(XDLOG, "    - result : failed!!!\n\n");
		return -1;
	}
	log_print(XDLOG, "    - result : pass. (cpu_num=%d)\n\n", ret);

	log_print(XDLOG, "[%s:%02d] xi_sysinfo_os_name ##########\n", tcname, t++);
	ret = xi_sysinfo_os_name(sbuf, sizeof(sbuf));
	if (ret < 0) {
		log_print(XDLOG, "    - result : failed!!!\n\n");
		return -1;
	}
	log_print(XDLOG, "    - result : pass. (strlen=%d/os_name=%s)\n\n", ret, sbuf);

	log_print(XDLOG, "[%s:%02d] xi_sysinfo_os_ver ###########\n", tcname, t++);
	ret = xi_sysinfo_os_ver(sbuf, sizeof(sbuf));
	if (ret < 0) {
		log_print(XDLOG, "    - result : failed!!!\n\n");
		return -1;
	}
	log_print(XDLOG, "    - result : pass. (strlen=%d/os_ver=%s)\n\n", ret, sbuf);

	log_print(XDLOG, "[%s:%02d] xi_sysinfo_user_name ########\n", tcname, t++);
	ret = xi_sysinfo_user_name(sbuf, sizeof(sbuf));
	if (ret < 0) {
		log_print(XDLOG, "    - result : failed!!!\n\n");
		return -1;
	}
	log_print(XDLOG, "    - result : pass. (strlen=%d/user_name=%s)\n\n", ret, sbuf);

	log_print(XDLOG, "[%s:%02d] xi_sysinfo_user_home ########\n", tcname, t++);
	ret = xi_sysinfo_user_home(lbuf, sizeof(lbuf));
	if (ret < 0) {
		log_print(XDLOG, "    - result : failed!!!\n\n");
		return -1;
	}
	log_print(XDLOG, "    - result : pass. (strlen=%d/user_home=%s)\n\n", ret, lbuf);

	log_print(XDLOG, "[%s:%02d] xi_sysinfo_user_home ########\n", tcname, t++);
	ret = xi_sysinfo_user_tz(sbuf, sizeof(sbuf));
	if (ret < 0) {
		log_print(XDLOG, "    - result : failed!!!\n\n");
		return -1;
	}
	log_print(XDLOG, "    - result : pass. (strlen=%d/user_tz=%s)\n\n", ret, sbuf);

	log_print(XDLOG, "================ DONE [xi_sysinfo.h] ===============\n\n");

	return 0;
}
