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

#include <windows.h>
#include <userenv.h>

#include "xi/xi_sysinfo.h"

#include "xi/xi_string.h"
#include "xi/xi_clock.h"

// ----------------------------------------------
// XI Functions
// ----------------------------------------------

xlong xi_sysinfo_cpu_num() {
	static xlong _s_cpunum = 0;

	if (!_s_cpunum) {
		typedef void (WINAPI *PTR_GETNATIVESYSTEM_INFO)(LPSYSTEM_INFO);
		SYSTEM_INFO sys_info;

		static PTR_GETNATIVESYSTEM_INFO pTrGetNativeSystemInfo = NULL;

		if (!pTrGetNativeSystemInfo) {
			HMODULE h = GetModuleHandleA("kernel32.dll");
			/*
			 * Use GetNativeSystemInfo if available in kernel
			 * It provides more accurate info in WOW64 mode
			 */
			pTrGetNativeSystemInfo = (PTR_GETNATIVESYSTEM_INFO) GetProcAddress(
					h, "GetNativeSystemInfo");
		}

		if (pTrGetNativeSystemInfo != NULL) {
			pTrGetNativeSystemInfo(&sys_info);
		} else {
			GetSystemInfo(&sys_info);
		}

		_s_cpunum = sys_info.dwNumberOfProcessors;
	}

	return _s_cpunum;
}

const xchar *xi_sysinfo_cpu_arch() {
#if defined(_IPF_)
	return "ia64";
#elif defined(_WIN64)
	return "x86_64";
#else
	return "x86";
#endif
}

xssize xi_sysinfo_os_name(xchar *nbuf, xsize nblen) {
	xchar * name_buf = NULL;

	OSVERSIONINFO vi;

	if (nbuf == NULL || nblen == 0) {
		return -1;
	}

	vi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	if (!GetVersionEx(&vi)) {
		return -1;
	}

	switch (vi.dwPlatformId) {
	case VER_PLATFORM_WIN32_NT: // Windows NT, Windows 2000, Windows XP, or Windows Server 2003 family.
		switch (vi.dwMajorVersion) {
		case 3:
		case 4:
			name_buf = "Windows NT";
			break;
		case 5:
			switch (vi.dwMinorVersion) {
			case 0:
				name_buf = "Windows 2000";
				break;
			case 1:
				name_buf = "Windows XP";
				break;
			case 2:
				name_buf = "Windows Server 2003";
				break;
			}
			break;
		}
		break;
	case VER_PLATFORM_WIN32_WINDOWS: // Windows 95, Windows 98, or Windows Me.
		if (4 == vi.dwMajorVersion) {
			switch (vi.dwMinorVersion) {
			case 0:
				name_buf = "Windows 95";
				break;
			case 10:
				name_buf = "Windows 98";
				break;
			case 90:
				name_buf = "Windows Me";
				break;
			}
		}
		break;
	case VER_PLATFORM_WIN32s: /* Win32s on Windows 3.1. */
		name_buf = "Win32s";
		break;
	}
	if (!name_buf) {
		name_buf = "Windows";
	}

	xi_strcpy(nbuf, name_buf);

	return xi_strlen(nbuf);
}

xssize xi_sysinfo_os_ver(xchar *nbuf, xsize nblen) {
	OSVERSIONINFO vi;

	if (nbuf == NULL || nblen == 0) {
		return -1;
	}

	vi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	if (!GetVersionEx(&vi)) {
		return -1;
	}

	xi_sprintf(nbuf, "%d.%d", vi.dwMajorVersion, vi.dwMinorVersion);

	return xi_strlen(nbuf);
}

xlong xi_sysinfo_pagesize() {
	static xlong psize = 0;

	if (!psize) {
		SYSTEM_INFO info;
		GetSystemInfo(&info);
		psize = info.dwAllocationGranularity;
	}

	return psize;
}

xssize xi_sysinfo_user_name(xchar *nbuf, xsize nblen) {
	DWORD len = (DWORD)nblen;

	if (nbuf == NULL || nblen == 0) {
		return -1;
	}

	if (!GetUserName(nbuf, &len)) {
		return -1;
	}

	return len;
}

xssize xi_sysinfo_user_home(xchar *nbuf, xsize nblen) {
	HANDLE token;
	DWORD len = (DWORD)nblen;

	if (nbuf == NULL || nblen == 0) {
		return -1;
	}

	if (OpenProcessToken(GetCurrentProcess(), READ_CONTROL | TOKEN_READ, &token)
			&& GetUserProfileDirectory(token, nbuf, &len) && CloseHandle(token)) {
		return len;
	}

	return -1;
}

xssize xi_sysinfo_user_tz(xchar *nbuf, xsize nblen) {
	xint32 bias = xi_clock_get_tz();

	if (nbuf == NULL || nblen == 0) {
		return -1;
	}

	xi_sprintf(nbuf, "GMT%+d:00", bias);

	return xi_strlen(nbuf);
}

xssize xi_sysinfo_exec_path(xchar *nbuf, xsize nblen) {
	xssize len;

	if (nbuf == NULL || nblen == 0) {
		return -1;
	}

	len = GetModuleFileName(0, nbuf, XCFG_PATHNAME_MAX);
	if (0 == len) {
		return -1;
	}

	return len;
}
