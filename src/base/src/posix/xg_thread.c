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
 * File : xg_thread.c
 */

#define _GNU_SOURCE
#include <pthread.h>

#include <errno.h>
#include <signal.h>
#include <setjmp.h>
#ifdef __APPLE__
#include <sys/time.h>
#endif

#include "xi/xi_thread.h"

#include "xi/xi_log.h"
#include "xi/xi_mem.h"
#include "xi/xi_string.h"
#include "xi/xi_hashtb.h"

// ----------------------------------------------
// Inner Structure
// ----------------------------------------------

typedef struct _xg_threads_info_t {
	xint32 threads_count;
	xint32 peak_threads;
	xint64 total_started_threads;
	xint32 all_threads_suspended;
	xint32 threads_waiting_to_start;
} xg_threads_info_t;

typedef struct _xg_thread_dat_t {
	xi_thread_t tid;
	xint32 state; // xi_thread_state_e
	xint32 suspend; // TRUE/FALSE
	xint32 blocking; // xi_thread_susblk_e
	xvoid *stack_top;
	xvoid *stack_base;
	xsize  stack_size;
	xi_thread_fn entry;
	xvoid *args;
	xchar tname[XCFG_ONAME_MAX];
} xg_thread_dat_t;

// ----------------------------------------------
// Global Variables
// ----------------------------------------------

static pthread_attr_t _g_thr_attr;

static pthread_mutex_t _g_thr_lock = PTHREAD_MUTEX_INITIALIZER;

static pthread_cond_t _g_thr_cond = PTHREAD_COND_INITIALIZER;

static xg_threads_info_t _g_thr_info = {
/* current_threads */1,
/* peak_threads */1,
/* total_started_threads */1,
/* all_threads_suspended */0,
/* threads_waiting_to_start */0, };

static xi_hashtb_t *_g_thr_db = NULL;

// ----------------------------------------------
// Part Internal Functions
// ----------------------------------------------

#if defined(__mips__)
#    define MBARRIER()              \
    __asm__ __volatile__ ("     \
        .set push\n             \
        .set mips2\n            \
        sync\n                  \
        .set  pop\n"            \
    ::: "memory")
#elif defined(__arm__)
#    define MBARRIER() __asm__ __volatile__ ("" ::: "memory")
#else // !__mips__
#    if __x86_64
#        define MBARRIER() __asm__ __volatile__ ("mfence" ::: "memory")
#    else // !__x86_64
#        define MBARRIER() __asm__ __volatile__ ("lock; addl $0,0(%%esp)" ::: "memory")
#    endif // __x86_64
#endif

_XI_INLINE xg_thread_dat_t *xg_thread_tdat_get(xi_thread_t tid) {
	xg_thread_dat_t *tdat;
	pthread_mutex_lock(&_g_thr_lock);
	tdat = xi_hashtb_get(_g_thr_db, &tid, sizeof(xi_thread_t));
	pthread_mutex_unlock(&_g_thr_lock);
	return tdat;
}

static xi_thread_re xg_thread_disable_suspend(xi_thread_t tid, xvoid *stack_top) {
	sigset_t mask;
	xg_thread_dat_t *tdat;

	tdat = xg_thread_tdat_get(tid);
	if (tdat == NULL) {
		return XI_THREAD_RV_ERR_ID;
	}

	tdat->stack_top = stack_top;
	tdat->blocking = XI_THREAD_SUSBLK_BLOCKING;
	MBARRIER();

	sigemptyset(&mask);
	sigaddset(&mask, SIGUSR1);
	pthread_sigmask(SIG_BLOCK, &mask, NULL);

	return XI_THREAD_RV_OK;
}

