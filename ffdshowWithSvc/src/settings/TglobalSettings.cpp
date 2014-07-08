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
#include "Tconfig.h"
#include "TglobalSettings.h"
#include "reg.h"
#include "ffdshow_mediaguids.h"
#include "ffcodecs.h"
#include "ffdshow_constants.h"
#include "TsampleFormat.h"
#include "ffImgfmt.h"
#include "TpresetSettings.h"
#include "ffdebug.h"
#include "Tstream.h"
#include "IffdshowDec.h"
#include "TcompatibilityManager.h"

//===================================== TglobalSettingsBase ======================================
TglobalSettingsBase::TglobalSettingsBase(const Tconfig *Iconfig,int Imode,const char_t *Ireg_child,TintStrColl *Icoll):filtermode(Imode),config(Iconfig),reg_child(Ireg_child),Toptions(Icoll)
{
 static const TintOptionT<TglobalSettingsBase> iopts[]=
  {
   IDFF_filterMode       ,&TglobalSettingsBase::filtermode       ,1,1,_l(""),0,
     NULL,0,
   IDFF_trayIcon         ,&TglobalSettingsBase::trayIcon         ,0,0,_l(""),0,
     _l("trayIcon"),TintOption::DEF_DYN,
   IDFF_trayIconType     ,&TglobalSettingsBase::trayIconType     ,0,2,_l(""),0,
     NULL,1,
   IDFF_trayIconChanged  ,&TglobalSettingsBase::trayIconChanged  ,0,0,_l(""),0,
     NULL,0,
   IDFF_trayIconExt      ,&TglobalSettingsBase::trayIconExt      ,0,0,_l(""),0,
     _l("trayIconExt"),0,
   IDFF_outputdebug      ,&TglobalSettingsBase::outputdebug      ,0,0,_l(""),0,
     _l("outputdebug"),1,
   IDFF_outputdebugfile  ,&TglobalSettingsBase::outputdebugfile  ,0,0,_l(""),0,
     _l("outputdebugfile"),0,
   IDFF_errorbox         ,&TglobalSettingsBase::errorbox         ,0,0,_l(""),0,
     _l("errorbox1"),1,
   IDFF_multipleInstances,&TglobalSettingsBase::multipleInstances,0,4,_l(""),0,
     _l("multipleInstances"),TintOption::DEF_DYN,
   IDFF_isBlacklist      ,&TglobalSettingsBase::isBlacklist      ,0,0,_l(""),0,
     _l("isBlacklist"),1,
   IDFF_isWhitelist      ,&TglobalSettingsBase::isWhitelist      ,0,0,_l(""),0,
     _l("isWhitelist"),1,   
   IDFF_compManagerMode  ,&TglobalSettingsBase::compOnLoadMode   ,1,4,_l(""),0,
     NULL,1,
   IDFF_isCompMgr        ,&TglobalSettingsBase::isCompMgr        ,0,0,_l(""),0,
     NULL,1,
   IDFF_isCompMgrChanged ,&TglobalSettingsBase::isCompMgrChanged ,0,0,_l(""),0,
     NULL,0,
   IDFF_addToROT         ,&TglobalSettingsBase::addToROT         ,0,0,_l(""),0,
     _l("addToROT"),0,
   IDFF_allowedCpuFlags  ,&TglobalSettingsBase::allowedCPUflags  ,1,1,_l(""),0,
     NULL,0,
   IDFF_allowDPRINTF     ,&TglobalSettingsBase::allowDPRINTF     ,0,0,_l(""),0,
     NULL,0,
   IDFF_allowDPRINTFchanged,&TglobalSettingsBase::allowDPRINTFchanged,0,0,_l(""),0,
     NULL,0,
   0
  };
 addOptions(iopts);
 static const TstrOption sopts[]=
  {
   IDFF_debugfile  ,(TstrVal)&TglobalSettingsBase::debugfile ,MAX_PATH,0,_l(""),0,
     _l("debugfile"),_l("\\ffdebug.log"),
   IDFF_dscalerPath,(TstrVal)&TglobalSettingsBase::dscalerPth,MAX_PATH,0,_l(""),0,
     NULL,NULL,
   IDFF_blacklist  ,(TstrVal)&TglobalSettingsBase::blacklist ,MAX_COMPATIBILITYLIST_LENGTH,0,_l(""),0,
     _l("blacklist"),BLACKLIST_EXE_FILENAME,
   IDFF_whitelist  ,(TstrVal)&TglobalSettingsBase::whitelist ,MAX_COMPATIBILITYLIST_LENGTH,0,_l(""),0,
     _l("whitelist"),WHITELIST_EXE_FILENAME,
   0
  };
 addOptions(sopts);
 static const TcreateParamList3<TmultipleInstances> listMultipleInstances(::multipleInstances,&TmultipleInstances::id,&TmultipleInstances::name);setParamList(IDFF_multipleInstances,&listMultipleInstances);

}
void TglobalSettingsBase::load(void)
{
 char_t rkey[MAX_PATH];
 tsnprintf_s(rkey, countof(rkey), _TRUNCATE, FFDSHOW_REG_PARENT _l("\\%s"), reg_child);
 TregOpRegRead tHKCU(HKEY_CURRENT_USER,rkey);
 reg_op(tHKCU);
 TregOpRegRead tHKLM(HKEY_LOCAL_MACHINE,rkey);
 reg_op_codec(tHKCU,&tHKLM);
 TregOpRegRead tDScaler(HKEY_LOCAL_MACHINE,FFDSHOW_REG_PARENT _l("\\") FFDSHOW);
 tDScaler._REG_OP_S(IDFF_dscalerPath,_l("dscalerPth"),dscalerPth,MAX_PATH,_l(""));
 TregOpRegRead tCPU(HKEY_CURRENT_USER,FFDSHOW_REG_PARENT _l("\\") FFDSHOW);
 tCPU._REG_OP_N(IDFF_allowedCpuFlags,_l("allowedCPUflags"),allowedCPUflags,255);
 firstBlacklist=firstWhitelist=true;

 // Load Icon type : shared by video, audio and vfw.
 TregOpRegRead tHKCU_global(HKEY_CURRENT_USER,FFDSHOW_REG_PARENT _l("\\") FFDSHOW);
 tHKCU_global._REG_OP_N(IDFF_trayIconType,_l("trayIconType"),trayIconType,1);

 // Load compatibility manager::dontask : shared by video and audio.
 tHKCU_global._REG_OP_N(IDFF_isCompMgr,_l("isCompMgr"),isCompMgr,1);

 // Load allowDPRINTF : shared by all.
 tHKCU_global._REG_OP_N(IDFF_allowDPRINTF,_l("allowDPRINTF"),allowDPRINTF,0);
}
void TglobalSettingsBase::save(void)
{
 char_t rkey[MAX_PATH];
 tsnprintf_s(rkey, countof(rkey), _TRUNCATE, FFDSHOW_REG_PARENT _l("\\%s"), reg_child);
 TregOpRegWrite tHKCU(HKEY_CURRENT_USER,rkey);
 reg_op(tHKCU);
 reg_op_codec(tHKCU,NULL);
 TregOpRegWrite tDScaler(HKEY_LOCAL_MACHINE,FFDSHOW_REG_PARENT _l("\\") FFDSHOW);
 tDScaler._REG_OP_S(IDFF_dscalerPath,_l("dscalerPth"),dscalerPth,MAX_PATH,_l(""));
 TregOpRegWrite tCPU(HKEY_CURRENT_USER,FFDSHOW_REG_PARENT _l("\\") FFDSHOW);
 tCPU._REG_OP_N(IDFF_allowedCpuFlags,_l("allowedCPUflags"),allowedCPUflags,255);

 // Save Icon type : shared by video, audio and vfw.
 if (trayIconChanged)
  {
   TregOpRegWrite tHKCU_global(HKEY_CURRENT_USER,FFDSHOW_REG_PARENT _l("\\") FFDSHOW);
   tHKCU_global._REG_OP_N(IDFF_trayIconType,_l("trayIconType"),trayIconType,0);
   trayIconChanged=0;
  }
 // Save compatibility manager::dontask : shared by video and audio.
 if (isCompMgrChanged)
  {
   TregOpRegWrite tHKCU_global(HKEY_CURRENT_USER,FFDSHOW_REG_PARENT _l("\\") FFDSHOW);
   tHKCU_global._REG_OP_N(IDFF_isCompMgr,_l("isCompMgr"),isCompMgr,0);
   isCompMgrChanged=0;
  }
 // Save allowDPRINTF : shared by all.
 if (allowDPRINTFchanged)
  {
   TregOpRegWrite tHKCU_global(HKEY_CURRENT_USER,FFDSHOW_REG_PARENT _l("\\") FFDSHOW);
   tHKCU_global._REG_OP_N(IDFF_allowDPRINTF,_l("allowDPRINTF"),allowDPRINTF,0);
   allowDPRINTFchanged=0;
  }
}
bool TglobalSettingsBase::exportReg(bool all,const char_t *regflnm,bool unicode)
{
 if (fileexists(regflnm) && DeleteFile(regflnm)==0)
  return false;
 TstreamFile f(regflnm,false,true,unicode?Tstream::ENC_LE16:Tstream::ENC_ASCII);if (!f) return false;
 if (unicode)
  f.printf(L"Windows Registry Editor Version 5.00\n\n");
 else
  f.printf("REGEDIT4\n\n");
 regExport(f,HKEY_LOCAL_MACHINE,FFDSHOW_REG_PARENT _l("\\") FFDSHOW,unicode);
 if (all || strcmp(reg_child,FFDSHOWDECVIDEO)==0)
  regExport(f,HKEY_CURRENT_USER ,FFDSHOW_REG_PARENT _l("\\") FFDSHOWDECVIDEO,unicode);
 if (all || strcmp(reg_child,FFDSHOWDECAUDIO)==0)
  {
   regExport(f,HKEY_CURRENT_USER ,FFDSHOW_REG_PARENT _l("\\") FFDSHOWDECAUDIO,unicode);
   regExport(f,HKEY_LOCAL_MACHINE,FFDSHOW_REG_PARENT _l("\\") FFDSHOWDECAUDIO,unicode);
  }
 if (all || strcmp(reg_child,FFDSHOWENC)==0)
  {
   regExport(f,HKEY_CURRENT_USER ,FFDSHOW_REG_PARENT _l("\\") FFDSHOWENC,unicode);
   regExport(f,HKEY_LOCAL_MACHINE,FFDSHOW_REG_PARENT _l("\\") FFDSHOWENC,unicode);
  }
 if (all || strcmp(reg_child,FFDSHOWDECVIDEOVFW)==0)
  {
   regExport(f,HKEY_CURRENT_USER ,FFDSHOW_REG_PARENT _l("\\") FFDSHOWDECVIDEOVFW,unicode);
   regExport(f,HKEY_LOCAL_MACHINE,FFDSHOW_REG_PARENT _l("\\") FFDSHOWDECVIDEOVFW,unicode);
  }
 return true;
}
void TglobalSettingsBase::_reg_op_codec(short id,TregOp &tHKCU,TregOp *tHKLM,const char_t *name,int &val,int def)
{
 if (name == NULL)
  {
   val=def;
   return;
  }
 if (!tHKCU._REG_OP_N(id,name,val,def) && tHKLM)
  tHKLM->_REG_OP_N(id,name,val,def);
}

bool TglobalSettingsBase::inBlacklist(const char_t *exe)
{
 if (firstBlacklist)
  {
   firstBlacklist=false;
   strtok(blacklist,_l(";"),blacklistList);
  }
 for (strings::const_iterator b=blacklistList.begin();b!=blacklistList.end();b++)
  if (DwStrcasecmp(*b,exe)==0)
   return true;
 return false;
}

bool TglobalSettingsBase::inWhitelist(const char_t *exe,IffdshowBase *Ideci)
{
 // MessageBox(NULL,exe,_l("ffdshow inWhitelist"),MB_OK);
 bool old_firstWhitelist=firstWhitelist;
 if (firstWhitelist)
  {
   firstWhitelist=false;
   strtok(whitelist,_l(";"),whitelistList);
  }
 for (strings::const_iterator b=whitelistList.begin();b!=whitelistList.end();b++)
  if (DwStrcasecmp(*b,exe)==0)
   return true;
 if (!isCompMgr)
  return false;
 if (TcompatibilityManager::s_mode==0)
  {
   TcompatibilityManager::s_mode=-1;
   TcompatibilityManager *dlg=new TcompatibilityManager(Ideci,NULL,exe);
   dlg->show();
   delete dlg;
  }
 bool result=false;
 switch (TcompatibilityManager::s_mode)
  {
   case -1:
   case 0:
   case 1:
    result=false;
    break;
   case 2:
    if (old_firstWhitelist) addToCompatiblityList(blacklist,exe,_l(";"));
    result=false;
    break;
   case 3:
    result=true;
    break;
   case 4:
    if (old_firstWhitelist) addToCompatiblityList(whitelist,exe,_l(";"));
    result=true;
    break;
  }
 if (isCompMgrChanged)
  save();
 return result;
} 

