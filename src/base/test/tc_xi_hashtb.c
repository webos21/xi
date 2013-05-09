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
 * File : tc_xi_log.c
 */

#include "xi/xi_hashtb.h"

#include "xi/xi_log.h"
#include "xi/xi_mem.h"
#include "xi/xi_string.h"

#define MAX_KEYGEN 102

static void tc_info() {
	log_print(XDLOG, "\n\n");
	log_print(XDLOG, "====================================================\n");
	log_print(XDLOG, "                     xi_hashtb.h\n");
	log_print(XDLOG, "----------------------------------------------------\n");
	log_print(XDLOG, " * Functions)\n");
	log_print(XDLOG, "   - prepare (generate the test data)\n");
	log_print(XDLOG, "   - xi_hashtb_create\n");
	log_print(XDLOG, "   - xi_hashtb_set\n");
	log_print(XDLOG, "   - xi_hashtb_get\n");
	log_print(XDLOG, "   - xi_hashtb_first / xi_hashtb_next #1\n");
	log_print(XDLOG, "   - xi_hashtb_clear\n");
	log_print(XDLOG, "   - xi_hashtb_set (again)\n");
	log_print(XDLOG, "   - xi_hashtb_first / xi_hashtb_next #2\n");
	log_print(XDLOG, "   - xi_hashtb_destroy\n");
	log_print(XDLOG, "====================================================\n\n");
}

int tc_xi_hashtb() {
	xint32 t = 1;
	xchar *tcname = "xi_hashtb.h";

	xi_hashtb_t *htb = NULL;
	xi_hashtb_idx_t *hidx = NULL;

	xchar *rval = NULL;
	xint32 cnt;

	xint32 td_intkey[MAX_KEYGEN];
	xchar td_strkey[MAX_KEYGEN][32];
	xchar td_strval[MAX_KEYGEN][32];

	tc_info();

	log_print(XDLOG, "[%s:%02d] prepare (generate the test data)\n", tcname, t++);
	for (cnt = 0; cnt < MAX_KEYGEN; cnt++) {
		td_intkey[cnt] = cnt;
		xi_mem_set(td_strkey[cnt], 0, 32);
		xi_mem_set(td_strval[cnt], 0, 32);
		xi_sprintf(td_strkey[cnt], "KEY-%03d", cnt);
		xi_sprintf(td_strval[cnt], "VALUE-%03d", cnt);
	}
	log_print(XDLOG, "    - result : pass. (htb=%p)\n\n", htb);

	log_print(XDLOG, "[%s:%02d] xi_hashtb_create ############\n", tcname, t++);
	htb = xi_hashtb_create();
	if (htb == NULL) {
		log_print(XDLOG, "    - result : failed!!!\n\n");
		return -1;
	}
	log_print(XDLOG, "    - result : pass. (htb=%p)\n\n", htb);

	log_print(XDLOG, "[%s:%02d] xi_hashtb_set ###############\n", tcname, t++);
	for (cnt = 0; cnt < MAX_KEYGEN; cnt++) {
		xi_hashtb_set(htb, &td_intkey[cnt], sizeof(xint32), td_strval[cnt]);
	}
	if (xi_hashtb_count(htb) != MAX_KEYGEN) {
		log_print(XDLOG, "    - result : failed!!!\n\n");
		return -1;
	}
	log_print(XDLOG, "    - result : pass. (count = %d / max = %d)\n\n", xi_hashtb_count(htb), xi_hashtb_max(htb));

	log_print(XDLOG, "[%s:%02d] xi_hashtb_get ###############\n", tcname, t++);
	cnt = 10;
	rval = xi_hashtb_get(htb, &cnt, sizeof(cnt));
	if (!rval) {
		log_print(XDLOG, "    - result : failed!!!\n\n");
		return -1;
	}
	log_print(XDLOG, "    - result : pass. (rval=%s)\n\n", rval);

	log_print(XDLOG, "[%s:%02d] xi_hashtb_first / next #1 ###\n", tcname, t++);
	log_print(XDLOG, "    - result : check below!!\n");
	for (cnt = 0, hidx = xi_hashtb_first(htb); hidx; hidx
			= xi_hashtb_next(hidx), cnt++) {
		xint32 klen;
		xint32 *key = NULL;
		xchar *val = NULL;
		xvoid *vkey = NULL, *vval = NULL;
		xi_hashtb_this(hidx, (const xvoid **)&vkey, &klen, &vval);
		key = vkey;
		val = vval;
		log_print(XDLOG, "[idx:%03d] key = %03d / klen = %d / val = %s\n", cnt, (*key), klen, val);
	}
	log_print(XDLOG, "\n");

	log_print(XDLOG, "[%s:%02d] xi_hashtb_clear #############\n", tcname, t++);
	xi_hashtb_clear(htb);
	if (xi_hashtb_count(htb) != 0) {
		log_print(XDLOG, "    - result : failed (count = %d)!!!\n\n", xi_hashtb_count(htb));
		return -1;
	}
	log_print(XDLOG, "    - result : pass. (count = %d / max = %d)\n\n", xi_hashtb_count(htb), xi_hashtb_max(htb));

	log_print(XDLOG, "[%s:%02d] xi_hashtb_set (again) #######\n", tcname, t++);
	for (cnt = 0; cnt < MAX_KEYGEN; cnt++) {
		xi_hashtb_set(htb, td_strkey[cnt], XI_HASHTB_KEY_STRING, td_strval[cnt]);
	}
	if (xi_hashtb_count(htb) != MAX_KEYGEN) {
		log_print(XDLOG, "    - result : failed!!!\n\n");
		return -1;
	}
	log_print(XDLOG, "    - result : pass. (count = %d / max = %d)\n\n", xi_hashtb_count(htb), xi_hashtb_max(htb));

	log_print(XDLOG, "[%s:%02d] xi_hashtb_first / next #2 ###\n", tcname, t++);
	log_print(XDLOG, "    - result : check below!!\n");
	for (cnt = 0, hidx = xi_hashtb_first(htb); hidx; hidx
			= xi_hashtb_next(hidx), cnt++) {
		xint32 klen = 0;
		xchar *key = NULL, *val = NULL;
		xvoid *vkey = NULL, *vval = NULL;
		xi_hashtb_this(hidx, (const xvoid **) &vkey, &klen, &vval);
		key = vkey;
		val = vval;
		log_print(XDLOG, "[idx:%03d] key = %s / klen = %d / val = %s\n", cnt, key, klen, val);
	}
	log_print(XDLOG, "\n");

	log_print(XDLOG, "[%s:%02d] xi_hashtb_destroy ###########\n", tcname, t++);
	xi_hashtb_destroy(htb);
	log_print(XDLOG, "    - result : pass.\n\n");

	log_print(XDLOG, "================== DONE [xi_hashtb.h] ==============\n\n");

	return 0;
}
