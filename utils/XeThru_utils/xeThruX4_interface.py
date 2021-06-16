import serial
import time
import numpy as np
from collections import deque

import pymoduleconnector
import matplotlib.pyplot as plt

from sys import platform

from utils.XeThru_utils.xeThruX4_algorithm import *

if platform == "win32":  # only enable module connector is on windows system, with which the radar is only compatible
    from pymoduleconnector.moduleconnectorwrapper import *

class xeThruX4SensorInterface:

    def __init__(self):

        # sys_info
        self.device_name = None
        self.min_range = None
        self.max_range = None
        self.center_frequency = None
        self.FPS = None
        self.baseband = False
        self.xep = None
        # sampling rate
        self.fs = 23.328e9
        # data buffer
        self.frame_history = deque(maxlen=200)
        self.baseband_history = deque(maxlen=200)
        self.clutter_removal_frame_history = deque(maxlen=200)
        self.clutter_removal_baseband_history = deque(maxlen=200)

        self.clutter = None

        self.connected = False

    def reset(self, device_name):
        try:
            mc = pymoduleconnector.ModuleConnector(device_name)
            xep = mc.get_xep()
            xep.module_reset()
            print("device find")
            mc.close()
            time.sleep(3)

            self.clutter = None
        except:
            print("Cannot find x4 device, please check connection or reconnect your device")
            raise

    def config_x4_sensor(self, device_name, min_range=-0.1, max_range=0.4, center_frequency=3, FPS=10, baseband=False):

        self.reset_buffer()

        self.mc = pymoduleconnector.ModuleConnector(device_name)

        app = self.mc.get_x4m300()
        # Stop running application and set module in manual mode.
        try:
            app.set_sensor_mode(XTID_SM_STOP, 0)  # Make sure no profile is running.
        except RuntimeError:
            # Profile not running, OK
            pass

        try:
            app.set_sensor_mode(XTID_SM_MANUAL, 0)  # Manual mode.
        except RuntimeError:
            # Maybe running XEP firmware only?
            pass

        try:
            self.xep = self.mc.get_xep()
            # set center frequency
            self.xep.x4driver_set_tx_center_frequency(center_frequency)

            # print(xep.x4driver_get_tx_center_frequency())

            self.xep.x4driver_set_dac_min(850)
            self.xep.x4driver_set_dac_max(1200)
            # Set integration
            self.xep.x4driver_set_iterations(64)
            self.xep.x4driver_set_pulses_per_step(48)
            # baseband?
            self.xep.x4driver_set_downconversion(int(baseband))
            # Start streaming of data
            self.xep.x4driver_set_frame_area(min_range, max_range)
            self.xep.x4driver_set_fps(FPS)

            self.connected = True

            self.display_xep_sys_info()
        except:
            print("error while config")
            return

        self.device_name = device_name
        self.FPS = FPS
        self.center_frequency = center_frequency
        self.min_range = min_range
        self.max_range = max_range
        # boolean
        self.baseband = baseband
        # self.display_xep_sys_info()

    def stop_sensor(self):
        if self.xep is not None:
            try:
                self.xep.module_reset()
                self.connected = False
            except:
                print("please check the connection")
                self.connected = False
        else:
            print("no connection history, please check usb")
            self.connected = False

    def display_xep_sys_info(self):
        if self.xep is not None:
            print("FirmWareID =", self.xep.get_system_info(2))
            print("Version =", self.xep.get_system_info(3))
            print("Build =", self.xep.get_system_info(4))
            print("SerialNumber =", self.xep.get_system_info(6))
            print("VersionList =", self.xep.get_system_info(7))
            frame_area = self.xep.x4driver_get_frame_area()
            print('x4driver_get_frame_area returned: [', frame_area.start, ', ', frame_area.end, ']')
            start = frame_area.start
            end = frame_area.end
            range_resolution = (end-start)/155
            print(range_resolution)

    def clear_xep_buffer(self):
        if self.xep is not None:
            while self.xep.peek_message_data_float():
                self.xep.read_message_data_float()

        else:
            print("device not found")

    def read_frame(self):
        if self.xep.peek_message_data_float():
            d = self.xep.read_message_data_float()

            #read rf; baseband; clutter free rf; clutter free baseband
            frame = np.array(d.data)
            baseband_frame = xep_rf_frame_downconversion(frame, self.center_frequency)
            clutter_removal_frame = self.read_clutter_removal_frame(frame, 0.05)
            clutter_removal_baseband_frame = xep_rf_frame_downconversion(clutter_removal_frame, self.center_frequency)

            # if self.baseband:
            #     n = len(frame)
            #     frame = frame[:n // 2] + 1j * frame[n // 2:]

            self.frame_history.append(frame)
            self.baseband_history.append(baseband_frame)
            self.clutter_removal_frame_history.append(clutter_removal_frame)
            self.clutter_removal_baseband_history.append(clutter_removal_baseband_frame)

            return frame, baseband_frame, clutter_removal_frame, clutter_removal_baseband_frame
        else:
            return None, None, None, None

    def read_clutter_removal_frame(self, rf_frame, signal_clutter_ratio):
        if self.clutter is None:
            self.clutter = rf_frame
            return rf_frame - self.clutter
        else:
            self.clutter = signal_clutter_ratio * self.clutter + (1 - signal_clutter_ratio) * rf_frame
            clutter_removal_rf_frame = rf_frame - self.clutter
            # self.clutter_removal_frame_history.append(clutter_removal_rf_frame)
            return clutter_removal_rf_frame

    def reset_buffer(self):
        self.frame_history.clear()
        self.baseband_history.clear()
        self.clutter_removal_frame_history.clear()
        self.clutter_removal_baseband_history.clear()
        self.clutter = None