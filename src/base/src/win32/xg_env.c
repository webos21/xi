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
 * File   : xg_env.c
 */

#include <windows.h>

#include "xi/xi_env.h"

#include "xi/xi_mem.h"
#include "xi/xi_string.h"

static xchar _g_aenv[128][512];

// ----------------------------------------------
// XI Functions
// ----------------------------------------------

xssize xi_env_all(xchar * aenv[], xsize num) {
	xint32 i = 0;
	xint32 j = 0;
	xvoid *envptr = NULL;
	xchar *envstr = NULL;

	xi_mem_set(_g_aenv, 0, 128*512);
	xi_mem_set(aenv, 0, sizeof(xchar*) * num);

	envptr = GetEnvironmentStrings();
	envstr = envptr;
	// Variable strings are separated by NULL byte, and the block is 
	// terminated by a NULL byte.
	for (i=0, j=0; *envstr; envstr++, j=0) {
		while (*envstr) {
			_g_aenv[i][j++] = *envstr++;
		}
		if (i < (xint32) num) {
			aenv[i] = _g_aenv[i];
		}
		i++;
	}
	FreeEnvironmentStrings(envptr);

	return i;
}

xssize xi_env_get(const xchar *key, xchar *buf, xsize buflen) {
	xi_mem_set(buf, 0, buflen);
	return GetEnvironmentVariable(key, buf, (DWORD)buflen) ? (xssize)xi_strlen(buf) : -1;
}

xssize xi_env_set(const xchar *key, const xchar *val) {
	return SetEnvironmentVariable(key, val) ? 0 : -1;
}

xssize xi_env_del(const xchar *key) {
	return SetEnvironmentVariable(key, NULL) ? 0 : -1;
}
