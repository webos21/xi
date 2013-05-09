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

// Defines for O_LARGEFILE and O_DIRECT
#define _GNU_SOURCE
#define _LARGEFILE_SOURCE
#ifndef _LARGEFILE64_SOURCE
#define _LARGEFILE64_SOURCE
#endif
#ifndef _FILE_OFFSET_BITS
#define _FILE_OFFSET_BITS	64
#endif

#include <stdio.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <time.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/uio.h>

#ifdef __APPLE__
#include <sys/mount.h>
#else
#include <sys/vfs.h>
#endif

#include "xg_fd.h"

#include "xi/xi_mem.h"
#include "xi/xi_string.h"

// ----------------------------------------------
// Inner Structure
// ----------------------------------------------

struct _xi_dir {
	DIR *dfd;
	xchar pathname[XCFG_PATHNAME_MAX];
};

// ----------------------------------------------
// XI Functions
// ----------------------------------------------

xint32 xi_file_open(const xchar *pathname, xint32 mode, xint32 perm) {
	xint32 rflag = 0;
	xint32 fd;
	xg_fd_t pfd;

	if (pathname == NULL) {
		return XI_FILE_RV_ERR_ARGS;
	}

	if ((mode & XI_FILE_MODE_READ) && (mode & XI_FILE_MODE_WRITE)) {
		rflag |= O_RDWR;
	} else {
		if (mode & XI_FILE_MODE_READ) {
			rflag |= O_RDONLY;
		}
		if (mode & XI_FILE_MODE_WRITE) {
			rflag |= O_WRONLY;
		}
	}
	if (mode & XI_FILE_MODE_CREATE) {
		rflag |= O_CREAT;
		if (mode & XI_FILE_MODE_EXCL) {
			rflag |= O_EXCL;
		}
	}
	if ((mode & XI_FILE_MODE_EXCL) && !(mode & XI_FILE_MODE_CREATE)) {
		return XI_FILE_RV_ERR_ARGS;
	}
	if (mode & XI_FILE_MODE_APPEND) {
		rflag |= O_APPEND;
	}
	if (mode & XI_FILE_MODE_TRUNCATE) {
		rflag |= O_TRUNC;
	}
	if (mode & XI_FILE_MODE_DIRECT) {
#ifdef O_DIRECT
		// not POSIX compliant, only valid since linux 2.4.10
		rflag |= O_DIRECT;
#endif
	}
	if (mode & XI_FILE_MODE_LARGEFILE) {
#ifdef O_LARGEFILE
		rflag |= O_LARGEFILE;
#endif
	}
	if (mode & XI_FILE_MODE_NONBLOCK) {
		rflag |= O_NONBLOCK;
	}

	fd = open(pathname, rflag, perm);
	if (fd < 0) {
		switch (errno) {
		case EEXIST:
			return XI_FILE_RV_ERR_AE;
		case EPERM:
			return XI_FILE_RV_ERR_PERM;
		case EACCES:
		case EFAULT:
		default:
			return XI_FILE_RV_ERR_ARGS;
		}
	}

	pfd.type = XG_FD_TYPE_FILE;
	pfd.desc.f.mode = mode;
	pfd.desc.f.perm = perm;
	pfd.desc.f.path = (xchar *) pathname;

	return xg_fd_open(fd, &pfd);
}

xint32 xi_file_rpeek(xint32 fd) {
	xint32 ret;
	xint32 avail = 0;

	if (fd < 0) {
		return XI_FILE_RV_ERR_ARGS;
	}

	ret = ioctl(fd, FIONREAD, &avail);
	if (ret < 0) {
		if (errno == ENOTTY) {
			return -2;
		}
		return -1;
	}
	if (avail < 0) {
		return 0;
	}

	return avail;
}

