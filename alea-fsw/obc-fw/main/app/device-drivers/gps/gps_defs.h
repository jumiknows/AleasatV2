/**
 * @file gps_defs.h
 * @brief Venus838FLPx_DS_v7 GPS defs
 */

#ifndef GPS_DEFS_H_
#define GPS_DEFS_H_

// FreeRTOS
#include "rtos.h"

// std lib
#include <stdint.h>
#include <stdbool.h>

/******************************************************************************/
/*                               D E F I N E S                                */
/******************************************************************************/

/**
 * @brief The maximum length of a GPS command.
 */
#define MAX_GPS_LEN_BYTES 200

/**
 * @brief Timeout for a GPS timeout msg.
 */
#define GPS_REPLY_TIMEOUT_MS 100

#define GPS_RESTART_MSG_LEN 22
#define GPS_MSG_OVERHEAD 7 // GPS MSG overhead in bytes.
#define GPS_HENN_LAT 4926 // Hundreds of a degree
#define GPS_HENN_LON -12325 // Hundreds of a degree
#define GPS_HENN_ALT 223   // meters

#define GPS_MSG_START_SEQ_0   ((uint8_t)0xa0)
#define GPS_MSG_START_SEQ_1   ((uint8_t)0xa1)
#define GPS_NMEA_MSG_START    ((uint8_t)0x24) // This is '$'
#define GPS_MSG_END_SEQ_0     ((uint8_t)0x0d) // This is '\r'
#define GPS_MSG_END_SEQ_1     ((uint8_t)0x0a) // This is '\n'
#define GPS_CMD_LEN_POS_0     2
#define GPS_CMD_LEN_POS_1     3
#define GPS_CMD_ID_POS        4
#define GPS_CMD_PAYLOAD_START 4
#define GPS_ACK_PKT_SIZE      9


/******************************************************************************/
/*                              T Y P E D E F S                               */
/******************************************************************************/

/**
 * @brief Binary input list to send to the GPS reciever.
 */
typedef enum {
    GPS_SYS_CMD_RESART                 = 0x01U, ///< Force System to restart.
    GPS_SYS_CMD_QUERY_SW_VER           = 0x02U, ///< Query revision information of software.
    GPS_SYS_CMD_QUERY_SW_CRC           = 0x03U, ///< Query the CRC of the software.
    GPS_SYS_CMD_SET_FACTORY_DEFAULT    = 0x04U, ///< Set system to factory default values.
    GPS_SYS_CMD_SET_SERIAL_PORT        = 0x05U, ///< Set up the serial port COM, baud rate, data bits, stop bits and parity.
    GPS_SYS_CMD_SET_NMEA               = 0x08U, ///< Configure NMEA output message.
    GPS_SYS_CMD_QUERY_NMEA             = 0x64U, ///< Query NMEA output message settings.
    GPS_SYS_CMD_SET_MSG_TYPE           = 0x09U, ///< Configure and select the output message type.
    GPS_SYS_CMD_SW_IMG_DOWNLOAD        = 0x0BU, ///< Software image download to system flash.
    GPS_SYS_CMD_SET_PWR_MODE           = 0x0CU, ///< Set system power mode.
    GPS_SYS_CMD_SET_POS_UPDATE_RATE    = 0x0EU, ///< Configure the position update rate of the GNSS system.
    GPS_SYS_CMD_QUERY_POS_UPDATE_RATE  = 0x10U, ///< Query the position update rate of the GNSS system.
    GPS_SYS_CMD_SET_NAV_DATA_MSG_INTER = 0x11U, ///< Configure the navigation output message interval.
    GPS_SYS_CMD_QUERY_PWR_MODE         = 0x15U, ///< Query the power mode status of the GNSS receiver.
} gps_sys_cmd_input_t;

/**
 * @brief Binary input GNSS Messages.
 */
