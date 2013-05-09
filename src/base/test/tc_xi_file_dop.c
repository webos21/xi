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
 * File : tc_xi_file_dop.c
 */

#include "xi/xi_file.h"

#include "xi/xi_log.h"

static void tc_info() {
	log_print(XDLOG, "====================================================\n");
	log_print(XDLOG, "              xi_file.h - dir/path OP\n");
	log_print(XDLOG, "----------------------------------------------------\n");
	log_print(XDLOG, " * Functions)\n");
	log_print(XDLOG, "   - xi_dir_open\n");
	log_print(XDLOG, "   - xi_dir_read\n");
	log_print(XDLOG, "   - xi_dir_rewind\n");
	log_print(XDLOG, "   - xi_dir_close\n");
	log_print(XDLOG, "   - xi_dir_make\n");
	log_print(XDLOG, "   - xi_dir_make_force\n");
	log_print(XDLOG, "   - xi_dir_remove\n");
	log_print(XDLOG, "   - xi_pathname_absolute\n");
	log_print(XDLOG, "   - xi_pathname_basename\n");
	log_print(XDLOG, "   - xi_pathname_get\n");
	log_print(XDLOG, "   - xi_pathname_set\n");
	log_print(XDLOG, "   - xi_pathname_split\n");
	log_print(XDLOG, "   - xi_pathname_merge\n");
	log_print(XDLOG, "====================================================\n\n");
}

