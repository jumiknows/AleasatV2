/**
 * @file comms_flash.c
 * @brief Functions for flashing the CC1110 on Comms
 */

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

#include "comms_defs.h"
#include "comms_utils.h"
#include "obc_comms.h"
#include "comms_cmd.h"
#include "comms_flash.h"

// OBC
#include "obc_error.h"

// Logging
#include "logger.h"

// Standard Library
#include <stdint.h>
#include <string.h>

/******************************************************************************/
/*                               D E F I N E S                                */
/******************************************************************************/

/**
 * @brief Timeout constants mostly determined from trial and error
 */
#define COMMS_FLASH_BL_START_TIMEOUT_MS 2000
#define COMMS_FLASH_APP_START_TIMEOUT_MS 3000
#define COMMS_FLASH_BL_ERASE_TIMEOUT_MS 1000
#define COMMS_FLASH_PAGE_WRITE_TIMEOUT_MS 300
#define COMMS_FLASH_DONE_PAGE_WRITES_TIMEOUT_MS 500

/******************************************************************************/
/*            P R I V A T E  F U N C T I O N  P R O T O T Y P E S             */
/******************************************************************************/

comms_err_t comms_enter_bootloader_once(void);
comms_err_t comms_attempt_flash(const comms_app_image_page_t* pages, uint8_t num_pages);

/******************************************************************************/
/*                       P U B L I C  F U N C T I O N S                       */
/******************************************************************************/

/**
 * @brief Flash Comms with prepared image. HWID used is defined in comms_defs.c
 *
 * @param[in] pages Array of pages to be flashed
 * @param[in] num_pages Number of elements in pages array
 *
 * @return COMMS_SUCCESS or COMMS_FLASH_FAIL
 */
comms_err_t comms_flash_image(const comms_app_image_page_t* pages, uint8_t num_pages) {
    comms_err_t err;
    comms_command_t resp = {0};
    comms_waiter_match_params_t app_start_match_spec = {
        .match_cmd_num_spec = COMMS_MATCH_EQUAL,
        .match_cmd_num = COMMS_RADIO_MSG_START,
        .cmd_ptr = &resp
    };

    if (num_pages == 0) {
        log_str(ERROR, LOG_COMMS_GENERAL, "Comms flsh no pages");
        return COMMS_FLASH_FAIL;
    }

    err = comms_enter_bootloader_once();
    if (err != COMMS_SUCCESS) {
        log_str(ERROR, LOG_COMMS_GENERAL, "Comms flsh btldr err %d", err);
        return COMMS_FLASH_FAIL;
    }

    err = comms_attempt_flash(pages, num_pages);
    if (err != COMMS_SUCCESS) {
        log_str(ERROR, LOG_COMMS_GENERAL, "Comms flsh app err %d", err);
        return COMMS_FLASH_FAIL;
    }

    err = comms_wait_for_cmd(&app_start_match_spec, COMMS_FLASH_APP_START_TIMEOUT_MS);
    if (err != COMMS_SUCCESS) {
        log_str(ERROR, LOG_COMMS_GENERAL, "Comms flsh start err %d", err);
        return err;
    }

    // TODO: check Comms app image version string when implemented on Comms

    return COMMS_SUCCESS;
}


/******************************************************************************/
/*                      P R I V A T E  F U N C T I O N S                      */
/******************************************************************************/

/**
 * @brief Attempt to put Comms in bootloader. Returns immediately when Comms enters bootloader.
 *
 * @return comms_err_t
 */
comms_err_t comms_enter_bootloader_once(void) {
    comms_err_t err;
    comms_command_t resp = {0};
    comms_waiter_match_params_t bl_start_match_spec = {
        .match_cmd_num_spec = COMMS_MATCH_EQUAL,
        .match_cmd_num = COMMS_BOOTLOADER_MSG_START,
        .cmd_ptr = &resp
    };

    err = comms_send_cmd(comms_hwid, COMMS_RADIO_MSG_REBOOT, NULL, 0, COMMS_MIBSPI_MUTEX_TIMEOUT_MS);
    if (err != COMMS_SUCCESS) {
        return err;
    }

    err = comms_wait_for_cmd(&bl_start_match_spec, COMMS_FLASH_BL_START_TIMEOUT_MS);
    if (err != COMMS_SUCCESS) {
        return err;
    }

    return COMMS_SUCCESS;
}

/**
 * @brief Attempt to flash Comms with given pages. Expects Comms to be in bootloader.
 *
 * @param[in] pages Array of pages to be flashed
 * @param[in] num_pages Number of elements in pages array
 *
 * @return COMMS_SUCCESS or COMMS_FLASH_FAIL
 */
comms_err_t comms_attempt_flash(const comms_app_image_page_t* pages, uint8_t num_pages) {
    comms_err_t cmd_ret = COMMS_UNKNOWN_ERR;
    comms_command_t resp = {0};
    uint8_t i;
    uint8_t final_page = 255;

    cmd_ret = comms_send_recv_cmd(comms_hwid, COMMS_BOOTLOADER_MSG_ERASE, NULL, 0, &resp, COMMS_FLASH_BL_ERASE_TIMEOUT_MS);
    if ((cmd_ret != COMMS_SUCCESS) || (resp.header.command != COMMS_BOOTLOADER_MSG_ACK) || (resp.data[0] != 1)) {
        return COMMS_FLASH_FAIL;
    }

    for(i = 0; i < num_pages; ++i) {
        cmd_ret = comms_send_recv_cmd(comms_hwid, COMMS_BOOTLOADER_MSG_WRITE_PAGE, pages[i].page_data, sizeof(pages[i].page_data), &resp, COMMS_FLASH_PAGE_WRITE_TIMEOUT_MS);
        if ((cmd_ret != COMMS_SUCCESS) || (resp.header.command != COMMS_BOOTLOADER_MSG_ACK) || (resp.data[0] != pages[i].page_data[0])) {
            return COMMS_FLASH_FAIL;
        }
    }

    cmd_ret = comms_send_recv_cmd(comms_hwid, COMMS_BOOTLOADER_MSG_WRITE_PAGE, &final_page, 1, &resp, COMMS_FLASH_DONE_PAGE_WRITES_TIMEOUT_MS);
    // TODO: check response params when Comms has it working
    if (cmd_ret != COMMS_SUCCESS) {
        return COMMS_FLASH_FAIL;
    }

    return COMMS_SUCCESS;
}
