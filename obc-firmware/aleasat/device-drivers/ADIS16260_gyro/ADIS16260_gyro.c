/**
 * @file ADIS16260_gyro.c
 * @brief Contains API and private function implementations for ADIS16260 panel gyros
 */


/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

//OBC
#include "obc_hardwaredefs.h"
#include "obc_misra.h"

//FreeRTOS
#include "rtos.h"
#include "logger.h"

#include "ADIS16260_gyro.h"

/******************************************************************************/
/*                P U B L I C  G L O B A L  V A R I A B L E S                 */
/******************************************************************************/

/* Panel 0 gyro */
ADIS16260_t panel_gyro_0 = {
OBC_MISRA_CHECK_OFF
    .dev_id = {0}, /*Remove MISRA 9.2 warning as for some reason it does not understand struct initialization within struct initialization*/
OBC_MISRA_CHECK_ON

    .cs_pin = PANEL0_CS_PIN,

    .cs_port = PANEL0_CS_PORT,

    .spi_port = PANEL0_SPI_PORT,

    .timer = {0},

OBC_MISRA_CHECK_OFF
    .s_timer = {0}, /*Remove MISRA 9.2 warning as for some reason it does not understand struct initialization within struct initialization*/
OBC_MISRA_CHECK_ON

    .asleep = false
};

/* Panel 1 gyro */
ADIS16260_t panel_gyro_1 = {
OBC_MISRA_CHECK_OFF
    .dev_id = {0}, /*Remove MISRA 9.2 warning as for some reason it does not understand struct initialization within struct initialization*/
OBC_MISRA_CHECK_ON

    .cs_pin = PANEL1_CS_PIN,

    .cs_port = PANEL1_CS_PORT,

    .spi_port = PANEL1_SPI_PORT,

    .timer = {0},

OBC_MISRA_CHECK_OFF
    .s_timer = {0}, /*Remove MISRA 9.2 warning as for some reason it does not understand struct initialization within struct initialization*/
OBC_MISRA_CHECK_ON

    .asleep = false
};

/* Panel 2 gyro */
ADIS16260_t panel_gyro_2 = {
OBC_MISRA_CHECK_OFF
    .dev_id = {0}, /*Remove MISRA 9.2 warning as for some reason it does not understand struct initialization within struct initialization*/
OBC_MISRA_CHECK_ON

    .cs_pin = PANEL2_CS_PIN,

    .cs_port = PANEL2_CS_PORT,

    .spi_port = PANEL2_SPI_PORT,

    .timer = {0},

OBC_MISRA_CHECK_OFF
    .s_timer = {0}, /*Remove MISRA 9.2 warning as for some reason it does not understand struct initialization within struct initialization*/
OBC_MISRA_CHECK_ON

    .asleep = false
};

/* Panel 3 gyro */
ADIS16260_t panel_gyro_3 = {
OBC_MISRA_CHECK_OFF
    .dev_id = {0}, /*Remove MISRA 9.2 warning as for some reason it does not understand struct initialization within struct initialization*/
OBC_MISRA_CHECK_ON

    .cs_pin = PANEL3_CS_PIN,

    .cs_port = PANEL3_CS_PORT,

    .spi_port = PANEL3_SPI_PORT,

    .timer = {0},

OBC_MISRA_CHECK_OFF
    .s_timer = {0}, /*Remove MISRA 9.2 warning as for some reason it does not understand struct initialization within struct initialization*/
OBC_MISRA_CHECK_ON

    .asleep = false
};


/******************************************************************************/
/*            P R I V A T E  F U N C T I O N  P R O T O T Y P E S             */
/******************************************************************************/

static ADIS16260_error_t ADIS16260_read_reg(ADIS16260_t* gyro, uint8_t addr, uint16_t* buf);
static ADIS16260_error_t ADIS16260_write_reg(ADIS16260_t* gyro, uint8_t addr, uint8_t* buf);
static ADIS16260_error_t ADIS16260_update_id(ADIS16260_t* gyro);
static void ADIS16260_timer_cb(xTimerHandle pxTimer);


/******************************************************************************/
/*                       P U B L I C  F U N C T I O N S                       */
/******************************************************************************/