void TglobalSettingsBase::addToCompatiblityList(char_t *list, const char_t *exe, const char_t *delimit)
{
 strings listList;
 strtok(list,delimit,listList);
 listList.push_back(exe);
 ffstring result;
 for (strings::const_iterator b=listList.begin();b!=listList.end();b++)
  result+=*b+delimit;
 ff_strncpy(list, result.c_str(), MAX_COMPATIBILITYLIST_LENGTH);
 save();
}

//===================================== TglobalSettingsDec ======================================
TglobalSettingsDec::TglobalSettingsDec(const Tconfig *Iconfig,int Imode,const char_t *Ireg_child,TintStrColl *Icoll,TOSDsettings *Iosd):TglobalSettingsBase(Iconfig,Imode,Ireg_child,Icoll),osd(Iosd)
{
 static const TintOptionT<TglobalSettingsDec> iopts[]=
  {
   IDFF_autoPreset         ,&TglobalSettingsDec::autoPreset         ,0,0,_l(""),0,
     _l("autoPresets"),0,
   IDFF_autoPresetFileFirst,&TglobalSettingsDec::autoPresetFileFirst,0,0,_l(""),0,
     _l("autoPresetFileFirst"),0,
   IDFF_streamsOptionsMenu ,&TglobalSettingsDec::streamsMenu        ,0,0,_l(""),0,
     _l("streamsOptionsMenu"),0,
   0
  };
 addOptions(iopts);
 static const TstrOption sopts[]=
  {
   IDFF_defaultPreset,(TstrVal)&TglobalSettingsDec::defaultPreset,MAX_PATH,0,_l(""),0,
     _l("activePreset"),FFPRESET_DEFAULT,
   0
  };
 addOptions(sopts);
}

void TglobalSettingsDec::reg_op(TregOp &t)
{
 TglobalSettingsBase::reg_op(t);
 osd->reg_op(t);
}

void TglobalSettingsDec::load(void)
{
 TglobalSettingsBase::load();
 osd->loadPresets(reg_child);
}
void TglobalSettingsDec::save(void)
{
 TglobalSettingsBase::save();
 osd->savePresets(reg_child);
}

void TglobalSettingsDec::fixMissing(int &codecId,int movie1,int movie2,int movie3)
{
 fixMissing(codecId,movie1,movie2);
 fixMissing(codecId,movie2,movie3);
 fixMissing(codecId,movie1,movie3);
}
void TglobalSettingsDec::fixMissing(int &codecId,int movie1,int movie2)
{
 if (!config->isDecoder[movie1] && !config->isDecoder[movie2]) codecId=IDFF_MOVIE_NONE;
 else if (codecId==movie1 && !config->isDecoder[movie1]) codecId=movie2;
 else if (codecId==movie2 && !config->isDecoder[movie2]) codecId=movie1;
}
void TglobalSettingsDec::fixMissing(int &codecId,int movie)
{
 if (!config->isDecoder[movie]) codecId=0;
}
int TglobalSettingsDec::getDefault(int id)
{
 switch (id)
  {
   case IDFF_trayIcon:return 1;
   default:return TglobalSettingsBase::getDefault(id);
  }
}

static bool compStrPtrs(const char_t *s1,const char_t *s2)
{
 return stricmp(s1,s2)<0;
}

void TglobalSettingsDec::cleanupCodecsList(std::vector<CodecID> &ids,Tstrptrs &codecs)
{
 std::sort(ids.begin(),ids.end());
 std::vector<CodecID>::iterator idsend=std::unique(ids.begin(),ids.end());
 codecs.clear();
 for (std::vector<CodecID>::const_iterator id=ids.begin();id!=idsend;id++)
  codecs.push_back(getCodecName(*id));
 std::sort(codecs.begin(),codecs.end());
 Tstrptrs::iterator codecsend=std::unique(codecs.begin(),codecs.end());
 codecs.erase(codecsend,codecs.end());
 std::sort(codecs.begin(),codecs.end(),compStrPtrs);
}

//=================================== TglobalSettingsDecVideo ===================================
TglobalSettingsDecVideo::TglobalSettingsDecVideo(const Tconfig *Iconfig,int Imode,TintStrColl *Icoll):TglobalSettingsDec(Iconfig,Imode,Imode&IDFF_FILTERMODE_VFW?FFDSHOWDECVIDEOVFW:FFDSHOWDECVIDEO,Icoll,&osd),sub(Icoll),osd(Icoll)
{
 static const TintOptionT<TglobalSettingsDecVideo> iopts[]=
  {
   IDFF_buildHistogram ,&TglobalSettingsDecVideo::buildHistogram ,0,0,_l(""),0,
     _l("buildHistogram"),0,
   IDFF_xvid           ,&TglobalSettingsDecVideo::xvid           ,0,IDFF_MOVIE_MAX,_l(""),0,NULL,0,
   IDFF_div3           ,&TglobalSettingsDecVideo::div3           ,0,IDFF_MOVIE_MAX,_l(""),0,NULL,0,
   IDFF_dx50           ,&TglobalSettingsDecVideo::dx50           ,0,IDFF_MOVIE_MAX,_l(""),0,NULL,0,
   IDFF_mp4v           ,&TglobalSettingsDecVideo::mp4v           ,0,IDFF_MOVIE_MAX,_l(""),0,NULL,0,
   IDFF_fvfw           ,&TglobalSettingsDecVideo::fvfw           ,0,IDFF_MOVIE_MAX,_l(""),0,NULL,0,
   IDFF_mp43           ,&TglobalSettingsDecVideo::mp43           ,0,IDFF_MOVIE_MAX,_l(""),0,NULL,0,
   IDFF_mp42           ,&TglobalSettingsDecVideo::mp42           ,0,IDFF_MOVIE_MAX,_l(""),0,NULL,0,
   IDFF_mp41           ,&TglobalSettingsDecVideo::mp41           ,0,IDFF_MOVIE_MAX,_l(""),0,NULL,0,
   IDFF_h263           ,&TglobalSettingsDecVideo::h263           ,0,IDFF_MOVIE_MAX,_l(""),0,NULL,0,
   IDFF_h264           ,&TglobalSettingsDecVideo::h264           ,0,IDFF_MOVIE_MAX,_l(""),0,NULL,0,
   IDFF_h261           ,&TglobalSettingsDecVideo::h261           ,0,IDFF_MOVIE_MAX,_l(""),0,NULL,0,
   IDFF_wmv1           ,&TglobalSettingsDecVideo::wmv1           ,0,IDFF_MOVIE_MAX,_l(""),0,NULL,0,
   IDFF_wmv2           ,&TglobalSettingsDecVideo::wmv2           ,0,IDFF_MOVIE_MAX,_l(""),0,NULL,0,
   IDFF_wmv3           ,&TglobalSettingsDecVideo::wmv3           ,0,IDFF_MOVIE_MAX,_l(""),0,NULL,0,
   IDFF_wvc1           ,&TglobalSettingsDecVideo::wvc1           ,0,IDFF_MOVIE_MAX,_l(""),0,NULL,0,
   IDFF_mss2           ,&TglobalSettingsDecVideo::mss2           ,0,IDFF_MOVIE_MAX,_l(""),0,NULL,0,
   IDFF_wvp2           ,&TglobalSettingsDecVideo::wvp2           ,0,IDFF_MOVIE_MAX,_l(""),0,NULL,0,
   IDFF_vp5            ,&TglobalSettingsDecVideo::vp5            ,0,IDFF_MOVIE_MAX,_l(""),0,NULL,0,
   IDFF_vp6            ,&TglobalSettingsDecVideo::vp6            ,0,IDFF_MOVIE_MAX,_l(""),0,NULL,0,
   IDFF_vp6f           ,&TglobalSettingsDecVideo::vp6f           ,0,IDFF_MOVIE_MAX,_l(""),0,NULL,0,
   IDFF_cavs           ,&TglobalSettingsDecVideo::cavs           ,0,IDFF_MOVIE_MAX,_l(""),0,NULL,0,
   IDFF_mjpg           ,&TglobalSettingsDecVideo::mjpg           ,0,IDFF_MOVIE_MAX,_l(""),0,NULL,0,
   IDFF_avrn           ,&TglobalSettingsDecVideo::avrn           ,0,IDFF_MOVIE_MAX,_l(""),0,NULL,0,
   IDFF_dvsd           ,&TglobalSettingsDecVideo::dvsd           ,0,IDFF_MOVIE_MAX,_l(""),0,NULL,0,
   IDFF_cdvc           ,&TglobalSettingsDecVideo::cdvc           ,0,IDFF_MOVIE_MAX,_l(""),0,NULL,0,
   IDFF_hfyu           ,&TglobalSettingsDecVideo::hfyu           ,0,IDFF_MOVIE_MAX,_l(""),0,NULL,0,
   IDFF_cyuv           ,&TglobalSettingsDecVideo::cyuv           ,0,IDFF_MOVIE_MAX,_l(""),0,NULL,0,
   IDFF_asv1           ,&TglobalSettingsDecVideo::asv1           ,0,IDFF_MOVIE_MAX,_l(""),0,NULL,0,
   IDFF_vcr1           ,&TglobalSettingsDecVideo::vcr1           ,0,IDFF_MOVIE_MAX,_l(""),0,NULL,0,
   IDFF_mpg1           ,&TglobalSettingsDecVideo::mpg1           ,0,IDFF_MOVIE_MAX,_l(""),0,NULL,0,
   IDFF_mpg2           ,&TglobalSettingsDecVideo::mpg2           ,0,IDFF_MOVIE_MAX,_l(""),0,NULL,0,
   IDFF_mpegAVI        ,&TglobalSettingsDecVideo::mpegAVI        ,0,IDFF_MOVIE_MAX,_l(""),0,NULL,0,
   IDFF_em2v           ,&TglobalSettingsDecVideo::em2v           ,0,IDFF_MOVIE_MAX,_l(""),0,NULL,0,
   IDFF_theo           ,&TglobalSettingsDecVideo::theo           ,0,IDFF_MOVIE_MAX,_l(""),0,NULL,0,
   IDFF_svq1           ,&TglobalSettingsDecVideo::svq1           ,0,IDFF_MOVIE_MAX,_l(""),0,NULL,0,
   IDFF_svq3           ,&TglobalSettingsDecVideo::svq3           ,0,IDFF_MOVIE_MAX,_l(""),0,NULL,0,
   IDFF_qtrpza         ,&TglobalSettingsDecVideo::qtrpza         ,0,IDFF_MOVIE_MAX,_l(""),0,NULL,0,
   IDFF_cram           ,&TglobalSettingsDecVideo::cram           ,0,IDFF_MOVIE_MAX,_l(""),0,NULL,0,
   IDFF_rt21           ,&TglobalSettingsDecVideo::rt21           ,0,IDFF_MOVIE_MAX,_l(""),0,NULL,0,
   IDFF_iv32           ,&TglobalSettingsDecVideo::iv32           ,0,IDFF_MOVIE_MAX,_l(""),0,NULL,0,
   IDFF_cvid           ,&TglobalSettingsDecVideo::cvid           ,0,IDFF_MOVIE_MAX,_l(""),0,NULL,0,
   IDFF_rv10           ,&TglobalSettingsDecVideo::rv10           ,0,IDFF_MOVIE_MAX,_l(""),0,NULL,0,
   IDFF_rv40           ,&TglobalSettingsDecVideo::rv40           ,0,IDFF_MOVIE_MAX,_l(""),0,NULL,0,
   IDFF_ffv1           ,&TglobalSettingsDecVideo::ffv1           ,0,IDFF_MOVIE_MAX,_l(""),0,NULL,0,
   IDFF_vp3            ,&TglobalSettingsDecVideo::vp3            ,0,IDFF_MOVIE_MAX,_l(""),0,NULL,0,
   IDFF_rle            ,&TglobalSettingsDecVideo::rle            ,0,IDFF_MOVIE_MAX,_l(""),0,NULL,0,
   IDFF_mszh           ,&TglobalSettingsDecVideo::mszh           ,0,IDFF_MOVIE_MAX,_l(""),0,NULL,0,
   IDFF_zlib           ,&TglobalSettingsDecVideo::zlib           ,0,IDFF_MOVIE_MAX,_l(""),0,NULL,0,
   IDFF_flv1           ,&TglobalSettingsDecVideo::flv1           ,0,IDFF_MOVIE_MAX,_l(""),0,NULL,0,
   IDFF_8bps           ,&TglobalSettingsDecVideo::_8bps          ,0,IDFF_MOVIE_MAX,_l(""),0,NULL,0,
   IDFF_png1           ,&TglobalSettingsDecVideo::png1           ,0,IDFF_MOVIE_MAX,_l(""),0,NULL,0,
   IDFF_qtrle          ,&TglobalSettingsDecVideo::qtrle          ,0,IDFF_MOVIE_MAX,_l(""),0,NULL,0,
   IDFF_duck           ,&TglobalSettingsDecVideo::duck           ,0,IDFF_MOVIE_MAX,_l(""),0,NULL,0,
   IDFF_tscc           ,&TglobalSettingsDecVideo::tscc           ,0,IDFF_MOVIE_MAX,_l(""),0,NULL,0,
   IDFF_qpeg           ,&TglobalSettingsDecVideo::qpeg           ,0,IDFF_MOVIE_MAX,_l(""),0,NULL,0,
   IDFF_loco           ,&TglobalSettingsDecVideo::loco           ,0,IDFF_MOVIE_MAX,_l(""),0,NULL,0,
   IDFF_wnv1           ,&TglobalSettingsDecVideo::wnv1           ,0,IDFF_MOVIE_MAX,_l(""),0,NULL,0,
   IDFF_cscd           ,&TglobalSettingsDecVideo::cscd           ,0,IDFF_MOVIE_MAX,_l(""),0,NULL,0,
   IDFF_zmbv           ,&TglobalSettingsDecVideo::zmbv           ,0,IDFF_MOVIE_MAX,_l(""),0,NULL,0,
   IDFF_ulti           ,&TglobalSettingsDecVideo::ulti           ,0,IDFF_MOVIE_MAX,_l(""),0,NULL,0,
   IDFF_vixl           ,&TglobalSettingsDecVideo::vixl           ,0,IDFF_MOVIE_MAX,_l(""),0,NULL,0,
   IDFF_aasc           ,&TglobalSettingsDecVideo::aasc           ,0,IDFF_MOVIE_MAX,_l(""),0,NULL,0,
   IDFF_fps1           ,&TglobalSettingsDecVideo::fps1           ,0,IDFF_MOVIE_MAX,_l(""),0,NULL,0,
   //IDFF_snow           ,&TglobalSettingsDecVideo::snow           ,0,IDFF_MOVIE_MAX,_l(""),0,NULL,0,
   IDFF_avisV          ,&TglobalSettingsDecVideo::avis           ,0,IDFF_MOVIE_MAX,_l(""),0,NULL,0,
   IDFF_rawv           ,&TglobalSettingsDecVideo::rawv           ,0,0,_l(""),0,
     NULL,0,
   IDFF_supDVDdec      ,&TglobalSettingsDecVideo::supdvddec      ,0,0,_l(""),0,
     _l("supDVDdec"),0,
   IDFF_fastMpeg2      ,&TglobalSettingsDecVideo::fastMpeg2      ,0,0,_l(""),1,
     _l("fastMpeg2"),0,
   IDFF_fastH264       ,&TglobalSettingsDecVideo::fastH264       ,0,3,_l(""),1,
     _l("fastH264"),0,
   IDFF_libtheoraPostproc,&TglobalSettingsDecVideo::libtheoraPostproc ,0,0,_l(""),1,
     _l("libtheoraPostproc"),0,
   IDFF_alternateUncompressed,&TglobalSettingsDecVideo::alternateUncompressed,0,0,_l(""),1,
     _l("alternateUncompressed"),0,
   IDFF_autodetect24P  ,&TglobalSettingsDecVideo::autodetect24P  ,0,0,_l(""),0,
     _l("autodetect24P"),1,
   0
  };
 addOptions(iopts);
}

