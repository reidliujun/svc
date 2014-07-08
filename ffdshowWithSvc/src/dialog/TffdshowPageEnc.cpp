/*
 * Copyright (c) 2003-2006 Milan Cutka
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
#include "IffdshowEnc.h"
#include "Cabout.h"
#include "Cgraph.h"
#include "Cgeneric.h"
#include "CgenericHuffyuv.h"
#include "CgenericWmv9.h"
#include "CgenericTHEO.h"
#include "CgenericRAW.h"
#include "CgenericFFV1.h"
#include "CgenericLJPEG.h"
#include "CgenericDV.h"
#include "Cme.h"
#include "CmeXvid.h"
#include "CmeX264.h"
#include "CmeSkal.h"
#include "Cquant.h"
#include "CquantTables.h"
#include "Cmasking.h"
#include "CmaskingXVID.h"
#include "CmaskingSKAL.h"
#include "CmaskingTHEO.h"
#include "CmaskingX264.h"
#include "Conepass.h"
#include "ConepassXvid.h"
#include "Ccredits.h"
#include "CfirstPass.h"
#include "CsecondPass.h"
#include "CcurveNormal.h"
#include "CcurveAlt.h"
#include "Cin.h"
#include "Cout.h"
#include "Ctray.h"
#include "TffdshowPageEnc.h"
#include "TvideoCodec.h"
#include "TpresetEnc.h"
#include "TpresetsEncDlg.h"
#include "ISpecifyPropertyPagesVE.h"

const char_t* TffdshowPageEnc::encModeNames[]=
{
 _l("one pass - average bitrate"),
 _l("one pass - quality"),
 _l("one pass - quantizer"),
 _l("two passes - 1st pass"),
 _l("two passes - 2nd pass ext"),
 _l("two passes - 2nd pass int")
};

CUnknown* WINAPI TffdshowPageEncVFW::CreateInstance(LPUNKNOWN punk,HRESULT *phr)
{
 TffdshowPageEncVFW *pNewObject=new TffdshowPageEncVFW(punk,phr,L"Encoder",NAME("TffdshowEncVFW"),IDD_FFDSHOWENC,IDS_FFDSHOWENCVFW);
 if (pNewObject==NULL)
  *phr=E_OUTOFMEMORY;
 return pNewObject;
}
TffdshowPageEncVFW::TffdshowPageEncVFW(LPUNKNOWN pUnk,HRESULT *phr,const wchar_t *ItitleW,const char_t *name,int dialogId,int resstr):TffdshowPageEnc(pUnk,phr,ItitleW,name,dialogId,resstr)
{
}

CUnknown* WINAPI TffdshowPageEnc::CreateInstance(LPUNKNOWN lpunk,HRESULT *phr)
{
 TffdshowPageEnc *pNewObject=new TffdshowPageEnc(lpunk,phr,L"ffdshow video encoder",NAME("TffdshowEncPage"),IDD_FFDSHOWENC,IDS_FFDSHOWENC);
 if (pNewObject==NULL)
  *phr=E_OUTOFMEMORY;
 return pNewObject;
}
TffdshowPageEnc::TffdshowPageEnc(LPUNKNOWN pUnk,HRESULT *phr,const wchar_t *ItitleW,const char_t *name,int dialogId,int resstr):TffdshowPageBase(pUnk,phr,ItitleW,name,dialogId,resstr)
{
 isAbout=isGraph=0;
 presets=NULL;
}
TffdshowPageEnc::~TffdshowPageEnc()
{
}
HRESULT TffdshowPageEnc::OnConnect(IUnknown *pUnk)
{
 if (ve=pUnk)
  pUnk=ve->get(IID_IffdshowEnc);
 if (!(deciE=pUnk)) return E_NOINTERFACE;
 deciE->getCoSettingsPtr((const TcoSettings**)&oldCoSettingsPtr);
 localCfg=*oldCoSettingsPtr;
 deciE->setCoSettingsPtr(&localCfg);
 HRESULT hr=TffdshowPageBase::OnConnect(pUnk);
 if (SUCCEEDED(hr))
  {
   isGraph=cfgGet(IDFF_dlgEncGraph);cfgSet(IDFF_dlgEncGraph,0);
   isAbout=cfgGet(IDFF_dlgEncAbout);cfgSet(IDFF_dlgEncAbout,0);
   if (ve) ve->setIffdshowBase(IID_IffdshowEnc,deci);
   codecId=(CodecID)cfgGet(IDFF_enc_codecId);
  }
 return FAILED(hr)?hr:S_OK;
}
HRESULT TffdshowPageEnc::OnDisconnect(void)
{
 if (deci==NULL) return E_UNEXPECTED;
 deciE->setCoSettingsPtr((TcoSettings*)oldCoSettingsPtr);
 deci->loadGlobalSettings();
 deciE=NULL;
 if (presets) delete presets;presets=NULL;
 ve=NULL;
 return TffdshowPageBase::OnDisconnect();
}

void TffdshowPageEnc::addPropsIDs(const int *propsIDs)
{
 const int *p=propsIDs;
 while (*p)
  {
   allPropsIDs.push_back(*p);
   p++;
  }
}

ThtiPage* TffdshowPageEnc::addTI(TVINSERTSTRUCT *tvis,const TconfPages &pages,int *Iid)
{
 for (TconfPages::const_iterator p=pages.begin();p!=pages.end();p++)
  addPropsIDs((*p)->propsIDs);
 return TffdshowPageBase::addTI(tvis,pages,Iid);
}
ThtiPage* TffdshowPageEnc::addTI(TVINSERTSTRUCT *tvis,TconfPageBase *page)
{
 addPropsIDs(page->propsIDs);
 return TffdshowPageBase::addTI(tvis,page);
}

void TffdshowPageEnc::onActivate(void)
{
 hcbxE=GetDlgItem(m_hwnd,IDC_CBX_ENCODER);
 cbxSetDroppedWidth(IDC_CBX_ENCODER,280);

 TVINSERTSTRUCT tvis;
 tvis.hParent=NULL;
 tvis.hInsertAfter=TVI_LAST;
 tvis.item.mask=TVIF_PARAM|TVIF_TEXT|TVIF_IMAGE|TVIF_SELECTEDIMAGE;
 static const int idPresets[]={IDC_BT_PRESET,IDC_CHB_NOT_REGISTRY,0};
 if (isAbout)
  {
   addTI(&tvis,new TaboutPageEnc(this));
   enable(false,idPresets);
  }
 else if (isGraph)
  {
   addTI(&tvis,new TgraphPage(this));
   enable(false,idPresets);
  }
 else
  {
   addTI(&tvis,TconfPages(new TgenericPage(this),
                          new TgenericHuffyuvPage(this),
                          new TgenericWmv9page(this),
                          new TgenericTHEOpage(this),
                          new TgenericRAWpage(this),
                          new TgenericFFV1page(this),
                          new TgenericLJPEGpage(this),
                          new TgenericX264page(this),
                          new TgenericDVpage(this),
                          NULL),&localCfg.codecId);
   addTI(&tvis,TconfPages(new TmePage(this),
                          new TmeXvidPage(this),
                          new TmeSkalPage(this),
                          new TmeX264page(this),
                          NULL),&localCfg.codecId);
   HTREEITEM htiQuant=addTI(&tvis,new TquantPage(this))->hti;
   tvis.hParent=htiQuant;
   addTI(&tvis,new TquantTablesPage(this));
   tvis.hParent=NULL;
   TreeView_Expand(htv,htiQuant,TVE_EXPAND);
   addTI(&tvis,TconfPages(new TmaskingPage(this),
                          new TmaskingPageXvid(this),
                          new TmaskingPageSkal(this),
                          new TmaskingPageTheo(this),
                          new TmaskingPageX264(this),
                          NULL),&localCfg.codecId);
   addTI(&tvis,TconfPages(new TonePassPage(this),
                          new TonePassXvidPage(this),
                          NULL),&localCfg.codecId);
   addTI(&tvis,new TcreditsPage(this));
   addTI(&tvis,new TfirstPassPage(this));
   HTREEITEM htiSecond=addTI(&tvis,new TsecondPassPage(this))->hti;
   tvis.hParent=htiSecond;
   addTI(&tvis,new TcurveNormalPage(this));
   addTI(&tvis,new TcurveAltPage(this));
   TreeView_Expand(htv,htiSecond,TVE_EXPAND);
   tvis.hParent=NULL;
   addTI(&tvis,new TinPage(this));
   addTI(&tvis,new ToutPage(this));
   addTI(&tvis,new TgraphPage(this));
   addTI(&tvis,new TdlgMiscPage(this));
   addTI(&tvis,new TaboutPageEnc(this));
  }
/*
 for (TconfPages::const_iterator p=pages.begin();p!=pages.end();p++)
  addPropsIDs((*p)->propsIDs);
*/
 static const int propsIDs[]={IDFF_enc_mode,IDFF_enc_bitrate1000,IDFF_enc_qual,IDFF_enc_desiredSize,IDFF_enc_codecId,IDFF_enc_fourcc,0};
 addPropsIDs(propsIDs);
 allPropsIDs.push_back(0);

 const TvideoCodecs *enclibs;deciE->getVideoCodecs(&enclibs);
 for (TvideoCodecs::const_iterator el=enclibs->begin();el!=enclibs->end();el++)
  for (Tencoders::const_iterator enc=(*el)->encoders.begin();enc!=(*el)->encoders.end();enc++)
   cbxAdd(IDC_CBX_ENCODER,(*el)->getName(),intptr_t(*enc));

 oldmode=-1;valIdff=0;
 if (!isAbout) codec2dlg();
 quick2dlg(true);

 setCheck(IDC_CHB_NOT_REGISTRY,cfgGet(IDFF_notreg));
}
void TffdshowPageEnc::onApplyChanges(void)
{
 deciE->saveEncodingSettings();
 *oldCoSettingsPtr=localCfg;
}
STDMETHODIMP TffdshowPageEnc::Deactivate(void)
{
 return TffdshowPageBase::Deactivate();
}

