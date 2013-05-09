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
 * File   : tc_xi_socket_mcast.c
 */

#include "xi/xi_socket.h"

#include "xi/xi_log.h"
#include "xi/xi_mem.h"
#include "xi/xi_thread.h"
#include "xi/xi_string.h"

static void tc_info() {
	log_print(XDLOG, "====================================================\n");
	log_print(XDLOG, "                 xi_socket.h - mcast\n");
	log_print(XDLOG, "----------------------------------------------------\n");
	log_print(XDLOG, " * Functions)\n");
	log_print(XDLOG, "   - xi_mcast_join\n");
	log_print(XDLOG, "   - xi_mcast_leave\n");
	log_print(XDLOG, "====================================================\n\n");
}

int tc_xi_socket_mcast() {
	xint32 t = 1;
	xchar *tcname = "xi_socket.h";

	xssize ret, i;
	xchar buf[2048];

	xint32 msfd;

	xi_sock_addr_t iface = { XI_SOCK_FAMILY_INET, XI_SOCK_TYPE_DATAGRAM,
			XI_SOCK_PROTO_IP, { '\0' }, 5000 };
	xi_sock_addr_t grp = { XI_SOCK_FAMILY_INET, XI_SOCK_TYPE_DATAGRAM,
			XI_SOCK_PROTO_IP, { '\0' }, 5000 };
	xi_sock_addr_t tmp;

	tc_info();

	log_print(XDLOG, "[%s:%02d] xi_socket_open ##############\n", tcname, t++);
	msfd = xi_socket_open(iface.family, iface.type, iface.proto);
	if (msfd < 0) {
		log_print(XDLOG, "    - result : failed!!!\n\n");
		return -1;
	}
	log_print(XDLOG, "    - result : pass. (socket=%d)\n\n", msfd);

	log_print(XDLOG, "  (%s:%02d) xi_socket_opt_set #########\n", tcname, t++);
	ret = xi_socket_opt_set(msfd, XI_SOCK_OPT_REUSEADDR, TRUE);
	if (ret < 0) {
		log_print(XDLOG, "    - result : failed!!! (ret=%d)\n\n", ret);
		return -1;
	}
	log_print(XDLOG, "    - result : pass. (opt:REUSEADDR)\n\n");

	log_print(XDLOG, "[%s:%02d] xi_socket_bind ##############\n", tcname, t++);
	xi_strcpy(iface.host, "0.0.0.0");
	ret = xi_socket_bind(msfd, iface);
	if (ret < 0) {
		log_print(XDLOG, "    - result : failed!!! (ret=%d)\n\n", ret);
		return -1;
	}
	log_print(XDLOG, "    - result : pass. (bind:addr=%s/port=%d)\n\n", iface.host, iface.port);

	log_print(XDLOG, "[%s:%02d] xi_mcast_join ###############\n", tcname, t++);
	xi_strcpy(grp.host, "233.1.1.10");
	ret = xi_mcast_join(msfd, iface, grp, NULL);
	if (ret < 0) {
		log_print(XDLOG, "    - result : failed!!! (ret=%d)\n\n", ret);
		return -1;
	}
	log_print(XDLOG, "    - result : pass. (join-addr = %s)\n\n", grp.host);

	log_print(XDLOG, "[%s:%02d] xi_socket_sendto (5sec) #####\n", tcname, t++);
	xi_strcpy(buf, "multicast streaming...............");
	ret = xi_socket_sendto(msfd, buf, xi_strlen(buf), grp);
	if (ret < 0) {
		log_print(XDLOG, "    - result : failed!!! (ret=%d)\n\n", ret);
		return -1;
	}
	for (i = 0; i < 10; i++) {
		xi_thread_sleep(500);
		ret = xi_socket_sendto(msfd, buf, xi_strlen(buf), grp);
	}
	log_print(XDLOG, "    - result : pass. (wbytes=%d/msg=%s)\n\n", ret, buf);

	log_print(XDLOG, "[%s:%02d] xi_socket_recvfrom ##########\n", tcname, t++);
	xi_mem_set(buf, 0, sizeof(buf));
	xi_mem_set(&tmp, 0, sizeof(tmp));
	ret = xi_socket_recvfrom(msfd, buf, sizeof(buf), &tmp);
	if (ret < 0) {
		log_print(XDLOG, "    - result : failed!!! (ret=%d)\n\n", ret);
		return -1;
	}
	log_print(XDLOG, "    - result : pass. (fromaddr=%s/rbytes=%d/msg=%s)\n\n", tmp.host, ret, buf);

	log_print(XDLOG, "[%s:%02d] xi_mcast_leave ##############\n", tcname, t++);
	ret = xi_mcast_leave(msfd, iface, grp, NULL);
	if (ret < 0) {
		log_print(XDLOG, "    - result : failed!!! (ret=%d)\n\n", ret);
		return -1;
	}
	log_print(XDLOG, "    - result : pass.\n\n");

	log_print(XDLOG, "[%s:%02d] xi_socket_close #############\n", tcname, t++);
	ret = xi_socket_close(msfd);
	if (ret < 0) {
		log_print(XDLOG, "    - result : failed!!! (ret=%d)\n\n", ret);
		return -1;
	}
	log_print(XDLOG, "    - result : pass.\n\n");

	log_print(XDLOG, "[%s:%02d] xi_socket_open ##############\n", tcname, t++);
	msfd = xi_socket_open(iface.family, iface.type, iface.proto);
	if (msfd < 0) {
		log_print(XDLOG, "    - result : failed!!!\n\n");
		return -1;
	}
	log_print(XDLOG, "    - result : pass. (socket=%d)\n\n", msfd);

	log_print(XDLOG, "  (%s:%02d) xi_socket_opt_set #########\n", tcname, t++);
	ret = xi_socket_opt_set(msfd, XI_SOCK_OPT_REUSEADDR, TRUE);
	if (ret < 0) {
		log_print(XDLOG, "    - result : failed!!! (ret=%d)\n\n", ret);
		return -1;
	}
	log_print(XDLOG, "    - result : pass. (opt:REUSEADDR)\n\n");

	log_print(XDLOG, "[%s:%02d] xi_socket_bind ##############\n", tcname, t++);
	xi_strcpy(iface.host, "0.0.0.0");
	iface.port = 9201;
	ret = xi_socket_bind(msfd, iface);
	if (ret < 0) {
		log_print(XDLOG, "    - result : failed!!! (ret=%d)\n\n", ret);
		return -1;
	}
	log_print(XDLOG, "    - result : pass. (bind:addr=%s/port=%d)\n\n", iface.host, iface.port);

	log_print(XDLOG, "[%s:%02d] xi_mcast_join ###############\n", tcname, t++);
	xi_strcpy(iface.host, "192.168.31.69");
	xi_strcpy(grp.host, "225.0.1.1");
	grp.port = 9201;
	ret = xi_mcast_join(msfd, iface, grp, NULL);
	if (ret < 0) {
		log_print(XDLOG, "    - result : failed!!! (ret=%d)\n\n", ret);
		return -1;
	}
	log_print(XDLOG, "    - result : pass. (join-addr = %s)\n\n", grp.host);

	log_print(XDLOG, "[%s:%02d] xi_socket_recvfrom ##########\n", tcname, t++);
	xi_mem_set(buf, 0, sizeof(buf));
	xi_mem_set(&tmp, 0, sizeof(tmp));
	ret = xi_socket_recvfrom(msfd, buf, sizeof(buf), &tmp);
	if (ret < 0) {
		log_print(XDLOG, "    - result : failed!!! (ret=%d)\n\n", ret);
		return -1;
	}
	log_print(XDLOG, "    - result : pass. (fromaddr=%s/rbytes=%d/msg=%s)\n\n", tmp.host, ret, buf);

	log_print(XDLOG, "[%s:%02d] xi_mcast_leave ##############\n", tcname, t++);
	ret = xi_mcast_leave(msfd, iface, grp, NULL);
	if (ret < 0) {
		log_print(XDLOG, "    - result : failed!!! (ret=%d)\n\n", ret);
		return -1;
	}
	log_print(XDLOG, "    - result : pass.\n\n");

	log_print(XDLOG, "[%s:%02d] xi_socket_close #############\n", tcname, t++);
	ret = xi_socket_close(msfd);
	if (ret < 0) {
		log_print(XDLOG, "    - result : failed!!! (ret=%d)\n\n", ret);
		return -1;
	}
	log_print(XDLOG, "    - result : pass.\n\n");

	log_print(XDLOG, "============ DONE [xi_socket.h - mcast] ============\n\n");

	return 0;
}
