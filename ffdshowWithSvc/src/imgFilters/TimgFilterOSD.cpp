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
#include "TimgFilterOSD.h"
#include "IffdshowBase.h"
#include "IffdshowDec.h"
#include "IffdshowDecVideo.h"
#include "TOSDsettings.h"
#include "Ttranslate.h"

//=============================== TimgFilterOSD::TosdLine::TosdValue =========================
const char_t* TimgFilterOSD::TosdLine::TosdValue::getName(void) const
{
 if (!name)
  name=provider->getInfoItemName(type);
 return name;
}
const char_t* TimgFilterOSD::TosdLine::TosdValue::getVal(bool &wasChange,bool &splitline,const TffPict &pict)
{
 splitline=false;
 switch (type)
  {
   case IDFF_OSDtype_outputSize:
    {
     char_t news[256];
     tsprintf(news,_l("%ux%u"),pict.rectFull.dx,pict.rectFull.dy);
     if (strcmp(news,s)!=0)
      {
       ff_strncpy(s, news, countof(s));
       wasChange=true;
      }
     break;
    }
   case IDFF_OSDtype_outputAspect:
    {
     const Rational &sar=pict.rectFull.sar,dar=pict.rectFull.dar();
     bool sarOk=sar;
     bool darOk=dar;
     if (oldSar!=sar || oldDar!=dar)
      {
       if (sarOk)
        tsprintf(s,_l("SAR: %i/%i, "),sar.num,sar.den);
       else
        tsprintf(s,_l("SAR: N/A, "));
       if (darOk)
        strncatf(s, countof(s), _l("DAR: %i/%i"),dar.num,dar.den);
       else
        strncatf(s, countof(s), _l("DAR: N/A"));
       wasChange=true;
      }
     oldSar=sar;oldDar=dar;
     break;
    }
   case IDFF_frameType:
    {
     const char_t *ipb0=FRAME_TYPE::name(pict.frametype&FRAME_TYPE::typemask);
     char_t ipb[30];
     strcpy(ipb,stricmp(ipb0,_l("s"))==0?_l("GMC"):ipb0);
     tsnprintf_s(s, countof(s), _TRUNCATE, _l("%s%s"), strupr(ipb), pict.frametype & FRAME_TYPE::QPEL ? _l("\t QPEL") : _l(""));
     wasChange=true;
     break;
    }
   case IDFF_frameSize:
    {
     tsprintf(s,_l("%u"),(unsigned int)pict.srcSize);
     wasChange=true;
     break;
    }
   case IDFF_OSDtype_gmcWarpPoints:
    tsprintf(s,_l("%i (used: %i)"),pict.gmcWarpingPoints,pict.gmcWarpingPointsReal);
    wasChange=strcmp(s,olds)!=0;
    break;
   case IDFF_OSDtype_frameMD5:
    {
     uint32_t sum[4];
     pict.md5sum((uint8_t*)sum);
     tsprintf(s,_l("%08X%08X%08X%08X"),sum[0],sum[1],sum[2],sum[3]);
     wasChange=true;
     break;
    }
   case IDFF_OSDtype_frameTimestamps:
    {
     tsprintf(s,_l("%I64i - %I64i"),pict.rtStart,pict.rtStop);
     wasChange=true;
     break;
    }
   case IDFF_OSDtype_frameDuration:
    {
     double duration=(pict.rtStop-pict.rtStart)/(REF_SECOND_MULT/1000.0);
     tsprintf(s,_l("%g ms"),duration);
     wasChange=duration!=oldDuration;
     oldDuration=duration;
     break;
    }
   default:
    {
     const char_t *val;int wasChangeI,splitlineI;
     if (provider->getInfoItemValue(type,&val,&wasChangeI,&splitlineI)==S_OK)
      {
       ff_strncpy(s,val,countof(s)-1);s[countof(s)-1]='\0';
       wasChange=!!wasChangeI;splitline=!!splitlineI;
      }
     else
      assert(0);
     break;
    }
  }
 ff_strncpy(olds, s, countof(olds));
 return s;
}

//============================== TimgFilterOSD::TosdLine::TosdToken ==========================
const char_t* TimgFilterOSD::TosdLine::TosdToken::getName(void) const
{
 switch (type)
  {
   case TOKEN_VALUE:return val.getName();
   default:
   case TOKEN_STRING:return string.c_str();
  }
}

