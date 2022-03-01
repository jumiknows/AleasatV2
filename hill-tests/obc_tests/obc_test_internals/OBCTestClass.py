import unittest, glob, sys, serial
from obc_test_internals import obc_handler


class OBCTest(unittest.TestCase):
    obc = None
    PORT = "COM8"

    def setUp(self) -> None:
        self.obc = obc_handler.OBCHandler()
        if self.obc.connect(self.PORT):
            print("SUCCESS: Connected to " + self.PORT)
        else:
            print("ERROR: Could not connect to " + self.PORT)
            exit(1)

        self.obc.start()

    def tearDown(self) -> None:
        self.obc.send_q.put("0 nrst * *")
        self.obc.stop()


def get_serial_ports():
    """ Lists serial port names

        :raises EnvironmentError:
            On unsupported or unknown platforms
        :returns:
            A list of the serial ports available on the system
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

    result = []
    for port in ports:
        try:
            s = serial.Serial(port)
            s.close()
            result.append(port)
        except (OSError, serial.SerialException):
            pass
    return result
