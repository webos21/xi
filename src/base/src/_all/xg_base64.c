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
 * File : xg_base64.c
 */

#include "xi/xi_base64.h"

// ----------------------------------------------
// Global Variables
// ----------------------------------------------

static const xchar _g_basis_64[] =
		"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static const xuint8 _g_b64_pr2six[256] = { 64, 64, 64, 64, 64, 64, 64, 64, 64,
		64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
		64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 62, 64,
		64, 64, 63, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 64, 64, 64, 64, 64,
		64, 64, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17,
		18, 19, 20, 21, 22, 23, 24, 25, 64, 64, 64, 64, 64, 64, 26, 27, 28, 29,
		30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47,
		48, 49, 50, 51, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
		64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
		64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
		64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
		64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
		64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
		64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
		64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64 };

// ----------------------------------------------
// XI Functions
// ----------------------------------------------

xssize xi_base64_encode_len(xsize len) {
	return (xssize)((len + 2) / 3 * 4) + 1;
}

xssize xi_base64_encode(xchar * coded_dst, const xchar *plain_src,
		xsize len_plain_src) {
	xsize i;
	xchar *p;

	p = coded_dst;
	for (i = 0; i < len_plain_src - 2; i += 3) {
		*p++ = _g_basis_64[(plain_src[i] >> 2) & 0x3F];
		*p++ = _g_basis_64[((plain_src[i] & 0x3) << 4) | ((xint32) (plain_src[i
				+ 1] & 0xF0) >> 4)];
		*p++ = _g_basis_64[((plain_src[i + 1] & 0xF) << 2)
				| ((xint32) (plain_src[i + 2] & 0xC0) >> 6)];
		*p++ = _g_basis_64[plain_src[i + 2] & 0x3F];
	}
	if (i < len_plain_src) {
		*p++ = _g_basis_64[(plain_src[i] >> 2) & 0x3F];
		if (i == (len_plain_src - 1)) {
			*p++ = _g_basis_64[((plain_src[i] & 0x3) << 4)];
			*p++ = '=';
		} else {
			*p++ = _g_basis_64[((plain_src[i] & 0x3) << 4)
					| ((xint32) (plain_src[i + 1] & 0xF0) >> 4)];
			*p++ = _g_basis_64[((plain_src[i + 1] & 0xF) << 2)];
		}
		*p++ = '=';
	}

	*p++ = '\0';
	return (xssize) (p - coded_dst);
}

xssize xi_base64_decode_len(const xchar * coded_src) {
	xssize nbytesdecoded;
	register const xuint8 *bufin;
	register xsize nprbytes;

	bufin = (const xuint8 *) coded_src;
	while (_g_b64_pr2six[*(bufin++)] <= 63) {
		// Nothing to do
	}

	nprbytes = (xsize)(bufin - (const xuint8 *) coded_src) - 1;
	nbytesdecoded = (((xssize) nprbytes + 3) / 4) * 3;

	return nbytesdecoded + 1;
}

xssize xi_base64_decode(xchar * plain_dst, const xchar *coded_src) {
	xssize len = xi_base64_decode_binary((xuint8 *) plain_dst, coded_src);
	plain_dst[len] = '\0';
	return len;
}

xssize xi_base64_decode_binary(xuint8 * plain_dst, const xchar *coded_src) {
	xssize nbytesdecoded;
	register const xuint8 *bufin;
	register xuint8 *bufout;
	register xsize nprbytes;

	bufin = (const xuint8 *) coded_src;
	while (_g_b64_pr2six[*(bufin++)] <= 63) {
		// do nothing
	}

	nprbytes = (xsize)(bufin - (const xuint8 *) coded_src) - 1;
	nbytesdecoded = (((xssize) nprbytes + 3) / 4) * 3;

	bufout = (xuint8 *) plain_dst;
	bufin = (const xuint8 *) coded_src;

	while (nprbytes > 4) {
		*(bufout++) = (xuint8) (_g_b64_pr2six[*bufin] << 2
				| _g_b64_pr2six[bufin[1]] >> 4);
		*(bufout++) = (xuint8) (_g_b64_pr2six[bufin[1]] << 4
				| _g_b64_pr2six[bufin[2]] >> 2);
		*(bufout++) = (xuint8) (_g_b64_pr2six[bufin[2]] << 6
				| _g_b64_pr2six[bufin[3]]);
		bufin += 4;
		nprbytes -= 4;
	}

	// Note: (nprbytes == 1) would be an error, so just ignore that case
	if (nprbytes > 1) {
		*(bufout++) = (xuint8) (_g_b64_pr2six[*bufin] << 2
				| _g_b64_pr2six[bufin[1]] >> 4);
	}
	if (nprbytes > 2) {
		*(bufout++) = (xuint8) (_g_b64_pr2six[bufin[1]] << 4
				| _g_b64_pr2six[bufin[2]] >> 2);
	}
	if (nprbytes > 3) {
		*(bufout++) = (xuint8) (_g_b64_pr2six[bufin[2]] << 6
				| _g_b64_pr2six[bufin[3]]);
	}

	nbytesdecoded -= (4 - (xint32) nprbytes) & 3;
	return nbytesdecoded;
}
