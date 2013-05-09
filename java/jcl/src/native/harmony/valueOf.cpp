/*
 * Copyright (C) 2010 The Android Open Source Project
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

#define LOG_TAG "valueOf"

#include "valueOf.h"
#include "JNIHelp.h"
#include "JniConstants.h"

template <typename T>
static jobject valueOf(JNIEnv* env, jclass c, const char* signature, const T& value) {
    static jmethodID valueOfMethod = env->GetStaticMethodID(c, "valueOf", signature);
    return env->CallStaticObjectMethod(c, valueOfMethod, value);
}

jobject booleanValueOf(JNIEnv* env, jboolean value) {
	jclass booleanClass = env->FindClass("java/lang/Boolean");
    return valueOf(env, booleanClass, "(Z)Ljava/lang/Boolean;", value);
}

jobject doubleValueOf(JNIEnv* env, jdouble value) {
	jclass doubleClass = env->FindClass("java/lang/Double");
    return valueOf(env, doubleClass, "(D)Ljava/lang/Double;", value);
}

jobject integerValueOf(JNIEnv* env, jint value) {
	jclass integerClass = env->FindClass("java/lang/Integer");
    return valueOf(env, integerClass, "(I)Ljava/lang/Integer;", value);
}

jobject longValueOf(JNIEnv* env, jlong value) {
	jclass longClass = env->FindClass("java/lang/Long");
    return valueOf(env, longClass, "(J)Ljava/lang/Long;", value);
}
