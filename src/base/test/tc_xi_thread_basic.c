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
 * File : tc_xi_thread_basic.c
 */

#include "xi/xi_thread.h"

#include "xi/xi_log.h"
#include "xi/xi_clock.h"

static xi_thread_t _g_tid1;
static xi_thread_t _g_tid2;
static xi_thread_cond_t _g_cond;
static xi_thread_mutex_t _g_mutex;

static xbool _g_trun;

static xint32 _g_ecode;

#define TC_EXIT_ERROR do{_g_ecode = -1; _g_trun = FALSE; return NULL;}while(0)

static void *test_thread1(void *arg) {
	xint32 t = 1;
	xchar *tcname = "xi_thread.h-tt01";

	xint32 ret = 0;
	xvoid *val = NULL;
	xi_thread_key_t tkey;

	UNUSED(arg);

	log_print(XDLOG, ">>> TEST_THREAD01 - started (tid=0x%x)\n\n", xi_thread_self());

	xi_thread_sleep(10); // wait to creation of the second thread

	log_print(XDLOG, "  (%s:%02d) create tkey ###############\n", tcname, t++);
	ret = xi_thread_key_create(&tkey);
	if (ret != XI_TKEY_RV_OK) {
		log_print(XDLOG, "    - result : failed!!! (ret=%d)\n\n", ret);
		TC_EXIT_ERROR;
	}
	log_print(XDLOG, "    - result : pass. (key=0x%x)\n\n", tkey);

	log_print(XDLOG, "  (%s:%02d) set tkey (&_g_tid1) ##########\n", tcname, t++);
	ret = xi_thread_key_set(tkey, &_g_tid1);
	if (ret != XI_TKEY_RV_OK) {
		log_print(XDLOG, "    - result : failed!!! (ret=%d)\n\n", ret);
		TC_EXIT_ERROR;
	}
	log_print(XDLOG, "    - result : pass. (&_g_tid1=%p)\n\n", &_g_tid1);

	log_print(XDLOG, "  (%s:%02d) get tkey  #################\n", tcname, t++);
	val = xi_thread_key_get(tkey);
	if (val == NULL || val != &_g_tid1) {
		log_print(XDLOG, "    - result : failed!!! (ret=%d)\n\n", ret);
		TC_EXIT_ERROR;
	}
	log_print(XDLOG, "    - result : pass. (val=%p)\n\n", val);

	log_print(XDLOG, "  (%s:%02d) destroy tkey  #############\n", tcname, t++);
	ret = xi_thread_key_destroy(tkey);
	if (ret != XI_TKEY_RV_OK) {
		log_print(XDLOG, "    - result : failed!!! (ret=%d)\n\n", ret);
		TC_EXIT_ERROR;
	}
	log_print(XDLOG, "    - result : pass.\n\n");

	log_print(XDLOG, "  (%s:%02d) cond_signal to tt02  ######\n", tcname, t++);
	ret = xi_thread_cond_signal(&_g_cond);
	if (ret != XI_COND_RV_OK) {
		log_print(XDLOG, "    - result : failed!!! (ret=%d)\n\n", ret);
		TC_EXIT_ERROR;
	}
	log_print(XDLOG, "    - result : pass.\n\n");

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

	xint32 ret = 0;
	xvoid *stackbase = NULL;
	xsize stacksize = 0;

	UNUSED(arg);

	log_print(XDLOG, ">>> TEST_THREAD02 - started (tid=0x%x)\n\n", xi_thread_self());

	log_print(XDLOG, "  (%s:%02d) enter to wait #############\n\n", tcname, t++);
	ret = xi_thread_cond_wait(&_g_cond, &_g_mutex);
	if (ret != XI_COND_RV_OK) {
		log_print(XDLOG, "[TT02] - abnormal exit from cond_wait\n");
		TC_EXIT_ERROR;
	}
	log_print(XDLOG, "    - result : pass. (%s : exit from wait)\n\n", tcname);

	log_print(XDLOG, "  (%s:%02d) get stack info ############\n", tcname, t++);
	stackbase = xi_thread_get_stackbase(xi_thread_self());
	stacksize = xi_thread_get_stacksize(xi_thread_self());
	if (stackbase == NULL) {
		log_print(XDLOG, "    - result : failed!!! (stack_base=%p/stack_size=%d)\n\n", stackbase, stacksize);
		TC_EXIT_ERROR;
	}
	log_print(XDLOG, "    - result : pass. (stack_base=%p/stack_size=%d)\n\n", stackbase, stacksize);

	xi_thread_sleep(1000); // sleep until tt01 running

	log_print(XDLOG, "  (%s:%02d) thread_suspend(tid1) ######\n", tcname, t++);
	ret = xi_thread_suspend(_g_tid1);
	if (ret == XI_THREAD_RV_OK) {
		log_print(XDLOG, "    - result : pass. (ret=%d)\n\n", ret);
	} else if (ret == XI_THREAD_RV_ERR_NOSUP) {
		log_print(XDLOG, "    - result : xi_thread_suspend is not supported\n");
	} else {
		log_print(XDLOG, "    - result : failed!!! (ret=%d)\n\n", ret);
		TC_EXIT_ERROR;
	}

	log_print(XDLOG, "  (%s:%02d) wait confirm suspend ######\n\n", tcname, t++);
	xi_thread_sleep(3000);

	log_print(XDLOG, "  (%s:%02d) thread_resume(tid1) #######\n", tcname, t++);
	ret = xi_thread_resume(_g_tid1);
	if (ret == XI_THREAD_RV_OK) {
		log_print(XDLOG, "    - result : pass. (ret=%d)\n\n", ret);
	} else if (ret == XI_THREAD_RV_ERR_NOSUP) {
		log_print(XDLOG, "    - result : xi_thread_resume is not supported\n");
	} else {
		log_print(XDLOG, "    - result : failed!!! (ret=%d)\n\n", ret);
		TC_EXIT_ERROR;
	}

	log_print(XDLOG, "  (%s:%02d) wait confirm resume ######\n\n", tcname, t++);
	xi_thread_sleep(3000);

	log_print(XDLOG, "  (%s:%02d) leave child threads #######\n\n", tcname, t++);
	_g_trun = FALSE;

	log_print(XDLOG, "<<< TEST_THREAD02 - exited\n\n");
	return NULL;
}

