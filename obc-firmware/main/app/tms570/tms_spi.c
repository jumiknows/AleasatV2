/**
 * @file tms_spi.c
 * @brief Low level generic SPI driver implementation
 */

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

#include "tms_spi.h"

// OBC
#include "obc_hardwaredefs.h"
#include "obc_utils.h"

// FreeRTOS
#include "rtos.h"

// Standard Library
#include <stdbool.h>


/******************************************************************************/
/*                               D E F I N E S                                */
/******************************************************************************/

#define SPI_CPU_TICK_TO_US 47

/******************************************************************************/
/*               P R I V A T E  G L O B A L  V A R I A B L E S                */
/******************************************************************************/

static SemaphoreHandle_t xSPIMutex;

/******************************************************************************/
/*            P R I V A T E  F U N C T I O N  P R O T O T Y P E S             */
/******************************************************************************/

static spi_err_t spi_send(spiBASE_t* port, spi_config_t* cfg, uint32_t block_count , uint16_t* databuff);
static spi_err_t spi_receive(spiBASE_t* port, spi_config_t* cfg, uint32_t block_count , uint16_t* databuff);
static spi_err_t spi_decode_err(uint32_t bits);
static uint8_t spi_get_wdel(SPIDATAFMT_t fmt, spiBASE_t* reg);
static void spi_clr_err(spiBASE_t* reg);
static inline uint8_t spi_get_setup_del(uint32_t delay_reg);
static inline uint8_t spi_get_hold_del(uint32_t delay_reg);

/******************************************************************************/
/*                       P U B L I C  F U N C T I O N S                       */
/******************************************************************************/

/**
 * @note Right now SPI API functions are locked with a single mutex since only SPI4 is in use.
 *       In future this may be updated to support future SPI ports.
*/

/**
 * @brief Sets up the SPI hardware.
 */
void tms_spi_init(void)
{
    spiInit();
}

/**
 * @brief Instantiates the mutex used to protect SPI access
 */
void tms_spi_create_infra(void)
{
    static StaticSemaphore_t xSPIStaticMutex;
    xSPIMutex = xSemaphoreCreateMutexStatic(&xSPIStaticMutex);
}

/**
 * @brief Wrapper around SPI to add synchronization via a mutex
 */
spi_err_t tms_spi_send(spiBASE_t* port, spi_config_t* cfg, uint32_t block_count , uint16_t* databuff, uint32_t mtx_timeout_ms)
{
    spi_err_t result;

    if (xSemaphoreTake(xSPIMutex, pdMS_TO_TICKS(mtx_timeout_ms)) == pdTRUE)
    {
        result = spi_send(port, cfg, block_count, databuff);
        xSemaphoreGive(xSPIMutex);
    }
    else
    {
        result = SPI_MUTEX_TIMEOUT;
    }
    return result;
}

/**
 * @brief Wrapper around SPI to add synchronization via a mutex
 */
spi_err_t tms_spi_receive(spiBASE_t* port, spi_config_t* cfg, uint32_t block_count , uint16_t* databuff, uint32_t mtx_timeout_ms)
{
    spi_err_t result;
    if (xSemaphoreTake(xSPIMutex, pdMS_TO_TICKS(mtx_timeout_ms)) == pdTRUE)
    {
        result = spi_receive(port, cfg, block_count, databuff);
        xSemaphoreGive(xSPIMutex);
    }
    else
    {
        result = SPI_MUTEX_TIMEOUT;
    }
    return result;
}

/******************************************************************************/
/*                      P R I V A T E  F U N C T I O N S                      */
/******************************************************************************/

/**
 * @brief Function decodes SPI response values and returns first detected error bit set
 *
 * @param[in] bits: 32 bit response from SPI HAL functions (spiReceiveData etc)
 * @return SPI error status
 *
 * @note From the TMS570 datasheet, any errors detected in spi transactions will be reflected in the returns from the SPI API
 * The value 3F can be attributed to the lower 6 error bits in the SPIFLG register. More information on these error bits
 * and their respective errors can be found at page 1638 on
 * https://www.ti.com/lit/ug/spnu515c/spnu515c.pdf?ts=1631514087070&ref_url=https%253A%252F%252Fwww
*/
static spi_err_t spi_decode_err(uint32_t bits)
{

    if (bits & (1<<6))
    {
        /* Highest priority, malformed packet has been received */
        return SPI_RX_OVERRUN_ERR;
    }
    else if (bits & (1<<4))
    {
        /* High priority, internal data transmit does not match data sent, invalidates sent data. May indicate erros with SPI bus hardware */
        return SPI_BIT_ERR;
    }
    else if (bits & (1<<2))
    {
        /* High priority. received parity does not match calculated, invalid data packet revieved */
        return SPI_PARITY_ERR;
    }
    else if (bits & (1<<0))
    {
        /* Medium priority, malformed datapacket given to SPI hardware. *Should not occur due to driver abstraction* */
        return SPI_DLEN_ERR;
    }
    else if (bits & (1<<3))
    {
        /* Low priority, only valid if ENA line is used */
        return SPI_DESYNC_ERR;
    }
    else if (bits & (1<<1))
    {
        /* Lowest priority error, only valid if ENA line is used */
        return SPI_TIMEOUT_ERR;
    }
    else
    {
        return SPI_SUCCESS;
    }
}

