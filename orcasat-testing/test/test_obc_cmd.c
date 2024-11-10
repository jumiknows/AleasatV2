#include "unity.h"
#include "obc_cmd.h"
#include "obc_utils.h"
#include "obc_error.h"
#include "obc_time.h"
#include "mock_logger.h"
#include "mock_scheduler.h"
#include "mock_obc_heartbeat.h"
#include "mock_obc_prompt_cmd_actions.h"

/**
 * @brief Tests for the command system.
 *
 * This video was made as a tutorial based on these tests:
 * https://www.youtube.com/watch?v=QnkO4q95o6s
 */

// Tests
void test_basic_cmd_immediate(void);
void test_basic_cmd_immediate_args(void);
void test_basic_cmd_immediate_period(void);
void test_basic_cmd_scheduled(void);
void test_basic_cmd_scheduled_no_interval(void);
void test_basic_cmd_sched_fail(void);
void test_internal_cmd_immediate_period(void);
void test_internal_cmd_sched_fail(void);
void test_seq_num_incrementing(void);
void test_seq_num_valid_and_invalid(void);

/**
 * @brief Required by Ceedling, and executed before test functions run.
 *
 * Set the sequence number variable within obc_cmd.c to 1, which is the
 * value it takes at OBC startup. Since Ceedling doesn't run test functions
 * in different threads, if we didn't do this, the saved sequence number
 * in the file would be carried over between tests and could cause issues
 * with the sequence numbers that we expect to have in tests.
 */
void setUp(void) {
    ceedling_set_seq_num(1);
}

/**
 * @brief Tests that a basic immediate command can be scheduled immediately
 * with a period of 0.
 */
void test_basic_cmd_immediate(void) {
    char cmd[] = "0 ack * 0";

    // Set up the expectations for the scheduler call.
    // Expect 0 arguments and 0 period.
    // NULL is used for params that are ignored in this test.
    scheduler_schedule_action_start_now_ExpectAndReturn(NULL, EXTERNAL, 0, NULL, 0, NULL, SCHEDULER_SUCCESS);
    scheduler_schedule_action_start_now_IgnoreArg_id();
    scheduler_schedule_action_start_now_IgnoreArg_action();
    scheduler_schedule_action_start_now_IgnoreArg_argument();

    // Expect a response from the command system.
    log_str_no_time_Expect(INFO, CMD_GENERAL, false, "> %s immediate");

    // Invoke the command.
    TEST_ASSERT_EQUAL_INT32(OBC_IMMEDIATE_SUCCESS, obc_cmd_parse_and_invoke(cmd, EXTERNAL));
}

/**
 * @brief Tests that a basic immediate command will be scheduled
 * immediately with args.
 */
void test_basic_cmd_immediate_args(void) {
    char cmd[] = "0 ack * 0 23 99";

    // Set up the expectations for the scheduler call.
    // Expect a period of 0 (one execution) and two arguments.
    // NULL is used for params that are ignored in this test.
    scheduler_schedule_action_start_now_ExpectAndReturn(NULL, EXTERNAL, 0, NULL, 2 * ARGUMENT_SIZE, 0, SCHEDULER_SUCCESS);
    scheduler_schedule_action_start_now_IgnoreArg_id();
    scheduler_schedule_action_start_now_IgnoreArg_action();
    scheduler_schedule_action_start_now_IgnoreArg_argument();

    // Expect a response from the command system.
    log_str_no_time_Expect(INFO, CMD_GENERAL, false, "> %s immediate");

    // Invoke the command.
    TEST_ASSERT_EQUAL_INT32(OBC_IMMEDIATE_SUCCESS, obc_cmd_parse_and_invoke(cmd, EXTERNAL));
}

/**
 * @brief Tests that a basic immediate command will be scheduled
 * immediately with a nonzero period.
 */
void test_basic_cmd_immediate_period(void) {
    char cmd[] = "0 ack * 5";

    // Set up the expectations for the scheduler call. Expect to see a period of 5.
    // NULL is used for params that are ignored in this test.
    scheduler_schedule_action_start_now_ExpectAndReturn(NULL, EXTERNAL, 5, NULL, 0, NULL, SCHEDULER_SUCCESS);
    scheduler_schedule_action_start_now_IgnoreArg_id();
    scheduler_schedule_action_start_now_IgnoreArg_action();
    scheduler_schedule_action_start_now_IgnoreArg_argument();

    // Expect a response from the command system.
    log_str_no_time_Expect(INFO, CMD_GENERAL, false, "> %s immediate");

    // Invoke the command.
    TEST_ASSERT_EQUAL_INT32(OBC_IMMEDIATE_SUCCESS, obc_cmd_parse_and_invoke(cmd, EXTERNAL));
}

