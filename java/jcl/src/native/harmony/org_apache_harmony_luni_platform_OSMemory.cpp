/*
 * Copyright (C) 2007 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define LOG_TAG "OSMemory"

#include "org_apache_harmony_luni_platform_OSMemory.h"
#include "JNIHelp.h"
#include "JniConstants.h"
#include "UniquePtr.h"

//#include <errno.h>
//#include <stdlib.h>
//#include <string.h>
// by cmcho
//#include <sys/mman.h>

#include "xi/xi_log.h"
#include "xi/xi_mem.h"
#include "xi/xi_mmap.h"
#include "xi/xi_sysinfo.h"

//#include <stdint.h>
//#include <errno.h>

template<typename T> static T cast(jint address) {
	//return reinterpret_cast<T> (static_cast<uintptr_t> (address));
	return reinterpret_cast<T> (static_cast<xuintptr> (address));
}

//static jint OSMemory_malloc(JNIEnv* env, jclass, jint size) {
JNIEXPORT jint JNICALL
Java_org_apache_harmony_luni_platform_OSMemory_malloc(JNIEnv* env, jclass,
		jint size) {
	/* by jshwang
	 jboolean allowed = env->CallBooleanMethod(gIDCache.runtimeInstance,
	 gIDCache.method_trackExternalAllocation, static_cast<jlong>(size));
	 if (!allowed) {
	 LOGW("External allocation of %d bytes was rejected\n", size);
	 jniThrowException(env, "java/lang/OutOfMemoryError", NULL);
	 return 0;
	 }
	 */
	//log_trace(XDLOG, "OSMemory alloc %d\n", size);
	void* block = xi_mem_calloc(size, sizeof(jlong));
	if (block == NULL) {
		jniThrowException(env, "java/lang/OutOfMemoryError", NULL);
		return 0;
	}

	/*
	 * Tuck a copy of the size at the head of the buffer.  We need this
	 * so OSMemory_free() knows how much memory is being freed.
	 */
	jlong* result = reinterpret_cast<jlong*> (block);
	*result++ = size;
	return static_cast<jint> (reinterpret_cast<xuintptr> (result));
	//return static_cast<jint> (reinterpret_cast<uintptr_t> (result));
}

//static void OSMemory_free(JNIEnv* env, jclass, jint address) {
JNIEXPORT void JNICALL
Java_org_apache_harmony_luni_platform_OSMemory_free(JNIEnv* env, jclass,
		jint address) {
	jlong* p = reinterpret_cast<jlong*> (static_cast<xuintptr> (address));
	//jlong* p = reinterpret_cast<jlong*> (static_cast<uintptr_t> (address));
	jlong size = *--p;
	log_trace(XDLOG, "OSMemory free %ld\n", size);
	/* by jshwang
	 env->CallVoidMethod(gIDCache.runtimeInstance, gIDCache.method_trackExternalFree, size);
	 */
	xi_mem_free(reinterpret_cast<void*> (p));
}

//static void OSMemory_memset(JNIEnv*, jclass, jint dstAddress, jbyte value, jlong length) {
JNIEXPORT void JNICALL
Java_org_apache_harmony_luni_platform_OSMemory_memset(JNIEnv*, jclass,
		jint dstAddress, jbyte value, jlong length) {
	xi_mem_set(cast<void*> (dstAddress), value, length);
}

//static void OSMemory_memmove(JNIEnv*, jclass, jint dstAddress, jint srcAddress, jlong length) {
JNIEXPORT void JNICALL
Java_org_apache_harmony_luni_platform_OSMemory_memmove(JNIEnv*, jclass,
		jint dstAddress, jint srcAddress, jlong length) {
	xi_mem_move(cast<void*> (dstAddress), cast<const void*> (srcAddress),
			length);
}

//static jbyte OSMemory_getByte(JNIEnv*, jclass, jint srcAddress) {
JNIEXPORT jbyte JNICALL
Java_org_apache_harmony_luni_platform_OSMemory_getByte(JNIEnv*, jclass,
		jint srcAddress) {
	return *cast<const jbyte*> (srcAddress);
}

//static void OSMemory_getByteArray(JNIEnv* env, jclass, jint srcAddress,
JNIEXPORT void JNICALL
Java_org_apache_harmony_luni_platform_OSMemory_getByteArray(JNIEnv* env,
		jclass, jint srcAddress, jbyteArray dst, jint offset, jint length) {
	// by jshwang
	//    env->SetByteArrayRegion(dst, offset, length, cast<const jbyte*>(srcAddress));
	env->SetByteArrayRegion(dst, offset, length, cast<jbyte*> (srcAddress));
}

