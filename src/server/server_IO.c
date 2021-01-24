#include <string.h>
#include <stdlib.h>

#include "lib/common.h"
#include "server/server_IO.h"
#include "lib/IO.h"

int config_server_io(void) {
    FILE *err_log = fopen("./logs/server.log", "a");
    if (err_log < 0) {
        fprintf(stderr, "%s\n", "Invalid log stream");
        return ERROR;
    }

    LOG_STREAM = err_log;
    OUT_STREAM = stdout;
    IN_STREAM = stdin;
    return SUCCESS;
}

int validate_args(int argc, const char **argv) {
    if (argc < 2) {
        fprintf(stdout, "%s\n\t %s <PORT>\n",
            "Invalid parameter count", *argv);
        return ERROR;
    }
    return SUCCESS;
}