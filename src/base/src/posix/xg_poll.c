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

#include <errno.h>
#include <poll.h>

#ifdef __APPLE__
#include <stdio.h>
#include <sys/event.h>
#include <sys/time.h>
#else
#include <fcntl.h>
#include <sys/epoll.h>
#endif // __APPLE__
#include <unistd.h>

#include "xg_fd.h"

#include "xi/xi_poll.h"
#include "xi/xi_mem.h"
#include "xi/xi_thread.h"

// ----------------------------------------------
// Inner Structure
// ----------------------------------------------

struct _xi_pollset {
	xi_pollfd_t *pfds;
	xuint32 size;
	xuint32 used;
	xi_pollset_opt_e opt;
	xi_thread_mutex_t lock;
	struct pollfd *rfds;
	xint32 epfd;
};

// ----------------------------------------------
// Part Internal Functions
// ----------------------------------------------

static xint16 xg_pollset_events_2pg(xi_poll_event_e events) {
	xint16 ret = 0;

	if (events & XI_POLL_EVENT_IN) {
		ret |= POLLIN;
	}
	if (events & XI_POLL_EVENT_PRI) {
		ret |= POLLPRI;
	}
	if (events & XI_POLL_EVENT_OUT) {
		ret |= POLLOUT;
	}
	if (events & XI_POLL_EVENT_ERR) {
		ret |= POLLERR;
	}
	if (events & XI_POLL_EVENT_HUP) {
		ret |= POLLHUP;
	}
	if (events & XI_POLL_EVENT_NVAL) {
		ret |= POLLNVAL;
	}

	return ret;
}

static xi_poll_event_e xg_pollset_events_2pi(xint32 events) {
	xi_poll_event_e ret = 0;

	if (events & POLLIN) {
		ret |= XI_POLL_EVENT_IN;
	}
	if (events & POLLPRI) {
		ret |= XI_POLL_EVENT_PRI;
	}
	if (events & POLLOUT) {
		ret |= XI_POLL_EVENT_OUT;
	}
	if (events & POLLERR) {
		ret |= XI_POLL_EVENT_ERR;
	}
	if (events & POLLHUP) {
		ret |= XI_POLL_EVENT_HUP;
	}
	if (events & POLLNVAL) {
		ret |= XI_POLL_EVENT_NVAL;
	}

	return ret;
}

// ----------------------------------------------
// XI Functions
// ----------------------------------------------

xi_pollset_t *xi_pollset_create(xuint32 size, xint32 opt) {
	xint32 rv;

	xi_pollset_t *pset = xi_mem_calloc(1, sizeof(xi_pollset_t));
	if (pset == NULL) {
		return NULL;
	}

	pset->pfds = (xi_pollfd_t *)xi_mem_calloc(size, sizeof(xi_pollfd_t));
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

	if (pset->opt & XI_POLLSET_OPT_EPOLL) {
#ifdef __APPLE__
		pset->epfd = kqueue();
#else
		pset->epfd = epoll_create((xint32)size);
#endif
		if (pset->epfd < 0) {
			xi_mem_free(pset->pfds);
			xi_mem_free(pset);
			return NULL;
		}

#ifndef __APPLE__
		if ((rv = fcntl(pset->epfd, F_GETFD, NULL)) >= 0) {
			fcntl(pset->epfd, F_SETFD, rv | FD_CLOEXEC);
		}
#endif

		pset->rfds = NULL;
	} else {
		pset->rfds = xi_mem_calloc(size, sizeof(struct pollfd));
		if (pset->rfds == NULL) {
			xi_mem_free(pset->pfds);
			xi_mem_free(pset);
			return NULL;
		}
	}

	return pset;
}

