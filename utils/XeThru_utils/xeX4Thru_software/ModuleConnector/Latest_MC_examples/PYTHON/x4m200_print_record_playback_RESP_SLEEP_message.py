#!/usr/bin/env python
""" \example x4m200_print_record_playback_RESP_SLEEP_message.py

Latest examples is located at https://github.com/xethru/XeThru_ModuleConnector_Examples or https://dev.azure.com/xethru/XeThruApps/_git/XeThru_ModuleConnector_Examples.

# Target module:
# X4M200

# Introduction:
# This is an example of how to print out application messages from X4M200 module.

# prerequisite:
# ModuleConnector python lib is installed, check XeThruSensorsIntroduction application note to get detail
# xt_modules_print_info.py should be in the same folder
# xt_modules_record_playback_messages.py should be in the same folder

# Command to run:
# 1. Use "python x4m200_print_record_playback_RESP_SLEEP_message.py" to print application message. If device is not be automatically recognized,add argument "-d com8" to specify device. change "com8" with your device name, using "--help" to see other args. Using TCP server address as device name is also supported by specify TCP address like "-d tcp://192.168.1.169:3000". Adding "-r" to enable recording during application messages printing out.
# 2. Use  "python x4m200_print_record_playback_RESP_SLEEP_message.py -f xethru_recording_xxxx/xethru_recording_meta.dat" to play back recording file.

"""
from __future__ import print_function, division
import sys
from argparse import ArgumentParser

import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animation

import pymoduleconnector
from pymoduleconnector import ModuleConnector
from pymoduleconnector import DataType
from pymoduleconnector import DataRecorder
from pymoduleconnector import DataPlayer
from pymoduleconnector import RecordingOptions

from pymoduleconnector.extras.auto import auto
from pymoduleconnector.ids import *

from xt_modules_print_info import print_module_info
from xt_modules_print_info import print_sensor_settings
from xt_modules_record_playback_messages import start_recorder
from xt_modules_record_playback_messages import start_player

respiration_sensor_state_text = (
    "BREATHING", "MOVEMENT", "MOVEMENT TRACKING", "NO MOVEMENT", "INITIALIZING")
# User settings introduction
"""
output control:
Only one of XTS_ID_BASEBAND_IQ and XTS_ID_BASEBAND_AMPLITUDE_PHASE can be enabled at a time. Enabling one disables the other. Disabling one, even if it is already disabled, disables the other.
Only one of XTS_ID_PULSEDOPPLER_FLOAT and XTS_ID_PULSEDOPPLER_BYTE can be enabled for each of XTID_OUTPUT_CONTROL_PD_SLOW_ENABLE and XTID_OUTPUT_CONTROL_PD_FAST_ENABLE. Same with XTS_ID_NOISEMAP_FLOAT and XTS_ID_NOISEMAP_BYTE. Turning on a float output automatically disables the byte output, and vice versa.
"""
x4m200_par_settings = {'detection_zone': (0.4, 2),
                       'sensitivity': 5,
                       'tx_center_frequency': 3, # 3: TX 7.29GHz low band center frequency, 4: TX 8.748GHz low band center frequency.
                       'led_control': (XTID_LED_MODE_OFF, 0),
                       # initialize noisemap everytime when get start (approximately 120s)
                       'noisemap_control': 0b110,
                       # only uncomment the message when you need them to avoide confliction.
                       #    'output_control1': (XTS_ID_BASEBAND_IQ, 0),
                       #    'output_control2': (XTS_ID_BASEBAND_AMPLITUDE_PHASE, 0),
                       #    'output_control3': (XTS_ID_PULSEDOPPLER_FLOAT, 0),
                       #    'output_control4': (XTS_ID_PULSEDOPPLER_BYTE, 0),
                       #    'output_control5': (XTS_ID_NOISEMAP_FLOAT, 0),
                       #    'output_control6': (XTS_ID_NOISEMAP_BYTE, 0),
                       'output_control7': (XTS_ID_SLEEP_STATUS, 1),
                       #     'output_control8': (XTS_ID_RESP_STATUS, 1),
                       #    'output_control9': (XTS_ID_VITAL_SIGNS, 0),   
                       'output_control10': (XTS_ID_RESPIRATION_MOVINGLIST, 1),
                       #    'output_control11': (XTS_ID_RESPIRATION_DETECTIONLIST, 0)
                       }


