#ifndef _TVIDEOCODECUNCOMPRESSED_H_
#define _TVIDEOCODECUNCOMPRESSED_H_

#include "TvideoCodec.h"

struct TcspInfo;
struct Textradata;
class TvideoCodecUncompressed :public TvideoCodecDec,public TvideoCodecEnc
{
private:
 int csp;const TcspInfo *cspInfo;
 RGBQUAD palette[256];unsigned int palcolors;
 stride_t stride[4];
 Trect rd;
protected:
 virtual LRESULT beginCompress(int cfgcomode,int csp,const Trect &r);
 virtual bool beginDecompress(TffPictBase &pict,FOURCC infcc,const CMediaType &mt,int sourceFlags);
public:
 TvideoCodecUncompressed(IffdshowBase *Ideci,IdecVideoSink *IsinkD);
 TvideoCodecUncompressed(IffdshowBase *Ideci,IencVideoSink *IsinkE);

 static void getVersion(char **vers);
 virtual int getType(void) const {return IDFF_MOVIE_RAW;}

 virtual void getCompressColorspaces(Tcsps &csps,unsigned int outDx,unsigned int outDy);
 virtual bool prepareHeader(BITMAPINFOHEADER *outhdr);
 virtual HRESULT compress(const TffPict &pict,TencFrameParams &params);

 virtual HRESULT decompress(const unsigned char *src,size_t srcLen,IMediaSample *pIn);
};

#endif
