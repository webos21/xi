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

#include "java_io_ObjectStreamClass.h"
#include "JNIHelp.h"
#include "JniConstants.h"

// by jshwang
//static jobject getSignature(JNIEnv* env, jclass c, jobject object) {
static jstring getSignature(JNIEnv* env, jclass c, jobject object) {
	//log_print(XDLOG, "\n    *#*#*#*#*#*#*#*#* getSignature of class: %p, field: %p *#*#*#*#*#*#*\n", c, object);
    jmethodID mid = env->GetMethodID(c, "getSignature", "()Ljava/lang/String;");
    if (!mid) {
        return NULL;
    }
    jclass objectClass = env->GetObjectClass(object);
// by jshwang
//    return env->CallNonvirtualObjectMethod(object, objectClass, mid);
	return (jstring)(env->CallNonvirtualObjectMethod(object, objectClass, mid));
}

//static jobject ObjectStreamClass_getFieldSignature(JNIEnv* env, jclass, jobject field) {
JNIEXPORT jstring JNICALL
Java_java_io_ObjectStreamClass_getFieldSignature(JNIEnv* env, jclass, jobject field) {
	jclass fieldClass = env->FindClass("java/lang/reflect/Field");
	return getSignature(env, fieldClass, field);
}

//static jobject ObjectStreamClass_getMethodSignature(JNIEnv* env, jclass, jobject method) {
JNIEXPORT jstring JNICALL
Java_java_io_ObjectStreamClass_getMethodSignature(JNIEnv* env, jclass, jobject method) {
	jclass methodClass = env->FindClass("java/lang/reflect/Method");
    return getSignature(env, methodClass, method);
}

//static jobject ObjectStreamClass_getConstructorSignature(JNIEnv* env, jclass, jobject constructor) {
JNIEXPORT jstring JNICALL
Java_java_io_ObjectStreamClass_getConstructorSignature(JNIEnv* env, jclass, jobject constructor) {
	jclass constructorClass = env->FindClass("java/lang/reflect/Constructor");
    return getSignature(env, constructorClass, constructor);
}

//static jboolean ObjectStreamClass_hasClinit(JNIEnv * env, jclass, jclass targetClass) {
JNIEXPORT jboolean JNICALL
Java_java_io_ObjectStreamClass_hasClinit(JNIEnv * env, jclass, jclass targetClass) {
    jmethodID mid = env->GetStaticMethodID(targetClass, "<clinit>", "()V");
    env->ExceptionClear();
    return (mid != 0);
}

/* by jshwang
static JNINativeMethod gMethods[] = {
    NATIVE_METHOD(ObjectStreamClass, getConstructorSignature, "(Ljava/lang/reflect/Constructor;)Ljava/lang/String;"),
    NATIVE_METHOD(ObjectStreamClass, getFieldSignature, "(Ljava/lang/reflect/Field;)Ljava/lang/String;"),
    NATIVE_METHOD(ObjectStreamClass, getMethodSignature, "(Ljava/lang/reflect/Method;)Ljava/lang/String;"),
    NATIVE_METHOD(ObjectStreamClass, hasClinit, "(Ljava/lang/Class;)Z"),
};
int register_java_io_ObjectStreamClass(JNIEnv* env) {
    return jniRegisterNativeMethods(env, "java/io/ObjectStreamClass", gMethods, NELEM(gMethods));
}
*/
int register_java_io_ObjectStreamClass(JNIEnv*) {
    return JNI_OK;
}