def configure_x4m200(device_name, record=False, x4m200_settings=x4m200_par_settings):

    mc = ModuleConnector(device_name)
    x4m200 = mc.get_x4m200()

    ''' Following setting will enalbe debug info which contains MCU workload
    mc = ModuleConnector(device_name, log_level=2)
    x4m200 = mc.get_x4m200()
    x4m200.set_debug_level(9)
    mc.get_not_supported().set_parameter_file(
        "profiling.par", "[Debug]\nprofileReportPeriod=10;\n")
    '''

    print('Clearing buffer')
    while x4m200.peek_message_baseband_iq():
        x4m200.read_message_baseband_iq()
    while x4m200.peek_message_baseband_ap():
        x4m200.read_message_baseband_ap()
    while x4m200.peek_message_respiration_legacy():
        x4m200.read_message_respiration_legacy()
    while x4m200.peek_message_respiration_sleep():
        x4m200.read_message_respiration_sleep()
    while x4m200.peek_message_respiration_movinglist():
        x4m200.read_message_respiration_movinglist()
    while x4m200.peek_message_pulsedoppler_byte():
        x4m200.read_message_pulsedoppler_byte()
    while x4m200.peek_message_pulsedoppler_float():
        x4m200.read_message_pulsedoppler_float()
    while x4m200.peek_message_noisemap_byte():
        x4m200.read_message_noisemap_byte()
    while x4m200.peek_message_noisemap_float():
        x4m200.read_message_noisemap_float()

    print('Start recorder if recording is enabled')
    if record:
        start_recorder(mc)

    print('Ensuring no Xethru profile running')
    try:
        x4m200.set_sensor_mode(XTID_SM_STOP, 0)
    except RuntimeError:
        print('Xethru module could not enter stop mode')
    print('Loading new Xethru profile')
    # x4m200.load_profile(XTS_ID_APP_RESPIRATION_2)# XTS_ID_APP_RESPIRATION_2 for adult and XTS_ID_APP_RESPIRATION_3 for baby
    # XTS_ID_APP_RESPIRATION_2 for adult and XTS_ID_APP_RESPIRATION_3 for baby
    x4m200.load_profile(XTS_ID_APP_RESPIRATION_2)

    print('Set parameters')
    for variable, value in x4m200_settings.items():
        try:
            if 'output_control' in variable:
                variable = 'output_control'
            setter_set = getattr(x4m200, 'set_' + variable)
        except AttributeError as e:
            print("X4M200 does not have a setter function for '%s'." % variable)
            raise e
        if isinstance(value, tuple):
            setter_set(*value)
        else:
            setter_set(value)
        print("Setting %s to %s" % (variable, value))

    print_sensor_settings(x4m200)

    print('Set module to RUN mode')
    try:
        x4m200.set_sensor_mode(XTID_SM_RUN, 0)  # RUN mode
    except RuntimeError:
        print('Xethru module cloud not enter run mode')

    return x4m200


def print_x4m200_messages(x4m200):
    try:
        while True:
            # while x4m200.peek_message_respiration_legacy(): # update every 1/17 second
            #     rdata = x4m200.read_message_respiration_legacy() 
            #     print("message_respiration_legacy:\n frame_counter: {} sensor_state: {} respiration_rate: {} distance: {} Breath Pattern: {} signal_quality: {}\n" .format(rdata.frame_counter, rdata.sensor_state, rdata.respiration_rate, rdata.distance, rdata.movement, rdata.signal_quality))
            while x4m200.peek_message_respiration_sleep(): # update every 1 second
                rdata = x4m200.read_message_respiration_sleep()  
                print("message_respiration_sleep: frame_counter: {} sensor_state: {} respiration_rate: {} distance: {} movement_slow: {} movement_fast: {}".format(rdata.frame_counter, respiration_sensor_state_text[rdata.sensor_state], rdata.respiration_rate, rdata.distance, rdata.movement_slow, rdata.movement_fast))
            # while x4m200.peek_message_vital_signs():  # update every 1 second
            #     rdata = x4m200.read_message_vital_signs() 
            #      print("message_vital_signs: frame_counter: {} sensor_state: {} respiration_rate: {} respiration_distance: {} respiration_confidence: {} heart_rate: {} heart_distance: {} heart_confidence: {}".format(
            #      rdata.frame_counter, respiration_state_text[rdata.sensor_state], rdata.respiration_rate, rdata.respiration_distance, rdata.respiration_confidence, rdata.heart_rate, rdata.heart_distance, rdata.heart_confidence))
            while x4m200.peek_message_respiration_movinglist(): # update every 1 second
                rdata = x4m200.read_message_respiration_movinglist() # update every 1 second
                print("message_respiration_movinglist:\ncounter: {} \nmovement_slow_items: {} \nmovement_fast_items: {}\n".format(rdata.counter, np.array(rdata.movement_slow_items), np.array(rdata.movement_fast_items)))
    except:
        print('Messages output finish!')
    sys.exit(0)


def main():
    parser = ArgumentParser()
    parser.add_argument(
        "-d",
        "--device",
        dest="device_name",
        help="Seral port name used by target XeThru sensor, i.e com8, /dev/ttyACM0",
        metavar="FILE")
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
        print_module_info(device_name)
        x4m200 = configure_x4m200(
            device_name, args.record, x4m200_par_settings)

    else:
        player = start_player(meta_filename=args.meta_filename)
        mc = ModuleConnector(player, log_level=0)
        x4m200 = mc.get_x4m200()
    print_x4m200_messages(x4m200)


if __name__ == "__main__":
    main()
