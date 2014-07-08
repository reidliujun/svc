/*
 * Copyright (c) 2003-2006 Milan Cutka
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "stdafx.h"
#include "TresizeAspectSettings.h"
#include "TimgFilterResize.h"
#include "CresizeAspect.h"
#include "CresizeBorders.h"
#include "CresizeSettings.h"
#include "TffdshowPageDec.h"
#include "postproc/swscale.h"

const TresizeAspectSettings::TmethodProps TresizeAspectSettings::methodsProps[]=
{
 METHOD_SWS_FAST_BILINEAR,_l("Fast bilinear"),LIB_SWSCALER,SWS_FAST_BILINEAR,
 METHOD_SWS_BILINEAR     ,_l("Bilinear"),     LIB_SWSCALER,SWS_BILINEAR,
 METHOD_SWS_BICUBIC      ,_l("Bicubic"),      LIB_SWSCALER,SWS_BICUBIC,
 METHOD_SWS_X            ,_l("Experimental"), LIB_SWSCALER,SWS_X,
 METHOD_SWS_POINT        ,_l("Point"),        LIB_SWSCALER,SWS_POINT,
 METHOD_SWS_AREA         ,_l("Area"),         LIB_SWSCALER,SWS_AREA,
 METHOD_SWS_BICUBLIN     ,_l("Bicublin"),     LIB_SWSCALER,0,//SWS_BICUBLIN,
 METHOD_SWS_GAUSS        ,_l("Gauss"),        LIB_SWSCALER,SWS_GAUSS,
 METHOD_SWS_SINC         ,_l("Sinc"),         LIB_SWSCALER,SWS_SINC,
 METHOD_SWS_LANCZOS      ,_l("Lanczos"),      LIB_SWSCALER,SWS_LANCZOS,
 METHOD_SWS_SPLINE       ,_l("Spline"),       LIB_SWSCALER,SWS_SPLINE,
 METHOD_NONE             ,_l("None"),         LIB_NONE,0,
 METHOD_SIMPLE           ,_l("Simple resize"),LIB_SIMPLE,0,
 METHOD_WARPED           ,_l("Warped resize"),LIB_SIMPLE,1,
 METHOD_SUPER2XSAI       ,_l("Super2xSaI")   ,LIB_SAI,0,
 METHOD_2XSAI            ,_l("2xSaI")        ,LIB_SAI,1,
 METHOD_HQ2X             ,_l("hq2x")         ,LIB_SAI,2,
 NULL
};
const int TresizeAspectSettings::methodsOrder[]=
{
 METHOD_SWS_FAST_BILINEAR,
 METHOD_SWS_BILINEAR,
 METHOD_SWS_BICUBIC,
// METHOD_SWS_X,
 METHOD_SWS_POINT,
 METHOD_SWS_AREA,
 //METHOD_SWS_BICUBLIN,
 METHOD_SWS_GAUSS,
 METHOD_SWS_SINC,
 METHOD_SWS_LANCZOS,
 METHOD_SWS_SPLINE,
 METHOD_SIMPLE,
 METHOD_WARPED,
 METHOD_SUPER2XSAI,
 METHOD_2XSAI,
 METHOD_HQ2X,
 METHOD_NONE,
 -1
};

const TfilterIDFF TresizeAspectSettings::idffs=
{
 /*name*/      _l("Resize & aspect"),
 /*id*/        IDFF_filterResize,
 /*is*/        IDFF_isResize,
 /*order*/     IDFF_orderResize,
 /*show*/      IDFF_showResize,
 /*full*/      IDFF_fullResize,
 /*half*/      0,
 /*dlgId*/     IDD_RESIZEASPECT,
};

