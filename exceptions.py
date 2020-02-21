class MGesFError(Exception):
    """Base class for other exceptions"""
    pass


class DataPortNotOpenError(MGesFError):
    """Raised when attempting to read from unopen data port"""
    pass
