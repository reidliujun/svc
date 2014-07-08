/*
 * Copyright (c) 2002-2006 Milan Cutka
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "stdafx.h"
#include "TpresetEnc.h"
#include "TcodecSettings.h"
#include "ffdshow_constants.h"

const char_t* TpresetsEnc::preset_lavc_vcd_pal=_l("Libavcodec VCD PAL");
const char_t* TpresetsEnc::preset_lavc_vcd_ntsc=_l("Libavcodec VCD NTSC");
const char_t* TpresetsEnc::preset_lavc_svcd_pal=_l("Libavcodec SVCD PAL");
const char_t* TpresetsEnc::preset_lavc_svcd_ntsc=_l("Libavcodec SVCD NTSC");
const char_t* TpresetsEnc::preset_lavc_dvd_pal=_l("Libavcodec DVD PAL");
const char_t* TpresetsEnc::preset_lavc_dvd_ntsc=_l("Libavcodec DVD NTSC");

//================================== TpresetEnc ====================================
void TpresetEnc::initBuiltin(void)
{
 strcpy(name,_l("Builtin"));

 struct TregOpDefault :public TregOp
  {
  private:
   TpresetEnc *preset;
  public:
   TregOpDefault(TpresetEnc *Ipreset):preset(Ipreset) {}
   virtual bool _REG_OP_N(short int id,const char_t *X,int &Y,const int Z)
    {
     preset->vals.insert(std::make_pair(X,Tval(Z)));
     return true;
    }
   virtual void _REG_OP_S(short int id, const char_t *X, char_t *Y, size_t buflen, const char_t *Z, bool multipleLines = false)
    {
     preset->vals.insert(std::make_pair(X,Tval(Z)));
    }
  } t(this);
 TcoSettings co(new TintStrColl);
 co.reg_op(t);
}
void TpresetEnc::load(const char_t *Iname)
{
 char_t key[MAX_PATH];strcpy(key,FFDSHOW_REG_PARENT _l("\\") FFDSHOWENC);
 ff_strncpy(name, Iname ? Iname : _l("Default"), countof(name));
 if (Iname)
  {
   strncat_s(key, countof(key), _l("\\"), _TRUNCATE);
   strncat_s(key, countof(key), name, _TRUNCATE);
  };

 struct TregOpPresetRegRead :public TregOp
  {
  private:
   TpresetEnc *preset;
   HKEY hKey;
  public:
   TregOpPresetRegRead(TpresetEnc *Ipreset,HKEY hive,const char_t *key):preset(Ipreset)
    {
     hKey=NULL;
     RegOpenKeyEx(hive,key,0,KEY_READ,&hKey);
    }
   virtual ~TregOpPresetRegRead()
    {
     if (hKey) RegCloseKey(hKey);
    }
   virtual bool _REG_OP_N(short int id,const char_t *X,int &Y,const int Z)
    {
     DWORD size=sizeof(int);
     int val;
     if (hKey && RegQueryValueEx(hKey,X,0,0,(LPBYTE)&val,&size)==ERROR_SUCCESS)
      {
       preset->vals.insert(std::make_pair(X,Tval(val)));
       return true;
      }
     else
      return false;
    }
   virtual void _REG_OP_S(short int id, const char_t *X, char_t *Y, size_t buflen, const char_t *Z, bool multipleLines = false)
    {
     DWORD size=MAX_PATH*sizeof(char_t);
     char_t val[MAX_PATH];
     if (hKey && RegQueryValueEx(hKey,X,0,0,(LPBYTE)val,&size)==ERROR_SUCCESS)
      preset->vals.insert(std::make_pair(X,Tval(val)));
    }
  } t(this,HKEY_CURRENT_USER,key);
 TcoSettings co(new TintStrColl);
 co.reg_op(t);
}
void TpresetEnc::save(void)
{
 char_t key[MAX_PATH];strcpy(key,FFDSHOW_REG_PARENT _l("\\") FFDSHOWENC);
 strncat_s(key, countof(key), _l("\\"), _TRUNCATE);
 strncat_s(key, countof(key), name, _TRUNCATE);
 DWORD dispo;
 HKEY hKey=NULL;
 if (RegCreateKeyEx(HKEY_CURRENT_USER,key,0,FFDSHOW_REG_CLASS,REG_OPTION_NON_VOLATILE,KEY_WRITE,0,&hKey,&dispo)!=ERROR_SUCCESS) return;
 for (Tvals::const_iterator v=vals.begin();v!=vals.end();v++)
  if (v->second.s.empty())
   RegSetValueEx(hKey,v->first,0,REG_DWORD,(LPBYTE)&v->second.i,sizeof(int));
  else
   RegSetValueEx(hKey,v->first,0,REG_SZ,(LPBYTE)v->second.s.c_str(),DWORD((v->second.s.size()+1)*sizeof(char_t)));
 RegCloseKey(hKey);
}

bool TpresetEnc::TregOpCategory::isIn(int id)
{
 if (id==0 || !propsIDs) return false;
 const int *p=propsIDs;
 while (*p)
  {
   if (*p==id)
    return true;
   p++;
  }
 return false;
}

void TpresetEnc::apply(TcoSettings *co,const int *propsIDs)
{
 struct TregOpApply :public TregOpCategory
  {
  public:
   TregOpApply(TpresetEnc *Ipreset,TcoSettings *Ico,const int *IpropsIDs):TregOpCategory(Ipreset,Ico,IpropsIDs) {}
   virtual bool _REG_OP_N(short int id,const char_t *X,int &Y,const int Z)
    {
     if (isIn(id))
      {
       TpresetEnc::Tvals::iterator val=preset->vals.find(X);
       if (val!=preset->vals.end())
        {
         Y=val->second.i;
         return false;
        }
      }
     return true;
    }
   virtual void _REG_OP_S(short int id, const char_t *X, char_t *Y, size_t buflen, const char_t *Z, bool multipleLines = false)
    {
     if (isIn(id))
      {
       TpresetEnc::Tvals::iterator val=preset->vals.find(X);
       if (val!=preset->vals.end())
        {
         ff_strncpy(Y,val->second.s.c_str(),buflen);
         Y[buflen-1]='\0';
        }
      }
    }
  } t(this,co,propsIDs);
 co->reg_op(t);
}
void TpresetEnc::store(TcoSettings *co,const int *propsIDs)
{
 struct TregOpStore :public TregOpCategory
  {
  public:
   TregOpStore(TpresetEnc *Ipreset,TcoSettings *Ico,const int *IpropsIDs):TregOpCategory(Ipreset,Ico,IpropsIDs) {}
   virtual bool _REG_OP_N(short int id,const char_t *X,int &Y,const int Z)
    {
     if (isIn(id))
      preset->vals[X]=Tval(Y);
     return true;
    }
   virtual void _REG_OP_S(short int id, const char_t *X, char_t *Y, size_t buflen, const char_t *Z, bool multipleLines = false)
    {
     if (isIn(id))
      preset->vals[X]=Tval(Y);
    }
  } t(this,co,propsIDs);
 co->reg_op(t);
}

void TpresetEnc::rename(const char_t *newname)
{
 remove();
 ff_strncpy(name, newname, countof(name));
 save();
}
void TpresetEnc::remove(void)
{
 char_t key[MAX_PATH];
 strcpy(key,FFDSHOW_REG_PARENT _l("\\") FFDSHOWENC);
 strncat_s(key, countof(key), _l("\\"), _TRUNCATE);
 strncat_s(key, countof(key), name, _TRUNCATE);
 RegDeleteKey(HKEY_CURRENT_USER,key);
}

//================================= TpresetsEnc ====================================
void TpresetsEnc::init(void)
{
 if (!empty()) return;
 TpresetEnc preset;
 preset.initBuiltin();
 push_back(preset);
 preset.vals.clear();
 preset.load(NULL);
 push_back(preset);

 HKEY hKey;
 RegOpenKeyEx(HKEY_CURRENT_USER,FFDSHOW_REG_PARENT _l("\\") FFDSHOWENC,0,KEY_READ,&hKey);
 for (int i=0,retCode=ERROR_SUCCESS;retCode==ERROR_SUCCESS;i++)
  {
   char_t keyName[MAX_PATH];DWORD keyNameSize=MAX_PATH;
   FILETIME ftLastWriteTime;
   retCode = RegEnumKeyEx(hKey,
                          i,
                          keyName,
                          &keyNameSize,
                          NULL,
                          NULL,
                          NULL,
                          &ftLastWriteTime
                         );
   if (retCode==ERROR_SUCCESS)
    {
     if (strcmp(keyName,_l("makeAVIS"))!=0)
      {
       TpresetEnc presetAVIS;
       presetAVIS.load(keyName);
       push_back(presetAVIS);
      }
    }
   else
    break;
  }
 RegCloseKey(hKey);
 if (!getPreset(preset_lavc_vcd_pal))
  {
   TpresetEnc pe;
   ff_strncpy(pe.name, preset_lavc_vcd_pal, countof(pe.name));
   TcoSettings co(new TintStrColl);
   co.mode=ENC_MODE::CBR;
   co.codecId=CODEC_ID_MPEG1VIDEO;
   co.min_key_interval=1;
   co.max_key_interval=15;
   co.bitrate1000=1150;
   co.ff1_rc_max_rate1000=1150;
   co.ff1_rc_min_rate1000=1150;
   co.ff1_rc_buffer_size=40*1024*8;
   static const int idffs[]={IDFF_enc_min_key_interval,IDFF_enc_max_key_interval,IDFF_enc_mode,IDFF_enc_codecId,IDFF_enc_bitrate1000,IDFF_enc_ff1_rc_max_rate1000,IDFF_enc_ff1_rc_min_rate1000,IDFF_enc_ff1_rc_buffer_size,0};
   pe.store(&co,idffs);
   push_back(pe);
  }
 if (!getPreset(preset_lavc_vcd_ntsc))
  {
   TpresetEnc pe;
   ff_strncpy(pe.name, preset_lavc_vcd_ntsc, countof(pe.name));
   TcoSettings co(new TintStrColl);
   co.mode=ENC_MODE::CBR;
   co.codecId=CODEC_ID_MPEG1VIDEO;
   co.min_key_interval=1;
   co.max_key_interval=15;
   co.bitrate1000=1150;
   co.ff1_rc_max_rate1000=1150;
   co.ff1_rc_min_rate1000=1150;
   co.ff1_rc_buffer_size=40*1024*8;
   static const int idffs[]={IDFF_enc_min_key_interval,IDFF_enc_max_key_interval,IDFF_enc_mode,IDFF_enc_codecId,IDFF_enc_bitrate1000,IDFF_enc_ff1_rc_max_rate1000,IDFF_enc_ff1_rc_min_rate1000,IDFF_enc_ff1_rc_buffer_size,0};
   pe.store(&co,idffs);
   push_back(pe);
  }
 if (!getPreset(preset_lavc_svcd_pal))
  {
   TpresetEnc pe;
   ff_strncpy(pe.name, preset_lavc_svcd_pal, countof(pe.name));
   TcoSettings co(new TintStrColl);
   co.mode=ENC_MODE::CBR;
   co.codecId=CODEC_ID_MPEG2VIDEO;
   co.min_key_interval=1;
   co.max_key_interval=15;
   co.bitrate1000=2040;
   co.ff1_rc_max_rate1000=2516;
   co.ff1_rc_min_rate1000=0;
   co.ff1_rc_buffer_size=224*1024*8;
   co.svcd_scan_offset=1;
   static const int idffs[]={IDFF_enc_min_key_interval,IDFF_enc_max_key_interval,IDFF_enc_mode,IDFF_enc_codecId,IDFF_enc_bitrate1000,IDFF_enc_ff1_rc_max_rate1000,IDFF_enc_ff1_rc_min_rate1000,IDFF_enc_ff1_rc_buffer_size,IDFF_enc_svcd_scan_offset,0};
   pe.store(&co,idffs);
   push_back(pe);
  }
 if (!getPreset(preset_lavc_svcd_ntsc))
  {
   TpresetEnc pe;
   ff_strncpy(pe.name, preset_lavc_svcd_ntsc, countof(pe.name));
   TcoSettings co(new TintStrColl);
   co.mode=ENC_MODE::CBR;
   co.codecId=CODEC_ID_MPEG2VIDEO;
   co.min_key_interval=1;
   co.max_key_interval=18;
   co.bitrate1000=2040;
   co.ff1_rc_max_rate1000=2516;
   co.ff1_rc_min_rate1000=0;
   co.ff1_rc_buffer_size=224*1024*8;
   co.svcd_scan_offset=1;
   static const int idffs[]={IDFF_enc_min_key_interval,IDFF_enc_max_key_interval,IDFF_enc_mode,IDFF_enc_codecId,IDFF_enc_bitrate1000,IDFF_enc_ff1_rc_max_rate1000,IDFF_enc_ff1_rc_min_rate1000,IDFF_enc_ff1_rc_buffer_size,IDFF_enc_svcd_scan_offset,0};
   pe.store(&co,idffs);
   push_back(pe);
  }
 if (!getPreset(preset_lavc_dvd_pal))
  {
   TpresetEnc pe;
   ff_strncpy(pe.name, preset_lavc_dvd_pal, countof(pe.name));
   TcoSettings co(new TintStrColl);
   co.mode=ENC_MODE::CBR;
   co.codecId=CODEC_ID_MPEG2VIDEO;
   co.min_key_interval=1;
   co.max_key_interval=15;
   co.bitrate1000=6000;
   co.ff1_rc_max_rate1000=9000;
   co.ff1_rc_min_rate1000=0;
   co.ff1_rc_buffer_size=224*1024*8;
   co.svcd_scan_offset=0;
   static const int idffs[]={IDFF_enc_min_key_interval,IDFF_enc_max_key_interval,IDFF_enc_mode,IDFF_enc_codecId,IDFF_enc_bitrate1000,IDFF_enc_ff1_rc_max_rate1000,IDFF_enc_ff1_rc_min_rate1000,IDFF_enc_ff1_rc_buffer_size,IDFF_enc_svcd_scan_offset,0};
   pe.store(&co,idffs);
   push_back(pe);
  }
 if (!getPreset(preset_lavc_dvd_ntsc))
  {
   TpresetEnc pe;
   ff_strncpy(pe.name, preset_lavc_dvd_ntsc, countof(pe.name));
   TcoSettings co(new TintStrColl);
   co.mode=ENC_MODE::CBR;
   co.codecId=CODEC_ID_MPEG2VIDEO;
   co.min_key_interval=1;
   co.max_key_interval=18;
   co.bitrate1000=6000;
   co.ff1_rc_max_rate1000=9000;
   co.ff1_rc_min_rate1000=0;
   co.ff1_rc_buffer_size=224*1024*8;
   co.svcd_scan_offset=0;
   static const int idffs[]={IDFF_enc_min_key_interval,IDFF_enc_max_key_interval,IDFF_enc_mode,IDFF_enc_codecId,IDFF_enc_bitrate1000,IDFF_enc_ff1_rc_max_rate1000,IDFF_enc_ff1_rc_min_rate1000,IDFF_enc_ff1_rc_buffer_size,IDFF_enc_svcd_scan_offset,0};
   pe.store(&co,idffs);
   push_back(pe);
  }
}
void TpresetsEnc::save(void)
{
 for (size_t i=2;i<size();i++)
  at(i).save();
}
TpresetEnc* TpresetsEnc::getPreset(const char_t *name)
{
 for (iterator i=begin();i!=end();i++)
  if (stricmp(name,i->name)==0)
   return &*i;
 return NULL;
}
TpresetEnc* TpresetsEnc::createPreset(const char_t *name)
{
 TpresetEnc preset;
 ff_strncpy(preset.name, name, countof(preset.name));
 push_back(preset);
 return &*rbegin();
}
void TpresetsEnc::remove(TpresetEnc *preset)
{
 for (iterator p=begin();p!=end();p++)
  if (stricmp(p->name,preset->name)==0)
   {
    p->remove();
    erase(p);
    return;
   }
}
