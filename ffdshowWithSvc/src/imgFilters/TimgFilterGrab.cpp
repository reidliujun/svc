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
#include "TimgFilterGrab.h"
#include "TgrabSettings.h"
#include "Tdll.h"
#include "IffdshowBase.h"
#include "IffdshowDecVideo.h"
#include "TimgFilters.h"

//========================== TimgExportLibavcodec ===========================
TimgFilterGrab::TimgExportLibavcodec::TimgExportLibavcodec(const Tconfig *config,IffdshowBase *deci,CodecID IcodecId):codecId(IcodecId)
{
 deci->getLibavcodec(&dll);
 ok=(dll && dll->ok);
 avctx=NULL;picture=NULL;
}
TimgFilterGrab::TimgExportLibavcodec::~TimgExportLibavcodec()
{
 done();
 if (dll) dll->Release();
}
void TimgFilterGrab::TimgExportLibavcodec::init(unsigned int dx,unsigned int dy)
{
 avctxinited=false;
 if (dll->ok && !dll->dec_only)
  {
   AVCodec *avcodec=dll->avcodec_find_encoder(codecId);
   avctx=dll->avcodec_alloc_context();
   avctx->width=dx;
   avctx->height=dy;
   avctx->flags=CODEC_FLAG_QSCALE;
   avctx->pix_fmt=csp_ffdshow2lavc(requiredCSP());
   avctx->time_base.num=avctx->time_base.den=1;
   if (dll->avcodec_open(avctx,avcodec)<0)
    {
     ok=false;
     return;
    }
   avctxinited=true;
   picture=dll->avcodec_alloc_frame();
   inited=true;
  }
}
int TimgFilterGrab::TimgExportLibavcodec::compress(const unsigned char *src[4],stride_t stride[4],unsigned char *dst,unsigned int dstlen,int qual)
{
 if (!avctx) return 0;
 for (int i=0;i<4;i++)
  {
   picture->data[i]=(uint8_t*)src[i];
   picture->linesize[i]=(int)stride[i];
  }
 picture->quality=setQual(qual);
 int len=dll->avcodec_encode_video(avctx,dst,dstlen,picture);
 return len>0?len:0;
}
void TimgFilterGrab::TimgExportLibavcodec::done(void)
{
 if (avctx)
  {
   if (avctxinited) dll->avcodec_close(avctx);avctxinited=false;
   dll->av_free(avctx);avctx=NULL;
   dll->av_free(picture);picture=NULL;
  }
 inited=false;
}

//============================== TimgExportBMP =============================
TimgFilterGrab::TimgExportBMP::TimgExportBMP(void)
{
 memset(&bfh,0,sizeof(bfh));
 memcpy(&bfh.bfType,"BM",2);
 bfh.bfOffBits=sizeof(bch)+sizeof(bfh);
 memset(&bch,0,sizeof(bch));
 bch.bcSize=sizeof(bch);
 bch.bcPlanes=1;
 bch.bcBitCount=24;
 ok=true;
}
void TimgFilterGrab::TimgExportBMP::init(unsigned int dx,unsigned int dy)
{
 bch.bcWidth=(WORD)dx;bch.bcHeight=(WORD)dy;
 inited=true;
}
int TimgFilterGrab::TimgExportBMP::compress(const unsigned char *src[4],stride_t stride[4],unsigned char *dst,unsigned int dstlen,int qual)
{
 memcpy(dst,&bfh,sizeof(bfh));
 memcpy(dst+sizeof(bfh),&bch,sizeof(bch));
 TffPict::copy(dst+sizeof(bfh)+sizeof(bch),bch.bcWidth*3,src[0],stride[0],bch.bcWidth*3,bch.bcHeight);
 bfh.bfSize=sizeof(bfh)+sizeof(bch)+bch.bcWidth*bch.bcHeight*3;
 return bfh.bfSize;
}

