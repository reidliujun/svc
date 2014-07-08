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
#include "reg.h"
#include "TpresetSettings.h"
#include "TfilterSettings.h"
#include "IffdshowBase.h"
#include "IffdshowDec.h"
#include "ffcodecs.h"
#include "dsutil.h"

const char_t *FFPRESET_DEFAULT=_l("default");

//============================ TautoPresetProps ===========================
TautoPresetProps::TautoPresetProps(IffdshowBase *Ideci):deci(Ideci),deciD(Ideci),wasVolume(-1),decoder(NULL),graphPtr(NULL)
{
}
TautoPresetProps::~TautoPresetProps()
{
}
const char_t* TautoPresetProps::getSourceFullFlnm(void)
{
 if (sourceName.empty())
  getSourceFlnm();
 return sourceFullFlnm.c_str();
}
const char_t* TautoPresetProps::getSourceName(void)
{
 if (sourceName.empty())
  getSourceFlnm();
 return sourceName.c_str();
}
const char_t* TautoPresetProps::getSourceNameExt(void)
{
 if (sourceName.empty())
  getSourceFlnm();
 return sourceNameExt.c_str();
}
const char_t* TautoPresetProps::getExeflnm(void)
{
 return deci->getExeflnm();
}
const char_t* TautoPresetProps::getExeItem(IffdshowDec*,unsigned int index)
{
 static const char_t *exes[]=
  {
   _l("mplayerc.exe"),
   _l("zplayer.exe"),
   _l("bsplayer.exe"),_l("bsplay.exe"),
   _l("coreplayer.exe"),
   _l("mplayer2.exe"),_l("wmplayer.exe"),
   _l("graphedt.exe")
  };
 return index<countof(exes)?exes[index]:NULL;
}
void TautoPresetProps::getVolume(void)
{
 const char_t *flnm=deci->getSourceName();
 char_t dsk[MAX_PATH];
 _splitpath_s(flnm,dsk,MAX_PATH,NULL,0,NULL,0,NULL,0);
 DWORD serial,maximumComponentLength,volumeFlags;
 wasVolume=GetVolumeInformation(dsk,volumeName,256,&serial,&maximumComponentLength,&volumeFlags,NULL,0);
 if (wasVolume)
  tsnprintf_s(volumeSerial, countof(volumeSerial), _TRUNCATE, _l("%X-%X"),(int)LOWORD(volumeSerial),(int)HIWORD(volumeSerial));
}
void TautoPresetProps::getSourceFlnm(void)
{
 Tpreset::normalizePresetName(sourceFullFlnm,deci->getSourceName());
 extractfilename(sourceFullFlnm.c_str(),sourceNameExt);
 extractfilenameWOext(sourceFullFlnm.c_str(),sourceName);
}
const char_t* TautoPresetProps::getVolumeName(void)
{
 if (wasVolume==-1)
  getVolume();
 return wasVolume?volumeName:_l("");
}
const char_t* TautoPresetProps::getVolumeSerial(void)
{
 if (wasVolume==-1)
  getVolume();
 return wasVolume?volumeSerial:_l("");
}
const char_t* TautoPresetProps::getDecoder(void)
{
 if (!decoder)
  decoder=getCodecName((CodecID)deci->getCurrentCodecId2());
 return decoder;
}
const char_t* TautoPresetProps::getDecoderItem(IffdshowDec *deciD,unsigned int index)
{
 const Tstrptrs *decoders=deciD->getCodecsList();
 return index<decoders->size()?(*decoders)[index]:NULL;
}
const char_t* TautoPresetProps::getPresetName(void)
{
 if (sourceName.empty())
  getSourceFlnm();
 return presetName;
}
bool TautoPresetProps::presetNameMatch(const char_t*,const char_t *presetName)
{
 return _stricoll(presetName,sourceFullFlnm.c_str())==0 || _stricoll(presetName,sourceNameExt.c_str())==0 || _stricoll(presetName,sourceName.c_str())==0;
}
bool TautoPresetProps::wildcardmatch(const char_t *mask,const char_t *flnm)
{
 return fnmatch(mask,flnm);
}
bool TautoPresetProps::stricoll(const char_t *s1,const char_t *s2)
{
 return ::_stricoll(s1,s2)==0;
}

const char_t* TautoPresetProps::getDSfilterName(void)
{
 return _l("dummy");
}
bool TautoPresetProps::dsfilterMatch(const char_t *f1,const char_t *)
{
 if (!graphPtr)
  {
   deci->getGraph(&graphPtr);
   if (graphPtr)
    {
     comptr<IEnumFilters> eff;
     if (graphPtr->EnumFilters(&eff)==S_OK)
      {
       eff->Reset();
       for (comptr<IBaseFilter> bff;eff->Next(1,&bff,NULL)==S_OK;bff=NULL)
        {
         char_t name[MAX_PATH],filtername[MAX_PATH];
         getFilterName(bff,name,filtername,countof(filtername));
         filtersnames.push_back(name);
        }
      }
    }
  }
 for (strings::const_iterator f2=filtersnames.begin();f2!=filtersnames.end();f2++)
  if (fnmatch(f1,f2->c_str()))
   return true;
 return false;
}

