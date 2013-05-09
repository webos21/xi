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

#ifndef _XI_LOG_H_
#define _XI_LOG_H_

/**
 * @brief XI Logging API
 *
 * @file xi_log.h
 * @date 2010-08-31
 * @author Cheolmin Jo (webos21@gmail.com)
 */

#include "xtype.h"

/**
* Start Declaration
*/
_XI_EXTERN_C_BEGIN

/**
 * @defgroup xi_log Logging API
 * @ingroup XI
 * @{
 * @brief
 *
 * MACRO based log APIs for easy using.
 *
 */

#define XI_LOG_LINE_MAX	 1024   ///< Maximum length of one line
#define XI_LOG_NAME_MAX	 64     ///< Maximum length of logger-name
#define XI_LOG_INST_MAX  128    ///< Maximum number of logger

/**
 * The level of Logs
 */
typedef enum _me_log_level {
	XI_LOG_LEVEL_ALL    = 0,    ///< Turn on all logs
	XI_LOG_LEVEL_TRACE  = 1,    ///< Allow above the trace level (Same as ALL)
	XI_LOG_LEVEL_DEBUG  = 2,    ///< Allow above the debug level
	XI_LOG_LEVEL_INFO   = 3,    ///< Allow above the info level
	XI_LOG_LEVEL_WARN   = 4,    ///< Allow above the warn level
	XI_LOG_LEVEL_ERROR  = 5,    ///< Allow above the error level
	XI_LOG_LEVEL_FATAL  = 6,    ///< Allow above the fatal level
	XI_LOG_LEVEL_PRINT  = 7,    ///< Just print log without any decoration
	XI_LOG_LEVEL_OFF    = 8     ///< Turn off all logs
} xi_log_level_e;

/**
 * The structure of log-options
 */
typedef struct _st_logopt {
	xi_log_level_e level;    ///< Printable log level
	xbool          showDate; ///< Want to print date
	xbool          showFile; ///< Want to print file name
	xbool          showFunc; ///< Want to print function name
	xbool          showLine; ///< Want to print file line
} xi_logopt_t;

/**
 * Abstract handle of logger
 */
typedef struct _st_logger xi_logger_t;

/**
 * The structure of logger ID
 */
typedef struct _st_logger_id {
	xchar id[XI_LOG_NAME_MAX];  ///< ID-string of Logger
} xi_logger_id_t;

/**
 * The function pointer of real print function
 */
typedef xvoid (*xi_log_fn)(xchar *msg);

/**
 * Set the custom logging function.
 *
 * @param fn the custom function
 */
xvoid        xi_logger_set_handle(xi_log_fn fn);

/**
 * Get or create a specific logger.
 *
 * @param id the id of logger
 * @return the pointer of logger instance
 *
 * @remark do not use it. use logger_get
 */
xi_logger_t *xi_logger_fetch(const xchar *id);

/**
 * Get the list of loggers
 *
 * @param[out] count the pointer of number to get
 * @param[out] ids the pointer of \a xi_logger_id_t array
 */
xvoid        xi_logger_get_ids(xuint32 *count, xi_logger_id_t **ids);

/**
 * Get the configuration of logger
 *
 * @param logger the logger handle
 * @return the option structure of given logger
 */
xi_logopt_t  xi_logger_get_conf(xi_logger_t *logger);

/**
 * Set the configuration of logger
 *
 * @param logger the logger handle
 * @param opt the option structure to set the logger
 */
xvoid        xi_logger_set_conf(xi_logger_t *logger, xi_logopt_t opt);

/**
 * Print the log message
 *
 * @param logger the logger handle
 * @param level the level of this log message
 * @param fname the file-name of caller
 * @param function the function name of caller
 * @param line the line number of caller
 * @param format the format string of log message
 * @param ... the ambiguous argument of log message
 */
xvoid        xi_logger_write(xi_logger_t *logger, xi_log_level_e level,
		               const xchar *fname, const xchar *function, xuint32 line,
                       const xchar *format, ...);
/// @}

#define XDLOG  NULL  ///< Default Logger

#ifdef XCFG_DEBUG
#		define logger_get(id)                      xi_logger_fetch(id)               ///< Get the logger (xi_logger_fetch)
#		define logger_get_conf(logger)             xi_logger_get_conf(logger)        ///< Get the options of logger
#		define logger_set_conf(logger, opt)        xi_logger_set_conf(logger, opt)   ///< Set the options of logger
#		define log_trace(logger, format, ...)      xi_logger_write(logger, XI_LOG_LEVEL_TRACE, __FILE__, __FUNCTION__, __LINE__, format, ## __VA_ARGS__)  ///< Print the TRACE level log
#		define log_debug(logger, format, ...)      xi_logger_write(logger, XI_LOG_LEVEL_DEBUG, __FILE__, __FUNCTION__, __LINE__, format, ## __VA_ARGS__)  ///< Print the DEBUG level log
#		define log_info(logger, format, ...)       xi_logger_write(logger, XI_LOG_LEVEL_INFO,  __FILE__, __FUNCTION__, __LINE__, format, ## __VA_ARGS__)  ///< Print the INFO  level log
#		define log_warn(logger, format, ...)       xi_logger_write(logger, XI_LOG_LEVEL_WARN,  __FILE__, __FUNCTION__, __LINE__, format, ## __VA_ARGS__)  ///< Print the WARN  level log
#		define log_error(logger, format, ...)      xi_logger_write(logger, XI_LOG_LEVEL_ERROR, __FILE__, __FUNCTION__, __LINE__, format, ## __VA_ARGS__)  ///< Print the ERROR level log
#		define log_fatal(logger, format, ...)      xi_logger_write(logger, XI_LOG_LEVEL_FATAL, __FILE__, __FUNCTION__, __LINE__, format, ## __VA_ARGS__)  ///< Print the FATAL level log
#		define log_print(logger, format, ...)      xi_logger_write(logger, XI_LOG_LEVEL_PRINT, __FILE__, __FUNCTION__, __LINE__, format, ## __VA_ARGS__)  ///< Print the PRINT level log
#else // !XCFG_DEBUG
#		define logger_get(id)                      NULL  ///< Get the logger (xi_logger_fetch)
#		define logger_get_conf(logger)             {0}   ///< Get the options of logger
#		define logger_set_conf(logger, opt)              ///< Set the options of logger
#		define log_trace(logger, format, ...)            ///< Print the TRACE level log
#		define log_debug(logger, format, ...)            ///< Print the DEBUG level log
#		define log_info(logger, format, ...)             ///< Print the INFO  level log
#		define log_warn(logger, format, ...)             ///< Print the WARN  level log
#		define log_error(logger, format, ...)            ///< Print the ERROR level log
#		define log_fatal(logger, format, ...)            ///< Print the FATAL level log
#		define log_print(logger, format, ...)            ///< Print the PRINT level log
#endif // XCFG_DEBUG

/**
 * @}  // end of xi_log
 */

/**
* End Declaration
*/
_XI_EXTERN_C_END

#endif // _XI_LOG_H_
