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
 * File : tc.h
 */

#ifndef _TC_H_
#define _TC_H_

#ifdef _WIN32
#include <windows.h>
#else
#include <stdio.h>
#endif

/**
 * Start Declaration
 */
#ifdef __cplusplus
extern "C" {
#endif

typedef enum _e_tc_rv {
	TC_RV_OK = 0, TC_RV_ERR = -1
} tcre;

///////////////////////////////////////
//        Prerequisite Test          //
///////////////////////////////////////

int tc_pre();

///////////////////////////////////////
//            XI Test-Case           //
///////////////////////////////////////

int tc_xi_arrays();
int tc_xi_base64();
int tc_xi_clock();
int tc_xi_ctype();
int tc_xi_dso();
int tc_xi_env();
int tc_xi_file_dop();
int tc_xi_file_fop();
int tc_xi_hashtb();
int tc_xi_log();
int tc_xi_mem();
int tc_xi_poll_echosrv();
int tc_xi_proc();
int tc_xi_select_echosrv();
int tc_xi_socket_basic();
int tc_xi_socket_mcast();
int tc_xi_sysinfo();
int tc_xi_thread_basic();
int tc_xi_thread_java();
int tc_xi_thread_stress();

/**
 * End Declaration
 */
#ifdef __cplusplus
}
#endif

#endif // _TC_H_
