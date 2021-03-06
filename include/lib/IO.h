/**
 * @file io.h
 * @author Niklas Seppälä (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2021-01-23
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#if !defined IO_H
#define IO_H
#include <stdio.h>

#define SYS_ERROR 2

// Console colors
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_RESET   "\x1b[0m"

FILE *IN_STREAM;
FILE *LOG_STREAM;
FILE *OUT_STREAM;

/**
 * @brief Print error message with source info.
 * 
 * @param details user specified details about error
 * @param flags define what to do @see SYS_ERR, FATAL
 */
#define log_error(details, flags) \
    error(__func__, __FILE__, __LINE__, details, flags)

/**
 * @brief Configures global io streams for application.
 * 
 * @param error error stream
 * @param out output stream
 * @param in input stream
 */
void io_config(FILE *error, FILE *out, FILE *in);

/**
 * @brief Print error message.
 * 
 * @param funcname Source function name
 * @param filename Source file name
 * @param line_num Source line number
 * @param details user specified details about error
 * @param flags define what to do, @see SYS_ERR, FATAL
 */
void error(const char *funcname, const char *filename,
    const int line_num, const char *details, const int flags);

/**
 * @brief prints info with timestamp to LOG_STREAM.
 * 
 * @param info info message
 */
void log_info(const char *info);

/**
 * @brief Variadic format log.
 * 
 * @param fmt format string
 * @param ... vargs
 */
void vflog_info(const char *fmt, ...);

/**
 * @brief prints warning message with timestamp to LOG_STREAM.
 * 
 * @param warn warning message
 */
void log_warn(const char *warn);

/**
 * @brief Get the time str object
 * 
 * @return char* pointer to string containing datetime
 */
char *time_str(void);

#endif