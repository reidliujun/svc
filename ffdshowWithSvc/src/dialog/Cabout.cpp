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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "stdafx.h"
#include "Cabout.h"
#include "TffdshowPageBase.h"
#include "Tlibavcodec.h"
#include "Tlibmplayer.h"
#include "avisynth/Tavisynth.h"
#include "TvideoCodecXviD4.h"
#include "TvideoCodecLibmpeg2.h"
#include "TvideoCodecSkal.h"
#include "TvideoCodecTheora.h"
#define TREMOR_OGG_H // FIXME (dirty) This is necessary to avoid including various versions of ogg.h.
#define OGG_OGG_H
#include "TvideoCodecWmv9.h"
#include "TvideoCodecX264.h"
#include "TaudioCodecLibMAD.h"
#include "TaudioCodecLibFAAD.h"
#include "TaudioCodecLiba52.h"
#include "TaudioCodecLibDTS.h"
#include "TaudioCodecTremor.h"
#include "TaudioCodecAudX.h"
#include "TimgFilterTomsMoComp.h"
#include "TimgFilterKernelDeint.h"
#include "TaudioFilterResample.h"
#include "TaudioCodecRealaac.h"
#include "Tstream.h"
#include "ffdebug.h"
#include "Ttranslate.h"

void TaboutPage::init(void)
{
 //addHint(IDC_BT_DONATE,"Thank you!");
 if (!lic)
  {
   lic=loadText(IDD_LICENSE);
   if (lic)
    {
     setDlgItemText(m_hwnd,IDC_ED_LICENSE,lic);
     SendDlgItemMessage(m_hwnd,IDC_ED_LICENSE,EM_SETSEL,0,-1);
    }
  }
}

void TaboutPage::exportReg(Twindow *w,char_t *regflnm)
{
 DWORD filterIndex=sizeof(char)==sizeof(char_t)?1:2;;
 if (dlgGetFile(true,w->m_hwnd,w->_(-IDD_ABOUT,_l("Export ffdshow settings")),_l("Win9x/NT4 Registration Files\0*.reg\0Unicode Registration files\0*.reg\0"),_l("reg"),regflnm,_l("."),0,&filterIndex))
  if (w->deci->exportRegSettings(true,regflnm,filterIndex==2?1:0)==S_OK)
   w->msg(w->_(-IDD_ABOUT,_l("Settings have been succefully exported")));
  else
   w->err(w->_(-IDD_ABOUT,_l("Settings export has failed")));
}
void TaboutPage::onExport(void)
{
 parent->OnApplyChanges();
 exportReg(this,regflnm);
}
void TaboutPage::onLicense(void)
{
 int is=getCheck(IDC_CHB_LICENSE);
 show(!is,IDC_ED_HELP);
 show(is,IDC_ED_LICENSE);
}

template<class T> bool TaboutPage::getVersion(const Tconfig *config,ffstring &vers,ffstring &license)
{
 Tdll *dl=new Tdll(T::dllname,config);
 void (__stdcall *getVersion)(char *ver,const char* *license);
 dl->loadFunction(getVersion,"getVersion");
 bool res=false;
 if (getVersion)
  {
   res=true;
   char ver[256];const char *lic;
   getVersion(ver,&lic);
   vers=ver;
   license=lic;
  }
 else
  {
   vers="not found";
   license.clear();
  }
 delete dl;
 return res;
}

void TaboutPage::onAbout(void)
{
 TversionsDlg verdlg(deci,m_hwnd,fcsVersions());
}
void TaboutPage::onDonate(void)
{
 ShellExecute(m_hwnd,_l("open"),_l("http://order.kagi.com/?6FAEY"),NULL,_l("."),SW_SHOWNORMAL);
}

TaboutPage::TaboutPage(TffdshowPageBase *Iparent):TconfPageBase(Iparent)
{
 dialogId=IDD_ABOUT;
 helpURL=_l("http://ffdshow-tryout.wiki.sourceforge.net/Credits");
 strcpy(regflnm,_l("ffdshow.reg"));
 static const TbindButton<TaboutPage> bt[]=
  {
   IDC_BT_EXPORT,&TaboutPage::onExport,
   //IDC_LBL_FFDSHOWVERSION,&TaboutPage::onAbout,
   IDC_BT_FFDSHOWVERSION,&TaboutPage::onAbout,
   IDC_BT_DONATE,&TaboutPage::onDonate,
   IDC_CHB_LICENSE,&TaboutPage::onLicense,
   0,NULL
  };
 bindButtons(bt);
 lic=NULL;
}
TaboutPage::~TaboutPage()
{
 if (lic) free(lic);
}
void TaboutPage::translate(void)
{
 TconfPageBase::translate();
 setText(IDC_LBL_FFDSHOW,_(IDC_LBL_FFDSHOW,capt));
 const char_t *help= tr->translate(aboutStringID);
 if(help[0]==0)
  loadHelpStr(helpId);
 else
  {
   setDlgItemText(m_hwnd,IDC_ED_HELP,help);
   SendDlgItemMessage(m_hwnd,IDC_ED_HELP,EM_SETSEL,0,-1);
  }
 int revision;
 TregOpRegRead tNSI(HKEY_LOCAL_MACHINE,FFDSHOW_REG_PARENT _l("\\") FFDSHOW);
 tNSI._REG_OP_N(0,_l("revision"),revision,0);
 if(revision)
  setText(IDC_LBL_FFDSHOWVERSION,_l("ffdshow tryouts revision %d  %s"),revision,FFDSHOW_VER);
 else
  setText(IDC_LBL_FFDSHOWVERSION,_l("%s %s"),_(IDC_LBL_FFDSHOWVERSION),FFDSHOW_VER);
}

