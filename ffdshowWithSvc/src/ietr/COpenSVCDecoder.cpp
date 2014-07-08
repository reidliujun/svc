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
#include "COpenSVCDecoder.h"
#include "TvideoCodec.h"
#include "TaudioCodec.h"
#include "ffdshow_mediaguids.h"
#include "Tconfig.h"
#include "TffdshowPageDec.h"
#include "Tinfo.h"
#include "Ttranslate.h"
#include "TcompatibilityList.h"

//=================================== TInfoOpenSVCDecoder =======================================
void TInfoOpenSVCDecoder::init(void)
{
 setCheck(IDC_CHB_MMX     ,Tconfig::cpu_flags&FF_CPU_MMX     );
 setCheck(IDC_CHB_MMXEXT  ,Tconfig::cpu_flags&FF_CPU_MMXEXT  );
 setCheck(IDC_CHB_SSE     ,Tconfig::cpu_flags&FF_CPU_SSE     );
 setCheck(IDC_CHB_SSE2    ,Tconfig::cpu_flags&FF_CPU_SSE2    );
 setCheck(IDC_CHB_SSE3    ,Tconfig::cpu_flags&FF_CPU_SSE3    );
 setCheck(IDC_CHB_SSSE3   ,Tconfig::cpu_flags&FF_CPU_SSSE3   );
 setCheck(IDC_CHB_3DNOW   ,Tconfig::cpu_flags&FF_CPU_3DNOW   );
 setCheck(IDC_CHB_3DNOWEXT,Tconfig::cpu_flags&FF_CPU_3DNOWEXT);
 
 hlv=GetDlgItem(m_hwnd,IDC_LV_INFO); 
 CRect r=getChildRect(IDC_LV_INFO);
 int ncol=0;
 ListView_AddCol(hlv,ncol,r.Width(),_l("Property"),false);
 ListView_SetExtendedListViewStyleEx(hlv,LVS_EX_FULLROWSELECT|LVS_EX_INFOTIP,LVS_EX_FULLROWSELECT|LVS_EX_INFOTIP);
 infoitems.clear(); 
 const int *infos=getInfos();
 for (int i=0;;i++)
  {
   Titem it;
   if (!info->getInfo(i,&it.id,&it.name))
    break;
   for (int j=0;infos[j];j++) 
    if (infos[j]==it.id)
     {
      it.index=j;
      infoitems.push_back(it);
     } 
  }
 std::sort(infoitems.begin(),infoitems.end(),TsortItem(infos)); 
 ListView_SetItemCount(hlv,infoitems.size()); 
 SendMessage(hlv,LVM_SETBKCOLOR,0,GetSysColor(COLOR_BTNFACE));
 SendMessage(hlv,LVM_SETTEXTBKCOLOR,0,GetSysColor(COLOR_BTNFACE));
 setCheck(IDC_CHB_WRITEINFO2DBG, cfgGet(IDFF_allowDPRINTF));
}

void TInfoOpenSVCDecoder::cfg2dlg(void)
{
 int allow=cfgGet(IDFF_allowedCpuFlags);
 setCheck(IDC_CHB_ALLOW_MMX     ,allow&FF_CPU_MMX     );
 setCheck(IDC_CHB_ALLOW_MMXEXT  ,allow&FF_CPU_MMXEXT  );
 setCheck(IDC_CHB_ALLOW_SSE     ,allow&FF_CPU_SSE     );
 setCheck(IDC_CHB_ALLOW_SSE2    ,allow&FF_CPU_SSE2    );
 setCheck(IDC_CHB_ALLOW_SSE3    ,allow&FF_CPU_SSE3    );
 setCheck(IDC_CHB_ALLOW_SSSE3   ,allow&FF_CPU_SSSE3   );
 setCheck(IDC_CHB_ALLOW_3DNOW   ,allow&FF_CPU_3DNOW   );
 setCheck(IDC_CHB_ALLOW_3DNOWEXT,allow&FF_CPU_3DNOWEXT);
}