int tc_xi_file_dop() {
	xint32 t = 1;
	xchar *tcname = "xi_file.h";

	xint32 ret;

	xi_dir_t *tdir;
	xi_file_stat_t fs;

	xchar pathbuf[1024];
	xchar pathtmp[1024];
	xchar *pathptr = NULL;

	tc_info();

	log_print(XDLOG, "[%s:%02d] Create a directory ##########\n", tcname, t++);
	ret = xi_dir_make("xx_test", 0755);
	if (ret < 0) {
		log_print(XDLOG, "    - result : failed!!! (ret=%d)\n\n", ret);
		return -1;
	}
	log_print(XDLOG, "    - result : pass.\n\n");

	log_print(XDLOG, "[%s:%02d] Create same directory #######\n", tcname, t++);
	ret = xi_dir_make("xx_test", 0x755);
	if (ret != XI_FILE_RV_ERR_AE) {
		log_print(XDLOG, "    - result : failed!! (must return -4 / ret=%d)\n\n", ret);
		return -1;
	}
	log_print(XDLOG, "    - result : pass. (must return -4 / ret=%d)\n\n", ret);

	log_print(XDLOG, "[%s:%02d] Remove the directory ########\n", tcname, t++);
	ret = xi_dir_remove("xx_test");
	if (ret < 0) {
		log_print(XDLOG, "    - result : failed!!! (ret=%d)\n\n", ret);
		return -1;
	}
	log_print(XDLOG, "    - result : pass.\n\n");

	log_print(XDLOG, "[%s:%02d] Create a nested directory ###\n", tcname, t++);
	ret = xi_dir_make_force("xx_test/depth", 0755);
	if (ret < 0) {
		log_print(XDLOG, "    - result : failed!!! (ret=%d)\n\n", ret);
		return -1;
	}
	log_print(XDLOG, "    - result : pass.\n\n");

	log_print(XDLOG, "[%s:%02d] Remove a top directory ######\n", tcname, t++);
	ret = xi_dir_remove("xx_test");
	if (ret != XI_FILE_RV_ERR_NOTEMPT) {
		log_print(XDLOG, "    - result : failed!! (must return -14 / ret=%d)\n\n", ret);
		return -1;
	}
	log_print(XDLOG, "    - result : pass. (must return -14 / ret=%d)\n\n", ret);

	log_print(XDLOG, "[%s:%02d] Open the directory ##########\n", tcname, t++);
	tdir = xi_dir_open("xx_test");
	if (tdir == NULL) {
		log_print(XDLOG, "    - result : failed!! (dir=%p)\n\n", tdir);
		return -1;
	}
	log_print(XDLOG, "    - result : pass. (dir=%p)\n\n", tdir);

	log_print(XDLOG, "[%s:%02d] Read the directory ##########\n", tcname, t++);
	log_print(XDLOG, "xx_test >\n");
	while ((ret = xi_dir_read(tdir, &fs)) > 0) {
		log_print(XDLOG, "\t%s\n", fs.filename);
	}
	if (ret < 0) {
		log_print(XDLOG, "    - result : failed!! (ret=%d)\n\n", ret);
		return -1;
	}
	log_print(XDLOG, "    - result : pass.\n\n");

	log_print(XDLOG, "[%s:%02d] Close the directory #########\n", tcname, t++);
	ret = xi_dir_close(tdir);
	if (ret < 0) {
		log_print(XDLOG, "    - result : failed!! (ret=%d)\n\n", ret);
		return -1;
	}
	log_print(XDLOG, "    - result : pass.\n\n");

	log_print(XDLOG, "[%s:%02d] Delete test directories #####\n", tcname, t++);
	ret = xi_dir_remove("xx_test/depth");
	if (ret < 0) {
		log_print(XDLOG, "    - result : failed!! (ret=%d)\n\n", ret);
		return -1;
	}
	ret = xi_dir_remove("xx_test");
	if (ret < 0) {
		log_print(XDLOG, "    - result : failed!! (ret=%d)\n\n", ret);
		return -1;
	}
	log_print(XDLOG, "    - result : pass.\n\n");

	log_print(XDLOG, "[%s:%02d] xi_pathname_absolute(..) ####\n", tcname, t++);
	ret = xi_pathname_absolute(pathbuf, sizeof(pathbuf), "../");
	if (ret != XI_FILE_RV_OK) {
		log_print(XDLOG, "    - result : failed!! (ret=%d)\n\n", ret);
		return -1;
	}
	log_print(XDLOG, "    - result : pass. (../ = %s)\n\n", pathbuf);

	log_print(XDLOG, "[%s:%02d] xi_pathname_basename(..) ####\n", tcname, t++);
	pathptr = xi_pathname_basename(pathbuf);
	log_print(XDLOG, "    - result : pass. (basename ../ = %s)\n\n", pathptr);

	log_print(XDLOG, "[%s:%02d] xi_pathname_get #############\n", tcname, t++);
	ret = xi_pathname_get(pathbuf, sizeof(pathbuf));
	if (ret != XI_FILE_RV_OK) {
		log_print(XDLOG, "    - result : failed!! (ret=%d)\n\n", ret);
		return -1;
	}
	log_print(XDLOG, "    - result : pass.(current dir = %s)\n\n", pathbuf);

	log_print(XDLOG, "[%s:%02d] xi_pathname_set(..) #########\n", tcname, t++);
	ret = xi_pathname_set("..");
	if (ret != XI_FILE_RV_OK) {
		log_print(XDLOG, "    - result : failed!! (ret=%d)\n\n", ret);
		return -1;
	}
	xi_pathname_get(pathtmp, sizeof(pathtmp));
	log_print(XDLOG, "    - result : pass.(current dir = %s)\n\n", pathtmp);

	log_print(XDLOG, "[%s:%02d] xi_pathname_set (old) #######\n", tcname, t++);
	ret = xi_pathname_set(pathbuf);
	if (ret != XI_FILE_RV_OK) {
		log_print(XDLOG, "    - result : failed!! (ret=%d)\n\n", ret);
		return -1;
	}
	xi_pathname_get(pathtmp, sizeof(pathtmp));
	log_print(XDLOG, "    - result : pass.(current dir = %s)\n\n", pathtmp);

	log_print(XDLOG, "========= DONE [xi_file.h - dir/path OP] =========\n\n");

	return 0;
}
