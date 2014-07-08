#include "stdafx.h"
#include <streams.h>
#include "ffdebug.h"
#include "TffOutputQ.h"
#include "TffdshowDecVideoOutputPin.h"

// derived from Microsoft baseclass file outputq.cpp
// modified code is by h.yamagata

//
//  COutputQueue Constructor :
//
//  Determines if a thread is to be created and creates resources
//
//     pInputPin  - the downstream input pin we're queueing samples to
//
//     phr        - changed to a failure code if this function fails
//                  (otherwise unchanges)
//
//     bAuto      - Ask pInputPin if it can block in Receive by calling
//                  its ReceiveCanBlock method and create a thread if
//                  it can block, otherwise not.
//
//     bQueue     - if bAuto == FALSE then we create a thread if and only
//                  if bQueue == TRUE
//
//     lBatchSize - work in batches of lBatchSize
//
//     bBatchEact - Use exact batch sizes so don't send until the
//                  batch is full or SendAnyway() is called
//
//     lListSize  - If we create a thread make the list of samples queued
//                  to the thread have this size cache
//
//     dwPriority - If we create a thread set its priority to this
//
TffOutputQueue::TffOutputQueue(
             TffdshowDecVideoOutputPin *pOut,
             IPin         *pInputPin,          //  Pin to send stuff to
             HRESULT      *phr,                //  'Return code'
             BOOL          bAuto,              //  Ask pin if queue or not
             BOOL          bQueue,             //  Send through queue
             LONG          lBatchSize,         //  Batch
             BOOL          bBatchExact,        //  Batch exactly to BatchSize
             LONG          lListSize,
             DWORD         dwPriority,
             bool          bFlushingOpt        // flushing optimization
            ) :  COutputQueue(pInputPin, phr, bAuto, bQueue, lBatchSize, bBatchExact, lListSize, dwPriority, bFlushingOpt),
                 m_pOutputDecVideo(pOut)
{
 DPRINTF(_l("TffOutputQueue::Constructor"));
}

//
//  Thread sending the samples downstream :
//
//  When there is nothing to do the thread sets m_lWaiting (while
//  holding the critical section) and then waits for m_hSem to be
//  set (not holding the critical section)
//
DWORD TffOutputQueue::ThreadProc()
{
    while (TRUE) {
        BOOL          bWait = FALSE;
        IMediaSample *pSample;
        LONG          lNumberToSend; // Local copy
        NewSegmentPacket* ppacket;

        //
        //  Get a batch of samples and send it if possible
        //  In any case exit the loop if there is a control action
        //  requested
        //
        {
            CAutoLock lck(this);
            while (TRUE) {

                if (m_bTerminate) {
                    FreeSamples();
                    return 0;
                }
                if (m_bFlushing) {
                    FreeSamples();
                    SetEvent(m_evFlushComplete);
                }

                //  Get a sample off the list

                pSample = m_List->RemoveHead();
		// inform derived class we took something off the queue
		if (m_hEventPop) {
                    //DbgLog((LOG_TRACE,3,TEXT("Queue: Delivered  SET EVENT")));
		    SetEvent(m_hEventPop);
		}

                if (pSample != NULL &&
                    !IsSpecialSample(pSample)) {

                    //  If its just a regular sample just add it to the batch
                    //  and exit the loop if the batch is full

                    m_ppSamples[m_nBatched++] = pSample;
                    if (m_nBatched == m_lBatchSize) {
                        break;
                    }
                } else {

                    //  If there was nothing in the queue and there's nothing
                    //  to send (either because there's nothing or the batch
                    //  isn't full) then prepare to wait

                    if (pSample == NULL &&
                        (m_bBatchExact || m_nBatched == 0)) {

                        //  Tell other thread to set the event when there's
                        //  something do to

                        ASSERT(m_lWaiting == 0);
                        m_lWaiting++;
                        bWait      = TRUE;
                    } else {

                        //  We break out of the loop on SEND_PACKET unless
                        //  there's nothing to send

                        if (pSample == SEND_PACKET && m_nBatched == 0) {
                            continue;
                        }

                        if (pSample == NEW_SEGMENT) {
                            // now we need the parameters - we are
                            // guaranteed that the next packet contains them
                            ppacket = (NewSegmentPacket *) m_List->RemoveHead();
			    // we took something off the queue
			    if (m_hEventPop) {
                    	        //DbgLog((LOG_TRACE,3,TEXT("Queue: Delivered  SET EVENT")));
		    	        SetEvent(m_hEventPop);
			    }

                            ASSERT(ppacket);
                        }
                        //  EOS_PACKET falls through here and we exit the loop
                        //  In this way it acts like SEND_PACKET
                    }
                    break;
                }
            } // while(TRUE)
            if (!bWait) {
                // We look at m_nBatched from the client side so keep
                // it up to date inside the critical section
                lNumberToSend = m_nBatched;  // Local copy
                m_nBatched = 0;
            }
        } // scope for CAutoLock

        //  Wait for some more data

        if (bWait) {
            DbgWaitForSingleObject(m_hSem);
            continue;
        }



        //  OK - send it if there's anything to send
        //  We DON'T check m_bBatchExact here because either we've got
        //  a full batch or we dropped through because we got
        //  SEND_PACKET or EOS_PACKET - both of which imply we should
        //  flush our batch

        if (lNumberToSend != 0) {
            long nProcessed;
            if (m_hr == S_OK) {
                ASSERT(!m_bFlushed);

                HRESULT hr = m_pInputPin->ReceiveMultiple(m_ppSamples,
                                                          lNumberToSend,
                                                          &nProcessed);
                //DPRINTF(_l("Receive Returned %d"),hr);
                /*  Don't overwrite a flushing state HRESULT */
                CAutoLock lck(this);
                if (m_hr == S_OK) {
                    m_hr = hr;
                }
                ASSERT(!m_bFlushed);
            }
            while (lNumberToSend != 0) {
                m_ppSamples[--lNumberToSend]->Release();
            }
            if (m_hr != S_OK) {

                //  In any case wait for more data - S_OK just
                //  means there wasn't an error

                //DPRINTF(_l("ReceiveMultiple returned %8.8X"), m_hr);
            }
        }

        //  Check for end of stream

        if (pSample == EOS_PACKET) {

            //  We don't send even end of stream on if we've previously
            //  returned something other than S_OK
            //  This is because in that case the pin which returned
            //  something other than S_OK should have either sent
            //  EndOfStream() or notified the filter graph

            if (m_hr == S_OK) {
                DPRINTF(_l("COutputQueue sending EndOfStream()"));
                HRESULT hr = m_pPin->EndOfStream();
                if (FAILED(hr)) {
                    DPRINTF(_l("COutputQueue got code 0x%8.8X from EndOfStream()"));
                }
            }
        }

        //  Data from a new source

        if (pSample == RESET_PACKET) {
            m_hr = S_OK;
            SetEvent(m_evFlushComplete);
        }

        if (pSample == NEW_SEGMENT) {
            m_pPin->NewSegment(ppacket->tStart, ppacket->tStop, ppacket->dRate);
            delete ppacket;
        }
    }
}

