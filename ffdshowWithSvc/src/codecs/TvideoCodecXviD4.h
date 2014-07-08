#ifndef _TVIDEOCODECXVID4_H_
#define _TVIDEOCODECXVID4_H_

#include "TvideoCodec.h"

class Tdll;
struct xvid_enc_frame_t;
struct Textradata;
class TvideoCodecXviD4 :public TvideoCodecDec,public TvideoCodecEnc
{
private:
 void create(void);
 Tdll *dll;
public:
 TvideoCodecXviD4(IffdshowBase *Ideci,IdecVideoSink *IsinkD);
 TvideoCodecXviD4(IffdshowBase *Ideci,IencVideoSink *IsinkE);
 virtual ~TvideoCodecXviD4();
 int (*xvid_global)(void *handle,int opt,void *param1,void *param2);
 int (*xvid_decore)(void *handle,int opt,void *param1,void *param2);
 int (*xvid_encore)(void *handle,int opt,void *param1,void *param2);
 int (*xvid_plugin_single)(void *handle,int opt,void *param1,void *param2);
 int (*xvid_plugin_lumimasking)(void *handle,int opt,void *param1,void *param2);
 static bool getVersion(const Tconfig *config,ffstring &vers,ffstring &license);
 static const char_t *dllname;
private:
 void *enchandle,*dechandle;
 xvid_enc_frame_t *frame;
 int psnr;
 TffPict pict;Tbuffer pictbuf;
 static int me_hq(int rd3),me_(int me3);
 Textradata *extradata;
 REFERENCE_TIME rtStart,rtStop;
protected:
 virtual LRESULT beginCompress(int cfgcomode,int csp,const Trect &r);
 virtual bool beginDecompress(TffPictBase &pict,FOURCC infcc,const CMediaType &mt,int sourceFlags);
 virtual HRESULT flushDec(void);
public:
 virtual int getType(void) const {return IDFF_MOVIE_XVID4;}
 virtual int caps(void) const {return CAPS::VIS_QUANTS;}

 virtual HRESULT compress(const TffPict &pict,TencFrameParams &params);
 virtual HRESULT flushEnc(const TffPict &pict,TencFrameParams &params) {return compress(pict,params);}
 virtual void end(void);

 virtual HRESULT decompress(const unsigned char *src,size_t srcLen,IMediaSample *pIn);
};

#endif
