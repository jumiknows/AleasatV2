from enum import Enum

class WriteSetting(Enum):
    SRAM = 0
    FLASH = 1
    TEMP = 2
    
    def get(index: int):
        return [WriteSetting.SRAM, WriteSetting.FLASH, WriteSetting.TEMP][index]

class RestartMode(Enum):
    HOT = 1
    WARM = 2
    COLD = 3

    def get(index: int):
        return [RestartMode.HOT, RestartMode.WARM, RestartMode.COLD][index]