/**
 * @brief Function initializes device and fills in basic internal data structure fields
 *
 * @param[in] gyro: Gyro struct containing target device addressing information
 *
 * Function returns ADIS16260_SUCCESS upon successful execution and ADIS16260_error otherwise
*/
ADIS16260_error_t ADIS16260_init(ADIS16260_t* gyro)
{

    TickType_t ticks;

    if (ADIS16260_update_id(gyro) == ADIS16260_ERROR)
    {
        return ADIS16260_ERROR;
    }

    /*Base number of sleep ticks set to 1, this will be updated when ADIS16260_set_sleep is called*/
    ticks = pdMS_TO_TICKS(1*ADIS_SLP_TICKS_MS);

    /* Disabling Rule 12.2 because MISRA seems to not be able to parse something about this function pointer*/
OBC_MISRA_CHECK_OFF
    gyro->timer = xTimerCreateStatic("ADIS_timer", ticks , pdFALSE, &gyro->dev_id.serial_num, &ADIS16260_timer_cb, &gyro->s_timer);
OBC_MISRA_CHECK_ON

    return ADIS16260_SUCCESS;
}


/**
 * @brief Function queries gyro for device identifiers and places them in a single 64 bit buffer
 *
 * @param[in] gyro:  ADIS16260 gyro object pointer containing device address information
 * @param[out] buf: Response buffer for product id data
 *
 * Function returns ADIS16260_SUCCESS if device ID acquisition successful, ADIS16260_error otherwise
 *
*/
ADIS16260_error_t ADIS16260_whoami(ADIS16260_t* gyro, ADIS16260_ID_t* buf)
{
    uint16_t rx[ADIS_DEVICE_ID_WORD_SIZE] = {0};
    uint8_t cmd_array[ADIS_DEVICE_ID_WORD_SIZE] = {ADIS_LOTID_1_ADDR, ADIS_LOTID_2_ADDR,
                                            ADIS_PROD_ID_ADDR, ADIS_SERIAL_NUM_ADDR};
    uint8_t i = 0;

    /*Find all 4 product ID values*/
    for (i=0; i<ADIS_DEVICE_ID_WORD_SIZE; i++)
    {
        if (ADIS16260_read_reg(gyro, cmd_array[i], &rx[i]) != ADIS16260_SUCCESS)
        {
            return ADIS16260_READ_ERROR;
        }
    }

    /*Verify the Product ID matches "1625" in decimal (0x3F89 in hex)*/
    if (rx[2] != 0x3F89)
    {
        return ADIS16260_INVALID_DATA;
    }

    /*Build response buffer*/
    buf->lot_id1 = rx[0];
    buf->lot_id2 = rx[1];
    buf->prod_id = rx[2];
    buf->serial_num = rx[3];

    return ADIS16260_SUCCESS;
}


/**
 * @brief Wrapper function initiates gyro software reset
 *
 * @param[in] gyro: Gyro struct containing device address to reset
 *
 * Function returns ADIS16260_SUCCESS upon successful reset request and ADIS16260_ERROR otherwise
*/
ADIS16260_error_t ADIS16260_soft_reset(ADIS16260_t* gyro)
{

    uint8_t tx = ADIS_SOFT_RESET;

    if (ADIS16260_write_reg(gyro, ADIS_GLOB_CMD_ADDR, &tx) != ADIS16260_SUCCESS)
    {
        return ADIS16260_WRITE_ERROR;
    }

    /*Give time for device to reset*/
    vTaskDelay(pdMS_TO_TICKS(1));

    return ADIS16260_SUCCESS;
}