TresizeAspectSettings::TresizeAspectSettings(TintStrColl *Icoll,TfilterIDFFs *filters):TfilterSettingsVideo(sizeof(*this),Icoll,filters,&idffs)
{
 half=0;
 static const TintOptionT<TresizeAspectSettings> iopts[]=
  {
   IDFF_isResize           ,&TresizeAspectSettings::is             ,0,0,_l(""),1,
     _l("isResize"),0,
   IDFF_showResize         ,&TresizeAspectSettings::show           ,0,0,_l(""),1,
     _l("showResize"),1,
   IDFF_orderResize        ,&TresizeAspectSettings::order          ,1,1,_l(""),1,
     _l("orderResize"),0,
   IDFF_fullResize         ,&TresizeAspectSettings::full           ,0,0,_l(""),1,
     _l("fullResize"),0,
   IDFF_resizeMode         ,&TresizeAspectSettings::mode           ,0,5,_l(""),1,
     _l("resizeMode"),0,
   IDFF_resizeDx           ,(TintVal)&TresizeAspectSettings::dx    ,64,16384,_l(""),1,
     _l("resizeDx"),640,
   IDFF_resizeSpecifyHorizontalSizeOnly,(TintVal)&TresizeAspectSettings::specifyHorizontalSizeOnly ,0,1,_l(""),1,
     _l("resizeIsDy0"),0,  // if(resizeDy/*registry*/==0) older version crashes. When specifyHorizontalSizeOnly==1, Vertical size is calculated automatically.
   IDFF_resizeDy           ,(TintVal)&TresizeAspectSettings::dyReg ,0,16384,_l(""),1,
     _l("resizeDy"),480,
   IDFF_resizeDy_real      ,(TintVal)&TresizeAspectSettings::dy    ,0,16384,_l(""),1,
     NULL,480,
   IDFF_resizeMultOf       ,&TresizeAspectSettings::multOf         ,1,128,_l(""),1,
     _l("resizeMulfOf"),16,
   IDFF_resizeA1           ,&TresizeAspectSettings::a1             ,1,10000,_l(""),1,
     _l("resizeA1"),4,
   IDFF_resizeA2           ,&TresizeAspectSettings::a2             ,1,10000,_l(""),1,
     _l("resizeA2"),3,
   IDFF_resizeMult1000     ,&TresizeAspectSettings::mult1000       ,1,1000*1000,_l(""),1,
     _l("resizeMult1000"),2000,
   IDFF_resizeSARinternally,&TresizeAspectSettings::SARinternally  ,0,0,_l(""),1,
     _l("resizeSARinternally"),1,
   IDFF_resizeOutDeviceA1  ,&TresizeAspectSettings::outDeviceA1    ,1,10000,_l(""),1,
     _l("resizeOutDeviceA1"),1,
   IDFF_resizeOutDeviceA2  ,&TresizeAspectSettings::outDeviceA2    ,1,10000,_l(""),1,
     _l("resizeOutDeviceA2"),1,
   IDFF_resizeIf           ,&TresizeAspectSettings::_if            ,0,2,_l(""),1,
     _l("resizeIf"),0,
   IDFF_resizeIfXcond      ,&TresizeAspectSettings::xcond          ,-1,1,_l(""),1,
     _l("resizeIfXcond"),1,
   IDFF_resizeIfXval       ,(TintVal)&TresizeAspectSettings::xval  ,0,16384,_l(""),1,
     _l("resizeIfXval"),640,
   IDFF_resizeIfYcond      ,&TresizeAspectSettings::ycond          ,-1,1,_l(""),1,
     _l("resizeIfYcond"),1,
   IDFF_resizeIfYval       ,(TintVal)&TresizeAspectSettings::yval  ,0,16384,_l(""),1,
     _l("resizeIfYval"),480,
   IDFF_resizeIfXYcond     ,&TresizeAspectSettings::xycond         ,0,1,_l(""),1,
     _l("resizeIfXYcond"),1,
   IDFF_resizeIfPixCond    ,&TresizeAspectSettings::pixcond        ,-1,1,_l(""),1,
     _l("resizeIfPixCond"),1,
   IDFF_resizeIfPixVal     ,(TintVal)&TresizeAspectSettings::pixval,0,16384*16384,_l(""),1,
     _l("resizeIfPixVal"),640*480,

   IDFF_bordersInside      ,&TresizeAspectSettings::bordersInside  ,0,0,_l(""),1,
     _l("bordersInside"),1,
   IDFF_bordersUnits       ,&TresizeAspectSettings::bordersUnits   ,0,1,_l(""),1,
     _l("bordersUnits"),0,
   IDFF_bordersLocked      ,&TresizeAspectSettings::bordersLocked  ,0,0,_l(""),1,
     _l("bordersLocked"),1,
   IDFF_bordersPercentX    ,&TresizeAspectSettings::bordersPercentX,0,100,_l(""),1,
     _l("bordersX"),0,
   IDFF_bordersPercentY    ,&TresizeAspectSettings::bordersPercentY,0,100,_l(""),1,
     _l("bordersY"),0,
   IDFF_bordersPixelsX     ,&TresizeAspectSettings::bordersPixelsX ,0,300,_l(""),1,
     _l("bordersPixelsX"),0,
   IDFF_bordersPixelsY     ,&TresizeAspectSettings::bordersPixelsY ,0,300,_l(""),1,
     _l("bordersPixelsY"),0,
   IDFF_bordersDivX        ,&TresizeAspectSettings::bordersDivX    ,0,100,_l(""),1,
     _l("bordersDivX"),50,
   IDFF_bordersDivY        ,&TresizeAspectSettings::bordersDivY    ,0,100,_l(""),1,
     _l("bordersDivY"),50,

   IDFF_isAspect           ,&TresizeAspectSettings::isAspect    ,0,0,_l(""),1,
     _l("resizeAscpect"),1,
   IDFF_aspectRatio        ,&TresizeAspectSettings::aspectRatio ,0,0,_l(""),1,
     _l("aspectRatio"),int(1.33*65536),

   IDFF_resizeMethodLuma      ,&TresizeAspectSettings::methodLuma      ,0,numMethods-1,_l(""),1,
     _l("resizeMethod"),methodDef,
   IDFF_resizeMethodChroma    ,&TresizeAspectSettings::methodChroma    ,0,numMethods-1,_l(""),1,
     _l("resizeMethodChroma"),-1,
   IDFF_resizeMethodsLocked   ,&TresizeAspectSettings::methodsLocked   ,0,0,_l(""),1,
     _l("resizeMethodsLocked"),1,

   IDFF_resizeBicubicLumaParam   ,&TresizeAspectSettings::bicubicLumaParam    ,0,400,_l(""),1,
     _l("resizeBicubicParam"),0,
   IDFF_resizeBicubicChromaParam ,&TresizeAspectSettings::bicubicChromaParam  ,0,400,_l(""),1,
     _l("resizeBicubicChromaParam"),-1,

   IDFF_resizeGaussLumaParam     ,&TresizeAspectSettings::gaussLumaParam      ,0,100,_l(""),1,
     _l("resizeGaussParam"),0,
   IDFF_resizeGaussChromaParam   ,&TresizeAspectSettings::gaussChromaParam    ,0,100,_l(""),1,
     _l("resizeGaussChromaParam"),-1,

   IDFF_resizeLanczosLumaParam   ,&TresizeAspectSettings::lanczosLumaParam    ,0,10,_l(""),1,
     _l("resizeLanczosParam"),0,
   IDFF_resizeLanczosChromaParam ,&TresizeAspectSettings::lanczosChromaParam  ,0,10,_l(""),1,
     _l("resizeLanczosChromaParam"),-1,

   IDFF_resizeSimpleWarpXparam,&TresizeAspectSettings::simpleWarpXparam,800,1300,_l(""),1,
     _l("resizeSimpleWarpXparam"),int(1.15*1000),
   IDFF_resizeSimpleWarpYparam,&TresizeAspectSettings::simpleWarpYparam,800,1300,_l(""),1,
     _l("resizeSimpleWarpYparam"),int(0.95*1000),
   IDFF_resizeGblurLum        ,&TresizeAspectSettings::GblurLum        ,0,200,_l(""),1,
     _l("resizeGblurLum"),0,
   IDFF_resizeGblurChrom      ,&TresizeAspectSettings::GblurChrom      ,0,200,_l(""),1,
     _l("resizeGblurChrom"),0,
   IDFF_resizeSharpenLum      ,&TresizeAspectSettings::sharpenLum      ,0,200,_l(""),1,
     _l("resizeSharpenLum"),0,
   IDFF_resizeSharpenChrom    ,&TresizeAspectSettings::sharpenChrom    ,0,200,_l(""),1,
     _l("resizeSharpenChrom"),0,
   IDFF_resizeInterlaced      ,&TresizeAspectSettings::interlaced      ,0,2,_l(""),1,
     _l("resizeInterlaced"),0,
   IDFF_resizeAccurateRounding,&TresizeAspectSettings::accurateRounding,0,0,_l(""),1,
     _l("resizeAccurateRounding"),1,
   0
  };
 addOptions(iopts);

 static const TcreateParamList2<TmethodProps> listMethod(TresizeAspectSettings::methodsProps,&TmethodProps::name);setParamList(IDFF_resizeMethodLuma,&listMethod);setParamList(IDFF_resizeMethodChroma,&listMethod);
}

