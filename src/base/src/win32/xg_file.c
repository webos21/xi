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
 * File   : xg_file.c
 */

#ifdef _WIN32
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "xg_fd.h"

#include "xi/xi_mem.h"
#include "xi/xi_string.h"

#include <direct.h>
#include <io.h>

// ----------------------------------------------
// Inner Structure
// ----------------------------------------------

struct _xi_dir {
	HANDLE dfd;
	xchar pathname[XCFG_PATHNAME_MAX];
};

// ----------------------------------------------
// XI Functions
// ----------------------------------------------

xint32 xi_file_open(const xchar *pathname, xint32 mode, xint32 perm) {
	xg_fd_t pfd;

	DWORD oflags = 0;
	DWORD createflags = 0;
	DWORD attributes = 0;
	DWORD sharemode = FILE_SHARE_READ | FILE_SHARE_WRITE;

	if (pathname == NULL) {
		return XI_FILE_RV_ERR_ARGS;
	}

	if (mode & XI_FILE_MODE_READ) {
		oflags |= GENERIC_READ;
	}
	if (mode & XI_FILE_MODE_WRITE) {
		oflags |= GENERIC_WRITE;
	}

	if (mode & XI_FILE_MODE_CREATE) {
		if (mode & XI_FILE_MODE_EXCL) {
			createflags = CREATE_NEW;
		} else if (mode & XI_FILE_MODE_TRUNCATE) {
			createflags = CREATE_ALWAYS;
		} else {
			createflags = OPEN_ALWAYS;
		}
	} else if (mode & XI_FILE_MODE_TRUNCATE) {
		createflags = TRUNCATE_EXISTING;
	} else {
		createflags = OPEN_EXISTING;
	}

	if ((mode & XI_FILE_MODE_EXCL) && !(mode & XI_FILE_MODE_CREATE)) {
		return XI_FILE_RV_ERR_ARGS;
	}

	pfd.desc.f.fd = CreateFile(pathname, oflags, sharemode, NULL, createflags,
			attributes, 0);
	if (pfd.desc.f.fd == INVALID_HANDLE_VALUE) {
		return XI_FILE_RV_ERR_ARGS;
	}

	pfd.type = XG_FD_TYPE_FILE;
	pfd.desc.f.mode = (xi_file_mode_e)mode;
	pfd.desc.f.perm = (xi_file_perm_e)perm;
	pfd.desc.f.path = (xchar *) pathname;

	return xg_fd_open(&pfd);
}

xint32 xi_file_rpeek(xint32 fd) {
	xbool ret = 0;
	DWORD rbytes = 0;
	xg_fd_t *fdesc = NULL;

	if (fd < 0) {
		return XI_FILE_RV_ERR_ARGS;
	}

	fdesc = xg_fd_get(fd);

	if (fdesc->type == XG_FD_TYPE_FILE) {
		ret = TRUE;
	} else if (fdesc->type == XG_FD_TYPE_SOCK) {
		ret = ioctlsocket(fdesc->desc.s.fd, FIONREAD, &rbytes);
	}
	if (!ret) {
		return XI_FILE_RV_ERR_ARGS;
	}

	return rbytes;
}

xssize xi_file_read(xint32 fd, xvoid *buf, xsize buflen) {
	xbool ret = 0;
	xg_fd_t *fdesc = NULL;
	DWORD rbytes = 0;
	DWORD blen = (DWORD) buflen;

	if (fd < 0 || buf == NULL) {
		return XI_FILE_RV_ERR_ARGS;
	}

	if (buflen == 0) {
		return 0;
	}

	fdesc = xg_fd_get(fd);

	if (fdesc->type == XG_FD_TYPE_FILE || fdesc->type == XG_FD_TYPE_PIPE) {
		ret = ReadFile(fdesc->desc.f.fd, buf, blen, &rbytes, NULL);
		if (!ret) {
			return XI_FILE_RV_ERR_ARGS;
		}
	} else if (fdesc->type == XG_FD_TYPE_SOCK) {
		rbytes = recv(fdesc->desc.s.fd, (xchar *)buf, blen, 0);
		if ((xint32)rbytes == SOCKET_ERROR) {
			return XI_FILE_RV_ERR_ARGS;
		}
	}

	return rbytes;
}