/**
 * @brief Function reads gyro rate data from device and stores raw and compensated value in gyro rate packet
 *
 * @param[in] gyro: Gyro struct containing device address information\
 * @param[out] buf: Struct to hold response packet
 *
 * Function returns ADIS16260_SUCCESS upon successful read and ADIS16260_ERROR otherwise
 *
*/
ADIS16260_error_t ADIS16260_read_gyro(ADIS16260_t* gyro, ADIS16260_gdata_t* buf)
{
    uint16_t rx = 0;

    /*Read raw gyro data*/
    if (ADIS16260_read_reg(gyro, ADIS_RATE_ADDR, &rx) != ADIS16260_SUCCESS)
    {
        return ADIS16260_READ_ERROR;
    }

    /*Verify new data flag was set and the ERROR bit is clear*/
    if (((rx & (uint16_t)ADIS_NEW_DATA) == 0) || ((rx & (uint16_t)ADIS_ERROR_ALARM) != 0))
    {
        return ADIS16260_INVALID_DATA;
    }

    /**Convert 14 bit 2's complement value to interpretable 16 bit while preserving sign
     * See: https://stackoverflow.com/questions/34075922/convert-raw-14-bit-twos-complement-to-signed-16-bit-integer
     *
     * Remove MISRA check 10.5 as we cannot immediately cast this bitshift to uint16_t or else the operation becomes invalid
     */
OBC_MISRA_CHECK_OFF
    buf->data_raw = ((int16_t)(rx << 2)) / 4;
OBC_MISRA_CHECK_ON

    /*Calculated rate = raw rate (LSB's) * degrees per LSB * Default scale (LSB'S) * degrees per scale LSB + extra offset*/
    buf->data = (buf->data_raw * ADIS_GYRO_DEG_LSB) * (ADIS_GYRO_SCALE_DEG_LSB * ADIS_GYRO_DEFAULT_SCALE);

    return ADIS16260_SUCCESS;
}


/**
 * @brief Function reads temperature data from device and stores raw and compensated value in gyro temp packet
 *
 * @param[in] gyro: Gyro struct containing device address information\
 * @param[out] buf: Struct to hold response packet
 *
 * Function returns ADIS16260_SUCCESS upon successful read and ADIS16260_ERROR otherwise
 *
*/
ADIS16260_error_t ADIS16260_read_temp(ADIS16260_t* gyro, ADIS16260_tdata_t* buf)
{
    uint16_t rx = 0;

    /*Read raw temperature data*/
    if (ADIS16260_read_reg(gyro, ADIS_TEMP_ADDR, &rx) != ADIS16260_SUCCESS)
    {
        return ADIS16260_READ_ERROR;
    }

    /*Verify new data flag was set and the ERROR bit is clear*/
    if (((rx & (uint16_t)ADIS_NEW_DATA) == 0) || ((rx & (uint16_t)ADIS_ERROR_ALARM) != 0))
    {
        return ADIS16260_INVALID_DATA;
    }

    /**Convert 12 bit 2's complement value to interpretable 16 bit while preserving sign
     * See: https://stackoverflow.com/questions/34075922/convert-raw-14-bit-twos-complement-to-signed-16-bit-integer
     *
     * Remove MISRA check 10.5 as we cannot immediately cast this bitshift to uint16_t or else the operation becomes invalid
     */
OBC_MISRA_CHECK_OFF
    buf->data_raw_temp = ((int16_t)(rx << 4)) / 16;
OBC_MISRA_CHECK_ON

    /*Calculated temperature = raw temp (LSB's) * degrees per LSB + standard offset (25 degrees C)*/
    buf->data_temp = (buf->data_raw_temp * ADIS_TEMP_DEG_LSB) + ADIS_TEMP_DEFAULT_OFFSET;

    return ADIS16260_SUCCESS;
}


/**
 * @brief Function puts device to sleep for time in units of 1/2 seconds (e.g. 1 sleep_tick = 0.5s)
 *
 * @param[in] gyro: Gyro struct containing target device addressing information
 * @param[in] sleep_ticks: Number of half second time units to set device to sleep for
 *
 * Function returns ADIS16260_SUCCESS upon successful execution and ADIS16260_error otherwise
*/
ADIS16260_error_t ADIS16260_set_sleep(ADIS16260_t* gyro, uint8_t sleep_ticks)
{

    TickType_t ticks;

    if (gyro->asleep  == true)
    {
        return ADIS16260_GYRO_SLEEPING;
    }

    /*Calculate number of timer ticks*/
    ticks = pdMS_TO_TICKS(sleep_ticks*ADIS_SLP_TICKS_MS);

    if (xTimerChangePeriod(gyro->timer, ticks, 0) != pdFAIL)
    {
        /*Write device sleep command*/
        if (ADIS16260_write_reg(gyro, ADIS_SLEEP_CNT_ADDR, &sleep_ticks) != ADIS16260_SUCCESS)
        {
            return ADIS16260_WRITE_ERROR;
        }

        /* Start timer */
        gyro->asleep = true;
        if (xTimerStart(gyro->timer, 0) == pdFAIL)
        {
            return ADIS16260_TIMER_SET_ERR;
        }
    }

    return ADIS16260_SUCCESS;
}


