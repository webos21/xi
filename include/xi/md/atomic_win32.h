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

#ifdef _WIN32

/**
 * @brief Atomic API for Windows
 *
 * @file atomic_win32.h
 * @date 2011-04-10
 * @author Cheolmin Jo (webos21@gmail.com)
 */

#include "xtype.h"

#include <windows.h>

_XI_INLINE xuint8 xi_atomic_cas8(volatile xuint8 *mem, xuint8 with, xuint8 comp) {
	__asm {
		mov  al,  comp
			mov  dl,  with
			mov  ecx, mem
			lock cmpxchg [ecx], dl
			mov  comp, al
	}
	return comp;
}

_XI_INLINE xuint16 xi_atomic_cas16(volatile xuint16 *mem, xuint16 with, xuint16 cmp) {
	return _InterlockedCompareExchange16(mem, with, cmp);
}

_XI_INLINE xuint32 xi_atomic_cas32(volatile xuint32 *mem, xuint32 with, xuint32 cmp) {
	return _InterlockedCompareExchange(mem, with, cmp);
}

_XI_INLINE xuint64 xi_atomic_cas64(volatile xuint64 *mem, xuint64 with, xuint64 cmp) {
	return _InterlockedCompareExchange64(mem, with, cmp);
}

_XI_INLINE xvoid* xi_atomic_casptr(volatile xvoid **mem, xvoid *with, const xvoid *cmp) {
	return InterlockedCompareExchangePointer((xvoid* volatile*)mem, with, (xvoid*)cmp);
}

_XI_INLINE xuint8 xi_atomic_xchg8(volatile xuint8 *mem, xuint8 val) {
	volatile xuint8 oldValue = *mem;

	__asm {
		mov  al,  oldValue
			mov  dl,  val
			mov  ecx, mem
			lock cmpxchg [ecx], dl
			mov  oldValue, al
	}
	return oldValue;
}

_XI_INLINE xuint16 xi_atomic_xchg16(volatile xuint16 *mem, xuint16 val) {
	volatile xuint16 cmp = *mem;
	return _InterlockedCompareExchange16(mem, val, cmp);
}

_XI_INLINE xuint32 xi_atomic_xchg32(volatile xuint32 *mem, xuint32 val) {
	return InterlockedExchange(mem, val);
}

_XI_INLINE xuint64 xi_atomic_xchg64(volatile xuint64 *mem, xuint64 val) {
	volatile xuint64 cmp = *mem;
	return _InterlockedCompareExchange64(mem, val, cmp);
}

_XI_INLINE xvoid* xi_atomic_xchgptr(volatile xvoid **mem, xvoid *val) {
	return InterlockedExchangePointer((xvoid**)mem, val);
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
	volatile xuint8 oldValue = *mem;

	__asm {
		mov  al,  oldValue
			mov  dl,  val
			mov  ecx, mem
			lock cmpxchg [ecx], dl
			mov  oldValue, al
	}
}

_XI_INLINE xvoid xi_atomic_set16(volatile xuint16 *mem, xuint16 val) {
	volatile xuint16 cmp = *mem;
	_InterlockedCompareExchange16(mem, val, cmp);
}

_XI_INLINE xvoid xi_atomic_set32(volatile xuint32 *mem, xuint32 val) {
	InterlockedExchange(mem, val);
}

_XI_INLINE xvoid xi_atomic_set64(volatile xuint64 *mem, xuint64 val) {
	volatile xuint64 cmp = *mem;
	_InterlockedCompareExchange64(mem, val, cmp);
}

_XI_INLINE xuint8 xi_atomic_add8(volatile xuint8 *mem, xuint8 val) {
	volatile xuint8 oldValue = *mem;
	volatile xuint8 newValue = oldValue + val;

	__asm {
		mov  al,  oldValue
			mov  dl,  newValue
			mov  ecx, mem
			lock cmpxchg [ecx], dl
			mov  oldValue, al
	}
	return oldValue;
}

