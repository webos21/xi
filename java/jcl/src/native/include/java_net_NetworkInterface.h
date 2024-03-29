/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class java_net_NetworkInterface */

#ifndef _Included_java_net_NetworkInterface
#define _Included_java_net_NetworkInterface
#ifdef __cplusplus
extern "C" {
#endif
#undef java_net_NetworkInterface_CHECK_CONNECT_NO_PORT
#define java_net_NetworkInterface_CHECK_CONNECT_NO_PORT -1L
#undef java_net_NetworkInterface_NO_INTERFACE_INDEX
#define java_net_NetworkInterface_NO_INTERFACE_INDEX 0L
#undef java_net_NetworkInterface_UNSET_INTERFACE_INDEX
#define java_net_NetworkInterface_UNSET_INTERFACE_INDEX -1L
/*
 * Class:     java_net_NetworkInterface
 * Method:    getAllInterfaceAddressesImpl
 * Signature: ()[Ljava/net/InterfaceAddress;
 */
JNIEXPORT jobjectArray JNICALL Java_java_net_NetworkInterface_getAllInterfaceAddressesImpl
  (JNIEnv *, jclass);

/*
 * Class:     java_net_NetworkInterface
 * Method:    isUpImpl
 * Signature: (Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_java_net_NetworkInterface_isUpImpl
  (JNIEnv *, jclass, jstring);

/*
 * Class:     java_net_NetworkInterface
 * Method:    isLoopbackImpl
 * Signature: (Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_java_net_NetworkInterface_isLoopbackImpl
  (JNIEnv *, jclass, jstring);

/*
 * Class:     java_net_NetworkInterface
 * Method:    isPointToPointImpl
 * Signature: (Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_java_net_NetworkInterface_isPointToPointImpl
  (JNIEnv *, jclass, jstring);

/*
 * Class:     java_net_NetworkInterface
 * Method:    supportsMulticastImpl
 * Signature: (Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_java_net_NetworkInterface_supportsMulticastImpl
  (JNIEnv *, jclass, jstring);

/*
 * Class:     java_net_NetworkInterface
 * Method:    getHardwareAddressImpl
 * Signature: (Ljava/lang/String;)[B
 */
JNIEXPORT jbyteArray JNICALL Java_java_net_NetworkInterface_getHardwareAddressImpl
  (JNIEnv *, jclass, jstring);

/*
 * Class:     java_net_NetworkInterface
 * Method:    getMTUImpl
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_java_net_NetworkInterface_getMTUImpl
  (JNIEnv *, jclass, jstring);

#ifdef __cplusplus
}
#endif
#endif
