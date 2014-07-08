#ifndef _TVIDEOCODECAVISYNTH_H_
#define _TVIDEOCODECAVISYNTH_H_

#include "TvideoCodec.h"
#include "avisynth/Tavisynth.h"

struct Textradata;
class TvideoCodecAvisynth :public TvideoCodecDec
{
private:
 struct Tavisynth : public Tavisynth_c
  {
  public:
   Tavisynth(const Textradata &extradata,TffPictBase &pict,Trect &r,int &csp,IffdshowBase *deci);
   ~Tavisynth();
   IScriptEnvironment *env;
   const char *script;
   PClip *clip;
  } *avisynth;
 int csp;
 Trect r;
protected:
 virtual bool beginDecompress(TffPictBase &pict,FOURCC infcc,const CMediaType &mt,int sourceFlags);
public:
 static bool getVersion(const Tconfig *config,char *vers);
 virtual int getType(void) const {return IDFF_MOVIE_AVIS;}
 TvideoCodecAvisynth(IffdshowBase *Ideci,IdecVideoSink *Isink);
 virtual ~TvideoCodecAvisynth();
 virtual void forceOutputColorspace(const BITMAPINFOHEADER *hdr,int *ilace,TcspInfos &forcedCsps);
 virtual HRESULT decompress(const unsigned char *src,size_t srcLen,IMediaSample *pIn);
 virtual void end(void);
};

#endif
