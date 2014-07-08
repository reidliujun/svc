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
#include "TdialogSettings.h"
#include "resource.h"
#include "ffdshow_constants.h"
#include "Tconfig.h"
#include "Twindow.h"

//===================================== TdialogSettingsBase =====================================
TdialogSettingsBase::TdialogSettingsBase(const char_t *Ireg_child,TintStrColl *Icoll):reg_child(Ireg_child),loaded(false),Toptions(Icoll)
{
 static const TintOptionT<TdialogSettingsBase> iopts[]=
  {
   IDFF_lastPage           ,&TdialogSettingsBase::mainLastPage    ,1,1,_l(""),0,
     _l("lastPage"),TintOption::DEF_DYN,
   IDFF_dlgRestorePos      ,&TdialogSettingsBase::mainIsRestorePos,0,0,_l(""),0,
     _l("dlgRestorePos"),0,
   IDFF_dlgPosX            ,&TdialogSettingsBase::mainPosX        ,-4096,4096,_l(""),0,
     _l("dlgPosX"),0,
   IDFF_dlgPosY            ,&TdialogSettingsBase::mainPosY        ,-4096,4096,_l(""),0,
     _l("dlgPosY"),0,
   IDFF_showHints          ,&TdialogSettingsBase::showHints       ,0,0,_l(""),0,
     _l("showHints"),1,
   IDFF_lvCodecsWidth0     ,&TdialogSettingsBase::codecsLvWidth0  ,1,2048,_l(""),0,
     _l("lvCodecsWidth0"),TintOption::DEF_DYN,
   IDFF_lvCodecsWidth1     ,&TdialogSettingsBase::codecsLvWidth1  ,1,2048,_l(""),0,
     _l("lvCodecsWidth1"),TintOption::DEF_DYN,
   IDFF_lvCodecsWidth2     ,&TdialogSettingsBase::codecsLvWidth2  ,1,2048,_l(""),0,
     _l("lvCodecsWidth2"),TintOption::DEF_DYN,
   IDFF_lvCodecsSelected   ,&TdialogSettingsBase::codecsLastSel   ,0,1000,_l(""),0,
     _l("lvCodecsSelected"),0,
   0
  };
 addOptions(iopts);
 static const TstrOption sopts[]=
  {
   IDFF_lang               ,(TstrVal)&TdialogSettingsBase::lang   ,MAX_PATH,0,_l(""),0,
     NULL,NULL,
   0
  };
 addOptions(sopts);
}
void TdialogSettingsBase::reg_op(TregOp &t)
{
 Toptions::reg_op(t);
 t._REG_OP_N(0,_l("translateMode"),translate,0);
}

void TdialogSettingsBase::load(const Tconfig *config)
{
 loaded=true;
 char_t key[MAX_PATH];
 tsnprintf_s(key, countof(key), _TRUNCATE, FFDSHOW_REG_PARENT _l("\\%s"), reg_child);
 TregOpRegRead t(HKEY_CURRENT_USER,key);
 reg_op(t);
 getUILanguage(config->pth,lang);
}
void TdialogSettingsBase::save(void)
{
 char_t key[MAX_PATH];
 tsnprintf_s(key, countof(key), _TRUNCATE, FFDSHOW_REG_PARENT _l("\\%s"), reg_child);
 TregOpRegWrite t(HKEY_CURRENT_USER,key);
 reg_op(t);

 TregOpRegWrite tl(HKEY_CURRENT_USER,FFDSHOW_REG_PARENT _l("\\") FFDSHOW);
 tl._REG_OP_S(IDFF_lang,_l("lang"),lang,20,_l("en"));
}

//===================================== TdialogSettingsDec ======================================
TdialogSettingsDec::TdialogSettingsDec(const char_t *Ireg_child,TintStrColl *Icoll):TdialogSettingsBase(Ireg_child,Icoll)
{
 static const TintOptionT<TdialogSettingsDec> iopts[]=
  {
   IDFF_lvWidth0        ,&TdialogSettingsDec::presetsLvWidth0  ,1,2048,_l(""),0,
     _l("lvWidth0"),300,
   IDFF_lvKeysWidth0     ,&TdialogSettingsDec::keysLvWidth0    ,1,2048,_l(""),0,
     _l("lvKeysWidth0"),240,
   IDFF_lvKeysWidth1     ,&TdialogSettingsDec::keysLvWidth1    ,1,2048,_l(""),0,
     _l("lvKeysWidth1"),90,
   IDFF_dlgDecCurrentPage,&TdialogSettingsDec::currentPage     ,0,1,_l(""),0,
     _l("dlgDecCurrentPage"),0,
   0
  };
 addOptions(iopts);
}

