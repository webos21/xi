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
 * File : xg_string.c
 */

#include "xi/xi_string.h"

#define _ISOC99_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <strings.h>

// ----------------------------------------------
// XI Functions
// ----------------------------------------------

xsize xi_strlen(const xchar *s) {
	if (s == NULL) {
		return 0;
	} else {
		return strlen(s);
	}
}

xchar *xi_strcat(xchar *dest, const xchar *src) {
	if (dest == NULL || src == NULL) {
		return NULL;
	} else {
		return strcat(dest, src);
	}
}

xchar *xi_strncat(xchar *dest, const xchar *src, xsize n) {
	if (dest == NULL || src == NULL || n == 0) {
		return NULL;
	} else {
		return strncat(dest, src, n);
	}
}

xchar *xi_strcpy(xchar *dest, const xchar *src) {
	if (dest == NULL || src == NULL) {
		return NULL;
	} else {
		return strcpy(dest, src);
	}
}

xchar *xi_strncpy(xchar *dest, const xchar *src, xsize n) {
	if (dest == NULL || src == NULL || n == 0) {
		return NULL;
	} else {
		return strncpy(dest, src, n);
	}
}

xchar *xi_strchr(const xchar *s, xint32 c) {
	if (s == NULL) {
		return NULL;
	} else {
		return strchr(s, c);
	}
}

xchar *xi_strrchr(const xchar *s, xint32 c) {
	if (s == NULL) {
		return NULL;
	} else {
		return strrchr(s, c);
	}
}

xchar *xi_strstr(const xchar *s, const xchar *k) {
	if (s == NULL || k == NULL) {
		return NULL;
	} else {
		return strstr(s, k);
	}
}

xchar *xi_strtok(xchar *s, const xchar *delim, xchar **ptrptr) {
	return strtok_r(s, delim, ptrptr);
}

xint32 xi_strcmp(const xchar *s1, const xchar *s2) {
	if (s1 == NULL || s2 == NULL) {
		return -1;
	} else {
		return strcmp(s1, s2);
	}
}

xint32 xi_strncmp(const xchar *s1, const xchar *s2, xsize n) {
	if (s1 == NULL || s2 == NULL) {
		return -2;
	} else {
		return strncmp(s1, s2, n);
	}
}

xint32 xi_strcasecmp(const xchar *s1, const xchar *s2) {
	if (s1 == NULL || s2 == NULL) {
		return -2;
	} else {
		return strcasecmp(s1, s2);
	}
}

xint32 xi_strncasecmp(const xchar *s1, const xchar *s2, xsize n) {
	return strncasecmp(s1, s2, n);
}

xint32 xi_sprintf(xchar *str, const xchar *format, ...) {
	xint32 ret;
	va_list va;

	if (str == NULL || format == NULL) {
		return -2;
	}

	va_start(va, format);
	ret = vsprintf(str, format, va);
	va_end(va);

	return ret;
}

xint32 xi_snprintf(xchar *str, xsize size, const xchar *format, ...) {
	xint32 ret;
	va_list va;

	if (str == NULL || size == 0 || format == NULL) {
		return -2;
	}

	va_start(va, format);
	ret = vsnprintf(str, size, format, va);
	va_end(va);

	return ret;
}

xint32 xi_strtoi(const xchar *str, xchar **endptr, xint32 base) {
	if (str == NULL) {
		return 0;
	} else {
		return (xint32) strtol(str, endptr, base);
	}
}

xint64 xi_strtoi64(const xchar *str, xchar **endptr, xint32 base) {
	if (str == NULL) {
		return 0;
	} else {
		return strtoll(str, endptr, base);
	}
}

xfloat32 xi_strtof(const xchar *str, xchar **endptr) {
	if (str == NULL) {
		return 0;
	} else {
		return strtof(str, endptr);
	}
}

xfloat64 xi_strtof64(const xchar *str, xchar **endptr) {
	if (str == NULL) {
		return 0;
	} else {
		return strtod(str, endptr);
	}
}
