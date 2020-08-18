#!/usr/bin/env python
# -*- coding: utf-8 -*-
""" \example XEP_X4M200_X4M300_access_registers.py

#Target module: X4M200,X4M300,X4M03

#Introduction: This is an example of how to use the XEP interface from python with the regmap class for easy access to chip registers.

#Command to run: "python XEP_X4M200_X4M300_access_registers.py" or "python3 XEP_X4M200_X4M300_access_registers.py"

"""

from __future__ import print_function
from pymoduleconnector import ModuleConnector
from pymoduleconnector.extras.x4_regmap_autogen import X4
from pymoduleconnector.extras.auto import auto
import pymoduleconnector
import sys
import time


def main():
    module = auto('x4')
    mc = ModuleConnector(module[0])
    xep = mc.get_xep()
    regmap = X4(xep)
    # Use regmap object to access registers, for example to set tx_power
    print("Setting TX power to 1")
    regmap.tx_power = 1
    print("TX power is now %d" % regmap.tx_power)
    # In an interactive session use tab-completion to get a list of
    # available registers
    return 0



if __name__ == "__main__":
    sys.exit(main())
