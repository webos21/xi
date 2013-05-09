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
 * File : xg_ctype.c
 */

#include "xi/xi_ctype.h"

#define _XOPEN_SOURCE
#include <ctype.h>

// ----------------------------------------------
// XI Functions
// ----------------------------------------------

xint32 xi_isalnum(xint32 c) {
	return isalnum(c);
}

xint32 xi_isalpha(xint32 c) {
	return isalpha(c);
}

xint32 xi_isascii(xint32 c) {
	return isascii(c);
}

xint32 xi_iscntrl(xint32 c) {
	return iscntrl(c);
}

xint32 xi_isdigit(xint32 c) {
	return isdigit(c);
}

xint32 xi_isgraph(xint32 c) {
	return isgraph(c);
}

xint32 xi_islower(xint32 c) {
	return islower(c);
}

xint32 xi_isprint(xint32 c) {
	return isprint(c);
}
xint32 xi_ispunct(xint32 c) {
	return ispunct(c);
}

xint32 xi_isspace(xint32 c) {
	return isspace(c);
}
xint32 xi_isupper(xint32 c) {
	return isupper(c);
}

xint32 xi_isxdigit(xint32 c) {
	return isxdigit(c);
}

xint32 xi_toascii(xint32 c) {
	return toascii(c);
}

xint32 xi_tolower(xint32 c) {
	return tolower(c);
}

xint32 xi_toupper(xint32 c) {
	return toupper(c);
}
