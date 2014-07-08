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
#include "IffdshowBase.h"
#include "Tconfig.h"
#include "IffdshowDecVideo.h"
#include "TffdshowPageDecVideo.h"
#include "Ttranslate.h"
#include "Ccodecs.h"
#include "Cdirectshowcontrol.h"
#include "COpenSVCDecoder.h"//mbl work, Open SVC Decoder Interface
#include "Cinfosimd.h"
#include "Ctray.h"
#include "Cpresets.h"
#include "Cfont.h"
#include "CdecoderOptions.h"
#include "Cqueue.h"
#include "Cabout.h"
#include "Ckeys.h"
#include "Coverlay.h"
#include "Coutcsps.h"
#include "ChideShow.h"
#include "COSD.h"
#include "CcspOptions.h"
#include "ISpecifyPropertyPagesVE.h"
#include "resource.h"

CUnknown* WINAPI TffdshowPageDecVideo::CreateInstance(LPUNKNOWN punk,HRESULT *phr)
{
 TffdshowPageDecVideo *pNewObject=new TffdshowPageDecVideo(punk,phr,L"ffdshow",NAME("TffdshowDecVideoPage"),IDD_FFDSHOW,IDS_FFDSHOWDECVIDEO);
 if (pNewObject==NULL)
  *phr=E_OUTOFMEMORY;
 return pNewObject;
}
TffdshowPageDecVideo::TffdshowPageDecVideo(LPUNKNOWN pUnk,HRESULT *phr,const wchar_t *ItitleW,const char_t *name,int dialogId,int resstr):TffdshowPageDec(pUnk,phr,ItitleW,name,dialogId,resstr,true)
{
}

CUnknown* WINAPI TffdshowPageDecVideoRaw::CreateInstance(LPUNKNOWN punk,HRESULT *phr)
{
 TffdshowPageDecVideoRaw *pNewObject=new TffdshowPageDecVideoRaw(punk,phr,L"ffdshow raw",NAME("TffdshowDecVideoRawPage"),IDD_FFDSHOW,IDS_FFDSHOWRAW);
 if (pNewObject==NULL)
  *phr=E_OUTOFMEMORY;
 return pNewObject;
}
TffdshowPageDecVideoRaw::TffdshowPageDecVideoRaw(LPUNKNOWN pUnk,HRESULT *phr,const wchar_t *ItitleW,const char_t *name,int dialogId,int resstr):TffdshowPageDecVideo(pUnk,phr,ItitleW,name,dialogId,resstr)
{
}

CUnknown* WINAPI TffdshowPageDecVideoSubtitles::CreateInstance(LPUNKNOWN punk,HRESULT *phr)
{
 TffdshowPageDecVideoSubtitles *pNewObject=new TffdshowPageDecVideoSubtitles(punk,phr,L"ffdshow subtitles",NAME("TffdshowDecVideoSubtitlePage"),IDD_FFDSHOW,IDS_FFDSHOWRAW);
 if (pNewObject==NULL)
  *phr=E_OUTOFMEMORY;
 return pNewObject;
}
TffdshowPageDecVideoSubtitles::TffdshowPageDecVideoSubtitles(LPUNKNOWN pUnk,HRESULT *phr,const wchar_t *ItitleW,const char_t *name,int dialogId,int resstr):TffdshowPageDecVideo(pUnk,phr,ItitleW,name,dialogId,resstr)
{
}

CUnknown* WINAPI TffdshowPageDecVideoVFW::CreateInstance(LPUNKNOWN punk,HRESULT *phr)
{
 TffdshowPageDecVideoVFW *pNewObject=new TffdshowPageDecVideoVFW(punk,phr,L"Decoder",NAME("TffdshowDecVideoVFWpage"),IDD_FFDSHOW,IDS_FFDSHOWVFW);
 if (pNewObject==NULL)
  *phr=E_OUTOFMEMORY;
 return pNewObject;
}
TffdshowPageDecVideoVFW::TffdshowPageDecVideoVFW(LPUNKNOWN pUnk,HRESULT *phr,const wchar_t *ItitleW,const char_t *name,int dialogId,int resstr):TffdshowPageDecVideo(pUnk,phr,ItitleW,name,dialogId,resstr)
{
}

