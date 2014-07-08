#ifndef _TIMGFILTER_H_
#define _TIMGFILTER_H_

#include "Tfilter.h"
#include "TffRect.h"
#include "TffPict.h"
#include "ffImgfmt.h"
#include "TimgFilters.h"

struct TfilterSettings;
struct TglobalSettingsDecVideo;
class Tconvert;
class TimgFilter :public Tfilter
{
private:
 void free(void);
 Tbuffer own1;  // getCur
 Tbuffer own2;  // getNext, GetCurNext
 Tbuffer own3;  // getCurNext3 which is used in TimgFilterSubtitles
 Tconvert *convert1,*convert2;
 bool getCur(int csp,TffPict &pict,int full,const unsigned char **src[4]);
 bool getNext(int csp,TffPict &pict,int full,unsigned char **dst[4],const Trect *rect2=NULL);
 bool getNext(int csp,TffPict &pict,const Trect &clipRect,unsigned char **dst[4],const Trect *rect2=NULL);
 bool getCurNext(int csp,TffPict &pict,int full,int copy,unsigned char **dst[4],Tbuffer &buf);

 int pictHalf;
 int oldBrightness;
protected:
 int old_cspOptionsRgbInterlaceMode;
 Trect pictRect;
 unsigned int dx1[4],dy1[4],dx2[4],dy2[4];
 stride_t stride1[4]; // call GetCur to fill.
 stride_t stride2[4];
 int csp1,csp2;
 comptrQ<IffdshowDecVideo> deciV;
 TimgFilters *parent;
 void init(const TffPict &pict,int full,int half);
 virtual void onSizeChange(void) {}
 void checkBorder(TffPict &pict);
 virtual bool is(const TffPictBase &pict,const TfilterSettingsVideo *cfg);
 virtual int getSupportedInputColorspaces(const TfilterSettingsVideo *cfg) const=0;
 virtual int getSupportedOutputColorspaces(const TfilterSettingsVideo *cfg) const {return getSupportedInputColorspaces(cfg);}
 bool getCur(int csp,TffPict &pict,int full,const unsigned char **src0,const unsigned char **src1,const unsigned char **src2,const unsigned char **src3)
  {
   const unsigned char **src[4]={src0,src1,src2,src3};
   return getCur(csp,pict,full,src);
  }
 bool getCur(int csp,TffPict &pict,int full,const unsigned char *src[4])
  {
   const unsigned char **srcP[4]={&src[0],&src[1],&src[2],&src[3]};
   return getCur(csp,pict,full,srcP);
  }
 bool getNext(int csp,TffPict &pict,int full,unsigned char **dst0,unsigned char **dst1,unsigned char **dst2,unsigned char **dst3,const Trect *rect2=NULL)
  {
   unsigned char **dst[4]={dst0,dst1,dst2,dst3};
   return getNext(csp,pict,full,dst,rect2);
  }
 bool getNext(int csp,TffPict &pict,int full,unsigned char *dst[4],const Trect *rect2=NULL)
  {
   unsigned char **dstP[4]={&dst[0],&dst[1],&dst[2],&dst[3]};
   return getNext(csp,pict,full,dstP,rect2);
  }
 bool getNext(int csp,TffPict &pict,const Trect &clipRect,unsigned char **dst0,unsigned char **dst1,unsigned char **dst2,unsigned char **dst3,const Trect *rect2=NULL)
  {
   unsigned char **dst[4]={dst0,dst1,dst2,dst3};
   return getNext(csp,pict,clipRect,dst,rect2);
  }
 bool getNext(int csp,TffPict &pict,const Trect &clipRect,unsigned char *dst[4],const Trect *rect2=NULL)
  {
   unsigned char **dstP[4]={&dst[0],&dst[1],&dst[2],&dst[3]};
   return getNext(csp,pict,clipRect,dstP,rect2);
  }
 enum
  {
   COPYMODE_NO  =0, // picture will be copied only if TimgFilter::pictHalf is set true.
   COPYMODE_CLIP=1, // picture in rectClip will be copied
   COPYMODE_FULL=2, // whole picture will be copied
   COPYMODE_DEF =3  // In call to getCurNext, if full is ture, COPYMODE_FULL else COPYMODE_CLIP.
  };
 bool getCurNext(int csp,TffPict &pict,int full,int copy,unsigned char **dst0,unsigned char **dst1,unsigned char **dst2,unsigned char **dst3)
  {
   unsigned char **dst[4]={dst0,dst1,dst2,dst3};
   return getCurNext(csp,pict,full,copy,dst,own2);
  }
 bool getCurNext(int csp,TffPict &pict,int full,int copy,unsigned char *dst[4])
  {
   unsigned char **dstP[4]={&dst[0],&dst[1],&dst[2],&dst[3]};
   return getCurNext(csp,pict,full,copy,dstP,own2);
  }
 bool getCurNext3(int csp,TffPict &pict,int full,int copy,unsigned char *dst[4])
  {
   unsigned char **dstP[4]={&dst[0],&dst[1],&dst[2],&dst[3]};
   return getCurNext(csp,pict,full,copy,dstP,own3);
  }
 bool screenToPict(CPoint &pt);
public:
 TimgFilter(IffdshowBase *Ideci,Tfilters *Iparent);
 virtual ~TimgFilter();
 virtual bool acceptRandomYV12andRGB32(void) {return false;} // Subtitle filter may change color space to RGB32 randomly.
 virtual bool getOutputFmt(TffPictBase &pict,const TfilterSettingsVideo *cfg);
 virtual HRESULT process(TfilterQueue::iterator it,TffPict &pict,const TfilterSettingsVideo *cfg0)=0;
 virtual HRESULT flush(TfilterQueue::iterator it,TffPict &pict,const TfilterSettingsVideo *cfg0);
 virtual int getImgFilterID(void) {return IMGFILTER_UNKNOWN;}

 /**
  * onEndOfStream
  * Over-ride this function if your image filter needs forward temporal image processing.
  * At the end of stream, output frames in your frame buffer on this function call.
  */
 virtual HRESULT onEndOfStream(void) {return E_NOTIMPL;}

 /**
  * onPullImageFromSubtitlesFilter
  * Over-ride this function if your image filter needs forward temporal image processing.
  * This function is called on updating DVD menu from TimgFilterSubtitles::ctlSubtitles,
  * if your filter is in the upper stream of subtitle filter where DVD menu handling is
  * implemented.
  * The back ground image of DVD menu may be a single image, so if your filter buffer it
  * and does not deliver immediately, the menu will be drown on a wrong image.
  *
  * Send out only one image.
  * @return true if your filter send out a image.
  */
 virtual bool onPullImageFromSubtitlesFilter(void) {return false;}

 enum
  {
   IMGFILTER_UNKNOWN,
   IMGFILTER_SUBTITLES
  };
};

#endif
