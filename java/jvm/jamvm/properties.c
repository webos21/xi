/*
 * Copyright (C) 2003, 2004, 2005, 2006, 2007, 2008, 2009
 * Robert Lougher <rob@jamvm.org.uk>.
 *
 * This file is part of JamVM.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2,
 * or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include "jam.h"
#include "symbol.h"
#include "properties.h"

#include "xi/xi_mem.h"
#include "xi/xi_env.h"
#include "xi/xi_string.h"
#include "xi/xi_file.h"
#include "xi/xi_sysinfo.h"

/*
 #ifdef HAVE_LOCALE_H
 #include <locale.h>
 #endif
 */

static Property *commandline_props;
static int commandline_props_count;

void initialiseProperties(InitArgs *args) {
	commandline_props = args->commandline_props;
	commandline_props_count = args->props_count;
}

char *getCommandLineProperty(char *key) {
	int i;

	for (i = 0; i < commandline_props_count; i++)
		if (xi_strcmp(commandline_props[i].key, key) == 0)
			return commandline_props[i].value;

	return NULL;
}

void setProperty(Object *properties, char *key, char *value) {
	Object *k = Cstr2String(key);
	Object *v = Cstr2String(value ? value : "?");

	MethodBlock *mb = lookupMethod(properties->class, SYMBOL(put),
			SYMBOL(_java_lang_Object_java_lang_Object__java_lang_Object));
	executeMethod(properties, mb, k, v);
}

void addCommandLineProperties(Object *properties) {
	if (commandline_props_count) {
		int i;

		for (i = 0; i < commandline_props_count; i++) {
			setProperty(properties, commandline_props[i].key,
					commandline_props[i].value);
			sysFree(commandline_props[i].key);
		}

		commandline_props_count = 0;
		sysFree(commandline_props);
	}
}

void setLocaleProperties(Object *properties) {
	/*
	 #if defined(HAVE_SETLOCALE) && defined(HAVE_LC_MESSAGES)
	 char *locale;

	 setlocale(LC_ALL, "");
	 if((locale = setlocale(LC_MESSAGES, "")) != NULL) {
	 int len = strlen(locale);

	 // Check if the returned string is in the expected format,
	 // e.g. de, or en_GB
	 if(len == 2 || (len > 4 && locale[2] == '_')) {
	 char code[3];

	 code[0] = locale[0];
	 code[1] = locale[1];
	 code[2] = '\0';
	 setProperty(properties, "user.language", code);

	 // Set the region -- the bit after the "_"
	 if(len > 4) {
	 code[0] = locale[3];
	 code[1] = locale[4];
	 setProperty(properties, "user.region", code);
	 }
	 }
	 }
	 #endif
	 */
}

char *getCwd() {
	char *cwd = NULL;
	int size = 256;

	for (;;) {
		cwd = sysRealloc(cwd, size);

		if (xi_pathname_get(cwd, size) != XI_FILE_RV_OK) {
			log_fatal(XDLOG, "Couldn't get cwd");
			exitVM(1);
		} else {
			return cwd;
		}
	}
}

void setUserDirProperty(Object *properties) {
	char *cwd = getCwd();

	setProperty(properties, "user.dir", cwd);
	sysFree(cwd);
}

void setOSProperties(Object *properties) {
	char os_name[32];
	char os_ver[32];
	char cpu_arch[32];

	xi_sysinfo_os_name(os_name, sizeof(os_name));
	xi_sysinfo_os_ver(os_ver, sizeof(os_ver));
	xi_strcpy(cpu_arch, xi_sysinfo_cpu_arch());

	setProperty(properties, "os.arch", cpu_arch);
	setProperty(properties, "os.name", os_name);
	setProperty(properties, "os.version", os_ver);
}

