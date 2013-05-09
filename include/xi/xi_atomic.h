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

#ifndef _XI_ATOMIC_H_
#define _XI_ATOMIC_H_

/**
 * @brief XI Atomic API
 *
 * @file xi_atomic.h
 * @date 2010-08-31
 * @author Cheolmin Jo (webos21@gmail.com)
 */

#include "xtype.h"

/**
 * Start Declaration
 */
_XI_EXTERN_C_BEGIN

/**
 * @defgroup xi_atomic Atomic API
 * @ingroup XI
 * @{
 * @brief
 *
 * CAS (Compare and Swap) is the primary API.
 * All the other APIs can be derived from it!!
 *
 * I will implement the whole-set of these APIs
 * for solving performance issues.
 *
 * Notice)
 *  - Linux/SMP8654 use same GCC Built-in API.
 *  - BCM use kernel API.
 *  - Win32 uses the VC intrinsic API.
 */

/**
 * compare an (xuint8 *mem)'s value with 'cmp'.
 * If they are the same swap the value with 'with'
 *
 * @param mem pointer to the value
 * @param with what to swap it with
 * @param cmp the value to compare it to
 * @return the old value of *mem
 */
xuint8      xi_atomic_cas8(volatile xuint8 *mem, xuint8 with, xuint8 cmp);

/**
 * compare an (xuint16 *mem)'s value with 'cmp'.
 * If they are the same swap the value with 'with'
 *
 * @param mem pointer to the value
 * @param with what to swap it with
 * @param cmp the value to compare it to
 * @return the old value of *mem
 */
xuint16     xi_atomic_cas16(volatile xuint16 *mem, xuint16 with, xuint16 cmp);

/**
 * compare an (xuint32 *mem)'s value with 'cmp'.
 * If they are the same swap the value with 'with'
 *
 * @param mem pointer to the value
 * @param with what to swap it with
 * @param cmp the value to compare it to
 * @return the old value of *mem
 */
xuint32     xi_atomic_cas32(volatile xuint32 *mem, xuint32 with, xuint32 cmp);

/**
 * compare an xuint64's value with 'cmp'.
 * If they are the same swap the value with 'with'
 *
 * @param mem pointer to the value
 * @param with what to swap it with
 * @param cmp the value to compare it to
 * @return the old value of *mem
 */
xuint64     xi_atomic_cas64(volatile xuint64 *mem, xuint64 with, xuint64 cmp);

/**
 * compare the pointer's value with cmp.
 * If they are the same swap the value with 'with'
 *
 * @param mem pointer to the pointer
 * @param with what to swap it with
 * @param cmp the value to compare it to
 * @return the old value of the pointer
 */
xvoid      *xi_atomic_casptr(volatile xvoid **mem, xvoid *with, const xvoid *cmp);

/**
 * exchange an xuint8's value with 'val'.
 *
 * @param mem pointer to the value
 * @param val what to swap it with
 * @return the old value of *mem
 */
xuint8      xi_atomic_xchg8(volatile xuint8 *mem, xuint8 val);

/**
 * exchange an xuint16's value with 'val'.
 *
 * @param mem pointer to the value
 * @param val what to swap it with
 * @return the old value of *mem
 */
xuint16     xi_atomic_xchg16(volatile xuint16 *mem, xuint16 val);

/**
 * exchange an xuint32's value with 'val'.
 *
 * @param mem pointer to the value
 * @param val what to swap it with
 * @return the old value of *mem
 */
xuint32     xi_atomic_xchg32(volatile xuint32 *mem, xuint32 val);

/**
 * exchange an xuint64's value with 'val'.
 *
 * @param mem pointer to the value
 * @param val what to swap it with
 * @return the old value of *mem
 */
xuint64     xi_atomic_xchg64(volatile xuint64 *mem, xuint64 val);

/**
 * exchange a pair of pointer values
 *
 * @param mem pointer to the pointer
 * @param val what to swap it with
 * @return the old value of the pointer
 */
xvoid      *xi_atomic_xchgptr(volatile xvoid **mem, xvoid *val);

/**
 * atomically read an xuint8 from memory
 *
 * @param mem the pointer
 * @return	  xuint8
 */
xuint8      xi_atomic_read8(volatile xuint8 *mem);

/**
 * atomically read an xuint16 from memory
 *
 * @param mem the pointer
 * @return	  xuint16
 */
xuint16     xi_atomic_read16(volatile xuint16 *mem);

/**
 * atomically read an xuint32 from memory
 *
 * @param mem the pointer
 * @return	  xuint32
 */
xuint32     xi_atomic_read32(volatile xuint32 *mem);

/**
 * atomically read an xuint64 from memory
 *
 * @param mem the pointer
 * @return	  xuint64
 */
xuint64     xi_atomic_read64(volatile xuint64 *mem);

