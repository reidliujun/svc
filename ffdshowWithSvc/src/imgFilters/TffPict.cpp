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
#include "TffRect.h"
#include "TffPict.h"
#include "Tconvert.h"
#include "Tconfig.h"
#include "ffImgfmt.h"
#include "Tlibavcodec.h"
#include "IffdshowBase.h"
#include "simd.h"
#include "md5sum.h"

int FRAME_TYPE::fromSample(IMediaSample *pIn)
{
 return (!pIn || pIn->IsSyncPoint()==S_OK)?I:P;
}
int FIELD_TYPE::fromSample(IMediaSample *pIn,bool isInterlacedRawVideo)
{
 if (pIn && isInterlacedRawVideo)
  if (comptrQ<IMediaSample2> pIn2=pIn)
   {
    AM_SAMPLE2_PROPERTIES inProp2;
    if (SUCCEEDED(pIn2->GetProperties(FIELD_OFFSET(AM_SAMPLE2_PROPERTIES,tStart),(PBYTE)&inProp2)))
     {
      // Set interlace information (every sample)
      if (!(inProp2.dwTypeSpecificFlags&AM_VIDEO_FLAG_WEAVE))
       if (inProp2.dwTypeSpecificFlags&AM_VIDEO_FLAG_FIELD1FIRST)
        return INT_TFF;
       else
        return INT_BFF;
     }
   }
 return PROGRESSIVE_FRAME;
}

//===================================== Tpalette ====================================
Tpalette::Tpalette(const AVPaletteControl *palctrl)
{
 pal=palctrl?(const unsigned char*)palctrl->palette:NULL;
 numcolors=palctrl?256:0;
}

//=================================== TffPictBase ===================================
TffPictBase::TffPictBase(unsigned int Idx,unsigned int Idy)
{
 csp=0;
 setSize(Idx,Idy);
}
void TffPictBase::setSize(unsigned int Idx,unsigned int Idy)
{
 rectFull=rectClip=Trect(0,0,Idx,Idy);
 memset(diff,0,sizeof(diff));
}
void TffPictBase::setDar(const Rational &dar)
{
 rectFull.setDar(dar);
 rectClip.setDar(dar);
}
void TffPictBase::setSar(const Rational &sar)
{
 rectFull.sar=sar;
 rectClip.sar=sar;
}

Trect TffPictBase::getRect(int full,int half) const
{
 Trect rect=full?rectFull:rectClip;
 if (half) {rect.dx/=2;rect.x+=rect.dx;}
 return rect;
}