//==================================== TdialogSettingsDecVideo =====================================
TdialogSettingsDecVideo::TdialogSettingsDecVideo(bool vfwdec,TintStrColl *Icoll):TdialogSettingsDec(vfwdec?FFDSHOWDECVIDEOVFW:FFDSHOWDECVIDEO,Icoll)
{
 static const TintOptionT<TdialogSettingsDecVideo> iopts[]=
  {
   IDFF_dlgCustColor0 ,&TdialogSettingsDecVideo::dlgCustColor0 ,1,1,_l(""),0,
     _l("dlgCustColor0") ,RGB( 0*16, 0*16, 0*16),
   IDFF_dlgCustColor1 ,&TdialogSettingsDecVideo::dlgCustColor1 ,1,1,_l(""),0,
     _l("dlgCustColor1") ,RGB( 1*16, 1*16, 1*16),
   IDFF_dlgCustColor2 ,&TdialogSettingsDecVideo::dlgCustColor2 ,1,1,_l(""),0,
     _l("dlgCustColor2") ,RGB( 2*16, 2*16, 2*16),
   IDFF_dlgCustColor3 ,&TdialogSettingsDecVideo::dlgCustColor3 ,1,1,_l(""),0,
     _l("dlgCustColor3") ,RGB( 3*16, 3*16, 3*16),
   IDFF_dlgCustColor4 ,&TdialogSettingsDecVideo::dlgCustColor4 ,1,1,_l(""),0,
     _l("dlgCustColor4") ,RGB( 4*16, 4*16, 4*16),
   IDFF_dlgCustColor5 ,&TdialogSettingsDecVideo::dlgCustColor5 ,1,1,_l(""),0,
     _l("dlgCustColor5") ,RGB( 5*16, 5*16, 5*16),
   IDFF_dlgCustColor6 ,&TdialogSettingsDecVideo::dlgCustColor6 ,1,1,_l(""),0,
     _l("dlgCustColor6") ,RGB( 6*16, 6*16, 6*16),
   IDFF_dlgCustColor7 ,&TdialogSettingsDecVideo::dlgCustColor7 ,1,1,_l(""),0,
     _l("dlgCustColor7") ,RGB( 7*16, 7*16, 7*16),
   IDFF_dlgCustColor8 ,&TdialogSettingsDecVideo::dlgCustColor8 ,1,1,_l(""),0,
     _l("dlgCustColor8") ,RGB( 8*16, 8*16, 8*16),
   IDFF_dlgCustColor9 ,&TdialogSettingsDecVideo::dlgCustColor9 ,1,1,_l(""),0,
     _l("dlgCustColor9") ,RGB( 9*16, 9*16, 9*16),
   IDFF_dlgCustColor10,&TdialogSettingsDecVideo::dlgCustColor10,1,1,_l(""),0,
     _l("dlgCustColor10"),RGB(10*16,10*16,10*16),
   IDFF_dlgCustColor11,&TdialogSettingsDecVideo::dlgCustColor11,1,1,_l(""),0,
     _l("dlgCustColor11"),RGB(11*16,11*16,11*16),
   IDFF_dlgCustColor12,&TdialogSettingsDecVideo::dlgCustColor12,1,1,_l(""),0,
     _l("dlgCustColor12"),RGB(12*16,12*16,12*16),
   IDFF_dlgCustColor13,&TdialogSettingsDecVideo::dlgCustColor13,1,1,_l(""),0,
     _l("dlgCustColor13"),RGB(13*16,13*16,13*16),
   IDFF_dlgCustColor14,&TdialogSettingsDecVideo::dlgCustColor14,1,1,_l(""),0,
     _l("dlgCustColor14"),RGB(14*16,14*16,14*16),
   IDFF_dlgCustColor15,&TdialogSettingsDecVideo::dlgCustColor15,1,1,_l(""),0,
     _l("dlgCustColor15"),RGB(  255,  255,  255),
   0
  };
 addOptions(iopts);
}

int TdialogSettingsDecVideo::getDefault(int id)
{
 switch (id)
  {
   case IDFF_lastPage:return IDD_POSTPROC;
   case IDFF_lvCodecsWidth0:return 90;
   case IDFF_lvCodecsWidth1:return 76;
   case IDFF_lvCodecsWidth2:return 202;
   default:return TdialogSettingsDec::getDefault(id);
  }
}