void TglobalSettingsDecVideo::reg_op(TregOp &t)
{
 TglobalSettingsDec::reg_op(t);
 sub.reg_op(t);
}

int TglobalSettingsDecVideo::getDefault(int id)
{
 switch (id)
  {
   case IDFF_multipleInstances:return filtermode&IDFF_FILTERMODE_VIDEORAW?0:2;
   default:return TglobalSettingsDec::getDefault(id);
  }
}

void TglobalSettingsDecVideo::reg_op_codec(TregOp &t,TregOp *t2)
{
 _reg_op_codec(IDFF_xvid,t,t2,_l("xvid"),xvid,0);
 _reg_op_codec(IDFF_div3,t,t2,_l("div3"),div3,0);
 _reg_op_codec(IDFF_dx50,t,t2,_l("dx50"),dx50,0);
 _reg_op_codec(IDFF_mp4v,t,t2,_l("mp4v"),mp4v,0);
 _reg_op_codec(IDFF_mp43,t,t2,_l("mp43"),mp43,0);
 _reg_op_codec(IDFF_mp42,t,t2,_l("mp42"),mp42,0);
 _reg_op_codec(IDFF_mp41,t,t2,_l("mp41"),mp41,0);
 _reg_op_codec(IDFF_h261,t,t2,_l("h261"),h261,0);
 _reg_op_codec(IDFF_h263,t,t2,_l("h263"),h263,0);
 _reg_op_codec(IDFF_h264,t,t2,_l("h264"),h264,0);
 _reg_op_codec(IDFF_wmv1,t,t2,_l("wmv1"),wmv1,0);
 _reg_op_codec(IDFF_wmv2,t,t2,_l("wmv2"),wmv2,0);
 _reg_op_codec(IDFF_wmv3,t,t2,_l("wmv3"),wmv3,0);
 _reg_op_codec(IDFF_wvc1,t,t2,_l("wvc1"),wvc1,0);
 _reg_op_codec(IDFF_mss2,t,t2,_l("mss2"),mss2,0);
 _reg_op_codec(IDFF_wvp2,t,t2,_l("wvp2"),wvp2,0);
 _reg_op_codec(IDFF_vp5 ,t,t2,_l("vp5") ,vp5,0);
 _reg_op_codec(IDFF_vp6 ,t,t2,_l("vp6") ,vp6,0);
 _reg_op_codec(IDFF_vp6f,t,t2,_l("vp6f"),vp6f,0);
 _reg_op_codec(IDFF_cavs,t,t2,_l("cavs"),cavs,0);
 _reg_op_codec(IDFF_rawv,t,t2,
  filtermode & IDFF_FILTERMODE_VIDEOSUBTITLES ? NULL :
   filtermode & IDFF_FILTERMODE_VIDEORAW ? _l("raw_rawv") : _l("rawv"),
  rawv,
  filtermode & IDFF_FILTERMODE_VIDEORAW ? 1 : 0);
 _reg_op_codec(IDFF_mpg1,t,t2,_l("mpg1"),mpg1,0);
 _reg_op_codec(IDFF_mpg2,t,t2,_l("mpg2"),mpg2,0);
 _reg_op_codec(IDFF_mpegAVI,t,t2,_l("mpegAVI"),mpegAVI,0);
 _reg_op_codec(IDFF_em2v,t,t2,_l("em2v"),em2v,0);
 _reg_op_codec(IDFF_avrn,t,t2,_l("avrn"),avrn,0);
 _reg_op_codec(IDFF_mjpg,t,t2,_l("mjpg"),mjpg,0);
 _reg_op_codec(IDFF_dvsd,t,t2,_l("dvsd"),dvsd,0);
 _reg_op_codec(IDFF_cdvc,t,t2,_l("cdvc"),cdvc,0);
 _reg_op_codec(IDFF_hfyu,t,t2,_l("hfyu"),hfyu,0);
 _reg_op_codec(IDFF_cyuv,t,t2,_l("cyuv"),cyuv,0);
 _reg_op_codec(IDFF_theo,t,t2,_l("theo"),theo,0);
 _reg_op_codec(IDFF_fvfw,t,t2,_l("fvfw"),fvfw,IDFF_MOVIE_LAVC);
 _reg_op_codec(IDFF_asv1,t,t2,_l("asv1"),asv1,0);
 _reg_op_codec(IDFF_vcr1,t,t2,_l("vcr1"),vcr1,0);
 _reg_op_codec(IDFF_svq1,t,t2,_l("svq1"),svq1,0);
 _reg_op_codec(IDFF_svq3,t,t2,_l("svq3"),svq3,0);
 _reg_op_codec(IDFF_qtrpza,t,t2,_l("qtrpza"),qtrpza,0);
 _reg_op_codec(IDFF_cram,t,t2,_l("cram"),cram,0);
 _reg_op_codec(IDFF_rt21,t,t2,_l("rt21"),rt21,0);
 _reg_op_codec(IDFF_iv32,t,t2,_l("iv32"),iv32,0);
 _reg_op_codec(IDFF_cvid,t,t2,_l("cvid"),cvid,0);
 _reg_op_codec(IDFF_rv10,t,t2,_l("rv10"),rv10,0);
 _reg_op_codec(IDFF_rv40,t,t2,_l("rv40"),rv40,0);
 _reg_op_codec(IDFF_ffv1,t,t2,_l("ffv1"),ffv1,IDFF_MOVIE_LAVC);
 _reg_op_codec(IDFF_vp3 ,t,t2,_l("vp3") ,vp3 ,0);
 _reg_op_codec(IDFF_rle ,t,t2,_l("rle") ,rle ,0);
 _reg_op_codec(IDFF_avisV,t,t2,_l("avis"),avis,IDFF_MOVIE_AVIS);
 _reg_op_codec(IDFF_mszh,t,t2,_l("mszh"),mszh,0);
 _reg_op_codec(IDFF_zlib,t,t2,_l("zlib"),zlib,0);
 _reg_op_codec(IDFF_flv1,t,t2,_l("flv1"),flv1,0);
 _reg_op_codec(IDFF_8bps,t,t2,_l("8bps"),_8bps,0);
 _reg_op_codec(IDFF_png1,t,t2,_l("png1"),png1,0);
 _reg_op_codec(IDFF_qtrle,t,t2,_l("qtrle"),qtrle,0);
 _reg_op_codec(IDFF_duck,t,t2,_l("duck"),duck,0);
 _reg_op_codec(IDFF_tscc,t,t2,_l("tscc"),tscc,0);
 _reg_op_codec(IDFF_qpeg,t,t2,_l("qpeg"),qpeg,0);
 _reg_op_codec(IDFF_loco,t,t2,_l("loco"),loco,0);
 _reg_op_codec(IDFF_wnv1,t,t2,_l("wnv1"),wnv1,0);
 _reg_op_codec(IDFF_cscd,t,t2,_l("cscd"),cscd,0);
 _reg_op_codec(IDFF_zmbv,t,t2,_l("zmbv"),zmbv,0);
 _reg_op_codec(IDFF_ulti,t,t2,_l("ulti"),ulti,0);
 _reg_op_codec(IDFF_vixl,t,t2,_l("vixl"),vixl,0);
 _reg_op_codec(IDFF_aasc,t,t2,_l("aasc"),aasc,0);
 _reg_op_codec(IDFF_fps1,t,t2,_l("fps1"),fps1,0);
 //_reg_op_codec(IDFF_snow,t,t2,_l("snow"),snow,0);
 _reg_op_codec(0,t,t2,_l("needCodecFix"),needCodecFix,1);
}

void TglobalSettingsDecVideo::fixNewCodecs(void)
{
 if (mpg1==2) mpg1=IDFF_MOVIE_LIBMPEG2;
 if (mpg2==2) mpg2=IDFF_MOVIE_LIBMPEG2;
 if (em2v==2) em2v=IDFF_MOVIE_LIBMPEG2;
 if (mpegAVI==2) mpegAVI=IDFF_MOVIE_LIBMPEG2;
 if (theo==1) theo=IDFF_MOVIE_THEO;
 //if (rawv==1 && forceInCSP!=0) rawv=forceInCSP;
}