static xvoid xg_sighnd_suspend_loop(xg_thread_dat_t *tdat) {
	sigjmp_buf jmp_env;
	sigset_t suspend_mask;

	xint32 old_state;

	if (tdat == NULL) {
		return;
	}

	old_state = tdat->state;

	sigsetjmp(jmp_env, FALSE);
	tdat->stack_top = &jmp_env;

	//log_print(XDLOG, "[SUSPEND_LOOP] %s - old = %d\n", tdat->tname, old_state);
	tdat->state = XI_THREAD_STATE_SUSPENDED;
	MBARRIER();

	sigfillset(&suspend_mask);
	sigdelset(&suspend_mask, SIGUSR1);
	sigdelset(&suspend_mask, SIGTERM);

	while (tdat->suspend && !tdat->blocking) {
		//	log_print(XDLOG, "[SUSPEND_LOOP] %s - cur = %d\n", tdat->tname, tdat->state);
		sigsuspend(&suspend_mask);
	}

	tdat->state = old_state;
	MBARRIER();
	//log_print(XDLOG, "[SUSPEND_LOOP] %s - res = %d\n", tdat->tname, tdat->state);
}

static xvoid xg_sighnd_suspend(xint32 sig) {
	xg_thread_dat_t *tdat = xg_thread_tdat_get(xi_thread_self());
	UNUSED(sig);
	xg_sighnd_suspend_loop(tdat);
}

static xvoid xg_sighnd_init() {
	struct sigaction sigusr1;
	//sigset_t sig_procmask;//XXX : no need to catch the process signal

	sigusr1.sa_flags = 0;
	sigusr1.sa_handler = xg_sighnd_suspend;
	sigemptyset(&sigusr1.sa_mask);
	sigaction(SIGUSR1, &sigusr1, NULL);

	//XXX : no need to catch the process signal
	//sigemptyset(&sig_procmask);
	//sigaddset(&sig_procmask, SIGQUIT);
	//sigaddset(&sig_procmask, SIGINT);
	//sigaddset(&sig_procmask, SIGPIPE);
	//sigprocmask(SIG_BLOCK, &sig_procmask, NULL);

	return;
}

static xvoid xg_thrattr_init() {
	pthread_attr_init(&_g_thr_attr);
	pthread_attr_setschedpolicy(&_g_thr_attr, SCHED_FIFO);
}

static xvoid xg_thrmain_init() {
	xg_thread_dat_t *tdat;
	size_t stacksize;

	tdat = xi_mem_calloc(1, sizeof(xg_thread_dat_t));
	if (tdat == NULL) {
		return;
	}
	pthread_attr_getstacksize(&_g_thr_attr, &stacksize);
	tdat->entry = NULL;
	tdat->args = NULL;
	tdat->stack_size = stacksize;
	xi_strncpy(tdat->tname, "main", (XCFG_ONAME_MAX - 1));
	tdat->tid = (xi_thread_t) pthread_self();
	tdat->stack_base = &tdat;
	tdat->stack_top = &tdat;
	tdat->state = XI_THREAD_STATE_RUNNING;
	tdat->blocking = XI_THREAD_SUSBLK_NOBLOCK;

	pthread_mutex_lock(&_g_thr_lock);
	xi_hashtb_set(_g_thr_db, &tdat->tid, sizeof(xi_thread_t), tdat);
	pthread_mutex_unlock(&_g_thr_lock);

	//	log_print(XDLOG, "[MAIN_THREAD] =======================\n");
	//	log_print(XDLOG, "tdat->entry = %p\n", tdat->entry);
	//	log_print(XDLOG, "tdat->args = %p\n", tdat->args);
	//	log_print(XDLOG, "tdat->stack_size = %d\n", tdat->stack_size);
	//	log_print(XDLOG, "tdat->tname = %s\n", tdat->tname);
	//	log_print(XDLOG, "tdat->tid = 0x%x\n", tdat->tid);
	//	log_print(XDLOG, "tdat->stack_base = %p\n", tdat->stack_base);
	//	log_print(XDLOG, "tdat->stack_top = %p\n", tdat->stack_top);
	//	log_print(XDLOG, "tdat->state = %d\n", tdat->state);
	//	log_print(XDLOG, "tdat->blocking = %d\n", tdat->blocking);
	//	log_print(XDLOG, "=====================================\n");
}

static xint32 xg_thread_prior_pi2sys(xint32 prior) {
	switch (prior) {
	case 1:
		return 10;
	case 2:
		return 20;
	case 3:
		return 30;
	case 4:
		return 40;
	case 5:
		return 50;
	case 6:
		return 60;
	case 7:
		return 70;
	case 8:
		return 80;
	case 9:
		return 90;
	case 10:
		return 99;
	default:
		return 50;
	}
}