//================================== TimgFilterOSD::TosdLine =================================
TimgFilterOSD::TosdLine::TosdLine(IffdshowBase *Ideci,IffdshowDec *IdeciD,IffdshowDecVideo *IdeciV,const Tconfig *Iconfig,const ffstring &format,const TfontSettings *fontSettings,unsigned int Iduration,IOSDprovider *Iprovider,bool Iitalic):
// deci(Ideci),
 config(Iconfig),
 font(Ideci),
 sub(0,TSubtitleProps(Iitalic,false)),
 provider(Iprovider)
{
 duration=Iduration;
 font.init(fontSettings);
 if (strncmp(format.c_str(),_l("user"),4)==0)
  {
   char_t s0[256]=_l(""),*s=s0;
   for (const char_t *f=format.c_str()+4;*f;)
    if (f[0]=='%' && f[1])
     {
      if (f[1]=='%')
       {
        *s++='%';f+=2;
        continue;
       }
      int idff=0;
      if (isIn(f[1],_l('0'),_l('9')))
       {
        char_t *endl;
        idff=strtol(f+1,&endl,10);
        f=endl;
       }
      else
       {
        int toklen;
        if (int sidff=provider->getInfoShortcutItem(f+1,&toklen))
         {
          idff=sidff;
          f+=toklen+1;
         }
       }
      if (idff)
       {
        if (s0[0])
         {
          *s='\0';
          tokens.push_back(TosdToken(s0));
         }
        tokens.push_back(TosdToken(idff,provider));
        s0[0]='\0';s=s0;
       }
      else
       f++;
     }
    else
     *s++=*f++;
    if (s0[0])
     {
      *s='\0';
      tokens.push_back(TosdToken(s0));
     }
  }
 else if (strncmp(format.c_str(),_l("shortosd"),8)==0)
  tokens.push_back(TosdToken(format.c_str()+8));
 else
  {
   int type=atoi(format.c_str()+1);
   char_t valName[256];
   ff_strncpy(valName, provider->getInfoItemName(type), countof(valName));
   strncat_s(valName, countof(valName), _l(": "), _TRUNCATE);
   tokens.push_back(TosdToken(valName));
   tokens.push_back(TosdToken(type,provider));
  }
 firsttime=true;
}
const char_t* TimgFilterOSD::TosdLine::getName(unsigned int i) const
{
 return i>=tokens.size()?_l(""):tokens[i].getName();
}

void TimgFilterOSD::TosdLine::print(IffdshowBase *deci,const TffPict &pict,unsigned char *dst[4],stride_t stride[4],unsigned int dxY,unsigned int dyY,unsigned int x,unsigned int &y,int linespace,FILE *f,bool fileonly)
{
 bool wasChange=firsttime,splitline=false;
 firsttime=false;
 ffstring text;
 for (TosdTokens::iterator t=tokens.begin();t!=tokens.end();t++)
  {
   bool wasChange0,splitline0;
   const char_t *val=t->getStr(wasChange0,splitline0,pict);
   if(val[0]!=':') // deleted item
    text+=val;
   wasChange|=wasChange0;splitline|=splitline0;
   if (f && t->type==TosdToken::TOKEN_VALUE)
    tfprintf(f,_l("%s"),val);
  }
 if (!fileonly && !text.empty())
  {
   if (y<dyY)
    {
     TrenderedSubtitleLines::TprintPrefs printprefs(deci);
     printprefs.isOSD=true;
     printprefs.dst=dst;
     printprefs.stride=stride;
     printprefs.shiftX=pict.cspInfo.shiftX;printprefs.shiftY=pict.cspInfo.shiftY;
     printprefs.dx=dxY;
     printprefs.dy=dyY;
     printprefs.xpos=-1*int(x);
     printprefs.ypos=-1*int(y);
     printprefs.align=ALIGN_LEFT;
     printprefs.linespacing=linespace;
     printprefs.deci=deci;
     printprefs.config=config;
     printprefs.shadowMode=-1;
     printprefs.csp=pict.csp & FF_CSPS_MASK;
     printprefs.cspBpp=pict.cspInfo.Bpp;
     printprefs.sar=pict.rectFull.sar;
     if (splitline)
      {
       printprefs.fontchangesplit=1;
       printprefs.fontsplit=1;
      }
     sub.set(text);
     font.print(&sub,wasChange,printprefs,&y);
    }
  }
}