xssize xi_file_write(xint32 fd, const xvoid *buf, xsize buflen) {
	xbool ret = 0;
	xg_fd_t *fdesc = NULL;
	DWORD wbytes = 0;
	DWORD blen = (DWORD) buflen;

	if (fd < 0 || buf == NULL) {
		return XI_FILE_RV_ERR_ARGS;
	}

	if (buflen == 0) {
		return 0;
	}

	fdesc = xg_fd_get(fd);

	if (fdesc->type == XG_FD_TYPE_FILE || fdesc->type == XG_FD_TYPE_PIPE) {
		ret = WriteFile(fdesc->desc.f.fd, buf, blen, &wbytes, NULL);
		if (!ret) {
			return XI_FILE_RV_ERR_ARGS;
		}
	} else if (fdesc->type == XG_FD_TYPE_SOCK) {
		wbytes = send(fdesc->desc.s.fd, (xchar *)buf, blen, 0);
		if ((xint32)wbytes == SOCKET_ERROR) {
			return XI_FILE_RV_ERR_ARGS;
		}
	}

	return wbytes;
}

xssize xi_file_readv(xint32 fd, const xi_file_iovec_t *iov, xint32 iovlen) {
	xbool ret = 0;
	DWORD rbytes = 0;
	xg_fd_t *fdesc = NULL;
	xint32 i = 0;
	xssize totalRead = 0;

	if (fd < 0 || iov == NULL) {
		return XI_FILE_RV_ERR_ARGS;
	}

	if (iovlen == 0) {
		return 0;
	}

	fdesc = xg_fd_get(fd);

	while (i < iovlen) {
		if (fdesc->type == XG_FD_TYPE_FILE || fdesc->type == XG_FD_TYPE_PIPE) {
			ret = ReadFile(fdesc->desc.f.fd, iov[i].iov_base, iov[i].iov_len,
					&rbytes, NULL);
			if (!ret) {
				break;
			}
		} else if (fdesc->type == XG_FD_TYPE_SOCK) {
			rbytes = recv(fdesc->desc.s.fd, (xchar *)iov[i].iov_base,
					iov[i].iov_len, 0);
			if ((xint32)rbytes == SOCKET_ERROR) {
				break;
			}
		}
		totalRead += rbytes;
		i++;
	}
	return totalRead;
}

xssize xi_file_writev(xint32 fd, const xi_file_iovec_t *iov, xint32 iovlen) {
	xbool ret = 0;
	DWORD wbytes = 0;
	xg_fd_t *fdesc = NULL;
	xint32 i = 0;
	xssize totalWrite = 0;

	if (fd < 0 || iov == NULL) {
		return XI_FILE_RV_ERR_ARGS;
	}

	if (iovlen == 0) {
		return 0;
	}

	fdesc = xg_fd_get(fd);

	while (i < iovlen) {
		if (fdesc->type == XG_FD_TYPE_FILE || fdesc->type == XG_FD_TYPE_PIPE) {
			ret = WriteFile(fdesc->desc.f.fd, iov[i].iov_base, iov[i].iov_len,
					&wbytes, NULL);
			if (!ret) {
				break;
			}
		} else if (fdesc->type == XG_FD_TYPE_SOCK) {
			wbytes = send(fdesc->desc.s.fd, (xchar *)iov[i].iov_base,
					iov[i].iov_len, 0);
			if ((xint32)wbytes == SOCKET_ERROR) {
				break;
			}
		}
		totalWrite += wbytes;
		i++;
	}

	return totalWrite;
}

