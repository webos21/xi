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
 * File   : xg_socket.c
 */

#ifndef _LARGEFILE64_SOURCE
#define _LARGEFILE64_SOURCE
#endif
#ifndef _FILE_OFFSET_BITS
#define _FILE_OFFSET_BITS	64
#endif

#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/ioctl.h>
#ifndef __APPLE__
#include <sys/sendfile.h>
#endif
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <net/if.h>
#include <poll.h>

#include "xg_fd.h"

#include "xi/xi_mem.h"
#include "xi/xi_string.h"

// ----------------------------------------------
// Part Internal Functions
// ----------------------------------------------

static xint32 xg_sock_family_2pg(xi_sock_family_e family) {
	switch (family) {
	case XI_SOCK_FAMILY_LOCAL:
		return AF_LOCAL;
	case XI_SOCK_FAMILY_INET:
		return AF_INET;
	case XI_SOCK_FAMILY_INET6:
		return AF_INET6;
	default:
		return -1;
	}
}

#if 0
/* Not Used */
static xi_sock_family_e xg_sock_family_2pi(xint32 family) {
	switch (family) {
		case AF_LOCAL:
		return XI_SOCK_FAMILY_LOCAL;
		case AF_INET:
		return XI_SOCK_FAMILY_INET;
		case AF_INET6:
		return XI_SOCK_FAMILY_INET6;
		default:
		return -1;
	}
}
#endif // 0
static xint32 xg_sock_type_2pg(xi_sock_type_e type) {
	switch (type) {
	case XI_SOCK_TYPE_STREAM:
		return SOCK_STREAM;
	case XI_SOCK_TYPE_DATAGRAM:
		return SOCK_DGRAM;
	case XI_SOCK_TYPE_RAW:
		return SOCK_RAW;
	default:
		return -1;
	}
}

#if 0
/* Not Used */
static xi_sock_type_e xg_sock_type_2pi(xint32 type) {
	switch (type) {
		case SOCK_STREAM:
		return XI_SOCK_TYPE_STREAM;
		case SOCK_DGRAM:
		return XI_SOCK_TYPE_DATAGRAM;
		case SOCK_RAW:
		return XI_SOCK_TYPE_RAW;
		default:
		return -1;
	}
}
#endif // 0
static xint32 xg_sock_proto_2pg(xi_sock_proto_e proto) {
	switch (proto) {
	case XI_SOCK_PROTO_IP:
		return IPPROTO_IP;
	case XI_SOCK_PROTO_TCP:
		return IPPROTO_TCP;
	case XI_SOCK_PROTO_UDP:
		return IPPROTO_UDP;
	case XI_SOCK_PROTO_RAW:
		return IPPROTO_RAW;
	default:
		return -1;
	}
}

#if 0
/* Not Used */
static xi_sock_proto_e xg_sock_proto_2pi(xint32 proto) {
	switch (proto) {
		case IPPROTO_IP:
		return XI_SOCK_PROTO_IP;
		case IPPROTO_TCP:
		return XI_SOCK_PROTO_TCP;
		case IPPROTO_UDP:
		return XI_SOCK_PROTO_UDP;
		case IPPROTO_RAW:
		return XI_SOCK_PROTO_RAW;
		default:
		return -1;
	}
}
#endif // 0
static xint32 xg_sock_opt_2pg(xi_sock_opt_e opt) {
	switch (opt) {
	case XI_SOCK_OPT_LINGER:
		return SO_LINGER;
	case XI_SOCK_OPT_KEEPALIVE:
		return SO_KEEPALIVE;
	case XI_SOCK_OPT_DEBUG:
		return SO_DEBUG;
	case XI_SOCK_OPT_NONBLOCK:
		return O_NONBLOCK;
	case XI_SOCK_OPT_REUSEADDR:
		return SO_REUSEADDR;
	case XI_SOCK_OPT_SENDBUF:
		return SO_SNDBUF;
	case XI_SOCK_OPT_RECVBUF:
		return SO_RCVBUF;
	case XI_SOCK_OPT_SNDTIMEO:
		return SO_SNDTIMEO;
	case XI_SOCK_OPT_RCVTIMEO:
		return SO_RCVTIMEO;
	case XI_SOCK_OPT_IPV6ONLY:
	default:
		return -1;
	}
}

static xint32 xg_sock_get_if_idx(xint32 sfd, const xchar *ip) {
	struct ifconf cnf;

	xint32 ret = ioctl(sfd, SIOCGIFCONF, &cnf);
	if (ret < 0) {
		return 0;
	} else {
		xint32 i;
		struct ifreq ifr;
		char buf[XCFG_HOSTNAME_MAX];

		for (i = 0; i < cnf.ifc_len; i++) {
			ifr = cnf.ifc_ifcu.ifcu_req[i];
			inet_ntop(AF_INET, &ifr.ifr_addr, buf, sizeof(ifr.ifr_addr));
			if (xi_strcmp(ip, buf) == 0) {
				return (xint32) if_nametoindex(ifr.ifr_name);
			}
		}
	}

	return 0;
}

// ----------------------------------------------
// XI Functions
// ----------------------------------------------

