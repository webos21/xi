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

#ifndef _XI_ARRAYS_H_
#define _XI_ARRAYS_H_

/**
 * @brief XI ARRAY Search & Sort API
 *
 * @file xi_arrays.h
 * @date 2010-08-31
 * @author Cheolmin Jo (webos21@gmail.com)
 */

#include "xtype.h"

/**
 * Start Declaration
 */
_XI_EXTERN_C_BEGIN

/**
 * @defgroup xi_arrays ARRAY Search & Sort API
 * @ingroup XI
 * @{
 */

/**
 * Bit Scan (32bit word)
 *
 * @param i 32-bit word integer
 * @return the index position of first set bit
 */
xint32 xi_arrays_bscan32(xint32 i);

// Not supported on BCM
#if 0
/**
 * Bit Scan (64bit word)
 *
 * @param i 64-bit word integer
 * @return the index position of first set bit
 */
xint32 xi_arrays_bscan64(xint64 i);
#endif // 0

/**
 * Binary Search
 *
 * @param key the key value to find
 * @param base the pointer of array
 * @param nmemb the length of array
 * @param esize the size of each element in array
 * @param compar the compare function
 */
xvoid *xi_arrays_bsearch(const xvoid *key, const xvoid *base,
              xsize nmemb, xsize esize,
              xint32 (*compar)(const xvoid *, const xvoid *));


/**
 * Sort an array (with Quick-Sort Algorithm)
 *
 * @param base the pointer of array
 * @param nmemb the length of array
 * @param esize the size of each element in array
 * @param compar the compare function
 */
xvoid xi_arrays_qsort(xvoid *base, xsize nmemb, xsize esize,
           xint32(*compar)(const xvoid *, const xvoid *));


/**
 * @}  // end of xi_arrays
 */

/**
 * End Declaration
 */
_XI_EXTERN_C_END

#endif // _XI_ARRAYS_H_
