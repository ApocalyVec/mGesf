import random
from ctypes import *
from collections import namedtuple
import os
import numpy as np
import socket

class LeapInterface:
    listensocket = socket.socket() # Creates an instance of socket
    Port = 8000
    maxConnections = 999
    IP = socket.gethostname()
    clientsocket = socket
    address = socket
    running = False

    def __init__(self):
        pass

    def connect_sensor(self):
        # connect to the sensor
        self._set_up_local_network_port()

    def start_sensor(self):
        # tell the sensor to start sending frames
        self._send_start_command()

    def process_frame(self):
        # return a frame of the sensor
        # this function should return NONE WITHOUT blocking if a frame is not complete
        # return random.random()
        frame = self._get_frame_from_network_port()
        return frame

    def sensor_stop(self):
        self._send_stop_command()

    def _set_up_local_network_port(self):
        self.listensocket.bind(('',self.Port))
        self.listensocket.listen(self.maxConnections)
        print("Server started at " + self.IP + " on port " + str(self.Port))
        (self.clientsocket, self.address) = self.listensocket.accept()
        print("New connnection made")

    def _send_start_command(self):
        self.running = True

    def _get_frame_from_network_port(self):
        if self.running:
            return self.clientsocket.recv(1024).decode() #Gets the incoming message
        else:
            return ""

    def _send_stop_command(self):
        self.running = False


# lib_path, lib_file = 'x64', 'LeapC.dll'
#
# # lib_path is actually relative to this here file (leap.py)
# lib_path = os.path.join(os.path.dirname(__file__), lib_path)
#
# # this is done for Windows and Linux so that we can find and load the
# # library: cd to the directory with the library so that sibling dlls can be loaded from the cwd.
# prev_dir = os.getcwd()
# os.chdir(lib_path)
# #_leap = CDLL(lib_file)
# os.chdir(prev_dir)
#
#
# # structures from the LeapC code. These must match exactly with the structures in LeapC.cpp
# class cLeapInfo(Structure):
#     _fields_ = [
#         ('service', c_bool),
#         ('connected', c_bool),
#         ('has_focus', c_bool),
#     ]
#
#
# class cLeapFinger(Structure):
#     _fields_ = [
#         ('pos', c_float * 3),
#         ('finger_type', c_char_p),
#     ]
#
#
# class cLeapHand(Structure):
#     _fields_ = [
#         ('hand_type', c_char_p),
#         ('palm_pos', c_float * 3),
#         ('fingers', cLeapFinger * 5),
#     ]
#
#
# class cLeapFrame(Structure):
#     _fields_ = [
#         ('hands', cLeapHand * 2),
#     ]
#
# # set up function args
# #_leap.getInfo.argtypes = [POINTER(cLeapInfo)]
# #_leap.getFrame.argtypes = [POINTER(cLeapFrame)]
#
#
# def toArray(p):
#     return np.array((p[0], p[1], p[2]))
#
# def anotherToArray(type, p):
#     return np.array((type, 'x:', round(p[0], 3), ' y:', round(p[1], 3), ' z:', round(p[2], 3)))
#
#
# # public interface:
# LeapInfo = namedtuple('LeapInfo', 'service connected has_focus')
#
# class LeapHand(namedtuple('LeapHand', 'hand_type palm_pos fingers')):
#     def __str__(self):
#         'add a custom printing for a hand so it looks nicer'
#         fingers = '\n    ' + '\n    '.join([str(f) for f in self.fingers])
#         return 'LeapHand(hand_type={}, palm_pos={}, fingers=[{}])'.format(self.hand_type, self.palm_pos,
#                                                                                 fingers)
#
# class LeapFrame(namedtuple('LeapFrame', 'hands')):
#     """
#     Custom data structure for positional data from the Leap sensor.
#     """
#
#     def __str__(self):
#         'add a custom printing for a frame so it looks nicer'
#         hands = '\n  ' + '\n  '.join([str(h) for h in self.hands])
#         return 'LeapFrame(hands=[{}])'.format(hands)
#
# # public functions
# def getLeapInfo():
#     """
#     Returns a structure with the following fields.
#
#     Fields
#         | **service** -  True if drivers are installed.
#         | **connected** - True if the Leap is connected to the computer.
#         | **has_focus** - True if the app is currently in the foreground.
#     """
#     info = cLeapInfo()
#     _leap.getInfo(info)
#
#     return LeapInfo(service=info.service, connected=info.connected, has_focus=info.has_focus)
#
#
# def getLeapFrame():
#     """
#     Returns a LeapFrame containing 3D position data from the Leap sensor for all hands.
#
#     Fields
#         | **hands** - An array of LeapHands.
#
#     Each LeapHand contains the following fields -
#
#     Fields
#         | **finger_type** - The type of finger that is being represented
#         | **palm_pos** - A numpy array ``[x, y, z]`` of the palm position in millimeters.
#         | **fingers** - A 5-element array of finger positions as numpy arrays ``[x, y, z]`` in millimeters.
#     """
#
#     frame = cLeapFrame()
#     _leap.getFrame(frame)
#
#     hands = [LeapHand(hand_type=h.hand_type,
#                       palm_pos=toArray(h.palm_pos),
#                       fingers=[anotherToArray(f.finger_type, f.pos) for f in h.fingers])
#              for h in frame.hands]
#
#     return LeapFrame(hands=hands)
#
#
# #_leap.init()
#
# def run_test():
#     import time
#
#     while 1:
#         # info = getLeapInfo()
#         frame = getLeapFrame()
#
#         # print(info)
#         print(frame)
#
#         time.sleep(1)
#
#
# # normally, this file is used as a library that you import
# # however, you can also just test by running "python leap.py" to see if things are working
# if __name__ == "__main__":
#     run_test()
import time

leap_interface = LeapInterface()
leap_interface.connect_sensor()
leap_interface.start_sensor()


def run_test():
    while 1:
        frame = leap_interface.process_frame()
        print(frame)
        time.sleep(1e-3)


if __name__ == "__main__":
    run_test()
