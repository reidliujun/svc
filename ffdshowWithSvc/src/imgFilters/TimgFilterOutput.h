#ifndef _TIMGFILTEROUTPUT_H_
#define _TIMGFILTEROUTPUT_H_

#include "TimgFilter.h"

struct ToutputVideoSettings;
struct Tlibavcodec;
struct AVCodecContext;
struct AVFrame;
DECLARE_FILTER(TimgFilterOutput,public,TimgFilter)
private:
 Tconvert *convert;
 Tlibavcodec *libavcodec;
 bool firsttime,dv;
 AVCodecContext *avctx;AVFrame *frame;int dvcsp;TffPict *dvpict;Tbuffer dvpictbuf;
protected:
 int old_cspOptionsRgbInterlaceMode, old_avisynthYV12_RGB;
 virtual int getSupportedInputColorspaces(const TfilterSettingsVideo *cfg) const {return FF_CSPS_MASK;}
public:
 TimgFilterOutput(IffdshowBase *Ideci,Tfilters *Iparent);
 virtual ~TimgFilterOutput();
 virtual HRESULT process(TfilterQueue::iterator it,TffPict &pict,const TfilterSettingsVideo *cfg0) {return E_NOTIMPL;}
 HRESULT process(const TffPict &pict,int dstcsp,unsigned char *dst[4],int dstStride[4],LONG &dstSize,const ToutputVideoSettings *cfg); //S_FALSE = dv
};

DECLARE_FILTER(TimgFilterOutputConvert,public,TimgFilter)
protected:
 virtual int getSupportedInputColorspaces(const TfilterSettingsVideo *cfg) const {return FF_CSPS_MASK;}
public:
 TimgFilterOutputConvert(IffdshowBase *Ideci,Tfilters *Iparent):TimgFilter(Ideci,Iparent) {}
 virtual HRESULT process(TfilterQueue::iterator it,TffPict &pict,const TfilterSettingsVideo *cfg0);
};

#endif
