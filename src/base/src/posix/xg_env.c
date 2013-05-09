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

#include <stdlib.h>

#include "xi/xi_env.h"

#include "xi/xi_mem.h"
#include "xi/xi_string.h"

// ----------------------------------------------
// Global Variables
// ----------------------------------------------

#ifdef __APPLE__
#include <crt_externs.h>
#define environ (*_NSGetEnviron())
#else
extern xchar **environ;
#endif

// ----------------------------------------------
// XI Functions
// ----------------------------------------------

xssize xi_env_all(xchar *aenv[], xsize num) {
	xsize i = 0;

	xi_mem_set(aenv, 0, num);
	while (environ[i] != NULL && i < num) {
		aenv[i] = environ[i];
		i++;
	}
	return (xssize)i;
}

xssize xi_env_get(const xchar *key, xchar *buf, xsize buflen) {
	xchar *val = getenv(key);
	if (val) {
		xi_strncpy(buf, val, buflen);
		return (xssize) xi_strlen(buf);
	} else {
		return -1;
	}
}

xssize xi_env_set(const xchar *key, const xchar *val) {
	return setenv(key, val, TRUE);
}

xssize xi_env_del(const xchar *key) {
	return unsetenv(key);
}
