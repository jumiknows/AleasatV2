/**
 * @file backup_epoch.c
 * @brief The backup epoch powered by a timer inside the TMS570.
 * @author Victor L
 * @date Feb 10, 2020
 */

#include "backup_epoch.h"
#include "obc_hardwaredefs.h"
#include "rti.h"

/**
 * @brief The offset from the counter value to use when calculating the epoch.
 *
 * To adjust the backup epoch, we apply an offset to the number of counts in the
 * RTI counter register. This has two benefits:
 * 	- Allows the time to be adjusted, since we can't load the counter register without being in
 * privileged mode.
 * 	- By reading just the count register, allows us to determine the number of counts (seconds)
 * since boot, not just the number of seconds since we made an adjustment.
 *
 * This value starts at 0, but is adjusted with the set_backup_epoch() function.
 */
int32_t offset;

/**
 * @brief Starts the RTI counter using the configured RTI block which creates a second counter.
 *
 * @pre @ref rtiInit has been called in @ref sys_main.c
 */
void backup_epoch_init(void) {
    offset = 0;
    rtiStartCounter(BACKUP_EPOCH_RTI_COUNTER_BLOCK);
}

/**
 * @brief Gets the backup epoch.
 *
 * The RTI_CNT_FRCx register is used for the backup epoch. It is a free running counter
 * that will increment its count once per second. It is clocked by the RTI. This counter
 * value is adjusted by the offset when the user requests a time change.
 *
 * @pre @ref backup_epoch_init has to be called before this function can be used.
 *
 * @return The backup epoch.
 */
epoch_t get_backup_epoch(void) {
    return BACKUP_EPOCH_RTI_REG->CNT[BACKUP_EPOCH_RTI_COMPARE_REG].FRCx + offset;
}

/**
 * @brief Updates the backup epoch by changing the offset.
 * @pre the desired_time is known to be valid. Use @ref is_real_time_valid() to check it.
 * @param[in] desired_time:  time to set to. The real_time component will be used, not the epoch.
 */
void set_backup_epoch(real_time_t* desired_time) {
    set_real_time_epoch(desired_time);
    offset = desired_time->epoch - BACKUP_EPOCH_RTI_REG->CNT[BACKUP_EPOCH_RTI_COMPARE_REG].FRCx;
}
