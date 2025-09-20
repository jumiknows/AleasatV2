/**
 * @file telem.c
 * @brief Telemetry
 */

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

#include "telem.h"

// Utils
#include "telem_gen.h"
#include "telem_exec.h"
#include "telem_error.h"
#include "data_fmt.h"
#include "io_stream.h"
#include "obc_utils.h"
#include "obc_watchdog.h"
#include "obc_rtos.h"
#include "obc_filesystem.h"
#include "lfs.h"
#include "obc_rtc.h"
#include "obc_time.h"
#include "obc_featuredefs.h"
#include "obc_mram.h"

// RTOS
#include "rtos.h"

// Standard Library
#include <stddef.h>
#include <stdint.h>

// Logger
#include "logger.h"

/******************************************************************************/
/*                               D E F I N E S                                */
/******************************************************************************/

#define TELEM_COLLECT_POLL_PERIOD_S         1U

#define TELEM_FS_MUTEX_TIMEOUT_MS           2000U

/* @brief Max sizes of the telem files
 *
 * Constrained by the downlink capacity calculated in Data Budget V3
 * ~50KB downlink capacity per pass allocated to telemetry
 * Priority 0 is transmitted fully and maximum leftover amount of priority 1 is transmitted
 * Thus, max size of priority 0 = priority 1 = 50KB. Leftover (8MB - 100KB) is for priority 2
*/
#define TELEM_PRIORITY_0_MAX_FILE_SIZE      51200U
#define TELEM_PRIORITY_1_MAX_FILE_SIZE      51200U
#define TELEM_PRIORITY_2_MAX_FILE_SIZE      8286208U

/******************************************************************************/
/*            P R I V A T E  F U N C T I O N  P R O T O T Y P E S             */
/******************************************************************************/

static void telem_collect_task(void *pvParameters);

// The compiler complains about unused functions unless we use an #if here
#if FEATURE_FLASH_FS
static telem_err_t generate_outer_header(uint8_t *header_buf, uint16_t num_units);
static void deserialize_outer_header(const uint8_t *header_buf, epoch_t *epoch, uint16_t *num_units);
static telem_err_t read_outer_header(lfs_file_t *file, int32_t *offset, epoch_t *cur_epoch, uint16_t *num_units);

static void generate_inner_header(uint8_t *header_buf, telem_id_t id, uint16_t resp_len);
static void deserialize_inner_header(const uint8_t *header_buf, telem_id_t *id, uint16_t *resp_len);
static telem_err_t read_inner_header(lfs_file_t *file, int32_t *offset, telem_id_t *cur_telem_id, uint16_t *resp_len);

static telem_err_t search_for_stop_word(lfs_file_t *file, int32_t *offset, const int32_t *file_size, uint8_t *stop_word_buf);

static telem_err_t check_file_sizes(void);
#endif

/******************************************************************************/
/*               P R I V A T E  G L O B A L  V A R I A B L E S                */
/******************************************************************************/

#if FEATURE_FLASH_FS
static const uint8_t telem_stop_word[TELEM_STOP_WORD_SIZE] = { 0xA1, 0xEA, 0x50, 0xBC }; // "ALEA OBC"
static uint32_t file_0_size = 0;
static uint32_t file_1_size = 0;
static uint32_t file_2_size = 0;
#endif

static bool telem_collect_enabled = true;

static TaskHandle_t telem_exec_task_handle = NULL;

static uint16_t telem_config_start_addr = 0x2000;

static uint8_t loaded = 0;

struct telem_config_vars {
    uint8_t priority;
    uint32_t period;
};

/******************************************************************************/
/*                       P U B L I C  F U N C T I O N S                       */
/******************************************************************************/

/**
 * @brief Starts the telemetry collection task.
*/
void telem_collect_post_init(void) {
    telem_init_mram();
    telem_exec_task_handle = xTaskGetHandle("TELEM_EXEC");
    obc_rtos_create_task(OBC_TASK_ID_TELEM_COLLECT, &telem_collect_task, NULL, OBC_WATCHDOG_ACTION_ALLOW);
}

