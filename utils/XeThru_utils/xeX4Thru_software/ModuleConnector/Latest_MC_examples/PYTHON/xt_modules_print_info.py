#!/usr/bin/env python
""" \example xt_module_print_info.py

Latest examples is located at https://github.com/xethru/XeThru_ModuleConnector_Examples or https://dev.azure.com/xethru/XeThruApps/_git/XeThru_ModuleConnector_Examples.

# Target module:
# X4M200
# X4M300
# X4M03(XEP)

# Introduction:
# This is an example of how to print out XeThru Module infromation.

# prerequisite:
# ModuleConnector python lib is installed, check XeThruSensorsIntroduction application note to get detail

# Command to run:
# Use "xt_module_print_info.py" directly. If device is not be automatically recognized,add argument "-d com8" to specify device. change "com8" with your device name.

"""
from __future__ import print_function, division
import sys
from argparse import ArgumentParser

import pymoduleconnector
from pymoduleconnector import ModuleConnector
from pymoduleconnector.extras.auto import auto
from pymoduleconnector.ids import *


def print_module_info(device_name):
    # Stop running application and set module in manual mode.
    mc = ModuleConnector(device_name)
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
        # Maybe already running at X4driver level
        pass
    xep = mc.get_xep()
    pong = xep.ping()
    print("")
    print("********** XeThru Module Information **********")
    print("")
    print("Received pong= ", hex(pong) + "  connection build!")
    print("FirmWareID = ", xep.get_system_info(XTID_SSIC_FIRMWAREID))
    print("Version = ", xep.get_system_info(XTID_SSIC_VERSION))
    print("Build = ", xep.get_system_info(XTID_SSIC_BUILD))
    print("VersionList = ", xep.get_system_info(XTID_SSIC_VERSIONLIST))

    # Following three item only supported by XeThru Sensor, e.g.X4M200, X4M300. X4M03 does not these information and will feedback error message when read.
    try:
        OrderCode = "X4Mxx"
        OrderCode = xep.get_system_info(XTID_SSIC_ORDERCODE)
        print("OrderCode = ", OrderCode)
        print("ItemNumber = ", xep.get_system_info(XTID_SSIC_ITEMNUMBER))
        print("SerialNumber = ", xep.get_system_info(XTID_SSIC_SERIALNUMBER))
    except:
        # This is not a sensor but a development kit running XEP.
        pass
    # Uncomment following line to enable print X4 setting from XeThru module
    # print_x4_settings(xep)
    mc.close()
    return OrderCode


def print_sensor_settings(xethru_sensor):
    # Check values (to confirm we have the values we want):
    print("")
    print("********** Current sensor settings **********")
    print("")
    print("profile id: " + str(xethru_sensor.get_profileid()))
    print("sensitivity: " + str(xethru_sensor.get_sensitivity()))
    print("led control: " + str(xethru_sensor.get_led_control()))
    print("noise map control: " + str(xethru_sensor.get_noisemap_control()))
    print("tx_center_frequency: " +
          str(xethru_sensor.get_tx_center_frequency()))
    frame_area = xethru_sensor.get_detection_zone()
    print("Detection zone: " + str(frame_area.start) +
          " to " + str(frame_area.end))


def print_x4_settings(xep):
    # Check values (to confirm we have the values we want):
    print("")
    print("********** Current X4 settings **********")
    print("")
    print("downconversion: " + str(xep.x4driver_get_downconversion()))

    frequency = xep.x4driver_get_tx_center_frequency()
    if frequency == 3:
        freqstr = "6.0 to 8.5 Ghz range, centre=7.29GHz"
    else:
        freqstr = "7.2 to 10.2 GHz range, centre=8.4GHz"
    print("tx_center_frequency: " + freqstr)

    txpower = xep.x4driver_get_tx_power()
    if txpower == 0:
        txpowerstr = "OFF"
    elif txpower == 1:
        txpowerstr = "Low"
    elif txpower == 2:
        txpowerstr = "Medium"
    elif txpower == 3:
        txpowerstr = "High"
    print("tx_power: " + txpowerstr)

    prf_div = xep.x4driver_get_prf_div()
    prf_base_freq = 243e6
    prf_freq = prf_base_freq/prf_div
    c = 3e8
    Ramb = c/(2*prf_freq)
    print("Prf-div = " + str(prf_div) + "--> PRF_Freq= " + str(prf_freq /
                                                               1e6) + "MHz giving ambigous range Ramb = " + str(Ramb) + "m")
    print("iterations: " + str(xep.x4driver_get_iterations()))
    print("pulses_per_step: " + str(xep.x4driver_get_pulses_per_step()))
    print("dac_min: " + str(xep.x4driver_get_dac_min()))
    print("dac_max: " + str(xep.x4driver_get_dac_max()))
    #print("prf_div: " + str(xep.x4driver_get_prf_div()))
    print("tx_power: " + str(xep.x4driver_get_tx_power()))
    print("Frame area offset: " + str(xep.x4driver_get_frame_area_offset()))
    frame_area = xep.x4driver_get_frame_area()
    print("Frame Area: " + str(frame_area.start) + " to " + str(frame_area.end))
    print("frameBinCount = " + str(xep.x4driver_get_frame_bin_count()))
    print("fps: " + str(xep.x4driver_get_fps()))


def main():
    parser = ArgumentParser()
    parser.add_argument(
        "-d",
        "--device",
        dest="device_name",
        help="Seral port name used by target XeThru sensor, i.e com8, /dev/ttyACM0",
        metavar="FILE")

    args = parser.parse_args()
    if args.device_name:
        device_name = args.device_name
    else:
        try:
            device_name = auto()[0]
        except:
            print("Fail to find serial port, please specify it by use -d!")
            raise

    print_module_info(device_name)
    """
    # print_sensor_settings for xethru sensor  xethru_sensor
    configuration for sensor setting output
    mc = ModuleConnector(device_name)
    mc.get_xep()
    x4m200 = mc.get_x4m200()
    x4m200.set_sensor_mode(XTS_SM_STOP, 0)
    # need to be change according to sensors
    x4m200.load_profile(XTS_ID_APP_RESPIRATION_2)
    print_sensor_settings(x4m200)
    """


if __name__ == "__main__":
    main()
