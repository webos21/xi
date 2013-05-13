/*
 * Copyright (C) 2007 The Android Open Source Project
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

#include "org_apache_harmony_luni_platform_OSNetworkSystem.h"

#include "AsynchronousSocketCloseMonitor.h"
#include "JNIHelp.h"
#include "JniConstants.h"
#include "JniException.h"
#include "LocalArray.h"
#include "NetFd.h"
#include "NetworkUtilities.h"
#include "ScopedPrimitiveArray.h"
#include "valueOf.h"

#include "xi/xi_log.h"
#include "xi/xi_mem.h"
#include "xi/xi_string.h"
#include "xi/xi_socket.h"
#include "xi/xi_select.h"
#include "xi/xi_poll.h"

#if 0 // by cmjo
#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/un.h>
#endif // 0
// Temporary hack to build on systems that don't have up-to-date libc headers.
#ifndef IPV6_TCLASS
#ifdef __linux__
#define IPV6_TCLASS 67 // Linux
#else
#define IPV6_TCLASS -1 // BSD(-like); TODO: Something better than this!
#endif
#endif

#define EBADF            9      /* Bad file number */

#define EACCES          13      /* Permission denied */
#define EFAULT          14      /* Bad address */

#define EINVAL          22      /* Invalid argument */

#define EADDRINUSE      98      /* Address already in use */
#define EADDRNOTAVAIL   99      /* Cannot assign requested address */
#define ENETDOWN        100     /* Network is down */
#define ENETUNREACH     101     /* Network is unreachable */
#define ENETRESET       102     /* Network dropped connection because of reset */
#define ECONNABORTED    103     /* Software caused connection abort */
#define ECONNRESET      104     /* Connection reset by peer */
#define ENOBUFS         105     /* No buffer space available */
#define EISCONN         106     /* Transport endpoint is already connected */
#define ENOTCONN        107     /* Transport endpoint is not connected */
#define ESHUTDOWN       108     /* Cannot send after transport endpoint shutdown */
#define ETOOMANYREFS    109     /* Too many references: cannot splice */
#define ETIMEDOUT       110     /* Connection timed out */
#define ECONNREFUSED    111     /* Connection refused */
#define EHOSTDOWN       112     /* Host is down */
#define EHOSTUNREACH    113     /* No route to host */
#define EALREADY        114     /* Operation already in progress */
#define EINPROGRESS     115     /* Operation now in progress */

/*
 * TODO: The multicast code is highly platform-dependent, and for now
 * we just punt on anything but Linux.
 */
#if 1 //def __linux__
#define ENABLE_MULTICAST
#endif

#define JAVASOCKOPT_IP_MULTICAST_IF 16
#define JAVASOCKOPT_IP_MULTICAST_IF2 31
#define JAVASOCKOPT_IP_MULTICAST_LOOP 18
#define JAVASOCKOPT_IP_TOS 3
#define JAVASOCKOPT_MCAST_JOIN_GROUP 19
#define JAVASOCKOPT_MCAST_LEAVE_GROUP 20
#define JAVASOCKOPT_MULTICAST_TTL 17
#define JAVASOCKOPT_SO_BROADCAST 32
#define JAVASOCKOPT_SO_KEEPALIVE 8
#define JAVASOCKOPT_SO_LINGER 128
#define JAVASOCKOPT_SO_OOBINLINE  4099
#define JAVASOCKOPT_SO_RCVBUF 4098
#define JAVASOCKOPT_SO_TIMEOUT  4102
#define JAVASOCKOPT_SO_REUSEADDR 4
#define JAVASOCKOPT_SO_SNDBUF 4097
#define JAVASOCKOPT_TCP_NODELAY 1

/* constants for OSNetworkSystem_selectImpl */
#define SOCKET_OP_NONE 0
#define SOCKET_OP_READ 1
#define SOCKET_OP_WRITE 2

/*
 static struct CachedFields {
 jfieldID iaddr_ipaddress;
 jfieldID integer_class_value;
 jfieldID boolean_class_value;
 jfieldID socketimpl_address;
 jfieldID socketimpl_port;
 jfieldID socketimpl_localport;
 jfieldID dpack_address;
 jfieldID dpack_port;
 jfieldID dpack_length;
 } gCachedFields;
 */

/**
 * Returns the port number in a sockaddr_storage structure.
 *
 * @param address the sockaddr_storage structure to get the port from
 *
 * @return the port number, or -1 if the address family is unknown.
 */
static int getSocketAddressPort(xi_sock_addr_t* ss) {
	return ss->port;
}

/**
 * Obtain the socket address family from an existing socket.
 *
 * @param socket the file descriptor of the socket to examine
 * @return an integer, the address family of the socket
 */
static int getSocketAddressFamily(int socket) {
	int ret = 0;
	xi_sock_addr_t addr;

	ret = xi_socket_get_local(socket, &addr);
	if (ret < 0) {
		// Windows getsockname is different from linux
		// It does fail on the FD (not operated by connect/accept)
		// So, we need assume, if it failed AF_INET
		return XI_SOCK_FAMILY_INET;
	} else {
		return addr.family;
	}
}

// Handles translating between IPv4 and IPv6 addresses so -- where possible --
// we can use either class of address with either an IPv4 or IPv6 socket.
class CompatibleSocketAddress {
public:
	// Constructs an address corresponding to 'ss' that's compatible with 'fd'.
	CompatibleSocketAddress(int fd, const xi_sock_addr_t& ss,
			bool mapUnspecified) {
		mCompatibleAddress = reinterpret_cast<const xi_sock_addr_t*> (&ss);
	}

	// Returns a pointer to an address compatible with the socket.
	const xi_sock_addr_t* get() const {
		return mCompatibleAddress;
	}
private:
	const xi_sock_addr_t* mCompatibleAddress;
};

/**
 * Converts an InetAddress object and port number to a native address structure.
 */
static bool inetAddressToSocketAddress(JNIEnv* env, jobject inetAddress,
		int port, xi_sock_addr_t* ss) {
	// Get the byte array that stores the IP address bytes in the InetAddress.
	if (inetAddress == NULL) {
		jniThrowNullPointerException(env, NULL);
		return false;
	}

	jclass inetAddressClass = env->FindClass("java/net/InetAddress");
	jfieldID iaddr_ipaddress = env->GetFieldID(inetAddressClass, "ipaddress",
			"[B");
	jbyteArray addressBytes =
			reinterpret_cast<jbyteArray> (env->GetObjectField(inetAddress,
					iaddr_ipaddress));

	return byteArrayToSocketAddress(env, NULL, addressBytes, port, ss);
}

/*
 // Converts a number of milliseconds to a timeval.
 static timeval toTimeval(long ms) {
 timeval tv;
 tv.tv_sec = ms / 1000;
 tv.tv_usec = (ms - tv.tv_sec * 1000) * 1000;
 return tv;
 }

 // Converts a timeval to a number of milliseconds.
 static long toMs(const timeval& tv) {
 return tv.tv_sec * 1000 + tv.tv_usec / 1000;
 }
 */

/**
 * Query OS for timestamp.
 * Retrieve the current value of system clock and convert to milliseconds.
 *
 * @param[in] portLibrary The port library.
 *
 * @return 0 on failure, time value in milliseconds on success.
 * @deprecated Use @ref time_hires_clock and @ref time_hires_delta
 *
 * technically, this should return uint64_t since both timeval.tv_sec and
 * timeval.tv_usec are long
 */

/*
 static int time_msec_clock() {
 timeval tp;
 struct timezone tzp;
 gettimeofday(&tp, &tzp);
 return toMs(tp);
 }
 */

/**
 * Establish a connection to a peer with a timeout.  The member functions are called
 * repeatedly in order to carry out the connect and to allow other tasks to
 * proceed on certain platforms. The caller must first call ConnectHelper::start.
 * if the result is -EINPROGRESS it will then
 * call ConnectHelper::isConnected until either another error or 0 is returned to
 * indicate the connect is complete.  Each time the function should sleep for no
 * more than 'timeout' milliseconds.  If the connect succeeds or an error occurs,
 * the caller must always end the process by calling ConnectHelper::done.
 *
 * Member functions return 0 if no errors occur, otherwise -errno. TODO: use +errno.
 */
class ConnectHelper {
public:
	ConnectHelper(JNIEnv* env) :
		mEnv(env) {
	}

	int start(NetFd& fd, jobject inetAddr, jint port) {
		int ret = XI_SOCK_RV_ERR_ARGS;
		xi_sock_addr_t ss;
		if (!inetAddressToSocketAddress(mEnv, inetAddr, port, &ss)) {
			return -EINVAL; // Bogus, but clearly a failure, and we've already thrown.
		}

		log_trace(XDLOG, "!!!!!!!!! conn start !!!!!! fd=%d / ss.host=%s\n", fd.get(), ss.host);

		// Set the socket to non-blocking and initiate a connection attempt...
		//const CompatibleSocketAddress compatibleAddress(fd.get(), ss, true);
		setBlocking(fd.get(), false);
		if ((ret = xi_socket_connect(fd.get(), ss)) < 0) {
			//		if (xi_socket_connect(fd.get(), ss) < 0) {
			if (fd.isClosed()) {
				log_error(XDLOG, "!!!!!!!!! conn closed !!!!! fd=%d / ss.host=%s\n", fd.get(), ss.host);
				return -EINVAL; // Bogus, but clearly a failure, and we've already thrown.
			}
			if (ret == XI_SOCK_RV_ERR_TRYLATER) {
				log_trace(XDLOG, "!!!!!!!!! conn progress !!!!!! fd=%d / ss.host=%s\n", fd.get(), ss.host);
				return -EINPROGRESS;
			} else {
				log_error(XDLOG, "!!!!!!!!! conn error !!!!!! fd=%d / ss.host=%s\n", fd.get(), ss.host);
				didFail(fd.get(), -ENETUNREACH);
				return -EINVAL;
			}
			//			if (errno != EINPROGRESS) {
			//				didFail(fd.get(), -errno);
			//			}
		}
		// We connected straight away!
		didConnect(fd.get());
		return 0;
	}

