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

#include "java_io_File.h"
#include "JNIHelp.h"
#include "JniConstants.h"
#include "LocalArray.h"
#include "ScopedFd.h"
#include "ScopedLocalRef.h"
#include "ScopedPrimitiveArray.h"
#include "ScopedUtfChars.h"

#include <utime.h>

#if 0 // by cmjo
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/vfs.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#endif

#include "xi/xi_file.h"

static bool doStat(JNIEnv* env, jstring javaPath, xi_file_stat_t& sb) {
	ScopedUtfChars path(env, javaPath);
	if (path.c_str() == NULL) {
		return JNI_FALSE;
	}
	return (xi_file_stat(path.c_str(), &sb) == 0);
}

//static jboolean File_deleteImpl(JNIEnv* env, jclass, jstring javaPath) {
JNIEXPORT jboolean JNICALL
Java_java_io_File_deleteImpl(JNIEnv* env, jclass, jstring javaPath) {
	xi_file_stat_t sb;
	ScopedUtfChars path(env, javaPath);
	if (path.c_str() == NULL) {
		return JNI_FALSE;
	}
	if (xi_file_stat(path.c_str(), &sb) != 0) {
		return JNI_FALSE;
	}
	if ((sb.type & XI_FILE_TYPE_DIR) == XI_FILE_TYPE_DIR) {
		return (xi_dir_remove(path.c_str()) == 0);
	} else {
		return (xi_file_remove(path.c_str()) == 0);
	}
}

//static jlong File_lengthImpl(JNIEnv* env, jclass, jstring javaPath) {
JNIEXPORT jlong JNICALL
Java_java_io_File_lengthImpl(JNIEnv* env, jclass, jstring javaPath) {
	xi_file_stat_t sb;

	if (!doStat(env, javaPath, sb)) {
		return 0;
	}
	//log_trace(XDLOG, "sb.type = %d / sb.blocks = %d\n", sb.type, sb.blocks);
	if ((sb.type & XI_FILE_TYPE_REG) != XI_FILE_TYPE_REG || sb.blocks == 0) {
		return 0;
	}
	return sb.size;
}

//static jlong File_lastModifiedImpl(JNIEnv* env, jclass, jstring javaPath) {
JNIEXPORT jlong JNICALL
Java_java_io_File_lastModifiedImpl(JNIEnv* env, jclass, jstring javaPath) {
	xi_file_stat_t sb;

	if (!doStat(env, javaPath, sb)) {
		return 0;
	}

	return sb.modified;
}

//static jboolean File_isDirectoryImpl(JNIEnv* env, jclass, jstring javaPath) {
JNIEXPORT jboolean JNICALL
Java_java_io_File_isDirectoryImpl(JNIEnv* env, jclass, jstring javaPath) {
	xi_file_stat_t sb;

	if (!doStat(env, javaPath, sb)) {
		return JNI_FALSE;
	}

	return ((sb.type & XI_FILE_TYPE_DIR) == XI_FILE_TYPE_DIR);
}

//static jboolean File_isFileImpl(JNIEnv* env, jclass, jstring javaPath) {
JNIEXPORT jboolean JNICALL
Java_java_io_File_isFileImpl(JNIEnv* env, jclass, jstring javaPath) {
	xi_file_stat_t sb;

	if (!doStat(env, javaPath, sb)) {
		return JNI_FALSE;
	}

	return ((sb.type & XI_FILE_TYPE_REG) == XI_FILE_TYPE_REG);
}

static jboolean doAccess(JNIEnv* env, jstring javaPath, int mode) {
	xi_file_stat_t sb;
	xint32 ret;
	xint32 t = 0;

	ScopedUtfChars path(env, javaPath);

	if (path.c_str() == NULL) {
		return JNI_FALSE;
	}

	ret = xi_file_stat(path.c_str(), &sb);
	if (ret < 0) {
		return JNI_FALSE;
	}

	t = (sb.perm >> 16);

	return ((t & mode) == mode);
}

