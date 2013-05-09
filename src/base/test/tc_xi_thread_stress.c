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
 * File : tc_xi_thread_stress.c
 */

#include "xi/xi_thread.h"

#include "xi/xi_log.h"
#include "xi/xi_mem.h"
#include "xi/xi_string.h"

static void *test_thread_massive(void *arg) {
	xint32 ret;
	xint32 i;

	xchar cname[32];
	xchar lname[32];
	xchar tname[32];

	xi_thread_cond_t cond;
	xi_thread_mutex_t lock;

	xuintptr tcnt = (xuintptr) arg;

	xi_sprintf(tname, "t%02d", tcnt);
	xi_sprintf(cname, "c%s", tname);
	xi_sprintf(lname, "l%x", tname);

	log_print(XDLOG, "[%s] enter >>>>\n", tname);

	ret = xi_thread_cond_create(&cond, cname);
	if (ret != XI_COND_RV_OK) {
		log_print(XDLOG, "[%s] cond create error!! (ret=%d)\n", tname, ret);
		return NULL;
	}
	ret = xi_thread_mutex_create(&lock, lname);
	if (ret != XI_MUTEX_RV_OK) {
		log_print(XDLOG, "[%s] mutex create error!! (ret=%d)\n", tname, ret);
		return NULL;
	}

	for (i = 0; i < 15; i++) {
		xi_thread_sleep(1000);
		log_print(XDLOG, "%s ", tname);
	}

	ret = xi_thread_mutex_destroy(&lock);
	if (ret != XI_MUTEX_RV_OK) {
		log_print(XDLOG, "[%s] mutex destroy error!! (ret=%d)\n", tname, ret);
	}
	ret = xi_thread_cond_destroy(&cond);
	if (ret != XI_COND_RV_OK) {
		log_print(XDLOG, "[%s] cond destroy error!! (ret=%d)\n", tname, ret);
	}

	log_print(XDLOG, "[%s] exit <<<<<\n", tname);

	return NULL;
}

static void tc_info() {
	log_print(XDLOG, "====================================================\n");
	log_print(XDLOG, "                xi_thread.h - stress\n");
	log_print(XDLOG, "----------------------------------------------------\n");
	log_print(XDLOG, " * Test Items)\n");
	log_print(XDLOG, "   > massive creation / destruction\n");
	log_print(XDLOG, "     - thread / mutex / cond / tkey\n");
	log_print(XDLOG, "   > many threads suspend/resume\n");
	log_print(XDLOG, "====================================================\n\n");
}

int tc_xi_thread_stress() {
	xint32 t = 1;
	xchar *tcname = "xi_thread.h";

	xint32 ret;
	xintptr i;

	xi_thread_t tid[64];

	tc_info();

	log_print(XDLOG, "[%s:%02d] new(thread/mutex/cond)s #####\n", tcname, t++);
	for (i = 0; i < 64; i++) {
		xchar tname[16];
		xi_mem_set(tname, 0, sizeof(tname));
		xi_sprintf(tname, "TEST_THREAD-%02d", i);
		ret = xi_thread_create(&tid[i], tname, test_thread_massive, (xvoid*) i,
				256 * 1024, XCFG_THREAD_PRIOR_NORM);
		if (ret != XI_THREAD_RV_OK) {
			log_print(XDLOG, "    - result : thread creation error(i=%d)\n\n", i);
			return -1;
		}
	}
	log_print(XDLOG, "    - result : pass. (i=%d)\n\n", i);

	log_print(XDLOG, "[%s:%02d] wait all thread dead ########\n\n", tcname, t++);
	xi_thread_sleep(25000);
	log_print(XDLOG, "    - result : all thread dead wait.\n\n");

	log_print(XDLOG, "[%s:%02d] renew(thread/mutex/cond)s ###\n", tcname, t++);
	for (i = 0; i < 64; i++) {
		xchar tname[16];
		xi_mem_set(tname, 0, sizeof(tname));
		xi_sprintf(tname, "TEST_THREAD-%02d", i);
		ret = xi_thread_create(&tid[i], tname, test_thread_massive, (xvoid*) i,
				256 * 1024, XCFG_THREAD_PRIOR_NORM);
		if (ret != XI_THREAD_RV_OK) {
			log_print(XDLOG, "    - result : thread creation error(i=%d)\n\n", i);
			return -1;
		}
	}
	log_print(XDLOG, "\n    - result : pass. (i=%d)\n\n", i);

	log_print(XDLOG, "[%s:%02d] wait after renew #######\n\n", tcname, t++);
	xi_thread_sleep(2000);

	log_print(XDLOG, "\n[%s:%02d] suspend even threads ########\n", tcname, t++);
	for (i = 0; i < 64; i += 2) {
		ret = xi_thread_suspend(tid[i]);
		if (ret != XI_THREAD_RV_OK) {
			log_print(XDLOG, "\n    - result : thread suspend error(i=%d)\n\n", i);
			return -1;
		}
	}
	log_print(XDLOG, "\n    - result : pass. (i=%d)\n\n", i);

	log_print(XDLOG, "[%s:%02d] wait after suspends #########\n\n", tcname, t++);
	xi_thread_sleep(5000);

	log_print(XDLOG, "\n[%s:%02d] resume even threads #######\n", tcname, t++);
	for (i = 0; i < 64; i += 2) {
		ret = xi_thread_resume(tid[i]);
		if (ret != XI_THREAD_RV_OK) {
			log_print(XDLOG, "\n    - result : thread resume error(i=%d)\n\n", i);
			return -1;
		}
	}
	log_print(XDLOG, "\n    - result : pass. (i=%d)\n\n", i);

	log_print(XDLOG, "\n[%s:%02d] wait after resume ###########\n\n", tcname, t++);
	xi_thread_sleep(5000);

	log_print(XDLOG, "\n[%s:%02d] suspend all threads #########\n\n", tcname, t++);
	xi_thread_suspend_all();

	log_print(XDLOG, "\n[%s:%02d] wait after suspend_all ######\n\n", tcname, t++);
	xi_thread_sleep(5000);

	log_print(XDLOG, "\n[%s:%02d] resume all threads ##########\n\n", tcname, t++);
	xi_thread_resume_all();

	log_print(XDLOG, "\n[%s:%02d] wait all thread dead ########\n\n", tcname, t++);
	xi_thread_sleep(10000);
	log_print(XDLOG, "\n    - result : all thread dead wait.\n\n");

	log_print(XDLOG, "============ DONE [xi_thread.h - stress] ===========\n\n");

	return 0;
}
