/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class java_io_File */

#ifndef _Included_java_io_File
#define _Included_java_io_File
#ifdef __cplusplus
extern "C" {
#endif
#undef java_io_File_serialVersionUID
#define java_io_File_serialVersionUID 301077366599181567LL
/*
 * Class:     java_io_File
 * Method:    canExecuteImpl
 * Signature: (Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_java_io_File_canExecuteImpl
  (JNIEnv *, jclass, jstring);

/*
 * Class:     java_io_File
 * Method:    canReadImpl
 * Signature: (Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_java_io_File_canReadImpl
  (JNIEnv *, jclass, jstring);

/*
 * Class:     java_io_File
 * Method:    canWriteImpl
 * Signature: (Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_java_io_File_canWriteImpl
  (JNIEnv *, jclass, jstring);

/*
 * Class:     java_io_File
 * Method:    deleteImpl
 * Signature: (Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_java_io_File_deleteImpl
  (JNIEnv *, jclass, jstring);

/*
 * Class:     java_io_File
 * Method:    existsImpl
 * Signature: (Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_java_io_File_existsImpl
  (JNIEnv *, jclass, jstring);

/*
 * Class:     java_io_File
 * Method:    readlink
 * Signature: (Ljava/lang/String;)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_java_io_File_readlink
  (JNIEnv *, jclass, jstring);

/*
 * Class:     java_io_File
 * Method:    isDirectoryImpl
 * Signature: (Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_java_io_File_isDirectoryImpl
  (JNIEnv *, jclass, jstring);

/*
 * Class:     java_io_File
 * Method:    isFileImpl
 * Signature: (Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_java_io_File_isFileImpl
  (JNIEnv *, jclass, jstring);

/*
 * Class:     java_io_File
 * Method:    lastModifiedImpl
 * Signature: (Ljava/lang/String;)J
 */
JNIEXPORT jlong JNICALL Java_java_io_File_lastModifiedImpl
  (JNIEnv *, jclass, jstring);

/*
 * Class:     java_io_File
 * Method:    setLastModifiedImpl
 * Signature: (Ljava/lang/String;J)Z
 */
JNIEXPORT jboolean JNICALL Java_java_io_File_setLastModifiedImpl
  (JNIEnv *, jclass, jstring, jlong);

/*
 * Class:     java_io_File
 * Method:    setExecutableImpl
 * Signature: (Ljava/lang/String;ZZ)Z
 */
JNIEXPORT jboolean JNICALL Java_java_io_File_setExecutableImpl
  (JNIEnv *, jclass, jstring, jboolean, jboolean);

/*
 * Class:     java_io_File
 * Method:    setReadableImpl
 * Signature: (Ljava/lang/String;ZZ)Z
 */
JNIEXPORT jboolean JNICALL Java_java_io_File_setReadableImpl
  (JNIEnv *, jclass, jstring, jboolean, jboolean);

/*
 * Class:     java_io_File
 * Method:    setWritableImpl
 * Signature: (Ljava/lang/String;ZZ)Z
 */
JNIEXPORT jboolean JNICALL Java_java_io_File_setWritableImpl
  (JNIEnv *, jclass, jstring, jboolean, jboolean);

/*
 * Class:     java_io_File
 * Method:    lengthImpl
 * Signature: (Ljava/lang/String;)J
 */
JNIEXPORT jlong JNICALL Java_java_io_File_lengthImpl
  (JNIEnv *, jclass, jstring);

/*
 * Class:     java_io_File
 * Method:    listImpl
 * Signature: (Ljava/lang/String;)[Ljava/lang/String;
 */
JNIEXPORT jobjectArray JNICALL Java_java_io_File_listImpl
  (JNIEnv *, jclass, jstring);

/*
 * Class:     java_io_File
 * Method:    mkdirImpl
 * Signature: (Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_java_io_File_mkdirImpl
  (JNIEnv *, jclass, jstring);

/*
 * Class:     java_io_File
 * Method:    createNewFileImpl
 * Signature: (Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_java_io_File_createNewFileImpl
  (JNIEnv *, jclass, jstring);

/*
 * Class:     java_io_File
 * Method:    renameToImpl
 * Signature: (Ljava/lang/String;Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_java_io_File_renameToImpl
  (JNIEnv *, jclass, jstring, jstring);

/*
 * Class:     java_io_File
 * Method:    getTotalSpaceImpl
 * Signature: (Ljava/lang/String;)J
 */
JNIEXPORT jlong JNICALL Java_java_io_File_getTotalSpaceImpl
  (JNIEnv *, jclass, jstring);

/*
 * Class:     java_io_File
 * Method:    getUsableSpaceImpl
 * Signature: (Ljava/lang/String;)J
 */
JNIEXPORT jlong JNICALL Java_java_io_File_getUsableSpaceImpl
  (JNIEnv *, jclass, jstring);

/*
 * Class:     java_io_File
 * Method:    getFreeSpaceImpl
 * Signature: (Ljava/lang/String;)J
 */
JNIEXPORT jlong JNICALL Java_java_io_File_getFreeSpaceImpl
  (JNIEnv *, jclass, jstring);

#ifdef __cplusplus
}
#endif
#endif