xssize xi_file_read(xint32 fd, xvoid *buf, xsize buflen) {
	xssize ret;

	if (fd < 0 || buf == NULL) {
		return XI_FILE_RV_ERR_ARGS;
	}

	if (buflen == 0) {
		return 0;
	}

	ret = read(fd, buf, buflen);
	if (ret < 0) {
		switch (errno) {
		case EISDIR:
			return XI_FILE_RV_ERR_DIR;
		case EBADF:
			return XI_FILE_RV_ERR_FD;
		case EINTR:
			return XI_FILE_RV_ERR_INTR;
		case EIO:
			return XI_FILE_RV_ERR_IO;
		default:
			return XI_FILE_RV_ERR_ARGS;
		}
	}

	return ret;
}

xssize xi_file_write(xint32 fd, const xvoid *buf, xsize buflen) {
	xssize ret;

	if (fd < 0 || buf == NULL) {
		return XI_FILE_RV_ERR_ARGS;
	}

	if (buflen == 0) {
		return 0;
	}

	ret = write(fd, buf, buflen);
	if (ret < 0) {
		switch (errno) {
		case EISDIR:
			return XI_FILE_RV_ERR_DIR;
		case EBADF:
			return XI_FILE_RV_ERR_FD;
		case EINTR:
			return XI_FILE_RV_ERR_INTR;
		case EFBIG:
			return XI_FILE_RV_ERR_BIG;
		case EIO:
			return XI_FILE_RV_ERR_IO;
		default:
			return XI_FILE_RV_ERR_ARGS;
		}
	}

	return ret;
}

xssize xi_file_readv(xint32 fd, const xi_file_iovec_t *iov, xint32 iovlen) {
	xssize ret = 0;

	ret = readv(fd, (const struct iovec *) iov, iovlen);
	if (ret < 0) {
		return XI_FILE_RV_ERR_ARGS;
	}

	return ret;
}

xssize xi_file_writev(xint32 fd, const xi_file_iovec_t *iov, xint32 iovlen) {
	xssize ret = 0;

	ret = writev(fd, (const struct iovec *) iov, iovlen);
	if (ret < 0) {
		return XI_FILE_RV_ERR_ARGS;
	}

	return ret;
}

xoff64 xi_file_seek(xint32 fd, xoff64 pos, xint32 whence) {
	xoff64 ret;

	if (fd < 0 || (whence != XI_FILE_SEEK_SET && whence != XI_FILE_SEEK_CUR
			&& whence != XI_FILE_SEEK_END)) {
		return XI_FILE_RV_ERR_ARGS;
	}

#ifdef __APPLE__
	// In case of BSD, if the offset_t length of [lseek] is 64bit,
	// then it automatically process such as [lseek64].
	ret = lseek(fd, pos, whence);
#else
	ret = lseek64(fd, pos, whence);
#endif // __APPLE__
	if (ret < 0) {
		switch (errno) {
		case EBADF:
			return XI_FILE_RV_ERR_FD;
		case EOVERFLOW:
			return XI_FILE_RV_ERR_OVER;
		default:
			return XI_FILE_RV_ERR_ARGS;
		}
	}

	return ret;
}

xint32 xi_file_ftruncate(xint32 fd, xoff64 len) {
	xint32 ret = ftruncate(fd, len);
	if (ret < 0) {
		return XI_FILE_RV_ERR_ARGS;
	}
	return ret;
}

xi_file_re xi_file_lock(xint32 fd, xoff64 spos, xoff64 len, xint32 bexcl,
		xint32 bwait) {
	struct flock lock;
	xint32 waitMode = 0;

	xint32 ret = 0;

	xi_mem_set(&lock, 0, sizeof(lock));

	lock.l_whence = SEEK_SET;
	lock.l_start = spos;
	lock.l_len = len;

	if (bexcl) {
		lock.l_type = F_WRLCK;
	} else {
		lock.l_type = F_RDLCK;
	}

	waitMode = (bwait) ? F_SETLKW : F_SETLK;

	ret = fcntl(fd, waitMode, &lock);
	if (ret < 0) {
		return XI_FILE_RV_ERR_ARGS;
	}

	return XI_FILE_RV_OK;
}

