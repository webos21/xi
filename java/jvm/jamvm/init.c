/*
 * Copyright (C) 2003, 2004, 2005, 2006, 2007, 2008
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

#include "xi/xi_mem.h"
#include "xi/xi_string.h"
#include "xi/xi_file.h"
#include "xi/xi_process.h"

#include <stdio.h>

static int VM_initing = TRUE;
extern void initialisePlatform();

static int xfprintf(int fd, const char *fmt, va_list ap) {
	xchar msg[1024];
	int mlen = 0;

	xi_mem_set(msg, 0, sizeof(msg));
	vsnprintf(msg, sizeof(msg), fmt, ap);
	mlen = xi_strlen(msg);
	xi_file_write(fd, msg, mlen);

	return mlen;
}

/* Setup default values for command line args */

void setDefaultInitArgs(InitArgs *args) {
	xi_mem_set(args, 0, sizeof(InitArgs));

	args->asyncgc = FALSE;

	args->verbosegc = FALSE;
	args->verbosedll = FALSE;
	args->verboseclass = FALSE;

	args->compact_specified = FALSE;

	args->classpath = NULL;
	args->bootpath = NULL;

	args->java_stack = DEFAULT_STACK;
	args->min_heap = DEFAULT_MIN_HEAP;
	args->max_heap = DEFAULT_MAX_HEAP;

	args->props_count = 0;

	args->vfprintf = xfprintf;
	args->abort = xi_proc_abort;
	args->exit = xi_proc_exit;
}

int VMInitialising() {
	return VM_initing;
}

void initVM(InitArgs *args) {
	xint32 verbose = args->verboseclass | args->verbosedll | args->verbosegc;
	/* Perform platform dependent initialisation */
	//initialisePlatform();

	/* Initialise the VM modules -- ordering is important! */
	if (verbose) log_trace(XDLOG, "Init Hooks....\n");
	initialiseHooks(args);
	if (verbose) log_trace(XDLOG, "Init Properties....\n");
	initialiseProperties(args);
	if (verbose) log_trace(XDLOG, "Init Alloc....\n");
	initialiseAlloc(args);
	if (verbose) log_trace(XDLOG, "Init DLL....\n");
	initialiseDll(args);
	if (verbose) log_trace(XDLOG, "Init UTF8....\n");
	initialiseUtf8();
	if (verbose) log_trace(XDLOG, "Init ThreadStage1....\n");
	initialiseThreadStage1(args);
	if (verbose) log_trace(XDLOG, "Init Symbol....\n");
	initialiseSymbol();
	if (verbose) log_trace(XDLOG, "Init Class....\n");
	initialiseClass(args);
	if (verbose) log_trace(XDLOG, "Init Monitor....\n");
	initialiseMonitor();
	if (verbose) log_trace(XDLOG, "Init String....\n");
	initialiseString();
	if (verbose) log_trace(XDLOG, "Init Exception....\n");
	initialiseException();
	if (verbose) log_trace(XDLOG, "Init Natives....\n");
	initialiseNatives();
	if (verbose) log_trace(XDLOG, "Init JNI....\n");
	initialiseJNI();
	if (verbose) log_trace(XDLOG, "Init Interpreter....\n");
	initialiseInterpreter(args);
	if (verbose) log_trace(XDLOG, "Init ThreadStage2....\n");
	initialiseThreadStage2(args);
	if (verbose) log_trace(XDLOG, "Init gc....\n");
	initialiseGC(args);

	VM_initing = FALSE;
}

unsigned long parseMemValue(char *str) {
	char *end;
	unsigned long n = xi_strtoi(str, &end, 0);

	switch (end[0]) {
	case '\0':
		break;
	case 'M':
	case 'm':
		n *= MB;
		break;
	case 'K':
	case 'k':
		n *= KB;
		break;
	default:
		n = 0;
	}

	return n;
}