//================================== TimgFilterOSD::Tosds ====================================
TimgFilterOSD::Tosds::Tosds(IOSDprovider *Iprovider,const char_t *Iname):f(NULL),provider(Iprovider)
{
 if (const char_t *sfmt=provider->getStartupFormat(&startupDuration))
  startupFormat=sfmt;
 oldFormat[0]='\0';
 oldSave=-1;oldSaveFlnm[0]='\0';
 if (Iname)
  {
   ff_strncpy(name,Iname,MAX_PATH);
   name[259]='\0';
  }
 else
  name[0]='\0';
}
TimgFilterOSD::Tosds::~Tosds()
{
 freeOsds();
}

void TimgFilterOSD::Tosds::init(bool allowSave,IffdshowBase *deci,IffdshowDec *deciD,IffdshowDecVideo *deciV,const Tconfig *config,TfontSettingsOSD *oldFont,const TOSDsettings *cfg,int framecnt)
{
 if (framecnt<startupDuration || provider->isOSD())
  {
   is=true;
   const char_t *format;
   if (framecnt<startupDuration)
    format=startupFormat.c_str();
   else
    format=provider->getFormat();

   int cfgIsSave=false;const char_t* cfgSaveFlnm;provider->getSave(&cfgIsSave,&cfgSaveFlnm);
   int initSave=-1;
   if (oldSave!=cfgIsSave || stricmp(oldSaveFlnm,cfgSaveFlnm))
    {
     oldSave=cfgIsSave;
     ff_strncpy(oldSaveFlnm, cfgSaveFlnm, countof(oldSaveFlnm));
     initSave=allowSave && cfgIsSave && cfgSaveFlnm[0]!='\0'?1:0;
    }
   if (strcmp(oldFormat,format)!=0)
    {
     ff_strncpy(oldFormat, format, countof(oldFormat));
     freeOsds();
     if (name)
      push_back(new TosdLine(deci,deciD,deciV,config,ffstring(_l("user"))+ffstring(name),oldFont,0,provider,true));
     if (strncmp(format,_l("user"),4)==0)
      {
       strings lines;
       strtok(format,_l("\\n"),lines);
       for (size_t i=0;i<lines.size();i++)
        push_back(new TosdLine(deci,deciD,deciV,config,ffstring(i==0?_l(""):_l("user"))+lines[i],oldFont,0,provider));
       initSave=0;
      }
     else
      {
       ints types;strtok(format,_l(" "),types);
       for (size_t i=0;i<types.size();i++)
        {
         char_t pomS[10];
         push_back(new TosdLine(deci,deciD,deciV,config,ffstring(_l("%"))+ffstring(_itoa(types[i],pomS,10)),oldFont,0,provider));
        }
       initSave=cfgIsSave && cfgSaveFlnm[0]!='\0'?1:0;
      }
    }
   if (initSave!=-1)
    {
     done();
     if (initSave==1)
      {
       f=fopen(cfgSaveFlnm,_l("wt"));
       if (f)
        {
         for (const_iterator o=begin();o!=end();o++)
          tfprintf(f,_l("%s%s"),(*o)->getName(0),o!=end()-1?_l(";"):_l(""));
         tfprintf(f,_l("\n"));
        }
      }
    }
  }
 else
  {
   is=false;
   oldFormat[0]='\0';
  }
}

void TimgFilterOSD::Tosds::fontInit(const TfontSettingsOSD *fontSettings)
{
 for (iterator i=begin();i!=end();i++)
  (*i)->font.init(fontSettings);
}

void TimgFilterOSD::Tosds::print(IffdshowBase *deci,const TffPict &pict,unsigned char *dst[4],stride_t stride2[4],unsigned int dxY,unsigned int dyY,unsigned int x,unsigned int &y,int linespace,bool fileonly)
{
 for (iterator o=begin();o!=end() && y<dyY;)
  {
   (*o)->print(deci,pict,dst,stride2,dxY,dyY,x,y,linespace,f,fileonly);
   if (f && o!=end()-1)
    fprintf(f,";");

   if ((*o)->duration>0)
    {
     (*o)->duration--;
     if ((*o)->duration==0)
      {
       delete *o;
       o=erase(o);
       continue;
      }
    }
   o++;
  }
 if (f) fprintf(f,"\n");
}

