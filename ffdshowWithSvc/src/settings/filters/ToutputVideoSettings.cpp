/*
 * Copyright (c) 2002-2006 Milan Cutka
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
#include "Tconvert.h"
#include "ToutputVideoSettings.h"

const TfilterIDFF ToutputVideoSettings::idffs=
{
 /*name*/      _l("Output"),
 /*id*/        IDFF_filterOutputVideo,
 /*is*/        0,
 /*order*/     0,
 /*show*/      0,
 /*full*/      0,
 /*half*/      0,
 /*dlgId*/     0,
};

const char_t* ToutputVideoSettings::dvNorms[]=
{
 _l("PAL"),
 _l("NTSC"),
 _l("closest"),
 NULL
};

const char_t* ToutputVideoSettings::deintMethods[]=
{
 _l("Auto"),
 _l("Force weave"),
 _l("Force bob"),
 NULL
};

const char_t* ToutputVideoSettings::deintFieldOrder[]=
{
 _l("Auto"),
 _l("Top field first"),
 _l("Bottom field first"),
 NULL
};

const char_t* ToutputVideoSettings::rgbInterlaceMethods[]=
{
 _l("Auto"),
 _l("Force progressive"),
 _l("Force interlace"),
 NULL
};

ToutputVideoSettings::ToutputVideoSettings(TintStrColl *Icoll,TfilterIDFFs *filters):TfilterSettingsVideo(sizeof(*this),Icoll,filters,&idffs,false)
{
 half=0;full=1;
 static const TintOptionT<ToutputVideoSettings> iopts[]=
  {
   IDFF_flip               ,&ToutputVideoSettings::flip               ,0,0,_l(""),1,
     _l("flip"),0,

   IDFF_hwOverlayOld       ,&ToutputVideoSettings::hwOverlayOld       ,0,2,_l(""),1,
     _l("hwOverlay"),2,
   IDFF_setSARinOutSample  ,&ToutputVideoSettings::hwOverlay          ,0,2,_l(""),1,
     _l("setSARinOutSample"),TintOption::DEF_DYN,
   IDFF_hwOverlayAspect    ,&ToutputVideoSettings::hwOverlayAspect    ,1,1,_l(""),1,
     _l("hwOverlayAspect"),0,
   IDFF_hwDeinterlaceOld   ,&ToutputVideoSettings::hwDeinterlaceOld   ,0,0,_l(""),1,
     _l("hwDeinterlace"),0,
   IDFF_setDeintInOutSample,&ToutputVideoSettings::hwDeinterlace      ,0,0,_l(""),1,
     _l("setDeintInOutSample"),TintOption::DEF_DYN,
   IDFF_hwDeintMethod      ,&ToutputVideoSettings::hwDeintMethod      ,0,2,_l(""),1,
     _l("hwDeintMethod"),0,
   IDFF_hwDeintFieldOrder  ,&ToutputVideoSettings::hwDeintFieldOrder  ,0,2,_l(""),1,
     _l("hwDeintFieldOrder"),0,

   IDFF_outI420            ,&ToutputVideoSettings::i420               ,0,0,_l(""),0,
     _l("outI420"),0,
   IDFF_outYV12            ,&ToutputVideoSettings::yv12               ,0,0,_l(""),0,
     _l("outYV12"),1,
   IDFF_outYUY2            ,&ToutputVideoSettings::yuy2               ,0,0,_l(""),0,
     _l("outYUY2"),1,
   IDFF_outYVYU            ,&ToutputVideoSettings::yvyu               ,0,0,_l(""),0,
     _l("outYVYU"),1,
   IDFF_outUYVY            ,&ToutputVideoSettings::uyvy               ,0,0,_l(""),0,
     _l("outUYVY"),1,
   IDFF_outNV12            ,&ToutputVideoSettings::nv12               ,0,0,_l(""),0,
     _l("outNV12"),0,
   IDFF_outRGB32           ,&ToutputVideoSettings::rgb32              ,0,0,_l(""),0,
     _l("outRGB32"),1,
   IDFF_outRGB24           ,&ToutputVideoSettings::rgb24              ,0,0,_l(""),0,
     _l("outRGB24"),1,
   IDFF_outRGB555          ,&ToutputVideoSettings::rgb555             ,0,0,_l(""),0,
     _l("outRGB555"),1,
   IDFF_outRGB565          ,&ToutputVideoSettings::rgb565             ,0,0,_l(""),0,
     _l("outRGB565"),1,
   IDFF_outClosest         ,&ToutputVideoSettings::closest            ,0,0,_l(""),1,
     _l("outClosest"),1,
   IDFF_outDV              ,&ToutputVideoSettings::dv                 ,0,0,_l(""),0,
     _l("outDV"),0,
   IDFF_outDVnorm          ,&ToutputVideoSettings::dvNorm             ,0,2,_l(""),0,
     _l("outDVnorm"),2,
   IDFF_allowOutChange     ,&ToutputVideoSettings::allowOutChange3    ,0,2,_l(""),1,
     _l("allowOutChange"),2,
   IDFF_outChangeCompatOnly,&ToutputVideoSettings::outChangeCompatOnly,0,0,_l(""),1,
     _l("outChangeCompatOnly"),1,
   IDFF_avisynthYV12_RGB   ,&ToutputVideoSettings::avisynthYV12_RGB   ,0,0,_l(""),1,
     _l("avisynthYV12_RGB"),0,
   IDFF_cspOptionsIturBt                ,&ToutputVideoSettings::cspOptionsIturBt           ,0,0,_l(""),1,
     _l("cspOptionsIturBt"),TrgbPrimaries::ITUR_BT601,
   IDFF_cspOptionsCutoffMode            ,&ToutputVideoSettings::cspOptionsCutoffMode       ,0,0,_l(""),1,
     _l("cspOptionsCutoffMode"),TrgbPrimaries::RecYCbCr,
   IDFF_cspOptionsBlackCutoff           ,&ToutputVideoSettings::cspOptionsBlackCutoff      ,0,32,_l(""),1,
     _l("cspOptionsBlackCutoff"),16,
   IDFF_cspOptionsWhiteCutoff           ,&ToutputVideoSettings::cspOptionsWhiteCutoff      ,215,255,_l(""),1,
     _l("cspOptionsWhiteCutoff"),235,
   IDFF_cspOptionsChromaCutoff          ,&ToutputVideoSettings::cspOptionsChromaCutoff     ,1,32,_l(""),1,
     _l("cspOptionsChromaCutoff"),16,
   IDFF_cspOptionsInterlockChroma       ,&ToutputVideoSettings::cspOptionsInterlockChroma  ,0,0,_l(""),1,
     _l("cspOptionsInterlockChroma"),1,
   IDFF_cspOptionsRgbInterlaceMode      ,&ToutputVideoSettings::cspOptionsRgbInterlaceMode ,0,2,_l(""),1,
     _l("cspOptionsRgbInterlaceMode"),0,
   0
  };
 addOptions(iopts);
 static const TcreateParamList1 listDVnorm(dvNorms);
 setParamList(IDFF_outDVnorm,&listDVnorm);
 static const TcreateParamList1 listDeintMethods(deintMethods);
 setParamList(IDFF_hwDeintMethod,&listDeintMethods);
 static const TcreateParamList1 listDeintFieldOrder(deintFieldOrder);
 setParamList(IDFF_hwDeintFieldOrder,&listDeintFieldOrder);
 static const TcreateParamList1 listRgbInterlaceMethods(rgbInterlaceMethods);
 setParamList(IDFF_cspOptionsRgbInterlaceMode,&listRgbInterlaceMethods);
}

