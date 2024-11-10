/**
 * @file test_cube_util.c
 * @brief CubeADCS - utility library unit tests
 * @author Andres Martinez
 * @date Oct 9, 2020
 *
 */

#include "unity.h"

#include "cube_util.h"

#include <stdlib.h>
#include <string.h>

// Tests
void test_tcm_ack();
void test_comms_status();

// Test telecommand ACK frames
static const cube_tcm_ack_t TCM_ACK_OBJ = {
    240,                        // ID of last received TC
    true,                       // Flag to indicate if the last TC has been processed.
    TcErrorReason_IncorrectLen, // Status of last processed telecommand
    2,                          // Index of incorrect TC parameter
};

// Test communication status frames
static const cube_comms_status_t COMMS_STATUS_OBJ = {
    123,   // No. of telecommands received
    456,   // No. of telemetry requests received
    true,  // TC buffer was overrun while receiving a telecommand
    false, // UART protocol error occurred
    true,  // UART start-of-message identifier was received without a preceding end-of-message
    true,  // Number of data clocked out was more than telemetry package
    false, // Telecommand sent exceeds buffer size
    true,  // Telecommand sent exceeds buffer size
};

static char buff[CUBE_COMPUTER_MAX_PAYLOAD_SIZE];

/**
 * @brief Test TCM acknowledgement frame serialization and parsing.
 * Checks that a preset data frame is un-changed after being serialized then de-serialized.
 */
void test_tcm_ack() {
    cube_serialize_tcm_ack(&TCM_ACK_OBJ, buff);

    /* Initialize test struct memory with calloc so that padding bytes are zero
     * This will allow shallow equality comparison of test vs expected values using memcmp
     */
    cube_tcm_ack_t* test_obj = (cube_tcm_ack_t*)calloc(1, sizeof(cube_tcm_ack_t));
    TEST_ASSERT_NOT_EQUAL(0, memcmp(test_obj, &TCM_ACK_OBJ, sizeof(TCM_ACK_OBJ)));

    cube_parse_tcm_ack(buff, test_obj);

    TEST_ASSERT_EQUAL(0, memcmp(test_obj, &TCM_ACK_OBJ, sizeof(TCM_ACK_OBJ)));

    free(test_obj);
}

/**
 * @brief Test CubeComputer comms status frame serialization and parsing.
 * Checks that a preset data frame is un-changed after being serialized then de-serialized.
 */
void test_comms_status() {
    cube_serialize_comms_status(&COMMS_STATUS_OBJ, buff);

    /* Initialize test struct memory with calloc so that padding bytes are zero
     * This will allow shallow equality comparison of test vs expected values using memcmp
     */
    cube_comms_status_t* test_obj = (cube_comms_status_t*)calloc(1, sizeof(cube_comms_status_t));
    TEST_ASSERT_NOT_EQUAL(0, memcmp(test_obj, &COMMS_STATUS_OBJ, sizeof(COMMS_STATUS_OBJ)));

    cube_parse_comms_status(buff, test_obj);

    TEST_ASSERT_EQUAL(0, memcmp(test_obj, &COMMS_STATUS_OBJ, sizeof(COMMS_STATUS_OBJ)));

    free(test_obj);
}