static xint32 xg_thread_prior_sys2pi(xint32 prior) {
	switch (prior) {
	case 10:
		return 1;
	case 20:
		return 2;
	case 30:
		return 3;
	case 40:
		return 4;
	case 50:
		return 5;
	case 60:
		return 6;
	case 70:
		return 7;
	case 80:
		return 8;
	case 90:
		return 9;
	case 99:
		return 10;
	default:
		return 5;
	}
}

static xvoid *xg_thread_starter(xvoid *args) {
	xg_thread_dat_t *tdat = args;

	// filling the Thread-Data
	tdat->tid = xi_thread_self();
	tdat->stack_base = &tdat;
	tdat->stack_top = &tdat;

	// Lock the world!!
	pthread_mutex_lock(&_g_thr_lock);

	// Add the thread to HashTable
	xi_hashtb_set(_g_thr_db, &tdat->tid, sizeof(xi_thread_t), tdat);

	// Wait if all-suspended
	_g_thr_info.threads_waiting_to_start++;
	while (_g_thr_info.all_threads_suspended) {
		pthread_cond_wait(&_g_thr_cond, &_g_thr_lock);
	}
	_g_thr_info.threads_waiting_to_start--;

	// Increase the threads counter
	if (++_g_thr_info.threads_count > _g_thr_info.peak_threads) {
		_g_thr_info.peak_threads = _g_thr_info.threads_count;
	}
	_g_thr_info.total_started_threads++;

	// Update the thread state
	tdat->state = XI_THREAD_STATE_RUNNING;

	// Unlock the world!!
	pthread_cond_broadcast(&_g_thr_cond);
	pthread_mutex_unlock(&_g_thr_lock);

	// Run the real thread function
	tdat->entry(tdat->args);

	// Remove and destroy info
	pthread_mutex_lock(&_g_thr_lock);

	//	log_print(XDLOG, "[THREAD-OUT] =======================\n");
	//	log_print(XDLOG, "tdat->entry = %p\n", tdat->entry);
	//	log_print(XDLOG, "tdat->args = %p\n", tdat->args);
	//	log_print(XDLOG, "tdat->stack_size = %d\n", tdat->stack_size);
	//	log_print(XDLOG, "tdat->tname = %s\n", tdat->tname);
	//	log_print(XDLOG, "tdat->tid = 0x%x\n", tdat->tid);
	//	log_print(XDLOG, "tdat->stack_base = %p\n", tdat->stack_base);
	//	log_print(XDLOG, "tdat->stack_top = %p\n", tdat->stack_top);
	//	log_print(XDLOG, "tdat->state = %d\n", tdat->state);
	//	log_print(XDLOG, "tdat->blocking = %d\n", tdat->blocking);
	//	log_print(XDLOG, "=====================================\n");

	xi_hashtb_set(_g_thr_db, &tdat->tid, sizeof(xi_thread_t), NULL);
	_g_thr_info.threads_count--;
	xi_mem_free(tdat);
	pthread_mutex_unlock(&_g_thr_lock);

	return NULL;
}

// ----------------------------------------------
// XI Functions
// ----------------------------------------------

