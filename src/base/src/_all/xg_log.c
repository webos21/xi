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
 * File : xg_log.c
 */

#ifdef _WIN32
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <stdio.h>
#include <stdarg.h>

#include "xi/xi_log.h"

#include "xi/xi_mem.h"
#include "xi/xi_string.h"
#include "xi/xi_clock.h"
#include "xi/xi_file.h"

// ----------------------------------------------
// Inner Structures
// ----------------------------------------------

struct _st_logger {
	xchar id[XI_LOG_NAME_MAX];
	xi_logopt_t lopt;
};

// ----------------------------------------------
// Global Variables
// ----------------------------------------------

static xi_logger_t _g_logger_list[XI_LOG_INST_MAX];
static xi_logger_id_t _g_logger_ids[XI_LOG_INST_MAX];
static xint32 _g_logger_count = 0;
static xi_logger_t *_g_logger_def = NULL;
static xi_log_fn _g_log_fn = NULL;

// ----------------------------------------------
// Part Internal Functions
// ----------------------------------------------

static xint32 xg_logger_find(const xvoid *key) {
	xint32 i;
	xbool found = FALSE;

	for (i = 0; i < _g_logger_count; i++) {
		if (xi_strcmp(key, _g_logger_list[i].id) == 0) {
			found = TRUE;
			break;
		}
	}

	return (found ? i : -1);
}

static xvoid xg_logger_set_optdef(xi_logopt_t *opt) {
	opt->level = XI_LOG_LEVEL_INFO;
	opt->showDate = FALSE;
	opt->showFile = TRUE;
	opt->showFunc = TRUE;
	opt->showLine = TRUE;
}

static xvoid xg_logger_write_def(const xchar *msg) {
	static xint32 deffd = -1;

	if (deffd < 0) {
		deffd = xi_file_get_stdout();
	}

	xi_file_write(deffd, msg, xi_strlen(msg));
}

// ----------------------------------------------
// XI Functions
// ----------------------------------------------

xvoid xi_logger_set_handle(xi_log_fn fn) {
	_g_log_fn = fn;
}

xi_logger_t *xi_logger_fetch(const xchar *id) {
	xint32 idx;

	idx = xg_logger_find(id);
	if (idx < 0) {
		xi_logger_t *logger = &_g_logger_list[_g_logger_count];
		xi_sprintf(logger->id, "%s", id);
		xg_logger_set_optdef(&logger->lopt);
		_g_logger_count++;
		return logger;
	} else {
		xi_logger_t *logger = &_g_logger_list[idx];
		return logger;
	}
}

xvoid xi_logger_get_ids(xuint32 *count, xi_logger_id_t **ids) {
	xint32 i = 0;

	(*count) = (xuint32)_g_logger_count;
	for (i = 0; i < _g_logger_count; i++) {
		xi_strcpy(_g_logger_ids[i].id, _g_logger_list[i].id);
	}

	(*ids) = _g_logger_ids;
}

xi_logopt_t xi_logger_get_conf(xi_logger_t *logger) {
	return logger->lopt;
}

xvoid xi_logger_set_conf(xi_logger_t *logger, xi_logopt_t opt) {
	logger->lopt = opt;
}

xvoid xi_logger_write(xi_logger_t *logger, xi_log_level_e level,
		const xchar *fname, const xchar *function, xuint32 line,
		const xchar *format, ...) {
	xi_logger_t *rlog = NULL;

	if (_g_logger_def == NULL) {
		_g_logger_def = xi_logger_fetch("DEFLOG");
	}

	rlog = (logger == NULL) ? _g_logger_def : logger;

	if (level >= rlog->lopt.level) {
		xchar log[XI_LOG_LINE_MAX];
		xchar msg[XI_LOG_LINE_MAX - 64];

		va_list va;

		// Boost Variables
		xsize totch = 0;

		xi_mem_set(log, 0, sizeof(log));
		xi_mem_set(msg, 0, sizeof(msg));

		va_start(va, format);
		vsnprintf(msg, sizeof(msg), format, va);
		va_end(va);

		switch (level) {
		case XI_LOG_LEVEL_TRACE:
			xi_strncpy(log, "TRACE", 5);
			totch = 5;
			break;
		case XI_LOG_LEVEL_DEBUG:
			xi_strncpy(log, "DEBUG", 5);
			totch = 5;
			break;
		case XI_LOG_LEVEL_INFO:
			xi_strncpy(log, "INFO", 4);
			totch = 4;
			break;
		case XI_LOG_LEVEL_WARN:
			xi_strncpy(log, "WARN", 4);
			totch = 4;
			break;
		case XI_LOG_LEVEL_ERROR:
			xi_strncpy(log, "ERROR", 5);
			totch = 5;
			break;
		case XI_LOG_LEVEL_FATAL:
			xi_strncpy(log, "FATAL", 5);
			totch = 5;
			break;
		case XI_LOG_LEVEL_PRINT:
			if (_g_log_fn == NULL) {
				xg_logger_write_def(msg);
			} else {
				_g_log_fn(msg);
			}
			return;
		default:
			return;
		}

		if (rlog->lopt.showDate == TRUE) {
			xchar tstr[25];
			xi_time_t now;

			xi_clock_gettime(&now);

			xi_sprintf(tstr, "|%d-%02d-%02d %02d:%02d:%02d.%03d", now.year,
					now.mon, now.day, now.hour, now.min, now.sec, now.msec);
			xi_strcat(log + (totch - 2), tstr);
			totch += 24;
		}

		if (rlog->lopt.showFile == TRUE) {
			xi_strcat(log + (totch - 2), "|");
			totch += 1;
			xi_strcat(log + (totch - 2), xi_pathname_basename(fname));
			totch += xi_strlen(xi_pathname_basename(fname));
		}

		if (rlog->lopt.showFunc == TRUE) {
			xi_strcat(log + (totch - 2), "|");
			totch += 1;
			xi_strcat(log, function);
			totch += xi_strlen(function);
		}

		if (rlog->lopt.showLine == TRUE) {
			xchar lstr[8];// 8

			xi_strcat(log + (totch - 2), "|");
			totch += 1;

			xi_sprintf(lstr, "%05u", line);
			xi_strcat(log + (totch - 2), lstr);
			totch += 5;
		}

		xi_strcat(log + (totch - 2), "|");
		totch += 1;
		xi_strcat(log + (totch - 2), msg);
		totch += xi_strlen(msg);

		if (_g_log_fn == NULL) {
			xg_logger_write_def(log);
		} else {
			_g_log_fn(log);
		}
	}
}
