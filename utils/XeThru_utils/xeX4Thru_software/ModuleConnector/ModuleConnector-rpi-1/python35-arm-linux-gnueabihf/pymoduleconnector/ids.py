""" Various IDs used when communicating with modules.
"""

from . import moduleconnectorwrapper as mc_i


__all__ = [
    'XTFILE_TYPE_CRASHDUMP',
    'XTFILE_TYPE_PARFILE',
    'XTFILE_TYPE_NOISEMAP_SLOW',
    'XTFILE_TYPE_NOISEMAP_FAST',
    'XTFILE_TYPE_USER_DEFINED'
]

XTFILE_TYPE_CRASHDUMP      = 0x04554D50
XTFILE_TYPE_PARFILE        = 0x037DD98C
XTFILE_TYPE_NOISEMAP_SLOW  = 0x05F09BCA
XTFILE_TYPE_NOISEMAP_FAST  = 0x05F09BCB
XTFILE_TYPE_USER_DEFINED   = 0x80000000

for name in dir(mc_i):
    if name[:3] == "XTS" or name[:4] == "XTID":
        globals()[name] = getattr(mc_i, name)
        __all__.append(name)
