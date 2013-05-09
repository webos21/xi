/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class com_ibm_icu4jni_util_ICU */

#ifndef _Included_com_ibm_icu4jni_util_ICU
#define _Included_com_ibm_icu4jni_util_ICU
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     com_ibm_icu4jni_util_ICU
 * Method:    toLowerCase
 * Signature: (Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_ibm_icu4jni_util_ICU_toLowerCase
  (JNIEnv *, jclass, jstring, jstring);

/*
 * Class:     com_ibm_icu4jni_util_ICU
 * Method:    toUpperCase
 * Signature: (Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_ibm_icu4jni_util_ICU_toUpperCase
  (JNIEnv *, jclass, jstring, jstring);

/*
 * Class:     com_ibm_icu4jni_util_ICU
 * Method:    getAvailableBreakIteratorLocalesNative
 * Signature: ()[Ljava/lang/String;
 */
JNIEXPORT jobjectArray JNICALL Java_com_ibm_icu4jni_util_ICU_getAvailableBreakIteratorLocalesNative
  (JNIEnv *, jclass);

/*
 * Class:     com_ibm_icu4jni_util_ICU
 * Method:    getAvailableCalendarLocalesNative
 * Signature: ()[Ljava/lang/String;
 */
JNIEXPORT jobjectArray JNICALL Java_com_ibm_icu4jni_util_ICU_getAvailableCalendarLocalesNative
  (JNIEnv *, jclass);

/*
 * Class:     com_ibm_icu4jni_util_ICU
 * Method:    getAvailableCollatorLocalesNative
 * Signature: ()[Ljava/lang/String;
 */
JNIEXPORT jobjectArray JNICALL Java_com_ibm_icu4jni_util_ICU_getAvailableCollatorLocalesNative
  (JNIEnv *, jclass);

/*
 * Class:     com_ibm_icu4jni_util_ICU
 * Method:    getAvailableDateFormatLocalesNative
 * Signature: ()[Ljava/lang/String;
 */
JNIEXPORT jobjectArray JNICALL Java_com_ibm_icu4jni_util_ICU_getAvailableDateFormatLocalesNative
  (JNIEnv *, jclass);

/*
 * Class:     com_ibm_icu4jni_util_ICU
 * Method:    getAvailableLocalesNative
 * Signature: ()[Ljava/lang/String;
 */
JNIEXPORT jobjectArray JNICALL Java_com_ibm_icu4jni_util_ICU_getAvailableLocalesNative
  (JNIEnv *, jclass);

/*
 * Class:     com_ibm_icu4jni_util_ICU
 * Method:    getAvailableNumberFormatLocalesNative
 * Signature: ()[Ljava/lang/String;
 */
JNIEXPORT jobjectArray JNICALL Java_com_ibm_icu4jni_util_ICU_getAvailableNumberFormatLocalesNative
  (JNIEnv *, jclass);

/*
 * Class:     com_ibm_icu4jni_util_ICU
 * Method:    getCurrencyCodeNative
 * Signature: (Ljava/lang/String;)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_ibm_icu4jni_util_ICU_getCurrencyCodeNative
  (JNIEnv *, jclass, jstring);

/*
 * Class:     com_ibm_icu4jni_util_ICU
 * Method:    getCurrencyFractionDigitsNative
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_com_ibm_icu4jni_util_ICU_getCurrencyFractionDigitsNative
  (JNIEnv *, jclass, jstring);

/*
 * Class:     com_ibm_icu4jni_util_ICU
 * Method:    getCurrencySymbolNative
 * Signature: (Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_ibm_icu4jni_util_ICU_getCurrencySymbolNative
  (JNIEnv *, jclass, jstring, jstring);

/*
 * Class:     com_ibm_icu4jni_util_ICU
 * Method:    getDisplayCountryNative
 * Signature: (Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_ibm_icu4jni_util_ICU_getDisplayCountryNative
  (JNIEnv *, jclass, jstring, jstring);

/*
 * Class:     com_ibm_icu4jni_util_ICU
 * Method:    getDisplayLanguageNative
 * Signature: (Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_ibm_icu4jni_util_ICU_getDisplayLanguageNative
  (JNIEnv *, jclass, jstring, jstring);

/*
 * Class:     com_ibm_icu4jni_util_ICU
 * Method:    getDisplayVariantNative
 * Signature: (Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_ibm_icu4jni_util_ICU_getDisplayVariantNative
  (JNIEnv *, jclass, jstring, jstring);

/*
 * Class:     com_ibm_icu4jni_util_ICU
 * Method:    getISO3CountryNative
 * Signature: (Ljava/lang/String;)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_ibm_icu4jni_util_ICU_getISO3CountryNative
  (JNIEnv *, jclass, jstring);

/*
 * Class:     com_ibm_icu4jni_util_ICU
 * Method:    getISO3LanguageNative
 * Signature: (Ljava/lang/String;)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_ibm_icu4jni_util_ICU_getISO3LanguageNative
  (JNIEnv *, jclass, jstring);

/*
 * Class:     com_ibm_icu4jni_util_ICU
 * Method:    getISOLanguagesNative
 * Signature: ()[Ljava/lang/String;
 */
JNIEXPORT jobjectArray JNICALL Java_com_ibm_icu4jni_util_ICU_getISOLanguagesNative
  (JNIEnv *, jclass);

/*
 * Class:     com_ibm_icu4jni_util_ICU
 * Method:    getISOCountriesNative
 * Signature: ()[Ljava/lang/String;
 */
JNIEXPORT jobjectArray JNICALL Java_com_ibm_icu4jni_util_ICU_getISOCountriesNative
  (JNIEnv *, jclass);

/*
 * Class:     com_ibm_icu4jni_util_ICU
 * Method:    initLocaleDataImpl
 * Signature: (Ljava/lang/String;Lcom/ibm/icu4jni/util/LocaleData;)Z
 */
JNIEXPORT jboolean JNICALL Java_com_ibm_icu4jni_util_ICU_initLocaleDataImpl
  (JNIEnv *, jclass, jstring, jobject);

#ifdef __cplusplus
}
#endif
#endif