/**
 * @brief Sets gyro onboard filtering
 *
 * @param[in] gyro: Gyro addressing information struct
 * @param[in] range: Gyro range to set
 * @param[in] bw: Gyro bandwidth to set
 * @param[in] taps: Number of gyro taps to set
*/
ADIS16260_error_t ADIS16260_set_filter(ADIS16260_t* gyro, ADIS_range_t range, ADIS_bw_t bw, uint8_t taps)
{

    uint8_t tx[2] = {0};

    /*Boundary check for number of sensor taps (3 bit resolution value)*/
    if ((gyro == NULL) || (taps > 7))
    {
        return ADIS16260_INVALID_ARGUMENT;
    }

    switch (range)
    {
        case ADIS16260_RANGE_320:
            tx[1] |= ADIS_RANGE_320;
            break;
        case ADIS16260_RANGE_160:
            tx[1] |= ADIS_RANGE_160;
            break;
        case ADIS16260_RANGE_80:
            tx[1] |= ADIS_RANGE_80;
            break;

OBC_MISRA_CHECK_OFF
        default:
            /* Removing MISRA check 15.2 as mandatory break statement is not needed here as
             * function will return.
             */
            return ADIS16260_INVALID_ARGUMENT;
OBC_MISRA_CHECK_ON
    }

    switch (bw)
    {
        case ADIS16260_BW_330:
            tx[0] |= ADIS_BW_330;
            break;
        case ADIS16260_BW_50:
            tx[0] |= ADIS_BW_50;
            break;

OBC_MISRA_CHECK_OFF
        default:
            /* Removing MISRA check 15.2 as mandatory break statement is not needed here as
             * function will return.
             */
            return ADIS16260_INVALID_ARGUMENT;
OBC_MISRA_CHECK_ON
    }

    tx[0] |= taps;

    /*Write both data buffers, datasheet suggests writing upper byte values first*/
    if (ADIS16260_write_reg(gyro, ADIS_SENS_AVG_ADDR_UPPER, &tx[1]) != ADIS16260_SUCCESS)
    {
        return ADIS16260_WRITE_ERROR;
    }

    if (ADIS16260_write_reg(gyro, ADIS_SENS_AVG_ADDR, &tx[0]) != ADIS16260_SUCCESS)
    {
        return ADIS16260_WRITE_ERROR;
    }

    vTaskDelay(pdMS_TO_TICKS(100));

    return ADIS16260_SUCCESS;
}

/******************************************************************************/
/*                      P R I V A T E  F U N C T I O N S                      */
/******************************************************************************/

/**
 * @brief Function reads 16 bit word from ADIS16260 gyro register and puts it into buffer
 *
 * @param[in] gyro: ADIS16260 gyro object pointer containing device address information
 * @param[in] addr: Register address of ADIS16260 gyro to read data
 * @param[out] buf: Response data buffer
 *
 * Returns ADIS16260_error or ADIS16260_SUCCESS depending if read was successful or not
 */
static ADIS16260_error_t ADIS16260_read_reg(ADIS16260_t* gyro, uint8_t addr, uint16_t* buf)
{

    uint16_t tx = 0;

    /* Setup chip select config packet */
    const spi_cs_data_t cs_config = {
        SPI_CS_SOFTWARE,
        CS_ACTIVE_LOW,
        gyro->cs_pin,
        gyro->cs_port,
        0
    };

    /* Setup spi bus config packet */
    spi_config_t spi_config = {
        SPI_FMT_3,
        false,
        false,
        cs_config
    };

    /* According to device datasheet, when doing a read operation send first word as address shifted by 8 bits */
    tx = (uint16_t)addr << 8;

    if (buf == NULL)
    {
        return ADIS16260_INVALID_ARGUMENT;
    }

    /* Send the spi read command */
    if (tms_spi_send(gyro->spi_port, &spi_config, 1, &tx, 500) != SPI_SUCCESS)
    {
        return ADIS16260_WRITE_ERROR;
    }

    /* Receive SPI data */
    if (tms_spi_receive(gyro->spi_port, &spi_config, 1, buf, 500) != SPI_SUCCESS)
    {
        return ADIS16260_READ_ERROR;
    }

    return ADIS16260_SUCCESS;
}

