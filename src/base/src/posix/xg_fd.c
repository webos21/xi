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
 * File : xg_fd.c
 */

#include "xg_fd.h"

#include "xi/xi_mem.h"
#include "xi/xi_log.h"
#include "xi/xi_string.h"

// ----------------------------------------------
// Global Variables
// ----------------------------------------------

static xg_fd_t _g_fds[XCFG_FD_MAX];
static xbool _g_fd_init = FALSE;
static xint32 _g_fd_total = 0;

// ----------------------------------------------
// Inner Functions
// ----------------------------------------------

static xvoid xg_fd_init() {
	if (!_g_fd_init) {
		xi_mem_set(_g_fds, 0, sizeof(_g_fds));

		// STDIN
		_g_fds[0].type = XG_FD_TYPE_FILE;
		_g_fds[0].desc.f.mode = XI_FILE_MODE_READ;
		_g_fds[0].desc.f.perm = 0x400;
		_g_fds[0].desc.f.path = NULL;

		// STDOUT
		_g_fds[1].type = XG_FD_TYPE_FILE;
		_g_fds[1].desc.f.mode = XI_FILE_MODE_WRITE | XI_FILE_MODE_APPEND;
		_g_fds[1].desc.f.perm = 0x200;
		_g_fds[1].desc.f.path = NULL;

		// STDERR
		_g_fds[2].type = XG_FD_TYPE_FILE;
		_g_fds[2].desc.f.mode = XI_FILE_MODE_WRITE | XI_FILE_MODE_APPEND;
		_g_fds[2].desc.f.perm = 0x200;
		_g_fds[2].desc.f.path = NULL;

		_g_fd_total = 3;
		_g_fd_init = TRUE;
	}
}

// ----------------------------------------------
// XG Interface Functions
// ----------------------------------------------

_XI_API_INTERN xint32 xg_fd_count() {
	return _g_fd_total;
}

_XI_API_INTERN xg_fd_t * xg_fd_get(xint32 idx) {
	if (idx < 0 || idx >= XCFG_FD_MAX) {
		log_error(XDLOG, "idx=%d\n", idx);
		return NULL;
	}
	if (!_g_fd_init) {
		xg_fd_init();
	}
	return &(_g_fds[idx]);
}

_XI_API_INTERN xint32 xg_fd_open(xint32 idx, xg_fd_t *fd) {
	if (!_g_fd_init) {
		xg_fd_init();
	}
	if (idx < 0 || idx >= XCFG_FD_MAX || fd == NULL || (fd->type
			!= XG_FD_TYPE_FILE && fd->type != XG_FD_TYPE_SOCK)) {
		log_error(XDLOG, "idx=%d/fd=%p/fd->type=%d\n", idx, fd, (fd) ? fd->type : 0);
		return -1;
	}
	xi_mem_copy(&(_g_fds[idx]), fd, sizeof(xg_fd_t));
	if (fd->type == XG_FD_TYPE_FILE && fd->desc.f.path != NULL) {
		_g_fds[idx].desc.f.path = xi_mem_alloc(xi_strlen(fd->desc.f.path)+1);
		xi_strcpy(_g_fds[idx].desc.f.path, fd->desc.f.path);
	}
	_g_fd_total++;
	if (_g_fd_total > (XCFG_FD_MAX - 10)) {
		log_warn(XDLOG, "FD-Total is very close to Limit(4096)\n");
	}

	return idx;
}

_XI_API_INTERN xint32 xg_fd_close(xint32 idx) {
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
