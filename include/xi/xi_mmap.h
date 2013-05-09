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

#ifndef _XI_MMAP_H_
#define _XI_MMAP_H_

/**
 * @brief XI Memory Map API
 *
 * @file xi_mmap.h
 * @date 2010-08-31
 * @author Cheolmin Jo (webos21@gmail.com)
 */

#include "xtype.h"

/**
 * Start Declaration
 */
_XI_EXTERN_C_BEGIN

/**
 * @defgroup xi_mmap Memory Map API
 * @ingroup XI
 * @{
 */

/**
 * Return values of MMAP Functions
 */
typedef enum _e_mmap_rv {
	XI_MMAP_RV_OK           = 0,    // OK
	XI_MMAP_RV_ERR_ACCESS   = -1,   // File mode or Protection is invalid
	XI_MMAP_RV_ERR_BADFD    = -2,   // Bad File Descriptor
	XI_MMAP_RV_ERR_NFILE    = -3,   // Too many file opened
	XI_MMAP_RV_ERR_NODEV    = -4,   // Not supported file type
	XI_MMAP_RV_ERR_NOMEM    = -5,   // Request too many memory
	XI_MMAP_RV_ERR_PERM     = -6,   // Permission error
	XI_MMAP_RV_ERR_BUSY     = -7,   // Memory is using
	XI_MMAP_RV_ERR_ARGS     = -8    // Invalid Arguments
} xi_mmap_re;

/**
 * Protection Type
 */
typedef enum _e_mmap_prot {
	XI_MMAP_PROT_NONE    = 0x0,  // No Access
	XI_MMAP_PROT_READ    = 0x1,  // Can be Read
	XI_MMAP_PROT_WRITE   = 0x2,  // Can be Write
	XI_MMAP_PROT_EXEC    = 0x4   // Can be Executed
} xi_mmap_prot_e;

/**
 * Share Type
 */
typedef enum _e_mmap_type {
	XI_MMAP_TYPE_SHARED  = 0x1,  // Share changes
	XI_MMAP_TYPE_PRIVATE = 0x2,  // Changes are private
	XI_MMAP_TYPE_ANON    = 0x20  // Don't use a file
} xi_mmap_type_e;

/**
 * Sync Type
 */
typedef enum _e_mmap_sync {
	XI_MMAP_SYNC_ASYNC      = 0x1,  // Sync memory asynchronously
	XI_MMAP_SYNC_INVALIDATE = 0x2,  // Invalidate the caches
	XI_MMAP_SYNC_SYNC       = 0x4   // Synchronous memory sync
} xi_mmap_sync_e;


/**
 * Creates a new mapping in the virtual address space of the calling process.
 *
 * @param addr    hint about where to place the mapping (can be NULL)
 * @param length  desired memory length of mapping
 * @param prot    desired memory protection of the mapping (must not conflict with the open mode of the file)
 * @param flags   operation flags (visibility, using file...)
 * @param fd      opened-file descriptor (open mode must be matched protection field)
 * @param offset  starting offset (must be a multiple of the page size)
 *
 * @return The result value of operation (xi_mmap_re)
 */
xi_mmap_re xi_mmap_map(xvoid **addr, xsize length,
		xint32 prot,  // xi_mmap_prot_e
		xint32 flags, // xi_mmap_type_e
        xint32 fd,    // file descriptor
        xoff64 offset);

/**
 * Changes protection for the calling process's memory page.
 *
 * @param addr    mapped memory pointer
 * @param length  desired memory length of changing protection
 * @param prot    desired memory protection of the mapping (must not conflict with the open mode of the file)
 *
 * @return The result value of operation (xi_mmap_re)
 */
xi_mmap_re  xi_mmap_protect(xvoid *addr, xsize length,
		xint32 prot // xi_mmap_prot_e
		);

/**
 * Lock part or all of the calling process's virtual address space into RAM,
 *
 * @param addr    mapped memory pointer
 * @param length  desired memory length of locking
 */
xi_mmap_re  xi_mmap_lock(xvoid *addr, xsize length);

/**
  * Unlock part or all of the calling process's virtual address space,
 *
 * @param addr    mapped memory pointer
 * @param length  desired memory length of unlocking
 */
xi_mmap_re  xi_mmap_unlock(xvoid *addr, xsize length);

/**
 * Flushes changes made to the in-core copy of a file
 * that was mapped into memory back to disk.
 *
 * @param addr    mapped memory pointer
 * @param length  desired memory length of synchronizing
 * @param flags   synchronizing method (xi_mmap_sync_e)
 */
xi_mmap_re  xi_mmap_sync(xvoid *addr, xsize length,
		xint32 flags // xi_mmap_sync_e
		);

/**
 * Deletes the mappings for the specified address range.
 *
 * @param addr    mapped memory pointer
 * @param length  desired memory length of deleting
 */
xi_mmap_re  xi_mmap_unmap(xvoid *addr, xsize length);

/**
 * @}  // end of xi_mmap
 */

/**
 * End Declaration
 */
_XI_EXTERN_C_END

#endif // _XI_MMAP_H_