void TimgFilterOSD::Tosds::done(void)
{
 if (f) fclose(f);f=NULL;
}
void TimgFilterOSD::Tosds::freeOsds(void)
{
 for (Tosds::iterator i=begin();i!=end();i++)
  delete *i;
 clear();
}

//===================================== TimgFilterOSD ========================================
bool TimgFilterOSD::TprovOSDs::empty(void) const
{
 if (std::vector<Tosds*>::empty()) return true;
 for (const_iterator op=begin();op!=end();op++)
  if ((*op)->is && !(*op)->empty())
   return false;
 return true;
}

//===================================== TimgFilterOSD ========================================
TimgFilterOSD::TimgFilterOSD(IffdshowBase *Ideci,Tfilters *Iparent):
 TimgFilter(Ideci,Iparent),
 parent((TimgFiltersPlayer*)Iparent),
 fontUser(Ideci),
 subUser(Ideci->getParam2(IDFF_OSD_userformat)),
 framecnt(0),
 oldFont((TfontSettingsOSD*)malloc(sizeof(TfontSettingsOSD))),
 trans(NULL),
 shortOSD(this),
 TOSDprovider(deci,deciD)
{
 provOSDs.push_back(new Tosds(this));
 oldFont->weight=-1;
 oldLinesUser[0]='\0';
}
TimgFilterOSD::~TimgFilterOSD()
{
 CAutoLock l(&csProvider);
 for (TprovOSDs::iterator po=provOSDs.begin();po!=provOSDs.end();po++)
  delete *po;
 ::free(oldFont);
 if (trans) trans->release();
}
void TimgFilterOSD::done(void)
{
 CAutoLock l(&csProvider);
 for (TprovOSDs::iterator po=provOSDs.begin();po!=provOSDs.end();po++)
  (*po)->done();
}

void TimgFilterOSD::onSizeChange(void)
{
 oldFont->weight=-1;
}

bool TimgFilterOSD::shortOSDmessage(const char_t *msg,unsigned int duration)
{
 cs.Lock();
 shortOSDtemp.push_back(std::make_pair(msg,duration));
 cs.Unlock();
 return true;
}

bool TimgFilterOSD::is(const TffPictBase &pict,const TfilterSettingsVideo *cfg0)
{
 const TOSDsettingsVideo *cfg=(const TOSDsettingsVideo*)cfg0;
 const char_t *startup;
 return cfg->is || ((startup=cfg->getStartupFormat(NULL))!=NULL && startup[0]) || cfg->user[0];
}

