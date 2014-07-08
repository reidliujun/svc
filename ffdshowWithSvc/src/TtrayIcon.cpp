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
#include <shellapi.h>
#include <shlwapi.h>
#include "TtrayIcon.h"
#include "resource.h"
#include "IffdshowBase.h"
#include "IffdshowDec.h"
#include "IffdshowDecVideo.h"
#include "TsubtitlesFile.h"
#include "TfilterSettings.h"
#include "IffdshowEnc.h"
#include "IffdshowDecAudio.h"
#include "Tconfig.h"
#include "Ttranslate.h"
#include "TSpecifyPropertyPagesVE.h"
#include "dsutil.h"

//==================================== TtrayIconBase =====================================
unsigned int TtrayIconBase::run(CAMEvent *ev,HWND *hwndRef)
{
 init();
 if (h)
  {
   *hwndRef=h;
   ev->Set();
   MSG msg;
   while(GetMessage(&msg,NULL,0,0))
    {
     TranslateMessage(&msg);
     DispatchMessage(&msg);
    }
   hide();
  }
 else
  ev->Set();
 delete this;
 _endthreadex(0);
 return 0;
}

TtrayIconBase::TtrayIconBase(IffdshowBase *Ideci):deci(Ideci)
{
 randomize();
 deci->getInstance(&hi);
 deci->getConfig(&config);
 mode=deci->getParam2(IDFF_filterMode);
 at=(ATOM)NULL;h=NULL;
 visible=false;
 strcpy(classname,_l("ffdshow unknown"));
 tr=NULL;
}
TtrayIconBase::~TtrayIconBase()
{
 if (visible) Shell_NotifyIcon(NIM_DELETE,(::NOTIFYICONDATA*)&nid);
 if (nid.hIcon) DestroyIcon(nid.hIcon);
 if (h) DestroyWindow(h);
 if (at) UnregisterClass(classname,hi);
 if (tr) tr->release();
}

HMENU TtrayIconBase::createMenu(int &ord)
{
 if (!tr) deci->getTranslator(&tr);
 return CreatePopupMenu();
}

void TtrayIconBase::insertSeparator(HMENU hm,int &ord)
{
 MENUITEMINFO mii;
 mii.cbSize=sizeof(mii);
 mii.fMask=MIIM_TYPE;
 mii.fType=MFT_SEPARATOR;
 InsertMenuItem(hm,ord++,TRUE,&mii);
}

void TtrayIconBase::insertSubmenu(HMENU hm,int &ord,const char_t *caption,bool translate,HMENU subMenu)
{
 MENUITEMINFO mii;
 mii.cbSize=sizeof(mii);
 mii.fMask=MIIM_TYPE|MIIM_SUBMENU;
 mii.fType=MFT_STRING;
 mii.hSubMenu=subMenu;
 mii.dwTypeData=LPTSTR(translate?tr->translate(caption):caption);
 mii.cch=(UINT)strlen(mii.dwTypeData);
 InsertMenuItem(hm,ord++,TRUE,&mii);
}

int TtrayIconBase::insertMenuItem(HMENU hm,int &ord,int id,const char_t *caption,bool translate,bool checked,bool enabled)
{
 MENUITEMINFO mii;
 mii.cbSize=sizeof(mii);
 mii.fMask=MIIM_ID|MIIM_STATE|MIIM_TYPE;
 mii.fType=MFT_STRING;
 mii.wID=id;
 mii.fState=(checked?MFS_CHECKED:0)|(!enabled?MFS_DISABLED:0);
 mii.dwTypeData=LPTSTR(translate?tr->translate(caption):caption);
 mii.cch=(UINT)strlen(mii.dwTypeData);
 InsertMenuItem(hm,ord++,TRUE,&mii);
 return mii.wID;
}

