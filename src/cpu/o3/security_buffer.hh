/*
 * Security Buffer
 */

#ifndef __CPU_O3_SECURITY_BUFFER_HH__
#define __CPU_O3_SECURITY_BUFFER_HH__

// Instruction seqnum define
#include "cpu/inst_seq.hh"
// SMT queuing policy
#include "enums/SMTQueuePolicy.hh"
// debug flags
#include "debug/AshishBasic.hh"
#include "debug/AshishSecBuf.hh"

template<class Impl>
class SecBuf
{
  public:
    //Typedefs from the Impl.
    typedef typename Impl::O3CPU O3CPU;

  private:
    /** pointer to the cpu */
    O3CPU *cpu;

    /** Number of active threads. */
    ThreadID numThreads;

    /** Number of instructions in the security buffer. */
    int numInstsInSecBuf;

    /** Number of entries in the Security Buffer. */
    unsigned numEntries;

    /** Entries Per Thread */
    unsigned threadEntries[Impl::MaxThreads];

    /** Max Insts a Thread Can Have in the ROB */
    unsigned maxEntries[Impl::MaxThreads];

    /** One entry of security buffer */
    typedef struct{
      InstSeqNum seqNum;    //sequence number of the entry instruction
      bool valid;           //if the entry is valid or not
      // addr_ address;
      // data_ data;
      // DEEPALI to add
      // All the entries from a packet, that are required to push this data back
      // into the cache once it is marked non-speculative should be element of
      // this struct
    } secBufElement;

    // std::vector<InstSeqNum, secBufElement> my_buf;

    /** All the entries. Space allocated at run time*/

    /** SMT resource sharing policy for security buffer*/
    SMTQueuePolicy secBufPolicy;

  public:
    /** Constructor */
    SecBuf(O3CPU *_cpu, DerivO3CPUParams *params);

    /** Reset function*/
    void reset();
};

#endif //__CPU_O3_SECURITY_BUFFER_HH__
