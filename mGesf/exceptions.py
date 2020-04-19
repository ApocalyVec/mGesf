class MGesFError(Exception):
    """Base class for other exceptions"""
    pass


class DataPortNotOpenError(MGesFError):
    def __str__(self):
        return 'attempting to read from unopen data port'
    """Raised when attempting to read from unopen data port"""
    pass


class GeneralMmWError(MGesFError):
    def __str__(self):
        return 'general mmWave error occurred, please debug the tlv buffer and decoder'
    pass


class BufferOverFlowError(MGesFError):
    """Raised when data buffer overflows """
    def __str__(self):
        return 'TLV buffer overflowed'
    pass