typedef enum {
    GPS_GNSS_CMD_SET_DATUM            = 0x29U, ///< Configure Datum of the GNSS receiver.
    GPS_GNSS_CMD_SET_DOP_MASK         = 0x2AU, ///< Configure DOP Mask.
    GPS_GNSS_CMD_SET_ELEV_CNR_MASK    = 0x2BU, ///< Configure values of Elevation and CNR Mask.
    GPS_GNSS_CMD_QUERY_DATUM          = 0x2DU, ///< Query Datum.
    GPS_GNSS_CMD_QUERY_DOP_MASK       = 0x2EU, ///< Query the information of DOP mask used by GNSS receiver.
    GPS_GNSS_CMD_QUERY_ELEV_CNR_MASK  = 0x2FU, ///< Query the values of elevation mask and CNR mask used by the GNSS receiver.
    GPS_GNSS_CMD_QUERY_GPS_EPHEM      = 0x30U, ///< Retrieve GPS ephemeris data of the GNSS receiver.
    GPS_GNSS_CMD_SET_POS_PIN          = 0x39U, ///< Enable of disable position pinning of GNSS receiver.
    GPS_GNSS_CMD_QUERY_POS_PIN        = 0x3AU, ///< Query position pinning status of the GNSS receiver.
    GPS_GNSS_CMD_SET_POS_PIN_PARAM    = 0x3BU, ///< Set position pining status of the GNSS receiver.
    GPS_GNSS_CMD_SET_GPS_EPHEMERIS    = 0x41U, ///< Set GPS ephemeris data to the GNSS receiver.
    GPS_GNSS_CMD_QUERY_1PPS_TIMING    = 0x44U, ///< Query 1PPS timing of the GNSS receiver.
    GPS_GNSS_CMD_SET_1PPS_CABLE_DELAY = 0x45U, ///< Configure cable delay of 1PPS timing.
    GPS_GNSS_CMD_GET_1PPS_CABLE_DELAY = 0x46U, ///< Query 1PPS cable delay of GNSS reciever.
    GPS_GNSS_CMD_SET_NMEA_TALKER_ID   = 0x4BU, ///< Configure NMEA talker ID of GNSS receiver.
    GPS_GNSS_CMD_GET_NMEA_TALKER_ID   = 0x4FU, ///< Query NMEA talker ID of GNSS receiver.
    GPS_GNSS_CMD_GET_ALMANAC          = 0x50U, ///< Retrieve GPS almanac data of the GNSS receiver.
    GPS_GNSS_CMD_SET_ALMANAC          = 0x51U, ///< Set GPS almanac.
    GPS_GNSS_CMD_SET_1PPS_TIMING      = 0x54U, ///< Configure 1PPS timing of GNSS receiver.
} gps_gnss_cmd_input_t;

/**
 * @brief Output system messages.
 */
typedef enum {
    GPS_SYS_MSG_OUTPUT_SW_VER   = 0x80U, ///< Software revision of the receiver.
    GPS_SYS_MSG_OUPUT_CRC       = 0x81U, ///< Software CRC of the reciever.
    GPS_SYS_MSG_ACK             = 0x83U, ///< GPS ACK to a successfull input message.
    GPS_SYS_MSG_NACK            = 0x84U, ///< GPS Response to an unsuccessful input message.
    GPS_SYS_MSG_POS_UPDATE_RATE = 0x86U, ///< GPS position update rate of GNSS system.
    GPS_SYS_MSG_NMEA_TALKER_ID  = 0x93U, ///< NMEA talker ID of GNSS receiver.
    GPS_SYS_MSG_PAYLOAD         = 0x94U, ///< Message I made up to identify a payload.
} gps_sys_msg_output_t;

/**
 * @brief Output GPS messages.
 */
typedef enum {
    GPS_GNSS_MSG_NAV_DATA          = 0xA8U, ///< Output user nagivation data in binary format.
    GPS_GNSS_MSG_GNSS_DATUM        = 0xAEU, ///< GNSS Datum.
    GPS_GNSS_MSG_DOP_MASK          = 0xAFU, ///< GPS DOP mask.
    GPS_GNSS_MSG_ELEV_AND_CNR_MASK = 0xB0U, ///< Elevation and CNR Mask used by the GNSS receiver.
    GPS_GNSS_MSG_EPHEM_DATA        = 0xB1U, ///< GPS ephemeris data of the GNSS receiver.
    GPS_GNSS_MSG_POS_PIN_STAT      = 0xB4U, ///< Position pinning status of the GNSS receiver.
    GPS_GNSS_MSG_PWR_MODE_STAT     = 0xB9U, ///< Power mode status of GNSS receiver.
    GPS_GNSS_MSG_1PPS_CABLE_DELAY  = 0xBBU, ///< 1PPS cable delay of the GNSS receiver.
    GPS_GNSS_MSG_ALAMAC_DATA       = 0xBEU, ///< GPS almanac data of the GNSS receiver.
    GPS_GNSS_MSG_1PPS_TIMING       = 0xC2U, ///< 1PPS timing information of the GNSS receiver.
} gps_gnss_msg_output_t;

