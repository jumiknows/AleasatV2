// /*******************************************************
//  * scheduler.c
//  *
//  * Created on: May 22, 2019
//  *  Author(s): Alireza.A
//  *
//  * Scheduler and Executer communicate through the actions_queue:
//  *
//  * The scheduler is responsible for adding, removing and triggering actions.
//  * The scheduler serializes its state for persistence.
//  * The scheduler triggers an action by enquing an action_call (all the information required for
//  *executing) to the actions_queue.
//  *
//  * An action_call holds a function pointer and argument to the function.
//  * The executer dequeues an action_call from the actions_queue and calls the function pointer with
//  *the argument.
//  *
//  * The run time optimization and serialization are decoupled the scheduling and are triggered by one
//  *of the following conditions:
//  * - When the number of diffs between the in memory representation and the serialized object have
//  *passed a threshold
//  * - After a fixed period of time has passed since the sort/sync has occured
//  *
//  *******************************************************/

// #include "scheduler.h"
// #include "obc_task_info.h"
// #include "rtos.h"
// #include "logger.h"
// #include "obc_watchdog.h"
// #include "obc_time.h"
// #include "sys_common.h"
// #include <string.h>
// #include <stdbool.h>

// #define MAX_NUM_ACTIONS    20 // Maximum number of active actions at any point in time
// #define ACTIONS_QUEUE_SIZE 16 // Size of the queue between the scheduler and executer tasks

// #define ACTION_CALL_SIZE (sizeof(action_call_header_t) + MAX_ARGUMENT_SIZE)
// #define ACTION_INFO_SIZE (sizeof(action_info_header_t) + MAX_ARGUMENT_SIZE)

// ASSERT(ACTION_CALL_SIZE < SCHEDULER_TASK_STACK_SIZE)

// #define SYNC_ACTION_CHANGE_THRESHOLD 10  // number of state changes before a sync is required
// #define SORT_ACTION_CHANGE_THRESHOLD 100 // number of state changes before a sort is required

// /**
//  * @brief States that an action slot can take.
//  * @warning Update @ref action_state_strings if changing this enum.
//  * @warning This enum must start at 0 and monotonically increase so that
//  * it can be used to index @ref action_state_strings.
//  */
// typedef enum {
//     EMPTY               = 0, // has never been allocated
//     ACTIVE              = 1, // any action that's not COMPLETED or MARKED_FOR_DELETION
//     COMPLETED           = 2, // action is not periodic and has been executed
//     MARKED_FOR_DELETION = 3  // action has been marked for deletion
// } action_state_t;

// /**
//  * @brief String representations of the possible action states,
//  * indexable by action_state_t.
//  * Must match the order and meaning of @ref action_state_t.
//  */
// const char* action_state_strings[4] = {"EMPTY", "ACTIVE", "COMPLETE", "DELETE"};

// /**
//  * @brief String representations of the submission sources.
//  * Indexable by @ref scheduler_submission_source_t.
//  */
// const char* submission_source_strings[2] = {"INT", "EXT"};

// typedef struct {
//     action_id_t id;
//     action_state_t state;
//     scheduler_submission_source_t source;
//     real_time_t submission_time;
//     real_time_t start_at;
//     uint32_t interval;
//     scheduler_action_t action;
//     uint8_t argument_size;
// } action_info_header_t;

// typedef struct {
//     scheduler_action_t action;
//     uint8_t argument_size;
// } action_call_header_t;

// static void vSchedulerTask(void* pvParameters);
// static void vExecuterTask(void* pvParameters);
// static void vSyncTask(void* pvParameters);
// static void vSortTask(void* pvParameters);

// static int32_t find_insertion_point(void);
// static scheduler_err_t serialize(void);
// static scheduler_err_t deserialize(void);
// static void handle_state_change(void);
// static void sort_actions(void);
// static int32_t find_action(action_id_t id);
// static bool should_execute_action(action_info_header_t* action_info_header, real_time_t* current_time);
// static void create_action_call(uint8_t* action_info, uint8_t* action_call);