//
// leave flush mode - pass this downstream
void TffOutputQueue::EndFlush()
{
    if(!m_bFlushing)
     return;
    return COutputQueue::EndFlush();
}

//  COutputQueue::QueueSample
//
//  private method to Send a sample to the output queue
//  The critical section MUST be held when this is called

void TffOutputQueue::QueueSample(IMediaSample *pSample)
{
    if (NULL == m_List->AddTail(pSample)) {
        if (!IsSpecialSample(pSample)) {
            pSample->Release();
        }
    }
}

//
//  COutputQueue::ReceiveMultiple()
//
//  Send a set of samples to the downstream pin
//
//      ppSamples           - array of samples
//      nSamples            - how many
//      nSamplesProcessed   - How many were processed
//
//  On return all samples will have been Release()'d
//

HRESULT TffOutputQueue::ReceiveMultiple (
    IMediaSample **ppSamples,
    long nSamples,
    long *nSamplesProcessed)
{
    CAutoLock lck(this);
    //  Either call directly or queue up the samples

    /*  We're sending to our thread */

    if (m_hr != S_OK) {
        *nSamplesProcessed = 0;
        DPRINTF(_l("COutputQueue (queued) : Discarding %d samples code 0x%8.8X"), nSamples, m_hr);
        for (int i = 0; i < nSamples; i++) {
            ppSamples[i]->Release();
        }
        return m_hr;
    }
    m_bFlushed = FALSE;
    for (long i = 0; i < nSamples; i++) {
        QueueSample(ppSamples[i]);
    }
    *nSamplesProcessed = nSamples;
    if (!m_bBatchExact ||
        m_nBatched + m_List->GetCount() >= m_lBatchSize) {
        //DPRINTF(_l("COutputQueue queued a sample"));
        NotifyThread();
    }
    return S_OK;
}

//  Get ready for new data - cancels sticky m_hr
void TffOutputQueue::Reset()
{
    if (!IsQueued()) {
        m_hr = S_OK;
    } else {
        {
            CAutoLock lck(this);
            QueueSample(RESET_PACKET);
            NotifyThread();
        }
        m_evFlushComplete.Wait();
    }
}

//  Remove and Release() all queued and Batched samples
void TffOutputQueue::FreeSamples()
{
    CAutoLock lck(this);
    if (IsQueued()) {
        while (TRUE) {
            IMediaSample *pSample = m_List->RemoveHead();
	    // inform derived class we took something off the queue
	    if (m_hEventPop) {
                //DbgLog((LOG_TRACE,3,TEXT("Queue: Delivered  SET EVENT")));
	        SetEvent(m_hEventPop);
	    }

            if (pSample == NULL) {
                break;
            }
            if (!IsSpecialSample(pSample)) {
                pSample->Release();
            } else {
                if (pSample == NEW_SEGMENT) {
                    //  Free NEW_SEGMENT packet
                    NewSegmentPacket *ppacket =
                        (NewSegmentPacket *) m_List->RemoveHead();
		    // inform derived class we took something off the queue
		    if (m_hEventPop) {
                        //DbgLog((LOG_TRACE,3,TEXT("Queue: Delivered  SET EVENT")));
		        SetEvent(m_hEventPop);
		    }

                    ASSERT(ppacket != NULL);
                    delete ppacket;
                }
            }
        }
    }
    for (int i = 0; i < m_nBatched; i++) {
        m_ppSamples[i]->Release();
    }
    m_nBatched = 0;
}