xi_file_re xi_file_unlock(xint32 fd, xoff64 spos, xoff64 len) {
	struct flock lock;

	xint32 ret = 0;

	xi_mem_set(&lock, 0, sizeof(lock));

	lock.l_whence = SEEK_SET;
	lock.l_start = spos;
	lock.l_len = len;
	lock.l_type = F_UNLCK;

	ret = fcntl(fd, F_SETLKW, &lock);
	if (ret < 0) {
		return XI_FILE_RV_ERR_ARGS;
	}

	return XI_FILE_RV_OK;
}

xi_file_re xi_file_sync(xint32 fd) {
	xint32 ret;
	xg_fd_t *fdesc;

	if (fd < 0) {
		return XI_FILE_RV_ERR_ARGS;
	}

	fdesc = xg_fd_get(fd);
	if (fdesc->type != XG_FD_TYPE_FILE) {
		return XI_FILE_RV_ERR_ARGS;
	}

	ret = fsync(fd);
	if (ret < 0) {
		switch (errno) {
		case EBADF:
			return XI_FILE_RV_ERR_FD;
		case EIO:
			return XI_FILE_RV_ERR_IO;
		case EROFS:
		case EINVAL:
			return XI_FILE_RV_ERR_NF;
		default:
			return XI_FILE_RV_ERR_ARGS;
		}
	}

	return XI_FILE_RV_OK;
}

xi_file_re xi_file_pipe(xint32 fd[2]) {
	xint32 ret;
	xg_fd_t pfd[2];

	if (fd == NULL) {
		return XI_FILE_RV_ERR_ARGS;
	}

	ret = pipe(fd);
	if (ret < 0) {
		switch (errno) {
		case EFAULT:
			return XI_FILE_RV_ERR_FD;
		case EMFILE:
			return XI_FILE_RV_ERR_NOMEM;
		default:
			return XI_FILE_RV_ERR_ARGS;
		}
	}

	pfd[0].type = XG_FD_TYPE_PIPE;
	pfd[0].desc.f.mode = XI_FILE_MODE_READ;
	pfd[0].desc.f.perm = 00400;
	pfd[0].desc.f.path = "PIPE";

	pfd[1].type = XG_FD_TYPE_PIPE;
	pfd[1].desc.f.mode = XI_FILE_MODE_WRITE;
	pfd[1].desc.f.perm = 00200;
	pfd[1].desc.f.path = "PIPE";

	ret = xg_fd_open(fd[0], &pfd[0]);
	ret = xg_fd_open(fd[1], &pfd[1]);

	return XI_FILE_RV_OK;
}

xi_file_re xi_file_close(xint32 fd) {
	xint32 ret;

	if (fd < 0) {
		return XI_FILE_RV_ERR_ARGS;
	}

	ret = close(fd);
	if (ret < 0) {
		switch (errno) {
		case EBADF:
			return XI_FILE_RV_ERR_FD;
		case EINTR:
			return XI_FILE_RV_ERR_INTR;
		case EIO:
			return XI_FILE_RV_ERR_IO;
		default:
			return XI_FILE_RV_ERR_ARGS;
		}
	}

	return xg_fd_close(fd);
}

xint32 xi_file_get_stdin() {
	return STDIN_FILENO;
}

xint32 xi_file_get_stdout() {
	return STDOUT_FILENO;
}

xint32 xi_file_get_stderr() {
	return STDERR_FILENO;
}

xssize xi_file_readlink(const xchar *pathname, xchar *buf, xsize buflen) {
	xssize ret;

	ret = readlink(pathname, buf, buflen);
	if (ret < 0) {
		switch (errno) {
		case ENOENT:
			return XI_FILE_RV_ERR_NF;
		case EACCES:
		case EPERM:
		case EROFS:
			return XI_FILE_RV_ERR_PERM;
		case EIO:
			return XI_FILE_RV_ERR_IO;
		case ENOTDIR:
			return XI_FILE_RV_ERR_NOTDIR;
		case ENOMEM:
			return XI_FILE_RV_ERR_NOMEM;
		case EBUSY:
			return XI_FILE_RV_ERR_BUSY;
		default:
			return XI_FILE_RV_ERR_ARGS;
		}
	}

	return ret;
}

