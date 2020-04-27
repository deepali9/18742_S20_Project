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
//Constructor
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

  DPRINTF(AshishBasic, "SecBuf : Security Buffer Constructed with size"
         "= %d\n", numEntries);
}

//Initialize with empty/reset valued entries
template <class Impl>
void SecBuf<Impl>::reset(){
  DPRINTF(AshishBasic, "SecBuf : Reset the security buffer\n");
  for (ThreadID tid = 0; tid < numThreads; tid++) {
    threadEntries[tid] = 0;
    secBufPointer = 0;
  }
  numInstsInSecBuf = 0;
}

//if security buffer is full.
template <class Impl>
bool SecBuf<Impl>::isFull()
{   DPRINTF(AshishBasic, "SecBuf : Security Buffer of size = %d"
        "has %d entries = %d\n", numEntries, numInstsInSecBuf);
    return numInstsInSecBuf == numEntries; }

/** Returns if a specific thread's partition is full. */
template <class Impl>
bool SecBuf<Impl>::isFull(ThreadID tid)
{    DPRINTF(AshishBasic, "SecBuf : Security Buffer of size = %d"
        "has %d entries\n", maxEntries[tid], threadEntries[tid]);
     return threadEntries[tid] == maxEntries[tid]; }

/** Function called by lsq to add an entry to te security
     buffer - as soon as there is a load issued to the cache*/
template <class Impl>
void SecBuf<Impl>::addEntry(InstSeqNum seqNum, Addr addr,
     uint8_t *data, ThreadID tid) {
    secBufElement secbufel;
    secbufel.seqNum = seqNum;
    secbufel.address = addr;
    secbufel.valid = false;
    secbufel.fill = false;
    secbufel.data = data;
    secbufel.tid = tid;
    security_buf.push_back(secbufel);
    threadEntries[tid] = threadEntries[tid] + 1;
    numInstsInSecBuf = numInstsInSecBuf + 1;
    DPRINTF(AshishBasic, "SecBuf : Element added [sn:%lli]\n", seqNum);
}

/** Function called by lsq to add an entry to te security
     buffer - as soon as there is a load issued to the cache*/
template <class Impl>
void SecBuf<Impl>::updateEntry(InstSeqNum seqNum,
     uint8_t *data, ThreadID tid) {
    for (int i = 0; i < numInstsInSecBuf; i++) {
        if (security_buf[i].seqNum == seqNum &&
             security_buf[i].tid == tid) {
            security_buf[i].valid = true;
            security_buf[i].data = data;
            DPRINTF(AshishBasic, "SecBuf : Element updated"
                 "[sn:%lli]\n", seqNum);
        }
    }
}

/**This command just mark the instruction as to be filled**/
template <class Impl>
void SecBuf<Impl>::commitEntry(InstSeqNum seqNum, ThreadID tid) {
    bool found=false;
    for (int i = 0; i < numInstsInSecBuf; i++) {
        if (security_buf[i].seqNum == seqNum &&
            security_buf[i].tid == tid) {
            security_buf[i].fill = true;
            found = true;
            DPRINTF(AshishBasic, "SecBuf : Element committed"
                 "[sn:%lli]\n", seqNum);
            break;
        }
    }

    if (!found) {
         DPRINTF(AshishBasic, "SecBuf : Element not found"
            " to commit [sn:%lli]\n", seqNum);
    }
}

/** invalidate entry */
template <class Impl>
void SecBuf<Impl>::squashEntry(InstSeqNum seqNum, ThreadID tid) {
    int match_indx;
    bool found=false;
    for (int i = 0; i < numInstsInSecBuf; i++) {
        if (security_buf[i].seqNum == seqNum &&
            security_buf[i].tid == tid) {
            security_buf[i].valid = false;
            found = true;
            match_indx = i;
            break;
        }
    }

    if (found) {
        security_buf.erase (security_buf.begin()+match_indx);
        DPRINTF(AshishBasic, "SecBuf : Element squashed"
            "[sn:%lli]\n", seqNum);
        numInstsInSecBuf = numInstsInSecBuf - 1;
        threadEntries[tid] = threadEntries[tid] - 1;
    }
    else {
         DPRINTF(AshishBasic, "SecBuf : Element not found"
            " to squash [sn:%lli]\n", seqNum);
    }
}

