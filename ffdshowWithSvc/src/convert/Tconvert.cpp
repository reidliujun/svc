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
#include "IffdshowBase.h"
#include "IffdshowDecVideo.h"
#include "ffdshow_constants.h"
#include "Tconfig.h"
#include "Tconvert.h"
#include "ffImgfmt.h"
#include "convert_yv12.h"
#include "convert_yuy2.h"
#include "Tswscale.h"
#include "image.h"
#include "ffdebug.h"
#include "TffPict.h"
#include "Tlibmplayer.h"
#include "libavcodec/bitstream.h"
#include "ToutputVideoSettings.h"

//======================================= Tconvert =======================================
Tconvert::Tconvert(IffdshowBase *deci,unsigned int Idx,unsigned int Idy) :
 TrgbPrimaries(deci)
{
 Tlibmplayer *libmplayer;
 deci->getPostproc(&libmplayer);
 bool avisynthYV12_RGB = !!deci->getParam2(IDFF_avisynthYV12_RGB);
 int rgbInterlaceMode = deci->getParam2(IDFF_cspOptionsRgbInterlaceMode);
 init(libmplayer, avisynthYV12_RGB, Idx, Idy, rgbInterlaceMode);
}
Tconvert::Tconvert(Tlibmplayer *Ilibmplayer, bool IavisynthYV12_RGB, unsigned int Idx, unsigned int Idy, const TrgbPrimaries &IrgbPrimaries, int rgbInterlaceMode) :
 TrgbPrimaries(IrgbPrimaries)
{
 Ilibmplayer->AddRef();
 init(Ilibmplayer,IavisynthYV12_RGB,Idx,Idy,rgbInterlaceMode);
}
void Tconvert::init(Tlibmplayer *Ilibmplayer,bool IavisynthYV12_RGB,unsigned int Idx,unsigned int Idy, int IrgbInterlaceMode)
{
 libmplayer=Ilibmplayer;
 dx=Idx;
 dy=Idy&~1;
 outdy=Idy;
 swscale=NULL;
 oldincsp=oldoutcsp=-1;incspInfo=outcspInfo=NULL;
 initsws=true;
 tmp[0]=tmp[1]=tmp[2]=NULL;
 tmpConvert1=tmpConvert2=NULL;
 avisynthYV12_RGB=IavisynthYV12_RGB;
 rgbInterlaceMode = IrgbInterlaceMode;
}

Tconvert::~Tconvert()
{
 freeTmpConvert();
 if (swscale) delete swscale;
 libmplayer->Release();
}

const char_t* Tconvert::getModeName(int mode)
{
 switch (mode)
  {
   case MODE_none:return _l("none");
   case MODE_avisynth_yv12_to_yuy2:return _l("avisynth_yv12_to_yuy2");
   case MODE_xvidImage_output:return _l("xvidImage_output");
   case MODE_avisynth_yuy2_to_yv12:return _l("avisynth_yuy2_to_yv12");
   case MODE_mmx_ConvertRGB32toYUY2:return _l("mmx_ConvertRGB32toYUY2");
   case MODE_mmx_ConvertRGB24toYUY2:return _l("mmx_ConvertRGB24toYUY2");
   case MODE_mmx_ConvertYUY2toRGB32:return _l("mmx_ConvertYUY2toRGB32");
   case MODE_mmx_ConvertYUY2toRGB24:return _l("mmx_ConvertYUY2toRGB24");
   case MODE_mmx_ConvertUYVYtoRGB32:return _l("mmx_ConvertUYVYtoRGB32");
   case MODE_mmx_ConvertUYVYtoRGB24:return _l("mmx_ConvertUYVYtoRGB24");
   case MODE_palette8torgb:return _l("palette8torgb");
   case MODE_CLJR:return _l("CLJR");
   case MODE_xvidImage_input:return _l("xvidImage_input");
   case MODE_swscale:return _l("swscale");
   case MODE_avisynth_bitblt:return _l("avisynth_bitblt");
   case MODE_fallback:return _l("fallback");
   default:return _l("unknown");
  }
}

void Tconvert::freeTmpConvert(void)
{
 if (tmp[0]) aligned_free(tmp[0]);tmp[0]=NULL;
 if (tmp[1]) aligned_free(tmp[1]);tmp[1]=NULL;
 if (tmp[2]) aligned_free(tmp[2]);tmp[2]=NULL;
 if (tmpConvert1) delete tmpConvert1;tmpConvert1=NULL;
 if (tmpConvert2) delete tmpConvert2;tmpConvert2=NULL;
}

