/**
 * @file comms_device.c
 *
 * @brief Implementation of comms device driver interface. Provides a generic API
 *        for transferring data to and from an RF card.
 */

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/
#include <stddef.h>
#include "comms_device.h"
#include "logger.h"


/******************************************************************************/
/*                       P U B L I C  F U N C T I O N S                       */
/******************************************************************************/

/**
 * @brief Transfer a block of data to a comms device
 *
 * @param[in] cdev_hdl comms device handle
 * @param[in] buffer   pointer to data buffer to be transferred. Size of the transfer
 *                     is specified in the device specific implementation file.
 *
 * @return COMMS_DEV_SUCCESS if the transfer was successful.
 *         COMMS_DEV_BUS_FAILURE if communication to the comms device failed
 */
comms_dev_err_type_t comms_dev_send(
        comms_dev_handle_t cdev_hdl,
        uint16_t* buffer,
        uint16_t num_bytes
) {
    comms_dev_err_type_t ret = COMMS_DEV_SUCCESS;

    comms_device_t* cdev = (comms_device_t*) cdev_hdl;

    ret = cdev->tx(buffer, num_bytes);

    return ret;
}

/**
 * @brief Receive a block of data from a comms device
 *
 * @param[in] cdev_hdl comms device handle
 * @param[in] buffer   pointer to storage buffer for the received data
 *
 * @return COMMS_DEV_SUCCESS if the transfer was successful.
 *         COMMS_DEV_BUS_FAILURE if communication to the comms device failed
 */
comms_dev_err_type_t comms_dev_receive(
        comms_dev_handle_t cdev_hdl,
        uint16_t* buffer,
        uint16_t buffer_len
) {
    comms_dev_err_type_t ret = COMMS_DEV_SUCCESS;

    comms_device_t* cdev = (comms_device_t*) cdev_hdl;

    ret = cdev->rx(buffer, buffer_len);

    return ret;
}

/**
 * @brief Register a callback function to be invoked when data is ready to be received
 *
 * @param[in] cdev_hdl comms device handle
 * @param[in] cb       pointer to callback function
 */
void comms_dev_register_callback(comms_dev_handle_t cdev_hdl,
                                 comms_dev_cb_func_t cb,
                                 void* cb_param
) {
    comms_device_t* cdev;

    if ((cdev_hdl != NULL) && (cb != NULL)) {
        cdev = (comms_device_t*) cdev_hdl;
        cdev->cb = cb;
        cdev->cb_param = cb_param;
    }
}
