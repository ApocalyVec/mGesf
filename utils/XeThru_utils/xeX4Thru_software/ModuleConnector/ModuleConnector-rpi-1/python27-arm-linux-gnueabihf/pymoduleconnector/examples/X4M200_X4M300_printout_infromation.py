#!/usr/bin/env python
# -*- coding: utf-8 -*-
""" \example X4M200_X4M300_printout_infromation.py

#Target module: X4M200,X4M300

#Introduction: This is an example of how to use the XEP interface from python to get module infromation.
			   
#Command to run: "python X4M200_X4M300_printout_infromation.py -d com8" or "python3 X4M200_X4M300_printout_infromation.py -d com8"
                 Using TCP server address as device name is also supported, e.g. 
                 "python X4M200_sleep_record.py -d tcp://192.168.1.169:3000".


"""
from __future__ import print_function

import pymoduleconnector
from pymoduleconnector import ModuleConnector
from pymoduleconnector.ids import *
from optparse import OptionParser
import sys
import time


def try_xep(device_name):

    log_level = 0
    mc = ModuleConnector(device_name, log_level)
    x4m300 = mc.get_x4m300()

    # we have to go to manual mode
    x4m300.set_sensor_mode(XTS_SM_STOP, 0)
    x4m300.set_sensor_mode(XTS_SM_MANUAL, 0);

    xep = mc.get_xep()
    pong = xep.ping()
    print("Received pong:", hex(pong))

    print('ItemNumber =', xep.get_system_info(XTID_SSIC_ITEMNUMBER));
    print('OrderCode =', xep.get_system_info(XTID_SSIC_ORDERCODE));
    print('FirmWareID =', xep.get_system_info(XTID_SSIC_FIRMWAREID));
    print('Version =', xep.get_system_info(XTID_SSIC_VERSION));
    print('Build =', xep.get_system_info(XTID_SSIC_BUILD));
    print('SerialNumber =', xep.get_system_info(XTID_SSIC_SERIALNUMBER));
    print('VersionList =', xep.get_system_info(XTID_SSIC_VERSIONLIST));

    # inti x4driver
    xep.x4driver_init()

    # Set enable pin
    xep.x4driver_set_enable(1);

    # Set iterations
    xep.x4driver_set_iterations(16);
    # Set pulses per step
    xep.x4driver_set_pulses_per_step(256);
    # Set dac min
    xep.x4driver_set_dac_min(949);
    # Set dac max
    xep.x4driver_set_dac_max(1100);
    # Set TX power
    xep.x4driver_set_tx_power(2);

    # Enable downconversion
    xep.x4driver_set_downconversion(1);

    # Set frame area offset
    xep.x4driver_set_frame_area_offset(0.18)
    offset = xep.x4driver_get_frame_area_offset()
    print('x4driver_get_frame_area_offset returned: ', offset)


    # Set frame area
    xep.x4driver_set_frame_area(2,6)
    frame_area = xep.x4driver_get_frame_area()
    print('x4driver_get_frame_area returned: [', frame_area.start, ', ', frame_area.end, ']');

    # Set TX center freq
    xep.x4driver_set_tx_center_frequency(3);

    # Set PRFdiv
    xep.x4driver_set_prf_div(16)
    prf_div = xep.x4driver_get_prf_div()
    print('x4driver_get_prf_div returned: ', prf_div)


    # Start streaming
    xep.x4driver_set_fps(20)
    fps = xep.x4driver_get_fps()
    print('xep_x4driver_get_fps returned: ' ,fps)

    # Wait 5 sec.
    time.sleep(5)

    # Stop streaming
    xep.x4driver_set_fps(0);

    # Read data float if available.
    if xep.peek_message_data_float() > 0:
        data_float = xep.read_message_data_float()
    else:
        print('No data float messages available.')

    # Reset module
    xep.module_reset()


def main():
    parser = OptionParser()
    parser.add_option(
        "-d",
        "--device",
        dest="device_name",
        help="device file to use",
        metavar="FILE")

    (options, args) = parser.parse_args()

    if not options.device_name:
        print("you have to specify device, e.g.: python %s -d /dev/ttyACM0" % sys.argv[0])
        sys.exit(1)

    try_xep(options.device_name)

if __name__ == "__main__":
    main()