static void tc_info() {
	log_print(XDLOG, "\n\n");
	log_print(XDLOG, "====================================================\n");
	log_print(XDLOG, "                xi_thread.h - basic\n");
	log_print(XDLOG, "----------------------------------------------------\n");
	log_print(XDLOG, " * Functions)\n");
	log_print(XDLOG, "   > thread_key\n");
	log_print(XDLOG, "     - xi_thread_key_create\n");
	log_print(XDLOG, "     - xi_thread_key_get\n");
	log_print(XDLOG, "     - xi_thread_key_set\n");
	log_print(XDLOG, "     - xi_thread_key_destroy\n");
	log_print(XDLOG, "   > thread_mutex\n");
	log_print(XDLOG, "     - xi_thread_mutex_create\n");
	log_print(XDLOG, "     - xi_thread_mutex_lock\n");
	log_print(XDLOG, "     - xi_thread_mutex_trylock\n");
	log_print(XDLOG, "     - xi_thread_mutex_unlock\n");
	log_print(XDLOG, "     - xi_thread_mutex_destroy\n");
	log_print(XDLOG, "   > thread_cond\n");
	log_print(XDLOG, "     - xi_thread_cond_create\n");
	log_print(XDLOG, "     - xi_thread_cond_wait\n");
	log_print(XDLOG, "     - xi_thread_cond_timedwait\n");
	log_print(XDLOG, "     - xi_thread_cond_signal\n");
	log_print(XDLOG, "     - xi_thread_cond_broadcast\n");
	log_print(XDLOG, "     - xi_thread_cond_destroy\n");
	log_print(XDLOG, "   > thread\n");
	log_print(XDLOG, "     - xi_thread_create\n");
	log_print(XDLOG, "     - xi_thread_self\n");
	log_print(XDLOG, "     - xi_thread_set_prior\n");
	log_print(XDLOG, "     - xi_thread_get_prior\n");
	log_print(XDLOG, "     - xi_thread_sleep\n");
	log_print(XDLOG, "     - xi_thread_usleep\n");
	log_print(XDLOG, "     - xi_thread_yield\n");
	log_print(XDLOG, "     - xi_thread_suspend\n");
	log_print(XDLOG, "     - xi_thread_resume\n");
	log_print(XDLOG, "     - xi_thread_stack_info\n");
	log_print(XDLOG, "     - xi_thread_detach\n");
	log_print(XDLOG, "     - xi_thread_join\n");
	log_print(XDLOG, "     - xi_thread_cancel\n");
	log_print(XDLOG, "====================================================\n\n");

}