CUnknown* WINAPI TffdshowPageDecVideoProc::CreateInstance(LPUNKNOWN punk,HRESULT *phr)
{
 TffdshowPageDecVideoProc *pNewObject=new TffdshowPageDecVideoProc(punk,phr,L"Decoder",NAME("TffdshowDecVideoProcPage"),IDD_FFDSHOW,IDS_FFDSHOWVIDEOPROC);
 if (pNewObject==NULL)
  *phr=E_OUTOFMEMORY;
 return pNewObject;
}
TffdshowPageDecVideoProc::TffdshowPageDecVideoProc(LPUNKNOWN pUnk,HRESULT *phr,const wchar_t *ItitleW,const char_t *name,int dialogId,int resstr):TffdshowPageDecVideo(pUnk,phr,ItitleW,name,dialogId,resstr)
{
}

void TffdshowPageDecVideo::onActivate(void)
{
 deciD->initKeys();deciD->initRemote();
 TVINSERTSTRUCT tvis;
 tvis.hParent=NULL;
 tvis.hInsertAfter=TVI_LAST;
 tvis.item.mask=TVIF_PARAM|TVIF_TEXT;
 if ((filterMode&IDFF_FILTERMODE_PROC)==0)
  {
   if ((filterMode & IDFF_FILTERMODE_VIDEOSUBTITLES)==0)
    addTI(&tvis,new TcodecsPageVideo(this));
   if ((filterMode&IDFF_FILTERMODE_VFW)==0)
    {
     if ((filterMode & IDFF_FILTERMODE_VIDEOSUBTITLES)==0)
      {
       addTI(&tvis,new TdirectshowControlPageDec(this))->hti;
	   addTI(&tvis,new TInfoOpenSVCDecoderVideo(this))->hti;//mbl work, Open SVC Decoder Interface
       HTREEITEM htiInfo=addTI(&tvis,new TinfoPageDecVideo(this))->hti;
       tvis.hParent=htiInfo;
       HTREEITEM htiOSD=addTI(&tvis,new TOSDpageVideo(this))->hti;
       //tvis.hParent=htiOSD;
       addTI(&tvis,new TfontPageOSD(this));
       TreeView_Expand(htv,htiOSD,TVE_EXPAND);
       tvis.hParent=NULL;
       TreeView_Expand(htv,htiInfo,TVE_EXPAND);
      }

     HTREEITEM htiMisc=addTI(&tvis,new TdlgMiscPage(this))->hti;
     tvis.hParent=htiMisc;
     addTI(&tvis,new TkeysPage(this));
     TreeView_Expand(htv,htiMisc,TVE_EXPAND);
    }
  }
 //presets page must be the last in pages list
 tvis.hParent=NULL;
 htiPresets=addTI(&tvis,new TpresetsPageVideo(this))->hti;
 tvis.hParent=htiPresets;
 if ((filterMode & IDFF_FILTERMODE_VIDEOSUBTITLES)==0)
  htiBeforeShowHide=addTI(&tvis,new ThideShowPage(this,filterPages))->hti;
 if ((filterMode&IDFF_FILTERMODE_PROC)==0)
  {
   if ((filterMode&IDFF_FILTERMODE_VIDEOSUBTITLES)==0)
    addTI(&tvis,new TmiscPage(this));
   addTI(&tvis,new TqueuePage(this));
   addTI(&tvis,new ToutcspsPage(this));
   addTI(&tvis,new TcspOptionsPage(this));
  }
 sortOrder();
 if ((filterMode&IDFF_FILTERMODE_VFW)==0)
  {
   tvis.hParent=NULL;
   addTI(&tvis,new TaboutPageDecVideo(this));
  }
 static const TanchorInfo ainfo[]=
  {
   IDC_TV_TREE,TanchorInfo::LEFT|TanchorInfo::TOP|TanchorInfo::BOTTOM,
   IDC_BT_RESETORDER,TanchorInfo::LEFT|TanchorInfo::BOTTOM,
   IDC_TOPLINE,TanchorInfo::LEFT|TanchorInfo::TOP|TanchorInfo::RIGHT,
   IDC_TOPLINE2,TanchorInfo::TOP|TanchorInfo::RIGHT,
   IDC_CBX_PRESETS,TanchorInfo::LEFT|TanchorInfo::TOP|TanchorInfo::RIGHT,
   IDC_CHB_PROCESSFULL,TanchorInfo::TOP|TanchorInfo::RIGHT,
   IDC_CHB_PROCESSHALF,TanchorInfo::TOP|TanchorInfo::RIGHT,
   IDC_BT_RESET,TanchorInfo::TOP|TanchorInfo::RIGHT,
   IDC_BT_HELP,TanchorInfo::TOP|TanchorInfo::RIGHT,
   0,0
  };
 anchors.add(ainfo,*this);
 TffdshowPageDec::onActivate();
}
HRESULT TffdshowPageDecVideo::OnConnect(IUnknown *pUnk)
{
 if (ve=pUnk)
  pUnk=ve->get(getGUID<IffdshowDecVideo>());
 deciV=pUnk;
 HRESULT res=TffdshowPageDec::OnConnect(pUnk);
 if (SUCCEEDED(res) && ve)
  ve->setIffdshowBase(getGUID<IffdshowDecVideo>(),deci);
 return FAILED(res)?res:S_OK;
}
HRESULT TffdshowPageDecVideo::OnDisconnect(void)
{
 if (deci==NULL) return E_UNEXPECTED;
 deciV=NULL;
 ve=NULL;
 return TffdshowPageDec::OnDisconnect();
}