xint32 xi_socket_open(xi_sock_family_e family, xi_sock_type_e type,
		xi_sock_proto_e proto) {
	xint32 sock;
	xg_fd_t sdesc;
	xint32 fa = xg_sock_family_2pg(family);
	xint32 ty = xg_sock_type_2pg(type);
	xint32 pr = xg_sock_proto_2pg(proto);

	if (fa < 0 || ty < 0 || pr < 0) {
		return XI_SOCK_RV_ERR_ARGS;
	}

	sock = socket(fa, ty, pr);
	if (sock < 0) {
		switch (errno) {
		case EACCES:
			return XI_SOCK_RV_ERR_PERM;
		case ENOBUFS:
		case ENOMEM:
			return XI_SOCK_RV_ERR_NOADDR;
		case EINVAL:
		case EPROTONOSUPPORT:
		default:
			return XI_SOCK_RV_ERR_ARGS;
		}
	}

	sdesc.type = XG_FD_TYPE_SOCK;
	sdesc.desc.s.family = family;
	sdesc.desc.s.type = type;
	sdesc.desc.s.proto = proto;

	return xg_fd_open(sock, &sdesc);
}

xi_sock_re xi_socket_bind(xint32 sfd, xi_sock_addr_t baddr) {
	xint32 ret;

	if (sfd < 0) {
		return XI_SOCK_RV_ERR_ARGS;
	}

	switch (baddr.family) {
	case XI_SOCK_FAMILY_LOCAL: {
		struct sockaddr_un addr;

		addr.sun_family = (sa_family_t) xg_sock_family_2pg(baddr.family);
		xi_strcpy(addr.sun_path, baddr.host);

		ret = bind(sfd, (struct sockaddr *) &addr, sizeof(addr));
		break;
	}
	case XI_SOCK_FAMILY_INET: {
		struct sockaddr_in addr;

		addr.sin_family = (sa_family_t) xg_sock_family_2pg(baddr.family);
		if (!inet_pton(AF_INET, baddr.host, &addr.sin_addr)) {
			return XI_SOCK_RV_ERR_ADDR;
		}
		addr.sin_port = htons(baddr.port);

		ret = bind(sfd, (struct sockaddr *) &addr, sizeof(addr));
		break;
	}
	case XI_SOCK_FAMILY_INET6: {
		struct sockaddr_in6 addr;

		addr.sin6_family = (sa_family_t) xg_sock_family_2pg(baddr.family);
		if (!inet_pton(AF_INET6, baddr.host, &addr.sin6_addr)) {
			return XI_SOCK_RV_ERR_ADDR;
		}
		addr.sin6_port = htons(baddr.port);

		ret = bind(sfd, (struct sockaddr *) &addr, sizeof(addr));
		break;
	}
	default:
		return XI_SOCK_RV_ERR_ARGS;
	}

	if (ret < 0) {
		switch (errno) {
		case EACCES:
		case EROFS:
			return XI_SOCK_RV_ERR_PERM;
		case EBADF:
			return XI_SOCK_RV_ERR_FD;
		case ENOTSOCK:
			return XI_SOCK_RV_ERR_NOTSOCK;
		default:
			return XI_SOCK_RV_ERR_ARGS;
		}
	}

	return XI_SOCK_RV_OK;
}

xi_sock_re xi_socket_listen(xint32 sfd, xint32 backlog) {
	xint32 ret;

	if (sfd < 0) {
		return XI_SOCK_RV_ERR_ARGS;
	}

	ret = listen(sfd, backlog);
	if (ret < 0) {
		switch (errno) {
		case EBADF:
			return XI_SOCK_RV_ERR_FD;
		case ENOTSOCK:
			return XI_SOCK_RV_ERR_NOTSOCK;
		case EADDRINUSE:
			return XI_SOCK_RV_ERR_INUSE;
		case EOPNOTSUPP:
			return XI_SOCK_RV_ERR_NS;
		default:
			return XI_SOCK_RV_ERR_ARGS;
		}
	}

	return XI_SOCK_RV_OK;
}

