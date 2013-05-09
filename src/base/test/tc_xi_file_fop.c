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
 * File : tc_xi_file_fop.c
 */

#include "xi/xi_file.h"

#include "xi/xi_log.h"
#include "xi/xi_mem.h"
#include "xi/xi_string.h"

static void tc_info() {
	log_print(XDLOG, "====================================================\n");
	log_print(XDLOG, "               xi_file.h - file OP\n");
	log_print(XDLOG, "----------------------------------------------------\n");
	log_print(XDLOG, " * Functions)\n");
	log_print(XDLOG, "   - xi_file_get_stdin\n");
	log_print(XDLOG, "   - xi_file_get_stdout\n");
	log_print(XDLOG, "   - xi_file_get_stderr\n");
	log_print(XDLOG, "   - xi_file_open\n");
	log_print(XDLOG, "   - xi_file_read\n");
	log_print(XDLOG, "   - xi_file_write\n");
	log_print(XDLOG, "   - xi_file_sync\n");
	log_print(XDLOG, "   - xi_file_close\n");
	log_print(XDLOG, "   - xi_file_seek\n");
	log_print(XDLOG, "   - xi_file_stat\n");
	log_print(XDLOG, "   - xi_file_fstat\n");
	log_print(XDLOG, "   - xi_file_chmod\n");
	log_print(XDLOG, "   - xi_file_rename\n");
	log_print(XDLOG, "   - xi_file_remove\n");
	log_print(XDLOG, "====================================================\n\n");
}

int tc_xi_file_fop() {
	xint32 t = 1;
	xchar *tcname = "xi_file.h";

	xchar buf[64];
	xsize len;
	xssize ret;

	xint32 sin, sout, serr, tfd;

	tc_info();

	sin = xi_file_get_stdin();
	sout = xi_file_get_stdout();
	serr = xi_file_get_stdout();
	tfd = -1;

	log_print(XDLOG, "[%s:%02d] Read from STDIN (64bytes) ###\n", tcname, t++);
	xi_mem_set(buf, 0, sizeof(buf));
	ret = xi_file_read(sin, buf, sizeof(buf) - 1);
	if (ret < 0) {
		log_print(XDLOG, "    - result : failed!!! (ret=%d)\n\n", ret);
		return -1;
	}
	log_print(XDLOG, "    - result : pass. (input=%s)\n\n", buf);

	log_print(XDLOG, "[%s:%02d] Write to STDOUT #############\n", tcname, t++);
	xi_mem_set(buf, 0, sizeof(buf));
	xi_strcpy(buf, "Hello World to STDOUT!!\n");
	len = xi_strlen(buf);
	ret = xi_file_write(sout, buf, len);
	if (ret < 0) {
		log_print(XDLOG, "    - result : failed!!! (ret=%d)\n\n", ret);
		return -1;
	}
	log_print(XDLOG, "    - result : pass.\n\n");

	log_print(XDLOG, "[%s:%02d] Write to STDERR #############\n", tcname, t++);
	xi_mem_set(buf, 0, sizeof(buf));
	xi_strcpy(buf, "Hello World to STDERR!!\n");
	len = xi_strlen(buf);
	ret = xi_file_write(serr, buf, len);
	if (ret < 0) {
		log_print(XDLOG, "    - result : failed!!! (ret=%d)\n\n", ret);
		return -1;
	}
	log_print(XDLOG, "    - result : pass.\n\n");

	log_print(XDLOG, "[%s:%02d] Open new File (test.dat) ####\n", tcname, t++);
	tfd = xi_file_open("test.dat",
			XI_FILE_MODE_CREATE | XI_FILE_MODE_WRITE | XI_FILE_MODE_READ, 0666);
	if (tfd < 0) {
		log_print(XDLOG, "    - result : failed!!!\n\n");
		return -1;
	}
	log_print(XDLOG, "    - result : pass. (fd=%d)\n\n", tfd);

	log_print(XDLOG, "[%s:%02d] Write dummy data (twice) ####\n", tcname, t++);
	xi_mem_set(buf, 'X', sizeof(buf));
	buf[(sizeof(buf) - 1)] = '\n';
	ret = xi_file_write(tfd, buf, sizeof(buf));
	if (ret < 0) {
		log_print(XDLOG, "    - result : failed!!! (ret=%d)\n\n", ret);
		return -1;
	}
	ret = xi_file_write(tfd, buf, sizeof(buf));
	if (ret < 0) {
		log_print(XDLOG, "    - result : failed!!! (ret=%d)\n\n", ret);
		return -1;
	}
	log_print(XDLOG, "    - result : pass. (wbytes=%d)\n\n", ret);

	log_print(XDLOG, "[%s:%02d] Sync File (Flush) ###########\n", tcname, t++);
	ret = xi_file_sync(tfd);
	if (ret < 0) {
		log_print(XDLOG, "    - result : failed!!! (ret=%d)\n\n", ret);
		return -1;
	}
	log_print(XDLOG, "    - result : pass.\n\n");

	log_print(XDLOG, "[%s:%02d] Seek to First ###############\n", tcname, t++);
	ret = (xint32) xi_file_seek(tfd, 0, XI_FILE_SEEK_SET);
	if (tfd < 0) {
		log_print(XDLOG, "    - result : failed!!! (ret=%d)\n\n", ret);
		return -1;
	}
	log_print(XDLOG, "    - result : pass. (pos=%d)\n\n", ret);

	log_print(XDLOG, "[%s:%02d] Dump data until EOF #########\n", tcname, t++);
	do {
		ret = xi_file_read(tfd, buf, sizeof(buf));
		if (ret < 0) {
			log_print(XDLOG, "    - result : failed!!! (ret=%d)\n\n", ret);
			return -1;
		}
		xi_file_write(sout, buf, (xsize)ret);
	} while (ret > 0);
	log_print(XDLOG, "    - result : pass.\n\n");

	log_print(XDLOG, "[%s:%02d] Close File (test.dat) #######\n", tcname, t++);
	ret = xi_file_close(tfd);
	if (ret < 0) {
		log_print(XDLOG, "    - result : failed!!! (ret=%d)\n\n", ret);
		return -1;
	}
	log_print(XDLOG, "    - result : pass.\n\n");

	log_print(XDLOG, "[%s:%02d] Rename test.dat => t.dat ####\n", tcname, t++);
	ret = xi_file_rename("test.dat", "t.dat");
	if (ret < 0) {
		log_print(XDLOG, "    - result : failed!!! (ret=%d)\n\n", ret);
		return -1;
	}
	log_print(XDLOG, "    - result : pass.\n\n");

	log_print(XDLOG, "[%s:%02d] Change Permission (t.dat) ###\n", tcname, t++);
	ret = xi_file_chmod("t.dat", 0644);
	if (ret < 0) {
		log_print(XDLOG, "    - result : failed!!! (ret=%d)\n\n", ret);
		return -1;
	}
	log_print(XDLOG, "    - result : pass.\n\n");

	log_print(XDLOG, "[%s:%02d] Remove (t.dat) ##############\n", tcname, t++);
	ret = xi_file_remove("t.dat");
	if (ret < 0) {
		log_print(XDLOG, "    - result : failed!!! (ret=%d)\n\n", ret);
		return -1;
	}
	log_print(XDLOG, "    - result : pass.\n\n");

	log_print(XDLOG, "============= DONE [xi_file.h - file OP] ===========\n\n");

	return 0;
}
