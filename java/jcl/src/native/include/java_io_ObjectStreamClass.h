/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class java_io_ObjectStreamClass */

#ifndef _Included_java_io_ObjectStreamClass
#define _Included_java_io_ObjectStreamClass
#ifdef __cplusplus
extern "C" {
#endif
#undef java_io_ObjectStreamClass_serialVersionUID
#define java_io_ObjectStreamClass_serialVersionUID -6120832682080437368LL
#undef java_io_ObjectStreamClass_CONSTRUCTOR_IS_NOT_RESOLVED
#define java_io_ObjectStreamClass_CONSTRUCTOR_IS_NOT_RESOLVED -1LL
#undef java_io_ObjectStreamClass_CLINIT_MODIFIERS
#define java_io_ObjectStreamClass_CLINIT_MODIFIERS 8L
/*
 * Class:     java_io_ObjectStreamClass
 * Method:    getConstructorSignature
 * Signature: (Ljava/lang/reflect/Constructor;)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_java_io_ObjectStreamClass_getConstructorSignature
  (JNIEnv *, jclass, jobject);

/*
 * Class:     java_io_ObjectStreamClass
 * Method:    getFieldSignature
 * Signature: (Ljava/lang/reflect/Field;)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_java_io_ObjectStreamClass_getFieldSignature
  (JNIEnv *, jclass, jobject);

/*
 * Class:     java_io_ObjectStreamClass
 * Method:    getMethodSignature
 * Signature: (Ljava/lang/reflect/Method;)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_java_io_ObjectStreamClass_getMethodSignature
  (JNIEnv *, jclass, jobject);

/*
 * Class:     java_io_ObjectStreamClass
 * Method:    hasClinit
 * Signature: (Ljava/lang/Class;)Z
 */
JNIEXPORT jboolean JNICALL Java_java_io_ObjectStreamClass_hasClinit
  (JNIEnv *, jclass, jclass);

#ifdef __cplusplus
}
#endif
#endif
