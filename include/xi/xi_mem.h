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

#ifndef _XI_MEM_H_
#define _XI_MEM_H_

/**
 * @brief XI Memory API
 *
 * @file xi_mem.h
 * @date 2010-08-31
 * @author Cheolmin Jo (webos21@gmail.com)
 */

#include "xtype.h"

/**
 * Start Declaration
 */
_XI_EXTERN_C_BEGIN

/**
 * @defgroup xi_mem Memory API
 * @ingroup XI
 * @{
 */

/**
 * Allocates size bytes and returns a pointer to the allocated memory.
 * The memory is not cleared.
 *
 * @param size The size of memory to allocate
 * @return a pointer of allocated memory
 */
xvoid  *xi_mem_alloc(xsize size);


/**
 * Allocates memory for an array of nmemb elements of size bytes each
 * and returns a pointer to the allocated memory.
 * The memory is set to zero.
 *
 * @param nmemb The number of memory block
 * @param size The size of each memory block
 * @return a pointer of allocated memory
 */
xvoid  *xi_mem_calloc(xsize nmemb, xsize size);


/**
 * Changes the size of the memory block pointed to by ptr to size bytes.
 * The contents will be unchanged to the minimum of the old and new sizes.
 * Newly allocated memory will be uninitialized.
 *
 * @param ptr The pointer to be changed
 * @param size The size that changed to
 * @return a pointer of reallocated memory
 */
xvoid  *xi_mem_realloc(xvoid *ptr, xsize size);


/**
 * Frees the memory space pointed to by ptr,
 * which must have been returned by a previous call to malloc(), calloc() or realloc().
 *
 * @param ptr The pointer to be freed
 */
xvoid   xi_mem_free(xvoid *ptr);


/**
 * Fills the first len bytes of the memory area pointed to
 * by ptr with the constant byte val.
 *
 * @param ptr The pointer of memory to be filled
 * @param val The value to fill
 * @param len The length to fill
 * @return Same pointer as a input ptr
 */
xvoid  *xi_mem_set(xvoid *ptr, xint32 val, xsize len);


/**
 * Copies len bytes from memory area src to memory area dest.
 * The memory areas should not overlap.
 *
 * @param dest The destination pointer to be copied
 * @param src The source pointer to be copied
 * @param len The length to be copied
 * @return Same pointer as the destination pointer
 */
xvoid  *xi_mem_copy(xvoid *dest, const xvoid *src, xsize len);


/**
 * Move memory area.
 * The memory areas may overlap:
 *     copying takes place as though the bytes in src are first
 *     copied into a temporary array that does not overlap src or dest,
 *     and the bytes are then copied from the temporary array to dest.
 *
 * @param dest The destination pointer to be moved
 * @param src The source pointer to be moved
 * @param n The length to be copied
 * @return Same pointer as the destination pointer
 */
xvoid  *xi_mem_move(xvoid *dest, const xvoid *src, xsize n);


/**
 * Compares the first n bytes of the memory areas s1 and s2.
 *
 * @param s1 The pointer to be compared
 * @param s2 The pointer to be compared
 * @param n The length to be compared
 * @return -1(s1<s2), 0(s1==s2), 1(s1>s2)
 */
xint32  xi_mem_cmp(const xvoid *s1, const xvoid *s2, xsize n);


/**
 * Scan memory for a character.
 * The first byte to match c (interpreted as an unsigned character)
 * stops the operation.
 *
 * @param s The memory area pointer to be scanned
 * @param c The character value to find
 * @param n The length to be searched
 * @return pointer of the character or NULL
 */
xvoid  *xi_mem_chr(const xvoid *s, xint32 c, xsize n);


/**
 * Tries to read specified number of bytes from given address into buffer.
 *
 * @param addr   - memory address to read.
 * @param size   - size of bytes to read.
 * @param buf    - buffer to read to.
 * @return <code>0</code> if OK; nonzero if an error occured.
 */
xssize	xi_mem_read(xvoid *addr, xsize size, xvoid *buf);


/**
 * Tries to write specified number of bytes from buffer to given address.
 *
 * @param addr   - memory address to write.
 * @param size   - size of bytes to write.
 * @param buf    - buffer to write from.
 * @return <code>0</code> if OK; nonzero if an error occured.
 */
xssize	xi_mem_write(xvoid *addr, xsize size, xvoid *buf);

#ifndef __arm__

#ifdef _WIN32
#include "md/membar_win32.h"
#elif __APPLE__
#include "md/membar_macosx.h"
#else
#include "md/membar_linux.h"
#endif

/**
 * Prevent re-ordering read and write
 */
#define xi_mem_barrier_rdwr() _xi_mem_barrier_rdwr()

/**
 * Prevent re-ordering write
 */
#define xi_mem_barrier_wr() _xi_mem_barrier_wr()

#endif // __arm__

#ifdef _WIN32
#include <malloc.h>
#else // !defined(_WIN32) || defined(__MINGW32__)
#include <alloca.h>
#endif // _WIN32
#define xi_alloca  alloca    ///< System independent mapping of function : alloca

/**
 * @}  // end of xi_mem
 */

/**
 * End Declaration
 */
_XI_EXTERN_C_END

#endif // _XI_MEM_H_
