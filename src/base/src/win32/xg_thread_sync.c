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
 * File : xg_thread_sync.c
 */

#include <windows.h>

#include "xi/xi_thread.h"

#include "xi/xi_mem.h"
#include "xi/xi_log.h"
#include "xi/xi_string.h"

// ----------------------------------------------
// Inner Structure
// ----------------------------------------------

typedef struct _st_thread_mutex {
	xint32 used;
	xchar oname[XCFG_ONAME_MAX];
	HANDLE lock;
//CRITICAL_SECTION  lock;
} xg_thread_mutex_t;

typedef struct _st_thread_cond {
	xint32 used;
	xchar oname[XCFG_ONAME_MAX];
	HANDLE cond;
	CRITICAL_SECTION csec;
	xuint32 nwait;
	xuint32 nwake;
	xuint32 generation;
} xg_thread_cond_t;

typedef struct _st_thread_mutex_pool {
	xint32 init;
	xint32 counter;
	xuintptr last;
	xg_thread_mutex_t locks[XCFG_MUTEX_MAX];
} xg_thr_locks_t;

typedef struct _st_thread_cond_pool {
	xint32 init;
	xint32 counter;
	xuintptr last;
	xg_thread_cond_t conds[XCFG_COND_MAX];
} xg_thr_conds_t;

// ----------------------------------------------
// Global Variables
// ----------------------------------------------

static xg_thr_locks_t _g_thr_locks;
static xg_thr_conds_t _g_thr_conds;

// ----------------------------------------------
// Part Internal Functions
// ----------------------------------------------

static xuintptr xg_tmutex_alloc() {
	xuintptr i;

	if (!_g_thr_locks.init) {
		xi_mem_set(&_g_thr_locks, 0, sizeof(_g_thr_locks));
		_g_thr_locks.init = 1;
	}

	for (i = _g_thr_locks.last; i < XCFG_MUTEX_MAX; i++) {
		if (!_g_thr_locks.locks[i].used) {
			_g_thr_locks.locks[i].used = 1;
			_g_thr_locks.last = i;
			_g_thr_locks.counter++;
			//log_trace(XDLOG, "[1st] idx=%d / total=%d\n", i, _g_thr_locks.counter);
			return i;
		}
	}

	if (_g_thr_locks.counter < XCFG_MUTEX_MAX) {
		_g_thr_locks.last = 0;
	}

	for (i = _g_thr_locks.last; i < XCFG_MUTEX_MAX; i++) {
		if (!_g_thr_locks.locks[i].used) {
			_g_thr_locks.locks[i].used = 1;
			_g_thr_locks.last = i;
			_g_thr_locks.counter++;
			//log_trace(XDLOG, "[2nd] idx=%d / total=%d\n", i, _g_thr_locks.counter);
			return i;
		}
	}

	log_error(XDLOG, "Cannot find free mutex-element!!\n");
	return -1;
}

static xuintptr xg_tmutex_free(xuintptr idx) {
	if (_g_thr_locks.locks[idx].used) {
		_g_thr_locks.counter--;
		_g_thr_locks.locks[idx].used = 0;
		xi_mem_set(_g_thr_locks.locks[idx].oname, 0, XCFG_ONAME_MAX);
	}
	return idx;
}

static xuintptr xg_tcond_alloc() {
	xuintptr i;

	if (!_g_thr_locks.init) {
		xi_mem_set(&_g_thr_conds, 0, sizeof(_g_thr_conds));
		_g_thr_conds.init = 1;
	}

	for (i = _g_thr_conds.last; i < XCFG_COND_MAX; i++) {
		if (!_g_thr_conds.conds[i].used) {
			_g_thr_conds.conds[i].used = 1;
			_g_thr_conds.last = i;
			_g_thr_conds.counter++;
			//log_trace(XDLOG, "[1st] idx=%d / total=%d\n", i, _g_thr_conds.counter);
			return i;
		}
	}

	if (_g_thr_conds.counter < XCFG_COND_MAX) {
		_g_thr_conds.last = 0;
	}

	for (i = _g_thr_conds.last; i < XCFG_COND_MAX; i++) {
		if (!_g_thr_conds.conds[i].used) {
			_g_thr_conds.conds[i].used = 1;
			_g_thr_conds.last = i;
			_g_thr_conds.counter++;
			//log_trace(XDLOG, "[2nd] idx=%d / total=%d\n", i, _g_thr_conds.counter);
			return i;
		}
	}

	log_error(XDLOG, "Cannot find free cond-element!!\n");
	return -1;
}

static xuintptr xg_tcond_free(xuintptr idx) {
	if (_g_thr_conds.conds[idx].used) {
		_g_thr_conds.counter--;
		_g_thr_conds.conds[idx].used = 0;
		_g_thr_conds.conds[idx].nwait = 0;
		_g_thr_conds.conds[idx].nwake = 0;
		_g_thr_conds.conds[idx].generation = 0;
		xi_mem_set(_g_thr_conds.conds[idx].oname, 0, XCFG_ONAME_MAX);
	}
	return idx;
}