xi_file_re xi_file_chmod(const xchar *pathname, xint32 perm) {
	xint32 ret;

	if (pathname == NULL) {
		return XI_FILE_RV_ERR_ARGS;
	}

	ret = chmod(pathname, (mode_t)perm);
	if (ret < 0) {
		switch (errno) {
		case ENOENT:
			return XI_FILE_RV_ERR_NF;
		case EACCES:
		case EPERM:
		case EROFS:
			return XI_FILE_RV_ERR_PERM;
		case EIO:
			return XI_FILE_RV_ERR_IO;
		case ENOTDIR:
			return XI_FILE_RV_ERR_NOTDIR;
		case ENOMEM:
			return XI_FILE_RV_ERR_NOMEM;
		case EBUSY:
			return XI_FILE_RV_ERR_BUSY;
		default:
			return XI_FILE_RV_ERR_ARGS;
		}
	}

	return XI_FILE_RV_OK;
}

xi_file_re xi_file_rename(const xchar *frompath, const xchar *topath) {
	xint32 ret;

	if (frompath == NULL || topath == NULL) {
		return XI_FILE_RV_ERR_ARGS;
	}

	ret = rename(frompath, topath);
	if (ret < 0) {
		switch (errno) {
		case ENOENT:
			return XI_FILE_RV_ERR_NF;
		case EACCES:
		case EPERM:
		case EROFS:
			return XI_FILE_RV_ERR_PERM;
		case EISDIR:
			return XI_FILE_RV_ERR_DIR;
		case ENOTDIR:
			return XI_FILE_RV_ERR_NOTDIR;
		case ENOMEM:
			return XI_FILE_RV_ERR_NOMEM;
		case EBUSY:
			return XI_FILE_RV_ERR_BUSY;
		case ENOTEMPTY:
		case EEXIST:
			return XI_FILE_RV_ERR_AE;
		default:
			return XI_FILE_RV_ERR_ARGS;
		}
	}

	return XI_FILE_RV_OK;
}

xi_file_re xi_file_remove(const xchar *pathname) {
	xint32 ret;

	if (pathname == NULL) {
		return XI_FILE_RV_ERR_ARGS;
	}

	ret = unlink(pathname);
	if (ret < 0) {
		switch (errno) {
		case ENOENT:
			return XI_FILE_RV_ERR_NF;
		case EACCES:
		case EPERM:
		case EROFS:
			return XI_FILE_RV_ERR_PERM;
		case EISDIR:
			return XI_FILE_RV_ERR_DIR;
		case ENOMEM:
			return XI_FILE_RV_ERR_NOMEM;
		case EBUSY:
			return XI_FILE_RV_ERR_BUSY;
		default:
			return XI_FILE_RV_ERR_ARGS;
		}
	}

	return XI_FILE_RV_OK;
}

xi_file_re xi_file_stat(const xchar* pathname, xi_file_stat_t *s) {
	struct stat se;

	xint32 masked_value;
	xint32 ret;

	if (pathname == NULL || s == NULL) {
		return XI_FILE_RV_ERR_ARGS;
	}

	ret = stat(pathname, &se);
	if (ret < 0) {
		switch (errno) {
		case ENOENT:
			return XI_FILE_RV_ERR_NF;
		case EBADF:
			return XI_FILE_RV_ERR_FD;
		case ENOTDIR:
			return XI_FILE_RV_ERR_NOTDIR;
		case EACCES:
			return XI_FILE_RV_ERR_PERM;
		case ENOMEM:
			return XI_FILE_RV_ERR_NOMEM;
		default:
			return XI_FILE_RV_ERR_ARGS;
		}
	}

	masked_value = se.st_mode & S_IFMT;
	switch (masked_value) {
	case S_IFSOCK:
		s->type = XI_FILE_TYPE_SOCK;
		break;
	case S_IFLNK:
		s->type = XI_FILE_TYPE_LNK;
		break;
	case S_IFREG:
		s->type = XI_FILE_TYPE_REG;
		break;
	case S_IFBLK:
		s->type = XI_FILE_TYPE_BLK;
		break;
	case S_IFDIR:
		s->type = XI_FILE_TYPE_DIR;
		break;
	case S_IFCHR:
		s->type = XI_FILE_TYPE_CHR;
		break;
	default:
		s->type = XI_FILE_TYPE_UNKNOWN;
		break;
	}

	s->perm = se.st_mode & 07777;
	s->size = se.st_size;
	s->blocks = se.st_blocks;

	s->created = (se.st_ctime * 1000);
	s->accessed = (se.st_atime * 1000);
	s->modified = (se.st_mtime * 1000);

	xi_strcpy(s->pathname, pathname);
	xi_strcpy(s->filename, xi_pathname_basename(pathname));

	return XI_FILE_RV_OK;
}