/**
 * @brief Function writes 8 bit word to ADIS16260 gyro register
 *
 * @param[in] gyro: ADIS16260 gyro object pointer containing device address information
 * @param[in] addr: Register address of ADIS16260 gyro to write data
 * @param[in] buf: Data byte to write
 *
 * Returns ADIS16260_error or ADIS16260_SUCCESS depending if read was successful or not
*/
static ADIS16260_error_t ADIS16260_write_reg(ADIS16260_t* gyro, uint8_t addr, uint8_t* buf)
{
    uint16_t tx = 0;

    /* Setup chip select config packet */
    const spi_cs_data_t cs_config = {
        SPI_CS_SOFTWARE,
        CS_ACTIVE_LOW,
        gyro->cs_pin,
        gyro->cs_port,
        0
    };

    /* Setup spi bus config packet */
    spi_config_t spi_config = {
        SPI_FMT_3,
        false,
        false,
        cs_config
    };

    if (buf == NULL)
    {
        return ADIS16260_INVALID_ARGUMENT;
    }

    /**See pages 10/11 from https://www.mouser.ca/datasheet/2/609/ADIS16260_16265-878526.pdf
     * Data write packet has MSB (R/w) = 1 followed by 7 bit address and finally data byte
     */
    tx = (ADIS_SET_WRITE_BIT) | (uint16_t)((uint16_t)addr << ADIS_DATA_ADDRESS_SHIFT) | (*buf);

    /* Send the spi read command */
    if (tms_spi_send(gyro->spi_port, &spi_config, 1, &tx, 500) != SPI_SUCCESS)
    {
        return ADIS16260_WRITE_ERROR;
    }

    return ADIS16260_SUCCESS;
}

/**
 * @brief Function reads gyro device ID and updates internal gyro struct
 *
 * @param[in] gyro: Gyro struct containing device address information and response data buffer
 *
 * Function returns ADIS16260_SUCCESS upon successful read and ADIS16260_ERROR otherwise
 *
 * NOTE: This function should not be used to repeatedly query gyro device ID, use ADIS16260_whoami instead.
 *
 * This function should be used at Satellite power-on to initially populate unique gyro device ID's for future
 * comparison.
 *
*/
static ADIS16260_error_t ADIS16260_update_id(ADIS16260_t* gyro)
{
    ADIS16260_ID_t rx = {0};

    if (ADIS16260_whoami(gyro, &rx) == ADIS16260_ERROR)
    {
        return ADIS16260_ERROR;
    }

    gyro->dev_id = rx;

    return ADIS16260_SUCCESS;
}

/**
 * @brief Callback function to process gyro sleep related timer events
 *
 * Function will check which device sleep timer it has been called by and set the device asleep flag to false.
*/
static void ADIS16260_timer_cb(xTimerHandle pxTimer)
{

    uint16_t* timer_id;

    timer_id = (uint16_t*)pvTimerGetTimerID(pxTimer);

    if (*timer_id == panel_gyro_0.dev_id.serial_num) {
        panel_gyro_0.asleep = false;
    }
    else if (*timer_id == panel_gyro_1.dev_id.serial_num) {
        panel_gyro_1.asleep = false;
    }
    else if (*timer_id == panel_gyro_2.dev_id.serial_num) {
        panel_gyro_2.asleep = false;
    }
    else if (*timer_id == panel_gyro_3.dev_id.serial_num) {
        panel_gyro_3.asleep = false;
    }
    else
    {
        log_str(ERROR, LOG_ADCS_GYRO, "Sleep callback triggered on removed handler");
    }
}
