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
 * File   : tc_xi_base64.c
 */

#include "xi/xi_base64.h"

#include "xi/xi_log.h"
#include "xi/xi_mem.h"
#include "xi/xi_thread.h"
#include "xi/xi_string.h"

static void tc_info() {
	log_print(XDLOG, "====================================================\n");
	log_print(XDLOG, "                    xi_base64.h\n");
	log_print(XDLOG, "----------------------------------------------------\n");
	log_print(XDLOG, " * Functions)\n");
	log_print(XDLOG, "   - xi_base64_encode_len\n");
	log_print(XDLOG, "   - xi_base64_encode\n");
	log_print(XDLOG, "   - xi_base64_decode_len\n");
	log_print(XDLOG, "   - xi_base64_decode\n");
	log_print(XDLOG, "   - xi_base64_decode_binary\n");
	log_print(XDLOG, "====================================================\n\n");
}

int tc_xi_base64() {
	xint32 t = 1;
	xchar *tcname = "xi_base64.h";

	xssize ret;
	xchar *encdst = NULL;
	xssize enclen = 0;
	xchar *decdst = NULL;
	xssize declen = 0;
	xchar *tstring = "abcdefghijklmnopqrstuvwxyz";
	xsize  tstrlen = xi_strlen(tstring);

	tc_info();

	log_print(XDLOG, "[%s:%02d] xi_base64_encode_len ########\n", tcname, t++);
	enclen = xi_base64_encode_len(tstrlen);
	if (enclen != 37) {
		log_print(XDLOG, "    - result : failed!!!\n\n");
		return -1;
	}
	log_print(XDLOG, "    - result : pass. (encoded length=%d)\n\n", enclen);

	log_print(XDLOG, "[%s:%02d] xi_base64_encode ############\n", tcname, t++);
	encdst = (xchar *) xi_mem_alloc((xsize)enclen);
	ret = xi_base64_encode(encdst, tstring, tstrlen);
	if (ret != enclen) {
		log_print(XDLOG, "    - result : failed!!!\n\n");
		return -1;
	}
	log_print(XDLOG, "    - result : pass. (original = %s / encoded = %s)\n\n", tstring, encdst);

	log_print(XDLOG, "[%s:%02d] xi_base64_decode_len ########\n", tcname, t++);
	declen = xi_base64_decode_len(encdst);
	if (declen < 0) {
		log_print(XDLOG, "    - result : failed!!!\n\n");
		return -1;
	}
	log_print(XDLOG, "    - result : pass. (encoded string = %s / decoded length=%d)\n\n", encdst, declen);

	log_print(XDLOG, "[%s:%02d] xi_base64_decode ############\n", tcname, t++);
	decdst = (xchar *) xi_mem_alloc((xsize)declen);
	ret = xi_base64_decode(decdst, encdst);
	if (ret < 0) {
		log_print(XDLOG, "    - result : failed!!!\n\n");
		return -1;
	}
	log_print(XDLOG, "    - result : pass. (decoded string = %s / encoded string = %s)\n\n", decdst, encdst);

	log_print(XDLOG, "[%s:%02d] xi_base64_decode_binary #####\n", tcname, t++);
	ret = xi_base64_decode_binary((xuint8*) decdst, encdst);
	if (ret < 0) {
		log_print(XDLOG, "    - result : failed!!!\n\n");
		return -1;
	}
	log_print(XDLOG, "    - result : pass. (decoded string = %s / encoded string = %s)\n\n", decdst, encdst);

	log_print(XDLOG, "================ DONE [xi_base64.h] ================\n\n");

	return 0;
}
