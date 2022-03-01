# File will contain all the constants used

# Texts
serial_status_connected_text = "Status: Connected"
serial_status_disconnected_text = "Status: Disconnected"

# Colors
red = 'red'
blue = 'blue'
green = 'green'
black = 'black'
yellow = 'yellow'

def get_background_colour(color: str)->str:
    return "background-color: " + color

def get_colour(color: str)->str:
    return "color: " + color

# Program Constants
DEFAULT_MAX_LINES_HISTORY = 300
END_SERIAL_MESSAGE = "Serial Port Closed"
SLEEP_CONSTANT = 0.1
MAX_ALEA_MESSAGE_LEN = 25

# Settings constants
TIMESTAMPS_ON = "Timestamps: ON"
TIMESTAMPS_OFF = "Timestamps: OFF"
FUNC_ID_ON = "Function ID: ON"
FUNC_ID_OFF = "Function ID: OFF"
MSG_LVL_ON = "Message Level: ON"
MSG_LVL_OFF = "Message Level: OFF"