//static jboolean File_existsImpl(JNIEnv* env, jclass, jstring javaPath) {
JNIEXPORT jboolean JNICALL
Java_java_io_File_existsImpl(JNIEnv* env, jclass, jstring javaPath) {
	return doAccess(env, javaPath, F_OK);
}

//static jboolean File_canExecuteImpl(JNIEnv* env, jclass, jstring javaPath) {
JNIEXPORT jboolean JNICALL
Java_java_io_File_canExecuteImpl(JNIEnv* env, jclass, jstring javaPath) {
	return doAccess(env, javaPath, X_OK);
}

//static jboolean File_canReadImpl(JNIEnv* env, jclass, jstring javaPath) {
JNIEXPORT jboolean JNICALL
Java_java_io_File_canReadImpl(JNIEnv* env, jclass, jstring javaPath) {
	return doAccess(env, javaPath, R_OK);
}

//static jboolean File_canWriteImpl(JNIEnv* env, jclass, jstring javaPath) {
JNIEXPORT jboolean JNICALL
Java_java_io_File_canWriteImpl(JNIEnv* env, jclass, jstring javaPath) {
	return doAccess(env, javaPath, W_OK);
}

//static jstring File_readlink(JNIEnv* env, jclass, jstring javaPath) {
JNIEXPORT jstring JNICALL
Java_java_io_File_readlink(JNIEnv* env, jclass, jstring javaPath) {
	ScopedUtfChars path(env, javaPath);
	if (path.c_str() == NULL) {
		return NULL;
	}

	// We can't know how big a buffer readlink(2) will need, so we need to
	// loop until it says "that fit".
	size_t bufSize = 512;
	while (true) {
		LocalArray<512> buf(bufSize);
		xint32 len = xi_file_readlink(path.c_str(), &buf[0], buf.size() - 1);
		if (len < 0) {
			// An error occurred.
			return javaPath;
		}
		if (static_cast<size_t> (len) < buf.size() - 1) {
			// The buffer was big enough.
			buf[len] = '\0'; // readlink(2) doesn't NUL-terminate.
			return env->NewStringUTF(&buf[0]);
		}
		// Try again with a bigger buffer.
		bufSize *= 2;
	}
}

//static jboolean File_setLastModifiedImpl(JNIEnv* env, jclass, jstring javaPath, jlong ms) {
JNIEXPORT jboolean JNICALL
Java_java_io_File_setLastModifiedImpl(JNIEnv* env, jclass, jstring javaPath,
		jlong ms) {
	ScopedUtfChars path(env, javaPath);
	if (path.c_str() == NULL) {
		return JNI_FALSE;
	}

	// We want to preserve the access time.
	xi_file_stat_t sb;
	if (xi_file_stat(path.c_str(), &sb) < 0) {
		return JNI_FALSE;
	}

	// TODO: we could get microsecond resolution with utimes(3), "legacy" though it is.
	utimbuf times;
	xint64 msec = sb.accessed;
	times.actime = static_cast<time_t> (msec / 1000);
	times.modtime = static_cast<time_t> (ms / 1000);
	return (utime(path.c_str(), &times) == 0);
}

static jboolean doChmod(JNIEnv* env, jstring javaPath, int mask, bool set) {
	ScopedUtfChars path(env, javaPath);
	if (path.c_str() == NULL) {
		return JNI_FALSE;
	}

	xi_file_stat_t sb;
	if (xi_file_stat(path.c_str(), &sb) == -1) {
		return JNI_FALSE;
	}
	int newMode = set ? (sb.perm | mask) : (sb.perm & ~mask);
	return (xi_file_chmod(path.c_str(), newMode) == 0);
}

//static jboolean File_setExecutableImpl(JNIEnv* env, jclass, jstring javaPath,
JNIEXPORT jboolean JNICALL
Java_java_io_File_setExecutableImpl(JNIEnv* env, jclass, jstring javaPath,
		jboolean set, jboolean ownerOnly) {
	return doChmod(
			env,
			javaPath,
			ownerOnly ? XI_FILE_PERM_USR_EXEC : (XI_FILE_PERM_USR_EXEC
					| XI_FILE_PERM_GRP_EXEC | XI_FILE_PERM_OTH_EXEC), set);
}

