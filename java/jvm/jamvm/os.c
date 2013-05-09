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

#include "xi/xi_dso.h"
#include "xi/xi_string.h"
#include "xi/xi_sysinfo.h"
#include "xi/xi_thread.h"

void *nativeStackBase() {
	return xi_thread_get_stackbase(xi_thread_self());
}

int nativeAvailableProcessors() {
	return xi_sysinfo_cpu_num();
}

char *nativeLibError() {
	return xi_dso_error();
}

char *nativeLibPath() {
	static char npath[2048];
	xi_dso_get_searchpath(npath, 2048);
	return npath;
}

void *nativeLibOpen(char *path) {
	return xi_dso_load(path);
}

void nativeLibClose(void *handle) {
	xi_dso_unload(handle);
}

void *nativeLibSym(void *handle, char *symbol) {
	return xi_dso_get_func(handle, symbol);
}

char *nativeLibMapName(char *name) {
	char *buff = sysMalloc(xi_strlen(name) + 10);
	xi_dso_get_sysname(buff, name);
	return buff;
}
