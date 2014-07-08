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
#include "TffdshowPageDecAudio.h"
#include "IffdshowBase.h"
#include "IffdshowDecAudio.h"
#include "ISpecifyPropertyPagesVE.h"
#include "Ccodecs.h"
#include "Ctray.h"
#include "Ckeys.h"
#include "Cabout.h"
#include "Cdirectshowcontrol.h"
#include "Cinfosimd.h"
#include "Cpresets.h"
#include "ChideShow.h"
#include "Cprocsfs.h"
#include "audio/CaudioDecoderOptions.h"
#include "Coutsfs.h"
#include "CaudioSwitcher.h"
#include "COSD.h"

CUnknown* WINAPI TffdshowPageDecAudio::CreateInstance(LPUNKNOWN lpunk,HRESULT *phr)
{
 TffdshowPageDecAudio *pNewObject=new TffdshowPageDecAudio(lpunk,phr,L"ffdshow audio",NAME("TffdshowDecAudioPage"),IDD_FFDSHOWAUDIO,IDS_FFDSHOWDECAUDIO);
 if (pNewObject==NULL)
  *phr=E_OUTOFMEMORY;
 return pNewObject;
}

CUnknown* WINAPI TffdshowPageDecAudioRaw::CreateInstance(LPUNKNOWN lpunk,HRESULT *phr)
{
 TffdshowPageDecAudioRaw *pNewObject=new TffdshowPageDecAudioRaw(lpunk,phr,L"ffdshow audio processor",NAME("TffdshowDecAudioRawPage"),IDD_FFDSHOWAUDIO,IDS_FFDSHOWDECAUDIO);
 if (pNewObject==NULL)
  *phr=E_OUTOFMEMORY;
 return pNewObject;
}
TffdshowPageDecAudioRaw::TffdshowPageDecAudioRaw(LPUNKNOWN pUnk,HRESULT *phr,const wchar_t *ItitleW,const char_t *name,int dialogId,int resstr):TffdshowPageDecAudio(pUnk,phr,ItitleW,name,dialogId,resstr)
{
}

TffdshowPageDecAudio::TffdshowPageDecAudio(LPUNKNOWN pUnk,HRESULT *phr,const wchar_t *ItitleW,const char_t *name,int dialogId,int resstr):TffdshowPageDec(pUnk,phr,ItitleW,name,dialogId,resstr,false)
{
}
TffdshowPageDecAudio::~TffdshowPageDecAudio()
{
}

HRESULT TffdshowPageDecAudio::OnConnect(IUnknown *pUnk)
{
 if (ve=pUnk)
  pUnk=ve->get(getGUID<IffdshowDec>());
 deciA=pUnk;
 HRESULT res=TffdshowPageDec::OnConnect(pUnk);
 if (SUCCEEDED(res) && ve)
  {
   ve->setIffdshowBase(getGUID<IffdshowDecAudio>(),deci);
   resorePos=false;
  }
 return FAILED(res)?res:S_OK;
}
HRESULT TffdshowPageDecAudio::OnDisconnect(void)
{
 if (deci==NULL) return E_UNEXPECTED;
 deciA=NULL;
 ve=NULL;
 return TffdshowPageDec::OnDisconnect();
}

void TffdshowPageDecAudio::onActivate(void)
{
 TVINSERTSTRUCT tvis;
 tvis.hParent=NULL;
 tvis.hInsertAfter=TVI_LAST;
 tvis.item.mask=TVIF_PARAM|TVIF_TEXT;
 addTI(&tvis,new TcodecsPageAudio(this));
 addTI(&tvis,new TdirectshowControlPageDec(this))->hti;
 HTREEITEM htiInfo=addTI(&tvis,new TinfoPageDecAudio(this))->hti;
 tvis.hParent=htiInfo;
 addTI(&tvis,new TOSDpageAudio(this));
 tvis.hParent=NULL;
 TreeView_Expand(htv,htiInfo,TVE_EXPAND);
 HTREEITEM htiMisc=addTI(&tvis,new TdlgMiscPage(this))->hti;
 if (0)
  {
   tvis.hParent=htiMisc;
   addTI(&tvis,new TkeysPage(this));
   tvis.hParent=NULL;
   tvis.item.mask=TVIF_PARAM|TVIF_TEXT;
   TreeView_Expand(htv,htiMisc,TVE_EXPAND);
  }
 addTI(&tvis,new TaudioSwitcherPage(this));
 //presets page must be the last in pages list
 htiPresets=addTI(&tvis,new TpresetsPageAudio(this))->hti;
 tvis.hParent=htiPresets;
 htiBeforeShowHide=addTI(&tvis,new ThideShowPage(this,filterPages))->hti;
/*
 const char *activePresetName=deciD->getActivePresetName2();
 if (activePresetName)
  deciD->createPresetPages(activePresetName,this);
*/
 addTI(&tvis,new TaudioDecoderOptionsPage(this));
 addTI(&tvis,new TprocsfsPage(this));
 addTI(&tvis,new ToutsfsPage(this));
 tvis.hParent=NULL;
 addTI(&tvis,new TaboutPageDecAudio(this));
 static const TanchorInfo ainfo[]=
  {
   IDC_TV_TREE,TanchorInfo::LEFT|TanchorInfo::TOP|TanchorInfo::BOTTOM,
   IDC_BT_RESETORDER,TanchorInfo::LEFT|TanchorInfo::BOTTOM,
   IDC_TOPLINE,TanchorInfo::LEFT|TanchorInfo::TOP|TanchorInfo::RIGHT,
   IDC_CBX_PRESETS,TanchorInfo::LEFT|TanchorInfo::TOP|TanchorInfo::RIGHT,
   IDC_BT_RESET,TanchorInfo::TOP|TanchorInfo::RIGHT,
   IDC_BT_HELP,TanchorInfo::TOP|TanchorInfo::RIGHT,
   0,0
  };
 anchors.add(ainfo,*this);
 TffdshowPageDec::onActivate();
}

// -------------------- configure ----------------------
extern "C" void CALLBACK configureAudio(HWND hwnd,HINSTANCE hinst,LPTSTR lpCmdLine,int nCmdShow);
void CALLBACK configureAudio(HWND hwnd,HINSTANCE hinst,LPTSTR lpCmdLine,int nCmdShow)
{
 TffdshowPageBase::configure<IffdshowDecAudio>(CLSID_FFDSHOWAUDIO,IDFF_FILTERMODE_AUDIO,lpCmdLine);
}
extern "C" void CALLBACK configureAudioRaw(HWND hwnd,HINSTANCE hinst,LPTSTR lpCmdLine,int nCmdShow);
void CALLBACK configureAudioRaw(HWND hwnd,HINSTANCE hinst,LPTSTR lpCmdLine,int nCmdShow)
{
 TffdshowPageBase::configure<IffdshowDecAudio>(CLSID_FFDSHOWAUDIORAW,IDFF_FILTERMODE_AUDIORAW,lpCmdLine);
}
