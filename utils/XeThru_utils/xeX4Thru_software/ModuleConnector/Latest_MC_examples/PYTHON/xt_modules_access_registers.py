#!/usr/bin/env python
# -*- coding: utf-8 -*-
""" \example xt_modules_access_registers.py

Latest examples is located at https://github.com/xethru/XeThru_ModuleConnector_Examples or https://dev.azure.com/xethru/XeThruApps/_git/XeThru_ModuleConnector_Examples.

# Target module: 
# XeThru moduelse using X4 chip 

# Introduction: 
# This is an example of how to use the XEP interface from python with the regmap class for easy access to chip registers.

# prerequisite:
# ModuleConnector python lib is installed, check XeThruSensorsIntroduction application note to get detail

# Command to run: 
# "python xt_modules_access_registers.py" or "python3 xt_modules_access_registers.py"

"""

from __future__ import print_function
import sys

from pymoduleconnector import ModuleConnector
from pymoduleconnector.extras.x4_regmap_autogen import X4
from pymoduleconnector.extras.auto import auto


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