/**
 * @brief Initialise MRAM config with firmware values of telem unit priority and period
*/
void telem_init_mram(void) {
    // WARNING: This is assuming mram_init() has been called already
    // Write the inital config values from flash into mram

    struct telem_config_vars telem_vars[TELEM_COUNT];

    for (uint32_t i = 0; i < TELEM_COUNT; i++) {
        telem_vars[i].priority = TELEM_SPEC_TABLE[i].priority;
        telem_vars[i].period = TELEM_SPEC_TABLE[i].period;
    }

    mram_err_t err = mram_write(telem_config_start_addr, TELEM_COUNT * sizeof(struct telem_config_vars), (uint8_t *)&telem_vars);

    if (err != MRAM_OK) {
#if FEATURE_HW_MRAM
        LOG_TELEM__MRAM_FAILURE();
#endif
    } else {
        loaded = 1;
    }

    return;
}

/**
 * @brief Puts the priority of the given telemetry id into output
*/
telem_err_t telem_get_priority(telem_id_t id, uint8_t *output) {

    // Check bounds
    if (id > TELEM_COUNT) {
        return TELEM_ERR_DNE;
    }

    if (loaded) {
        mram_err_t err = mram_read(telem_config_start_addr + id * sizeof(struct telem_config_vars) + offsetof(struct telem_config_vars, priority), 1,
                                   (uint8_t *)output);

        if (err != MRAM_OK) {
#if FEATURE_HW_MRAM
            LOG_TELEM__MRAM_FAILURE();
#endif
            *output = TELEM_SPEC_TABLE[id].priority;
        }
    } else {
        *output = TELEM_SPEC_TABLE[id].priority;
        telem_init_mram();
    }

    return TELEM_SUCCESS;
}

/**
 * @brief Puts the period of the given telemetry id into output
*/
telem_err_t telem_get_period(telem_id_t id, uint32_t *output) {

    // Check bounds
    if (id > TELEM_COUNT) {
        return TELEM_ERR_DNE;
    }

    if (loaded) {
        mram_err_t err = mram_read(telem_config_start_addr + id * sizeof(struct telem_config_vars) + offsetof(struct telem_config_vars, period), 4,
                                   (uint8_t *)output);

        if (err != MRAM_OK) {
#if FEATURE_HW_MRAM
            LOG_TELEM__MRAM_FAILURE();
#endif
            *output = TELEM_SPEC_TABLE[id].period;
        }
    } else {
        *output = TELEM_SPEC_TABLE[id].period;
        telem_init_mram();
    }

    return TELEM_SUCCESS;
}

/**
 * @brief sets the priority of the given telemetry id to input
*/
telem_err_t telem_set_priority(telem_id_t id, uint8_t input) {

    // Check bounds
    if (id > TELEM_COUNT) {
        return TELEM_ERR_DNE;
    }

    if (loaded) {
        mram_err_t err = mram_write(telem_config_start_addr + id * sizeof(struct telem_config_vars) + offsetof(struct telem_config_vars, priority), 1,
                                    (uint8_t *)&input);

        if (err != MRAM_OK) {
#if FEATURE_HW_MRAM
            LOG_TELEM__MRAM_FAILURE();
#endif
            return TELEM_ERR_MRAM;
        }
    } else {
        telem_init_mram();
        return TELEM_ERR_MRAM;
    }

    return TELEM_SUCCESS;
}

/**
 * @brief sets the period of the given telemetry id to input
*/
telem_err_t telem_set_period(telem_id_t id, uint32_t input) {

    // Check bounds
    if (id > TELEM_COUNT) {
        return TELEM_ERR_DNE;
    }

    if (loaded) {
        mram_err_t err = mram_write(telem_config_start_addr + id * sizeof(struct telem_config_vars) + offsetof(struct telem_config_vars, period), 4,
                                    (uint8_t *)&input);

        if (err != MRAM_OK) {
#if FEATURE_HW_MRAM
            LOG_TELEM__MRAM_FAILURE();
#endif
            return TELEM_ERR_MRAM;
        }
    } else {
        telem_init_mram();
        return TELEM_ERR_MRAM;
    }

    return TELEM_SUCCESS;
}

/**
 * @brief Enables telemetry collection after downlink
*/
void telem_collect_enable(void) {
    telem_collect_enabled = true;
}

/**
 * @brief Disables telemetry collection during downlink
*/
void telem_collect_disable(void) {
    telem_collect_enabled = false;
}

/**
 * @brief Serialize response and pass it to filesystem task
 *
 * @param[in] resp_struct Pointer struct containing response data
 * @param[in] resp_desc   Pointer to description of the response data struct for serialization using the data_fmt module
 * @param[in] resp_len    Number of bytes for the serialized response data
 * @param[in] buf         Buffer used to store serialized response data. Must be at least as large as resp_len.
 *
 * @return Status code:
 *            - TELEM_ERR_FS_TIMEOUT if writing to the filesystem task stream times out
 *            - TELEM_ERR_DATA_FMT if the response cannot be properly serialized
 *            - TELEM_ERR_FLASH_WRITE if filesystem returned an error
 */
