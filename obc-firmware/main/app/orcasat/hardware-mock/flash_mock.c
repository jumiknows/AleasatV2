/**
 * @file flash_mock.c
 * @brief Uses flash bank 7 on the launchpad to mock NOR flash access for the
 *        filesystem. For a breakdown of the flash bank sectoring see the Reference
 *        Manual, page 113: http://www.ti.com/lit/ug/spnu515c/spnu515c.pdf#page=113
 *
 * @author Andrada Zoltan
 * @date March 26, 2020
 */

#ifdef PLATFORM_LAUNCHPAD_1224

#include "flash_mock.h"
#include "obc_hardwaredefs.h"
#include "obc_utils.h"
#include "obc_watchdog.h"
#include "rtos.h"
#include <string.h>
#include "sys_common.h"
#include "system.h"
#include "F021.h"

/**
 * @brief Can only do accesses of 16-bytes long at a time, this is limitation
 * of the flash bank width.
 * Reference manual page 113: flash banks are 144-bit(18-byte) wide with ECC
 *                  page 114: 8-bit ECC for every 64-bits of data
 *
 * So the actual bank width is 144 - 2 * 8 = 128-bits = 16 bytes
 */
#define FLASH_WRITE_SIZE 16

// Useful defines
#define ADDR_BASE (uint32_t*)F021_EEPROM_MAP_BEGIN
#define SIZE_64K  (64 * 1024)
#define SIZE_32K  (32 * 1024)
#define SIZE_16K  (16 * 1024)
#define SIZE_4K   (4 * 1024)
#define SIZE_1K   (1024)

// Private function prototypes
static flash_err_t flash_activate_bank(void);
static bool flash_is_busy(void);

/**
 * @brief Initializes the flash banks through the Flash API, and
 * activates flash bank 7.
 */
void flash_init_mock(void) {
    Fapi_initializeFlashBanks((uint32_t)HCLK_FREQ);
    flash_erase_mock(0x00, FULL_CHIP);
}

/**
 * @brief Erases the flash in various sized chunks
 *
 * @param addr: Address within the sector/block that is to be erased
 * @param erase_size: Block size to erase. Can do 64KB, 32KB, 4KB, 1KB, or full flash bank.
 * @return Negative error code if unsuccessful
 */
flash_err_t flash_erase_mock(uint32_t addr, flash_erase_sz_t erase_size) {
    Fapi_StatusType err = Fapi_Status_Success;

    // Activate the bank first, this is similar to a write enable.
    if (flash_activate_bank() != FLASH_OK) {
        return FLASH_MOCK_ERR;
    }

    uint32_t sector_addr;
    static uint8_t erased_data[SIZE_16K] = { 0 };
    switch (erase_size) {
        case FULL_CHIP:
        case SECTOR_64K:
            err = Fapi_issueAsyncCommandWithAddress(Fapi_EraseSector, ADDR_BASE);
            while (flash_is_busy()) {
            }
            err = Fapi_issueAsyncCommandWithAddress(Fapi_EraseSector, ADDR_BASE + SIZE_16K);
            while (flash_is_busy()) {
            }
            err = Fapi_issueAsyncCommandWithAddress(Fapi_EraseSector, ADDR_BASE + 2 * SIZE_16K);
            while (flash_is_busy()) {
            }
            err = Fapi_issueAsyncCommandWithAddress(Fapi_EraseSector, ADDR_BASE + 3 * SIZE_16K);
            break;
        case SECTOR_32K:
            sector_addr = addr - (addr % SIZE_32K); // Align to 32K sized chunk
            err         = Fapi_issueAsyncCommandWithAddress(Fapi_EraseSector, ADDR_BASE + sector_addr);
            while (flash_is_busy()) {
            }
            err = Fapi_issueAsyncCommandWithAddress(Fapi_EraseSector, ADDR_BASE + sector_addr + SIZE_16K);
            break;
        /**
         * Sector sizes 4K and 1K do not exist in this flash bank, but the filesystem will not work
         * well with the few 16K sectors that are present. Instead, mock these smaller sectors by
         * erasing the full 16K size and rewriting the data that should not have been erased.
         */
        case SECTOR_4K:
            sector_addr = addr - (addr % SIZE_16K); // Align to 16K sector sized chunk
            flash_read_mock(sector_addr, SIZE_16K, erased_data);
            err = Fapi_issueAsyncCommandWithAddress(Fapi_EraseSector, ADDR_BASE + sector_addr);
            while (flash_is_busy()) {
            }

            /**
             *     _________________________________________
             *    |________|||||||||||___________|__________|
             *  sector    addr      addr                 sector_addr
             *   addr               + 4K                   + 16K
             *
             * We only want to erase the 4K section starting at addr. The other three sections need
             * to be written back. We can do this in two write operations:
             *     - sector_addr -> addr (the size of this write is (addr - sector_addr))
             *     - addr + 4K   -> sector_addr + 16K (the size of this write is (sector_addr + 16K
             * - (addr + 4K))
             */
            addr -= (addr % SIZE_4K);
            flash_write_mock(sector_addr, addr - sector_addr, erased_data);
            flash_write_mock(addr + SIZE_4K, (sector_addr + SIZE_16K) - (addr + SIZE_4K), &erased_data[(addr + SIZE_4K) - sector_addr]);
            break;
        case SECTOR_1K:
            sector_addr = addr - (addr % SIZE_16K); // Align to 16K sector sized chunk
            flash_read_mock(sector_addr, SIZE_16K, erased_data);
            err = Fapi_issueAsyncCommandWithAddress(Fapi_EraseSector, ADDR_BASE + sector_addr);
            while (flash_is_busy()) {
            }

            // Same concept as in the 4K case, but replace the size of the sector we erase with a 1K
            // size.
            addr -= (addr % SIZE_1K);
            flash_write_mock(sector_addr, addr - sector_addr, erased_data);
            flash_write_mock(addr + SIZE_1K, (sector_addr + SIZE_16K) - (addr + SIZE_1K), &erased_data[(addr + SIZE_1K) - sector_addr]);
            break;
        default:
            // Don't do anything
            break;
    }

    while (flash_is_busy()) {
    }

    if (err != Fapi_Status_Success) {
        return FLASH_MOCK_ERR;
    }

    return FLASH_OK;
}