//static jboolean File_setReadableImpl(JNIEnv* env, jclass, jstring javaPath,
JNIEXPORT jboolean JNICALL
Java_java_io_File_setReadableImpl(JNIEnv* env, jclass, jstring javaPath,
		jboolean set, jboolean ownerOnly) {
	return doChmod(
			env,
			javaPath,
			ownerOnly ? XI_FILE_PERM_USR_READ : (XI_FILE_PERM_USR_READ
					| XI_FILE_PERM_GRP_READ | XI_FILE_PERM_OTH_READ), set);
}

//static jboolean File_setWritableImpl(JNIEnv* env, jclass, jstring javaPath,
JNIEXPORT jboolean JNICALL
Java_java_io_File_setWritableImpl(JNIEnv* env, jclass, jstring javaPath,
		jboolean set, jboolean ownerOnly) {
	return doChmod(
			env,
			javaPath,
			ownerOnly ? XI_FILE_PERM_USR_WRITE : (XI_FILE_PERM_USR_WRITE
					| XI_FILE_PERM_GRP_WRITE | XI_FILE_PERM_OTH_WRITE), set);
}

static bool doStatFs(JNIEnv* env, jstring javaPath, xi_fs_space_t& sb) {
	ScopedUtfChars path(env, javaPath);
	if (path.c_str() == NULL) {
		return JNI_FALSE;
	}

	int rc = xi_file_fsspace(path.c_str(), &sb);
	return (rc == 0);
}

//static jlong File_getFreeSpaceImpl(JNIEnv* env, jclass, jstring javaPath) {
JNIEXPORT jlong JNICALL
Java_java_io_File_getFreeSpaceImpl(JNIEnv* env, jclass, jstring javaPath) {
	xi_fs_space_t sb;
	if (!doStatFs(env, javaPath, sb)) {
		return 0;
	}
	return sb.free;
}

//static jlong File_getTotalSpaceImpl(JNIEnv* env, jclass, jstring javaPath) {
JNIEXPORT jlong JNICALL
Java_java_io_File_getTotalSpaceImpl(JNIEnv* env, jclass, jstring javaPath) {
	xi_fs_space_t sb;
	if (!doStatFs(env, javaPath, sb)) {
		return 0;
	}
	return sb.total;
}

//static jlong File_getUsableSpaceImpl(JNIEnv* env, jclass, jstring javaPath) {
JNIEXPORT jlong JNICALL
Java_java_io_File_getUsableSpaceImpl(JNIEnv* env, jclass, jstring javaPath) {
	xi_fs_space_t sb;
	if (!doStatFs(env, javaPath, sb)) {
		return 0;
	}
	return sb.avail;
}

// Iterates over the filenames in the given directory.
class ScopedReaddir {
public:
	ScopedReaddir(const char* path) {
		mDirStream = xi_dir_open(path);
		mIsBad = (mDirStream == NULL);
		//log_trace(XDLOG, "Open Directory : %s\n", path);
	}

	~ScopedReaddir() {
		if (mDirStream != NULL) {
			xi_dir_close(mDirStream);
		}
	}

	// Returns the next filename, or NULL.
	const char* next() {
		int rc = xi_dir_read(mDirStream, &mEntry);
		if (rc <= 0) {
			mIsBad = true;
			return NULL;
		}
		return mEntry.filename;
	}

	// Has an error occurred on this stream?
	bool isBad() const {
		return mIsBad;
	}

private:
	xi_dir_t *mDirStream;
	xi_file_stat_t mEntry;
	bool mIsBad;

	// Disallow copy and assignment.
	ScopedReaddir(const ScopedReaddir&);
	void operator=(const ScopedReaddir&);
};

