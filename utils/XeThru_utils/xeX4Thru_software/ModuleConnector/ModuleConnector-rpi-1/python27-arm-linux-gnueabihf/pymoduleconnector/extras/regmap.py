""" Generic classes, designed to be overloaded by
    classes describing the actual registers
"""
import logging
import configobj
import contextlib

class RegmapError(Exception):
  """ Generic register map error
  """

  def __init__(self, value):
      self.value = value
  def __str__(self):
      return repr(self.value)

class Reg(object):
  """ Generic register class.

  This class is intended to be sub-classed for each
  specific register. The sub-classing enables us to use
  class attributes for each register segment without
  the different registers interfering with each other.

  The register maintains a shadow copy of the last known value. This should
  be used with care since it could easily become out of sync with the chip, for
  instance in status registers.
  """

  @classmethod
  def read(cls,map):
    """ Read register value from chip and update shadow value

    Return the new register value
    """
#    if self.__use_shadow__:
#      self.logger.warning("Using shadow register value instead of actually reading from chip because __use_shadow__ is true")
#      return self.__value__
#    else:
      #val = self.__get_register__(self.address)
    val = map.get_register(cls.address)
    #self.__value__ = val
    return val

  @classmethod
  def write(cls, map, start, stop, size, value):
    """ Update register value on chip based on new segment value
    """
#    if self.__use_shadow__:
#      self.logger.warning("Using shadow register value instead of actually reading from chip because __use_shadow__ is true")
#      regval = self.__value__
#    else:
    if not cls.action:
      regval = cls.read(map)
    else:
      regval = 0x00

    shifted = value << stop
    mask = (0xFF>>(8-size))<<stop
    cls.logger.debug("Mask: %s" % bin(mask))
    cls.logger.debug("Reg was: %s" % regval)
    regval = (regval & ~mask) | ( value << stop)
    cls.logger.debug("Reg is now: %s" % regval)
    map.set_register(cls.address, regval)



class RegSegment(object):
  """ Register segment class.

  Represents a register segment.
  """

  def __init__(self, reg, start, stop, name, readonly,action = False):
    """ Initialize a new RegSegment object.

    The segment is defined as segment<start:stop> so start >= stop.

    Arguments:
    reg       -- The register object the segment belongs to
    start     -- The uppermost bit of the segment
    stop      -- The lowest bit of the segment
    name      -- The name of the segment
    readonly  -- True if the segment is readonly
    """
    self.start      = start
    self.stop       = stop
    self.size       = 1 + self.start - self.stop
    self.reg        = reg
    self.name       = name
    self.readonly   = readonly
    self.action     = action
    self.logger     = logging.getLogger("%s.%s.%s.%s" % (self.reg.map.__block__.__class__.__name__,self.reg.map.__name__,reg.name,name))
    self.__use_shadow__ = False

  def __set__(self, instance, value):
    """
    """
    if self.readonly:
      raise RegmapError("Attempted to write to a readonly segment")
    else:
      if value < 2**(self.size):
        self.logger.debug("Setting segment value to %s" % value)
        self.reg.write(self, value)
      else:
        raise RegmapError("Attempted to set a %s bit register segment to %s." % (self.size, value))

  def __get__(self, instance, owner):
    """
    """
    if self.action:
      raise RegmapError("Attempted to read to a action segment")
    regval = self.reg.read()
    val = (regval >> self.stop) & (0xFF>>(8-self.size))
    self.logger.debug("Extracted segment value %s from register value: %s" % (bin(val), bin(regval)))

    return val

class RegMap(object):
  def __init__(self, block, name):
    """ Create a new RegMap object.

    The RegMap class is intended to be sub-classed for a specific
    register map on a specific chip. One register map belongs
    to one register block. The subclass must define or assign
    the __set_register__ and __get_register__ to a function that knows
    how to access the physical chip registers.

    Arguments:
    block -- A pointer to the parent class
    name  -- The name of this object.
    """
    with self._addattr():
        self.__block__ = block
        self.__name__  = name
        self.__set_register__ = None
        self.__get_register__ = None
        self.__use_shadow__ = False
        self.__shadow__ = {}
        self.__keys__ = []
        self.logger = None

  def __add_reg__(self, reg_class):
    """ Add register to local shadow
    """
    # Store the register in a local shadow dict
    self.__shadow__[reg_class.address] = reg_class.default

  def set_register(self, address, value):
    """ Set a register value in the register map.

    Uses the __set_register__ function to update the chip register.
    Also updates the local shadow register
    """
    self.__shadow__[address] = value
    self.__set_register__(address, value)

  def get_register(self, address):
    """ Get a register value from the register map.

    If __use_shadow__ is true, only reads from local shadow buffer.
    """
    if self.__use_shadow__:
      self.logger.warning("Using shadow register value instead of actually reading from chip because self.__use_shadow__ is true")
      return self.__shadow__[address]
    else:
      return self.__get_register__(address)

  def __refresh_shadow__(self):
    """ Refresh local shadow from chip
    """
    for key in self.__shadow__:
        self.__shadow__[key] = self.__get_register__(key)

  def __iter__(self):
    """ Enable iteration through registers
    """
    for x in self.__keys__:
      yield x

  def __getitem__(self, seg):
    """ Emulate dict behavior

    Gives access to register segments as regmap[segment].
    """
    return getattr(self, seg)

  def __setitem__(self, seg, value):
    """ Emulate dict behavior

    Gives access to register segments as regmap[segment].
    """
    setattr(self, seg, value)


  # Prevent unintentional addition of attributes.
  # Use "with self._addattr():" to add attributes.
  _allow_new_attributes = False
  @contextlib.contextmanager
  def _addattr(self):
    self._allow_new_attributes = True
    try:
      yield
    finally:
      self._allow_new_attributes = False

  def __setattr__(self, name, value):
    attrexists = name in self.__dict__
    c = self.__class__
    while c:
      attrexists = attrexists or name in c.__dict__
      c = c.__base__
    if attrexists or self._allow_new_attributes:
      object.__setattr__(self, name, value)
    else:
      raise RegmapError('Unknown attribute %r' % name)


