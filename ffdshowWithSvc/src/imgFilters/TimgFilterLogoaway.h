#ifndef _TIMGFILTERLOGOAWAY_H_
#define _TIMGFILTERLOGOAWAY_H_

#include "TimgFilter.h"

struct TlogoawaySettings;
DECLARE_FILTER(TimgFilterLogoaway,public,TimgFilter)
private:
 int oldLumaOnly,oldBlur,oldMode;char_t oldparambitmap[MAX_PATH];
 TffPict logotemp;Tbuffer logotempbuf;
 TffPict *parambitmap;Tbuffer parambitmapbuf;
 struct Tplane :safe_bool<Tplane>
  {
  private:
   typedef int FIX;
   unsigned char *bordn,*borde,*bords,*bordw;
   FIX *vd;  // when interpolation goes 2D
   FIX *vt;  // it is nice to store one dimension start/delta to put calc number down
   double *uwetable,*uweweightsum;
   void createBorder(const TlogoawaySettings *cfg,int side,int bmode);
   void selectPoints(const TlogoawaySettings *cfg,
                     int bmode,
                     unsigned char **s,unsigned char **e,
                     int psd,int ped,
                     int pso,int peo,
                     int psi,int pei);
   unsigned char* getCornerAddr(const TlogoawaySettings *cfg,int corner);
   inline unsigned char* getPixelAddr(int x,int y);
   static void createBorderH(unsigned char *border,const unsigned char *s,const unsigned char *e,int width,int bmode);
   static void calcGradient(const unsigned char *s,const unsigned char *e,int len,FIX *r,FIX *rd);
   static void createBorderV(unsigned char *border,const unsigned char *s,const unsigned char *e,int height,stride_t pitch,int bmode);
   void saveLogoBorder(int border);
   void setAverageVH(const TlogoawaySettings *cfg);
   void blurLogotemp(bool useparambitmap);
   void uwe(const TlogoawaySettings *cfg);
   void calcUweWeightTable(int w,int h,int power);
   void setSolid(const TlogoawaySettings *cfg);
   void loadLogo(const TlogoawaySettings *cfg,bool withborderN,bool withborderE,bool withborderS,bool withborderW);
   void renderShapeXY(const TlogoawaySettings *cfg);
   void calcShapeXY_VContourNeighbours(const TlogoawaySettings *cfg);
   static unsigned char mix2Pixels32(unsigned char a,unsigned char b,FIX wa,FIX wb);
   struct SHAPEXY_VCNDEF
    {
     int left,right;
     int up,down;
    } *shapexy_cn; // vertical contour neighbour
   void renderShapeUwe(const TlogoawaySettings *cfg);
  public:
   unsigned char *dst;stride_t stride2;
   int shiftX,shiftY;
   int w,h;
   unsigned char *logotempdata;stride_t logotempstride;
   int solidcolor;
   static const int SHAPEMAP_NOCHANGE= 16;
   static const int SHAPEMAP_CONTOUR =236;
   const unsigned char *parambitmapdata;stride_t parambitmapstride;

   Tplane(void)
    {
     bordn=bords=borde=bordw=NULL;
     vd=vt=NULL;
     uwetable=uweweightsum=NULL;
     shapexy_cn=NULL;
    }
   void init(const TlogoawaySettings *cfg);
   void process(const TlogoawaySettings *cfg);
   void done(void);
   bool boolean_test() const {return !!bordn;}
  } plane[3];
protected:
 virtual bool is(const TffPictBase &pict,const TfilterSettingsVideo *cfg);
 virtual int getSupportedInputColorspaces(const TfilterSettingsVideo *cfg) const {return FF_CSPS_MASK_YUV_PLANAR;}
 virtual int getSupportedOutputColorspaces(const TfilterSettingsVideo *cfg) const {return FF_CSPS_MASK_YUV_PLANAR;}
public:
 TimgFilterLogoaway(IffdshowBase *Ideci,Tfilters *Iparent);
 virtual void onSizeChange(void);
 virtual void done(void);
 virtual HRESULT process(TfilterQueue::iterator it,TffPict &pict,const TfilterSettingsVideo *cfg0);
};

#endif
