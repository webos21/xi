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
 * File : tc_xi_thread_java.c
 */

#include "xi/xi_thread.h"
#include "xi/xi_hashtb.h"

#include "xi/xi_log.h"
#include "xi/xi_clock.h"

static xi_thread_t _g_tidm;
static xi_thread_t _g_tid1;
static xi_thread_t _g_tid2;

static xbool _g_trun;
static xint32 _g_ecode;

#define TC_EXIT_ERROR do{_g_ecode = -1; _g_trun = FALSE; return NULL;}while(0)

static void *test_thread1(void *arg) {
	xint32 t = 1;
	xchar *tcname = "xi_thread.h-tt01";

	xint32 ret = 0;

	UNUSED(arg);

	log_print(XDLOG, ">>> TEST_THREAD01 - started (tid=0x%x)\n\n", xi_thread_self());

	xi_thread_sleep(10); // wait to creation of the second thread

	log_print(XDLOG, "  (%s:%02d) suspend all thread ########\n", tcname, t++);
	ret = xi_thread_suspend_all();
	if (ret != XI_TKEY_RV_OK) {
		log_print(XDLOG, "    - result : failed!!! (ret=%d)\n\n", ret);
		TC_EXIT_ERROR;
	}
	log_print(XDLOG, "    - result : pass. (ret=%d)\n\n", ret);

	xi_thread_sleep(4000);

	log_print(XDLOG, "  (%s:%02d) resume all thread #########\n", tcname, t++);
	ret = xi_thread_resume_all();
	if (ret != XI_TKEY_RV_OK) {
		log_print(XDLOG, "    - result : failed!!! (ret=%d)\n\n", ret);
		TC_EXIT_ERROR;
	}
	log_print(XDLOG, "    - result : pass. (ret=%d)\n\n", ret);

	log_print(XDLOG, "  (%s:%02d) suspend main thread #######\n", tcname, t++);
	ret = xi_thread_suspend(_g_tidm);
	if (ret != XI_TKEY_RV_OK) {
		log_print(XDLOG, "    - result : failed!!! (ret=%d)\n\n", ret);
		TC_EXIT_ERROR;
	}
	log_print(XDLOG, "    - result : pass. (ret=%d)\n\n", ret);

	xi_thread_sleep(4000);

	log_print(XDLOG, "  (%s:%02d) resume main thread ########\n", tcname, t++);
	ret = xi_thread_resume(_g_tidm);
	if (ret != XI_TKEY_RV_OK) {
		log_print(XDLOG, "    - result : failed!!! (ret=%d)\n\n", ret);
		TC_EXIT_ERROR;
	}
	log_print(XDLOG, "    - result : pass. (ret=%d)\n\n", ret);

	log_print(XDLOG, "  (%s:%02d) wait with print '.' #######\n\n", tcname, t++);
	while (_g_trun) {
		xi_thread_sleep(100);
		log_print(XDLOG, ".");
	}

	log_print(XDLOG, "<<< TEST_THREAD01 - exited\n\n");
	return NULL;
}

static void *test_thread2(void *arg) {
	xint32 t = 1;
	xchar *tcname = "xi_thread.h-tt02";

	xint32 i = 0;

	UNUSED(arg);

	log_print(XDLOG, ">>> TEST_THREAD02 - started (tid=0x%x)\n\n", xi_thread_self());

	log_print(XDLOG, "  (%s:%02d) wait all operations #####\n\n", tcname, t++);
	for (i=0; i<100; i++) {
		xi_thread_sleep(100);
		log_print(XDLOG, "?");
	}

	log_print(XDLOG, "  (%s:%02d) leave test!!!! ##########\n\n", tcname, t++);
	_g_trun = FALSE;

	log_print(XDLOG, "<<< TEST_THREAD02 - exited\n\n");
	return NULL;
}