//===================================== TffPict =====================================
TffPict::TffPict(void)
{
 init();
}
void TffPict::init(void)
{
 csp=FF_CSP_NULL;memset(&cspInfo,0,sizeof(cspInfo));
 memset(data,0,sizeof(data));memset(stride,0,sizeof(stride));memset(ro,0,sizeof(ro));memset(diff,0,sizeof(diff));
 frametype=FRAME_TYPE::UNKNOWN;fieldtype=FIELD_TYPE::PROGRESSIVE_FRAME;
 rtStart=rtStop=mediatimeStart=mediatimeStop=REFTIME_INVALID;
 gmcWarpingPoints=gmcWarpingPointsReal=0;
 edge=0;
 discontinuity = false;
}
void TffPict::init(int Icsp,unsigned char *Idata[4],const stride_t Istride[4],const Trect &r,bool Iro,int Iframetype,int Ifieldtype,size_t IsrcSize,const Tpalette &Ipalette)
{
 cspInfo=*csp_getInfo(csp=Icsp);
 frametype=Iframetype;fieldtype=Ifieldtype;
 unsigned int i;
 for (i=0;i<cspInfo.numPlanes;i++)
  {
   data[i]=Idata[i];
   stride[i]=Istride[i];
   ro[i]=Iro;
   diff[i]=0;
  }
 for (;i<4;i++)
  {
   data[i]=NULL;
   stride[i]=diff[i]=0;
   ro[i]=false;
  }
 palette=Ipalette;
 rectFull=rectClip=r;
 rtStart=rtStop=mediatimeStart=mediatimeStop=REFTIME_INVALID;
 gmcWarpingPoints=gmcWarpingPointsReal=0;
 srcSize=IsrcSize;
 edge=0;
 discontinuity = false;
}
TffPict::TffPict(int Icsp,unsigned char *Idata[4],const stride_t Istride[4],const Trect &r,bool Iro,int Iframetype,int Ifieldtype,size_t IsrcSize,IMediaSample *pIn,const Tpalette &Ipalette)
{
 init(Icsp,Idata,Istride,r,Iro,Iframetype,Ifieldtype,IsrcSize,Ipalette);
 if (pIn)
  {
   setTimestamps(pIn);
   setDiscontinuity(pIn);
  }
}
TffPict::TffPict(int Icsp,unsigned char *data[4],const stride_t stride[4],const Trect &r,bool ro,IMediaSample *pIn,const Tpalette &Ipalette,bool isInterlacedRawVideo)
{
 init(Icsp,data,stride,r,ro,FRAME_TYPE::fromSample(pIn),FIELD_TYPE::fromSample(pIn,isInterlacedRawVideo),pIn?pIn->GetSize():0,Ipalette);
 if (pIn)
  {
   setTimestamps(pIn);
   setDiscontinuity(pIn);
  }
}
void TffPict::setTimestamps(IMediaSample *pIn)
{
 HRESULT hr = pIn->GetTime(&rtStart,&rtStop); 
 if (FAILED(hr))
  rtStart=rtStop=REFTIME_INVALID;
 else if (hr == VFW_S_NO_STOP_TIME)
  rtStop = rtStart + 1;
 if (FAILED(pIn->GetMediaTime(&mediatimeStart,&mediatimeStop)))
  mediatimeStart=mediatimeStop=REFTIME_INVALID;
}