xint32 xi_socket_accept(xint32 sfd, xi_sock_addr_t *fromaddr) {
	xint32 rsock;
	xg_fd_t *sdesc = NULL;

	if (sfd < 0) {
		return XI_SOCK_RV_ERR_ARGS;
	}

	sdesc = xg_fd_get(sfd);

	switch (sdesc->desc.s.family) {
	case XI_SOCK_FAMILY_LOCAL: {
		struct sockaddr_un addr;
		xuint32 addrlen = sizeof(addr);

		rsock = accept(sfd, (struct sockaddr *) &addr, &addrlen);
		if (rsock < 0) {
			switch (errno) {
			case EACCES:
			case EROFS:
			case EPERM:
				return XI_SOCK_RV_ERR_PERM;
			case EBADF:
				return XI_SOCK_RV_ERR_FD;
			case ENOTSOCK:
				return XI_SOCK_RV_ERR_NOTSOCK;
			case EAFNOSUPPORT:
			case EFAULT:
				return XI_SOCK_RV_ERR_ADDR;
			case EADDRINUSE:
				return XI_SOCK_RV_ERR_INUSE;
			case ECONNREFUSED:
				return XI_SOCK_RV_ERR_REFUSED;
			case EISCONN:
				return XI_SOCK_RV_ERR_ALREADY;
			case ETIMEDOUT:
				return XI_SOCK_RV_ERR_TIMEOUT;
			default:
				return XI_SOCK_RV_ERR_ARGS;
			}
		}

		fromaddr->family = addr.sun_family;
		fromaddr->type = sdesc->desc.s.type;
		fromaddr->proto = sdesc->desc.s.proto;
		xi_strcpy(fromaddr->host, addr.sun_path);
		fromaddr->port = 0;

		return xg_fd_open(rsock, sdesc);
	}
	case XI_SOCK_FAMILY_INET: {
		struct sockaddr_in addr;
		xuint32 addrlen = sizeof(addr);

		rsock = accept(sfd, (struct sockaddr *) &addr, &addrlen);
		if (rsock < 0) {
			switch (errno) {
			case EACCES:
			case EROFS:
			case EPERM:
				return XI_SOCK_RV_ERR_PERM;
			case EBADF:
				return XI_SOCK_RV_ERR_FD;
			case ENOTSOCK:
				return XI_SOCK_RV_ERR_NOTSOCK;
			case EAFNOSUPPORT:
			case EFAULT:
				return XI_SOCK_RV_ERR_ADDR;
			case EADDRINUSE:
				return XI_SOCK_RV_ERR_INUSE;
			case ECONNREFUSED:
				return XI_SOCK_RV_ERR_REFUSED;
			case EISCONN:
				return XI_SOCK_RV_ERR_ALREADY;
			case ETIMEDOUT:
				return XI_SOCK_RV_ERR_TIMEOUT;
			default:
				return XI_SOCK_RV_ERR_ARGS;
			}
		}

		fromaddr->family = addr.sin_family;
		fromaddr->type = sdesc->desc.s.type;
		fromaddr->proto = sdesc->desc.s.proto;
		inet_ntop(AF_INET, &addr.sin_addr, fromaddr->host, addrlen);
		fromaddr->port = ntohs(addr.sin_port);

		return xg_fd_open(rsock, sdesc);
	}
	case XI_SOCK_FAMILY_INET6: {
		struct sockaddr_in6 addr;
		xuint32 addrlen = sizeof(addr);

		rsock = accept(sfd, (struct sockaddr *) &addr, &addrlen);
		if (rsock < 0) {
			switch (errno) {
			case EACCES:
			case EROFS:
			case EPERM:
				return XI_SOCK_RV_ERR_PERM;
			case EBADF:
				return XI_SOCK_RV_ERR_FD;
			case ENOTSOCK:
				return XI_SOCK_RV_ERR_NOTSOCK;
			case EAFNOSUPPORT:
			case EFAULT:
				return XI_SOCK_RV_ERR_ADDR;
			case EADDRINUSE:
				return XI_SOCK_RV_ERR_INUSE;
			case ECONNREFUSED:
				return XI_SOCK_RV_ERR_REFUSED;
			case EISCONN:
				return XI_SOCK_RV_ERR_ALREADY;
			case ETIMEDOUT:
				return XI_SOCK_RV_ERR_TIMEOUT;
			default:
				return XI_SOCK_RV_ERR_ARGS;
			}
		}
		fromaddr->family = sdesc->desc.s.family;
		fromaddr->type = sdesc->desc.s.type;
		fromaddr->proto = sdesc->desc.s.proto;
		inet_ntop(AF_INET6, &addr.sin6_addr, fromaddr->host, addrlen);
		fromaddr->port = ntohs(addr.sin6_port);

		return xg_fd_open(rsock, sdesc);
	}
	default:
		return XI_SOCK_RV_ERR_NS;
	}
}

xi_sock_re xi_socket_connect(xint32 sfd, xi_sock_addr_t caddr) {
	xint32 ret;

	if (sfd < 0) {
		return XI_SOCK_RV_ERR_ARGS;
	}

	switch (caddr.family) {
	case XI_SOCK_FAMILY_UNSPEC: {
		struct sockaddr sa;

		xi_mem_set(&sa, 0, sizeof(sa));
		sa.sa_family = AF_UNSPEC;

		ret = connect(sfd, &sa, sizeof(sa));
		break;
	}
	case XI_SOCK_FAMILY_LOCAL: {
		struct sockaddr_un addr;

		addr.sun_family = (sa_family_t) xg_sock_family_2pg(caddr.family);
		xi_strcpy(addr.sun_path, caddr.host);

		ret = connect(sfd, (struct sockaddr *) &addr, sizeof(addr));
		break;
	}
	case XI_SOCK_FAMILY_INET: {
		struct sockaddr_in addr;

		addr.sin_family = (sa_family_t) xg_sock_family_2pg(caddr.family);
		if (!inet_pton(AF_INET, caddr.host, &addr.sin_addr)) {
			return XI_SOCK_RV_ERR_ADDR;
		}
		addr.sin_port = htons(caddr.port);

		ret = connect(sfd, (struct sockaddr *) &addr, sizeof(addr));
		break;
	}
	case XI_SOCK_FAMILY_INET6: {
		struct sockaddr_in6 addr;

		addr.sin6_family = (sa_family_t) xg_sock_family_2pg(caddr.family);
		if (!inet_pton(AF_INET6, caddr.host, &addr.sin6_addr)) {
			return XI_SOCK_RV_ERR_ADDR;
		}
		addr.sin6_port = htons(caddr.port);

		ret = connect(sfd, (struct sockaddr *) &addr, sizeof(addr));
		break;
	}
	default:
		return XI_SOCK_RV_ERR_ARGS;
	}

	if (ret < 0) {
		switch (errno) {
		case EACCES:
		case EROFS:
		case EPERM:
			return XI_SOCK_RV_ERR_PERM;
		case EBADF:
			return XI_SOCK_RV_ERR_FD;
		case ENOTSOCK:
			return XI_SOCK_RV_ERR_NOTSOCK;
		case EAFNOSUPPORT:
		case EFAULT:
			return XI_SOCK_RV_ERR_ADDR;
		case EADDRINUSE:
			return XI_SOCK_RV_ERR_INUSE;
		case ECONNREFUSED:
			return XI_SOCK_RV_ERR_REFUSED;
		case EISCONN:
			return XI_SOCK_RV_ERR_ALREADY;
		case ETIMEDOUT:
			return XI_SOCK_RV_ERR_TIMEOUT;
		case EINPROGRESS:
			return XI_SOCK_RV_ERR_TRYLATER;
		default:
			return XI_SOCK_RV_ERR_ARGS;
		}
	}

	return XI_SOCK_RV_OK;
}