//======================== Tpreset::TautoPresetItem =======================
Tpreset::TautoPresetItem::TautoPresetItem(const TautoPresetItemDef *Iitem):item(Iitem)
{
 is=item->defIs;
 if (item->defVal)
  vals.push_back(item->defVal);
}
void Tpreset::TautoPresetItem::addWild(void)
{
 for (strings::iterator val=vals.begin();val!=vals.end();val++)
  *val=_l("*.")+*val;
}
void Tpreset::TautoPresetItem::reg_op(TregOp &t)
{
 t._REG_OP_N(0,item->regIs,is,item->defIs);
 if (item->regVal)
  {
   char_t val[MAX_PATH];mergetok(val,MAX_PATH,_l(";"),vals);
   t._REG_OP_S(0,item->regVal,val,MAX_PATH,item->defVal);
   strtok(val,_l(";"),vals);
  }
}
bool Tpreset::TautoPresetItem::match(TautoPresetProps &props) const
{
 if (!is) return false;
 const char_t *testVal=(props.*(item->getVal))();
 if (!testVal || testVal[0]=='\0') return false;
 if (item->regVal==NULL)
  return (props.*(item->compareFc))(NULL,testVal);
 else
  {
   for (strings::const_iterator val=vals.begin();val!=vals.end();val++)
    if ((props.*(item->compareFc))(val->c_str(),testVal))
     return true;
   return false;
  }
}

bool Tpreset::TautoPresetItem::getIs() const
{
 return !!is;
}

void Tpreset::TautoPresetItem::get(const char_t* *name,const char_t* *hint,int *allowWildcard,int *isPtr,int *isVal,char_t *val,size_t vallen,int *isList,int *isHelp) const
{
 *name=item->desc;
 *hint=item->hint;
 *allowWildcard=item->compareFc==&TautoPresetProps::wildcardmatch;
 *isPtr=is;
 *isVal=item->regVal!=NULL;
 if (*isVal)
  mergetok(val,vallen,_l(";"),vals);
 if (isList) *isList=item->getListItem!=NULL;
 if (isHelp) *isHelp=item->help!=NULL;
}
void Tpreset::TautoPresetItem::set(int Iis,const char_t *Ival)
{
 is=Iis;
 if (item->regVal)
  strtok(Ival,_l(";"),vals);
}
const char_t* Tpreset::TautoPresetItem::getList(IffdshowDec *deciD,unsigned int index)
{
 return item->getListItem(deciD,index);
}

//================================ Tpreset ================================
Tpreset::Tpreset(const char_t *Ireg_child, const char_t *IpresetName, int Imin_order, int Ifiltermode):
 Toptions(options=new TintStrColl),
 reg_child(Ireg_child),
 min_order(Imin_order),
 filtermode(Ifiltermode),
 filters(new TfilterIDFFs(Imin_order))
{
 memset(presetName,0,sizeof(presetName));
 ff_strncpy(presetName, IpresetName, countof(presetName));
 autoLoadedFromFile=0;
 autoLoadLogic=0;

 static const TautoPresetItemDef autoPresetItems[]=
  {
   {
    _l("on movie file name match with preset name"),NULL,
    _l("autoloadFlnm"),0,
    NULL,NULL,
    &TautoPresetProps::presetNameMatch,
    &TautoPresetProps::getPresetName,
   },
   {
    _l("on movie file name match (with wildcards)"),NULL,
    _l("autoloadExt"),0,
    _l("autoloadExts"),_l(""),
    &TautoPresetProps::wildcardmatch,
    &TautoPresetProps::getSourceNameExt
   },
   {
    _l("on application exe file name match"),NULL,
    _l("autoloadExe"),0,
    _l("autoloadExes"),_l(""),
    &TautoPresetProps::wildcardmatch,
    &TautoPresetProps::getExeflnm,
    &TautoPresetProps::getExeItem,
   },
   {
    _l("on volume name match"),NULL,
    _l("autoloadVolumeName"),0,
    _l("autoloadVolumeNames"),_l(""),
    &TautoPresetProps::wildcardmatch,
    &TautoPresetProps::getVolumeName,
   },
   {
    _l("on volume serial match"),_l("Format: XXXX-YYYY"),
    _l("autoloadVolumeSerial"),0,
    _l("autoloadVolumeSerials"),_l(""),
    &TautoPresetProps::stricoll,
    &TautoPresetProps::getVolumeSerial,
   },
   {
    _l("on decoder match"),NULL,
    _l("autoloadDecoder"),0,
    _l("autoloadDecoders"),_l(""),
    &TautoPresetProps::stricoll,
    &TautoPresetProps::getDecoder,
    &TautoPresetProps::getDecoderItem,
   },
   {
    _l("on a DirectShow filter presence"),_l("Names of DirectShow filters, wildcard allowed"),
    _l("autoloadDSfilter"),0,
    _l("autoloadDSfilters"),_l(""),
    &TautoPresetProps::dsfilterMatch,
    &TautoPresetProps::getDSfilterName
   },
   NULL
  };
 addAutoPresetItems(autoPresetItems);

 static const TintOptionT<Tpreset> iopts[]=
  {
   IDFF_presetAutoloadLogic        ,&Tpreset::autoLoadLogic          ,0,0,_l(""),0,
     _l("autoLoadLogic"),0,
   IDFF_autoLoadedFromFile         ,&Tpreset::autoLoadedFromFile     ,0,0,_l(""),0,
     NULL,0,
   0
  };
 addOptions(iopts);
}
Tpreset::~Tpreset()
{
 for (TfilterIDFFs::iterator f=filters->begin();f!=filters->end();f++)
  delete f->cfg;
 delete filters;
}

