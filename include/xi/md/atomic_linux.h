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

#if !defined(_WIN32) && !defined(__APPLE__)

/**
 * @brief Atomic API for Linux
 *
 * @file atomic_linux.h
 * @date 2011-04-10
 * @author Cheolmin Jo (webos21@gmail.com)
 */

#include "xtype.h"

#ifdef __GCC_HAVE_SYNC_COMPARE_AND_SWAP_4

_XI_INLINE xuint8 xi_atomic_cas8(volatile xuint8 *mem, xuint8 with, xuint8 cmp) {
	return __sync_val_compare_and_swap(mem, cmp, with);
}

_XI_INLINE xuint16 xi_atomic_cas16(volatile xuint16 *mem, xuint16 with, xuint16 cmp) {
	return __sync_val_compare_and_swap(mem, cmp, with);
}

_XI_INLINE xuint32 xi_atomic_cas32(volatile xuint32 *mem, xuint32 with, xuint32 cmp) {
	return __sync_val_compare_and_swap(mem, cmp, with);
}

_XI_INLINE xuint64 xi_atomic_cas64(volatile xuint64 *mem, xuint64 with, xuint64 cmp) {
	return __sync_val_compare_and_swap(mem, cmp, with);
}

_XI_INLINE xvoid *xi_atomic_casptr(volatile xvoid **mem, xvoid *with, const xvoid *cmp) {
	return (xvoid*) __sync_val_compare_and_swap(mem, cmp, with);
}

_XI_INLINE xuint8 xi_atomic_xchg8(volatile xuint8 *mem, xuint8 val) {
	__sync_synchronize();
	return __sync_lock_test_and_set(mem, val);
}

_XI_INLINE xuint16 xi_atomic_xchg16(volatile xuint16 *mem, xuint16 val) {
	__sync_synchronize();
	return __sync_lock_test_and_set(mem, val);
}

_XI_INLINE xuint32 xi_atomic_xchg32(volatile xuint32 *mem, xuint32 val) {
	__sync_synchronize();
	return __sync_lock_test_and_set(mem, val);
}

_XI_INLINE xuint64 xi_atomic_xchg64(volatile xuint64 *mem, xuint64 val) {
	__sync_synchronize();
	return __sync_lock_test_and_set(mem, val);
}

_XI_INLINE xvoid* xi_atomic_xchgptr(volatile xvoid **mem, xvoid *val) {
	__sync_synchronize();
	return (xvoid*) __sync_lock_test_and_set(mem, val);
}

_XI_INLINE xuint8 xi_atomic_read8(volatile xuint8 *mem) {
	return *mem;
}

_XI_INLINE xuint16 xi_atomic_read16(volatile xuint16 *mem) {
	return *mem;
}

_XI_INLINE xuint32 xi_atomic_read32(volatile xuint32 *mem) {
	return *mem;
}

_XI_INLINE xuint64 xi_atomic_read64(volatile xuint64 *mem) {
	return *mem;
}

_XI_INLINE xvoid xi_atomic_set8(volatile xuint8 *mem, xuint8 val) {
	*mem = val;
}

_XI_INLINE xvoid xi_atomic_set16(volatile xuint16 *mem, xuint16 val) {
	*mem = val;
}

_XI_INLINE xvoid xi_atomic_set32(volatile xuint32 *mem, xuint32 val) {
	*mem = val;
}

_XI_INLINE xvoid xi_atomic_set64(volatile xuint64 *mem, xuint64 val) {
	*mem = val;
}

_XI_INLINE xuint8 xi_atomic_add8(volatile xuint8 *mem, xuint8 val) {
	return __sync_fetch_and_add(mem, val);
}

_XI_INLINE xuint16 xi_atomic_add16(volatile xuint16 *mem, xuint16 val) {
	return __sync_fetch_and_add(mem, val);
}

_XI_INLINE xuint32 xi_atomic_add32(volatile xuint32 *mem, xuint32 val) {
	return __sync_fetch_and_add(mem, val);
}

_XI_INLINE xuint64 xi_atomic_add64(volatile xuint64 *mem, xuint64 val) {
	return __sync_fetch_and_add(mem, val);
}