// ----------------------------------------------
// XI Functions
// ----------------------------------------------

//------------
// Mutex
//------------

xi_thread_mutex_re xi_thread_mutex_create(xi_thread_mutex_t *lock, xchar *mname) {
	xuintptr idx;
	xg_thread_mutex_t *flock;

	idx = xg_tmutex_alloc();
	if ((xintptr)idx < 0) {
		log_error(XDLOG, "Cannot create mutex : [%s]!!! (total=%d)\n", mname, _g_thr_locks.counter);
		(*lock) = -1;
		return XI_MUTEX_RV_ERR_NOMORE;
	}
	flock = &(_g_thr_locks.locks[idx]);

	flock->lock = CreateMutex(NULL, FALSE, NULL);
	if (flock->lock == NULL) {
		return XI_MUTEX_RV_ERR_ARGS;
	}
	//InitializeCriticalSection(&flock->lock);

	(*lock) = (xi_thread_mutex_t) (idx);
	xi_strncpy(flock->oname, mname, (XCFG_ONAME_MAX - 1));

	return XI_MUTEX_RV_OK;
}

xi_thread_mutex_re xi_thread_mutex_lock(xi_thread_mutex_t *lock) {
	DWORD ret;
	xuintptr idx = (*lock);
	xg_thread_mutex_t *flock = NULL;

	if (idx > XCFG_MUTEX_MAX) {
		return XI_MUTEX_RV_ERR_ARGS;
	}

	flock = &(_g_thr_locks.locks[idx]);

	ret = WaitForSingleObject(flock->lock, INFINITE);
	if ((ret != WAIT_OBJECT_0) && (ret != WAIT_ABANDONED)) {
		return (ret == WAIT_TIMEOUT) ? XI_MUTEX_RV_ERR_BUSY
				: XI_MUTEX_RV_ERR_ARGS;
	}

	//EnterCriticalSection(&flock->lock);

	return XI_MUTEX_RV_OK;
}

xi_thread_mutex_re xi_thread_mutex_trylock(xi_thread_mutex_t *lock) {
	DWORD ret;
	xuintptr idx = (*lock);
	xg_thread_mutex_t *flock = NULL;

	if (idx > XCFG_MUTEX_MAX) {
		return XI_MUTEX_RV_ERR_ARGS;
	}

	flock = &(_g_thr_locks.locks[idx]);

	ret = WaitForSingleObject(flock->lock, INFINITE);
	if ((ret != WAIT_OBJECT_0) && (ret != WAIT_ABANDONED)) {
		return (ret == WAIT_TIMEOUT) ? XI_MUTEX_RV_ERR_BUSY
				: XI_MUTEX_RV_ERR_ARGS;
	} else {
		return XI_MUTEX_RV_OK;
	}

	//if (TryEnterCriticalSection(&flock->lock)) {
	//	return XI_MUTEX_RV_OK;
	//} else {
	//	return XI_MUTEX_RV_ERR_BUSY;
	//}
}

xi_thread_mutex_re xi_thread_mutex_unlock(xi_thread_mutex_t *lock) {
	xbool ret;
	xuintptr idx = (*lock);
	xg_thread_mutex_t *flock = NULL;

	if (idx > XCFG_MUTEX_MAX) {
		return XI_MUTEX_RV_ERR_ARGS;
	}

	flock = &(_g_thr_locks.locks[idx]);

	ret = ReleaseMutex(flock->lock);
	if (!ret) {
		return XI_MUTEX_RV_ERR_ARGS;
	}

	//LeaveCriticalSection(&flock->lock);

	return XI_MUTEX_RV_OK;
}

xi_thread_mutex_re xi_thread_mutex_destroy(xi_thread_mutex_t *lock) {
	xbool ret;
	xuintptr idx = (*lock);
	xg_thread_mutex_t *flock = NULL;

	if (idx > XCFG_MUTEX_MAX) {
		return XI_MUTEX_RV_ERR_ARGS;
	}

	flock = &(_g_thr_locks.locks[idx]);

	ret = CloseHandle(flock->lock);
	if (!ret) {
		return XI_MUTEX_RV_ERR_ARGS;
	}

	//DeleteCriticalSection(&flock->lock);

	xg_tmutex_free(idx);

	return XI_MUTEX_RV_OK;
}

//------------
// Cond
//------------

