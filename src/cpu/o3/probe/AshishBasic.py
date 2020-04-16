# Custom Basic debug flag

from m5.params import *
from m5.SimObject import SimObject

class AshishBasic(SimObject):
    type = 'AshishBasic'
    cxx_header = "cpu/o3/probe/ashish_basic.hh"
