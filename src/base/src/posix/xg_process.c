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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <semaphore.h>
#include <signal.h>
#include <sys/wait.h>

#include "xi/xi_process.h"

#include "xi/xi_log.h"
#include "xi/xi_mem.h"
#include "xi/xi_string.h"
#include "xi/xi_file.h"

// ----------------------------------------------
// Inner Structures
// ----------------------------------------------

struct _xi_proc_mutex {
	xchar filepath[XCFG_PATHNAME_MAX];
	sem_t *lock;
};

// ----------------------------------------------
// XI Functions
// ----------------------------------------------

xi_proc_mutex_re xi_proc_mutex_create(xi_proc_mutex_t **lock,
		const xchar *filepath) {
	xi_proc_mutex_t *mutex = xi_mem_calloc(1, sizeof(xi_proc_mutex_t));
	if (mutex == NULL) {
		return XI_PROC_MUTEX_RV_ERR_ALLOC;
	}

	mutex->lock = sem_open(filepath, O_CREAT | O_EXCL | O_RDWR, 0600, 1);
	if (mutex->lock == SEM_FAILED) {
		xi_mem_free(mutex);
		switch (errno) {
		case EEXIST:
			return XI_PROC_MUTEX_RV_ERR_EXIST;
		default:
			return XI_PROC_MUTEX_RV_ERR_CREATE;
		}
	}

	xi_strcpy(mutex->filepath, filepath);
	*lock = mutex;

	return XI_PROC_MUTEX_RV_OK;
}

xi_proc_mutex_re xi_proc_mutex_open(xi_proc_mutex_t **lock,
		const xchar *filepath) {
	xi_proc_mutex_t *mutex = xi_mem_calloc(1, sizeof(xi_proc_mutex_t));
	if (mutex == NULL) {
		return XI_PROC_MUTEX_RV_ERR_ALLOC;
	}

	mutex->lock = sem_open(filepath, O_RDWR, 0644, 1);
	if (mutex->lock == SEM_FAILED) {
		xi_mem_free(mutex);
		switch (errno) {
		case EEXIST:
			return XI_PROC_MUTEX_RV_ERR_EXIST;
		default:
			return XI_PROC_MUTEX_RV_ERR_CREATE;
		}
	}

	xi_strcpy(mutex->filepath, filepath);
	*lock = mutex;

	return XI_PROC_MUTEX_RV_OK;
}

xi_proc_mutex_re xi_proc_mutex_lock(xi_proc_mutex_t *lock) {
	xint32 ret;

	if (lock == NULL) {
		return XI_PROC_MUTEX_RV_ERR_ARGS;
	}

	ret = sem_wait(lock->lock);
	if (ret < 0) {
		switch (errno) {
		case EINVAL:
			return XI_PROC_MUTEX_RV_ERR_ARGS;
		default:
			return XI_PROC_MUTEX_RV_ERR_LOCK;
		}
	}

	return XI_PROC_MUTEX_RV_OK;
}

xi_proc_mutex_re xi_proc_mutex_unlock(xi_proc_mutex_t *lock) {
	xint32 ret;

	if (lock == NULL) {
		return XI_PROC_MUTEX_RV_ERR_ARGS;
	}

	ret = sem_post(lock->lock);
	if (ret < 0) {
		switch (errno) {
		case EINVAL:
			return XI_PROC_MUTEX_RV_ERR_ARGS;
		default:
			return XI_PROC_MUTEX_RV_ERR_LOCK;
		}
	}

	return XI_PROC_MUTEX_RV_OK;
}

xi_proc_mutex_re xi_proc_mutex_close(xi_proc_mutex_t *lock) {
	xint32 ret;

	if (lock == NULL) {
		return XI_PROC_MUTEX_RV_ERR_ARGS;
	}

	ret = sem_close(lock->lock);
	if (ret < 0) {
		return XI_PROC_MUTEX_RV_ERR_ARGS;
	}

	return XI_PROC_MUTEX_RV_OK;
}

xi_proc_mutex_re xi_proc_mutex_destroy(xi_proc_mutex_t *lock) {
	xint32 ret;

	if (lock == NULL) {
		return XI_PROC_MUTEX_RV_ERR_ARGS;
	}

	ret = sem_close(lock->lock);
	if (ret < 0) {
		return XI_PROC_MUTEX_RV_ERR_ARGS;
	}

	sem_unlink(lock->filepath);

	return XI_PROC_MUTEX_RV_OK;
}

