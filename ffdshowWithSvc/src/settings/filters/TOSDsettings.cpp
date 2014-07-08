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
#include "TOSDsettings.h"
#include "TimgFilterOSD.h"

//========================================= TOSDsettings =========================================
TOSDsettings::TOSDsettings(size_t IsizeofthisAll,TintStrColl *Icoll,TfilterIDFFs *filters,const char_t *IdefPreset):
 TfilterSettings(IsizeofthisAll,sizeof(TfilterSettings),Icoll,filters,NULL,true),
 defPreset(IdefPreset),
 changed(true)
{
 order=INT_MAX;
 memset(startPreset,0,sizeof(startPreset));
 static const TintOptionT<TOSDsettings> iopts[]=
  {
   IDFF_isOSD                     ,&TOSDsettings::is            ,0,0,_l(""),0,
     _l("isOSD"),0,
   IDFF_OSDstartDuration          ,&TOSDsettings::startDuration ,1,10000,_l(""),0,
     _l("OSDstartDuration"),100,
   IDFF_OSDisSave                 ,&TOSDsettings::isSave        ,0,0,_l(""),0,
     _l("OSDisSave"),0,
   IDFF_OSDsaveOnly               ,&TOSDsettings::saveOnly      ,0,0,_l(""),0,
     _l("OSDsaveOnly"),0,
   0
  };
 addOptions(iopts);
 static const TstrOption sopts[]=
  {
   IDFF_OSDcurPreset  ,(TstrVal)&TOSDsettings::curPreset  ,40       ,0 ,_l(""),0,
     _l("OSDcurPreset"),_l("default"),
   IDFF_OSDstartPreset,(TstrVal)&TOSDsettings::startPreset,40       ,0 ,_l(""),0,
     _l("OSDstartPreset"),_l(""),
   IDFF_OSDsaveFlnm   ,(TstrVal)&TOSDsettings::saveFlnm   ,MAX_PATH ,0 ,_l(""),0,
     _l("OSDsaveFlnm"),_l(""),
   0
  };
 addOptions(sopts);
 setOnChange(IDFF_OSDcurPreset,this,&TOSDsettings::onCurPresetChange);
}

void TOSDsettings::savePresets(const char_t *reg_child)
{
 char_t rkey[MAX_PATH];
 tsnprintf_s(rkey, countof(rkey), _TRUNCATE, FFDSHOW_REG_PARENT _l("\\%s"), reg_child);
 TregOpRegWrite t(HKEY_CURRENT_USER,rkey);
 int size=(int)presets.size();
 t._REG_OP_N(0,_l("OSDpresetsCount"),size,0);
 for (int i=0;i<size;i++)
  {
   char_t regname[100];
   tsprintf(regname,_l("OSDpresetName%i"),i);
   t._REG_OP_S(0,regname,(char_t*)presets[i].first.c_str(),40,_l(""));
   tsprintf(regname,_l("OSDpresetFormat%i"),i);
   t._REG_OP_S(0,regname,(char_t*)presets[i].second.c_str(),256,_l(""));
  }
}
void TOSDsettings::loadPresets(const char_t *reg_child)
{
 char_t rkey[MAX_PATH];
 tsnprintf_s(rkey, countof(rkey), _TRUNCATE, FFDSHOW_REG_PARENT _l("\\%s"), reg_child);
 TregOpRegRead t(HKEY_CURRENT_USER,rkey);
 int size;
 presets.clear();
 t._REG_OP_N(0,_l("OSDpresetsCount"),size,0);
 if (size==0)
  {
   char_t oldOSDformat[256];
   t._REG_OP_S(0,_l("OSDformat"),oldOSDformat,256,defPreset);
   presets.push_back(std::make_pair((const char_t*)_l("default"),(const char_t*)oldOSDformat));
  }
 else
  for (int i=0;i<size;i++)
   {
    char_t regname[100];
    tsprintf(regname,_l("OSDpresetName%i"),i);
    char_t name[40];
    t._REG_OP_S(0,regname,name,40,_l(""));
    tsprintf(regname,_l("OSDpresetFormat%i"),i);
    char_t format[256];
    t._REG_OP_S(0,regname,format,256,_l(""));
    presets.push_back(std::make_pair((const char_t*)name,(const char_t*)format));
   }
 if (!getFormat())
  strcpy(curPreset,_l("default"));
 changed=true;
}

HRESULT TOSDsettings::setPresetName(unsigned int i,const char_t *name)
{
 if (i>=presets.size()) return E_INVALIDARG;
 changed=true;
 presets[i].first=name;
 return S_OK;
}

