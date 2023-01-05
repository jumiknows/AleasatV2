class OBCError(Exception):
    pass

class OBCCmdSysSpecError(OBCError):
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