//static void OSMemory_setByte(JNIEnv*, jclass, jint dstAddress, jbyte value) {
JNIEXPORT void JNICALL
Java_org_apache_harmony_luni_platform_OSMemory_setByte(JNIEnv*, jclass,
		jint dstAddress, jbyte value) {
	*cast<jbyte*> (dstAddress) = value;
}

/*
 Java_org_apache_harmony_luni_platform_OSMemory_setByteArray(JNIEnv* env, jclass,
 jint dstAddress, jbyteArray src, jint offset, jint length) {
 */
JNIEXPORT void JNICALL
Java_org_apache_harmony_luni_platform_OSMemory_setByteArray(JNIEnv* env,
		jclass, jint dstAddress, jbyteArray src, jint offset, jint length) {
	env->GetByteArrayRegion(src, offset, length, cast<jbyte*> (dstAddress));
}

static void swapShorts(jshort* shorts, int count) {
	jbyte* src = reinterpret_cast<jbyte*> (shorts);
	jbyte* dst = src;
	for (int i = 0; i < count; ++i) {
		jbyte b0 = *src++;
		jbyte b1 = *src++;
		*dst++ = b1;
		*dst++ = b0;
	}
}

static void swapInts(jint* ints, int count) {
	jbyte* src = reinterpret_cast<jbyte*> (ints);
	jbyte* dst = src;
	for (int i = 0; i < count; ++i) {
		jbyte b0 = *src++;
		jbyte b1 = *src++;
		jbyte b2 = *src++;
		jbyte b3 = *src++;
		*dst++ = b3;
		*dst++ = b2;
		*dst++ = b1;
		*dst++ = b0;
	}
}

/*
 Java_org_apache_harmony_luni_platform_OSMemory_setFloatArray(JNIEnv* env, jclass, jint dstAddress,
 jfloatArray src, jint offset, jint length, jboolean swap) {
 */
JNIEXPORT void JNICALL
Java_org_apache_harmony_luni_platform_OSMemory_setFloatArray(JNIEnv* env,
		jclass, jint dstAddress, jfloatArray src, jint offset, jint length,
		jboolean swap) {
	jfloat* dst = cast<jfloat*> (dstAddress);
	env->GetFloatArrayRegion(src, offset, length, dst);
	if (swap) {
		swapInts(reinterpret_cast<jint*> (dst), length);
	}
}

/*
 Java_org_apache_harmony_luni_platform_OSMemory_setIntArray(JNIEnv* env, jclass,
 jint dstAddress, jintArray src, jint offset, jint length, jboolean swap) {
 */
JNIEXPORT void JNICALL
Java_org_apache_harmony_luni_platform_OSMemory_setIntArray(JNIEnv* env, jclass,
		jint dstAddress, jintArray src, jint offset, jint length, jboolean swap) {
	jint* dst = cast<jint*> (dstAddress);
	env->GetIntArrayRegion(src, offset, length, dst);
	if (swap) {
		swapInts(dst, length);
	}
}

/*
 Java_org_apache_harmony_luni_platform_OSMemory_setShortArray(JNIEnv* env, jclass,
 jint dstAddress, jshortArray src, jint offset, jint length, jboolean swap) {
 */
JNIEXPORT void JNICALL
Java_org_apache_harmony_luni_platform_OSMemory_setShortArray(JNIEnv* env,
		jclass, jint dstAddress, jshortArray src, jint offset, jint length,
		jboolean swap) {
	jshort* dst = cast<jshort*> (dstAddress);
	env->GetShortArrayRegion(src, offset, length, dst);
	if (swap) {
		swapShorts(dst, length);
	}
}

//static jshort OSMemory_getShort(JNIEnv*, jclass, jint srcAddress) {
JNIEXPORT jshort JNICALL
Java_org_apache_harmony_luni_platform_OSMemory_getShort(JNIEnv*, jclass,
		jint srcAddress) {
	if ((srcAddress & 0x1) == 0) {
		return *cast<const jshort*> (srcAddress);
	} else {
		// Handle unaligned memory access one byte at a time
		jshort result;
		const jbyte* src = cast<const jbyte*> (srcAddress);
		jbyte* dst = reinterpret_cast<jbyte*> (&result);
		dst[0] = src[0];
		dst[1] = src[1];
		return result;
	}
}

