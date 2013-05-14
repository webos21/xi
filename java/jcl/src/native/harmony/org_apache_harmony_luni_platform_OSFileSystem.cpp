/*
 *  Licensed to the Apache Software Foundation (ASF) under one or more
 *  contributor license agreements.  See the NOTICE file distributed with
 *  this work for additional information regarding copyright ownership.
 *  The ASF licenses this file to You under the Apache License, Version 2.0
 *  (the "License"); you may not use this file except in compliance with
 *  the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#define LOG_TAG "OSFileSystem"

/* Values for HyFileOpen */
#define HyOpenRead    1
#define HyOpenWrite   2
#define HyOpenCreate  4
#define HyOpenTruncate  8
#define HyOpenAppend  16
/* Use this flag with HyOpenCreate, if this flag is specified then
 * trying to create an existing file will fail
 */
#define HyOpenCreateNew 64
#define HyOpenSync      128
#define SHARED_LOCK_TYPE 1L

#include "org_apache_harmony_luni_platform_OSFileSystem.h"
#include "JNIHelp.h"
#include "JniConstants.h"
#include "LocalArray.h"
#include "ScopedPrimitiveArray.h"
#include "ScopedUtfChars.h"
#include "UniquePtr.h"

// by jshwang
//#include "stdint.h"
// by cmjo
//#define LOGW printf
//#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
//#include <stdio.h>
#include <stdlib.h>
//#include <string.h>
// by cmjo
//#include <sys/ioctl.h>
//#include <sys/stat.h>
//#include <sys/types.h>
// by cmjo
//#include <sys/uio.h>
//#include <unistd.h>

#include "xi/xi_log.h"
#include "xi/xi_file.h"
#include "xi/xi_socket.h"
#include "xi/xi_string.h"
#include "xi/xi_sysinfo.h"

#if 0 // by cmjo
#if HAVE_SYS_SENDFILE_H
#include <sys/sendfile.h>
#else
/*
 * Define a small adapter function: sendfile() isn't part of a standard,
 * and its definition differs between Linux, BSD, and OS X. This version
 * works for OS X but will probably not work on other BSDish systems.
 * Note: We rely on function overloading here to define a same-named
 * function with different arguments.
 */
//#include <sys/socket.h>
#include <sys/types.h>
static inline ssize_t sendfilex(int out_fd, int in_fd, off_t* offset, size_t count) {
	off_t len = count;
	int result = sendfile(in_fd, out_fd, *offset, &len, NULL, 0);
	if (result < 0) {
		return -1;
	}
	return len;
}
#endif
static int EsTranslateOpenFlags(int flags) {
	int realFlags = 0;

	if (flags & HyOpenAppend) {
		realFlags |= O_APPEND;
	}
	if (flags & HyOpenTruncate) {
		realFlags |= O_TRUNC;
	}
	if (flags & HyOpenCreate) {
		realFlags |= O_CREAT;
	}
	if (flags & HyOpenCreateNew) {
		realFlags |= O_EXCL | O_CREAT;
	}
#ifdef O_SYNC
	if (flags & HyOpenSync) {
		realFlags |= O_SYNC;
	}
#endif
	if (flags & HyOpenRead) {
		if (flags & HyOpenWrite) {
			return (O_RDWR | realFlags);
		}
		return (O_RDONLY | realFlags);
	}
	if (flags & HyOpenWrite) {
		return (O_WRONLY | realFlags);
	}
	return -1;
}
#endif // 0 - sendfile & EsTranslateOpenFlags
/*
 static jint OSFileSystem_lockImpl(JNIEnv* env, jobject, jint fd,
 jlong start, jlong length, jint typeFlag, jboolean waitFlag) {
 */
JNIEXPORT jint JNICALL
Java_org_apache_harmony_luni_platform_OSFileSystem_lockImpl(JNIEnv*,
		jobject, jint fd, jlong start, jlong length, jint typeFlag,
		jboolean waitFlag) {
	xint32 bexcl = ((typeFlag & SHARED_LOCK_TYPE) == SHARED_LOCK_TYPE) ? 0 : 1;
	xint32 bwait = (waitFlag) ? 1 : 0;
	xint32 ret = 0;

	ret = xi_file_lock(fd, start, length, bexcl, bwait);
	if (ret < 0) {
		return -1;
	} else {
		return 0;
	}
}

