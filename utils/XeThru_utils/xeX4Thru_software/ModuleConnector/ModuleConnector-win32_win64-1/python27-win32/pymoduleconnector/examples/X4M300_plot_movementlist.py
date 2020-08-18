#!/usr/bin/env python
# -*- coding: utf-8 -*-
""" \example X4M300_plot_movementlist.py

#Target module: X4M300

#Introduction: This is an example of how to plot MovementSlow list and MovementFast list read from X4M300.
               The initialization will around 2 minutes.

#Command to run: "python X4M300_plot_movementlist.py -d com8" or "python3 X4M300_plot_movementlist.py -d com8"
                 change "com8" with your device name, using "--help" to see other options.
                 Using TCP server address as device name is also supported, e.g. 
                 "python X4M200_sleep_record.py -d tcp://192.168.1.169:3000".
"""
from __future__ import print_function
import numpy as np
import sys
from time import sleep
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
from optparse import OptionParser
from pymoduleconnector import ModuleConnector

def reset(device_name):

    mc = ModuleConnector(device_name)
    XEP = mc.get_xep()
    XEP.module_reset()
    mc.close()
    sleep(3)

def x4m300_presence_simpleoutput(device_name, detection_zone=(0.5,9), sensitivity=5):
    # User settings
    detzone_start = detection_zone[0]
    detzone_end = detection_zone[1]

    #Reset module
    reset(device_name)

    mc = ModuleConnector(device_name, log_level=0)
    x4m300 = mc.get_x4m300()

    try:
        x4m300.set_sensor_mode(13, 0) # Make sure no profile is running.
        print("Stopped already running profile.")
    except RuntimeError:
        # If not initialized, stop returns error. Still OK, just wanted to make sure the profile was not running.
        pass

    # Read module info
    print("FirmwareID: " + x4m300.get_system_info(2))
    print("Version: " + x4m300.get_system_info(3))
    print("Build: " + x4m300.get_system_info(4))
    print("Serial number: " + x4m300.get_system_info(6))

    print("Loading new profile.")
    x4m300.load_profile(0x014d4ab8)

    print("Selecting module output.")
    x4m300.set_output_control(0x723bfa1f, 1) # PresenceMovingList

    print("Setting user settings: DetectionZone = " + str(detzone_start) + " to " + str(detzone_end) + ", Sensitivity = " + str(sensitivity))
    x4m300.set_detection_zone(detzone_start, detzone_end)
    x4m300.set_sensitivity(sensitivity)

    print("Start profile execution.")
    x4m300.set_sensor_mode(1, 0) # Make sure no profile is running.

    def clear_buffer():
        """Clears the frame buffer"""
        while x4m300.peek_message_presence_movinglist():
            x4m300.read_message_presence_movinglist()

    def read_movlist_fast():
        d = x4m300.read_message_presence_movinglist()
        mfast = np.array(d.movement_fast_items)
        mslow = np.array(d.movement_slow_items)
        return mfast, mslow

    def animate(i):
        mfast, mslow = read_movlist_fast();
        line1.set_ydata(mfast)  # update the data
        line2.set_ydata(mslow)  # update the data
        return line1, line2

    d = x4m300.read_message_presence_movinglist()
    State = d.presence_state
    print("Initializing,this will take around 2mins")
    while State == 2:
        d = x4m300.read_message_presence_movinglist()
        State = d.presence_state
    print("Initializing Done!")

    print("Start streaming movinglist...")
    fig = plt.figure()
    fig.suptitle("PresenceMovingList example")
    ax1 = fig.add_subplot(2,1,1)
    ax2 = fig.add_subplot(2,1,2)
    ax2.set_xlabel('Distance')
    ax1.set_ylabel('Fast Movement Metric')
    ax2.set_ylabel('Slow Movement Metric')

    mfast, mslow = read_movlist_fast()

    #setting upperlimits for y-axsis
    ax1.set_ylim(0,100)
    ax2.set_ylim(0,100)

    line1, = ax1.plot(mfast)
    line2, = ax2.plot(mslow)

    clear_buffer()
    ani = FuncAnimation(fig, animate, interval=100)
    plt.show()

    # stop streaming
    x4m300.set_sensor_mode(0x13, 0)

def main():
    parser = OptionParser()
    parser.add_option(
        "-d",
        "--device",
        dest = "device_name",
        help = "device file to use, example: python %s -d COM4"%sys.argv[0],
        metavar = "FILE")

    parser.add_option('-z', '--detection_zone', nargs = 2, type = 'float',
        help = 'Start and stop of detection zone.', metavar = 'START STOP',
        default = (0.5, 9))

    parser.add_option('-s', '--sensitivity', nargs = 1, type = 'int',
        help = 'Sensor Sensitivity.', metavar = 'SENSITIVITY',
        default = 5)

    (options, args) = parser.parse_args()

    if not options.device_name:
        print("Please specify a device name, example: python %s -d COM4"%sys.argv[0])
        sys.exit(1)
    x4m300_presence_simpleoutput(**vars(options))

if __name__ == "__main__":
    main()
