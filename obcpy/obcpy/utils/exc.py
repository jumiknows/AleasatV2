class OBCError(Exception):
    pass

class OBCTaskSpecError(OBCError):
    pass

class OBCLogSysSpecError(OBCError):
    pass

class OBCCmdSysSpecError(OBCError):
    pass

class OBCTaskNotFoundError(OBCError):
    pass

class OBCLogNotFoundError(OBCError):
    pass

class OBCCmdNotFoundError(OBCError):
    pass

class OBCCmdSysResponseError(OBCError):
    pass

class OBCEncodeError(OBCError):
    pass

class OBCDecodeError(OBCError):
    pass

class OBCDataFieldError(OBCError):
    pass

class OBCTaskTraceError(OBCError):
    pass
