/*
 * Copyright 2013 Cheolmin Jo (webos21@gmail.com)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * File : tc_pre.c
 */

#include "xi/xi_log.h"

int tc_pre() {
	log_print(XDLOG, "IF THIS MESSAGE IS SHOWING, THEN PRINTING IS OK!!!!!!\n\n");
	log_print(XDLOG, "============ fixed size type ===========\n");
	log_print(XDLOG, "sizeof(xbool)    = %d\n", sizeof(xbool));
	log_print(XDLOG, "sizeof(xint8)    = %d\n", sizeof(xint8));
	log_print(XDLOG, "sizeof(xint16)   = %d\n", sizeof(xint16));
	log_print(XDLOG, "sizeof(xint32)   = %d\n", sizeof(xint32));
	log_print(XDLOG, "sizeof(xint64)   = %d\n", sizeof(xint64));
	log_print(XDLOG, "sizeof(xfloat32) = %d\n", sizeof(xfloat32));
	log_print(XDLOG, "sizeof(xfloat64) = %d\n", sizeof(xfloat64));
	log_print(XDLOG, "\n========== variable size type ==========\n");
	log_print(XDLOG, "sizeof(xlong)    = %d\n", sizeof(xlong));
	log_print(XDLOG, "sizeof(xssize)   = %d\n", sizeof(xssize));
	log_print(XDLOG, "sizeof(xintptr)  = %d\n", sizeof(xintptr));
	log_print(XDLOG, "sizeof(xvoid *)  = %d\n", sizeof(xvoid *));
	return 0;
}
