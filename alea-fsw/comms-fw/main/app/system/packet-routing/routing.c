#include "hwid.h"
#include "routing.h"

__xdata dest_hwid_range_t hwid_table[NUM_DESTS] = {
    // TODO: TBD
    {HWID_OBC_START, HWID_OBC_END},  // OBC
    {0, 0},  // Comms, correct values set by init code
    {HWID_GS1_START, HWID_GS1_END},  // GS1
    {HWID_GS2_START, HWID_GS2_END},  // GS2
    {HWID_ARO_START, HWID_ARO_END},  // ARO
    {HWID_LOCAL_START, HWID_LOCAL_END}  // local debug
};

// see pages of https://drive.google.com/file/d/1yhzYxwlpqrsQnbvlC2NSqwu4_J33Uf_J/view?usp=sharing
// TODO: Allow us to send msgs to AROs?
const __xdata routing_action_t routing_table[NUM_DESTS][NUM_DESTS] = {  // [src][dest]
    {ROUTE_FORWARD, ROUTE_HANDLE, ROUTE_FORWARD, ROUTE_FORWARD, ROUTE_FORWARD, ROUTE_FORWARD},
    {ROUTE_DROP,    ROUTE_DROP,   ROUTE_DROP,    ROUTE_DROP,    ROUTE_DROP,    ROUTE_DROP   },
    {ROUTE_FORWARD, ROUTE_HANDLE, ROUTE_FORWARD, ROUTE_FORWARD, ROUTE_DROP,    ROUTE_FORWARD},
    {ROUTE_FORWARD, ROUTE_HANDLE, ROUTE_FORWARD, ROUTE_FORWARD, ROUTE_DROP,    ROUTE_FORWARD},
    {ROUTE_FORWARD, ROUTE_DROP,   ROUTE_DROP,    ROUTE_DROP,    ROUTE_DROP,    ROUTE_DROP   },
    {ROUTE_FORWARD, ROUTE_HANDLE, ROUTE_FORWARD, ROUTE_FORWARD, ROUTE_DROP,    ROUTE_FORWARD},
};

// TODO: also allow GS to be connected via UART
// This would require function to change the interface table and also a new openlst command
// We can also have a variable called "src_iface" associated with each received packet
const __xdata interface_id_t dest_interface_table[NUM_DESTS] = {
    INTERFACE_SPI,
    INTERFACE_INVALID,
    INTERFACE_RADIO,
    INTERFACE_RADIO,
    INTERFACE_RADIO,
    INTERFACE_UART
};

void hwid_table_init() {
    hwid_table[DEST_COMMS].start = hwid_flash;
    hwid_table[DEST_COMMS].end = hwid_flash;
}

dest_t get_dest_from_hwid(hwid_t hwid) {
    // TODO: check performance and maybe make faster, by optimizing table values and changing algorithm
    dest_t dest = 0;

    while (dest < NUM_DESTS) {
        if ((hwid >= hwid_table[dest].start) && (hwid <= hwid_table[dest].end)) {
            break;
        }
        ++dest;
    }

    if (dest == NUM_DESTS) {
        dest = DEST_INVALID;
    }
    return dest;
}

__xdata uint16_t dest_seqnum_table[NUM_DESTS] = {
    0,
    0,  // not relevant, COMMS can never send to itself
    0,
    0,
    0,  // not relevant, ARO can never send to COMMS
    0  // not relevant, any seqnum will be accepted
};
