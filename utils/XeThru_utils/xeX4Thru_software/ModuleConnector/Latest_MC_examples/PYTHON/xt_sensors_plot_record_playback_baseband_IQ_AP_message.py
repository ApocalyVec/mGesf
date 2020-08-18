#!/usr/bin/env python
""" \example xt_sensors_plot_record_baseband_IQ_AP_message.py

Latest examples is located at https://github.com/xethru/XeThru_ModuleConnector_Examples or https://dev.azure.com/xethru/XeThruApps/_git/XeThru_ModuleConnector_Examples.

# Target module:
# X4M200
# X4M300

# Introduction:
# XeThru sensors support baseband IQ or Amplitude-Phase data output at application level. Developer can use Module Connecter API to read, record baseband IQ or AP data.

# prerequisite:
# ModuleConnector python lib is installed, check XeThruSensorsIntroduction application note to get detail
# xt_modules_print_info.py should be in the same folder
# xt_modules_record_playback_messages.py should be in the same folder

# Command to run:
# "python xt_sensors_plot_record_baseband_IQ_AP_message.py -d com8", change "com8" with your device name, using "--help" to see other args. Using TCP server address as device name is also supported, e.g. "python xt_sensors_plot_record_baseband_IQ_AP_message.py -d tcp://192.168.1.169:3000".
"""
from __future__ import print_function, division

import sys
from argparse import ArgumentParser
from time import sleep

import numpy as np
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation

import pymoduleconnector
from pymoduleconnector import ModuleConnector
from pymoduleconnector import DataType
from pymoduleconnector.extras.auto import auto
from pymoduleconnector.ids import *

from xt_modules_print_info import print_module_info
from xt_modules_print_info import print_x4_settings
from xt_modules_record_playback_messages import start_recorder
from xt_modules_record_playback_messages import start_player
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
                       'led_control': (XTID_LED_MODE_OFF, 0),
                       # initialize noisemap everytime when get start (approximately 120s)
                       'noisemap_control': 0b111,
                       # only uncomment the message when you need them to avoide confliction.
                       #    'output_control1': (XTS_ID_BASEBAND_IQ, 0),
                       #    'output_control2': (XTS_ID_BASEBAND_AMPLITUDE_PHASE, 0),
                       #    'output_control3': (XTS_ID_PULSEDOPPLER_FLOAT, 0),
                       #    'output_control4': (XTS_ID_PULSEDOPPLER_BYTE, 0),
                       #    'output_control5': (XTS_ID_NOISEMAP_FLOAT, 0),
                       #    'output_control6': (XTS_ID_NOISEMAP_BYTE, 0),
                       #     'output_control7': (XTS_ID_SLEEP_STATUS, 0),
                       #    'output_control8': (XTS_ID_RESP_STATUS, 0),
                       #    'output_control9': (XTS_ID_VITAL_SIGNS, 0),
                       #    'output_control10': (XTS_ID_RESPIRATION_MOVINGLIST, 0),
                       #    'output_control11': (XTS_ID_RESPIRATION_DETECTIONLIST, 0)
                       }

x4m300_par_settings = {'detection_zone': (0.4, 2),
                       'sensitivity': 5,
                       'led_control': (XTID_LED_MODE_OFF, 0),
                       # initialize noisemap everytime when get start (approximately 120s)
                       'noisemap_control': 0b111,
                       #    'output_control1': (XTS_ID_BASEBAND_IQ, 0),
                       #    'output_control2': (XTS_ID_BASEBAND_AMPLITUDE_PHASE, 0),
                       #    'output_control3': (XTS_ID_PULSEDOPPLER_FLOAT, 0),
                       #    'output_control4': (XTS_ID_PULSEDOPPLER_BYTE, 0),
                       #    'output_control5': (XTS_ID_NOISEMAP_FLOAT, 0),
                       #    'output_control6': (XTS_ID_NOISEMAP_BYTE, 0),
                       #    'output_control7': (XTS_ID_PRESENCE_SINGLE, 0),
                       #    'output_control8': (XTS_ID_PRESENCE_MOVINGLIST, 0)
                       }


