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
 * File : xg_process.c
 */

#include <windows.h>
#include <process.h>

#include "xi/xi_process.h"

#include "xi/xi_mem.h"
#include "xi/xi_string.h"
#include "xi/xi_file.h"
#include "xi/xi_log.h"

// ----------------------------------------------
// Inner Structures
// ----------------------------------------------

struct _xi_proc_mutex {
	xchar filepath[XCFG_PATHNAME_MAX];
	xvoid *lock;
};

// ----------------------------------------------
// XG Functions
// ----------------------------------------------

xvoid xg_proc_mutex_mkname(xchar *dest, const xchar *src) {
	xssize len = xi_strlen(src);
	xint32 i, cnt;

	for (i = 0, cnt = 0; i < len; i++) {
		if ((src[i] >= 'A' && src[i] <= 'Z')
				|| (src[i] >= 'a' && src[i] <= 'z') || (src[i] >= '0' && src[i]
				<= '9')) {
			dest[cnt] = src[i];
			cnt++;
		}
	}
}

// ----------------------------------------------
// XI Functions
// ----------------------------------------------

xi_proc_mutex_re xi_proc_mutex_create(xi_proc_mutex_t **lock,
		const xchar *filepath) {
	xi_proc_mutex_t *mutex = xi_mem_calloc(1, sizeof(xi_proc_mutex_t));
	if (mutex == NULL) {
		return XI_PROC_MUTEX_RV_ERR_ALLOC;
	}

	xg_proc_mutex_mkname(mutex->filepath, filepath);

	mutex->lock = CreateMutex(NULL, FALSE, mutex->filepath);
	if (mutex->lock == NULL) {
		xi_mem_free(mutex);
		switch (GetLastError()) {
		case ERROR_ALREADY_EXISTS:
			return XI_PROC_MUTEX_RV_ERR_EXIST;
		default:
			return XI_PROC_MUTEX_RV_ERR_CREATE;
		}
	}

	*lock = mutex;

	return XI_PROC_MUTEX_RV_OK;
}

xi_proc_mutex_re xi_proc_mutex_open(xi_proc_mutex_t **lock,
		const xchar *filepath) {
	xi_proc_mutex_t *mutex = xi_mem_calloc(1, sizeof(xi_proc_mutex_t));
	if (mutex == NULL) {
		return XI_PROC_MUTEX_RV_ERR_ALLOC;
	}

	xg_proc_mutex_mkname(mutex->filepath, filepath);

	mutex->lock = OpenMutex(MUTEX_ALL_ACCESS, FALSE, mutex->filepath);
	if (mutex->lock == NULL) {
		xi_mem_free(mutex);
		return XI_PROC_MUTEX_RV_ERR_ARGS;
	}

	*lock = mutex;

	return XI_PROC_MUTEX_RV_OK;
}

xi_proc_mutex_re xi_proc_mutex_lock(xi_proc_mutex_t *lock) {
	DWORD ret;

	if (lock == NULL) {
		return XI_PROC_MUTEX_RV_ERR_ARGS;
	}

	ret = WaitForSingleObject(lock->lock, INFINITE);
	if (ret == WAIT_OBJECT_0 || ret == WAIT_ABANDONED) {
		return XI_PROC_MUTEX_RV_OK;
	}

	return XI_PROC_MUTEX_RV_ERR_LOCK;
}

xi_proc_mutex_re xi_proc_mutex_unlock(xi_proc_mutex_t *lock) {
	xint32 ret;

	if (lock == NULL) {
		return XI_PROC_MUTEX_RV_ERR_ARGS;
	}

	ret = ReleaseMutex(lock->lock);
	if (!ret) {
		return XI_PROC_MUTEX_RV_ERR_LOCK;
	}

	return XI_PROC_MUTEX_RV_OK;
}

xi_proc_mutex_re xi_proc_mutex_close(xi_proc_mutex_t *lock) {
	xbool ret;

	if (lock == NULL) {
		return XI_PROC_MUTEX_RV_ERR_ARGS;
	}

	ret = CloseHandle(lock->lock);
	if (!ret) {
		return XI_PROC_MUTEX_RV_ERR_ARGS;
	}

	return XI_PROC_MUTEX_RV_OK;
}

