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
 * File   : xg_sysinfo.c
 */

#include <unistd.h>
#include <sys/utsname.h>
#include <limits.h>
#include <errno.h>
#include <pwd.h>
#ifdef __APPLE__
#include <mach-o/dyld.h>
#endif

#include "xi/xi_sysinfo.h"

#include "xi/xi_string.h"
#include "xi/xi_clock.h"

// ----------------------------------------------
// XI Functions
// ----------------------------------------------

xlong xi_sysinfo_cpu_num() {
	static xlong _s_cpunum = 0;

	if (!_s_cpunum) {
		_s_cpunum = sysconf(_SC_NPROCESSORS_CONF);
	}

	return _s_cpunum;
}

const xchar *xi_sysinfo_cpu_arch() {
#if defined(_IPF_)       // IA64
	return "ia64";
#elif defined (__x86_64) || defined(__amd64) || (defined(_WIN32) && defined(_M_AMD64)) // x86_64
	return "x86_64";
#elif defined (__mips__) // MIPS
	return "MIPS";
#elif defined (__arm__)  // ARM
	return "ARM";
#else                    // others
	return "x86";
#endif                   // end of #if
}

xssize xi_sysinfo_os_name(xchar *nbuf, xsize nblen) {
	struct utsname sys_info;
	xssize ret;

	if (nbuf == NULL || nblen == 0) {
		return -1;
	}

	ret = uname(&sys_info);
	if (-1 == ret) {
		return -1;
	}

	xi_strcpy(nbuf, sys_info.sysname);

	return (xssize) xi_strlen(nbuf);
}

xssize xi_sysinfo_os_ver(xchar *nbuf, xsize nblen) {
	struct utsname sys_info;
	xint32 ret;

	if (nbuf == NULL || nblen == 0) {
		return -1;
	}

	ret = uname(&sys_info);
	if (-1 == ret) {
		return -1;
	}

	xi_strcpy(nbuf, sys_info.release);

	return (xssize) xi_strlen(nbuf);
}

xlong xi_sysinfo_pagesize() {
	static xlong _s_psize = 0;

	if (!_s_psize) {
		_s_psize = sysconf(_SC_PAGE_SIZE);
		if (!_s_psize) {
			_s_psize = 4 * 1024;
		}
	}

	return _s_psize;
}

xssize xi_sysinfo_user_name(xchar *nbuf, xsize nblen) {
	struct passwd *user_info;

	if (nbuf == NULL || nblen == 0) {
		return -1;
	}

	errno = 0;
	user_info = getpwuid(getuid());
	if (errno != 0) {
		return -1;
	}

	xi_strcpy(nbuf, user_info->pw_name);

	return (xssize) xi_strlen(nbuf);
}

xssize xi_sysinfo_user_home(xchar *nbuf, xsize nblen) {
	struct passwd *user_info;

	if (nbuf == NULL || nblen == 0) {
		return -1;
	}

	errno = 0;
	user_info = getpwuid(getuid());
	if (errno != 0) {
		return -1;
	}

	xi_strcpy(nbuf, user_info->pw_dir);

	return (xssize) xi_strlen(nbuf);
}

xssize xi_sysinfo_user_tz(xchar *nbuf, xsize nblen) {
	xint32 bias = xi_clock_get_tz();

	if (nbuf == NULL || nblen == 0) {
		return -1;
	}

	xi_sprintf(nbuf, "GMT%+d:00", bias);

	return (xssize) xi_strlen(nbuf);
}

xssize xi_sysinfo_exec_path(xchar *nbuf, xsize nblen) {
#ifdef __APPLE__
	if (_NSGetExecutablePath(nbuf, (xuint32 *)&nblen) != 0) {
		return -1;
	}
	return xi_strlen(nbuf);
#else  // !__APPLE__
	xssize len;

	if (nbuf == NULL || nblen == 0) {
		return -1;
	}

	len = readlink("/proc/self/exe", nbuf, nblen);
	if (len == -1) {
		return -1;
	}
	nbuf[len] = '\0';

	return len;
#endif //__APPLE__
}