LRESULT CALLBACK TtrayIconBase::trayWndProc(HWND hwnd, UINT msg, WPARAM wprm, LPARAM lprm)
{
 TtrayIconBase *ti=(TtrayIconBase*)GetWindowLongPtr(hwnd,GWLP_USERDATA);
 switch (msg)
  {
   case WM_CREATE:
    SetTimer(hwnd,TMR_TRAYICON,500,NULL);
    break;
   case WM_DESTROY:
    KillTimer(hwnd,TMR_TRAYICON);
    if (ti) ti->h=NULL;
    PostQuitMessage(0);
    break;
   case WM_TIMER:
    if (ti->deci->getParam2(IDFF_trayIcon)) ti->show(); else ti->hide();
    return 0;
   case MSG_TRAYICON:
    if (!ti)
     return 0;
    else
     {
      if (!ti->tr) ti->deci->getTranslator(&ti->tr);
      return ti->processTrayMsg(hwnd,wprm,lprm);
     }
  }
 return DefWindowProc(hwnd,msg,wprm,lprm);
}

LRESULT TtrayIconBase::processTrayMsg(HWND hwnd,WPARAM wprm,LPARAM lprm)
{
 switch (lprm)
  {
   case WM_MOUSEMOVE:
    if (deci->getParam2(IDFF_trayIconExt))
     {
      char_t in[64];if (FAILED(deci->getInCodecString(in,64))) in[0]='\0';
      char_t out[64];if (FAILED(deci->getOutCodecString(out,64))) out[0]='\0';
      char_t *tipptr;int tiplen;char_t inputS[40],outputS[40];
      const char_t *sep1,*sep2;
      if (nid.cbSize==sizeof(nid))
       {
        tipptr=nid.szTip;
        tiplen=128;
        tsnprintf_s(inputS, 40, _TRUNCATE,_l("%s: "),tr->translate(_l("Input")));
        tsnprintf_s(outputS, 40, _TRUNCATE,_l("%s: "),tr->translate(_l("Output")));
        sep1=sep2=_l("\n");
       }
      else
       {
        tipptr=nid.szTip;
        tiplen=64;
        inputS[0]=outputS[0]='\0';sep1=_l("");sep2=_l("->");
        in[26]='\0';out[26]='\0';
       }
      if (in[0] && out[0])
       tsnprintf_s(tipptr, tiplen, _TRUNCATE,_l("%s: %s%s%s%s%s%s"),tip,sep1,inputS,in,sep2,outputS,out);
      else if (in[0])
       tsnprintf_s(tipptr, tiplen, _TRUNCATE,_l("%s: %s"),tip,in);
      else
       tsnprintf_s(tipptr, tiplen, _TRUNCATE,_l("%s"),tip);
      nid.szTip[tiplen-1]='\0';
      Shell_NotifyIcon(NIM_MODIFY,(NOTIFYICONDATA*)&nid);
     }
    break;
   case WM_LBUTTONDBLCLK:
    deci->showCfgDlg(NULL);
    break;
   case WM_RBUTTONUP:
   case WM_CONTEXTMENU:
    {
     if (deci->getCfgDlgHwnd()) break;
     POINT p;
     GetCursorPos(&p);
     SetForegroundWindow(hwnd);
     int ord=0;
     HMENU hm=createMenu(ord);
     int cmd=TrackPopupMenu(hm,TPM_RETURNCMD,p.x,p.y,0,hwnd,NULL);
     PostMessage(hwnd,WM_NULL,0,0);
     processCmd(hm,cmd);
     DestroyMenu(hm);
     return TRUE;
    }
  }
 return 0;
}

void TtrayIconBase::processCmd(HMENU hm,int cmd)
{
}

int TtrayIconBase::cfgGet(int i)
{
 return deci->getParam2(i);
}
int TtrayIconBase::cfgSet(int i,int val)
{
 deci->putParam(i,val);
 return val;
}

void TtrayIconBase::init(void)
{
 h=createInvisibleWindow(hi,classname,classname,trayWndProc,0,&at);
 SetWindowLongPtr(h,GWLP_USERDATA,LONG_PTR(this));
 memset(&nid,0,sizeof(nid));
 nid.cbSize=config->getShellVersion()<(5<<24)?88:sizeof(nid);
 nid.hWnd=h;
 nid.uID=TRAYICON;
 nid.uFlags=NIF_ICON|NIF_TIP|NIF_MESSAGE;//|(nid.cbSize==sizeof(nid)?NIF_INFO:0);
 nid.hIcon=(HICON)LoadImage(hi,MAKEINTRESOURCE(icon),IMAGE_ICON,16,16,LR_DEFAULTCOLOR);
 nid.uCallbackMessage=MSG_TRAYICON;
 ff_strncpy(nid.szTip, tip, countof(nid.szTip));
}
void TtrayIconBase::show(void)
{
 if (!visible)
  {
   if (!h) init();
   Shell_NotifyIcon(NIM_ADD,(::NOTIFYICONDATA*)&nid);
   visible=true;
  }
}
void TtrayIconBase::hide(void)
{
 if (visible)
  {
   Shell_NotifyIcon(NIM_DELETE,(::NOTIFYICONDATA*)&nid);
   visible=false;
  }
}

