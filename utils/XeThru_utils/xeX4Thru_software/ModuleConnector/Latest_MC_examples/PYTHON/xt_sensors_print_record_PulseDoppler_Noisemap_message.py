#!/usr/bin/env python
# -*- coding: utf-8 -*-
""" \example xt_sensors_print_record_PulseDoppler_Noisemap_message.py

Latest examples is located at https://github.com/xethru/XeThru_ModuleConnector_Examples or https://dev.azure.com/xethru/XeThruApps/_git/XeThru_ModuleConnector_Examples.

# Target module:
# X4M200
# X4M300

# Introduction:  This is an example of how to set up a module for streaming pulse-Doppler telegrams, and how to assemble them to whole range-Doppler matrices.

# prerequisite:
# ModuleConnector python lib is installed, check XeThruSensorsIntroduction application note to get detail
# xt_modules_print_info.py should be in the same folder
# xt_modules_record_playback_messages.py should be in the same folder

# Command to run:  "python xt_sensors_print_record_PulseDoppler_Noisemap_message.py -d com8" or "python3 xt_sensors_print_record_PulseDoppler_Noisemap_message.py -d com8"
                  change "com8" with your device name, using "--help" to see other args.
                  
"""

from __future__ import print_function
from argparse import ArgumentParser
import sys
import time

import numpy as np

import pymoduleconnector
from pymoduleconnector import ModuleConnector
from pymoduleconnector import DataType
from pymoduleconnector.extras.auto import auto
from pymoduleconnector import DataRecorder
from pymoduleconnector.ids import *

from xt_modules_print_info import print_module_info
from xt_modules_record_playback_messages import start_recorder
from x4m200_print_record_playback_RESP_SLEEP_message import configure_x4m200
from x4m300_print_record_playback_PRESENCE_message import configure_x4m300
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


def configure_sensor_dp_output(device_name, record, datatype, format, dopplers, num_messages):

    # Turn on outputs.
    if dopplers == "both":
        ctrl = XTID_OUTPUT_CONTROL_PD_FAST_ENABLE | XTID_OUTPUT_CONTROL_PD_SLOW_ENABLE
    elif dopplers == "fast":
        ctrl = XTID_OUTPUT_CONTROL_PD_FAST_ENABLE
    elif dopplers == "slow":
        ctrl = XTID_OUTPUT_CONTROL_PD_SLOW_ENABLE
    else:
        print("Pulse-Doppler instance not recognized.", file=sys.stderr)
        raise SystemExit(1)

    if datatype == "pulsedoppler" and format == "byte":
        x4m200_par_settings['output_control'] = (
            XTS_ID_PULSEDOPPLER_BYTE, ctrl)
        x4m300_par_settings['output_control'] = (
            XTS_ID_PULSEDOPPLER_BYTE, ctrl)
    elif datatype == "pulsedoppler" and format == "float":
        x4m200_par_settings['output_control'] = (
            XTS_ID_PULSEDOPPLER_FLOAT, ctrl)
        x4m300_par_settings['output_control'] = (
            XTS_ID_PULSEDOPPLER_FLOAT, ctrl)
    elif datatype == "noisemap" and format == "byte":
        x4m200_par_settings['output_control'] = (XTS_ID_NOISEMAP_BYTE, ctrl)
        x4m300_par_settings['output_control'] = (XTS_ID_NOISEMAP_BYTE, ctrl)
    elif datatype == "noisemap" and format == "float":
        x4m200_par_settings['output_control'] = (XTS_ID_NOISEMAP_FLOAT, ctrl)
        x4m300_par_settings['output_control'] = (XTS_ID_NOISEMAP_FLOAT, ctrl)
    else:
        print("Datatype/format not recognized.", file=sys.stderr)
        raise SystemExit(1)

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


