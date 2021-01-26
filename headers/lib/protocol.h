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
 *        of it's char representation.
 * 
 *        VAL = VALID,
 *        INV = INVALID,
 *        ADD = ADD STUFF,
 *        LOG = TELL ME STUFF,
 *        RMV = REMOVE STUFF
 */

/**
 * @brief Code for invalid network message
 */
#define INV 0xEd
/**
 * @brief Code for requesting to
 *        add stuff to server.
 */
#define ADD 0xC9
/**
 * @brief Code for request to log user data
 *        in server.
 */
#define LOG 0xE2
/**
 * @brief Code for request to remove user data
 *        from server.
 */
#define RMV 0xF5
/**
 * @brief Code for valid network message
 */
#define VAL 0xE3

#define CMD_INV "INV"
#define CMD_ADD "ADD"
#define CMD_LOG "LOG"
#define CMD_RMV "RMV"
#define CMD_VAL "VAL"

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
 * @brief 
 * 
 * @param main_pkg 
 * @param curr_size 
 * @param new_size 
 * @return int 
 */
int alloc_main_pkg(char **main_pkg, size_t *curr_size,
    const size_t new_size);

/**
 * @brief 
 * 
 * @param pkg 
 * @return size_t 
 */
size_t main_pkg_size(char *pkg);

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
 * @brief Sets network endianness.
 * 
 * @param header header struct
 */
void header_to_network(struct header *header);

/**
 * @brief 
 * 
 * @param header 
 */
void header_from_network(struct header* header);

/**
 * @brief Copies the buffer contents to header struct
 *        and return a pointer. Buffer remains the same.
 *        ALLOCATES HEAP MEMORY!
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
