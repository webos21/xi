/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class java_util_zip_Deflater */

#ifndef _Included_java_util_zip_Deflater
#define _Included_java_util_zip_Deflater
#ifdef __cplusplus
extern "C" {
#endif
#undef java_util_zip_Deflater_BEST_COMPRESSION
#define java_util_zip_Deflater_BEST_COMPRESSION 9L
#undef java_util_zip_Deflater_BEST_SPEED
#define java_util_zip_Deflater_BEST_SPEED 1L
#undef java_util_zip_Deflater_DEFAULT_COMPRESSION
#define java_util_zip_Deflater_DEFAULT_COMPRESSION -1L
#undef java_util_zip_Deflater_DEFAULT_STRATEGY
#define java_util_zip_Deflater_DEFAULT_STRATEGY 0L
#undef java_util_zip_Deflater_DEFLATED
#define java_util_zip_Deflater_DEFLATED 8L
#undef java_util_zip_Deflater_FILTERED
#define java_util_zip_Deflater_FILTERED 1L
#undef java_util_zip_Deflater_HUFFMAN_ONLY
#define java_util_zip_Deflater_HUFFMAN_ONLY 2L
#undef java_util_zip_Deflater_NO_COMPRESSION
#define java_util_zip_Deflater_NO_COMPRESSION 0L
#undef java_util_zip_Deflater_NO_FLUSH
#define java_util_zip_Deflater_NO_FLUSH 0L
#undef java_util_zip_Deflater_SYNC_FLUSH
#define java_util_zip_Deflater_SYNC_FLUSH 2L
#undef java_util_zip_Deflater_FULL_FLUSH
#define java_util_zip_Deflater_FULL_FLUSH 3L
#undef java_util_zip_Deflater_FINISH
#define java_util_zip_Deflater_FINISH 4L
/*
 * Class:     java_util_zip_Deflater
 * Method:    deflateImpl
 * Signature: ([BIIJI)I
 */
JNIEXPORT jint JNICALL Java_java_util_zip_Deflater_deflateImpl
  (JNIEnv *, jobject, jbyteArray, jint, jint, jlong, jint);

/*
 * Class:     java_util_zip_Deflater
 * Method:    endImpl
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_java_util_zip_Deflater_endImpl
  (JNIEnv *, jobject, jlong);

/*
 * Class:     java_util_zip_Deflater
 * Method:    getAdlerImpl
 * Signature: (J)I
 */
JNIEXPORT jint JNICALL Java_java_util_zip_Deflater_getAdlerImpl
  (JNIEnv *, jobject, jlong);

/*
 * Class:     java_util_zip_Deflater
 * Method:    getTotalInImpl
 * Signature: (J)J
 */
JNIEXPORT jlong JNICALL Java_java_util_zip_Deflater_getTotalInImpl
  (JNIEnv *, jobject, jlong);

/*
 * Class:     java_util_zip_Deflater
 * Method:    getTotalOutImpl
 * Signature: (J)J
 */
JNIEXPORT jlong JNICALL Java_java_util_zip_Deflater_getTotalOutImpl
  (JNIEnv *, jobject, jlong);

/*
 * Class:     java_util_zip_Deflater
 * Method:    resetImpl
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_java_util_zip_Deflater_resetImpl
  (JNIEnv *, jobject, jlong);

/*
 * Class:     java_util_zip_Deflater
 * Method:    setDictionaryImpl
 * Signature: ([BIIJ)V
 */
JNIEXPORT void JNICALL Java_java_util_zip_Deflater_setDictionaryImpl
  (JNIEnv *, jobject, jbyteArray, jint, jint, jlong);

/*
 * Class:     java_util_zip_Deflater
 * Method:    setLevelsImpl
 * Signature: (IIJ)V
 */
JNIEXPORT void JNICALL Java_java_util_zip_Deflater_setLevelsImpl
  (JNIEnv *, jobject, jint, jint, jlong);

/*
 * Class:     java_util_zip_Deflater
 * Method:    setInputImpl
 * Signature: ([BIIJ)V
 */
JNIEXPORT void JNICALL Java_java_util_zip_Deflater_setInputImpl
  (JNIEnv *, jobject, jbyteArray, jint, jint, jlong);

/*
 * Class:     java_util_zip_Deflater
 * Method:    createStream
 * Signature: (IIZ)J
 */
JNIEXPORT jlong JNICALL Java_java_util_zip_Deflater_createStream
  (JNIEnv *, jobject, jint, jint, jboolean);

#ifdef __cplusplus
}
#endif
#endif
