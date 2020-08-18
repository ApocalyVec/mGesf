#!/usr/bin/env python
# -*- coding: utf-8 -*-
""" \example xt_sensors_plot_record_movinglist_message.py

Latest examples is located at https://github.com/xethru/XeThru_ModuleConnector_Examples or https://dev.azure.com/xethru/XeThruApps/_git/XeThru_ModuleConnector_Examples.

# Target module:
# X4M200
# X4M300

# Introduction: This is an example of how to get and plot movement_list messages from X4M200, X4M300.

# prerequisite:
# ModuleConnector python lib is installed, check XeThruSensorsIntroduction application note to get detail
# xt_modules_print_info.py should be in the same folder
# xt_modules_record_playback_messages.py should be in the same folder

# Command to run: "python xt_sensors_plot_record_movinglist_message.py -d com8" or "python3 xt_sensors_plot_record_movinglist_message.py -d com8"
                 change "com8" with your device name, using "--help" to see other args.
                 Using TCP server address as device name is also supported, e.g.
                 "python xt_sensors_plot_record_movinglist_message.py -d tcp://192.168.1.169:3000".
"""
from __future__ import print_function
import sys
from argparse import ArgumentParser

from time import sleep
import collections

import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animation
from matplotlib.animation import FuncAnimation

import pymoduleconnector
from pymoduleconnector import ModuleConnector
from pymoduleconnector import DataType
from pymoduleconnector import DataRecorder
from pymoduleconnector import RecordingOptions
from pymoduleconnector.extras.auto import auto
from pymoduleconnector.ids import *

from xt_modules_print_info import print_module_info
from xt_modules_print_info import print_x4_settings
from x4m200_print_record_playback_RESP_SLEEP_message import configure_x4m200
from x4m300_print_record_playback_PRESENCE_message import configure_x4m300

# User settings introduction
"""
output control:
Only one of XTS_ID_BASEBAND_IQ and XTS_ID_BASEBAND_AMPLITUDE_PHASE can be enabled at a time. Enabling one disables the other. Disabling one, even if it is already disabled, disables the other.
Only one of XTS_ID_PULSEDOPPLER_FLOAT and XTS_ID_PULSEDOPPLER_BYTE can be enabled for each of XTID_OUTPUT_CONTROL_PD_SLOW_ENABLE and XTID_OUTPUT_CONTROL_PD_FAST_ENABLE. Same with XTS_ID_NOISEMAP_FLOAT and XTS_ID_NOISEMAP_BYTE. Turning on a float output automatically disables the byte output, and vice versa.
"""
x4m200_par_settings = {'detection_zone': (0.4, 2),
                       'sensitivity': 5,
                       'tx_center_frequency': 3,
                       'led_control': (XTID_LED_MODE_FULL, 0),
                       # initialize noisemap everytime when get start (approximately 20s)
                       'noisemap_control': 0b110,
                       # only uncomment the message when you need them to avoide confliction.
                       # disable all message at first
                       #    'output_control1': (XTS_ID_BASEBAND_IQ, 0),
                       #    'output_control2': (XTS_ID_BASEBAND_AMPLITUDE_PHASE, 0),
                       #    'output_control3': (XTS_ID_PULSEDOPPLER_FLOAT, 0),
                       #    'output_control4': (XTS_ID_PULSEDOPPLER_BYTE, 0),
                       #    'output_control5': (XTS_ID_NOISEMAP_FLOAT, 0),
                       #    'output_control6': (XTS_ID_NOISEMAP_BYTE, 0),
                       'output_control7': (XTS_ID_SLEEP_STATUS, 1),
                       #    'output_control8': (XTS_ID_RESP_STATUS, 0),
                       #    'output_control9': (XTS_ID_VITAL_SIGNS, 0),
                       'output_control10': (XTS_ID_RESPIRATION_MOVINGLIST, 1),
                       #    'output_control11': (XTS_ID_RESPIRATION_DETECTIONLIST, 0)
                       }
x4m300_par_settings = {'detection_zone': (0.4, 2),
                       'sensitivity': 5,
                       'led_control': (XTID_LED_MODE_FULL, 0),
                       # initialize noisemap everytime when get start (approximately 20s)
                       'noisemap_control': 0b110,
                       # disable all message at first
                       #    'output_control1': (XTS_ID_BASEBAND_IQ, 0),
                       #    'output_control2': (XTS_ID_BASEBAND_AMPLITUDE_PHASE, 0),
                       #    'output_control3': (XTS_ID_PULSEDOPPLER_FLOAT, 0),
                       #    'output_control4': (XTS_ID_PULSEDOPPLER_BYTE, 0),
                       #    'output_control5': (XTS_ID_NOISEMAP_FLOAT, 0),
                       #    'output_control6': (XTS_ID_NOISEMAP_BYTE, 0),
                       'output_control7': (XTS_ID_PRESENCE_SINGLE, 1),
                       'output_control8': (XTS_ID_PRESENCE_MOVINGLIST, 1)
                       }