xi_thread_cond_re xi_thread_cond_create(xi_thread_cond_t *cond, xchar *cname) {
	xuintptr idx;
	xg_thread_cond_t *fcond;

	idx = xg_tcond_alloc();
	if ((xintptr)idx < 0) {
		log_error(XDLOG, "Cannot create cond : [%s]!!! (total=%d)\n", cname, _g_thr_conds.counter);
		(*cond) = -1;
		return XI_COND_RV_ERR_NOMORE;
	}
	fcond = &(_g_thr_conds.conds[idx]);

	fcond->cond = CreateSemaphore(NULL, 0, LONG_MAX, NULL);
	if (fcond->cond == NULL) {
		return XI_COND_RV_ERR_ARGS;
	}
	InitializeCriticalSection(&fcond->csec);

	xi_strncpy(fcond->oname, cname, (XCFG_ONAME_MAX - 1));
	(*cond) = (xi_thread_cond_t) (idx);

	return XI_COND_RV_OK;
}

xi_thread_cond_re xi_thread_cond_wait(xi_thread_cond_t *cond,
		xi_thread_mutex_t *lock) {
	return xi_thread_cond_timedwait(cond, lock, INFINITE);
}

xi_thread_cond_re xi_thread_cond_timedwait(xi_thread_cond_t *cond,
		xi_thread_mutex_t *lock, xuint32 msec) {
	xi_thread_cond_re rv;
	DWORD res;

	xint32 wake = 0;
	xint32 generation;

	xuintptr cidx = (*cond);
	xuintptr lidx = (*lock);
	xg_thread_cond_t *fcond = NULL;
	xg_thread_mutex_t *flock = NULL;

	if (cidx > XCFG_COND_MAX || lidx > XCFG_MUTEX_MAX) {
		return XI_MUTEX_RV_ERR_ARGS;
	}

	fcond = &(_g_thr_conds.conds[cidx]);
	flock = &(_g_thr_locks.locks[lidx]);

	UNUSED(flock);

	EnterCriticalSection(&fcond->csec);
	fcond->nwait++;
	generation = fcond->generation;
	LeaveCriticalSection(&fcond->csec);

	xi_thread_mutex_unlock(lock);

	do {
		res = WaitForSingleObject(fcond->cond, msec);

		EnterCriticalSection(&fcond->csec);

		if (fcond->nwake) {
			if ((xint32)fcond->generation != generation) {
				fcond->nwake--;
				fcond->nwait--;
				rv = XI_COND_RV_OK;
				break;
			} else {
				wake = 1;
			}
		} else if (res != WAIT_OBJECT_0) {
			fcond->nwait--;
			rv = XI_COND_RV_ERR_TIMEOUT;
			break;
		}

		LeaveCriticalSection(&fcond->csec);

		if (wake) {
			wake = 0;
			ReleaseSemaphore(fcond->cond, 1, NULL);
		}
	} while (1);

	LeaveCriticalSection(&fcond->csec);
	xi_thread_mutex_lock(lock);

	return rv;
}

xi_thread_cond_re xi_thread_cond_broadcast(xi_thread_cond_t *cond) {
	xuint32 num_wake = 0;

	xuintptr cidx = (*cond);
	xg_thread_cond_t *fcond = NULL;

	if (cidx > XCFG_COND_MAX) {
		return XI_MUTEX_RV_ERR_ARGS;
	}

	fcond = &(_g_thr_conds.conds[cidx]);

	EnterCriticalSection(&fcond->csec);
	if (fcond->nwait > fcond->nwake) {
		num_wake = fcond->nwait - fcond->nwake;
		fcond->nwake = fcond->nwait;
		fcond->generation++;
	}
	LeaveCriticalSection(&fcond->csec);

	if (num_wake) {
		ReleaseSemaphore(fcond->cond, num_wake, NULL);
	}

	return XI_COND_RV_OK;
}

xi_thread_cond_re xi_thread_cond_signal(xi_thread_cond_t *cond) {
	xuint32 wake = 0;

	xuintptr cidx = (*cond);
	xg_thread_cond_t *fcond = NULL;

	if (cidx > XCFG_COND_MAX) {
		return XI_MUTEX_RV_ERR_ARGS;
	}

	fcond = &(_g_thr_conds.conds[cidx]);

	EnterCriticalSection(&fcond->csec);
	if (fcond->nwait > fcond->nwake) {
		wake = 1;
		fcond->nwake++;
		fcond->generation++;
	}
	LeaveCriticalSection(&fcond->csec);

	if (wake) {
		ReleaseSemaphore(fcond->cond, 1, NULL);
	}

	return XI_COND_RV_OK;
}

xi_thread_cond_re xi_thread_cond_destroy(xi_thread_cond_t *cond) {
	xbool ret;
	xuintptr cidx = (*cond);
	xg_thread_cond_t *fcond = NULL;

	if (cidx > XCFG_COND_MAX) {
		return XI_MUTEX_RV_ERR_ARGS;
	}

	fcond = &(_g_thr_conds.conds[cidx]);

	ret = CloseHandle(fcond->cond);
	if (!ret) {
		return XI_COND_RV_ERR_BUSY;
	}

	DeleteCriticalSection(&fcond->csec);
	xg_tcond_free(cidx);

	return XI_COND_RV_OK;
}
