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
 * File : xg_mmap.c
 */

#include <sys/mman.h>
#include <errno.h>

#include "xi/xi_mmap.h"

// ----------------------------------------------
// XI Functions
// ----------------------------------------------

xi_mmap_re xi_mmap_map(xvoid **addr, xsize length, xint32 prot, xint32 flags,
		xint32 fd, xoff64 offset) {
	xvoid *ret;
	xint32 rprot;
	xint32 rflags;

	rprot = PROT_NONE;
	if (prot & XI_MMAP_PROT_EXEC) {
		rprot |= PROT_EXEC;
	}
	if (prot & XI_MMAP_PROT_WRITE) {
		rprot |= PROT_WRITE;
	}
	if (prot & XI_MMAP_PROT_READ) {
		rprot |= PROT_READ;
	}

	rflags = 0x0;
	if (flags & XI_MMAP_TYPE_SHARED) {
		rflags |= MAP_SHARED;
	}
	if (flags & XI_MMAP_TYPE_PRIVATE) {
		rflags |= MAP_PRIVATE;
	}
	if (flags & XI_MMAP_TYPE_ANON) {
		rflags |= MAP_ANON;
	}

	ret = mmap((*addr), (length & 0x7fffffff), rprot, rflags, fd, offset);
	if (ret == MAP_FAILED) {
		switch (errno) {
		case EACCES:
			return XI_MMAP_RV_ERR_ACCESS;
		case EBADF:
			return XI_MMAP_RV_ERR_BADFD;
		case ENFILE:
			return XI_MMAP_RV_ERR_NFILE;
		case ENODEV:
			return XI_MMAP_RV_ERR_NODEV;
		case EAGAIN:
		case ENOMEM:
			return XI_MMAP_RV_ERR_NOMEM;
		case EPERM:
			return XI_MMAP_RV_ERR_PERM;
		default:
			return XI_MMAP_RV_ERR_ARGS;
		}
	}
	(*addr) = ret;
	return XI_MMAP_RV_OK;
}

xi_mmap_re xi_mmap_protect(xvoid *addr, xsize length, xint32 prot) {
	xint32 rprot;

	rprot = PROT_NONE;
	if (prot & XI_MMAP_PROT_EXEC) {
		rprot |= PROT_EXEC;
	}
	if (prot & XI_MMAP_PROT_WRITE) {
		rprot |= PROT_WRITE;
	}
	if (prot & XI_MMAP_PROT_READ) {
		rprot |= PROT_READ;
	}

	if (mprotect(addr, length, rprot) < 0) {
		switch (errno) {
		case EACCES:
			return XI_MMAP_RV_ERR_ACCESS;
		case ENOMEM:
			return XI_MMAP_RV_ERR_NOMEM;
		default:
			return XI_MMAP_RV_ERR_ARGS;
		}
	}
	return XI_MMAP_RV_OK;
}

xi_mmap_re xi_mmap_lock(xvoid *addr, xsize length) {
	if (mlock(addr, length) < 0) {
		switch (errno) {
		case EPERM:
			return XI_MMAP_RV_ERR_PERM;
		case ENOMEM:
			return XI_MMAP_RV_ERR_NOMEM;
		default:
			return XI_MMAP_RV_ERR_ARGS;
		}
	}
	return XI_MMAP_RV_OK;
}

xi_mmap_re xi_mmap_unlock(xvoid *addr, xsize length) {
	if (munlock(addr, length) < 0) {
		switch (errno) {
		case EPERM:
			return XI_MMAP_RV_ERR_PERM;
		case ENOMEM:
			return XI_MMAP_RV_ERR_NOMEM;
		default:
			return XI_MMAP_RV_ERR_ARGS;
		}
	}
	return XI_MMAP_RV_OK;
}

xi_mmap_re xi_mmap_sync(xvoid *addr, xsize length, xint32 flags) {
	xint32 rflags = 0x0;

	if (flags & XI_MMAP_SYNC_ASYNC) {
		rflags |= MS_ASYNC;
	}
	if (flags & XI_MMAP_SYNC_INVALIDATE) {
		rflags |= MS_INVALIDATE;
	}
	if (flags & XI_MMAP_SYNC_SYNC) {
		rflags |= MS_SYNC;
	}

	if (msync(addr, length, rflags) < 0) {
		switch (errno) {
		case EBUSY:
			return XI_MMAP_RV_ERR_BUSY;
		case ENOMEM:
			return XI_MMAP_RV_ERR_NOMEM;
		default:
			return XI_MMAP_RV_ERR_ARGS;
		}
	}
	return XI_MMAP_RV_OK;
}

xi_mmap_re xi_mmap_unmap(xvoid *addr, xsize length) {
	if (munmap(addr, length) < 0) {
		switch (errno) {
		case EACCES:
			return XI_MMAP_RV_ERR_ACCESS;
		case EBADF:
			return XI_MMAP_RV_ERR_BADFD;
		case ENFILE:
			return XI_MMAP_RV_ERR_NFILE;
		case ENODEV:
			return XI_MMAP_RV_ERR_NODEV;
		case EAGAIN:
		case ENOMEM:
			return XI_MMAP_RV_ERR_NOMEM;
		case EPERM:
			return XI_MMAP_RV_ERR_PERM;
		default:
			return XI_MMAP_RV_ERR_ARGS;
		}
	}
	return XI_MMAP_RV_OK;
}