xi_thread_re xi_thread_create(xi_thread_t *tid, const xchar *tname,
		xi_thread_fn func, xvoid *args, xsize stack_size, xint32 prior) {
	xint32 ret;

	xg_thread_dat_t *tdat = NULL;
	pthread_t *ptid = (pthread_t *) tid;
#ifndef __arm__
	struct sched_param sarg;
#endif

	if (_g_thr_db == NULL) {
		_g_thr_db = xi_hashtb_create();
		xg_sighnd_init();
		xg_thrattr_init();
		xg_thrmain_init();
	}

	if (prior < XCFG_THREAD_PRIOR_MIN || prior > XCFG_THREAD_PRIOR_MAX) {
		return XI_THREAD_RV_ERR_PRIOR;
	}

	ret = pthread_attr_setstacksize(&_g_thr_attr, stack_size);
	if (ret == EINVAL) {
		return XI_THREAD_RV_ERR_STACK;
	}

	// Add for avoiding ENOMEM in pthread_create
	ret = pthread_attr_setdetachstate(&_g_thr_attr, PTHREAD_CREATE_DETACHED);
	if (ret == EINVAL) {
		return XI_THREAD_RV_ERR_STACK;
	}

#ifndef __arm__
	sarg.sched_priority = xg_thread_prior_pi2sys(prior);

	ret = pthread_attr_setschedparam(&_g_thr_attr, &sarg);
	switch (ret) {
	case 0:
		break;
	default:
		return XI_THREAD_RV_ERR_PRIOR;
	}
#endif

	tdat = xi_mem_calloc(1, sizeof(xg_thread_dat_t));
	if (tdat == NULL) {
		return XI_THREAD_RV_ERR_NONEM;
	}
	tdat->entry = func;
	tdat->args = args;
	tdat->stack_size = stack_size;
	xi_strncpy(tdat->tname, tname, (XCFG_ONAME_MAX - 1));
	tdat->state = XI_THREAD_STATE_CREATING;
	tdat->blocking = XI_THREAD_SUSBLK_NOBLOCK;

	ret = pthread_create(ptid, &_g_thr_attr, xg_thread_starter, tdat);
	switch (ret) {
	case 0:
		break;
	case EAGAIN:
		return XI_THREAD_RV_ERR_COUNT;
	case EINVAL:
		return XI_THREAD_RV_ERR_STACK;
	case EPERM:
		return XI_THREAD_RV_ERR_PERM;
	default:
		log_error(XDLOG, "[pthread_create] error = %d\n", ret);
		return XI_THREAD_RV_ERR_ARGS;
	}

	return XI_THREAD_RV_OK;
}

xvoid xi_thread_yield() {
	sched_yield();
}

xi_thread_re xi_thread_sleep(xuint32 msec) {
	struct timespec req = { 0, 0 };
	struct timespec rem = { 0, 0 };
	xint32 ret;

	req.tv_sec = ((xint32)msec / 1000);
	req.tv_nsec = ((xint32)msec % 1000) * 1000000;

	ret = nanosleep(&req, &rem);
	if (ret != 0) {
		switch (errno) {
		case EINTR:
			return XI_THREAD_RV_ERR_INTR;
		case EINVAL:
		default:
			return XI_THREAD_RV_ERR_ARGS;
		}
	}

	return XI_THREAD_RV_OK;
}

xi_thread_re xi_thread_usleep(xuint32 usec) {
	struct timespec req = { 0, 0 };
	struct timespec rem = { 0, 0 };
	xint32 ret;

	if (usec <= 10 || usec >= 10000) {
		return XI_THREAD_RV_ERR_ARGS;
	}

	req.tv_sec = ((xint32)usec / 1000000);
	req.tv_nsec = ((xint32)usec * 1000);

	ret = nanosleep(&req, &rem);
	if (ret != 0) {
		switch (errno) {
		case EINTR:
			return XI_THREAD_RV_ERR_INTR;
		case EINVAL:
		default:
			return XI_THREAD_RV_ERR_ARGS;
		}
	}

	return XI_THREAD_RV_OK;
}

xi_thread_re xi_thread_suspend(xi_thread_t tid) {
	xint32 ret;
	xg_thread_dat_t *tdat;

	tdat = xg_thread_tdat_get(tid);
	if (tdat == NULL) {
		return XI_THREAD_RV_ERR_ID;
	}

	tdat->suspend = TRUE;
	MBARRIER();

	if (!tdat->blocking) {
		ret = pthread_kill((pthread_t) tid, SIGUSR1);
		if (ret != 0) {
			switch (ret) {
			case ESRCH:
				return XI_THREAD_RV_ERR_ID;
			case EINVAL:
			default:
				return XI_THREAD_RV_ERR_ARGS;
			}
		}
		while (tdat->blocking != XI_THREAD_SUSBLK_BLOCKING && tdat->state
				!= XI_THREAD_STATE_SUSPENDED) {
			xi_thread_yield();
		}
		return XI_THREAD_RV_OK;
	} else {
		return XI_THREAD_RV_OK;
	}
}

