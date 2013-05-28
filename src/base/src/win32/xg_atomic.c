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
 * File : xg_atomic.c
 */

#include "xi/xi_atomic.h"

#include <intrin.h>
#include <windows.h>

// ----------------------------------------------
// XI Functions
// ----------------------------------------------

#ifdef _M_AMD64
#if defined(__MINGW32__) || defined(__MINGW64__)
xuint8 _InterlockedCompareExchange8(volatile xuint8 *Destination, xuint8 Exchange, xuint8 Comparand) {
	return __sync_val_compare_and_swap(Destination, Comparand, Exchange);
}
#endif
/*
#else
extern xuint8 _InterlockedCompareExchange8(volatile xuint8 *Destination, xuint8 Exchange, xuint8 Comparand);
*/

xuint8 xi_atomic_cas8(volatile xuint8 *mem, xuint8 with, xuint8 comp) {
	_InterlockedCompareExchange8(mem, with, comp);
	return comp;
}

xuint16 xi_atomic_cas16(volatile xuint16 *mem, xuint16 with, xuint16 cmp) {
	return _InterlockedCompareExchange16((xint16*)mem, with, cmp);
}

xuint32 xi_atomic_cas32(volatile xuint32 *mem, xuint32 with, xuint32 cmp) {
	// Windows long is always 32 bits
	return _InterlockedCompareExchange((long volatile *)mem, with, cmp);
}

xuint64 xi_atomic_cas64(volatile xuint64 *mem, xuint64 with, xuint64 cmp) {
	return _InterlockedCompareExchange64((xint64 *)mem, with, cmp);
}

xvoid* xi_atomic_casptr(volatile xvoid **mem, xvoid *with, const xvoid *cmp) {
	return InterlockedCompareExchangePointer((xvoid* volatile*)mem, with, (xvoid*)cmp);
}

xuint8 xi_atomic_xchg8(volatile xuint8 *mem, xuint8 val) {
	volatile xuint8 oldValue = *mem;
	_InterlockedCompareExchange8(mem, val, oldValue);
	return oldValue;
}

xuint16 xi_atomic_xchg16(volatile xuint16 *mem, xuint16 val) {
	volatile xuint16 cmp = *mem;

	return _InterlockedCompareExchange16((xint16 *)mem, val, cmp);
}

xuint32 xi_atomic_xchg32(volatile xuint32 *mem, xuint32 val) {
	// Windows long is always 32 bits
	return InterlockedExchange((long volatile *)mem, val);
}

xuint64 xi_atomic_xchg64(volatile xuint64 *mem, xuint64 val) {
	volatile xuint64 cmp = *mem;

	return _InterlockedCompareExchange64((xint64*)mem, val, cmp);
}

xvoid* xi_atomic_xchgptr(volatile xvoid **mem, xvoid *val) {
	return InterlockedExchangePointer((xvoid**)mem, val);
}

xuint8 xi_atomic_read8(volatile xuint8 *mem) {
	return *mem;
}

xuint16 xi_atomic_read16(volatile xuint16 *mem) {
	return *mem;
}

xuint32 xi_atomic_read32(volatile xuint32 *mem) {
	return *mem;
}

xuint64 xi_atomic_read64(volatile xuint64 *mem) {
	return *mem;
}

xvoid xi_atomic_set8(volatile xuint8 *mem, xuint8 val) {
	volatile xuint8 oldValue = *mem;

	_InterlockedCompareExchange8(mem, val, oldValue);
}

xvoid xi_atomic_set16(volatile xuint16 *mem, xuint16 val) {
	volatile xuint16 cmp = *mem;
	_InterlockedCompareExchange16((xint16 *)mem, val, cmp);
}

xvoid xi_atomic_set32(volatile xuint32 *mem, xuint32 val) {
	// Windows long is always 32 bits
	InterlockedExchange((long volatile*)mem, val);
}

xvoid xi_atomic_set64(volatile xuint64 *mem, xuint64 val) {
	volatile xuint64 cmp = *mem;

	_InterlockedCompareExchange64((xint64*)mem, val, cmp);
}

