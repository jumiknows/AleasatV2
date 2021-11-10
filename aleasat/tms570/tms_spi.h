/**
 * @file tms_spi.h
 * @brief Low level generic SPI driver prototypes and defines
 *
 * @note AN IMPORTANT NOTE ABOUT CS AND DATA HOLD TIMINGS
 *
 *       All bus timing information is copied from the HALCOGEN settings, this includes fields such as CS setup time,
 *       CS hold time, data transfer delays and more. Due to the resolution of the RTI timer used to calculate
 *       these delays in SOFTWARE_CS mode, the resolution of the timer will be about 1/47th of the configured value.
 *
 *       Each timer tick is equivelent to 1 us
 *       Each VCLK tick is equivelent to 0.02us
 *
 *       Since the driver can only run the timer in terms of the timer tick, the value of the configured tick will be divided by
 *       47 and truncated before being cast as timer ticks.
 *
 *       In effect only the following time steps are valid:
 *
 *       CONFIGURED TICKS | TIME DELAY
 *      --------------------------------
 *
 *             - 0:46    -> 0us delay
 *             - 47:93   -> 1us delay
 *             - 94:140  -> 2us delay
 *             - 141:187 -> 3us delay
 *             - 188:234 -> 4us delay
 *             - 235:255 -> 5us delay
 *
 *      In software CS mode, there is an additional wdel_ext parameter as part of the spi_cs_data_t structure. This parameter allows
 *      the calling API to append an additional WDEL (in us) in the event that the HALCOGEN configured WDEL is not large enough.
 */

#ifndef TMS_SPI_H_
#define TMS_SPI_H_

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

//OBC
#include "obc_gpio.h"

//HAL
#include "spi.h"
#include "reg_spi.h"

/******************************************************************************/
/*                               D E F I N E S                                */
/******************************************************************************/

#define CS_ACTIVE_LOW false
#define CS_ACTIVE_HIGH true

/******************************************************************************/
/*                              T Y P E D E F S                               */
/******************************************************************************/

/**
 * @brief SPI error enum
*/
typedef enum {

    /*SPI operation successful*/
    SPI_SUCCESS,

    /*Receive buffer overrun*/
    SPI_RX_OVERRUN_ERR,

    /*SPI bit mismatch error*/
    SPI_BIT_ERR,

    /*SPI slave desynchronization*/
    SPI_DESYNC_ERR,

    /*Calculated parity differs from receive parity bit*/
    SPI_PARITY_ERR,

    /*SPI ENA signal timout*/
    SPI_TIMEOUT_ERR,

    /*SPI datalength error*/
    SPI_DLEN_ERR,

    /*SPI request queue full buffer*/
    SPI_QUEUE_FULL,

    /*SPI bus timeout due to missing mutex*/
    SPI_MUTEX_TIMEOUT

} spi_err_t;

/**
 * @brief SPI GPIO type
*/
typedef enum {

    SPI_CS_SOFTWARE,

    SPI_CS_HARDWARE

} spi_cs_t;

/**
 * @brief SPI Chip Select line data
*/
typedef struct {

    /* SPI CS fields */

    /* SPI CS type */
    spi_cs_t cs_type;

    /* CS pin polarity if cs_type is set to SPI_CS_GPIO */
    bool pol;

    /* CS pin number */
    uint8_t cs_pin;

    /* CS port */
    gpio_port_t cs_port;

    /* WDELAY time extension in us, NOTE: ONLY VALID FOR SOFTWARE CS DRIVER */
    uint32_t wdel_ext;

} spi_cs_data_t;


typedef struct {

    /* spiDAT1_t data fields */

    /* SPI data format (found in HALCOGEN SPI bus settings) */
    SPIDATAFMT_t datafmt;

    /* hold CS line open between transactions (only applies to multiblock reads/writes)*/
    bool cs_hold;

    /* hold CS line for length of time detailed in SPI data format after transaction complete */
    bool wdelay;

    /* SPI Chip select data */
    spi_cs_data_t cs_data;

} spi_config_t;

/******************************************************************************/
/*                             F U N C T I O N S                              */
/******************************************************************************/

/* Startup functions */
void tms_spi_create_infra(void);
void tms_spi_init(void);

/* API */
spi_err_t tms_spi_send(spiBASE_t* port, spi_config_t* cfg, uint32_t block_count , uint16_t* databuff, uint32_t mtx_timeout_ms);
spi_err_t tms_spi_receive(spiBASE_t* port, spi_config_t* cfg, uint32_t block_count , uint16_t* databuff, uint32_t mtx_timeout_ms);

#endif /*TMS_SPI_H_*/
