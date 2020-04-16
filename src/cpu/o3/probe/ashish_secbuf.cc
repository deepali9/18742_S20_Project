// Security Buffer Debug Flag

#include "cpu/o3/probe/ashish_secbuf.hh"

#include <iostream>

AshishSecBuf::AshishSecBuf(AshishSecBufParams *params) :
    SimObject(params)
{
    std::cout << "Hello World! From a SimObject!" << std::endl;
}

AshishSecBuf*
AshishSecBufParams::create()
{
    return new AshishSecBuf(this);
}
