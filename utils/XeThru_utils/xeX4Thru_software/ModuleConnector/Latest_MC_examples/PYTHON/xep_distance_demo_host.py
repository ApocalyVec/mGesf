#!/usr/bin/env python
""" \example xep_distance_demo_host.py

Latest examples is located at https://github.com/xethru/XeThru_ModuleConnector_Examples or https://dev.azure.com/xethru/XeThruApps/_git/XeThru_ModuleConnector_Examples.

# Target module:
# X4M03(running XEP Distance Demo)

XEP Distance Demo: https://www.xethru.com/community/resources/xep-x4-distance-demo.89/

# Introduction: This is XEP distance demo host side example. It can plot baseband data and print presence and distance infomation from module.
This example is modified from "xt_modules_plot_record_playback_radar_raw_data_message.py".

# prerequisite:
# ModuleConnector python lib is installed, check XeThruSensorsIntroduction application note to get detail
# xt_modules_print_info.py should be in the same folder
# xt_modules_record_playback_messages.py should be in the same folder


# Command to run:
# 1. Use "python xep_distance_demo_host.py" to plot radar raw data. If device is not be automatically recognized,add argument "-d com8" to specify device. change "com8" with your device name, using "--help" to see other args. Using TCP server address as device name is also supported by specify TCP address like "-d tcp://192.168.1.169:3000".
# 2. add "-r" to enable recording.
# 3. use  "python xep_distance_demo_host.py -f xethru_recording_xxxx/xethru_recording_meta.dat" to play back recording file. Add "-b" if the recording is baseband data.
"""
from __future__ import print_function, division
import sys
from argparse import ArgumentParser

import numpy as np
from six.moves import queue
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
from matplotlib.colors import cnames
from matplotlib.animation import FuncAnimation

import pymoduleconnector
from pymoduleconnector import ModuleConnector
from pymoduleconnector import DataType
from pymoduleconnector.extras.x4_regmap_autogen import X4
from pymoduleconnector.extras.auto import auto
from pymoduleconnector.ids import *

from xt_modules_print_info import *
from xt_modules_record_playback_messages import *

# User settings
x4_par_settings = {'downconversion': 0,  # 0: output rf data; 1: output baseband data
                   'dac_min': 949,
                   'dac_max': 1100,
                   'dac_step': 0,  # real dac step is 1 when set dac_step = 0
                   'iterations': 16,
                   'pulses_per_step': 300,
                   'frame_area_offset': 0.18,
                   'frame_area': (-0.5, 3),
                   'fps': 17,
                   }


def configure_x4(device_name, record=False, baseband=True, x4_settings=x4_par_settings):
    mc = pymoduleconnector.ModuleConnector(device_name)
    # Assume an X4M300/X4M200 module and try to enter XEP mode
    app = mc.get_x4m200()
    # Stop running application and set module in manual mode.
    try:
        app.set_sensor_mode(XTS_SM_STOP, 0)  # Make sure no profile is running.
    except RuntimeError:
        # Profile not running, OK
        pass
    try:
        app.set_sensor_mode(XTS_SM_MANUAL, 0)  # Manual mode.
    except RuntimeError:
        pass
    xep = mc.get_xep()

    print('Clearing buffer')
    while xep.peek_message_data_float():
        xep.read_message_data_float()
    while xep.peek_message_data_string():
        xep.read_message_data_string()

    print('Start recorder if recording is enabled')
    if record:
        start_recorder(mc)

    print_x4_settings(xep)
    return xep


def plot_radar_raw_data_message(xep, baseband=True, frames_number=1):
    mc_i = pymoduleconnector.moduleconnectorwrapper
    id_ptr = mc_i.new_uint32_t_ptr()
    info_ptr = mc_i.new_uint32_t_ptr()
    data = mc_i.new_string_ptr()

    def read_frame():
        """Gets frame data from module"""
        xep.read_message_data_string(id_ptr, info_ptr, data)
        print("Info: content_id: {} info: {} data: {} .".format(
            mc_i.uint32_t_ptr_value(id_ptr), mc_i.uint32_t_ptr_value(info_ptr), mc_i.string_ptr_value(data)))
        d = xep.read_message_data_float()
        frame = np.array(d.data)
        # print(frame)
        # print('frame length:' + str(len(frame)))
        # Convert the resulting frame to a complex array if downconversion is enabled
        if baseband:
            n = len(frame)
            frame = abs((frame[:n//2] + 1j*frame[n//2:]))
        return frame
    # initialization function: plot the background of each frame

    def init():
        for i in range(len(frames)):
            lines[0].set_data(x, Y[i])
            lines[0].set_3d_properties(frames[i])
        return lines

    def update_lines(ii):
        q.appendleft(read_frame())
        for i in range(len(lines)):
            lines[i].set_data(x, Y[i])
            lines[i].set_3d_properties(q[i])
        fig.canvas.draw()
        return lines

    frames = np.array([read_frame() for i in range(frames_number)])
    rangebins_number = len(frames[0])
    fps = xep.x4driver_get_fps()

    fig = plt.figure()
    ax = fig.add_subplot(111, projection='3d')
    # ax.axis('off')
    # keep graph in frame (FIT TO YOUR DATA), can be adjusted
    ax.set_xlabel('Range Bins Number: ' + str(rangebins_number))
    ax.set_ylabel('Frames Number: ' + str(frames_number))
    # ax.set_zlabel('Power')
    ax.set_zlim3d(0 if baseband else -0.15, 0.15)

    x = np.arange(rangebins_number)
    y = np.arange(frames_number)
    # y=np.arange(len(frames[0]))
    X, Y = np.meshgrid(x, y)
    q = queue.deque(frames, frames_number)
    colors = ['b']*frames_number
    colors[0] = 'r'
    lines = [ax.plot(x, Y[i], frames[i], '-', color=colors[i])[0]
             for i in range(frames_number)]
    anim = FuncAnimation(fig, update_lines, interval=1,
                         init_func=init, blit=True)
    plt.show()


def main():
    parser = ArgumentParser()
    parser.add_argument(
        "-d",
        "--device",
        dest="device_name",
        help="device file to use",
        metavar="FILE")
    parser.add_argument(
        "-b",
        "--baseband",
        action="store_true",
        default=True,
        dest="baseband",
        help="Enable baseband output")
    parser.add_argument(
        "-rf",
        "--radiofrequency",
        action="store_false",
        dest="baseband",
        help="Enable rf output")
    parser.add_argument(
        "-n",
        "--framesnumber",
        metavar="NUMBER",
        type=int,
        default=10,
        dest="frames_number",
        help="Decide how mange frames shown on plotting")
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
        xep = configure_x4(device_name,
                           args.record, args.baseband, x4_par_settings)
    else:
        player = start_player(meta_filename=args.meta_filename)
        mc = ModuleConnector(player, log_level=0)
        xep = mc.get_xep()

    plot_radar_raw_data_message(
        xep, baseband=args.baseband, frames_number=args.frames_number)


if __name__ == "__main__":
    main()
