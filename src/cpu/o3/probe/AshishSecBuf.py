# Security Buffer Debug Flag

from m5.params import *
from m5.SimObject import SimObject

class AshishSecBuf(SimObject):
    type = 'AshishSecBuf'
    cxx_header = "cpu/o3/probe/ashish_secbuf.hh"