// // --------------------------------- VARIABLES ---------------------------------

// // Queue
// QueueHandle_t actions_queue;
// uint8_t actions_queue_storage_buffer[ACTIONS_QUEUE_SIZE * ACTION_CALL_SIZE];
// StaticQueue_t actions_queue_buffer;

// // Mutex
// SemaphoreHandle_t actions_mutex = NULL;
// StaticSemaphore_t actions_mutex_buffer;

// // Tasks
// TaskHandle_t scheduler_task_handle = NULL;
// StackType_t scheduler_stack_buffer[SCHEDULER_TASK_STACK_SIZE];
// StaticTask_t scheduler_task_buffer;

// TaskHandle_t executer_task_handle = NULL;
// StackType_t executer_stack_buffer[EXECUTER_TASK_STACK_SIZE];
// StaticTask_t executer_task_buffer;

// TaskHandle_t sync_task_handle = NULL;
// StackType_t sync_stack_buffer[SYNC_TASK_STACK_SIZE];
// StaticTask_t sync_task_buffer;

// TaskHandle_t sort_task_handle = NULL;
// StackType_t sort_stack_buffer[SORT_TASK_STACK_SIZE];
// StaticTask_t sort_task_buffer;

// // ----------------------------------- STATE -----------------------------------

// uint8_t actions[MAX_NUM_ACTIONS * ACTION_INFO_SIZE] = {'\0'};
// // number of modifications to the actions state variable, used for serialization/optimization of the
// // scheduler
// uint32_t state_changes;
// action_id_t idx = 0;

// // ------------------------------------ API ------------------------------------

// /**
//  * @brief Creates mutexes and the actions queue.
//  *
//  * Once these pieces are created, actions can be placed on the queue. This allows commands to be
//  * processed soon after startup.
//  */
// void scheduler_create_infra(void) {
//     actions_queue = xQueueCreateStatic(ACTIONS_QUEUE_SIZE, ACTION_CALL_SIZE, actions_queue_storage_buffer, &actions_queue_buffer);
//     actions_mutex = xSemaphoreCreateMutexStatic(&actions_mutex_buffer);
// }

// /**
//  * scheduler_start
//  *
//  * Deserializes the actions for the non-volatile memory.
//  * Creates the actions-queue along with scheduler, executer and sync tasks.
//  *
//  * returns: scheduler return code of SCHEDULER_SUCCESS when it SCHEDULER_SUCCESSfully initialized,
//  * otherwise an error.
//  *
//  */
// scheduler_err_t scheduler_start(void) {
//     scheduler_err_t errcode = SCHEDULER_SUCCESS;

//     xSemaphoreTake(actions_mutex, portMAX_DELAY);
//     errcode = deserialize();
//     xSemaphoreGive(actions_mutex);
//     if (errcode != SCHEDULER_SUCCESS) {
//         return errcode;
//     }

//     scheduler_task_handle = task_create_static(&vSchedulerTask, "scheduler_task", SCHEDULER_TASK_STACK_SIZE, NULL, SCHEDULER_TASK_PRIORITY, scheduler_stack_buffer, &scheduler_task_buffer);
//     if (!scheduler_task_handle) {
//         return FAILED_TO_CREATE_SCHEDULER_TASK;
//     }

//     executer_task_handle = task_create_static(&vExecuterTask, "executer_task", EXECUTER_TASK_STACK_SIZE, NULL, EXECUTER_TASK_PRIORITY, executer_stack_buffer, &executer_task_buffer);
//     if (!executer_task_handle) {
//         return FAILED_TO_CREATE_EXECUTER_TASK;
//     }

//     sync_task_handle = task_create_static(&vSyncTask, "sync_task", SYNC_TASK_STACK_SIZE, NULL, SYNC_TASK_PRIORITY, sync_stack_buffer, &sync_task_buffer);
//     if (!sync_task_handle) {
//         return FAILED_TO_CREATE_SYNC_TASK;
//     }