xint64 xi_file_seek(xint32 fd, xint64 pos, xint32 whence) {
	LARGE_INTEGER ret;

	xint32 method = 0;
	xg_fd_t *fdesc;

	if (fd < 0) {
		return XI_FILE_RV_ERR_ARGS;
	}

	ret.QuadPart = pos;
	switch (whence) {
	case XI_FILE_SEEK_SET:
		method = FILE_BEGIN;
		break;
	case XI_FILE_SEEK_CUR:
		method = FILE_CURRENT;
		break;
	case XI_FILE_SEEK_END:
		method = FILE_END;
		break;
	default:
		return XI_FILE_RV_ERR_ARGS;
	}

	fdesc = xg_fd_get(fd);

	ret.LowPart = SetFilePointer(fdesc->desc.f.fd, ret.LowPart, &ret.HighPart,
			method);
	if (ret.LowPart == INVALID_SET_FILE_POINTER) {
		return XI_FILE_RV_ERR_ARGS;
	}

	return ret.QuadPart;
}

xint32 xi_file_ftruncate(xint32 fd, xoff64 len) {
	xint32 ret = _chsize_s(fd, len);
	if (ret) {
		return XI_FILE_RV_ERR_ARGS;
	}
	return ret;
}

xi_file_re xi_file_lock(xint32 fd, xint64 spos, xint64 len, xint32 bexcl,
		xint32 bwait) {
	DWORD dwFlags = 0;
	OVERLAPPED overlapped;
	const DWORD offsetLow = (DWORD)(spos & 0xFFFFFFFF);
	const DWORD offsetHigh = (DWORD)((spos >> 0x20) & 0x7FFFFFFF);
	const DWORD nNumberOfBytesToLockLow = (DWORD)(len & 0xFFFFFFFF);
	const DWORD nNumberOfBytesToLockHigh = (DWORD)((len >> 0x20) & 0x7FFFFFFF);
	xg_fd_t *fdesc;

	if (fd < 0) {
		return XI_FILE_RV_ERR_ARGS;
	}

	fdesc = xg_fd_get(fd);

	if (!bwait) {
		dwFlags |= LOCKFILE_FAIL_IMMEDIATELY;
	}

	if (bexcl) {
		dwFlags |= LOCKFILE_EXCLUSIVE_LOCK;
	}

	xi_mem_set(&overlapped, 0, sizeof(overlapped));
	overlapped.Offset = offsetLow;
	overlapped.OffsetHigh = offsetHigh;

	if (!LockFileEx(fdesc->desc.f.fd, /* [in] file handle to lock */
	dwFlags, /* [in] flags describing lock type */
	(DWORD) 0, /* [in] reserved */
	nNumberOfBytesToLockLow, /* [in] number of bytes to lock (low) */
	nNumberOfBytesToLockHigh, /* [in] number of bytes to lock (high) */
	(LPOVERLAPPED) & overlapped)) { /* [in] contains file offset to lock start */
		return XI_FILE_RV_ERR_ARGS;
	}

	return XI_FILE_RV_OK;
}

xi_file_re xi_file_unlock(xint32 fd, xint64 spos, xint64 len) {
	OVERLAPPED overlapped;
	const DWORD offsetLow = (DWORD)(spos & 0xFFFFFFFF);
	const DWORD offsetHigh = (DWORD)((spos >> 0x20) & 0x7FFFFFFF);
	const DWORD nNumberOfBytesToUnlockLow = (DWORD)(len & 0xFFFFFFFF);
	const DWORD nNumberOfBytesToUnlockHigh =
			(DWORD)((len >> 0x20) & 0x7FFFFFFF);
	xg_fd_t *fdesc;

	if (fd < 0) {
		return XI_FILE_RV_ERR_ARGS;
	}

	fdesc = xg_fd_get(fd);

	xi_mem_set(&overlapped, 0, sizeof(overlapped));
	overlapped.Offset = offsetLow;
	overlapped.OffsetHigh = offsetHigh;

	if (!UnlockFileEx(fdesc->desc.f.fd, /* [in] file handle to lock */
	(DWORD) 0, /* [in] reserved */
	nNumberOfBytesToUnlockLow, /* [in] number of bytes to lock (low) */
	nNumberOfBytesToUnlockHigh, /* [in] number of bytes to lock (high) */
	(LPOVERLAPPED) & overlapped)) { /* [in] contains file offset to lock start */
		return XI_FILE_RV_ERR_ARGS;
	}

	return XI_FILE_RV_OK;
}

