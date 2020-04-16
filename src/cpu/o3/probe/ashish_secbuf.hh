// Security Buffer Debug Flag

#ifndef __LEARNING_GEM5_ASHISH_SECBUF_HH__
#define __LEARNING_GEM5_ASHISH_SECBUF_HH__

#include "params/AshishSecBuf.hh"
#include "sim/sim_object.hh"

class AshishSecBuf : public SimObject
{
  public:
    AshishSecBuf(AshishSecBufParams *p);
};

#endif // __LEARNING_GEM5_ASHISH_SECBUF_HH___