//==================================== TtrayIconDec ========================================
TtrayIconDec::TtrayIconDec(IffdshowBase *Ideci):TtrayIconBase(Ideci),deciD(Ideci)
{
}

void TtrayIconDec::insertMenuItemFilter(HMENU hm,int &ord,const TfilterIDFF *f)
{
 insertMenuItem(hm,ord,IDC_FIRST_FILTER+f->is,stringreplace(ffstring(tr->translate(NULL,f->dlgId,0,f->name)),_l("&"),_l("&&"),rfReplaceAll).c_str(),false,!!cfgGet(f->is),true);
}

HMENU TtrayIconDec::createMenu(int &ord)
{
 HMENU hm=TtrayIconBase::createMenu(ord);
 IFilterGraph *m_pGraph;deciD->getGraph(&m_pGraph);
 if (m_pGraph)
  {
   int i=0;
   comptr<IEnumFilters> eff;
   if (m_pGraph->EnumFilters(&eff)==S_OK)
    {
     eff->Reset();
     for (comptr<IBaseFilter> bff;eff->Next(1,&bff,NULL)==S_OK;bff=NULL)
      {
       char_t name[MAX_PATH],filtername[MAX_PATH];
       getFilterName(bff,name,filtername,countof(filtername));
       char_t compactname[41];
       PathCompactPathEx(compactname,name,40,0);
       graphnames[insertMenuItem(hm,ord,IDC_FIRST_GRAPH+(i++),compactname,false,false,!!comptrQ<ISpecifyPropertyPages>(bff))]=filtername;
      }
    }
   if (i>0) insertSeparator(hm,ord);
  }
 unsigned int len;
 deciD->getNumPresets(&len);
 char_t actPreset[1024];
 deciD->getActivePresetName(actPreset,1023);
 for (unsigned int ii=0;ii<len;ii++)
  {
   char_t preset[1024];
   deciD->getPresetName(ii,preset,1023);
   insertMenuItem(hm,ord,IDC_FIRST_PRESET+ii,stringreplace(ffstring(preset),_l("&"),_l("&&"),rfReplaceAll).c_str(),false,stricmp(preset,actPreset)==0,true);
  }
 insertSeparator(hm,ord);

 typedef array_vector<TordFilters,255> TordFiltersVector;
 TordFiltersVector ordFilters;
 const char_t *activepresetname=deciD->getActivePresetName2();
 if (activepresetname)
  {
   const TfilterIDFFs *filters;deciD->getFilterIDFFs(activepresetname,&filters);
   for (TfilterIDFFs::const_iterator f=filters->begin();f!=filters->end();f++)
    if (f->idff->is)
     ordFilters.push_back(std::make_pair(f->idff->order?cfgGet(f->idff->order):0,f->idff));
  }
 bool notorder=false;
 for (const TfilterIDFF *f=deciD->getNextFilterIDFF();f && f->name;f++)
  if (f->order)
   ordFilters.push_back(std::make_pair(f->order?cfgGet(f->order):0,f));
  else
   notorder=true;
 std::sort(ordFilters.begin(),ordFilters.end(),sortOrdFilters);

 for (TordFiltersVector::const_iterator fo=ordFilters.begin();fo!=ordFilters.end();fo++)
  if (fo->second->show==0 || cfgGet(fo->second->show))
   {
    insertMenuItemFilter(hm,ord,fo->second);
    insertSubmenuCallback(hm,ord,fo->second);
   }
 if (notorder)
  {
   insertSeparator(hm,ord);
   for (const TfilterIDFF *f=deciD->getNextFilterIDFF();f && f->name;f++)
    if (!f->order)
     {
      insertMenuItemFilter(hm,ord,f);
      insertSubmenuCallback(hm,ord,f);
     }
  }
 return hm;
}