//     sync_task_handle = task_create_static(&vSortTask, "sort_task", SORT_TASK_STACK_SIZE, NULL, SORT_TASK_PRIORITY, sort_stack_buffer, &sort_task_buffer);
//     if (!sync_task_handle) {
//         return FAILED_TO_CREATE_SORT_TASK;
//     }

//     state_changes = 0;

//     return SCHEDULER_SUCCESS;
// }

// /**
//  * @brief Schedules an action to start now.
//  *
//  *  @param[out] id  A unique identifier for the action, populated by this function.
//  *  @param[in] source EXTERNAL or INTERNAL, whether this action was scheduled by an internal OBC
//  * task or by Houston.
//  *  @param[in] interval The period between each execution of the action. If 0, the action will
//  * execute only once.
//  *  @param[in] action A pointer to the action; a function pointer.
//  *  @param[in] argument_size The size of the action argument in bytes.
//  *  @param[in] argument A pointer to the action argument.
//  *
//  * @returns Scheduler return code of SCHEDULER_SUCCESS, otherwise an error.
//  */
// scheduler_err_t scheduler_schedule_action_start_now(action_id_t* id, scheduler_submission_source_t source, uint32_t interval, scheduler_action_t action, uint8_t argument_size, void* argument) {
//     scheduler_invoke_action(action, argument_size, argument);

//     if (interval == 0) {
//         return SCHEDULER_SUCCESS;
//     }

//     // find the next invocation time
//     real_time_t start_at = orca_time_init;
//     rtc_err_t errcode    = get_current_time(&start_at);
//     epoch_t start_epoch  = start_at.epoch + interval;
//     epoch_to_real_time(start_epoch, &start_at);
//     if (errcode != RTC_NO_ERR) {
//         return TIME_FAILURE;
//     }
//     return scheduler_schedule_action(id, source, start_at, interval, action, argument_size, argument);
// }

// /**
//  * @brief immediately invokes an action.
//  *
//  *  @param[in] action A pointer to the action; a function pointer.
//  *  @param[in] argument_size The size of the action argument in bytes.
//  *  @param[in] argument A pointer to the action argument.
//  */
// void scheduler_invoke_action(scheduler_action_t action, uint8_t argument_size, void* argument) {
//     // invoke the action immediately by pushing it to the action-queue
//     uint8_t action_call[ACTION_CALL_SIZE]    = {'\0'};
//     action_call_header_t* action_call_header = (action_call_header_t*)action_call;
//     action_call_header->action               = action;
//     action_call_header->argument_size        = argument_size;
//     memcpy(action_call + sizeof(action_call_header_t), argument, argument_size);
//     xQueueSend(actions_queue, action_call, portMAX_DELAY);
// }

// /**
//  * @brief Schedules an action to start at some point in the future.
//  *
//  *  @param[out] id  Uniquely identifies the action, populated by this function.
//  *  @param[in] source EXTERNAL or INTERNAL, whether this action was scheduled by an internal OBC
//  * task or by Houston.
//  *  @param[in] start_at When the action should first be executed. Real time fields are used, not
//  * epoch.
//  *  @param[in] interval The period between each execution of the action. If 0, the action will
//  * execute only once.
//  *  @param[in] action A pointer to the action; a function pointer.
//  *  @param[in] argument_size The size of the action argument in bytes.
//  *  @param[in] argument A pointer to the action argument.
//  *
//  * @returns Scheduler return code of SCHEDULER_SUCCESS, otherwise an error.
//  */
// scheduler_err_t scheduler_schedule_action(action_id_t* id, scheduler_submission_source_t source, real_time_t start_at, uint32_t interval, scheduler_action_t action, uint8_t argument_size,
//                                           void* argument) {
//     if (!is_real_time_valid(&start_at)) {
//         return INVALID_REALTIME;
//     }

//     action_info_header_t* action_info_header;
//     uint8_t* action_argument;
//     int32_t insertion_point;

//     real_time_t submission_time = {'\0'};
//     rtc_err_t err               = get_current_time(&submission_time);
//     if (err != RTC_NO_ERR) {
//         return TIME_FAILURE;
//     }