//static void OSFileSystem_unlockImpl(JNIEnv* env, jobject, jint fd, jlong start, jlong length) {
JNIEXPORT void JNICALL
Java_org_apache_harmony_luni_platform_OSFileSystem_unlockImpl(JNIEnv* env,
		jobject, jint fd, jlong start, jlong length) {
	xint32 ret = 0;

	ret = xi_file_unlock(fd, start, length);
	if (ret < 0) {
		jniThrowIOExceptionMsg(env, "file unlock error!!", ret);
	}
}

/**
 * Returns the granularity of the starting address for virtual memory allocation.
 * (It's the same as the page size.)
 */
//static jint OSFileSystem_getAllocGranularity(JNIEnv*, jobject) {
JNIEXPORT jint JNICALL
Java_org_apache_harmony_luni_platform_OSFileSystem_getAllocGranularity(JNIEnv*,
		jobject) {
	return xi_sysinfo_pagesize();
}

// Translate three Java int[]s to a native iovec[] for readv and writev.
static xi_file_iovec_t* initIoVec(JNIEnv* env, jintArray jBuffers,
		jintArray jOffsets, jintArray jLengths, jint size) {
	UniquePtr<xi_file_iovec_t[]> vectors(new xi_file_iovec_t[size]);
	if (vectors.get() == NULL) {
		jniThrowException(env, "java/lang/OutOfMemoryError", "native heap");
		return NULL;
	}
	ScopedIntArrayRO buffers(env, jBuffers);
	if (buffers.get() == NULL) {
		return NULL;
	}
	ScopedIntArrayRO offsets(env, jOffsets);
	if (offsets.get() == NULL) {
		return NULL;
	}
	ScopedIntArrayRO lengths(env, jLengths);
	if (lengths.get() == NULL) {
		return NULL;
	}
	for (int i = 0; i < size; ++i) {
		vectors[i].iov_base = reinterpret_cast<void*> (buffers[i] + offsets[i]);
		vectors[i].iov_len = lengths[i];
	}
	return vectors.release();
}

/*
 static jlong OSFileSystem_readv(JNIEnv* env, jobject, jint fd,
 jintArray jBuffers, jintArray jOffsets, jintArray jLengths, jint size) {
 */
JNIEXPORT jlong JNICALL
Java_org_apache_harmony_luni_platform_OSFileSystem_readv(JNIEnv* env, jobject,
		jint fd, jintArray jBuffers, jintArray jOffsets, jintArray jLengths,
		jint size) {
	UniquePtr<xi_file_iovec_t[]> vectors(
			initIoVec(env, jBuffers, jOffsets, jLengths, size));
	if (vectors.get() == NULL) {
		return -1;
	}
	long result = xi_file_readv(fd, vectors.get(), size);
	if (result == 0) {
		return -1;
	}
	if (result == -1) {
		jniThrowIOExceptionMsg(env, "readv error!!", result);
	}
	return result;
}

/*
 static jlong OSFileSystem_writev(JNIEnv* env, jobject, jint fd,
 jintArray jBuffers, jintArray jOffsets, jintArray jLengths, jint size) {
 */
JNIEXPORT jlong JNICALL
Java_org_apache_harmony_luni_platform_OSFileSystem_writev(JNIEnv* env, jobject,
		jint fd, jintArray jBuffers, jintArray jOffsets, jintArray jLengths,
		jint size) {
	UniquePtr<xi_file_iovec_t[]> vectors(
			initIoVec(env, jBuffers, jOffsets, jLengths, size));
	if (vectors.get() == NULL) {
		return -1;
	}
	long result = xi_file_writev(fd, vectors.get(), size);
	if (result < 0) {
		jniThrowIOExceptionMsg(env, "writev error!!", result);
	}
	return result;
}

/*
 static jlong OSFileSystem_transfer(JNIEnv* env, jobject, jint fd, jobject sd,
 jlong offset, jlong count) {
 */
JNIEXPORT jlong JNICALL
Java_org_apache_harmony_luni_platform_OSFileSystem_transfer(JNIEnv* env,
		jobject, jint fd, jobject sd, jlong offset, jlong count) {

	int socket = jniGetFDFromFileDescriptor(env, sd);
	if (socket == -1) {
		return -1;
	}

	/* Value of offset is checked in jint scope (checked in java layer)
	 The conversion here is to guarantee no value lost when converting offset to off_t
	 */
	xoff64 off = offset;

	xint64 rc = xi_socket_sendfile(socket, fd, &off, count);
	if (rc < 0) {
		jniThrowIOExceptionMsg(env, "sendfile error!!", rc);
	}
	return rc;
}