void TresizeAspectSettings::createFilters(size_t filtersorder,Tfilters *filters,TfilterQueue &queue) const
{
 idffOnChange(idffs,filters,queue.temporary);
 if (show)
  queueFilter<TimgFilterResize>(filtersorder,filters,queue);
}
void TresizeAspectSettings::createPages(TffdshowPageDec *parent) const
{
 if ((parent->deci->getParam2(IDFF_filterMode)&IDFF_FILTERMODE_VFW)==0)
  {
   parent->addFilterPage<TresizeAspectPage>(&idffs);
   parent->addFilterPage<TresizeBordersPage>(&idffs);
   parent->addFilterPage<TresizeSettingsPage>(&idffs);
  }
}

const int* TresizeAspectSettings::getResets(unsigned int pageId)
{
 if (pageId==0) // TODO: unify
  {
   static const int idResets[]={
    IDFF_resizeMode,IDFF_resizeDx,IDFF_resizeDy,IDFF_resizeA1,IDFF_resizeA2,IDFF_resizeMult1000,IDFF_resizeIf,IDFF_resizeIfXcond,IDFF_resizeIfXval,IDFF_resizeIfYcond,IDFF_resizeIfYval,IDFF_resizeIfXYcond,IDFF_resizeIfPixCond,IDFF_resizeIfPixVal,
    IDFF_isAspect,IDFF_aspectRatio,
    IDFF_bordersInside,IDFF_bordersLocked,IDFF_bordersUnits,
    IDFF_bordersX,IDFF_bordersY,
    IDFF_bordersPixelsX,IDFF_bordersPixelsY,
    IDFF_resizeMethodLuma,IDFF_resizeMethodChroma,IDFF_resizeMethodsLocked,
    IDFF_resizeGblurLum,IDFF_resizeGblurChrom,
    IDFF_resizeSharpenLum,IDFF_resizeSharpenChrom,
    IDFF_resizeBicubicLumaParam,IDFF_resizeBicubicChromaParam,
    IDFF_resizeGaussLumaParam,IDFF_resizeGaussChromaParam,
    IDFF_resizeLanczosLumaParam,IDFF_resizeLanczosChromaParam,
    IDFF_resizeSimpleWarpXparam,IDFF_resizeSimpleWarpYparam,
    IDFF_resizeInterlaced,IDFF_resizeAccurateRounding,
    0};
   return idResets;
  }
 else if (pageId==2)
  {
   static const int idResets[]={
    IDFF_bordersInside,IDFF_bordersLocked,IDFF_bordersUnits,
    IDFF_bordersX,IDFF_bordersY,
    IDFF_bordersPixelsX,IDFF_bordersPixelsY,
    IDFF_bordersDivX,IDFF_bordersDivY,
    0};
   return idResets;
  }
 else if (pageId==3)
  {
   static const int idResets[]={
    IDFF_resizeMethodLuma,IDFF_resizeMethodChroma,IDFF_resizeMethodsLocked,
    IDFF_resizeGblurLum,IDFF_resizeGblurChrom,
    IDFF_resizeSharpenLum,IDFF_resizeSharpenChrom,
    IDFF_resizeBicubicLumaParam,IDFF_resizeBicubicChromaParam,
    IDFF_resizeGaussLumaParam,IDFF_resizeGaussChromaParam,
    IDFF_resizeLanczosLumaParam,IDFF_resizeLanczosChromaParam,
    IDFF_resizeSimpleWarpXparam,IDFF_resizeSimpleWarpYparam,
    IDFF_resizeInterlaced,IDFF_resizeAccurateRounding,
    0};
   return idResets;
  }
 else
  return NULL;
}
bool TresizeAspectSettings::reset(unsigned int pageId)
{
 bool res=TfilterSettingsVideo::reset(pageId);
 if (res && pageId==3)
  {
   methodChroma=methodLuma;
   bicubicChromaParam=bicubicLumaParam;
   gaussChromaParam=gaussLumaParam;
   lanczosChromaParam=lanczosLumaParam;
  }
 return res;
}

