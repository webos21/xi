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
 * File   : tc_xi_proc.c
 */

#include "xi/xi_arrays.h"

#include "xi/xi_log.h"
#include "xi/xi_mem.h"
#include "xi/xi_thread.h"
#include "xi/xi_string.h"

static struct mi {
	xint32 nr;
	xchar *name;
} _g_months[] = { { 1, "jan" }, { 2, "feb" }, { 3, "mar" }, { 4, "apr" }, { 5,
		"may" }, { 6, "jun" }, { 7, "jul" }, { 8, "aug" }, { 9, "sep" }, { 10,
		"oct" }, { 11, "nov" }, { 12, "dec" } };

static xint32 tc_compare_fn(const xvoid *m1, const xvoid *m2) {
	struct mi *mi1 = (struct mi *) m1;
	struct mi *mi2 = (struct mi *) m2;
	return xi_strcmp(mi1->name, mi2->name);
}

static void tc_info() {
	log_print(XDLOG, "====================================================\n");
	log_print(XDLOG, "                     xi_arrays.h\n");
	log_print(XDLOG, "----------------------------------------------------\n");
	log_print(XDLOG, " * Functions)\n");
	log_print(XDLOG, "   - xi_arrays_bscan32\n");
	log_print(XDLOG, "   - xi_arrays_qsort\n");
	log_print(XDLOG, "   - xi_arrays_bsearch\n");
	log_print(XDLOG, "====================================================\n\n");
}

int tc_xi_arrays() {
	xint32 t = 1;
	xchar *tcname = "xi_arrays.h";

	xint32 ret;
	xint32 bscanv = 0x00000100;
	xsize i;

	struct mi key, *res;

	tc_info();

	log_print(XDLOG, "[%s:%02d] xi_arrays_bscan32 ###########\n", tcname, t++);
	ret = xi_arrays_bscan32(bscanv);
	if (ret != 9) {
		log_print(XDLOG, "    - result : failed!!! (val=0x%x / expected=9 / ret=%d)\n\n", bscanv, ret);
		return -1;
	}
	log_print(XDLOG, "    - result : pass. (val=0x%x / pos=%d)\n\n", bscanv, ret);

	log_print(XDLOG, "[%s:%02d] xi_arrays_qsort ###########\n", tcname, t++);
	log_print(XDLOG, "  - original array :\n");
	for (i = 0; i < (sizeof(_g_months) / sizeof(struct mi)); i++) {
		log_print(XDLOG, "    month = %d / month_name = %s\n", _g_months[i].nr, _g_months[i].name);
	}
	xi_arrays_qsort(_g_months, sizeof(_g_months) / sizeof(struct mi),
			sizeof(struct mi), tc_compare_fn);
	log_print(XDLOG, "  - after sort by month_name :\n");
	for (i = 0; i < (sizeof(_g_months) / sizeof(struct mi)); i++) {
		log_print(XDLOG, "    month = %d / month_name = %s\n", _g_months[i].nr, _g_months[i].name);
	}
	if (_g_months[0].nr != 4) {
		log_print(XDLOG, "    - result : failed!!! (expected month[0).nr=4, but=%d)\n\n", _g_months[0].nr);
		return -1;
	}
	log_print(XDLOG, "    - result : pass.\n\n");

	log_print(XDLOG, "[%s:%02d] xi_arrays_bsearch(oct) ######\n", tcname, t++);
	key.name = "oct";
	res = xi_arrays_bsearch(&key, _g_months,
			sizeof(_g_months) / sizeof(struct mi), sizeof(struct mi),
			tc_compare_fn);
	if (res == NULL) {
		log_print(XDLOG, "    - result : failed!!! (key_name=%s / result=(nil))\n\n", key.name);
		return -1;
	}
	log_print(XDLOG, "    - result : pass. (key_name=%s / result_month=%d)\n\n", key.name, res->nr);

	log_print(XDLOG, "================ DONE [xi_arrays.h] ==============\n\n");

	return 0;
}
