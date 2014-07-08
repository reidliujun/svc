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
#include "Tsubreader.h"
#include "ffdebug.h"
#include "TsubtitlesSettings.h"
#include "Tconfig.h"
#include "TsubreaderUSF.h"

Tsubreader::~Tsubreader()
{
 clear();
}
void Tsubreader::clear(void)
{
 for (iterator s=begin();s!=end();s++)
  {
   delete *s;
   *s=NULL;
  }
 std::vector<value_type>::clear();
}

int Tsubreader::sub_autodetect(Tstream &fd,const Tconfig *config)
{
 int j=0;
 static const int LINE_LEN=1000;
 int format=SUB_INVALID;
 while (j < 100)
  {
   j++;
   char line[LINE_LEN+1];
   if (!fd.fgets (line, LINE_LEN))
    {
     format=SUB_INVALID;
     break;
    }
   int i;char p;
   if (sscanf (line, "{%d}{%d}", &i, &i)==2)
    {
     format=SUB_MICRODVD;
     break;
    }
   if (sscanf (line, "{%d}{}", &i)==1)
    {
     format=SUB_MICRODVD;
     break;
    }
   if (sscanf (line, "[%d][%d]", &i, &i)==2)
    {
     format=SUB_MPL2|SUB_USESTIME;
     break;
    }
   if (sscanf (line, "%d:%d:%d.%d,%d:%d:%d.%d",     &i, &i, &i, &i, &i, &i, &i, &i)==8)
    {
     format=SUB_SUBRIP|SUB_USESTIME;
     break;
    }
   if (sscanf (line, "%d:%d:%d%[,.:]%d --> %d:%d:%d%[,.:]%d", &i, &i, &i, (char *)&i, &i, &i, &i, &i, (char *)&i, &i)==10)
    {
     format=SUB_SUBVIEWER|SUB_USESTIME;
     break;
    }
   if (sscanf (line, "{T %d:%d:%d:%d",&i, &i, &i, &i)==4)
    {
     format=SUB_SUBVIEWER2|SUB_USESTIME;
     break;
    }
   if (strstr (line, "<SAMI>"))
    {
     format=SUB_SAMI|SUB_USESTIME;
     break;
    }
   if (sscanf (line, "%d:%d:%d:",     &i, &i, &i )==3)
    {
     format=SUB_VPLAYER|SUB_USESTIME;
     break;
    }
   if (sscanf (line, "%d:%d:%d ",     &i, &i, &i )==3)
    {
     format=SUB_VPLAYER|SUB_USESTIME;
     break;
    }
   if (stristr(line,"<USFSubtitles ")!=NULL)
    {
     if (config->check(TsubreaderUSF2::dllname))
      format=SUB_USF|SUB_USESTIME;
     break;
    }
   //TODO: just checking if first line of sub starts with "<" is WAY
   // too weak test for RT
   // Please someone who knows the format of RT... FIX IT!!!
   // It may conflict with other sub formats in the future (actually it doesn't)
   //should be better now
   if ( stristr(line,"time begin")!=NULL )
    {
     format=SUB_RT|SUB_USESTIME;
     break;
    }

   if (!memcmp(line, "Dialogue: Marked", 16))
    {
     format=SUB_SSA|SUB_USESTIME;
     break;
    }
   if (!memcmp(line, "Dialogue: ", 10))
    {
     format=SUB_SSA|SUB_USESTIME;
     break;
    }
   if (!memcmp(line, "# VobSub index file", 19))
    {
     format=SUB_VOBSUB|SUB_USESTIME;
     break;
    }
   if (sscanf (line, "%d,%d,\"%c", &i, &i, (char *) &i) == 3)
    {
     format=SUB_DUNNOWHAT;
     break;
    }
   if (sscanf (line, "FORMAT=%d", &i) == 1)
    {
     format=SUB_MPSUB;
     break;
    }
   if (sscanf (line, "FORMAT=TIM%c", &p)==1 && p=='E')
    {
     format=SUB_MPSUB|SUB_USESTIME;
     break;
    }
   if (strstr (line, "-->>"))
    {
     format=SUB_AQTITLE|SUB_USESTIME;
     break;
    }
   if (sscanf (line, "[%d:%d:%d]", &i, &i, &i)==3)
    {
     format=SUB_SUBRIP09|SUB_USESTIME;
     break;
    }
  }
 if (format!=SUB_INVALID) setSubEnc(format,fd);
 return format;  // too many bad lines
}

bool Tsubreader::subComp(const Tsubtitle *s1,const Tsubtitle *s2)
{
 return s1->start<s2->start;
}
void Tsubreader::timesort(void)
{
 std::sort(begin(),end(),subComp);
}

void Tsubreader::processDuration(const TsubtitlesSettings *cfg)
{
 timesort();
 if (cfg->isMinDuration)
  for (iterator s=begin();s!=end();s++)
   {
    REFERENCE_TIME minduration=0;
    switch (cfg->minDurationType)
     {
      case 0:minduration=cfg->minDurationSubtitle;break;
      case 1:minduration=cfg->minDurationLine*(*s)->numlines();break;
      case 2:minduration=cfg->minDurationChar*(*s)->numchars();break;
     }
    minduration*=REF_SECOND_MULT/1000;
    minduration=std::max(REFERENCE_TIME(1),minduration);
    if ((*s)->stop-(*s)->start<minduration)
     (*s)->stop=(*s)->start+minduration;
   }
}

void Tsubreader::adjust_subs_time(float subtime)
{
 if (empty()) return;
 timesort();

 int n=0,m=0;
 iterator sub=begin(),nextsub;
 int i = (int)size();
 REFERENCE_TIME subfms=REFERENCE_TIME(subtime)*REF_SECOND_MULT;
 for (;;)
  {
   if ((*sub)->stop <= (*sub)->start)
    {
     (*sub)->stop = (*sub)->start + subfms;
     m++;
     n++;
    }
   if (!--i) break;
   nextsub = sub + 1;
   if ((*sub)->stop >= (*nextsub)->start)
    {
     (*sub)->stop = (*nextsub)->start - 1;
     if ((*sub)->stop - (*sub)->start > subfms)
      (*sub)->stop = (*sub)->start + subfms;
     if (!m)
      n++;
    }
   sub = nextsub;
   m = 0;
  }
}

Tstream::ENCODING Tsubreader::getSubEnc(int format)
{
 format&=SUB_ENC_MASK;
 switch (format)
  {
   case SUB_ENC_UTF8:return Tstream::ENC_UTF8;
   case SUB_ENC_UNILE:return Tstream::ENC_LE16;
   case SUB_ENC_UNIBE:return Tstream::ENC_BE16;
   default:return Tstream::ENC_ASCII;
  }
}
void Tsubreader::setSubEnc(int &format,const Tstream &fs)
{
 switch (fs.encoding)
  {
   case Tstream::ENC_BE16:format|=SUB_ENC_UNIBE;break;
   case Tstream::ENC_LE16:format|=SUB_ENC_UNILE;break;
   case Tstream::ENC_UTF8:format|=SUB_ENC_UTF8 ;break;
  }
}