/**
 * @brief Tests that a basic scheduled command can get scheduled.
 */
void test_basic_cmd_scheduled(void) {
    char cmd[] = "0 ack 20-01-01 0:0:50 5";
    real_time_t time;
    action_id_t id;

    // Set up the expectations for the scheduler call.
    // NULL is used for params that are ignored in this test.
    scheduler_schedule_action_ExpectAndReturn(NULL, EXTERNAL, time, 5, NULL, 0, NULL, SCHEDULER_SUCCESS);
    scheduler_schedule_action_IgnoreArg_id();
    scheduler_schedule_action_IgnoreArg_start_at();
    scheduler_schedule_action_IgnoreArg_action();
    scheduler_schedule_action_IgnoreArg_argument();

    // Expect a response from the command system.
    log_str_no_time_Expect(INFO, CMD_GENERAL, false, "> %s scheduled %d");

    // Invoke the command.
    TEST_ASSERT_EQUAL_INT32(OBC_SCHEDULED_SUCCESS, obc_cmd_parse_and_invoke(cmd, EXTERNAL));
}

/**
 * @brief Tests that a scheduled command with no interval will return
 * an error message.
 */
void test_basic_cmd_scheduled_no_interval(void) {
    char cmd[] = "0 ack 20-01-01 0:0:5";

    // Expect an error response from the command system to the command with no interval.
    log_str_Expect(ERROR, CMD_GENERAL, true, "> %s no interval");

    // Invoke the command.
    TEST_ASSERT_EQUAL_INT32(OBC_CMD_NO_INTERVAL, obc_cmd_parse_and_invoke(cmd, EXTERNAL));
}

/**
 * @brief Tests that if the scheduler gives an error, a message will be reported when
 * a command is issued.
 */
void test_basic_cmd_sched_fail(void) {
    char cmd[] = "0 ack * 0";

    // Command processing calls scheduler_schedule_action_start now.
    // Mock this function but have it return an INVALD_REALTIME error.
    scheduler_schedule_action_start_now_IgnoreAndReturn(INVALID_REALTIME);

    // Expect that the command system will report the scheduler error.
    log_str_Expect(ERROR, CMD_GENERAL, false, "> %s sched err");

    // Invoke and parse the command.
    TEST_ASSERT_EQUAL_INT32(OBC_CMD_FAILED_TO_SCHEDULE, obc_cmd_parse_and_invoke(cmd, EXTERNAL));
}

/**
 * @brief Tests that a internally-scheduled immediate command will be scheduled
 * immediately with a nonzero period.
 */
void test_internal_cmd_immediate_period(void) {
    char cmd[] = "0 ack * 5";

    // Set up the expectations for the scheduler call. Expect to see a period of 5.
    // NULL is used for params that are ignored in this test.
    scheduler_schedule_action_start_now_ExpectAndReturn(NULL, INTERNAL, 5, NULL, 0, NULL, SCHEDULER_SUCCESS);
    scheduler_schedule_action_start_now_IgnoreArg_id();
    scheduler_schedule_action_start_now_IgnoreArg_action();
    scheduler_schedule_action_start_now_IgnoreArg_argument();

    // Invoke the command.
    TEST_ASSERT_EQUAL_INT32(OBC_IMMEDIATE_SUCCESS, obc_cmd_parse_and_invoke(cmd, INTERNAL));
}

/**
 * @brief Tests that if the scheduler gives an error, a message will be logged
 * when a command is scheduled internally.
 */
void test_internal_cmd_sched_fail(void) {
    char cmd[] = "0 ack * 0";

    // Command processing calls scheduler_schedule_action_start now.
    // Mock this function but have it return an INVALD_REALTIME error.
    scheduler_schedule_action_start_now_IgnoreAndReturn(INVALID_REALTIME);

    // Expect that the command system will log the scheduler error.
    log_str_Expect(ERROR, CMD_GENERAL, false, "> %s sched err");

    // Invoke and parse the command.
    TEST_ASSERT_EQUAL_INT32(OBC_CMD_FAILED_TO_SCHEDULE, obc_cmd_parse_and_invoke(cmd, INTERNAL));
}

