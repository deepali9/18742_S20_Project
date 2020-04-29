/*
 * Security Buffer
 */

#ifndef __CPU_O3_SECURITY_BUFFER_HH__
#define __CPU_O3_SECURITY_BUFFER_HH__

#include "cpu/inst_seq.hh"
#include "debug/AshishBasic.hh"
#include "debug/AshishSecBuf.hh"
#include "enums/SMTQueuePolicy.hh"

//ASHISH_NEW
struct DerivO3CPUParams;
//ASHISH_NEW

template<class Impl>
class SecBuf
{
  public:
    //Typedefs from the Impl.
    typedef typename Impl::O3CPU O3CPU;

    //if security buffer is full.
    bool isFull();

    /** Returns if a specific thread's partition is full. */
    bool isFull(ThreadID tid);

    /** Function called by lsq to add an entry to te security buffer*/
    void addEntry(InstSeqNum seqNum, Addr addr, uint8_t *data, ThreadID tid);

    /** Function called by lsq to add an entry to te security buffer*/
    void updateEntry(InstSeqNum seqNum, uint8_t *data, ThreadID tid);

    /**This command just mark the instruction as to be filled**/
    void commitEntry(InstSeqNum seqNum, ThreadID tid);

    /** invalidate entry */
    void squashEntry(InstSeqNum seqNum, ThreadID tid);

    /** Allows it invalidate all entries of secuirty buffer in one go */
    void flushBuffer(ThreadID tid);

    /**Check if there are no instructions to be filled**/
    /**Has to be calculated from current pointer till the end**/
    bool isFillEmpty();

    /**Invalidate the following instruction after their fill request
        has been successfully sent to the memory**/
    void invalidateSuccessfulFills(InstSeqNum seqNum);

    /**Get the next fill element from the memory**/
    bool getFillEntry(InstSeqNum *secBufseqNum, Addr *secBufAddr,
         uint8_t *secBufData, ThreadID *secBufTid);

    /**Reset the pointer to the started of the security
        buffer to check for fill again **/
    void resetFillPointer();

    /**Increment Security Buffer Pointer**/
    void incrementSecBufPointer();

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
      Addr address;
      uint8_t *data;
      bool fill;
      ThreadID tid;
    } secBufElement;

    /** All the entries. Space allocated at run time*/
    std::vector<secBufElement> security_buf;

    /**Pointer is which index of the secuirty
        buffer is currently being accesed*/
    // ASHISH_QUESTION: why is pointer unsigned type
    unsigned secBufPointer;

    /** SMT resource sharing policy for security buffer*/
    SMTQueuePolicy secBufPolicy;

  public:
    /** Constructor */
    SecBuf(O3CPU *_cpu, DerivO3CPUParams *params);

    /** Reset function*/
    void reset();
};

#endif //__CPU_O3_SECURITY_BUFFER_HH__