xi_file_re xi_file_fstat(xint32 fd, xi_file_stat_t *s) {
	struct stat se;

	xint32 masked_value;
	xint32 ret;
	xg_fd_t *fdesc = NULL;

	if (fd < 0 || s == NULL) {
		return XI_FILE_RV_ERR_ARGS;
	}

	ret = fstat(fd, &se);
	if (ret < 0) {
		switch (errno) {
		case ENOENT:
			return XI_FILE_RV_ERR_NF;
		case EBADF:
			return XI_FILE_RV_ERR_FD;
		case ENOTDIR:
			return XI_FILE_RV_ERR_NOTDIR;
		case EACCES:
			return XI_FILE_RV_ERR_PERM;
		case ENOMEM:
			return XI_FILE_RV_ERR_NOMEM;
		default:
			return XI_FILE_RV_ERR_ARGS;
		}
	}

	masked_value = se.st_mode & S_IFMT;
	switch (masked_value) {
	case S_IFSOCK:
		s->type = XI_FILE_TYPE_SOCK;
		break;
	case S_IFLNK:
		s->type = XI_FILE_TYPE_LNK;
		break;
	case S_IFREG:
		s->type = XI_FILE_TYPE_REG;
		break;
	case S_IFBLK:
		s->type = XI_FILE_TYPE_BLK;
		break;
	case S_IFDIR:
		s->type = XI_FILE_TYPE_DIR;
		break;
	case S_IFCHR:
		s->type = XI_FILE_TYPE_CHR;
		break;
	default:
		s->type = XI_FILE_TYPE_UNKNOWN;
		break;
	}

	s->perm = se.st_mode & 07777;
	s->size = se.st_size;
	s->blocks = se.st_blocks;

	s->created = (se.st_ctime * 1000);
	s->accessed = (se.st_atime * 1000);
	s->modified = (se.st_mtime * 1000);

	fdesc = xg_fd_get(fd);

	xi_strcpy(s->pathname, fdesc->desc.f.path);
	xi_strcpy(s->filename, xi_pathname_basename(fdesc->desc.f.path));

	return XI_FILE_RV_OK;
}

xi_file_re xi_file_fsspace(const xchar* pathname, xi_fs_space_t *s) {
	xint32 ret;
	struct statfs se;

	ret = statfs(pathname, &se);
	if (ret < 0) {
		switch (errno) {
		case ENOENT:
			return XI_FILE_RV_ERR_NF;
		case EBADF:
			return XI_FILE_RV_ERR_FD;
		case ENOTDIR:
			return XI_FILE_RV_ERR_NOTDIR;
		case EACCES:
			return XI_FILE_RV_ERR_PERM;
		case ENOMEM:
			return XI_FILE_RV_ERR_NOMEM;
		default:
			return XI_FILE_RV_ERR_ARGS;
		}
	}

	s->total = (xint64)(se.f_blocks * (xulong)se.f_bsize);
	s->avail = (xint64)(se.f_bavail * (xulong)se.f_bsize);
	s->free = (xint64)(se.f_bfree * (xulong)se.f_bsize);

	return XI_FILE_RV_OK;
}

xi_dir_t *xi_dir_open(const xchar *pathname) {
	xi_dir_t *dfd;

	if (pathname == NULL) {
		return NULL;
	}

	dfd = xi_mem_calloc(1, sizeof(xi_dir_t));
	if (dfd == NULL) {
		return NULL;
	}

	dfd->dfd = opendir(pathname);
	if (dfd->dfd == NULL) {
		xi_mem_free(dfd);
		return NULL;
	}

	xi_strcpy(dfd->pathname, pathname);

	return dfd;
}