INT_PTR TffdshowPageDecVideo::msgProc(UINT uMsg,WPARAM wParam,LPARAM lParam)
{
 switch (uMsg)
  {
   case WM_FFONCHANGE:
    if (wParam==IDFF_lang && ve)
     translate();
    break;
   case WM_COMMAND:
    switch (LOWORD(wParam))
     {
      case IDC_CHB_PROCESSFULL:
       if (HIWORD(wParam)==BN_CLICKED)
        {
         int full=page->getProcessFull();
         page->setProcessFull(1-full);
         return TRUE;
        }
       break;
      case IDC_CHB_PROCESSHALF:
       if (HIWORD(wParam)==BN_CLICKED)
        {
         int half=page->getProcessHalf();
         page->setProcessHalf(1-half);
         return TRUE;
        }
       break;
     }
    break;
  }
 return TffdshowPageDec::msgProc(uMsg,wParam,lParam);
}

// -------------------- configure ----------------------
extern "C" void CALLBACK configure(HWND hwnd,HINSTANCE hinst,LPTSTR lpCmdLine,int nCmdShow);
void CALLBACK configure(HWND hwnd,HINSTANCE hinst,LPTSTR lpCmdLine,int nCmdShow)
{
 TffdshowPageDec::configure<IffdshowDecVideo>(CLSID_FFDSHOW,IDFF_FILTERMODE_VIDEO,lpCmdLine);
}
extern "C" void CALLBACK configureRaw(HWND hwnd,HINSTANCE hinst,LPTSTR lpCmdLine,int nCmdShow);
void CALLBACK configureRaw(HWND hwnd,HINSTANCE hinst,LPTSTR lpCmdLine,int nCmdShow)
{
 TffdshowPageDec::configure<IffdshowDecVideo>(CLSID_FFDSHOWRAW,IDFF_FILTERMODE_VIDEORAW,lpCmdLine);
}
extern "C" void CALLBACK configureSubtitles(HWND hwnd,HINSTANCE hinst,LPTSTR lpCmdLine,int nCmdShow);
void CALLBACK configureSubtitles(HWND hwnd,HINSTANCE hinst,LPTSTR lpCmdLine,int nCmdShow)
{
 TffdshowPageDec::configure<IffdshowDecVideo>(
  CLSID_FFDSHOWSUBTITLES,
  IDFF_FILTERMODE_VIDEORAW | IDFF_FILTERMODE_VIDEOSUBTITLES,
  lpCmdLine);
}
