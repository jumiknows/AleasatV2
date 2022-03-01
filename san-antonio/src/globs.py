import constants

# CHANGE THIS MANUALLY IF YOU WANT OPTIONS ON OR OFF
func_id_status = constants.FUNC_ID_OFF
msg_lvl_status = constants.MSG_LVL_ON
timestamp_status = constants.TIMESTAMPS_ON


def toggle_msg_lvl_status():
    global msg_lvl_status
    if msg_lvl_status == constants.MSG_LVL_ON:
        msg_lvl_status = constants.MSG_LVL_OFF
        return False
    else:
        msg_lvl_status = constants.MSG_LVL_ON
        return True


def toggle_func_id_status():
    global func_id_status
    if func_id_status == constants.FUNC_ID_ON:
        func_id_status = constants.FUNC_ID_OFF
        return False
    else:
        func_id_status = constants.FUNC_ID_ON
        return True


def toggle_timestamp_status():
    global timestamp_status
    if timestamp_status == constants.TIMESTAMPS_ON:
        timestamp_status = constants.TIMESTAMPS_OFF
        return False
    else:
        timestamp_status = constants.TIMESTAMPS_ON
        return True
