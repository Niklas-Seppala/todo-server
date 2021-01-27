/**
 * @file server_IO.h
 * @author Niklas Seppälä
 * @brief Module that provides IO-communication functions
 *        to server application.
 * @version 0.1
 * @date 2021-01-27
 * 
 * @copyright Copyright (c) 2021
 */

#if !defined(SERVER_IO)
#define SERVER_IO

#include "lib/IO.h"

/**
 * @brief Server setup failed, prints instruction
 *        message to OUT_STREAM
 */
void io_setup_fail(void);

/**
 * @brief Sets std streams as default streams
 *        to LOG_STREAM, IN_STREAM, OUT_STREAM
 */
void io_set_default_streams(void);

/**
 * @brief Server setup was success, print
 *        server address to LOG_STREAM
 */
void io_setup_success(void);

/**
 * @brief setup LOG_STREAM to defined
 *        FILE
 * 
 * @return int SUCCESS or ERROR
 */
int io_setup(void);

/**
 * @brief Validates CLI arguments.
 * 
 * @param argc argument count
 * @param argv argument vector
 * @return int SUCCESS or ERROR
 */
int validate_args(int argc, const char **argv);

#endif // SERVER_IO