const char_t* TresizeAspectSettings::printSwsParams(char_t *algParam,int method,int bicubicParam,int gaussParam,int lanczosParam)
{
 switch (method)
  {
   case METHOD_SWS_BICUBIC:
    tsprintf(algParam,_l(", parameter: %-5.2f"),-0.01f*bicubicParam);
    break;
   case METHOD_SWS_GAUSS:
    tsprintf(algParam,_l(", parameter: %-5.2f"),0.1f*gaussParam);
    break;
   case METHOD_SWS_LANCZOS:
    tsprintf(algParam,_l(", parameter: %-5.2f"),float(lanczosParam));
    break;
   default:
    algParam[0]='\0';
    break;
  }
 return algParam;
}
bool TresizeAspectSettings::getTip(unsigned int pageId,char_t *buf,size_t len)
{
 if (pageId==1)
  {
   char_t tipS[256];
   strcpy(tipS,_l("Resizing "));
   switch (_if)
    {
     case 0:
      strncat_s(tipS, countof(tipS), _l("always"), _TRUNCATE);
      break;
     case 1:
      strncatf(tipS, countof(tipS), _l("if source width is %s %i %s source height is %s %i"),getXcondStr(xcond),xval,getXYcondStr(xycond),getXcondStr(ycond),yval);
      break;
     case 2:
      strncatf(tipS, countof(tipS), _l("if number of pixels is %s %i"),getPixCondStr(pixcond),pixval);
      break;
    }
   strncat_s(tipS, countof(tipS), _l(" "), _TRUNCATE);
   switch (mode)
    {
     case 0:
      strncatf(tipS, countof(tipS),_l("to size %ix%i"),dx,dy);
      break;
     case 1:
      strncatf(tipS, countof(tipS),_l("to aspect ratio %i:%i"),a1,a2);
      break;
     case 2:
      strncatf(tipS, countof(tipS),_l("to next multiply of %i"),multOf);
      break;
     case 3:
      strncatf(tipS, countof(tipS),_l("by %g"),mult1000/1000.0f);
      break;
     case 4:
      strncatf(tipS, countof(tipS),_l("to horizontal size %i,vertical size=auto"),dx);
      break;
    }
   strncat_s(tipS, countof(tipS), _l("\nAspect ratio: "), _TRUNCATE);
   switch (isAspect)
    {
     case 0:
      strncat_s(tipS, countof(tipS), _l("no change"), _TRUNCATE);
      break;
     case 1:
      strncat_s(tipS, countof(tipS), _l("keeping original aspect ratio"), _TRUNCATE);
      break;
     case 2:
      strncatf(tipS, countof(tipS),_l("set to %3.2f:1"),aspectRatio/65536.0f);
      break;
    }
   ff_strncpy(buf,tipS,len);
   buf[len-1]='\0';
  }
 else if (pageId==2)
  {
   if (areBorders())
    tsnprintf_s(buf, len, _TRUNCATE, _l("%s borders %i%s,%i%s"), bordersInside?_l("Inside"):_l("Outside"),bordersUnits==0?bordersPercentX:bordersPixelsX,bordersUnits==0?_l("%"):_l(" pixels"),bordersUnits==0?bordersPercentY:bordersPixelsY,bordersUnits==0?_l("%"):_l(" pixels"));
   else
    tsnprintf_s(buf, len, _TRUNCATE,_l("No borders"));
   buf[len-1]='\0';
  }
 else if (pageId==3)
  {
   if (methodsProps[methodLuma].library==LIB_SWSCALER)
    {
     if (methodLuma==methodChroma || methodsLocked)
      {
       char_t algParam[256];
       tsnprintf_s(buf, len, _TRUNCATE,_l("Method: %s%s\n"),methodsProps[methodLuma].name,printSwsParams(algParam,methodLuma,bicubicLumaParam,gaussLumaParam,lanczosLumaParam));
      }
     else
      {
       char_t algParam[256];
       tsnprintf_s(buf, len, _TRUNCATE,_l("Luma method: %s%s\n"),methodsProps[methodLuma].name,printSwsParams(algParam,methodLuma,bicubicLumaParam,gaussLumaParam,lanczosLumaParam));
       strncatf(buf,len,_l("Chroma method: %s%s\n"),methodsProps[methodChroma].name,printSwsParams(algParam,methodChroma,bicubicChromaParam,gaussChromaParam,lanczosChromaParam));
      }
     strncatf(buf,len,_l("Luma gaussian blur:%3.2f, luma sharpen:%3.2f\nChroma gaussian blur:%3.2f, chroma sharpen:%3.2f"),GblurLum/100.0f,sharpenLum/100.0f,GblurChrom/100.0f,sharpenChrom/100.0f);
    }
   else if (methodsProps[methodLuma].library==LIB_SIMPLE)
    {
     tsnprintf_s(buf, len, _TRUNCATE,_l("Method: %s"),methodsProps[methodLuma].name);
     if (methodLuma==METHOD_WARPED)
      strncatf(buf,len,_l(", horizontal warp: %-5.3f, vertical warp: %-5.3f"),simpleWarpXparam/1000.0f,simpleWarpYparam/1000.0f);
    }
   else
    tsnprintf_s(buf, len, _TRUNCATE,_l("Method: %s"),methodsProps[methodLuma].name);
   buf[len-1]='\0';
  }
 return true;
}
const char_t* TresizeAspectSettings::getXcondStr(int cond)
{
 switch (cond)
  {
   case -1:return _l("<");
   case  1:return _l(">");
   default:return _l("=");
  }
}
const char_t* TresizeAspectSettings::getXYcondStr(int cond)
{
 switch (cond)
  {
   case 0 :return _l("and");
   case 1 :return _l("or");
   default:return _l("");
  }
}
const char_t* TresizeAspectSettings::getPixCondStr(int cond)
{
 switch (cond)
  {
   case -1:return _l("less than");
   case  1:return _l("greater than");
   default:return _l("");
  }
}