def print_dp_messages(xt_sensor, datatype, format, num_messages):
    # Set up state machinery for matrix assembly
    matrix = []
    is_negative = True
    prev_range_idx = -1
    ok = True
    instance = None
    n_printed = 0
    got_errors = False

    # Assemble whole range-Doppler matrices from the individual packets
    while True:
        if datatype == "pulsedoppler" and format == "byte":
            pkt = xt_sensor.read_message_pulsedoppler_byte()
        elif datatype == "pulsedoppler" and format == "float":
            pkt = xt_sensor.read_message_pulsedoppler_float()
        elif datatype == "noisemap" and format == "byte":
            pkt = xt_sensor.read_message_noisemap_byte()
        elif datatype == "noisemap" and format == "float":
            pkt = xt_sensor.read_message_noisemap_float()

        if instance is None:
            instance = pkt.pulsedoppler_instance

        if pkt.range_idx == 0 and is_negative and matrix:
            # We now have a complete range-Doppler matrix.
            matrix = np.array(matrix)
            print("Instance:", ["slow", "fast"][instance], matrix.shape)
            print(matrix)
            n_printed += 1
            if num_messages != 0 and n_printed >= num_messages:
                break
            # Reset
            matrix = []
            prev_range_idx = -1
            instance = pkt.pulsedoppler_instance

        if is_negative and pkt.range_idx == prev_range_idx + 1:
            ok = True
            prev_range_idx = pkt.range_idx
        elif not is_negative and pkt.range_idx == prev_range_idx:
            pass
        else:
            if ok:
                print("bad range_idx ", pkt.range_idx, " in instance ", instance,
                      ", prev ", prev_range_idx, ", resetting and waiting for "
                      "zero.", sep="")
                matrix = []
                is_negative = True
                prev_range_idx = -1
                ok = False
                got_errors = True
            continue

        if format == "byte":
            # Convert byte data to float.
            data = bytearray(pkt.get_data())
            offset = pkt.byte_step_start
            step = pkt.byte_step_size
            data = [10.0**((float(x)*step + offset)/10.0) for x in data]
        else:
            data = list(pkt.get_data())

        # A noisemap misses a piece in the middle, so fill it with zeroes
        # Has no effect if nothing is missing.
        if is_negative:
            pkt_end = pkt.frequency_start + pkt.frequency_step * pkt.frequency_count
            if pkt_end < 0.000001:
                data.extend(0 for _ in range(
                    int(round(-pkt_end / pkt.frequency_step))))
        else:
            if pkt.frequency_start > 0.000001:
                pre = [0 for _ in range(
                    int(round(pkt.frequency_start / pkt.frequency_step)))]
                data = pre + data

        if len(matrix) <= pkt.range_idx:
            matrix.append([])

        matrix[pkt.range_idx].extend(data)

        is_negative ^= True

    xt_sensor.set_sensor_mode(XTS_SM_STOP, 0)

    if got_errors:
        sys.exit(1)


def main():
    parser = ArgumentParser()
    parser.add_argument(
        "-d",
        "--device",
        dest="device_name",
        help="device file to use, example: python %s -d COM4" % sys.argv[0],
        metavar="FILE")
    parser.add_argument(
        "-r",
        "--record",
        action="store_true",
        default=False,
        dest="record",
        help="Enable recording")
    parser.add_argument(
        "-w",
        "--datatype",
        dest="datatype",
        default="pulsedoppler",
        help="Data to get. pulsedoppler or noisemap",
        metavar="TYPE")

    parser.add_argument(
        "-f",
        "--format",
        dest="format",
        default="byte",
        help="Data format to get. byte or float",
        metavar="FORMAT")

    parser.add_argument(
        "-m",
        "--dopplers",
        dest="dopplers",
        default="both",
        help="Which pD instance to get. fast, slow or both.",
        metavar="FORMAT")

    parser.add_argument(
        "-n",
        "--num-messages",
        dest="num_messages",
        type=int,
        default=0,
        help="how many matrices to read (0 = infinite)",
        metavar="INT")

    args = parser.parse_args()

    if args.device_name:
        device_name = args.device_name
    else:
        try:
            device_name = auto()[0]
        except:
            print("Fail to find serial port, please specify it by use -d!")
            raise
    xt_sensor = configure_sensor_dp_output(
        device_name, args.record, args.datatype, args.format, args.dopplers, args.num_messages)
    print_dp_messages(xt_sensor, args.datatype,
                      args.format, args.num_messages)


if __name__ == "__main__":
    main()
