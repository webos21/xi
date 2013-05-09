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

#include <windows.h>

#include "xi/xi_thread.h"

// ----------------------------------------------
// XI Functions
// ----------------------------------------------

xi_thread_key_re xi_thread_key_create(xi_thread_key_t *key) {
	DWORD hnd;

	hnd = TlsAlloc();
	if (hnd == TLS_OUT_OF_INDEXES) {
		return XI_TKEY_RV_ERR_NOMORE;
	}
	(*key) = hnd;

	return XI_TKEY_RV_OK;
}

xvoid *xi_thread_key_get(xi_thread_key_t key) {
	return TlsGetValue(key);
}

xi_thread_key_re xi_thread_key_set(xi_thread_key_t key, const xvoid *data) {
	xbool ret;

	ret = TlsSetValue(key, (xvoid *) data);
	if (!ret) {
		return XI_TKEY_RV_ERR_ARGS;
	}

	return XI_TKEY_RV_OK;
}

xi_thread_key_re xi_thread_key_destroy(xi_thread_key_t key) {
	xbool ret;

	ret = TlsFree(key);
	if (!ret) {
		return XI_TKEY_RV_ERR_ARGS;
	}

	return XI_TKEY_RV_OK;
}