void TffPict::setDiscontinuity(IMediaSample *pIn)
{
 if (pIn->IsDiscontinuity() == S_OK)
  discontinuity = true;
}
void TffPict::readLibavcodec(int Icsp,const char_t *flnm,const char_t *ext,Tbuffer &buf,IffdshowBase *deci)
{
 Tlibavcodec *libavcodec;
 if (FAILED(deci->getLibavcodec(&libavcodec))) return;
 AVCodec *avcodec=libavcodec->avcodec_find_decoder(stricmp(ext,_l(".png"))==0?CODEC_ID_PNG:CODEC_ID_MJPEG);
 if (!avcodec) return;
 AVCodecContext *avctx=libavcodec->avcodec_alloc_context();
 avctx->flags|=CODEC_FLAG_TRUNCATED;
 if (libavcodec->avcodec_open(avctx,avcodec)>=0)
  {
   FILE *f=fopen(flnm,_l("rb"));if (!f) return;
   int srclen=_filelength(fileno(f));
   uint8_t *src=(uint8_t*)calloc(srclen,2);
   fread(src,1,srclen,f);
   fclose(f);
   AVFrame *frame=libavcodec->avcodec_alloc_frame();
   int got_picture=0;
   int ret=libavcodec->avcodec_decode_video(avctx,frame,&got_picture,src,srclen);
   if (got_picture && frame->data[0] && frame->data[1])
    {
     const stride_t linesize[4]={frame->linesize[0],frame->linesize[1],frame->linesize[2],frame->linesize[3]};
     Tpalette pal(frame->data[1],256);
     init(csp_lavc2ffdshow(avctx->pix_fmt),frame->data,linesize,Trect(0,0,avctx->width,avctx->height),true,frametype,fieldtype,srclen,pal);
     Tconvert *convert=new Tconvert(deci,avctx->width,avctx->height);
     convertCSP(csp_bestMatch(csp,Icsp),buf,convert);
     delete convert;
    }
   else
    readOle(Icsp,flnm,ext,buf,deci);
   free(src);
   libavcodec->av_free(frame);
  }
 libavcodec->avcodec_close(avctx);
 libavcodec->av_free(avctx);
 libavcodec->Release();
}
void TffPict::readOle(int Icsp,const char_t *flnm,const char_t *ext,Tbuffer &buf,IffdshowBase *deci)
{
 //wchar_t flnmL[MAX_PATH];memset(flnmL,0,sizeof(flnmL));nCopyAnsiToWideChar(flnmL,flnm);
 comptr<IPicture> ip;
 if (OleLoadPicturePath((LPOLESTR)(const wchar_t*)text<wchar_t>(flnm),NULL,0,0,IID_IPicture,(void**)&ip)==S_OK)
  {
   long dxm,dym;
   ip->get_Width(&dxm);ip->get_Height(&dym);
   static const int HIMETRIC_PER_INCH=2540;
   const HDC hDCScreen=GetDC(NULL);
   const int nPixelsPerInchX=GetDeviceCaps(hDCScreen,LOGPIXELSX);
   const int nPixelsPerInchY=GetDeviceCaps(hDCScreen,LOGPIXELSY);
   ReleaseDC(NULL,hDCScreen);
   Tbuffer bufRGB;
   int pdx=(nPixelsPerInchX*dxm+HIMETRIC_PER_INCH/2)/HIMETRIC_PER_INCH,pdy=(nPixelsPerInchY*dym+HIMETRIC_PER_INCH/2)/HIMETRIC_PER_INCH;
   int pdx4=pdx;if (pdx4&3) pdx4=(pdx/4+1)*4;
   alloc(pdx4,pdy,FF_CSP_RGB24,bufRGB);
   HDC cdc=CreateCompatibleDC(NULL);
   HRESULT hr=ip->SelectPicture(cdc,NULL,NULL);
   BITMAPINFO bmi;
   bmi.bmiHeader.biSize=sizeof(bmi.bmiHeader);
   bmi.bmiHeader.biWidth=pdx4;
   bmi.bmiHeader.biHeight=-pdy;
   bmi.bmiHeader.biPlanes=1;
   bmi.bmiHeader.biBitCount=24;
   bmi.bmiHeader.biCompression=BI_RGB;
   bmi.bmiHeader.biSizeImage=3*rectFull.dx*rectFull.dy;
   bmi.bmiHeader.biXPelsPerMeter=75;
   bmi.bmiHeader.biYPelsPerMeter=75;
   bmi.bmiHeader.biClrUsed=0;
   bmi.bmiHeader.biClrImportant=0;
   HBITMAP hbmp;ip->get_Handle((OLE_HANDLE*)&hbmp);
   int lines=GetDIBits(cdc,hbmp,0,pdy,bufRGB,&bmi,DIB_RGB_COLORS);
   DeleteDC(cdc);
   Tconvert *convert=new Tconvert(deci,rectFull.dx,rectFull.dy);
   convertCSP(csp_bestMatch(csp,Icsp),buf,convert);
   delete convert;
   rectFull.dx=rectClip.dx=pdx;
  }
}

TffPict::TffPict(int Icsp,const char_t *flnm,Tbuffer &buf,IffdshowBase *deci)
{
 init();
 if (!fileexists(flnm)) return;
 char_t ext[MAX_PATH];
 _splitpath_s(flnm,NULL,0,NULL,0,NULL,0,ext,MAX_PATH);
 if (stricmp(ext,_l(".jpg"))==0 || stricmp(ext,_l(".jpeg"))==0 || stricmp(ext,_l(".png"))==0)
  readLibavcodec(Icsp,flnm,ext,buf,deci);
 else if (stricmp(ext,_l(".bmp"))==0 || stricmp(ext,_l(".gif"))==0)
  readOle(Icsp,flnm,ext,buf,deci);
}