INT_PTR TInfoOpenSVCDecoder::msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
 switch (uMsg)
  {
   case WM_COMMAND:
    switch (LOWORD(wParam))  
     {
      case IDC_CHB_ALLOW_MMX:
      case IDC_CHB_ALLOW_MMXEXT:
      case IDC_CHB_ALLOW_SSE:
      case IDC_CHB_ALLOW_SSE2:
      case IDC_CHB_ALLOW_SSE3:
      case IDC_CHB_ALLOW_SSSE3:
      case IDC_CHB_ALLOW_3DNOW:
      case IDC_CHB_ALLOW_3DNOWEXT:
       {
        int allow=0;
        if (getCheck(IDC_CHB_ALLOW_MMX     )) allow|=FF_CPU_MMX;
        if (getCheck(IDC_CHB_ALLOW_MMXEXT  )) allow|=FF_CPU_MMXEXT;
        if (getCheck(IDC_CHB_ALLOW_SSE     )) allow|=FF_CPU_SSE;
        if (getCheck(IDC_CHB_ALLOW_SSE2    )) allow|=FF_CPU_SSE2;
        if (getCheck(IDC_CHB_ALLOW_SSE3    )) allow|=FF_CPU_SSE3;
        if (getCheck(IDC_CHB_ALLOW_SSSE3   )) allow|=FF_CPU_SSSE3;
        if (getCheck(IDC_CHB_ALLOW_3DNOW   )) allow|=FF_CPU_3DNOW;
        if (getCheck(IDC_CHB_ALLOW_3DNOWEXT)) allow|=FF_CPU_3DNOWEXT;
        cfgSet(IDFF_allowedCpuFlags,allow);
        return TRUE;
       }
     }    
    break;
   case WM_NOTIFY:
    {
     NMHDR *nmhdr=LPNMHDR(lParam);
     if (nmhdr->hwndFrom==hlv && nmhdr->idFrom==IDC_LV_INFO)
      switch (nmhdr->code)
       {
        case LVN_GETDISPINFO:
         {
          NMLVDISPINFO *nmdi=(NMLVDISPINFO*)lParam;
          int i=nmdi->item.iItem;
          if (i==-1) break;
          if (nmdi->item.mask&LVIF_TEXT)
           switch (nmdi->item.iSubItem)
            {
             case 0:
              {
               nmdi->item.pszText = pszTextBuf;
               tsnprintf_s(pszTextBuf,countof(pszTextBuf),_TRUNCATE,_l("%s: %s"),infoitems[i].translatedName,infoitems[i].val?infoitems[i].val:_l(""));
               break;
              }
            }
          return TRUE;
         }
       }  
     break;  
    }  
  }
 return TconfPageDec::msgProc(uMsg,wParam,lParam);
}

void TInfoOpenSVCDecoder::onFrame(void)
{
 if (!IsWindowVisible(m_hwnd)) return;
 for (Titems::iterator i=infoitems.begin();i!=infoitems.end();i++)
  {
   int wasChange;
   i->val=info->getVal(i->id,&wasChange,NULL);
   if (wasChange)
    ListView_Update(hlv,i-infoitems.begin());
  } 
}

void TInfoOpenSVCDecoder::onAllowDPRINTF(void)
{
 cfgSet(IDFF_allowDPRINTFchanged,1);
}

void TInfoOpenSVCDecoder::translate(void)
{
 TconfPageDec::translate();

 for (Titems::iterator i=infoitems.begin();i!=infoitems.end();i++)
  i->translatedName=_(IDC_LV_INFO,i->name);
}

TInfoOpenSVCDecoder::TInfoOpenSVCDecoder(TffdshowPageDec *Iparent,TinfoBase *Iinfo):TconfPageDec(Iparent,NULL,0),info(Iinfo)
{
 dialogId=IDD_OPENSVCDECODER;

 static const TbindCheckbox<TInfoOpenSVCDecoder> chb[]=
  {
   IDC_CHB_WRITEINFO2DBG,IDFF_allowDPRINTF,&TInfoOpenSVCDecoder::onAllowDPRINTF,
   0,NULL,NULL
  };
 bindCheckboxes(chb);
}
TInfoOpenSVCDecoder::~TInfoOpenSVCDecoder()
{
 delete info;
}
//================================= TInfoOpenSVCDecoderVideo ====================================
TInfoOpenSVCDecoderVideo::TInfoOpenSVCDecoderVideo(TffdshowPageDec *Iparent):TInfoOpenSVCDecoder(Iparent,new TinfoDecVideo(Iparent->deci))
{
}

//================================= TInfoOpenSVCDecoderAudio ====================================
TInfoOpenSVCDecoderAudio::TInfoOpenSVCDecoderAudio(TffdshowPageDec *Iparent):TInfoOpenSVCDecoder(Iparent,new TinfoDecAudio(Iparent->deci))
{
}
