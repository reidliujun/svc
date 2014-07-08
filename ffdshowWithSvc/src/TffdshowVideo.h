#ifndef _TFFDSHOWVIDEO_H_
#define _TFFDSHOWVIDEO_H_

#include "interfaces.h"

class CTransformFilter;
class CTransformOutputPin;
struct IdecVideoSink;
class TffdshowVideo
{
private:
 CTransformFilter *filter;
 CTransformOutputPin* &output;
 IffdshowBase *deci;
 IdecVideoSink *sink;
 friend class TffdshowVideoInputPin;
public:
 TffdshowVideo(CTransformFilter *Ifilter,CTransformOutputPin* &Ioutput,IffdshowBase *Ideci,IdecVideoSink *Isink):filter(Ifilter),deci(Ideci),sink(Isink),output(Ioutput) {}
 virtual int getVideoCodecId(const BITMAPINFOHEADER *hdr,const GUID *subtype,FOURCC *AVIfourcc)=0;
 virtual void initCodecSettings(void)=0;
 virtual void lockReceive(void)=0,unlockReceive(void)=0;
 const char_t* getExefilename(void) { return deci->getExeflnm(); }
};

#endif