int ToutputVideoSettings::getDefault(int id)
{
 switch (id) // for upgrade. IDFF_setDeintInOutSample is now independent from IDFF_setSARinOutSample.
  {
   case IDFF_setSARinOutSample:return hwOverlayOld;
   case IDFF_setDeintInOutSample:return hwOverlayOld?hwDeinterlaceOld:0;
   default:return TfilterSettingsVideo::getDefault(id);
  }
}

void ToutputVideoSettings::reg_op_outcsps(TregOp &t)
{
 t._REG_OP_N(IDFF_outI420  ,_l("outI420")  ,i420  ,0);
 t._REG_OP_N(IDFF_outYV12  ,_l("outYV12")  ,yv12  ,1);
 t._REG_OP_N(IDFF_outYUY2  ,_l("outYUY2")  ,yuy2  ,1);
 t._REG_OP_N(IDFF_outYVYU  ,_l("outYVYU")  ,yvyu  ,1);
 t._REG_OP_N(IDFF_outUYVY  ,_l("outUYVY")  ,uyvy  ,1);
 t._REG_OP_N(IDFF_outNV12  ,_l("outNV12")  ,nv12  ,0);
 t._REG_OP_N(IDFF_outRGB32 ,_l("outRGB32") ,rgb32 ,1);
 t._REG_OP_N(IDFF_outRGB24 ,_l("outRGB24") ,rgb24 ,1);
 t._REG_OP_N(IDFF_outRGB555,_l("outRGB555"),rgb555,1);
 t._REG_OP_N(IDFF_outRGB565,_l("outRGB565"),rgb565,1);
 t._REG_OP_N(IDFF_outClosest,_l("outClosest"),closest,1);
 t._REG_OP_N(IDFF_setSARinOutSample,_l("setSARinOutSample"),hwOverlay,2);
 t._REG_OP_N(IDFF_hwOverlayAspect,_l("hwOverlayAspect"),hwOverlayAspect,0);
}