void TtrayIconDec::negate_Param(int id)
{
 deci->invParam(id);
 deci->saveGlobalSettings();
 deciD->saveKeysSettings();
 deciD->saveActivePreset(NULL);
}

LRESULT TtrayIconDec::processTrayMsg(HWND hwnd,WPARAM wprm,LPARAM lprm)
{
/*
 switch (lprm)
  {
   case WM_LBUTTONDBLCLK:
    {
     IFilterGraph *m_pGraph;deciD->getGraph(&m_pGraph);
     IUnknown *deciV=searchFilter(m_pGraph,CLSID_FFDSHOW);
     IUnknown *deciA=searchFilter(m_pGraph,CLSID_FFDSHOWAUDIO);
     if (deciV && deciA)
      {
       TSpecifyPropertyPagesVE::show2configPages(IID_IffdshowDecVideo,deciV,IID_IffdshowDecAudio,deciA,IDS_FFDSHOWDEC,IDI_FFDSHOW,IDFF_dlgDecCurrentPage);
       return TRUE;
      }
    }
  }*/
 return TtrayIconBase::processTrayMsg(hwnd,wprm,lprm);
}

void TtrayIconDec::processCmd(HMENU hm,int cmd)
{
 if (cmd>=IDC_FIRST_FILTER)
  {
   int i=cmd-IDC_FIRST_FILTER;
   negate_Param(i);
  }
 else if (cmd>=IDC_FIRST_GRAPH)
  {
   char_t filterName[1024];
   GetMenuString(hm,cmd,filterName,1024,MF_BYCOMMAND);
   if (!showffdshowCfg(filterName))
    showFilterCfg(graphnames[cmd].c_str());
  }
 else if (cmd>=IDC_FIRST_PRESET)
  {
   char_t preset[1024];
   GetMenuString(hm,cmd,preset,1024,MF_BYCOMMAND);
   deciD->setActivePreset(preset,false);
  }
 else
  TtrayIconBase::processCmd(hm,cmd);
}

void TtrayIconDec::showFilterCfg(const char_t *fltname)
{
 OutputDebugString(fltname);
 IFilterGraph *m_pGraph;deciD->getGraph(&m_pGraph);
 WCHAR fltnameW[1024];
 text<wchar_t>(fltname,fltnameW);//MultiByteToWideChar(CP_ACP,0,fltname,(int)strlen(fltname)+1,fltnameW,512);
 comptr<IBaseFilter> flt;
 if (FAILED(m_pGraph->FindFilterByName(fltnameW,&flt))) return;
 if (comptrQ<ISpecifyPropertyPages> ispp=flt)
  {
   CAUUID pages;
   if (ispp->GetPages(&pages)==S_OK)
    {
     IUnknown *ifflist[]={ispp};
     OleCreatePropertyFrame(NULL,10,10,fltnameW,
                            1,ifflist,
                            pages.cElems,pages.pElems,
                            LOCALE_SYSTEM_DEFAULT,
                            0,0
                           );
     CoTaskMemFree(pages.pElems);
    }
  }
}

bool TtrayIconDec::sortOrdFilters(const TordFilters &of1,const TordFilters &of2)
{
 return of2.first>of1.first;
}

//================================== TtrayIconDecVideo =====================================
TtrayIconDecVideo::TtrayIconDecVideo(IffdshowBase *Ideci):TtrayIconDec(Ideci),deciV(Ideci)
{
 if (mode & IDFF_FILTERMODE_VIDEOSUBTITLES)
  {
   tsprintf(classname,_l("ffdshowsubtitle_tray_%i"),rand()%1000);
   tip = _l("ffdshow subtitle filter");
  }
 else if (mode & IDFF_FILTERMODE_VIDEORAW)
  {
   tsprintf(classname,_l("ffdshowraw_tray_%i"),rand()%1000);
   tip = _l("ffdshow video decoder raw");
  }
 else
  {
   tsprintf(classname,_l("ffdshow_tray_%i"),rand()%1000);
   tip = _l("ffdshow video decoder");
  }
 icon=IDI_MODERN_ICON_V;
 if (cfgGet(IDFF_trayIconType)==2)
  icon=IDI_FFDSHOW;
 else
  if (!Tconfig::get_trayIconFullColorOS())
   icon=IDI_MODERN_4BIT_ICON_V;
 setThreadName(DWORD(-1),"trayDecVideo");
}

