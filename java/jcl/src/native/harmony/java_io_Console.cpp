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

//#define LOG_TAG "Console"

#include "java_io_Console.h"
#include "JNIHelp.h"
#include "JniConstants.h"

#include "xi/xi_log.h"

#if 0 // by cmjo
#include <errno.h>
#include <termios.h>
#include <unistd.h>
#endif //0

//static jboolean Console_isatty(JNIEnv*, jclass, jint fd) {
JNIEXPORT jboolean JNICALL
Java_java_io_Console_isatty(JNIEnv*, jclass, jint fd) {
	if (fd == 0 || fd == 1) {
		return JNI_TRUE;
	}
	return JNI_FALSE;
    //return TEMP_FAILURE_RETRY(isatty(fd));
}

//static jint Console_setEchoImpl(JNIEnv* env, jclass, jboolean on, jint previousState) {
JNIEXPORT jint JNICALL
Java_java_io_Console_setEchoImpl(JNIEnv*, jclass, jboolean on, jint previousState) {
	static xint32 lflag = 0;
//    termios state;
//    if (TEMP_FAILURE_RETRY(tcgetattr(STDIN_FILENO, &state)) == -1) {
//        jniThrowIOException(env, errno);
//        return 0;
//    }
    if (on) {
        lflag = previousState;
    } else {
        previousState = lflag;
        lflag = 0;
    }
//    if (TEMP_FAILURE_RETRY(tcsetattr(STDIN_FILENO, TCSAFLUSH, &state)) == -1){
//        jniThrowIOException(env, errno);
//        return 0;
//    }
    return previousState;
}

/* by jshwang
static JNINativeMethod gMethods[] = {
    NATIVE_METHOD(Console, isatty, "(I)Z"),
    NATIVE_METHOD(Console, setEchoImpl, "(ZI)I"),
};
int register_java_io_Console(JNIEnv* env) {
    return jniRegisterNativeMethods(env, "java/io/Console", gMethods, NELEM(gMethods));
}
*/
int register_java_io_Console(JNIEnv*) {
    return JNI_OK;
}