TaboutPageDecVideo::TaboutPageDecVideo(TffdshowPageBase *Iparent):TaboutPage(Iparent)
{
 capt=_l("ffdshow video decoder");
 helpId=IDD_ABOUT;
 aboutStringID=IDD_README_VIDEO;
}

const TversionsDlg::TversionInfo* TaboutPageDecVideo::fcsVersions(void)
{
 static const TversionsDlg::TversionInfo fcsDecVideo[]=
  {
   _l("libavcodec.dll"),Tlibavcodec::getVersion,
   Tlibmplayer::dllname,getVersion<Tlibmplayer>,
   TvideoCodecXviD4::dllname,TvideoCodecXviD4::getVersion,
   TvideoCodecTheora::dllname,getVersion<TvideoCodecTheora>,
   TvideoCodecLibmpeg2::dllname,getVersion<TvideoCodecLibmpeg2>,
   TvideoCodecWmv9::dllname,getVersion<TvideoCodecWmv9>,
   Tavisynth_c::dllname,Tavisynth_c::getVersion,
   TimgFilterTomsMoComp::dllname,getVersion<TimgFilterTomsMoComp>,
   TimgFilterKernelDeint2::dllname,getVersion<TimgFilterKernelDeint2>,
   TstreamRAR::dllname,getVersion<TstreamRAR>,
   NULL
  };
 return fcsDecVideo;
}

TaboutPageDecAudio::TaboutPageDecAudio(TffdshowPageBase *Iparent):TaboutPage(Iparent)
{
 capt=_l("ffdshow audio decoder");
 helpId=IDD_ABOUTAUDIO;
 aboutStringID=IDD_README_AUDIO;
}
const TversionsDlg::TversionInfo* TaboutPageDecAudio::fcsVersions(void)
{
 static const TversionsDlg::TversionInfo fcsDecAudio[]=
  {
   _l("libavcodec.dll"),Tlibavcodec::getVersion,
   Tlibmplayer::dllname,getVersion<Tlibmplayer>,
   TaudioCodecLibMAD::dllname,getVersion<TaudioCodecLibMAD>,
   TaudioCodecLibFAAD::dllname,getVersion<TaudioCodecLibFAAD>,
   TaudioCodecRealaac::dllname,getVersion<TaudioCodecRealaac>,
   TaudioCodecLiba52::dllname,getVersion<TaudioCodecLiba52>,
   TaudioCodecLibDTS::dllname,getVersion<TaudioCodecLibDTS>,
   TaudioCodecTremor::dllname,getVersion<TaudioCodecTremor>,
   TaudioCodecAudX::dllname,TaudioCodecAudX::getVersion,
   TaudioFilterResampleSRC::dllname,getVersion<TaudioFilterResampleSRC>,
   Tavisynth_c::dllname,Tavisynth_c::getVersion,
   NULL
  };
 return fcsDecAudio;
}

TaboutPageEnc::TaboutPageEnc(TffdshowPageBase *Iparent):TaboutPage(Iparent)
{
 capt=_l("ffdshow video encoder");
 helpId=IDD_ABOUTENC;
 aboutStringID=IDD_README_ENC;
}
const TversionsDlg::TversionInfo* TaboutPageEnc::fcsVersions(void)
{
 static const TversionsDlg::TversionInfo fcsEnc[]=
  {
   _l("libavcodec.dll"),Tlibavcodec::getVersion,
   Tlibmplayer::dllname,getVersion<Tlibmplayer>,
   TvideoCodecXviD4::dllname,TvideoCodecXviD4::getVersion,
   TvideoCodecTheora::dllname,getVersion<TvideoCodecTheora>,
   TvideoCodecWmv9::dllname,getVersion<TvideoCodecWmv9>,
   TvideoCodecX264::dllname,getVersion<TvideoCodecX264>,
   TvideoCodecSkal::dllname,TvideoCodecSkal::getVersion,
   Tavisynth_c::dllname,Tavisynth_c::getVersion,
   NULL
  };
 return fcsEnc;
}
