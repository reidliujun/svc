/*
 * Copyright (c) 2005,2006 Milan Cutka
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
#include "TsubreaderUSF.h"
#include "TsubtitleUSF.h"
#include "Tdll.h"
#include "Tconvert.h"
#include "IffdshowBase.h"
#include "ffdebug.h"

const char_t* TsubreaderUSF2::dllname=_l("C:\\Program Files\\cvsnt\\utils\\pixifier\\release\\pixiDLL.dll");

TsubreaderUSF2::TsubreaderUSF2(const Tstream &f,IffdshowBase *Ideci,bool initonly):
 deci(Ideci),
 pixi(NULL),
 pcfg(NULL),
 convert(NULL),dx(0),dy(0),
 sublines(NULL),subbmp(NULL)
{
 memset(img,0,sizeof(img));memset(alpha,0,sizeof(alpha));
 const Tconfig *config;deci->getConfig(&config);
 dll=new Tdll(dllname,config);
 dll->loadFunction(init,"init");
 dll->loadFunction(deinit,"deinit");
 dll->loadFunction(setDimension,"setDimension");
 dll->loadFunction(loadUSFText,"loadUSFText");
 dll->loadFunction(loadInitText,"loadInitText");
 dll->loadFunction(addSubtitleText,"addSubtitleText");
 dll->loadFunction(countVSubs,"countVSubs");
 dll->loadFunction(getVSubStart,"getVSubStart");
 dll->loadFunction(getVSubStop,"getVSubStop");
 dll->loadFunction(getVSub,"getVSub");
 dll->loadFunction(freeVSub,"freeVSub");
 dll->loadFunction(changed,"changed");
 dll->loadFunction(FlushSubtitles,"FlushSubtitles");
 if (!dll->ok) return;

 if (init(NULL,&pixi,&pcfg)!=0) return;
 wchar_t *text=stream2text(f);
 if (initonly)
  loadInitText(pixi,text);
 else
  loadUSFText(pixi,text);
 delete text;
 if (!initonly)
  {
   int cnt=countVSubs(pixi);
   for (int i=0;i<cnt;i++)
    {
     int start=getVSubStart(pixi,i),stop=getVSubStop(pixi,i);
     push_back(new TsubtitleUSF2(this,i,REF_SECOND_MULT*start/1000,REF_SECOND_MULT*stop/1000));
    }
  }
}
TsubreaderUSF2::~TsubreaderUSF2()
{
 destroy();
 if (sublines && subbmp) freeSub(subbmp,sublines);
 if (pixi) deinit(pixi);pixi=NULL;
 delete dll;
}

void TsubreaderUSF2::destroy(void)
{
 if (convert) delete convert;convert=NULL;
 if (img[0]) aligned_free(img[0]);img[0]=NULL;
 if (img[1]) aligned_free(img[1]);img[1]=NULL;
 if (img[2]) aligned_free(img[2]);img[2]=NULL;
 if (alpha[0]) aligned_free(alpha[0]);alpha[0]=NULL;
 if (alpha[1]) aligned_free(alpha[1]);alpha[1]=NULL;
}
void TsubreaderUSF2::create(unsigned int Idx,unsigned int Idy)
{
 destroy();
 convert=new Tconvert(deci,dx=Idx,dy=Idy);
 stride[0]=(dx/16+2)*16;stride[1]=stride[2]=(dx/32+2)*16;
 img[0]=(unsigned char*)aligned_malloc(stride[0]*dy);
 img[1]=(unsigned char*)aligned_malloc(stride[1]*(dy/2));
 img[2]=(unsigned char*)aligned_malloc(stride[2]*(dy/2));
 alpha[0]=(unsigned char*)aligned_malloc(stride[0]*dy);
 alpha[1]=(unsigned char*)aligned_malloc(stride[1]*(dy/2));
 setDimension(pixi,(WORD)dx,(WORD)dy);
}

ffwstring TsubreaderUSF2::referenceTimeToStr(const REFERENCE_TIME &time)
{
 int mSec=int(time/10000);
 int hour=mSec/3600000;
 int min=mSec/60000 - hour*60;
 int sec=mSec/1000 - min*60 - hour*3600;
 mSec=mSec - sec*1000 - min*60000 - hour*3600000;
 wchar_t ws[40];
 swprintf(ws,L"%02i:%02i:%02i.%03i",hour,min,sec,mSec);
 return ws;
}

void TsubreaderUSF2::addSub(REFERENCE_TIME start,REFERENCE_TIME stop,const unsigned char *data,unsigned int datalen)
{
 if (!pixi) return;
 wchar_t *text=stream2text(TstreamMem(data,datalen,Tstream::ENC_UTF8));
 ffwstring stext=L"<subtitle start=\"" + referenceTimeToStr(start) + L"\" stop=\"" + referenceTimeToStr(stop) + L"\">" + text + L"</subtitle>";
 delete text;
 int i=addSubtitleText(pixi,stext.c_str())/2;
 int cnt=countVSubs(pixi);
/*
 for (int i=0;i<cnt;i++)
  {
   int start=getVSubStart(pixi,i),stop=getVSubStop(pixi,i);
   DPRINTF("%i %i %i",i,start,stop);
  }*/
 push_back(new TsubtitleUSF2(this,i,start,stop));
}

void* TsubreaderUSF2::getSub(int idx,int ms,int *width,int *height,int32_t** *lines,bool *wasChangePtr)
{
 if (wasChange(ms))
  {
   if (subbmp && sublines) freeSub(subbmp,sublines);
   subbmp=getVSub(pixi,idx,ms,width,height,&sublines);
   *wasChangePtr=true;
  }
 else
  *wasChangePtr=false;
 *lines=sublines;
 return subbmp;
}
void TsubreaderUSF2::freeSub(void *bmp,int32_t **lines)
{
 freeVSub(bmp,lines);
}

bool TsubreaderUSF2::wasChange(int ms)
{
 return !!changed(pixi,ms);
}

void TsubreaderUSF2::clear(void)
{
 Tsubreader::clear();
 if (pixi) FlushSubtitles(pixi);
}

wchar_t* TsubreaderUSF2::stream2text(const Tstream &f)
{
 int len=2*f.length();
 f.encoding=Tstream::ENC_UTF8;
 wchar_t *text=new wchar_t[len],*t=text;
 while (!f.eof())
  {
   if (!f.fgets(t,len)) break;
   t=strchr(t,'\0');
  }
 return text;
}