int Tconvert::convert(int incsp0,const uint8_t*const src0[],const stride_t srcStride0[],int outcsp0,uint8_t* dst0[],stride_t dstStride0[],const Tpalette *srcpal)
{
 bool wasChange;
 int incsp=incsp0,outcsp=outcsp0;
 if (rgbInterlaceMode == 1) // Force Interlace
  {
   incsp |= FF_CSP_FLAGS_INTERLACED;
   outcsp0 |= FF_CSP_FLAGS_INTERLACED;
   outcsp |= FF_CSP_FLAGS_INTERLACED;
  }
 else if (rgbInterlaceMode == 2) // Force progressive
  {
   incsp &= ~FF_CSP_FLAGS_INTERLACED;
   outcsp &= ~FF_CSP_FLAGS_INTERLACED;
   outcsp0 &= ~FF_CSP_FLAGS_INTERLACED;
  }

 if (!incspInfo || incspInfo->id!=(incsp&FF_CSPS_MASK) || !outcspInfo || outcspInfo->id!=(outcsp&FF_CSPS_MASK))
  {
   incspInfo=csp_getInfo(incsp);
   outcspInfo=csp_getInfo(outcsp);
   wasChange=true;
  }
 else wasChange=false;

 const unsigned char *src[]={src0[0],src0[1],src0[2],src0[3]};
 stride_t srcStride[]={srcStride0[0],srcStride0[1],srcStride0[2],srcStride0[3]};
 csp_yuv_adj_to_plane(incsp ,incspInfo ,dy,(unsigned char**)src,srcStride);
 csp_yuv_order(incsp ,(unsigned char**)src,srcStride);
 csp_vflip(incsp ,incspInfo, (unsigned char**)src,srcStride,dy);

 unsigned char *dst[]={dst0[0],dst0[1],dst0[2],dst0[3]};
 stride_t dstStride[]={dstStride0[0],dstStride0[1],dstStride0[2],dstStride0[3]};
 if(outcspInfo->id==FF_CSP_420P)
  csp_yuv_adj_to_plane(outcsp,outcspInfo,ODD2EVEN(outdy),(unsigned char**)dst,dstStride);
 else
  csp_yuv_adj_to_plane(outcsp,outcspInfo,dy,(unsigned char**)dst,dstStride);
 csp_yuv_order(outcsp,(unsigned char**)dst,dstStride);
 csp_vflip(outcsp,outcspInfo,(unsigned char**)dst,dstStride,dy);

 if (wasChange || oldincsp!=incsp || oldoutcsp!=outcsp)
  {
   oldincsp=incsp;oldoutcsp=outcsp;
   freeTmpConvert();
   incsp1=incsp&FF_CSPS_MASK;outcsp1=outcsp&FF_CSPS_MASK;
   mode=MODE_none;
   #ifdef AVISYNTH_BITBLT
   if (incsp1==outcsp1)
    {
     rowsize=dx*incspInfo->Bpp;
     mode=MODE_avisynth_bitblt;
    }
   else
   #else
    switch (incsp1)
     {
      case FF_CSP_420P:
       switch (outcsp1)
        {
         case FF_CSP_YUY2: //YV12 -> YUY2
          mode=MODE_avisynth_yv12_to_yuy2;
          if (incsp & FF_CSP_FLAGS_INTERLACED)
         #ifdef __SSE2__
           if (Tconfig::cpu_flags&FF_CPU_SSE2)
            avisynth_yv12_to_yuy2=TconvertYV12<Tsse2>::yv12_i_to_yuy2;
           else
         #endif
           if (Tconfig::cpu_flags&FF_CPU_MMXEXT)
            avisynth_yv12_to_yuy2=TconvertYV12<Tmmxext>::yv12_i_to_yuy2;
           else
            avisynth_yv12_to_yuy2=TconvertYV12<Tmmx>::yv12_i_to_yuy2;
          else
         #ifdef __SSE2__
           if (Tconfig::cpu_flags&FF_CPU_SSE2)
            avisynth_yv12_to_yuy2=TconvertYV12<Tsse2>::yv12_to_yuy2;
           else
         #endif
           if (Tconfig::cpu_flags&FF_CPU_MMXEXT)
            avisynth_yv12_to_yuy2=TconvertYV12<Tmmxext>::yv12_to_yuy2;
           else
            avisynth_yv12_to_yuy2=TconvertYV12<Tmmx>::yv12_to_yuy2;
          break;
         default:
          if (avisynthYV12_RGB && (outcsp1==FF_CSP_RGB24 || outcsp1==FF_CSP_RGB32))
           {
            mode=MODE_fallback;
            tmpcsp=FF_CSP_YUY2;
            tmpStride[0]=2*(dx/16+2)*16;
            tmp[0]=(unsigned char*)aligned_malloc(tmpStride[0]*dy);
            tmpConvert1=new Tconvert(libmplayer,avisynthYV12_RGB,dx,dy,*this,rgbInterlaceMode);
            tmpConvert2=new Tconvert(libmplayer,avisynthYV12_RGB,dx,dy,*this,rgbInterlaceMode);
            if (incsp&FF_CSP_FLAGS_INTERLACED || outcsp&FF_CSP_FLAGS_INTERLACED) tmpcsp|=FF_CSP_FLAGS_INTERLACED;
            if ((incsp | outcsp) & FF_CSP_FLAGS_YUV_JPEG)
             tmpcsp |= FF_CSP_FLAGS_YUV_JPEG;
           }
          else
           if (csp_supXvid(outcsp1)
            #ifndef XVID_BITBLT
             && outcsp1!=FF_CSP_420P
            #endif
             )
            mode=MODE_xvidImage_output;
          break;
        }
       break;
      case FF_CSP_YUY2:
       switch (outcsp1)
        {
         case FF_CSP_420P: // YUY2 -> YV12
          mode=MODE_avisynth_yuy2_to_yv12;
          if (incsp&FF_CSP_FLAGS_INTERLACED)
         #ifdef __SSE2__
           if (Tconfig::cpu_flags&FF_CPU_SSE2)
            avisynth_yuy2_to_yv12=TconvertYV12<Tsse2>::yuy2_i_to_yv12;
           else
         #endif
           if (Tconfig::cpu_flags&FF_CPU_MMXEXT)
            avisynth_yuy2_to_yv12=TconvertYV12<Tmmxext>::yuy2_i_to_yv12;
           else
            avisynth_yuy2_to_yv12=TconvertYV12<Tmmx>::yuy2_i_to_yv12;
          else
         #ifdef __SSE2__
           if (Tconfig::cpu_flags&FF_CPU_SSE2)
            avisynth_yuy2_to_yv12=TconvertYV12<Tsse2>::yuy2_to_yv12;
           else
         #endif
           if (Tconfig::cpu_flags&FF_CPU_MMXEXT)
            avisynth_yuy2_to_yv12=TconvertYV12<Tmmxext>::yuy2_to_yv12;
           else
            avisynth_yuy2_to_yv12=TconvertYV12<Tmmx>::yuy2_to_yv12;
          break;
         case FF_CSP_RGB24:
           mode=MODE_mmx_ConvertYUY2toRGB24; // YUY2 -> RGB24
          break;
         case FF_CSP_RGB32:
          mode=MODE_mmx_ConvertYUY2toRGB32; // YUY2 -> RGB32
          break;
        }
       break;
      case FF_CSP_UYVY:
       switch (outcsp1)
        {
         case FF_CSP_RGB24:
          mode=MODE_mmx_ConvertUYVYtoRGB24; // UYVY -> RGB24
          break;
         case FF_CSP_RGB32:
          mode=MODE_mmx_ConvertUYVYtoRGB32; // UYVY -> RGB32
          break;
        }
       break;
      case FF_CSP_RGB32:
       if (outcsp1==FF_CSP_YUY2) // RGB32 -> YUY2
        {
         mode=MODE_mmx_ConvertRGB32toYUY2;
         break;
        }
       if (outcsp1==FF_CSP_NV12) // RGB32 -> YV12 -> NV12
        {
         mode=MODE_fallback;
         tmpcsp=FF_CSP_420P;
         tmpStride[1]=tmpStride[2]=(tmpStride[0]=(dx/16+2)*16)/2;
         tmp[0]=(unsigned char*)aligned_malloc(tmpStride[0]*dy  );
         tmp[1]=(unsigned char*)aligned_malloc(tmpStride[1]*dy/2);
         tmp[2]=(unsigned char*)aligned_malloc(tmpStride[2]*dy/2);
         tmpConvert1=new Tconvert(libmplayer,avisynthYV12_RGB,dx,dy,*this,rgbInterlaceMode);
         tmpConvert2=new Tconvert(libmplayer,avisynthYV12_RGB,dx,dy,*this,rgbInterlaceMode);
         break;
        }
       if (outcsp1 == FF_CSP_UYVY || outcsp1 == FF_CSP_YVYU) // RGB32 -> YUY2 -> UYVY/YVYU
        {
         mode=MODE_fallback;
         tmpcsp=FF_CSP_YUY2;
         tmpStride[0]=2*(dx/16+2)*16;
         tmp[0]=(unsigned char*)aligned_malloc(tmpStride[0]*dy);
         tmpConvert1=new Tconvert(libmplayer,avisynthYV12_RGB,dx,dy,*this,rgbInterlaceMode);
         tmpConvert2=new Tconvert(libmplayer,avisynthYV12_RGB,dx,dy,*this,rgbInterlaceMode);
         break;
        }
       break;
      case FF_CSP_RGB24:
       if (outcsp1==FF_CSP_YUY2) // RGB24 -> YUY2
        {
         mode=MODE_mmx_ConvertRGB24toYUY2;
         break;
        }
       if (outcsp1==FF_CSP_NV12) // RGB24 -> YV12 -> NV12
        {
         mode=MODE_fallback;
         tmpcsp=FF_CSP_420P;
         tmpStride[1]=tmpStride[2]=(tmpStride[0]=(dx/16+2)*16)/2;
         tmp[0]=(unsigned char*)aligned_malloc(tmpStride[0]*dy  );
         tmp[1]=(unsigned char*)aligned_malloc(tmpStride[1]*dy/2);
         tmp[2]=(unsigned char*)aligned_malloc(tmpStride[2]*dy/2);
         tmpConvert1=new Tconvert(libmplayer,avisynthYV12_RGB,dx,dy,*this,rgbInterlaceMode);
         tmpConvert2=new Tconvert(libmplayer,avisynthYV12_RGB,dx,dy,*this,rgbInterlaceMode);
         break;
        }
       if (outcsp1 == FF_CSP_UYVY || outcsp1 == FF_CSP_YVYU) // RGB24 -> YUY2 -> UYVY/YVYU
        {
         mode=MODE_fallback;
         tmpcsp=FF_CSP_YUY2;
         tmpStride[0]=2*(dx/16+2)*16;
         tmp[0]=(unsigned char*)aligned_malloc(tmpStride[0]*dy);
         tmpConvert1=new Tconvert(libmplayer,avisynthYV12_RGB,dx,dy,*this,rgbInterlaceMode);
         tmpConvert2=new Tconvert(libmplayer,avisynthYV12_RGB,dx,dy,*this,rgbInterlaceMode);
         break;
        }
       break;
      case FF_CSP_PAL8:
       switch (outcsp1)
        {
         // libmplayer refers to the write order, FF_CSP_ enum refers to the "memory byte order", 
         // which under x86 is reversed, see the comment above the FF_CSP_ enum definition.
         case FF_CSP_RGB32:palette8torgb=libmplayer->palette8tobgr32;break;
         case FF_CSP_BGR32:palette8torgb=libmplayer->palette8torgb32;break;
         case FF_CSP_RGB24:palette8torgb=libmplayer->palette8tobgr24;break;
         case FF_CSP_BGR24:palette8torgb=libmplayer->palette8torgb24;break;
         case FF_CSP_RGB16:palette8torgb=libmplayer->palette8tobgr16;break;
         case FF_CSP_BGR16:palette8torgb=libmplayer->palette8torgb16;break;
         case FF_CSP_RGB15:palette8torgb=libmplayer->palette8tobgr15;break;
         case FF_CSP_BGR15:palette8torgb=libmplayer->palette8torgb15;break;
         default:palette8torgb=NULL;break;
        }
       if (palette8torgb)
        mode=MODE_palette8torgb;
       break;
      case FF_CSP_CLJR:
       if (outcsp1==FF_CSP_420P)
        mode=MODE_CLJR;
       break;
     }
   #endif
   if (mode==MODE_none)
    if (incsp1!=FF_CSP_420P && csp_supXvid(incsp1)/* && incsp1!=FF_CSP_RGB24 */&& outcsp1==FF_CSP_420P) // x -> YV12
     mode=MODE_xvidImage_input;
    else if (csp_supSWSin(incsp1) && csp_supSWSout(outcsp1))
     {
      if (!swscale) swscale=new Tswscale(libmplayer);
      UpdateSettings();
      setJpeg(!!((incsp | outcsp) & FF_CSP_FLAGS_YUV_JPEG));
      swscale->init(dx,dy,incsp,outcsp,toSwscaleTable());
      mode=MODE_swscale;
     }
    else
     {
      mode=MODE_fallback;
      tmpcsp=(incsp1==FF_CSP_PAL8)?FF_CSP_RGB32:FF_CSP_420P;
      if (tmpcsp==FF_CSP_RGB32)
       {
        tmpStride[0]=4*(dx/16+2)*16;
        tmp[0]=(unsigned char*)aligned_malloc(tmpStride[0]*dy);
       }
      else
       {
        tmpStride[1]=tmpStride[2]=(tmpStride[0]=(dx/16+2)*16)/2;
        tmp[0]=(unsigned char*)aligned_malloc(tmpStride[0]*dy  );
        tmp[1]=(unsigned char*)aligned_malloc(tmpStride[1]*dy/2);
        tmp[2]=(unsigned char*)aligned_malloc(tmpStride[2]*dy/2);
       }
      tmpConvert1=new Tconvert(libmplayer,avisynthYV12_RGB,dx,dy,*this,rgbInterlaceMode);
      tmpConvert2=new Tconvert(libmplayer,avisynthYV12_RGB,dx,dy,*this,rgbInterlaceMode);
      if (incsp&FF_CSP_FLAGS_INTERLACED || outcsp&FF_CSP_FLAGS_INTERLACED) tmpcsp|=FF_CSP_FLAGS_INTERLACED;
     }

#ifdef DEBUG
   char_t incspS[256],outcspS[256];
   DPRINTF(_l("colorspace conversion: %s -> %s (%s)"),csp_getName(incsp0,incspS,256),csp_getName(outcsp0,outcspS,256),getModeName(mode));
#endif

   if (mode==MODE_xvidImage_input || mode==MODE_xvidImage_output)
    {
     static bool inited=false;
     if (!inited)
      {
       inited=true;
       initXvid();
      }
    }
  }
 if (outcsp==FF_CSP_NULL) return 0;

 switch (mode)
  {
   #ifdef AVISYNTH_BITBLT
   case MODE_avisynth_bitblt:
    {
     for (unsigned int i=0;i<incspInfo->numPlanes;i++)
      TffPict::copy(dst[i],dstStride[i],src[i],srcStride[i],rowsize>>incspInfo->shiftX[i],dy>>incspInfo->shiftY[i]);
     return dy;
    }
   #endif
   case MODE_avisynth_yv12_to_yuy2:
    avisynth_yv12_to_yuy2(src[0],src[1],src[2],dx,srcStride[0],srcStride[1],
                          dst[0],dstStride[0],
                          dy);
    return dy;
   case MODE_xvidImage_output:
    {
     UpdateSettings();
     setJpeg(!!((incsp | outcsp) & FF_CSP_FLAGS_YUV_JPEG));
     writeToXvidYuv2RgbMatrix(&yv12_to_bgr_mmx_data);
     IMAGE srcPict;srcPict.y=(unsigned char*)src[0];srcPict.u=(unsigned char*)src[1];srcPict.v=(unsigned char*)src[2];
     image_output(&srcPict,
                  dx,outdy,srcStride,
                  dst,dstStride,
                  outcsp&~FF_CSP_FLAGS_INTERLACED,outcsp&FF_CSP_FLAGS_INTERLACED,(incsp|outcsp)&FF_CSP_FLAGS_YUV_JPEG);
     return outdy;
    }
   case MODE_avisynth_yuy2_to_yv12:
    avisynth_yuy2_to_yv12(src[0],dx*2,srcStride[0],
                          dst[0],dst[1],dst[2],dstStride[0],dstStride[1],
                          dy);
    return dy;
   case MODE_mmx_ConvertRGB32toYUY2:
    {
     UpdateSettings();
     setJpeg(!!((incsp | outcsp) & FF_CSP_FLAGS_YUV_JPEG));
     const Tmmx_ConvertRGBtoYUY2matrix *matrix=getAvisynthRgb2YuvMatrix();
     Tmmx_ConvertRGBtoYUY2<false,false>::mmx_ConvertRGBtoYUY2(src[0],dst[0],srcStride[0],dstStride[0],dx,dy,matrix);
     return dy;
    }
   case MODE_mmx_ConvertRGB24toYUY2:
    {
     UpdateSettings();
     setJpeg(!!((incsp | outcsp) & FF_CSP_FLAGS_YUV_JPEG));
     const Tmmx_ConvertRGBtoYUY2matrix *matrix=getAvisynthRgb2YuvMatrix();
     Tmmx_ConvertRGBtoYUY2<true ,false>::mmx_ConvertRGBtoYUY2(src[0],dst[0],srcStride[0],dstStride[0],dx,dy,matrix);
     return dy;
    }
   case MODE_mmx_ConvertYUY2toRGB24:
    {
     UpdateSettings();
     setJpeg(!!((incsp | outcsp) & FF_CSP_FLAGS_YUV_JPEG));
     const unsigned char *matrix=getAvisynthYuv2RgbMatrix();
     Tmmx_ConvertYUY2toRGB<0,0>::mmx_ConvertYUY2toRGB(src[0],dst[0],src[0]+dy*srcStride[0],srcStride[0],dstStride[0],dx*2,matrix);
     return dy;
    }
   case MODE_mmx_ConvertYUY2toRGB32:
    {
     UpdateSettings();
     setJpeg(!!((incsp | outcsp) & FF_CSP_FLAGS_YUV_JPEG));
     const unsigned char *matrix=getAvisynthYuv2RgbMatrix();
     Tmmx_ConvertYUY2toRGB<0,1>::mmx_ConvertYUY2toRGB(src[0],dst[0],src[0]+dy*srcStride[0],srcStride[0],dstStride[0],dx*2,matrix);
     return dy;
    }
   case MODE_mmx_ConvertUYVYtoRGB24:
    {
     UpdateSettings();
     setJpeg(!!((incsp | outcsp) & FF_CSP_FLAGS_YUV_JPEG));
     const unsigned char *matrix=getAvisynthYuv2RgbMatrix();
     Tmmx_ConvertYUY2toRGB<1,0>::mmx_ConvertYUY2toRGB(src[0],dst[0],src[0]+dy*srcStride[0],srcStride[0],dstStride[0],dx*2,matrix);
     return dy;
    }
   case MODE_mmx_ConvertUYVYtoRGB32:
    {
     UpdateSettings();
     setJpeg(!!((incsp | outcsp) & FF_CSP_FLAGS_YUV_JPEG));
     const unsigned char *matrix=getAvisynthYuv2RgbMatrix();
     Tmmx_ConvertYUY2toRGB<1,1>::mmx_ConvertYUY2toRGB(src[0],dst[0],src[0]+dy*srcStride[0],srcStride[0],dstStride[0],dx*2,matrix);
     return dy;
    }
   case MODE_palette8torgb:
    {
     if (srcpal && srcpal->pal)
      for (unsigned int y=0;y<dy;y++)
       palette8torgb(src[0]+srcStride[0]*y,dst[0]+dstStride[0]*y,dx,srcpal->pal);
     return dy;
    }
   case MODE_CLJR:
    {
     // Copyright (c) 2003 Alex Beregszaszi
     GetBitContext gb;
     init_get_bits(&gb,src[0],int(srcStride[0]*dy*8));
     for (unsigned int y=0;y<dy;y++)
      {
       uint8_t *luma=&dst[0][y*dstStride[0]];
       uint8_t *cb  =&dst[1][(y/2)*dstStride[1]];
       uint8_t *cr  =&dst[2][(y/2)*dstStride[2]];
       for (unsigned int x=0;x<dx;x+=4)
        {
         luma[3]=uint8_t(get_bits(&gb,5)<<3);
         luma[2]=uint8_t(get_bits(&gb,5)<<3);
         luma[1]=uint8_t(get_bits(&gb,5)<<3);
         luma[0]=uint8_t(get_bits(&gb,5)<<3);
         luma+=4;
         cb[0]=cb[1]=uint8_t(get_bits(&gb,6)<<2);cb+=2;
         cr[0]=cr[1]=uint8_t(get_bits(&gb,6)<<2);cr+=2;
        }
      }
     return dy;
    }
   case MODE_xvidImage_input:
    {
     UpdateSettings();
     setJpeg(!!((incsp | outcsp) & FF_CSP_FLAGS_YUV_JPEG));
     writeToXvidRgb2YuvMatrix(&bgr_to_yv12_mmx_data);
     IMAGE dstPict={dst[0],dst[1],dst[2]};
     image_input(&dstPict,dx,dy,dstStride[0],dstStride[1],src[0],srcStride[0],incsp,incsp&FF_CSP_FLAGS_INTERLACED,(incsp|outcsp)&FF_CSP_FLAGS_YUV_JPEG);
     return dy;
    }
   case MODE_swscale:
    swscale->convert(src,srcStride,dst,dstStride);
    return dy;
   case MODE_fallback:
    tmpConvert1->convert(incsp,src,srcStride,tmpcsp,tmp,tmpStride,srcpal);
    tmpConvert2->convert(tmpcsp,(const uint8_t**)tmp,tmpStride,outcsp,dst,dstStride,NULL);
    return dy;
   default:
    return 0;
  }
}
int Tconvert::convert(const TffPict &pict,int outcsp,uint8_t* dst[],stride_t dstStride[])
{
 return convert(pict.csp|((pict.fieldtype&FIELD_TYPE::MASK_INT)?FF_CSP_FLAGS_INTERLACED:0),pict.data,pict.stride,outcsp,dst,dstStride,&pict.palette);
}

