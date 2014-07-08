/*
 * Copyright (c) 2004-2006 Milan Cutka
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
#include "TsubtitleVobsub.h"
#include "vobsub.h"
#include "spudec.h"

//#define DVDSUB

TsubtitleVobsub::TsubtitleVobsub(Tspudec *Ispu,Tvobsub *Ivobsub):spu(Ispu),vobsub(Ivobsub),dvdsub(NULL)
{
 start=0;stop=int64_t(1)<<60;
 oldtimer=-1;
}
Tsubtitle* TsubtitleVobsub::copy(void)
{
 return new TsubtitleVobsub(*this);
}

TsubtitleVobsub::~TsubtitleVobsub()
{
 if (dvdsub) delete dvdsub;
}

void TsubtitleVobsub::drawalpha(int x0,int y0, unsigned int w,unsigned int h, const unsigned char* srcY, const unsigned char *srcaY, int strideY,const unsigned char* srcUV, const unsigned char *srcaUV, int strideUV,const TrenderedSubtitleLines::TprintPrefs &prefs)
{
 if (x0<0 || y0<0 || w==0 || h==0) return;
 TrenderedVobsubWord wrd;
 for (int i=0;i<3;i++)
  {
   wrd.dx[i]=w>>prefs.shiftX[i];
   wrd.dy[i]=h>>prefs.shiftY[i];
  }
 wrd.dxCharY=wrd.dx[0]; wrd.dyCharY=wrd.dy[0];
 wrd.bmp[0]=(unsigned char*)srcY;wrd.msk[0]=(unsigned char*)srcaY;
 wrd.bmp[1]=wrd.bmp[2]=(unsigned char*)srcUV;wrd.msk[1]=wrd.msk[2]=(unsigned char*)srcaUV;
 wrd.bmpmskstride[0]=strideY;wrd.bmpmskstride[1]=wrd.bmpmskstride[2]=strideUV;
 TrenderedSubtitleLine ln(&wrd);
 TrenderedSubtitleLines lines(&ln);
 if (!prefs.vobchangeposition)
  {
   TrenderedSubtitleLines::TprintPrefs prefs2=prefs;
   prefs2.xpos=-x0;prefs2.ypos=-y0;
   lines.print(prefs2);
  }
 else
  lines.print(prefs);
}
void TsubtitleVobsub::print(REFERENCE_TIME time,bool wasseek,Tfont &f,bool forceChange,TrenderedSubtitleLines::TprintPrefs &prefs) const
{
 int timer=(int)(90*time/(REF_SECOND_MULT/1000));
 if (wasseek)
  {
   if (vobsub) vobsub->vobsub_seek(timer);
   spu->spudec_reset();
   if (dvdsub)
    {
     delete dvdsub;
     dvdsub=NULL;
    }
  }
 if (oldtimer!=timer)
  spu->spudec_heartbeat(timer);
 oldtimer=timer;
 if (dvdsub && time>dvdsub->stop)
  {
   delete dvdsub;
   dvdsub=NULL;
  }
 while (vobsub)
  {
   unsigned char* packet=NULL;
   int timestamp;
   int len=vobsub->vobsub_get_packet(timer,(void**)&packet,&timestamp);
   if (len>0)
    {
     //timestamp -= (sh_video->pts + sub_delay - sh_video->timer)*90000;
     //mp_dbg(MSGT_CPLAYER,MSGL_V,"\rVOB sub: len=%d v_pts=%5.3f v_timer=%5.3f sub=%5.3f ts=%d \n",len,sh_video->pts,sh_video->timer,timestamp / 90000.0,timestamp);
    }
   if (len<=0 || !packet) break;
   if (timestamp<0)
    timestamp = 0;
   else
    {
     #ifdef DVDSUB
     if (!dvdsub)
      dvdsub=new TsubtitleDVD(time,packet,len,vobsub);
     else
      dvdsub->append(packet,len);
     #else
     spu->spudec_assemble(packet,len,timestamp);
     #endif
    }
  }
 if (dvdsub)
  dvdsub->print(time,wasseek,f,forceChange,prefs);
 spu->spudec_draw_scaled(prefs.dx,prefs.dy,drawalpha,prefs);
}
