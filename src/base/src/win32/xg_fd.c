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
 * File   : xg_fd.c
 */

#include "xg_fd.h"

#include "xi/xi_log.h"
#include "xi/xi_mem.h"
#include "xi/xi_string.h"

// ----------------------------------------------
// Global Variables
// ----------------------------------------------

static xg_fd_t _g_fds[XCFG_FD_MAX];
static xbool _g_fd_init = FALSE;
static xint32 _g_fd_total = 0;
static xint32 _g_fd_last = 0;

// ----------------------------------------------
// Inner Functions
// ----------------------------------------------

static xvoid xg_fd_init() {
	if (!_g_fd_init) {
		xi_mem_set(_g_fds, 0, sizeof(_g_fds));

		// STDIN
		_g_fds[0].type = XG_FD_TYPE_FILE;
		_g_fds[0].desc.f.fd = GetStdHandle(STD_INPUT_HANDLE);
		_g_fds[0].desc.f.mode = XI_FILE_MODE_READ;
		_g_fds[0].desc.f.perm = 0x400;
		_g_fds[0].desc.f.path = NULL;

		// STDOUT
		_g_fds[1].type = XG_FD_TYPE_FILE;
		_g_fds[1].desc.f.fd = GetStdHandle(STD_OUTPUT_HANDLE);
		_g_fds[1].desc.f.mode = XI_FILE_MODE_WRITE | XI_FILE_MODE_APPEND;
		_g_fds[1].desc.f.perm = 0x200;
		_g_fds[1].desc.f.path = NULL;

		// STDERR
		_g_fds[2].type = XG_FD_TYPE_FILE;
		_g_fds[2].desc.f.fd = GetStdHandle(STD_ERROR_HANDLE);
		_g_fds[2].desc.f.mode = XI_FILE_MODE_WRITE | XI_FILE_MODE_APPEND;
		_g_fds[2].desc.f.perm = 0x200;
		_g_fds[2].desc.f.path = NULL;

		_g_fd_total = 3;
		_g_fd_last = 2;
		_g_fd_init = TRUE;
	}
}

// ----------------------------------------------
// XG Functions
// ----------------------------------------------

xint32 xg_fd_count() {
	return _g_fd_total;
}

xg_fd_t *xg_fd_get(xint32 idx) {
	if (idx < 0 || idx >= XCFG_FD_MAX) {
		log_error(XDLOG, "idx=%d\n", idx);
		return NULL;
	}
	if (!_g_fd_init) {
		xg_fd_init();
	}
	return &(_g_fds[idx]);
}

xint32 xg_fd_open(xg_fd_t *fd) {
	xint32 i;

	if (!_g_fd_init) {
		xg_fd_init();
	}
	if (fd == NULL || (fd->type != XG_FD_TYPE_FILE && fd->type != XG_FD_TYPE_SOCK)) {
		log_error(XDLOG, "fd=%p/fd->type=%d\n", fd, (fd) ? fd->type : 0);
		return -1;
	}

	for (i = _g_fd_last; i < XCFG_FD_MAX; i++) {
		if (_g_fds[i].type == XG_FD_TYPE_NOTUSED) {
			xg_fd_t *dest = &(_g_fds[i]);
			xi_mem_copy(dest, fd, sizeof(xg_fd_t));
			if (fd->type == XG_FD_TYPE_FILE && fd->desc.f.path != NULL) {
				dest->desc.f.path = (xchar *)xi_mem_alloc(xi_strlen(fd->desc.f.path)+1);
				xi_strcpy(dest->desc.f.path, fd->desc.f.path);
			}
			_g_fd_last = i;
			_g_fd_total++;
			if (_g_fd_total > (XCFG_FD_MAX - 10)) {
				log_warn(XDLOG, "FD-Total is very close to Limit(4096)\n");
			}
			return i;
		}
	}

	if (_g_fd_total < XCFG_FD_MAX) {
		_g_fd_last = 0;
	}

	for (i = _g_fd_last; i < XCFG_FD_MAX; i++) {
		if (_g_fds[i].type == XG_FD_TYPE_NOTUSED) {
			xg_fd_t *dest = &(_g_fds[i]);
			xi_mem_copy(dest, fd, sizeof(xg_fd_t));
			if (fd->type == XG_FD_TYPE_FILE && fd->desc.f.path != NULL) {
				dest->desc.f.path = (xchar *)xi_mem_alloc(xi_strlen(fd->desc.f.path)+1);
				xi_strcpy(dest->desc.f.path, fd->desc.f.path);
			}
			_g_fd_last = i;
			_g_fd_total++;
			if (_g_fd_total > (XCFG_FD_MAX - 10)) {
				log_warn(XDLOG, "FD-Total is very close to Limit(4096)\n");
			}
			return i;
		}
	}

	log_error(XDLOG, "Cannot find free-fd!!!\n");
	return -1;
}

xint32 xg_fd_close(xint32 idx) {
	if (idx < 0 || idx >= XCFG_FD_MAX) {
		log_error(XDLOG, "idx=%d\n", idx);
		return -1;
	}

	if (_g_fds[idx].type == XG_FD_TYPE_FILE && _g_fds[idx].desc.f.path != NULL) {
		xi_mem_free(_g_fds[idx].desc.f.path);
		_g_fds[idx].desc.f.path = NULL;
	}
	xi_mem_set(&(_g_fds[idx]), 0, sizeof(xg_fd_t));
	_g_fd_total--;

	return 0;
}
