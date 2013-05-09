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

#include <winsock2.h>
#include <ws2tcpip.h>
#include <mswsock.h>

#include "xg_fd.h"

#include "xi/xi_mem.h"
#include "xi/xi_string.h"

// ----------------------------------------------
// Definitions
// ----------------------------------------------

#ifndef IN6ADDRSZ
#define IN6ADDRSZ   16
#endif

#ifndef INT16SZ
#define INT16SZ sizeof(xint16)
#endif

#ifndef INADDRSZ
#define INADDRSZ    4
#endif

#ifndef __P
#define __P(x) x
#endif

#if !defined(EAFNOSUPPORT) && defined(WSAEAFNOSUPPORT)
#define EAFNOSUPPORT WSAEAFNOSUPPORT
#endif

// ----------------------------------------------
// Static Variables
// ----------------------------------------------

static xbool _g_socket_init = FALSE;

// ----------------------------------------------
// XG Functions
// ----------------------------------------------

xint32 xg_sock_family_2pg(xi_sock_family_e family) {
	switch (family) {
	case XI_SOCK_FAMILY_LOCAL:
		return AF_UNIX;
	case XI_SOCK_FAMILY_INET:
		return AF_INET;
	case XI_SOCK_FAMILY_INET6:
		return AF_INET6;
	default:
		return -1;
	}
}

xi_sock_family_e xg_sock_family_2pi(xint32 family) {
	switch (family) {
	case AF_UNIX:
		return XI_SOCK_FAMILY_LOCAL;
	case AF_INET:
		return XI_SOCK_FAMILY_INET;
	case AF_INET6:
		return XI_SOCK_FAMILY_INET6;
	default:
		return -1;
	}
}

