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
 * File : tc_xi_poll_echosrv.c
 */

#include "xi/xtype.h"
#include "xi/xi_log.h"
#include "xi/xi_mem.h"
#include "xi/xi_select.h"
#include "xi/xi_socket.h"
#include "xi/xi_string.h"

/* Event Control Block (ecb) */
struct ecb {
	xint32 brun;
	xint32 cnum;
	xint32 ssck;
	xint32 nfds;
	xi_fdset_t *rfds;
	xint32 clients[64];
};

static xvoid tc_client_echo(struct ecb *ectx, xint32 csck) {
	xssize ret;
	xchar buf[1024];

	log_print(XDLOG, "Client:%d receive\n", csck);
	ret = xi_socket_recv(csck, buf, sizeof(buf));
	log_print(XDLOG, "received: %d bytes\n", ret);
	if (ret < 0) {
		log_error(XDLOG, "Failed to receive from the socket: %d\n", ret);
		ectx->cnum--;
		ectx->brun = FALSE;
		xi_socket_close(csck);
		return;
	} else if (ret == 0) {
		log_print(XDLOG, "EOF! Disconnected. Close the client socket\n");
		ectx->clients[csck] = -1;
		ectx->cnum--;
		if (ectx->cnum == 0) {
			log_print(XDLOG, "All clients have been disconnected. Exit this echo server.\n\n");
			ectx->brun = FALSE;
		}
		xi_socket_close(csck);
		return;
	}

	ret = xi_socket_send(csck, buf, (xsize)ret);
	if (ret < 0) {
		log_error(XDLOG, "Failed to send to the socket: %d\n", ret);
		ectx->cnum--;
		ectx->brun = FALSE;
		return;
	}
	log_print(XDLOG, "sent: %d bytes\n", ret);
}

static xvoid tc_accept_client(struct ecb *ectx) {
	xint32 csck;
	xi_sock_addr_t cliaddr;

	// accept a client
	log_print(XDLOG, "Accept a client\n");
	csck = xi_socket_accept(ectx->ssck, &cliaddr);
	if (csck < 0) {
		log_error(XDLOG, "Failed to accept a client!!\n");
		ectx->brun = FALSE;
		return;
	}

	// setup an event for the client
	xi_sel_fdset(csck, ectx->rfds);

	// set an event control block for the new client
	ectx->nfds = (ectx->nfds > csck + 1) ? ectx->nfds : csck + 1;
	ectx->clients[csck] = csck;

	ectx->cnum++;
}

static void tc_start_event_loop(struct ecb *ectx) {
	int cnt = 0;

	log_print(XDLOG, "\nThe echo server is running. Connect to the port 56789\n");

	while (ectx->brun) {
		int i, ret;

		// setup an event for the server
		xi_sel_fdzero(ectx->rfds);
		xi_sel_fdset(ectx->ssck, ectx->rfds);
		for (i = 0; i < ectx->nfds; i++) {
			if (ectx->clients[i] > 0) {
				xi_sel_fdset(ectx->clients[i], ectx->rfds);
			}
		}

		log_print(XDLOG, "\n\n=============== event_loop: %d ==================\n", cnt);
		ret = xi_sel_select(ectx->nfds, ectx->rfds, NULL, NULL, -1);
		if (ret < 0) {
			log_error(XDLOG, "Failed to select: %d\n", ret);
			break;
		} else if (ret == 0) {
			log_error(XDLOG, "timeout\n");
			break;
		}

		for (i = 0; i < ectx->nfds; i++) {
			if (xi_sel_fdisset(i, ectx->rfds)) {
				if (i == ectx->ssck) {
					tc_accept_client(ectx);
				} else {
					tc_client_echo(ectx, i);
				}
			}
		}
		++cnt;
	}

}

static xint32 tc_start_server(xint32 srvsock) {
	xint32 i;
	xi_fdset_t *rfd;
	struct ecb ectx;

	// create a rfd-set
	rfd = xi_sel_fdcreate();
	if (rfd == NULL) {
		log_error(XDLOG, "Failed to create a rfd-set!!\n");
		xi_socket_close(srvsock);
		return -1;
	}

	// setup an event for the server
	xi_sel_fdzero(rfd);
	xi_sel_fdset(srvsock, rfd);

	// set an event control block for the server
	ectx.brun = TRUE;
	ectx.cnum = 0;
	ectx.ssck = srvsock;
	ectx.nfds = srvsock + 1;
	ectx.rfds = rfd;
	for (i = 0; i < 64; i++) {
		ectx.clients[i] = -1;
	}

	// start event loop
	tc_start_event_loop(&ectx);

	// cleanup resources
	xi_socket_close(srvsock);
	xi_sel_fddestroy(rfd);

	log_print(XDLOG, "============ DONE [xi_poll.h - echo server] ============\n\n");

	return 0;
}

static xvoid tc_info() {
	log_print(XDLOG, "====================================================\n");
	log_print(XDLOG, "                 xi_select.h - echo server\n");
	log_print(XDLOG, "----------------------------------------------------\n");
	log_print(XDLOG, " * Functions)\n");
	log_print(XDLOG, "   - xi_sel_fdcreate\n");
	log_print(XDLOG, "   - xi_sel_fdzero\n");
	log_print(XDLOG, "   - xi_sel_fdset\n");
	log_print(XDLOG, "   - xi_sel_select\n");
	log_print(XDLOG, "   - xi_sel_fdisset\n");
	log_print(XDLOG, "   - xi_sel_fddestroy\n");
	log_print(XDLOG, "====================================================\n\n");
}

xint32 tc_xi_select_echosrv() {
	xint32 ret;
	xint32 srvsock;
	xi_sock_addr_t lsnaddr = { XI_SOCK_FAMILY_INET, XI_SOCK_TYPE_STREAM,
			XI_SOCK_PROTO_TCP, { '\0' }, 56789 };

	tc_info();

	// setup a listen socket
	srvsock = xi_socket_open(lsnaddr.family, lsnaddr.type, lsnaddr.proto);
	if (srvsock < 0) {
		log_error(XDLOG, "Failed to creat a socket!\n");
		return -1;
	}

	// set socket options
	ret = xi_socket_opt_set(srvsock, XI_SOCK_OPT_REUSEADDR, TRUE);
	if (ret < 0) {
		log_error(XDLOG, "Failed to setsocketopt: %d\n", ret);
		return -1;
	}

	// setup a local addr
	xi_strcpy(lsnaddr.host, "0.0.0.0");

	// bind to the listen socket
	ret = xi_socket_bind(srvsock, lsnaddr);
	if (ret < 0) {
		log_error(XDLOG, "Failed to bind a socket: %d\n", ret);
		return -1;
	}

	// listen
	ret = xi_socket_listen(srvsock, 10);
	if (ret < 0) {
		log_error(XDLOG, "Failed to listen to socket: %d\n", ret);
		return -1;
	}

	// start a server
	return tc_start_server(srvsock);
}