HMENU TtrayIconDecVideo::makeSubtitlesMenu(void)
{
 strings files;
 TsubtitlesFile::findPossibleSubtitles(deci->getSourceName(),deci->getParamStr2(IDFF_subSearchDir),files);
 int textpinconnectedCnt=deciV->getConnectedTextPinCnt();
 if (files.empty() && !textpinconnectedCnt) return NULL;
 HMENU hm=CreatePopupMenu();
 int ord=0;
 const char_t *cursubflnm=deciV->getCurrentSubFlnm();
 for (strings::const_iterator f=files.begin();f!=files.end();f++,ord++)
  insertMenuItem(hm,ord,IDC_FIRST_SUBFILE+ord, stringreplace(*f,_l("&"),_l("&&"),rfReplaceAll).c_str() ,false,stricmp(f->c_str(),cursubflnm)==0,true);
 if (textpinconnectedCnt)
  {
   std::vector<std::pair<ffstring,int> > textpins;
   for (int i=0;i<textpinconnectedCnt;i++)
    {
     const char_t *textname;int found,id;
     deciV->getConnectedTextPinInfo(i,&textname,&id,&found);
     if (found)
      {
       char_t s[256];
       ff_strncpy(s, tr->translate(_l("embedded")), countof(s));
       if (textname[0])
        strncatf(s, countof(s), _l(" (%s)"), textname);
       textpins.push_back(std::make_pair(ffstring(s),id));
      }
    }
   if (!textpins.empty())
    {
     if (!files.empty()) insertSeparator(hm,ord);
     int subShown=deci->getParam2(IDFF_subShowEmbedded);
     for (size_t i=0;i<textpins.size();i++)
      insertMenuItem(hm,ord,IDC_FIRST_TEXTPIN+textpins[i].second,stringreplace(textpins[i].first,_l("&"),_l("&&"),rfReplaceAll).c_str() ,false,textpins[i].second==subShown,true);
    }
  }
 if (int langcnt=deciV->getSubtitleLanguagesCount2())
  {
   insertSeparator(hm,ord);
   int curlang=deci->getParam2(IDFF_subCurLang);
   for (int i=0;i<langcnt;i++)
    {
     const char_t *lang;deciV->getSubtitleLanguageDesc(i,&lang);
     if (lang[0])
      insertMenuItem(hm,ord,IDC_FIRST_SUBLANG+i,lang,false,i==curlang,true);
    }
  }
 return ord?hm:(DestroyMenu(hm),(HMENU)NULL);
}

void TtrayIconDecVideo::insertSubmenuCallback(HMENU hm,int &ord,const TfilterIDFF *f)
{
 if (f->id==IDFF_filterSubtitles)
  if (HMENU smn=makeSubtitlesMenu())
   insertSubmenu(hm,ord,_l("Subtitle sources"),true,smn);
}

void TtrayIconDecVideo::processCmd(HMENU hm,int cmd)
{
 if (cmd>=IDC_FIRST_TEXTPIN)
  {
   int id=cmd-IDC_FIRST_TEXTPIN;
   int oldId=deci->getParam2(IDFF_subShowEmbedded);
   deci->putParam(IDFF_subShowEmbedded,id==oldId?0:id);
  }
 else if (cmd>=IDC_FIRST_SUBLANG)
  deci->putParam(IDFF_subCurLang,cmd-IDC_FIRST_SUBLANG);
 else if (cmd>=IDC_FIRST_SUBFILE)
  {
   char_t subflnm0[MAX_PATH];
   GetMenuString(hm,cmd,subflnm0,MAX_PATH,MF_BYCOMMAND);
   ffstring subflnm(subflnm0);
   deci->putParamStr(IDFF_subTempFilename,stringreplace(subflnm,_l("&&"),_l("&"),rfReplaceAll).c_str());
   deci->putParam(IDFF_isSubtitles,1);
   deci->putParam(IDFF_subShowEmbedded,0);
  }
 else if (cmd==IDFF_isAvisynth+IDC_FIRST_FILTER)
  {
   deci->putParamStr(IDFF_OSDuser,_l(""));
   TtrayIconDec::processCmd(hm,cmd);
  }
 else
  TtrayIconDec::processCmd(hm,cmd);
}


