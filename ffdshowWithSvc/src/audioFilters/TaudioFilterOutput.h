#ifndef _TAUDIOFILTEROUTPUT_H_
#define _TAUDIOFILTEROUTPUT_H_

#include "TaudioFilter.h"

struct Tlibavcodec;
struct AVCodecContext;
DECLARE_FILTER(TaudioFilterOutput,public,TaudioFilter)
private:
 Tlibavcodec *libavcodec;
 TsampleFormat oldsf;
 AVCodecContext *avctx;
 bool ac3inited;
 uint8_t *ac3buf;
 TbyteBuffer ac3inputbuf;
protected:
 virtual int getSupportedFormats(const TfilterSettingsAudio *cfg,bool *honourPreferred,const TsampleFormat &sf) const;
public:
 TaudioFilterOutput(IffdshowBase *Ideci,Tfilters *Iparent);
 virtual ~TaudioFilterOutput();
 virtual void done(void);
 virtual bool getOutputFmt(TsampleFormat &fmt,const TfilterSettingsAudio *cfg);
 virtual HRESULT process(TfilterQueue::iterator it,TsampleFormat &fmt,void *samples,size_t numsamples,const TfilterSettingsAudio *cfg);
 virtual void onSeek(void);
};

#endif