int tc_xi_thread_basic() {
	xint32 t = 1;
	xchar *tcname = "xi_thread.h";

	xint32 ret;

	xi_thread_t tid;
	xint32 prior = 0;
	xint64 ssec = 0;
	xint64 esec = 0;

	tc_info();

	log_print(XDLOG, "[%s:%02d] main thread tid #############\n", tcname, t++);
	tid = xi_thread_self();
	if (tid == 0xFFFFFFFF || tid == 0x00000000) {
		log_print(XDLOG, "    - result : failed!!! ((tid=0x%x)\n\n", tid);
		return -1;
	}
	log_print(XDLOG, "    - result : pass. (tid=0x%x)\n\n", tid);

	log_print(XDLOG, "[%s:%02d] main thread prior ###########\n", tcname, t++);
	ret = xi_thread_get_prior(tid, &prior);
	if (ret != XI_THREAD_RV_OK) {
		log_print(XDLOG, "    - result : failed!!! (ret=%d)\n\n", ret);
		return -1;
	}
	log_print(XDLOG, "    - result : pass. (priror=%d)\n\n", prior);

	log_print(XDLOG, "[%s:%02d] thread_sleep ################\n", tcname, t++);
	ssec = xi_clock_msec();
	xi_thread_sleep(1000);
	esec = xi_clock_msec();
	ret = (xint32) (esec - ssec);
	if (ret < 750 || ret > 1250) {
		log_print(XDLOG, "    - result : failed!!! (ret=%d)\n\n", ret);
		return -1;
	}
	log_print(XDLOG, "    - result : pass. (wait=%dmsec)\n\n", ret);

	log_print(XDLOG, "[%s:%02d] thread_usleep ###############\n", tcname, t++);
	ssec = xi_clock_ntick();
	xi_thread_usleep(5000);
	esec = xi_clock_ntick();
	ret = (xint32) ((esec - ssec) / 1000);
	if (ret < 4000 || ret > 6000) {
		log_print(XDLOG, "    - result : failed!!! (ret=%d)\n\n", ret);
		return -1;
	}
	log_print(XDLOG, "    - result : pass. (wait=%dusec)\n\n", ret);

	log_print(XDLOG, "[%s:%02d] create control mutex ########\n", tcname, t++);
	ret = xi_thread_mutex_create(&_g_mutex, "TMUTEX");
	if (ret != XI_MUTEX_RV_OK) {
		log_print(XDLOG, "    - result : failed!!! (ret=%d)\n\n", ret);
		return -1;
	}
	log_print(XDLOG, "    - result : pass. (_g_mutex=%d)\n\n", _g_mutex);

	log_print(XDLOG, "[%s:%02d] create control cond #########\n", tcname, t++);
	ret = xi_thread_cond_create(&_g_cond, "TCOND");
	if (ret != XI_COND_RV_OK) {
		log_print(XDLOG, "    - result : failed!!! (ret=%d)\n\n", ret);
		return -1;
	}
	log_print(XDLOG, "    - result : pass. (_g_cond=%d)\n\n", _g_cond);

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
		xi_thread_sleep(1000);
		log_print(XDLOG, "\n >>> main wait loop .... \n");
	}
	log_print(XDLOG, "    - result : main thread end.\n\n");

	log_print(XDLOG, "[%s:%02d] cond_broadcast ##############\n", tcname, t++);
	ret = xi_thread_cond_broadcast(&_g_cond);
	if (ret != XI_COND_RV_OK) {
		log_print(XDLOG, "    - result : failed!!! (ret=%d)\n\n", ret);
		return -1;
	}
	log_print(XDLOG, "    - result : pass.\n\n");

	log_print(XDLOG, "[%s:%02d] cond destroy ################\n", tcname, t++);
	ret = xi_thread_cond_destroy(&_g_cond);
	if (ret != XI_COND_RV_OK) {
		log_print(XDLOG, "    - result : failed!!! (ret=%d)\n\n", ret);
		return -1;
	}
	log_print(XDLOG, "    - result : pass.\n\n");

	log_print(XDLOG, "[%s:%02d] mutex destroy ###############\n", tcname, t++);
	ret = xi_thread_mutex_destroy(&_g_mutex);
	if (ret != XI_MUTEX_RV_OK) {
		log_print(XDLOG, "    - result : failed!!! (ret=%d)\n\n", ret);
		return -1;
	}
	log_print(XDLOG, "    - result : pass.\n\n");

	log_print(XDLOG, "============ DONE [xi_thread.h - basic] ============\n\n");

	return 0;
}