//     // TODO: [FUTURE] get the OBC state when the APIs are defined

//     if (argument_size > MAX_ARGUMENT_SIZE) {
//         return ARG_SIZE_EXCEEDS_MAX_ARG_SIZE;
//     }

//     xSemaphoreTake(actions_mutex, portMAX_DELAY);

//     insertion_point = find_insertion_point();
//     if (insertion_point == -1) {
//         xSemaphoreGive(actions_mutex);
//         return REACHED_MAX_NUM_ACTIVE_ACTIONS;
//     }

//     action_info_header                  = (action_info_header_t*)(actions + (insertion_point * ACTION_INFO_SIZE));
//     action_argument                     = &actions[(insertion_point * ACTION_INFO_SIZE) + sizeof(action_info_header_t)];
//     action_info_header->id              = idx;
//     action_info_header->state           = ACTIVE;
//     action_info_header->source          = source;
//     action_info_header->submission_time = submission_time;
//     action_info_header->start_at        = start_at;
//     action_info_header->interval        = interval;
//     action_info_header->action          = action;
//     action_info_header->argument_size   = argument_size;
//     memcpy(action_argument, argument, argument_size);

//     handle_state_change();

//     // Update the action ID output.
//     if (id != NULL) {
//         *id = action_info_header->id;
//     }

//     // Increment the action index
//     idx++;
//     xSemaphoreGive(actions_mutex);
//     return SCHEDULER_SUCCESS;
// }

// /**
//  * scheduler_remove_action
//  *
//  * Marks an action for deletion. After an action is marked for deletion it would not be
//  * executed again and the scheduler can reclaim its space.
//  *
//  *  id: The id that was used to create the action in `scheduler_schedule_action`
//  *
//  * returns: scheduler return code of SCHEDULER_SUCCESS when it SCHEDULER_SUCCESSfully initialized,
//  * otherwise an error.
//  *
//  */
// scheduler_err_t scheduler_remove_action(action_id_t id) {
//     action_info_header_t* action_info_header;
//     int32_t action_offset;

//     xSemaphoreTake(actions_mutex, portMAX_DELAY);

//     action_offset = find_action(id);

//     if (action_offset == -1) {
//         xSemaphoreGive(actions_mutex);
//         return UNDEFINED_ACTION_ID;
//     }

//     action_info_header        = (action_info_header_t*)(actions + (action_offset * ACTION_INFO_SIZE));
//     action_info_header->state = MARKED_FOR_DELETION;

//     handle_state_change();

//     xSemaphoreGive(actions_mutex);
//     return SCHEDULER_SUCCESS;
// }

// /**
//  * scheduler_get_period
//  *
//  * @brief returns the period of an action
//  *
//  *  @param[in] id The id that was used to create the action in `scheduler_schedule_action`
//  *  @param[out] period interval of an action. Zero if action is not periodic.
//  *
//  * @returns SCHEDULER_SUCCESS if action exists otherwise returns UNDEFINED_ACTION_ID
//  *
//  */
// scheduler_err_t scheduler_get_period(action_id_t id, uint32_t* period) {
//     action_info_header_t* action_info_header;
//     int32_t action_offset;

//     xSemaphoreTake(actions_mutex, portMAX_DELAY);

//     action_offset = find_action(id);

//     if (action_offset == -1) {
//         xSemaphoreGive(actions_mutex);
//         return UNDEFINED_ACTION_ID;
//     }

//     action_info_header = (action_info_header_t*)(actions + (action_offset * ACTION_INFO_SIZE));
//     *period            = action_info_header->interval;

//     xSemaphoreGive(actions_mutex);
//     return SCHEDULER_SUCCESS;
// }

// /**
//  * scheduler_set_period
//  *
//  * @brief returns the period of an action
//  *
//  *  @param[in] id The id that was used to create the action in `scheduler_schedule_action`
//  *  @param[in] period the new interval for a periodic action
//  *
//  * @returns
//  *      UNDEFINED_ACTION_ID when action doesn't exists
//  *      NO_LONGER_ACTIVE when action has reached its terminal state
//  *      NOT_PERIODIC when action was not created as a periodic action
//  */
// scheduler_err_t scheduler_set_period(action_id_t id, uint32_t period) {
//     action_info_header_t* action_info_header;
//     int32_t action_offset;

