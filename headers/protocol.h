/**
 * @file protocol.h
 * @author Niklas Seppälä (you@domain.com)
 * @brief TODO: EXPLAIN
 * 
 *  HEADER [
 *      size = 16-bytes
 *      COMMAND      [ size = 2-bytes  ] ushort
 *      SENDER       [ size = 12-bytes ] char buffer
 *      CONTENT-SIZE [ size = 2-bytes  ] ushort
 *  ]
 * 
 *  CONTENT [
 *      size = defined in CONTENT-SIZE
 * ]
 * 
 * @version 0.1
 * @date 2021-01-21
 */

#if !defined PROTOCOL_H
#define PROTOCOL_H
#include <stdint.h>


enum CMD {
    INV = 0xEd,
    ADD = 0xC9,
    LOG = 0xE2,
    RMV = 0xF5,
    OK  = 0x9A
};

#define CMD_INV "INV"
#define CMD_ADD "ADD"
#define CMD_LOG "LOG"
#define CMD_RMV "RMV"
#define CMD_OK "OK"

#define HEADER_SIZE 16
#define H_SENDER_SIZE 12

struct header {
    uint16_t cmd;
    char sender[H_SENDER_SIZE];
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
int create_header(struct header *header, uint8_t cmd, char *sender, uint8_t content_size);

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