xuint8 xi_atomic_add8(volatile xuint8 *mem, xuint8 val) {
	volatile xuint8 oldValue = *mem;
	volatile xuint8 newValue = oldValue + val;

	_InterlockedCompareExchange8(mem, newValue, oldValue);

	return newValue;
}

xuint16 xi_atomic_add16(volatile xuint16 *mem, xuint16 val) {
	volatile xuint16 oldValue = *mem;
	volatile xuint16 newValue = oldValue + val;

	return _InterlockedCompareExchange16((xint16*)mem, newValue, oldValue);
}

xuint32 xi_atomic_add32(volatile xuint32 *mem, xuint32 val) {
	// Windows long is always 32 bits
	return InterlockedExchangeAdd((long volatile*)mem, val);
}

xuint64 xi_atomic_add64(volatile xuint64 *mem, xuint64 val) {
	volatile xuint64 oldValue = *mem;
	volatile xuint64 newValue = oldValue + val;

	return _InterlockedCompareExchange64((xint64*)mem, newValue, oldValue);
}

xvoid xi_atomic_sub8(volatile xuint8 *mem, xuint8 val) {
	volatile xuint8 oldValue = *mem;
	volatile xuint8 newValue = oldValue - val;

	_InterlockedCompareExchange8(mem, newValue, oldValue);
}

xvoid xi_atomic_sub16(volatile xuint16 *mem, xuint16 val) {
	volatile xuint16 oldValue = *mem;
	volatile xuint16 newValue = oldValue - val;

	_InterlockedCompareExchange16((xint16*)mem, newValue, oldValue);
}

xvoid xi_atomic_sub32(volatile xuint32 *mem, xuint32 val) {
	// Windows long is always 32 bits
	InterlockedExchangeAdd((long volatile*)mem, (0-val));
}

xvoid xi_atomic_sub64(volatile xuint64 *mem, xuint64 val) {
	volatile xuint64 oldValue = *mem;
	volatile xuint64 newValue = oldValue - val;

	_InterlockedCompareExchange64((xint64*)mem, newValue, oldValue);
}

xuint8 xi_atomic_inc8(volatile xuint8 *mem) {
	volatile xuint8 oldValue = *mem;
	volatile xuint8 newValue = oldValue + 1;

	_InterlockedCompareExchange8(mem, newValue, oldValue);
	return newValue;
}

xuint16 xi_atomic_inc16(volatile xuint16 *mem) {
	volatile xuint16 oldValue = *mem;
	volatile xuint16 newValue = oldValue + 1;

	return _InterlockedCompareExchange16((xint16*)mem, newValue, oldValue);
}

xuint32 xi_atomic_inc32(volatile xuint32 *mem) {
	// Windows long is always 32 bits
	return _InterlockedIncrement((long volatile*)mem) - 1;
}

xuint64 xi_atomic_inc64(volatile xuint64 *mem) {
	volatile xuint64 oldValue = *mem;
	volatile xuint64 newValue = oldValue + 1;

	return _InterlockedCompareExchange64((xint64*)mem, newValue, oldValue);
}

xuint8 xi_atomic_dec8(volatile xuint8 *mem) {
	volatile xuint8 oldValue = *mem;
	volatile xuint8 newValue = oldValue - 1;

	_InterlockedCompareExchange8(mem, newValue, oldValue);
	return newValue;
}

xuint16 xi_atomic_dec16(volatile xuint16 *mem) {
	volatile xuint16 oldValue = *mem;
	volatile xuint16 newValue = oldValue - 1;

	return _InterlockedCompareExchange16((xint16*)mem, newValue, oldValue);
}

xuint32 xi_atomic_dec32(volatile xuint32 *mem) {
	// Windows long is always 32 bits
	return _InterlockedDecrement((long volatile*)mem);
}

xuint64 xi_atomic_dec64(volatile xuint64 *mem) {
	volatile xuint64 oldValue = *mem;
	volatile xuint64 newValue = oldValue - 1;

	return _InterlockedCompareExchange64((xint64*)mem, newValue, oldValue);
}

