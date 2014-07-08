#ifndef _TRESIZEASPECTSETTINGS_H_
#define _TRESIZEASPECTSETTINGS_H_

#include "TfilterSettings.h"
#include "TffRect.h"

struct TresizeAspectSettings :TfilterSettingsVideo
{
private:
 static const TfilterIDFF idffs;
 static const char_t* printSwsParams(char_t *algParam,int method,int bicubicParam,int gaussParam,int lanczosParam);
protected:
 virtual const int *getResets(unsigned int pageId);
public:
 TresizeAspectSettings(TintStrColl *Icoll=NULL,TfilterIDFFs *filters=NULL);
 int mode;
 unsigned int dx,dy,specifyHorizontalSizeOnly,dyReg;
 int multOf;
 int a1,a2;
 int mult1000;
 int SARinternally,outDeviceA1,outDeviceA2;
 void calcNewRects(Trect *rectFull, Trect *rectClip, bool flip) const;
 int _if;
 int xcond;unsigned int xval;
 int ycond;unsigned int yval;
 int xycond;
 int pixcond;unsigned int pixval;
 static const char_t *getXcondStr(int cond),*getXYcondStr(int cond),*getPixCondStr(int cond);
 bool ifResize(unsigned int AVIdx,unsigned int AVIdy) const;
 int isAspect,aspectRatio;

 int bordersInside,bordersLocked;
 int bordersUnits; //0 - percent, 1 - pixels
 int bordersPercentX,bordersPercentY;
 int bordersPixelsX,bordersPixelsY,bordersDivX,bordersDivY;

 bool areBorders(void) const
  {
   return (bordersUnits==0 && (bordersPercentX || bordersPercentY)) ||
          (bordersUnits==1 && (bordersPixelsX  || bordersPixelsY ));
  }
 int methodLuma,methodChroma,methodsLocked;
 enum RESIZE_LIBRARY
  {
   LIB_SWSCALER,
   LIB_SIMPLE,
   LIB_SAI,
   LIB_NONE
  };
 enum METHOD
  {
   METHOD_SWS_FAST_BILINEAR=0,
   METHOD_SWS_BILINEAR=1,
   METHOD_SWS_BICUBIC=2,
   METHOD_SWS_X=3,
   METHOD_SWS_POINT=4,
   METHOD_SWS_AREA=5,
   METHOD_SWS_BICUBLIN=6,
   METHOD_SWS_GAUSS=7,
   METHOD_SWS_SINC=8,
   METHOD_SWS_LANCZOS=9,
   METHOD_SWS_SPLINE=10,
   METHOD_NONE=11,
   METHOD_SIMPLE=12,
   METHOD_WARPED=13,
   METHOD_SUPER2XSAI=14,
   METHOD_2XSAI=15,
   METHOD_HQ2X=16,
   METHOD_LAST
  };
 struct TmethodProps
  {
   int id;
   const char_t *name;
   RESIZE_LIBRARY library;
   int flags;
  };
 static const TmethodProps methodsProps[];
 static const int methodDef=METHOD_SWS_BICUBIC,numMethods=METHOD_LAST,methodsOrder[];
 int bicubicLumaParam,bicubicChromaParam;
 int gaussLumaParam,gaussChromaParam;
 int lanczosLumaParam,lanczosChromaParam;
 int simpleWarpXparam,simpleWarpYparam;
 int GblurLum,GblurChrom;
 int sharpenLum,sharpenChrom;
 int interlaced;
 int accurateRounding;
 virtual void createFilters(size_t filtersorder,Tfilters *filters,TfilterQueue &queue) const;
 virtual void createPages(TffdshowPageDec *parent) const;
 virtual bool getTip(unsigned int pageId,char_t *buf,size_t buflen);
 virtual void reg_op(TregOp &t);
 virtual bool reset(unsigned int pageId=0);
};

#endif
