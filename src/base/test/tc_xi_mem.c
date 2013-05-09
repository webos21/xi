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
 * File : tc_xi_mem.c
 */

#include "xi/xi_mem.h"

#include "xi/xi_log.h"

static void tc_info() {
	log_print(XDLOG, "\n\n");
	log_print(XDLOG, "====================================================\n");
	log_print(XDLOG, "                      xi_mem.h\n");
	log_print(XDLOG, "----------------------------------------------------\n");
	log_print(XDLOG, " * Functions)\n");
	log_print(XDLOG, "   - xi_mem_alloc\n");
	log_print(XDLOG, "   - xi_mem_calloc\n");
	log_print(XDLOG, "   - xi_mem_realloc\n");
	log_print(XDLOG, "   - xi_mem_set\n");
	log_print(XDLOG, "   - xi_mem_chr\n");
	log_print(XDLOG, "   - xi_mem_cmp\n");
	log_print(XDLOG, "   - xi_mem_copy\n");
	log_print(XDLOG, "   - xi_mem_move\n");
	log_print(XDLOG, "   - xi_mem_free\n");
	log_print(XDLOG, "   - xi_mem_read\n");
	log_print(XDLOG, "   - xi_mem_write\n");
	log_print(XDLOG, "====================================================\n\n");
}

int tc_xi_mem() {
	xint32 t = 1;
	xchar *tcname = "xi_mem.h";

	xchar *tc = NULL;
	xint32 *ti = NULL;
	xchar *chk = NULL;
	xssize ret = 0;
	xchar buf[64];

	tc_info();

	log_print(XDLOG, "[%s:%02d] alloc[char]/calloc[int] #####\n", tcname, t++);
	tc = xi_mem_alloc(sizeof(xchar) * 50);
	if (tc == NULL) {
		log_print(XDLOG, "    - result : failed!!!\n\n");
		return -1;
	}
	log_print(XDLOG, "    - result : pass. (tc=%p)\n", tc);
	ti = xi_mem_calloc(50, sizeof(xint32));
	if (ti == NULL) {
		log_print(XDLOG, "    - result : failed!!!\n\n");
		return -1;
	}
	log_print(XDLOG, "    - result : pass. (ti=%p)\n\n", ti);

	log_print(XDLOG, "[%s:%02d] mem_set [char] with 'A' #####\n", tcname, t++);
	xi_mem_set(tc, 'A', sizeof(xchar) * 50);
	if (tc[49] != 'A') {
		log_print(XDLOG, "    - result : failed!!!\n\n");
		return -1;
	}
	log_print(XDLOG, "    - result : pass. char[49] = '%c'\n\n", tc[49]);

	log_print(XDLOG, "[%s:%02d] realloc [char] 50=>100 ######\n", tcname, t++);
	tc = xi_mem_realloc(tc, sizeof(xchar) * 100);
	tc[99] = '\0';
	if (tc[49] != 'A' || tc[99] != '\0') {
		log_print(XDLOG, "    - result : failed!!!\n\n");
		return -1;
	}
	log_print(XDLOG, "    - result : pass. tc=%p, char[49] = '%c', char[99] = '%d'\n\n", tc, tc[49], tc[99]);

	log_print(XDLOG, "[%s:%02d] mem_set [char]>50 with 'C' ##\n", tcname, t++);
	xi_mem_set(&tc[50], 'C', sizeof(xchar) * 49);
	if (tc[49] != 'A' || tc[50] != 'C') {
		log_print(XDLOG, "    - result : failed!!!\n\n");
		return -1;
	}
	log_print(XDLOG, "    - result : pass. tc=%p, char[49] = '%c', char[50] = '%c'\n\n", tc, tc[49], tc[50]);

	log_print(XDLOG, "[%s:%02d] mem_copy [char] to [int] ####\n", tcname, t++);
	xi_mem_copy(ti, tc, 4);
	if (ti[0] != 0x41414141) {
		log_print(XDLOG, "    - result : failed!!!\n\n");
		return -1;
	}
	log_print(XDLOG, "    - result : pass. ti[0]=0x%x\n\n", ti[0]);

	log_print(XDLOG, "[%s:%02d] mem_move [char] and [int] ###\n", tcname, t++);
	xi_mem_move(&ti[1], &tc[5], 4);
	if (ti[1] != 0x41414141) {
		log_print(XDLOG, "    - result : failed!!!\n\n");
		return -1;
	}
	log_print(XDLOG, "    - result : pass. ti[1]=0x%x\n\n", ti[1]);

	log_print(XDLOG, "[%s:%02d] mem_chr [char] find 'C' #####\n", tcname, t++);
	chk = xi_mem_chr(tc, 'C', 90);
	if (*chk != 'C') {
		log_print(XDLOG, "    - result : failed!!!\n\n");
		return -1;
	}
	log_print(XDLOG, "    - result : pass. chk='%c'\n\n", (*chk));

	log_print(XDLOG, "[%s:%02d] mem_cmp [char] and [int] ####\n", tcname, t++);
	ret = xi_mem_cmp(ti, tc, 4);
	if (ret != 0) {
		log_print(XDLOG, "    - result : failed!!!\n\n");
		return -1;
	}
	log_print(XDLOG, "    - result : pass. ret=%d\n\n", ret);

	log_print(XDLOG, "[%s:%02d] mem_read [char] len-10 ######\n", tcname, t++);
	xi_mem_set(buf, 0, sizeof(buf));
	ret = xi_mem_read(tc, 10, buf);
	if (ret != 10) {
		log_print(XDLOG, "    - result : failed!!!\n\n");
		return -1;
	}
	log_print(XDLOG, "    - result : pass. buf=%s/ret=%d\n\n", buf, ret);

	log_print(XDLOG, "[%s:%02d] mem_write [char]-pos:60 #####\n", tcname, t++);
	ret = xi_mem_write(&tc[60], 10, buf);
	if (ret != 10) {
		log_print(XDLOG, "    - result : failed!!!\n\n");
		return -1;
	}
	log_print(XDLOG, "    - result : pass. tc=%s/ret=%d\n\n", tc, ret);

	log_print(XDLOG, "[%s:%02d] free [char] #################\n", tcname, t++);
	xi_mem_free(tc);
	log_print(XDLOG, "    - result : pass.\n\n");

	log_print(XDLOG, "[%s:%02d] free [int] ##################\n", tcname, t++);
	xi_mem_free(ti);
	log_print(XDLOG, "    - result : pass.\n\n");

	log_print(XDLOG, "================== DONE [xi_mem.h] =================\n\n");

	return 0;
}