/**
 * @brief Function reads configured WDELAY from dataformat registers
 *
 * @param fmt SPI data format enum
 * @param reg SPI base register
 *
 * @return 8 bit delay in ticks
*/
static uint8_t spi_get_wdel(SPIDATAFMT_t fmt, spiBASE_t* reg)
{
    uint8_t resp = 0;

        switch (fmt)
        {
            case SPI_FMT_0:
                /* WDELAY is 8 MSB in SPIFMTx registers */
                resp = (uint8_t)((reg->FMT0) >> 24);
                break;
            case SPI_FMT_1:
                resp = (uint8_t)((reg->FMT1) >> 24);
                break;
            case SPI_FMT_2:
                resp = (uint8_t)((reg->FMT2) >> 24);
                break;
            case SPI_FMT_3:
                resp = (uint8_t)((reg->FMT3) >> 24);
                break;
            default:
                break;
        }
    return resp;
}

/**
 * @brief Function clears SPI error registers following the LS07x TRM
 *
 * @param reg SPI base register
*/
static void spi_clr_err(spiBASE_t* reg)
{
    /* Clear SPIFLG registers by writing 1 to each of the error bits as defined in section 25.9.5 in LS07x TRM*/
    reg->FLG = (uint32_t)(0b1011111);
    /* Read SPIBUF following clearing errors in SPIFLG as per 25.9.5 in LS07x TRM*/
    volatile uint32_t temp = reg->BUF;
}

/**
 * @brief Inline function to calculate configured setup time from SPI register
*/
static inline uint8_t spi_get_setup_del(uint32_t delay_reg)
{
    return (uint8_t)((delay_reg) >> 24);
}

/**
 * @brief Inline function to calculate configured hold time from SPI register
*/
static inline uint8_t spi_get_hold_del(uint32_t delay_reg)
{
    return (uint8_t)(((delay_reg) >> 16) & 0xFF);
}


/**
 * @brief Internal function to send 16 bit word to spi device
 *
 * @param[in]  port Base SPI port
 * @param[in]  cfg SPI command data buffer
 * @param[in]  block_count Number of datablocks to receive
 * @param[out] databuff Data buffer
 *
 * @return spi_err_t operation result.
*/
static spi_err_t spi_send(spiBASE_t* port, spi_config_t* cfg, uint32_t block_count , uint16_t* databuff)
{
    uint32_t enc_resp = 0;
    spi_err_t resp = SPI_SUCCESS;
    spiDAT1_t dataconfig;
    uint8_t delay = 0;

    if (cfg->cs_data.cs_type == SPI_CS_HARDWARE)
    {
        /* Build data config buffer that must be sent to SPI HAL driver */
        dataconfig.CS_HOLD = cfg->cs_hold;
        dataconfig.WDEL = cfg->wdelay;
        dataconfig.DFSEL = cfg->datafmt;
        dataconfig.CSNR = cfg->cs_data.cs_pin;

        /* Can run fully from existing HAL SPI driver */
        enc_resp = spiTransmitData(port, &dataconfig, block_count , databuff);
        resp = spi_decode_err(enc_resp);

        if (resp != SPI_SUCCESS)
        {
            spi_clr_err(port);
        }
    }
    else
    {
        /**
         * If we are running a GPIO driven SPI operation run the following setting to the HAL driver:
         *              - CS_HOLD: False, we will send the data chunk by chunk so this is irrelevant
         *              - WDEL:    False, CS pin delay will be set by tms driver
         *              - datafmt: Selected data format from HAL driver
         *              - CSNR:    SPI_CS_NONE, set no hardware CS pin
        */
        dataconfig.CS_HOLD = FALSE;
        dataconfig.WDEL = FALSE;
        dataconfig.DFSEL = cfg->datafmt;
        dataconfig.CSNR = SPI_CS_NONE;

        /* Send all datablocks in buffer */
        while (block_count > 0)
        {
            /* Set the CS port to the CS active polarity */
            obc_gpio_write(cfg->cs_data.cs_port, cfg->cs_data.cs_pin,  cfg->cs_data.pol);
            /* Get SPI CS setup delay */
            delay = spi_get_setup_del(port->DELAY);
            obc_delay_us((uint32_t)(delay/SPI_CPU_TICK_TO_US));

            enc_resp = spiTransmitData(port, &dataconfig, 1, databuff);
            resp = spi_decode_err(enc_resp);
            /* If any errors are recoreded, return early */
            if (resp != SPI_SUCCESS)
            {
                spi_clr_err(port);
                break;
            }

            block_count--;

            /* If there is still data to transmit, increment datapointer */
            if (block_count != 0)
            {
                databuff++;
            }


            /*Check if the CS_HOLD has been set or the databuffer has been fully sent */
            if ((cfg->cs_hold == 0) || (block_count == 0))
            {
                /* De activate the CS pin */
                /* Get CS Hold timing delay */
                delay = spi_get_hold_del(port->DELAY);
                obc_delay_us((uint32_t)(delay/SPI_CPU_TICK_TO_US));
                obc_gpio_write(cfg->cs_data.cs_port, cfg->cs_data.cs_pin,  (cfg->cs_data.pol) == 0);
            }

            /* If the wait delay is set, run the SPI bus data delay */
            if (cfg->wdelay)
            {
                /* Get the configured hardware WDEL, precompute scale */
                delay = spi_get_wdel(cfg->datafmt, port);
                /* Delay for configured delay in HALCOGEN plus additional wdel_ext */
                obc_delay_us((uint32_t)(delay/SPI_CPU_TICK_TO_US) + (cfg->cs_data.wdel_ext));
            }
        }
    }

    return resp;
}