void TglobalSettingsDecVideo::load(void)
{
 TglobalSettingsDec::load();
 if (needCodecFix)
  {
   fixNewCodecs();
   needCodecFix=0;
  }

 fixMissing(xvid,IDFF_MOVIE_LAVC,IDFF_MOVIE_XVID4);
 fixMissing(div3,IDFF_MOVIE_LAVC);
 fixMissing(dx50,IDFF_MOVIE_LAVC,IDFF_MOVIE_XVID4);
 fixMissing(mp4v,IDFF_MOVIE_LAVC,IDFF_MOVIE_XVID4);
 fixMissing(fvfw,IDFF_MOVIE_LAVC,IDFF_MOVIE_XVID4);
 fixMissing(mp43,IDFF_MOVIE_LAVC);
 fixMissing(mp42,IDFF_MOVIE_LAVC);
 fixMissing(mp41,IDFF_MOVIE_LAVC);
 fixMissing(h261,IDFF_MOVIE_LAVC);
 fixMissing(h263,IDFF_MOVIE_LAVC);
 fixMissing(h264,IDFF_MOVIE_LAVC, IDFF_MOVIE_LSVC);
 fixMissing(wmv1,IDFF_MOVIE_LAVC,IDFF_MOVIE_WMV9);
 fixMissing(wmv2,IDFF_MOVIE_LAVC,IDFF_MOVIE_WMV9);
 fixMissing(wmv3,IDFF_MOVIE_LAVC,IDFF_MOVIE_WMV9);
 fixMissing(wvc1,IDFF_MOVIE_LAVC,IDFF_MOVIE_WMV9);
 fixMissing(mss2,IDFF_MOVIE_WMV9);
 fixMissing(wvp2,IDFF_MOVIE_WMV9);
 fixMissing(vp5 ,IDFF_MOVIE_LAVC);
 fixMissing(vp6 ,IDFF_MOVIE_LAVC);
 fixMissing(vp6f,IDFF_MOVIE_LAVC);
 fixMissing(cavs,IDFF_MOVIE_LAVC);
 fixMissing(avrn,IDFF_MOVIE_LAVC);
 fixMissing(mjpg,IDFF_MOVIE_LAVC);
 fixMissing(dvsd,IDFF_MOVIE_LAVC);
 fixMissing(cdvc,IDFF_MOVIE_LAVC);
 fixMissing(hfyu,IDFF_MOVIE_LAVC);
 fixMissing(cyuv,IDFF_MOVIE_LAVC);
 fixMissing(asv1,IDFF_MOVIE_LAVC);
 fixMissing(vcr1,IDFF_MOVIE_LAVC);
 fixMissing(svq1,IDFF_MOVIE_LAVC);
 fixMissing(svq3,IDFF_MOVIE_LAVC);
 fixMissing(qtrpza,IDFF_MOVIE_LAVC);
 fixMissing(mpg1,IDFF_MOVIE_LAVC,IDFF_MOVIE_LIBMPEG2);
 fixMissing(mpg2,IDFF_MOVIE_LAVC,IDFF_MOVIE_LIBMPEG2);
 fixMissing(mpegAVI,IDFF_MOVIE_LAVC,IDFF_MOVIE_LIBMPEG2);
 fixMissing(em2v,IDFF_MOVIE_LAVC,IDFF_MOVIE_LIBMPEG2);
 fixMissing(theo,IDFF_MOVIE_LAVC,IDFF_MOVIE_THEO);
 fixMissing(cram,IDFF_MOVIE_LAVC);
 fixMissing(rt21,IDFF_MOVIE_LAVC);
 fixMissing(iv32,IDFF_MOVIE_LAVC);
 fixMissing(cvid,IDFF_MOVIE_LAVC);
 fixMissing(rv10,IDFF_MOVIE_LAVC);
 fixMissing(rv40,IDFF_MOVIE_LAVC);
 fixMissing(ffv1,IDFF_MOVIE_LAVC);
 fixMissing(vp3 ,IDFF_MOVIE_LAVC);
 fixMissing(rle ,IDFF_MOVIE_LAVC);
 fixMissing(mszh,IDFF_MOVIE_LAVC);
 fixMissing(zlib,IDFF_MOVIE_LAVC);
 fixMissing(flv1,IDFF_MOVIE_LAVC);
 fixMissing(_8bps,IDFF_MOVIE_LAVC);
 fixMissing(png1,IDFF_MOVIE_LAVC);
 fixMissing(qtrle,IDFF_MOVIE_LAVC);
 fixMissing(duck,IDFF_MOVIE_LAVC);
 fixMissing(tscc,IDFF_MOVIE_LAVC);
 fixMissing(qpeg,IDFF_MOVIE_LAVC);
 fixMissing(loco,IDFF_MOVIE_LAVC);
 fixMissing(wnv1,IDFF_MOVIE_LAVC);
 fixMissing(cscd,IDFF_MOVIE_LAVC);
 fixMissing(zmbv,IDFF_MOVIE_LAVC);
 fixMissing(ulti,IDFF_MOVIE_LAVC);
 fixMissing(vixl,IDFF_MOVIE_LAVC);
 fixMissing(aasc,IDFF_MOVIE_LAVC);
 fixMissing(fps1,IDFF_MOVIE_LAVC);
 fixMissing(avis,IDFF_MOVIE_AVIS);
 //fixMissing(snow,IDFF_MOVIE_LAVC);
}

#define FF_FOURCC1_CODEC(fourCC1,decoder,codec)  \
 case FOURCC_##fourCC1:                          \
  codecId=(decoder)?codec:CODEC_ID_NONE;         \
  break;
#define FF_FOURCC_CODEC(fourCC1,decoder,codecs)  \
 case FOURCC_##fourCC1:                          \
  codecId=codecs[decoder];                       \
  break;