xi_file_re xi_file_sync(xint32 fd) {
	xbool ret;
	xg_fd_t *fdesc;

	if (fd < 0) {
		return XI_FILE_RV_ERR_ARGS;
	}

	fdesc = xg_fd_get(fd);
	if (fdesc->type != XG_FD_TYPE_FILE) {
		return XI_FILE_RV_ERR_ARGS;
	}

	ret = FlushFileBuffers(fdesc->desc.f.fd);
	if (!ret) {
		return XI_FILE_RV_ERR_ARGS;
	}

	return XI_FILE_RV_OK;
}

xi_file_re xi_file_pipe(xint32 fd[2]) {
	HANDLE hr = NULL;
	HANDLE hw = NULL;
	SECURITY_ATTRIBUTES sAttrib;

	xint32 ret = 0;
	xg_fd_t pfd[2];

	UNUSED(fd);

	sAttrib.bInheritHandle = 1;
	sAttrib.nLength = sizeof(sAttrib);

	ret = CreatePipe(&hr, &hw, &sAttrib, 512);
	if (ret == 0) {
		return XI_FILE_RV_ERR_ARGS;
	}

	pfd[0].desc.f.fd = hr;
	pfd[0].type = XG_FD_TYPE_PIPE;
	pfd[0].desc.f.mode = XI_FILE_MODE_READ;
	pfd[0].desc.f.perm = 00400;
	pfd[0].desc.f.path = "PIPE";

	pfd[1].desc.f.fd = hw;
	pfd[1].type = XG_FD_TYPE_PIPE;
	pfd[1].desc.f.mode = XI_FILE_MODE_WRITE;
	pfd[1].desc.f.perm = 00200;
	pfd[1].desc.f.path = "PIPE";

	ret = xg_fd_open(&pfd[0]);
	ret = xg_fd_open(&pfd[1]);

	return XI_FILE_RV_OK;
}

xi_file_re xi_file_close(xint32 fd) {
	xbool ret;
	xg_fd_t *fdesc;

	if (fd < 0) {
		return XI_FILE_RV_ERR_ARGS;
	}

	fdesc = xg_fd_get(fd);

	ret = CloseHandle(fdesc->desc.f.fd);
	if (!ret) {
		return XI_FILE_RV_ERR_ARGS;
	}

	return xg_fd_close(fd);
}

xint32 xi_file_get_stdin() {
	return 0;
}

xint32 xi_file_get_stdout() {
	return 1;
}

xint32 xi_file_get_stderr() {
	return 2;
}

xssize xi_file_readlink(const xchar *pathname, xchar *buf, xsize buflen) {
	UNUSED(pathname);
	UNUSED(buf);
	UNUSED(buflen);
	return -1;
}

xi_file_re xi_file_chmod(const xchar *pathname, xint32 perm) {
	UNUSED(pathname);
	UNUSED(perm);
	return XI_FILE_RV_OK;
}

xi_file_re xi_file_rename(const xchar *frompath, const xchar *topath) {
	xbool ret;

	if (frompath == NULL || topath == NULL) {
		return XI_FILE_RV_ERR_ARGS;
	}

	ret = MoveFile(frompath, topath);
	if (!ret) {
		return XI_FILE_RV_ERR_ARGS;
	}

	return XI_FILE_RV_OK;
}

xi_file_re xi_file_remove(const xchar *pathname) {
	xbool ret;

	if (pathname == NULL) {
		return XI_FILE_RV_ERR_ARGS;
	}

	ret = DeleteFile(pathname);
	if (!ret) {
		return XI_FILE_RV_ERR_ARGS;
	}

	return XI_FILE_RV_OK;
}

