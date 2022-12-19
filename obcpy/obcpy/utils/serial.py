"""Utilities related to serial ports.
"""

from typing import List
import sys
import glob

import serial

def get_serial_ports() -> List[str]:
    """Collects a list of available serial ports.

    Each serial port listed by the OS is tested by opening and closing
    a serial.Serial instance on it and checking for errors.

    Raises:
        EnvironmentError: The current OS platform is not supported

    Returns:
        A list of available serial port device names.
    """
    if sys.platform.startswith('win'):
        ports = ['COM%s' % (i + 1) for i in range(256)]
    elif sys.platform.startswith('linux') or sys.platform.startswith('cygwin'):
        # this excludes your current terminal "/dev/tty"
        ports = glob.glob('/dev/tty[A-Za-z]*')
    elif sys.platform.startswith('darwin'):
        ports = glob.glob('/dev/tty.*')
    else:
        raise EnvironmentError('Unsupported platform')

    result: List[str] = []
    for port in ports:
        try:
            s = serial.Serial(port)
            s.close()
            result.append(port)
        except (OSError, serial.SerialException):
            pass
    return result
