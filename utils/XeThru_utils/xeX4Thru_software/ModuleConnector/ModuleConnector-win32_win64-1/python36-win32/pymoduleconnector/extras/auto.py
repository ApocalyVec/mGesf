"""@package pymoduleconnector.extras.auto

Automatically locating modules for ModuleConnector.
"""

import serial.tools.list_ports
import weakref
from contextlib import contextmanager

from .. import moduleconnector


def auto(dev='all'):
    """ Automatically find modules.

    Arguments
    ---------
    `dev` : a comma-separated list of modules to search for, and
    may contain ftdi, bl, bootloader, x4, and all.

    Returns a list of matching devices.
    """
    devs = dev.split(',')
    vidpids = set()
    for dev in devs:
        if dev == 'ftdi' or dev == 'all':
            vidpids |= {(0x0403, 0x6001), (0x0403, 0x6014)}
        if dev == 'bl' or dev == 'bootloader' or dev == 'all':
            vidpids |= {(0x03eb, 0x2404)}
        if dev == 'x4' or dev == 'all':
            vidpids |= {(0x03eb, 0x6124)}
    devs = []
    for p in serial.tools.list_ports.comports():
        if type(p) == tuple:
            for vid, pid in vidpids:
                if hex(vid)[2:].zfill(4) + ":" + hex(pid)[2:].zfill(4) in p[2]:
                    devs.append(p[0])
        elif (p.vid, p.pid) in vidpids:
            devs.append(p.device)
    return devs

def auto_open(dev='all', ix=0, log_level=0):
    """ Open an automatically found module.

    Arguments
    ---------
    dev : string
        Same as for `auto`
    ix : int
        index into the list returned by auto() to open.
    log_level : int
        argument to `ModuleConnector` for how much information to print

    Throws
    ------
    `IndexError` : if the module was not found

    Returns
    -------
    `pymoduleconnector.moduleconnector.ModuleConnector` instance
    """
    devs = auto(dev)
    if len(devs) <= ix:
        raise IndexError("Module not found")
    return moduleconnector.ModuleConnector(devs[ix], log_level=log_level)

@contextmanager
def create_auto(*args, **kwargs):
    """ Create a context managed ModuleConnector instance.

    pymoduleconnector.moduleconnector.create_mc, but for auto.
    """

    mc = auto_open(*args, **kwargs)
    yield weakref.proxy(mc)
    del mc
