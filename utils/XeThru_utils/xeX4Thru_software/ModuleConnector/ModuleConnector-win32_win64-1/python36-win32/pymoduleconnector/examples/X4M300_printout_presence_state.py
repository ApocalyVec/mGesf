#!/usr/bin/env python
""" \example X4M300_printout_presence_state.py

#Target module: X4M300

#Introduction: This is an example of how to set up and read presence single messages from the
               X4M300 module with the ModuleConnector python wrapper.

#Command to run: "python X4M300_printout_presence_state.py -d com8" or "python3 X4M300_printout_presence_state.py -d com8"
                 change "com8" with your device name, using "--help" to see other options.
                 Using TCP server address as device name is also supported, e.g. 
                 "python X4M200_sleep_record.py -d tcp://192.168.1.169:3000".
		  
"""

from __future__ import print_function
import numpy as np
import time
from pymoduleconnector import ModuleConnector
from pymoduleconnector.ids import *
from time import sleep


def x4m300_presence_simpleoutput(device_name, detection_zone=(0.5,9), sensitivity=5, num_messages=0):

    # User settings
    detzone_start = detection_zone[0]
    detzone_end = detection_zone[1]

    presence_state_text = []
    presence_state_text.append("No presence")
    presence_state_text.append("Presence")
    presence_state_text.append("Initializing")

    mc = ModuleConnector(device_name, log_level=0)
    x4m300 = mc.get_x4m300()

    sleep(1) # Allow for MC to read waiting messages from module.

    try:
        x4m300.set_sensor_mode(XTID_SM_STOP, 0) # Make sure no profile is running.
        print("Stopped already running profile.")
    except RuntimeError:
        # If not initialized, stop returns error. Still OK, just wanted to make sure the profile was not running.
        pass

    # Now flush old messages from module
    print("Flushing any old data.")
    while x4m300.peek_message_presence_single():
        presence_single = x4m300.read_message_presence_single()

    # Read module info
    print("FirmwareID:", x4m300.get_system_info(XTID_SSIC_FIRMWAREID))
    print("Version:", x4m300.get_system_info(XTID_SSIC_VERSION))
    print("Build:", x4m300.get_system_info(XTID_SSIC_BUILD))
    print("Serial number:", x4m300.get_system_info(XTID_SSIC_SERIALNUMBER))

    print("Loading new profile.")
    x4m300.load_profile(XTS_ID_APP_PRESENCE_2)

    print("Selecting module output.")
    x4m300.set_output_control(XTS_ID_PRESENCE_SINGLE, 1) # PresenceSingle
    x4m300.set_output_control(XTS_ID_PRESENCE_MOVINGLIST, 0) # PresenceMovingList

    print("Setting user settings: DetectionZone = " + str(detzone_start) + " to " + str(detzone_end) + ", Sensitivity = " + str(sensitivity))
    x4m300.set_detection_zone(detzone_start, detzone_end)
    x4m300.set_sensitivity(sensitivity)

    print("Start profile execution.")
    x4m300.set_sensor_mode(XTID_SM_RUN, 0) # Make sure no profile is running.

    print("Waiting for data...")

    n = 0
    while num_messages == 0 or n < num_messages:
        time.sleep(0.1)

        while x4m300.peek_message_presence_single():
            presence_single = x4m300.read_message_presence_single()
            print("Presence ->"
                + " FrameCounter: " + str(presence_single.frame_counter)
                + ", State: " + presence_state_text[presence_single.presence_state]
                + ", Distance: " + str(round(presence_single.distance,2))
                + ", SignalQuality: " + str(presence_single.signal_quality)
                )
            n += 1

    x4m300.set_sensor_mode(XTID_SM_STOP, 0)

def main():
    import sys
    from optparse import OptionParser
    parser = OptionParser()
    parser.add_option(
        "-d",
        "--device",
        dest="device_name",
        help="device file to use, example: python %s -d COM4"%sys.argv[0],
        metavar="FILE")

    parser.add_option(
        "-n",
        "--num-messages",
        dest="num_messages",
        type=int,
        default=0,
        help="how many messages to read (0 = infinite)",
        metavar="INT")

    parser.add_option('-z', '--detection_zone', nargs=2, type='float',
        help='Start and stop of detection zone.', metavar='START STOP',
        default=(0.5, 9))

    parser.add_option('-s', '--sensitivity', nargs=1, type='int',
        help='Sensor Sensitivity.', metavar='SENSITIVITY',
        default=5)

    (options, args) = parser.parse_args()

    if not options.device_name:
        print("Please specify a device name, example: python %s -d COM4"%sys.argv[0])
        sys.exit(1)
    x4m300_presence_simpleoutput(**vars(options))


if __name__ == "__main__":
    main()
