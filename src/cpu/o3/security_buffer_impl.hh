/*
 * Security Buffer
 */

#ifndef __CPU_O3_SECURITY_BUFFER_IMPL_IMPL_HH__
#define __CPU_O3_SECURITY_BUFFER_IMPL_IMPL_HH__

#include "cpu/o3/security_buffer.hh"
#include "params/DerivO3CPU.hh"
#include "debug/AshishBasic.hh"
#include "debug/AshishSecBuf.hh"

using namespace std;
template <class Impl>
SecBuf<Impl>::SecBuf(O3CPU *_cpu, DerivO3CPUParams *params)
  : cpu(_cpu),
    numThreads(params->numThreads),
    numEntries(params->numSecBufEntries),
    secBufPolicy(params->smtSecBufPolicy)
{
  //figure out the sec buf policy. We are only supporting partitioned policy
  assert(secBufPolicy == SMTQueuePolicy::Partitioned);
  if(secBufPolicy == SMTQueuePolicy::Partitioned) {
    int part_amt = numEntries / numThreads;

    //Divide security buffer evenly
    for (ThreadID tid = 0; tid < numThreads; tid++) {
      maxEntries[tid] = part_amt;
    }
  }

  // reset
  reset();

  DPRINTF(AshishSecBuf, "Security Buffer Constructed with size = %d\n", numEntries);
}

template <class Impl>
void SecBuf<Impl>::reset(){
  DPRINTF(AshishSecBuf, "Reset the security buffer\n");
  for (ThreadID tid = 0; tid < numThreads; tid++) {
    threadEntries[tid] = 0;
  }
  numInstsInSecBuf = 0;
}

#endif