/*
 static jlong OSFileSystem_readDirect(JNIEnv* env, jobject, jint fd,
 jint buf, jint offset, jint byteCount) {
 */
JNIEXPORT jlong JNICALL
Java_org_apache_harmony_luni_platform_OSFileSystem_readDirect(JNIEnv* env,
		jobject, jint fd, jint buf, jint offset, jint byteCount) {
	//	log_print(XDLOG, "\n    [OSFileSystem_readDirect] fd: %d, offset: %d, byteCount: %d\n", fd, offset, byteCount);
	if (byteCount == 0) {
		return 0;
	}
	jbyte* dst = reinterpret_cast<jbyte*> (buf + offset);
	jlong rc = xi_file_read(fd, dst, byteCount);
	if (rc == 0) {
		return -1;
	}
	if (rc < 0) {
		// We return 0 rather than throw if we try to read from an empty non-blocking pipe.
		if (rc == -1) {
			return 0;
		}
		jniThrowIOExceptionMsg(env, "read direct error!!", rc);
	}
	return rc;
}

/*
 static jlong OSFileSystem_read(JNIEnv* env, jobject, jint fd,
 jbyteArray byteArray, jint offset, jint byteCount) {
 */
JNIEXPORT jlong JNICALL
Java_org_apache_harmony_luni_platform_OSFileSystem_read(JNIEnv* env, jobject,
		jint fd, jbyteArray byteArray, jint offset, jint byteCount) {
	ScopedByteArrayRW bytes(env, byteArray);
	if (bytes.get() == NULL) {
		return 0;
	}
	jint buf = static_cast<jint> (reinterpret_cast<xlong> (bytes.get()));
	return Java_org_apache_harmony_luni_platform_OSFileSystem_readDirect(env,
			NULL, fd, buf, offset, byteCount);
}

/*
 static jlong OSFileSystem_writeDirect(JNIEnv* env, jobject, jint fd,
 jint buf, jint offset, jint byteCount) {
 */
JNIEXPORT jlong JNICALL
Java_org_apache_harmony_luni_platform_OSFileSystem_writeDirect(JNIEnv* env,
		jobject, jint fd, jint buf, jint offset, jint byteCount) {

	// log_print(XDLOG, "    [OSFileSystem_writeDirect] fd: %d, buf: %d, offset: %d, byteCount: %d\n", fd, buf, offset, byteCount);
	if (byteCount == 0) {
		log_error(XDLOG, "    [OSFileSystem_writeDirec] Error: byteCount=0\n");
		return 0;
	}
	jbyte* src = reinterpret_cast<jbyte*> (buf + offset);
	jlong rc = xi_file_write(fd, src, byteCount);
	if (rc < 0) {
		log_error(XDLOG, "    [OSFileSystem_writeDirec] Error: rc=-1\n");
		jniThrowIOExceptionMsg(env, "write direct error!!", rc);
	}
	return rc;
}

/*
 static jlong OSFileSystem_write(JNIEnv* env, jobject, jint fd,
 jbyteArray byteArray, jint offset, jint byteCount) {
 */
JNIEXPORT jlong JNICALL
Java_org_apache_harmony_luni_platform_OSFileSystem_write(JNIEnv* env, jobject,
		jint fd, jbyteArray byteArray, jint offset, jint byteCount) {
	ScopedByteArrayRO bytes(env, byteArray);
	if (bytes.get() == NULL) {
		return 0;
	}
	jint buf = static_cast<jint> (reinterpret_cast<xlong> (bytes.get()));
	return Java_org_apache_harmony_luni_platform_OSFileSystem_writeDirect(env,
			NULL, fd, buf, offset, byteCount);
}

//static jlong OSFileSystem_seek(JNIEnv* env, jobject, jint fd, jlong offset, jint javaWhence) {
JNIEXPORT jlong JNICALL
Java_org_apache_harmony_luni_platform_OSFileSystem_seek(JNIEnv* env, jobject,
		jint fd, jlong offset, jint javaWhence) {
	/* Convert whence argument */
	xi_file_seek_e nativeWhence = XI_FILE_SEEK_SET;
	switch (javaWhence) {
	case 1:
		nativeWhence = XI_FILE_SEEK_SET;
		break;
	case 2:
		nativeWhence = XI_FILE_SEEK_CUR;
		break;
	case 4:
		nativeWhence = XI_FILE_SEEK_END;
		break;
	default:
		return -1;
	}

	jlong result = xi_file_seek(fd, offset, nativeWhence);
	if (result < 0) {
		jniThrowIOExceptionMsg(env, "seek error!!", result);
	}
	return result;
}