xi_thread_re xi_thread_suspend_all() {
	xi_hashtb_idx_t *hidx = NULL;
	xg_thread_dat_t *self = xg_thread_tdat_get(xi_thread_self());

	pthread_mutex_lock(&_g_thr_lock);

	//	log_print(XDLOG, "[THREAD-INFO] =======================\n");
	//	log_print(XDLOG, "_g_thr_info = %d\n", _g_thr_info.threads_count);
	//	log_print(XDLOG, "_g_thr_info.peak_threads = %d\n", _g_thr_info.peak_threads);
	//	log_print(XDLOG, "_g_thr_info.total_started_threads = %lld\n", _g_thr_info.total_started_threads);
	//	log_print(XDLOG, "_g_thr_info.all_threads_suspended = %d\n", _g_thr_info.all_threads_suspended);
	//	log_print(XDLOG, "_g_thr_info.threads_waiting_to_start = %d\n", _g_thr_info.threads_waiting_to_start);
	//	log_print(XDLOG, "=====================================\n");
	//	log_print(XDLOG, "[SUSPEND_ALL] self = %p\n", self);

	for (hidx = xi_hashtb_first(_g_thr_db); hidx; hidx = xi_hashtb_next(hidx)) {
		xint32 klen = 0;
		xvoid *vkey = NULL;
		xvoid *vdat = NULL;
		xg_thread_dat_t *tdat = NULL;

		xi_hashtb_this(hidx, (const void**)&vkey, &klen, &vdat);
		tdat = vdat;

		if (tdat == self) {
			//	log_print(XDLOG, "[SUSPEND_ALL] self->tname = %s / tdat->tname = %s\n", self->tname, tdat->tname);
			continue;
		}

		tdat->suspend = TRUE;
		MBARRIER();

		if (!tdat->blocking) {
			//	log_print(XDLOG, "[SUSPEND_ALL] target->tname = %s\n", tdat->tname);
			pthread_kill((pthread_t)tdat->tid, SIGUSR1);
		}
	}

	// TODO : check this
	//	for (hidx = xi_hashtb_first(_g_thr_db); hidx; hidx = xi_hashtb_next(hidx)) {
	//		xg_thread_dat_t *tdat = NULL;
	//		xi_hashtb_this(hidx, NULL, NULL, (xvoid **) &tdat);
	//		if (tdat == self) {
	//			log_print(XDLOG, "[SUSPEND_ALL] SKIP : tdat->tname = %s\n", tdat->tname);
	//			continue;
	//		}
	//		while (tdat->blocking != XI_THREAD_SUSBLK_BLOCKING && tdat->state
	//				!= XI_THREAD_STATE_SUSPENDED) {
	//			log_print(XDLOG, "[SUSPEND_ALL] Wait for state --> tdat->tname = %s, tdat->blocking = %d, tdat->state = %d\n",
	//					tdat->tname, tdat->blocking, tdat->state);
	//			xi_thread_yield();
	//		}
	//	}

	_g_thr_info.all_threads_suspended = TRUE;

	pthread_mutex_unlock(&_g_thr_lock);
	return XI_THREAD_RV_OK;
}

xi_thread_re xi_thread_resume(xi_thread_t tid) {
	xint32 ret;
	xg_thread_dat_t *tdat;

	tdat = xg_thread_tdat_get(tid);
	if (tdat == NULL) {
		return XI_THREAD_RV_ERR_ID;
	}

	tdat->suspend = FALSE;
	MBARRIER();

	if (!tdat->blocking) {
		ret = pthread_kill((pthread_t) tid, SIGUSR1);
		if (ret != 0) {
			switch (ret) {
			case ESRCH:
				return XI_THREAD_RV_ERR_ID;
			case EINVAL:
			default:
				return XI_THREAD_RV_ERR_ARGS;
			}
		}
		while (tdat->state == XI_THREAD_STATE_SUSPENDED) {
			xi_thread_yield();
		}
		return XI_THREAD_RV_OK;
	} else {
		return XI_THREAD_RV_OK;
	}
}