#else // !_M_AMD64

#if defined(__MINGW32__) || defined(__MINGW64__)
xuint8 _InterlockedCompareExchange8(volatile xuint8 *Destination, xuint8 Exchange, xuint8 Comparand) {
	return __sync_val_compare_and_swap(Destination, Comparand, Exchange);
}
#endif
/*
extern xuint8 _InterlockedCompareExchange8(volatile xuint8 *Destination, xuint8 Exchange, xuint8 Comparand); {
	__asm {
		mov  al,  Comparand
			mov  dl,  Exchange
			mov  ecx, Destination
			lock cmpxchg [ecx], dl
			mov  Comparand, al
	}
	return Comparand;
}
*/

xuint8 xi_atomic_cas8(volatile xuint8 *mem, xuint8 with, xuint8 comp) {
	return _InterlockedCompareExchange8(mem, with, comp);
}

xuint16 xi_atomic_cas16(volatile xuint16 *mem, xuint16 with, xuint16 cmp) {
	return _InterlockedCompareExchange16((xint16*)mem, with, cmp);
}

xuint32 xi_atomic_cas32(volatile xuint32 *mem, xuint32 with, xuint32 cmp) {
	// Windows long is always 32 bits
	return _InterlockedCompareExchange((long volatile*)mem, with, cmp);
}

xuint64 xi_atomic_cas64(volatile xuint64 *mem, xuint64 with, xuint64 cmp) {
	return _InterlockedCompareExchange64((xint64*)mem, with, cmp);
}

xvoid* xi_atomic_casptr(volatile xvoid **mem, xvoid *with, const xvoid *cmp) {
	return InterlockedCompareExchangePointer((xvoid* volatile*)mem, with, (xvoid*)cmp);
}

xuint8 xi_atomic_xchg8(volatile xuint8 *mem, xuint8 val) {
	volatile xuint8 oldValue = *mem;
	_InterlockedCompareExchange8(mem, val, oldValue);
	return oldValue;
}

xuint16 xi_atomic_xchg16(volatile xuint16 *mem, xuint16 val) {
	volatile xint16 cmp = *mem;
	return _InterlockedCompareExchange16((xint16*)mem, val, cmp);
}

xuint32 xi_atomic_xchg32(volatile xuint32 *mem, xuint32 val) {
	// Windows long is always 32 bits
	return InterlockedExchange((long volatile*)mem, val);
}

xuint64 xi_atomic_xchg64(volatile xuint64 *mem, xuint64 val) {
	volatile xuint64 cmp = *mem;
	return _InterlockedCompareExchange64((xint64*)mem, val, cmp);
}

xvoid* xi_atomic_xchgptr(volatile xvoid **mem, xvoid *val) {
	return InterlockedExchangePointer((xvoid**)mem, val);
}

xuint8 xi_atomic_read8(volatile xuint8 *mem) {
	return *mem;
}

xuint16 xi_atomic_read16(volatile xuint16 *mem) {
	return *mem;
}

xuint32 xi_atomic_read32(volatile xuint32 *mem) {
	return *mem;
}

xuint64 xi_atomic_read64(volatile xuint64 *mem) {
	return *mem;
}

xvoid xi_atomic_set8(volatile xuint8 *mem, xuint8 val) {
	volatile xuint8 oldValue = *mem;
	_InterlockedCompareExchange8(mem, val, oldValue);
}

xvoid xi_atomic_set16(volatile xuint16 *mem, xuint16 val) {
	volatile xuint16 cmp = *mem;
	_InterlockedCompareExchange16((xint16*)mem, val, cmp);
}

xvoid xi_atomic_set32(volatile xuint32 *mem, xuint32 val) {
	// Windows long is always 32 bits
	InterlockedExchange((long volatile*)mem, val);
}

xvoid xi_atomic_set64(volatile xuint64 *mem, xuint64 val) {
	volatile xuint64 cmp = *mem;
	_InterlockedCompareExchange64((xint64*)mem, val, cmp);
}