#define VIDEO_FOURCCS  \
 FF_FOURCC_OP (FFDS,fvfw & rawmask,c_mpeg4) \
 FF_FOURCC_OP (FVFW,fvfw & rawmask,c_mpeg4) \
 FF_FOURCC_OP (XVID,xvid & rawmask,c_mpeg4) \
 FF_FOURCC_OP (DIVX,dx50 & rawmask,c_mpeg4) \
 FF_FOURCC_OP (DX50,dx50 & rawmask,c_mpeg4) \
 FF_FOURCC_OP (MP4V,mp4v & rawmask,c_mpeg4) \
 FF_FOURCC_OP (3IVX,mp4v & rawmask,c_mpeg4) \
 FF_FOURCC_OP (WV1F,mp4v & rawmask,c_mpeg4) \
 FF_FOURCC_OP (FMP4,mp4v & rawmask,c_mpeg4) \
 FF_FOURCC_OP (SMP4,mp4v & rawmask,c_mpeg4) \
 FF_FOURCC_OP (WMV1,wmv1 & rawmask,c_wmv1) \
 FF_FOURCC_OP (WMV2,wmv2 & rawmask,c_wmv2) \
 FF_FOURCC_OP (WMV3,wmv3 & rawmask,c_wmv3) \
 FF_FOURCC1_OP(MSS1,mss2 & rawmask,CODEC_ID_WMV9_LIB) \
 FF_FOURCC1_OP(MSS2,mss2 & rawmask,CODEC_ID_WMV9_LIB) \
 FF_FOURCC1_OP(WVP2,wvp2 & rawmask,CODEC_ID_WMV9_LIB) \
 FF_FOURCC1_OP(WMVP,wvp2 & rawmask,CODEC_ID_WMV9_LIB) \
 FF_FOURCC_OP (WVC1,wvc1 & rawmask,c_wvc1) \
 FF_FOURCC_OP (WMVA,wvc1 & rawmask,c_wvc1) \
 FF_FOURCC1_OP(VP50,vp5 & rawmask,CODEC_ID_VP5) \
 FF_FOURCC1_OP(VP60,vp6 & rawmask,CODEC_ID_VP6) \
 FF_FOURCC1_OP(VP61,vp6 & rawmask,CODEC_ID_VP6) \
 FF_FOURCC1_OP(VP62,vp6 & rawmask,CODEC_ID_VP6) \
 FF_FOURCC1_OP(VP6F,vp6f & rawmask,CODEC_ID_VP6F) \
 FF_FOURCC1_OP(FLV4,vp6f & rawmask,CODEC_ID_VP6F) \
 FF_FOURCC1_OP(VP6A,vp6f & rawmask,CODEC_ID_VP6A) \
 FF_FOURCC1_OP(CAVS,cavs & rawmask,CODEC_ID_CAVS) \
 FF_FOURCC_OP (MPG1,mpg1 & rawmask,c_mpeg1) \
 FF_FOURCC_OP (MPG2,mpg2 & rawmask,c_mpeg2) \
 FF_FOURCC_OP (EM2V,em2v & rawmask,c_mpeg2) \
 FF_FOURCC_OP (MMES,em2v & rawmask,c_mpeg2) \
 FF_FOURCC_OP (MPEG,mpegAVI & rawmask,c_mpeg1) \
 FF_FOURCC1_OP(H263,h263 & rawmask,CODEC_ID_H263) \
 /*FF_FOURCC1_OP(I263,h263 & rawmask,CODEC_ID_H263I)*/ \
 FF_FOURCC1_OP(S263,h263 & rawmask,CODEC_ID_H263) \
 FF_FOURCC1_OP(L263,h263 & rawmask,CODEC_ID_H263) \
 FF_FOURCC1_OP(M263,h263 & rawmask,CODEC_ID_H263) \
 FF_FOURCC1_OP(U263,h263 & rawmask,CODEC_ID_H263) \
 FF_FOURCC1_OP(X263,h263 & rawmask,CODEC_ID_H263) \
 FF_FOURCC_OP(H264,h264 & rawmask,c_h264) \
 FF_FOURCC_OP(X264,h264 & rawmask,c_h264) \
 FF_FOURCC_OP(VSSH,h264 & rawmask,c_h264) \
 FF_FOURCC_OP(DAVC,h264 & rawmask,c_h264) \
 FF_FOURCC_OP(PAVC,h264 & rawmask,c_h264) \
 FF_FOURCC_OP(AVC1,h264 & rawmask,c_h264) \
 /*FF_FOURCC1_OP(SVM2,h264 & rawmask,CODEC_ID_H264)*/ \
 FF_FOURCC1_OP(MJPG,mjpg & rawmask,CODEC_ID_MJPEG) \
 FF_FOURCC1_OP(LJPG,mjpg & rawmask,CODEC_ID_MJPEG) \
 FF_FOURCC1_OP(MJLS,mjpg & rawmask,CODEC_ID_JPEGLS) \
 FF_FOURCC1_OP(JPEG,(mjpg || svq1 || svq3) & rawmask,CODEC_ID_MJPEG) \
 FF_FOURCC1_OP(AVRN,avrn & rawmask,CODEC_ID_MJPEG) \
 FF_FOURCC1_OP(MJPA,avrn & rawmask,CODEC_ID_MJPEG) \
 FF_FOURCC1_OP(AMVV,avrn & rawmask,CODEC_ID_AMV) \
 FF_FOURCC1_OP(SP5X,avrn & rawmask,CODEC_ID_SP5X) \
 FF_FOURCC1_OP(SP51,avrn & rawmask,CODEC_ID_SP5X) \
 FF_FOURCC1_OP(SP52,avrn & rawmask,CODEC_ID_SP5X) \
 FF_FOURCC1_OP(SP53,avrn & rawmask,CODEC_ID_SP5X) \
 FF_FOURCC1_OP(SP54,avrn & rawmask,CODEC_ID_SP5X) \
 FF_FOURCC1_OP(SP55,avrn & rawmask,CODEC_ID_SP5X) \
 FF_FOURCC1_OP(SP56,avrn & rawmask,CODEC_ID_SP5X) \
 FF_FOURCC1_OP(SP57,avrn & rawmask,CODEC_ID_SP5X) \
 FF_FOURCC1_OP(DVSD,dvsd & rawmask,CODEC_ID_DVVIDEO) \
 FF_FOURCC1_OP(DV25,dvsd & rawmask,CODEC_ID_DVVIDEO) \
 FF_FOURCC1_OP(DV50,dvsd & rawmask,CODEC_ID_DVVIDEO) \
 FF_FOURCC1_OP(CDVC,cdvc & rawmask,CODEC_ID_DVVIDEO) \
 FF_FOURCC1_OP(CDV5,cdvc & rawmask,CODEC_ID_DVVIDEO) \
 FF_FOURCC1_OP(DVIS,cdvc & rawmask,CODEC_ID_DVVIDEO) \
 FF_FOURCC1_OP(PDVC,cdvc & rawmask,CODEC_ID_DVVIDEO) \
 FF_FOURCC1_OP(HFYU,hfyu & rawmask,CODEC_ID_HUFFYUV) \
 FF_FOURCC1_OP(FFVH,hfyu & rawmask,CODEC_ID_HUFFYUV) \
 FF_FOURCC1_OP(CYUV,cyuv & rawmask,CODEC_ID_CYUV) \
 FF_FOURCC1_OP(ASV1,asv1 & rawmask,CODEC_ID_ASV1) \
 FF_FOURCC1_OP(ASV2,asv1 & rawmask,CODEC_ID_ASV2) \
 FF_FOURCC1_OP(VCR1,vcr1 & rawmask,CODEC_ID_VCR1) \
 FF_FOURCC_OP (THEO,theo & rawmask,c_theora) \
 FF_FOURCC1_OP(SVQ1,svq1 & rawmask,CODEC_ID_SVQ1) \
 FF_FOURCC1_OP(SVQ3,svq3 & rawmask,CODEC_ID_SVQ3) \
 FF_FOURCC1_OP(RPZA,qtrpza & rawmask,CODEC_ID_RPZA) \
 FF_FOURCC1_OP(FFV1,ffv1 & rawmask,CODEC_ID_FFV1) \
 FF_FOURCC1_OP(VP31,vp3  & rawmask,CODEC_ID_VP3) \
 FF_FOURCC1_OP(RLE8,rle  & rawmask,CODEC_ID_MSRLE) \
 FF_FOURCC1_OP(MSZH,mszh & rawmask,CODEC_ID_MSZH) \
 FF_FOURCC1_OP(ZLIB,zlib & rawmask,CODEC_ID_ZLIB) \
 FF_FOURCC1_OP(FLV1,flv1 & rawmask,CODEC_ID_FLV1) \
 FF_FOURCC1_OP(PNG1,png1 & rawmask,CODEC_ID_COREPNG) \
 FF_FOURCC1_OP(MPNG,png1 & rawmask,CODEC_ID_PNG) \
 FF_FOURCC1_OP(AVIS,avis & rawmask,CODEC_ID_AVISYNTH) \
 FF_FOURCC1_OP(CRAM,cram & rawmask,CODEC_ID_MSVIDEO1) \
 FF_FOURCC1_OP(RT21,rt21 & rawmask,CODEC_ID_INDEO2) \
 FF_FOURCC1_OP(IV32,iv32 & rawmask,CODEC_ID_INDEO3) \
 FF_FOURCC1_OP(IV31,iv32 & rawmask,CODEC_ID_INDEO3) \
 FF_FOURCC1_OP(CVID,cvid & rawmask,CODEC_ID_CINEPAK) \
 FF_FOURCC1_OP(RV10,rv10 & rawmask,CODEC_ID_RV10) \
 FF_FOURCC1_OP(RV20,rv10 & rawmask,CODEC_ID_RV20) \
 FF_FOURCC1_OP(RV40,rv40 & rawmask,CODEC_ID_RV40) \
 FF_FOURCC1_OP(8BPS,_8bps& rawmask,CODEC_ID_8BPS) \
 FF_FOURCC1_OP(QRLE,qtrle& rawmask,CODEC_ID_QTRLE) \
 FF_FOURCC1_OP(DUCK,duck & rawmask,CODEC_ID_TRUEMOTION1) \
 FF_FOURCC1_OP(TM20,duck & rawmask,CODEC_ID_TRUEMOTION2) \
 FF_FOURCC1_OP(TSCC,tscc & rawmask,CODEC_ID_TSCC) \
 /*FF_FOURCC1_OP(SNOW,snow & rawmask,CODEC_ID_SNOW)*/ \
 FF_FOURCC1_OP(QPEG,qpeg & rawmask,CODEC_ID_QPEG) \
 FF_FOURCC1_OP(Q1_0,qpeg & rawmask,CODEC_ID_QPEG) \
 FF_FOURCC1_OP(Q1_1,qpeg & rawmask,CODEC_ID_QPEG) \
 FF_FOURCC1_OP(H261,h261 & rawmask,CODEC_ID_H261) \
 FF_FOURCC1_OP(M261,h261 & rawmask,CODEC_ID_H261) \
 FF_FOURCC1_OP(LOCO,loco & rawmask,CODEC_ID_LOCO) \
 FF_FOURCC1_OP(WNV1,wnv1 & rawmask,CODEC_ID_WNV1) \
 FF_FOURCC1_OP(CSCD,cscd & rawmask,CODEC_ID_CSCD) \
 FF_FOURCC1_OP(ZMBV,zmbv & rawmask,CODEC_ID_ZMBV) \
 FF_FOURCC1_OP(ULTI,ulti & rawmask,CODEC_ID_ULTI) \
 FF_FOURCC1_OP(VIXL,vixl & rawmask,CODEC_ID_VIXL) \
 FF_FOURCC1_OP(AASC,aasc & rawmask,CODEC_ID_AASC) \
 FF_FOURCC1_OP(FPS1,fps1 & rawmask,CODEC_ID_FRAPS) \
 /* uncompressed video */ \
 FF_FOURCC1_OP(YUY2,rawv==1 || rawv==2 || rawv==FOURCC_YUY2,CODEC_ID_YUY2) \
 FF_FOURCC1_OP(YV12,rawv==1 || rawv==2 || rawv==FOURCC_YV12,CODEC_ID_YV12) \
 FF_FOURCC1_OP(YVYU,rawv==1 || rawv==2 || rawv==FOURCC_YVYU,CODEC_ID_YVYU) \
 FF_FOURCC1_OP(UYVY,rawv==1 || rawv==2 || rawv==FOURCC_UYVY,CODEC_ID_UYVY) \
 FF_FOURCC1_OP(VYUY,rawv==1 || rawv==2 || rawv==FOURCC_VYUY,CODEC_ID_VYUY) \
 FF_FOURCC1_OP(I420,rawv==1 || rawv==2 || rawv==FOURCC_I420,CODEC_ID_I420) \
 FF_FOURCC1_OP(IYUV,rawv==1 || rawv==2 || rawv==FOURCC_IYUV,CODEC_ID_I420) \
 FF_FOURCC1_OP(RGB2,rawv==1 || rawv==3 || rawv==FOURCC_RGB2,CODEC_ID_RGB2) \
 FF_FOURCC1_OP(RGB3,rawv==1 || rawv==3 || rawv==FOURCC_RGB3,CODEC_ID_RGB3) \
 FF_FOURCC1_OP(RGB5,rawv==1 || rawv==3 || rawv==FOURCC_RGB5,CODEC_ID_RGB5) \
 FF_FOURCC1_OP(RGB6,rawv==1 || rawv==3 || rawv==FOURCC_RGB6,CODEC_ID_RGB6) \
 FF_FOURCC1_OP(CLJR,rawv==1            || rawv==FOURCC_CLJR,CODEC_ID_CLJR) \
 FF_FOURCC1_OP(Y800,rawv==1 || rawv==2 || rawv==FOURCC_Y800,CODEC_ID_Y800) \
 FF_FOURCC1_OP(444P,rawv==1 || rawv==2 || rawv==FOURCC_444P,CODEC_ID_444P) \
 FF_FOURCC1_OP(YV24,rawv==1 || rawv==2 || rawv==FOURCC_YV24,CODEC_ID_444P) \
 FF_FOURCC1_OP(422P,rawv==1 || rawv==2 || rawv==FOURCC_422P,CODEC_ID_422P) \
 FF_FOURCC1_OP(YV16,rawv==1 || rawv==2 || rawv==FOURCC_YV16,CODEC_ID_YV16) \
 FF_FOURCC1_OP(411P,rawv==1 || rawv==2 || rawv==FOURCC_411P,CODEC_ID_411P) \
 FF_FOURCC1_OP(Y41B,rawv==1 || rawv==2 || rawv==FOURCC_Y41B,CODEC_ID_411P) \
 FF_FOURCC1_OP(410P,rawv==1 || rawv==2 || rawv==FOURCC_410P,CODEC_ID_410P) \
 FF_FOURCC1_OP(NV12,rawv==1 || rawv==2 || rawv==FOURCC_NV12,CODEC_ID_NV12) \
 FF_FOURCC1_OP(NV21,rawv==1 || rawv==2 || rawv==FOURCC_NV12,CODEC_ID_NV21) \
 /*FF_FOURCC1_OP(PAL1,rawv==1 || rawv==4 || rawv==FOURCC_PAL1,CODEC_ID_PAL1)*/ \
 /*FF_FOURCC1_OP(PAL4,rawv==1 || rawv==4 || rawv==FOURCC_PAL4,CODEC_ID_PAL4)*/ \
 /*FF_FOURCC1_OP(PAL8,rawv==1 || rawv==4 || rawv==FOURCC_PAL8,CODEC_ID_PAL8)*/ \
 /* mencoder's raw formats */ \
 FF_FOURCC1_OP(RGB24MPLAYER,rawv==1 || rawv==3 || rawv==FOURCC_RGB2,CODEC_ID_BGR2) \
 FF_FOURCC1_OP(RGB32MPLAYER,rawv==1 || rawv==3 || rawv==FOURCC_RGB3,CODEC_ID_BGR3) \
 FF_FOURCC1_OP(RGB15MPLAYER,rawv==1 || rawv==3 || rawv==FOURCC_RGB5,CODEC_ID_BGR5) \
 FF_FOURCC1_OP(RGB16MPLAYER,rawv==1 || rawv==3 || rawv==FOURCC_RGB6,CODEC_ID_BGR6) \
 /* exotic */ \
 FF_FOURCC_OP (MP4S,mp4v & rawmask,c_mpeg4) \
 FF_FOURCC_OP (SEDG,mp4v & rawmask,c_mpeg4) \
 FF_FOURCC_OP (M4S2,mp4v & rawmask,c_mpeg4) \
 FF_FOURCC_OP (BLZ0,mp4v & rawmask,c_mpeg4) \
 FF_FOURCC_OP (DXGM,mp4v & rawmask,c_mpeg4) \
 FF_FOURCC_OP (LMP4,mp4v & rawmask,c_mpeg4) \
 FF_FOURCC_OP (RMP4,mp4v & rawmask,c_mpeg4) \
 FF_FOURCC_OP (DM4V,mp4v & rawmask,c_mpeg4) \
 FF_FOURCC_OP (HDX4,mp4v & rawmask,c_mpeg4) \
 FF_FOURCC_OP (UMP4,mp4v & rawmask,c_mpeg4) \
 FF_FOURCC_OP (NDIG,mp4v & rawmask,c_mpeg4) \
 FF_FOURCC_OP (XVIX,xvid & rawmask,c_mpeg4) \
 FF_FOURCC_OP (3IV1,mp4v & rawmask,c_mpeg4) \
 FF_FOURCC_OP (3IV2,mp4v & rawmask,c_mpeg4) \
 FF_FOURCC1_OP(DIV3,div3 & rawmask,CODEC_ID_MSMPEG4V3) \
 FF_FOURCC1_OP(DVX3,div3 & rawmask,CODEC_ID_MSMPEG4V3) \
 FF_FOURCC1_OP(MP43,mp43 & rawmask,CODEC_ID_MSMPEG4V3) \
 FF_FOURCC1_OP(COL1,div3 & rawmask,CODEC_ID_MSMPEG4V3) \
 FF_FOURCC1_OP(DIV4,div3 & rawmask,CODEC_ID_MSMPEG4V3) \
 FF_FOURCC1_OP(DIV5,div3 & rawmask,CODEC_ID_MSMPEG4V3) \
 FF_FOURCC1_OP(DIV6,div3 & rawmask,CODEC_ID_MSMPEG4V3) \
 FF_FOURCC1_OP(AP41,div3 & rawmask,CODEC_ID_MSMPEG4V3) \
 FF_FOURCC1_OP(MPG3,div3 & rawmask,CODEC_ID_MSMPEG4V3) \
 FF_FOURCC1_OP(DIV2,mp42 & rawmask,CODEC_ID_MSMPEG4V2) \
 FF_FOURCC1_OP(MP42,mp42 & rawmask,CODEC_ID_MSMPEG4V2) \
 FF_FOURCC1_OP(MPG4,mp41 & rawmask,CODEC_ID_MSMPEG4V1) \
 FF_FOURCC1_OP(DIV1,mp41 & rawmask,CODEC_ID_MSMPEG4V1) \
 FF_FOURCC1_OP(MP41,mp41 & rawmask,CODEC_ID_MSMPEG4V1) \