static const xchar * xg_proc_errmsg(xint32 errnum) {
	switch (errnum) {
	case EACCES:
		return "EACCES Permission denied";
	case EPERM:
		return "EPERM Not super-user";
	case E2BIG:
		return "E2BIG Arg list too long";
	case ENOEXEC:
		return "ENOEXEC Exec format error";
	case EFAULT:
		return "EFAULT Bad address";
	case ENAMETOOLONG:
		return "ENAMETOOLONG path name is too long     ";
	case ENOENT:
		return "ENOENT No such file or directory";
	case ENOMEM:
		return "ENOMEM Not enough core";
	case ENOTDIR:
		return "ENOTDIR Not a directory";
	case ELOOP:
		return "ELOOP Too many symbolic links";
	case ETXTBSY:
		return "ETXTBSY Text file busy";
	case EIO:
		return "EIO I/O error";
	case ENFILE:
		return "ENFILE Too many open files in system";
	case EINVAL:
		return "EINVAL Invalid argument";
	case EISDIR:
		return "EISDIR Is a directory";
#ifdef ELIBBAD
	case ELIBBAD:
		return "ELIBBAD Accessing a corrupted shared lib";
#endif
	default:
		if (errnum) {
			return "Unknown error!!";
		}
		return "";
	}
}

xint32 xi_proc_create(xchar * const cmdp[], xint32 cmdln, xchar * const envp[],
		xint32 envln, const xchar *workdir) {
	xint32 pid;

	xint32 chkchild[2] = { 0, 0 };
	xchar *cmd;

	UNUSED(cmdln);
	UNUSED(envln);

	if (cmdp == NULL) {
		log_error(XDLOG, "give command is NULL!!\n");
		return -1;
	}

	if (pipe(chkchild) == -1) {
		log_error(XDLOG, "cannot create command pipe!!\n");
		return -1;
	}

	cmd = cmdp[0];

	pid = fork();
	if (pid == 0) { // Child Process
		xint32 ret = 0;
		xchar dummy = '\0';

		if (-1 == write(chkchild[1], &dummy, 1)) {
			log_error(XDLOG, "[child] cannot write to the pipe!!!\n");
			close(chkchild[0]);
			close(chkchild[1]);
			exit(-1);
		}

		if (workdir) {
			if (chdir(workdir) == -1) {
				exit(-1);
			}
		}

		log_trace(XDLOG, "child write done!!!\n");
		close(chkchild[0]);
		close(chkchild[1]);

		ret = execve(cmd, cmdp, envp);
		if (ret == -1) {
			log_error(XDLOG, "[child] execve err (cmd=%s): %s\n", cmd, xg_proc_errmsg(errno));
			exit(errno);
		}

		exit(0);
	} else if (pid > 0) { // Parent Process
		xchar dummy;

		if (-1 == read(chkchild[0], &dummy, 1)) {
			log_error(XDLOG, "[parent] cannot read from the pipe!!!\n");
			close(chkchild[0]);
			close(chkchild[1]);
			return -1;
		}
		close(chkchild[0]);
		close(chkchild[1]);
	} else {
		close(chkchild[0]);
		close(chkchild[1]);
		log_error(XDLOG, "fork is failed!!\n");
		return -1;
	}

	return pid;
}

xint32 xi_proc_daemonize() {
	xint32 pid;

	pid = fork();
	if (pid < 0) {
		printf("Error!!: fork()\n");
		exit(EXIT_FAILURE);
	} else if (pid > 0) {
		exit(EXIT_SUCCESS);
	}

	if (setsid() == -1) {
		printf("Error!!: setsid()\n");
		return -1;
	}

	if (freopen("/dev/null", "r", stdin) == NULL) {
		log_error(XDLOG, "cannot close STDIN!!\n");
		return -1;
	}
	if (freopen("/dev/null", "w", stdout) == NULL) {
		log_error(XDLOG, "cannot close STDOUT!!\n");
		return -1;
	}
	if (freopen("/dev/null", "w", stderr) == NULL) {
		log_error(XDLOG, "cannot close STDERR!!\n");
		return -1;
	}

	return 0;
}

xint32 xi_proc_getpid() {
	return getpid();
}

xint32 xi_proc_waitpid(xint32 pid, xint32 *status) {
	xint32 stat = 0;
	xint32 ret = 0;

	if (pid < 0) {
		ret = waitpid(pid, &stat, WNOHANG);
	} else {
		ret = waitpid(pid, &stat, 0);
	}
	if (ret > 0) {
		if (WIFEXITED(stat) != 0) {
			stat = WEXITSTATUS(stat);
		} else {
			if (WIFSIGNALED(stat) != 0) {
				stat = WTERMSIG(stat);
			} else {
				if (WIFSTOPPED(stat) != 0) {
					stat = WSTOPSIG(stat);
				}
			}
		}
		(*status) = stat;
	}

	return ret;
}

xint32 xi_proc_term(xint32 pid) {
	xint32 ret = 0;
	xint32 status = 0;

	ret = kill(pid, SIGTERM);
	if (ret < 0) {
		return errno;
	}

	waitpid(pid, &status, 0); // wipe zombie

	return ret;
}

xint32 xi_proc_atexit(xvoid(*func)()) {
	return atexit(func);
}

xvoid xi_proc_exit(xint32 status) {
	exit(status);
}

xvoid xi_proc_abort() {
	abort();
}