//static void OSMemory_setShort(JNIEnv*, jclass, jint dstAddress, jshort value) {
JNIEXPORT void JNICALL
Java_org_apache_harmony_luni_platform_OSMemory_setShort(JNIEnv*, jclass,
		jint dstAddress, jshort value) {
	if ((dstAddress & 0x1) == 0) {
		*cast<jshort*> (dstAddress) = value;
	} else {
		// Handle unaligned memory access one byte at a time
		const jbyte* src = reinterpret_cast<const jbyte*> (&value);
		jbyte* dst = cast<jbyte*> (dstAddress);
		dst[0] = src[0];
		dst[1] = src[1];
	}
}

//static jint OSMemory_getInt(JNIEnv*, jclass, jint srcAddress) {
JNIEXPORT jint JNICALL
Java_org_apache_harmony_luni_platform_OSMemory_getInt(JNIEnv*, jclass,
		jint srcAddress) {
	if ((srcAddress & 0x3) == 0) {
		return *cast<const jint*> (srcAddress);
	} else {
		// Handle unaligned memory access one byte at a time
		jint result;
		const jbyte* src = cast<const jbyte*> (srcAddress);
		jbyte* dst = reinterpret_cast<jbyte*> (&result);
		dst[0] = src[0];
		dst[1] = src[1];
		dst[2] = src[2];
		dst[3] = src[3];
		return result;
	}
}

//static void OSMemory_setInt(JNIEnv*, jclass, jint dstAddress, jint value) {
JNIEXPORT void JNICALL
Java_org_apache_harmony_luni_platform_OSMemory_setInt(JNIEnv*, jclass,
		jint dstAddress, jint value) {
	if ((dstAddress & 0x3) == 0) {
		*cast<jint*> (dstAddress) = value;
	} else {
		// Handle unaligned memory access one byte at a time
		const jbyte* src = reinterpret_cast<const jbyte*> (&value);
		jbyte* dst = cast<jbyte*> (dstAddress);
		dst[0] = src[0];
		dst[1] = src[1];
		dst[2] = src[2];
		dst[3] = src[3];
	}
}

template<typename T> static T get(jint srcAddress) {
	if ((srcAddress & (sizeof(T) - 1)) == 0) {
		return *cast<const T*> (srcAddress);
	} else {
		// Cast to void* so GCC can't assume correct alignment and optimize this out.
		const void* src = cast<const void*> (srcAddress);
		T result;
		xi_mem_copy(&result, src, sizeof(T));
		return result;
	}
}

template<typename T> static void set(jint dstAddress, T value) {
	if ((dstAddress & (sizeof(T) - 1)) == 0) {
		*cast<T*> (dstAddress) = value;
	} else {
		// Cast to void* so GCC can't assume correct alignment and optimize this out.
		void* dst = cast<void*> (dstAddress);
		xi_mem_copy(dst, &value, sizeof(T));
	}
}

//static jlong OSMemory_getLong(JNIEnv*, jclass, jint srcAddress) {
JNIEXPORT jlong JNICALL
Java_org_apache_harmony_luni_platform_OSMemory_getLong(JNIEnv*, jclass,
		jint srcAddress) {
	return get<jlong> (srcAddress);
}

//static void OSMemory_setLong(JNIEnv*, jclass, jint dstAddress, jlong value) {
JNIEXPORT void JNICALL
Java_org_apache_harmony_luni_platform_OSMemory_setLong(JNIEnv*, jclass,
		jint dstAddress, jlong value) {
	set<jlong> (dstAddress, value);
}

//static jfloat OSMemory_getFloat(JNIEnv*, jclass, jint srcAddress) {
JNIEXPORT jfloat JNICALL
Java_org_apache_harmony_luni_platform_OSMemory_getFloat(JNIEnv*, jclass,
		jint srcAddress) {
	return get<jfloat> (srcAddress);
}

//static void OSMemory_setFloat(JNIEnv*, jclass, jint dstAddress, jfloat value) {
JNIEXPORT void JNICALL
Java_org_apache_harmony_luni_platform_OSMemory_setFloat(JNIEnv*, jclass,
		jint dstAddress, jfloat value) {
	set<jfloat> (dstAddress, value);
}

//static jdouble OSMemory_getDouble(JNIEnv*, jclass, jint srcAddress) {
JNIEXPORT jdouble JNICALL
Java_org_apache_harmony_luni_platform_OSMemory_getDouble(JNIEnv*, jclass,
		jint srcAddress) {
	return get<jdouble> (srcAddress);
}

