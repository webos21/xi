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

#include <windows.h>
#include <setjmp.h>

#include "xi/xi_thread.h"

#include "xi/xi_log.h"
#include "xi/xi_mem.h"
#include "xi/xi_string.h"
#include "xi/xi_clock.h"
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
	xint32 prior;
	xchar tname[XCFG_ONAME_MAX];
} xg_thread_dat_t;

#if defined(__MINGW32__) || defined(__MINGW64__)
#    if _M_AMD64
#        define MBARRIER() __asm__ __volatile__ ("mfence" ::: "memory")
#    else // !_M_AMD64
#        define MBARRIER() __asm__ __volatile__ ("lock; addl $0,0(%%esp)" ::: "memory")
#    endif // _M_AMD64
#else
#define MBARRIER() do {_mm_mfence();_ReadWriteBarrier();} while(0)
#endif

// ----------------------------------------------
// Global Variables
// ----------------------------------------------

static CRITICAL_SECTION _g_thr_lock;

static HANDLE _g_thr_cond = NULL;

static DWORD _g_thr_self = 0;

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
_XI_INLINE xg_thread_dat_t *xg_thread_tdat_get(xi_thread_t tid) {
	xg_thread_dat_t *tdat;
	EnterCriticalSection(&_g_thr_lock);
	tdat = (xg_thread_dat_t *) xi_hashtb_get(_g_thr_db, &tid, sizeof(xi_thread_t));
	LeaveCriticalSection(&_g_thr_lock);
	return tdat;
}

static xvoid xg_thrmain_init() {
	xg_thread_dat_t *tdat;

	xvoid *stackbase = NULL;
	xsize stacksize = 0;

	MEMORY_BASIC_INFORMATION meminfo;

	tdat = (xg_thread_dat_t *) xi_mem_calloc(1, sizeof(xg_thread_dat_t));
	if (tdat == NULL) {
		return;
	}

	VirtualQuery(&meminfo, &meminfo, sizeof(meminfo));
	stackbase = (xvoid*) (meminfo.BaseAddress + meminfo.RegionSize);
	stacksize = stackbase - meminfo.AllocationBase;

	//	log_print(XDLOG, "meminfo.AllocationBase = %p\n", meminfo.AllocationBase);
	//	log_print(XDLOG, "meminfo.BaseAddress = %p\n", meminfo.BaseAddress);
	//	log_print(XDLOG, "meminfo.RegionSize = %d\n", meminfo.RegionSize);
	//	log_print(XDLOG, "meminfo.Type = %d\n", meminfo.Type);

	_g_thr_self = TlsAlloc();

	tdat->entry = NULL;
	tdat->args = NULL;
	tdat->stack_size = stacksize;
	xi_strncpy(tdat->tname, "main", (XCFG_ONAME_MAX-1));
	tdat->state = XI_THREAD_STATE_CREATING;
	tdat->blocking = XI_THREAD_SUSBLK_NOBLOCK;
	tdat->prior = 5;
	{
		HANDLE htarget;
		HANDLE hthread;
		HANDLE hproc;

		hproc = GetCurrentProcess();
		hthread = GetCurrentThread();
		DuplicateHandle(hproc, hthread, hproc, &htarget, 0, FALSE,
				DUPLICATE_SAME_ACCESS);
		TlsSetValue(_g_thr_self, htarget);

		tdat->tid = (xi_thread_t) htarget;
	}
	tdat->stack_base = stackbase;
	tdat->stack_top = &tdat;
	tdat->state = XI_THREAD_STATE_RUNNING;
	tdat->blocking = XI_THREAD_SUSBLK_NOBLOCK;

	EnterCriticalSection(&_g_thr_lock);
	xi_hashtb_set(_g_thr_db, &tdat->tid, sizeof(xi_thread_t), tdat);
	LeaveCriticalSection(&_g_thr_lock);

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
	//	log_print(XDLOG, "tdat->prior = %d\n", tdat->prior);
	//	log_print(XDLOG, "=====================================\n");
}

static xuint32 xg_thread_prior_pi2sys(xuint32 prior) {
	switch (prior) {
	case 1:
		return THREAD_PRIORITY_IDLE;
	case 2:
		return THREAD_PRIORITY_LOWEST;
	case 3:
	case 4:
		return THREAD_PRIORITY_BELOW_NORMAL;
	case 5:
		return THREAD_PRIORITY_NORMAL;
	case 6:
	case 7:
		return THREAD_PRIORITY_ABOVE_NORMAL;
	case 8:
	case 9:
		return THREAD_PRIORITY_HIGHEST;
	case 10:
		return THREAD_PRIORITY_TIME_CRITICAL;
	default:
		return 50;
	}
}

