import intelhex
from pymoduleconnector import ModuleConnector
from pymoduleconnector import Bootloader
from pymoduleconnector.ids import *
from pymoduleconnector.extras.auto import *
from time import sleep
from optparse import OptionParser

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
    sleep(1) # Allow for MC to read waiting messages from module.
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
    sleep(1) # Allow for MC to read waiting messages from module.
    print(xep.get_system_info(XTID_SSIC_FIRMWAREID))
    print(xep.get_system_info(XTID_SSIC_VERSION))
    print(xep.get_system_info(XTID_SSIC_BUILD))
    mc.close()

def main():
    parser = OptionParser()
    parser.add_option(
        "-s",
        "--skip",
        action="store_true",
        dest="skip_goto",
        default=False,
        help="skip goto bootloader")

    parser.add_option(
        "-f",
        "--file",
        dest="hex_file_name",
        default="xep_x4m0x_s70_4bl_checksum.hex",
        help="hex file to flash",
        metavar="FILE")

    parser.add_option(
        "-d",
        "--device",
        dest="device_name",
        default="auto",
        help="device file to use",
        metavar="FILE")

    (options, args) = parser.parse_args()

    upgrade_fw(options.hex_file_name, options.skip_goto, options.device_name)
    sleep(3)
    display_information(options.device_name)

if __name__ == "__main__":
    main()
