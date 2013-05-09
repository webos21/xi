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

#ifndef _XI_SOCKET_H_
#define _XI_SOCKET_H_

/**
 * @brief XI Socket API
 *
 * @file xi_socket.h
 * @date 2010-08-31
 * @author Cheolmin Jo (webos21@gmail.com)
 */

#include "xtype.h"

/**
 * Start Declaration
 */
_XI_EXTERN_C_BEGIN

/**
 * @defgroup xi_socket Socket API
 * @ingroup XI
 * @{
 * @brief
 *
 * Define the API to handle the TCP/IP sockets.
 */

/**
 * Socket Return Values
 */
typedef enum _e_sock_rv {
	XI_SOCK_RV_OK           = 0,   ///< OK
	XI_SOCK_RV_ERR_CLOSE    = 0,   ///< Peer is closed (Recv/Send)
	XI_SOCK_RV_ERR_FD       = -1,  ///< Error File Descripter
	XI_SOCK_RV_ERR_NOTCONN  = -2,  ///< Not connected
	XI_SOCK_RV_ERR_NOTSOCK  = -3,  ///< Not a socket
	XI_SOCK_RV_ERR_ADDR     = -4,  ///< Address is invalid / Specific address is needed
	XI_SOCK_RV_ERR_INUSE    = -5,  ///< Address already in use
	XI_SOCK_RV_ERR_NOENTRY  = -6,  ///< File not found (Local Socket Only)
	XI_SOCK_RV_ERR_PERM     = -7,  ///< Permission Denyed (Local Socket Only)
	XI_SOCK_RV_ERR_NS       = -8,  ///< Not support
	XI_SOCK_RV_ERR_REFUSED  = -9,  ///< Connection is refused
	XI_SOCK_RV_ERR_UNREACH  = -10, ///< Connection is unreachable
	XI_SOCK_RV_ERR_TIMEOUT  = -11, ///< Connection or Poll reached to the timeout
	XI_SOCK_RV_ERR_ALREADY  = -12, ///< Already connected
	XI_SOCK_RV_ERR_OVERFLOW = -13, ///< Send buffer is full
	XI_SOCK_RV_ERR_INTR     = -14, ///< Interrup is occurred
	XI_SOCK_RV_ERR_HOST_NF  = -15, ///< Host not found
	XI_SOCK_RV_ERR_NOADDR   = -16, ///< No available address
	XI_SOCK_RV_ERR_TRYLATER = -17, ///< Try it later
	XI_SOCK_RV_ERR_ARGS     = -18  ///< Invalid Arguments
} xi_sock_re;


/**
 * Socket Family
 */
typedef enum _e_sock_family {
	XI_SOCK_FAMILY_UNSPEC = 0,  ///< Unspecified Socket Family (For Connected UDP)
	XI_SOCK_FAMILY_LOCAL  = 1,  ///< Local or Unix Socket Family
	XI_SOCK_FAMILY_INET   = 4,  ///< IPv4 Socket Family
	XI_SOCK_FAMILY_INET6  = 6   ///< IPv6 Socket Family
} xi_sock_family_e;


/**
 * Socket Type
 */
typedef enum _e_sock_type {
	XI_SOCK_TYPE_STREAM   = 1,  ///< Use Stream, Connection-oriented Type
	XI_SOCK_TYPE_DATAGRAM = 2,  ///< Use Datagram, Connection-less Type
	XI_SOCK_TYPE_RAW      = 3   ///< Use Custom, Raw Type
} xi_sock_type_e;


/**
 * Socket Protocol
 */
typedef enum _e_sock_proto {
	XI_SOCK_PROTO_IP    = 0,   ///< Dummy protocol for TCP
	XI_SOCK_PROTO_TCP   = 1,   ///< Transmission Control Protocol
	XI_SOCK_PROTO_UDP   = 2,   ///< User Datagram Protocol
	XI_SOCK_PROTO_RAW   = 3    ///< Raw IP packets
} xi_sock_proto_e;


/**
 * Socket Shutdown Method
 */
typedef enum _e_sock_shutdown {
	XI_SOCK_SHUTDOWN_RD    = 0,  ///< Read is disabled
	XI_SOCK_SHUTDOWN_WR    = 1,  ///< Write is disabled
	XI_SOCK_SHUTDOWN_RDWR  = 2   ///< Read/Write are all disabled
} xi_sock_shutdown_e;


/**
 * Socket Option
 */