/**
 * @brief Program the flash with data to store.
 *
 * @param addr: Address to write to, must be byte-aligned
 * @param size: Amount of data to write in bytes
 * @param data: Buffer storing the data to write
 * @return: FLASH_OK
 */
flash_err_t flash_write_mock(uint32_t addr, uint32_t size_bytes, const uint8_t* data) {
    // Activate the bank first, this is similar to a write enable.
    if (flash_activate_bank() != FLASH_OK) {
        return FLASH_MOCK_ERR;
    }

    uint32_t i;
    uint8_t to_write[FLASH_WRITE_SIZE];
    for (i = 0; i < size_bytes; i += FLASH_WRITE_SIZE) {
        // Copy the data so we can safely get rid of the const qualifier, the Fapi function
        // takes a non-const buffer.
        memcpy(to_write, data + i, MIN(FLASH_WRITE_SIZE, size_bytes - i));

        // Wait for the flash to have finished previous action
        while (flash_is_busy()) {
        }

        // Note that ADDR_BASE is of type uint32_t*, so every increment jumps by 4 bytes.
        Fapi_StatusType err = Fapi_issueProgrammingCommand(ADDR_BASE + ((addr + i) / 4), to_write, MIN(FLASH_WRITE_SIZE, size_bytes - i), NULL, 0, Fapi_DataOnly);

        if (err != Fapi_Status_Success) {
            return FLASH_MOCK_ERR;
        }
    }

    return FLASH_OK;
}

/**
 * @brief Read data from the mocked flash.
 *
 * @param addr: Address to read from, must be byte-aligned
 * @param size: Amount of data to read in bytes
 * @param data: Buffer where read data will be stored
 * @return FLASH_OK
 */
flash_err_t flash_read_mock(uint32_t addr, uint32_t size_bytes, uint8_t* data) {
    memcpy(data, ADDR_BASE + (addr / 4), size_bytes);
    return FLASH_OK;
}

/**
 * @brief Activate flash bank 7.
 *
 * @return Negative error code if unsuccessful
 */
static flash_err_t flash_activate_bank(void) {
    // Activate flash bank 7
    Fapi_StatusType err = Fapi_setActiveFlashBank(Fapi_FlashBank7);
    if (err != Fapi_Status_Success) {
        return FLASH_MOCK_ERR;
    }

    /**
     * On the launchpad, flash bank 7 is split up into 4 sectors. Enable these
     * sectors by setting the lower 4 bits of the first argument.
     */
    err = Fapi_enableEepromBankSectors(0x0F, 0x00);
    if (err != Fapi_Status_Success) {
        return FLASH_MOCK_ERR;
    }

    return FLASH_OK;
}

/**
 * @brief Checks if the state of the flash FSM is busy.
 *
 * @return True if the flash is busy, false otherwise
 */
static bool flash_is_busy(void) {
    if (FLASH_CONTROL_REGISTER->FmStat.FMSTAT_BITS.BUSY) {
        return true;
    } else {
        return false;
    }
}

#endif // PLATFORM_LAUNCHPAD_1224
