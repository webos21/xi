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
 * File   : tc_xi_proc.c
 */

#include "xi/xi_process.h"

#include "xi/xi_log.h"
#include "xi/xi_mem.h"
#include "xi/xi_thread.h"
#include "xi/xi_string.h"

#ifdef WIN32
#define PING_CMD "ping"
#else // !WIN32
#	ifdef __APPLE__
#		define PING_CMD "/sbin/ping"
#   else
#		define PING_CMD "/bin/ping"
#	endif
#endif // WIN32

static void tc_info() {
	log_print(XDLOG, "====================================================\n");
	log_print(XDLOG, "                    xi_process.h\n");
	log_print(XDLOG, "----------------------------------------------------\n");
	log_print(XDLOG, " * Functions)\n");
	log_print(XDLOG, "   - xi_proc_mutex_create\n");
	log_print(XDLOG, "   - xi_proc_mutex_open\n");
	log_print(XDLOG, "   - xi_proc_mutex_lock\n");
	log_print(XDLOG, "   - xi_proc_mutex_unlock\n");
	log_print(XDLOG, "   - xi_proc_mutex_close\n");
	log_print(XDLOG, "   - xi_proc_mutex_destroy\n");
	log_print(XDLOG, "   > xi_proc_create\n");
	log_print(XDLOG, "   - xi_proc_detach\n");
	log_print(XDLOG, "   > xi_proc_getpid\n");
	log_print(XDLOG, "   > xi_proc_waitpid\n");
	log_print(XDLOG, "   > xi_proc_term\n");
	log_print(XDLOG, "   - xi_proc_atexit\n");
	log_print(XDLOG, "   - xi_proc_exit\n");
	log_print(XDLOG, "   - xi_proc_abort\n");
	log_print(XDLOG, "====================================================\n\n");
}

int tc_xi_proc() {
	xint32 t = 1;
	xchar *tcname = "xi_process.h";

	xint32 ret;
	xint32 cpid;
	xint32 status;

	xchar * const cmdline[] = { PING_CMD, "-nc", "10", "localhost", NULL };

	tc_info();

	log_print(XDLOG, "[%s:%02d] xi_proc_getpid ##############\n", tcname, t++);
	ret = xi_proc_getpid();
	if (ret < 0) {
		log_print(XDLOG, "    - result : failed!!! (ret=%d)\n\n", ret);
		return -1;
	}
	log_print(XDLOG, "    - result : pass. (current pid=%d)\n\n", ret);

	log_print(XDLOG, "[%s:%02d] xi_proc_create ##############\n", tcname, t++);
	cpid = xi_proc_create(cmdline, 4, NULL, 0, NULL);
	if (cpid < 0) {
		log_print(XDLOG, "    - result : failed!!!\n\n");
		return -1;
	}
	log_print(XDLOG, "    - result : pass. (child-pid=%d)\n\n", cpid);

	log_print(XDLOG, "[%s:%02d] xi_proc_term ################\n", tcname, t++);
	ret = xi_proc_term(cpid);
	if (ret < 0) {
		log_print(XDLOG, "    - result : failed!!!\n\n");
		return -1;
	}
	log_print(XDLOG, "    - result : pass. (child-pid=%d)\n\n", cpid);

	log_print(XDLOG, "[%s:%02d] xi_proc_create ##############\n", tcname, t++);
	cpid = xi_proc_create(cmdline, 4, NULL, 0, NULL);
	if (cpid < 0) {
		log_print(XDLOG, "    - result : failed!!!\n\n");
		return -1;
	}
	log_print(XDLOG, "    - result : pass. (child-pid=%d)\n\n", cpid);

	log_print(XDLOG, "[%s:%02d] xi_proc_waitpid #############\n", tcname, t++);
	ret = xi_proc_waitpid(cpid, &status);
	if (ret < 0) {
		log_print(XDLOG, "    - result : failed!!! (ret=%d)\n\n", ret);
		return -1;
	}
	log_print(XDLOG, "    - result : pass. (child-process id=%d, status=%d)\n\n", ret, status);

	log_print(XDLOG, "================ DONE [xi_process.h] ===============\n\n");

	return 0;
}