/**
 * atomically set an xuint8 in memory
 *
 * @param mem pointer to the object
 * @param val value that the object will assume
*/
xvoid       xi_atomic_set8(volatile xuint8 *mem, xuint8 val);

/**
 * atomically set an xuint16 in memory
 *
 * @param mem pointer to the object
 * @param val value that the object will assume
*/
xvoid       xi_atomic_set16(volatile xuint16 *mem, xuint16 val);

/**
 * atomically set an xuint32 in memory
 *
 * @param mem pointer to the object
 * @param val value that the object will assume
*/
xvoid       xi_atomic_set32(volatile xuint32 *mem, xuint32 val);

/**
 * atomically set an xuint64 in memory
 *
 * @param mem pointer to the object
 * @param val value that the object will assume
*/
xvoid       xi_atomic_set64(volatile xuint64 *mem, xuint64 val);

/**
 * atomically add 'val' to an xuint8
 *
 * @param mem pointer to the object
 * @param val amount to add
 * @return old value pointed to by mem
 */
xuint8      xi_atomic_add8(volatile xuint8 *mem, xuint8 val);

/**
 * atomically add 'val' to an xuint16
 *
 * @param mem pointer to the object
 * @param val amount to add
 * @return old value pointed to by mem
 */
xuint16     xi_atomic_add16(volatile xuint16 *mem, xuint16 val);

/**
 * atomically add 'val' to an xuint32
 *
 * @param mem pointer to the object
 * @param val amount to add
 * @return old value pointed to by mem
 */
xuint32     xi_atomic_add32(volatile xuint32 *mem, xuint32 val);

/**
 * atomically add 'val' to an xuint64
 *
 * @param mem pointer to the object
 * @param val amount to add
 * @return old value pointed to by mem
 */
xuint64     xi_atomic_add64(volatile xuint64 *mem, xuint64 val);

/**
 * atomically subtract 'val' from an xuint8
 *
 * @param mem pointer to the object
 * @param val amount to subtract
 */
xvoid       xi_atomic_sub8(volatile xuint8 *mem, xuint8 val);

/**
 * atomically subtract 'val' from an xuint16
 *
 * @param mem pointer to the object
 * @param val amount to subtract
 */
xvoid       xi_atomic_sub16(volatile xuint16 *mem, xuint16 val);

/**
 * atomically subtract 'val' from an xuint32
 *
 * @param mem pointer to the object
 * @param val amount to subtract
 */
xvoid       xi_atomic_sub32(volatile xuint32 *mem, xuint32 val);

/**
 * atomically subtract 'val' from an xuint64
 *
 * @param mem pointer to the object
 * @param val amount to subtract
 */
xvoid       xi_atomic_sub64(volatile xuint64 *mem, xuint64 val);

/**
 * atomically increment an xuint8 by 1
 *
 * @param mem pointer to the object
 * @return old value pointed to by mem
 */
xuint8      xi_atomic_inc8(volatile xuint8 *mem);

/**
 * atomically increment an xuint16 by 1
 *
 * @param mem pointer to the object
 * @return old value pointed to by mem
 */
xuint16     xi_atomic_inc16(volatile xuint16 *mem);

/**
 * atomically increment an xuint32 by 1
 *
 * @param mem pointer to the object
 * @return old value pointed to by mem
 */
xuint32     xi_atomic_inc32(volatile xuint32 *mem);

/**
 * atomically increment an xuint64 by 1
 *
 * @param mem pointer to the object
 * @return old value pointed to by mem
 */
xuint64     xi_atomic_inc64(volatile xuint64 *mem);

/**
 * atomically decrement an xuint8 by 1
 *
 * @param mem pointer to the atomic value
 * @return zero if the value becomes zero on decrement, otherwise non-zero
 */
xuint8       xi_atomic_dec8(volatile xuint8 *mem);

/**
 * atomically decrement an xuint16 by 1
 *
 * @param mem pointer to the atomic value
 * @return zero if the value becomes zero on decrement, otherwise non-zero
 */
xuint16      xi_atomic_dec16(volatile xuint16 *mem);

/**
 * atomically decrement an xuint32 by 1
 *
 * @param mem pointer to the atomic value
 * @return zero if the value becomes zero on decrement, otherwise non-zero
 */
xuint32      xi_atomic_dec32(volatile xuint32 *mem);

/**
 * atomically decrement an xuint64 by 1
 *
 * @param mem pointer to the atomic value
 * @return zero if the value becomes zero on decrement, otherwise non-zero
 */
xuint64      xi_atomic_dec64(volatile xuint64 *mem);



/**
 * @}  // end of xi_atomic
 */

/**
 * End Declaration
 */
_XI_EXTERN_C_END

#endif // _XI_ATOMIC_H_