	// Returns 0 if we're connected; -EINPROGRESS if we're still hopeful, -errno if we've failed.
	// 'timeout' the timeout in milliseconds. If timeout is negative, perform a blocking operation.
#if 1
	int isConnected(int fd, int timeout) {
		// Initialize the fd sets for the select.
		xi_fdset_t *readSet = xi_sel_fdcreate();
		xi_fdset_t *writeSet = xi_sel_fdcreate();

		xi_sel_fdzero(readSet);
		xi_sel_fdzero(writeSet);
		xi_sel_fdset(fd, readSet);
		xi_sel_fdset(fd, writeSet);

		int nfds = fd + 1;
		int rc = xi_sel_select(nfds, readSet, writeSet, NULL, timeout);
		if (rc < 0) {
			xi_sel_fddestroy(readSet);
			xi_sel_fddestroy(writeSet);
			return rc;
		} else if (rc == 0) {
			xi_sel_fddestroy(readSet);
			xi_sel_fddestroy(writeSet);
			return -EINPROGRESS;
		}

		// If the fd is just in the write set, we're connected.
		if (xi_sel_fdisset(fd, writeSet) && !xi_sel_fdisset(fd, readSet)) {
			xi_sel_fddestroy(readSet);
			xi_sel_fddestroy(writeSet);
			return 0;
		}

		// If the fd is in both the read and write set, there was an error.
		if (xi_sel_fdisset(fd, readSet) || xi_sel_fdisset(fd, writeSet)) {
			// Get the pending error.
			//			int error = 0;
			//			socklen_t errorLen = sizeof(error);
			//			if (getsockopt(fd, SOL_SOCKET, SO_ERROR, &error, &errorLen) == -1) {
			//				return -errno; // Couldn't get the real error, so report why not.
			//			}
			xi_sel_fddestroy(readSet);
			xi_sel_fddestroy(writeSet);
			return 3;
		}

		xi_sel_fddestroy(readSet);
		xi_sel_fddestroy(writeSet);
		return -EINPROGRESS;
	}
# else // Use SELECT
	int isConnected(int fd, int timeout) {
		struct timeval passedTimeout;

		passedTimeout.tv_sec = timeout / 1000;
		passedTimeout.tv_usec = (timeout % 1000) * 1000;

		// Initialize the fd sets for the select.
		fd_set readSet;
		fd_set writeSet;
		FD_ZERO(&readSet);
		FD_ZERO(&writeSet);
		FD_SET(fd, &readSet);
		FD_SET(fd, &writeSet);

		int nfds = fd + 1;
		timeval* tp = timeout >= 0 ? &passedTimeout : NULL;
		int rc = select(nfds, &readSet, &writeSet, NULL, tp);
		if (rc == -1) {
			if (errno == EINTR) {
				// We can't trivially retry a select with TEMP_FAILURE_RETRY, so punt and ask the
				// caller to try again.
				return -EINPROGRESS;
			}
			return -errno;
		}

		// If the fd is just in the write set, we're connected.
		if (FD_ISSET(fd, &writeSet) && !FD_ISSET(fd, &readSet)) {
			return 0;
		}

		// If the fd is in both the read and write set, there was an error.
		if (FD_ISSET(fd, &readSet) || FD_ISSET(fd, &writeSet)) {
			// Get the pending error.
			//			int error = 0;
			//			socklen_t errorLen = sizeof(error);
			//			if (getsockopt(fd, SOL_SOCKET, SO_ERROR, &error, &errorLen) == -1) {
			//				return -errno; // Couldn't get the real error, so report why not.
			//			}
			return -errno;
		}

		// Timeout expired.
		return -EINPROGRESS;
	}
#endif

	void didConnect(int fd) {
		if (fd != -1) {
			setBlocking(fd, true);
		}
	}

	void didFail(int fd, int result) {
		if (fd != -1) {
			setBlocking(fd, true);
		}

		//		if (result == XI_SOCK_RV_ERR_ADDR || result == XI_SOCK_RV_ERR_REFUSED
		//				|| result == XI_SOCK_RV_ERR_INUSE) {
		//			jniThrowSocketExceptionMsg(mEnv, "java/net/ConnectException",
		//					"connect failed!!", result);
		//			//jniThrowConnectException(mEnv, -result);
		//		} else if (result == XI_SOCK_RV_ERR_PERM) {
		//			jniThrowSocketExceptionMsg(mEnv, "java/lang/SecurityException",
		//					"security error!!", result);
		//			//jniThrowSecurityException(mEnv, -result);
		//		} else if (result == XI_SOCK_RV_ERR_TIMEOUT) {
		//			jniThrowSocketExceptionMsg(mEnv, "java/net/SocketTimeoutException",
		//					"time-out occurred!!", result);
		//			//jniThrowSocketTimeoutException(mEnv, -result);
		//		} else {
		//			jniThrowSocketExceptionMsg(mEnv, "java/net/SocketException",
		//					"generic socket error!!", result);
		//			//jniThrowSocketException(mEnv, -result);
		//		}

		if (result == -ECONNRESET || result == -ECONNREFUSED || result
				== -EADDRNOTAVAIL || result == -EADDRINUSE || result
				== -ENETUNREACH) {
			log_error(XDLOG, "ConnectionException!!!!");
			jniThrowSocketExceptionMsg(mEnv, "java/net/ConnectException",
					"connect failed!!", result);
			//jniThrowConnectException(mEnv, -result);
		} else if (result == -EACCES) {
			log_error(XDLOG, "SecurityException!!!!");
			jniThrowSocketExceptionMsg(mEnv, "java/lang/SecurityException",
					"security error!!", result);
			//jniThrowSecurityException(mEnv, -result);
		} else if (result == -ETIMEDOUT) {
			log_error(XDLOG, "SocketTimeoutException!!!!");
			jniThrowSocketExceptionMsg(mEnv, "java/net/SocketTimeoutException",
					"time-out occurred!!", result);
			//jniThrowSocketTimeoutException(mEnv, -result);
		} else {
			log_error(XDLOG, "SocketException!!!!");
			jniThrowSocketExceptionMsg(mEnv, "java/net/SocketException",
					"generic socket error!!", result);
			//jniThrowSocketException(mEnv, -result);
		}
	}

private:
	JNIEnv* mEnv;
};

#ifdef ENABLE_MULTICAST
static void mcastJoinLeaveGroup(JNIEnv* env, int fd, jobject javaGroupRequest,
		bool join) {
	//	group_req groupRequest;
	//
	//	// Get the IPv4 or IPv6 multicast address to join or leave.
	//	jclass multicastGroupRequestClass = env->FindClass("java/net/MulticastGroupRequest");
	//	jfieldID fid = env->GetFieldID(multicastGroupRequestClass,
	//			"gr_group", "Ljava/net/InetAddress;");
	//	jobject group = env->GetObjectField(javaGroupRequest, fid);
	//	if (!inetAddressToSocketAddress(env, group, 0, &groupRequest.gr_group)) {
	//		return;
	//	}
	//
	//	// Get the interface index to use (or 0 for "whatever").
	//	fid = env->GetFieldID(multicastGroupRequestClass, "gr_interface", "I");
	//	groupRequest.gr_interface = env->GetIntField(javaGroupRequest, fid);
	//
	//	int level = groupRequest.gr_group.ss_family == AF_INET ? IPPROTO_IP : IPPROTO_IPV6;
	//	int option = join ? MCAST_JOIN_GROUP : MCAST_LEAVE_GROUP;
	//	int rc = setsockopt(fd, level, option, &groupRequest, sizeof(groupRequest));
	//	if (rc == -1) {
	//		jniThrowSocketException(env, errno);
	//		return;
	//	}

	xi_sock_addr_t grpaddr;
	xi_sock_addr_t ifaddr;

	// Get the IPv4 or IPv6 multicast address to join or leave.
	jclass multicastGroupRequestClass = env->FindClass(
			"java/net/MulticastGroupRequest");
	jfieldID fid = env->GetFieldID(multicastGroupRequestClass, "gr_group",
			"Ljava/net/InetAddress;");
	jobject group = env->GetObjectField(javaGroupRequest, fid);
	if (!inetAddressToSocketAddress(env, group, 0, &grpaddr)) {
		return;
	}
	grpaddr.type = XI_SOCK_TYPE_DATAGRAM;
	xi_mem_copy(&ifaddr, &grpaddr, sizeof(xi_sock_addr_t));

	if (grpaddr.family == XI_SOCK_FAMILY_INET) {
		xi_strcpy(ifaddr.host, "0.0.0.0");
	} else {
		xi_strcpy(ifaddr.host, "::0.0.0.0");
	}

	int rc;
	if (join) {
		rc = xi_mcast_join(fd, ifaddr, grpaddr, NULL);
	} else {
		rc = xi_mcast_leave(fd, ifaddr, grpaddr, NULL);
	}
	if (rc != 0) {
		jniThrowSocketExceptionMsg(env, "java/net/SocketException",
				"Cannot join or leave multicast!!", rc);
		return;
	}
}
#endif // def ENABLE_MULTICAST
/*
 static bool initCachedFields(JNIEnv* env) {
 memset(&gCachedFields, 0, sizeof(gCachedFields));
 struct CachedFields* c = &gCachedFields;

 struct fieldInfo {
 jfieldID* field;
 jclass clazz;
 const char* name;
 const char* type;
 } fields[] = {
 {&c->iaddr_ipaddress, JniConstants::inetAddressClass, "ipaddress", "[B"},
 {&c->integer_class_value, JniConstants::integerClass, "value", "I"},
 {&c->boolean_class_value, JniConstants::booleanClass, "value", "Z"},
 {&c->socketimpl_port, JniConstants::socketImplClass, "port", "I"},
 {&c->socketimpl_localport, JniConstants::socketImplClass, "localport", "I"},
 {&c->socketimpl_address, JniConstants::socketImplClass, "address", "Ljava/net/InetAddress;"},
 {&c->dpack_address, JniConstants::datagramPacketClass, "address", "Ljava/net/InetAddress;"},
 {&c->dpack_port, JniConstants::datagramPacketClass, "port", "I"},
 {&c->dpack_length, JniConstants::datagramPacketClass, "length", "I"}
 };
 for (unsigned i = 0; i < sizeof(fields) / sizeof(fields[0]); i++) {
 fieldInfo f = fields[i];
 *f.field = env->GetFieldID(f.clazz, f.name, f.type);
 if (*f.field == NULL) return false;
 }
 return true;
 }
 */