void TffdshowPageEnc::quick2dlg(bool redraw)
{
 static const int idQuickAll[]={IDC_LBL_ENCODER,IDC_CBX_ENCODER,IDC_LBL_FOURCC,IDC_CBX_FOURCC,IDC_LBL_MODES,IDC_CBX_MODES,IDC_LBL_QUICK,IDC_TBR_QUICK,IDC_ED_QUICK,0};
 if (isAbout)
  {
   enable(false,idQuickAll);
   return;
  }
 int mode=cfgGet(IDFF_enc_mode);
 int ii=cbxFindItemData(IDC_CBX_MODES,mode);
 if (ii==CB_ERR && cbxGetItemCount(IDC_CBX_MODES)>0)
  {
   ii=0;
   mode=(int)cbxGetItemData(IDC_CBX_MODES,0);
   cfgSet(IDFF_enc_mode,mode);
  };
 cbxSetCurSel(IDC_CBX_MODES,ii);
 static const int idQuick[]={IDC_LBL_QUICK,IDC_ED_QUICK,IDC_TBR_QUICK,0};
 if (ii!=CB_ERR)
  {
   if (redraw || oldmode!=mode)
    {
     oldmode=mode;
     const char_t *text=_l("");
     valMin=0;valMax=0;
     valIdff=0;
     switch (oldmode)
      {
       case ENC_MODE::CBR:
        text=_l("Bitrate (kbps)");
        valIdff=IDFF_enc_bitrate1000;
        valMin=1;valMax=10000;
        break;
       case ENC_MODE::VBR_QUAL:
        text=_l("Quality");
        valIdff=IDFF_enc_qual;
        valMin=0;valMax=100;
        break;
       case ENC_MODE::VBR_QUANT:
        text=_l("Quantizer");
        valIdff=IDFF_enc_quant;
        valMin=qmin;valMax=qmax;
        break;
       case ENC_MODE::PASS2_1:
       case ENC_MODE::PASS2_2_EXT:
        text=_l("No settings");
        break;
       case ENC_MODE::PASS2_2_INT:
        text=_l("Size (Kbytes)");
        valIdff=IDFF_enc_desiredSize;
        valMin=1;valMax=4*1024*1024;
        break;
      }
     setDlgItemText(m_hwnd,IDC_LBL_QUICK,_(IDC_LBL_QUICK,text));
     tbrSetRange(IDC_TBR_QUICK,valMin,valMax,(valMax-valMin)/10);
    }
   if (valIdff)
    {
     enable(1,idQuick);
     int val=cfgGet(valIdff);
     if (!isIn(val,valMin,valMax))
      {
       val=limit(val,valMin,valMax);
       cfgSet(valIdff,val);
      }
     tbrSet(IDC_TBR_QUICK,val);
     SetDlgItemInt(m_hwnd,IDC_ED_QUICK,val,FALSE);
    }
   else
    {
     enable(0,idQuick);
     tbrSet(IDC_TBR_QUICK,0);
     setDlgItemText(m_hwnd,IDC_ED_QUICK,_l(""));
    }
  }
 else
  enable(0,idQuick);

 if (page && redraw) selectPage(page);
 if (isGraph) enable(false,idQuickAll);
}