xi_pollset_re xi_pollset_add(xi_pollset_t *pset, xi_pollfd_t fd) {
	if (pset == NULL) {
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

	if (pset->opt & XI_POLLSET_OPT_EPOLL) {
		xint32 rv;
#ifdef __APPLE__
		xint32 n = 0;
		struct kevent ev[2];

		if (fd.evts & XI_POLL_EVENT_IN) {
			printf("[poll_add] POLL_EVENT_IN>  desc: %d, ctx: %p\n", fd.desc, fd.context);
			EV_SET(&ev[n++], fd.desc, EVFILT_READ, EV_ADD | EV_CLEAR, 0, 0, 0);
		}
		if (fd.evts & XI_POLL_EVENT_OUT) {
			printf("[poll_add] POLL_EVENT_OUT> desc: %d, ctx: %p\n", fd.desc, fd.context);
			EV_SET(&ev[n++], fd.desc, EVFILT_WRITE, EV_ADD | EV_CLEAR, 0, 0, 0);
		}

		rv = kevent(pset->epfd, ev, n, NULL, 0, NULL);
#else // !__APPLE__
		struct epoll_event ev;

		ev.events = (uint32_t)EPOLLET;
		if (fd.evts & XI_POLL_EVENT_IN) {
			ev.events |= EPOLLIN;
		}
		if (fd.evts & XI_POLL_EVENT_OUT) {
			ev.events |= EPOLLOUT;
		}
		ev.data.fd = fd.desc;

		rv = epoll_ctl(pset->epfd, EPOLL_CTL_ADD, ev.data.fd, &ev);
#endif // __APPLE__
		if (rv < 0) {
			if (pset->opt & XI_POLLSET_OPT_USELOCK) {
				xi_thread_mutex_unlock(&pset->lock);
			}
			return XI_POLLSET_RV_ERR_OP;
		}
	} else {
		pset->rfds[pset->used].fd = fd.desc;
		pset->rfds[pset->used].events = xg_pollset_events_2pg(fd.evts);
		pset->rfds[pset->used].revents = 0;
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

	if (pset == NULL) {
		return XI_POLLSET_RV_ERR_ARGS;
	}

	if (pset->opt & XI_POLLSET_OPT_USELOCK) {
		xi_thread_mutex_lock(&pset->lock);
	}

	found = FALSE;
	for (i = 0; i < pset->used; i++) {
		if (pset->pfds[i].desc == fd.desc) {
			xi_mem_copy(&(pset->pfds[i]), &(pset->pfds[i + 1]),
					sizeof(xi_pollfd_t) * (pset->used - i - 1));
			if (!(pset->opt & XI_POLLSET_OPT_EPOLL)) {
				xi_mem_copy(&(pset->rfds[i]), &(pset->rfds[i + 1]),
						sizeof(struct pollfd) * (pset->used - i - 1));
			}
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

	if (pset->opt & XI_POLLSET_OPT_EPOLL) {
		xint32 rv;
#ifdef __APPLE__
		xint32 n = 0;
		struct kevent ev[2];

		if (fd.evts == XI_POLL_EVENT_IN) {
			EV_SET(&ev[n++], fd.desc, EVFILT_READ, EV_DELETE, 0, 0, 0);
		} else if (fd.evts == XI_POLL_EVENT_OUT) {
			EV_SET(&ev[n++], fd.desc, EVFILT_WRITE, EV_DELETE, 0, 0, 0);
		}

		rv = kevent(pset->epfd, ev, n, NULL, 0, NULL);
#else // !__APPLE__
		struct epoll_event ev;

		ev.events = (uint32_t)EPOLLET;
		if (fd.evts & XI_POLL_EVENT_IN) {
			ev.events |= EPOLLIN;
		}
		if (fd.evts & XI_POLL_EVENT_OUT) {
			ev.events |= EPOLLOUT;
		}
		ev.data.fd = fd.desc;

		rv = epoll_ctl(pset->epfd, EPOLL_CTL_DEL, ev.data.fd, &ev);
#endif // __APPLE__
		if (rv < 0) {
			if (pset->opt & XI_POLLSET_OPT_USELOCK) {
				xi_thread_mutex_unlock(&pset->lock);
			}
			return XI_POLLSET_RV_ERR_OP;
		}
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

	if (pset->opt & XI_POLLSET_OPT_EPOLL) {
#ifndef __APPLE__
		xuint32 i;
		xi_pollfd_t fd;

		xint32 rv;
		struct epoll_event ev;

		for (i = 0; i < pset->used; i++) {
			fd = pset->pfds[i];

			ev.events = (uint32_t)EPOLLET;
			if (fd.evts & XI_POLL_EVENT_IN) {
				ev.events |= EPOLLIN;
			}
			if (fd.evts & XI_POLL_EVENT_OUT) {
				ev.events |= EPOLLOUT;
			}
			ev.data.fd = fd.desc;

			rv = epoll_ctl(pset->epfd, EPOLL_CTL_DEL, ev.data.fd, &ev);
			if (rv < 0) {
				if (pset->opt & XI_POLLSET_OPT_USELOCK) {
					xi_thread_mutex_unlock(&pset->lock);
				}
				return XI_POLLSET_RV_ERR_OP;
			}
		}
#endif // !__APPLE__
		close(pset->epfd);
	} else {
		xi_mem_free(pset->rfds);
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
	xint32 ret, loopcnt;
	xuint32 i, j;

	if (pset == NULL || rfds == NULL) {
		return XI_POLLSET_RV_ERR_ARGS;
	}

	if (pset->opt & XI_POLLSET_OPT_USELOCK) {
		xi_thread_mutex_lock(&pset->lock);
	}

	if (pset->opt & XI_POLLSET_OPT_EPOLL) {
#ifdef __APPLE__
		struct kevent ev[rlen];
		struct timeval tv;
		struct timespec ts;

		gettimeofday(&tv, NULL);
		ts.tv_sec = tv.tv_sec + (msecs / 1000);
		ts.tv_nsec = tv.tv_usec * 1000 + (msecs % 1000) * 1000000;

		ret = kevent(pset->epfd, NULL, 0, ev, rlen, NULL);
//		ret = kevent(pset->epfd, NULL, 0, ev, rlen, &ts);
		printf("[poll_poll] ret: %d\n", ret);
#else // !__APPLE__
		struct epoll_event ev[rlen];

		ret = epoll_wait(pset->epfd, ev, rlen, msecs);
#endif // __APPLE__
		if (ret > 0) {
			loopcnt = (ret > rlen) ? rlen : ret;
			for (i = 0; i < (xuint32)loopcnt; i++) {
				for (j = 0; j < pset->used; j++) {
#ifdef __APPLE__
					if ((xint32)ev[i].ident == pset->pfds[j].desc) {
						rfds[i].desc = pset->pfds[j].desc;
						rfds[i].evts = 0;
						if (ev[i].filter == EVFILT_READ) {
							rfds[i].evts |= XI_POLL_EVENT_IN;
						} else if (ev[i].filter == EVFILT_WRITE) {
							rfds[i].evts |= XI_POLL_EVENT_OUT;
						}
						rfds[i].context = pset->pfds[j].context;
						break;
					}
#else // !__APPLE__
					if (ev[i].data.fd == pset->pfds[j].desc) {
						rfds[i].desc = pset->pfds[j].desc;
						rfds[i].evts = 0;
						if (ev[i].events & EPOLLIN) {
							rfds[i].evts |= XI_POLL_EVENT_IN;
						}
						if (ev[i].events & EPOLLOUT) {
							rfds[i].evts |= XI_POLL_EVENT_OUT;
						}
						rfds[i].context = pset->pfds[j].context;
						break;
					}
#endif // __APPLE__
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
			switch (errno) {
			case EINTR:
				return XI_POLLSET_RV_ERR_INTR;
			case EBADF:
			default:
				return XI_POLLSET_RV_ERR_ARGS;
			}
		}
	} else {
		ret = poll(pset->rfds, pset->used, msecs);
		if (ret > 0) {
			loopcnt = (ret > rlen) ? rlen : ret;
			for (i = 0, j = 0; i < pset->used && j < (xuint32)loopcnt; i++) {
				if (pset->rfds[i].revents) {
					rfds[j] = pset->pfds[i];
					rfds[j].evts = xg_pollset_events_2pi(
							pset->rfds[i].revents);
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
			switch (errno) {
			case EINTR:
				return XI_POLLSET_RV_ERR_INTR;
			case EBADF:
			default:
				return XI_POLLSET_RV_ERR_ARGS;
			}
		}
	}

	if (pset->opt & XI_POLLSET_OPT_USELOCK) {
		xi_thread_mutex_unlock(&pset->lock);
	}

	return loopcnt;
}