Tpreset& Tpreset::operator =(const Tpreset &src)
{
 min_order=src.min_order;
 reg_child=src.reg_child;

 ff_strncpy(presetName, src.presetName, countof(presetName));
 autoLoadedFromFile=src.autoLoadedFromFile;
 autoloadExtsNeedFix=src.autoloadExtsNeedFix;
 autoPresetItems=src.autoPresetItems;
 autoLoadLogic=src.autoLoadLogic;

 filters->copy(src.filters);
 return *this;
}

void Tpreset::reg_op(TregOp &t)
{
 Toptions::reg_op(t);

 for (TfilterIDFFs::iterator f=filters->begin();f!=filters->end();f++)
  if (f->cfg)
   f->cfg->reg_op(t);

 t._REG_OP_N(0,_l("autoloadExtsNeedFix"),autoloadExtsNeedFix,1);
 for (TautoPresetItems::iterator a=autoPresetItems.begin();a!=autoPresetItems.end();a++)
  {
   a->reg_op(t);
   if (autoloadExtsNeedFix && a->item->getVal==&TautoPresetProps::getSourceName)
     {
      autoloadExtsNeedFix=0;
      a->addWild();
     }
  }
}

void Tpreset::loadDefault(void)
{
 ff_strncpy(presetName, FFPRESET_DEFAULT, countof(presetName));
 loadReg();
}

void Tpreset::loadReg(void)
{
 char_t presetRegStr[MAX_PATH];
 tsnprintf_s(presetRegStr, countof(presetRegStr), _TRUNCATE, FFDSHOW_REG_PARENT _l("\\%s\\%s"), reg_child, presetName);
 TregOpRegRead t(HKEY_CURRENT_USER,presetRegStr);
 reg_op(t);
 fixOrder();
}
void Tpreset::saveReg(void)
{
 char_t presetRegStr[MAX_PATH];
 tsnprintf_s(presetRegStr, countof(presetRegStr), _TRUNCATE, FFDSHOW_REG_PARENT _l("\\%s\\%s"), reg_child, presetName);
 TregOpRegWrite t(HKEY_CURRENT_USER,presetRegStr);
 reg_op(t);
}

bool Tpreset::loadFile(const char_t *flnm)
{
 if (!fileexists(flnm)) return false;
 char_t sections[4096]=_l("");
 Tinifile ini(flnm);
 ini.getPrivateProfileSectionNames(sections,4095);
 if (sections[0]=='\0') return false;
 _splitpath_s(flnm,NULL,0,NULL,0,presetName,MAX_PATH,NULL,0);
 TregOpFileRead t(flnm,sections);
 reg_op(t);
 fixOrder();
 return true;
}
bool Tpreset::saveFile(const char_t *flnm)
{
 DeleteFile(flnm);
 TregOpFileWrite t(flnm,_l("presetSettings"));
 reg_op(t);
 return fileexists(flnm); //TODO: TregOpFileWrite should throw exception when writing fails
}

void Tpreset::normalizePresetName(char_t *dst, const char_t *src, size_t bufsize)
{
 int i = 0;
 char_t c;
 do
 {
  c=*(src++);
  if (c=='\\') c='/';
  dst[i++] = c;
 } while (c && i < bufsize);
 dst[bufsize - 1] = 0;
}

void Tpreset::normalizePresetName(ffstring &dst,const char_t *src)
{
 char_t c;
 size_t len = strlen(src) + 1;
 if (len > 4096)
  {
   dst = _l("");
  }
 char_t *dstbuf = (char_t *)_alloca(len * sizeof(char_t));
 size_t i = 0;
 do
 {
  c=*(src++);
  if (c=='\\') c='/';
  dstbuf[i++] = c;
 } while (c);
 dst = dstbuf;
}

