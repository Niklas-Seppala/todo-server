#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <time.h>

#include "lib/IO.h"
#include "lib/common.h"

extern int errno;
#ifndef DEBUG
static const char *err_header = "[ERROR] ";
static const char *err_format = "%s\n";
#else
static const char *err_header =
    "[ERROR] [DEBUG] in function: %s()\n\t\t" \
    "at file:   %s\n\t\t" \
    "at line:   %d\n";
static const char *err_format = "\t\t%s\n";
#endif // !DEBUG

char *time_str(void) {
    const int LEN = 80;
    char *buffer = malloc(LEN);
    time_t now = time(NULL);
    strftime(buffer, LEN, "%Y-%m-%d %H:%M:%S", localtime(&now));
    return buffer;
}

void io_config(FILE *error, FILE *out, FILE *in) {
    LOG_STREAM = error;
    OUT_STREAM = out;
    IN_STREAM = in;
}

void log_new_session(void) {
    fprintf(LOG_STREAM, "\n");
    log_info("Starting new session");
}

void log_warn(const char *warn) {
    char *t_str = time_str();
    fprintf(LOG_STREAM, "%s - [WARN]: %s\n", t_str, warn);
    fflush(LOG_STREAM);
    safe_free((void **)&t_str);
}

void log_info(const char *info) {
    char *t_str = time_str();
    fprintf(LOG_STREAM, "%s - [INFO]: %s\n", t_str, info);
    fflush(LOG_STREAM);
    safe_free((void **)&t_str);
}

void error(const char *fname, const char *filename,
    const int line, const char *details, const int flags) {

    char *t_str = time_str();
    fprintf(LOG_STREAM, "%s - ", t_str);
    safe_free((void **)&t_str);


    // If compiled with DEBUG, give extra details about the
    // Source of the error.
#ifdef DEBUG
    fprintf(LOG_STREAM, err_header, fname, filename, line);
#else
    fprintf(LOG_STREAM, "%s", err_header);
#endif

    // Print either system error, or user defined error.
    if (flags & SYS_ERROR) {
        fprintf(LOG_STREAM, err_format, strerror(errno));
    } else if (details) {
        fprintf(LOG_STREAM, err_format, details);
    }

    fflush(LOG_STREAM);
    // Exit application if FATAL flag was set.
    if (flags & FATAL) {
        exit(EXIT_FAILURE);
    }
}