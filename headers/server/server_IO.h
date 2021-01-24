#if !defined(SERVER_IO)
#define SERVER_IO

#include "lib/IO.h"

/**
 * @brief Set server io streams
 * 
 */
int config_server_io(void);

/**
 * @brief 
 * 
 * @param argc 
 * @param argv 
 * @return int 
 */
int validate_args(int argc, const char **argv);

#endif // SERVER_IO