xi_thread_re xi_thread_resume_all() {
	xi_hashtb_idx_t *hidx = NULL;
	xg_thread_dat_t *self = xg_thread_tdat_get(xi_thread_self());

	pthread_mutex_lock(&_g_thr_lock);

	for (hidx = xi_hashtb_first(_g_thr_db); hidx; hidx = xi_hashtb_next(hidx)) {
		xint32 klen = 0;
		xvoid *vkey = NULL;
		xvoid *vdat = NULL;
		xg_thread_dat_t *tdat = NULL;

		xi_hashtb_this(hidx, (const xvoid **)&vkey, &klen, &vdat);
		tdat = vdat;

		if (tdat == self) {
			//	log_print(XDLOG, "[RESUME_ALL] self->tname = %s\n", tdat->tname);
			continue;
		}

		tdat->suspend = FALSE;
		MBARRIER();

		if (!tdat->blocking) {
			//	log_print(XDLOG, "[RESUME_ALL] target->tname = %s\n", tdat->tname);
			pthread_kill((pthread_t)tdat->tid, SIGUSR1);
		}
	}

	// TODO : check this
	//	for (hidx = xi_hashtb_first(_g_thr_db); hidx; hidx = xi_hashtb_next(hidx)) {
	//		xg_thread_dat_t *tdat = NULL;
	//		xi_hashtb_this(hidx, NULL, NULL, (xvoid **) &tdat);
	//		while (tdat->state == XI_THREAD_STATE_SUSPENDED) {
	//			xi_thread_yield();
	//		}
	//	}

	_g_thr_info.all_threads_suspended = FALSE;
	if (_g_thr_info.threads_waiting_to_start) {
		pthread_cond_broadcast(&_g_thr_cond);
	}

	pthread_mutex_unlock(&_g_thr_lock);
	return XI_THREAD_RV_OK;
}

xi_thread_re xi_thread_enable_suspend(xi_thread_t tid) {
	xg_thread_dat_t *tdat;
	sigset_t mask;

	tdat = xg_thread_tdat_get(tid);
	if (tdat == NULL) {
		return XI_THREAD_RV_ERR_ID;
	}

	tdat->blocking = FALSE;
	MBARRIER();

	if (tdat->suspend) {
		xg_sighnd_suspend_loop(tdat);
	}

	sigemptyset(&mask);
	sigaddset(&mask, SIGUSR1);
	pthread_sigmask(SIG_UNBLOCK, &mask, NULL);

	return XI_THREAD_RV_OK;
}

xi_thread_re xi_thread_enable_suspend_fast(xi_thread_t tid) {
	xg_thread_dat_t *tdat;

	tdat = xg_thread_tdat_get(tid);
	if (tdat == NULL) {
		return XI_THREAD_RV_ERR_ID;
	}

	tdat->blocking = FALSE;
	MBARRIER();

	if (tdat->suspend) {
		sigset_t mask;
		sigemptyset(&mask);
		sigaddset(&mask, SIGUSR1);
		pthread_sigmask(SIG_BLOCK, &mask, NULL);

		xg_sighnd_suspend_loop(tdat);

		pthread_sigmask(SIG_UNBLOCK, &mask, NULL);
	}

	return XI_THREAD_RV_OK;
}

xi_thread_re xi_thread_disable_suspend(xi_thread_t tid) {
	sigjmp_buf env;
	sigsetjmp(env, FALSE);
	return xg_thread_disable_suspend(tid, (void*) &env);
}

xi_thread_re xi_thread_disable_suspend_fast(xi_thread_t tid) {
	xg_thread_dat_t *tdat;

	tdat = xg_thread_tdat_get(tid);
	if (tdat == NULL) {
		return XI_THREAD_RV_ERR_ID;
	}

	tdat->blocking = XI_THREAD_SUSBLK_CRITICAL;
	MBARRIER();

	return XI_THREAD_RV_OK;
}

xi_thread_t xi_thread_self() {
	if (_g_thr_db == NULL) {
		_g_thr_db = xi_hashtb_create();
		xg_sighnd_init();
		xg_thrattr_init();
		xg_thrmain_init();
	}
	return (xi_thread_t) pthread_self();
}

xi_thread_re xi_thread_set_name(xi_thread_t tid, const xchar *tname) {
	xg_thread_dat_t *tdat;

	tdat = xg_thread_tdat_get(tid);
	if (tdat == NULL) {
		return XI_THREAD_RV_ERR_ID;
	}

	xi_mem_set(tdat->tname, 0, XCFG_ONAME_MAX);
	xi_strncpy(tdat->tname, tname, (XCFG_ONAME_MAX - 1));

	return XI_THREAD_RV_OK;
}

