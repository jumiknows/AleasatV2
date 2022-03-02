/**
 * @file comms_flash.h
 * @brief Functions for flashing the CC1110 on Comms
 */

#ifndef COMMS_FLASH_H_
#define COMMS_FLASH_H_

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

#include "comms_defs.h"

// Standard Library
#include <stdint.h>

/******************************************************************************/
/*                               D E F I N E S                                */
/******************************************************************************/

#define COMMS_FLASH_APP_START 0x0400
#define COMMS_FLASH_APP_END 0x6BFF
#define COMMS_FLASH_PAGE_SIZE_BYTES 128


/******************************************************************************/
/*                              T Y P E D E F S                               */
/******************************************************************************/

typedef struct comms_app_image_page_struct {
    uint8_t page_data[1 + COMMS_FLASH_PAGE_SIZE_BYTES];  // page_num + COMMS_FLASH_PAGE_SIZE_BYTES
} comms_app_image_page_t;

/******************************************************************************/
/*                             F U N C T I O N S                              */
/******************************************************************************/

comms_err_t comms_flash_image(comms_app_image_page_t* pages, uint8_t num_pages);

#endif /* COMMS_FLASH_H_ */