xi_file_re xi_file_stat(const xchar* pathname, xi_file_stat_t *s) {
	WIN32_FILE_ATTRIBUTE_DATA se;
	SYSTEMTIME st;
	LARGE_INTEGER fsize;

	xbool ret;
	xi_time_t xtime;
	xlong utc;

	if (pathname == NULL || s == NULL) {
		return XI_FILE_RV_ERR_ARGS;
	}

	ret = GetFileAttributesEx(pathname, GetFileExInfoStandard, &se);
	if (!ret) {
		return XI_FILE_RV_ERR_ARGS;
	}

	if (se.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
		s->type = XI_FILE_TYPE_DIR;
	} else {
		s->type = XI_FILE_TYPE_REG;
	}

	if (se.dwFileAttributes & FILE_ATTRIBUTE_READONLY) {
		s->perm = 00400;
	} else {
		s->perm = 00600;
	}

	fsize.LowPart = se.nFileSizeLow;
	fsize.HighPart = se.nFileSizeHigh;
	s->size = fsize.QuadPart;
	s->blocks = (s->size / 4096L) + 1;

	FileTimeToSystemTime(&se.ftCreationTime, &st);
	xtime.year = st.wYear + 1601;
	xtime.mon = st.wMonth;
	xtime.day = st.wDay;
	xtime.hour = st.wHour;
	xtime.min = st.wMinute;
	xtime.sec = st.wSecond;
	xtime.msec = st.wMilliseconds;
	xi_clock_time2sec(&utc, xtime);
	s->created = (utc * 1000) + xtime.msec;

	FileTimeToSystemTime(&se.ftLastAccessTime, &st);
	xtime.year = st.wYear + 1601;
	xtime.mon = st.wMonth;
	xtime.day = st.wDay;
	xtime.hour = st.wHour;
	xtime.min = st.wMinute;
	xtime.sec = st.wSecond;
	xtime.msec = st.wMilliseconds;
	xi_clock_time2sec(&utc, xtime);
	s->accessed = (utc * 1000) + xtime.msec;

	FileTimeToSystemTime(&se.ftLastWriteTime, &st);
	xtime.year = st.wYear + 1601;
	xtime.mon = st.wMonth;
	xtime.day = st.wDay;
	xtime.hour = st.wHour;
	xtime.min = st.wMinute;
	xtime.sec = st.wSecond;
	xtime.msec = st.wMilliseconds;
	xi_clock_time2sec(&utc, xtime);
	s->modified = (utc * 1000) + xtime.msec;

	xi_strcpy(s->pathname, pathname);
	xi_strcpy(s->filename, xi_pathname_basename(pathname));

	return XI_FILE_RV_OK;
}

xi_file_re xi_file_fstat(xint32 fd, xi_file_stat_t *s) {
	BY_HANDLE_FILE_INFORMATION se;
	SYSTEMTIME st;
	LARGE_INTEGER fsize;

	xbool ret;
	xg_fd_t *fdesc;
	xi_time_t xtime;
	xlong utc;

	if (fd < 0 || s == NULL) {
		return XI_FILE_RV_ERR_ARGS;
	}

	fdesc = xg_fd_get(fd);

	ret = GetFileInformationByHandle(fdesc->desc.f.fd, &se);
	if (!ret) {
		return XI_FILE_RV_ERR_ARGS;
	}

	if (se.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
		s->type = XI_FILE_TYPE_DIR;
	} else {
		s->type = XI_FILE_TYPE_REG;
	}

	if (se.dwFileAttributes & FILE_ATTRIBUTE_READONLY) {
		s->perm = 00400;
	} else {
		s->perm = 00600;
	}

	fsize.LowPart = se.nFileSizeLow;
	fsize.HighPart = se.nFileSizeHigh;
	s->size = fsize.QuadPart;
	s->blocks = (s->size / 4096L) + 1;

	FileTimeToSystemTime(&se.ftCreationTime, &st);
	xtime.year = st.wYear + 1601;
	xtime.mon = st.wMonth;
	xtime.day = st.wDay;
	xtime.hour = st.wHour;
	xtime.min = st.wMinute;
	xtime.sec = st.wSecond;
	xtime.msec = st.wMilliseconds;
	xi_clock_time2sec(&utc, xtime);
	s->created = (utc * 1000) + xtime.msec;

	FileTimeToSystemTime(&se.ftLastAccessTime, &st);
	xtime.year = st.wYear + 1601;
	xtime.mon = st.wMonth;
	xtime.day = st.wDay;
	xtime.hour = st.wHour;
	xtime.min = st.wMinute;
	xtime.sec = st.wSecond;
	xtime.msec = st.wMilliseconds;
	xi_clock_time2sec(&utc, xtime);
	s->accessed = (utc * 1000) + xtime.msec;

	FileTimeToSystemTime(&se.ftLastWriteTime, &st);
	xtime.year = st.wYear + 1601;
	xtime.mon = st.wMonth;
	xtime.day = st.wDay;
	xtime.hour = st.wHour;
	xtime.min = st.wMinute;
	xtime.sec = st.wSecond;
	xtime.msec = st.wMilliseconds;
	xi_clock_time2sec(&utc, xtime);
	s->modified = (utc * 1000) + xtime.msec;

	xi_strcpy(s->pathname, fdesc->desc.f.path);
	xi_strcpy(s->filename, xi_pathname_basename(fdesc->desc.f.path));

	return XI_FILE_RV_OK;
}