//     xSemaphoreTake(actions_mutex, portMAX_DELAY);

//     action_offset = find_action(id);

//     if (action_offset == -1) {
//         xSemaphoreGive(actions_mutex);
//         return UNDEFINED_ACTION_ID;
//     }

//     action_info_header = (action_info_header_t*)(actions + (action_offset * ACTION_INFO_SIZE));
//     if (action_info_header->state != ACTIVE) {
//         xSemaphoreGive(actions_mutex);
//         return NO_LONGER_ACTIVE;
//     }
//     if (action_info_header->interval == 0) {
//         xSemaphoreGive(actions_mutex);
//         return NOT_PERIODIC;
//     }

//     action_info_header->interval = period;

//     xSemaphoreGive(actions_mutex);
//     return SCHEDULER_SUCCESS;
// }

// /**
//  * @brief Prints out information about all actions in the schedule.
//  */
// void print_schedule(void) {
//     action_info_header_t* action_info_header;
//     uint8_t i = 0;
//     for (i = 0; i < MAX_NUM_ACTIONS; i++) {
//         action_info_header = (action_info_header_t*)(actions + (i * ACTION_INFO_SIZE));

//         // Determine the opcode string. It will be "NONE" if the action slot is empty.
//         const char* opcode_str = NULL;
//         obc_cmd_opcode_from_action(action_info_header->action, &opcode_str);

//         // First line - command info: number, ID, state, opcode
//         log_str(INFO, LOG_SCHEDULER, "%d %d %s %s", i, action_info_header->id, action_state_strings[action_info_header->state], opcode_str);

//         // Second line - submission info: number, ID, submission epoch, internal/external
//         log_str(INFO, LOG_SCHEDULER, "%d %d %d %s", i, action_info_header->id, action_info_header->submission_time.epoch, submission_source_strings[action_info_header->source]);

//         // Third line - schedule info: number, ID, scheduled epoch, interval
//         log_str(INFO, LOG_SCHEDULER, "%d %d %d %d", i, action_info_header->id, action_info_header->start_at.epoch, action_info_header->interval);
//     }
// }

// // ----------------------------------- TASKS -----------------------------------

// /**
//  * vSchedulerTask
//  *
//  * Executes every 1000ms. It iterates through the actions state variable and
//  * enqueues all the actions that should execute in that execution cycle.
//  *
//  */
// static void vSchedulerTask(void* pvParameters) {
//     uint32_t i;
//     real_time_t current_time;
//     const TickType_t xDelay1s = pdMS_TO_TICKS(1000);
//     TickType_t xLastWakeTime;
//     action_info_header_t* action_info_header;
//     uint8_t action_call[ACTION_CALL_SIZE] = {'\0'};
//     task_id_t wd_task_id                  = WD_TASK_ID(pvParameters);

//     xLastWakeTime = xTaskGetTickCount();

//     for (;;) {
//         set_task_status(wd_task_id, task_alive);

//         rtc_err_t errcode = rtc_get_current_time(&current_time);
//         if (errcode != RTC_NO_ERR) {
//             log_str_without_time(ERROR, LOG_SCHEDULER_FAILED_TO_READ_RTC_CURRENT_TIME, "rtc err: %d", errcode);
//         } else {
//             xSemaphoreTake(actions_mutex, portMAX_DELAY);

//             for (i = 0; i < MAX_NUM_ACTIONS; i++) {
//                 action_info_header = (action_info_header_t*)(actions + (i * ACTION_INFO_SIZE));
//                 if (should_execute_action(action_info_header, &current_time)) {
//                     create_action_call((uint8_t*)action_info_header, action_call);
//                     xQueueSend(actions_queue, action_call, portMAX_DELAY);
//                     if (action_info_header->interval == 0) {
//                         action_info_header->state = COMPLETED;
//                         handle_state_change();
//                     }
//                 }
//             }
//             xSemaphoreGive(actions_mutex);
//         }

