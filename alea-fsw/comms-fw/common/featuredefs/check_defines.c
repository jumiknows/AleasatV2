#include <assert.h>

// #define xstr(s) str(s)
// #define str(s) #s

// #include "board_defaults.h"


#include "commands.h"
#include "radio.h"

// use this to print size of type at compile time, replace xxx with type
// char (*__kaboom)[sizeof( xxx )] = 1;

_Static_assert(LOCAL_PACKET_MAX_LEN == sizeof(command_header_t) + LOCAL_PACKET_DATA_MAX_NUM_BYTES, "bad LOCAL_PACKET_MAX_LEN");
_Static_assert(sizeof(command_t) == LOCAL_PACKET_MAX_LEN, "bad command_t size");


_Static_assert(RF_BUFFER_SIZE_BYTES == 1 + LOCAL_PACKET_MAX_LEN + sizeof(rf_command_footer_t), "bad RF_BUFFER_SIZE_BYTES");