//static void OSNetworkSystem_socket(JNIEnv* env, jobject, jobject fileDescriptor, jboolean stream) {
JNIEXPORT void JNICALL
Java_org_apache_harmony_luni_platform_OSNetworkSystem_socket(JNIEnv* env,
		jobject, jobject fileDescriptor, jboolean stream) {
	if (fileDescriptor == NULL) {
		jniThrowNullPointerException(env, NULL);
		//errno = EBADF;
		return;
	}

	// Try IPv6 but fall back to IPv4...
	xi_sock_type_e type = stream ? XI_SOCK_TYPE_STREAM : XI_SOCK_TYPE_DATAGRAM;
	//int fd = xi_socket_open(XI_SOCK_FAMILY_INET6, type, XI_SOCK_PROTO_IP);
	//if (fd < 0) {
	int fd = xi_socket_open(XI_SOCK_FAMILY_INET, type, XI_SOCK_PROTO_IP);
	//}
	if (fd < 0) {
		jniThrowSocketExceptionMsg(env, "java/net/SocketException",
				"cannot open socket", fd);
		return;
	} else {
		jniSetFileDescriptorOfFD(env, fileDescriptor, fd);
	}
}

/*
 static jint OSNetworkSystem_writeDirect(JNIEnv* env, jobject,
 jobject fileDescriptor, jint address, jint offset, jint count) {
 */
JNIEXPORT jint JNICALL
Java_org_apache_harmony_luni_platform_OSNetworkSystem_writeDirect(JNIEnv* env,
		jobject, jobject fileDescriptor, jint address, jint offset, jint count) {
	if (count <= 0) {
		return 0;
	}

	NetFd fd(env, fileDescriptor);
	if (fd.isClosed()) {
		return 0;
	}

	jbyte* src = reinterpret_cast<jbyte*> (static_cast<xuint32> (address
			+ offset));

	xint32 bytesSent;
	{
		int intFd = fd.get();
		AsynchronousSocketCloseMonitor monitor(intFd);
		//log_trace(XDLOG, "Send Message(fd=%d / bytes=%d)\n", intFd, xi_strlen((const xchar*)src));
		bytesSent = xi_file_write(intFd, src, count);
		//bytesSent = NET_FAILURE_RETRY(fd, xi_file_write(intFd, src, count));
	}
	if (env->ExceptionOccurred()) {
		return -1;
	}

	if (bytesSent < 0) {
		//		if (errno == EAGAIN || errno == EWOULDBLOCK) {
		//			// We were asked to write to a non-blocking socket, but were told
		//			// it would block, so report "no bytes written".
		//			return 0;
		//		} else {
		log_error(XDLOG, "Send Message Error!!! (fd=%d / msg=%s)\n", fd.get(), src);
		jniThrowSocketExceptionMsg(env, "java/net/SocketException",
				"writeDirect error!!", bytesSent);
		return 0;
		//		}
	}
	return bytesSent;
}

/*
 static jint OSNetworkSystem_write(JNIEnv* env, jobject,
 jobject fileDescriptor, jbyteArray byteArray, jint offset, jint count) {
 */
JNIEXPORT jint JNICALL
Java_org_apache_harmony_luni_platform_OSNetworkSystem_write(JNIEnv* env,
		jobject, jobject fileDescriptor, jbyteArray byteArray, jint offset,
		jint count) {
	ScopedByteArrayRW bytes(env, byteArray);
	if (bytes.get() == NULL) {
		return -1;
	}
	jint address = static_cast<jint> (reinterpret_cast<xlong> (bytes.get()));
	// by jshwang
	//    int result = OSNetworkSystem_writeDirect(env, NULL, fileDescriptor, address, offset, count);
	int result =
			Java_org_apache_harmony_luni_platform_OSNetworkSystem_writeDirect(
					env, NULL, fileDescriptor, address, offset, count);
	return result;
}

//static jboolean OSNetworkSystem_connectNonBlocking(JNIEnv* env, jobject, jobject fileDescriptor, jobject inetAddr, jint port) {
JNIEXPORT jboolean JNICALL
Java_org_apache_harmony_luni_platform_OSNetworkSystem_connectNonBlocking(
		JNIEnv* env, jobject, jobject fileDescriptor, jobject inetAddr,
		jint port) {
	NetFd fd(env, fileDescriptor);
	if (fd.isClosed()) {
		return JNI_FALSE;
	}

	//log_trace(XDLOG, "!!!!!!!!! fd=%d / port=%d\n", fd.get(), port);

	ConnectHelper context(env);
	return context.start(fd, inetAddr, port) == 0;
}

//static jboolean OSNetworkSystem_isConnected(JNIEnv* env, jobject, jobject fileDescriptor, jint timeout) {
JNIEXPORT jboolean JNICALL
Java_org_apache_harmony_luni_platform_OSNetworkSystem_isConnected(JNIEnv* env,
		jobject, jobject fileDescriptor, jint timeout) {
	NetFd fd(env, fileDescriptor);
	if (fd.isClosed()) {
		return JNI_FALSE;
	}

	ConnectHelper context(env);
	int result = context.isConnected(fd.get(), timeout);
	if (result == 0) {
		context.didConnect(fd.get());
		return JNI_TRUE;
	} else if (result == -EINPROGRESS) {
		// Not yet connected, but not yet denied either... Try again later.
		return JNI_FALSE;
	} else {
		context.didFail(fd.get(), result);
		return JNI_FALSE;
	}
}

// TODO: move this into Java, using connectNonBlocking and isConnected!
/*
 static void OSNetworkSystem_connect(JNIEnv* env, jobject, jobject fileDescriptor,
 jobject inetAddr, jint port, jint timeout) {
 */
JNIEXPORT void JNICALL
Java_org_apache_harmony_luni_platform_OSNetworkSystem_connect(JNIEnv* env,
		jobject, jobject fileDescriptor, jobject inetAddr, jint port,
		jint timeout) {

	/* if a timeout was specified calculate the finish time value */
	bool hasTimeout = timeout > 0;
	xint64 finishTime = xi_clock_msec();
	if (hasTimeout) {
		log_trace(XDLOG, "current = %lld / timeout=%d / finish=%lld\n", finishTime, timeout, (finishTime + timeout));
		finishTime += timeout;
	}

	NetFd fd(env, fileDescriptor);
	if (fd.isClosed()) {
		return;
	}

	//log_trace(XDLOG, "!!!!!!!!! fd=%d / port=%d\n", fd.get(), port);

	//	ConnectHelper context(env);
	//	xint32 ret = context.start(fd, inetAddr, port);
	//	if (ret < 0) {
	//		log_error(XDLOG, "!!!!!!!!! connection failed!!! fd=%d / port=%d\n", fd.get(), port);
	//	}

	ConnectHelper context(env);
	int result = context.start(fd, inetAddr, port);
	int remainingTimeout = timeout;
	while (result == -EINPROGRESS) {
		log_trace(XDLOG, "!!!!!!!!! connection check!!! fd=%d / port=%d / ret=%d\n", fd.get(), port, result);

		/*
		 * ok now try and connect. Depending on the platform this may sleep
		 * for up to passedTimeout milliseconds
		 */
		result = context.isConnected(fd.get(), remainingTimeout);
		// FIXME : avoid VM addRefL Error!!
		//		if (fd.isClosed()) {
		//			log_error(XDLOG, "Socket is closed!!!\n");
		//			return;
		//		}
		if (result == 0) {
			log_trace(XDLOG, "connection is done!!! fd=%d / port=%d / ret=%d\n", fd.get(), port, result);
			context.didConnect(fd.get());
			return;
		} else if (result != -EINPROGRESS) {
			log_error(XDLOG, "connection is failed!!! fd=%d / port=%d / ret=%d\n", fd.get(), port, result);
			context.didFail(fd.get(), result);
			return;
		}

		/* check if the timeout has expired */
		if (hasTimeout) {
			remainingTimeout = finishTime - xi_clock_msec();
			if (remainingTimeout <= 0) {
				log_error(XDLOG, "connection is timeout!!! fd=%d / port=%d / ret=%d\n", fd.get(), port, result);
				context.didFail(fd.get(), -ETIMEDOUT);
				return;
			}
			log_trace(XDLOG, "connection retry!!! fd=%d / port=%d / remain=%lld\n", fd.get(), port, remainingTimeout);
		} else {
			remainingTimeout = 100;
			log_trace(XDLOG, "connection retry!!! fd=%d / port=%d / remain=%lld\n", fd.get(), port, remainingTimeout);
		}
	}
}

/*
 static void OSNetworkSystem_bind(JNIEnv* env, jobject, jobject fileDescriptor,
 jobject inetAddress, jint port) {
 */
JNIEXPORT void JNICALL
Java_org_apache_harmony_luni_platform_OSNetworkSystem_bind(JNIEnv* env,
		jobject, jobject fileDescriptor, jobject inetAddress, jint port) {
	xi_sock_addr_t socketAddress;
	xi_mem_set(&socketAddress, 0, sizeof(socketAddress));
	if (!inetAddressToSocketAddress(env, inetAddress, port, &socketAddress)) {
		return;
	}

	//log_trace(XDLOG, "!!!!!!!!! bind to %s\n", socketAddress.host);

	NetFd fd(env, fileDescriptor);
	if (fd.isClosed()) {
		return;
	}

	const CompatibleSocketAddress compatibleAddress(fd.get(), socketAddress,
			false);
	//	int
	//			rc =
	//					TEMP_FAILURE_RETRY(bind(fd.get(), compatibleAddress.get(), sizeof(sockaddr_storage)));
	int rc = xi_socket_bind(fd.get(), socketAddress);
	if (rc < 0) {
		jniThrowSocketExceptionMsg(env, "java/net/BindException",
				"bind error!!", rc);
		//jniThrowBindException(env, errno);
	}
}

