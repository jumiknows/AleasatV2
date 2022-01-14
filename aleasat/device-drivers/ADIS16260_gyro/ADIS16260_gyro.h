/**
 * @file ADIS16260_gyro.h
 * @brief File contains structs, defines and API's used by panel_gyro.c
 */

#ifndef ADIS16260_GYRO_H_
#define ADIS16260_GYRO_H_

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

#include "ADIS16260_gyro_regs.h"

//OBC
#include "tms_spi.h"
#include "obc_gpio.h"

//FreeRTOS
#include "FreeRTOS.h"
#include "rtos_timer.h"


/******************************************************************************/
/*                              T Y P E D E F S                               */
/******************************************************************************/

/* Device ID struct */
typedef struct {

    uint16_t lot_id1;

    uint16_t lot_id2;

    uint16_t prod_id;

    uint16_t serial_num;

} ADIS16260_ID_t;

/* Device addressing/calibration struct */
typedef struct {

    /*Unique device ID*/
    ADIS16260_ID_t dev_id;

    /*TMS570 SPI port*/
    spiBASE_t* spi_port;

    /*SPI chip select port*/
    gpio_port_t cs_port;

    /*SPI chip select pin*/
    const uint8_t cs_pin;

    /*Timer handler*/
    xTimerHandle timer;

    /*Static timer buffer*/
    StaticTimer_t s_timer;

    /*Timer sleeping flag*/
    bool asleep;

} ADIS16260_t;


/* Gyro sensor data packet */
typedef struct {

    /*Processed data*/
    float32 data;

    /*Raw gyro data in LSB's*/
    int16_t data_raw;

} ADIS16260_gdata_t;


/* Temp sensor data packet */
typedef struct {

    /*Processed temperature data*/
    float32 data_temp;

    /*Raw temperature data in LSB's*/
    int16_t data_raw_temp;

} ADIS16260_tdata_t;

/* Device error type enum */
typedef enum {

    ADIS16260_ERROR = 0,

    ADIS16260_READ_ERROR,

    ADIS16260_WRITE_ERROR,

    ADIS16260_INVALID_DATA,

    ADIS16260_INVALID_ARGUMENT,

    ADIS16260_GYRO_SLEEPING,

    ADIS16260_TIMER_SET_ERR,

    ADIS16260_SUCCESS

} ADIS16260_error_t;

/* Device output range enum */
typedef enum {

    ADIS16260_RANGE_320,

    ADIS16260_RANGE_160,

    ADIS16260_RANGE_80

} ADIS_range_t;

/* Device bw enum */
typedef enum {

    ADIS16260_BW_330,

    ADIS16260_BW_50

} ADIS_bw_t;

/******************************************************************************/
/*                       G L O B A L  V A R I A B L E S                       */
/******************************************************************************/

extern ADIS16260_t panel_gyro_0;
extern ADIS16260_t panel_gyro_1;
extern ADIS16260_t panel_gyro_2;
extern ADIS16260_t panel_gyro_3;

/******************************************************************************/
/*                             F U N C T I O N S                              */
/******************************************************************************/

ADIS16260_error_t ADIS16260_init(ADIS16260_t* gyro);
ADIS16260_error_t ADIS16260_whoami(ADIS16260_t* gyro, ADIS16260_ID_t* buf);
ADIS16260_error_t ADIS16260_soft_reset(ADIS16260_t* gyro);
ADIS16260_error_t ADIS16260_read_gyro(ADIS16260_t* gyro, ADIS16260_gdata_t* buf);
ADIS16260_error_t ADIS16260_read_temp(ADIS16260_t* gyro, ADIS16260_tdata_t* buf);
ADIS16260_error_t ADIS16260_set_sleep(ADIS16260_t* gyro, uint8_t sleep_ticks);
ADIS16260_error_t ADIS16260_set_filter(ADIS16260_t* gyro, ADIS_range_t range, ADIS_bw_t bw, uint8_t taps);

#endif /*ADIS16260_GYRO_H_*/
