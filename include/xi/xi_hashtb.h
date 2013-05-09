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

#ifndef _XI_HASHTB_H_
#define _XI_HASHTB_H_

/**
 * @brief XI HASH Table API
 *
 * @file xi_hashtb.h
 * @date 2012-02-27
 * @author Cheolmin Jo (webos21@gmail.com)
 */

#include "xtype.h"

/**
 * Start Declaration
 */
_XI_EXTERN_C_BEGIN

/**
 * @defgroup xi_hashtb HASH Table API
 * @ingroup XI
 * @{
 */

/**
 * When passing a key to xi_hashtb_set or xi_hashtb_get, this value can be
 * passed to indicate a string-valued key, and have xi_hashtb compute the
 * length automatically.
 *
 * @remark xi_hashtb will use xi_strlen(key) for the length. The NUL terminator
 *         is not included in the hash value (why throw a constant in?).
 *         Since the hash table merely references the provided key (rather
 *         than copying it), xi_hashtb_this() will return the NUL-term'd key.
 */
#define XI_HASHTB_KEY_STRING     (-1)

/**
 * Abstract type for hash tables.
 */
typedef struct _xi_hashtb xi_hashtb_t;

/**
 * Abstract type for scanning hash tables.
 */
typedef struct _xi_hashtb_idx xi_hashtb_idx_t;

/**
 * Custom functions for calculating hash values.
 *
 * @param key The key.
 * @param klen The length of the key
 */
typedef xuint32 (*xi_hashtb_keygen)(const xvoid *key, xint32 *klen);

/**
 * Create a hash table.
 *
 * @return The hash table just created
 */
xi_hashtb_t *xi_hashtb_create();

/**
 * Create a hash table with custom hash-function
 *
 * @param func A custom hash function.
 */
xi_hashtb_t *xi_hashtb_create_custom(xi_hashtb_keygen func);

/**
 * Associate a value with a key in a hash table.
 *
 * @param htb The hash table
 * @param key Pointer to the key (the key can be any type of C Language.)
 * @param klen Length of the key. Can be XI_HASH_KEY_STRING to use the string length.
 * @param val Value to associate with the key
 *
 * @remark The maximum length of key is 32 bytes
 * @remark If the value is NULL the hash entry is deleted.
 */
xvoid xi_hashtb_set(xi_hashtb_t *htb, const xvoid *key, xint32 klen,
		const xvoid *val);

/**
 * Look up the value associated with a key in a hash table.
 *
 * @param htb The hash table
 * @param key Pointer to the key
 * @param klen Length of the key. Can be APR_HASH_KEY_STRING to use the string length.
 * @return Returns NULL if the key is not present.
 */
xvoid *xi_hashtb_get(xi_hashtb_t *htb, const xvoid *key, xint32 klen);

/**
 * Get the number of key/value pairs in the hash table.
 *
 * @param htb The hash table
 * @return The number of key/value pairs in the hash table.
 */
xuint32 xi_hashtb_count(xi_hashtb_t *htb);

/**
 * Get the maximum available number of the hash table.
 *
 * @param htb The hash table
 * @return The maximum available number of the hash table.
 */
xuint32 xi_hashtb_max(xi_hashtb_t *htb);

/**
 * Start iterating over the entries in a hash table.
 *
 * @param htb The hash table
 * @return The iteration state
 *
 * @remark  There is no restriction on adding or deleting hash entries during
 * an iteration (although the results may be unpredictable unless all you do
 * is delete the current entry) and multiple iterations can be in
 * progress at the same time.
 *
 * @par Example:
 *
 * @code
 * int sum_values(xi_hashtb_t *ht)
 * {
 *     xi_hashtb_idx_t *hi;
 *     void *val;
 *     int sum = 0;
 *     for (hi = xi_hashtb_first(p, ht); hi; hi = xi_hashtb_next(hi)) {
 *         xi_hashtb_this(hi, NULL, NULL, &val);
 *         sum += *(int *)val;
 *     }
 *     return sum;
 * }
 * @endcode
 */
xi_hashtb_idx_t *xi_hashtb_first(xi_hashtb_t *htb);

/**
 * Continue iterating over the entries in a hash table.
 *
 * @param hidx The iteration state
 *
 * @return a pointer to the updated iteration state.  NULL if there are no more
 *         entries.
 */
xi_hashtb_idx_t *xi_hashtb_next(xi_hashtb_idx_t *hidx);

/**
 * Get the current entry's details from the iteration state.
 *
 * @param hidx The iteration state
 * @param key Return pointer for the pointer to the key.
 * @param klen Return pointer for the key length.
 * @param val Return pointer for the associated value.
 *
 * @remark The return pointers should point to a variable that will be set to the
 *         corresponding data, or they may be NULL if the data isn't interesting.
 */
xvoid xi_hashtb_this(xi_hashtb_idx_t *hidx, const xvoid **key, xint32 *klen,
		xvoid **val);

/**
 * Clear any key/value pairs in the hash table.
 *
 * @param htb The hash table
 */
xvoid xi_hashtb_clear(xi_hashtb_t *htb);

/**
 * Clear any key/value pairs in the hash table.
 *
 * @param htb The hash table
 */
xi_hashtb_t *xi_hashtb_clone(xi_hashtb_t *htb);

/**
 * Destroy the hash table.
 *
 * @param htb The hash table
 */
xvoid xi_hashtb_destroy(xi_hashtb_t *htb);

/**
 * @}  // end of xi_hashtb
 */

/**
 * End Declaration
 */
_XI_EXTERN_C_END

#endif // _XI_HASHTB_H_