//static void OSNetworkSystem_listen(JNIEnv* env, jobject, jobject fileDescriptor, jint backlog) {
JNIEXPORT void JNICALL
Java_org_apache_harmony_luni_platform_OSNetworkSystem_listen(JNIEnv* env,
		jobject, jobject fileDescriptor, jint backlog) {
	NetFd fd(env, fileDescriptor);
	if (fd.isClosed()) {
		return;
	}

	int rc = xi_socket_listen(fd.get(), backlog);
	if (rc < 0) {
		jniThrowSocketExceptionMsg(env, "java/net/SocketException",
				"listen error!!", rc);
		//jniThrowSocketException(env, errno);
	}
}

/*
 static void OSNetworkSystem_accept(JNIEnv* env, jobject, jobject serverFileDescriptor,
 jobject newSocket, jobject clientFileDescriptor) {
 */
JNIEXPORT void JNICALL
Java_org_apache_harmony_luni_platform_OSNetworkSystem_accept(JNIEnv* env,
		jobject, jobject serverFileDescriptor, jobject newSocket,
		jobject clientFileDescriptor) {

	if (newSocket == NULL) {
		jniThrowNullPointerException(env, NULL);
		return;
	}

	NetFd serverFd(env, serverFileDescriptor);
	if (serverFd.isClosed()) {
		return;
	}

	//sockaddr_storage ss;
	xi_sock_addr_t ss;
	int clientFd;
	{
		int intFd = serverFd.get();
		AsynchronousSocketCloseMonitor monitor(intFd);
		clientFd = xi_socket_accept(intFd, &ss);
	}
	if (env->ExceptionOccurred()) {
		return;
	}
	if (clientFd < 0) {
		jniThrowSocketExceptionMsg(env, "java/net/SocketException",
				"accept error!!", clientFd);
		//		if (errno == EAGAIN || errno == EWOULDBLOCK) {
		//			jniThrowSocketTimeoutException(env, errno);
		//		} else {
		//			jniThrowSocketException(env, errno);
		//		}
		return;
	}

	// Reset the inherited read timeout to the Java-specified default of 0.
	//	timeval timeout(toTimeval(0));
	//	int rc = setsockopt(clientFd, SOL_SOCKET, SO_RCVTIMEO, &timeout,
	//			sizeof(timeout));
	//	if (rc == -1) {
	//		log_print(XDLOG, "couldn't reset SO_RCVTIMEO on accepted socket fd %i: %s", clientFd, strerror(errno));
	//		jniThrowSocketException(env, errno);
	//	}

	/*
	 * For network sockets, put the peer address and port in instance variables.
	 * We don't bother to do this for UNIX domain sockets, since most peers are
	 * anonymous anyway.
	 */
	if (ss.family == XI_SOCK_FAMILY_INET || ss.family == XI_SOCK_FAMILY_INET6) {
		// Remote address and port.
		jobject remoteAddress = socketAddressToInetAddress(env, &ss);
		if (remoteAddress == NULL) {
			xi_socket_close(clientFd);
			return;
		}
		int remotePort = getSocketAddressPort(&ss);
		jclass socketImplClass = env->FindClass("java/net/SocketImpl");
		jfieldID socketimpl_address = env->GetFieldID(socketImplClass,
				"address", "Ljava/net/InetAddress;");
		jfieldID socketimpl_port =
				env->GetFieldID(socketImplClass, "port", "I");

		env->SetObjectField(newSocket, socketimpl_address, remoteAddress);
		env->SetIntField(newSocket, socketimpl_port, remotePort);

		// Local port.
		xi_mem_set(&ss, 0, sizeof(xi_sock_addr_t));
		int rc = xi_socket_get_local(clientFd, &ss);
		if (rc < 0) {
			xi_socket_close(clientFd);
			jniThrowSocketExceptionMsg(env, "java/net/SocketException",
					"get local port error!!", rc);
			//jniThrowSocketException(env, errno);
			return;
		}
		int localPort = getSocketAddressPort(&ss);
		jfieldID socketimpl_localport = env->GetFieldID(socketImplClass,
				"localport", "I");

		env->SetIntField(newSocket, socketimpl_localport, localPort);
	}

	jniSetFileDescriptorOfFD(env, clientFileDescriptor, clientFd);
}

/*
 static void OSNetworkSystem_sendUrgentData(JNIEnv* env, jobject,
 jobject fileDescriptor, jbyte value) {
 */
JNIEXPORT void JNICALL
Java_org_apache_harmony_luni_platform_OSNetworkSystem_sendUrgentData(
		JNIEnv* env, jobject, jobject fileDescriptor, jbyte value) {
	NetFd fd(env, fileDescriptor);
	if (fd.isClosed()) {
		return;
	}

	int rc = xi_socket_send(fd.get(), &value, 1);
	if (rc < 0) {
		jniThrowSocketExceptionMsg(env, "java/net/SocketException",
				"send urgent data error!!", rc);
		//jniThrowSocketException(env, errno);
	}
}

//static void OSNetworkSystem_disconnectDatagram(JNIEnv* env, jobject, jobject fileDescriptor) {
JNIEXPORT void JNICALL
Java_org_apache_harmony_luni_platform_OSNetworkSystem_disconnectDatagram(
		JNIEnv* env, jobject, jobject fileDescriptor) {
	NetFd fd(env, fileDescriptor);
	if (fd.isClosed()) {
		return;
	}

	// To disconnect a datagram socket, we connect to a bogus address with
	// the family AF_UNSPEC.
	xi_sock_addr_t ss;
	xi_mem_set(&ss, 0, sizeof(ss));
	ss.family = XI_SOCK_FAMILY_UNSPEC;
	int rc = xi_socket_connect(fd.get(), ss);
	if (rc < 0) {
		jniThrowSocketExceptionMsg(env, "java/net/SocketException",
				"datagram disconnect error!!", rc);
		//jniThrowSocketException(env, errno);
	}
}

/*
 Java_org_apache_harmony_luni_platform_OSNetworkSystem_setInetAddress(JNIEnv* env, jobject,
 jobject sender, jbyteArray address) {
 */
JNIEXPORT void JNICALL
Java_org_apache_harmony_luni_platform_OSNetworkSystem_setInetAddress(
		JNIEnv* env, jobject, jobject sender, jbyteArray address) {

	jclass inetAddressClass = env->FindClass("java/net/InetAddress");
	jfieldID iaddr_ipaddress = env->GetFieldID(inetAddressClass, "ipaddress",
			"[B");
	env->SetObjectField(sender, iaddr_ipaddress, address);
}

// TODO: can we merge this with recvDirect?
/*
 static jint OSNetworkSystem_readDirect(JNIEnv* env, jobject, jobject fileDescriptor,
 jint address, jint count) {
 */
JNIEXPORT jint JNICALL
Java_org_apache_harmony_luni_platform_OSNetworkSystem_readDirect(JNIEnv* env,
		jobject, jobject fileDescriptor, jint address, jint count) {
	NetFd fd(env, fileDescriptor);
	if (fd.isClosed()) {
		return 0;
	}

	jbyte* dst = reinterpret_cast<jbyte*> (static_cast<xuint32> (address));
	xint32 bytesReceived;
	{
		int intFd = fd.get();
		AsynchronousSocketCloseMonitor monitor(intFd);
		bytesReceived = xi_file_read(intFd, dst, count);
	}
	if (env->ExceptionOccurred()) {
		return -1;
	}
	if (bytesReceived == 0) {
		return -1;
	} else if (bytesReceived < 0) {
		jniThrowSocketExceptionMsg(env, "java/net/SocketException",
				"readDirect error!!", bytesReceived);
		return 0;
		//		if (errno == EAGAIN || errno == EWOULDBLOCK) {
		//			// We were asked to read a non-blocking socket with no data
		//			// available, so report "no bytes read".
		//			return 0;
		//		} else {
		//			jniThrowSocketException(env, errno);
		//			return 0;
		//		}
	} else {
		return bytesReceived;
	}
}

/*
 static jint OSNetworkSystem_read(JNIEnv* env, jobject, jobject fileDescriptor,
 jbyteArray byteArray, jint offset, jint count) {
 */
JNIEXPORT jint JNICALL
Java_org_apache_harmony_luni_platform_OSNetworkSystem_read(JNIEnv* env,
		jobject, jobject fileDescriptor, jbyteArray byteArray, jint offset,
		jint count) {
	ScopedByteArrayRW bytes(env, byteArray);
	if (bytes.get() == NULL) {
		return -1;
	}
	jint address = static_cast<jint> (reinterpret_cast<xlong> (bytes.get()
			+ offset));
	// by jshwang
	//    return OSNetworkSystem_readDirect(env, NULL, fileDescriptor, address, count);
	return Java_org_apache_harmony_luni_platform_OSNetworkSystem_readDirect(
			env, NULL, fileDescriptor, address, count);
}

// TODO: can we merge this with readDirect?
/*
 static jint OSNetworkSystem_recvDirect(JNIEnv* env, jobject, jobject fileDescriptor, jobject packet,
 jint address, jint offset, jint length, jboolean peek, jboolean connected) {
 */
JNIEXPORT jint JNICALL
Java_org_apache_harmony_luni_platform_OSNetworkSystem_recvDirect(JNIEnv* env,
		jobject, jobject fileDescriptor, jobject packet, jint address,
		jint offset, jint length, jboolean peek, jboolean connected) {
	NetFd fd(env, fileDescriptor);
	if (fd.isClosed()) {
		return 0;
	}

	char* buf =
			reinterpret_cast<char*> (static_cast<xuint32> (address + offset));
	//const int flags = peek ? MSG_PEEK : 0;
	xi_sock_addr_t ss;
	xi_mem_set(&ss, 0, sizeof(ss));

	xint32 bytesReceived;
	{
		int intFd = fd.get();
		AsynchronousSocketCloseMonitor monitor(intFd);
		bytesReceived = xi_socket_recvfrom(intFd, buf, length, &ss);
	}
	if (env->ExceptionOccurred()) {
		return -1;
	}
	if (bytesReceived < 0) {
		jniThrowSocketExceptionMsg(env, "java/net/SocketException",
				"recvfrom error!!", bytesReceived);
		//		if (connected && errno == ECONNREFUSED) {
		//			jniThrowException(env, "java/net/PortUnreachableException", "");
		//		} else if (errno == EAGAIN || errno == EWOULDBLOCK) {
		//			jniThrowSocketTimeoutException(env, errno);
		//		} else {
		//			jniThrowSocketException(env, errno);
		//		}
		return 0;
	}

	if (packet != NULL) {
		jclass datagramPacketClass = env->FindClass("java/net/DatagramPacket");
		jfieldID dpack_length = env->GetFieldID(datagramPacketClass, "length",
				"I");
		env->SetIntField(packet, dpack_length, bytesReceived);
		if (!connected) {
			jbyteArray addr = socketAddressToByteArray(env, &ss);
			if (addr == NULL) {
				return 0;
			}
			int port = getSocketAddressPort(&ss);
			jobject sender = byteArrayToInetAddress(env, addr);
			if (sender == NULL) {
				return 0;
			}

			jfieldID dpack_address = env->GetFieldID(datagramPacketClass,
					"address", "Ljava/net/InetAddress;");
			jfieldID dpack_port = env->GetFieldID(datagramPacketClass, "port",
					"I");
			env->SetObjectField(packet, dpack_address, sender);
			env->SetIntField(packet, dpack_port, port);
		}
	}
	return bytesReceived;
}