//============================== TimgFilterGrab =============================
TimgFilterGrab::TimgFilterGrab(IffdshowBase *Ideci,Tfilters *Iparent):TimgFilter(Ideci,Iparent)
{
 exp[0]=exp[1]=exp[2]=NULL;
 dstbuf=NULL;dstbuflen=0;
 now=0;
}
TimgFilterGrab::~TimgFilterGrab()
{
 if (exp[0]) delete exp[0];exp[0]=NULL;
 if (exp[1]) delete exp[1];exp[1]=NULL;
 if (exp[2]) delete exp[2];exp[2]=NULL;
}
void TimgFilterGrab::done(void)
{
 if (dstbuf) ::free(dstbuf);dstbuf=NULL;
}
void TimgFilterGrab::onSizeChange(void)
{
 done();
 if (exp[0]) exp[0]->done();
 if (exp[1]) exp[1]->done();
 if (exp[2]) exp[2]->done();
}

bool TimgFilterGrab::is(const TffPictBase &pict,const TfilterSettingsVideo *cfg)
{
 return true;
}

int TimgFilterGrab::getSupportedInputColorspaces(const TfilterSettingsVideo *cfg0) const
{
 const TgrabSettings *cfg=(const TgrabSettings*)cfg0;
 return exp[cfg->format] && exp[cfg->format]->ok?exp[cfg->format]->requiredCSP():FF_CSPS_MASK;
}

HRESULT TimgFilterGrab::process(TfilterQueue::iterator it,TffPict &pict,const TfilterSettingsVideo *cfg0)
{
 const TgrabSettings *cfg=(const TgrabSettings*)cfg0;
 LONG wasNow=now;
 if (cfg->is || wasNow)
  {
   int framenum=deci->getParam2(IDFF_currentFrame);
   if (wasNow ||
       (cfg->mode==0 && (framenum%cfg->step==0)) ||
       (cfg->mode==1 && (framenum==cfg->frameNum)) ||
       (cfg->mode==2 && isIn(framenum,cfg->frameNum1,cfg->frameNum2) && (framenum-cfg->frameNum1)%cfg->step==0))
    {
     if (!exp[cfg->format])
      switch (cfg->format)
       {
        case 0:exp[0]=new TimgExportJPEG(parent->config,deci);break;
        case 1:exp[1]=new TimgExportBMP;break;
        case 2:exp[2]=new TimgExportPNG(parent->config,deci);break;
       }

     if (exp[cfg->format]->ok)
      {
       init(pict,cfg->full,0);

       if (!exp[cfg->format]->inited)
        exp[cfg->format]->init(pictRect.dx,pictRect.dy);

       if (exp[cfg->format]->ok)
        {
         const unsigned char *src[4];
         Trect tempR(pictRect.x,pictRect.y,pictRect.dx,pictRect.dy);
         temp.copyFrom(pict, buffer, &tempR);
         getCur(exp[cfg->format]->requiredCSP(),temp,cfg->full,src);

         if (!dstbuf) dstbuf=(unsigned char*)malloc(dstbuflen=pictRect.dx*pictRect.dy*4+1024);

         int len=exp[cfg->format]->compress(src,stride1,dstbuf,dstbuflen,cfg->qual);
         if (len)
          {
           char_t name[MAX_PATH];
           tsnprintf_s(name, countof(name), _TRUNCATE, _l("%s%0*i.%s"),cfg->prefix,cfg->digits,framenum,TgrabSettings::formats[cfg->format].ext);
           char_t flnm[MAX_PATH];
           _makepath_s(flnm,MAX_PATH,NULL,cfg->path,name,NULL);
           FILE *f=fopen(flnm,_l("wb"));
           if (f)
            {
             fwrite(dstbuf,1,len,f);
             fclose(f);
            }
          }
        }
      }
    }
  }
 if (wasNow) InterlockedDecrement((LONG*)&now);
 return parent->deliverSample(++it,pict);
}

HRESULT TimgFilterGrab::queryInterface(const IID &iid,void **ptr) const
{
 if (iid==IID_IimgFilterGrab) {*ptr=(IimgFilterGrab*)this;return S_OK;}
 else return E_NOTIMPL;
}
STDMETHODIMP TimgFilterGrab::grabNow(void)
{
 InterlockedIncrement((LONG*)&now);
 return S_OK;
}
