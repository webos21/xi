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
 * File   : xi_select.c
 */

#include <sys/select.h>
#include <errno.h>

#include "xi/xi_select.h"

#include "xi/xi_mem.h"

// ----------------------------------------------
// Inner Structure
// ----------------------------------------------

struct _xi_fdset {
	fd_set fdset;
};

// ----------------------------------------------
// XI Functions
// ----------------------------------------------

xi_fdset_t *xi_sel_fdcreate() {
	return xi_mem_alloc(sizeof(struct _xi_fdset));
}

xvoid xi_sel_fdzero(xi_fdset_t *fdset) {
	if (fdset == NULL) {
		return;
	} else {
		fd_set *t = &fdset->fdset;
		FD_ZERO(t);
	}
}

xvoid xi_sel_fdclr(xint32 fd, xi_fdset_t *fdset) {
	if (fdset == NULL) {
		return;
	} else {
		fd_set *t = &fdset->fdset;
		FD_CLR((xuint32)fd, t);
	}
}

xint32 xi_sel_fdisset(xint32 fd, xi_fdset_t *fdset) {
	if (fdset == NULL) {
		return XI_SELECT_RV_ERR_ARGS;
	} else {
		fd_set *t = &fdset->fdset;
		return FD_ISSET((xuint32)fd, t);
	}
}

xvoid xi_sel_fdset(xint32 fd, xi_fdset_t *fdset) {
	if (fdset == NULL) {
		return;
	} else {
		fd_set *t = &fdset->fdset;
		FD_SET((xuint32)fd, t);
	}
}

xvoid xi_sel_fddestroy(xi_fdset_t *fdset) {
	if (fdset == NULL) {
		return;
	}
	xi_mem_free(fdset);
}

xint32 xi_sel_select(xint32 nfds, xi_fdset_t *rfds, xi_fdset_t *wfds,
		xi_fdset_t *efds, xint32 msec) {
	fd_set *rt = NULL;
	fd_set *wt = NULL;
	fd_set *et = NULL;

	xint32 ret = 0;

	if (rfds != NULL) {
		rt = &rfds->fdset;
	}
	if (wfds != NULL) {
		wt = &wfds->fdset;
	}
	if (efds != NULL) {
		et = &efds->fdset;
	}

	if (msec > 0) {
		struct timeval tv;

		tv.tv_sec = msec / 1000;
		tv.tv_usec = (msec % 1000) * 1000;

		ret = select(nfds, rt, wt, et, &tv);
	} else {
		ret = select(nfds, rt, wt, et, NULL);
	}

	if (ret < 0) {
		switch (errno) {
		case EBADF:
			return XI_SELECT_RV_ERR_BADFD;
		case EINTR:
			return XI_SELECT_RV_ERR_INTR;
		case ENOMEM:
			return XI_SELECT_RV_ERR_NOMEM;
		case EINVAL:
		default:
			return XI_SELECT_RV_ERR_ARGS;
		}
	} else {
		return ret;
	}
}