telem_err_t telem_handle_resp(const telem_id_t id, const uint8_t priority, const void *resp_struct, const data_fmt_desc_t *resp_desc,
                              const uint16_t resp_len,
                              uint8_t *buf) {

#if FEATURE_FLASH_FS
    uint32_t written_bytes = data_fmt_serialize_struct(resp_struct, resp_desc, buf, resp_len);

    if (written_bytes != resp_len) {
        return TELEM_ERR_DATA_FMT;
    }

    generate_inner_header(buf + resp_len, id, resp_len);

    char filename[17] = "";
    convert_priority_to_filename(filename, priority);

    fs_err_t err = fs_write_enqueue(buf, resp_len + TELEM_INNER_HEADER_SIZE, filename, TELEM_FS_MUTEX_TIMEOUT_MS);

    if (err != FS_OK) {
        return (err == FS_MUTEX_TIMEOUT) ? TELEM_ERR_FS_TIMEOUT : TELEM_ERR_FLASH_WRITE;
    }

    switch (priority) {
    case 0:
        file_0_size += resp_len + TELEM_INNER_HEADER_SIZE;
        break;

    case 1:
        file_1_size += resp_len + TELEM_INNER_HEADER_SIZE;
        break;

    default: // Also doubles as case 2
        file_2_size += resp_len + TELEM_INNER_HEADER_SIZE;
    }

#endif
    return TELEM_SUCCESS;
}

/**
 * @brief Search for the last collected value of a telem unit
 *
 * @param[in] telem_id         ID of the telem unit
 * @param[in] buf              Pointer to buf to store value in (if found)
 * @param[in] exec_epoch       The epoch at which value was collected (if found)
 * @param[in] mutex_timeout    Timeout to wait for mutex to access flash
 *
 * @return Status code:
 *            - TELEM_ERR_FLASH_READ if filesystem returned an error
 *            - TELEM_ERR_DNE if telem_id does not exist
 *            - TELEM_ERR_ENTRY_DNE if could not find last value of telem unit
 *            - TELEM_ERR_FS_TIMEOUT if could not compete for filesystem mutex
 *            - TELEM_ERR_MUTEX_TIMEOUT if didn't wait for executing units to finish in time
 */
telem_err_t telem_get_last_value(const telem_id_t telem_id, uint8_t *buf, epoch_t *exec_epoch, uint16_t mutex_timeout) {
#if FEATURE_FLASH_FS

    if (telem_id >= TELEM_COUNT) {
        return TELEM_ERR_DNE;
    }

    uint8_t prio;
    telem_err_t perr = telem_get_priority(telem_id, &prio);

    if (perr != TELEM_SUCCESS) {
        return TELEM_ERR_ENTRY_DNE;
    }

    TickType_t total_ticks = pdMS_TO_TICKS(mutex_timeout);
    TickType_t start_ticks = xTaskGetTickCount();
    TickType_t cur_ticks;

    char filename[17] = "";
    convert_priority_to_filename(filename, prio);

    lfs_file_t file = { 0 };
    fs_err_t err = fs_open(&file, filename, pdTICKS_TO_MS(total_ticks));

    if (err != FS_OK) {
        return (err == FS_MUTEX_TIMEOUT) ? TELEM_ERR_FS_TIMEOUT : TELEM_ERR_FLASH_READ;
    }

    int32_t offset = 0;
    const int32_t file_size = fs_size(&file);

    if (file_size <= 14) { // Min size of outer and inner headers and stop word
        fs_close(&file);
        return TELEM_ERR_ENTRY_DNE;
    }

    // Searching backward as last value is closer to the end of the file
    if (fs_seek(&file, 0, FS_SEEK_END) != FS_OK) {
        return TELEM_ERR_FLASH_READ;
    }

    cur_ticks = xTaskGetTickCount();

    if (cur_ticks - start_ticks >= total_ticks) {
        fs_close(&file);
        return TELEM_ERR_SEARCH_TIMEOUT;
    }

    uint8_t stop_word_buf[TELEM_STOP_WORD_SIZE];

    // Loop will always terminate since offset increases at every iteration
    while (offset < file_size) {

        telem_err_t search_err = search_for_stop_word(&file, &offset, &file_size, stop_word_buf);

        if (search_err != TELEM_SUCCESS) {
            return TELEM_ERR_FLASH_READ;
        }

        epoch_t cur_epoch;
        uint16_t num_units;

        if (read_outer_header(&file, &offset, &cur_epoch, &num_units) != TELEM_SUCCESS) {
            return TELEM_ERR_FLASH_READ;
        }

        while (num_units > 0 && offset < file_size) {

            telem_id_t cur_telem_id;
            uint16_t resp_len;

            if (read_inner_header(&file, &offset, &cur_telem_id, &resp_len) != TELEM_SUCCESS) {
                return TELEM_ERR_FLASH_READ;
            }

            // Found the unit we're looking for
            if (cur_telem_id == telem_id) {
                // Set return value of epoch
                *exec_epoch = cur_epoch;

                // Cast to int32_t to ensure we don't overflow when we take the negative value
                if (fs_seek(&file, -((int32_t) resp_len), FS_SEEK_CUR) != FS_OK) {
                    return TELEM_ERR_FLASH_READ;
                }

                if (fs_read(&file, buf, resp_len) != FS_OK) {
                    return TELEM_ERR_FLASH_READ;
                }

                fs_close(&file);

                return TELEM_SUCCESS;
            }

            if (fs_seek(&file, -((int32_t) resp_len), FS_SEEK_CUR) != FS_OK) {
                return TELEM_ERR_FLASH_READ;
            }

            num_units--;
            offset += resp_len;
        }

        cur_ticks = xTaskGetTickCount();

        if (cur_ticks - start_ticks >= total_ticks) {
            fs_close(&file);
            return TELEM_ERR_SEARCH_TIMEOUT;
        }
    }

    fs_close(&file);
#endif
    return TELEM_ERR_ENTRY_DNE;
}