/**
 * @brief Types of writes that can be done on the GPS.
*/
typedef enum {
    GPS_WRITE_TO_SRAM = 0x00,
    GPS_WRITE_TO_SRAM_AND_FLASH = 0x01,
    GPS_WRITE_TEMP = 0x02,
} gps_write_method_t;

/**
 * @brief GPS restart types.
 */
typedef enum {
    GPS_RST_HOT_START  = 0x1U, ///< System Reset, Hot Start.
    GPS_RST_WARM_START = 0x2U, ///< System reset, Warm Start.
    GPS_RST_COLD_START = 0x3U, ///< System reset, Cold start.
} gps_restart_modes_t;

/**
 * @brief GPS receiver errors.
 */
typedef enum {
    GPS_SUCCESS      = 0,  // GPS receiver has received our configuration command and responded with an ACK
    GPS_RECV_NACK    = -1, // GPS receiver has received our configuration command and responded with a NACK
    GPS_BAD_RESPONSE = -2, // GPS receiver is giving a non-sense response
    GPS_RTC_ERROR    = -4, // Received an RTC failure.
    GPS_BAD_ARGUMENTS = -5, // Passed erroneous commands.
    GPS_MSG_TIMEOUT = -6,   // Didn't get a reply from the GPS.
    GPS_MUTEX_TIMEOUT = -7  // Mutex timed out.
} gps_err_t;

/**
 * @brief GPS configure serial port.
 */
typedef enum {
    GPS_CONFIG_SERIAL_4800   = 0x0U, ///< 4800 baud.
    GPS_CONFIG_SERIAL_9600   = 0x1U, ///< 9600 baud.
    GPS_CONFIG_SERIAL_19200  = 0x2U, ///< 19200 baud.
    GPS_CONFIG_SERIAL_38400  = 0x3U, ///< 38400 baud.
    GPS_CONFIG_SERIAL_57600  = 0x4U, ///< 57600 baud.
    GPS_CONFIG_SERIAL_115200 = 0x5U, ///< 115200 baud.
    GPS_CONFIG_SERIAL_230400 = 0x6U, ///< 230400 baud.
    GPS_CONFIG_SERIAL_460800 = 0x7U, ///< 460800 baud.
    GPS_CONFIG_SERIAL_921600 = 0x8U, ///< 921600 baud.
} gps_config_serial_baud_t;

/**
 * @brief GPS Message types.
 */
typedef enum {
    GPS_CONFIG_MSG_TYPE_NONE   = 0x0U, /// No output.
    GPS_CONFIG_MSG_TYPE_NMEA   = 0x1U, /// NMEA Message.
    GPS_CONFIG_MSG_TYPE_BINARY = 0x2U, /// Binary Message.
} gps_msg_type_t;

/**
 * @brief Valid GPS update rates in Hz.
 */
typedef enum {
    GPS_POS_UPDATE_1HZ  = 0x01U,
    GPS_POS_UPDATE_2HZ  = 0x02U,
    GPS_POS_UPDATE_4HZ  = 0x04U,
    GPS_POS_UPDATE_5HZ  = 0x05U,
    GPS_POS_UPDATE_8HZ  = 0x08U,
    GPS_POS_UPDATE_10HZ = 0x0AU,
    GPS_POS_UPDATE_20HZ = 0x14U,
    GPS_POS_UPDATE_25HZ = 0x19U,
    GPS_POS_UPDATE_40HZ = 0x28U,
    GPS_POS_UPDATE_50HZ = 0x32U,
} gps_pos_update_rate_t;

/**
 * @brief GPS reply packet structure.
*/
typedef struct {
    bool ack;
    uint8_t id;
    uint16_t len;
    uint8_t *payload;
    uint8_t payload_len;
} gps_packet_t;

/**
 * @brief GPS parsing errors.
 */
typedef enum {
    GPS_PARSE_OK = 0,
    GPS_INTERNAL_ERROR = -1,
    GPS_PARSE_TOO_SHORT = -2,
    // Means reply was larger than the allocated buffer.
    GPS_PARSE_MAX_SIZE = -3,
    GPS_PARSE_MALFORMED = -4,
} gps_parsing_err_t;

/**
 * GPS State struct
*/
typedef struct {
    TaskHandle_t xGpsTaskToNotify;
    uint8_t rx_buf[MAX_GPS_LEN_BYTES];
    uint8_t rx_buf_len;
    bool expect_extended_reply;
    bool success;
    SemaphoreHandle_t xGpsMutex;
} gps_state_t;


#endif /* GPS_DEFS_H_ */
