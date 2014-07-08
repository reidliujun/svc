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
#include "TsubtitlesTextpinText.h"
#include "TsubreaderMplayer.h"
#include "ffdebug.h"
#include "IffdshowBase.h"
#include "Tstream.h"
#include "IffdshowDecVideo.h"

TsubtitlesTextpinText::TsubtitlesTextpinText(int Itype,IffdshowBase *Ideci,const unsigned char *extradata,unsigned int extradatalen):
 TsubtitlesTextpin(Itype,Ideci),
 initdata(extradatalen)
{
 subs=new Tsubreader;
 if ((type&Tsubreader::SUB_FORMATMASK)==Tsubreader::SUB_SSA && extradata && extradatalen)
  memcpy(initdata,extradata,extradatalen);
 parser=NULL;
 sub_format=type;
}

TsubtitlesTextpinText::~TsubtitlesTextpinText()
{
 if (parser) delete parser;
}

void TsubtitlesTextpinText::resetSubtitles(void)
{
 subs->clear();
 if (parser) delete parser;parser=NULL;
 TsubtitlesTextpin::resetSubtitles();
}

void TsubtitlesTextpinText::addSubtitle(REFERENCE_TIME start,REFERENCE_TIME stop,const unsigned char *data,unsigned int datalen,const TsubtitlesSettings *cfg,bool utf8)
{
 if (memcmp(data,"GAB2",4)==0)
  {
   if (subs) delete subs;
   const unsigned char *ptr=data+strlen("GAB2")+1;
   const unsigned char *end=data+datalen;
   while (data<end)
    {
     WORD tag = *((WORD*)(ptr)); ptr += 2;
     DWORD size = *((DWORD*)(ptr)); ptr += 4;
     if (tag==__GAB1_LANGUAGE_UNICODE__)
      ;//pRTS->m_name = (WCHAR*)ptr;
     else if(tag==__GAB1_RAWTEXTSUBTITLE__)
      {
       TstreamMem fs(ptr,end-ptr,Tstream::ENC_AUTODETECT);
       sub_format=Tsubreader::sub_autodetect(fs,ffcfg);
       fs.rewind();
       if (sub_format!=Tsubreader::SUB_INVALID)
        {
         unsigned int fps1000=25000;
         if (comptrQ<IffdshowDecVideo> deciV=deci)
          deciV->getAVIfps(&fps1000);
         subs=new TsubreaderMplayer(fs,sub_format,fps1000/1000.0,cfg,ffcfg,true);
        }
       else
        subs=NULL;
       break;
      }
     ptr+=size;
    }
  }
 else
  {
   if (!parser)
    {
     unsigned int fps1000=25000;
     if (comptrQ<IffdshowDecVideo> deciV=deci)
      deciV->getAVIfps(&fps1000);
     parser=TsubtitleParserBase::getParser(type,fps1000/1000.0,cfg,ffcfg,subs,utf8,true);
     if (initdata.size())
      {
       TstreamMem fs(initdata,initdata.size(),utf8 ? Tstream::ENC_UTF8 : Tstream::ENC_ASCII);
       parser->parse(fs, 0, start, stop);
      }
    }
   TstreamMem fs(data,datalen,utf8 ? Tstream::ENC_UTF8 : Tsubreader::getSubEnc(type));
   Tsubtitle *sub=parser->parse(fs, (type&Tsubreader::SUB_FORMATMASK)==Tsubreader::SUB_SSA ? TsubtitleParserBase::SSA_NODIALOGUE : 0, start, stop);
   if (sub)
    {
     sub->start=start;
     sub->stop=stop;
     init();
    }
  }
}