xi_file_re xi_file_fsspace(const xchar* pathname, xi_fs_space_t *s) {
	if (!GetDiskFreeSpaceEx(pathname, (PULARGE_INTEGER) & s->avail,
			(PULARGE_INTEGER) & s->total, (PULARGE_INTEGER) & s->free)) {
		return -1;
	}
	return XI_FILE_RV_OK;
}

xi_dir_t *xi_dir_open(const xchar *pathname) {
	WIN32_FIND_DATA ddat;

	xi_dir_t *dfd;
	xchar buf[XCFG_PATHNAME_MAX];

	if (pathname == NULL) {
		return NULL;
	}

	dfd = xi_mem_alloc(sizeof(xi_dir_t));
	if (dfd == NULL) {
		return NULL;
	}

	xi_strcpy(buf, pathname);
	xi_strcat(buf, "/*");

	dfd->dfd = FindFirstFile(buf, &ddat);
	if (dfd->dfd == NULL) {
		xi_mem_free(dfd);
		return NULL;
	}

	xi_strcpy(dfd->pathname, pathname);

	return dfd;
}

xint32 xi_dir_read(xi_dir_t *dfd, xi_file_stat_t *s) {
	WIN32_FIND_DATA ddat;
	SYSTEMTIME st;
	LARGE_INTEGER fsize;

	xchar path[XCFG_PATHNAME_MAX];
	xbool ret;
	xi_time_t xtime;
	xlong utc;

	if (dfd == NULL || s == NULL) {
		return XI_FILE_RV_ERR_ARGS;
	}

	ret = FindNextFile(dfd->dfd, &ddat);
	if (!ret) {
		switch (GetLastError()) {
		case ERROR_NO_MORE_FILES:
			return XI_FILE_RV_EOF;
		default:
			return XI_FILE_RV_ERR_ARGS;
		}
	}

	if (ddat.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
		s->type = XI_FILE_TYPE_DIR;
	} else {
		s->type = XI_FILE_TYPE_REG;
	}

	if (ddat.dwFileAttributes & FILE_ATTRIBUTE_READONLY) {
		s->perm = 00400;
	} else {
		s->perm = 00600;
	}

	fsize.LowPart = ddat.nFileSizeLow;
	fsize.HighPart = ddat.nFileSizeHigh;
	s->size = fsize.QuadPart;
	s->blocks = s->size / 4096;

	FileTimeToSystemTime(&ddat.ftCreationTime, &st);
	xtime.year = st.wYear + 1601;
	xtime.mon = st.wMonth;
	xtime.day = st.wDay;
	xtime.hour = st.wHour;
	xtime.min = st.wMinute;
	xtime.sec = st.wSecond;
	xtime.msec = st.wMilliseconds;
	xi_clock_time2sec(&utc, xtime);
	s->created = (utc * 1000) + xtime.msec;

	FileTimeToSystemTime(&ddat.ftLastAccessTime, &st);
	xtime.year = st.wYear + 1601;
	xtime.mon = st.wMonth;
	xtime.day = st.wDay;
	xtime.hour = st.wHour;
	xtime.min = st.wMinute;
	xtime.sec = st.wSecond;
	xtime.msec = st.wMilliseconds;
	xi_clock_time2sec(&utc, xtime);
	s->accessed = (utc * 1000) + xtime.msec;

	FileTimeToSystemTime(&ddat.ftLastWriteTime, &st);
	xtime.year = st.wYear + 1601;
	xtime.mon = st.wMonth;
	xtime.day = st.wDay;
	xtime.hour = st.wHour;
	xtime.min = st.wMinute;
	xtime.sec = st.wSecond;
	xtime.msec = st.wMilliseconds;
	xi_clock_time2sec(&utc, xtime);
	s->modified = (utc * 1000) + xtime.msec;

	xi_sprintf(path, "%s/%s", dfd->pathname, ddat.cFileName);

	xi_strcpy(s->pathname, path);
	xi_strcpy(s->filename, ddat.cFileName);

	return 1;
}

