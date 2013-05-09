/* System.c -- native code for java.lang.System
 Copyright (C) 1998, 1999, 2000, 2002, 2004 Free Software Foundation, Inc.

 This file is part of GNU Classpath.

 GNU Classpath is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2, or (at your option)
 any later version.
 
 GNU Classpath is distributed in the hope that it will be useful, but
 WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with GNU Classpath; see the file COPYING.  If not, write to the
 Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 02110-1301 USA.

 Linking this library statically or dynamically with other modules is
 making a combined work based on this library.  Thus, the terms and
 conditions of the GNU General Public License cover the whole
 combination.

 As a special exception, the copyright holders of this library give you
 permission to link this library with independent modules to produce an
 executable, regardless of the license terms of these independent
 modules, and to copy and distribute the resulting executable under
 terms of your choice, provided that you also meet, for each linked
 independent module, the terms and conditions of the license of that
 module.  An independent module is a module which is not derived from
 or based on this library.  If you modify this library, you may extend
 this exception to your version of the library, but you are not
 obligated to do so.  If you do not wish to do so, delete this
 exception statement from your version. */

#include "java_lang_VMSystem.h"

#include "xi/xi_log.h"
#include "xi/xi_mem.h"
#include "xi/xi_clock.h"
#include "xi/xi_env.h"

/* the begin by jshwang - moved from jcl.c */
static void JCL_ThrowException(JNIEnv * env, const char *className,
		const char *errMsg) {
	jclass excClass;
	if ((*env)->ExceptionOccurred(env)) {
		(*env)->ExceptionClear(env);
	}
	excClass = (*env)->FindClass(env, className);
	if (excClass == NULL) {
		jclass errExcClass;
		errExcClass
				= (*env)->FindClass(env, "java/lang/ClassNotFoundException");
		if (errExcClass == NULL) {
			errExcClass = (*env)->FindClass(env, "java/lang/InternalError");
			if (errExcClass == NULL) {
				log_error(XDLOG, "JCL: Utterly failed to throw exeption %s with message %s\n", className, errMsg);
				return;
			}
		}
		/* Removed this (more comprehensive) error string to avoid the need for
		 * a static variable or allocation of a buffer for this message in this
		 * (unlikely) error case. --Fridi.
		 *
		 * sprintf(errstr,"JCL: Failed to throw exception %s with message %s: could not find exception class.", className, errMsg);
		 */
		(*env)->ThrowNew(env, errExcClass, className);
	}
	(*env)->ThrowNew(env, excClass, errMsg);
}

static jclass JCL_FindClass(JNIEnv * env, const char *className) {
	jclass retval = (*env)->FindClass(env, className);
	if (retval == NULL) {
		JCL_ThrowException(env, "java/lang/ClassNotFoundException", className);
	}
	return retval;
}

static const char * JCL_jstring_to_cstring(JNIEnv * env, jstring s) {
	const char *cstr;
	if (s == NULL) {
		JCL_ThrowException(env, "java/lang/NullPointerException", "Null string");
		return NULL;
	}
	cstr = (const char *) (*env)->GetStringUTFChars(env, s, NULL);
	if (cstr == NULL) {
		JCL_ThrowException(env, "java/lang/InternalError",
				"GetStringUTFChars() failed.");
		return NULL;
	}
	return cstr;
}

static void JCL_free_cstring(JNIEnv * env, jstring s, const char *cstr) {
	(*env)->ReleaseStringUTFChars(env, s, cstr);
}

/*
 * Class:     java_lang_VMSystem
 * Method:    setIn0
 * Signature: (Ljava/io/InputStream;)V
 */
JNIEXPORT void JNICALL
Java_java_lang_VMSystem_setIn(JNIEnv * env, jclass thisClass __attribute__ ((__unused__)), jobject obj) {
	jclass cls;
	jfieldID field;

	cls = JCL_FindClass(env, "java/lang/System");
	if (!cls)
		return;

	field = (*env)->GetStaticFieldID(env, cls, "in", "Ljava/io/InputStream;");
	if (!field)
		return;
	(*env)->SetStaticObjectField(env, cls, field, obj);
}

/*
 * Class:     java_lang_VMSystem
 * Method:    setOut0
 * Signature: (Ljava/io/PrintStream;)V
 */
JNIEXPORT void JNICALL
Java_java_lang_VMSystem_setOut(JNIEnv * env, jclass thisClass __attribute__ ((__unused__)), jobject obj) {
	jclass cls;
	jfieldID field;

	cls = JCL_FindClass(env, "java/lang/System");
	if (!cls)
		return;

	field = (*env)->GetStaticFieldID(env, cls, "out", "Ljava/io/PrintStream;");
	if (!field)
		return;
	(*env)->SetStaticObjectField(env, cls, field, obj);
}

/*
 * Class:     java_lang_VMSystem
 * Method:    setErr0
 * Signature: (Ljava/io/PrintStream;)V
 */
JNIEXPORT void JNICALL
Java_java_lang_VMSystem_setErr(JNIEnv * env, jclass thisClass __attribute__ ((__unused__)), jobject obj) {
	jclass cls;
	jfieldID field;

	cls = JCL_FindClass(env, "java/lang/System");
	if (!cls)
		return;

	field = (*env)->GetStaticFieldID(env, cls, "err", "Ljava/io/PrintStream;");
	if (!field)
		return;
	(*env)->SetStaticObjectField(env, cls, field, obj);
}

/*
 * Class:     java_lang_VMSystem
 * Method:    nanoTime
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL
Java_java_lang_VMSystem_nanoTime(JNIEnv * env __attribute__ ((__unused__)), jclass thisClass __attribute__ ((__unused__))) {
	/* Note: this implementation copied directly from Japhar's, by Chris Toshok. */
	jlong result;

	result = xi_clock_msec();
	result *= (jlong) 1000000L;

	return result;
}

JNIEXPORT jstring JNICALL
Java_java_lang_VMSystem_getenv(JNIEnv * env, jclass klass __attribute__ ((__unused__)), jstring jname) {
	const xchar *cname;
	xchar envname[1024];
	xint32 ret;

	cname = JCL_jstring_to_cstring(env, jname);
	if (cname == NULL) {
		return NULL;
	}

	xi_mem_set(envname, 0, sizeof(envname));
	ret = xi_env_get(cname, envname, sizeof(envname));
	if (ret == 0) {
		return NULL;
	}

	JCL_free_cstring(env, jname, cname);
	return (*env)->NewStringUTF(env, envname);
}

JNIEXPORT jobject JNICALL
Java_java_lang_VMSystem_environ(JNIEnv *env, jclass klass __attribute__((__unused__))) {
	jobject variables;
	jclass list_class;
	jmethodID list_constructor;
	jmethodID add;

	xchar *env_pointer[128];
	xint32 ret, i;

	list_class = (*env)->FindClass(env, "java/util/LinkedList");
	if (list_class == NULL) {
		return NULL;
	}
	list_constructor = (*env)->GetMethodID(env, list_class, "<init>", "()V");
	if (list_constructor == NULL) {
		return NULL;
	}
	variables = (*env)->NewObject(env, list_class, list_constructor);
	if (variables == NULL) {
		return NULL;
	}
	add = (*env)->GetMethodID(env, list_class, "add", "(Ljava/lang/Object;)Z");
	if (add == NULL) {
		return NULL;
	}

	ret = xi_env_all(env_pointer, 128);
	for (i = 0; i < ret; i++) {
		jstring string = (*env)->NewStringUTF(env, env_pointer[i]);
		(*env)->CallBooleanMethod(env, variables, add, string);
	}

	return variables;
}
