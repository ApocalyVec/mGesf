#!/usr/bin/env python
# -*- coding: utf-8 -*-
""" \example xt_module_firmware_update.py

Latest examples is located at https://github.com/xethru/XeThru_ModuleConnector_Examples or https://dev.azure.com/xethru/XeThruApps/_git/XeThru_ModuleConnector_Examples.

# Target module: 
# XeThru module using X4 chip, running firmware with bootloader.

# Introduction: 
# This is an example of how to update X4M200, X4M300 firmware.

# prerequisite:
# ModuleConnector python lib is installed, check XeThruSensorsIntroduction application note to get detail

# Command to run: 
# "python xt_module_firmware_update.py -f Annapurna_4bl_cert.hex", using "--help" to see other args.
Latest firmware can be download from: https://www.xethru.com/community/resources/annapurna-firmware.98/
"""
import intelhex
from pymoduleconnector import ModuleConnector
from pymoduleconnector import Bootloader
from pymoduleconnector.ids import *
from pymoduleconnector.extras.auto import auto
from time import sleep
from argparse import ArgumentParser


def goto_bootloader(device_name):
    if device_name == "auto":
        devices = auto()
        if len(devices) == 0:
            print("Failed to find COM port matching known vid and pids")
            return 1
        device_name = devices[0]

    log_level = 1
    mc = ModuleConnector(device_name, log_level)
    x4m300 = mc.get_x4m300()
    sleep(1)  # Allow for MC to read waiting messages from module.
    try:
        # if this fails it will throw
        x4m300.start_bootloader()
    except:
        print("Could not go to bootloader, may be there already.")
    mc.close()


def upgrade_fw(hex_file_name, skip_goto, device_name):
    ih = intelhex.IntelHex()
    ih.fromfile(hex_file_name, "hex")

    if not skip_goto:
        goto_bootloader(device_name)
        sleep(6)

    log_level = 1
    if device_name == "auto":
        devices = auto()
        if len(devices) == 0:
            print("Failed to find COM port matching known vid and pids")
            return 1
        device_name = devices[0]
    bootloader = Bootloader(device_name, log_level)

    print(bootloader.get_bootloader_info())

    flash_offset = 0x400000
    blob = ih.tobinarray(start=flash_offset)
    page_size = 512
    pages = len(blob) / page_size
    if (len(blob) % page_size) > 0:
        pages += 1

    for page in range(0, int(pages)):
        if page == 0 or page > 127:
            address = flash_offset + page * page_size
            bootloader.write_page(
                page,
                ih.tobinarray(start=address, size=page_size))

    sleep(1)
    bootloader.start_application(12)
    print("done")


def display_information(device_name):
    print("trying to read information from module")
    log_level = 1
    if device_name == "auto":
        devices = auto()
        if len(devices) == 0:
            print("Failed to find COM port matching known vid and pids")
            return 1
        device_name = devices[0]
    mc = ModuleConnector(device_name, log_level)
    xep = mc.get_xep()
    sleep(1)  # Allow for MC to read waiting messages from module.
    print(xep.get_system_info(XTID_SSIC_FIRMWAREID))
    print(xep.get_system_info(XTID_SSIC_VERSION))
    print(xep.get_system_info(XTID_SSIC_BUILD))
    mc.close()


def main():
    parser = ArgumentParser()
    parser.add_argument(
        "-s",
        "--skip",
        action="store_true",
        dest="skip_goto",
        default=False,
        help="skip goto bootloader")

    parser.add_argument(
        "-f",
        "--file",
        dest="hex_file_name",
        default="xep_x4m0x_s70_4bl_checksum.hex",
        help="hex file to flash",
        metavar="FILE")

    parser.add_argument(
        "-d",
        "--device",
        dest="device_name",
        default="auto",
        help="device file to use",
        metavar="FILE")

    args = parser.parse_args()

    upgrade_fw(args.hex_file_name, args.skip_goto, args.device_name)
    sleep(3)
    display_information(args.device_name)


if __name__ == "__main__":
    main()