xi_file_re xi_dir_rewind(xi_dir_t *dfd) {
	WIN32_FIND_DATA ddat;

	if (dfd == NULL) {
		return XI_FILE_RV_ERR_ARGS;
	}

	FindClose(dfd->dfd);
	dfd->dfd = FindFirstFile(dfd->pathname, &ddat);

	return XI_FILE_RV_OK;
}

xi_file_re xi_dir_close(xi_dir_t *dfd) {
	if (dfd == NULL) {
		return XI_FILE_RV_ERR_ARGS;
	}

	FindClose(dfd->dfd);

	return XI_FILE_RV_OK;
}

xi_file_re xi_dir_make(const xchar *pathname, xint32 perm) {
	xbool ret;

	UNUSED(perm);

	if (pathname == NULL) {
		return XI_FILE_RV_ERR_ARGS;
	}

	ret = CreateDirectory(pathname, NULL);
	if (!ret) {
		switch (GetLastError()) {
		case ERROR_ALREADY_EXISTS:
			return XI_FILE_RV_ERR_AE;
		case ERROR_PATH_NOT_FOUND:
		default:
			return XI_FILE_RV_ERR_ARGS;
		}
	}

	return XI_FILE_RV_OK;
}

xi_file_re xi_dir_make_force(const xchar *pathname, xint32 perm) {
	xchar opath[XCFG_PATHNAME_MAX];
	xchar* p;
	xssize len;
	xi_file_re ret;

	UNUSED(perm);

	if (pathname == NULL) {
		return XI_FILE_RV_ERR_ARGS;
	}

	len = xi_strlen(pathname);
	xi_strncpy(opath, pathname, len);
	opath[len] = '\0';

	if (opath[len - 1] == '/') {
		opath[len - 1] = '\0';
	}

	for (p = opath; *p; p++) {
		if (*p == '/') {
			*p = '\0';
			ret = xi_dir_make(opath, perm);
			if (ret != XI_FILE_RV_OK && ret != XI_FILE_RV_ERR_AE) {
				return ret;
			}
			*p = '/';
		}
	}

	ret = xi_dir_make(opath, perm);
	if (ret != XI_FILE_RV_OK && ret != XI_FILE_RV_ERR_AE) {
		return ret;
	}

	return XI_FILE_RV_OK;
}

xi_file_re xi_dir_remove(const xchar *pathname) {
	xbool ret;

	if (pathname == NULL) {
		return XI_FILE_RV_ERR_ARGS;
	}

	ret = RemoveDirectory(pathname);
	if (!ret) {
		switch (GetLastError()) {
		case ERROR_DIR_NOT_EMPTY:
			return XI_FILE_RV_ERR_NOTEMPT;
		case ERROR_PATH_BUSY:
			return XI_FILE_RV_ERR_BUSY;
		default:
			return XI_FILE_RV_ERR_ARGS;
		}
	}

	return XI_FILE_RV_OK;
}

