
/**
 * @file cmd_impl.c
 * @brief Implementation of GPS commands.
 */

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

#include "cmd_sys_gen.h"
#include "cmd_sys.h"

// OBC
#include "obc_gps.h"
#include "obc_featuredefs.h"

// Logger
#include "logger.h"

// Utils
#include "obc_utils.h"

// FreeRTOS
#include "rtos.h"

/******************************************************************************/
/*            P R I V A T E  F U N C T I O N  P R O T O T Y P E S             */
/******************************************************************************/

cmd_sys_resp_code_t gps_to_cmd_sys_resp(gps_err_t gps_err);
gps_write_method_t parse_write_method(int index);

/******************************************************************************/
/*                       P U B L I C  F U N C T I O N S                       */
/******************************************************************************/

cmd_sys_resp_code_t cmd_impl_GPS_RESTART(const cmd_sys_cmd_t *cmd, cmd_GPS_RESTART_args_t *args) {
    gps_restart_modes_t gps_restart_mode;

    if (args->start_mode == 1) {
        gps_restart_mode = GPS_RST_HOT_START;
    } else if (args->start_mode == 2) {
        gps_restart_mode = GPS_RST_WARM_START;
    } else if (args->start_mode == 3) {
        gps_restart_mode = GPS_RST_COLD_START;
    } else {
        return CMD_SYS_RESP_CODE_ERROR;
    }

    gps_err_t err = gps_restart_receiver(gps_restart_mode);
    return gps_to_cmd_sys_resp(err);
}

cmd_sys_resp_code_t cmd_impl_GPS_QUERY_SW_VER(const cmd_sys_cmd_t *cmd, cmd_GPS_QUERY_SW_VER_resp_t *resp) {
    gps_err_t err = gps_query_software_version((uint8_t *)&resp->gps_software_version);
    return gps_to_cmd_sys_resp(err);
}

cmd_sys_resp_code_t cmd_impl_GPS_SET_FACTORY_SETTINGS(const cmd_sys_cmd_t *cmd) {
    gps_err_t err = gps_set_factory_settings();
    return gps_to_cmd_sys_resp(err);
}

cmd_sys_resp_code_t cmd_impl_GPS_CONFIGURE_POWER_MODE(const cmd_sys_cmd_t *cmd, cmd_GPS_CONFIGURE_POWER_MODE_args_t *args) {
    gps_write_method_t write_method = parse_write_method(args->write_settings);
    gps_err_t err                   = gps_configure_power_mode(write_method, args->enable_power_save);
    return gps_to_cmd_sys_resp(err);
}

cmd_sys_resp_code_t cmd_impl_GPS_QUERY_POWER_MODE(const cmd_sys_cmd_t *cmd, cmd_GPS_QUERY_POWER_MODE_resp_t *resp) {
    gps_err_t err = gps_query_power_mode(&resp->gps_in_power_save_mode);
    return gps_to_cmd_sys_resp(err);
}

cmd_sys_resp_code_t cmd_impl_GPS_QUERY_SW_CRC(const cmd_sys_cmd_t *cmd, cmd_GPS_QUERY_SW_CRC_resp_t *resp) {
    gps_err_t err = gps_query_crc_info(&resp->software_crc);
    return gps_to_cmd_sys_resp(err);
}

cmd_sys_resp_code_t cmd_impl_GPS_CONFIGURE_POSITION_UPATE_RATE(const cmd_sys_cmd_t *cmd, cmd_GPS_CONFIGURE_POSITION_UPATE_RATE_args_t *args) {
    gps_err_t err = gps_configure_system_postion_rate((gps_pos_update_rate_t)args->position_update_rate);
    return gps_to_cmd_sys_resp(err);
}

cmd_sys_resp_code_t cmd_impl_GPS_QUERY_POSITION_UPDATE_RATE(const cmd_sys_cmd_t *cmd, cmd_GPS_QUERY_POSITION_UPDATE_RATE_resp_t *resp) {
    gps_err_t err = gps_query_system_postion_rate(&resp->update_rate);
    return gps_to_cmd_sys_resp(err);
}

cmd_sys_resp_code_t cmd_impl_GPS_CONFIGURE_NMEA_MESSGES(const cmd_sys_cmd_t *cmd, cmd_GPS_CONFIGURE_NMEA_MESSGES_args_t *args) {
    gps_err_t err = gps_configure_nmea_messages(args->gga_interval, args->gsa_interval, args->gsv_interval, args->gll_interval, args->rmc_interval,
                    args->vtg_interval, args->zda_interval);
    return gps_to_cmd_sys_resp(err);
}

cmd_sys_resp_code_t cmd_impl_GPS_QUERY_NMEA_MESSGES(const cmd_sys_cmd_t *cmd, cmd_GPS_QUERY_NMEA_MESSGES_resp_t *resp) {
    gps_err_t err = gps_query_nmea_messages(&resp->gga_interval, &resp->gsa_interval, &resp->gsv_interval, &resp->gll_interval, &resp->rmc_interval,
                                            &resp->vtg_interval, &resp->zda_interval);
    return gps_to_cmd_sys_resp(err);
}

cmd_sys_resp_code_t cmd_impl_GPS_CONFIGURE_SERIAL_PORT(const cmd_sys_cmd_t *cmd, cmd_GPS_CONFIGURE_SERIAL_PORT_args_t *args) {
    // TODO(ALEA-165): Figure out the order of steps to change the underslying serial hw.
    gps_err_t err = gps_configure_serial_port((gps_config_serial_baud_t)args->baud_rate);
    return gps_to_cmd_sys_resp(err);
}

cmd_sys_resp_code_t cmd_impl_GPS_CONFIGURE_NAV_MSG_INTERVAL(const cmd_sys_cmd_t *cmd, cmd_GPS_CONFIGURE_NAV_MSG_INTERVAL_args_t *args) {
    gps_write_method_t write_method = parse_write_method(args->write_settings);
    gps_err_t err                   = gps_configure_nav_message_interval(write_method, args->navigation_msg_int);
    return gps_to_cmd_sys_resp(err);
}

/******************************************************************************/
/*                       P R I V A T E  F U N C T I O N S                     */
/******************************************************************************/

cmd_sys_resp_code_t gps_to_cmd_sys_resp(gps_err_t gps_err) {
    if (gps_err == GPS_SUCCESS) {
        return CMD_SYS_RESP_CODE_SUCCESS;
    } else if (gps_err == GPS_RTC_ERROR) {
        LOG_GPS__INTERNAL_RTC_ERROR();
    } else if (gps_err == GPS_RECV_NACK) {
        LOG_GPS__RECV_NACK();
    } else if (gps_err == GPS_BAD_RESPONSE) {
        LOG_GPS__BAD_RESPONSE();
    } else if (gps_err == GPS_BAD_ARGUMENTS) {
        LOG_GPS__BAD_ARGUMENTS();
    } else if (gps_err == GPS_MSG_TIMEOUT) {
        LOG_GPS__TIMEOUT();
    } else if (gps_err == GPS_MUTEX_TIMEOUT) {
        LOG_GPS__MUTEX_TIMEOUT();
    }

    return CMD_SYS_RESP_CODE_ERROR;
}

gps_write_method_t parse_write_method(int index) {
    gps_write_method_t write_method;

    if (index == 0) {
        write_method = GPS_WRITE_TO_SRAM;
    } else if (index == 1) {
        write_method = GPS_WRITE_TO_SRAM_AND_FLASH;
    } else {
        write_method = GPS_WRITE_TEMP;
    }

    return write_method;
}