void TffPict::clear(const Trect &r, unsigned int plane, int brightness, int brightnessY)
{
 if (r.dx!=0 && r.dy!=0 && !ro[plane])
  clear(cspInfo.Bpp, cspInfo.black[plane], data[plane] + r.y * stride[plane] + cspInfo.Bpp*r.x, stride[plane], cspInfo.Bpp * r.dx, r.dy, brightness, brightnessY);
}
void TffPict::clear(int full,unsigned int plane)
{
 const Trect &r=full?rectFull:rectClip;
 for (unsigned int i=0;i<cspInfo.numPlanes;i++)
  if (plane==PLANE_ALL || i==plane)
   clear(Trect(r.x>>cspInfo.shiftX[i],r.y>>cspInfo.shiftY[i],r.dx>>cspInfo.shiftX[i],r.dy>>cspInfo.shiftY[i]),i);
}
void TffPict::clearBorder(int brightness, int brightnessY)
{
 if (rectFull==rectClip) return;
 for (unsigned int i=0;i<cspInfo.numPlanes;i++)
  {
   const int rectFulldx=rectFull.dx>>cspInfo.shiftX[i],rectFulldy=rectFull.dy>>cspInfo.shiftY[i];
   const int rectClipx =rectClip.x >>cspInfo.shiftX[i],rectClipy =rectClip.y >>cspInfo.shiftY[i];
   const int rectClipdx=rectClip.dx>>cspInfo.shiftX[i],rectClipdy=rectClip.dy>>cspInfo.shiftY[i];
   clear(Trect(0, 0, rectFulldx, rectClipy),i, brightness, brightnessY);
   clear(Trect(0, rectClipy + rectClipdy, rectFulldx, rectFulldy - (rectClipy + rectClipdy)), i, brightness, brightnessY);
   clear(Trect(0, rectClipy, rectClipx, rectClipdy), i, brightness, brightnessY);
   clear(Trect(rectClipx+rectClipdx, rectClipy, rectFulldx - (rectClipx + rectClipdx), rectClipdy),i, brightness, brightnessY);
  }
}
void TffPict::copyBorder(const TffPict &src,unsigned int plane)
{
 if (rectFull==rectClip) return;
 for (unsigned int i=0;i<cspInfo.numPlanes;i++)
  if (plane==PLANE_ALL || i==plane)
   {
    //copy(data[i],stride[i],src.data[i],src.stride[i],src.rectFull.dx>>cspInfo.shiftX[i],src.rectFull.dy>>cspInfo.shiftY[i]);
    const int rectFulldx=rectFull.dx>>cspInfo.shiftX[i],rectFulldy=rectFull.dy>>cspInfo.shiftY[i];
    const int rectClipx =rectClip.x >>cspInfo.shiftX[i],rectClipy =rectClip.y >>cspInfo.shiftY[i];
    const int rectClipdx=rectClip.dx>>cspInfo.shiftX[i],rectClipdy=rectClip.dy>>cspInfo.shiftY[i];
    copyRect(src,Trect(0,0,rectFulldx,rectClipy),i);
    copyRect(src,Trect(0,rectClipy+rectClipdy,rectFulldx,rectFulldy-(rectClipy+rectClipdy)),i);
    copyRect(src,Trect(0,rectClipy,rectClipx,rectClipdy),i);
    copyRect(src,Trect(rectClipx+rectClipdx,rectClipy,rectFulldx-(rectClipx+rectClipdx),rectClipdy),i);
   }
}
void TffPict::copyRect(const TffPict &src,const Trect &r,unsigned int plane)
{
 if (r.dx!=0 && r.dy!=0)
  copy(data[plane]+r.y*stride[plane]+cspInfo.Bpp*r.x,stride[plane],
       src.data[plane]+r.y*src.stride[plane]+cspInfo.Bpp*r.x,src.stride[plane],
   cspInfo.Bpp*r.dx,r.dy);
}

void TffPict::setRO(bool Iro)
{
 for (unsigned int i=0;i<cspInfo.numPlanes;i++) ro[i]=Iro;
}

