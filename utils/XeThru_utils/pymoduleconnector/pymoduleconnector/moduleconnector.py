"""@package pymoduleconnector.moduleconnector

Entrypoint functionality.
"""
from .moduleconnectorwrapper import \
        PythonModuleConnector, PyDataRecorder, PyDataReader, PyDataPlayer
from .moduleconnectorwrapper import Bootloader as cBootloader

import socket
import struct
import sys

class Bootloader(cBootloader):
    """ Inherits pymoduleconnector.moduleconnectorwrapper.Bootloader
    
    Examples:
        >>> from pymoduleconnector import Bootloader
        >>> boot = Bootloader("/dev/ttyACM0", log_level=9)

    Open an IP:
        >>> boot = Bootloader("tcp://127.0.0.1:3000")
    """
    def __init__(self, device_name=None, log_level=0):
        if sys.version_info.major == 3:
            if isinstance(device_name, type(b"")):
                device_name = device_name.decode("utf8")
        else:
            if isinstance(device_name, type(u"")):
                device_name = device_name.encode("utf8")
        if not device_name:
            super(Bootloader, self).__init__(log_level)
        elif device_name[:6] == "tcp://":
            dev = device_name[6:].split(":")
            ip = struct.unpack('=L', socket.inet_aton(dev[0]))[0]
            port = socket.htons(int(dev[1]))
            super(Bootloader, self).__init__(ip, port, log_level)
        else:
            super(Bootloader, self).__init__(device_name, log_level)


class ModuleConnector(PythonModuleConnector):
    """ Inherits pymoduleconnector.moduleconnectorwrapper.PythonModuleConnector
    
    @see create_mc

    Examples:
        >>> from pymoduleconnector import ModuleConnector
        >>> mc = ModuleConnector("/dev/ttyACM0", log_level=9)
        >>> x2m200 = mc.get_x2m200()
        >>> print(hex(x2m200.ping()))
        0xaaeeaeeaL

    Open an IP:
        >>> mc = ModuleConnector("tcp://127.0.0.1:3000")
    """
    def __init__(self, device_name=None, log_level=0):
        if sys.version_info.major == 3:
            if isinstance(device_name, type(b"")):
                device_name = device_name.decode("utf8")
        else:
            if isinstance(device_name, type(u"")):
                device_name = device_name.encode("utf8")
        if isinstance(device_name, DataPlayer):
            super(ModuleConnector, self).__init__(device_name, log_level)
        elif not device_name:
            super(ModuleConnector, self).__init__(log_level)
        elif device_name[:6] == "tcp://":
            dev = device_name[6:].split(":")
            ip = struct.unpack('=L', socket.inet_aton(dev[0]))[0]
            port = socket.htons(int(dev[1]))
            super(ModuleConnector, self).__init__(ip, port, log_level)
        elif device_name[:6] == "ish://":            
            super(ModuleConnector, self).__init__("ISH", log_level)            
        else:
            super(ModuleConnector, self).__init__(device_name, log_level)

    # Workaround for scoping issue: if mc object goes out of scope while
    # there's still a reference to the interface, we get dangling pointers.
    def get_x4m200(self):
        x = super(ModuleConnector, self).get_x4m200()
        x._parent = self
        return x

    def get_x4m300(self):
        x = super(ModuleConnector, self).get_x4m300()
        x._parent = self
        return x

    def get_xep(self):
        x = super(ModuleConnector, self).get_xep()
        x._parent = self
        return x

    def get_x2m200(self):
        x = super(ModuleConnector, self).get_x2m200()
        x._parent = self
        return x

class DataRecorder(PyDataRecorder):
    """ Inherits pymoduleconnector.moduleconnectorwrapper.PyDataRecorder
    """
    pass

class DataReader(PyDataReader):
    """ Inherits pymoduleconnector.moduleconnectorwrapper.PyDataReader
    """
    pass

class DataPlayer(PyDataPlayer):
    """ Inherits pymoduleconnector.moduleconnectorwrapper.PyDataPlayer
    """
    pass

from contextlib import contextmanager
import weakref

@contextmanager
def create_mc(*args, **kwargs):
    """Initiate a context managed ModuleConnector object.

    Convenience function to get a context managed ModuleConnector object.

    All references to the object is deleted, thus the serial port connection is
    closed.

    Examples:
        >>> from pymoduleconnector import create_mc
        >>> with create_mc('com11') as mc:
        >>>     print(hex(mc.get_x2m200().ping()))
        0xaaeeaeeaL
    """
    mc = ModuleConnector(*args, **kwargs)
    yield weakref.proxy(mc)
    del mc