class RegBlock(object):
  """ The RegBlock class encapsulates one or more RegMap objects (register maps).

  This class is intended to be sub-classed for specific chips or chip versions. The
  sub-class will overload the __init__ function and call the __add_map__ function
  for each register map that is part of the register block.
  """

  def __init__(self, radar, configfile_section_name = "registers"):
    """ Create a new RegBlock object. """
    with self._addattr():
      self.__radar__ = radar
      self.__maps__ = {}
      self.__keys__ = []
      self.__config_file_section__ = configfile_section_name

  def set_use_shadow(self,use_shadow=False):
    """ Set use_shadow on each register """
    for map in self.__maps__.values():
      map.__use_shadow__ = use_shadow

  def refresh_shadow(self):
    """ Refresh local shadow of all maps
    """
    for map in self.__maps__.values():
       map.__refresh_shadow__()

  def load_config(self, configfile=None):
    """ Load register configuration from an ini-file.

    """
    config = configobj.ConfigObj(configfile)
    try:
        regs = config[self.__config_file_section__]
    except KeyError:
        raise RegmapError("Expected to find a section in the config file called %s" % self.__config_file_section__)

    for key in regs:
        self[key] = regs.as_int(key)


  def write_config(self, configfile="config.ini"):
    """ Write current register configuration to an ini-file.

    """
    #config = configobj.ConfigObj(configfile)
    #config[self.__config_file_section__] = {}

    # Sort list so order of regs is deterministic
    sorted_list = list(self)
    sorted_list.sort()

    # Write file manually to ensure that the order is maintained
    # (configobj stores things in a dict internally which has no guaranteed order)
    s = "[%s]\n" % self.__config_file_section__
    for seg in sorted_list:
      s += "%s=%s\n" %(seg, self[seg])
      #config[self.__config_file_section__][seg] = self[seg]

    with open(configfile,'w') as f:
        f.write(s)

  def __add_map__(self, map):
    """ Add a register map to the register block.

    The provided RegMap object is added to the register block.

    Arguments:
    map  -- The RegMap object to be added
    """
    # Add the map to the local dict so we can keep track of it
    self.__maps__[map.__name__] = map
    self.__keys__.extend(map.__keys__)
    # Add it as a class attribute so we can use tab-completion
    #setattr(self.__class__, map.__name__, map)

    # Finally add each segment in each register in the map
    # as a class attribute
#    for reg in map.__regs__.values():
#      for seg in reg.__segments__.values():
#        setattr(self.__class__, seg.name, seg)


  def __iter__(self):
    """ Enable iteration through registers
    """
    for x in self.__keys__:
      yield x

  def __getitem__(self, seg):
    """ Emulate dict behavior

    Gives access to register segments as regblock[segment].
    """
    if isinstance(seg, type("")) or isinstance(seg, type(u"")):
        return getattr(self, seg)
    else:
        raise TypeError("Expected a string")

  def __setitem__(self, seg, value):
    """ Emulate dict behavior

    Gives access to register segments as regblock[segment].
    """
    if isinstance(seg, type("")) or isinstance(seg, type(u"")):
        setattr(self, seg, value)
    else:
        raise TypeError("Expected a string")

  # Prevent unintentional addition of attributes.
  # Use "with self._addattr():" to add attributes.
  _allow_new_attributes = False
  @contextlib.contextmanager
  def _addattr(self):
    self._allow_new_attributes = True
    try:
      yield
    finally:
      self._allow_new_attributes = False

  def __setattr__(self, name, value):
    attrexists = name in self.__dict__
    c = self.__class__
    while c:
      attrexists = attrexists or name in c.__dict__
      c = c.__base__
    if attrexists or self._allow_new_attributes:
      object.__setattr__(self, name, value)
    else:
      raise RegmapError('Unknown attribute %r' % name)