/*
 static jint OSNetworkSystem_recv(JNIEnv* env, jobject, jobject fd, jobject packet,
 jbyteArray javaBytes, jint offset, jint length, jboolean peek, jboolean connected) {
 */
JNIEXPORT jint JNICALL
Java_org_apache_harmony_luni_platform_OSNetworkSystem_recv(JNIEnv* env,
		jobject, jobject fd, jobject packet, jbyteArray javaBytes, jint offset,
		jint length, jboolean peek, jboolean connected) {
	ScopedByteArrayRW bytes(env, javaBytes);
	if (bytes.get() == NULL) {
		return -1;
	}
	jint address = reinterpret_cast<xlong> (bytes.get());
	// by jshwang
	//    return OSNetworkSystem_recvDirect(env, NULL, fd, packet, address, offset, length, peek, connected);
	return Java_org_apache_harmony_luni_platform_OSNetworkSystem_recvDirect(
			env, NULL, fd, packet, address, offset, length, peek, connected);
}

//static jint OSNetworkSystem_sendDirect(JNIEnv* env, jobject, jobject fileDescriptor, jint address, jint offset, jint length, jint port, jobject inetAddress) {
JNIEXPORT jint JNICALL
Java_org_apache_harmony_luni_platform_OSNetworkSystem_sendDirect(JNIEnv* env,
		jobject, jobject fileDescriptor, jint address, jint offset,
		jint length, jint port, jobject inetAddress) {
	NetFd fd(env, fileDescriptor);
	if (fd.isClosed()) {
		return -1;
	}

	xi_sock_addr_t receiver;
	if (inetAddress != NULL && !inetAddressToSocketAddress(env, inetAddress,
			port, &receiver)) {
		return -1;
	}

	char* buf =
			reinterpret_cast<char*> (static_cast<xuint32> (address + offset));
	//	sockaddr* to = inetAddress ? reinterpret_cast<sockaddr*> (&receiver) : NULL;
	//	socklen_t toLength = inetAddress ? sizeof(receiver) : 0;

	xint32 bytesSent;
	{
		int intFd = fd.get();
		AsynchronousSocketCloseMonitor monitor(intFd);
		bytesSent = xi_socket_sendto(intFd, buf, length, receiver);
	}
	if (env->ExceptionOccurred()) {
		return -1;
	}
	if (bytesSent < 0) {
		jniThrowSocketExceptionMsg(env, "java/net/SocketException",
				"sendDirect error!!", bytesSent);
		//		if (errno == ECONNRESET || errno == ECONNREFUSED) {
		//			return 0;
		//		} else {
		//			jniThrowSocketException(env, errno);
		//		}
	}
	return bytesSent;
}

/*
 static jint OSNetworkSystem_send(JNIEnv* env, jobject, jobject fd,
 jbyteArray data, jint offset, jint length,
 jint port, jobject inetAddress) {
 */
JNIEXPORT jint JNICALL
Java_org_apache_harmony_luni_platform_OSNetworkSystem_send(JNIEnv* env,
		jobject, jobject fd, jbyteArray data, jint offset, jint length,
		jint port, jobject inetAddress) {
	ScopedByteArrayRO bytes(env, data);
	if (bytes.get() == NULL) {
		return -1;
	}
	/* by jshwang
	 return OSNetworkSystem_sendDirect(env, NULL, fd,
	 reinterpret_cast<uintptr_t>(bytes.get()), offset, length, port, inetAddress);
	 */
	return Java_org_apache_harmony_luni_platform_OSNetworkSystem_sendDirect(
			env, NULL, fd, reinterpret_cast<xlong> (bytes.get()), offset,
			length, port, inetAddress);
}

//
//static bool isValidFd(int fd) {
//	return fd >= 0 && fd < 64;
//}
//
//static bool initFdSet(JNIEnv* env, jobjectArray fdArray, jint count,
//		fd_set* fdSet, int* maxFd) {
//	for (int i = 0; i < count; ++i) {
//		jobject fileDescriptor = env->GetObjectArrayElement(fdArray, i);
//		if (fileDescriptor == NULL) {
//			return false;
//		}
//
//		const int fd = jniGetFDFromFileDescriptor(env, fileDescriptor);
//		if (!isValidFd(fd)) {
//			log_print(XDLOG, "selectImpl: ignoring invalid fd %i", fd);
//			continue;
//		}
//
//		FD_SET(fd, fdSet);
//
//		if (fd > *maxFd) {
//			*maxFd = fd;
//		}
//	}
//	return true;
//}
//
///*
// * Note: fdSet has to be non-const because although on Linux FD_ISSET() is sane
// * and takes a const fd_set*, it takes fd_set* on Mac OS. POSIX is not on our
// * side here:
// *   http://www.opengroup.org/onlinepubs/000095399/functions/select.html
// */
//static bool translateFdSet(JNIEnv* env, jobjectArray fdArray, jint count,
//		fd_set& fdSet, jint* flagArray, size_t offset, jint op) {
//	for (int i = 0; i < count; ++i) {
//		jobject fileDescriptor = env->GetObjectArrayElement(fdArray, i);
//		if (fileDescriptor == NULL) {
//			return false;
//		}
//
//		const int fd = jniGetFDFromFileDescriptor(env, fileDescriptor);
//		if (isValidFd(fd) && FD_ISSET(fd, &fdSet)) {
//			flagArray[i + offset] = op;
//		} else {
//			flagArray[i + offset] = SOCKET_OP_NONE;
//		}
//	}
//	return true;
//}

/*
 static jboolean OSNetworkSystem_selectImpl(JNIEnv* env, jclass,
 jobjectArray readFDArray, jobjectArray writeFDArray, jint countReadC,
 jint countWriteC, jintArray outFlags, jlong timeoutMs) {
 */
JNIEXPORT jboolean JNICALL
Java_org_apache_harmony_luni_platform_OSNetworkSystem_selectImpl(JNIEnv* env,
		jclass, jobjectArray readFDArray, jobjectArray writeFDArray,
		jint countReadC, jint countWriteC, jintArray outFlags, jlong timeoutMs) {
	xint32 i;
	xi_pollset_t *pset = xi_pollset_create(64, XI_POLLSET_OPT_EPOLL);

	// Read PollSet
	for (i = 0; i < countReadC; ++i) {
		jobject fileDescriptor = env->GetObjectArrayElement(readFDArray, i);
		if (fileDescriptor == NULL) {
			xi_pollset_destroy(pset);
			return -1;
		}

		const int fd = jniGetFDFromFileDescriptor(env, fileDescriptor);
		if (fd >= 0 && fd < 64) {
			log_error(XDLOG, "selectImpl: ignoring invalid fd %i", fd);
			continue;
		}

		xi_pollfd_t pfd;

		pfd.desc = fd;
		pfd.evts = XI_POLL_EVENT_IN;
		pfd.context = NULL;

		xi_pollset_add(pset, pfd);
	}

	// Write PollSet
	for (i = 0; i < countWriteC; ++i) {
		jobject fileDescriptor = env->GetObjectArrayElement(writeFDArray, i);
		if (fileDescriptor == NULL) {
			xi_pollset_destroy(pset);
			return -1;
		}

		const int fd = jniGetFDFromFileDescriptor(env, fileDescriptor);
		if (fd >= 0 && fd < 64) {
			log_warn(XDLOG, "selectImpl: ignoring invalid fd %i", fd);
			continue;
		}

		xi_pollfd_t pfd;

		pfd.desc = fd;
		pfd.evts = XI_POLL_EVENT_OUT;
		pfd.context = NULL;

		xi_pollset_add(pset, pfd);
	}

	// Perform the poll.
	xi_pollfd_t rfds[64];
	int result = xi_pollset_poll(pset, &rfds[0], 64, timeoutMs);
	if (result == 0) {
		// Timeout.
		return JNI_FALSE;
	} else if (result < 0) {
		jniThrowSocketExceptionMsg(env, "java/net/SocketException",
				"poll error!!", result);
		return JNI_FALSE;
		//		// Error.
		//		if (errno == EINTR) {
		//			return JNI_FALSE;
		//		} else {
		//			jniThrowSocketException(env, errno);
		//			return JNI_FALSE;
		//		}
	}

	// Translate the result into the int[] we're supposed to fill in.
	ScopedIntArrayRW flagArray(env, outFlags);
	if (flagArray.get() == NULL) {
		return JNI_FALSE;
	}

	jint *flags = flagArray.get();

	for (i = 0; i < result; i++) {
		if ((rfds[i].evts & XI_POLL_EVENT_IN) == XI_POLL_EVENT_IN) {
			for (int j = 0; j < countReadC; j++) {
				jobject tofd = env->GetObjectArrayElement(readFDArray, j);
				const int tfd = jniGetFDFromFileDescriptor(env, tofd);
				if (tfd == rfds[i].desc) {
					flags[j] = SOCKET_OP_READ;
				} else {
					flags[j] = SOCKET_OP_NONE;
				}
			}
		}
		if ((rfds[i].evts & XI_POLL_EVENT_OUT) == XI_POLL_EVENT_OUT) {
			for (int j = 0; j < countWriteC; j++) {
				jobject tofd = env->GetObjectArrayElement(writeFDArray, j);
				const int tfd = jniGetFDFromFileDescriptor(env, tofd);
				if (tfd == rfds[i].desc) {
					flags[j + countReadC] = SOCKET_OP_WRITE;
				} else {
					flags[j + countReadC] = SOCKET_OP_NONE;
				}
			}
		}
	}

	//	return translateFdSet(env, readFDArray, countReadC, readFds,
	//			flagArray.get(), 0, SOCKET_OP_READ) && translateFdSet(env,
	//			writeFDArray, countWriteC, writeFds, flagArray.get(), countReadC,
	//			SOCKET_OP_WRITE);

	return JNI_TRUE;
}

