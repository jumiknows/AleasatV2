/**
 * @file comms_device.h
 *
 * @brief External API for the comms device driver interface.
 */
#ifndef COMMS_DEVICE_H_
#define COMMS_DEVICE_H_

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

// Standard Library
#include <stdint.h>
#include <stdbool.h>

/******************************************************************************/
/*                               D E F I N E S                                */
/******************************************************************************/

/**
 * @brief Minimum buffer size that should be used by code when passing buffers
 * to a comms device.
 * 
 * This is driven by the fact that the MIBSPI comms device implementation requires
 * fixed size transfers of 256 bytes.
 */
#define COMMS_DEV_MIN_BUFFER_SIZE 256U

/******************************************************************************/
/*                              T Y P E D E F S                               */
/******************************************************************************/

typedef enum comms_dev_err_type {
    COMMS_DEV_SUCCESS = 0,
    COMMS_DEV_ERR_BUS_FAILURE = -1,
    COMMS_DEV_ERR_INVALID_ARG = -2
} comms_dev_err_type_t;

typedef void* comms_dev_handle_t;
typedef void (*comms_dev_cb_func_t)(bool is_isr, void* param);

typedef struct {
    comms_dev_err_type_t (*tx)(const uint16_t* buffer, uint16_t msg_len);
    comms_dev_err_type_t (*rx)(uint16_t* buffer, uint16_t buffer_len);
    comms_dev_cb_func_t cb; // (c)all(b)ack function to notify upper layers of incoming data
    void* cb_param;
} comms_device_t;

/******************************************************************************/
/*                       P U B L I C  F U N C T I O N S                       */
/******************************************************************************/

comms_dev_err_type_t comms_dev_send(
    comms_dev_handle_t cdev_hdl,
    const uint16_t* buffer,
    uint16_t msg_len
);

comms_dev_err_type_t comms_dev_receive(
    comms_dev_handle_t cdev_hdl,
    uint16_t* buffer,
    uint16_t buffer_len
);

void comms_dev_register_callback(
    comms_dev_handle_t cdev_hdl,
    comms_dev_cb_func_t cb,
    void* cb_param
);

#endif // COMMS_DEVICE_H_
