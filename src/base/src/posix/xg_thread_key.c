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
 * File : xg_thread_key.c
 */

#include <pthread.h>
#include <errno.h>

#include "xi/xi_thread.h"

// ----------------------------------------------
// XI Functions
// ----------------------------------------------

xi_thread_key_re xi_thread_key_create(xi_thread_key_t *key) {
	xint32 ret;

	ret = pthread_key_create((pthread_key_t *) key, NULL);
	switch (ret) {
	case 0:
		return XI_TKEY_RV_OK;
	case EAGAIN:
		key = NULL;
		return XI_TKEY_RV_ERR_NOMORE;
	case ENOMEM:
		key = NULL;
		return XI_TKEY_RV_ERR_NOMEM;
	default:
		key = NULL;
		return XI_TKEY_RV_ERR_ARGS;
	}
}

xvoid *xi_thread_key_get(xi_thread_key_t key) {
	return pthread_getspecific(key);
}

xi_thread_key_re xi_thread_key_set(xi_thread_key_t key, const xvoid *data) {
	xint32 ret;

	ret = pthread_setspecific(key, data);
	switch (ret) {
	case 0:
		return XI_TKEY_RV_OK;
	case ENOMEM:
		return XI_TKEY_RV_ERR_NOMEM;
	case EINVAL:
	default:
		return XI_TKEY_RV_ERR_ARGS;
	}
}

xi_thread_key_re xi_thread_key_destroy(xi_thread_key_t key) {
	xint32 ret;

	ret = pthread_key_delete(key);
	switch (ret) {
	case 0:
		return XI_TKEY_RV_OK;
	case EINVAL:
	default:
		return XI_TKEY_RV_ERR_ARGS;
	}
}