static xvoid *xg_thread_starter(xvoid *args) {
	xg_thread_dat_t *tdat = args;
	xvoid *ret;

	// filling the Thread-Data
	tdat->tid = xi_thread_self();
	tdat->stack_base = &tdat;
	tdat->stack_top = &tdat;

	// Lock the world!!
	EnterCriticalSection(&_g_thr_lock);

	// Add the thread to HashTable
	xi_hashtb_set(_g_thr_db, &tdat->tid, sizeof(xi_thread_t), tdat);

	// Wait if all-suspended
	_g_thr_info.threads_waiting_to_start++;
	while (_g_thr_info.all_threads_suspended) {
		WaitForSingleObject(_g_thr_cond, INFINITE);
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
	ReleaseMutex(_g_thr_cond);
	LeaveCriticalSection(&_g_thr_lock);

	// Run the real thread function
	ret = tdat->entry(tdat->args);

	// Remove and destroy info
	EnterCriticalSection(&_g_thr_lock);

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
	//	log_print(XDLOG, "tdat->prior = %d\n", tdat->prior);
	//	log_print(XDLOG, "=====================================\n");

	xi_hashtb_set(_g_thr_db, &tdat->tid, sizeof(xi_thread_t), NULL);
	_g_thr_info.threads_count--;
	xi_mem_free(tdat);
	LeaveCriticalSection(&_g_thr_lock);

	return ret;
}

// ----------------------------------------------
// XI Functions
// ----------------------------------------------

xi_thread_re xi_thread_create(xi_thread_t *tid, const xchar *tname,
		xi_thread_fn func, xvoid *args, xsize stack_size, xint32 prior) {
	HANDLE hnd;
	DWORD thrid; // Useless
	xg_thread_dat_t *tdat = NULL;

	if (_g_thr_db == NULL) {
		_g_thr_db = xi_hashtb_create();
		InitializeCriticalSectionAndSpinCount(&_g_thr_lock, 0x80000400);
		_g_thr_cond = CreateMutex(NULL, FALSE, NULL);
		xg_thrmain_init();
	}

	if (prior < XCFG_THREAD_PRIOR_MIN || prior > XCFG_THREAD_PRIOR_MAX) {
		return XI_THREAD_RV_ERR_PRIOR;
	}

	if (tid == NULL) {
		return XI_THREAD_RV_ERR_ARGS;
	}

	tdat = xi_mem_calloc(1, sizeof(xg_thread_dat_t));
	if (tdat == NULL) {
		return XI_THREAD_RV_ERR_NONEM;
	}
	tdat->entry = func;
	tdat->args = args;
	tdat->stack_size = stack_size;
	xi_strncpy(tdat->tname, tname, (XCFG_ONAME_MAX-1));
	tdat->state = XI_THREAD_STATE_CREATING;
	tdat->blocking = XI_THREAD_SUSBLK_NOBLOCK;
	tdat->prior = prior;

	hnd = CreateThread(NULL, stack_size,
			(LPTHREAD_START_ROUTINE)(xg_thread_starter), tdat, 0, &thrid);
	if (hnd == NULL) {
		return XI_THREAD_RV_ERR_ARGS;
	}

	while (tdat->state != XI_THREAD_STATE_RUNNING) {
		xi_thread_yield();
	}
	(*tid) = tdat->tid;
	//xi_mem_copy(tid, &tdat->tid, sizeof(xi_thread_t));
	//(*tid) = tdat->tid;
	//(*tid) = (xi_thread_t)hnd;


	return XI_THREAD_RV_OK;
}

xvoid xi_thread_yield() {
	SwitchToThread();
}

xi_thread_re xi_thread_sleep(xuint32 msec) {
	Sleep(msec);
	return XI_THREAD_RV_OK;
}

xi_thread_re xi_thread_usleep(xuint32 usec) {
	xint64 bval = usec * 1000;
	xint64 cval = 0;
	xint64 stick = xi_clock_ntick();

	while (bval > cval) {
		cval = xi_clock_ntick() - stick;
	}
	return XI_THREAD_RV_OK;
}

xi_thread_re xi_thread_suspend(xi_thread_t tid) {
	xg_thread_dat_t *tdat;

	//log_print(XDLOG, "Given tid = 0x%x\n", tid);

	tdat = xg_thread_tdat_get(tid);
	if (tdat == NULL) {
		//log_error(XDLOG, "why you are here??? tid = 0x%x\n", tid);
		return XI_THREAD_RV_ERR_ID;
	}

	tdat->suspend = TRUE;
	MBARRIER();

	if (!tdat->blocking) {
		CONTEXT ctx;

		SuspendThread((HANDLE) tdat->tid);

		ZeroMemory(&ctx, sizeof(CONTEXT));
		ctx.ContextFlags = CONTEXT_FULL;
		GetThreadContext((HANDLE) tdat->tid, &ctx);

		tdat->state = XI_THREAD_STATE_SUSPENDED;
#ifdef _M_AMD64
		tdat->stack_top = (xvoid *) ctx.Rbp;
#else // !_M_AMD64
		tdat->stack_top = (xvoid *) ctx.Ebp;
#endif // _M_AMD64
		MBARRIER();
	}

	return XI_THREAD_RV_OK;
}

xi_thread_re xi_thread_suspend_all() {
	xi_hashtb_idx_t *hidx = NULL;
	xg_thread_dat_t *self = xg_thread_tdat_get(xi_thread_self());

	EnterCriticalSection(&_g_thr_lock);

	//	log_print(XDLOG, "[THREAD-INFO] =======================\n");
	//	log_print(XDLOG, "_g_thr_info = %d\n", _g_thr_info.threads_count);
	//	log_print(XDLOG, "_g_thr_info.peak_threads = %d\n", _g_thr_info.peak_threads);
	//	log_print(XDLOG, "_g_thr_info.total_started_threads = %lld\n", _g_thr_info.total_started_threads);
	//	log_print(XDLOG, "_g_thr_info.all_threads_suspended = %d\n", _g_thr_info.all_threads_suspended);
	//	log_print(XDLOG, "_g_thr_info.threads_waiting_to_start = %d\n", _g_thr_info.threads_waiting_to_start);
	//	log_print(XDLOG, "=====================================\n");

	for (hidx = xi_hashtb_first(_g_thr_db); hidx; hidx = xi_hashtb_next(hidx)) {
		xi_thread_t *key = NULL;
		xint32 klen = 0;
		xg_thread_dat_t *tdat = NULL;

		xi_hashtb_this(hidx, (const xvoid **) &key, &klen, (xvoid **) &tdat);

		//log_print(XDLOG, "[SUSPEND_ALL] tdat->tname = %s : ", tdat->tname);

		if (tdat == self) {
			//log_print(XDLOG, "Self\n");
			//	log_print(XDLOG, "[SUSPEND_ALL] self->tname = %s / tdat->tname = %s\n", self->tname, tdat->tname);
			continue;
		}

		tdat->suspend = TRUE;
		MBARRIER();

		if (!tdat->blocking) {
			CONTEXT ctx;
			HANDLE hnd = (HANDLE) tdat->tid;

			//	log_print(XDLOG, "[SUSPEND_ALL] target->tname = %s\n", tdat->tname);
			SuspendThread(hnd);

			ZeroMemory(&ctx, sizeof(CONTEXT));
			ctx.ContextFlags = CONTEXT_FULL;
			GetThreadContext((HANDLE) tdat->tid, &ctx);

			tdat->state = XI_THREAD_STATE_SUSPENDED;
#ifdef _M_AMD64
			tdat->stack_top = (xvoid *) ctx.Rbp;
#else // !_M_AMD64
			tdat->stack_top = (xvoid *) ctx.Ebp;
#endif // _M_AMD64
			MBARRIER();

			//log_print(XDLOG, "Suspended, stack_base=%p, stack_top=%p\n", tdat->stack_base, tdat->stack_top);

			// CONTEXT_SEGMENTS
			//log_print(XDLOG, "[CONTEXT-SEG] ctx.SegGs = 0x%x\n", ctx.SegGs);
			//log_print(XDLOG, "[CONTEXT-SEG] ctx.SegFs = 0x%x\n", ctx.SegFs);
			//log_print(XDLOG, "[CONTEXT-SEG] ctx.SegEs = 0x%x\n", ctx.SegEs);
			//log_print(XDLOG, "[CONTEXT-SEG] ctx.SegDs = 0x%x\n", ctx.SegDs);

			// CONTEXT_CONTROL
			//log_print(XDLOG, "[CONTEXT-CTL] ctx.Ebp = 0x%x\n", ctx.Ebp); // frame offset - fiberData[0x32]
			//log_print(XDLOG, "[CONTEXT-CTL] ctx.Esp = 0x%x\n", ctx.Esp); // stack offset - fiberData[0x36]
			//log_print(XDLOG, "[CONTEXT-CTL] ctx.Eip = 0x%x\n", ctx.Eip); // program counter
			//log_print(XDLOG, "[CONTEXT-CTL] ctx.SegCs = 0x%x\n", ctx.SegCs);
			//log_print(XDLOG, "[CONTEXT-CTL] ctx.EFlags = 0x%x\n", ctx.EFlags);
			//log_print(XDLOG, "[CONTEXT-CTL] ctx.SegSs = 0x%x\n", ctx.SegSs);

			// CONTEXT_INTEGER
			//log_print(XDLOG, "[CONTEXT-INT] ctx.Eax = 0x%x\n", ctx.Eax); // value on (Esp+0)
			//log_print(XDLOG, "[CONTEXT-INT] ctx.Edx = 0x%x\n", ctx.Edx); // value on (Esp+4)
			//log_print(XDLOG, "[CONTEXT-INT] ctx.Ecx = 0x%x\n", ctx.Ecx); // value on (Esp+8)
			//log_print(XDLOG, "[CONTEXT-INT] ctx.Edi = 0x%x\n", ctx.Edi); // fiberData[0x2c]
			//log_print(XDLOG, "[CONTEXT-INT] ctx.Esi = 0x%x\n", ctx.Esi); // fiberData[0x2d]
			//log_print(XDLOG, "[CONTEXT-INT] ctx.Ebx = 0x%x\n", ctx.Ebx); // fiberData[0x2e]
		} else {
			//log_print(XDLOG, "Skipped, stack_base=%p, stack_top=%p\n", tdat->stack_base, tdat->stack_top);
		}
	}

	_g_thr_info.all_threads_suspended = TRUE;

	LeaveCriticalSection(&_g_thr_lock);
	return XI_THREAD_RV_OK;
}

xi_thread_re xi_thread_resume(xi_thread_t tid) {
	xg_thread_dat_t *tdat;

	tdat = xg_thread_tdat_get(tid);
	if (tdat == NULL) {
		return XI_THREAD_RV_ERR_ID;
	}

	tdat->suspend = FALSE;
	MBARRIER();

	if (!tdat->blocking) {
		ResumeThread((HANDLE) tdat->tid);
		tdat->state = XI_THREAD_STATE_RUNNING;
		MBARRIER();
	}

	return XI_THREAD_RV_OK;
}

xi_thread_re xi_thread_resume_all(xi_thread_t tid) {
	xi_hashtb_idx_t *hidx = NULL;
	xg_thread_dat_t *self = xg_thread_tdat_get(xi_thread_self());

	UNUSED(tid);

	EnterCriticalSection(&_g_thr_lock);

	for (hidx = xi_hashtb_first(_g_thr_db); hidx; hidx = xi_hashtb_next(hidx)) {
		xi_thread_t *key = NULL;
		xint32 klen = 0;
		xg_thread_dat_t *tdat = NULL;

		xi_hashtb_this(hidx, (const xvoid **) &key, &klen, (xvoid **) &tdat);

		//log_print(XDLOG, "[RESUME_ALL] tdat->tname = %s  : ", tdat->tname);

		if (tdat == self) {
			//log_print(XDLOG, "Self\n");
			//	log_print(XDLOG, "[RESUME_ALL] self->tname = %s\n", tdat->tname);
			continue;
		}

		tdat->suspend = FALSE;
		MBARRIER();

		if (!tdat->blocking) {
			HANDLE hnd = (HANDLE) tdat->tid;
			//	log_print(XDLOG, "[RESUME_ALL] target->tname = %s\n", tdat->tname);
			ResumeThread(hnd);
			//log_print(XDLOG, "Resumed, stack_base=%p, stack_top=%p\n", tdat->stack_base, tdat->stack_top);
			tdat->state = XI_THREAD_STATE_RUNNING;
			MBARRIER();
		} else {
			//log_print(XDLOG, "Skipped\n");
		}
	}

	_g_thr_info.all_threads_suspended = FALSE;
	if (_g_thr_info.threads_waiting_to_start) {
		ReleaseMutex(_g_thr_cond);
	}

	LeaveCriticalSection(&_g_thr_lock);
	return XI_THREAD_RV_OK;
}

xi_thread_re xi_thread_enable_suspend(xi_thread_t tid) {
	xg_thread_dat_t *tdat;

	HANDLE hnd;
	CONTEXT ctx;

	tdat = xg_thread_tdat_get(tid);
	if (tdat == NULL) {
		return XI_THREAD_RV_ERR_ID;
	}

	hnd = (HANDLE) tdat->tid;

	if (xi_thread_self() != tdat->tid) {
		SuspendThread(hnd);

		ZeroMemory(&ctx, sizeof(CONTEXT));
		ctx.ContextFlags = CONTEXT_FULL;
		GetThreadContext(hnd, &ctx);
#ifdef _M_AMD64
		tdat->stack_top = (xvoid *) ctx.Rbp;
#else // !_M_AMD64
		tdat->stack_top = (xvoid *) ctx.Ebp;
#endif // _M_AMD64

		tdat->blocking = FALSE;
		MBARRIER();

		ResumeThread(hnd);
	} else {
		tdat->stack_top = &tdat;
		tdat->blocking = FALSE;
		MBARRIER();
	}

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

	return XI_THREAD_RV_OK;
}

xi_thread_re xi_thread_disable_suspend(xi_thread_t tid) {
	xg_thread_dat_t *tdat;

	HANDLE hnd;
	CONTEXT ctx;

	tdat = xg_thread_tdat_get(tid);
	if (tdat == NULL) {
		return XI_THREAD_RV_ERR_ID;
	}

	hnd = (HANDLE) tdat->tid;

	if (xi_thread_self() != tdat->tid) {
		SuspendThread(hnd);

		ZeroMemory(&ctx, sizeof(CONTEXT));
		ctx.ContextFlags = CONTEXT_FULL;
		GetThreadContext(hnd, &ctx);
#ifdef _M_AMD64
		tdat->stack_top = (xvoid *) ctx.Rbp;
#else // !_M_AMD64
		tdat->stack_top = (xvoid *) ctx.Ebp;
#endif // _M_AMD64

		tdat->blocking = XI_THREAD_SUSBLK_BLOCKING;
		MBARRIER();

		ResumeThread(hnd);
	} else {
		tdat->stack_top = &tdat;
		tdat->blocking = XI_THREAD_SUSBLK_BLOCKING;
		MBARRIER();
	}

	return XI_THREAD_RV_OK;
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
	HANDLE hthread;
	HANDLE hproc;
	HANDLE htarget;

	if (_g_thr_db == NULL) {
		_g_thr_db = xi_hashtb_create();
		InitializeCriticalSectionAndSpinCount(&_g_thr_lock, 0x80000400);
		_g_thr_cond = CreateMutex(NULL, FALSE, NULL);
		xg_thrmain_init();
	}

	hthread = (HANDLE) TlsGetValue(_g_thr_self);
	if (hthread) {
		//log_print(XDLOG, "it is needed!!!\n");
		return (xi_thread_t) hthread;
	}

	//log_print(XDLOG, "it is not wanted!!!\n");

	hproc = GetCurrentProcess();
	hthread = GetCurrentThread();
	if (!DuplicateHandle(hproc, hthread, hproc, &htarget, 0, FALSE,
			DUPLICATE_SAME_ACCESS)) {
		return 0;
	}
	// log_print(XDLOG, "[SELF] hthread=%p / htarget=%p\n", hthread, htarget);
	TlsSetValue(_g_thr_self, htarget);
	return (xi_thread_t) htarget;
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
	xg_thread_dat_t *tdat;
	xint32 rp;

	if (new_prior < XCFG_THREAD_PRIOR_MIN || new_prior > XCFG_THREAD_PRIOR_MAX) {
		return XI_THREAD_RV_ERR_PRIOR;
	}

	tdat = xg_thread_tdat_get(tid);
	if (tdat == NULL) {
		return XI_THREAD_RV_ERR_ID;
	}

	tdat->prior = new_prior;

	rp = xg_thread_prior_pi2sys(new_prior);
	SetThreadPriority((HANDLE) tid, rp);
	return XI_THREAD_RV_OK;
}

xi_thread_re xi_thread_get_prior(xi_thread_t tid, xint32 *prior) {
	xg_thread_dat_t *tdat;

	tdat = xg_thread_tdat_get(tid);
	if (tdat == NULL) {
		return XI_THREAD_RV_ERR_ID;
	}

	*prior = tdat->prior;
	return XI_THREAD_RV_OK;
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
	EnterCriticalSection(&_g_thr_lock);
}

xvoid xi_thread_list_unlock() {
	LeaveCriticalSection(&_g_thr_lock);
}