/** Allows it invalidate all entries of secuirty buffer in one go */
template <class Impl>
void SecBuf<Impl>::flushBuffer(ThreadID tid) {
    for (int i = 0; i < numInstsInSecBuf; i++) {
        if (security_buf[i].tid == tid) {
            security_buf.erase (security_buf.begin()+i);
            numInstsInSecBuf = numInstsInSecBuf - 1;
            threadEntries[tid] = threadEntries[tid] - 1;
        }
    }
}

/**Check if there are no instructions to be filled**/
/**Has to be calculated from current secBufPointer till the end**/
template <class Impl>
bool SecBuf<Impl>::isFillEmpty() {
    bool found=false;
    for (int i = secBufPointer; i < numInstsInSecBuf; i++) {
        if (security_buf[i].fill) {
            found = true;
            break;
        }
    }
    if (!found)
        return true;
    else
        return false;
}

/**Invalidate the following instruction after their fill
     request has been successfully sent to the memory**/
template <class Impl>
void SecBuf<Impl>::invalidateSuccessfulFills(InstSeqNum seqNum) {
    int match_indx;
    bool found=false;
    for (int i = 0; i < numInstsInSecBuf; i++) {
        if (security_buf[i].seqNum == seqNum &&
             security_buf[i].fill && security_buf[i].valid) {
            security_buf[i].valid = false;
            found = true;
            match_indx = i;
            break;
        }
    }

    if (found) {
        security_buf.erase (security_buf.begin()+match_indx);
        DPRINTF(AshishBasic, "SecBuf : Element invalidated post"
             "fill [sn:%lli]\n", seqNum);
        numInstsInSecBuf = numInstsInSecBuf - 1;
        threadEntries[security_buf[match_indx].tid] =
            threadEntries[security_buf[match_indx].tid] - 1;
    }
    else {
         DPRINTF(AshishBasic, "SecBuf : Element not found"
            "to invalidate post fill [sn:%lli]\n", seqNum);
    }
}

/**Get the next fill element from the memory**/
template <class Impl>
bool SecBuf<Impl>::getFillEntry(InstSeqNum *secBufseqNum,
    Addr *secBufAddr, uint8_t *secBufData, ThreadID *secBufTid) {
    int match_indx;
    bool found=false;
    for (int i = secBufPointer; i < numInstsInSecBuf; i++) {
        if (security_buf[i].fill && security_buf[i].valid) {
            match_indx = i;
            *secBufseqNum = security_buf[match_indx].seqNum;
            *secBufAddr = security_buf[match_indx].address;
            *secBufData = *(security_buf[match_indx].data);
            *secBufTid = security_buf[match_indx].tid;
            DPRINTF(AshishBasic, "SecBuf : Element to be issues as"
                "fill [sn:%lli]\n", security_buf[match_indx].seqNum);
            found = true;
            break;
        }
    }
    if (!found) {
        DPRINTF(AshishBasic, "SecBuf : No element found to be filled\n");
    }
    return found;
}

/**Reset the pointer to the started of the security
    buffer to check for fill again **/
template <class Impl>
void SecBuf<Impl>::resetFillPointer() {
    secBufPointer = 0;
}

/**Increment Security Buffer Pointer**/
template <class Impl>
void SecBuf<Impl>::incrementSecBufPointer() {
    if (secBufPointer<numInstsInSecBuf-1)
        secBufPointer = secBufPointer + 1;
    else
        secBufPointer = numInstsInSecBuf-1;
}
#endif