#define FF_FOURCC_LIST(fourCC,decoder,codec) _l(#fourCC),

const char_t* TglobalSettingsDecVideo::fourccs[]=
{
 #undef FF_FOURCC_OP
 #undef FF_FOURCC1_OP
 #define FF_FOURCC_OP  FF_FOURCC_LIST
 #define FF_FOURCC1_OP FF_FOURCC_LIST
 VIDEO_FOURCCS
 NULL
};
const char_t** TglobalSettingsDecVideo::getFOURCClist(void) const
{
 return fourccs;
}

const CodecID TglobalSettingsDecVideo::c_h264[IDFF_MOVIE_MAX+1]=
{
 CODEC_ID_NONE,
 CODEC_ID_H264,
 CODEC_ID_SVC
};

const CodecID TglobalSettingsDecVideo::c_mpeg4[IDFF_MOVIE_MAX+1]=
{
 CODEC_ID_NONE,
 CODEC_ID_MPEG4,
 CODEC_ID_NONE,
 CODEC_ID_NONE,
 CODEC_ID_NONE,
 CODEC_ID_NONE,
 CODEC_ID_NONE,
 CODEC_ID_NONE,
 CODEC_ID_NONE,
 CODEC_ID_XVID4
};
const CodecID TglobalSettingsDecVideo::c_mpeg1[IDFF_MOVIE_MAX+1]=
{
 CODEC_ID_NONE,
 CODEC_ID_MPEG1VIDEO,
 CODEC_ID_NONE,
 CODEC_ID_NONE,
 CODEC_ID_NONE,
 CODEC_ID_LIBMPEG2
};
const CodecID TglobalSettingsDecVideo::c_mpeg2[IDFF_MOVIE_MAX+1]=
{
 CODEC_ID_NONE,
 CODEC_ID_MPEG2VIDEO,
 CODEC_ID_NONE,
 CODEC_ID_NONE,
 CODEC_ID_NONE,
 CODEC_ID_LIBMPEG2
};
const CodecID TglobalSettingsDecVideo::c_theora[IDFF_MOVIE_MAX+1]=
{
 CODEC_ID_NONE,
 CODEC_ID_THEORA,
 CODEC_ID_NONE,
 CODEC_ID_THEORA_LIB
};
const CodecID TglobalSettingsDecVideo::c_wvc1[IDFF_MOVIE_MAX+1]=
{
 CODEC_ID_NONE,
 CODEC_ID_VC1,
 CODEC_ID_NONE,
 CODEC_ID_NONE,
 CODEC_ID_NONE,
 CODEC_ID_NONE,
 CODEC_ID_NONE,
 CODEC_ID_NONE,
 CODEC_ID_NONE,
 CODEC_ID_NONE,
 CODEC_ID_NONE,
 CODEC_ID_NONE,
 CODEC_ID_WMV9_LIB
};
const CodecID TglobalSettingsDecVideo::c_wmv3[IDFF_MOVIE_MAX+1]=
{
 CODEC_ID_NONE,
 CODEC_ID_WMV3,
 CODEC_ID_NONE,
 CODEC_ID_NONE,
 CODEC_ID_NONE,
 CODEC_ID_NONE,
 CODEC_ID_NONE,
 CODEC_ID_NONE,
 CODEC_ID_NONE,
 CODEC_ID_NONE,
 CODEC_ID_NONE,
 CODEC_ID_NONE,
 CODEC_ID_WMV9_LIB
};
const CodecID TglobalSettingsDecVideo::c_wmv2[IDFF_MOVIE_MAX+1]=
{
 CODEC_ID_NONE,
 CODEC_ID_WMV2,
 CODEC_ID_NONE,
 CODEC_ID_NONE,
 CODEC_ID_NONE,
 CODEC_ID_NONE,
 CODEC_ID_NONE,
 CODEC_ID_NONE,
 CODEC_ID_NONE,
 CODEC_ID_NONE,
 CODEC_ID_NONE,
 CODEC_ID_NONE,
 CODEC_ID_WMV9_LIB
};
const CodecID TglobalSettingsDecVideo::c_wmv1[IDFF_MOVIE_MAX+1]=
{
 CODEC_ID_NONE,
 CODEC_ID_WMV1,
 CODEC_ID_NONE,
 CODEC_ID_NONE,
 CODEC_ID_NONE,
 CODEC_ID_NONE,
 CODEC_ID_NONE,
 CODEC_ID_NONE,
 CODEC_ID_NONE,
 CODEC_ID_NONE,
 CODEC_ID_NONE,
 CODEC_ID_NONE,
 CODEC_ID_WMV9_LIB
};

void TglobalSettingsDecVideo::getCodecsList(Tstrptrs &codecs) const
{
 std::vector<CodecID> ids;
 #undef FF_FOURCC_OP
 #undef FF_FOURCC1_OP
 #define FF_FOURCC_OP(format,decoder,codecs) for (int j=0;j<=IDFF_MOVIE_MAX;j++) if (codecs[j]!=CODEC_ID_NONE) ids.push_back(codecs[j]);
 #define FF_FOURCC1_OP(format,decoder,codec) ids.push_back(codec);
 VIDEO_FOURCCS
 cleanupCodecsList(ids,codecs);
}

CodecID TglobalSettingsDecVideo::getCodecId(DWORD fourCC,FOURCC *AVIfourCC) const
{
 fourCC=FCCupper(fourCC);
 CodecID codecId=CODEC_ID_NONE;
 int rawmask=filtermode&IDFF_FILTERMODE_VIDEORAW?0:0xffff;
 switch(fourCC)
  {
   #undef FF_FOURCC_OP
   #undef FF_FOURCC1_OP
   #define FF_FOURCC_OP FF_FOURCC_CODEC
   #define FF_FOURCC1_OP FF_FOURCC1_CODEC
   VIDEO_FOURCCS
   default:return CODEC_ID_NONE;
  }

 if (codecId && AVIfourCC)
  *AVIfourCC=fourCC;
 return codecId;
}

//======================== TglobalSettingsDecVideo::TsubtitlesSettings ==========================
TglobalSettingsDecVideo::TsubtitlesSettings::TsubtitlesSettings(TintStrColl *Icoll):Toptions(Icoll)
{
 static const TintOptionT<TsubtitlesSettings> iopts[]=
  {
   IDFF_subTextpin        ,&TsubtitlesSettings::textpin        ,0,0,_l(""),0,
     _l("subTextpin"),0,
   IDFF_subTextpinSSA     ,&TsubtitlesSettings::textpinSSA     ,0,0,_l(""),0,
     _l("subTextpinSSA"),0,
   IDFF_subWatch          ,&TsubtitlesSettings::watch          ,0,0,_l(""),1,
     _l("subWatch"),0,
   IDFF_subSearchHeuristic,&TsubtitlesSettings::searchHeuristic,0,0,_l(""),1,
     _l("subSearchHeuristic"),0,
   0
  };
 addOptions(iopts);
 static const TstrOption sopts[]=
  {
   IDFF_subSearchDir,(TstrVal)&TsubtitlesSettings::searchDir,2*MAX_PATH ,0 ,_l(""),0,
     _l("subSearchDir"),NULL,
   IDFF_subSearchExt,(TstrVal)&TsubtitlesSettings::searchExt,2*MAX_PATH ,0 ,_l(""),0,
     _l("subSearchExt"),_l("utf;idx;sub;srt;smi;rt;txt;ass;ssa;aqt;mpl;usf"),
   0
  };
 addOptions(sopts);
}

void TglobalSettingsDecVideo::TsubtitlesSettings::getDefaultStr(int id,char_t *buf,size_t buflen)
{
 if (id==IDFF_subSearchDir)
  {
   TregOpRegRead tt(HKEY_LOCAL_MACHINE,_l("SOFTWARE\\PRR\\DivXG400"));
   tt._REG_OP_S(0,_l("SubPath"),buf,buflen,_l("."));
  }
}

//=================================== TglobalSettingsDecAudio ===================================
TglobalSettingsDecAudio::TglobalSettingsDecAudio(const Tconfig *Iconfig,int Imode,TintStrColl *Icoll,const char_t *Ireg_child):TglobalSettingsDec(Iconfig,Imode,Ireg_child,Icoll,&osd),osd(Icoll)
{
 static const TintOptionT<TglobalSettingsDecAudio> iopts[]=
  {
   IDFF_wma7                   ,&TglobalSettingsDecAudio::wma1                   ,0,IDFF_MOVIE_MAX,_l(""),0,NULL,0,
   IDFF_wma8                   ,&TglobalSettingsDecAudio::wma2                   ,0,IDFF_MOVIE_MAX,_l(""),0,NULL,0,
   IDFF_mp3                    ,&TglobalSettingsDecAudio::mp3                    ,0,IDFF_MOVIE_MAX,_l(""),0,NULL,0,
   IDFF_mp2                    ,&TglobalSettingsDecAudio::mp2                    ,0,IDFF_MOVIE_MAX,_l(""),0,NULL,0,
   IDFF_ac3                    ,&TglobalSettingsDecAudio::ac3                    ,0,IDFF_MOVIE_MAX,_l(""),0,NULL,0,
   IDFF_mlp                    ,&TglobalSettingsDecAudio::mlp                    ,0,IDFF_MOVIE_MAX,_l(""),0,NULL,0,
   IDFF_eac3                   ,&TglobalSettingsDecAudio::eac3                   ,0,IDFF_MOVIE_MAX,_l(""),0,NULL,0,
   IDFF_dts                    ,&TglobalSettingsDecAudio::dts                    ,0,IDFF_MOVIE_MAX,_l(""),0,NULL,0,
   IDFF_aac                    ,&TglobalSettingsDecAudio::aac                    ,0,IDFF_MOVIE_MAX,_l(""),0,NULL,0,
   IDFF_amr                    ,&TglobalSettingsDecAudio::amr                    ,0,IDFF_MOVIE_MAX,_l(""),0,NULL,0,
   IDFF_iadpcm                 ,&TglobalSettingsDecAudio::iadpcm                 ,0,IDFF_MOVIE_MAX,_l(""),0,NULL,0,
   IDFF_msadpcm                ,&TglobalSettingsDecAudio::msadpcm                ,0,IDFF_MOVIE_MAX,_l(""),0,NULL,0,
   IDFF_otherAdpcm             ,&TglobalSettingsDecAudio::otherAdpcm             ,0,IDFF_MOVIE_MAX,_l(""),0,NULL,0,
   IDFF_law                    ,&TglobalSettingsDecAudio::law                    ,0,IDFF_MOVIE_MAX,_l(""),0,NULL,0,
   IDFF_gsm                    ,&TglobalSettingsDecAudio::gsm                    ,0,IDFF_MOVIE_MAX,_l(""),0,NULL,0,
   IDFF_lpcm                   ,&TglobalSettingsDecAudio::lpcm                   ,0,IDFF_MOVIE_MAX,_l(""),0,NULL,0,
   IDFF_flac                   ,&TglobalSettingsDecAudio::flac                   ,0,IDFF_MOVIE_MAX,_l(""),0,NULL,0,
   IDFF_tta                    ,&TglobalSettingsDecAudio::tta                    ,0,IDFF_MOVIE_MAX,_l(""),0,NULL,0,
   IDFF_qdm2                   ,&TglobalSettingsDecAudio::qdm2                   ,0,IDFF_MOVIE_MAX,_l(""),0,NULL,0,
   IDFF_mace                   ,&TglobalSettingsDecAudio::mace                   ,0,IDFF_MOVIE_MAX,_l(""),0,NULL,0,
   IDFF_truespeech             ,&TglobalSettingsDecAudio::truespeech             ,0,IDFF_MOVIE_MAX,_l(""),0,NULL,0,
   IDFF_vorbis                 ,&TglobalSettingsDecAudio::vorbis                 ,0,IDFF_MOVIE_MAX,_l(""),0,NULL,0,
   IDFF_ra                     ,&TglobalSettingsDecAudio::ra                     ,0,IDFF_MOVIE_MAX,_l(""),0,NULL,0,
   IDFF_imc                    ,&TglobalSettingsDecAudio::imc                    ,0,IDFF_MOVIE_MAX,_l(""),0,NULL,0,
   IDFF_atrac3                 ,&TglobalSettingsDecAudio::atrac3                 ,0,IDFF_MOVIE_MAX,_l(""),0,NULL,0,
   IDFF_nellymoser             ,&TglobalSettingsDecAudio::nellymoser             ,0,IDFF_MOVIE_MAX,_l(""),0,NULL,0,
   IDFF_avisA                  ,&TglobalSettingsDecAudio::avis                   ,0,IDFF_MOVIE_MAX,_l(""),0,NULL,0,
   IDFF_rawa                   ,&TglobalSettingsDecAudio::rawa                   ,0,0             ,_l(""),0,NULL,0,
   IDFF_showCurrentVolume      ,&TglobalSettingsDecAudio::showCurrentVolume      ,0,0             ,_l(""),0,
     _l("showCurrentVolume"),0,
   IDFF_showCurrentFFT         ,&TglobalSettingsDecAudio::showCurrentFFT         ,0,0             ,_l(""),0,
     _l("showCurrentFFT"),0,
   IDFF_firIsUserDisplayMaxFreq,&TglobalSettingsDecAudio::firIsUserDisplayMaxFreq,0,0             ,_l(""),0,
     _l("firIsUserDisplayMaxFreq"),0,
   IDFF_firUserDisplayMaxFreq  ,&TglobalSettingsDecAudio::firUserDisplayMaxFreq  ,1,192000/2      ,_l(""),0,
     _l("firUserDisplayMaxFreq"),48000,
   IDFF_isAudioSwitcher        ,&TglobalSettingsDecAudio::isAudioSwitcher        ,0,0             ,_l(""),0,
     _l("isAudioSwitcher"),0,
   IDFF_alwaysextensible       ,&TglobalSettingsDecAudio::alwaysextensible       ,0,0             ,_l(""),0,
     _l("alwaysextensible"),1,
   IDFF_allowOutStream         ,&TglobalSettingsDecAudio::allowOutStream         ,0,0             ,_l(""),0,
     _l("allowOutStream"),1,
   IDFF_dtsinwav               ,&TglobalSettingsDecAudio::dtsinwav               ,0,0             ,_l(""),0,
     _l("dtsinwav"),1,
   IDFF_vorbisgain             ,&TglobalSettingsDecAudio::vorbisgain             ,0,0             ,_l(""),0,
     _l("vorbisgain"),0,
   0
  };
 addOptions(iopts);
 static const TstrOption sopts[]=
  {
   IDFF_winamp2dir             ,(TstrVal)&TglobalSettingsDecAudio::winamp2dir    ,MAX_PATH - 21 ,0     ,_l(""),0,
     _l("winamp2dir"),NULL,
   0
  };
 addOptions(sopts);
}
int TglobalSettingsDecAudio::getDefault(int id)
{
 switch (id)
  {
   case IDFF_multipleInstances:return filtermode&IDFF_FILTERMODE_AUDIORAW?0:1;
   default:return TglobalSettingsDec::getDefault(id);
  }
}
void TglobalSettingsDecAudio::getDefaultStr(int id,char_t *buf,size_t buflen)
{
 if (id==IDFF_winamp2dir)
  {
   TregOpRegRead wt(HKEY_CURRENT_USER,_l("Software\\Winamp"));
   wt._REG_OP_S(0,NULL,buf,buflen,_l(""));
  }
 else
  TglobalSettingsDec::getDefaultStr(id,buf,buflen);
}