const int* ToutputVideoSettings::getResets(unsigned int pageId)
{
 static const int idResets[]={IDFF_flip,IDFF_outI420,IDFF_outYV12,IDFF_outYUY2,IDFF_outYVYU,IDFF_outUYVY,IDFF_outNV12,IDFF_outRGB32,IDFF_outRGB24,IDFF_outRGB555,IDFF_outDV,IDFF_outRGB565,IDFF_outClosest,IDFF_hwOverlay,IDFF_hwDeinterlace,IDFF_avisynthYV12_RGB,/*IDFF_PC_YUV,*/IDFF_allowOutChange,IDFF_outChangeCompatOnly,0};
 return idResets;
}

void ToutputVideoSettings::getOutputColorspaces(ints &ocsps)
{
 ocsps.clear();
 if (i420  && !(hwDeinterlace)) ocsps.push_back(FF_CSP_420P|FF_CSP_FLAGS_YUV_ORDER);
 if (yv12  && !(hwDeinterlace)) ocsps.push_back(FF_CSP_420P);
 if (yuy2  ) ocsps.push_back(FF_CSP_YUY2);
 if (yvyu  ) ocsps.push_back(FF_CSP_YVYU);
 if (uyvy  ) ocsps.push_back(FF_CSP_UYVY);
 if (nv12  ) ocsps.push_back(FF_CSP_NV12);
 if (rgb32 ) ocsps.push_back(FF_CSP_RGB32);
 if (rgb24 ) ocsps.push_back(FF_CSP_RGB24);
 if (rgb555) ocsps.push_back(FF_CSP_RGB15);
 if (rgb565) ocsps.push_back(FF_CSP_RGB16);
}

void ToutputVideoSettings::getOutputColorspaces(TcspInfos &ocsps)
{
 ints ocspsi;
 getOutputColorspaces(ocspsi);
 ocsps.clear();
 for (ints::const_iterator o=ocspsi.begin();o!=ocspsi.end();o++)
  ocsps.push_back(csp_getInfo(*o));
}

void ToutputVideoSettings::getDVsize(unsigned int *dx,unsigned int *dy) const
{
 switch (dvNorm)
  {
   case 0:pal:*dx=720;*dy=576;break;
   case 1:ntsc:*dx=720;*dy=480;break;
   case 2:
    {
     int dif1=sqr(*dx-720)+sqr(*dy-576);
     int dif2=sqr(*dx-720)+sqr(*dy-480);
     if (dif2<dif1) goto ntsc; else goto pal;
    }
  }
}

int ToutputVideoSettings::get_cspOptionsWhiteCutoff() const
{
 if (cspOptionsCutoffMode == TrgbPrimaries::RecYCbCr)
  return 235;
 else  if (cspOptionsCutoffMode == TrgbPrimaries::PcYCbCr)
  return 255;

 return cspOptionsWhiteCutoff;
}

int ToutputVideoSettings::get_cspOptionsBlackCutoff() const
{
 if (cspOptionsCutoffMode == TrgbPrimaries::RecYCbCr)
  return 16;
 else  if (cspOptionsCutoffMode == TrgbPrimaries::PcYCbCr)
  return 0;

 return cspOptionsBlackCutoff;
}

int ToutputVideoSettings::get_cspOptionsChromaCutoff() const
{
 if (cspOptionsCutoffMode == TrgbPrimaries::RecYCbCr)
  return 16;
 else  if (cspOptionsCutoffMode == TrgbPrimaries::PcYCbCr)
  return 1;

 return get_cspOptionsChromaCutoffStatic(cspOptionsBlackCutoff, cspOptionsWhiteCutoff, cspOptionsChromaCutoff, cspOptionsInterlockChroma);
}

int ToutputVideoSettings::get_cspOptionsChromaCutoffStatic(int blackCutoff, int whiteCutoff, int chromaCutoff, int lock)
{
 int result=16;
 if (lock)
  result = int(double(255-(whiteCutoff-blackCutoff))/36.0*16.0);
 else
  result = chromaCutoff;
 if (result < 1)
  result = 1;
 return result;
}

int ToutputVideoSettings::brightness2luma(int brightness) const
{
 int b = get_cspOptionsBlackCutoff();
 int w = get_cspOptionsWhiteCutoff();
 int luma = int(0.5 + b + brightness * (w - b) / 255.0);
 return limit(luma, 0, 255);
}

#if 0
int ToutputVideoSettings::luma2brightness(int luma) const
{
 double Kr;
 int b = get_cspOptionsBlackCutoff();
 int w = get_cspOptionsWhiteCutoff();
 int c = 128 - get_cspOptionsChromaCutoff();
 if (cspOptionsIturBt == TrgbPrimaries::ITUR_BT601)
  Kr = 0.299;
 else
  Kr = 0.2125;
 int brightness = int(255.0 / (w - b) * luma + 255.0 / c * 128.0 * (1 - Kr) - (255.0 * b / (w - b) + 255.0 * 128.0 / c * (1-Kr)));
 return limit(brightness, 0, 255);
}
#endif