void TffdshowPageEnc::codec2dlg(void)
{
 codecId=(CodecID)cfgGet(IDFF_enc_codecId);
 SendDlgItemMessage(m_hwnd,IDC_CBX_FOURCC,CB_RESETCONTENT,0,0);
 int cnt=cbxGetItemCount(IDC_CBX_ENCODER);
 for (int i=0;i<cnt;i++)
  if (((Tencoder*)cbxGetItemData(IDC_CBX_ENCODER,i))->id==codecId)
   {
    cbxSetCurSel(IDC_CBX_ENCODER,i);
    const Tencoder *enc;
    deciE->getEncoder(codecId,&enc);
    if (enc)
     {
      for (Tfourccs::const_iterator f=enc->fourccs.begin();f!=enc->fourccs.end();f++)
       {
        char fccs[5]="1234";
        memcpy(fccs,&*f,4);
        if (fccs[0]) cbxAdd(IDC_CBX_FOURCC,text<char_t>(fccs),*f);
       }
      static const int idFourcc[]={IDC_LBL_FOURCC,IDC_CBX_FOURCC,0};
      if (cbxGetItemCount(IDC_CBX_FOURCC)==0)
       enable(false,idFourcc);
      else
       {
        enable(true,idFourcc);
        fourcc2dlg();
       }
      SendDlgItemMessage(m_hwnd,IDC_CBX_MODES,CB_RESETCONTENT,0,0);
      if (sup_CBR(codecId)) cbxAdd(IDC_CBX_MODES,_(IDC_CBX_MODES,encModeNames[ENC_MODE::CBR]),ENC_MODE::CBR);
      if (sup_VBR_QUAL(codecId)) cbxAdd(IDC_CBX_MODES,_(IDC_CBX_MODES,encModeNames[ENC_MODE::VBR_QUAL]),ENC_MODE::VBR_QUAL);
      if (sup_VBR_QUANT(codecId)) cbxAdd(IDC_CBX_MODES,_(IDC_CBX_MODES,encModeNames[ENC_MODE::VBR_QUANT]),ENC_MODE::VBR_QUANT);
      if (sup_XVID2PASS(codecId) && sup_perFrameQuant(codecId)) cbxAdd(IDC_CBX_MODES,_(IDC_CBX_MODES,encModeNames[ENC_MODE::PASS2_1]),ENC_MODE::PASS2_1);
      if (sup_XVID2PASS(codecId) && sup_perFrameQuant(codecId)) cbxAdd(IDC_CBX_MODES,_(IDC_CBX_MODES,encModeNames[ENC_MODE::PASS2_2_INT]),ENC_MODE::PASS2_2_INT);
      if (sup_XVID2PASS(codecId) && sup_perFrameQuant(codecId)) cbxAdd(IDC_CBX_MODES,_(IDC_CBX_MODES,encModeNames[ENC_MODE::PASS2_2_EXT]),ENC_MODE::PASS2_2_EXT);
      static const int idModes[]={IDC_LBL_MODES,IDC_CBX_MODES,0};
      enable(cbxGetItemCount(IDC_CBX_MODES)!=0,idModes);
      qmin=TcoSettings::getMinMaxQuant(codecId).first;qmax=TcoSettings::getMinMaxQuant(codecId).second;
      quick2dlg(true);
     }
    return;
   }
}
void TffdshowPageEnc::fourcc2dlg(void)
{
 FOURCC fourcc=cfgGet(IDFF_enc_fourcc);
 int cnt=cbxGetItemCount(IDC_CBX_FOURCC);
 for (int i=0;i<cnt;i++)
  if ((FOURCC)cbxGetItemData(IDC_CBX_FOURCC,i)==fourcc)
   {
    cbxSetCurSel(IDC_CBX_FOURCC,i);
    return;
   }
 cbxSetCurSel(IDC_CBX_FOURCC,0);
 cfgSet(IDFF_enc_fourcc,(int)cbxGetItemData(IDC_CBX_FOURCC,0));
}

