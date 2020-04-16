// Custom Basic debug flag

#include "cpu/o3/probe/ashish_basic.hh"

#include <iostream>

AshishBasic::AshishBasic(AshishBasicParams *params) :
    SimObject(params)
{
    std::cout << "Hello World! From a SimObject!" << std::endl;
}

AshishBasic*
AshishBasicParams::create()
{
    return new AshishBasic(this);
}