void TffPict::histogram(unsigned int histogram[256],int full,int half) const
{
 memset(histogram,0,256*sizeof(unsigned int));
 if (csp_isYUVplanar(csp))
  {
   Trect r=getRect(full,half);
   stride_t diff[4];calcDiff(cspInfo,r,stride,diff);
   for (const unsigned char *src=data[0]+diff[0],*srcEnd=src+r.dy*stride[0];src!=srcEnd;src+=stride[0])
    for (unsigned int x=0;x<r.dx;x++)
     histogram[src[x]]++;
  }
 else if ((csp&FF_CSPS_MASK)&FF_CSPS_MASK_YUV_PACKED)
  {
   Trect r=getRect(full,half);
   stride_t diff[4];calcDiff(cspInfo,r,stride,diff);
   int lumaoffset=cspInfo.packedLumaOffset;
   for (const unsigned char *src=data[0]+diff[0],*srcEnd=src+r.dy*stride[0];src!=srcEnd;src+=stride[0])
    for (unsigned int x=0;x<r.dx;x+=2)
     histogram[src[2*x+lumaoffset]]++;
  }
}
void TffPict::calcDiff(const TcspInfo &cspInfo,const Trect &rectClip,const stride_t stride[4],stride_t diff[4])
{
 for (unsigned int i=0;i<cspInfo.numPlanes;i++)
  diff[i]=cspInfo.Bpp*(rectClip.x>>cspInfo.shiftX[i])+(rectClip.y>>cspInfo.shiftY[i])*stride[i];
}
void TffPict::calcDiff(void)
{
 calcDiff(cspInfo,rectClip,stride,diff);
}
void TffPict::copyFrom(const TffPict &p,Tbuffer &buf,const Trect *rectCopy)
{
 if (!rectCopy)
  {
   rectFull=p.rectFull;
   rectClip=p.rectClip;
  }
 else
  rectFull=rectClip=*rectCopy;
 convertCSP(p.csp, buf);
 if (!rectCopy)
  for (unsigned int i=0;i<cspInfo.numPlanes;i++)
   copy(data[i],stride[i],p.data[i],p.stride[i],cspInfo.Bpp*(rectFull.dx>>cspInfo.shiftX[i]),rectFull.dy>>cspInfo.shiftY[i]);
 else
  for (unsigned int i=0;i<cspInfo.numPlanes;i++)
   copy(data[i],stride[i],p.data[i]+p.stride[i]*(rectCopy->y>>p.cspInfo.shiftY[i])+(rectCopy->x>>p.cspInfo.shiftX[i]),p.stride[i],cspInfo.Bpp*(rectFull.dx>>cspInfo.shiftX[i]),rectFull.dy>>cspInfo.shiftY[i]);
}
void TffPict::setCSP(int Icsp)
{
 cspInfo=*csp_getInfo(csp=Icsp);
}

/**
 * Prepare picture buffer for new image and fill it with converted image.
 */
void TffPict::convertCSP(int Icsp,Tbuffer &buf,Tconvert *convert,int edge)
{
 if (csp&FF_CSP_FLAGS_YUV_ADJ)
  {
   csp_yuv_adj_to_plane(csp,&cspInfo,rectFull.dy,data,stride);
   calcDiff();
  }
 if (csp&FF_CSP_FLAGS_YUV_ORDER) csp_yuv_order(csp,data,stride);
 int csp0=csp;
 const unsigned char *data0[4]={data[0],data[1],data[2],data[3]};
 stride_t stride0[4]={stride[0],stride[1],stride[2],stride[3]};
 Tpalette palette0=palette;
 convertCSP(Icsp,buf,edge);
 convert->convert(csp0|((fieldtype&FIELD_TYPE::MASK_INT)?FF_CSP_FLAGS_INTERLACED:0),data0,stride0,csp,data,stride,&palette0);
}

/**
 * Prepare picture buffer for new image.
 * New picture is empty black image.
 * No real color space conversion here.
 */
