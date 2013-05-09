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

#include <windows.h>

#include "xi/xi_mmap.h"

#include "xi/xi_log.h"

#include "xg_fd.h"

// ----------------------------------------------
// XI Functions
// ----------------------------------------------

xi_mmap_re xi_mmap_map(xvoid **addr, xsize length, xint32 prot, xint32 flags,
		xint32 fd, xint64 offset) {
	DWORD fmaccess = 0;
	DWORD mvaccess = 0;
	HANDLE fhnd = NULL;

	xvoid *ret = NULL;
	xg_fd_t *fdesc = NULL;

	xbool anon = ((flags & XI_MMAP_TYPE_ANON) == XI_MMAP_TYPE_ANON);

	if (!anon && (fd < 0)) {
		log_print(XDLOG, "ANON Flag is SET!!! given fd is %d\n",fd);
		return XI_MMAP_RV_ERR_ACCESS;
	}

	if (length == 0) {
		(*addr) = NULL;
		return XI_MMAP_RV_OK;
	}

	if (prot & XI_MMAP_PROT_EXEC) {
		mvaccess |= FILE_MAP_EXECUTE;
	}
	if (prot & XI_MMAP_PROT_WRITE) {
		mvaccess |= FILE_MAP_WRITE;
	}
	if (prot & XI_MMAP_PROT_READ) {
		mvaccess |= FILE_MAP_READ;
	}

	switch (prot) {
	case 0x01: // Read
		fmaccess = PAGE_READONLY;
		break;
	case 0x02: // Write
		fmaccess = PAGE_READWRITE;
		break;
	case 0x03: // Read/Write
		fmaccess = PAGE_READWRITE;
		break;
	case 0x04: // Exec
		fmaccess = PAGE_EXECUTE_READ;
		break;
	case 0x05: // Exec/Read
		fmaccess = PAGE_EXECUTE_READ;
		break;
	case 0x06: // Write/Exec
		fmaccess = PAGE_EXECUTE_READWRITE;
		break;
	case 0x07:
		fmaccess = PAGE_EXECUTE_READWRITE;
		break;
	default:
		log_print(XDLOG, "INVALID PROT!!!!!!!!!!!\n");
		return XI_MMAP_RV_ERR_ARGS;
	}

	if (anon) {
#ifdef _WIN64
			fhnd = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, fmaccess|SEC_RESERVE, (DWORD)((length>>0x20)&0x7fffffff), (DWORD)(length&0xffffffff), NULL);
#else // !_WIN64
			fhnd = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, fmaccess|SEC_RESERVE, 0, length, NULL);
#endif // _WIN64
		if (fhnd == NULL) {
			log_print(XDLOG, "File mapping is failed!!!!!!!!!!!\n");
			return XI_MMAP_RV_ERR_NFILE;
		}
		ret = MapViewOfFile(fhnd, mvaccess, (DWORD)((offset>>0x20)&0x7fffffff), (DWORD)(offset&0xffffffff), length);
		if (ret == NULL) {
			CloseHandle(fhnd);
			return XI_MMAP_RV_ERR_ARGS;
		} else {
			(*addr) = VirtualAlloc(ret, length, MEM_COMMIT, fmaccess);;
			CloseHandle(fhnd);
			return XI_MMAP_RV_OK;
		}
	} else {
		fdesc = xg_fd_get(fd);
		if (fdesc == NULL) {
			log_print(XDLOG, "Cannot get the descriptor!!!!!!!!!!!\n");
			return XI_MMAP_RV_ERR_BADFD;
		}
		fhnd = CreateFileMapping(fdesc->desc.f.fd, NULL, fmaccess, 0, 0, NULL);
		if (fhnd == NULL) {
			log_print(XDLOG, "File mapping is failed!!!!!!!!!!!\n");
			return XI_MMAP_RV_ERR_NFILE;
		}
		ret = MapViewOfFile(fhnd, mvaccess, (DWORD)((offset>>0x20)&0x7fffffff), (DWORD)(offset&0xffffffff), length);
		if (ret == NULL) {
			CloseHandle(fhnd);
			return XI_MMAP_RV_ERR_ARGS;
		} else {
			(*addr) = ret;
			CloseHandle(fhnd);
			return XI_MMAP_RV_OK;
		}
	}
}

xi_mmap_re xi_mmap_protect(xvoid *addr, xsize length, xint32 prot) {
	DWORD fmaccess = 0;
	DWORD oldflag = 0;

	switch (prot) {
	case 0x01: // Read
		fmaccess = PAGE_READONLY;
		break;
	case 0x02: // Write
		fmaccess = PAGE_READWRITE;
		break;
	case 0x03: // Read/Write
		fmaccess = PAGE_READWRITE;
		break;
	case 0x04: // Exec
		fmaccess = PAGE_EXECUTE_READ;
		break;
	case 0x05: // Exec/Read
		fmaccess = PAGE_EXECUTE_READ;
		break;
	case 0x06: // Write/Exec
		fmaccess = PAGE_EXECUTE_READWRITE;
		break;
	case 0x07:
		fmaccess = PAGE_EXECUTE_READWRITE;
		break;
	default:
		return XI_MMAP_RV_ERR_ARGS;
	}

	if (VirtualProtect(addr, length, fmaccess, &oldflag)) {
		return XI_MMAP_RV_OK;
	} else {
		return XI_MMAP_RV_ERR_ARGS;
	}
}

xi_mmap_re xi_mmap_lock(xvoid *addr, xsize length) {
	if (VirtualLock(addr, length)) {
		return XI_MMAP_RV_OK;
	} else {
		return XI_MMAP_RV_ERR_ARGS;
	}
}

xi_mmap_re xi_mmap_unlock(xvoid *addr, xsize length) {
	if (VirtualUnlock(addr, length)) {
		return XI_MMAP_RV_OK;
	} else {
		return XI_MMAP_RV_ERR_ARGS;
	}
}

xi_mmap_re xi_mmap_sync(xvoid *addr, xsize length, xint32 flags) {
	UNUSED(flags);
	if (FlushViewOfFile(addr, length)) {
		return XI_MMAP_RV_OK;
	} else {
		return XI_MMAP_RV_ERR_ARGS;
	}
}

xi_mmap_re xi_mmap_unmap(xvoid *addr, xsize length) {
	UNUSED(length);
	if (UnmapViewOfFile(addr)) {
		return XI_MMAP_RV_OK;
	} else {
		return XI_MMAP_RV_ERR_ARGS;
	}
}