//static void OSFileSystem_fsync(JNIEnv* env, jobject, jint fd, jboolean metadataToo) {
JNIEXPORT void JNICALL
Java_org_apache_harmony_luni_platform_OSFileSystem_fsync(JNIEnv* env, jobject,
		jint fd, jboolean metadataToo) {
	if (!metadataToo) {
		log_warn(XDLOG, "fdatasync(2) unimplemented on Android - doing fsync(2)"); // http://b/2667481
	}
	int rc = xi_file_sync(fd);
	// int rc = metadataToo ? fsync(fd) : fdatasync(fd);
	if (rc < 0) {
		jniThrowIOExceptionMsg(env, "fsync error!!", rc);
	}
}

//static jint OSFileSystem_truncate(JNIEnv* env, jobject, jint fd, jlong length) {
JNIEXPORT void JNICALL
Java_org_apache_harmony_luni_platform_OSFileSystem_truncate(JNIEnv* env,
		jobject, jint fd, jlong length) {
	int rc = xi_file_ftruncate(fd, length);
	if (rc < 0) {
		jniThrowIOExceptionMsg(env, "ftruncate error!!", rc);
	}
	return;
}

//static jint OSFileSystem_open(JNIEnv* env, jobject, jstring javaPath, jint jflags) {
JNIEXPORT jint JNICALL
Java_org_apache_harmony_luni_platform_OSFileSystem_open(JNIEnv* env, jobject,
		jstring javaPath, jint jflags) {
	int flags = 0;
	int mode = 0;

	// On Android, we don't want default permissions to allow global access.
	switch (jflags) {
	case 0:
		flags = XI_FILE_MODE_READ;
		mode = 0;
		break;
	case 1:
		flags = XI_FILE_MODE_CREATE | XI_FILE_MODE_WRITE
				| XI_FILE_MODE_TRUNCATE;
		mode = 0600;
		break;
	case 16:
		flags = XI_FILE_MODE_READ | XI_FILE_MODE_WRITE | XI_FILE_MODE_CREATE;
		mode = 0600;
		break;
	case 32:
		flags = XI_FILE_MODE_READ | XI_FILE_MODE_WRITE | XI_FILE_MODE_CREATE; // | XI_FILE_MODE_LARGEFILE;
		mode = 0600;
		break;
	case 256:
		flags = XI_FILE_MODE_WRITE | XI_FILE_MODE_CREATE | XI_FILE_MODE_APPEND;
		mode = 0600;
		break;
	}

	ScopedUtfChars path(env, javaPath);
	if (path.c_str() == NULL) {
		return -1;
	}

	//	log_print(XDLOG, "=============================================>\n");
	//	log_print(XDLOG, "==============> %s\n", path.c_str());

	jint rc = xi_file_open(path.c_str(), flags, mode);
	if (rc < 0) {
		// Get the human-readable form of errno.
		char buffer[256];

		// log_print(XDLOG, "!!!!!!!!!!!!!!!! File Open ERROR : %s\n", path.c_str());
		xi_snprintf(buffer, sizeof(buffer), "File Open ERROR!!! (%s)",
				path.c_str());

		// We always throw FileNotFoundException, regardless of the specific
		// failure. (This appears to be true of the RI too.)
		jniThrowException(env, "java/io/FileNotFoundException", buffer);
	}
	return rc;
}