respiration_sensor_state_text = (
    "BREATHING", "MOVEMENT", "MOVEMENT TRACKING", "NO MOVEMENT", "INITIALIZING")
presence_sensor_state_text = (
    "NO_PRESENCE", "PRESENCE", "INITIALIZING")


def plot_record_movinglist_messages(device_name, record=False):
    history = 100
    empty_list = [None]*history
    distance_list = collections.deque(empty_list, maxlen=history)

    order_code = print_module_info(device_name)

    if order_code == "X4M200":
        xt_sensor = configure_x4m200(device_name, record, x4m200_par_settings)
    elif order_code == "X4M300":
        xt_sensor = configure_x4m300(device_name, record, x4m300_par_settings)
    else:
        print('This module does not support movinglist output!!!')
    print("Initializing, it will take aroud 120s or 20s, depending on noise map option.")

    count = 0
    if order_code == "X4M200":
        while xt_sensor.read_message_respiration_sleep().sensor_state == 4:
            count = count+1
            print(str(count))
    elif order_code == "X4M300":
        while xt_sensor.read_message_presence_single().presence_state == 2:
            count = count + 1
            print(str(count))
    print("Initializing Done!")

    print("Start plot movinglist...")

    def read_movelist():
        if order_code == "X4M200":
            d2 = xt_sensor.read_message_respiration_sleep()
            # sensor state
            state_txt.set_text(
                'State: ' + respiration_sensor_state_text[d2.sensor_state])
            # Object Distance
            distance_list.append(d2.distance)
            distance_txt.set_text('Distance: ' + str(d2.distance))
            d = xt_sensor.read_message_respiration_movinglist()
        elif order_code == "X4M300":
            d2 = xt_sensor.read_message_presence_single()
            # Sensor state
            state_txt.set_text(
                'State: ' + presence_sensor_state_text[d2.presence_state])
            # Object Distance
            distance_list.append(d2.distance)
            distance_txt.set_text('Distance: ' + str(d2.distance))
            d = xt_sensor.read_message_presence_movinglist()
        # Movement fast and slow
        mfast = np.array(d.movement_fast_items)
        mslow = np.array(d.movement_slow_items)
        return mfast, mslow, distance_list

    def animate(i):
        # Update plot data
        mfast, mslow, distance_list = read_movelist()
        line1.set_ydata(mfast)
        line2.set_ydata(mslow)
        line3.set_ydata(distance_list)
        return line1, line2, line3

    fig = plt.figure()
    state_txt = fig.text(0.75, 0.9, 'State: ')
    distance_txt = fig.text(0.5, 0.9, 'Distance: ')
    fig.suptitle("MovingList Example")
    ax1 = fig.add_subplot(3, 1, 1)
    ax2 = fig.add_subplot(3, 1, 2)
    ax3 = fig.add_subplot(3, 1, 3)
    ax1.set_ylabel('Fast Movement Metric')
    ax2.set_ylabel('Slow Movement Metric')
    ax3.set_ylabel('Distance')

    mfast, mslow, dist = read_movelist()

    # setting upperlimits for y-axsis
    ax1.set_ylim(0, 100)
    ax2.set_ylim(0, 100)
    ax3.set_xlim(0, history)
    ax3.set_ylim(0, 2)
    line1, = ax1.plot(mfast)
    line2, = ax2.plot(mslow)
    line3, = ax3.plot(dist)
    ani = FuncAnimation(fig, animate, interval=1)
    try:
        plt.show()
    finally:
        xt_sensor.set_sensor_mode(XTID_SM_STOP, 0)


def main():
    parser = ArgumentParser()
    parser.add_argument(
        "-d",
        "--device",
        dest="device_name",
        help="device file to use",
        metavar="FILE")
    parser.add_argument(
        "-r",
        "--record",
        action="store_true",
        default=False,
        dest="record",
        help="Enable recording")

    args = parser.parse_args()
    if args.device_name:
        device_name = args.device_name
    else:
        try:
            device_name = auto()[0]
        except:
            print("Fail to find serial port, please specify it by use -d!")
            raise
    plot_record_movinglist_messages(device_name, args.record)


if __name__ == "__main__":
    main()