xi_sock_re xi_socket_opt_set(xint32 sfd, xi_sock_opt_e opt, xint32 val) {
	xint32 ret = 0;

	if (sfd < 0) {
		return XI_SOCK_RV_ERR_ARGS;
	}

	if (opt == XI_SOCK_OPT_NONBLOCK) {
		xint32 flags = 0;
		flags = fcntl(sfd, F_GETFL, 0);
		if (val == 0) {
			flags &= ~O_NONBLOCK;
		} else {
			flags |= O_NONBLOCK;
		}
		ret = fcntl(sfd, F_SETFL, flags);
	} else if (opt == XI_SOCK_OPT_IPV6ONLY) {
		ret = setsockopt(sfd, IPPROTO_IPV6, xg_sock_opt_2pg(opt), &val,
				sizeof(val));
	} else if (opt == XI_SOCK_OPT_SNDTIMEO || opt == XI_SOCK_OPT_RCVTIMEO) {
		struct timeval tv;

		xint32 ropt = xg_sock_opt_2pg(opt);
		tv.tv_sec = val / 1000;
		tv.tv_usec = (val % 1000) * 1000;

		ret = setsockopt(sfd, SOL_SOCKET, ropt, &tv, sizeof(struct timeval));
	} else {
		xint32 ropt = xg_sock_opt_2pg(opt);
		if (ropt < 0) {
			return XI_SOCK_RV_ERR_NS;
		}
		ret = setsockopt(sfd, SOL_SOCKET, ropt, &val, sizeof(val));
	}

	if (ret < 0) {
		switch (errno) {
		case EACCES:
		case EPERM:
			return XI_SOCK_RV_ERR_PERM;
		case EBADF:
			return XI_SOCK_RV_ERR_FD;
		case EINTR:
			return XI_SOCK_RV_ERR_INTR;
		case ENOTSOCK:
			return XI_SOCK_RV_ERR_NOTSOCK;
		case ENOPROTOOPT:
			return XI_SOCK_RV_ERR_NS;
		default:
			return XI_SOCK_RV_ERR_ARGS;
		}
	}

	return XI_SOCK_RV_OK;
}

xi_sock_re xi_socket_opt_get(xint32 sfd, xi_sock_opt_e opt, xint32 *val) {
	xint32 ret = 0;

	if (sfd < 0) {
		return XI_SOCK_RV_ERR_ARGS;
	}

	if (opt == XI_SOCK_OPT_NONBLOCK) {
		xint32 flags = 0;
		flags = fcntl(sfd, F_GETFL, 0);
		(*val) = (flags & O_NONBLOCK) ? TRUE : FALSE;
		ret = flags;
	} else if (opt == XI_SOCK_OPT_IPV6ONLY) {
		xuint32 len = 0;
		ret = getsockopt(sfd, IPPROTO_IPV6, xg_sock_opt_2pg(opt), val, &len);
	} else {
		xuint32 len = 0;
		xint32 ropt = xg_sock_opt_2pg(opt);
		if (ropt < 0) {
			return XI_SOCK_RV_ERR_NS;
		}
		ret = getsockopt(sfd, SOL_SOCKET, ropt, val, &len);
	}

	if (ret < 0) {
		switch (errno) {
		case EACCES:
		case EPERM:
			return XI_SOCK_RV_ERR_PERM;
		case EBADF:
			return XI_SOCK_RV_ERR_FD;
		case EINTR:
			return XI_SOCK_RV_ERR_INTR;
		case ENOTSOCK:
			return XI_SOCK_RV_ERR_NOTSOCK;
		case ENOPROTOOPT:
			return XI_SOCK_RV_ERR_NS;
		default:
			return XI_SOCK_RV_ERR_ARGS;
		}
	}

	return XI_SOCK_RV_OK;
}

