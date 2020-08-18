#!/usr/bin/env python
# -*- coding: utf-8 -*-
""" \example X4M200_X4M300_manipulate_noisemap.py

#Target module: X4M200,X4M300

#Introduction:  This is an example of how to use noisemap on X4M200 and X4M300 modules.
			   
#Command to run:  "python X4M200_X4M300_manipulate_noisemap.py -d com8 -i x4m300" or "python3 X4M200_X4M300_manipulate_noisemap.py -d com8 i x4m300"
                 Using TCP server address as device name is also supported, e.g. 
                 "python X4M200_sleep_record.py -d tcp://192.168.1.169:3000".

"""

from __future__ import print_function

import time
from optparse import OptionParser
import sys

import pymoduleconnector
from pymoduleconnector.ids import *


parser = OptionParser()
parser.add_option(
    "-d",
    "--device",
    dest="device_name",
    help="device file to use, example: python %s -d COM4" % sys.argv[0],
    metavar="FILE")

parser.add_option(
    "-i",
    "--interface",
    dest="interface",
    default="x4m300",
    help="Interface to use. x4m300 or x4m200",
    metavar="IF")


(options, args) = parser.parse_args()

mc = pymoduleconnector.ModuleConnector(options.device_name, 0)
# Get the XEP interface
xep = mc.get_xep()

def ls():
    files = xep.find_all_files()
    return set(zip(files.file_type_items, files.file_identifier_items))

# Get the app interface and set profile
if options.interface == 'x4m300':
    app = mc.get_x4m300()
    profile = XTS_ID_APP_PRESENCE_2
elif options.interface == 'x4m200':
    app = mc.get_x4m200()
    profile = XTS_ID_APP_RESPIRATION_2
else:
    raise SystemExit("Interface not recognized.")

# Load a profile. Noisemap controls are stored associated with this profile,
# meaning setting/getting it without loading a profile doesn't work.
app.load_profile(profile)

# Set a detection zone. The noisemap requires a matching detection zone.
app.set_detection_zone(1.0, 3.0)

# Set noisemap controls
ctrl = app.get_noisemap_control()
# Enable use of noisemap
ctrl |=  XTID_NOISEMAP_CONTROL_ENABLE
# Enable adaptive noisemap
ctrl |=  XTID_NOISEMAP_CONTROL_ADAPTIVE
# Don't reinitialize noisemap if stored, and store it after
# initialization if not present.
ctrl &= ~XTID_NOISEMAP_CONTROL_INIT_ON_RESET
app.set_noisemap_control(ctrl)

files = ls()

if (XTFILE_TYPE_NOISEMAP_FAST, 0) in files:
    xep.delete_file(XTFILE_TYPE_NOISEMAP_FAST, 0)
if (XTFILE_TYPE_NOISEMAP_SLOW, 0) in files:
    xep.delete_file(XTFILE_TYPE_NOISEMAP_SLOW, 0)

print('Initializing a noisemap. Vacate the premises.Please wait for ~2min.')
time.sleep(5)
status = app.set_sensor_mode(XTID_SM_RUN, 0)
time.sleep(130)
status = app.set_sensor_mode(XTID_SM_STOP, 0)
print('Done.')
time.sleep(1)

files = ls()

if (XTFILE_TYPE_NOISEMAP_FAST, 0) not in files:
    raise SystemExit("Did not store a fast noisemap")
if (XTFILE_TYPE_NOISEMAP_SLOW, 0) not in files:
    raise SystemExit("Did not store a slow noisemap")

nm_fast = xep.get_file(XTFILE_TYPE_NOISEMAP_FAST, 0)
nm_slow = xep.get_file(XTFILE_TYPE_NOISEMAP_SLOW, 0)

status = app.load_profile(profile)
status = app.set_detection_zone(1, 2)
xep.delete_file(XTFILE_TYPE_NOISEMAP_FAST, 0)
xep.delete_file(XTFILE_TYPE_NOISEMAP_SLOW, 0)

print('Initializing another one.Please wait for 130s')
app.set_sensor_mode(XTID_SM_RUN, 0)
time.sleep(130)
app.set_sensor_mode(XTID_SM_STOP, 0)
print('Done.')
time.sleep(1)

files = ls()

if (XTFILE_TYPE_NOISEMAP_FAST, 0) not in files:
    raise SystemExit("Did not store a fast noisemap")
if (XTFILE_TYPE_NOISEMAP_SLOW, 0) not in files:
    raise SystemExit("Did not store a slow noisemap")

short_nm_fast = xep.get_file(XTFILE_TYPE_NOISEMAP_FAST, 0)
short_nm_slow = xep.get_file(XTFILE_TYPE_NOISEMAP_SLOW, 0)

xep.delete_file(XTFILE_TYPE_NOISEMAP_FAST, 0)
xep.delete_file(XTFILE_TYPE_NOISEMAP_SLOW, 0)

# Once more, with a restored previous noisemap.

# Upload noisemap to module flash.
xep.set_file(XTFILE_TYPE_NOISEMAP_FAST, 0, nm_fast)
xep.set_file(XTFILE_TYPE_NOISEMAP_SLOW, 0, nm_slow)
time.sleep(1)

print('Starting profile with the first noisemap.')
app.load_profile(profile)

# This must match the detection zone the noisemap was made with for the
# restoring to work.
app.set_detection_zone(1.0, 3.0)

print('Initialization should now only be around 20 seconds.')
app.set_sensor_mode(XTID_SM_RUN, 0)
time.sleep(20)
print('Initialization done.')

print('Getting a list of noisemaps as they evolve over time.')

nms_fast = []
nms_slow = []
for ix in range(5):
    time.sleep(50)
    app.store_noisemap()
    # Store noisemap takes around a second to complete, pluss a varying
    # amount of time for the flash writing to catch up.
    time.sleep(10)
    nms_fast.append(xep.get_file(XTFILE_TYPE_NOISEMAP_FAST, 0))
    nms_slow.append(xep.get_file(XTFILE_TYPE_NOISEMAP_SLOW, 0))

app.set_sensor_mode(XTID_SM_STOP, 0)