_XI_INLINE xuint16 xi_atomic_add16(volatile xuint16 *mem, xuint16 val) {
	volatile xuint16 oldValue = *mem;
	volatile xuint16 newValue = oldValue + val;

	return _InterlockedCompareExchange16(mem, newValue, oldValue);
}

_XI_INLINE xuint32 xi_atomic_add32(volatile xuint32 *mem, xuint32 val) {
	return InterlockedExchangeAdd(mem, val);
}

_XI_INLINE xuint64 xi_atomic_add64(volatile xuint64 *mem, xuint64 val) {
	volatile xuint64 oldValue = *mem;
	volatile xuint64 newValue = oldValue + val;

	return _InterlockedCompareExchange64(mem, newValue, oldValue);
}

/* Of course we want the 2's compliment of the unsigned value, val */
#pragma warning(disable: 4146)

_XI_INLINE xvoid xi_atomic_sub8(volatile xuint8 *mem, xuint8 val) {
	volatile xuint8 oldValue = *mem;
	volatile xuint8 newValue = oldValue - val;

	__asm {
		mov  al,  oldValue
			mov  dl,  newValue
			mov  ecx, mem
			lock cmpxchg [ecx], dl
			mov  oldValue, al
	}
}

_XI_INLINE xvoid xi_atomic_sub16(volatile xuint16 *mem, xuint16 val) {
	volatile xuint16 oldValue = *mem;
	volatile xuint16 newValue = oldValue - val;

	_InterlockedCompareExchange16(mem, newValue, oldValue);
}

_XI_INLINE xvoid xi_atomic_sub32(volatile xuint32 *mem, xuint32 val) {
	InterlockedExchangeAdd(mem, -val);
}

_XI_INLINE xvoid xi_atomic_sub64(volatile xuint64 *mem, xuint64 val) {
	volatile xuint64 oldValue = *mem;
	volatile xuint64 newValue = oldValue - val;

	_InterlockedCompareExchange64(mem, newValue, oldValue);
}

_XI_INLINE xuint8 xi_atomic_inc8(volatile xuint8 *mem) {
	volatile xuint8 oldValue = *mem;
	volatile xuint8 newValue = oldValue + 1;

	__asm {
		mov  al,  oldValue
			mov  dl,  newValue
			mov  ecx, mem
			lock cmpxchg [ecx], dl
			mov  oldValue, al
	}
	return oldValue;
}

_XI_INLINE xuint16 xi_atomic_inc16(volatile xuint16 *mem) {
	volatile xuint16 oldValue = *mem;
	volatile xuint16 newValue = oldValue + 1;

	return _InterlockedCompareExchange16(mem, newValue, oldValue);
}

_XI_INLINE xuint32 xi_atomic_inc32(volatile xuint32 *mem) {
	return _InterlockedIncrement(mem) - 1;
}

_XI_INLINE xuint64 xi_atomic_inc64(volatile xuint64 *mem) {
	volatile xuint64 oldValue = *mem;
	volatile xuint64 newValue = oldValue + 1;

	return _InterlockedCompareExchange64(mem, newValue, oldValue);
}

_XI_INLINE xint8 xi_atomic_dec8(volatile xuint8 *mem) {
	volatile xuint8 oldValue = *mem;
	volatile xuint8 newValue = oldValue - 1;
	__asm {
		mov  al,  oldValue
			mov  dl,  newValue
			mov  ecx, mem
			lock cmpxchg [ecx], dl
			mov  oldValue, al
	}
	return oldValue;
}

_XI_INLINE xint16 xi_atomic_dec16(volatile xuint16 *mem) {
	volatile xuint16 oldValue = *mem;
	volatile xuint16 newValue = oldValue - 1;

	return _InterlockedCompareExchange16(mem, newValue, oldValue);
}

_XI_INLINE xint32 xi_atomic_dec32(volatile xuint32 *mem) {
	return _InterlockedDecrement(mem);
}

_XI_INLINE xint64 xi_atomic_dec64(volatile xuint64 *mem) {
	volatile xuint64 oldValue = *mem;
	volatile xuint64 newValue = oldValue - 1;

	return _InterlockedCompareExchange64(mem, newValue, oldValue);
}

#endif // _WIN32

