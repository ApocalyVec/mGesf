import serial
import numpy as np
from struct import *


class UWBSensorInterface:

    def __init__(self, role, frame_size, baud_rate=9600, uport=None):

        self.role = role
        self.baud_rate = baud_rate
        self.frame_size = frame_size
        self.data_buffer = b''
        self.uport = None

    def connect_virtual_port(self, virtual_port):
        try:
            self.uport = serial.Serial(port=virtual_port, baudrate=self.baud_rate, timeout=0)

        except:
            print("port is in use or not open")
            return

    def generate_frame(self):
        self.data_buffer += self.uport.read()
        # print(len(self.data_buffer))
        if len(self.data_buffer) > self.frame_size:
            real_imag_pairs = np.reshape(unpack("i" * 130, self.data_buffer[0:self.frame_size]), (-1, 2))
            print(real_imag_pairs)
            self.data_buffer = self.data_buffer[self.frame_size:]
            return real_imag_pairs
        else:
            return None

    def disconnect_virtual_port(self):
        try:
            self.uport.close()
        except:
            print("already closed or cannot close")
            return