xint32 xg_sock_type_2pg(xi_sock_type_e type) {
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

xi_sock_type_e xg_sock_type_2pi(xint32 type) {
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

xint32 xg_sock_proto_2pg(xi_sock_proto_e proto) {
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

xi_sock_proto_e xg_sock_proto_2pi(xint32 proto) {
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

xint32 xg_sock_opt_2pg(xi_sock_opt_e opt) {
	switch (opt) {
	case XI_SOCK_OPT_LINGER:
		return SO_LINGER;
	case XI_SOCK_OPT_KEEPALIVE:
		return SO_KEEPALIVE;
	case XI_SOCK_OPT_DEBUG:
		return SO_DEBUG;
	case XI_SOCK_OPT_NONBLOCK:
		return FIONBIO;
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
		return IPV6_V6ONLY;
	default:
		return -1;
	}
}

xint32 xg_sock_get_if_idx(xint32 sfd, const xchar *ip) {
	UNUSED(sfd);
	UNUSED(ip);
	return 0;
}

static const xchar *xg_inet_ntop4(const xuint8 *src, xchar *dst, xsize size) {
	const xuint32 MIN_SIZE = 16; // space for 255.255.255.255\0
	xint32 n = 0;
	xchar *next = dst;

	if (size < MIN_SIZE) {
		return NULL;
	}

	do {
		xuint8 u = *src++;
		if (u > 99) {
			*next++ = '0' + u / 100;
			u %= 100;
			*next++ = '0' + u / 10;
			u %= 10;
		} else if (u > 9) {
			*next++ = '0' + u / 10;
			u %= 10;
		}
		*next++ = '0' + u;
		*next++ = '.';
		n++;
	} while (n < 4);
	*--next = 0;
	return dst;
}

static const xchar *xg_inet_ntop6(const xuint8 *src, xchar *dst, xuint32 size) {
	/*
	 * Note that int32_t and int16_t need only be "at least" large enough
	 * to contain a value of the specified size.  On some systems, like
	 * Crays, there is no such thing as an integer variable with 16 bits.
	 * Keep this in mind if you think this function should have been coded
	 * to use pointer overlays.  All the world's not a VAX.
	 */
	xchar tmp[sizeof "ffff:ffff:ffff:ffff:ffff:ffff:255.255.255.255"], *tp;
	struct {
		int base, len;
	} best = { -1, 0 }, cur = { -1, 0 };
	xuint32 words[IN6ADDRSZ / INT16SZ];
	xint32 i;
	const xuint8 *next_src, *src_end;
	xuint32 *next_dest;

	/*
	 * Preprocess:
	 *	Copy the input (bytewise) array into a wordwise array.
	 *	Find the longest run of 0x00's in src[] for :: shorthanding.
	 */
	next_src = src;
	src_end = src + IN6ADDRSZ;
	next_dest = words;
	i = 0;
	do {
		xuint32 next_word = (xuint32) *next_src++;
		next_word <<= 8;
		next_word |= (xuint32) *next_src++;
		*next_dest++ = next_word;

		if (next_word == 0) {
			if (cur.base == -1) {
				cur.base = i;
				cur.len = 1;
			} else {
				cur.len++;
			}
		} else {
			if (cur.base != -1) {
				if (best.base == -1 || cur.len > best.len) {
					best = cur;
				}
				cur.base = -1;
			}
		}

		i++;
	} while (next_src < src_end);

	if (cur.base != -1) {
		if (best.base == -1 || cur.len > best.len) {
			best = cur;
		}
	}
	if (best.base != -1 && best.len < 2) {
		best.base = -1;
	}

	/*
	 * Format the result.
	 */
	tp = tmp;
	for (i = 0; i < (xint32)(IN6ADDRSZ / INT16SZ);) {
		/* Are we inside the best run of 0x00's? */
		if (i == best.base) {
			*tp++ = ':';
			i += best.len;
			continue;
		}
		/* Are we following an initial run of 0x00s or any real hex? */
		if (i != 0) {
			*tp++ = ':';
		}
		/* Is this address an encapsulated IPv4? */
		if (i == 6 && best.base == 0 && (best.len == 6 || (best.len == 5
				&& words[5] == 0xffff))) {
			if (!xg_inet_ntop4(src + 12, tp, sizeof tmp - (tp - tmp))) {
				return (NULL);
			}
			tp += strlen(tp);
			break;
		}
		tp += xi_snprintf(tp, sizeof tmp - (tp - tmp), "%x", words[i]);
		i++;
	}
	/* Was it a trailing run of 0x00's? */
	if (best.base != -1 && (best.base + best.len) == (IN6ADDRSZ / INT16SZ)) {
		*tp++ = ':';
	}
	*tp++ = '\0';

	/*
	 * Check for overflow, copy, and we're done.
	 */
	if ((xuint32) (tp - tmp) > size) {
		return (NULL);
	}
	xi_strcpy(dst, tmp);
	return (dst);
}

const xchar *xg_inet_ntop(int af, const xvoid *src, xchar *dst, xuint32 size) {
	switch (af) {
	case AF_INET:
		return (xg_inet_ntop4(src, dst, size));
	case AF_INET6:
		return (xg_inet_ntop6(src, dst, size));
	default:
		errno = WSAEAFNOSUPPORT;
		return (NULL);
	}
}

static xint32 xg_inet_pton4(const xchar *src, xuint8 *dst) {
	static const xchar digits[] = "0123456789";
	xint32 saw_digit, octets, ch;
	xuint8 tmp[INADDRSZ], *tp;

	saw_digit = 0;
	octets = 0;
	*(tp = tmp) = 0;
	while ((ch = *src++) != '\0') {
		const xchar *pch;

		if ((pch = strchr(digits, ch)) != NULL) {
			xuint32 new = *tp * 10 + (xuint32) (pch - digits);

			if (new > 255)
				return (0);
			*tp = new;
			if (!saw_digit) {
				if (++octets > 4)
					return (0);
				saw_digit = 1;
			}
		} else if (ch == '.' && saw_digit) {
			if (octets == 4)
				return (0);
			*++tp = 0;
			saw_digit = 0;
		} else
			return (0);
	}
	if (octets < 4)
		return (0);

	memcpy(dst, tmp, INADDRSZ);
	return (1);
}

static xint32 xg_inet_pton6(const xchar *src, xuint8 *dst) {
	static const xchar xdigits_l[] = "0123456789abcdef", xdigits_u[] =
			"0123456789ABCDEF";
	xuint8 tmp[IN6ADDRSZ], *tp, *endp, *colonp;
	const xchar *xdigits, *curtok;
	xint32 ch, saw_xdigit;
	xuint32 val;

	memset((tp = tmp), '\0', IN6ADDRSZ);
	endp = tp + IN6ADDRSZ;
	colonp = NULL;
	/* Leading :: requires some special handling. */
	if (*src == ':')
		if (*++src != ':')
			return (0);
	curtok = src;
	saw_xdigit = 0;
	val = 0;
	while ((ch = *src++) != '\0') {
		const xchar *pch;

		if ((pch = strchr((xdigits = xdigits_l), ch)) == NULL)
			pch = strchr((xdigits = xdigits_u), ch);
		if (pch != NULL) {
			val <<= 4;
			val |= (pch - xdigits);
			if (val > 0xffff)
				return (0);
			saw_xdigit = 1;
			continue;
		}
		if (ch == ':') {
			curtok = src;
			if (!saw_xdigit) {
				if (colonp)
					return (0);
				colonp = tp;
				continue;
			}
			if (tp + INT16SZ > endp)
				return (0);
			*tp++ = (xuint8) (val >> 8) & 0xff;
			*tp++ = (xuint8) val & 0xff;
			saw_xdigit = 0;
			val = 0;
			continue;
		}
		if (ch == '.' && ((tp + INADDRSZ) <= endp) && xg_inet_pton4(curtok, tp)
				> 0) {
			tp += INADDRSZ;
			saw_xdigit = 0;
			break; /* '\0' was seen by inet_pton4(). */
		}
		return (0);
	}
	if (saw_xdigit) {
		if (tp + INT16SZ > endp)
			return (0);
		*tp++ = (xuint8) (val >> 8) & 0xff;
		*tp++ = (xuint8) val & 0xff;
	}
	if (colonp != NULL) {
		/*
		 * Since some memmove()'s erroneously fail to handle
		 * overlapping regions, we'll do the shift by hand.
		 */
		const xuint32 n = (xuint32)(tp - colonp);
		xuint32 i;

		for (i = 1; i <= n; i++) {
			endp[n - i] = colonp[n - i];
			colonp[n - i] = 0;
		}
		tp = endp;
	}
	if (tp != endp)
		return (0);
	memcpy(dst, tmp, IN6ADDRSZ);
	return (1);
}

xint32 xg_inet_pton(int af, const char *src, void *dst) {
	switch (af) {
	case AF_INET:
		return (xg_inet_pton4(src, dst));
	case AF_INET6:
		return (xg_inet_pton6(src, dst));
	default:
		errno = WSAEAFNOSUPPORT;
		return (-1);
	}
}

// ----------------------------------------------
// XI Functions
// ----------------------------------------------

xint32 xi_socket_open(xi_sock_family_e family, xi_sock_type_e type,
		xi_sock_proto_e proto) {
	xint32 fa = xg_sock_family_2pg(family);
	xint32 ty = xg_sock_type_2pg(type);
	xint32 pr = xg_sock_proto_2pg(proto);
	xg_fd_t pfd;

	if (!_g_socket_init) {
		WSADATA wsaData;
		WSAStartup(MAKEWORD(2, 2), &wsaData);
	}

	if (fa < 0 || ty < 0 || pr < 0) {
		return XI_SOCK_RV_ERR_ARGS;
	}

	pfd.desc.s.fd = socket(fa, ty, pr);
	if (pfd.desc.s.fd == INVALID_SOCKET) {
		return XI_SOCK_RV_ERR_ARGS;
	}

	pfd.type = XG_FD_TYPE_SOCK;
	pfd.desc.s.family = family;
	pfd.desc.s.type = type;
	pfd.desc.s.proto = proto;

	return xg_fd_open(&pfd);
}

xi_sock_re xi_socket_bind(xint32 sfd, xi_sock_addr_t baddr) {
	xint32 ret;
	xg_fd_t *sdesc;

	if (sfd < 0) {
		return XI_SOCK_RV_ERR_ARGS;
	}

	sdesc = xg_fd_get(sfd);

	switch (baddr.family) {
	case XI_SOCK_FAMILY_LOCAL: {
		ret = 0;
		break;
	}
	case XI_SOCK_FAMILY_INET: {
		struct sockaddr_in addr;

		addr.sin_family = xg_sock_family_2pg(baddr.family);
		if (!xg_inet_pton(AF_INET, baddr.host, &addr.sin_addr)) {
			return XI_SOCK_RV_ERR_ADDR;
		}
		addr.sin_port = htons(baddr.port);

		ret = bind(sdesc->desc.s.fd, (struct sockaddr *) &addr, sizeof(addr));
		break;
	}
	case XI_SOCK_FAMILY_INET6: {
		struct sockaddr_in6 addr;

		addr.sin6_family = xg_sock_family_2pg(baddr.family);
		if (!xg_inet_pton(AF_INET6, baddr.host, &addr.sin6_addr)) {
			return XI_SOCK_RV_ERR_ADDR;
		}
		addr.sin6_port = htons(baddr.port);

		ret = bind(sdesc->desc.s.fd, (struct sockaddr *) &addr, sizeof(addr));
		break;
	}
	default:
		return XI_SOCK_RV_ERR_ARGS;
	}

	if (ret == SOCKET_ERROR) {
		switch (WSAGetLastError()) {
		case WSAEACCES:
		case WSAEADDRNOTAVAIL:
			return XI_SOCK_RV_ERR_PERM;
		case WSAEBADF:
			return XI_SOCK_RV_ERR_FD;
		case WSAENOTSOCK:
			return XI_SOCK_RV_ERR_NOTSOCK;
		default:
			return XI_SOCK_RV_ERR_ARGS;
		}
	}

	return XI_SOCK_RV_OK;
}

xi_sock_re xi_socket_listen(xint32 sfd, xint32 backlog) {
	xint32 ret;
	xg_fd_t *sdesc;

	if (sfd < 0) {
		return XI_SOCK_RV_ERR_ARGS;
	}

	sdesc = xg_fd_get(sfd);

	ret = listen(sdesc->desc.s.fd, backlog);
	if (ret == SOCKET_ERROR) {
		switch (WSAGetLastError()) {
		case WSAEBADF:
			return XI_SOCK_RV_ERR_FD;
		case WSAENOTSOCK:
			return XI_SOCK_RV_ERR_NOTSOCK;
		case WSAEADDRINUSE:
			return XI_SOCK_RV_ERR_INUSE;
		case WSAEOPNOTSUPP:
			return XI_SOCK_RV_ERR_NS;
		default:
			return XI_SOCK_RV_ERR_ARGS;
		}
	}

	return XI_SOCK_RV_OK;
}

xint32 xi_socket_accept(xint32 sfd, xi_sock_addr_t *fromaddr) {
	xg_fd_t pfd;
	xg_fd_t *sdesc;

	if (sfd < 0) {
		return XI_SOCK_RV_ERR_ARGS;
	}

	sdesc = xg_fd_get(sfd);

	switch (sdesc->desc.s.family) {
	case XI_SOCK_FAMILY_LOCAL: {
		return XI_SOCK_RV_ERR_NS;
	}
	case XI_SOCK_FAMILY_INET: {
		struct sockaddr_in addr;
		xint32 addrlen = sizeof(addr);

		pfd.desc.s.fd = accept(sdesc->desc.s.fd, (struct sockaddr *) &addr,
				&addrlen);
		if (pfd.desc.s.fd == INVALID_SOCKET) {
			return XI_SOCK_RV_ERR_ARGS;
		}

		fromaddr->family = addr.sin_family;
		fromaddr->type = sdesc->desc.s.type;
		fromaddr->proto = sdesc->desc.s.proto;
		xg_inet_ntop(AF_INET, &addr.sin_addr, fromaddr->host, addrlen);
		fromaddr->port = ntohs(addr.sin_port);

		pfd.type = XG_FD_TYPE_SOCK;
		pfd.desc.s.family = sdesc->desc.s.family;
		pfd.desc.s.type = sdesc->desc.s.type;
		pfd.desc.s.proto = sdesc->desc.s.proto;

		return xg_fd_open(&pfd);
	}
	case XI_SOCK_FAMILY_INET6: {
		struct sockaddr_in6 addr;
		xint32 addrlen = sizeof(addr);

		pfd.desc.s.fd = accept(sdesc->desc.s.fd, (struct sockaddr *) &addr,
				&addrlen);
		if (pfd.desc.s.fd == INVALID_SOCKET) {
			return XI_SOCK_RV_ERR_ARGS;
		}

		fromaddr->family = sdesc->desc.s.family;
		fromaddr->type = sdesc->desc.s.type;
		fromaddr->proto = sdesc->desc.s.proto;
		xg_inet_ntop(AF_INET6, &addr.sin6_addr, fromaddr->host, addrlen);
		fromaddr->port = ntohs(addr.sin6_port);

		pfd.type = XG_FD_TYPE_SOCK;
		pfd.desc.s.family = sdesc->desc.s.family;
		pfd.desc.s.type = sdesc->desc.s.type;
		pfd.desc.s.proto = sdesc->desc.s.proto;

		return xg_fd_open(&pfd);
	}
	default: {
		return XI_SOCK_RV_ERR_NS;
	}
	}
}

xi_sock_re xi_socket_connect(xint32 sfd, xi_sock_addr_t caddr) {
	xint32 ret;
	xg_fd_t *sdesc;

	if (sfd < 0) {
		return XI_SOCK_RV_ERR_ARGS;
	}

	sdesc = xg_fd_get(sfd);

	switch (caddr.family) {
	case XI_SOCK_FAMILY_UNSPEC: {
		struct sockaddr sa;

		xi_mem_set(&sa, 0, sizeof(sa));
		sa.sa_family = AF_UNSPEC;

		ret = connect(sdesc->desc.s.fd, &sa, sizeof(sa));
		break;
	}
	case XI_SOCK_FAMILY_LOCAL: {
		ret = 0;
		break;
	}
	case XI_SOCK_FAMILY_INET: {
		struct sockaddr_in addr;

		addr.sin_family = xg_sock_family_2pg(caddr.family);
		if (!xg_inet_pton(AF_INET, caddr.host, &addr.sin_addr)) {
			return XI_SOCK_RV_ERR_ADDR;
		}
		addr.sin_port = htons(caddr.port);

		ret
				= connect(sdesc->desc.s.fd, (struct sockaddr *) &addr,
						sizeof(addr));
		break;
	}
	case XI_SOCK_FAMILY_INET6: {
		struct sockaddr_in6 addr;

		addr.sin6_family = xg_sock_family_2pg(caddr.family);
		if (!xg_inet_pton(AF_INET6, caddr.host, &addr.sin6_addr)) {
			return XI_SOCK_RV_ERR_ADDR;
		}
		addr.sin6_port = htons(caddr.port);

		ret
				= connect(sdesc->desc.s.fd, (struct sockaddr *) &addr,
						sizeof(addr));
		break;
	}
	default:
		return XI_SOCK_RV_ERR_ARGS;
	}

	if (ret == SOCKET_ERROR) {
		switch (WSAGetLastError()) {
		case WSAEACCES:
			return XI_SOCK_RV_ERR_PERM;
		case WSAEBADF:
			return XI_SOCK_RV_ERR_FD;
		case WSAENOTSOCK:
			return XI_SOCK_RV_ERR_NOTSOCK;
		case WSAEAFNOSUPPORT:
		case WSAEFAULT:
			return XI_SOCK_RV_ERR_ADDR;
		case WSAEADDRINUSE:
			return XI_SOCK_RV_ERR_INUSE;
		case WSAECONNREFUSED:
			return XI_SOCK_RV_ERR_REFUSED;
		case WSAEISCONN:
			return XI_SOCK_RV_ERR_ALREADY;
		case WSAETIMEDOUT:
			return XI_SOCK_RV_ERR_TIMEOUT;
		case WSAEWOULDBLOCK:
			return XI_SOCK_RV_ERR_TRYLATER;
		default:
			return XI_SOCK_RV_ERR_ARGS;
		}
	}

	return XI_SOCK_RV_OK;
}

xi_sock_re xi_socket_opt_set(xint32 sfd, xi_sock_opt_e opt, xint32 val) {
	xint32 ret = 0;
	xg_fd_t *sdesc;

	if (sfd < 0) {
		return XI_SOCK_RV_ERR_ARGS;
	}

	sdesc = xg_fd_get(sfd);

	if (opt == XI_SOCK_OPT_NONBLOCK) {
		ret = ioctlsocket(sdesc->desc.s.fd, FIONBIO, (u_long *) &val);
	} else if (opt == XI_SOCK_OPT_IPV6ONLY) {
		xint32 ropt = xg_sock_opt_2pg(opt);
		if (ropt < 0) {
			return XI_SOCK_RV_ERR_NS;
		}
		ret = setsockopt(sdesc->desc.s.fd, IPPROTO_IPV6, ropt, (const xchar *) &val,
				sizeof(val));
	} else {
		xint32 ropt = xg_sock_opt_2pg(opt);
		if (ropt < 0) {
			return XI_SOCK_RV_ERR_NS;
		}
		ret = setsockopt(sdesc->desc.s.fd, SOL_SOCKET, ropt, (const xchar *) &val,
				sizeof(val));
	}

	if (ret == SOCKET_ERROR) {
		switch (WSAGetLastError()) {
		case WSAEACCES:
			return XI_SOCK_RV_ERR_PERM;
		case WSAEBADF:
			return XI_SOCK_RV_ERR_FD;
		case WSAEINTR:
			return XI_SOCK_RV_ERR_INTR;
		case WSAENOTSOCK:
			return XI_SOCK_RV_ERR_NOTSOCK;
		case WSAENOPROTOOPT:
			return XI_SOCK_RV_ERR_NS;
		default:
			return XI_SOCK_RV_ERR_ARGS;
		}
	}

	return XI_SOCK_RV_OK;
}

xi_sock_re xi_socket_opt_get(xint32 sfd, xi_sock_opt_e opt, xint32 *val) {
	xint32 ret = 0;
	xg_fd_t *sdesc;

	if (sfd < 0) {
		return XI_SOCK_RV_ERR_ARGS;
	}

	sdesc = xg_fd_get(sfd);

	if (opt == XI_SOCK_OPT_IPV6ONLY) {
		xint32 len = 0;
		ret = getsockopt(sdesc->desc.s.fd, IPPROTO_IPV6, xg_sock_opt_2pg(opt),
				(char *) val, &len);
	} else {
		xint32 len = 0;
		xint32 ropt = xg_sock_opt_2pg(opt);
		if (ropt < 0) {
			return XI_SOCK_RV_ERR_NS;
		}
		ret = getsockopt(sdesc->desc.s.fd, SOL_SOCKET, ropt, (char *) val,
						&len);
	}

	if (ret == SOCKET_ERROR) {
		switch (errno) {
		case WSAEACCES:
			return XI_SOCK_RV_ERR_PERM;
		case WSAEBADF:
			return XI_SOCK_RV_ERR_FD;
		case WSAEINTR:
			return XI_SOCK_RV_ERR_INTR;
		case WSAENOTSOCK:
			return XI_SOCK_RV_ERR_NOTSOCK;
		case WSAENOPROTOOPT:
			return XI_SOCK_RV_ERR_NS;
		default:
			return XI_SOCK_RV_ERR_ARGS;
		}
	}

	return XI_SOCK_RV_OK;
}

xssize xi_socket_recv(xint32 sfd, xvoid *buf, xsize blen) {
	xssize ret;
	xg_fd_t *sdesc;
	xint32 wblen = (xint32) blen; 

	if (sfd < 0 || buf == NULL || blen <= 0) {
		return XI_SOCK_RV_ERR_ARGS;
	}

	sdesc = xg_fd_get(sfd);

	ret = recv(sdesc->desc.s.fd, (xchar *)buf, wblen, 0);
	if (ret == SOCKET_ERROR) {
		switch (WSAGetLastError()) {
		case WSAEBADF:
			return XI_SOCK_RV_ERR_FD;
		case WSAENOTSOCK:
			return XI_SOCK_RV_ERR_NOTSOCK;
		case WSAENOTCONN:
			return XI_SOCK_RV_ERR_NOTCONN;
		case WSAECONNREFUSED:
			return XI_SOCK_RV_ERR_REFUSED;
		case WSAEINTR:
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
	xint32 wblen = (xint32) blen;

	if (sfd < 0 || buf == NULL || blen <= 0 || fromaddr == NULL) {
		return XI_SOCK_RV_ERR_ARGS;
	}

	sdesc = xg_fd_get(sfd);

	switch (sdesc->desc.s.family) {
	case XI_SOCK_FAMILY_LOCAL: {
		ret = 0;
		break;
	}
	case XI_SOCK_FAMILY_INET: {
		struct sockaddr_in addr;
		xint32 addrlen = sizeof(addr);

		ret = recvfrom(sdesc->desc.s.fd, (xchar *)buf, wblen, 0,
				(struct sockaddr *) &addr, &addrlen);

		if (ret == 0) {
			fromaddr->family = sdesc->desc.s.family;
			fromaddr->type = sdesc->desc.s.type;
			fromaddr->proto = sdesc->desc.s.proto;
			xg_inet_ntop(AF_INET, &addr.sin_addr, fromaddr->host, addrlen);
			fromaddr->port = ntohs(addr.sin_port);
		}
		break;
	}
	case XI_SOCK_FAMILY_INET6: {
		struct sockaddr_in6 addr;
		xint32 addrlen = sizeof(addr);

		ret = recvfrom(sdesc->desc.s.fd, (xchar *)buf, wblen, 0,
				(struct sockaddr *) &addr, &addrlen);

		if (ret == 0) {
			fromaddr->family = sdesc->desc.s.family;
			fromaddr->type = sdesc->desc.s.type;
			fromaddr->proto = sdesc->desc.s.proto;
			xg_inet_ntop(AF_INET6, &addr.sin6_addr, fromaddr->host, addrlen);
			fromaddr->port = ntohs(addr.sin6_port);
		}
		break;
	}
	default:
		return XI_SOCK_RV_ERR_ARGS;
	}

	if (ret == SOCKET_ERROR) {
		switch (WSAGetLastError()) {
		case WSAEBADF:
			return XI_SOCK_RV_ERR_FD;
		case WSAENOTSOCK:
			return XI_SOCK_RV_ERR_NOTSOCK;
		case WSAENOTCONN:
			return XI_SOCK_RV_ERR_NOTCONN;
		case WSAECONNREFUSED:
			return XI_SOCK_RV_ERR_REFUSED;
		case WSAEINTR:
			return XI_SOCK_RV_ERR_INTR;
		default:
			return XI_SOCK_RV_ERR_ARGS;
		}
	}

	return ret;
}

xssize xi_socket_send(xint32 sfd, const xvoid *buf, xsize blen) {
	xssize ret;
	xg_fd_t *sdesc;
	xint32 wblen = (xint32) blen;

	if (sfd < 0 || buf == NULL || blen <= 0) {
		return XI_SOCK_RV_ERR_ARGS;
	}

	sdesc = xg_fd_get(sfd);

	ret = send(sdesc->desc.s.fd, (xchar *)buf, wblen, 0);
	if (ret == SOCKET_ERROR) {
		switch (WSAGetLastError()) {
		case WSAEBADF:
			return XI_SOCK_RV_ERR_FD;
		case WSAENOTSOCK:
			return XI_SOCK_RV_ERR_NOTSOCK;
		case WSAENOTCONN:
			return XI_SOCK_RV_ERR_NOTCONN;
		case WSAECONNREFUSED:
			return XI_SOCK_RV_ERR_REFUSED;
		case WSAENOBUFS:
			return XI_SOCK_RV_ERR_OVERFLOW;
		case WSAEOPNOTSUPP:
			return XI_SOCK_RV_ERR_NS;
		case WSAEDESTADDRREQ:
			return XI_SOCK_RV_ERR_ADDR;
		case WSAEINTR:
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
	xint32 wblen = (xint32) blen;

	if (sfd < 0 || buf == NULL || blen <= 0) {
		return XI_SOCK_RV_ERR_ARGS;
	}

	sdesc = xg_fd_get(sfd);

	switch (sdesc->desc.s.family) {
	case XI_SOCK_FAMILY_LOCAL: {
		ret = 0;
		break;
	}
	case XI_SOCK_FAMILY_INET: {
		struct sockaddr_in addr;
		xint32 alen = sizeof(addr);

		addr.sin_family = xg_sock_family_2pg(toaddr.family);
		if (!xg_inet_pton(AF_INET, toaddr.host, &addr.sin_addr)) {
			return XI_SOCK_RV_ERR_ADDR;
		}
		addr.sin_port = htons(toaddr.port);

		ret = sendto(sdesc->desc.s.fd, (xchar*)buf, wblen, 0, (struct sockaddr *) &addr,
				alen);
		break;
	}
	case XI_SOCK_FAMILY_INET6: {
		struct sockaddr_in6 addr;
		xint32 alen = sizeof(addr);

		addr.sin6_family = xg_sock_family_2pg(toaddr.family);
		if (!xg_inet_pton(AF_INET6, toaddr.host, &addr.sin6_addr)) {
			return XI_SOCK_RV_ERR_ADDR;
		}
		addr.sin6_port = htons(toaddr.port);

		ret = sendto(sdesc->desc.s.fd, (xchar*)buf, wblen, 0, (struct sockaddr *) &addr,
				alen);
		break;
	}
	default:
		return XI_SOCK_RV_ERR_ARGS;
	}

	if (ret == SOCKET_ERROR) {
		switch (WSAGetLastError()) {
		case WSAEBADF:
			return XI_SOCK_RV_ERR_FD;
		case WSAENOTSOCK:
			return XI_SOCK_RV_ERR_NOTSOCK;
		case WSAENOTCONN:
			return XI_SOCK_RV_ERR_NOTCONN;
		case WSAECONNREFUSED:
			return XI_SOCK_RV_ERR_REFUSED;
		case WSAENOBUFS:
			return XI_SOCK_RV_ERR_OVERFLOW;
		case WSAEOPNOTSUPP:
			return XI_SOCK_RV_ERR_NS;
		case WSAEDESTADDRREQ:
			return XI_SOCK_RV_ERR_ADDR;
		case WSAEINTR:
			return XI_SOCK_RV_ERR_INTR;
		default:
			return XI_SOCK_RV_ERR_ARGS;
		}
	}

	return ret;
}

xssize xi_socket_sendfile(xint32 sfd, xint32 ffd, xoff64 *offset, xsize count) {
	xg_fd_t *sdesc;
	xg_fd_t *fdesc;
	DWORD wcnt = (DWORD) count;

	if (sfd < 0 || ffd < 0 || offset == NULL || count <= 0) {
		return XI_SOCK_RV_ERR_ARGS;
	}

	sdesc = xg_fd_get(sfd);
	fdesc = xg_fd_get(ffd);

	xi_file_seek(ffd, (*offset), XI_FILE_SEEK_CUR);
	if (!TransmitFile(sdesc->desc.s.fd, fdesc->desc.f.fd, wcnt, 0, NULL, NULL,
			0)) {
		return -1;
	}

	return count;
}

xi_sock_re xi_socket_shutdown(xint32 sfd, xi_sock_shutdown_e how) {
	xint32 ret;
	xint32 whow = 0;
	xg_fd_t *sdesc;

	if (sfd < 0) {
		return XI_SOCK_RV_ERR_ARGS;
	}

	sdesc = xg_fd_get(sfd);

	switch (how) {
	case XI_SOCK_SHUTDOWN_RD:
		whow = SD_RECEIVE;
		break;
	case XI_SOCK_SHUTDOWN_WR:
		whow = SD_SEND;
		break;
	case XI_SOCK_SHUTDOWN_RDWR:
		whow = SD_BOTH;
		break;
	default:
		return XI_SOCK_RV_ERR_ARGS;
	}

	ret = shutdown(sdesc->desc.s.fd, whow);
	if (ret == SOCKET_ERROR) {
		return XI_SOCK_RV_ERR_ARGS;
	}

	return XI_SOCK_RV_OK;
}

xi_sock_re xi_socket_close(xint32 sfd) {
	xint32 ret;
	xg_fd_t *sdesc;

	if (sfd < 0) {
		return XI_SOCK_RV_ERR_ARGS;
	}

	sdesc = xg_fd_get(sfd);

	ret = closesocket(sdesc->desc.s.fd);
	if (ret == SOCKET_ERROR) {
		return XI_SOCK_RV_ERR_FD;
	}

	ret = xg_fd_close(sfd);

	return (ret == 0) ? XI_SOCK_RV_OK : XI_SOCK_RV_ERR_FD;
}

xint32 xi_socket_get_hostname(xchar *buf, xint32 buflen) {
	xint32 ret;

	ret = gethostname(buf, buflen);
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

	if (!_g_socket_init) {
		WSADATA wsaData;
		WSAStartup(MAKEWORD(2, 2), &wsaData);
	}

	hosts = gethostbyname(hostname);
	if (hosts == NULL) {
		switch (WSAGetLastError()) {
		case WSAHOST_NOT_FOUND:
			return XI_SOCK_RV_ERR_HOST_NF;
		case WSANO_ADDRESS:
			return XI_SOCK_RV_ERR_NOADDR;
		case WSATRY_AGAIN:
			return XI_SOCK_RV_ERR_TRYLATER;
		case WSANO_RECOVERY:
		default:
			return XI_SOCK_RV_ERR_ARGS;
		}
	}

	if (hosts->h_addrtype == AF_INET) {
		struct sockaddr_in **paddr = (struct sockaddr_in **) hosts->h_addr_list;
		for (i = 0; paddr[i] != NULL && i < addrlen; i++) {
			xg_inet_ntop(AF_INET, paddr[i], addr[i].host,
					sizeof(struct sockaddr_in));
		}
	} else if (hosts->h_addrtype == AF_INET6) {
		struct sockaddr_in6 **paddr =
				(struct sockaddr_in6 **) hosts->h_addr_list;
		for (i = 0; paddr[i] != NULL && i < addrlen; i++) {
			xg_inet_ntop(AF_INET6, paddr[i], addr[i].host,
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
		ret = 0;
		break;
	}
	case XI_SOCK_FAMILY_INET: {
		struct sockaddr_in xaddr;
		xint32 alen = sizeof(xaddr);

		ret = getpeername(sdesc->desc.s.fd, (struct sockaddr *) &xaddr, &alen);
		if (ret == 0) {
			addr->family = sdesc->desc.s.family;
			addr->type = sdesc->desc.s.type;
			addr->proto = sdesc->desc.s.proto;
			xg_inet_ntop(AF_INET, &xaddr.sin_addr, addr->host, alen);
			addr->port = ntohs(xaddr.sin_port);
		}
		break;
	}
	case XI_SOCK_FAMILY_INET6: {
		struct sockaddr_in6 xaddr;
		xint32 alen = sizeof(xaddr);

		ret = getpeername(sdesc->desc.s.fd, (struct sockaddr *) &xaddr, &alen);
		if (ret == 0) {
			addr->family = sdesc->desc.s.family;
			addr->type = sdesc->desc.s.type;
			addr->proto = sdesc->desc.s.proto;
			xg_inet_ntop(AF_INET6, &xaddr.sin6_addr, addr->host, alen);
			addr->port = ntohs(xaddr.sin6_port);
		}
		break;
	}
	default:
		return XI_SOCK_RV_ERR_ARGS;
	}

	if (ret == SOCKET_ERROR) {
		switch (WSAGetLastError()) {
		case WSAEBADF:
			return XI_SOCK_RV_ERR_FD;
		case WSAENOTSOCK:
			return XI_SOCK_RV_ERR_NOTSOCK;
		case WSAENOTCONN:
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
		ret = 0;
		break;
	}
	case XI_SOCK_FAMILY_INET: {
		struct sockaddr_in xaddr;
		xint32 alen = sizeof(xaddr);

		ret = getsockname(sdesc->desc.s.fd, (struct sockaddr *) &xaddr, &alen);
		if (ret == 0) {
			addr->family = sdesc->desc.s.family;
			addr->type = sdesc->desc.s.type;
			addr->proto = sdesc->desc.s.proto;
			xg_inet_ntop(AF_INET, &xaddr.sin_addr, addr->host, alen);
			addr->port = ntohs(xaddr.sin_port);
		}
		break;
	}
	case XI_SOCK_FAMILY_INET6: {
		struct sockaddr_in6 xaddr;
		xint32 alen = sizeof(xaddr);

		ret = getsockname(sdesc->desc.s.fd, (struct sockaddr *) &xaddr, &alen);
		if (ret == 0) {
			addr->family = sdesc->desc.s.family;
			addr->type = sdesc->desc.s.type;
			addr->proto = sdesc->desc.s.proto;
			xg_inet_ntop(AF_INET6, &xaddr.sin6_addr, addr->host, alen);
			addr->port = ntohs(xaddr.sin6_port);
		}
		break;
	}
	default:
		return XI_SOCK_RV_ERR_ARGS;
	}

	if (ret == SOCKET_ERROR) {
		switch (WSAGetLastError()) {
		case WSAEBADF:
			return XI_SOCK_RV_ERR_FD;
		case WSAENOTSOCK:
			return XI_SOCK_RV_ERR_NOTSOCK;
		case WSAENOTCONN:
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

#if !defined(__MINGW32__) && !defined(__MINGW64__)
	if (src == NULL) {
#endif
		if (iface.family == XI_SOCK_FAMILY_INET) {
			struct ip_mreq mreq;

			if (!xg_inet_pton(AF_INET, iface.host, &mreq.imr_interface)) {
				return XI_SOCK_RV_ERR_ADDR;
			}
			if (!xg_inet_pton(AF_INET, grp.host, &mreq.imr_multiaddr)) {
				return XI_SOCK_RV_ERR_ADDR;
			}

			if (setsockopt(sdesc->desc.s.fd, IPPROTO_IP, IP_ADD_MEMBERSHIP,
					(char *) &mreq, sizeof(mreq)) == SOCKET_ERROR) {
				switch (WSAGetLastError()) {
				case WSAEBADF:
					return XI_SOCK_RV_ERR_FD;
				case WSAENOTSOCK:
					return XI_SOCK_RV_ERR_NOTSOCK;
				case WSAEFAULT:
					return XI_SOCK_RV_ERR_ADDR;
				default:
					return XI_SOCK_RV_ERR_ARGS;
				}
			}
		} else {
			struct ipv6_mreq mreq;

			mreq.ipv6mr_interface = xg_sock_get_if_idx(sfd, iface.host);
			if (!xg_inet_pton(AF_INET6, grp.host, &mreq.ipv6mr_multiaddr)) {
				return XI_SOCK_RV_ERR_ADDR;
			}

			if (setsockopt(sdesc->desc.s.fd, IPPROTO_IPV6, IPV6_JOIN_GROUP,
					(char *) &mreq, sizeof(mreq)) == SOCKET_ERROR) {
				switch (WSAGetLastError()) {
				case WSAEBADF:
					return XI_SOCK_RV_ERR_FD;
				case WSAENOTSOCK:
					return XI_SOCK_RV_ERR_NOTSOCK;
				case WSAEFAULT:
					return XI_SOCK_RV_ERR_ADDR;
				default:
					return XI_SOCK_RV_ERR_ARGS;
				}
			}
		}
#if !defined(__MINGW32__) && !defined(__MINGW64__)
	} else { // src != NULL
		struct group_source_req mreq;

		if (iface.family == XI_SOCK_FAMILY_INET) {
			struct sockaddr_in grpaddr;
			struct sockaddr_in srcaddr;

			mreq.gsr_interface = xg_sock_get_if_idx(sfd, iface.host);

			grpaddr.sin_family = AF_INET;
			if (!xg_inet_pton(AF_INET, grp.host, &grpaddr.sin_addr)) {
				return XI_SOCK_RV_ERR_ADDR;
			}
			grpaddr.sin_port = htons(grp.port);

			srcaddr.sin_family = AF_INET;
			if (!xg_inet_pton(AF_INET, src->host, &srcaddr.sin_addr)) {
				return XI_SOCK_RV_ERR_ADDR;
			}
			srcaddr.sin_port = htons(src->port);

			xi_mem_copy(&mreq.gsr_group, &grpaddr, sizeof(grpaddr));
			xi_mem_copy(&mreq.gsr_source, &srcaddr, sizeof(srcaddr));

			if (setsockopt(sdesc->desc.s.fd, IPPROTO_IP,
					MCAST_JOIN_SOURCE_GROUP, (char *) &mreq, sizeof(mreq))
					== SOCKET_ERROR) {
				switch (WSAGetLastError()) {
				case WSAEBADF:
					return XI_SOCK_RV_ERR_FD;
				case WSAENOTSOCK:
					return XI_SOCK_RV_ERR_NOTSOCK;
				case WSAEFAULT:
					return XI_SOCK_RV_ERR_ADDR;
				default:
					return XI_SOCK_RV_ERR_ARGS;
				}
			}
		} else {
			struct sockaddr_in6 grpaddr;
			struct sockaddr_in6 srcaddr;

			mreq.gsr_interface = xg_sock_get_if_idx(sfd, iface.host);

			grpaddr.sin6_family = AF_INET6;
			if (!xg_inet_pton(AF_INET6, grp.host, &grpaddr.sin6_addr)) {
				return XI_SOCK_RV_ERR_ADDR;
			}
			grpaddr.sin6_port = htons(grp.port);

			srcaddr.sin6_family = AF_INET6;
			if (!xg_inet_pton(AF_INET6, src->host, &srcaddr.sin6_addr)) {
				return XI_SOCK_RV_ERR_ADDR;
			}
			srcaddr.sin6_port = htons(src->port);

			xi_mem_copy(&mreq.gsr_group, &grpaddr, sizeof(grpaddr));
			xi_mem_copy(&mreq.gsr_source, &srcaddr, sizeof(srcaddr));

			if (setsockopt(sdesc->desc.s.fd, IPPROTO_IPV6,
					MCAST_JOIN_SOURCE_GROUP, (char *) &mreq, sizeof(mreq))
					== SOCKET_ERROR) {
				switch (WSAGetLastError()) {
				case WSAEBADF:
					return XI_SOCK_RV_ERR_FD;
				case WSAENOTSOCK:
					return XI_SOCK_RV_ERR_NOTSOCK;
				case WSAEFAULT:
					return XI_SOCK_RV_ERR_ADDR;
				default:
					return XI_SOCK_RV_ERR_ARGS;
				}
			}
		}
	}
#endif

	return XI_SOCK_RV_OK;
}

xi_sock_re xi_mcast_leave(xint32 sfd, xi_sock_addr_t iface, xi_sock_addr_t grp,
		xi_sock_addr_t *src) {
	xg_fd_t *sdesc;

	if (sfd < 0 || iface.type != XI_SOCK_TYPE_DATAGRAM || (iface.family
			!= XI_SOCK_FAMILY_INET && iface.family != XI_SOCK_FAMILY_INET6)
			|| grp.family != iface.family || (src != NULL && src->family
			!= iface.family)) {
		return XI_SOCK_RV_ERR_ARGS;
	}

	sdesc = xg_fd_get(sfd);

#if !defined(__MINGW32__) && !defined(__MINGW64__)
	if (src == NULL) {
#endif
		if (iface.family == XI_SOCK_FAMILY_INET) {
			struct ip_mreq mreq;

			if (!xg_inet_pton(AF_INET, iface.host, &mreq.imr_interface)) {
				return XI_SOCK_RV_ERR_ADDR;
			}
			if (!xg_inet_pton(AF_INET, grp.host, &mreq.imr_multiaddr)) {
				return XI_SOCK_RV_ERR_ADDR;
			}

			if (setsockopt(sdesc->desc.s.fd, IPPROTO_IP, IP_DROP_MEMBERSHIP,
					(xchar*) &mreq, sizeof(mreq)) == SOCKET_ERROR) {
				switch (WSAGetLastError()) {
				case WSAEBADF:
					return XI_SOCK_RV_ERR_FD;
				case WSAENOTSOCK:
					return XI_SOCK_RV_ERR_NOTSOCK;
				case WSAEFAULT:
					return XI_SOCK_RV_ERR_ADDR;
				default:
					return XI_SOCK_RV_ERR_ARGS;
				}
			}
		} else {
			struct ipv6_mreq mreq;

			mreq.ipv6mr_interface = xg_sock_get_if_idx(sfd, iface.host);
			if (!xg_inet_pton(AF_INET6, grp.host, &mreq.ipv6mr_multiaddr)) {
				return XI_SOCK_RV_ERR_ADDR;
			}

			if (setsockopt(sdesc->desc.s.fd, IPPROTO_IPV6, IPV6_LEAVE_GROUP,
					(xchar*) &mreq, sizeof(mreq)) == SOCKET_ERROR) {
				switch (WSAGetLastError()) {
				case WSAEBADF:
					return XI_SOCK_RV_ERR_FD;
				case WSAENOTSOCK:
					return XI_SOCK_RV_ERR_NOTSOCK;
				case WSAEFAULT:
					return XI_SOCK_RV_ERR_ADDR;
				default:
					return XI_SOCK_RV_ERR_ARGS;
				}
			}
		}
#if !defined(__MINGW32__) && !defined(__MINGW64__)
	} else {
		struct group_source_req mreq;

		if (iface.family == XI_SOCK_FAMILY_INET) {
			struct sockaddr_in grpaddr;
			struct sockaddr_in srcaddr;

			mreq.gsr_interface = xg_sock_get_if_idx(sfd, iface.host);

			grpaddr.sin_family = AF_INET;
			if (!xg_inet_pton(AF_INET, grp.host, &grpaddr.sin_addr)) {
				return XI_SOCK_RV_ERR_ADDR;
			}
			grpaddr.sin_port = htons(grp.port);

			srcaddr.sin_family = AF_INET;
			if (!xg_inet_pton(AF_INET, src->host, &srcaddr.sin_addr)) {
				return XI_SOCK_RV_ERR_ADDR;
			}
			srcaddr.sin_port = htons(src->port);

			xi_mem_copy(&mreq.gsr_group, &grpaddr, sizeof(grpaddr));
			xi_mem_copy(&mreq.gsr_source, &srcaddr, sizeof(srcaddr));

			if (setsockopt(sdesc->desc.s.fd, IPPROTO_IP,
					MCAST_LEAVE_SOURCE_GROUP, (xchar*) &mreq, sizeof(mreq))
					== SOCKET_ERROR) {
				switch (WSAGetLastError()) {
				case WSAEBADF:
					return XI_SOCK_RV_ERR_FD;
				case WSAENOTSOCK:
					return XI_SOCK_RV_ERR_NOTSOCK;
				case WSAEFAULT:
					return XI_SOCK_RV_ERR_ADDR;
				default:
					return XI_SOCK_RV_ERR_ARGS;
				}
			}
		} else {
			struct sockaddr_in6 grpaddr;
			struct sockaddr_in6 srcaddr;

			mreq.gsr_interface = xg_sock_get_if_idx(sfd, iface.host);

			grpaddr.sin6_family = AF_INET6;
			if (!xg_inet_pton(AF_INET6, grp.host, &grpaddr.sin6_addr)) {
				return XI_SOCK_RV_ERR_ADDR;
			}
			grpaddr.sin6_port = htons(grp.port);

			srcaddr.sin6_family = AF_INET6;
			if (!xg_inet_pton(AF_INET6, src->host, &srcaddr.sin6_addr)) {
				return XI_SOCK_RV_ERR_ADDR;
			}
			srcaddr.sin6_port = htons(src->port);

			xi_mem_copy(&mreq.gsr_group, &grpaddr, sizeof(grpaddr));
			xi_mem_copy(&mreq.gsr_source, &srcaddr, sizeof(srcaddr));

			if (setsockopt(sdesc->desc.s.fd, IPPROTO_IPV6,
					MCAST_LEAVE_SOURCE_GROUP, (char *) &mreq, sizeof(mreq))
					== -1) {
				switch (WSAGetLastError()) {
				case WSAEBADF:
					return XI_SOCK_RV_ERR_FD;
				case WSAENOTSOCK:
					return XI_SOCK_RV_ERR_NOTSOCK;
				case WSAEFAULT:
					return XI_SOCK_RV_ERR_ADDR;
				default:
					return XI_SOCK_RV_ERR_ARGS;
				}
			}
		}
	}
#endif

	return XI_SOCK_RV_OK;
}