INT_PTR TffdshowPageEnc::msgProc(UINT uMsg,WPARAM wParam,LPARAM lParam)
{
 switch (uMsg)
  {
   case WM_HSCROLL:
    if (HWND(lParam)==GetDlgItem(m_hwnd,IDC_TBR_QUICK))
     {
      if (valIdff)
       {
        cfgSet(valIdff,tbrGet(IDC_TBR_QUICK));
        quick2dlg(false);
       }
      return TRUE;
     }
    break;
   case WM_FFONCHANGE:
    if (wParam==IDFF_lang && ve)
     ve->commonOptionChanged(IID_IffdshowEnc,IDFF_lang);
    break;
   case WM_COMMAND:
    switch (LOWORD(wParam))
     {
      case IDC_CBX_ENCODER:
       if (HIWORD(wParam)==CBN_SELCHANGE)
        {
         cfgSet(IDFF_enc_codecId,((Tencoder*)cbxGetCurItemData(IDC_CBX_ENCODER))->id);
         codec2dlg();
         HTREEITEM hti=TreeView_GetSelection(htv);
         if (hti) selectPage(hti2page(hti));
         return TRUE;
        }
       break;
      case IDC_CBX_FOURCC:
       if (HIWORD(wParam)==CBN_SELCHANGE)
        {
         cfgSet(IDFF_enc_fourcc,(int)cbxGetCurItemData(IDC_CBX_FOURCC));
         fourcc2dlg();
         return TRUE;
        }
       break;
      case IDC_CBX_MODES:
       if (HIWORD(wParam)==CBN_SELCHANGE)
        {
         cfgSet(IDFF_enc_mode,(int)cbxGetCurItemData(IDC_CBX_MODES));
         quick2dlg(true);
         return TRUE;
        }
       break;
      case IDC_ED_QUICK:
       if (HIWORD(wParam)==EN_CHANGE && !isSetWindowText)
        {
         HWND hed=GetDlgItem(m_hwnd,LOWORD(wParam));
         if (hed!=GetFocus()) return FALSE;
         repaint(hed);
         switch (LOWORD(wParam))
          {
           case IDC_ED_QUICK:
            eval(hed,valMin,valMax,valIdff);
            tbrSet(IDC_TBR_QUICK,cfgGet(valIdff));
            break;
          }
         return TRUE;
        }
       break;
      case IDC_BT_PRESET:
       if (HIWORD(wParam)==BN_CLICKED)
        {
         showPresets();
         return TRUE;
        }
       break;
      case IDC_CHB_NOT_REGISTRY:
       cfgSet(IDFF_notreg,getCheck(IDC_CHB_NOT_REGISTRY));
       return TRUE;
     }
    break;
   case WM_CTLCOLOREDIT:
    {
     HWND hwnd=HWND(lParam);
     bool ok;
     switch (getId(hwnd))
      {
       case IDC_ED_QUICK:ok=eval(hwnd,valMin,valMax);break;
       default:return FALSE;
      }
     if (!ok)
      {
       HDC dc=HDC(wParam);
       SetBkColor(dc,RGB(255,0,0));
       return INT_PTR(getRed());
      }
     else return FALSE;
    }
   case WM_DRAWITEM:
    if (wParam==IDC_CBX_ENCODER)
     {
      DRAWITEMSTRUCT *dis=(DRAWITEMSTRUCT*)lParam;
      COLORREF crOldTextColor=GetTextColor(dis->hDC);
      COLORREF crOldBkColor=GetBkColor(dis->hDC);
      HBRUSH br;
      if ((dis->itemAction|ODA_SELECT) && (dis->itemState&ODS_SELECTED))
       {
        SetTextColor(dis->hDC,GetSysColor(COLOR_HIGHLIGHTTEXT));
        SetBkColor(dis->hDC,GetSysColor(COLOR_HIGHLIGHT));
        br=CreateSolidBrush(GetSysColor(COLOR_HIGHLIGHT));
       }
      else
       br=CreateSolidBrush(crOldBkColor);
      FillRect(dis->hDC,&dis->rcItem,br);
      if (int(dis->itemData)!=CB_ERR)
       {
        RECT r=dis->rcItem;r.left+=2;
        Tencoder *enc=(Tencoder*)dis->itemData;
        DrawText(dis->hDC,enc->name,(int)strlen(enc->name),&r,DT_LEFT|DT_SINGLELINE|DT_VCENTER);
        setDlgResult(TRUE);
        r.left+=180;
        FillRect(dis->hDC,&r,br);
        MoveToEx(dis->hDC,r.left,r.top,NULL);LineTo(dis->hDC,r.left,r.bottom);
        SetTextColor(dis->hDC,GetSysColor(COLOR_GRAYTEXT));
        r.left+=4;
        char_t text[70];SendMessage(hcbxE,CB_GETLBTEXT,dis->itemID,LPARAM(text));
        DrawText(dis->hDC,text,(int)strlen(text),&r,DT_LEFT|DT_SINGLELINE|DT_VCENTER);
       }
      SetTextColor(dis->hDC,crOldTextColor);
      SetBkColor(dis->hDC,crOldBkColor);
      DeleteObject(br);
      return TRUE;
     }
    break;
  }
 return TffdshowPageBase::msgProc(uMsg,wParam,lParam);
}

