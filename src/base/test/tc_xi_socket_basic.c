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
 * File   : tc_xi_socket_basic.c
 */

#include "xi/xi_socket.h"

#include "xi/xi_log.h"
#include "xi/xi_mem.h"
#include "xi/xi_thread.h"
#include "xi/xi_string.h"
#include "xi/xi_file.h"

static xbool _g_trun = FALSE;
static xint32 _g_ecode = 0;

#define TC_EXIT_ERROR do{_g_ecode = -1; _g_trun = FALSE; return NULL;}while(0)

static xvoid * tc_client_thread(xvoid *args) {
	xint32 t = 1;
	xchar *tcname = "xi_socket.h-clt";

	xssize ret;
	xchar buf[1024];

	xint32 clt_sock = -1;

	xi_sock_addr_t toaddr = { XI_SOCK_FAMILY_INET, XI_SOCK_TYPE_STREAM,
			XI_SOCK_PROTO_IP, { '\0' }, 9876 };

	UNUSED(args);

	log_print(XDLOG, "Client Thread - started >>>>>>>>>>>>>>>\n\n");

	log_print(XDLOG, "  (%s:%02d) xi_socket_open ############\n", tcname, t++);
	clt_sock = xi_socket_open(toaddr.family, toaddr.type, toaddr.proto);
	if (clt_sock < 0) {
		log_print(XDLOG, "    - result : failed!!!\n\n");
		TC_EXIT_ERROR;
	}
	log_print(XDLOG, "    - result : pass. (socket=%d)\n\n", clt_sock);

	log_print(XDLOG, "  (%s:%02d) xi_socket_connect #########\n", tcname, t++);
	xi_strcpy(toaddr.host, "127.0.0.1");
	ret = xi_socket_connect(clt_sock, toaddr);
	if (ret < 0) {
		log_print(XDLOG, "    - result : failed!!! (ret=%d)\n\n", ret);
		TC_EXIT_ERROR;
	}
	log_print(XDLOG, "    - result : pass. (svr=%s)\n\n", toaddr.host);

	log_print(XDLOG, "  (%s:%02d) xi_socket_send #############\n", tcname, t++);
	xi_strcpy(buf, "Hello, SVR!!");
	ret = xi_socket_send(clt_sock, buf, xi_strlen(buf));
	if (ret < 0) {
		log_print(XDLOG, "    - result : failed!!! (ret=%d)\n\n", ret);
		TC_EXIT_ERROR;
	}
	log_print(XDLOG, "    - result : pass. (wbytes=%d/msg=%s)\n\n", ret, buf);

	log_print(XDLOG, "  (%s:%02d) xi_socket_recv #############\n", tcname, t++);
	xi_mem_set(buf, 0, sizeof(buf));
	ret = xi_socket_recv(clt_sock, buf, sizeof(buf));
	if (ret < 0) {
		log_print(XDLOG, "    - result : failed!!! (ret=%d)\n\n", ret);
		TC_EXIT_ERROR;
	}
	log_print(XDLOG, "    - result : pass. (rbytes=%d/msg=%s)\n\n", ret, buf);

	log_print(XDLOG, "  (%s:%02d) xi_file_write #############\n", tcname, t++);
	xi_strcpy(buf, "client file write!");
	ret = xi_file_write(clt_sock, buf, xi_strlen(buf));
	if (ret < 0) {
		log_print(XDLOG, "    - result : failed!!! (ret=%d)\n\n", ret);
		TC_EXIT_ERROR;
	}
	log_print(XDLOG, "    - result : pass. (wbytes=%d/msg=%s)\n\n", ret, buf);

	log_print(XDLOG, "  (%s:%02d) xi_file_read #############\n", tcname, t++);
	xi_mem_set(buf, 0, sizeof(buf));
	ret = xi_file_read(clt_sock, buf, sizeof(buf));
	if (ret < 0) {
		log_print(XDLOG, "    - result : failed!!! (ret=%d)\n\n", ret);
		TC_EXIT_ERROR;
	}
	log_print(XDLOG, "    - result : pass. (rbytes=%d/msg=%s)\n\n", ret, buf);

	log_print(XDLOG, "  (%s:%02d) xi_socket_shutdown ########\n", tcname, t++);
	ret = xi_socket_shutdown(clt_sock, XI_SOCK_SHUTDOWN_RDWR);
	if (ret < 0) {
		log_print(XDLOG, "    - result : failed!!! (ret=%d)\n\n", ret);
		TC_EXIT_ERROR;
	}
	log_print(XDLOG, "    - result : pass.\n\n");

	log_print(XDLOG, "  (%s:%02d) xi_socket_close ###########\n", tcname, t++);
	ret = xi_socket_close(clt_sock);
	if (ret < 0) {
		log_print(XDLOG, "    - result : failed!!! (ret=%d)\n\n", ret);
		TC_EXIT_ERROR;
	}
	log_print(XDLOG, "    - result : pass.\n\n");

	log_print(XDLOG, "Client Thread - ended <<<<<<<<<<<<<<<<<\n\n");

	return NULL;
}

