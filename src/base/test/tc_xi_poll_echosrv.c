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
#include "xi/xi_poll.h"
#include "xi/xi_socket.h"
#include "xi/xi_string.h"

static int _g_run = TRUE;
static int _g_client_num = 0;

typedef xvoid (callback_fn)(xi_pollset_t *pset, xi_pollfd_t *pfd);

/* Event Control Block (ecb) */
struct ecb {
	callback_fn		*do_read;
	callback_fn		*do_write;
	xchar			 buf[1024];
	xssize			 buflen;
};

static xvoid tc_client_send(xi_pollset_t *pset, xi_pollfd_t *pfd) {
	xssize ret;
	xi_pollfd_t cpfd;
	struct ecb *ecbp = (struct ecb *)pfd->context;

	log_print(XDLOG, "Client send\n");
	ret = xi_socket_send(pfd->desc, ecbp->buf, (xsize)ecbp->buflen);

    if (ret < 0) {
        log_error(XDLOG, "Failed to send to the socket: %d\n", ret);
        --_g_client_num;
		_g_run = FALSE;
        xi_socket_close(pfd->desc);
        xi_mem_free(ecbp);
        return;
    }
	log_print(XDLOG, "sent: %d bytes\n", ret);

    // setup a read event
	cpfd.desc = pfd->desc;
	cpfd.evts = XI_POLL_EVENT_IN;
	cpfd.context = ecbp;

	// disable write
    ret = xi_pollset_remove(pset, *pfd);
	if (ret != XI_POLLSET_RV_OK) {
		log_error(XDLOG, "Failed to remove the write filter: %d\n", ret);
        --_g_client_num;
		_g_run = FALSE;
        xi_socket_close(pfd->desc);
        xi_mem_free(ecbp);
        return;
	}

	// enable read
	ret = xi_pollset_add(pset, cpfd);
	if (ret != XI_POLLSET_RV_OK) {
		log_error(XDLOG, "Failed to add a read filter: %d\n", ret);
        --_g_client_num;
		_g_run = FALSE;
        xi_socket_close(pfd->desc);
        xi_mem_free(ecbp);
        return;
	}
}

static xvoid tc_client_recv(xi_pollset_t *pset, xi_pollfd_t *pfd) {
    xssize ret;
	xi_pollfd_t cpfd;
	struct ecb *ecbp = (struct ecb *)pfd->context;

	log_print(XDLOG, "Client receive\n");
    ecbp->buflen = xi_socket_recv(pfd->desc, ecbp->buf, sizeof(ecbp->buf));
	log_print(XDLOG, "received: %d bytes\n", ecbp->buflen);
    if (ecbp->buflen < 0) {
        log_error(XDLOG, "Failed to receive from the socket: %d\n", ecbp->buflen);
        --_g_client_num;
		_g_run = FALSE;
        xi_socket_close(pfd->desc);
        xi_mem_free(ecbp);
        return;
    } else if (ecbp->buflen == 0) {
    	log_print(XDLOG, "EOF! Disconnected. Close the client socket\n");
        --_g_client_num;
        ret = xi_pollset_remove(pset, *pfd);
    	if (ret != XI_POLLSET_RV_OK) {
    		log_error(XDLOG, "Failed to remove the read filter: %d\n", ret);
    		_g_run = FALSE;
            xi_socket_close(pfd->desc);
            xi_mem_free(ecbp);
            return;
    	}
        if (_g_client_num == 0) {
        	log_print(XDLOG, "All clients have been disconnected. Exit this echo server.\n\n");
        	_g_run = FALSE;
        }
        xi_socket_close(pfd->desc);
        xi_mem_free(ecbp);
        return;
    }

    // setup a write event
	cpfd.desc = pfd->desc;
	cpfd.evts = XI_POLL_EVENT_OUT;
	cpfd.context = ecbp;

    // disable read
    ret = xi_pollset_remove(pset, *pfd);
	if (ret != XI_POLLSET_RV_OK) {
		log_error(XDLOG, "Failed to remove the read filter: %d\n", ret);
		_g_run = FALSE;
        xi_socket_close(pfd->desc);
        xi_mem_free(ecbp);
        return;
	}

	// enable write
	ret = xi_pollset_add(pset, cpfd);
	if (ret != XI_POLLSET_RV_OK) {
		log_error(XDLOG, "Failed to add a write filter: %d\n", ret);
		_g_run = FALSE;
        xi_socket_close(pfd->desc);
        xi_mem_free(ecbp);
        return;
	}
}