/**
 * @brief Adds outer header to flash write queue
*/
telem_err_t write_outer_header_and_stop_word(uint16_t num_units, char *filename, uint8_t priority) {
#if FEATURE_FLASH_FS
    uint8_t header_buf[TELEM_OUTER_HEADER_SIZE + TELEM_STOP_WORD_SIZE];

    if (generate_outer_header(header_buf, num_units) != TELEM_SUCCESS) {
        return TELEM_ERR_GEN_HEADER;
    }

    memcpy(header_buf + TELEM_OUTER_HEADER_SIZE, &telem_stop_word, TELEM_STOP_WORD_SIZE);

    if (fs_write_enqueue(header_buf, TELEM_OUTER_HEADER_SIZE + TELEM_STOP_WORD_SIZE, filename, TELEM_FS_MUTEX_TIMEOUT_MS) != FS_OK) {
        return TELEM_ERR_FLASH_WRITE;
    }

    switch (priority) {
    case 0:
        file_0_size += TELEM_OUTER_HEADER_SIZE + TELEM_STOP_WORD_SIZE;
        break;

    case 1:
        file_1_size += TELEM_OUTER_HEADER_SIZE + TELEM_STOP_WORD_SIZE;
        break;

    default: // Also doubles as case 2
        file_2_size += TELEM_OUTER_HEADER_SIZE + TELEM_STOP_WORD_SIZE;
    }

#endif
    return TELEM_SUCCESS;
}

/******************************************************************************/
/*                      P R I V A T E  F U N C T I O N S                      */
/******************************************************************************/