const char_t* TOSDsettings::getStartupFormat(int *duration) const
{
 if (const char_t *format=getFormat(startPreset))
  {
   if (duration) *duration=startDuration;
   return format;
  }
 else
  {
   if (duration) *duration=0;
   return _l("");
  }
}

const char_t* TOSDsettings::getFormat(void) const
{
 if (changed)
  {
   oldformat=getFormat(curPreset);
   changed=false;
  }
 return oldformat;
}

const char_t* TOSDsettings::getFormat(const char_t *presetName) const
{
 if (!presetName)
  presetName=curPreset;
 for (Tpresets::const_iterator i=presets.begin();i!=presets.end();i++)
  if (i->first==presetName)
   return i->second.c_str();
 return NULL;
}
void TOSDsettings::addPreset(const char_t *presetName,const char_t *format)
{
 changed=true;
 presets.push_back(std::make_pair(presetName,format));
}
bool TOSDsettings::setFormat(const char_t *presetName,const char_t *format)
{
 if (!presetName) presetName=curPreset;
 for (Tpresets::iterator i=presets.begin();i!=presets.end();i++)
  if (i->first==presetName)
   {
    changed=true;
    i->second=format;
    return true;
   }
 return false;
}
bool TOSDsettings::erase(const char_t *presetName)
{
 if (!presetName) presetName=curPreset;
 for (Tpresets::iterator i=presets.begin();i!=presets.end();i++)
  if (i->first==presetName)
   if (i==presets.begin())
    return false;
   else
    {
     changed=true;
     presets.erase(i);
     return true;
    }
 return false;
}
bool TOSDsettings::cycle(void)
{
 if (presets.size()<=1) return false;
 for (Tpresets::const_iterator i=presets.begin();i!=presets.end();i++)
  if (i->first==curPreset)
   {
    i++;
    if (i==presets.end()) i=presets.begin();
    ff_strncpy(curPreset,i->first.c_str(),countof(curPreset));
    changed=true;
    return true;
   }
 return false;
}
void TOSDsettings::onCurPresetChange(int id,const char_t*val)
{
 changed=true;
}

//====================================== TOSDsettingsVideo =======================================
TOSDsettingsVideo::TOSDsettingsVideo(TintStrColl *Icoll,TfilterIDFFs *filters):
 TOSDsettings(sizeof(*this),Icoll,filters,_l("14 1522 1520 1521")),
 font(Icoll)
{
 linespace=100;
 memset(user,0,sizeof(user));
 userPx=userPy=0;

 static const TintOptionT<TOSDsettingsVideo> iopts[]=
  {
   IDFF_OSDuserPx                 ,&TOSDsettingsVideo::userPx        ,-1,-1,_l(""),0,NULL,0,
   IDFF_OSDuserPy                 ,&TOSDsettingsVideo::userPy        ,-1,-1,_l(""),0,NULL,0,
   IDFF_OSDposX                   ,&TOSDsettingsVideo::posX          ,0,100,_l(""),0,
     _l("OSDposX"),0,
   IDFF_OSDposY                   ,&TOSDsettingsVideo::posY          ,0,100,_l(""),0,
     _l("OSDposY"),0,
   IDFF_OSD_userformat            ,&TOSDsettingsVideo::userFormat    ,0,100,_l(""),0,
     _l("OSDuserFormat"),3,
   0
  };
 addOptions(iopts);

 static const TstrOption sopts[]=
  {
   IDFF_OSDuser                   ,(TstrVal)&TOSDsettingsVideo::user ,2048,0 ,_l(""),0,NULL,NULL,
   0
  };
 addOptions(sopts);
}

void TOSDsettingsVideo::reg_op(TregOp &t)
{
 TfilterSettings::reg_op(t);
 font.reg_op(t);
}

void TOSDsettingsVideo::resetLook(void)
{
 posX=0;posY=0;
}

void TOSDsettingsVideo::createFilters(size_t filtersorder,Tfilters *filters,TfilterQueue &queue) const
{
 if (!queue.temporary) setOnChange(IDFF_isOSD,filters,&Tfilters::onQueueChange);
 queueFilter<TimgFilterOSD>(filtersorder,filters,queue);
}

//====================================== TOSDsettingsAudio =======================================
TOSDsettingsAudio::TOSDsettingsAudio(TintStrColl *Icoll,TfilterIDFFs *filters):
 TOSDsettings(sizeof(*this),Icoll,filters,_l("1544 1529"))
{
}

void TOSDsettingsAudio::createFilters(size_t filtersorder,Tfilters *filters,TfilterQueue &queue) const
{
 //if (!queue.temporary) setOnChange(IDFF_isOSD,filters,&Tfilters::onQueueChange);
 //queueFilter<TimgFilterOSD>(filtersorder,filters,queue);
}