_XI_INLINE xvoid xi_atomic_sub8(volatile xuint8 *mem, xuint8 val) {
	__sync_fetch_and_sub(mem, val);
}

_XI_INLINE xvoid xi_atomic_sub16(volatile xuint16 *mem, xuint16 val) {
	__sync_fetch_and_sub(mem, val);
}

_XI_INLINE xvoid xi_atomic_sub32(volatile xuint32 *mem, xuint32 val) {
	__sync_fetch_and_sub(mem, val);
}

_XI_INLINE xvoid xi_atomic_sub64(volatile xuint64 *mem, xuint64 val) {
	__sync_fetch_and_sub(mem, val);
}

_XI_INLINE xuint8 xi_atomic_inc8(volatile xuint8 *mem) {
	return __sync_fetch_and_add(mem, 1);
}

_XI_INLINE xuint16 xi_atomic_inc16(volatile xuint16 *mem) {
	return __sync_fetch_and_add(mem, 1);
}

_XI_INLINE xuint32 xi_atomic_inc32(volatile xuint32 *mem) {
	return __sync_fetch_and_add(mem, 1);
}

_XI_INLINE xuint64 xi_atomic_inc64(volatile xuint64 *mem) {
	return __sync_fetch_and_add(mem, 1);
}

_XI_INLINE xint8 xi_atomic_dec8(volatile xuint8 *mem) {
	return __sync_sub_and_fetch(mem, 1);
}

_XI_INLINE xint16 xi_atomic_dec16(volatile xuint16 *mem) {
	return __sync_sub_and_fetch(mem, 1);
}

_XI_INLINE xint32 xi_atomic_dec32(volatile xuint32 *mem) {
	return __sync_sub_and_fetch(mem, 1);
}

_XI_INLINE xint64 xi_atomic_dec64(volatile xuint64 *mem) {
	return __sync_sub_and_fetch(mem, 1);
}

#else // !__GCC_HAVE_SYNC_COMPARE_AND_SWAP_4


#include <atomic.h>

_XI_INLINE xuint8 xi_atomic_cas8(volatile xuint8 *mem, xuint8 with, xuint8 cmp) {
	return atomic_compare_and_exchange_val_rel(mem, with, cmp);
}

_XI_INLINE xuint16 xi_atomic_cas16(volatile xuint16 *mem, xuint16 with, xuint16 cmp) {
	return atomic_compare_and_exchange_val_rel(mem, with, cmp);
}

_XI_INLINE xuint32 xi_atomic_cas32(volatile xuint32 *mem, xuint32 with, xuint32 cmp) {
	return atomic_compare_and_exchange_val_rel(mem, with, cmp);
}

_XI_INLINE xuint64 xi_atomic_cas64(volatile xuint64 *mem, xuint64 with, xuint64 cmp) {
	return atomic_compare_and_exchange_val_rel(mem, with, cmp);
}

_XI_INLINE xvoid *xi_atomic_casptr(volatile xvoid **mem, xvoid *with, const xvoid *cmp) {
	return (xvoid*) (xuint32*) atomic_compare_and_exchange_val_rel((volatile xuint32 *)mem, (xuint32)(xuint32 *)with, (xuint32)(xuint32 *)cmp);
}

_XI_INLINE xuint8 xi_atomic_xchg8(volatile xuint8 *mem, xuint8 val) {
	return atomic_exchange_rel(mem, val);
}

_XI_INLINE xuint16 xi_atomic_xchg16(volatile xuint16 *mem, xuint16 val) {
	return atomic_exchange_rel(mem, val);
}

_XI_INLINE xuint32 xi_atomic_xchg32(volatile xuint32 *mem, xuint32 val) {
	return atomic_exchange_rel(mem, val);
}

_XI_INLINE xuint64 xi_atomic_xchg64(volatile xuint64 *mem, xuint64 val) {
	return atomic_exchange_rel(mem, val);
}

_XI_INLINE xvoid *xi_atomic_xchgptr(volatile xvoid **mem, xvoid *val) {
	return (xvoid*) (xuint32*) atomic_exchange_rel((volatile xuint32 *)mem, (xuint32)(xuint32 *)val);
}