//static void OSMemory_setDouble(JNIEnv*, jclass, jint dstAddress, jdouble value) {
JNIEXPORT void JNICALL
Java_org_apache_harmony_luni_platform_OSMemory_setDouble(JNIEnv*, jclass,
		jint dstAddress, jdouble value) {
	set<jdouble> (dstAddress, value);
}

//static jint OSMemory_getAddress(JNIEnv*, jclass, jint srcAddress) {
JNIEXPORT jint JNICALL
Java_org_apache_harmony_luni_platform_OSMemory_getAddress(JNIEnv*, jclass,
		jint srcAddress) {
	return *cast<const jint*> (srcAddress);
}

//static void OSMemory_setAddress(JNIEnv*, jclass, jint dstAddress, jint value) {
JNIEXPORT void JNICALL
Java_org_apache_harmony_luni_platform_OSMemory_setAddress(JNIEnv*, jclass,
		jint dstAddress, jint value) {
	*cast<jint*> (dstAddress) = value;
}

//static jint OSMemory_mmapImpl(JNIEnv* env, jclass, jint fd, jlong offset, jlong size, jint mapMode) {
JNIEXPORT jint JNICALL
Java_org_apache_harmony_luni_platform_OSMemory_mmapImpl(JNIEnv* env, jclass,
		jint fd, jlong offset, jlong size, jint mapMode) {
	int prot, flags, ret;
	void* mapAddress = NULL;

	switch (mapMode) {
	case 0: // MapMode.PRIVATE
		prot = XI_MMAP_PROT_READ | XI_MMAP_PROT_WRITE;
		flags = XI_MMAP_TYPE_PRIVATE;
		break;
	case 1: // MapMode.READ_ONLY
		prot = XI_MMAP_PROT_READ;
		flags = XI_MMAP_TYPE_SHARED;
		break;
	case 2: // MapMode.READ_WRITE
		prot = XI_MMAP_PROT_READ | XI_MMAP_PROT_WRITE;
		flags = XI_MMAP_TYPE_SHARED;
		break;
	default:
		// TODO : <errno.h>
		//jniThrowIOException(env, EINVAL);
		jniThrowIOException(env, 22);
		log_error(XDLOG, "bad mapMode %i", mapMode);
		return -1;
	}

	ret = xi_mmap_map(&mapAddress, size, prot, flags, fd, offset);
	if (ret != XI_MMAP_RV_OK) {
		jniThrowIOException(env, ret);
	}
	return reinterpret_cast<xuintptr> (mapAddress);
	//return reinterpret_cast<uintptr_t> (mapAddress);
}

//static void OSMemory_unmap(JNIEnv*, jclass, jint address, jlong size) {
JNIEXPORT void JNICALL
Java_org_apache_harmony_luni_platform_OSMemory_unmap(JNIEnv*, jclass,
		jint address, jlong size) {
	xi_mmap_unmap(cast<void*> (address), size);
}

//static void OSMemory_load(JNIEnv*, jclass, jint address, jlong size) {
JNIEXPORT void JNICALL
Java_org_apache_harmony_luni_platform_OSMemory_load(JNIEnv*, jclass,
		jint address, jlong size) {
	if (xi_mmap_lock(cast<void*> (address), size) != -1) {
		xi_mmap_unlock(cast<void*> (address), size);
	}
}

//static jboolean OSMemory_isLoaded(JNIEnv*, jclass, jint address, jlong size) {
JNIEXPORT jboolean JNICALL
Java_org_apache_harmony_luni_platform_OSMemory_isLoaded(JNIEnv*, jclass,
		jint address, jlong size) {
	if (size == 0) {
		return JNI_TRUE;
	}

	/* FIXME:
	 * Windows only provides readiness status of memory pages
	 * by VirtualQuery(EX), seems not enough to support this
	 * function
	 * */
	//	static int page_size = xi_sysinfo_pagesize();
	//
	//	int align_offset = address % page_size;// addr should align with the boundary of a page.
	//	address -= align_offset;
	//	size += align_offset;
	//	int page_count = (size + page_size - 1) / page_size;
	//
	//	UniquePtr<unsigned char[]> vec(new unsigned char[page_count]);
	//	int rc = mincore(cast<void*> (address), size, vec.get());
	//	if (rc == -1) {
	//		return JNI_FALSE;
	//	}
	//
	//	for (int i = 0; i < page_count; ++i) {
	//		if (vec[i] != 1) {
	//			return JNI_FALSE;
	//		}
	//	}
	//	return JNI_TRUE;

	return JNI_FALSE;
}