void TglobalSettingsDecAudio::reg_op_codec(TregOp &t,TregOp *t2)
{
 _reg_op_codec(IDFF_wma7      ,t,t2,_l("wma1")      ,wma1      ,0);
 _reg_op_codec(IDFF_wma8      ,t,t2,_l("wma2")      ,wma2      ,0);
 _reg_op_codec(IDFF_mp2       ,t,t2,_l("mp2")       ,mp2       ,0);
 _reg_op_codec(IDFF_mp3       ,t,t2,_l("mp3")       ,mp3       ,0);
 _reg_op_codec(IDFF_ac3       ,t,t2,_l("ac3")       ,ac3       ,0);
 _reg_op_codec(IDFF_mlp       ,t,t2,_l("mlp")       ,mlp       ,0);
 _reg_op_codec(IDFF_eac3      ,t,t2,_l("eac3")      ,eac3      ,0);
 _reg_op_codec(IDFF_dts       ,t,t2,_l("dts")       ,dts       ,0);
 _reg_op_codec(IDFF_aac       ,t,t2,_l("aac")       ,aac       ,0);
 _reg_op_codec(IDFF_amr       ,t,t2,_l("amr")       ,amr       ,0);
 _reg_op_codec(IDFF_iadpcm    ,t,t2,_l("iadpcm")    ,iadpcm    ,0);
 _reg_op_codec(IDFF_msadpcm   ,t,t2,_l("msadpcm")   ,msadpcm   ,0);
 _reg_op_codec(IDFF_otherAdpcm,t,t2,_l("otherAdpcm"),otherAdpcm,0);
 _reg_op_codec(IDFF_law       ,t,t2,_l("law")       ,law       ,0);
 _reg_op_codec(IDFF_gsm       ,t,t2,_l("gsm")       ,gsm       ,0);
 _reg_op_codec(IDFF_lpcm      ,t,t2,_l("lpcm")      ,lpcm      ,0);
 _reg_op_codec(IDFF_flac      ,t,t2,_l("flac")      ,flac      ,0);
 _reg_op_codec(IDFF_tta       ,t,t2,_l("tta")       ,tta       ,0);
 _reg_op_codec(IDFF_qdm2      ,t,t2,_l("qdm2")      ,qdm2      ,0);
 _reg_op_codec(IDFF_mace      ,t,t2,_l("mace")      ,mace      ,0);
 _reg_op_codec(IDFF_truespeech,t,t2,_l("truespeech"),truespeech,0);
 _reg_op_codec(IDFF_vorbis    ,t,t2,_l("vorbis")    ,vorbis    ,0);
 _reg_op_codec(IDFF_ra        ,t,t2,_l("ra")        ,ra        ,0);
 _reg_op_codec(IDFF_imc       ,t,t2,_l("imc")       ,imc       ,0);
 _reg_op_codec(IDFF_atrac3    ,t,t2,_l("atrac3")    ,atrac3    ,0);
 _reg_op_codec(IDFF_nellymoser,t,t2,_l("nellymoser"),nellymoser,0);
 _reg_op_codec(IDFF_avisA     ,t,t2,_l("avis")      ,avis      ,IDFF_MOVIE_AVIS);
 _reg_op_codec(IDFF_rawa      ,t,t2,filtermode&IDFF_FILTERMODE_AUDIORAW?_l("raw_rawa"):_l("rawa"),rawa,filtermode&IDFF_FILTERMODE_AUDIORAW?IDFF_MOVIE_RAW:0);
}
void TglobalSettingsDecAudio::load(void)
{
 TglobalSettingsDec::load();

 fixMissing(wma1      ,IDFF_MOVIE_LAVC);
 fixMissing(wma2      ,IDFF_MOVIE_LAVC);
 fixMissing(amr       ,IDFF_MOVIE_LAVC);
 fixMissing(ac3       ,IDFF_MOVIE_LIBA52,IDFF_MOVIE_LAVC);
 fixMissing(eac3      ,IDFF_MOVIE_LAVC);
 fixMissing(dts       ,IDFF_MOVIE_LIBDTS,IDFF_MOVIE_LAVC);
 fixMissing(mp2       ,IDFF_MOVIE_MPLAYER,IDFF_MOVIE_LIBMAD,IDFF_MOVIE_LAVC);
 fixMissing(mp3       ,IDFF_MOVIE_MPLAYER,IDFF_MOVIE_LIBMAD,IDFF_MOVIE_LAVC);
 fixMissing(aac       ,IDFF_MOVIE_LIBFAAD,IDFF_MOVIE_REALAAC);
 fixMissing(avis      ,IDFF_MOVIE_AVIS);
 fixMissing(iadpcm    ,IDFF_MOVIE_LAVC);
 fixMissing(msadpcm   ,IDFF_MOVIE_LAVC);
 fixMissing(otherAdpcm,IDFF_MOVIE_LAVC);
 fixMissing(law       ,IDFF_MOVIE_LAVC);
 fixMissing(gsm       ,IDFF_MOVIE_LAVC);
 fixMissing(flac      ,IDFF_MOVIE_LAVC);
 fixMissing(tta       ,IDFF_MOVIE_LAVC);
 fixMissing(qdm2      ,IDFF_MOVIE_LAVC);
 fixMissing(mace      ,IDFF_MOVIE_LAVC);
 fixMissing(truespeech,IDFF_MOVIE_LAVC);
 fixMissing(vorbis    ,IDFF_MOVIE_LAVC,IDFF_MOVIE_TREMOR);
 fixMissing(ra        ,IDFF_MOVIE_LAVC);
 fixMissing(imc       ,IDFF_MOVIE_LAVC);
 fixMissing(atrac3    ,IDFF_MOVIE_LAVC);
 fixMissing(nellymoser,IDFF_MOVIE_LAVC);
}

#define FF_WAVE_FORMAT1_CODEC(format,decoder,codec) \
 case WAVE_FORMAT_##format:                         \
  codecId=(decoder)?codec:CODEC_ID_NONE;            \
  break;
#define FF_WAVE_FORMAT_CODEC(format,decoder,codecs) \
 case WAVE_FORMAT_##format:                         \
  codecId=codecs[decoder];                          \
  break;