HRESULT TimgFilterOSD::process(TfilterQueue::iterator it,TffPict &pict,const TfilterSettingsVideo *cfg0)
{
 const TOSDsettingsVideo *cfg=(const TOSDsettingsVideo*)cfg0;tempcfg=cfg;
 if (subUser.subformat != cfg->userFormat)
	 subUser.subformat = cfg->userFormat;

 init(pict,true,0);
 csProvider.Lock();
 if (memcmp(oldFont->name,parent->fontSettingsOSD->name,sizeof(TfontSettingsOSD)-sizeof(Toptions))!=0)
  {
   memcpy(oldFont,parent->fontSettingsOSD,sizeof(*oldFont));
   for (TprovOSDs::iterator po=provOSDs.begin();po!=provOSDs.end();po++)
    (*po)->fontInit(oldFont);
   fontUser.init(oldFont);
  }

 for (TprovOSDs::iterator po=provOSDs.begin();po!=provOSDs.end();po++)
  (*po)->init(true,deci,deciD,deciV,parent->config,oldFont,cfg,framecnt);
 framecnt++;

 if (!shortOSDtemp.empty())
  {
   cs.Lock();
   for (std::vector<TshortOsdTemp>::const_iterator o=shortOSDtemp.begin();o!=shortOSDtemp.end();o++)
    shortOSD.push_back(new TosdLine(deci,deciD,deciV,parent->config,_l("shortosd")+o->first,oldFont,o->second,NULL));
   shortOSDtemp.clear();
   cs.Unlock();
  }

 if (!provOSDs.empty()/*!osds.empty()*/ || !shortOSD.empty() || cfg->user[0]!='\0')
  {
   unsigned char *dst[4];
   char_t outputfourcc[20];
   deciV->getOutputFourcc(outputfourcc,20);
   if ((strncmp(outputfourcc,_l("RGB"),3)==0 && !parent->isAnyActiveDownstreamFilter(it)) || pict.csp==FF_CSP_RGB32)
    {
     int outcsp = (pict.fieldtype & FIELD_TYPE::MASK_INT) ? FF_CSP_RGB32 | FF_CSP_FLAGS_INTERLACED : FF_CSP_RGB32;
     getCurNext(outcsp, pict, true, COPYMODE_FULL, dst);
    }
   else
    getCurNext(FF_CSP_420P,pict,true,COPYMODE_FULL,dst);

   unsigned int x=dx1[0]*cfg->posX/100,y=dy1[0]*cfg->posY/100;

   for (TprovOSDs::iterator po=provOSDs.begin();po!=provOSDs.end();po++)
    if ((*po)->is)
     (*po)->print(deci,pict,dst,stride2,dx1[0],dy1[0],x,y,cfg->linespace,!!cfg->saveOnly);
   shortOSD.print(deci,pict,dst,stride2,dx1[0],dy1[0],x,y,cfg->linespace,false);

   if (cfg->user[0]!='\0')
    {
     if (strcmp(oldLinesUser,cfg->user)!=0)
      {
       ff_strncpy(oldLinesUser, cfg->user, countof(oldLinesUser));
       strtok(cfg->user,_l("\n"),linesUser);
       subUser.set(linesUser);
	   TsubtitleFormat subtitleFormat = TsubtitleFormat(parent->config->getHtmlColors());
	   subUser.format(subtitleFormat);
      }
     TrenderedSubtitleLines::TprintPrefs printprefs(deci);
     printprefs.isOSD=true;
     printprefs.dst=dst;
     printprefs.stride=stride2;
     printprefs.shiftX=pict.cspInfo.shiftX;printprefs.shiftY=pict.cspInfo.shiftY;
     printprefs.dx=dx1[0];
     printprefs.dy=dy1[0];
     printprefs.xpos=deci->getParam2(IDFF_OSDuserPx);
     printprefs.ypos=deci->getParam2(IDFF_OSDuserPy);
     printprefs.align=ALIGN_LEFT;
     printprefs.linespacing=cfg->linespace;
     printprefs.deci=deci;
     printprefs.config=parent->config;
     printprefs.shadowMode=-1;
     printprefs.csp=pict.csp & FF_CSPS_MASK;
     printprefs.cspBpp=pict.cspInfo.Bpp;
     printprefs.sar=pict.rectFull.sar;

     fontUser.print(&subUser,true,printprefs);
    }
  }
 csProvider.Unlock();
 return parent->deliverSample(++it,pict);
}

STDMETHODIMP_(const char_t*) TimgFilterOSD::getInfoItemName(int type)
{
 if (!trans)
  deci->getTranslator(&trans);
 const char_t *name=deci->getInfoItemName(type);
 return trans?trans->translate(name):name;
}

HRESULT TimgFilterOSD::registerOSDprovider(IOSDprovider *provider,const char *name)
{
 if (!provider) return E_POINTER;
 CAutoLock l(&csProvider);
 for (TprovOSDs::iterator po=provOSDs.begin();po!=provOSDs.end();po++)
  if ((*po)->provider==provider)
   return E_UNEXPECTED;
 provOSDs.push_back(new Tosds(provider,text<char_t>(name)));
 return S_OK;
}
HRESULT TimgFilterOSD::unregisterOSDprovider(IOSDprovider *provider)
{
 if (!provider) return E_POINTER;
 CAutoLock l(&csProvider);
 for (TprovOSDs::iterator po=provOSDs.begin();po!=provOSDs.end();po++)
  if ((*po)->provider==provider)
   {
    delete *po;
    provOSDs.erase(po);
    return S_OK;
   }
 return VFW_E_NOT_FOUND;
}
