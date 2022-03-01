import serial, sys, queue, time, threading
from PyQt5 import QtCore
import constants, alea_message


class SerialHandler(QtCore.QThread):
    signal = QtCore.pyqtSignal('PyQt_PyObject')
    status = constants.serial_status_disconnected_text
    status_color_map = {
        constants.serial_status_disconnected_text: constants.get_colour(constants.red),
        constants.serial_status_connected_text: constants.get_colour(constants.green),
    }
    port = None
    ser = None
    send_q = None
    message_assistant = alea_message.AleaMessageParser()

    def __init__(self, send_q: queue.Queue):
        QtCore.QThread.__init__(self)
        self.send_q = send_q

    def connected(self) -> bool:
        return self.port is not None

    # returns status string and color
    def get_status(self) -> (str, str):
        return self.status, self.status_color_map[self.status]

    def connect(self, serial_port) -> bool:
        print(serial_port)
        try:
            ser = serial.Serial(
                port=serial_port,
                baudrate=115200,
                timeout=5,
                parity=serial.PARITY_NONE,
                stopbits=serial.STOPBITS_ONE,
                bytesize=serial.EIGHTBITS
            )
            if not ser.is_open:
                ser.open()

            self.ser = ser
            self.status = constants.serial_status_connected_text
            self.port = serial_port

            return True

        except Exception as e:
            print(e)
            return False

    def run(self):
        while 1:
            if not self.send_q.empty():
                s = self.send_q.get()
                self.ser.write(bytes(s + '\r', 'ascii'))
                print(s)

            if not self.ser.is_open:
                self.signal.emit(constants.END_SERIAL_MESSAGE)
                return

            while self.ser.in_waiting > 0:
                out = self.ser.readline()  # todo parsing here
                if out:
                    print(out)
                    try:
                        msgs = self.message_assistant.parse(out)
                        print(msgs)
                        for msg in msgs:
                            self.signal.emit(msg.print())

                        #self.signal.emit(str(out[7:], 'ascii'))  # TODO make this into both listener and sender, # todo parse object
                    except Exception as e:
                        print("Failed to parse", e)

            time.sleep(constants.SLEEP_CONSTANT)