//         set_task_status(wd_task_id, task_asleep);
//         vTaskDelayUntil(&xLastWakeTime, xDelay1s);
//     }
// }

// /**
//  * vExecuterTask
//  *
//  * consumes action_calls from the actions_queue and executes them.
//  *
//  */
// static void vExecuterTask(void* pvParameters) {
//     action_call_header_t call_header;
//     uint8_t action_call[ACTION_CALL_SIZE];
//     task_id_t wd_task_id = WD_TASK_ID(pvParameters);

//     for (;;) {
//         set_task_status(wd_task_id, task_asleep);
//         xQueueReceive(actions_queue, (void*)action_call, portMAX_DELAY);
//         set_task_status(wd_task_id, task_alive);
//         call_header = *((action_call_header_t*)action_call);
//         call_header.action(call_header.argument_size, (void*)(action_call + sizeof(action_call_header_t)));
//     }
// }

// /**
//  * vSyncTask
//  *
//  * serializes the actions state variable to the non-volatile memory every 10s.
//  *
//  */
// static void vSyncTask(void* pvParameters) {
//     const TickType_t xDelay10s = pdMS_TO_TICKS(10000);
//     TickType_t xLastWakeTime;
//     task_id_t wd_task_id = WD_TASK_ID(pvParameters);

//     xLastWakeTime = xTaskGetTickCount();

//     for (;;) {
//         set_task_status(wd_task_id, task_alive);
//         xSemaphoreTake(actions_mutex, portMAX_DELAY);

//         serialize();
//         xSemaphoreGive(actions_mutex);

//         set_task_status(wd_task_id, task_asleep);
//         vTaskDelayUntil(&xLastWakeTime, xDelay10s);
//     }
// }

// /**
//  * vSortTask
//  *
//  * orders the actions in the actions state varaible to optimize the run time of the scheduler ever
//  * 10s.
//  *
//  */
// static void vSortTask(void* pvParameters) {
//     const TickType_t xDelay10s = pdMS_TO_TICKS(10000);
//     TickType_t xLastWakeTime;
//     task_id_t wd_task_id = WD_TASK_ID(pvParameters);

//     xLastWakeTime = xTaskGetTickCount();

//     for (;;) {
//         set_task_status(wd_task_id, task_alive);
//         xSemaphoreTake(actions_mutex, portMAX_DELAY);

//         sort_actions();
//         xSemaphoreGive(actions_mutex);

//         set_task_status(wd_task_id, task_asleep);
//         vTaskDelayUntil(&xLastWakeTime, xDelay10s);
//     }
// }

// // ---------------------------------- HELPERS ----------------------------------
// // WARNING: all the helpers must be executed after aquiring the actions_mutex mutex

// /**
//  * create_action_call
//  *
//  * Creates a action call message to be pushed to actions_queue.
//  *
//  *  action_info: a pointer to the action header, followed by the data
//  *  action_call: a pointer to the action call buffer
//  *
//  */
// static void create_action_call(uint8_t* action_info, uint8_t* action_call) {
//     action_info_header_t* action_info_header = (action_info_header_t*)action_info;
//     action_call_header_t* action_call_header = (action_call_header_t*)action_call;
//     action_call_header->action               = action_info_header->action;
//     action_call_header->argument_size        = action_info_header->argument_size;

//     memcpy(action_call + sizeof(action_call_header_t), action_info + sizeof(action_info_header_t), action_call_header->argument_size);
// }

// /**
//  * @brief Decides whether an action should be executed based on action header info and current time.
//  *
//  * @pre The time in the action_info_header must have been validated prior to creating the action.
//  * This is automatically done in @ref parse, when the command is parsed.
//  *
//  * @param action_info_header: a pointer to the action header
//  * @param current_time: the current time
//  * @return True if the action should be executed in the current scheduler cycle.
//  */
// static bool should_execute_action(action_info_header_t* action_info_header, real_time_t* current_time) {
//     // Check if there is any action allocated to begin with
//     if (action_info_header->state == EMPTY) {
//         return false;
//     }