static void tc_info() {
	log_print(XDLOG, "\n\n");
	log_print(XDLOG, "====================================================\n");
	log_print(XDLOG, "                xi_thread.h - java\n");
	log_print(XDLOG, "----------------------------------------------------\n");
	log_print(XDLOG, " * Test Items)\n");
	log_print(XDLOG, "   > make a thread to be suspendless\n");
	log_print(XDLOG, "   > test the suspending and resuming  to that threads\n");
	log_print(XDLOG, "====================================================\n\n");
}

int tc_xi_thread_java() {
	xint32 t = 1;
	xchar *tcname = "xi_thread.h";

	xint32 ret;

	tc_info();

	log_print(XDLOG, "[%s:%02d] main thread tid #############\n", tcname, t++);
	_g_tidm = xi_thread_self();
	if (_g_tidm == 0xFFFFFFFF || _g_tidm == 0x00000000) {
		log_print(XDLOG, "    - result : failed!!! ((tid=0x%x)\n\n", _g_tidm);
		return -1;
	}
	log_print(XDLOG, "    - result : pass. (_g_tidm=0x%x)\n\n", _g_tidm);

	log_print(XDLOG, "[%s:%02d] main thread stack info ######\n", tcname, t++);
	log_print(XDLOG, "    - result : pass. (main thread : stack_base=%p, stack_top=%p)\n\n",
		xi_thread_get_stackbase(_g_tidm),
		xi_thread_get_stacktop(_g_tidm)
		);

	log_print(XDLOG, "[%s:%02d] main thread is suspendable ##\n", tcname, t++);
	ret = xi_thread_is_suspendable(_g_tidm);
	log_print(XDLOG, "    - result : pass. (ret=%d)\n\n", ret);

	log_print(XDLOG, "[%s:%02d] set main thread suspendless #\n", tcname, t++);
	ret = xi_thread_disable_suspend(_g_tidm);
	if (ret < 0) {
		log_print(XDLOG, "    - result : failed!!! ((ret=%d)\n\n", ret);
		return -1;
	}
	log_print(XDLOG, "    - result : pass. (ret=%d)\n\n", ret);

	log_print(XDLOG, "[%s:%02d] main thread is suspendable ##\n", tcname, t++);
	ret = xi_thread_is_suspendable(_g_tidm);
	log_print(XDLOG, "    - result : pass. (ret=%d)\n\n", ret);

	log_print(XDLOG, "[%s:%02d] set main thread suspendable #\n", tcname, t++);
	ret = xi_thread_enable_suspend(_g_tidm);
	if (ret < 0) {
		log_print(XDLOG, "    - result : failed!!! ((ret=%d)\n\n", ret);
		return -1;
	}
	log_print(XDLOG, "    - result : pass. (ret=%d)\n\n", ret);

	log_print(XDLOG, "[%s:%02d] create test threads #########\n", tcname, t++);
	ret = xi_thread_create(&_g_tid1, "TEST_THREAD01", test_thread1, NULL,
		256 * 1024, XCFG_THREAD_PRIOR_NORM);
	if (ret != XI_THREAD_RV_OK) {
		log_print(XDLOG, "    - result : failed!!! (ret=%d)\n\n", ret);
		return -1;
	}
	ret = xi_thread_create(&_g_tid2, "TEST_THREAD02", test_thread2, NULL,
		256 * 1024, XCFG_THREAD_PRIOR_NORM);
	if (ret != XI_THREAD_RV_OK) {
		log_print(XDLOG, "    - result : failed!!! (ret=%d)\n\n", ret);
		return -1;
	}
	log_print(XDLOG, "    - result : pass. (tid1=0x%x/tid2=0x%x)\n", _g_tid1, _g_tid2);
	log_print(XDLOG, "               check below logs!!\n\n");

	log_print(XDLOG, "[%s:%02d] main wait loop ##############\n\n", tcname, t++);
	_g_trun = TRUE;
	while (_g_trun) {
		xi_thread_sleep(500);
		log_print(XDLOG, "\n >>> main wait loop .... \n");
	}
	log_print(XDLOG, "    - result : main thread end.\n\n");

	log_print(XDLOG, "============ DONE [xi_thread.h - java] ============\n\n");

	return 0;
}