/**
 * @brief Telemetry collection task
 * Wakes up at set collection periods and invokes telemetry unit functions
*/
static void telem_collect_task(void *pvParameters) {

    TickType_t last_wake_time = xTaskGetTickCount();
    TickType_t poll_period_ticks = pdMS_TO_TICKS(TELEM_COLLECT_POLL_PERIOD_S * 1000);

    uint32_t elapsed_period = TELEM_COLLECT_POLL_PERIOD_S;

    vTaskDelayUntil(&last_wake_time, poll_period_ticks);

    while (1) {

        obc_watchdog_pet(OBC_TASK_ID_TELEM_COLLECT);

        /* If there are telem units to be invoked, they are enqueued onto the telem_exec task queue
         * If not, wake the task again after TELEM_COLLECT_POLL_PERIOD_MS
        */

        do {

            if (!telem_collect_enabled) {
                break;
            }

#if FEATURE_FLASH_FS

            if (check_file_sizes() != TELEM_SUCCESS) {
                LOG_TELEM__COLLECTION_FAILURE(TELEM_ERR_FILE_SIZE);
            }

#endif

            uint16_t num_priority_0 = 0;
            uint16_t num_priority_1 = 0;
            uint16_t num_priority_2 = 0;

            for (uint8_t i = 0; i < LEN(TELEM_UNITS_DICT_PERIODS); i++) {
                if (elapsed_period % TELEM_UNITS_DICT_PERIODS[i] == 0) {
                    for (uint16_t j = 0; j < TELEM_UNITS_DICT_SIZES[i]; j++) {

                        telem_id_t telem_id = TELEM_UNITS_DICT_SPECS[i][j];

                        uint8_t prio;
                        telem_err_t err = telem_get_priority(telem_id, &prio);

                        if (err == TELEM_SUCCESS) {
                            switch (prio) {
                            case 0:
                                num_priority_0++;
                                break;

                            case 1:
                                num_priority_1++;
                                break;

                            default: // Also doubles as case 2
                                num_priority_2++;
                            }
                        }

                        telem_exec_enqueue((uint16_t) telem_id);
                    }
                }
            }

            // Add outer headers to queue and notify telem_exec task to wake up and read from queue
            if (num_priority_0 || num_priority_1 || num_priority_2) {
                telem_exec_enqueue((uint16_t) TELEM_QUEUE_MAGIC_NUM);
                telem_exec_enqueue(num_priority_0);
                telem_exec_enqueue(num_priority_1);
                telem_exec_enqueue(num_priority_2);

                xTaskNotifyGive(telem_exec_task_handle);
            }
        } while (0);

        // Make sure we don't overflow uint32_t
        if (elapsed_period >= TELEM_LONGEST_COLLECTION_PERIOD) {
            elapsed_period = 0;
        }

        elapsed_period += TELEM_COLLECT_POLL_PERIOD_S;
        vTaskDelayUntil(&last_wake_time, poll_period_ticks);
    }
}

#if FEATURE_FLASH_FS
/**
 * @brief Generates the outer header for all telem units for a given timestamp
 *
 * Format : { epoch_t cur_epoch, uint16_t num_units } serialized as uint8_t array
*/
static telem_err_t generate_outer_header(uint8_t *header_buf, uint16_t num_units) {
    epoch_t cur_epoch = rtc_get_epoch_time();

    if (cur_epoch == RTC_EPOCH_ERR) {
        return TELEM_ERR_RTC_ERR;
    }

    // Serialize into byte array
    // epoch_t is casted into its native type of int32_t
    data_fmt_u32_to_arr_be((int32_t) cur_epoch, header_buf);
    data_fmt_u16_to_arr_be(num_units, header_buf + sizeof(int32_t));

    return TELEM_SUCCESS;
}

/**
 * @brief Generates the inner header associated with a singular telem unit
 *
 * Format : { telem_id_t telem_id, uint16_t resp_len } serialized as uint8_t array
*/
static void generate_inner_header(uint8_t *header_buf, telem_id_t id, uint16_t resp_len) {
    // Serialize into byte array
    // telem_id_t is casted into its native type of uint16_t
    data_fmt_u16_to_arr_be((uint16_t) id, header_buf);
    data_fmt_u16_to_arr_be(resp_len, header_buf + sizeof(uint16_t));
}

/**
 * @brief Deserializes outer header into constituent parts
*/
static void deserialize_outer_header(const uint8_t *header_buf, epoch_t *epoch, uint16_t *num_units) {
    *epoch = (epoch_t) data_fmt_arr_be_to_u32(header_buf);
    *num_units = data_fmt_arr_be_to_u16(header_buf + sizeof(int32_t));
}

/**
 * @brief Deserializes inner header into constituent parts
*/
static void deserialize_inner_header(const uint8_t *header_buf, telem_id_t *id, uint16_t *resp_len) {
    *id = (telem_id_t) data_fmt_arr_be_to_u16(header_buf);
    *resp_len = data_fmt_arr_be_to_u16(header_buf + sizeof(uint16_t));
}

/*
 * @brief Reads the outer header and moves cursor back to the start of the header
 *
 * @return This function should call fs_close() before returning anything other than TELEM_SUCCESS
 */