//static void OSMemory_flush(JNIEnv*, jclass, jint address, jlong size) {
JNIEXPORT void JNICALL
Java_org_apache_harmony_luni_platform_OSMemory_flush(JNIEnv*, jclass,
		jint address, jlong size) {
	xi_mmap_sync(cast<void*> (address), size, XI_MMAP_SYNC_SYNC);
}

/* by jshwang
 static JNINativeMethod gMethods[] = {
 NATIVE_METHOD(OSMemory, flush, "(IJ)V"),
 NATIVE_METHOD(OSMemory, free, "(I)V"),
 NATIVE_METHOD(OSMemory, getAddress, "(I)I"),
 NATIVE_METHOD(OSMemory, getByte, "(I)B"),
 NATIVE_METHOD(OSMemory, getByteArray, "(I[BII)V"),
 NATIVE_METHOD(OSMemory, getDouble, "(I)D"),
 NATIVE_METHOD(OSMemory, getFloat, "(I)F"),
 NATIVE_METHOD(OSMemory, getInt, "(I)I"),
 NATIVE_METHOD(OSMemory, getLong, "(I)J"),
 NATIVE_METHOD(OSMemory, getShort, "(I)S"),
 NATIVE_METHOD(OSMemory, isLoaded, "(IJ)Z"),
 NATIVE_METHOD(OSMemory, load, "(IJ)V"),
 NATIVE_METHOD(OSMemory, malloc, "(I)I"),
 NATIVE_METHOD(OSMemory, memmove, "(IIJ)V"),
 NATIVE_METHOD(OSMemory, memset, "(IBJ)V"),
 NATIVE_METHOD(OSMemory, mmapImpl, "(IJJI)I"),
 NATIVE_METHOD(OSMemory, setAddress, "(II)V"),
 NATIVE_METHOD(OSMemory, setByte, "(IB)V"),
 NATIVE_METHOD(OSMemory, setByteArray, "(I[BII)V"),
 NATIVE_METHOD(OSMemory, setDouble, "(ID)V"),
 NATIVE_METHOD(OSMemory, setFloat, "(IF)V"),
 NATIVE_METHOD(OSMemory, setFloatArray, "(I[FIIZ)V"),
 NATIVE_METHOD(OSMemory, setInt, "(II)V"),
 NATIVE_METHOD(OSMemory, setIntArray, "(I[IIIZ)V"),
 NATIVE_METHOD(OSMemory, setLong, "(IJ)V"),
 NATIVE_METHOD(OSMemory, setShort, "(IS)V"),
 NATIVE_METHOD(OSMemory, setShortArray, "(I[SIIZ)V"),
 NATIVE_METHOD(OSMemory, unmap, "(IJ)V"),
 };
 int register_org_apache_harmony_luni_platform_OSMemory(JNIEnv* env) {
 //
 // We need to call VMRuntime.trackExternal{Allocation,Free}.  Cache
 // method IDs and a reference to the singleton.
 //
 gIDCache.method_trackExternalAllocation = env->GetMethodID(JniConstants::vmRuntimeClass,
 "trackExternalAllocation", "(J)Z");
 gIDCache.method_trackExternalFree = env->GetMethodID(JniConstants::vmRuntimeClass,
 "trackExternalFree", "(J)V");

 jmethodID method_getRuntime = env->GetStaticMethodID(JniConstants::vmRuntimeClass,
 "getRuntime", "()Ldalvik/system/VMRuntime;");

 if (gIDCache.method_trackExternalAllocation == NULL ||
 gIDCache.method_trackExternalFree == NULL ||
 method_getRuntime == NULL)
 {
 LOGE("Unable to find VMRuntime methods\n");
 return -1;
 }

 jobject instance = env->CallStaticObjectMethod(JniConstants::vmRuntimeClass, method_getRuntime);
 if (instance == NULL) {
 LOGE("Unable to obtain VMRuntime instance\n");
 return -1;
 }
 gIDCache.runtimeInstance = env->NewGlobalRef(instance);

 return jniRegisterNativeMethods(env, "org/apache/harmony/luni/platform/OSMemory",
 gMethods, NELEM(gMethods));
 }
 */
int register_org_apache_harmony_luni_platform_OSMemory(JNIEnv* env) {
	return JNI_OK;
}

