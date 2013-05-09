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

#include "xi/xi_log.h"

#include <stdio.h>

static void tc_xi_log_handle(xchar *msg) {
	printf("[New Handle] %s", msg);
}

static void tc_info() {
	printf("\n\n");
	printf("====================================================\n");
	printf("                      xi_log.h\n");
	printf("----------------------------------------------------\n");
	printf(" * Functions)\n");
	printf("   - xi_logger_fetch\n");
	printf("   - xi_logger_get_conf\n");
	printf("   - xi_logger_get_ids\n");
	printf("   - xi_logger_set_conf\n");
	printf("   - xi_logger_set_handle\n");
	printf("   - xi_logger_write\n");
	printf("====================================================\n\n");
}

int tc_xi_log() {
	xint32 t = 1;
	xchar *tcname = "xi_log.h";

	xchar *tmsg = "XI-LOG TEST";
	xi_logger_t *tlog;
	xi_logopt_t topt;
	xuint32 cnt, j;
	xi_logger_id_t *ids;

	tc_info();

	printf("[%s:%02d] log_print with XDLOG ########\n", tcname, t++);
	printf("    %s\n\n", tmsg);

	printf("[%s:%02d] logger_get - tlog ###########\n", tcname, t++);
	tlog = logger_get("TC_LOG");
	printf("    result : tlog=%p\n\n", tlog);

	printf("[%s:%02d] check default options #######\n", tcname, t++);
	topt = xi_logger_get_conf(tlog);
	printf("    tlog.opt.level    = %d\n", topt.level);
	printf("    tlog.opt.showDate = %d\n", topt.showDate);
	printf("    tlog.opt.showFile = %d\n", topt.showFile);
	printf("    tlog.opt.showFunc = %d\n", topt.showFunc);
	printf("    tlog.opt.showLine = %d\n\n", topt.showLine);

	printf("[%s:%02d] call log_* with 'tlog' ######\n", tcname, t++);
	log_trace(tlog, "%s\n", tmsg);
	log_debug(tlog, "%s\n", tmsg);
	log_info(tlog, "%s\n", tmsg);
	log_warn(tlog, "%s\n", tmsg);
	log_error(tlog, "%s\n", tmsg);
	log_fatal(tlog, "%s\n\n", tmsg);

	printf("[%s:%02d] change options 'tlog' #######\n", tcname, t++);
	topt.level = XI_LOG_LEVEL_DEBUG;
	topt.showDate = TRUE;
	xi_logger_set_conf(tlog, topt);
	printf("    tlog.opt.level    = %d\n", topt.level);
	printf("    tlog.opt.showDate = %d\n", topt.showDate);
	printf("  * check the output below:\n");
	log_trace(tlog, "%s\n", tmsg);
	log_debug(tlog, "%s\n", tmsg);
	log_info(tlog, "%s\n", tmsg);
	log_warn(tlog, "%s\n", tmsg);
	log_error(tlog, "%s\n", tmsg);
	log_fatal(tlog, "%s\n\n", tmsg);

	printf("[%s:%02d] set log-writer func #########\n", tcname, t++);
	xi_logger_set_handle(tc_xi_log_handle);
	log_trace(tlog, "%s\n", tmsg);
	log_debug(tlog, "%s\n", tmsg);
	log_info(tlog, "%s\n", tmsg);
	log_warn(tlog, "%s\n", tmsg);
	log_error(tlog, "%s\n", tmsg);
	log_fatal(tlog, "%s\n\n", tmsg);

	printf("[%s:%02d] reset log-writer func #######\n", tcname, t++);
	xi_logger_set_handle(NULL);
	log_trace(tlog, "%s\n", tmsg);
	log_debug(tlog, "%s\n", tmsg);
	log_info(tlog, "%s\n", tmsg);
	log_warn(tlog, "%s\n", tmsg);
	log_error(tlog, "%s\n", tmsg);
	log_fatal(tlog, "%s\n\n", tmsg);

	printf("[%s:%02d] xi_log_get_ids ##############\n", tcname, t++);
	xi_logger_get_ids(&cnt, &ids);
	printf("  * logger count = %d\n", cnt);
	for (j = 0; j < cnt; j++) {
		printf("    - [logger:%02d] %s\n", j, ids[j].id);
	}
	printf("\n");

	printf("================== DONE [xi_log.h] =================\n\n");

	return 0;
}
