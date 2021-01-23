#include <stdlib.h>
#include <unistd.h>

#include "lib/io.h"

static void cleanup(void) {
    fclose(ERR_STREAM);
    fclose(IN_STREAM);
    fclose(OUT_STREAM);
}

void shutdown(int r_code) {
    cleanup();
    return r_code;
}