xint32 xi_dir_read(xi_dir_t *dfd, xi_file_stat_t *s) {
	struct dirent *de;

	xchar path[XCFG_PATHNAME_MAX];
	xi_file_re ret;

	if (dfd == NULL || s == NULL) {
		return XI_FILE_RV_ERR_ARGS;
	}

	de = readdir(dfd->dfd);
	if (de == NULL) {
		switch (errno) {
		case EBADF:
			return XI_FILE_RV_ERR_FD;
		default:
			return XI_FILE_RV_EOF;
		}
	}

	sprintf(path, "%s/%s", dfd->pathname, de->d_name);

	ret = xi_file_stat(path, s);

	return ((ret == XI_FILE_RV_OK) ? 1 : ret);
}

xi_file_re xi_dir_rewind(xi_dir_t *dfd) {
	if (dfd == NULL) {
		return XI_FILE_RV_ERR_ARGS;
	}

	rewinddir(dfd->dfd);

	return XI_FILE_RV_OK;
}

xi_file_re xi_dir_close(xi_dir_t *dfd) {
	xint32 ret;

	if (dfd == NULL) {
		return XI_FILE_RV_ERR_ARGS;
	}

	ret = closedir(dfd->dfd);
	if (ret < 0) {
		switch (errno) {
		case EBADF:
			return XI_FILE_RV_ERR_FD;
		default:
			return XI_FILE_RV_ERR_ARGS;
		}
	}

	return XI_FILE_RV_OK;
}

xi_file_re xi_dir_make(const xchar *pathname, xint32 perm) {
	xint32 ret;

	if (pathname == NULL) {
		return XI_FILE_RV_ERR_ARGS;
	}

	ret = mkdir(pathname, (mode_t)perm);
	if (ret < 0) {
		switch (errno) {
		case ENOENT:
			return XI_FILE_RV_ERR_NF;
		case EROFS:
		case EACCES:
		case EPERM:
			return XI_FILE_RV_ERR_PERM;
		case EEXIST:
			return XI_FILE_RV_ERR_AE;
		case ENOTDIR:
			return XI_FILE_RV_ERR_NOTDIR;
		case ENOSPC:
			return XI_FILE_RV_ERR_MAX;
		default:
			return XI_FILE_RV_ERR_ARGS;
		}
	}

	return XI_FILE_RV_OK;
}

xi_file_re xi_dir_make_force(const xchar *pathname, xint32 perm) {
	xchar opath[XCFG_PATHNAME_MAX];
	xchar* p;
	xint32 ret;
	xsize len;

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
	xint32 ret;

	if (pathname == NULL) {
		return XI_FILE_RV_ERR_ARGS;
	}

	ret = rmdir(pathname);
	if (ret < 0) {
		switch (errno) {
		case EACCES:
		case EPERM:
		case EROFS:
			return XI_FILE_RV_ERR_PERM;
		case EISDIR:
		case ENOTDIR:
			return XI_FILE_RV_ERR_NOTDIR;
		case ENOMEM:
			return XI_FILE_RV_ERR_NOMEM;
		case EBUSY:
			return XI_FILE_RV_ERR_BUSY;
		case ENOTEMPTY:
			return XI_FILE_RV_ERR_NOTEMPT;
		default:
			return XI_FILE_RV_ERR_ARGS;
		}
	}

	return XI_FILE_RV_OK;
}

xi_file_re xi_pathname_get(xchar *pathbuf, xuint32 pblen) {
	xchar *ret = getcwd(pathbuf, pblen);
	if (ret == NULL) {
		switch (errno) {
		case EACCES:
		case EFAULT:
			return XI_FILE_RV_ERR_PERM;
		case ERANGE:
			return XI_FILE_RV_ERR_OVER;
		case ENOENT:
			return XI_FILE_RV_ERR_NF;
		default:
			return XI_FILE_RV_ERR_ARGS;
		}
	}
	return XI_FILE_RV_OK;
}