/**
 * @brief Tests that the sequence number increments and rolls over
 * appropriately when commands are received.
 */
void test_seq_num_incrementing(void) {
    char cmd[] = "0 ack * 0";

    // The test startup function should have set the sequence number to 1.
    TEST_ASSERT_EQUAL(1, next_seq_num());

    // We want to check the rollover to 1, but unfortunately due to how
    // Ceedling queues expected functions, it doesn't have enough memory
    // to blast through hundreds of iterations of incrementing checking.
    // For this reason, artificially set the sequence number to where it
    // will soon overflow and should reset to 1, and begin checking for
    // valid incrementing behaviour from there.
    // See https://github.com/ThrowTheSwitch/CMock/issues/96
    ceedling_set_seq_num(250);

    uint8_t next_expected_seq_num = 250;

    // Check the incrementing behaviour before and after it rolls over,
    // but don't do so many iterations that Ceedling falls over.
    for (uint16_t i = 0; i < 30; i++) {
        // Check the sequence number reported by OBC
        TEST_ASSERT_EQUAL(next_expected_seq_num, next_seq_num());

        // Set up the expectations for the scheduler call and the response
        // from the command system.
        scheduler_schedule_action_start_now_IgnoreAndReturn(SCHEDULER_SUCCESS);
        log_str_no_time_Expect(INFO, CMD_GENERAL, false, "> %s immediate");

        // Invoke the command.
        TEST_ASSERT_EQUAL_INT32(OBC_IMMEDIATE_SUCCESS, obc_cmd_parse_and_invoke(cmd, EXTERNAL));

        // Increment the next expected sequence number.
        if (next_expected_seq_num == 255) {
            next_expected_seq_num = 1;
        } else {
            next_expected_seq_num++;
        }
    }
}

/**
 * @brief Tests that commands with valid and invalid sequence numbers
 * are accepted and rejected as appropriate.
 */
void test_seq_num_valid_and_invalid(void) {
    char cmd_buf[20] = {'\0'};
    sprintf(cmd_buf, "%d ack * 0", 1);

    // The test startup function should have set the sequence number to 1.
    TEST_ASSERT_EQUAL(1, next_seq_num());

    // Expect that the sequence number of 1 is accepted since it's the next valid.
    scheduler_schedule_action_start_now_IgnoreAndReturn(SCHEDULER_SUCCESS);
    log_str_no_time_Expect(INFO, CMD_GENERAL, false, "> %s immediate");
    TEST_ASSERT_EQUAL_INT32(OBC_IMMEDIATE_SUCCESS, obc_cmd_parse_and_invoke(cmd_buf, EXTERNAL));

    // Expect that the sequence number of 2 is accepted since it's the next valid.
    sprintf(cmd_buf, "%d ack * 0", 2);
    scheduler_schedule_action_start_now_IgnoreAndReturn(SCHEDULER_SUCCESS);
    log_str_no_time_Expect(INFO, CMD_GENERAL, false, "> %s immediate");
    TEST_ASSERT_EQUAL_INT32(OBC_IMMEDIATE_SUCCESS, obc_cmd_parse_and_invoke(cmd_buf, EXTERNAL));

    // Expect that the sequence number of 5 is not accepted because it's out of sequence.
    sprintf(cmd_buf, "%d ack * 0", 5);
    log_str_Expect(ERROR, CMD_GENERAL, true, "> %s invalid seq num");
    TEST_ASSERT_EQUAL_INT32(OBC_SEQ_NUM_INVALID, obc_cmd_parse_and_invoke(cmd_buf, EXTERNAL));

    // Check that the sequence number has been incremented to 4, which
    // should happen even though we tried to submit an out of sequence
    // command.
    TEST_ASSERT_EQUAL(4, next_seq_num());

    // Expect that the sequence number of 256 is not accepted because it's out of sequence and too large.
    sprintf(cmd_buf, "%d ack * 0", 256);
    log_str_Expect(ERROR, CMD_GENERAL, true, "> %s invalid seq num");
    TEST_ASSERT_EQUAL_INT32(OBC_SEQ_NUM_INVALID, obc_cmd_parse_and_invoke(cmd_buf, EXTERNAL));
    TEST_ASSERT_EQUAL(5, next_seq_num());
}