char *getJavaHome() {
	static xchar jhome[256];
	static xchar exepath[256];
	xint32 ret;

	xi_mem_set(jhome, 0, sizeof(jhome));
	xi_mem_set(exepath, 0, sizeof(exepath));

	ret = xi_env_get("JAVA_HOME", jhome, sizeof(jhome));
	if (ret > 0) {
		return jhome;
	}
	ret = xi_sysinfo_exec_path(exepath, sizeof(exepath));
	if (ret > 7) {
		xchar *cpos = exepath + xi_strlen(exepath) - 1;
		// remove path seperator
		if (*cpos == '/' || *cpos == '\\') {
			*cpos = '\0';
			cpos--;
		}
		// remove executable name
		if (cpos - exepath > 3) {
			while (*cpos != '/' && *cpos != '\\') {
				*cpos = '\0';
				cpos--;
			}
		}
		// remove path seperator
		if (*cpos == '/' || *cpos == '\\') {
			*cpos = '\0';
			cpos--;
		}
		// remove execution-directory
		if (cpos - exepath > 3) {
			while (*cpos != '/' && *cpos != '\\') {
				*cpos = '\0';
				cpos--;
			}
		}
		// remove path seperator
		if (*cpos == '/' || *cpos == '\\') {
			*cpos = '\0';
			cpos--;
		}
		return exepath;
	}

	return NULL;
}

void addDefaultProperties(Object *properties) {
	char usr_name[64];
	char usr_home[256];
	char usr_dir[256];

	xi_sysinfo_user_name(usr_name, sizeof(usr_name));
	xi_sysinfo_user_home(usr_home, sizeof(usr_home));
	xi_sysinfo_exec_path(usr_dir, sizeof(usr_dir));

	setProperty(properties, "java.vm.name", "JamVM");
	setProperty(properties, "java.vm.version", VERSION);
	setProperty(properties, "java.vm.vendor", "xi");
	setProperty(properties, "java.vm.vendor.url", "http://jamvm.org");
	setProperty(properties, "java.vm.specification.version", "1.0");
	setProperty(properties, "java.vm.specification.vendor",
			"Sun Microsystems, Inc.");
	setProperty(properties, "java.vm.specification.name",
			"Java Virtual Machine Specification");
	setProperty(properties, "java.runtime.version", VERSION);
	setProperty(properties, "java.version", JAVA_COMPAT_VERSION);
	setProperty(properties, "java.vendor", "Apache Classpath");
	setProperty(properties, "java.vendor.url", "http://www.classpath.org");
	setProperty(properties, "java.home", getJavaHome());
	setProperty(properties, "java.specification.version", "1.5");
	setProperty(properties, "java.specification.vendor",
			"Sun Microsystems, Inc.");
	setProperty(properties, "java.specification.name",
			"Java Platform API Specification");
	setProperty(properties, "java.class.version", "48.0");
	setProperty(properties, "java.class.path", getClassPath());
	setProperty(properties, "sun.boot.class.path", getBootClassPath());
	setProperty(properties, "java.boot.class.path", getBootClassPath());
	setProperty(properties, "gnu.classpath.boot.library.path", getBootDllPath());
	setProperty(properties, "java.library.path", getDllPath());
	setProperty(properties, "java.io.tmpdir", "/tmp");
	setProperty(properties, "java.compiler", "");
	setProperty(properties, "java.ext.dirs", "");
	setProperty(properties, "file.separator", XI_SEP_FILE_S);
	setProperty(properties, "path.separator", XI_SEP_PATH_S);
	setProperty(properties, "line.separator", "\n");
	setProperty(properties, "user.name", usr_name);
	setProperty(properties, "user.home", usr_home);
	setProperty(properties, "user.dir", usr_dir);
	setProperty(properties, "user.language", "ko");
	setProperty(properties, "user.region", "KR");
	setProperty(properties, "user.variant", "");
	setProperty(properties, "user.timezone", "Asia/Seoul");
	setProperty(properties, "gnu.cpu.endian", IS_BIG_ENDIAN ? "big" : "little");
	setProperty(properties, "http.keepAlive", "false");

	setOSProperties(properties);
	setUserDirProperty(properties);
	setLocaleProperties(properties);

	// added by cmjo
	{
		xchar proot[256];

		xi_mem_set(proot, 0, sizeof(proot));

		xi_strcpy(proot, getJavaHome());
		xi_strcat(proot, "/apps");

		setProperty(properties, "dvb.persistent.root", proot);

		setProperty(properties, "mhp.eb.version.major", "1");
		setProperty(properties, "mhp.eb.version.minor", "0");
		setProperty(properties, "mhp.eb.version.micro", "3");

		setProperty(properties, "mhp.ib.version.major", "1");
		setProperty(properties, "mhp.ib.version.minor", "0");
		setProperty(properties, "mhp.ib.version.micro", "3");

		setProperty(properties, "ocap.version.major", "1");
		setProperty(properties, "ocap.version.minor", "1");
		setProperty(properties, "ocap.version.micro", "3");
	}
}

