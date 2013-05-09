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
 * File   : xi_poll.c
 */

#include <winsock2.h>
#include <ws2tcpip.h>

#include "xg_fd.h"

#include "xi/xi_poll.h"
#include "xi/xi_thread.h"
#include "xi/xi_mem.h"

// ----------------------------------------------
// Inner Structure
// ----------------------------------------------

struct _xi_pollset {
	xi_pollfd_t *pfds;
	xuint32 size;
	xuint32 used;
	xi_pollset_opt_e opt;
	xi_thread_mutex_t lock;
	fd_set set_read;
	fd_set set_write;
	fd_set set_except;
};


// ----------------------------------------------
// XI Functions
// ----------------------------------------------

xi_pollset_t *xi_pollset_create(xuint32 size, xint32 opt) {
	xint32 rv;

	xi_pollset_t *pset = xi_mem_calloc(1, sizeof(xi_pollset_t));
	if (pset == NULL) {
		return NULL;
	}

	pset->pfds = xi_mem_calloc(size, sizeof(xi_pollfd_t));
	if (pset->pfds == NULL) {
		xi_mem_free(pset);
		return NULL;
	}

	pset->size = size;
	pset->used = 0;
	pset->opt = opt;

	if (pset->opt & XI_POLLSET_OPT_USELOCK) {
		rv = xi_thread_mutex_create(&pset->lock, "xg_poll");
		if (rv < 0) {
			xi_mem_free(pset->pfds);
			xi_mem_free(pset);
			return NULL;
		}
	}

	FD_ZERO(&pset->set_read);
	FD_ZERO(&pset->set_write);
	FD_ZERO(&pset->set_except);

	return pset;
}

xi_pollset_re xi_pollset_add(xi_pollset_t *pset, xi_pollfd_t fd) {
	xg_fd_t *sdesc;

	if (pset == NULL) {
		return XI_POLLSET_RV_ERR_ARGS;
	}

	if (fd.desc < 0) {
		return XI_POLLSET_RV_ERR_ARGS;
	}

	if (pset->opt & XI_POLLSET_OPT_USELOCK) {
		xi_thread_mutex_lock(&pset->lock);
	}

	if (pset->used >= pset->size) {
		if (pset->opt & XI_POLLSET_OPT_USELOCK) {
			xi_thread_mutex_unlock(&pset->lock);
		}
		return XI_POLLSET_RV_ERR_OVER;
	}

	sdesc = xg_fd_get(fd.desc);

	if (fd.evts & XI_POLL_EVENT_IN) {
		FD_SET(sdesc->desc.s.fd, &pset->set_read);
	}
	if (fd.evts & XI_POLL_EVENT_OUT) {
		FD_SET(sdesc->desc.s.fd, &pset->set_write);
	}
	if (fd.evts & XI_POLL_EVENT_ERR) {
		FD_SET(sdesc->desc.s.fd, &pset->set_except);
	}

	pset->pfds[pset->used].desc = fd.desc;
	pset->pfds[pset->used].evts = fd.evts;
	pset->pfds[pset->used].context = fd.context;

	pset->used++;

	if (pset->opt & XI_POLLSET_OPT_USELOCK) {
		xi_thread_mutex_unlock(&pset->lock);
	}

	return XI_POLLSET_RV_OK;
}

xi_pollset_re xi_pollset_remove(xi_pollset_t *pset, xi_pollfd_t fd) {
	xuint32 i;
	xbool found;
	xg_fd_t *sdesc;

	if (pset == NULL) {
		return XI_POLLSET_RV_ERR_ARGS;
	}

	if (fd.desc < 0) {
		return XI_POLLSET_RV_ERR_ARGS;
	}

	if (pset->opt & XI_POLLSET_OPT_USELOCK) {
		xi_thread_mutex_lock(&pset->lock);
	}

	sdesc = xg_fd_get(fd.desc);

	found = FALSE;
	for (i=0; i<pset->used; i++) {
		if (pset->pfds[i].desc == fd.desc) {
			xi_mem_copy(&(pset->pfds[i]), &(pset->pfds[i+1]),
					sizeof(xi_pollfd_t) * (pset->used -i-1));
			FD_CLR(sdesc->desc.s.fd, &pset->set_read);
			FD_CLR(sdesc->desc.s.fd, &pset->set_write);
			FD_CLR(sdesc->desc.s.fd, &pset->set_except);
			found = TRUE;
			break;
		}
	}

	if (!found) {
		if (pset->opt & XI_POLLSET_OPT_USELOCK) {
			xi_thread_mutex_unlock(&pset->lock);
		}
		return XI_POLLSET_RV_ERR_NF;
	}

	pset->used--;

	if (pset->opt & XI_POLLSET_OPT_USELOCK) {
		xi_thread_mutex_unlock(&pset->lock);
	}

	return XI_POLLSET_RV_OK;
}

