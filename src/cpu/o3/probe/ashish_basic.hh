// Custom Basic debug flag

#ifndef __LEARNING_GEM5_ASHISH_BASIC_HH__
#define __LEARNING_GEM5_ASHISH_BASIC_HH__

#include "params/AshishBasic.hh"
#include "sim/sim_object.hh"

class AshishBasic : public SimObject
{
  public:
    AshishBasic(AshishBasicParams *p);
};

#endif // __LEARNING_GEM5_ASHISH_BASIC_HH__