bool Tpreset::isValidPresetName(const char_t *presetName)
{
 if (presetName[0]=='\0') return false;
 for (unsigned int i=0;i<strlen(presetName);i++)
  if (presetName[i]=='*' || presetName[i]=='?') return false;
 return true;
}

void Tpreset::addAutoPresetItems(const TautoPresetItemDef *IautoPresetItems)
{
 for (;IautoPresetItems->regIs;IautoPresetItems++)
  autoPresetItems.push_back(IautoPresetItems);
}

bool Tpreset::isAutoPreset(TautoPresetProps &props) const
{
 props.presetName=presetName;

 unsigned int dx,dy;

 props.getSourceResolution(&dx,&dy);

 if (autoLoadLogic)
  {
   // On all conditions match (AND)
   bool match=false;
   if (is_autoloadSize())
    {
     if (autoloadSizeMatch(dx,dy))
      match=true;
     else
      return false;
    }

   for (TautoPresetItems::const_iterator a=autoPresetItems.begin();a!=autoPresetItems.end();a++)
    if (a->getIs())
     {
      if (!a->match(props))
       return false;
      else
       match=true;
     }

   return match;
  }
 else
  {
   // On one of the conditoins match (OR)
   if (is_autoloadSize() && autoloadSizeMatch(dx,dy))
    return true;
   for (TautoPresetItems::const_iterator a=autoPresetItems.begin();a!=autoPresetItems.end();a++)
    if (a->match(props))
     return true;
   return false;
  }
}

int Tpreset::getMinOrder(void) const
{
 return min_order;
}
int Tpreset::getMaxOrder(void) const
{
 return filters->maxOrder();
}

bool Tpreset::Torders::orderSort(const Torder &o1,const Torder &o2)
{
 if (o1.get<CFG>()->order==o2.get<CFG>()->order)
  return o1.get<ORDERDEF>()<o2.get<ORDERDEF>();
 else
  return o1.get<CFG>()->order<o2.get<CFG>()->order;
}
void Tpreset::Torders::sort(void)
{
 std::sort(begin(),end(),orderSort);
}

void Tpreset::fixOrder(void)
{
 Torders orders;getOrders(orders);
 orders.sort();
 int o=min_order;
 for (Torders::iterator i=orders.begin();i!=orders.end();i++,o++)
  i->get<CFG>()->order=o;
}
TfilterSettings* Tpreset::getSettings(int filterID) const
{
 for (TfilterIDFFs::iterator f=filters->begin();f!=filters->end();f++)
  if (f->idff->id==filterID)
   return f->cfg;
 return NULL;
}
const TfilterSettings* Tpreset::indexGetSettings(size_t index) const
{
 return index>=filters->size()?NULL:(*filters)[index].cfg;
}

void Tpreset::getOrders(Torders &orders,bool all) const
{
 for (TfilterIDFFs::const_iterator f=filters->begin();f!=filters->end();f++)
  if (all || f->idff->order)
   orders.push_back(std::make_tuple(f-filters->begin(),f->cfg,f->orderDef));
}

bool Tpreset::setFilterOrder(unsigned int filterID,unsigned int newOrder)
{
 int *o;
 TfilterSettings *fs=getSettings(filterID);
 if (!fs) return false;
 o=&fs->order;
 Torders orders;getOrders(orders);
 for (Torders::iterator i=orders.begin();i!=orders.end();i++)
  i->get<CFG>()->order=i->get<CFG>()->order*10+1;
 *o=newOrder*10;
 fixOrder();
 return true;
}

bool Tpreset::resetOrder(void)
{
 Torders orders;getOrders(orders);
 bool waschange=false;
 for (Torders::iterator o=orders.begin();o!=orders.end();o++)
  {
   waschange|=(o->get<CFG>()->order!=o->get<ORDERDEF>());
   o->get<CFG>()->order=o->get<ORDERDEF>();
  }
 return waschange;
}

const TfilterIDFFs* Tpreset::getFilters(void)
{
 return filters;
}

void Tpreset::createFilters(Tfilters *filters,TfilterQueue &queue) const
{
 Torders orders;
 getOrders(orders,true);
 orders.sort();
 for (Torders::const_iterator o=orders.begin();o!=orders.end();o++)
  o->get<CFG>()->createFilters(o->get<ORDER>(),filters,queue);
}

void Tpreset::createPages(TffdshowPageDec *pages) const
{
 Torders orders;
 getOrders(orders);
 for (Torders::const_iterator o=orders.begin();o!=orders.end();o++)
  o->get<CFG>()->createPages(pages);
}
