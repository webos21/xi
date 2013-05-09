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
 * File : tc_main.h
 */

#include "tc.h"

#include <stdio.h>

#include "xi/xi_log.h"
#include "xi/xi_thread.h"

#define XI_TC_TEST(tc) do { if(tc < 0) printf("!!!!!!!!!!!!!!!!!!!!!!!\n"); } while(0)

int main(int argc, char **argv) {
	UNUSED(argc); UNUSED(argv);

	printf("\n\n");
	printf("==============================================================\n");
	printf("\n");
	printf("   #    #      #             #####  ######   ####    #####\n");
	printf("    #  #       #               #    #       #          #  \n");
	printf("     ##        #    #####      #    #####    ####      #  \n");
	printf("     ##        #               #    #            #     #  \n");
	printf("    #  #       #               #    #       #    #     #  \n");
	printf("   #    #      #               #    ######   ####      #  \n");
	printf("\n");
	printf("==============================================================\n");

	printf("\n\n");

	printf("--------------------------------------------------------------\n");
	printf("1) Prerequisite Test\n");
	printf("--------------------------------------------------------------\n");

	XI_TC_TEST(tc_pre());

	printf("\n\n");

	printf("--------------------------------------------------------------\n");
	printf("2) XI Test\n");
	printf("--------------------------------------------------------------\n");

	XI_TC_TEST(tc_xi_log());
	XI_TC_TEST(tc_xi_mem());
	XI_TC_TEST(tc_xi_hashtb());
	XI_TC_TEST(tc_xi_clock());
	XI_TC_TEST(tc_xi_thread_basic());
	XI_TC_TEST(tc_xi_thread_java());
	XI_TC_TEST(tc_xi_thread_stress());
	XI_TC_TEST(tc_xi_dso());
	XI_TC_TEST(tc_xi_file_fop());
	XI_TC_TEST(tc_xi_file_dop());
	XI_TC_TEST(tc_xi_socket_basic());
	XI_TC_TEST(tc_xi_socket_mcast());
	XI_TC_TEST(tc_xi_poll_echosrv());
	XI_TC_TEST(tc_xi_select_echosrv());
	XI_TC_TEST(tc_xi_arrays());
	XI_TC_TEST(tc_xi_base64());
	XI_TC_TEST(tc_xi_sysinfo());
	XI_TC_TEST(tc_xi_env());
	XI_TC_TEST(tc_xi_ctype());
	XI_TC_TEST(tc_xi_proc());

	printf("\n\n");

	while (1) {
		xi_thread_sleep(1000);
		printf("Test Alive!!!!\n");
	}

	return 0;
}