bool TtrayIconDecVideo::showffdshowCfg(char_t* filterName)
{
   if (_strnicmp(_l("ffdshow video decoder"),filterName,22)==0)
    {
     deci->showCfgDlg(NULL);
     return true;
    }
   return false;
}
//==================================== TtrayIconDecAudio ====================================
TtrayIconDecAudio::TtrayIconDecAudio(IffdshowBase *Ideci):TtrayIconDec(Ideci),deciA(Ideci)
{
 tsprintf(classname,mode&IDFF_FILTERMODE_AUDIORAW?_l("ffdshowaudioraw_tray_%i"):_l("ffdshowaudio_tray_%i"),rand()%1000);
 tip=mode&IDFF_FILTERMODE_AUDIORAW?_l("ffdshow raw audio decoder"):_l("ffdshow audio decoder");
 icon=IDI_MODERN_ICON_A;
 if (cfgGet(IDFF_trayIconType)==2)
  icon=IDI_FFDSHOWAUDIO;
 else
  if (!Tconfig::get_trayIconFullColorOS())
   icon=IDI_MODERN_4BIT_ICON_A;
 setThreadName(DWORD(-1),"trayDecAudio");
}
HMENU TtrayIconDecAudio::createMenu(int &ord)
{
 HMENU hm=TtrayIconDec::createMenu(ord);
 unsigned int num=deciA->getNumStreams2();
 if (num>1)
  {
   insertSeparator(hm,ord);
   unsigned int cur=deciA->getCurrentStream2();
   for (unsigned int i=0;i<num;i++)
    {
     char_t descr[250];
     if (deciA->getStreamDescr(i,descr,250)==S_OK)
      {
       char_t stream[255];
       tsnprintf_s(stream, countof(stream), _TRUNCATE, _l("%u. %s"), i, descr);
       insertMenuItem(hm,ord,IDC_FIRST_STREAM+i,stream,false,i==cur,true);
      }
    }
  }
 return hm;
}
void TtrayIconDecAudio::processCmd(HMENU hm,int cmd)
{
 if (cmd>=IDC_FIRST_STREAM)
  deciA->setCurrentStream(cmd-IDC_FIRST_STREAM);
 else TtrayIconDec::processCmd(hm,cmd);
}


bool TtrayIconDecAudio::showffdshowCfg(char_t* filterName)
{
   if (_strnicmp(_l("ffdshow Audio Decoder"),filterName,22)==0)
    {
     deci->showCfgDlg(NULL);
     return true;
    }
   return false;
}
//===================================== TtrayIconEnc ======================================
TtrayIconEnc::TtrayIconEnc(IffdshowBase *Ideci):TtrayIconBase(Ideci),deciE(Ideci)
{
 tsprintf(classname,_l("ffdshowEnc_tray_%i"),rand()%1000);
 tip=_l("ffdshow video encoder");
 icon=IDI_MODERN_ICON_E;
 if (cfgGet(IDFF_trayIconType)==2)
  icon=IDI_FFVFW;
 else
  if (!Tconfig::get_trayIconFullColorOS())
   icon=IDI_MODERN_4BIT_ICON_E;
 setThreadName(DWORD(-1),"trayEnc");
}
LRESULT TtrayIconEnc::processTrayMsg(HWND hwnd,WPARAM wprm,LPARAM lprm)
{
 switch (lprm)
  {
   case WM_LBUTTONDBLCLK:
    InitCommonControls();
    deci->putParam(IDFF_dlgEncGraph,1);
    deci->showCfgDlg(NULL);
    return TRUE;
   case WM_RBUTTONUP:
   case WM_CONTEXTMENU:
    return TRUE;
  }
 return TtrayIconBase::processTrayMsg(hwnd,wprm,lprm);
}
