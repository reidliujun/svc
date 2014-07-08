#ifndef _TAUDIOFILTERDELAY_H_
#define _TAUDIOFILTERDELAY_H_

#include "TaudioFilter.h"
#include "TdelaySettings.h"

DECLARE_FILTER(TaudioFilterDelay,public,TaudioFilter)
private:
 TdelaySettings oldcfg;
 TsampleFormat oldfmt;
 struct TdelayBuffer
  {
  private:
   TsampleFormat sf;
   void *buf;
   unsigned int len,pos,Bpp;
  public:
   TdelayBuffer(void):buf(NULL),len(0),Bpp(0) {}
   ~TdelayBuffer() {if (buf) free(buf);}
   void init(unsigned int ms,const TsampleFormat &Isf);
   void clear(void);
   template<class T> void delay(T *samples,size_t numsamples)
    {
     if (len==0) return;
     T* buf=(T*)this->buf;
     for (size_t i=0;i<numsamples;i++,samples+=sf.nchannels)
      {
       T s=*samples;
       *samples=buf[pos];
       buf[pos]=s;
       pos++;if (pos==len) pos=0;
      }
    }
  } delay[8];
protected:
 virtual int getSupportedFormats(const TfilterSettingsAudio *cfg,bool *honourPreferred,const TsampleFormat &sf) const {*honourPreferred=false;return TsampleFormat::SF_ALL;}
public:
 TaudioFilterDelay(IffdshowBase *Ideci,Tfilters *Iparent);
 virtual HRESULT process(TfilterQueue::iterator it,TsampleFormat &fmt,void *samples,size_t numsamples,const TfilterSettingsAudio *cfg0);
 virtual void onSeek(void);
};

#endif