/*
 static jobject OSNetworkSystem_getSocketLocalAddress(JNIEnv* env,
 jobject, jobject fileDescriptor) {
 */
JNIEXPORT jobject JNICALL
Java_org_apache_harmony_luni_platform_OSNetworkSystem_getSocketLocalAddress(
		JNIEnv* env, jobject, jobject fileDescriptor) {
	NetFd fd(env, fileDescriptor);
	if (fd.isClosed()) {
		return NULL;
	}

	xi_sock_addr_t ss;
	int rc = xi_socket_get_local(fd.get(), &ss);
	if (rc < 0) {
		// TODO: the public API doesn't allow failure, so this whole method
		// represents a broken design. In practice, though, getsockname can't
		// fail unless we give it invalid arguments.
		log_error(XDLOG, "getsockname failed: get local address (errno=%i)", rc);
		return NULL;
	}
	return socketAddressToInetAddress(env, &ss);
}

/*
 static jint OSNetworkSystem_getSocketLocalPort(JNIEnv* env, jobject,
 jobject fileDescriptor) {
 */
JNIEXPORT jint JNICALL
Java_org_apache_harmony_luni_platform_OSNetworkSystem_getSocketLocalPort(
		JNIEnv* env, jobject, jobject fileDescriptor) {
	NetFd fd(env, fileDescriptor);
	if (fd.isClosed()) {
		return 0;
	}

	xi_sock_addr_t ss;
	int rc = xi_socket_get_local(fd.get(), &ss);
	if (rc < 0) {
		// TODO: the public API doesn't allow failure, so this whole method
		// represents a broken design. In practice, though, getsockname can't
		// fail unless we give it invalid arguments.
		log_error(XDLOG, "getsockname failed: get local address (errno=%i)", rc);
		return 0;
	}
	return getSocketAddressPort(&ss);
}

/*
 template<typename T>
 static bool getSocketOption(JNIEnv* env, const NetFd& fd, int level,
 int option, T* value) {
 socklen_t size = sizeof(*value);
 int rc = getsockopt(fd.get(), level, option, value, &size);
 if (rc < 0) {
 log_print(XDLOG, "getSocketOption(fd=%i, level=%i, option=%i) failed: (errno=%i)",
 fd.get(), level, option, rc);
 jniThrowSocketExceptionMsg(env, "java/net/SocketException",
 "get socket option error!!", rc);
 //jniThrowSocketException(env, errno);
 return false;
 }
 return true;
 }

 static jobject getSocketOption_Boolean(JNIEnv* env, const NetFd& fd, int level,
 int option) {
 int value;
 return getSocketOption(env, fd, level, option, &value) ? booleanValueOf(
 env, value) : NULL;
 }

 static jobject getSocketOption_Integer(JNIEnv* env, const NetFd& fd, int level,
 int option) {
 int value;
 return getSocketOption(env, fd, level, option, &value) ? integerValueOf(
 env, value) : NULL;
 }
 */

//static jobject OSNetworkSystem_getSocketOption(JNIEnv* env, jobject, jobject fileDescriptor, jint option) {
JNIEXPORT jobject JNICALL
Java_org_apache_harmony_luni_platform_OSNetworkSystem_getSocketOption(
		JNIEnv* env, jobject, jobject fileDescriptor, jint option) {
	NetFd fd(env, fileDescriptor);
	if (fd.isClosed()) {
		return NULL;
	}

	int family = getSocketAddressFamily(fd.get());
	if (family != XI_SOCK_FAMILY_INET && family != XI_SOCK_FAMILY_INET6) {
		jniThrowSocketExceptionMsg(env, "java/net/SocketException",
				"get socket option not support!!", 0);
		//jniThrowSocketException(env, EAFNOSUPPORT);
		return NULL;
	}

	xint32 val;
	switch (option) {
	case JAVASOCKOPT_TCP_NODELAY:
		//		return getSocketOption_Boolean(env, fd, IPPROTO_TCP, TCP_NODELAY);
		//log_trace(XDLOG, "!!!!!!!!!!!!!!!!!!!!!!!!!! JAVASOCKOPT_TCP_NODELAY\n");
		return booleanValueOf(env, JNI_FALSE);
	case JAVASOCKOPT_SO_SNDBUF:
		xi_socket_opt_get(fd.get(), XI_SOCK_OPT_SENDBUF, &val);
		//log_trace(XDLOG, "!!!!!!!!!!!!!!!!!!!!!!!!!! JAVASOCKOPT_SO_SNDBUF\n");
		return integerValueOf(env, val);
		//return getSocketOption_Integer(env, fd, SOL_SOCKET, SO_SNDBUF);
	case JAVASOCKOPT_SO_RCVBUF:
		xi_socket_opt_get(fd.get(), XI_SOCK_OPT_RECVBUF, &val);
		//log_trace(XDLOG, "!!!!!!!!!!!!!!!!!!!!!!!!!! JAVASOCKOPT_SO_RCVBUF\n");
		return integerValueOf(env, val);
		//return getSocketOption_Integer(env, fd, SOL_SOCKET, SO_RCVBUF);
	case JAVASOCKOPT_SO_BROADCAST:
		//		return getSocketOption_Boolean(env, fd, SOL_SOCKET, SO_BROADCAST);
		//log_trace(XDLOG, "!!!!!!!!!!!!!!!!!!!!!!!!!! JAVASOCKOPT_SO_BROADCAST\n");
		return booleanValueOf(env, JNI_FALSE);
	case JAVASOCKOPT_SO_REUSEADDR:
		xi_socket_opt_get(fd.get(), XI_SOCK_OPT_REUSEADDR, &val);
		//log_trace(XDLOG, "!!!!!!!!!!!!!!!!!!!!!!!!!! JAVASOCKOPT_SO_REUSEADDR\n");
		return booleanValueOf(env, val);
		//return getSocketOption_Boolean(env, fd, SOL_SOCKET, SO_REUSEADDR);
	case JAVASOCKOPT_SO_KEEPALIVE:
		xi_socket_opt_get(fd.get(), XI_SOCK_OPT_KEEPALIVE, &val);
		//log_trace(XDLOG, "!!!!!!!!!!!!!!!!!!!!!!!!!! JAVASOCKOPT_SO_KEEPALIVE\n");
		return booleanValueOf(env, val);
		//return getSocketOption_Boolean(env, fd, SOL_SOCKET, SO_KEEPALIVE);
	case JAVASOCKOPT_SO_OOBINLINE:
		//		return getSocketOption_Boolean(env, fd, SOL_SOCKET, SO_OOBINLINE);
		//log_trace(XDLOG, "!!!!!!!!!!!!!!!!!!!!!!!!!! JAVASOCKOPT_SO_OOBINLINE\n");
		return booleanValueOf(env, JNI_FALSE);
	case JAVASOCKOPT_IP_TOS:
		//		if (family == AF_INET) {
		//			return getSocketOption_Integer(env, fd, IPPROTO_IP, IP_TOS);
		//		} else {
		//			return getSocketOption_Integer(env, fd, IPPROTO_IPV6, IPV6_TCLASS);
		//		}
		//log_trace(XDLOG, "!!!!!!!!!!!!!!!!!!!!!!!!!! JAVASOCKOPT_IP_TOS\n");
		return 0;
	case JAVASOCKOPT_SO_LINGER:
		xi_socket_opt_get(fd.get(), XI_SOCK_OPT_LINGER, &val);
		//log_trace(XDLOG, "!!!!!!!!!!!!!!!!!!!!!!!!!! JAVASOCKOPT_SO_LINGER\n");
		return integerValueOf(env, val);
		//		{
		//		linger lingr;
		//		bool ok = getSocketOption(env, fd, SOL_SOCKET, SO_LINGER, &lingr);
		//		if (!ok) {
		//			return NULL; // We already threw.
		//		} else if (!lingr.l_onoff) {
		//			return booleanValueOf(env, false);
		//		} else {
		//			return integerValueOf(env, lingr.l_linger);
		//		}
		//	}
	case JAVASOCKOPT_SO_TIMEOUT: {
		//		timeval timeout;
		//		bool ok = getSocketOption(env, fd, SOL_SOCKET, SO_RCVTIMEO, &timeout);
		//		return ok ? integerValueOf(env, toMs(timeout)) : NULL;
		//log_trace(XDLOG, "!!!!!!!!!!!!!!!!!!!!!!!!!! JAVASOCKOPT_SO_TIMEOUT\n");
		return integerValueOf(env, 0);
	}
#ifdef ENABLE_MULTICAST
	case JAVASOCKOPT_IP_MULTICAST_IF: {
		// Although setsockopt(2) can take an ip_mreqn for IP_MULTICAST_IF, getsockopt(2)
		// always returns an in_addr.
		//		sockaddr_storage ss;
		//		memset(&ss, 0, sizeof(ss));
		//		ss.ss_family = AF_INET; // This call is IPv4-only.
		//		sockaddr_in* sa = reinterpret_cast<sockaddr_in*> (&ss);
		//		if (!getSocketOption(env, fd, IPPROTO_IP, IP_MULTICAST_IF,
		//				&sa->sin_addr)) {
		//			return NULL;
		//		}
		xi_sock_addr_t ss;
		ss.family = XI_SOCK_FAMILY_INET;
		xi_strcpy(ss.host, "0.0.0.0");

		return socketAddressToInetAddress(env, &ss);
	}
	case JAVASOCKOPT_IP_MULTICAST_IF2:
		if (family == XI_SOCK_FAMILY_INET) {
			// The caller's asking for an interface index, but that's not how IPv4 works.
			// Our Java should never get here, because we'll try IP_MULTICAST_IF first and
			// that will satisfy us.
			jniThrowSocketExceptionMsg(env, "java/net/SocketException",
					"Not Supported!!!", -1);
			//jniThrowSocketException(env, EAFNOSUPPORT);
		} else {
			//			return getSocketOption_Integer(env, fd, IPPROTO_IPV6,
			//					IPV6_MULTICAST_IF);
			return integerValueOf(env, 0);
		}
	case JAVASOCKOPT_IP_MULTICAST_LOOP:
		if (family == XI_SOCK_FAMILY_INET) {
			// Although IPv6 was cleaned up to use int, IPv4 multicast loopback uses a byte.
			//			u_char loopback;
			//			bool ok = getSocketOption(env, fd, IPPROTO_IP, IP_MULTICAST_LOOP,
			//					&loopback);
			//			return ok ? booleanValueOf(env, loopback) : NULL;
			return booleanValueOf(env, JNI_FALSE);
		} else {
			//			return getSocketOption_Boolean(env, fd, IPPROTO_IPV6,
			//					IPV6_MULTICAST_LOOP);
			return booleanValueOf(env, JNI_FALSE);
		}
	case JAVASOCKOPT_MULTICAST_TTL:
		if (family == XI_SOCK_FAMILY_INET) {
			// Although IPv6 was cleaned up to use int, and IPv4 non-multicast TTL uses int,
			// IPv4 multicast TTL uses a byte.
			//			u_char ttl;
			//			bool ok = getSocketOption(env, fd, IPPROTO_IP, IP_MULTICAST_TTL,
			//					&ttl);
			//			return ok ? integerValueOf(env, ttl) : NULL;
			return integerValueOf(env, 64);
		} else {
			//			return getSocketOption_Integer(env, fd, IPPROTO_IPV6,
			//					IPV6_MULTICAST_HOPS);
			return integerValueOf(env, 64);
		}
#else
		case JAVASOCKOPT_MULTICAST_TTL:
		case JAVASOCKOPT_IP_MULTICAST_IF:
		case JAVASOCKOPT_IP_MULTICAST_IF2:
		case JAVASOCKOPT_IP_MULTICAST_LOOP:
		jniThrowException(env, "java/lang/UnsupportedOperationException", NULL);
		return NULL;
#endif // def ENABLE_MULTICAST
	default:
		jniThrowSocketExceptionMsg(env, "java/net/SocketException",
				"get socket option!!", 0);
		//jniThrowSocketException(env, ENOPROTOOPT);
		return NULL;
	}
}