xi_file_re xi_pathname_get(xchar *pathbuf, xuint32 pblen) {
	xchar *buf;

	if (pathbuf == NULL || pblen == 0) {
		return XI_FILE_RV_ERR_ARGS;
	}

	buf = _getcwd(NULL, 0);
	if (buf == NULL) {
		return XI_FILE_RV_ERR_ARGS;
	}

	xi_strcpy(pathbuf, buf);
	free(buf);

	return XI_FILE_RV_OK;
}

xi_file_re xi_pathname_set(const xchar *path) {
	if (_chdir(path)) {
		return XI_FILE_RV_ERR_ARGS;
	}
	return XI_FILE_RV_OK;
}

xi_file_re xi_pathname_absolute(xchar *pathbuf, xuint32 pblen,
		const xchar *pathname) {
	xchar ch;
	xchar *path_name = (char*) pathname;
	xchar *cp = pathbuf;

	xi_mem_set(pathbuf, 0, pblen);

	if (*path_name != '/' && *path_name != '\\' && *(path_name + 1) != ':') {
		xi_pathname_get(pathbuf, pblen);
		while (*cp) {
			cp++;
		}
		*(cp++) = XI_SEP_FILE_C;
	}

	while ((ch = *(path_name++))) {
		switch (ch) {
		/*case '~':{
		 char *p = getenv("HOME");;
		 while (*p)*(cp++) = *(p++);
		 break; }*/
		case '\\':
		case '/':
			*(cp++) = XI_SEP_FILE_C;
			break;
		case '.': {
			switch (*path_name) {
			case '.': // we encounter '..'
				path_name += 2; //skip the following "./"
				if (*(cp - 1) == XI_SEP_FILE_C) //if cano_path now is "/xx/yy/"
					--cp; //skip backward the tail '/'
				*cp = '\0';
				cp = xi_strrchr(pathbuf, XI_SEP_FILE_C); //up a level
				++cp;
				break;
			case '\\':
			case '/':
				++path_name;
				break;
			default:
				*(cp++) = ch;
			}
			break;
		}
		default:
			*(cp++) = ch;
			break;
		}
	}
	*cp = '\0';

	return XI_FILE_RV_OK;
}

xchar *xi_pathname_basename(const xchar *path) {
	xssize pos;

	if (path == NULL) {
		return NULL;
	}

	pos = xi_strlen(path) - 1;
	if (pos < 0) {
		return (xchar*) path;
	}

	while (path[pos] == '/' || path[pos] == '\\') {
		((xchar *) path)[pos] = '\0';
		pos--;
	}
	while (pos >= 0 && path[pos] != '/' && path[pos] != '\\') {
		pos--;
	}

	if (pos > 0) {
		return (xchar *) &path[pos + 1];
	} else {
		return (xchar *) path;
	}
}

xi_file_re xi_pathname_split(xi_pathname_t *pathinfo, const xchar *path) {
	_splitpath(path, pathinfo->drive, pathinfo->dirname, pathinfo->filename,
			pathinfo->suffix);
	return XI_FILE_RV_OK;
}

xi_file_re xi_pathname_merge(xchar *pathbuf, xuint32 pblen,
		xi_pathname_t pathinfo) {
	xchar buf[XCFG_PATHNAME_MAX];

	if (pathbuf == NULL) {
		return XI_FILE_RV_ERR_ARGS;
	}

	if (pathinfo.drive[0] != '\0') {
		xi_sprintf(buf, "%s:%s/%s", pathinfo.drive, pathinfo.dirname,
				pathinfo.basename);
	} else {
		xi_sprintf(buf, "%s/%s", pathinfo.dirname, pathinfo.basename);
	}

	if (xi_strlen(buf) > pblen) {
		return XI_FILE_RV_ERR_OVER;
	}

	xi_strcpy(pathbuf, buf);

	return XI_FILE_RV_OK;
}
