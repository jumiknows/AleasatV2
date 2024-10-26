#ifndef _ROUTING_H
#define _ROUTING_H

#include "hwid.h"

typedef enum {
    DEST_OBC = 0,
    DEST_COMMS = 1,
    DEST_GS1 = 2,
    DEST_GS2 = 3,
    DEST_ARO = 4,
    DEST_LOCAL_DEBUG = 5,

    NUM_DESTS = 6,

    DEST_INVALID = 255
} dest_t;

typedef enum {
    ROUTE_DROP = 0,
    ROUTE_FORWARD = 1,
    ROUTE_HANDLE = 2
} routing_action_t;

typedef enum {
    INTERFACE_UART = 0,
    INTERFACE_SPI = 1,
    INTERFACE_RADIO = 2,
    INTERFACE_INVALID = 255
} interface_id_t;

typedef struct dest_hwid_range {
    hwid_t start;
    hwid_t end;  // inclusive
} dest_hwid_range_t;


extern __xdata dest_hwid_range_t hwid_table[NUM_DESTS];
extern __xdata uint16_t dest_seqnum_table[NUM_DESTS];
extern const __xdata routing_action_t routing_table[NUM_DESTS][NUM_DESTS];
extern const __xdata interface_id_t dest_interface_table[NUM_DESTS];


void hwid_table_init();

dest_t get_dest_from_hwid(hwid_t hwid);


#endif /* _ROUTING_H */
