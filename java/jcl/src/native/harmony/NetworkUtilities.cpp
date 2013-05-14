/*
 * Copyright (C) 2010 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define LOG_TAG "NetworkUtilities"

#include "NetworkUtilities.h"
#include "JNIHelp.h"
#include "JniConstants.h"

// by cmjo
//#include <arpa/inet.h>
//#include <fcntl.h>
//#include <stdio.h>
//#include <string.h>
#include "xi/xi_mem.h"
#include "xi/xi_string.h"

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

#if 0 // not used
static const xchar *xg_inet_ntop4(const xuint8 *src, xchar *dst, xuint32 size) {
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
	}while (n < 4);
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
	}best = {-1, 0}, cur = {-1, 0};
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
	}while (next_src < src_end);

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
	for (i = 0; i < (xint32) (IN6ADDRSZ / INT16SZ);) {
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
			tp += xi_strlen(tp);
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
#endif // 0 - not used
static xint32 xg_inet_pton4(const xchar *src, xuint8 *dst) {
	static const xchar digits[] = "0123456789";
	xint32 saw_digit, octets, ch;
	xuint8 tmp[INADDRSZ], *tp;

	saw_digit = 0;
	octets = 0;
	*(tp = tmp) = 0;
	while ((ch = *src++) != '\0') {
		const xchar *pch;

		if ((pch = xi_strchr(digits, ch)) != NULL) {
			xuint32 newaddr = *tp * 10 + (xuint32) (pch - digits);

			if (newaddr > 255)
				return (0);
			*tp = newaddr;
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

	xi_mem_copy(dst, tmp, INADDRSZ);
	return (1);
}

static xint32 xg_inet_pton6(const xchar *src, xuint8 *dst) {
	static const xchar xdigits_l[] = "0123456789abcdef", xdigits_u[] =
			"0123456789ABCDEF";
	xuint8 tmp[IN6ADDRSZ], *tp, *endp, *colonp;
	const xchar *xdigits, *curtok;
	xint32 ch, saw_xdigit;
	xuint32 val;

	xi_mem_set((tp = tmp), '\0', IN6ADDRSZ);
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

		if ((pch = xi_strchr((xdigits = xdigits_l), ch)) == NULL)
			pch = xi_strchr((xdigits = xdigits_u), ch);
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
		const xuint32 n = tp - colonp;
		xuint32 i;

		for (i = 1; i <= n; i++) {
			endp[n - i] = colonp[n - i];
			colonp[n - i] = 0;
		}
		tp = endp;
	}
	if (tp != endp)
		return (0);
	xi_mem_copy(dst, tmp, IN6ADDRSZ);
	return (1);
}

bool byteArrayToSocketAddress(JNIEnv* env, jclass, jbyteArray byteArray,
		int port, xi_sock_addr_t * ss) {
	if (byteArray == NULL) {
		jniThrowNullPointerException(env, NULL);
		return false;
	}

	// Convert the IP address bytes to the proper IP address type.
	xsize addressLength = env->GetArrayLength(byteArray);
	xi_mem_set(ss, 0, sizeof(*ss));
	if (addressLength == 4) {
		// IPv4 address.
		xuint8 dst[4];
		jbyte *dstptr = (jbyte *) dst;
		env->GetByteArrayRegion(byteArray, 0, 4, dstptr);
		ss->family = XI_SOCK_FAMILY_INET;
		ss->proto = XI_SOCK_PROTO_IP;
		ss->port = port;
		xi_mem_set(ss->host, 0, sizeof(ss->host));
		xi_sprintf(ss->host, "%d.%d.%d.%d", dst[0], dst[1], dst[2], dst[3]);
		//		sockaddr_in* sin = reinterpret_cast<sockaddr_in*> (ss);
		//		sin->sin_family = AF_INET;
		//		sin->sin_port = htons(port);
		//		jbyte* dst = reinterpret_cast<jbyte*> (&sin->sin_addr.s_addr);
		//		env->GetByteArrayRegion(byteArray, 0, 4, dst);
	} else if (addressLength == 16) {
		// IPv6 address.
		jbyte dst[16];
		env->GetByteArrayRegion(byteArray, 0, 16, dst);
		ss->family = XI_SOCK_FAMILY_INET6;
		ss->proto = XI_SOCK_PROTO_IP;
		ss->port = port;
		xi_mem_set(ss->host, 0, sizeof(ss->host));
		xi_sprintf(
				ss->host,
				"%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x",
				dst[0], dst[1], dst[2], dst[3], dst[4], dst[5], dst[6], dst[7],
				dst[8], dst[9], dst[10], dst[11], dst[12], dst[13], dst[14],
				dst[15]);
		//		sockaddr_in6* sin6 = reinterpret_cast<sockaddr_in6*> (ss);
		//		sin6->sin6_family = AF_INET6;
		//		sin6->sin6_port = htons(port);
		//		jbyte* dst = reinterpret_cast<jbyte*> (&sin6->sin6_addr.s6_addr);
		//		env->GetByteArrayRegion(byteArray, 0, 16, dst);
	} else {
		// We can't throw SocketException. We aren't meant to see bad addresses, so seeing one
		// really does imply an internal error.
		// TODO: fix the code (native and Java) so we don't paint ourselves into this corner.
		char buf[64];
		xi_snprintf(buf, sizeof(buf),
				"byteArrayToSocketAddress bad array length (%i)", addressLength);
		jniThrowException(env, "java/lang/IllegalArgumentException", buf);
		return false;
	}
	return true;
}

jbyteArray socketAddressToByteArray(JNIEnv* env, xi_sock_addr_t* ss) {
	xuint8 naddr[32];
	xint32 addressLength = 0;

	// Avoiding Error!!
	if (ss->family == 0) {
		ss->family = XI_SOCK_FAMILY_INET;
	}

	xi_mem_set(naddr, 0, sizeof(naddr));
	if (ss->family == XI_SOCK_FAMILY_INET) {
		xg_inet_pton4(ss->host, naddr);
		addressLength = 4;
	} else if (ss->family == XI_SOCK_FAMILY_INET6) {
		xg_inet_pton6(ss->host, naddr);
		addressLength = 16;
	} else {
		// We can't throw SocketException. We aren't meant to see bad addresses, so seeing one
		// really does imply an internal error.
		// TODO: fix the code (native and Java) so we don't paint ourselves into this corner.
		char buf[64];
		xi_snprintf(buf, sizeof(buf),
				"socketAddressToByteArray bad ss_family (%d)", ss->family);
		jniThrowException(env, "java/lang/IllegalArgumentException", buf);
		return NULL;
	}

	jbyteArray byteArray = env->NewByteArray(addressLength);
	if (byteArray == NULL) {
		return NULL;
	}
	env->SetByteArrayRegion(byteArray, 0, addressLength,
			reinterpret_cast<jbyte*> (naddr));

	return byteArray;
}

jobject byteArrayToInetAddress(JNIEnv* env, jbyteArray byteArray) {
	if (byteArray == NULL) {
		return NULL;
	}
	jclass inetAddressClass = env->FindClass("java/net/InetAddress");
	jmethodID getByAddressMethod = env->GetStaticMethodID(inetAddressClass,
			"getByAddress", "([B)Ljava/net/InetAddress;");
	if (getByAddressMethod == NULL) {
		return NULL;
	}
	return env->CallStaticObjectMethod(inetAddressClass, getByAddressMethod,
			byteArray);
}

jobject socketAddressToInetAddress(JNIEnv* env, xi_sock_addr_t* ss) {
	jbyteArray byteArray = socketAddressToByteArray(env, ss);
	return byteArrayToInetAddress(env, byteArray);
}

bool setBlocking(int fd, bool blocking) {
	xint32 rc = 0;
	xint32 block = (!blocking) ? 1 : 0;

	log_trace(XDLOG, "fd=%d / blocking=%d\n", fd, block);

	rc = xi_socket_opt_set(fd, XI_SOCK_OPT_NONBLOCK, block);
	return (rc == 0);

	//	int flags = fcntl(fd, F_GETFL);
	//	if (flags == -1) {
	//		return false;
	//	}
	//
	//	if (!blocking) {
	//		flags |= O_NONBLOCK;
	//	} else {
	//		flags &= ~O_NONBLOCK;
	//	}
	//
	//	int rc = fcntl(fd, F_SETFL, flags);
	//	return (rc != -1);
}