xssize xi_socket_recv(xint32 sfd, xvoid *buf, xsize blen) {
	xssize ret;

	if (sfd < 0 || buf == NULL || blen <= 0) {
		return XI_SOCK_RV_ERR_ARGS;
	}

	ret = recv(sfd, buf, blen, 0);
	if (ret < 0) {
		switch (errno) {
		case EBADF:
			return XI_SOCK_RV_ERR_FD;
		case ENOTSOCK:
			return XI_SOCK_RV_ERR_NOTSOCK;
		case ENOTCONN:
			return XI_SOCK_RV_ERR_NOTCONN;
		case ECONNREFUSED:
			return XI_SOCK_RV_ERR_REFUSED;
		case EINTR:
			return XI_SOCK_RV_ERR_INTR;
		default:
			return XI_SOCK_RV_ERR_ARGS;
		}
	}

	return ret;
}

xssize xi_socket_recvfrom(xint32 sfd, xvoid *buf, xsize blen,
		xi_sock_addr_t *fromaddr) {
	xssize ret;
	xg_fd_t *sdesc;

	if (sfd < 0 || buf == NULL || blen <= 0 || fromaddr == NULL) {
		return XI_SOCK_RV_ERR_ARGS;
	}

	sdesc = xg_fd_get(sfd);

	switch (sdesc->desc.s.family) {
	case XI_SOCK_FAMILY_LOCAL: {
		struct sockaddr_un addr;
		xuint32 addrlen = sizeof(addr);

		ret = recvfrom(sfd, buf, blen, 0, (struct sockaddr *) &addr, &addrlen);

		if (ret > 0) {
			fromaddr->family = sdesc->desc.s.family;
			fromaddr->type = sdesc->desc.s.type;
			fromaddr->proto = sdesc->desc.s.proto;
			xi_strcpy(fromaddr->host, addr.sun_path);
		}
		break;
	}
	case XI_SOCK_FAMILY_INET: {
		struct sockaddr_in addr;
		xuint32 addrlen = sizeof(addr);

		ret = recvfrom(sfd, buf, blen, 0, (struct sockaddr *) &addr, &addrlen);

		if (ret > 0) {
			fromaddr->family = sdesc->desc.s.family;
			fromaddr->type = sdesc->desc.s.type;
			fromaddr->proto = sdesc->desc.s.proto;
			inet_ntop(AF_INET, &addr.sin_addr, fromaddr->host, addrlen);
			fromaddr->port = ntohs(addr.sin_port);
		}
		break;
	}
	case XI_SOCK_FAMILY_INET6: {
		struct sockaddr_in6 addr;
		xuint32 addrlen = sizeof(addr);

		ret = recvfrom(sfd, buf, blen, 0, (struct sockaddr *) &addr, &addrlen);

		if (ret > 0) {
			fromaddr->family = sdesc->desc.s.family;
			fromaddr->type = sdesc->desc.s.type;
			fromaddr->proto = sdesc->desc.s.proto;
			inet_ntop(AF_INET6, &addr.sin6_addr, fromaddr->host, addrlen);
			fromaddr->port = ntohs(addr.sin6_port);
		}
		break;
	}
	default:
		return XI_SOCK_RV_ERR_ARGS;
	}

	if (ret < 0) {
		switch (errno) {
		case EBADF:
			return XI_SOCK_RV_ERR_FD;
		case ENOTSOCK:
			return XI_SOCK_RV_ERR_NOTSOCK;
		case ENOTCONN:
			return XI_SOCK_RV_ERR_NOTCONN;
		case ECONNREFUSED:
			return XI_SOCK_RV_ERR_REFUSED;
		case EINTR:
			return XI_SOCK_RV_ERR_INTR;
		default:
			return XI_SOCK_RV_ERR_ARGS;
		}
	}

	return ret;
}

xssize xi_socket_send(xint32 sfd, const xvoid *buf, xsize blen) {
	xssize ret;

	if (sfd < 0 || buf == NULL || blen <= 0) {
		return XI_SOCK_RV_ERR_ARGS;
	}

	ret = send(sfd, buf, blen, 0);
	if (ret < 0) {
		switch (errno) {
		case EBADF:
			return XI_SOCK_RV_ERR_FD;
		case ENOTSOCK:
			return XI_SOCK_RV_ERR_NOTSOCK;
		case ENOTCONN:
			return XI_SOCK_RV_ERR_NOTCONN;
		case ECONNREFUSED:
			return XI_SOCK_RV_ERR_REFUSED;
		case ENOBUFS:
			return XI_SOCK_RV_ERR_OVERFLOW;
		case EOPNOTSUPP:
			return XI_SOCK_RV_ERR_NS;
		case EDESTADDRREQ:
			return XI_SOCK_RV_ERR_ADDR;
		case EINTR:
			return XI_SOCK_RV_ERR_INTR;
		default:
			return XI_SOCK_RV_ERR_ARGS;
		}
	}

	return ret;
}

