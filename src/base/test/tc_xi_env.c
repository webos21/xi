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
 * File   : tc_xi_env.c
 */

#include "xi/xi_env.h"

#include "xi/xi_log.h"
#include "xi/xi_mem.h"
#include "xi/xi_thread.h"
#include "xi/xi_string.h"

static void tc_info() {
	log_print(XDLOG, "====================================================\n");
	log_print(XDLOG, "                     xi_env.h\n");
	log_print(XDLOG, "----------------------------------------------------\n");
	log_print(XDLOG, " * Functions)\n");
	log_print(XDLOG, "   - xi_env_all\n");
	log_print(XDLOG, "   - xi_env_get\n");
	log_print(XDLOG, "   - xi_env_set\n");
	log_print(XDLOG, "   - xi_env_del\n");
	log_print(XDLOG, "====================================================\n\n");
}

int tc_xi_env() {
	xint32 t = 1;
	xchar *tcname = "xi_env.h";

	xssize ret;
	xchar * envarr[128];
	xchar buf[256];

	tc_info();

	log_print(XDLOG, "[%s:%02d] xi_env_all ##################\n", tcname, t++);
	ret = xi_env_all(envarr, 128);
	if (ret < 0) {
		log_print(XDLOG, "    - result : failed!!!\n\n");
		return -1;
	}
	log_print(XDLOG, "    - result : pass. (envarr[3] = %s)\n\n", envarr[3]);

	log_print(XDLOG, "[%s:%02d] xi_env_get ##################\n", tcname, t++);
	ret = xi_env_get("PATH", buf, sizeof(buf));
	if (ret < 0) {
		log_print(XDLOG, "    - result : failed!!!\n\n");
		return -1;
	}
	log_print(XDLOG, "    - result : pass. (PATH = %s)\n\n", buf);

	log_print(XDLOG, "[%s:%02d] xi_env_set ##################\n", tcname, t++);
	ret = xi_env_set("MY_DUMMY_KEY", "MY_DUMMY_VAL");
	if (ret < 0) {
		log_print(XDLOG, "    - result : failed!!!\n\n");
		return -1;
	}
	xi_env_get("MY_DUMMY_KEY", buf, sizeof(buf));
	log_print(XDLOG, "    - result : pass. (MY_DUMMY_KEY = %s)\n\n", buf);

	log_print(XDLOG, "[%s:%02d] xi_env_del ##################\n", tcname, t++);
	ret = xi_env_del("MY_DUMMY_KEY");
	if (ret < 0) {
		log_print(XDLOG, "    - result : failed!!!\n\n");
		return -1;
	}
	log_print(XDLOG, "    - result : pass. (MY_DUMMY_KEY = %d)\n\n", xi_env_get("MY_DUMMY_KEY", buf, sizeof(buf)));

	log_print(XDLOG, "================== DONE [xi_env.h] =================\n\n");

	return 0;
}