//     real_time_t* start_at                 = &(action_info_header->start_at);
//     int32_t del                           = delta(start_at, current_time);
//     uint32_t interval                     = action_info_header->interval;
//     bool is_periodic                      = interval != 0;
//     bool starts_before_curr_time          = del < 0;
//     bool starts_at_current_time           = del == 0;
//     bool starts_at_or_before_current_time = starts_before_curr_time || starts_at_current_time;
//     bool is_completed                     = action_info_header->state == COMPLETED;
//     bool is_period                        = (abs(del) % interval) == 0;

//     bool aperiodic_action_should_execute = (!is_periodic) && (starts_at_current_time || ((!is_completed) && starts_before_curr_time));
//     bool periodic_action_should_execute  = is_periodic && is_period && starts_at_or_before_current_time;

//     return (aperiodic_action_should_execute || periodic_action_should_execute);
// }

// /**
//  * find_action
//  *
//  * Iterates through the actions state variable and finds an action with the matching id.
//  *
//  * returns: The position of an action with the matching action id. -Returns -1 if the action id is
//  * not found.
//  *
//  */
// static int32_t find_action(action_id_t id) {
//     action_info_header_t* action_info_header;
//     int32_t i;
//     for (i = 0; i < MAX_NUM_ACTIONS; i++) {
//         action_info_header = (action_info_header_t*)(actions + (i * ACTION_INFO_SIZE));
//         if (action_info_header->id == id) {
//             return i;
//         }
//     }

//     return -1;
// }

// /**
//  * find_insertion_point
//  *
//  * Iterates through the actions state variable and finds a non ACTIVE action.
//  *
//  * returns: The position of the first non ACTIVE action. -1 if there is no space available.
//  *
//  */
// static int32_t find_insertion_point(void) {
//     action_info_header_t* action_info_header;
//     int32_t i;
//     for (i = 0; i < MAX_NUM_ACTIONS; i++) {
//         action_info_header = (action_info_header_t*)(actions + (i * ACTION_INFO_SIZE));
//         if (action_info_header->state != ACTIVE) {
//             return i;
//         }
//     }

//     return -1;
// }

// /**
//  * handle_state_change
//  *
//  * Executes any logic associated with state change such as run time optimization and serialization
//  *
//  */
// static void handle_state_change(void) {
//     state_changes += 1;

//     if ((state_changes % SYNC_ACTION_CHANGE_THRESHOLD) == 0) {
//         serialize();
//     }

//     if ((state_changes % SORT_ACTION_CHANGE_THRESHOLD) == 0) {
//         sort_actions();
//     }
// }

// /**
//  * serialize
//  *
//  * Serializes the actions state variable to the non-volitile memory
//  * Only after a SCHEDULER_SUCCESSful serialization update any references to the serialized actions
//  *
//  * returns: scheduler return code of SCHEDULER_SUCCESS when it SCHEDULER_SUCCESSfully initialized,
//  * otherwise an error.
//  *
//  */
// static scheduler_err_t serialize(void) {
//     // TODO: serialize the actions and update references to the serialized actions
//     return SCHEDULER_SUCCESS;
// }

// /**
//  * deserialize
//  *
//  * Deserializes the actions from the non-volitile memory and
//  * Update the actions state variable
//  *
//  * returns: scheduler return code of SCHEDULER_SUCCESS when it SCHEDULER_SUCCESSfully initialized,
//  * otherwise an error.
//  *
//  */
// static scheduler_err_t deserialize(void) {
//     // TODO: deserialize the actions and update the state
//     return SCHEDULER_SUCCESS;
// }

// /**
//  * sort_actions
//  *
//  * Orders the elements of actions state variable to optimize the running time of the scheduler
//  *
//  */
// static void sort_actions(void) {
//     // TODO: order the elements of the actions state variable to optimize the running time of the
//     // scheduler
// }
