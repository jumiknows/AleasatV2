/**
 * @file gps_defs.h
 * @brief Venus838FLPx_DS_v7 GPS defs
 */

#ifndef GPS_DEFS_H_
#define GPS_DEFS_H_

/******************************************************************************/
/*                               D E F I N E S                                */
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
 * @brief GPS restart types.
 */
typedef enum {
    GPS_RST_HOT_START  = 0x1U, ///< System Reset, Hot Start.
    GPS_RST_WARM_START = 0x2U, ///< System reset, Warm Start.
    GPS_RST_COLD_START = 0x3U, ///< System reset, Cold start.
} gps_restart_modes_t;

#define GPS_YUL_LAT 45508  // Hundreds of a degree
#define GPS_YUL_LON -7356 // Hundreds of a degree
#define GPS_YUL_ALT 223    // meters

#define GPS_MSG_START_SEQ_0 0xA0
#define GPS_MSG_START_SEQ_1 0xA1
#define GPS_MSG_END_SEQ_0   0x0D
#define GPS_MSG_END_SEQ_1   0x0A

#define GPS_RESTART_MSG_LEN 22

/******************************************************************************/
/*                              T Y P E D E F S                               */
/******************************************************************************/

/**
 * @brief GPS receiver errors.
 */
typedef enum {
    GPS_SUCCESS      = 0,  // GPS receiver has received our configuration command and responded with an ACK
    GPS_RECV_NACK    = -1, // GPS receiver has received our configuration command and responded with a NACK
    GPS_BAD_RESPONSE = -2, // GPS receiver is giving a non-sense response
    GPS_RTC_ERROR    = -4, // Received an RTC failure.
} gps_err_t;

#endif /* GPS_DEFS_H_ */
