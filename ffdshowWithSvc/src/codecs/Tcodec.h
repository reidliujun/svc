#ifndef _TCODEC_H_
#define _TCODEC_H_

#include "ffcodecs.h"
#include "interfaces.h"

DECLARE_INTERFACE(IdecSink)
{
 STDMETHOD (flushDecodedSamples)(void) PURE;
};

struct Tconfig;
class Tcodec
{
protected:
 const Tconfig *config;
 comptr<IffdshowBase> deci;
 Tcodec(IffdshowBase *Ideci);
 virtual ~Tcodec();
public:
 CodecID codecId;
 virtual int getType(void) const =0;
 virtual const char_t* getName(void) const {return getMovieSourceName(getType());}
 virtual void getEncoderInfo(char_t *buf,size_t buflen) const {ff_strncpy(buf,_l("unknown"),buflen);buf[buflen-1]='\0';}
 static const char_t* getMovieSourceName(int source);

 virtual HRESULT flush(void) {return S_OK;}
 virtual bool onSeek(REFERENCE_TIME segmentStart) {return false;}
};

class TcodecDec : virtual public Tcodec
{
private:
 IdecSink *sink;
protected:
 comptrQ<IffdshowDec> deciD;
 TcodecDec(IffdshowBase *Ideci,IdecSink *Isink);
 virtual ~TcodecDec();
 virtual HRESULT flushDec(void) {return S_OK;}
public:
 virtual HRESULT flush(void);
};

#endif