static xvoid tc_accept_client(xi_pollset_t *pset, xi_pollfd_t *pfd) {
	int ret;
	xi_sock_t clisock;
	xi_sock_addr_t cliaddr;
	struct ecb *ecb_client;
	xi_pollfd_t cpfd;

	// accept a client
	log_print(XDLOG, "Accept a client\n");
	clisock = xi_socket_accept(pfd->desc, &cliaddr);
	if (clisock < 0) {
		log_error(XDLOG, "Failed to accept a client!!\n");
		_g_run = FALSE;
		return;
	}

	// set an event control block for the new client
	ecb_client = (struct ecb *) xi_mem_calloc(1, sizeof(struct ecb));
	ecb_client->do_read = &tc_client_recv;
	ecb_client->do_write = &tc_client_send;

    // setup an event for the client
	cpfd.desc = clisock;
	cpfd.evts = XI_POLL_EVENT_IN;
	cpfd.context = ecb_client;

	// add the client to the pollset
	log_print(XDLOG, "register client> clisock: %d, ctx: %p\n", clisock, cpfd.context);
	ret = xi_pollset_add(pset, cpfd);
	if (ret != XI_POLLSET_RV_OK) {
        log_error(XDLOG, "Failed to register a client: %d\n", ret);
		_g_run = FALSE;
		xi_mem_free(ecb_client);
		xi_socket_close(clisock);
		return;
	}

	++_g_client_num;
}

static void tc_start_event_loop(xi_pollset_t *pset) {
	int cnt = 0;

	log_print(XDLOG, "\nThe echo server is running. Connect to the port 56789\n");

	while (_g_run) {
		int i, ret;
		xi_pollfd_t rfds[64];

		log_print(XDLOG, "\n\n=============== event_loop: %d ==================\n", cnt);
		ret = xi_pollset_poll(pset, rfds, 64, -1);

		if (ret < 0) {
			log_error(XDLOG, "Failed to poll: %d\n", ret);
			break;
		} else if (ret == 0) {
			log_error(XDLOG, "timeout\n");
			break;
		}

		for (i = 0; i < ret; i++) {
			struct ecb *ecbp = (struct ecb *)(rfds[i].context);
			if (rfds[i].evts & XI_POLL_EVENT_IN) {
				log_print(XDLOG, " IN> fd: %d, ctx: %p\n", rfds[i].desc, ecbp);
//				log_print(XDLOG, "   *** do_read> BEGIN\n");
				ecbp->do_read(pset, &rfds[i]);
//				log_print(XDLOG, "   *** do_read> END\n");
			} else {
				log_print(XDLOG, "OUT>: fd: %d, ctx: %p\n", rfds[i].desc, ecbp);
//				log_print(XDLOG, "   ### do_write> BEGIN\n");
				ecbp->do_write(pset, &rfds[i]);
//				log_print(XDLOG, "   ### do_write> END\n");
			}
		}
		++cnt;
	}

}

static xint32 tc_start_server(xi_sock_t srvsock) {
	xint32 ret;
	xi_pollset_t *pset;
	xi_pollfd_t pfd;
	struct ecb ecb_server;

	// create a pollset
	pset = xi_pollset_create(64, XI_POLLSET_OPT_EPOLL);
	if (pset == NULL) {
		log_error(XDLOG, "Failed to create a pollset!!\n");
		xi_socket_close(srvsock);
		return -1;
	}

	// set an event control block for the server
    ecb_server.do_read = &tc_accept_client;

    // setup an event for the server
	pfd.desc = srvsock;
	pfd.evts = XI_POLL_EVENT_IN;
	pfd.context = &ecb_server;

	// register the server
	log_print(XDLOG, "register server> srvsock: %d, ctx: %p\n", srvsock, pfd.context);
	ret = xi_pollset_add(pset, pfd);
	if (ret == XI_POLLSET_RV_OK) {
		// start event loop
		tc_start_event_loop(pset);
	}

    // cleanup resources
    xi_socket_close(srvsock);
	xi_pollset_destroy(pset);
	if (ret != XI_POLLSET_RV_OK) {
		log_error(XDLOG, "Failed to destroy the pollset: %d\n", ret);
		return -1;
	}

	log_print(XDLOG, "============ DONE [xi_poll.h - echo server] ============\n\n");

    return 0;
}

static xvoid tc_info() {
	log_print(XDLOG, "====================================================\n");
	log_print(XDLOG, "                      xi_poll.h - echo server\n");
	log_print(XDLOG, "----------------------------------------------------\n");
	log_print(XDLOG, " * Functions)\n");
	log_print(XDLOG, "   - xi_pollset_create\n");
	log_print(XDLOG, "   - xi_pollset_add\n");
	log_print(XDLOG, "   - xi_pollset_remove\n");
	log_print(XDLOG, "   - xi_pollset_destroy\n");
	log_print(XDLOG, "====================================================\n\n");
}

xint32 tc_xi_poll_echosrv() {
	xint32 ret;
	xi_sock_t srvsock;
    xi_sock_addr_t lsnaddr = {XI_SOCK_FAMILY_INET, XI_SOCK_TYPE_STREAM,
    		XI_SOCK_PROTO_TCP, {'\0'}, 56789};

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
