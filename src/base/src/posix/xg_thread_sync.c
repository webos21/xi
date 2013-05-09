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

#include <pthread.h>
#include <errno.h>

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
	pthread_mutex_t lock;
} xg_thread_mutex_t;

typedef struct _st_thread_cond {
	xint32 used;
	xchar oname[XCFG_ONAME_MAX];
	pthread_cond_t cond;
} xg_thread_cond_t;

typedef struct _st_thread_mutex_pool {
	xint32 init;
	xint32 counter;
	xintptr last;
	xg_thread_mutex_t locks[XCFG_MUTEX_MAX];
} xg_thr_locks_t;

typedef struct _st_thread_cond_pool {
	xint32 init;
	xint32 counter;
	xintptr last;
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

static xintptr xg_tmutex_alloc() {
	xintptr i;

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

static xintptr xg_tmutex_free(xintptr idx) {
	if (_g_thr_locks.locks[idx].used) {
		_g_thr_locks.counter--;
		_g_thr_locks.locks[idx].used = 0;
		xi_mem_set(_g_thr_locks.locks[idx].oname, 0, XCFG_ONAME_MAX);
	}
	return idx;
}

static xintptr xg_tcond_alloc() {
	xintptr i;

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

static xintptr xg_tcond_free(xintptr idx) {
	if (_g_thr_conds.conds[idx].used) {
		_g_thr_conds.counter--;
		_g_thr_conds.conds[idx].used = 0;
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
	xint32 ret;
	xintptr idx;
	xg_thread_mutex_t *flock;

	idx = xg_tmutex_alloc();
	if (idx < 0) {
		log_error(XDLOG, "Cannot create mutex : [%s]!!! (total=%d)\n", mname,
				_g_thr_locks.counter);
		(*lock) = (xi_thread_mutex_t)-1;
		return XI_MUTEX_RV_ERR_NOMORE;
	}
	flock = &(_g_thr_locks.locks[idx]);

	ret = pthread_mutex_init(&flock->lock, NULL);
	switch (ret) {
	case 0:
		xi_strncpy(flock->oname, mname, (XCFG_ONAME_MAX - 1));
		(*lock) = (xi_thread_mutex_t) (idx);
		return XI_MUTEX_RV_OK;
	case EAGAIN:
		return XI_MUTEX_RV_ERR_NOMORE;
	case ENOMEM:
		return XI_MUTEX_RV_ERR_NOMEM;
	case EPERM:
		return XI_MUTEX_RV_ERR_PERM;
	case EBUSY:
		return XI_MUTEX_RV_ERR_BUSY;
	case EINVAL:
	default:
		return XI_MUTEX_RV_ERR_ARGS;
	}
}

xi_thread_mutex_re xi_thread_mutex_lock(xi_thread_mutex_t *lock) {
	xint32 ret;
	xintptr idx = (xintptr)(*lock);
	xg_thread_mutex_t *flock = NULL;

	if (idx > XCFG_MUTEX_MAX) {
		return XI_MUTEX_RV_ERR_ARGS;
	}

	flock = &(_g_thr_locks.locks[idx]);

	ret = pthread_mutex_lock(&flock->lock);
	switch (ret) {
	case 0:
		return XI_MUTEX_RV_OK;
	case EBUSY:
		return XI_MUTEX_RV_ERR_BUSY;
	case EAGAIN:
		return XI_MUTEX_RV_ERR_NOMORE;
	case EDEADLK:
		return XI_MUTEX_RV_ERR_AGAIN;
	case EINVAL:
	default:
		return XI_MUTEX_RV_ERR_ARGS;
	}
}

xi_thread_mutex_re xi_thread_mutex_trylock(xi_thread_mutex_t *lock) {
	xint32 ret;
	xintptr idx = (xintptr)(*lock);
	xg_thread_mutex_t *flock = NULL;

	if (idx > XCFG_MUTEX_MAX) {
		return XI_MUTEX_RV_ERR_ARGS;
	}

	flock = &(_g_thr_locks.locks[idx]);

	ret = pthread_mutex_trylock(&flock->lock);
	switch (ret) {
	case 0:
		return XI_MUTEX_RV_OK;
	case EBUSY:
		return XI_MUTEX_RV_ERR_BUSY;
	case EAGAIN:
		return XI_MUTEX_RV_ERR_NOMORE;
	case EDEADLK:
		return XI_MUTEX_RV_ERR_AGAIN;
	case EINVAL:
	default:
		return XI_MUTEX_RV_ERR_ARGS;
	}
}

xi_thread_mutex_re xi_thread_mutex_unlock(xi_thread_mutex_t *lock) {
	xint32 ret;
	xintptr idx = (xintptr)(*lock);
	xg_thread_mutex_t *flock = NULL;

	if (idx > XCFG_MUTEX_MAX) {
		return XI_MUTEX_RV_ERR_ARGS;
	}

	flock = &(_g_thr_locks.locks[idx]);

	ret = pthread_mutex_unlock(&flock->lock);
	switch (ret) {
	case 0:
		return XI_MUTEX_RV_OK;
	case EAGAIN:
		return XI_MUTEX_RV_ERR_NOMORE;
	case EDEADLK:
		return XI_MUTEX_RV_ERR_AGAIN;
	case EINVAL:
	default:
		return XI_MUTEX_RV_ERR_ARGS;
	}
}

xi_thread_mutex_re xi_thread_mutex_destroy(xi_thread_mutex_t *lock) {
	xint32 ret;
	xintptr idx = (xintptr)(*lock);
	xg_thread_mutex_t *flock = NULL;

	if (idx > XCFG_MUTEX_MAX) {
		return XI_MUTEX_RV_ERR_ARGS;
	}

	flock = &(_g_thr_locks.locks[idx]);

	ret = pthread_mutex_destroy(&(flock->lock));
	switch (ret) {
	case 0:
		xg_tmutex_free(idx);
		return XI_MUTEX_RV_OK;
	case EBUSY:
		return XI_MUTEX_RV_ERR_BUSY;
	case EINVAL:
	default:
		return XI_MUTEX_RV_ERR_ARGS;
	}
}

//------------
// Cond
//------------

xi_thread_cond_re xi_thread_cond_create(xi_thread_cond_t *cond, xchar *cname) {
	xint32 ret;
	xintptr idx;
	xg_thread_cond_t *fcond;

	idx = xg_tcond_alloc();
	if (idx < 0) {
		log_error(XDLOG, "Cannot create cond : [%s]!!! (total=%d)\n", cname,
				_g_thr_conds.counter);
		(*cond) = (xi_thread_cond_t)-1;
		return XI_COND_RV_ERR_NOMORE;
	}
	fcond = &(_g_thr_conds.conds[idx]);

	ret = pthread_cond_init(&fcond->cond, NULL);
	switch (ret) {
	case 0:
		xi_strncpy(fcond->oname, cname, (XCFG_ONAME_MAX - 1));
		(*cond) = (xi_thread_cond_t)idx;
		return XI_COND_RV_OK;
	case EAGAIN:
		return XI_COND_RV_ERR_NOMORE;
	case ENOMEM:
		return XI_COND_RV_ERR_NOMEM;
	case EBUSY:
		return XI_COND_RV_ERR_BUSY;
	case EINVAL:
	default:
		return XI_COND_RV_ERR_ARGS;
	}
}

xi_thread_cond_re xi_thread_cond_wait(xi_thread_cond_t *cond,
		xi_thread_mutex_t *lock) {
	xint32 ret;
	xintptr cidx = (xintptr)(*cond);
	xintptr lidx = (xintptr)(*lock);
	xg_thread_cond_t *fcond = NULL;
	xg_thread_mutex_t *flock = NULL;

	if (cidx > XCFG_COND_MAX || lidx > XCFG_MUTEX_MAX) {
		return XI_MUTEX_RV_ERR_ARGS;
	}

	fcond = &(_g_thr_conds.conds[cidx]);
	flock = &(_g_thr_locks.locks[lidx]);

	ret = pthread_cond_wait(&fcond->cond, &flock->lock);
	switch (ret) {
	case 0:
		return XI_COND_RV_OK;
	case EPERM:
		return XI_COND_RV_ERR_PERM;
	case EINVAL:
	default:
		return XI_COND_RV_ERR_ARGS;
	}
}
#include <sys/time.h>
#include <limits.h>
#include <stdio.h>
xi_thread_cond_re xi_thread_cond_timedwait(xi_thread_cond_t *cond,
		xi_thread_mutex_t *lock, xuint32 msec) {
#ifdef __APPLE__
	struct timeval tv;
#endif
	struct timespec ts;
	xint32 ret;
	xintptr cidx = (xintptr)(*cond);
	xintptr lidx = (xintptr)(*lock);
	xg_thread_cond_t *fcond = NULL;
	xg_thread_mutex_t *flock = NULL;

	if (cidx > XCFG_COND_MAX || lidx > XCFG_MUTEX_MAX) {
		return XI_MUTEX_RV_ERR_ARGS;
	}

	fcond = &(_g_thr_conds.conds[cidx]);
	flock = &(_g_thr_locks.locks[lidx]);

#ifdef __APPLE__
	ret = gettimeofday(&tv, NULL);
#else
	ret = clock_gettime(CLOCK_REALTIME, &ts);
#endif
	switch (ret) {
	case 0:
		break;
	case EFAULT:
	case EINVAL:
	default:
		return XI_COND_RV_ERR_ARGS;
	}

#ifdef __APPLE__
	ts.tv_sec = tv.tv_sec + (msec / 1000);
	ts.tv_nsec = (tv.tv_usec + ((msec % 1000) * 1000)) * 1000;
#else
	ts.tv_sec += ((xint32)msec / 1000);
	ts.tv_nsec += ((xint32)msec % 1000) * 1000000;
#endif
	if (ts.tv_nsec >= 1000000000) {
		ts.tv_nsec -= 1000000000;
		++ts.tv_sec;
	}

	ret = pthread_cond_timedwait(&fcond->cond, &flock->lock, &ts);
	switch (ret) {
	case 0:
		return XI_COND_RV_OK;
	case EPERM:
		return XI_COND_RV_ERR_PERM;
	case ETIMEDOUT:
		return XI_COND_RV_ERR_TIMEOUT;
	case EINVAL:
	default:
		return XI_COND_RV_ERR_ARGS;
	}
}

xi_thread_cond_re xi_thread_cond_broadcast(xi_thread_cond_t *cond) {
	xint32 ret;
	xintptr cidx = (xintptr)(*cond);
	xg_thread_cond_t *fcond = NULL;

	if (cidx > XCFG_COND_MAX) {
		return XI_MUTEX_RV_ERR_ARGS;
	}

	fcond = &(_g_thr_conds.conds[cidx]);

	ret = pthread_cond_broadcast(&fcond->cond);
	switch (ret) {
	case 0:
		return XI_COND_RV_OK;
	case EINVAL:
	default:
		return XI_COND_RV_ERR_ARGS;
	}
}

xi_thread_cond_re xi_thread_cond_signal(xi_thread_cond_t *cond) {
	xint32 ret;
	xintptr cidx = (xintptr)(*cond);
	xg_thread_cond_t *fcond = NULL;

	if (cidx > XCFG_COND_MAX) {
		return XI_MUTEX_RV_ERR_ARGS;
	}

	fcond = &(_g_thr_conds.conds[cidx]);

	ret = pthread_cond_signal(&fcond->cond);
	switch (ret) {
	case 0:
		return XI_COND_RV_OK;
	case EINVAL:
	default:
		return XI_COND_RV_ERR_ARGS;
	}
}

xi_thread_cond_re xi_thread_cond_destroy(xi_thread_cond_t *cond) {
	xint32 ret;
	xintptr cidx = (xintptr)(*cond);
	xg_thread_cond_t *fcond = NULL;

	if (cidx > XCFG_COND_MAX) {
		return XI_MUTEX_RV_ERR_ARGS;
	}

	fcond = &(_g_thr_conds.conds[cidx]);

	ret = pthread_cond_destroy(&fcond->cond);
	switch (ret) {
	case 0:
		xg_tcond_free(cidx);
		return XI_COND_RV_OK;
	case EBUSY:
		return XI_COND_RV_ERR_BUSY;
	case EINVAL:
	default:
		return XI_COND_RV_ERR_ARGS;
	}
}