typedef enum _e_sock_opt {
	XI_SOCK_OPT_LINGER     = 1,     ///< Lingers on close on/off
	XI_SOCK_OPT_KEEPALIVE  = 2,     ///< Keep connections active on/off
	XI_SOCK_OPT_DEBUG      = 4,     ///< Debug Info on/off
	XI_SOCK_OPT_NONBLOCK   = 8,     ///< Turn blocking on/off
	XI_SOCK_OPT_REUSEADDR  = 16,    ///< Reuse address on/off
	XI_SOCK_OPT_SENDBUF    = 64,    ///< Send Buffer Size
	XI_SOCK_OPT_RECVBUF    = 128,   ///< Receive Buffer Size
	XI_SOCK_OPT_SNDTIMEO   = 140,   ///< Send Timeout
	XI_SOCK_OPT_RCVTIMEO   = 141,   ///< Receive Timeout
	XI_SOCK_OPT_IPV6ONLY   = 256    ///< IPv6 Only on/off
} xi_sock_opt_e;


/**
 * Socket address type, used to ensure protocol independence.
 */
typedef struct _st_sock_addr {
	xi_sock_family_e  family;                     ///< family
	xi_sock_type_e    type;                       ///< type
	xi_sock_proto_e   proto;                      ///< protocol
	xchar             host[XCFG_HOSTNAME_MAX];    ///< host
	xuint16           port;                       ///< port
} xi_sock_addr_t;


/**
 * Abstract handle of socket (same as file descriptor)
 */
typedef xfd  xi_sock_t;


/**
 * Create a socket.
 *
 * @param family The address family of the socket (e.g., XI_SOCK_FAMILY_INET).
 * @param type The type of the socket (e.g., XI_SOCK_TYPE_STREAM).
 * @param proto The protocol of the socket (e.g., XI_SOCK_PROTO_TCP).
 * @return The new socket that has been set up.
 */
xint32       xi_socket_open(xi_sock_family_e family, xi_sock_type_e type, xi_sock_proto_e proto);


/**
 * Bind the socket to its associated port
 *
 * @param sfd The socket to bind
 * @param baddr The socket address to bind to
 *
 * @remark This may be where we will find out if there is any other process
 *      using the selected port.
 */
xi_sock_re   xi_socket_bind(xint32 sfd, xi_sock_addr_t baddr);


/**
 * Listen to a bound socket for connections.
 *
 * @param sfd The socket to listen on
 * @param backlog The number of outstanding connections allowed in the sockets
 *                listen queue.  If this value is less than zero, the listen
 *                queue size is set to zero.
 */
xi_sock_re   xi_socket_listen(xint32 sfd, xint32 backlog);


/**
 * Accept a new connection request
 *
 * @param sfd The socket we are listening on.
 * @param fromaddr Newly arrived connection address.
 * @return A copy of the socket that is connected to the socket that
 *         made the connection request.  This is the socket which should
 *         be used for all future communication.
 */
xint32       xi_socket_accept(xint32 sfd, xi_sock_addr_t *fromaddr);


/**
 * Issue a connection request to a socket either on the same machine
 * or a different one.
 *
 * @param sfd The socket we wish to use for our side of the connection
 * @param caddr The address of the machine we wish to connect to.
 */
xi_sock_re   xi_socket_connect(xint32 sfd, xi_sock_addr_t caddr);


/**
 * Setup socket options for the specified socket
 *
 * @param sfd The socket to set up.
 * @param opt The option we would like to configure.
 * @param val Value for the option.
 *
 * @see xi_sock_opt_e
 */
xi_sock_re   xi_socket_opt_set(xint32 sfd, xi_sock_opt_e opt, xint32 val);


/**
 * Query socket options for the specified socket
 *
 * @param sfd The socket to query
 * @param opt The option we would like to query.
 * @param val Socket option returned on the call.
 *
 * @see xi_sock_opt_e
 */
xi_sock_re   xi_socket_opt_get(xint32 sfd, xi_sock_opt_e opt, xint32 *val);


/**
 * Read data from a network.
 *
 * @param sfd The socket to read the data from.
 * @param buf The buffer to store the data in.
 * @param blen On entry, the number of bytes to receive; on exit, the number
 *             of bytes received.
 * @remark
 * This functions acts like a blocking read by default.  To change
 * this behavior, use the XI_SOCK_OPT_NONBLOCK socket option.
 * The number of bytes actually received is stored in argument 3.\n
 * It is possible for both bytes to be received and an EOF or
 * other error to be returned.
 */
xssize       xi_socket_recv(xint32 sfd, xvoid *buf, xsize blen);


/**
 * Read data from a socket.  On success, the address of the peer from
 * which the data was sent is copied into the @a from parameter, and the
 * @a blen parameter is updated to give the number of bytes written to
 * @a buf.
 *
 * @param sfd The socket to use
 * @param buf  The buffer to use
 * @param blen  The length of the available buffer
 * @param fromaddr Updated with the address from which the data was received
 */