bool TresizeAspectSettings::ifResize(unsigned int AVIdx,unsigned int AVIdy) const
{
 switch (_if)
  {
   case 0:
    return true;
   case 1:
    {
     bool isX=(xcond==-1 && AVIdx<xval) || (xcond==1 && AVIdx>xval);
     bool isY=(ycond==-1 && AVIdy<yval) || (ycond==1 && AVIdy>yval);
     return (xycond==0 && (isX && isY)) || (xycond==1 && (isX || isY));
    }
   case 2:
    {
     unsigned int pixnum=AVIdx*AVIdy;
     return (pixcond==-1 && pixnum<pixval) || (pixcond==1 && pixnum>pixval);
    }
   default:return true;
  }
}

void TresizeAspectSettings::calcNewRects(Trect *rectFull, Trect *rectClip, bool flip) const
{ //TODO: fix this mess!
 const Trect inRect=*(full?rectFull:rectClip);

/*****************  resize  *****************/
 if (is && ifResize(inRect.dx,inRect.dy))
  if (methodsProps[methodLuma].library==LIB_SAI)
   {
    rectFull->dx*=2;rectFull->dy*=2;
    rectClip->x *=2;rectClip->y *=2;
    rectClip->dx*=2;rectClip->dy*=2;
    //return;
   }
  else
   {
    if (SARinternally)
     {
      rectFull->sar.num=1;
      rectFull->sar.den=1;
     }
    switch (mode)
     {
      case 1: // Specify aspect ratio
       Trect::calcNewSizeAspect(inRect,a1,a2,*rectFull);
       break;
      case 2: // Expand to next multiply of
       rectFull->dx=(1+(inRect.dx-1)/multOf)*multOf;
       rectFull->dy=(1+(inRect.dy-1)/multOf)*multOf;
       break;
      case 3: // Multiply by
       rectFull->dx=mult1000*inRect.dx/1000;
       rectFull->dy=mult1000*inRect.dy/1000;
       break;
      case 4: // Specify horizontal size
       rectFull->dx=dx;
       if (SARinternally)
        rectFull->dy=dx*inRect.dy/inRect.dx*inRect.sar.den/inRect.sar.num;
       else
        rectFull->dy=dx*inRect.dy/inRect.dx;
       break;
	  case 5: // Stick size to screen resolution
		rectFull->dx=GetSystemMetrics(SM_CXSCREEN);
		rectFull->dy=GetSystemMetrics(SM_CYSCREEN);
	   break;
      case 0: // Specify horizontal and vertical size
      default:
       rectFull->dx=dx;
       rectFull->dy=dy;
       break;
     }
    rectFull->dx&=~1;rectFull->dy&=~1;
   }

/*****************  aspect  *****************/
 int ax=1,ay=1;
 if (methodsProps[methodLuma].library==LIB_NONE)
  {
   rectClip->dx=std::min(inRect.dx,rectFull->dx);
   rectClip->x=std::max(0,int(rectFull->dx-rectClip->dx)/2)&~1;
   rectClip->dy=std::min(inRect.dy,rectFull->dy);
   rectClip->y=std::max(0,int(rectFull->dy-rectClip->dy)/2)&~1;
  }
 else
  if (isAspect==0 || methodsProps[methodLuma].library==LIB_SAI) // No aspect ratio correction
   *rectClip=*rectFull;
  else
   {
    if (isAspect==1) // keep original aspect ratio
     {
      if(SARinternally && is)
       {
        ax=inRect.dx;
        ay=inRect.dy*inRect.sar.den*outDeviceA1/inRect.sar.num/outDeviceA2;
        rectClip->sar.num=1;rectClip->sar.den=1;
       }
      else
       {
        ax=inRect.dx;
        ay=inRect.dy;
       }
     }
    else // Manual: x.xx:1
     {
      if (is)
       {
        ax=aspectRatio;
        ay=1<<16;
       }
      else
       {
        ax=inRect.dx;
        ay=inRect.dy;
       }
     }
    if (ax==0) ax=1;if (ay==0) ay=1;
    rectClip->dx=rectFull->dx;
    rectClip->dy=rectFull->dx*ay/ax;
    if (mode==4) rectFull->dy=rectClip->dy;
    if (rectClip->dy>rectFull->dy) // have to shrink horizontally
     {
      rectClip->dx=rectFull->dy*ax/ay;
      rectClip->dy=rectFull->dy;
     }
    unsigned int mask=isAspect==1?_UI32_MAX:~1;
    rectClip->dx&=mask;
    rectClip->dy&=mask;
    rectClip->x=((rectFull->dx-rectClip->dx)/2)&mask;
    rectClip->y=((rectFull->dy-rectClip->dy)/2)&mask;
   }

/*****************  borders  *****************/
  unsigned int borderX=0,borderY=0;
  if (bordersUnits==0) // Percent
   {
    borderX=std::max(64U,(bordersInside?rectClip:rectFull)->dx*(100+(bordersInside?-1:1)*bordersPercentX)/100);
    borderY=std::max(24U,(bordersInside?rectClip:rectFull)->dy*(100+(bordersInside?-1:1)*bordersPercentY)/100);
   }
  else if (bordersUnits==1) // Pixels
   {
    borderX=std::max(64,int(rectClip->dx+(bordersInside?-2:2)*bordersPixelsX));
    borderY=std::max(24,int(rectClip->dy+(bordersInside?-2:2)*(bordersLocked?rectClip->dy*bordersPixelsY/rectClip->dx:bordersPixelsY)));
   }
  borderX&=~1;
  borderY&=~1;
  if (borderX || borderY)
   {
    if (bordersInside && methodsProps[methodLuma].library!=LIB_SAI)
     {
      if (isAspect>=1)
       {
        unsigned int bdy=borderX*ay/ax;
        if (bdy<=borderY)
         {
          rectClip->dx=borderX;
          rectClip->dy=bdy&~1;
         }
        else
         {
          rectClip->dx=borderY*ax/ay;
          rectClip->dy=borderY;
         }
       }
      else
       {
        rectClip->dx=borderX;
        rectClip->dy=borderY;
       }
     }
    else
     {
      if (methodsProps[methodLuma].library!=LIB_SAI || (borderX>rectFull->dx && borderY>rectFull->dy))
       {
        rectFull->dx=borderX;
        rectFull->dy=borderY;
       }
     }
    if (rectClip->dx>rectFull->dx) rectClip->dx=rectFull->dx;
    if (rectClip->dy>rectFull->dy) rectClip->dy=rectFull->dy;
   }

/*****************  finish  *****************/
  if (rectClip->dx>rectFull->dx) rectClip->dx=rectFull->dx;
  if (rectClip->dy>rectFull->dy) rectClip->dy=rectFull->dy;
  if (rectClip->dx<64) rectClip->dx=64;
  if (rectClip->dy<24) rectClip->dy=24;
  if (rectFull->dx<64) rectFull->dx=64;
  if (rectFull->dy<24) rectFull->dy=24;
  rectClip->dx=rectClip->dx&~1;
  rectClip->dy=rectClip->dy&~1;
  rectFull->dx=rectFull->dx&~1;
  rectFull->dy=rectFull->dy&~1;
  rectClip->x=((rectFull->dx-rectClip->dx)*bordersDivX/100)&~1;
  if (flip)
   rectClip->y=((rectFull->dy-rectClip->dy)*(100-bordersDivY)/100)&~1;
  else
   rectClip->y=((rectFull->dy-rectClip->dy)*bordersDivY/100)&~1;
  if (rectClip->x+rectClip->dx > rectFull->dx) rectClip->x=(rectFull->dx-rectClip->dx)&~1;
  if (rectClip->y+rectClip->dy > rectFull->dy) rectClip->y=(rectFull->dy-rectClip->dy)&~1;
}

void TresizeAspectSettings::reg_op(TregOp &t)
{
 if (specifyHorizontalSizeOnly && mode==4) // for downgrade compatibility. (before save. This is executed before load too. No problem)
  mode=0;
 TfilterSettingsVideo::reg_op(t);
 dy=dyReg;
 if (specifyHorizontalSizeOnly && mode==0) // after load (Of course, this is executed after save. Can be igrored.)
  mode=4;
 else
  if (mode==4) mode=0;
 if (methodLuma==METHOD_SWS_BICUBLIN)
  {
   methodLuma=METHOD_SWS_BICUBIC,
   methodChroma=METHOD_SWS_BILINEAR;
   methodsLocked=0;
  }
 if (methodChroma==-1)
  {
   if (methodsProps[methodLuma].library==LIB_SWSCALER)
    methodChroma=methodLuma;
   else
    methodChroma=METHOD_SWS_BICUBIC;
   methodsLocked=1;
  }
 if (bicubicChromaParam==-1) bicubicChromaParam=bicubicLumaParam;
 if (gaussChromaParam==-1) gaussChromaParam=gaussLumaParam;
 if (lanczosChromaParam==-1) lanczosChromaParam=lanczosLumaParam;
}
