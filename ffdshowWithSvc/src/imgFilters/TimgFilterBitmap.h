#ifndef _TIMGFILTERBITMAP_H_
#define _TIMGFILTERBITMAP_H_

#include "TimgFilter.h"
#include "Tfont.h"

struct TffPict;
struct TbitmapSettings;
DECLARE_FILTER(TimgFilterBitmap,public,TimgFilter)
private:
 TffPict *bitmap;
 Tbuffer bitmapbuf;
 char_t oldflnm[MAX_PATH];
 typedef void (*Tblendplane)(const TcspInfo &cspInfo,const unsigned int dx[3],const unsigned int dy[3],unsigned char *dst[3],const stride_t dststride[3],const unsigned char *src[3],const stride_t srcstride[3],int strength,int invstrength);

 class TrenderedSubtitleLineBitmap :public TrenderedSubtitleWordBase
  {
  public:
   TrenderedSubtitleLineBitmap(void):TrenderedSubtitleWordBase(false) {}
   TffPict *pict;
   const TbitmapSettings *cfg;
   Tblendplane blend;
   virtual void print(int startx, int starty /* not used */,unsigned int dx[3],int dy1[3],unsigned char *dstLn[3],const stride_t stride[3],const unsigned char *bmp[3],const unsigned char *msk[3],REFERENCE_TIME rtStart=REFTIME_INVALID) const;
  } w;
 TrenderedSubtitleLine l;
 TrenderedSubtitleLines ls;
 int oldmode;
 template<class _mm> static void blend(const TcspInfo &cspInfo,const unsigned int dx[3],const unsigned int dy[3],unsigned char *dst[3],const stride_t dststride[3],const unsigned char *src[3],const stride_t srcstride[3],int strength,int invstrength);
 template<class _mm> static void add(const TcspInfo &cspInfo,const unsigned int dx[3],const unsigned int dy[3],unsigned char *dst[3],const stride_t dststride[3],const unsigned char *src[3],const stride_t srcstride[3],int strength,int invstrength);
 template<class _mm> static void darken(const TcspInfo &cspInfo,const unsigned int dx[3],const unsigned int dy[3],unsigned char *dst[3],const stride_t dststride[3],const unsigned char *src[3],const stride_t srcstride[3],int strength,int invstrength);
 template<class _mm> static void lighten(const TcspInfo &cspInfo,const unsigned int dx[3],const unsigned int dy[3],unsigned char *dst[3],const stride_t dststride[3],const unsigned char *src[3],const stride_t srcstride[3],int strength,int invstrength);
 template<class _mm> static void softlight(const TcspInfo &cspInfo,const unsigned int dx[3],const unsigned int dy[3],unsigned char *dst[3],const stride_t dststride[3],const unsigned char *src[3],const stride_t srcstride[3],int strength,int invstrength);
 template<class _mm> static void exclusion(const TcspInfo &cspInfo,const unsigned int dx[3],const unsigned int dy[3],unsigned char *dst[3],const stride_t dststride[3],const unsigned char *src[3],const stride_t srcstride[3],int strength,int invstrength);
 template<class _mm> static Tblendplane getBlend(int mode);
protected:
 virtual bool is(const TffPictBase &pict,const TfilterSettingsVideo *cfg);
 virtual int getSupportedInputColorspaces(const TfilterSettingsVideo *cfg) const {return FF_CSPS_MASK_YUV_PLANAR;}
public:
 TimgFilterBitmap(IffdshowBase *Ideci,Tfilters *Iparent);
 virtual ~TimgFilterBitmap();
 virtual HRESULT process(TfilterQueue::iterator it,TffPict &pict,const TfilterSettingsVideo *cfg0);
};

#endif