#define AUDIO_FOURCCS \
 FF_WAVE_FORMAT1_OP(WMA7   ,wma1   & rawmask,CODEC_ID_WMAV1) \
 FF_WAVE_FORMAT1_OP(WMA8   ,wma2   & rawmask,CODEC_ID_WMAV2) \
 FF_WAVE_FORMAT_OP (MPEGLAYER3,mp3 & rawmask,c_mp123) \
 FF_WAVE_FORMAT_OP (MPEG   ,mp2    & rawmask,c_mp123) \
 FF_WAVE_FORMAT_OP (AC3_W  ,ac3    & rawmask,c_ac3) \
 FF_WAVE_FORMAT1_OP(EAC3   ,eac3   & rawmask,CODEC_ID_EAC3) \
 FF_WAVE_FORMAT_OP (DTS_W  ,dts    & rawmask,c_dts) \
 FF_WAVE_FORMAT_OP (AAC1   ,aac    & rawmask,c_aac) \
 FF_WAVE_FORMAT_OP (AAC2   ,aac    & rawmask,c_aac) \
 FF_WAVE_FORMAT_OP (AAC3   ,aac    & rawmask,c_aac) \
 FF_WAVE_FORMAT_OP (AAC4   ,aac    & rawmask,c_aac) \
 FF_WAVE_FORMAT_OP (AAC5   ,aac    & rawmask,c_aac) \
 FF_WAVE_FORMAT1_OP(AMR    ,amr    & rawmask,CODEC_ID_AMR_NB) \
 FF_WAVE_FORMAT1_OP(SAMR   ,amr    & rawmask,CODEC_ID_AMR_NB) \
 FF_WAVE_FORMAT1_OP(IMA_DK3,iadpcm & rawmask,CODEC_ID_ADPCM_IMA_DK3) \
 FF_WAVE_FORMAT1_OP(IMA_WAV,iadpcm & rawmask,CODEC_ID_ADPCM_IMA_WAV) \
 FF_WAVE_FORMAT1_OP(IMA_DK4,iadpcm & rawmask,CODEC_ID_ADPCM_IMA_DK4) \
 FF_WAVE_FORMAT1_OP(IMA4   ,iadpcm & rawmask,CODEC_ID_ADPCM_IMA_QT) \
 FF_WAVE_FORMAT1_OP(IMA_AMV,iadpcm & rawmask,CODEC_ID_ADPCM_IMA_AMV) \
 FF_WAVE_FORMAT1_OP(ADPCM  ,msadpcm& rawmask,CODEC_ID_ADPCM_MS) \
 FF_WAVE_FORMAT1_OP(CREATIVE_ADPCM,otherAdpcm& rawmask,CODEC_ID_ADPCM_CT) \
 FF_WAVE_FORMAT1_OP(YAMAHA_ADPCM  ,otherAdpcm& rawmask,CODEC_ID_ADPCM_YAMAHA) \
 FF_WAVE_FORMAT1_OP(G726          ,otherAdpcm& rawmask,CODEC_ID_ADPCM_G726) \
 FF_WAVE_FORMAT1_OP(ADPCM_SWF     ,otherAdpcm& rawmask,CODEC_ID_ADPCM_SWF) \
 FF_WAVE_FORMAT1_OP(ALAW   ,law    & rawmask,CODEC_ID_PCM_ALAW) \
 FF_WAVE_FORMAT1_OP(MULAW  ,law    & rawmask,CODEC_ID_PCM_MULAW) \
 /*FF_WAVE_FORMAT1_OP(QT_ULAW,law    & rawmask,CODEC_ID_PCM_MULAW)*/ \
 FF_WAVE_FORMAT1_OP(GSM610 ,gsm    & rawmask,CODEC_ID_GSM_MS) \
 /*FF_WAVE_FORMAT1_OP(QT_GSM ,gsm    & rawmask,CODEC_ID_GSM_MS)*/ \
 FF_WAVE_FORMAT1_OP(FLAC   ,flac   & rawmask,CODEC_ID_FLAC) \
 FF_WAVE_FORMAT1_OP(TTA    ,tta    & rawmask,CODEC_ID_TTA) \
 FF_WAVE_FORMAT1_OP(QDM2   ,qdm2   & rawmask,CODEC_ID_QDM2) \
 FF_WAVE_FORMAT1_OP(MAC3   ,mace   & rawmask,CODEC_ID_MACE3) \
 FF_WAVE_FORMAT1_OP(MAC6   ,mace   & rawmask,CODEC_ID_MACE6) \
 FF_WAVE_FORMAT1_OP(DSPGROUP_TRUESPEECH,truespeech & rawmask,CODEC_ID_TRUESPEECH) \
 /*FF_WAVE_FORMAT1_OP(COOK   ,1   & rawmask,CODEC_ID_COOK)*/ \
 FF_WAVE_FORMAT1_OP(14_4  ,ra & rawmask,CODEC_ID_RA_144) \
 FF_WAVE_FORMAT1_OP(28_8  ,ra & rawmask,CODEC_ID_RA_288) \
 FF_WAVE_FORMAT1_OP(IMC  ,imc & rawmask,CODEC_ID_IMC) \
 FF_WAVE_FORMAT1_OP(ATRAC3  ,atrac3 & rawmask,CODEC_ID_ATRAC3) \
 FF_WAVE_FORMAT1_OP(NELLYMOSER,nellymoser & rawmask,CODEC_ID_NELLYMOSER) \
 FF_WAVE_FORMAT_OP (VORBIS  ,vorbis & rawmask,c_vorbis) \
 FF_WAVE_FORMAT_OP (FFVORBIS,vorbis & rawmask,c_vorbis) \
 /*FF_WAVE_FORMAT1_OP(VORBIS1 ,vorbis& rawmask,CODEC_ID_TREMOR)*/ \
 /*FF_WAVE_FORMAT1_OP(VORBIS2 ,vorbis& rawmask,CODEC_ID_TREMOR)*/ \
 /*FF_WAVE_FORMAT1_OP(VORBIS3 ,vorbis& rawmask,CODEC_ID_TREMOR)*/ \
 /*FF_WAVE_FORMAT1_OP(VORBIS1P,vorbis& rawmask,CODEC_ID_TREMOR)*/ \
 /*FF_WAVE_FORMAT1_OP(VORBIS2P,vorbis& rawmask,CODEC_ID_TREMOR)*/ \
 /*FF_WAVE_FORMAT1_OP(VORBIS3P,vorbis& rawmask,CODEC_ID_TREMOR)*/ \
 FF_WAVE_FORMAT1_OP(LPCM   ,lpcm   & rawmask,CODEC_ID_LPCM) \
 FF_WAVE_FORMAT1_OP(AVIS   ,avis   & rawmask,CODEC_ID_AVISYNTH) \
 FF_WAVE_FORMAT1_OP(PCM8   ,rawa==IDFF_MOVIE_RAW || rawa==(TsampleFormat::SF_ALLINT<<8) || rawa==(TsampleFormat::SF_PCM8 <<8),CODEC_ID_PCM) \
 FF_WAVE_FORMAT1_OP(PCM16  ,rawa==IDFF_MOVIE_RAW || rawa==(TsampleFormat::SF_ALLINT<<8) || rawa==(TsampleFormat::SF_PCM16<<8),CODEC_ID_PCM) \
 FF_WAVE_FORMAT1_OP(PCM24  ,rawa==IDFF_MOVIE_RAW || rawa==(TsampleFormat::SF_ALLINT<<8) || rawa==(TsampleFormat::SF_PCM24<<8),CODEC_ID_PCM) \
 FF_WAVE_FORMAT1_OP(PCM32  ,rawa==IDFF_MOVIE_RAW || rawa==(TsampleFormat::SF_ALLINT<<8) || rawa==(TsampleFormat::SF_PCM32<<8),CODEC_ID_PCM) \
 FF_WAVE_FORMAT1_OP(FLOAT32,rawa==IDFF_MOVIE_RAW || rawa==(TsampleFormat::SF_FLOAT32<<8) || rawa==(TsampleFormat::SF_ALLFLOAT<<8),CODEC_ID_PCM) \
 FF_WAVE_FORMAT1_OP(FLOAT64,rawa==IDFF_MOVIE_RAW || rawa==(TsampleFormat::SF_FLOAT64<<8) || rawa==(TsampleFormat::SF_ALLFLOAT<<8),CODEC_ID_PCM) \
 FF_WAVE_FORMAT1_OP (MLP   ,mlp    & rawmask,CODEC_ID_MLP)

const char_t *TglobalSettingsDecAudio::wave_formats[]=
{
 #undef FF_WAVE_FORMAT_OP
 #undef FF_WAVE_FORMAT1_OP
 #define FF_WAVE_FORMAT_OP  FF_FOURCC_LIST
 #define FF_WAVE_FORMAT1_OP FF_FOURCC_LIST
 AUDIO_FOURCCS
 NULL
};
const char_t** TglobalSettingsDecAudio::getFOURCClist(void) const
{
 return wave_formats;
}

const CodecID TglobalSettingsDecAudio::c_mp123[IDFF_MOVIE_MAX+1]=
{
 CODEC_ID_NONE,
 CODEC_ID_MP3,
 CODEC_ID_NONE,
 CODEC_ID_NONE,
 CODEC_ID_NONE,
 CODEC_ID_NONE,
 CODEC_ID_MP3LIB,
 CODEC_ID_LIBMAD,
 CODEC_ID_NONE,
 CODEC_ID_NONE,
 CODEC_ID_NONE,
 CODEC_ID_NONE,
 CODEC_ID_NONE,
 CODEC_ID_NONE,
 CODEC_ID_NONE,
 CODEC_ID_NONE,
 CODEC_ID_NONE,
 CODEC_ID_NONE,
 CODEC_ID_NONE,
 CODEC_ID_NONE,
 CODEC_ID_AUDX
};
const CodecID TglobalSettingsDecAudio::c_ac3[IDFF_MOVIE_MAX+1]=
{
 CODEC_ID_NONE,
 CODEC_ID_AC3,
 CODEC_ID_NONE,
 CODEC_ID_NONE,
 CODEC_ID_NONE,
 CODEC_ID_NONE,
 CODEC_ID_NONE,
 CODEC_ID_NONE,
 CODEC_ID_NONE,
 CODEC_ID_NONE,
 CODEC_ID_NONE,
 CODEC_ID_NONE,
 CODEC_ID_NONE,
 CODEC_ID_NONE,
 CODEC_ID_NONE,
 CODEC_ID_LIBA52,
 CODEC_ID_SPDIF_AC3
};
const CodecID TglobalSettingsDecAudio::c_dts[IDFF_MOVIE_MAX+1]=
{
 CODEC_ID_NONE,
 CODEC_ID_DTS,
 CODEC_ID_NONE,
 CODEC_ID_NONE,
 CODEC_ID_NONE,
 CODEC_ID_NONE,
 CODEC_ID_NONE,
 CODEC_ID_NONE,
 CODEC_ID_NONE,
 CODEC_ID_NONE,
 CODEC_ID_NONE,
 CODEC_ID_NONE,
 CODEC_ID_NONE,
 CODEC_ID_NONE,
 CODEC_ID_NONE,
 CODEC_ID_NONE,
 CODEC_ID_SPDIF_DTS,
 CODEC_ID_LIBDTS
};
const CodecID TglobalSettingsDecAudio::c_aac[IDFF_MOVIE_MAX+1]=
{
 CODEC_ID_NONE,
 CODEC_ID_AAC,
 CODEC_ID_NONE,
 CODEC_ID_NONE,
 CODEC_ID_NONE,
 CODEC_ID_NONE,
 CODEC_ID_NONE,
 CODEC_ID_NONE,
 CODEC_ID_LIBFAAD,
 CODEC_ID_NONE,
 CODEC_ID_NONE,
 CODEC_ID_NONE,
 CODEC_ID_NONE,
 CODEC_ID_NONE,
 CODEC_ID_NONE,
 CODEC_ID_NONE,
 CODEC_ID_NONE,
 CODEC_ID_NONE,
 CODEC_ID_NONE,
 CODEC_ID_REALAAC
};
const CodecID TglobalSettingsDecAudio::c_vorbis[IDFF_MOVIE_MAX+1]=
{
 CODEC_ID_NONE,
 CODEC_ID_VORBIS,
 CODEC_ID_NONE,
 CODEC_ID_NONE,
 CODEC_ID_NONE,
 CODEC_ID_NONE,
 CODEC_ID_NONE,
 CODEC_ID_NONE,
 CODEC_ID_NONE,
 CODEC_ID_NONE,
 CODEC_ID_NONE,
 CODEC_ID_NONE,
 CODEC_ID_NONE,
 CODEC_ID_NONE,
 CODEC_ID_NONE,
 CODEC_ID_NONE,
 CODEC_ID_NONE,
 CODEC_ID_NONE,
 CODEC_ID_TREMOR
};

void TglobalSettingsDecAudio::getCodecsList(Tstrptrs &codecs) const
{
 std::vector<CodecID> ids;
 #undef FF_WAVE_FORMAT_OP
 #undef FF_WAVE_FORMAT1_OP
 #define FF_WAVE_FORMAT_OP(format,decoder,codecs) for (int j=0;j<=IDFF_MOVIE_MAX;j++) if (codecs[j]!=CODEC_ID_NONE) ids.push_back(codecs[j]);
 #define FF_WAVE_FORMAT1_OP(format,decoder,codec) ids.push_back(codec);
 AUDIO_FOURCCS
 cleanupCodecsList(ids,codecs);
}

CodecID TglobalSettingsDecAudio::getCodecId(DWORD fourCC,FOURCC*) const
{
 CodecID codecId=CODEC_ID_NONE;
 int rawmask=filtermode&IDFF_FILTERMODE_AUDIORAW?0:0xffff;
 switch(fourCC)
  {
   #undef FF_WAVE_FORMAT_OP
   #undef FF_WAVE_FORMAT1_OP
   #define FF_WAVE_FORMAT_OP FF_WAVE_FORMAT_CODEC
   #define FF_WAVE_FORMAT1_OP FF_WAVE_FORMAT1_CODEC
   AUDIO_FOURCCS
   default:return CODEC_ID_NONE;
  }
 return codecId;
}

//==================================== TglobalSettingsEnc ====================================
TglobalSettingsEnc::TglobalSettingsEnc(const Tconfig *Iconfig,int Imode,TintStrColl *Icoll):TglobalSettingsBase(Iconfig,Imode,FFDSHOWENC,Icoll)
{
 static const TintOptionT<TglobalSettingsEnc> iopts[]=
  {
   IDFF_enc_psnr         ,&TglobalSettingsEnc::psnr             ,0,0,_l(""),0,
     _l("psnr"),0,
   IDFF_isDyInterlaced   ,&TglobalSettingsEnc::isDyInterlaced   ,0,0,_l(""),0,
     _l("isDyInterlaced"),0,
   IDFF_dyInterlaced     ,&TglobalSettingsEnc::dyInterlaced     ,0,4096,_l(""),0,
     _l("dyInterlaced"),288,
   0
  };
 addOptions(iopts);
}
int TglobalSettingsEnc::getDefault(int id)
{
 switch (id)
  {
   case IDFF_trayIcon:return 0;
   case IDFF_multipleInstances:return 0;
   default:return TglobalSettingsBase::getDefault(id);
  }
}