void TffPict::convertCSP(int Icsp,Tbuffer &buf,int edge)
{
 cspInfo=*csp_getInfo(csp=Icsp);
 if (csp_isYUVplanar(csp))
  {
   if (csp&FF_CSP_FLAGS_YUV_ADJ)
    {
     stride[0]=rectFull.dx>>cspInfo.shiftX[0];
     stride[1]=(stride[0]>>cspInfo.shiftX[1])+edge;//((rectFull.dx>>shiftX[1])/16+2)*16;
     stride[2]=(stride[0]>>cspInfo.shiftX[2])+edge;//((rectFull.dx>>shiftX[2])/16+2)*16;
     stride[3]=(stride[0]>>cspInfo.shiftX[3])+edge;//((rectFull.dx>>shiftX[3])/16+2)*16;
     stride[0]+=edge;
    }
   else
    {
     stride[0]=(((rectFull.dx>>cspInfo.shiftX[0])+edge)/16+2)*16; // If you change these 4 lines, please update TimgFilterYadif::config. (Don't change.)
     stride[1]=(((rectFull.dx>>cspInfo.shiftX[1])+edge)/16+2)*16;
     stride[2]=(((rectFull.dx>>cspInfo.shiftX[2])+edge)/16+2)*16;
     stride[3]=(((rectFull.dx>>cspInfo.shiftX[3])+edge)/16+2)*16;
    }
   size_t size=0;
   for (unsigned int i=0;i<cspInfo.numPlanes;i++)
    size+=stride[i]*((ODD2EVEN(rectFull.dy)>>cspInfo.shiftY[i])+edge); // rectFull.dy should be added by 1 when rectFull.dy is odd number.
   buf.allocZ(size,128);
   data[0]=buf;
   data[1]=data[0]+stride[0]*((ODD2EVEN(rectFull.dy)>>cspInfo.shiftY[0])+edge);
   data[2]=data[1]+stride[1]*((ODD2EVEN(rectFull.dy)>>cspInfo.shiftY[1])+edge);
   data[3]=data[2]+stride[2]*((ODD2EVEN(rectFull.dy)>>cspInfo.shiftY[2])+edge);
  }
 else if ((csp & FF_CSPS_MASK) == FF_CSP_NV12)
  {
   stride[0]=stride[1]=((rectFull.dx+edge)/16+2)*16;
   size_t size = stride[0] * (ODD2EVEN(rectFull.dy) + (ODD2EVEN(rectFull.dy)>>1));
   buf.allocZ(size,128);
   data[0] = buf;
   data[1] = data[0] + stride[0] * (ODD2EVEN(rectFull.dy) + edge);
  }
 else
  {
   stride[0]=rectFull.dx*cspInfo.Bpp;
   size_t bufsize0=stride[0]*rectFull.dy;
   buf.alloc(bufsize0);
   data[0]=buf;
  }
 calcDiff();
}
void TffPict::alloc(unsigned int dx,unsigned int dy,int Icsp,Tbuffer &buf,int edge)
{
 rectFull=rectClip=Trect(0,0,dx,dy);
 convertCSP(Icsp,buf,edge);
}

void TffPict::copy(BYTE* dstp, stride_t dst_pitch, const BYTE* srcp, stride_t src_pitch, int row_size, int height, bool flip)
{
  if (dst_pitch == src_pitch && src_pitch == row_size && !flip)
   {
    memcpy(dstp, srcp, src_pitch * height);
   }
  else
   {
    if (!flip)
     {
      for (int y=height; y>0; --y)
       {
        memcpy(dstp, srcp, row_size);
        dstp += dst_pitch;
        srcp += src_pitch;
       }
     }
    else
     {
      dstp += dst_pitch * (height - 1);
      for (int y=height; y>0; --y)
       {
        memcpy(dstp, srcp, row_size);
        dstp -= dst_pitch;
        srcp += src_pitch;
       }
     }
   }
}

void TffPict::clear(int Bpp,unsigned int black,unsigned char *dst,stride_t stride,unsigned int row_size,unsigned int height)
{
 switch (Bpp)
  {
   case 1:
    for (unsigned int y=0;y<height;y++)
     memset(dst+y*stride,black,row_size);
    return;
   case 2:
    black|=black<<16;
    for (unsigned int y=0;y<height;y++)
     for (uint32_t *p=(uint32_t*)(dst+y*stride),*pEnd=(uint32_t*)((uint8_t*)p+(row_size&~3));p!=pEnd;p++)
      *p=black;
    return;
   case 3:
    for (unsigned int y=0;y<height;y++)
     for (uint8_t *p=dst+y*stride,*pEnd=p+row_size;p!=pEnd;p+=3)
      {
       p[0]=((uint8_t*)&black)[0];
       p[1]=((uint8_t*)&black)[1];
       p[2]=((uint8_t*)&black)[2];
      }
    return;
   case 4:
    for (unsigned int y=0;y<height;y++)
     memsetd((uint32_t*)(dst+y*stride),black,row_size);
    return;
  }
}