//================================= TffColorspaceConvert =================================
CUnknown* WINAPI TffColorspaceConvert::CreateInstance(LPUNKNOWN punk,HRESULT *phr)
{
 TffColorspaceConvert *pNewObject=new TffColorspaceConvert(punk,phr);
 if (pNewObject==NULL)
  *phr=E_OUTOFMEMORY;
 return pNewObject;
}
STDMETHODIMP TffColorspaceConvert::NonDelegatingQueryInterface(REFIID riid,void **ppv)
{
 CheckPointer(ppv, E_POINTER);
 if (riid==IID_IffColorspaceConvert)
  return GetInterface<IffColorspaceConvert>(this,ppv);
 else
  return CUnknown::NonDelegatingQueryInterface(riid,ppv);
}

TffColorspaceConvert::TffColorspaceConvert(LPUNKNOWN punk,HRESULT *phr):
 CUnknown(NAME("TffColorspaceConvert"),punk,phr),
 c(NULL),
 config(new Tconfig(g_hInst,0xff))
{
 libmplayer=new Tlibmplayer(config);libmplayer->AddRef();
}
TffColorspaceConvert::~TffColorspaceConvert()
{
 if (c) delete c;
 libmplayer->Release();
 delete config;
}

STDMETHODIMP TffColorspaceConvert::allocPicture(int csp,unsigned int dx,unsigned int dy,uint8_t *data[],stride_t stride[])
{
 if (!dx || !dy || csp==FF_CSP_NULL) return E_INVALIDARG;
 if (!data || !stride) return E_POINTER;
 Tbuffer buf;
 buf.free=false;
 TffPict pict;
 pict.alloc(dx,dy,csp,buf);
 for (unsigned int i=0;i<pict.cspInfo.numPlanes;i++)
  {
   data[i]=pict.data[i];
   stride[i]=pict.stride[i];
  }
 return S_OK;
}
STDMETHODIMP TffColorspaceConvert::freePicture(uint8_t *data[])
{
 if (!data) return E_POINTER;
 if (data[0])
  aligned_free(data[0]);
 return S_OK;
}

STDMETHODIMP TffColorspaceConvert::convert(unsigned int dx,unsigned int dy,int incsp,uint8_t *src[],const stride_t srcStride[],int outcsp,uint8_t *dst[],stride_t dstStride[])
{
 return convertPalette(dx,dy,incsp,src,srcStride,outcsp,dst,dstStride,NULL,0);
}
STDMETHODIMP TffColorspaceConvert::convertPalette(unsigned int dx,unsigned int dy,int incsp,uint8_t *src[],const stride_t srcStride[],int outcsp,uint8_t *dst[],stride_t dstStride[],const unsigned char *pal,unsigned int numcolors)
{
 if (!c || c->dx!=dx || c->dy!=dy)
  {
   if (c) delete c;
   c=new Tconvert(libmplayer,false,dx,dy,TrgbPrimaries(),0);
  }
 Tpalette p(pal,numcolors);
 c->convert(incsp,src,srcStride,outcsp,dst,dstStride,&p);
 return S_OK;
}