/**
 * @brief Internal function to receive 16 bit word from spi device
 *
 * @param[in]  port Base SPI port
 * @param[in]  cfg SPI command data buffer
 * @param[in]  block_count Number of datablocks to receive
 * @param[out] databuff Data response buffer
 *
 *  @return spi_err_t operation result.
*/
static spi_err_t spi_receive(spiBASE_t* port, spi_config_t* cfg, uint32_t block_count , uint16_t* databuff)
{
    uint32_t enc_resp = 0;
    spi_err_t resp = SPI_SUCCESS;
    spiDAT1_t dataconfig;

    if (cfg->cs_data.cs_type == SPI_CS_HARDWARE)
    {
        /* Build data config buffer that must be sent to SPI HAL driver */
        dataconfig.CS_HOLD = cfg->cs_hold;
        dataconfig.WDEL = cfg->wdelay;
        dataconfig.DFSEL = cfg->datafmt;
        dataconfig.CSNR = cfg->cs_data.cs_pin;

        /* Can run fully from existing HAL SPI driver */
        enc_resp = spiReceiveData(port, &dataconfig, block_count, databuff);
        resp = spi_decode_err(enc_resp);

        if (resp != SPI_SUCCESS)
        {
            spi_clr_err(port);
        }
    }
    else
    {
        /**
         * If we are running a GPIO driven SPI operation run the following setting to the HAL driver:
         *              - CS_HOLD: False, we will send the data chunk by chunk so this is irrelevant
         *              - WDEL:    False, CS pin delay will be set by tms driver
         *              - datamft: Selected data format from HAL driver
         *              - CSNR:    SPI_CS_NONE, set no hardware CS pin
        */
        dataconfig.CS_HOLD = FALSE;
        dataconfig.WDEL = FALSE;
        dataconfig.DFSEL = cfg->datafmt;
        dataconfig.CSNR = SPI_CS_NONE;

        /* Send all datablocks in buffer */
        while (block_count > 0)
        {
            /* Set the CS port to the CS active polarity */
            obc_gpio_write(cfg->cs_data.cs_port, cfg->cs_data.cs_pin,  cfg->cs_data.pol);
            /* Get SPI CS setup delay */
            uint8_t delay = spi_get_setup_del(port->DELAY);
            obc_delay_us((uint32_t)(delay/SPI_CPU_TICK_TO_US));

            /* receive SPI data one block at a time using HAL driver */
            enc_resp = spiReceiveData(port, &dataconfig, 1, databuff);
            resp = spi_decode_err(enc_resp);
            /* If any errors are recoreded, return early */
            if (resp != SPI_SUCCESS)
            {
                spi_clr_err(port);
                break;
            }

            block_count--;

            /* If there is still data to transmit, increment datapointer */
            if (block_count != 0)
            {
                databuff++;
            }

            /*Check if the CS_HOLD has been set or the databuffer has been fully sent */
            if ((cfg->cs_hold == 0) || (block_count == 0))
            {
                /* Get CS Hold timing delay (bits 23:15) */
                delay = spi_get_hold_del(port->DELAY);
                obc_delay_us((uint32_t)(delay/SPI_CPU_TICK_TO_US));
                obc_gpio_write(cfg->cs_data.cs_port, cfg->cs_data.cs_pin,  (cfg->cs_data.pol) == 0);
            }

            /* If the wait delay is set, run the SPI bus data delay */
            if (cfg->wdelay)
            {
                /* Get the configured hardware WDEL */
                delay = spi_get_wdel(cfg->datafmt, port);
                /* Delay for configured delay in HALCOGEN plus additional wdel_ext */
                obc_delay_us((uint32_t)(delay/SPI_CPU_TICK_TO_US) + cfg->cs_data.wdel_ext);
            }

        }
    }

    return resp;
}