xi_pollset_re xi_pollset_destroy(xi_pollset_t *pset) {
	if (pset == NULL) {
		return XI_POLLSET_RV_ERR_ARGS;
	}

	if (pset->opt & XI_POLLSET_OPT_USELOCK) {
		xi_thread_mutex_lock(&pset->lock);
	}

	if (pset->opt & XI_POLLSET_OPT_USELOCK) {
		xi_thread_mutex_unlock(&pset->lock);
	}

	xi_thread_mutex_destroy(&pset->lock);
	xi_mem_free(pset->pfds);
	xi_mem_free(pset);

	return XI_POLLSET_RV_OK;
}

xint32 xi_pollset_poll(xi_pollset_t *pset, xi_pollfd_t *rfds, xint32 rlen,
		xint32 msecs) {
	xint32 ret;
	xuint32 loopcnt, i, j;

	fd_set tr, tw, te;

	struct timeval tout;

	if (pset == NULL || rfds == NULL) {
		return XI_POLLSET_RV_ERR_ARGS;
	}

	if (pset->opt & XI_POLLSET_OPT_USELOCK) {
		xi_thread_mutex_lock(&pset->lock);
	}

	xi_mem_copy(&tr, &pset->set_read, sizeof(fd_set));
	xi_mem_copy(&tw, &pset->set_write, sizeof(fd_set));
	xi_mem_copy(&te, &pset->set_except, sizeof(fd_set));

	if (msecs < 0) {
		ret = select(pset->used+1, &tr, &tw, &te, NULL);
	} else {
		tout.tv_sec = msecs / 1000;
		tout.tv_usec = (msecs % 1000) * 1000;
		ret = select(pset->used+1, &tr, &tw, &te, &tout);
	}
	if (ret > 0) {
		loopcnt = (xuint32)((ret > rlen) ? rlen : ret);
		for (i=0, j=0; i<pset->used && j<loopcnt; i++) {
			int ret_events = 0;
			xg_fd_t *sdesc = xg_fd_get(pset->pfds[i].desc);

			if (pset->pfds[i].evts & XI_POLL_EVENT_IN) {
				if (FD_ISSET(sdesc->desc.s.fd, &tr)) {
					rfds[j] = pset->pfds[i];
					ret_events |= XI_POLL_EVENT_IN;
				}
			}
			if (pset->pfds[i].evts & XI_POLL_EVENT_OUT) {
				if (FD_ISSET(sdesc->desc.s.fd, &tw)) {
					rfds[j] = pset->pfds[i];
					ret_events |= XI_POLL_EVENT_OUT;
				}
			}
			if (pset->pfds[i].evts & XI_POLL_EVENT_ERR) {
				if (FD_ISSET(sdesc->desc.s.fd, &te)) {
					rfds[j] = pset->pfds[i];
					ret_events |= XI_POLL_EVENT_ERR;
				}
			}
			if (ret_events) {
				rfds[j].evts = ret_events;
				j++;
			}
		}
	} else if (ret == 0) {
		if (pset->opt & XI_POLLSET_OPT_USELOCK) {
			xi_thread_mutex_unlock(&pset->lock);
		}
		return XI_POLLSET_RV_ERR_TIMEOUT;
	} else {
		if (pset->opt & XI_POLLSET_OPT_USELOCK) {
			xi_thread_mutex_unlock(&pset->lock);
		}
		switch (WSAGetLastError()) {
		case WSAEINTR:
			return XI_POLLSET_RV_ERR_INTR;
		default:
			return XI_POLLSET_RV_ERR_ARGS;
		}
	}

	if (pset->opt & XI_POLLSET_OPT_USELOCK) {
		xi_thread_mutex_unlock(&pset->lock);
	}

	return loopcnt;
}