xssize xi_socket_sendto(xint32 sfd, const xvoid *buf, xsize blen,
		xi_sock_addr_t toaddr) {
	xssize ret;
	xg_fd_t *sdesc;

	if (sfd < 0 || buf == NULL || blen <= 0) {
		return XI_SOCK_RV_ERR_ARGS;
	}

	sdesc = xg_fd_get(sfd);

	switch (sdesc->desc.s.family) {
	case XI_SOCK_FAMILY_LOCAL: {
		struct sockaddr_un addr;
		xuint32 alen = sizeof(addr);

		addr.sun_family = (sa_family_t) xg_sock_family_2pg(toaddr.family);
		xi_strcpy(addr.sun_path, toaddr.host);

		ret = sendto(sfd, buf, blen, 0, (struct sockaddr *) &addr, alen);
		break;
	}
	case XI_SOCK_FAMILY_INET: {
		struct sockaddr_in addr;
		xuint32 alen = sizeof(addr);

		addr.sin_family = (sa_family_t) xg_sock_family_2pg(toaddr.family);
		if (!inet_pton(AF_INET, toaddr.host, &addr.sin_addr)) {
			return XI_SOCK_RV_ERR_ADDR;
		}
		addr.sin_port = htons(toaddr.port);

		ret = sendto(sfd, buf, blen, 0, (struct sockaddr *) &addr, alen);
		break;
	}
	case XI_SOCK_FAMILY_INET6: {
		struct sockaddr_in6 addr;
		xuint32 alen = sizeof(addr);

		addr.sin6_family = (sa_family_t) xg_sock_family_2pg(toaddr.family);
		if (!inet_pton(AF_INET6, toaddr.host, &addr.sin6_addr)) {
			return XI_SOCK_RV_ERR_ADDR;
		}
		addr.sin6_port = htons(toaddr.port);

		ret = sendto(sfd, buf, blen, 0, (struct sockaddr *) &addr, alen);
		break;
	}
	default:
		return XI_SOCK_RV_ERR_ARGS;
	}

	if (ret < 0) {
		switch (errno) {
		case EBADF:
			return XI_SOCK_RV_ERR_FD;
		case ENOTSOCK:
			return XI_SOCK_RV_ERR_NOTSOCK;
		case ENOTCONN:
			return XI_SOCK_RV_ERR_NOTCONN;
		case ECONNREFUSED:
			return XI_SOCK_RV_ERR_REFUSED;
		case ENOBUFS:
			return XI_SOCK_RV_ERR_OVERFLOW;
		case EOPNOTSUPP:
			return XI_SOCK_RV_ERR_NS;
		case EDESTADDRREQ:
			return XI_SOCK_RV_ERR_ADDR;
		case EINTR:
			return XI_SOCK_RV_ERR_INTR;
		default:
			return XI_SOCK_RV_ERR_ARGS;
		}
	}

	return ret;
}

xssize xi_socket_sendfile(xint32 sfd, xint32 ffd, xoff64 *offset, xsize count) {
	xssize ret = 0;

#ifdef __APPLE__
	xint64 len = count;
	ret = sendfile(ffd, sfd, *offset, &len, NULL, 0);
	if (ret < 0) {
		return -errno;
	}
	return len;
#else
	ret = sendfile(sfd, ffd, offset, count);
	if (ret < 0) {
		return -errno;
	}
	return ret;
#endif
}

xi_sock_re xi_socket_shutdown(xint32 sfd, xi_sock_shutdown_e how) {
	xint32 ret;

	if (sfd < 0) {
		return XI_SOCK_RV_ERR_ARGS;
	}

	ret = shutdown(sfd, (xint32)how);
	if (ret < 0) {
		switch (errno) {
		case EBADF:
			return XI_SOCK_RV_ERR_FD;
		case ENOTSOCK:
			return XI_SOCK_RV_ERR_NOTSOCK;
		case ENOTCONN:
			return XI_SOCK_RV_ERR_NOTCONN;
		default:
			return XI_SOCK_RV_ERR_ARGS;
		}
	}

	return XI_SOCK_RV_OK;
}

xi_sock_re xi_socket_close(xint32 sfd) {
	xint32 ret;

	if (sfd < 0) {
		return XI_SOCK_RV_ERR_ARGS;
	}

	ret = close(sfd);
	if (ret < 0) {
		return XI_SOCK_RV_ERR_FD;
	}

	return xg_fd_close(sfd);
}

xint32 xi_socket_get_hostname(xchar *buf, xint32 buflen) {
	xint32 ret;

	ret = gethostname(buf, (size_t)buflen);
	if (ret < 0) {
		return -1;
	}

	return (xint32) xi_strlen(buf);
}

xint32 xi_socket_get_addr(const xchar *hostname, xi_sock_addr_t *addr,
		xint32 addrlen) {
	struct hostent *hosts;
	xint32 i = 0;

	if (hostname == NULL || addr == NULL || addrlen <= 0) {
		return XI_SOCK_RV_ERR_ARGS;
	}

	hosts = gethostbyname(hostname);
	if (hosts == NULL) {
		switch (h_errno) {
		case HOST_NOT_FOUND:
			return XI_SOCK_RV_ERR_HOST_NF;
		case NO_ADDRESS:
			return XI_SOCK_RV_ERR_NOADDR;
		case TRY_AGAIN:
			return XI_SOCK_RV_ERR_TRYLATER;
		case NO_RECOVERY:
		default:
			return XI_SOCK_RV_ERR_ARGS;
		}
	}

	if (hosts->h_addrtype == AF_INET) {
		struct sockaddr_in **paddr = (struct sockaddr_in **) hosts->h_addr_list;
		for (i = 0; paddr[i] != NULL && i < addrlen; i++) {
			inet_ntop(AF_INET, paddr[i], addr[i].host,
					sizeof(struct sockaddr_in));
		}
	} else if (hosts->h_addrtype == AF_INET6) {
		struct sockaddr_in6 **paddr =
				(struct sockaddr_in6 **) hosts->h_addr_list;
		for (i = 0; paddr[i] != NULL && i < addrlen; i++) {
			inet_ntop(AF_INET6, paddr[i], addr[i].host,
					sizeof(struct sockaddr_in6));
		}
	}

	return i;
}