// DirEntry and DirEntries is a minimal equivalent of std::forward_list
// for the filenames.
struct DirEntry {
	DirEntry(const char* filename) :
		name(xi_strlen(filename)) {
		xi_strcpy(&name[0], filename);
		next = NULL;
	}
	// On Linux, the ext family all limit the length of a directory entry to
	// less than 256 characters.
	LocalArray<256> name;
	DirEntry* next;
};

class DirEntries {
public:
	DirEntries() :
		mSize(0), mHead(NULL) {
	}

	~DirEntries() {
		while (mHead) {
			pop_front();
		}
	}

	bool push_front(const char* name) {
		DirEntry* oldHead = mHead;
		mHead = new DirEntry(name);
		if (mHead == NULL) {
			return false;
		}
		mHead->next = oldHead;
		++mSize;
		return true;
	}

	const char* front() const {
		return &mHead->name[0];
	}

	void pop_front() {
		DirEntry* popped = mHead;
		if (popped != NULL) {
			mHead = popped->next;
			--mSize;
			delete popped;
		}
	}

	size_t size() const {
		return mSize;
	}

private:
	size_t mSize;
	DirEntry* mHead;

	// Disallow copy and assignment.
	DirEntries(const DirEntries&);
	void operator=(const DirEntries&);
};

// Reads the directory referred to by 'pathBytes', adding each directory entry
// to 'entries'.
static bool readDirectory(JNIEnv* env, jstring javaPath, DirEntries& entries) {
	ScopedUtfChars path(env, javaPath);
	if (path.c_str() == NULL) {
		return false;
	}

	ScopedReaddir dir(path.c_str());
	if (dir.isBad()) {
		log_error(XDLOG, "Cannot open directory : %s\n", path.c_str());
		return false;
	}
	const char* filename;
	while ((filename = dir.next()) != NULL) {
		//log_trace(XDLOG, "[ReadDirector] filename = %s\n", filename);
		if (xi_strcmp(filename, ".") != 0 && xi_strcmp(filename, "..") != 0) {
			if (!entries.push_front(filename)) {
				jniThrowException(env, "java/lang/OutOfMemoryError", NULL);
				return false;
			}
		}
	}
	return true;
}

//static jobjectArray File_listImpl(JNIEnv* env, jclass, jstring javaPath) {
JNIEXPORT jobjectArray JNICALL
Java_java_io_File_listImpl(JNIEnv* env, jclass, jstring javaPath) {
	// Read the directory entries into an intermediate form.
	DirEntries files;
	if (!readDirectory(env, javaPath, files)) {
		return NULL;
	}
	// Translate the intermediate form into a Java String[].
	jclass stringClass = env->FindClass("java/lang/String");
	jobjectArray result = env->NewObjectArray(files.size(), stringClass, NULL);
	for (int i = 0; files.size() != 0; files.pop_front(), ++i) {
		ScopedLocalRef<jstring> javaFilename(env,
				env->NewStringUTF(files.front()));
		if (env->ExceptionCheck()) {
			return NULL;
		}
		env->SetObjectArrayElement(result, i, javaFilename.get());
		if (env->ExceptionCheck()) {
			return NULL;
		}
	}
	return result;
}

//static jboolean File_mkdirImpl(JNIEnv* env, jclass, jstring javaPath) {
JNIEXPORT jboolean JNICALL
Java_java_io_File_mkdirImpl(JNIEnv* env, jclass, jstring javaPath) {
	ScopedUtfChars path(env, javaPath);
	if (path.c_str() == NULL) {
		return JNI_FALSE;
	}

	// On Android, we don't want default permissions to allow global access.
	return (xi_dir_make(path.c_str(), XI_FILE_PERM_USR_ALL) == 0);
}