//==================================== TdialogSettingsDecAudio =====================================
TdialogSettingsDecAudio::TdialogSettingsDecAudio(TintStrColl *Icoll,const char_t *Ireg_child):TdialogSettingsDec(Ireg_child,Icoll)
{
 static const TintOptionT<TdialogSettingsDecAudio> iopts[]=
  {
   IDFF_lvConvolverWidth0     ,&TdialogSettingsDecAudio::convolverLvWidth0,1,2048,_l(""),0,
     _l("lvConvolverWidth0"),70,
   IDFF_lvConvolverWidth1     ,&TdialogSettingsDecAudio::convolverLvWidth1,1,2048,_l(""),0,
     _l("lvConvolverWidth1"),200,
   IDFF_lvConvolverWidth2     ,&TdialogSettingsDecAudio::convolverLvWidth2,1,2048,_l(""),0,
     _l("lvConvolverWidth2"),300,
   IDFF_lvConvolverSelected   ,&TdialogSettingsDecAudio::convolverLvSelected,0,5,_l(""),0,
     _l("lvConvolverSelected"),0,

   IDFF_dlgVolumeDb           ,&TdialogSettingsDecAudio::volumeDb,0,0,_l(""),0,
     _l("dlgVolumeDb"),0,
   0
  };
 addOptions(iopts);
}
int TdialogSettingsDecAudio::getDefault(int id)
{
 switch (id)
  {
   case IDFF_lastPage:return IDD_CODECS;
   case IDFF_lvCodecsWidth0:return 100;
   case IDFF_lvCodecsWidth1:return 100;
   case IDFF_lvCodecsWidth2:return 168;
   default:return TdialogSettingsDec::getDefault(id);
  }
}

//===================================== TdialogSettingsEnc ======================================
TdialogSettingsEnc::TdialogSettingsEnc(TintStrColl *Icoll):TdialogSettingsBase(FFDSHOWENC,Icoll)
{
 static const TintOptionT<TdialogSettingsEnc> iopts[]=
  {
   IDFF_enc_showGraph    ,&TdialogSettingsEnc::showGraph   ,0,0,_l(""),0,
     _l("showGraph"),0,
   IDFF_dlgBpsFps1000    ,&TdialogSettingsEnc::bpsFps1000 ,1,1,_l(""),0,
     _l("bpsDlgFPS1000"),25000,
   IDFF_dlgBpsLen        ,&TdialogSettingsEnc::bpsLen     ,1,1,_l(""),0,
     _l("bpsDlgLen"),590000,
   IDFF_dlgPerfectDlgX   ,&TdialogSettingsEnc::perfectDlgX,1,1,_l(""),0,
     _l("perfectDlgX"),-1,
   IDFF_dlgPerfectDlgY   ,&TdialogSettingsEnc::perfectDlgY,1,1,_l(""),0,
     _l("perfectDlgY"),-1,
   IDFF_dlgEncCurrentPage,&TdialogSettingsEnc::currentPage,0,1,_l(""),0,
     _l("dlgEncCurrentPage"),0,
   0
  };
 addOptions(iopts);
}

int TdialogSettingsEnc::getDefault(int id)
{
 switch (id)
  {
   case IDFF_lastPage:return IDD_GENERIC;
   case IDFF_lvCodecsWidth0:return 100;
   case IDFF_lvCodecsWidth1:return 150;
   case IDFF_lvCodecsWidth2:return 50;
   default:return TdialogSettingsBase::getDefault(id);
  }
}

void getUILanguage(const char_t* pth, char_t* lang)
{
 TregOpRegRead tl(HKEY_CURRENT_USER,FFDSHOW_REG_PARENT _l("\\") FFDSHOW);
 tl._REG_OP_S(IDFF_lang,_l("lang"),lang,20,_l(""));
 if (lang[0]=='\0')
  {
   TregOpRegRead tNSI(HKEY_LOCAL_MACHINE,FFDSHOW_REG_PARENT _l("\\") FFDSHOW);
   char_t langId[MAX_PATH];
   tNSI._REG_OP_S(0,_l("lang"),langId,MAX_PATH,_l("1033"));
   char_t langfilesmask[MAX_PATH];
   tsnprintf_s(langfilesmask, countof(langfilesmask), _TRUNCATE, _l("%s.*"), langId);
   char_t lngMask[MAX_PATH];_makepath_s(lngMask,MAX_PATH,NULL,pth,_l("languages\\ffdshow"),langfilesmask);
   strings langfiles;
   findFiles(lngMask,langfiles,false);
   if (langfiles.size()==0)
    strcpy(lang,_l("en"));
   else
    extractfileext(langfiles[0].c_str(),lang);
  }
 if ((lang[0]=='J' || lang[0]=='j') && (lang[1]=='A' || lang[1]=='P' || lang[1]=='a'|| lang[1]=='p')) /* Japanese ANSI or Unicode auto fix */
  {
#ifdef UNICODE
   lang[0]='J';
   lang[1]='A';// JA Unicode
#else
   lang[0]='J';
   lang[1]='P';// JP ANSI
#endif
  }
}

bool getpth(char_t* pth)
{
 char_t sysdir[MAX_PATH];GetSystemDirectory(sysdir,MAX_PATH);
 TregOpRegRead t(HKEY_LOCAL_MACHINE,FFDSHOW_REG_PARENT _l("\\") FFDSHOW);
 t._REG_OP_S(0,_l("pthPriority"),pth,MAX_PATH,_l(""));
 if (pth[0]=='\0')
  {
   t._REG_OP_S(0,_l("pth"),pth,MAX_PATH,sysdir);
   return true;
  }
 else return false;
}