static telem_err_t read_outer_header(lfs_file_t *file, int32_t *offset, epoch_t *cur_epoch, uint16_t *num_units) {
    if (fs_seek(file, -TELEM_OUTER_HEADER_SIZE, FS_SEEK_CUR) != FS_OK) {
        return TELEM_ERR_FLASH_READ;
    }

    uint8_t outer_header[TELEM_OUTER_HEADER_SIZE];

    if (fs_read(file, outer_header, TELEM_OUTER_HEADER_SIZE) != FS_OK) {
        return TELEM_ERR_FLASH_READ;
    }

    deserialize_outer_header(outer_header, cur_epoch, num_units);

    if (fs_seek(file, -TELEM_OUTER_HEADER_SIZE, FS_SEEK_CUR) != FS_OK) {
        return TELEM_ERR_FLASH_READ;
    }

    *offset += TELEM_OUTER_HEADER_SIZE;
    return TELEM_SUCCESS;
}

/*
 * @brief Reads the inner header and moves cursor back to the start of the header
 *
 * @return This function should call fs_close() before returning anything other than TELEM_SUCCESS
 */
static telem_err_t read_inner_header(lfs_file_t *file, int32_t *offset, telem_id_t *cur_telem_id, uint16_t *resp_len) {
    if (fs_seek(file, -TELEM_INNER_HEADER_SIZE, FS_SEEK_CUR) != FS_OK) {
        return TELEM_ERR_FLASH_READ;
    }

    uint8_t inner_header[TELEM_INNER_HEADER_SIZE];

    if (fs_read(file, inner_header, TELEM_INNER_HEADER_SIZE) != FS_OK) {
        return TELEM_ERR_FLASH_READ;
    }

    deserialize_inner_header(inner_header, cur_telem_id, resp_len);

    if (fs_seek(file, -TELEM_INNER_HEADER_SIZE, FS_SEEK_CUR) != FS_OK) {
        return TELEM_ERR_FLASH_READ;
    }

    *offset += TELEM_INNER_HEADER_SIZE;
    return TELEM_SUCCESS;
}

/*
 * @brief Returns TELEM_SUCCESS if we meet a sequence of bytes in the file that matches the stop word
 *
 * @return This function should call fs_close() before returning anything other than TELEM_SUCCESS
 */
static telem_err_t search_for_stop_word(lfs_file_t *file, int32_t *offset, const int32_t *file_size, uint8_t *stop_word_buf) {
    while (*offset < *file_size) {
        if (fs_seek(file, -TELEM_STOP_WORD_SIZE, FS_SEEK_CUR) != FS_OK) {
            return TELEM_ERR_FLASH_READ;
        }

        if (fs_read(file, stop_word_buf, TELEM_STOP_WORD_SIZE) != FS_OK) {
            return TELEM_ERR_FLASH_READ;
        }

        if (memcmp(telem_stop_word, stop_word_buf, TELEM_STOP_WORD_SIZE)) {
            if (fs_seek(file, -1, FS_SEEK_CUR) != FS_OK) {
                return TELEM_ERR_FLASH_READ;
            }

            *offset += 1;
            continue; // Keep searching
        }

        // Found the stop word
        if (fs_seek(file, -TELEM_STOP_WORD_SIZE, FS_SEEK_CUR) != FS_OK) {
            return TELEM_ERR_FLASH_READ;
        }

        *offset += TELEM_STOP_WORD_SIZE;
        return TELEM_SUCCESS;
    }

    fs_close(file);
    return TELEM_ERR_ENTRY_DNE;
}

/**
 * @brief Check file sizes and reset to 0 if necessary
*/
static telem_err_t check_file_sizes(void) {
    if (file_0_size >= (int32_t) TELEM_PRIORITY_0_MAX_FILE_SIZE) {
        if (fs_remove("telem_priority_0", TELEM_FS_MUTEX_TIMEOUT_MS) != FS_OK) {
            return TELEM_ERR_FILE_SIZE;
        }
    }

    if (file_1_size >= (int32_t) TELEM_PRIORITY_1_MAX_FILE_SIZE) {
        if (fs_remove("telem_priority_1", TELEM_FS_MUTEX_TIMEOUT_MS) != FS_OK) {
            return TELEM_ERR_FILE_SIZE;
        }
    }

    if (file_2_size >= (int32_t) TELEM_PRIORITY_2_MAX_FILE_SIZE) {
        if (fs_remove("telem_priority_2", TELEM_FS_MUTEX_TIMEOUT_MS) != FS_OK) {
            return TELEM_ERR_FILE_SIZE;
        }
    }

    return TELEM_SUCCESS;
}
#endif
