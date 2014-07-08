/*
 * Copyright (c) 2004-2006 Milan Cutka
 * based of CSubpicInputPin by Gabest
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
#include "TsubtitlesTextpinDVD.h"
#include "Tsubreader.h"
#include "TsubtitleDVD.h"
#include "ffdebug.h"

TsubtitlesTextpinDVD::TsubtitlesTextpinDVD(int Itype,IffdshowBase *Ideci):TsubtitlesTextpin(Itype,Ideci)
{
 sub_format=Tsubreader::SUB_DVD;
 subs=new Tsubreader;
}

void TsubtitlesTextpinDVD::resetSubtitles(void)
{
 subs->clear();
 TsubtitlesTextpin::resetSubtitles();
}

void TsubtitlesTextpinDVD::addSubtitle(REFERENCE_TIME start,REFERENCE_TIME stop,const unsigned char *data,unsigned int datalen,const TsubtitlesSettings *cfg,bool utf8)
{
 if (type==Tsubreader::SUB_SVCD)
  {
   if (datalen<=4) return;
   data+=4;
   datalen-=4;
  }

 if (start==-1 && stop==-1)
  if (subs->size()>0)
   subs->back()->append(data,datalen);
  else
   DPRINTF(_l("Hanging subpicture"));
 else
  {
   for (Tsubreader::reverse_iterator s=subs->rbegin();s!=subs->rend();s++)
    if ((*s)->stop==_I64_MAX && (*s)->start<start)
     {
      (*s)->stop=start;
      break;
     }
   TsubtitleDVD *sub;
   switch (type)
    {
     case Tsubreader::SUB_DVD :sub=new TsubtitleDVD (start,data,datalen,this);break;
     case Tsubreader::SUB_SVCD:sub=new TsubtitleSVCD(start,data,datalen,this);break;
     case Tsubreader::SUB_CVD :sub=new TsubtitleCVD (start,data,datalen,this);break;
     default:return;
    }
   subs->push_back(sub);
  }
}

bool TsubtitlesTextpinDVD::ctlSubtitles(unsigned int id,const void *data,unsigned int datalen)
{
 bool refresh=false;
 switch (id)
  {
   case AM_PROPERTY_DVDSUBPIC_PALETTE:
    {
     const AM_PROPERTY_SPPAL *pSPPAL=(const AM_PROPERTY_SPPAL*)data;
     if (memcmp(sppal,pSPPAL->sppal,sizeof(sppal))!=0)
      for (Tsubreader::iterator s=subs->begin();s!=subs->end();s++)
       ((TsubtitleDVD*)*s)->changed=true;
     memcpy(sppal,pSPPAL->sppal,sizeof(AM_PROPERTY_SPPAL));
     fsppal=true;
     DPRINTF(_l("new palette"));
     break;
    }
   case AM_PROPERTY_DVDSUBPIC_HLI:
    {
     const AM_PROPERTY_SPHLI *pSPHLI=(const AM_PROPERTY_SPHLI*)data;
     if (psphli) psphli=NULL;
     if (pSPHLI->HLISS)
      {
       for (Tsubreader::iterator s=subs->begin();s!=subs->end();s++)
        if ((*s)->start<=TsubtitleDVD::pts2rt(pSPHLI->StartPTM) && TsubtitleDVD::pts2rt(pSPHLI->StartPTM)<(*s)->stop)
         {
          refresh=true;
          TsubtitleDVD *sd=(TsubtitleDVD*)*s;
          if (sd->psphli)
           {
           if (sd->psphli->StartX!=pSPHLI->StartX ||
               sd->psphli->StartY!=pSPHLI->StartY ||
               sd->psphli->StopX !=pSPHLI->StopX  ||
               sd->psphli->StopY !=pSPHLI->StopY  ||
               memcmp(&sd->psphli->ColCon,&pSPHLI->ColCon,sizeof(AM_COLCON))!=0)
            sd->changed=true;
           }
          else
           sd->changed=true;
          sd->psphli.release();
          sd->psphli=new AM_PROPERTY_SPHLI(*pSPHLI);
         }
       if(!refresh) // save it for later, a subpic might be late for this hli
        psphli.assign(new AM_PROPERTY_SPHLI(*pSPHLI));
      }
     else
      for (Tsubreader::iterator s=subs->begin();s!=subs->end();s++)
       {
        TsubtitleDVD *sd=(TsubtitleDVD*)*s;
        refresh|=(sd->changed=!!sd->psphli);
        sd->psphli.release();
       }
     if (pSPHLI->HLISS)
      DPRINTF(_l("hli: %I64d - %I64d, (%d,%d) - (%d,%d)"),TsubtitleDVD::pts2rt(pSPHLI->StartPTM)/10000,TsubtitleDVD::pts2rt(pSPHLI->EndPTM)/10000,pSPHLI->StartX, pSPHLI->StartY, pSPHLI->StopX, pSPHLI->StopY);
     break;
    }
   case AM_PROPERTY_DVDSUBPIC_COMPOSIT_ON:
    {
     AM_PROPERTY_COMPOSIT_ON *pCompositOn=(AM_PROPERTY_COMPOSIT_ON*)data;
     spon=!!*pCompositOn;
     break;
    }
  }
 return refresh;
}

const Tsubtitle* TsubtitlesTextpinDVD::getSubtitle(const TsubtitlesSettings *cfg,REFERENCE_TIME time,bool *forceChange)
{
 for (Tsubreader::iterator s0=subs->begin();s0!=subs->end();)
  if ((*s0)->stop<=time)
   {
    delete *s0;
    s0=subs->erase(s0);
   }
  else
   s0++;

 subtitles.clear();REFERENCE_TIME start=_I64_MAX,stop=_I64_MIN;
 for (Tsubreader::const_iterator s=subs->begin();s!=subs->end();s++)
  if ((*s)->start<=time && time<(*s)->stop && (spon || ((TsubtitleDVD*)*s)->psphli))
   {
    subtitles.push_back(*s);
    if ((*s)->start<start) start=(*s)->start;
    if ((*s)->stop >stop ) stop =(*s)->stop ;
   }
 return subtitles.empty()?NULL:(subtitles.start=start,subtitles.stop=stop,&subtitles);
}

void TsubtitlesTextpinDVD::Tsubtitles::print(REFERENCE_TIME time,bool wasseek,Tfont &f,bool forceChange,TrenderedSubtitleLines::TprintPrefs &prefs) const
{
 for (const_iterator s=begin();s!=end();s++)
  (*s)->print(time,wasseek,f,forceChange,prefs);
}
Tsubtitle* TsubtitlesTextpinDVD::Tsubtitles::copy(void)
{
 return new Tsubtitles(*this);
}