xi_file_re xi_pathname_set(const xchar *path) {
	xint32 ret = chdir(path);
	if (ret < 0) {
		switch (errno) {
		case EACCES:
			return XI_FILE_RV_ERR_PERM;
		case ENOTDIR:
			return XI_FILE_RV_ERR_NOTDIR;
		default:
			return XI_FILE_RV_ERR_ARGS;
		}
	}
	return XI_FILE_RV_OK;
}

#define PORT_FILE_SEPARATOR '/'

xi_file_re xi_pathname_absolute(xchar *pathbuf, xuint32 pblen,
		const xchar *pathname) {

	xchar ch;
	xchar *path_name = (char*) pathname;
	xchar *cp = pathbuf;

	xi_mem_set(pathbuf, 0, pblen);

	if (*path_name != '/') {
		xi_pathname_get(pathbuf, pblen);
		while (*cp) {
			cp++;
		}
		*(cp++) = PORT_FILE_SEPARATOR;
	}

	while ((ch = *(path_name++))) {
		switch (ch) {
		/*case '~':{
		 char *p = getenv("HOME");;
		 while (*p)*(cp++) = *(p++);
		 break; }*/
		case '\\':
		case '/':
			*(cp++) = PORT_FILE_SEPARATOR;
			break;
		case '.': {
			switch (*path_name) {
			case '.': // we encounter '..'
				path_name += 2; //skip the following "./"
				if (*(cp - 1) == PORT_FILE_SEPARATOR) //if cano_path now is "/xx/yy/"
					--cp; //skip backward the tail '/'
				*cp = '\0';
				cp = xi_strrchr(pathbuf, PORT_FILE_SEPARATOR); //up a level
				++cp;
				break;
			case '\\':
			case '/':
				++path_name;
				break;
			default:
				*(cp++) = ch;
				break;
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

	pos = ((xssize)xi_strlen(path)) - 1;
	if (pos < 0) {
		return (xchar*) path;
	}

	while (path[pos] == '/') {
		((xchar *) path)[pos] = '\0';
		pos--;
	}
	while (pos >= 0 && path[pos] != '/') {
		pos--;
	}

	if (pos > 0) {
		return (xchar *) &path[pos + 1];
	} else {
		return (xchar *) path;
	}
}

xi_file_re xi_pathname_split(xi_pathname_t *pathinfo, const xchar *path) {
	xssize cpos, epos, dpos, spos;

	if (path == NULL || pathinfo == NULL) {
		return XI_FILE_RV_ERR_ARGS;
	}

	cpos = 0;
	if (path[1] == ':') {
		pathinfo->drive[0] = path[0];
		pathinfo->drive[1] = '\0';
		cpos = 2;
	} else {
		pathinfo->drive[0] = '\0';
	}

	epos = ((xssize)xi_strlen(path)) - 1;
	while (path[epos] == '/') {
		epos--;
	}

	dpos = epos;
	while (dpos > cpos && path[dpos] != '.') {
		dpos--;
	}

	spos = epos;
	while (spos >= cpos && path[spos] != '/') {
		spos--;
	}

	xi_strncpy(pathinfo->dirname, &path[cpos], (xsize)(spos - cpos));
	pathinfo->dirname[spos - cpos] = '\0';
	xi_strncpy(pathinfo->basename, &path[spos + 1], (xsize)(epos - spos));
	pathinfo->basename[epos - spos] = '\0';
	if (dpos > cpos) {
		xi_strncpy(pathinfo->filename, &path[spos + 1], (xsize)(dpos - spos - 1));
		pathinfo->filename[dpos - spos - 1] = '\0';
		xi_strncpy(pathinfo->suffix, &path[dpos + 1], (xsize)(epos - dpos));
		pathinfo->suffix[epos - dpos] = '\0';
	} else {
		xi_strncpy(pathinfo->filename, &path[spos + 1], (xsize)(epos - spos - 1));
		pathinfo->filename[dpos - spos - 1] = '\0';
		pathinfo->suffix[0] = '\0';
	}

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