void TffdshowPageEnc::translate(void)
{
 TffdshowPageBase::translate();
 codec2dlg();
 quick2dlg(true);
}


HMENU TffdshowPageEnc::createPresetsSubmenu(int cmd,bool save)
{
 HMENU hm=CreatePopupMenu();
 int ord=0;
 for (TpresetsEnc::const_iterator p=presets->begin();p!=presets->end();p++)
  insertMenuItem(hm,ord,cmd++,p->name,false);
 if (save)
  {
   insertSeparator(hm,ord);
   insertMenuItem(hm,ord,(cmd/1000)*1000+999,_(-IDD_FFDSHOWENC,_l("Save as...")),false);
  }
 return hm;
}

void TffdshowPageEnc::showPresets(void)
{
 if (!presets)
  {
   presets=new TpresetsEnc;
   presets->init();
  }
 enum
  {
   IDI_MNI_LOAD_ALL =1000,
   IDI_MNI_LOAD_PAGE=2000,
   IDI_MNI_SAVE_ALL =3000,
   IDI_MNI_SAVE_PAGE=4000,
   IDI_MNI_MANAGE   =5000
  };
 HMENU hm=CreatePopupMenu();
 int ord=0;
 insertSubmenu(hm,ord,_(IDC_BT_PRESET,_l("Load all")),createPresetsSubmenu(IDI_MNI_LOAD_ALL,false));
 insertSubmenu(hm,ord,_(IDC_BT_PRESET,_l("Load page")),createPresetsSubmenu(IDI_MNI_LOAD_PAGE,false));enable(hm,1,pageIs && page->propsIDs?true:false);
 insertSeparator(hm,ord);
 HMENU hmPresets=createPresetsSubmenu(IDI_MNI_SAVE_ALL,true);enable(hmPresets,0,false);
 insertSubmenu(hm,ord,_(IDC_BT_PRESET,_l("Save all")),hmPresets);
 hmPresets=createPresetsSubmenu(IDI_MNI_SAVE_PAGE,true);enable(hmPresets,0,false);
 insertSubmenu(hm,ord,_(IDC_BT_PRESET,_l("Save page")),hmPresets);enable(hm,4,pageIs && page->propsIDs?true:false);
 insertSeparator(hm,ord);
 insertMenuItem(hm,ord,IDI_MNI_MANAGE,_(IDC_BT_PRESET,_l("Manage presets...")),false);enable(hm,6,presets->size()>=2);
 RECT r;
 GetWindowRect(GetDlgItem(m_hwnd,IDC_BT_PRESET),&r);
 POINT p={0,r.bottom-r.top};
 ClientToScreen(GetDlgItem(m_hwnd,IDC_BT_PRESET),&p);
 int cmd=TrackPopupMenu(hm,TPM_LEFTALIGN|TPM_TOPALIGN|TPM_RETURNCMD,p.x,p.y,0,m_hwnd,NULL);
 PostMessage(m_hwnd,WM_NULL,0,0);
 if (cmd!=0)
  if (cmd==5000)
   TpresetsEncDlg pdlg(m_hwnd,deci,*presets);
  else
   {
    int presetnum=cmd%1000;
    TpresetEnc *preset=NULL;
    if (presetnum==999)
     {
      char_t presetName[MAX_PATH]=_l("");
      if (inputString(_(IDC_BT_PRESET,_l("Save as new preset")),_(IDC_BT_PRESET,_l("Preset name")),presetName,MAX_PATH) && presetName[0])
       {
        preset=presets->getPreset(presetName);
        if (!preset)
         preset=presets->createPreset(presetName);
       }
     }
    else
     preset=&(*presets)[presetnum];
    if (preset)
     switch (cmd=cmd/1000)
      {
       case 1:
       case 2:
        preset->apply(&localCfg,cmd==1?&*allPropsIDs.begin():page->propsIDs);
        setChange();
        if (cmd==1) codec2dlg();
        selectPage(page);
        break;
       case 3:
       case 4:
        preset->store(&localCfg,cmd==3?&*allPropsIDs.begin():page->propsIDs);
        selectPage(page);
        presets->save();
        break;
      }
   }
 DestroyMenu(hm);
}

// -------------------- configure ----------------------
extern "C" void CALLBACK configureEnc(HWND hwnd,HINSTANCE hinst,LPTSTR lpCmdLine,int nCmdShow);
void CALLBACK configureEnc(HWND hwnd,HINSTANCE hinst,LPTSTR lpCmdLine,int nCmdShow)
{
 if (CoInitialize(NULL)!=S_OK) return;
 comptr<IffdshowEnc> deciE;
 if (CoCreateInstance(CLSID_FFDSHOWENC,NULL,CLSCTX_INPROC_SERVER,IID_IffdshowEnc,(void**)&deciE)==S_OK)
  {
   comptrQ<IffdshowBase> deci=deciE;
   deci->putParam(IDFF_filterMode,IDFF_FILTERMODE_ENC|IDFF_FILTERMODE_CONFIG);
   if (strstr(lpCmdLine,_l("top"))) deci->putParam(IDFF_alwaysOnTop,1);
   deci->showCfgDlg(NULL);
  }
 deciE->_release();
 CoUninitialize();
}