static xvoid * tc_server_thread(xvoid *args) {
	xint32 t = 1;
	xchar *tcname = "xi_socket.h-svr";

	xssize ret;
	xchar buf[1024];

	xint32 svr_sock = -1;
	xint32 client = -1;

	xi_sock_addr_t lsnaddr = { XI_SOCK_FAMILY_INET, XI_SOCK_TYPE_STREAM,
			XI_SOCK_PROTO_IP, { '\0' }, 9876 };
	xi_sock_addr_t laddr;
	xi_sock_addr_t fromaddr;

	UNUSED(args);

	log_print(XDLOG, "Server Thread - started >>>>>>>>>>>>>>>\n\n");

	log_print(XDLOG, "  (%s:%02d) xi_socket_open ############\n", tcname, t++);
	svr_sock = xi_socket_open(lsnaddr.family, lsnaddr.type, lsnaddr.proto);
	if (svr_sock < 0) {
		log_print(XDLOG, "    - result : failed!!!\n\n");
		TC_EXIT_ERROR;
	}
	log_print(XDLOG, "    - result : pass. (socket=%d)\n\n", svr_sock);

	log_print(XDLOG, "  (%s:%02d) xi_socket_opt_set #########\n", tcname, t++);
	ret = xi_socket_opt_set(svr_sock, XI_SOCK_OPT_REUSEADDR, TRUE);
	if (ret < 0) {
		log_print(XDLOG, "    - result : failed!!! (ret=%d)\n\n", ret);
		TC_EXIT_ERROR;
	}
	log_print(XDLOG, "    - result : pass.\n\n");

	log_print(XDLOG, "  (%s:%02d) xi_socket_bind ############\n", tcname, t++);
	xi_strcpy(lsnaddr.host, "0.0.0.0");
	ret = xi_socket_bind(svr_sock, lsnaddr);
	if (ret < 0) {
		log_print(XDLOG, "    - result : failed!!! (ret=%d)\n\n", ret);
		TC_EXIT_ERROR;
	}
	log_print(XDLOG, "    - result : pass.\n\n");

	log_print(XDLOG, "  (%s:%02d) xi_socket_listen ##########\n", tcname, t++);
	ret = xi_socket_listen(svr_sock, 10);
	if (ret < 0) {
		log_print(XDLOG, "    - result : failed!!! (ret=%d)\n\n", ret);
		TC_EXIT_ERROR;
	}
	log_print(XDLOG, "    - result : pass.\n\n");

	log_print(XDLOG, "  (%s:%02d) xi_socket_get_local #######\n", tcname, t++);
	ret = xi_socket_get_local(svr_sock, &laddr);
	if (ret < 0) {
		log_print(XDLOG, "    - result : failed!!! (ret=%d)\n\n", ret);
		TC_EXIT_ERROR;
	}
	log_print(XDLOG, "    - result : pass. (addr=%s,port=%d)\n\n", laddr.host, laddr.port);

	log_print(XDLOG, "  (%s:%02d) xi_socket_accept ##########\n", tcname, t++);
	client = xi_socket_accept(svr_sock, &fromaddr);
	if (client < 0) {
		log_print(XDLOG, "    - result : accept failed!!!\n\n");
		TC_EXIT_ERROR;
	}
	log_print(XDLOG, "    - result : pass. (client=%d, from=%s)\n\n", client, fromaddr.host);

	log_print(XDLOG, "  (%s:%02d) xi_socket_recv ############\n", tcname, t++);
	xi_mem_set(buf, 0, sizeof(buf));
	ret = xi_socket_recv(client, buf, sizeof(buf));
	if (ret < 0) {
		log_print(XDLOG, "    - result : recv failed!!!\n\n");
		TC_EXIT_ERROR;
	}
	log_print(XDLOG, "    - result : pass. (rbytes=%d/msg=%s)\n\n", ret, buf);

	log_print(XDLOG, "  (%s:%02d) xi_socket_send ############\n", tcname, t++);
	xi_strcpy(buf, "connected!");
	ret = xi_socket_send(client, buf, xi_strlen(buf));
	if (ret < 0) {
		log_print(XDLOG, "    - result : send failed!!!\n\n");
		TC_EXIT_ERROR;
	}
	log_print(XDLOG, "    - result : pass. (wbytes=%d/msg=%s)\n\n", ret, buf);

	log_print(XDLOG, "  (%s:%02d) xi_file_read #############\n", tcname, t++);
	xi_mem_set(buf, 0, sizeof(buf));
	ret = xi_file_read(client, buf, sizeof(buf));
	if (ret < 0) {
		log_print(XDLOG, "    - result : failed!!! (ret=%d)\n\n", ret);
		TC_EXIT_ERROR;
	}
	log_print(XDLOG, "    - result : pass. (rbytes=%d/msg=%s)\n\n", ret, buf);

	log_print(XDLOG, "  (%s:%02d) xi_file_write #############\n", tcname, t++);
	xi_strcpy(buf, "server file write!");
	ret = xi_file_write(client, buf, xi_strlen(buf));
	if (ret < 0) {
		log_print(XDLOG, "    - result : failed!!! (ret=%d)\n\n", ret);
		TC_EXIT_ERROR;
	}
	log_print(XDLOG, "    - result : pass. (wbytes=%d/msg=%s)\n\n", ret, buf);

	log_print(XDLOG, "  (%s:%02d) wait for client-end #######\n", tcname, t++);
	xi_thread_sleep(2000);

	log_print(XDLOG, "  (%s:%02d) xi_socket_close ###########\n", tcname, t++);
	ret = xi_socket_close(client);
	if (ret != XI_SOCK_RV_OK) {
		log_print(XDLOG, "    - result : client close err=%d!!!\n\n", ret);
		TC_EXIT_ERROR;
	}
	ret = xi_socket_close(svr_sock);
	if (ret != XI_SOCK_RV_OK) {
		log_print(XDLOG, "    - result : svr_sock close err=%d!!!\n\n", ret);
		TC_EXIT_ERROR;
	}
	log_print(XDLOG, "    - result : pass.\n\n", ret);

	log_print(XDLOG, "  (%s:%02d) signal to main thread #####\n", tcname, t++);
	_g_trun = FALSE;

	log_print(XDLOG, "Server Thread - ended <<<<<<<<<<<<<<<<<\n\n");

	return NULL;
}