void TffPict::clear(int Bpp, unsigned int black, unsigned char *dst, stride_t stride, unsigned int row_size, unsigned int height, int brightness, int brightnessY)
{
 switch (Bpp)
  {
   case 1:
    {
     if (black<=16)
      {
       if (csp_isYUV_NV(csp))
        black = brightnessY;
       else
        black = brightness;
      }
     for (unsigned int y=0;y<height;y++)
      memset(dst+y*stride,black,row_size);
     return;
    }
   case 2:
#if 0
     // disabled because I have no case to test.
     if ((black & 0xff) <= 16)
      black = (black & 0xff00) + brightness;
     if ((black >> 8) <= 16)
      black = (black & 0xff) + (brightness << 8);
#endif
    black|=black<<16;
    for (unsigned int y=0;y<height;y++)
     for (uint32_t *p=(uint32_t*)(dst+y*stride),*pEnd=(uint32_t*)((uint8_t*)p+(row_size&~3));p!=pEnd;p++)
      *p=black;
    return;
   case 3:
    for (unsigned int y=0;y<height;y++)
     for (uint8_t *p=dst+y*stride,*pEnd=p+row_size;p!=pEnd;p+=3)
      {
       p[0]=((uint8_t*)&black)[0];
       p[1]=((uint8_t*)&black)[1];
       p[2]=((uint8_t*)&black)[2];
      }
    return;
   case 4:
    black = brightness + (brightness << 8) + (brightness << 16);
    for (unsigned int y=0;y<height;y++)
     memsetd((uint32_t*)(dst+y*stride),black,row_size);
    return;
  }
}

void TffPict::draw_edges(uint8_t *buf, stride_t wrap, int width, int height, int w)
{
    uint8_t *ptr, *last_line;
    int i;

    last_line = buf + (height - 1) * wrap;
    for(i=0;i<w;i++) {
        /* top and bottom */
        memcpy(buf - (i + 1) * wrap, buf, width);
        memcpy(last_line + (i + 1) * wrap, last_line, width);
    }
    /* left and right */
    ptr = buf;
    for(i=0;i<height;i++) {
        memset(ptr - w, ptr[0], w);
        memset(ptr + width, ptr[width-1], w);
        ptr += wrap;
    }
    /* corners */
    for(i=0;i<w;i++) {
        memset(buf - (i + 1) * wrap - w, buf[0], w); /* top left */
        memset(buf - (i + 1) * wrap + width, buf[width-1], w); /* top right */
        memset(last_line + (i + 1) * wrap - w, last_line[0], w); /* top left */
        memset(last_line + (i + 1) * wrap + width, last_line[width-1], w); /* top right */
    }
}

void TffPict::createEdge(unsigned int Iedge,Tbuffer &buf)
{
 if (edge>=Iedge) return;
 edge=Iedge;
 Trect rectFull0=rectFull,rectClip0=rectClip;
 stride_t stride0[4];memcpy(stride0,stride,sizeof(stride));
 unsigned char *data0[4];memcpy(data0,data,sizeof(data));
 alloc(rectFull.dx,rectFull.dy,csp,buf,2*edge);
 rectFull=rectEdge=Trect(0,0,rectFull.dx+edge*2,rectFull.dy+edge*2);
 rectClip=Trect(edge,edge,rectFull0.dx,rectFull0.dy);
 calcDiff();
 for (unsigned int i=0;i<cspInfo.numPlanes;i++)
  {
   edgeData[i]=data[i];
   unsigned int dx=rectFull0.dx>>cspInfo.shiftX[i],dy=rectFull0.dy>>cspInfo.shiftY[i];
   copy(data[i]+=diff[i],stride[i],data0[i],stride0[i],dx*cspInfo.Bpp,dy);
   draw_edges(data[i],stride[i],dx,dy,edge);
  }
 rectFull=rectFull0;rectClip=rectClip0;
 calcDiff();
}

void TffPict::md5sum(uint8_t sum[16]) const
{
 memset(sum,0,16);
 Tmd5 md5;
 for (unsigned int i=0;i<cspInfo.numPlanes;i++)
  for (unsigned int y=0;y<rectFull.dy>>cspInfo.shiftY[i];y++)
   md5.sum(data[i]+y*stride[i],rectFull.dx>>cspInfo.shiftX[i]);
 md5.done(sum);
}