xi_sock_re xi_socket_get_peer(xint32 sfd, xi_sock_addr_t *addr) {
	xint32 ret;
	xg_fd_t *sdesc;

	if (sfd < 0 || addr == NULL) {
		return XI_SOCK_RV_ERR_ARGS;
	}

	sdesc = xg_fd_get(sfd);

	switch (sdesc->desc.s.family) {
	case XI_SOCK_FAMILY_LOCAL: {
		struct sockaddr_un xaddr;
		xuint32 alen = sizeof(xaddr);

		ret = getpeername(sfd, (struct sockaddr *) &xaddr, &alen);
		if (ret == 0) {
			addr->family = xaddr.sun_family;
			xi_strcpy(addr->host, xaddr.sun_path);
		}
		break;
	}
	case XI_SOCK_FAMILY_INET: {
		struct sockaddr_in xaddr;
		xuint32 alen = sizeof(xaddr);

		ret = getpeername(sfd, (struct sockaddr *) &xaddr, &alen);
		if (ret == 0) {
			addr->family = sdesc->desc.s.family;
			addr->type = sdesc->desc.s.type;
			addr->proto = sdesc->desc.s.proto;
			inet_ntop(AF_INET, &xaddr.sin_addr, addr->host, alen);
			addr->port = ntohs(xaddr.sin_port);
		}
		break;
	}
	case XI_SOCK_FAMILY_INET6: {
		struct sockaddr_in6 xaddr;
		xuint32 alen = sizeof(xaddr);

		ret = getpeername(sfd, (struct sockaddr *) &xaddr, &alen);
		if (ret == 0) {
			addr->family = sdesc->desc.s.family;
			addr->type = sdesc->desc.s.type;
			addr->proto = sdesc->desc.s.proto;
			inet_ntop(AF_INET6, &xaddr.sin6_addr, addr->host, alen);
			addr->port = ntohs(xaddr.sin6_port);
		}
		break;
	}
	default:
		return XI_SOCK_RV_ERR_ARGS;
	}

	if (ret < 0) {
		switch (errno) {
		case EBADF:
			return XI_SOCK_RV_ERR_FD;
		case ENOTSOCK:
			return XI_SOCK_RV_ERR_NOTSOCK;
		case ENOTCONN:
			return XI_SOCK_RV_ERR_NOTCONN;
		default:
			return XI_SOCK_RV_ERR_ARGS;
		}
	}

	return XI_SOCK_RV_OK;
}

xi_sock_re xi_socket_get_local(xint32 sfd, xi_sock_addr_t *addr) {
	xint32 ret;
	xg_fd_t *sdesc;

	if (sfd < 0 || addr == NULL) {
		return XI_SOCK_RV_ERR_ARGS;
	}

	sdesc = xg_fd_get(sfd);

	switch (sdesc->desc.s.family) {
	case XI_SOCK_FAMILY_LOCAL: {
		struct sockaddr_un xaddr;
		xuint32 alen = sizeof(xaddr);

		ret = getsockname(sfd, (struct sockaddr *) &xaddr, &alen);
		if (ret == 0) {
			addr->family = xaddr.sun_family;
			xi_strcpy(addr->host, xaddr.sun_path);
		}
		break;
	}
	case XI_SOCK_FAMILY_INET: {
		struct sockaddr_in xaddr;
		xuint32 alen = sizeof(xaddr);

		ret = getsockname(sfd, (struct sockaddr *) &xaddr, &alen);
		if (ret == 0) {
			addr->family = sdesc->desc.s.family;
			addr->type = sdesc->desc.s.type;
			addr->proto = sdesc->desc.s.proto;
			inet_ntop(AF_INET, &xaddr.sin_addr, addr->host, alen);
			addr->port = ntohs(xaddr.sin_port);
		}
		break;
	}
	case XI_SOCK_FAMILY_INET6: {
		struct sockaddr_in6 xaddr;
		xuint32 alen = sizeof(xaddr);

		ret = getsockname(sfd, (struct sockaddr *) &xaddr, &alen);
		if (ret == 0) {
			addr->family = sdesc->desc.s.family;
			addr->type = sdesc->desc.s.type;
			addr->proto = sdesc->desc.s.proto;
			inet_ntop(AF_INET6, &xaddr.sin6_addr, addr->host, alen);
			addr->port = ntohs(xaddr.sin6_port);
		}
		break;
	}
	default:
		return XI_SOCK_RV_ERR_ARGS;
	}

	if (ret < 0) {
		switch (errno) {
		case EBADF:
			return XI_SOCK_RV_ERR_FD;
		case ENOTSOCK:
			return XI_SOCK_RV_ERR_NOTSOCK;
		case ENOTCONN:
			return XI_SOCK_RV_ERR_NOTCONN;
		default:
			return XI_SOCK_RV_ERR_ARGS;
		}
	}

	return XI_SOCK_RV_OK;
}