def configure_sensor_bb_output(device_name, record=False, data_type="iq"):

    if data_type == "iq":
        x4m200_par_settings['output_control'] = (XTS_ID_BASEBAND_IQ, 1)
        x4m300_par_settings['output_control'] = (XTS_ID_BASEBAND_IQ, 1)
    elif data_type == "ap":
        x4m200_par_settings['output_control'] = (
            XTS_ID_BASEBAND_AMPLITUDE_PHASE, 1)
        x4m300_par_settings['output_control'] = (
            XTS_ID_BASEBAND_AMPLITUDE_PHASE, 1)
    else:
        print('data_taype is set to ' + data_type +
              ". It is not supported, please check arguments!")
        raise

    order_code = print_module_info(device_name)
    if order_code == "X4M200":
        xt_sensor = configure_x4m200(device_name, record, x4m200_par_settings)
    elif order_code == "X4M300":
        xt_sensor = configure_x4m300(device_name, record, x4m300_par_settings)
    else:
        print("This module is " + order_code +
              ". It does not support sensor baseband data output!!!")
        raise
    return xt_sensor


def plot_sensor_bb_message(xt_sensor, data_type="iq"):
    print("Start plot radar baseband data!")

    def read_bb_iq_data():
        """Gets frame data from module"""
        iq_data = xt_sensor.read_message_baseband_iq()
        i_data = np.array(iq_data.i_data)
        q_data = np.array(iq_data.q_data)
        # n = len(i_data)
        # iq_frame = i_data[:n] + 1j*q_data[:n]
        return i_data, q_data

    def read_bb_ap_data():
        ap_data = xt_sensor.read_message_baseband_ap()
        amp_data = np.array(ap_data.amplitude)
        pha_data = np.array(ap_data.phase)
        return amp_data, pha_data

    def animate_bb_iq(i):
        # Update plot data
        i_data, q_data = read_bb_iq_data()
        line1.set_ydata(i_data)
        line2.set_ydata(q_data)
        return line1, line2

    def animate_bb_ap(i):
        # Update plot data
        amp_data, pha_data = read_bb_ap_data()
        line1.set_ydata(amp_data)
        line2.set_ydata(pha_data)
        return line1, line2

    fig = plt.figure()
    fig.suptitle("Radar Raw Data")
    ax1 = fig.add_subplot(211)
    ax2 = fig.add_subplot(212)
    if data_type == "iq":
        ax1.set_ylabel('I Data')
        ax2.set_ylabel('Q Data')
        # keep graph in frame (FIT TO YOUR DATA), can be adjusted
        ax1.set_ylim(-0.05, 0.05)
        ax2.set_ylim(-0.05, 0.05)
        i_data, q_data = read_bb_iq_data()
        line1, = ax1.plot(i_data)
        line2, = ax2.plot(q_data)
        ani = FuncAnimation(fig, animate_bb_iq)
    else:
        ax1.set_ylabel('Amplitude')
        ax2.set_ylabel('Phase')
        ax1.set_ylim(0, 0.001)
        ax2.set_ylim(-3.14, 3.14)
        amp_data, pha_data = read_bb_ap_data()
        line1, = ax1.plot(amp_data)
        line2, = ax2.plot(pha_data)
        ani = FuncAnimation(fig, animate_bb_ap, interval=1)
    plt.show()
    sys.exit(0)


def main():
    parser = ArgumentParser()
    parser.add_argument(
        "-d",
        "--device",
        dest="device_name",
        help="device file to use",
        metavar="FILE")
    parser.add_argument(
        "-t",
        "--data_type",
        dest="data_type",
        default="iq",
        help="Data to get. iq or ap",  # only one option can be choose
        metavar="TYPE")
    parser.add_argument(
        "-r",
        "--record",
        action="store_true",
        default=False,
        dest="record",
        help="Enable recording")
    parser.add_argument(
        "-f",
        "--file",
        dest="meta_filename",
        metavar="FILE",
        help="meta file from recording")

    args = parser.parse_args()

    if not args.meta_filename:
        if args.device_name:
            device_name = args.device_name
        else:
            try:
                device_name = auto()[0]
            except:
                print("Fail to find serial port, please specify it by use -d!")
                raise

        xt_sensor = configure_sensor_bb_output(
            device_name, args.record, args.data_type)

    else:
        player = start_player(meta_filename=args.meta_filename)
        mc = ModuleConnector(player, log_level=0)
        xt_sensor = mc.get_x4m200()
    plot_sensor_bb_message(xt_sensor, args.data_type)


if __name__ == "__main__":
    main()