//static jboolean File_createNewFileImpl(JNIEnv* env, jclass, jstring javaPath) {
JNIEXPORT jboolean JNICALL
Java_java_io_File_createNewFileImpl(JNIEnv* env, jclass, jstring javaPath) {
	ScopedUtfChars path(env, javaPath);
	if (path.c_str() == NULL) {
		return JNI_FALSE;
	}

	// On Android, we don't want default permissions to allow global access.
	ScopedFd fd(
			xi_file_open(path.c_str(), XI_FILE_MODE_CREATE | XI_FILE_MODE_EXCL,
					0600));
	if (fd.get() > 0) {
		// We created a new file. Success!
		return JNI_TRUE;
	}
	if (fd.get() == XI_FILE_RV_ERR_AE) {
		// The file already exists.
		return JNI_FALSE;
	}
	jniThrowIOExceptionMsg(env, "create file error!!", fd.get());
	return JNI_FALSE; // Ignored by Java; keeps the C++ compiler happy.
}

//static jboolean File_renameToImpl(JNIEnv* env, jclass, jstring javaOldPath, jstring javaNewPath) {
JNIEXPORT jboolean JNICALL
Java_java_io_File_renameToImpl(JNIEnv* env, jclass, jstring javaOldPath,
		jstring javaNewPath) {
	ScopedUtfChars oldPath(env, javaOldPath);
	if (oldPath.c_str() == NULL) {
		return JNI_FALSE;
	}

	ScopedUtfChars newPath(env, javaNewPath);
	if (newPath.c_str() == NULL) {
		return JNI_FALSE;
	}

	return (xi_file_rename(oldPath.c_str(), newPath.c_str()) == 0);
}

/* by jshwang
 static JNINativeMethod gMethods[] = {
 NATIVE_METHOD(File, canExecuteImpl, "(Ljava/lang/String;)Z"),
 NATIVE_METHOD(File, canReadImpl, "(Ljava/lang/String;)Z"),
 NATIVE_METHOD(File, canWriteImpl, "(Ljava/lang/String;)Z"),
 NATIVE_METHOD(File, createNewFileImpl, "(Ljava/lang/String;)Z"),
 NATIVE_METHOD(File, deleteImpl, "(Ljava/lang/String;)Z"),
 NATIVE_METHOD(File, existsImpl, "(Ljava/lang/String;)Z"),
 NATIVE_METHOD(File, getFreeSpaceImpl, "(Ljava/lang/String;)J"),
 NATIVE_METHOD(File, getTotalSpaceImpl, "(Ljava/lang/String;)J"),
 NATIVE_METHOD(File, getUsableSpaceImpl, "(Ljava/lang/String;)J"),
 NATIVE_METHOD(File, isDirectoryImpl, "(Ljava/lang/String;)Z"),
 NATIVE_METHOD(File, isFileImpl, "(Ljava/lang/String;)Z"),
 NATIVE_METHOD(File, lastModifiedImpl, "(Ljava/lang/String;)J"),
 NATIVE_METHOD(File, lengthImpl, "(Ljava/lang/String;)J"),
 NATIVE_METHOD(File, listImpl, "(Ljava/lang/String;)[Ljava/lang/String;"),
 NATIVE_METHOD(File, mkdirImpl, "(Ljava/lang/String;)Z"),
 NATIVE_METHOD(File, readlink, "(Ljava/lang/String;)Ljava/lang/String;"),
 NATIVE_METHOD(File, renameToImpl, "(Ljava/lang/String;Ljava/lang/String;)Z"),
 NATIVE_METHOD(File, setExecutableImpl, "(Ljava/lang/String;ZZ)Z"),
 NATIVE_METHOD(File, setLastModifiedImpl, "(Ljava/lang/String;J)Z"),
 NATIVE_METHOD(File, setReadableImpl, "(Ljava/lang/String;ZZ)Z"),
 NATIVE_METHOD(File, setWritableImpl, "(Ljava/lang/String;ZZ)Z"),
 };
 int register_java_io_File(JNIEnv* env) {
 return jniRegisterNativeMethods(env, "java/io/File", gMethods, NELEM(gMethods));
 }
 */
int register_java_io_File(JNIEnv*) {
	return JNI_OK;
}