xi_sock_re xi_mcast_join(xint32 sfd, xi_sock_addr_t iface, xi_sock_addr_t grp,
		xi_sock_addr_t *src) {
	xg_fd_t *sdesc;

	if (sfd < 0) {
		return XI_SOCK_RV_ERR_ARGS;
	}

	sdesc = xg_fd_get(sfd);

	if (sdesc->desc.s.type != iface.type || iface.type != XI_SOCK_TYPE_DATAGRAM
			|| (iface.family != XI_SOCK_FAMILY_INET && iface.family
					!= XI_SOCK_FAMILY_INET6) || grp.family != iface.family
			|| (src != NULL && src->family != iface.family)) {
		return XI_SOCK_RV_ERR_ARGS;
	}

	if (src == NULL) {
		if (iface.family == XI_SOCK_FAMILY_INET) {
			struct ip_mreq mreq;

			if (!inet_pton(AF_INET, iface.host, &mreq.imr_interface)) {
				return XI_SOCK_RV_ERR_ADDR;
			}
			if (!inet_pton(AF_INET, grp.host, &mreq.imr_multiaddr)) {
				return XI_SOCK_RV_ERR_ADDR;
			}

			if (setsockopt(sfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq,
					sizeof(mreq)) == -1) {
				switch (errno) {
				case EBADF:
					return XI_SOCK_RV_ERR_FD;
				case ENOTSOCK:
					return XI_SOCK_RV_ERR_NOTSOCK;
				case EFAULT:
					return XI_SOCK_RV_ERR_ADDR;
				default:
					return XI_SOCK_RV_ERR_ARGS;
				}
			}
		} else {
			struct ipv6_mreq mreq;

			mreq.ipv6mr_interface = (xuint32) xg_sock_get_if_idx(sfd, iface.host);
			if (!inet_pton(AF_INET6, grp.host, &mreq.ipv6mr_multiaddr)) {
				return XI_SOCK_RV_ERR_ADDR;
			}

			if (setsockopt(sfd, IPPROTO_IPV6, IPV6_JOIN_GROUP, &mreq,
					sizeof(mreq)) == -1) {
				switch (errno) {
				case EBADF:
					return XI_SOCK_RV_ERR_FD;
				case ENOTSOCK:
					return XI_SOCK_RV_ERR_NOTSOCK;
				case EFAULT:
					return XI_SOCK_RV_ERR_ADDR;
				default:
					return XI_SOCK_RV_ERR_ARGS;
				}
			}
		}
	} else {
		return XI_SOCK_RV_ERR_NS;
	}

	return XI_SOCK_RV_OK;
}

xi_sock_re xi_mcast_leave(xint32 sfd, xi_sock_addr_t iface, xi_sock_addr_t grp,
		xi_sock_addr_t *src) {
	if (sfd < 0 || iface.type != XI_SOCK_TYPE_DATAGRAM || (iface.family
			!= XI_SOCK_FAMILY_INET && iface.family != XI_SOCK_FAMILY_INET6)
			|| grp.family != iface.family || (src != NULL && src->family
			!= iface.family)) {
		return XI_SOCK_RV_ERR_ARGS;
	}

	if (src == NULL) {
		if (iface.family == XI_SOCK_FAMILY_INET) {
			struct ip_mreq mreq;

			if (!inet_pton(AF_INET, iface.host, &mreq.imr_interface)) {
				return XI_SOCK_RV_ERR_ADDR;
			}
			if (!inet_pton(AF_INET, grp.host, &mreq.imr_multiaddr)) {
				return XI_SOCK_RV_ERR_ADDR;
			}

			if (setsockopt(sfd, IPPROTO_IP, IP_DROP_MEMBERSHIP, &mreq,
					sizeof(mreq)) == -1) {
				switch (errno) {
				case EBADF:
					return XI_SOCK_RV_ERR_FD;
				case ENOTSOCK:
					return XI_SOCK_RV_ERR_NOTSOCK;
				case EFAULT:
					return XI_SOCK_RV_ERR_ADDR;
				default:
					return XI_SOCK_RV_ERR_ARGS;
				}
			}
		} else {
			struct ipv6_mreq mreq;

			mreq.ipv6mr_interface = (xuint32) xg_sock_get_if_idx(sfd, iface.host);
			if (!inet_pton(AF_INET6, grp.host, &mreq.ipv6mr_multiaddr)) {
				return XI_SOCK_RV_ERR_ADDR;
			}

			if (setsockopt(sfd, IPPROTO_IPV6, IPV6_LEAVE_GROUP, &mreq,
					sizeof(mreq)) == -1) {
				switch (errno) {
				case EBADF:
					return XI_SOCK_RV_ERR_FD;
				case ENOTSOCK:
					return XI_SOCK_RV_ERR_NOTSOCK;
				case EFAULT:
					return XI_SOCK_RV_ERR_ADDR;
				default:
					return XI_SOCK_RV_ERR_ARGS;
				}
			}
		}
	} else {
		return XI_SOCK_RV_ERR_NS;
	}

	return XI_SOCK_RV_OK;
}