/*
 template<typename T>
 static void setSocketOption(JNIEnv* env, const NetFd& fd, int level,
 int option, T* value) {
 int rc = setsockopt(fd.get(), level, option, value, sizeof(*value));
 if (rc < 0) {
 log_print(XDLOG, "setSocketOption(fd=%i, level=%i, option=%i) failed: (errno=%i)",
 fd.get(), level, option, rc);
 jniThrowSocketExceptionMsg(env, "java/net/SocketException",
 "get socket option error!!", rc);
 //jniThrowSocketException(env, errno);
 }
 }
 */

//static void OSNetworkSystem_setSocketOption(JNIEnv* env, jobject, jobject fileDescriptor, jint option, jobject optVal) {
JNIEXPORT void JNICALL
Java_org_apache_harmony_luni_platform_OSNetworkSystem_setSocketOption(
		JNIEnv* env, jobject, jobject fileDescriptor, jint option,
		jobject optVal) {
	NetFd fd(env, fileDescriptor);
	if (fd.isClosed()) {
		return;
	}

	int intVal = 0;
	bool wasBoolean = false;

	jclass integerClass = env->FindClass("java/lang/Integer");
	jclass booleanClass = env->FindClass("java/lang/Boolean");
	jclass inetAddressClass = env->FindClass("java/net/InetAddress");
	jclass multicastGroupRequestClass = env->FindClass(
			"java/net/MulticastGroupRequest");

	if (env->IsInstanceOf(optVal, integerClass)) {
		jfieldID integer_class_value = env->GetFieldID(integerClass, "value",
				"I");
		intVal = (int) env->GetIntField(optVal, integer_class_value);
	} else if (env->IsInstanceOf(optVal, booleanClass)) {
		jfieldID boolean_class_value = env->GetFieldID(booleanClass, "value",
				"Z");
		intVal = (int) env->GetBooleanField(optVal, boolean_class_value);
		wasBoolean = true;
	} else if (env->IsInstanceOf(optVal, inetAddressClass)) {
		// We use optVal directly as an InetAddress for IP_MULTICAST_IF.
	} else if (env->IsInstanceOf(optVal, multicastGroupRequestClass)) {
		// We use optVal directly as a MulticastGroupRequest for MCAST_JOIN_GROUP/MCAST_LEAVE_GROUP.
	} else {
		jniThrowSocketExceptionMsg(env, "java/net/SocketException",
				"cannot get the java member field!!", 0);
		return;
	}

	int family = getSocketAddressFamily(fd.get());
	if (family != XI_SOCK_FAMILY_INET && family != XI_SOCK_FAMILY_INET6) {
		jniThrowSocketExceptionMsg(env, "java/net/SocketException",
				"set socket option not support!!", 0);
		//jniThrowSocketException(env, EAFNOSUPPORT);
		return;
	}

	// Since we expect to have a AF_INET6 socket even if we're communicating via IPv4, we always
	// set the IPPROTO_IP options. As long as we fall back to creating IPv4 sockets if creating
	// an IPv6 socket fails, we need to make setting the IPPROTO_IPV6 options conditional.
	switch (option) {
	case JAVASOCKOPT_IP_TOS:
		//		setSocketOption(env, fd, IPPROTO_IP, IP_TOS, &intVal);
		//		if (family == AF_INET6) {
		//			setSocketOption(env, fd, IPPROTO_IPV6, IPV6_TCLASS, &intVal);
		//		}
		//log_trace(XDLOG, "!!!!!!!!!!!!!!!!!!!!!!!!!! JAVASOCKOPT_IP_TOS\n");
		return;
	case JAVASOCKOPT_SO_BROADCAST:
		//setSocketOption(env, fd, SOL_SOCKET, SO_BROADCAST, &intVal);
		//log_trace(XDLOG, "!!!!!!!!!!!!!!!!!!!!!!!!!! JAVASOCKOPT_SO_BROADCAST\n");
		return;
	case JAVASOCKOPT_SO_KEEPALIVE:
		xi_socket_opt_set(fd.get(), XI_SOCK_OPT_KEEPALIVE, intVal);
		//setSocketOption(env, fd, SOL_SOCKET, SO_KEEPALIVE, &intVal);
		//log_trace(XDLOG, "!!!!!!!!!!!!!!!!!!!!!!!!!! JAVASOCKOPT_SO_KEEPALIVE\n");
		return;
	case JAVASOCKOPT_SO_LINGER:
		xi_socket_opt_set(fd.get(), XI_SOCK_OPT_LINGER, intVal);
		//log_trace(XDLOG, "!!!!!!!!!!!!!!!!!!!!!!!!!! JAVASOCKOPT_SO_LINGER\n");
		return;
		//	{
		//		linger l;
		//		l.l_onoff = !wasBoolean;
		//		l.l_linger = intVal <= 65535 ? intVal : 65535;
		//		setSocketOption(env, fd, SOL_SOCKET, SO_LINGER, &l);
		//		return;
		//	}
	case JAVASOCKOPT_SO_OOBINLINE:
		//		setSocketOption(env, fd, SOL_SOCKET, SO_OOBINLINE, &intVal);
		//log_trace(XDLOG, "!!!!!!!!!!!!!!!!!!!!!!!!!! JAVASOCKOPT_SO_OOBINLINE\n");
		return;
	case JAVASOCKOPT_SO_RCVBUF:
		xi_socket_opt_set(fd.get(), XI_SOCK_OPT_RECVBUF, intVal);
		//setSocketOption(env, fd, SOL_SOCKET, SO_RCVBUF, &intVal);
		//log_trace(XDLOG, "!!!!!!!!!!!!!!!!!!!!!!!!!! JAVASOCKOPT_SO_RCVBUF\n");
		return;
	case JAVASOCKOPT_SO_REUSEADDR:
		xi_socket_opt_set(fd.get(), XI_SOCK_OPT_REUSEADDR, intVal);
		//setSocketOption(env, fd, SOL_SOCKET, SO_REUSEADDR, &intVal);
		//log_trace(XDLOG, "!!!!!!!!!!!!!!!!!!!!!!!!!! JAVASOCKOPT_SO_REUSEADDR\n");
		return;
	case JAVASOCKOPT_SO_SNDBUF:
		xi_socket_opt_set(fd.get(), XI_SOCK_OPT_SENDBUF, intVal);
		//setSocketOption(env, fd, SOL_SOCKET, SO_SNDBUF, &intVal);
		//log_trace(XDLOG, "!!!!!!!!!!!!!!!!!!!!!!!!!! JAVASOCKOPT_SO_SNDBUF\n");
		return;
	case JAVASOCKOPT_SO_TIMEOUT: {
		xi_socket_opt_set(fd.get(), XI_SOCK_OPT_SNDTIMEO, intVal);
		xi_socket_opt_set(fd.get(), XI_SOCK_OPT_RCVTIMEO, intVal);
		//		timeval timeout(toTimeval(intVal));
		//		setSocketOption(env, fd, SOL_SOCKET, SO_RCVTIMEO, &timeout);
		//log_trace(XDLOG, "!!!!!!!!!!!!!!!!!!!!!!!!!! JAVASOCKOPT_SO_TIMEOUT\n");
		return;
	}
	case JAVASOCKOPT_TCP_NODELAY:
		//		setSocketOption(env, fd, IPPROTO_TCP, TCP_NODELAY, &intVal);
		//log_trace(XDLOG, "!!!!!!!!!!!!!!!!!!!!!!!!!! JAVASOCKOPT_TCP_NODELAY\n");
		return;
#ifdef ENABLE_MULTICAST
	case JAVASOCKOPT_MCAST_JOIN_GROUP:
		mcastJoinLeaveGroup(env, fd.get(), optVal, true);
		return;
	case JAVASOCKOPT_MCAST_LEAVE_GROUP:
		mcastJoinLeaveGroup(env, fd.get(), optVal, false);
		return;
	case JAVASOCKOPT_IP_MULTICAST_IF: {
		xi_sock_addr_t sockVal;
		jclass inetAddressClass = env->FindClass("java/net/InetAddress");
		if (!env->IsInstanceOf(optVal, inetAddressClass)
				|| !inetAddressToSocketAddress(env, optVal, 0, &sockVal)) {
			return;
		}
		// This call is IPv4 only. The socket may be IPv6, but the address
		// that identifies the interface to join must be an IPv4 address.
		if (sockVal.family != XI_SOCK_FAMILY_INET) {
			jniThrowSocketExceptionMsg(env, "java/net/SocketException",
					"Not Supported!!", 0);
			//jniThrowSocketException(env, EAFNOSUPPORT);
			return;
		}
		//		ip_mreqn mcast_req;
		//		memset(&mcast_req, 0, sizeof(mcast_req));
		//		mcast_req.imr_address
		//				= reinterpret_cast<sockaddr_in*> (&sockVal)->sin_addr;
		//		setSocketOption(env, fd, IPPROTO_IP, IP_MULTICAST_IF, &mcast_req);
		return;
	}
	case JAVASOCKOPT_IP_MULTICAST_IF2:
		// TODO: is this right? should we unconditionally set the IPPROTO_IP state in case
		// we have an IPv6 socket communicating via IPv4?
		if (family == XI_SOCK_FAMILY_INET) {
			// IP_MULTICAST_IF expects a pointer to an ip_mreqn struct.
			//			ip_mreqn multicastRequest;
			//			memset(&multicastRequest, 0, sizeof(multicastRequest));
			//			multicastRequest.imr_ifindex = intVal;
			//			setSocketOption(env, fd, IPPROTO_IP, IP_MULTICAST_IF,
			//					&multicastRequest);
		} else {
			// IPV6_MULTICAST_IF expects a pointer to an integer.
			//			setSocketOption(env, fd, IPPROTO_IPV6, IPV6_MULTICAST_IF, &intVal);
		}
		return;
	case JAVASOCKOPT_MULTICAST_TTL: {
		// Although IPv6 was cleaned up to use int, and IPv4 non-multicast TTL uses int,
		// IPv4 multicast TTL uses a byte.
		//		u_char ttl = intVal;
		//		setSocketOption(env, fd, IPPROTO_IP, IP_MULTICAST_TTL, &ttl);
		//		if (family == AF_INET6) {
		//			setSocketOption(env, fd, IPPROTO_IPV6, IPV6_MULTICAST_HOPS, &intVal);
		//		}
		return;
	}
	case JAVASOCKOPT_IP_MULTICAST_LOOP: {
		// Although IPv6 was cleaned up to use int, IPv4 multicast loopback uses a byte.
		//		u_char loopback = intVal;
		//		setSocketOption(env, fd, IPPROTO_IP, IP_MULTICAST_LOOP, &loopback);
		//		if (family == AF_INET6) {
		//			setSocketOption(env, fd, IPPROTO_IPV6, IPV6_MULTICAST_LOOP, &intVal);
		//		}
		return;
	}
#else
		case JAVASOCKOPT_MULTICAST_TTL:
		case JAVASOCKOPT_MCAST_JOIN_GROUP:
		case JAVASOCKOPT_MCAST_LEAVE_GROUP:
		case JAVASOCKOPT_IP_MULTICAST_IF:
		case JAVASOCKOPT_IP_MULTICAST_IF2:
		case JAVASOCKOPT_IP_MULTICAST_LOOP:
		jniThrowException(env, "java/lang/UnsupportedOperationException", NULL);
		return;
#endif // def ENABLE_MULTICAST
	default:
		jniThrowSocketExceptionMsg(env, "java/net/SocketException",
				"set socket option!!", 0);
	}
}