_XI_INLINE xuint8 xi_atomic_read8(volatile xuint8 *mem) {
	return *mem;
}

_XI_INLINE xuint16 xi_atomic_read16(volatile xuint16 *mem) {
	return *mem;
}

_XI_INLINE xuint32 xi_atomic_read32(volatile xuint32 *mem) {
	return *mem;
}

_XI_INLINE xuint64 xi_atomic_read64(volatile xuint64 *mem) {
	return *mem;
}

_XI_INLINE xvoid xi_atomic_set8(volatile xuint8 *mem, xuint8 val) {
	*mem = val;
}

_XI_INLINE xvoid xi_atomic_set16(volatile xuint16 *mem, xuint16 val) {
	*mem = val;
}

_XI_INLINE xvoid xi_atomic_set32(volatile xuint32 *mem, xuint32 val) {
	*mem = val;
}

_XI_INLINE xvoid xi_atomic_set64(volatile xuint64 *mem, xuint64 val) {
	*mem = val;
}

_XI_INLINE xuint8 xi_atomic_add8(volatile xuint8 *mem, xuint8 val) {
	xuint8 old = *mem;
	atomic_add(mem, val);
	return old;
}

_XI_INLINE xuint16 xi_atomic_add16(volatile xuint16 *mem, xuint16 val) {
	xuint16 old = *mem;
	atomic_add(mem, val);
	return old;
}

_XI_INLINE xuint32 xi_atomic_add32(volatile xuint32 *mem, xuint32 val) {
	xuint32 old = *mem;
	atomic_add(mem, val);
	return old;
}

_XI_INLINE xuint64 xi_atomic_add64(volatile xuint64 *mem, xuint64 val) {
	xuint64 old = *mem;
	atomic_add(mem, val);
	return old;
}

_XI_INLINE xvoid xi_atomic_sub8(volatile xuint8 *mem, xuint8 val) {
	atomic_add(mem, -val);
}

_XI_INLINE xvoid xi_atomic_sub16(volatile xuint16 *mem, xuint16 val) {
	atomic_add(mem, -val);
}

_XI_INLINE xvoid xi_atomic_sub32(volatile xuint32 *mem, xuint32 val) {
	atomic_add(mem, -val);
}

_XI_INLINE xvoid xi_atomic_sub64(volatile xuint64 *mem, xuint64 val) {
	atomic_add(mem, -val);
}

_XI_INLINE xuint8 xi_atomic_inc8(volatile xuint8 *mem) {
	xuint8 old = *mem;
	atomic_add(mem, 1);
	return old;
}

_XI_INLINE xuint16 xi_atomic_inc16(volatile xuint16 *mem) {
	xuint16 old = *mem;
	atomic_add(mem, 1);
	return old;
}

_XI_INLINE xuint32 xi_atomic_inc32(volatile xuint32 *mem) {
	xuint32 old = *mem;
	atomic_add(mem, 1);
	return old;
}

_XI_INLINE xuint64 xi_atomic_inc64(volatile xuint64 *mem) {
	xuint64 old = *mem;
	atomic_add(mem, 1);
	return old;
}

_XI_INLINE xint8 xi_atomic_dec8(volatile xuint8 *mem) {
	xint8 old = *mem;
	atomic_add(mem, -1);
	return old;
}

_XI_INLINE xint16 xi_atomic_dec16(volatile xuint16 *mem) {
	xint16 old = *mem;
	atomic_add(mem, -1);
	return old;
}

_XI_INLINE xint32 xi_atomic_dec32(volatile xuint32 *mem) {
	xuint32 old = *mem;
	atomic_add(mem, -1);
	return old;
}

_XI_INLINE xint64 xi_atomic_dec64(volatile xuint64 *mem) {
	xint64 old = *mem;
	atomic_add(mem, -1);
	return old;
}

#endif // __GCC_HAVE_SYNC_COMPARE_AND_SWAP_4

#endif // !defined(_WIN32) && !defined(__APPLE__)
