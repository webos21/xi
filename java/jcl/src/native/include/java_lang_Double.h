/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class java_lang_Double */

#ifndef _Included_java_lang_Double
#define _Included_java_lang_Double
#ifdef __cplusplus
extern "C" {
#endif
#undef java_lang_Double_serialVersionUID
#define java_lang_Double_serialVersionUID -8742448824652078965LL
#undef java_lang_Double_EXPONENT_BIAS
#define java_lang_Double_EXPONENT_BIAS 1023L
#undef java_lang_Double_EXPONENT_BITS
#define java_lang_Double_EXPONENT_BITS 12L
#undef java_lang_Double_MANTISSA_BITS
#define java_lang_Double_MANTISSA_BITS 52L
#undef java_lang_Double_NON_MANTISSA_BITS
#define java_lang_Double_NON_MANTISSA_BITS 12L
#undef java_lang_Double_SIGN_MASK
#define java_lang_Double_SIGN_MASK -9223372036854775808LL
#undef java_lang_Double_EXPONENT_MASK
#define java_lang_Double_EXPONENT_MASK 9218868437227405312LL
#undef java_lang_Double_MANTISSA_MASK
#define java_lang_Double_MANTISSA_MASK 4503599627370495LL
#undef java_lang_Double_serialVersionUID
#define java_lang_Double_serialVersionUID -9172774392245257468LL
#undef java_lang_Double_MAX_VALUE
#define java_lang_Double_MAX_VALUE 1.7976931348623157E308
#undef java_lang_Double_MIN_VALUE
#define java_lang_Double_MIN_VALUE 4.9E-324
#undef java_lang_Double_NaN
#define java_lang_Double_NaN NaN
#undef java_lang_Double_POSITIVE_INFINITY
#define java_lang_Double_POSITIVE_INFINITY InfD
#undef java_lang_Double_NEGATIVE_INFINITY
#define java_lang_Double_NEGATIVE_INFINITY -InfD
#undef java_lang_Double_MIN_NORMAL
#define java_lang_Double_MIN_NORMAL 2.2250738585072014E-308
#undef java_lang_Double_MAX_EXPONENT
#define java_lang_Double_MAX_EXPONENT 1023L
#undef java_lang_Double_MIN_EXPONENT
#define java_lang_Double_MIN_EXPONENT -1022L
#undef java_lang_Double_SIZE
#define java_lang_Double_SIZE 64L
/*
 * Class:     java_lang_Double
 * Method:    doubleToLongBits
 * Signature: (D)J
 */
JNIEXPORT jlong JNICALL Java_java_lang_Double_doubleToLongBits
  (JNIEnv *, jclass, jdouble);

/*
 * Class:     java_lang_Double
 * Method:    doubleToRawLongBits
 * Signature: (D)J
 */
JNIEXPORT jlong JNICALL Java_java_lang_Double_doubleToRawLongBits
  (JNIEnv *, jclass, jdouble);

/*
 * Class:     java_lang_Double
 * Method:    longBitsToDouble
 * Signature: (J)D
 */
JNIEXPORT jdouble JNICALL Java_java_lang_Double_longBitsToDouble
  (JNIEnv *, jclass, jlong);

#ifdef __cplusplus
}
#endif
#endif