xi_proc_mutex_re xi_proc_mutex_destroy(xi_proc_mutex_t *lock) {
	xbool ret;

	if (lock == NULL) {
		return XI_PROC_MUTEX_RV_ERR_ARGS;
	}

	ret = CloseHandle(lock->lock);
	if (!ret) {
		return XI_PROC_MUTEX_RV_ERR_ARGS;
	}

	return XI_PROC_MUTEX_RV_OK;
}

xintptr xi_proc_create(xchar * const cmdp[], xint32 cmdln, xchar * const envp[],
		xint32 envln, const xchar *workdir) {
	xint32 ret;

	DWORD dwCreationFlags;
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	xchar *realcmd = NULL;
	xchar *realenv = NULL;

	if (cmdp == NULL) {
		return -1;
	}

	xi_mem_set(&si, 0, sizeof(si));
	xi_mem_set(&pi, 0, sizeof(pi));

	si.cb = sizeof(si);
	si.dwFlags = STARTF_USESTDHANDLES;

	dwCreationFlags = GetVersion() & 0x80 ? 0 : CREATE_NO_WINDOW;

	if (envln) {
		xint32 i;
		xchar *envBldr;
		xsize envLength = envln + 1; // Length of strings + null terminators + final null terminator

		for (i = 0; i < envln; i++) {
			envLength += xi_strlen(envp[i]);
		}

		realenv = (xchar *) xi_mem_alloc(envLength);
		if (!realenv) {
			log_error(XDLOG, "Cannot allocate [env] memory!!!\n");
			return -1;
		}

		envBldr = realenv;

		for (i = 0; i < envln; i++) {
			xi_strcpy(envBldr, envp[i]);
			envBldr += (xi_strlen(envp[i]) + 1); // +1 for null terminator
		}
		*envBldr = '\0';
	}

	if (cmdp) {
		xint32 i;

		realcmd = (xchar *) xi_mem_alloc(1024);
		if (!realcmd) {
			log_error(XDLOG, "Cannot allocate [cmd] memory!!!\n");
			if (realenv) {
				xi_mem_free(realenv);
			}
			return -1;
		}

		for (i = 0; i < cmdln; i++) {
			xchar buf[256];
			if (cmdp[i][0]) {
				xi_mem_set(buf, 0, sizeof(buf));
				xi_sprintf(buf, "\"%s\" ", cmdp[i]);
				if (i == 0) {
					xi_strcpy(realcmd, buf);
				} else {
					xi_strcat(realcmd, buf);
				}
			}
		}
	}

	ret = CreateProcess(NULL, realcmd, /* Command line */
	NULL, NULL, /* Proc & thread security attributes */
	TRUE, /* Inherit handles */
	dwCreationFlags, /* Creation flags */
	realenv, /* Environment block */
	workdir, /* Current directory name */
	&si, &pi);

	if (realcmd) {
		xi_mem_free(realcmd);
	}
	if (realenv) {
		xi_mem_free(realenv);
	}

	return (ret == 0) ? -1 : (xintptr) pi.hProcess;
}

xintptr xi_proc_daemonize() {
	// TODO
	return 0;
}

xintptr xi_proc_getpid() {
	return (xintptr) OpenProcess(PROCESS_ALL_ACCESS, TRUE, GetCurrentProcessId());
}

xintptr xi_proc_waitpid(xintptr pid, xint32 *status) {
	DWORD procstat = 0;

	procstat = WaitForSingleObject((HANDLE) pid, INFINITE);
	GetExitCodeProcess((HANDLE) pid, &procstat);
	(*status) = (xint32) procstat;
	return pid;
}

xint32 xi_proc_term(xintptr pid) {
	int retVal = 0;
	retVal = TerminateProcess((HANDLE) pid, 0);
	return retVal;
}

xint32 xi_proc_atexit(xvoid(*func)()) {
	return atexit((void (__cdecl *)(void))func);
}

xvoid xi_proc_exit(xint32 status) {
	exit(status);
}

xvoid xi_proc_abort() {
	abort();
}
