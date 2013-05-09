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

#include "xi/xi_dso.h"

#include "xi/xi_mem.h"
#include "xi/xi_env.h"
#include "xi/xi_string.h"

#include <dlfcn.h>

// ----------------------------------------------
// Inner Structures
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

	ret->hnd = dlopen(path, RTLD_NOW | RTLD_GLOBAL);
#ifdef __APPLE__
	if (ret->hnd == NULL) {
		char *suffix = xi_strrchr(path, '.');
		if (suffix != NULL && xi_strcmp(suffix, ".dylib")) {
			char tmpbuf[256];

			xi_strncpy(tmpbuf, path, suffix - path);
			xi_strcat(tmpbuf, ".jnilib");
			ret->hnd = dlopen(path, RTLD_NOW | RTLD_GLOBAL);
		}
	}
#endif
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

	dlerror();
	ret = dlsym(dso->hnd, funcname);
	if (dlerror() != NULL) {
		return NULL;
	}

	return ret;
}

xchar *xi_dso_error() {
	return dlerror();
}

xssize xi_dso_get_searchpath(xchar *pathbuf, xsize pbuflen) {
#ifdef __APPLE__
	return xi_env_get("DYLD_LIBRARY_PATH", pathbuf, pbuflen);
#else
	return xi_env_get("LD_LIBRARY_PATH", pathbuf, pbuflen);
#endif
}

xssize xi_dso_get_sysname(xchar *sysnamebuf, const xchar *name) {
	xi_strcpy(sysnamebuf, "lib");
	xi_strcat(sysnamebuf, name);
#ifdef __APPLE__
	xi_strcat(sysnamebuf, ".dylib");
#else
	xi_strcat(sysnamebuf, ".so");
#endif
	return (xssize) xi_strlen(sysnamebuf);
}

xbool xi_dso_unload(xi_dso_t *dso) {
	if (dso == NULL) {
		return FALSE;
	}

	if (dlclose(dso->hnd)) {
		return FALSE;
	}

	xi_mem_free(dso);

	return TRUE;
}
