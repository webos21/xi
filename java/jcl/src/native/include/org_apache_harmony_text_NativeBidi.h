/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class org_apache_harmony_text_NativeBidi */

#ifndef _Included_org_apache_harmony_text_NativeBidi
#define _Included_org_apache_harmony_text_NativeBidi
#ifdef __cplusplus
extern "C" {
#endif
#undef org_apache_harmony_text_NativeBidi_UBIDI_DEFAULT_LTR
#define org_apache_harmony_text_NativeBidi_UBIDI_DEFAULT_LTR 254L
#undef org_apache_harmony_text_NativeBidi_UBIDI_DEFAULT_RTL
#define org_apache_harmony_text_NativeBidi_UBIDI_DEFAULT_RTL 255L
#undef org_apache_harmony_text_NativeBidi_UBIDI_MAX_EXPLICIT_LEVEL
#define org_apache_harmony_text_NativeBidi_UBIDI_MAX_EXPLICIT_LEVEL 61L
#undef org_apache_harmony_text_NativeBidi_UBIDI_LEVEL_OVERRIDE
#define org_apache_harmony_text_NativeBidi_UBIDI_LEVEL_OVERRIDE 128L
#undef org_apache_harmony_text_NativeBidi_UBIDI_KEEP_BASE_COMBINING
#define org_apache_harmony_text_NativeBidi_UBIDI_KEEP_BASE_COMBINING 1L
#undef org_apache_harmony_text_NativeBidi_UBIDI_DO_MIRRORING
#define org_apache_harmony_text_NativeBidi_UBIDI_DO_MIRRORING 2L
#undef org_apache_harmony_text_NativeBidi_UBIDI_INSERT_LRM_FOR_NUMERIC
#define org_apache_harmony_text_NativeBidi_UBIDI_INSERT_LRM_FOR_NUMERIC 4L
#undef org_apache_harmony_text_NativeBidi_UBIDI_REMOVE_BIDI_CONTROLS
#define org_apache_harmony_text_NativeBidi_UBIDI_REMOVE_BIDI_CONTROLS 8L
#undef org_apache_harmony_text_NativeBidi_UBIDI_OUTPUT_REVERSE
#define org_apache_harmony_text_NativeBidi_UBIDI_OUTPUT_REVERSE 16L
#undef org_apache_harmony_text_NativeBidi_UBiDiDirection_UBIDI_LTR
#define org_apache_harmony_text_NativeBidi_UBiDiDirection_UBIDI_LTR 0L
#undef org_apache_harmony_text_NativeBidi_UBiDiDirection_UBIDI_RTL
#define org_apache_harmony_text_NativeBidi_UBiDiDirection_UBIDI_RTL 1L
#undef org_apache_harmony_text_NativeBidi_UBiDiDirection_UBIDI_MIXED
#define org_apache_harmony_text_NativeBidi_UBiDiDirection_UBIDI_MIXED 2L
/*
 * Class:     org_apache_harmony_text_NativeBidi
 * Method:    ubidi_open
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL Java_org_apache_harmony_text_NativeBidi_ubidi_1open
  (JNIEnv *, jclass);

/*
 * Class:     org_apache_harmony_text_NativeBidi
 * Method:    ubidi_close
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_org_apache_harmony_text_NativeBidi_ubidi_1close
  (JNIEnv *, jclass, jlong);

/*
 * Class:     org_apache_harmony_text_NativeBidi
 * Method:    ubidi_setPara
 * Signature: (J[CII[B)V
 */
JNIEXPORT void JNICALL Java_org_apache_harmony_text_NativeBidi_ubidi_1setPara
  (JNIEnv *, jclass, jlong, jcharArray, jint, jint, jbyteArray);

/*
 * Class:     org_apache_harmony_text_NativeBidi
 * Method:    ubidi_setLine
 * Signature: (JII)J
 */
JNIEXPORT jlong JNICALL Java_org_apache_harmony_text_NativeBidi_ubidi_1setLine
  (JNIEnv *, jclass, jlong, jint, jint);

/*
 * Class:     org_apache_harmony_text_NativeBidi
 * Method:    ubidi_getDirection
 * Signature: (J)I
 */
JNIEXPORT jint JNICALL Java_org_apache_harmony_text_NativeBidi_ubidi_1getDirection
  (JNIEnv *, jclass, jlong);

/*
 * Class:     org_apache_harmony_text_NativeBidi
 * Method:    ubidi_getLength
 * Signature: (J)I
 */
JNIEXPORT jint JNICALL Java_org_apache_harmony_text_NativeBidi_ubidi_1getLength
  (JNIEnv *, jclass, jlong);

/*
 * Class:     org_apache_harmony_text_NativeBidi
 * Method:    ubidi_getParaLevel
 * Signature: (J)B
 */
JNIEXPORT jbyte JNICALL Java_org_apache_harmony_text_NativeBidi_ubidi_1getParaLevel
  (JNIEnv *, jclass, jlong);

/*
 * Class:     org_apache_harmony_text_NativeBidi
 * Method:    ubidi_getLevels
 * Signature: (J)[B
 */
JNIEXPORT jbyteArray JNICALL Java_org_apache_harmony_text_NativeBidi_ubidi_1getLevels
  (JNIEnv *, jclass, jlong);

/*
 * Class:     org_apache_harmony_text_NativeBidi
 * Method:    ubidi_countRuns
 * Signature: (J)I
 */
JNIEXPORT jint JNICALL Java_org_apache_harmony_text_NativeBidi_ubidi_1countRuns
  (JNIEnv *, jclass, jlong);

/*
 * Class:     org_apache_harmony_text_NativeBidi
 * Method:    ubidi_getRuns
 * Signature: (J)[Lorg/apache/harmony/text/BidiRun;
 */
JNIEXPORT jobjectArray JNICALL Java_org_apache_harmony_text_NativeBidi_ubidi_1getRuns
  (JNIEnv *, jclass, jlong);

/*
 * Class:     org_apache_harmony_text_NativeBidi
 * Method:    ubidi_reorderVisual
 * Signature: ([BI)[I
 */
JNIEXPORT jintArray JNICALL Java_org_apache_harmony_text_NativeBidi_ubidi_1reorderVisual
  (JNIEnv *, jclass, jbyteArray, jint);

#ifdef __cplusplus
}
#endif
#endif
