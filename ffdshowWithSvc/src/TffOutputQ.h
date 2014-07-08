#ifndef _TFFOUTPUTQ_H_
#define _TFFOUTPUTQ_H_

// derived from Microsoft baseclass file outputq.h
// modified code is by h.yamagata

class TffdshowDecVideoOutputPin;
class TffOutputQueue : public COutputQueue
{
public:
    //  Constructor
    TffOutputQueue(TffdshowDecVideoOutputPin *pOut,
                 IPin      *pInputPin,          //  Pin to send stuff to
                 HRESULT   *phr,                //  'Return code'
                 BOOL       bAuto = TRUE,       //  Ask pin if blocks
                 BOOL       bQueue = TRUE,      //  Send through queue (ignored if
                                                //  bAuto set)
                 LONG       lBatchSize = 1,     //  Batch
                 BOOL       bBatchExact = FALSE,//  Batch exactly to BatchSize
                 LONG       lListSize =         //  Likely number in the list
                                DEFAULTCACHE,
                 DWORD      dwPriority =        //  Priority of thread to create
                                THREAD_PRIORITY_NORMAL,
                 bool       bFlushingOpt = false // flushing optimization
                );
    virtual ~TffOutputQueue(){};

    // re-enable receives (pass this downstream)
    virtual void EndFlush();        // Complete flush of samples - downstream
                            // pin guaranteed not to block at this stage

    // do something with these media samples
    virtual HRESULT ReceiveMultiple (
        IMediaSample **pSamples,
        long nSamples,
        long *nSamplesProcessed);

    virtual void Reset();           // Reset m_hr ready for more data

    // Return handle of the worker thread
    HANDLE GetWorkerThread(void){return m_hThread;}

    // Return the numbers of objects in the list
    int GetCount(void){return m_List->GetCount();}

protected:
    virtual DWORD ThreadProc();

    //  The critical section MUST be held when this is called
    virtual void QueueSample(IMediaSample *pSample);

    //  Remove and Release() batched and queued samples
    virtual void FreeSamples();

    TffdshowDecVideoOutputPin* m_pOutputDecVideo;
};

#endif