xssize       xi_socket_recvfrom(xint32 sfd, xvoid *buf, xsize blen, xi_sock_addr_t *fromaddr);


/**
 * Send data over a network.
 *
 * @param sfd The socket to send the data over.
 * @param buf The buffer which contains the data to be sent.
 * @param blen On entry, the number of bytes to send; on exit, the number
 *            of bytes sent.
 * @remark
 * This functions acts like a blocking write by default.  To change
 * this behavior, use the XI_SOCK_OPT_NONBLOCK socket option.\n
 * \n
 * It is possible for both bytes to be sent and an error to be returned.
 */
xssize       xi_socket_send(xint32 sfd, const xvoid *buf, xsize blen);


/**
 * Send data over a network
 *
 * @param sfd The socket to send from
 * @param buf  The data to send
 * @param blen  The length of the data to send
 * @param toaddr The xi_sockaddr_t describing where to send the data
 */
xssize       xi_socket_sendto(xint32 sfd, const xvoid *buf, xsize blen, xi_sock_addr_t toaddr);


/**
 * Send file over a network
 *
 * @param sfd     The socket handle to send via
 * @param ffd     The file handle to send
 * @param offset  The start position of file to be sent
 * @param count   The length of the data to be sent
 * @return        The length to be sent if success, otherwise error value < 0
 */
xssize       xi_socket_sendfile(xint32 sfd, xint32 ffd, xoff64 *offset, xsize count);


/**
 * Shutdown either reading, writing, or both sides of a socket.
 *
 * @param sfd The socket to shutdown
 * @param how How to shutdown the socket.  One of:
 *            XI_SOCK_SHUTDOWN_RD      no longer allow read requests
 *            XI_SOCK_SHUTDOWN_WR      no longer allow write requests
 *            XI_SOCK_SHUTDOWN_RDWR    no longer allow read or write requests
 *
 * @see xi_sock_shutdown_e
 *
 * @remark This does not actually close the socket descriptor, it just
 *      controls which calls are still valid on the socket.
 */
xi_sock_re   xi_socket_shutdown(xint32 sfd, xi_sock_shutdown_e how);


/**
 * Close a socket.
 *
 * @param sfd The socket to close
 */
xi_sock_re   xi_socket_close(xint32 sfd);


/**
 * Query the IP address by host-name.
 *
 * @param buf The host-name buffer.
 * @param buflen The length of buffer.
 * @return The count of host-name length
 */
xint32       xi_socket_get_hostname(xchar *buf, xint32 buflen);


/**
 * Query the IP address by host-name.
 *
 * @param hostname The host-name to query.
 * @param addr The array of xi_sock_addr_t to receive IP addresses
 * @param addrlen The length of the array of xi_sock_addr_t
 * @return The count of IP addresses
 */
xint32       xi_socket_get_addr(const xchar *hostname, xi_sock_addr_t *addr, xint32 addrlen);


/**
 * Return an address of the peer
 *
 * @param sfd The socket to use
 * @param addr The returned xi_sockaddr_t.
 */
xi_sock_re   xi_socket_get_peer(xint32 sfd, xi_sock_addr_t *addr);


/**
 * Return an address of the local
 *
 * @param sfd The socket to use
 * @param addr The returned xi_sockaddr_t.
 */
xi_sock_re   xi_socket_get_local(xint32 sfd, xi_sock_addr_t *addr);


/**
 * Join a Multicast Group
 *
 * @param sfd The socket to join a multicast group
 * @param iface Address of the interface to use.  If NULL is passed, the
 *              default multicast interface will be used. (OS Dependent)
 * @param grp The address of the multicast group to join
 * @param src Source Address to accept transmissions from (non-NULL
 *            implies Source-Specific Multicast)
 */
xi_sock_re   xi_mcast_join(xint32 sfd, xi_sock_addr_t iface, xi_sock_addr_t grp, xi_sock_addr_t *src);


/**
 * Leave a Multicast Group.  All arguments must be the same as
 * xi_mcast_join.
 *
 * @param sfd The socket to leave a multicast group
 * @param iface Address of the interface to use.  If NULL is passed, the
 *              default multicast interface will be used. (OS Dependent)
 * @param grp The address of the multicast group to leave
 * @param src Source Address to accept transmissions from (non-NULL
 *            implies Source-Specific Multicast)
 */
xi_sock_re   xi_mcast_leave(xint32 sfd, xi_sock_addr_t iface, xi_sock_addr_t grp, xi_sock_addr_t *src);

/**
 * @}  // end of xi_socket
 */

/**
 * End Declaration
 */
_XI_EXTERN_C_END

#endif // _XI_SOCKET_H_