static void tc_info() {
	log_print(XDLOG, "====================================================\n");
	log_print(XDLOG, "                 xi_socket.h - basic\n");
	log_print(XDLOG, "----------------------------------------------------\n");
	log_print(XDLOG, " * Functions)\n");
	log_print(XDLOG, "   - xi_socket_get_addr\n");
	log_print(XDLOG, "   - xi_socket_open\n");
	log_print(XDLOG, "   - xi_socket_opt_get\n");
	log_print(XDLOG, "   - xi_socket_opt_set\n");
	log_print(XDLOG, "   - xi_socket_bind\n");
	log_print(XDLOG, "   - xi_socket_listen\n");
	log_print(XDLOG, "   - xi_socket_connect\n");
	log_print(XDLOG, "   - xi_socket_accept\n");
	log_print(XDLOG, "   - xi_socket_get_local\n");
	log_print(XDLOG, "   - xi_socket_get_peer\n");
	log_print(XDLOG, "   - xi_socket_recv\n");
	log_print(XDLOG, "   - xi_socket_recvfrom\n");
	log_print(XDLOG, "   - xi_socket_send\n");
	log_print(XDLOG, "   - xi_socket_sendto\n");
	log_print(XDLOG, "   - xi_socket_shutdown\n");
	log_print(XDLOG, "   - xi_socket_close\n");
	log_print(XDLOG, "====================================================\n\n");
}

int tc_xi_socket_basic() {
	xint32 t = 1;
	xchar *tcname = "xi_socket.h";

	xint32 ret;
	xint32 i;

	xi_sock_addr_t qaddr[10];

	xi_thread_t tid_svr;
	xi_thread_t tid_clt;

	tc_info();

	log_print(XDLOG, "[%s:%02d] get_addr(www.daum.net) ######\n", tcname, t++);
	ret = xi_socket_get_addr("daum.net", qaddr, 10);
	if (ret < 0) {
		log_print(XDLOG, "    - result : not found!!!\n\n");
	}
	log_print(XDLOG, "    - result : pass. (www.daum.net = \n");
	for (i = 0; i < ret; i++) {
		log_print(XDLOG, "        %s\n", qaddr[i].host);
	}
	log_print(XDLOG, "      )\n\n");

	log_print(XDLOG, "[%s:%02d] server/client threads #######\n", tcname, t++);
	log_print(XDLOG, " server) create listener socket\n");
	log_print(XDLOG, " client) create connect socket\n");
	log_print(XDLOG, " => they will connect and communicate\n");
	log_print(XDLOG, " => main thread will waiting....\n\n");
	xi_thread_create(&tid_svr, "TC_SVR_THREAD", tc_server_thread, NULL,
			256 * 1024, XCFG_THREAD_PRIOR_NORM);
	xi_thread_sleep(50);
	xi_thread_create(&tid_clt, "TC_CLT_THREAD", tc_client_thread, NULL,
			256 * 1024, XCFG_THREAD_PRIOR_NORM);

	_g_trun = TRUE;
	while (_g_trun) {
		xi_thread_sleep(300);
	}

	log_print(XDLOG, "============ DONE [xi_socket.h - basic] ============\n\n");

	return _g_ecode;
}
