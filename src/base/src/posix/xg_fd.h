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
 * File   : xg_fd.h
 */

#ifndef _XG_FD_H_
#define _XG_FD_H_

#include "xi/xi_file.h"
#include "xi/xi_socket.h"

typedef enum _e_fd_type {
	XG_FD_TYPE_NOTUSED = 0,
	XG_FD_TYPE_FILE    = 1,
	XG_FD_TYPE_SOCK    = 2,
	XG_FD_TYPE_PIPE    = 3
} xg_fd_type_e;

typedef struct _st_fd {
	xg_fd_type_e          type;
	union _u_desc {
		struct _st_file {
			xi_file_mode_e    mode;
			xi_file_perm_e    perm;
			xchar            *path;
		} f;
		struct _st_sock {
			xi_sock_family_e  family;
			xi_sock_type_e    type;
			xi_sock_proto_e   proto;
		} s;
	} desc;
} xg_fd_t;

_XI_API_INTERN xint32   xg_fd_count();
_XI_API_INTERN xg_fd_t *xg_fd_get(xint32 idx);

_XI_API_INTERN xint32   xg_fd_open(xint32 idx, xg_fd_t *fd);
_XI_API_INTERN xint32   xg_fd_close(xint32 idx);

#endif //_XG_FD_H_
