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
 * File : xg_arrays.c
 */

#define _GNU_SOURCE
#include <string.h>
#include <stdlib.h>

#include "xi/xi_arrays.h"

// ----------------------------------------------
// XI Functions
// ----------------------------------------------

xint32 xi_arrays_bscan32(xint32 i) {
	return ffs(i);
}

// Not supported BCM
#if 0
xint32 xi_arrays_bscan64(xint64 i) {
	return ffsll(i);
}
#endif // 0

xvoid *xi_arrays_bsearch(const xvoid *key, const xvoid *base, xsize nmemb,
		xsize esize, xint32 (*compar)(const xvoid *, const xvoid *)) {
	return bsearch(key, base, nmemb, esize, compar);
}

xvoid xi_arrays_qsort(xvoid *base, xsize nmemb, xsize esize,
		xint32 (*compar)(const xvoid *, const xvoid *)) {
	return qsort(base, nmemb, esize, compar);
}
