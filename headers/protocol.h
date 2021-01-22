/**
 * @file protocol.h
 * @author Niklas Seppälä
 * @brief C module for working with custom made data transfer protocol.
 *        Protocol utilizes TCP/IP protocol.
 * 
 *        Communication starts with header package:
 *          Header package:
 *              size : 16-bytes
 *              COMMAND
 *                  use    : describe what is the purpose
 *                           of the message.
 *                           @see enum CMD
 *                  type   : uint16_t
 *                  size   : 2 bytes
 *                  offset : 0
 *              SENDER
 *                  use    : Identify sender
 *                  type   : byte buffer
 *                  size   : 12 bytes
 *                  offset : 2
 *              CONTENT-SIZE
 *                  use    : inform the receiver the size
 *                           of the main package.
 *                  type   : uint16_t
 *                  size   : 2 bytes
 *                  offset : 14
 *        And the main content package is sent (if desired)
 *        after confirmation from receiver:
 *          Main package:
 *              type : byte buffer
 *              size : defined in header
 * @version 0.1
 * @date 2021-01-22
 */

#if !defined PROTOCOL_H
#define PROTOCOL_H
#include <stdint.h>

/**
 * @brief Command enum value is a sum
 *        of it's char representation
 */
enum CMD {
    INV = 0xEd,
    ADD = 0xC9,
    LOG = 0xE2,
    RMV = 0xF5,
    OK  = 0x9A,
};

#define CMD_INV "INV"
#define CMD_ADD "ADD"
#define CMD_LOG "LOG"
#define CMD_RMV "RMV"
#define CMD_OK "OK"

#define HEADER_SIZE 16
#define SENDER_SIZE 12

/**
 * @brief header package with
 *        protocol correct size and
 *        data alignment. Can be sent over
 *        socket as is.
 * 
 */
struct header {
    uint16_t cmd;
    char sender[SENDER_SIZE];
    uint16_t size;
};

/**
 * @brief Sets header struct fields.
 * 
 * @param header header struct
 * @param cmd CMD enum
 * @param sender sender string
 * @param content_size length of the content string.
 * @return int SUCCESS if OK
 */
int create_header(struct header *header, uint16_t cmd,
    char *sender, uint16_t content_size);

/**
 * @brief Cast protocol header to char pointer
 *        after setting network endianness.
 * 
 * @param header header struct
 */
char * header_to_network(struct header *header);

/**
 * @brief Casts network buffer to header struct
 *        and corrects endianness.
 * 
 * @param buffer network buffer, same size as header struct.
 * @return struct header* Pointer to header struct
 */
struct header *read_header(char *buffer);

/**
 * @brief Converts command enum to mapped
 *        string.
 * 
 * @param cmd Command enum
 * @return char* corresponding string
 */
char *enum_to_str(int cmd);

/**
 * @brief Converts command string to mapped
 *        enum value.
 * 
 * @param cmd command string
 * @return int corresponding enum value
 */
int str_to_enum(char *cmd);

#endif
