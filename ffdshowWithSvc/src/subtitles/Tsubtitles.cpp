/*
 * Copyright (c) 2002-2006 Milan Cutka
 * getSubtitle function uses code from mplayer
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
#include "Tsubtitles.h"
#include "Tsubreader.h"
#include "IffdshowBase.h"
#include "ffdshow_constants.h"

Tsubtitles::Tsubtitles(IffdshowBase *Ideci):deci(Ideci)
{
 subs=NULL;sub_format=0;
 deci->getConfig(&ffcfg);
}
Tsubtitles::~Tsubtitles()
{
 if (subs) delete subs;subs=NULL;
}

void Tsubtitles::done(void)
{
 if (subs) delete subs;subs=NULL;
}

void Tsubtitles::init(void)
{
 oldsub=NULL;
 current_sub=0;
 nosub_range_start=-1;
 nosub_range_end=-1;
}

void Tsubtitles::processOverlap(void)
{
 subs->processOverlap();
}

const Tsubtitle* Tsubtitles::getSubtitle(const TsubtitlesSettings *cfg,REFERENCE_TIME time,bool *forceChange)
{
 checkChange(cfg,forceChange);
 if (!subs || subs->empty()) return NULL;

 if ((sub_format&Tsubreader::SUB_FORMATMASK)==Tsubreader::SUB_VOBSUB)
  {
   int newlang=deci->getParam2(IDFF_subCurLang);
   if (subs->langid!=newlang)
    subs->setLang(newlang);
  }
/*
 for (Tsubreader::const_iterator s=subs->begin();s!=subs->end();s++)
  if ((*s)->start<time && time<(*s)->stop)
   return *s;
 return NULL;
*/
 if (oldsub)
  {
   if(time>=oldsub->start && time<=oldsub->stop)
    return oldsub; // OK!
  }
 else
  {
   if(time>nosub_range_start && time<nosub_range_end)
    return oldsub; // OK!
  }
 // sub changed!
 if (time<0)
  return oldsub=NULL; // no sub here
 // check current sub.
 if (time>=(*subs)[current_sub]->start && time<=(*subs)[current_sub]->stop)
  return oldsub=(*subs)[current_sub];
 // check next sub.
 if(current_sub>=0 && current_sub+1<subs->size())
  {
   if (time>(*subs)[current_sub]->stop && time<(*subs)[current_sub+1]->start)
    {
     nosub_range_start=(*subs)[current_sub]->stop;
     nosub_range_end  =(*subs)[current_sub+1]->start;
     return oldsub=NULL;
    }
   // next sub?
   ++current_sub;
   oldsub=(*subs)[current_sub];
   if (time>=oldsub->start && time<=oldsub->stop)
    return oldsub; // OK!
  }
  // use logarithmic search:
 int i=0,j=(int)subs->size()-1;
 while (j>=i)
  {
   current_sub=(i+j+1)/2;
   oldsub=(*subs)[current_sub];
   if (time<oldsub->start)
    j=current_sub-1;
   else if(time>oldsub->stop)
    i=current_sub+1;
   else
    return oldsub; // found!
  }
 // check where are we...
 if (time<oldsub->start)
  {
   if(current_sub<=0)
    {
     // before the first sub
     nosub_range_start=time-1; // tricky
     nosub_range_end  =oldsub->start;
     return oldsub=NULL;
    }
   --current_sub;
   if (time>(*subs)[current_sub]->stop && time<(*subs)[current_sub+1]->start)
    {
     // no sub
     nosub_range_start=(*subs)[current_sub]->stop;
     nosub_range_end  =(*subs)[current_sub+1]->start;
     return oldsub=NULL;
    }
  }
 else
  {
   if (time<=oldsub->stop) /*printf("JAJJ!  ")*/;
   else
    if (current_sub+1>=subs->size())
     {
      // at the stop?
      nosub_range_start=oldsub->stop;
      nosub_range_end=_I64_MAX;// 0x7FFFFFFF; // MAXINT
      return oldsub=NULL;
     }
    else
     if (time>(*subs)[current_sub]->stop && time<(*subs)[current_sub+1]->start)
      {
       // no sub
       nosub_range_start=(*subs)[current_sub]->stop;
       nosub_range_end  =(*subs)[current_sub+1]->start;
       return oldsub=NULL;
      }
  }
 return oldsub=NULL;
}
