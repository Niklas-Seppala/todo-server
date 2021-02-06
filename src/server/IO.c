#include <string.h>
#include <stdlib.h>

#include "lib/network.h"
#include "server/IO.h"
#include "server/database.h"

int io_setup(void) {
    FILE *err_log = fopen("./logs/server.log", "a");
    if (err_log < 0) {
        fprintf(stderr, "%s\n", "Invalid log stream");
        return ERROR;
    }
    LOG_STREAM = err_log;
    return SUCCESS;
}

void io_set_default_streams(void)
{
    OUT_STREAM = stdout;
    IN_STREAM = stdin;
    LOG_STREAM = stderr;
}

void io_setup_fail(void) {
    fprintf(OUT_STREAM, "%s\n",
        "Could not start the server, see log for errors");
}

void io_setup_success(const struct sockaddr *server_addr) {
    struct readable_addr addr;
    addr_to_readable(server_addr, &addr);
    log_info("Configurations successful!");
    vflog_info("Server running on %s:%s", addr.ip_addr, addr.port);
}

int validate_args(int argc, const char **argv) {
    if (argc < 2) {
        fprintf(stdout, "%s\n\t %s <SERVER PORT> <(DB_HOST)> <(DB_USER)> <(DB_PASSWORD)> <(DB_NAME)>\n",
            "Invalid parameter count", *argv);
        return ERROR;
    }
    return SUCCESS;
}

void query_db_info(db_info_t *info) {
    char user[DB_INFO_STR_LEN];
    char host[DB_INFO_STR_LEN];
    char pw[DB_INFO_STR_LEN];
    char db_name[DB_INFO_STR_LEN];

    fprintf(OUT_STREAM, "MySql database connection host: ");
    fgets(host, DB_INFO_STR_LEN, IN_STREAM);
    fprintf(OUT_STREAM, "MySQL database username: ");
    fgets(user, DB_INFO_STR_LEN, IN_STREAM);
    fprintf(OUT_STREAM, "MySQL password: ");
    fgets(pw, DB_INFO_STR_LEN, IN_STREAM);
    fprintf(OUT_STREAM, "MySQL database name: ");
    fgets(db_name, DB_INFO_STR_LEN, IN_STREAM);

    db_create_info(info, user, db_name, host, pw, 0);
}