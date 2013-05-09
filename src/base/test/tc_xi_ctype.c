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
 * File   : tc_xi_ctype.c
 */

#include "xi/xi_ctype.h"

#include "xi/xi_log.h"
#include "xi/xi_mem.h"
#include "xi/xi_thread.h"
#include "xi/xi_string.h"

static void tc_info() {
	log_print(XDLOG, "====================================================\n");
	log_print(XDLOG, "                    xi_ctype.h\n");
	log_print(XDLOG, "----------------------------------------------------\n");
	log_print(XDLOG, " * Functions)\n");
	log_print(XDLOG, "   - xi_isalnum\n");
	log_print(XDLOG, "   - xi_isalpha\n");
	log_print(XDLOG, "   - xi_isascii\n");
	log_print(XDLOG, "   - xi_iscntrl\n");
	log_print(XDLOG, "   - xi_isdigit\n");
	log_print(XDLOG, "   - xi_isgraph\n");
	log_print(XDLOG, "   - xi_islower\n");
	log_print(XDLOG, "   - xi_isprint\n");
	log_print(XDLOG, "   - xi_ispunct\n");
	log_print(XDLOG, "   - xi_isspace\n");
	log_print(XDLOG, "   - xi_isupper\n");
	log_print(XDLOG, "   - xi_isxdigit\n");
	log_print(XDLOG, "   - xi_toascii\n");
	log_print(XDLOG, "   - xi_tolower\n");
	log_print(XDLOG, "   - xi_toupper\n");
	log_print(XDLOG, "====================================================\n\n");
}

int tc_xi_ctype() {
	xint32 t = 1;
	xchar *tcname = "xi_ctype.h";

	xint32 ret;
	xchar ch;

	tc_info();

	log_print(XDLOG, "[%s:%02d] xi_isalnum('1') #############\n", tcname, t++);
	ret = xi_isalnum('1');
	if (!ret) {
		log_print(XDLOG, "    - result : failed!!!\n\n");
		return -1;
	}
	log_print(XDLOG, "    - result : pass. (ret=%d)\n\n", ret);

	log_print(XDLOG, "[%s:%02d] xi_isalpha('1') #############\n", tcname, t++);
	ret = xi_isalpha('1');
	if (ret) {
		log_print(XDLOG, "    - result : failed!!!\n\n");
		return -1;
	}
	log_print(XDLOG, "    - result : pass. (ret=%d)\n\n", ret);

	log_print(XDLOG, "[%s:%02d] xi_isascii('1') #############\n", tcname, t++);
	ret = xi_isascii('1');
	if (!ret) {
		log_print(XDLOG, "    - result : failed!!!\n\n");
		return -1;
	}
	log_print(XDLOG, "    - result : pass. (ret=%d)\n\n", ret);

	log_print(XDLOG, "[%s:%02d] xi_iscntrl('1') #############\n", tcname, t++);
	ret = xi_iscntrl('1');
	if (ret) {
		log_print(XDLOG, "    - result : failed!!!\n\n");
		return -1;
	}
	log_print(XDLOG, "    - result : pass. (ret=%d)\n\n", ret);

	log_print(XDLOG, "[%s:%02d] xi_isdigit('1') #############\n", tcname, t++);
	ret = xi_isdigit('1');
	if (!ret) {
		log_print(XDLOG, "    - result : failed!!!\n\n");
		return -1;
	}
	log_print(XDLOG, "    - result : pass. (ret=%d)\n\n", ret);

	log_print(XDLOG, "[%s:%02d] xi_isgraph('1') #############\n", tcname, t++);
	ret = xi_isgraph('1');
	if (!ret) {
		log_print(XDLOG, "    - result : failed!!!\n\n");
		return -1;
	}
	log_print(XDLOG, "    - result : pass. (ret=%d)\n\n", ret);

	log_print(XDLOG, "[%s:%02d] xi_islower('a') #############\n", tcname, t++);
	ret = xi_islower('a');
	if (!ret) {
		log_print(XDLOG, "    - result : failed!!!\n\n");
		return -1;
	}
	log_print(XDLOG, "    - result : pass. (ret=%d)\n\n", ret);

	log_print(XDLOG, "[%s:%02d] xi_isprint('a') #############\n", tcname, t++);
	ret = xi_isalnum('a');
	if (!ret) {
		log_print(XDLOG, "    - result : failed!!!\n\n");
		return -1;
	}
	log_print(XDLOG, "    - result : pass. (ret=%d)\n\n", ret);

	log_print(XDLOG, "[%s:%02d] xi_ispunct('a') #############\n", tcname, t++);
	ret = xi_ispunct('a');
	if (ret) {
		log_print(XDLOG, "    - result : failed!!!\n\n");
		return -1;
	}
	log_print(XDLOG, "    - result : pass. (ret=%d)\n\n", ret);

	log_print(XDLOG, "[%s:%02d] xi_isspace(' ') #############\n", tcname, t++);
	ret = xi_isspace(' ');
	if (!ret) {
		log_print(XDLOG, "    - result : failed!!!\n\n");
		return -1;
	}
	log_print(XDLOG, "    - result : pass. (ret=%d)\n\n", ret);

	log_print(XDLOG, "[%s:%02d] xi_isupper('a') #############\n", tcname, t++);
	ret = xi_isupper('a');
	if (ret) {
		log_print(XDLOG, "    - result : failed!!!\n\n");
		return -1;
	}
	log_print(XDLOG, "    - result : pass. (ret=%d)\n\n", ret);

	log_print(XDLOG, "[%s:%02d] xi_isxdigit('a') #############\n", tcname, t++);
	ret = xi_isxdigit('a');
	if (!ret) {
		log_print(XDLOG, "    - result : failed!!!\n\n");
		return -1;
	}
	log_print(XDLOG, "    - result : pass. (ret=%d)\n\n", ret);

	log_print(XDLOG, "[%s:%02d] xi_toascii('1') #############\n", tcname, t++);
	ret = xi_toascii('1');
	if (ret != '1') {
		log_print(XDLOG, "    - result : failed!!!\n\n");
		return -1;
	}
	log_print(XDLOG, "    - result : pass. (ret=%d)\n\n", ret);

	log_print(XDLOG, "[%s:%02d] xi_tolower('A') #############\n", tcname, t++);
	ch = (xchar)xi_tolower('A');
	if (ch != 'a') {
		log_print(XDLOG, "    - result : failed!!!\n\n");
		return -1;
	}
	log_print(XDLOG, "    - result : pass. (char=%c)\n\n", ch);

	log_print(XDLOG, "[%s:%02d] xi_toupper('a') #############\n", tcname, t++);
	ch = (xchar)xi_toupper('a');
	if (ch != 'A') {
		log_print(XDLOG, "    - result : failed!!!\n\n");
		return -1;
	}
	log_print(XDLOG, "    - result : pass. (char=%c)\n\n", ch);

	log_print(XDLOG, "================= DONE [xi_ctype.h] ================\n\n");

	return 0;
}