xi_thread_re xi_thread_set_prior(xi_thread_t tid, xint32 new_prior) {
#ifndef XI_BUILD_smp8654
	struct sched_param sarg;
	xint32 policy = 0;
	xint32 ret;

	if (new_prior < XCFG_THREAD_PRIOR_MIN || new_prior > XCFG_THREAD_PRIOR_MAX) {
		return XI_THREAD_RV_ERR_PRIOR;
	}

	ret = pthread_getschedparam((pthread_t) tid, &policy, &sarg);
	switch (ret) {
	case 0:
		break;
	case ESRCH:
	default:
		return XI_THREAD_RV_ERR_ID;
	}

	sarg.sched_priority = xg_thread_prior_pi2sys(new_prior);

	ret = pthread_setschedparam((pthread_t) tid, policy, &sarg);
	switch (ret) {
	case 0: {
		return XI_THREAD_RV_OK;
	}
	case ESRCH:
		return XI_THREAD_RV_ERR_ID;
	case EPERM:
		return XI_THREAD_RV_ERR_PERM;
	case EINVAL:
		return XI_THREAD_RV_ERR_ARGS;
	case ENOTSUP:
	default:
		return XI_THREAD_RV_ERR_NOSUP;
	}
#else
	if (_g_thr_db == NULL) {
		_g_thr_db = xi_hashtb_create();
		xg_sighnd_init();
		xg_thrattr_init();
		xg_thrmain_init();
	}
	return XI_THREAD_RV_OK;
#endif
}

xi_thread_re xi_thread_get_prior(xi_thread_t tid, xint32 *prior) {
#ifndef XI_BUILD_smp8654
	struct sched_param sarg;
	xint32 policy = 0;
	xint32 ret;

	ret = pthread_getschedparam((pthread_t) tid, &policy, &sarg);
	switch (ret) {
	case 0:
		*prior = xg_thread_prior_sys2pi(sarg.sched_priority);
		return XI_THREAD_RV_OK;
	case ESRCH:
	default:
		return XI_THREAD_RV_ERR_ID;
	}
#else
	if (_g_thr_db == NULL) {
		_g_thr_db = xi_hashtb_create();
		xg_sighnd_init();
		xg_thrattr_init();
		xg_thrmain_init();
	}

	*prior = XCFG_THREAD_PRIOR_NORM;
	return XI_THREAD_RV_OK;
#endif
}

xvoid *xi_thread_get_stackbase(xi_thread_t tid) {
	xg_thread_dat_t *tdat;

	tdat = xg_thread_tdat_get(tid);
	if (tdat == NULL) {
		return NULL;
	}

	return tdat->stack_base;
}

xvoid *xi_thread_get_stacktop(xi_thread_t tid) {
	xg_thread_dat_t *tdat;

	tdat = xg_thread_tdat_get(tid);
	if (tdat == NULL) {
		return NULL;
	}

	return tdat->stack_top;
}

xsize xi_thread_get_stacksize(xi_thread_t tid) {
	xg_thread_dat_t *tdat;

	tdat = xg_thread_tdat_get(tid);
	if (tdat == NULL) {
		return 0;
	}

	return tdat->stack_size;
}

xint32 xi_thread_get_state(xi_thread_t tid) {
	xg_thread_dat_t *tdat;

	tdat = xg_thread_tdat_get(tid);
	if (tdat == NULL) {
		return 0;
	}

	return tdat->state;
}

xint32 xi_thread_get_threads_count() {
	return _g_thr_info.threads_count;
}

xint32 xi_thread_get_peak_count() {
	return _g_thr_info.peak_threads;
}

xint64 xi_thread_get_total_starts() {
	return _g_thr_info.total_started_threads;
}

xint32 xi_thread_is_suspendable(xi_thread_t tid) {
	xg_thread_dat_t *tdat;

	tdat = xg_thread_tdat_get(tid);
	if (tdat == NULL) {
		return 0;
	}

	return tdat->blocking;
}

xvoid *xi_thread_list() {
	return _g_thr_db;
}

xvoid xi_thread_list_lock() {
	pthread_mutex_lock(&_g_thr_lock);
}

xvoid xi_thread_list_unlock() {
	pthread_mutex_unlock(&_g_thr_lock);
}