//static jint OSFileSystem_ioctlAvailable(JNIEnv*env, jobject, jobject fileDescriptor) {
JNIEXPORT jint JNICALL
Java_org_apache_harmony_luni_platform_OSFileSystem_ioctlAvailable(JNIEnv* env,
		jobject, jobject fileDescriptor) {
	/*
	 * On underlying platforms Android cares about (read "Linux"),
	 * ioctl(fd, FIONREAD, &avail) is supposed to do the following:
	 *
	 * If the fd refers to a regular file, avail is set to
	 * the difference between the file size and the current cursor.
	 * This may be negative if the cursor is past the end of the file.
	 *
	 * If the fd refers to an open socket or the read end of a
	 * pipe, then avail will be set to a number of bytes that are
	 * available to be read without blocking.
	 *
	 * If the fd refers to a special file/device that has some concept
	 * of buffering, then avail will be set in a corresponding way.
	 *
	 * If the fd refers to a special device that does not have any
	 * concept of buffering, then the ioctl call will return a negative
	 * number, and errno will be set to ENOTTY.
	 *
	 * If the fd refers to a special file masquerading as a regular file,
	 * then avail may be returned as negative, in that the special file
	 * may appear to have zero size and yet a previous read call may have
	 * actually read some amount of data and caused the cursor to be
	 * advanced.
	 */

	//log_print(XDLOG, "[OSFileSystem] BEGIN\n");
	int fd = jniGetFDFromFileDescriptor(env, fileDescriptor);
	if (fd == -1) {
		//log_error(XDLOG, "[OSFileSystem] fd = -1\n");
		return -1;
	}
	int avail = 0;
	int rc = xi_file_rpeek(fd);
	//log_info(XDLOG, "[available] = %d\n", rc);
	if (rc >= 0) {
		//log_trace(XDLOG, "[OSFileSystem] rc>= 0\n");
		avail = rc;
	} else if (rc == -2) {
		//log_trace(XDLOG, "[OSFileSystem] ENOTTY\n");
		avail = 0;
	} else {
		//		log_error(XDLOG, "[OSFileSystem] Exception : fd=%d\n", fd);
		//		return jniThrowIOExceptionMsg(env, "rpeek error!!", rc);
		return 0;
	}

	//log_print(XDLOG, "[OSFileSystem] avail: %d\n", avail);
	//log_print(XDLOG, "[OSFileSystem] END\n");
	return avail;
}

//static jlong OSFileSystem_length(JNIEnv* env, jobject, jint fd) {
JNIEXPORT jlong JNICALL
Java_org_apache_harmony_luni_platform_OSFileSystem_length(JNIEnv* env, jobject,
		jint fd) {
	xint64 ret = 0;
	xi_file_stat_t se;

	ret = xi_file_fstat(fd, &se);
	if (ret < 0) {
		log_trace(XDLOG, "ret = %d\n", ret);
		return jniThrowIOExceptionMsg(env, "fstat error!!", ret);
	} else {
		return se.size;
	}
}

/* by jshwang
 static JNINativeMethod gMethods[] = {
 NATIVE_METHOD(OSFileSystem, fsync, "(IZ)V"),
 NATIVE_METHOD(OSFileSystem, getAllocGranularity, "()I"),
 NATIVE_METHOD(OSFileSystem, ioctlAvailable, "(Ljava/io/FileDescriptor;)I"),
 NATIVE_METHOD(OSFileSystem, length, "(I)J"),
 NATIVE_METHOD(OSFileSystem, lockImpl, "(IJJIZ)I"),
 NATIVE_METHOD(OSFileSystem, open, "(Ljava/lang/String;I)I"),
 NATIVE_METHOD(OSFileSystem, read, "(I[BII)J"),
 NATIVE_METHOD(OSFileSystem, readDirect, "(IIII)J"),
 NATIVE_METHOD(OSFileSystem, readv, "(I[I[I[II)J"),
 NATIVE_METHOD(OSFileSystem, seek, "(IJI)J"),
 NATIVE_METHOD(OSFileSystem, transfer, "(ILjava/io/FileDescriptor;JJ)J"),
 NATIVE_METHOD(OSFileSystem, truncate, "(IJ)V"),
 NATIVE_METHOD(OSFileSystem, unlockImpl, "(IJJ)V"),
 NATIVE_METHOD(OSFileSystem, write, "(I[BII)J"),
 NATIVE_METHOD(OSFileSystem, writeDirect, "(IIII)J"),
 NATIVE_METHOD(OSFileSystem, writev, "(I[I[I[II)J"),
 };
 int register_org_apache_harmony_luni_platform_OSFileSystem(JNIEnv* env) {
 return jniRegisterNativeMethods(env, "org/apache/harmony/luni/platform/OSFileSystem", gMethods,
 NELEM(gMethods));
 }
 */
int register_org_apache_harmony_luni_platform_OSFileSystem(JNIEnv*) {
	return JNI_OK;
}

