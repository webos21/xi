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
 * @brief Memory Barrier API for Windows
 *
 * @file membar_win32.h
 * @date 2011-04-10
 * @author Cheolmin Jo (webos21@gmail.com)
 */

#ifndef __MINGW32__

void _ReadWriteBarrier(void);
void _WriteBarrier(void);
void _mm_mfence(void);
void _mm_sfence(void);

#pragma intrinsic (_ReadWriteBarrier)
#pragma intrinsic (_WriteBarrier)
#pragma intrinsic (_mm_mfence)
#pragma intrinsic (_mm_sfence)

_XI_INLINE xvoid _xi_mem_barrier_rdwr() {
	_mm_mfence();
	_ReadWriteBarrier();
}
_XI_INLINE xvoid _xi_mem_barrier_wr() {
	_mm_sfence();
	_WriteBarrier();
}

// !!! Caution !!!
// The _ReadBarrier, _WriteBarrier, and _ReadWriteBarrier compiler intrinsics prevent only compiler re-ordering.
// To prevent the CPU from re-ordering read and write operations, use the MemoryBarrier macro.

// But, MemoryBarrier macro is supported since Vista.
// So, we can only use _mm_mfence and _mm_sfence

#else // __MINGW32__
#define _xi_mem_barrier_rdwr() __asm__ __volatile__ ("lock; addl $0,0(%%esp)" ::: "memory")
#define _xi_mem_barrier_wr() __asm__ __volatile__ ("" ::: "memory")
#endif

#endif // _WIN32
