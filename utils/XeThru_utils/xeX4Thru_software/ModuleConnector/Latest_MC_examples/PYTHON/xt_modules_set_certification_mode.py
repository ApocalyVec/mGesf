#!/usr/bin/env python
""" 
Latest examples is located at https://github.com/xethru/XeThru_ModuleConnector_Examples or https://dev.azure.com/xethru/XeThruApps/_git/XeThru_ModuleConnector_Examples.

# Target module:
# X4M200
# X4M300
# X4M03(with XEP)

# Introduction:
# This is script to set XeThru Module to Certification Modes.

XeThru Module Regulation Approval application note:
https://www.xethru.com/community/resources/xethru-regulatory-approval.123/

# prerequisite:
# ModuleConnector python lib is installed, check [XeThruSensorsIntroduction](https://www.xethru.com/community/resources/xethru-sensors-introduction.111/) application note for more detail.
# xt_modules_print_info.py should be in the same folder


# Command to run:
# 1. Use "python xt_modules_set_certification_mode.py --hlep" to learn how to use this script. 

"""
from __future__ import print_function, division
import sys
from argparse import ArgumentParser
from pymoduleconnector import ModuleConnector
import pymoduleconnector.moduleconnectorwrapper as mcw
from pymoduleconnector.extras.auto import auto
# Manually set the device name, e.g. "COM3"
# or set to "auto" to search for the device

def set_certification_mode(device_name, mode_code):
    mc = ModuleConnector(device_name)
    xep = mc.get_xep()
    app = mc.get_x4m300()
    data = mcw.ucVector()
    app.system_run_test(mode_code, data)
    print("Set Mode:0x%X" %mode_code)
    xep.module_reset()
    mc.close()


def main():
    parser = ArgumentParser()

    parser.add_argument(
    "-d",
    "--device",
    dest="device_name",
    help="Optional, program can dectect serial port automatically, if not work, using this option to set seral port name used by target XeThru sensor manually, i.e com8, /dev/ttyACM0",
    metavar="FILE")

    parser.add_argument(
    "-m",
    "--mode",
    type= str,
    default='0x1A',
    dest="mode_code",
    help="Input test mode: 0x1A: Enalbe Certification Mode, 0x1D: Tx Only, 0x1E: TX Digital Only, 0x1F: Stream TX Off,  0x1B: Disable Certification Mode.",
    metavar="0xXX")
    args = parser.parse_args()

    if args.device_name:
        device_name = args.device_name
    else:
        try:
            device_name = auto()[0]
        except:
            print("Fail to find serial port, please specify it by use -d!")
            raise

    set_certification_mode(device_name, mode_code = int(args.mode_code,0))

if __name__ == "__main__":
    main()
