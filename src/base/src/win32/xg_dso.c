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
 * File   : xg_dso.c
 */

#include <windows.h>

#include "xi/xi_dso.h"

#include "xi/xi_mem.h"
#include "xi/xi_env.h"
#include "xi/xi_string.h"

// ----------------------------------------------
// Inner Structure
// ----------------------------------------------

struct _xi_dso {
	xvoid *hnd;
};

// ----------------------------------------------
// XI Functions
// ----------------------------------------------

xi_dso_t *xi_dso_load(const xchar *path) {
	xi_dso_t *ret = xi_mem_alloc(sizeof(xi_dso_t));
	if (ret == NULL) {
		return NULL;
	}

	ret->hnd = LoadLibrary(path);
	if (ret->hnd == NULL) {
		xi_mem_free(ret);
		return NULL;
	}

	return ret;
}

xvoid *xi_dso_get_func(xi_dso_t *dso, const xchar *funcname) {
	xvoid *ret;

	if (dso == NULL || funcname == NULL) {
		return NULL;
	}

	ret = GetProcAddress(dso->hnd, (LPCSTR)funcname);

	return ret;
}

xchar *xi_dso_error() {
	LPVOID lpMsgBuf;
	DWORD dw = GetLastError();

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | 
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dw,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR) &lpMsgBuf,
		0, NULL );

	return lpMsgBuf;
}

xssize xi_dso_get_searchpath(xchar *pathbuf, xsize pbuflen) {
	return xi_env_get("PATH", pathbuf, pbuflen);
}

xssize xi_dso_get_sysname(xchar *sysnamebuf, const xchar *name) {
	xi_strcpy(sysnamebuf, name);
	xi_strcat(sysnamebuf, ".dll");
	return xi_strlen(sysnamebuf);
}

xbool xi_dso_unload(xi_dso_t *dso) {
	if (dso == NULL) {
		return FALSE;
	}

	if (!FreeLibrary(dso->hnd)) {
		return FALSE;
	}

	xi_mem_free(dso);

	return TRUE;
}