static void doShutdown(JNIEnv* env, jobject fileDescriptor,
		xi_sock_shutdown_e how) {
	NetFd fd(env, fileDescriptor);
	if (fd.isClosed()) {
		return;
	}
	int rc = xi_socket_shutdown(fd.get(), how);
	if (rc < 0) {
		jniThrowSocketExceptionMsg(env, "java/net/SocketException",
				"shutdown error!!", rc);
		//jniThrowSocketException(env, errno);
	}
}

//static void OSNetworkSystem_shutdownInput(JNIEnv* env, jobject, jobject fd) {
JNIEXPORT
void JNICALL
Java_org_apache_harmony_luni_platform_OSNetworkSystem_shutdownInput(
		JNIEnv* env, jobject, jobject fd) {
	doShutdown(env, fd, XI_SOCK_SHUTDOWN_RD);
}

//static void OSNetworkSystem_shutdownOutput(JNIEnv* env, jobject, jobject fd) {
JNIEXPORT
void JNICALL
Java_org_apache_harmony_luni_platform_OSNetworkSystem_shutdownOutput(
		JNIEnv* env, jobject, jobject fd) {
	doShutdown(env, fd, XI_SOCK_SHUTDOWN_WR);
}

//static void OSNetworkSystem_close(JNIEnv* env, jobject, jobject fileDescriptor) {
JNIEXPORT
void JNICALL
Java_org_apache_harmony_luni_platform_OSNetworkSystem_close(JNIEnv* env,
		jobject, jobject fileDescriptor) {
	NetFd fd(env, fileDescriptor);
	if (fd.isClosed()) {
		return;
	}

	int oldFd = fd.get();
	jniSetFileDescriptorOfFD(env, fileDescriptor, -1);
	AsynchronousSocketCloseMonitor::signalBlockedThreads(oldFd);
	xi_socket_close(oldFd);
}

/* by jshwang
 static JNINativeMethod gMethods[] = {
 NATIVE_METHOD(OSNetworkSystem, accept, "(Ljava/io/FileDescriptor;Ljava/net/SocketImpl;Ljava/io/FileDescriptor;)V"),
 NATIVE_METHOD(OSNetworkSystem, bind, "(Ljava/io/FileDescriptor;Ljava/net/InetAddress;I)V"),
 NATIVE_METHOD(OSNetworkSystem, close, "(Ljava/io/FileDescriptor;)V"),
 NATIVE_METHOD(OSNetworkSystem, connectNonBlocking, "(Ljava/io/FileDescriptor;Ljava/net/InetAddress;I)Z"),
 NATIVE_METHOD(OSNetworkSystem, connect, "(Ljava/io/FileDescriptor;Ljava/net/InetAddress;II)V"),
 NATIVE_METHOD(OSNetworkSystem, disconnectDatagram, "(Ljava/io/FileDescriptor;)V"),
 NATIVE_METHOD(OSNetworkSystem, getSocketLocalAddress, "(Ljava/io/FileDescriptor;)Ljava/net/InetAddress;"),
 NATIVE_METHOD(OSNetworkSystem, getSocketLocalPort, "(Ljava/io/FileDescriptor;)I"),
 NATIVE_METHOD(OSNetworkSystem, getSocketOption, "(Ljava/io/FileDescriptor;I)Ljava/lang/Object;"),
 NATIVE_METHOD(OSNetworkSystem, isConnected, "(Ljava/io/FileDescriptor;I)Z"),
 NATIVE_METHOD(OSNetworkSystem, listen, "(Ljava/io/FileDescriptor;I)V"),
 NATIVE_METHOD(OSNetworkSystem, read, "(Ljava/io/FileDescriptor;[BII)I"),
 NATIVE_METHOD(OSNetworkSystem, readDirect, "(Ljava/io/FileDescriptor;II)I"),
 NATIVE_METHOD(OSNetworkSystem, recv, "(Ljava/io/FileDescriptor;Ljava/net/DatagramPacket;[BIIZZ)I"),
 NATIVE_METHOD(OSNetworkSystem, recvDirect, "(Ljava/io/FileDescriptor;Ljava/net/DatagramPacket;IIIZZ)I"),
 NATIVE_METHOD(OSNetworkSystem, selectImpl, "([Ljava/io/FileDescriptor;[Ljava/io/FileDescriptor;II[IJ)Z"),
 NATIVE_METHOD(OSNetworkSystem, send, "(Ljava/io/FileDescriptor;[BIIILjava/net/InetAddress;)I"),
 NATIVE_METHOD(OSNetworkSystem, sendDirect, "(Ljava/io/FileDescriptor;IIIILjava/net/InetAddress;)I"),
 NATIVE_METHOD(OSNetworkSystem, sendUrgentData, "(Ljava/io/FileDescriptor;B)V"),
 NATIVE_METHOD(OSNetworkSystem, setInetAddress, "(Ljava/net/InetAddress;[B)V"),
 NATIVE_METHOD(OSNetworkSystem, setSocketOption, "(Ljava/io/FileDescriptor;ILjava/lang/Object;)V"),
 NATIVE_METHOD(OSNetworkSystem, shutdownInput, "(Ljava/io/FileDescriptor;)V"),
 NATIVE_METHOD(OSNetworkSystem, shutdownOutput, "(Ljava/io/FileDescriptor;)V"),
 NATIVE_METHOD(OSNetworkSystem, socket, "(Ljava/io/FileDescriptor;Z)V"),
 NATIVE_METHOD(OSNetworkSystem, write, "(Ljava/io/FileDescriptor;[BII)I"),
 NATIVE_METHOD(OSNetworkSystem, writeDirect, "(Ljava/io/FileDescriptor;III)I"),
 };

 int register_org_apache_harmony_luni_platform_OSNetworkSystem(JNIEnv* env) {
 AsynchronousSocketCloseMonitor::init();
 return initCachedFields(env) && jniRegisterNativeMethods(env,
 "org/apache/harmony/luni/platform/OSNetworkSystem", gMethods, NELEM(gMethods));
 }
 */
int register_org_apache_harmony_luni_platform_OSNetworkSystem(JNIEnv* env) {
	AsynchronousSocketCloseMonitor::init();
	return JNI_OK;
	//    return initCachedFields(env);
}