xuint8 xi_atomic_add8(volatile xuint8 *mem, xuint8 val) {
	volatile xuint8 oldValue = *mem;
	volatile xuint8 newValue = oldValue + val;
	_InterlockedCompareExchange8(mem, newValue, oldValue);
	return oldValue;
}

xuint16 xi_atomic_add16(volatile xuint16 *mem, xuint16 val) {
	volatile xuint16 oldValue = *mem;
	volatile xuint16 newValue = oldValue + val;

	return _InterlockedCompareExchange16((xint16*)mem, newValue, oldValue);
}

xuint32 xi_atomic_add32(volatile xuint32 *mem, xuint32 val) {
	// Windows long is always 32 bits
	return InterlockedExchangeAdd((long volatile*)mem, val);
}

xuint64 xi_atomic_add64(volatile xuint64 *mem, xuint64 val) {
	volatile xuint64 oldValue = *mem;
	volatile xuint64 newValue = oldValue + val;

	return _InterlockedCompareExchange64((xint64*)mem, newValue, oldValue);
}

xvoid xi_atomic_sub8(volatile xuint8 *mem, xuint8 val) {
	volatile xuint8 oldValue = *mem;
	volatile xuint8 newValue = oldValue - val;
	_InterlockedCompareExchange8(mem, newValue, oldValue);
}

xvoid xi_atomic_sub16(volatile xuint16 *mem, xuint16 val) {
	volatile xuint16 oldValue = *mem;
	volatile xuint16 newValue = oldValue - val;

	_InterlockedCompareExchange16((xint16*)mem, newValue, oldValue);
}

xvoid xi_atomic_sub32(volatile xuint32 *mem, xuint32 val) {
	// Windows long is always 32 bits
	InterlockedExchangeAdd((long volatile*)mem, (0-val));
}

xvoid xi_atomic_sub64(volatile xuint64 *mem, xuint64 val) {
	volatile xuint64 oldValue = *mem;
	volatile xuint64 newValue = oldValue - val;

	_InterlockedCompareExchange64((xint64*)mem, newValue, oldValue);
}

xuint8 xi_atomic_inc8(volatile xuint8 *mem) {
	volatile xuint8 oldValue = *mem;
	volatile xuint8 newValue = oldValue + 1;
	_InterlockedCompareExchange8(mem, newValue, oldValue);
	return oldValue;
}

xuint16 xi_atomic_inc16(volatile xuint16 *mem) {
	volatile xuint16 oldValue = *mem;
	volatile xuint16 newValue = oldValue + 1;

	return _InterlockedCompareExchange16((xint16*)mem, newValue, oldValue);
}

xuint32 xi_atomic_inc32(volatile xuint32 *mem) {
	// Windows long is always 32 bits
	return _InterlockedIncrement((long volatile *)mem) - 1;
}

xuint64 xi_atomic_inc64(volatile xuint64 *mem) {
	volatile xuint64 oldValue = *mem;
	volatile xuint64 newValue = oldValue + 1;

	return _InterlockedCompareExchange64((xint64*)mem, newValue, oldValue);
}

xuint8 xi_atomic_dec8(volatile xuint8 *mem) {
	volatile xuint8 oldValue = *mem;
	volatile xuint8 newValue = oldValue - 1;
	_InterlockedCompareExchange8(mem, newValue, oldValue);
	return oldValue;
}

xuint16 xi_atomic_dec16(volatile xuint16 *mem) {
	volatile xuint16 oldValue = *mem;
	volatile xuint16 newValue = oldValue - 1;

	return _InterlockedCompareExchange16((xint16*)mem, newValue, oldValue);
}

xuint32 xi_atomic_dec32(volatile xuint32 *mem) {
	// Windows long is always 32 bits
	return _InterlockedDecrement((long volatile *)mem);
}

xuint64 xi_atomic_dec64(volatile xuint64 *mem) {
	volatile xuint64 oldValue = *mem;
	volatile xuint64 newValue = oldValue - 1;

	return _InterlockedCompareExchange64((xint64*)mem, newValue, oldValue);
}

#endif // _M_AMD64
