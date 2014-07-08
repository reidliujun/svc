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
#include "TsubreaderVobsub.h"
#include "vobsub.h"
#include "spudec.h"
#include "TsubtitleVobsub.h"
#include "IffdshowBase.h"
#include "IffdshowDecVideo.h"
#include "ffdshow_constants.h"
#include "Ttranslate.h"
#include "TsubtitlesSettings.h"

TsubreaderVobsub::TsubreaderVobsub(Tstream &f,const char_t *flnm,IffdshowBase *Ideci,Tspudec* *spuPtr):spu(NULL),deci(Ideci)
{
 char_t flnm2[MAX_PATH];
 if (flnm)
  {
   char_t dsk[MAX_PATH],dir[MAX_PATH],name[MAX_PATH];
   _splitpath_s(flnm,dsk,MAX_PATH,dir,MAX_PATH,name,MAX_PATH,NULL,0);
   _makepath_s(flnm2,MAX_PATH,dsk,dir,name,NULL);
  }
 else
  flnm2[0]='\0';
 vobsub=new Tvobsub(f,flnm2,NULL,0,&spu,deci);
 if (spuPtr) *spuPtr=spu;
 if (deciV=deci)
  {
   const char_t **langs=(const char_t**)calloc(vobsub->spu_streams_size+1,sizeof(const char_t*));
   for (unsigned int i=0;i<vobsub->spu_streams_size;i++)
    langs[i]=vobsub->spu_streams[i].altid?vobsub->spu_streams[i].altid:(vobsub->spu_streams[i].id?TsubtitlesSettings::getLangDescr(vobsub->spu_streams[i].id):_l(""));
   deciV->fillSubtitleLanguages(langs);
   free(langs);
  }
}
TsubreaderVobsub::~TsubreaderVobsub()
{
 if (deciV)
  {
   static const char_t *langs[]={NULL};
   deciV->fillSubtitleLanguages(langs);
  }
 delete vobsub;
 if (spu) delete spu;
}
int TsubreaderVobsub::findlang(int langname)
{
 if (vobsub->ok && spu && langname!=0)
  {
   char_t langS[3]=_l("  ");
   _tcsncpy(langS, (const char_t *)text<char_t>((const char*)&langname),2);
   for (unsigned int i=0;i<vobsub->spu_streams_size;i++)
    if (vobsub->spu_streams[i].id && stricmp(langS,vobsub->spu_streams[i].id)==0)
     return i;
  }
 return 0;
}

void TsubreaderVobsub::setLang(int newlang)
{
 if (!vobsub->ok || !spu || newlang>=(int)vobsub->spu_streams_size) return;
 vobsub->vobsub_id=newlang;
 langid=newlang;
 clear();
 spu->spudec_reset();
 vobsub->vobsub_reset();
 push_back(new TsubtitleVobsub(spu,vobsub));
 deci->putParam(IDFF_subCurLang,vobsub->vobsub_id);
}
