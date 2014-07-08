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
#include "Ccodecs.h"
#include "IffdshowDecVideo.h"
#include "TffdshowPageDec.h"
#include "Tconfig.h"
#include "ffdshow_mediaguids.h"
#include "TsampleFormat.h"
#include "Tcodec.h"
#include "ffImgfmt.h"

//============================== TcodecsPage::Tformat ===============================
TcodecsPage::Tformat::Tformat(const char_t *Idescr,int Iidff,const int *movie_sources,const char_t *Ihint,const Toption *Ioptions,LAVC_STATUS Ilavc_status):
 descr(Idescr),
 hint(Ihint),
 idff(Iidff),
 lavc_status(Ilavc_status)
{
 for (;*movie_sources;movie_sources++)
  decoders.push_back(Tdecoder::init(Tcodec::getMovieSourceName(*movie_sources),*movie_sources));
 if (Ioptions)
  for (;Ioptions->name;Ioptions++)
   options.push_back(*Ioptions);
}
TcodecsPage::Tformat::Tformat(const char_t *Idescr,int Iidff,int movie_source,const char_t *Ihint,const Toption *Ioptions,LAVC_STATUS Ilavc_status):
 descr(Idescr),
 hint(Ihint),
 idff(Iidff),
 lavc_status(Ilavc_status)
{
 decoders.push_back(Tdecoder::init(Tcodec::getMovieSourceName(movie_source),movie_source));
 if (Ioptions)
  for (;Ioptions->name;Ioptions++)
   options.push_back(*Ioptions);
}
TcodecsPage::Tformat::Tformat(const char_t *Idescr,int Iidff,const char_t *Ihint,LAVC_STATUS Ilavc_status):
 descr(Idescr),
 hint(Ihint),
 idff(Iidff),
 lavc_status(Ilavc_status)
{
}
TcodecsPage::Tformat::Tformat(const char_t *Idescr,int Iidff,const Tdecoder *Idecoders,const char_t *Ihint,const Toption *Ioptions,LAVC_STATUS Ilavc_status):
 descr(Idescr),
 hint(Ihint),
 idff(Iidff),
 lavc_status(Ilavc_status)
{
 for (;Idecoders->name;Idecoders++)
  decoders.push_back(*Idecoders);
 if (Ioptions)
  for (;Ioptions->name;Ioptions++)
   options.push_back(*Ioptions);
}

bool TcodecsPage::Tformat::decoderCheck(const Tconfig *config)
{
 for (Tdecoders::iterator d=decoders.begin();d!=decoders.end();)
  if (!config->isDecoder[d->id])
   d=decoders.erase(d);
  else
   d++;
 return decoders.size()>0;
}
const char_t* TcodecsPage::Tformat::getDecoderName(int val) const
{
 for (Tdecoders::const_iterator d=decoders.begin();d!=decoders.end();d++)
  if (d->id==val)
   return d->name;
 return _l("");
}

//=================================== TcodecsPage ===================================
TcodecsPage::TcodecsPage(TffdshowPageBase *Iparent):TconfPageBase(Iparent)
{
 dialogId=IDD_CODECS;hlv=NULL;setHints=-1;
}
LRESULT TcodecsPage::TwidgetCodecsLv::onKeyDown(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
 switch (wParam)
  {
   case VK_SPACE:
   case VK_RETURN:
    codecsPage->beginCodecChange(codecsPage->lvGetSelItem(IDC_LV_INCODECS));
    return 0;
  }
 return TwidgetCodecs::onKeyDown(hwnd,uMsg,wParam,lParam);
}
LRESULT TcodecsPage::TwidgetCodecsLv::onVscroll(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
 if (IsWindowVisible(codecsPage->hcbx))
  {
   codecsPage->show(false,IDC_CBX_INCODECS);
   //repaint(codecsPage->hcbx);
   repaint(hwnd);
  }
 return TwidgetCodecs::onVscroll(hwnd,uMsg,wParam,lParam);
}
LRESULT TcodecsPage::TwidgetCodecsLv::onHscroll(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
 if (IsWindowVisible(codecsPage->hcbx))
  {
   codecsPage->show(false,IDC_CBX_INCODECS);
   //repaint(codecsPage->hcbx);
   repaint(hwnd);
  }
 return TwidgetCodecs::onHscroll(hwnd,uMsg,wParam,lParam);
}
LRESULT TcodecsPage::TwidgetCodecsLv::onPaint(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
 if (IsWindowVisible(codecsPage->hcbx))
  {
   repaint(codecsPage->hcbx);
   repaint(hwnd);
  }
 return TwidgetCodecs::onPaint(hwnd,uMsg,wParam,lParam);
}
LRESULT TcodecsPage::TwidgetCodecsLv::onRbuttonDown(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
 static const char_t *items[]=
  {
   _l("Set all stable formats to libavcodec"),
   _l("Set all supported formats to Xvid"),
   NULL
  };
 int cmd=codecsPage->selectFromMenu(items,0);
 switch (cmd)
  {
   case 0:
	   for (TcodecsPage::Tformats::const_iterator f=codecsPage->formats.begin();f!=codecsPage->formats.end()-1;f++){
		if (f->lavc_status==LAVC_STABLE && f->decoders.find(IDFF_MOVIE_LAVC)!=-1)
		  codecsPage->cfgSet(f->idff,IDFF_MOVIE_LAVC);
		if (f->lavc_status==LAVC_STABLE && f->decoders.find(IDFF_MOVIE_LSVC)!=-1)
		  codecsPage->cfgSet(f->idff,IDFF_MOVIE_LSVC);
	   }
    repaint(hwnd);
    break;
   case 1:
    for (TcodecsPage::Tformats::const_iterator f=codecsPage->formats.begin();f!=codecsPage->formats.end()-1;f++)
     if (f->decoders.find(IDFF_MOVIE_XVID4)!=-1)
      codecsPage->cfgSet(f->idff,IDFF_MOVIE_XVID4);
    repaint(hwnd);
    break;
  }
 return TwidgetCodecs::onRbuttonDown(hwnd,uMsg,wParam,lParam);
}

LPARAM TcodecsPage::TwidgetCodecsCbx::onKillFocus(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
 codecsPage->show(false,IDC_CBX_INCODECS);
 return TwidgetCodecs::onKillFocus(hwnd,uMsg,wParam,lParam);
}
LPARAM TcodecsPage::TwidgetCodecsCbx::onGetDlgCode(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
 MSG *msg=(MSG*)lParam;
 if (msg && (msg->message==WM_KEYDOWN || msg->message==WM_KEYUP) && (msg->wParam==VK_RETURN || msg->wParam==VK_ESCAPE))
  {
   codecsPage->show(false,IDC_CBX_INCODECS);
   SetFocus(codecsPage->hlv);
   msg->message=WM_NULL;
  }
 return DLGC_WANTALLKEYS;
}

void TcodecsPage::initLV(const char_t *IthirdCol)
{
 hlv=GetDlgItem(m_hwnd,IDC_LV_INCODECS);
 ListView_SetExtendedListViewStyleEx(hlv,LVS_EX_FULLROWSELECT|LVS_EX_INFOTIP|LVS_EX_GRIDLINES,LVS_EX_FULLROWSELECT|LVS_EX_INFOTIP|LVS_EX_GRIDLINES);
 int ncol=0;
 ListView_AddCol(hlv,ncol,cfgGet(IDFF_lvCodecsWidth0),_(IDC_LV_INCODECS,_l("Format" )),false);
 ListView_AddCol(hlv,ncol,cfgGet(IDFF_lvCodecsWidth1),_(IDC_LV_INCODECS,_l("Decoder")),false);
 ListView_AddCol(hlv,ncol,cfgGet(IDFF_lvCodecsWidth2),_(IDC_LV_INCODECS,thirdCol=IthirdCol?IthirdCol:_l("Details")),false);

 hcbx=GetDlgItem(m_hwnd,IDC_CBX_INCODECS);

 formats.clear();fillCodecs();

 ListView_SetItemCount(hlv,formats.size());

 curitem=cfgGet(IDFF_lvCodecsSelected);
 lvSetSelItem(IDC_LV_INCODECS,curitem);
 ListView_EnsureVisible(hlv,curitem,FALSE);
 options2dlg(curitem);

 if (setHints!=-1)
  enableHints(!!setHints);
}

void TcodecsPage::enableHints(bool is)
{
 if (hlv)
  ListView_SetExtendedListViewStyleEx(hlv,LVS_EX_INFOTIP,is?LVS_EX_INFOTIP:0);
 else
  setHints=is?1:0;
 TconfPageBase::enableHints(is);
}

void TcodecsPage::moveCBX(bool isscroll)
{
 if (isscroll && !IsWindowVisible(hcbx)) return;
 LVCOLUMN lvc0,lvc1;
 lvc0.mask=lvc1.mask=LVCF_WIDTH;
 ListView_GetColumn(hlv,0,&lvc0);ListView_GetColumn(hlv,1,&lvc1);
 RECT lvr;GetWindowRect(hlv,&lvr);
 RECT pr;GetWindowRect(GetParent(hlv),&pr);
 OffsetRect(&lvr,-pr.left,-pr.top);
 RECT ir;
 int iItem=lvGetSelItem(IDC_LV_INCODECS);
 ListView_GetItemRect(hlv,iItem,&ir,LVIR_BOUNDS);
 MoveWindow(hcbx,lvr.left+lvc0.cx+3,lvr.top+ir.top-1,lvc1.cx,12,TRUE);
 show(true,IDC_CBX_INCODECS);
 SetWindowPos(hcbx,HWND_TOP,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_SHOWWINDOW);
 SetFocus(hcbx);
}
void TcodecsPage::beginCodecChange(int iItem)
{
 if (iItem<0 || iItem>=(int)formats.size())
  {
   show(false,IDC_CBX_INCODECS);
   curitem=-1;
  }
 else
  {
   cbxClear(IDC_CBX_INCODECS);
   for (Tformat::Tdecoders::const_iterator d=formats[iItem].decoders.begin();d!=formats[iItem].decoders.end();d++)
    cbxAdd(IDC_CBX_INCODECS,_(IDC_CBX_INCODECS,d->name),d->id);
   cbxSetDataCurSel(IDC_CBX_INCODECS,cfgGet(formats[iItem].idff));
   moveCBX(false);
   curitem=iItem;
  }
}

void TcodecsPage::options2dlg(int i)
{
 static const int idOptions[]={IDC_CHB_CODEC_OPT1,IDC_CHB_CODEC_OPT2, IDC_CHB_CODEC_OPT3, IDC_CHB_CODEC_OPT4,0};
 show(0,idOptions);
 const Tformat *f=isIn(i,0,(int)formats.size()-1)?&formats[i]:NULL;
 for (size_t ii=0;f && ii<f->options.size();ii++)
  {
   show(1,idOptions[ii]);
   const Tformat::Toption &o=f->options[ii];
   setText(idOptions[ii],_(idOptions[ii],o.name));
   setCheck(idOptions[ii],cfgGet(o.id)&o.val);
   enable(o.forCodec==0 || cfgGet(f->idff)==o.forCodec,idOptions[ii]);
  }
}
void TcodecsPage::dlg2options(int id)
{
 int ii=lvGetSelItem(IDC_LV_INCODECS);
 if (isIn(ii,0,(int)formats.size()))
  {
   int optionId = 0;
   switch(id)
   {
	   case IDC_CHB_CODEC_OPT1:
		optionId=0;break;
	   case IDC_CHB_CODEC_OPT2:
		optionId=1;break;
	   case IDC_CHB_CODEC_OPT3:
		optionId=2;break;
	   case IDC_CHB_CODEC_OPT4:
		optionId=3;break;
   }
	  const Tformat::Toption &o=formats[ii].options[optionId];
   int idff=o.id;
   int val=cfgGet(idff);
   if (getCheck(id))
    val|=o.val;
   else
    val&=~o.val;
   cfgSet(idff,val);
  }
}

INT_PTR TcodecsPage::msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
 switch (uMsg)
  {
   case WM_DESTROY:
    cfgSet(IDFF_lvCodecsWidth0,ListView_GetColumnWidth(hlv,0));
    cfgSet(IDFF_lvCodecsWidth1,ListView_GetColumnWidth(hlv,1));
    cfgSet(IDFF_lvCodecsWidth2,ListView_GetColumnWidth(hlv,2));
    cfgSet(IDFF_lvCodecsSelected,lvGetSelItem(IDC_LV_INCODECS));
    break;
   case WM_COMMAND:
    switch (LOWORD(wParam))
     {
      case IDC_CBX_INCODECS:
       if (HIWORD(wParam)==CBN_SELCHANGE && IsWindowVisible(hcbx))
        {
         int ii=lvGetSelItem(IDC_LV_INCODECS);
         int idff=formats[ii].idff;
         cfgSet(idff,(int)cbxGetCurItemData(IDC_CBX_INCODECS));
         options2dlg(ii);
         return TRUE;
        }
       break;
      case IDC_CHB_CODEC_OPT1:
      case IDC_CHB_CODEC_OPT2:
      case IDC_CHB_CODEC_OPT3:
      case IDC_CHB_CODEC_OPT4:
       dlg2options(LOWORD(wParam));
       return TRUE;
     }
    break;
   case WM_NOTIFY:
    {
     NMHDR *nmhdr=LPNMHDR(lParam);
     if (nmhdr->hwndFrom==hlv && nmhdr->idFrom==IDC_LV_INCODECS)
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
              strcpy(nmdi->item.pszText,_(IDC_LV_INCODECS,formats[i].descr));
              break;
             case 1:
              strcpy(nmdi->item.pszText,_(IDC_LV_INCODECS,formats[i].getDecoderName(cfgGet(formats[i].idff))));
              break;
             case 2:
              strcpy(nmdi->item.pszText,_(IDC_LV_INCODECS,formats[i].hint));
              break;
            }
          return TRUE;
         }
        case LVN_GETINFOTIP:
         {
          NMLVGETINFOTIP *nmit=(NMLVGETINFOTIP*)lParam;
          if (nmit->iItem!=-1 && nmit->iSubItem==0)
           ff_strncpy(nmit->pszText,formats[nmit->iItem].hint,nmit->cchTextMax);
          return TRUE;
         }
        case LVN_ITEMCHANGED:
         {
          NMLISTVIEW *nmlv=LPNMLISTVIEW(lParam);
          if (nmlv->iItem!=curitem)
           {
            curitem=-1;
            show(false,IDC_CBX_INCODECS);
            options2dlg(nmlv->iItem);
           }
          return TRUE;
         }
        case NM_CLICK:
         {
          NMITEMACTIVATE *nmia=LPNMITEMACTIVATE(lParam);
          if (nmia->iItem!=-1 && nmia->iSubItem==1)
           beginCodecChange(nmia->iItem);
          else
           show(false,IDC_CBX_INCODECS);
          return TRUE;
         }
       }
     break;
    }
  }
 return TconfPageBase::msgProc(uMsg,wParam,lParam);
}
Twidget* TcodecsPage::createDlgItem(int id,HWND h)
{
 if (id==IDC_LV_INCODECS)
  return new TwidgetCodecsLv(h,this);
 else if (id==IDC_CBX_INCODECS)
  return new TwidgetCodecsCbx(h,this);
 else
  return TconfPageBase::createDlgItem(id,h);
}

void TcodecsPage::translate(void)
{
 TconfPageBase::translate();
 ListView_SetColumnText(hlv,0,_(IDC_LV_INCODECS,_l("Format")));
 ListView_SetColumnText(hlv,1,_(IDC_LV_INCODECS,_l("Decoder")));
 ListView_SetColumnText(hlv,2,_(IDC_LV_INCODECS,thirdCol));
 options2dlg(lvGetSelItem(IDC_LV_INCODECS));
}

//================================= TcodecsPageVideo ===================================
TcodecsPageVideo::TcodecsPageVideo(TffdshowPageDec *Iparent):TcodecsPage(Iparent)
{
// helpURL="in_out.html";
}
void TcodecsPageVideo::init(void)
{
 initLV(_l("Supported FOURCCs/remarks"));
}
void TcodecsPageVideo::fillCodecs(void)
{
 if ((filterMode&IDFF_FILTERMODE_VIDEORAW)==0)
  {
   static const int movies_AVC[]={IDFF_MOVIE_LAVC,IDFF_MOVIE_LSVC,0};
   static const Tformat::Toption options_h264[]={{IDFF_MOVIE_LAVC,_l("skip deblocking when safe"),IDFF_fastH264,1},{IDFF_MOVIE_LAVC,_l("skip deblocking always"),IDFF_fastH264,2},{0,NULL,0,0}};
   formats.push_back(Tformat(_l("H.264/AVC")     ,IDFF_h264, movies_AVC,_l("H264, SVC, X264, VSSH (incomplete), DAVC, PAVC, AVC1"),options_h264));
   static const int movies_mpeg4[]={IDFF_MOVIE_LAVC,IDFF_MOVIE_XVID4,0};
   formats.push_back(Tformat(_l("Xvid")          ,IDFF_xvid,movies_mpeg4,_l("XVID, XVIX")));
   formats.push_back(Tformat(_l("DivX 4/5/6")    ,IDFF_dx50,movies_mpeg4,_l("DIVX, DX50")));
   formats.push_back(Tformat(_l("Other MPEG4")   ,IDFF_mp4v,movies_mpeg4,_l("MP4V, 3IVX, 3IV1, 3IV2, MP4S, M4S2, RMP4, DM4V, WV1F, FMP4, HDX4, SMP4, LMP4, NDIG, SEDG, BLZ0, DXGM")));
   formats.push_back(Tformat(_l("DivX 3")        ,IDFF_div3,IDFF_MOVIE_LAVC,_l("DIV3, DIV4, DIV5, DIV6, AP41, COL1, DVX3, MPG3")));
   formats.push_back(Tformat(_l("FVFW,FFDS")     ,IDFF_fvfw,movies_mpeg4,_l("ffdshow and ffvfw internal FOURCCs")));
   formats.push_back(Tformat(_l("MP43")          ,IDFF_mp43,IDFF_MOVIE_LAVC,_l("MP43")));
   formats.push_back(Tformat(_l("MP42")          ,IDFF_mp42,IDFF_MOVIE_LAVC,_l("MP42, DIV2")));
   formats.push_back(Tformat(_l("MP41")          ,IDFF_mp41,IDFF_MOVIE_LAVC,_l("MP41, MPG4, DIV1")));
   formats.push_back(Tformat(_l("H.263 (+)")     ,IDFF_h263,IDFF_MOVIE_LAVC,_l("H263, L263, M263, U263, X263, S263 (in 3gp files)")));
   formats.push_back(Tformat(_l("FLV1")          ,IDFF_flv1,IDFF_MOVIE_LAVC,_l("Flash Video (FLV1)")));
   static const int movies_theora[]={IDFF_MOVIE_LAVC,IDFF_MOVIE_THEO,0};
   static const Tformat::Toption options_theora[]={{IDFF_MOVIE_THEO,_l("Internal postprocessing"),IDFF_libtheoraPostproc,1},{0,NULL,0}};
   formats.push_back(Tformat(_l("Theora")        ,IDFF_theo,movies_theora,_l("Theora Video (THEO)"),options_theora));
   formats.push_back(Tformat(_l("VP3")           ,IDFF_vp3 ,IDFF_MOVIE_LAVC,_l("VP3 Video (VP31)")));
   formats.push_back(Tformat(_l("VP5")           ,IDFF_vp5 ,IDFF_MOVIE_LAVC,_l("VP5 Video (VP50)")));
   formats.push_back(Tformat(_l("VP6")           ,IDFF_vp6 ,IDFF_MOVIE_LAVC,_l("VP6 Video (VP60, VP61, VP62)")));
   formats.push_back(Tformat(_l("VP6F")          ,IDFF_vp6f,IDFF_MOVIE_LAVC,_l("Flash Video (VP6F, FLV4, VP6A)")));
   static const int movies_mpeg12[]={IDFF_MOVIE_LAVC,IDFF_MOVIE_LIBMPEG2,0};
   static const Tformat::Toption options_mpeg12[]={{IDFF_MOVIE_LAVC,_l("use speedup tricks"),IDFF_fastMpeg2,1},{0,NULL,0}};
   if ((filterMode&IDFF_FILTERMODE_VFW)==0)
    {
     formats.push_back(Tformat(_l("MPEG1")       ,IDFF_mpg1,movies_mpeg12,_l("MPEG1 codec"),options_mpeg12,LAVC_UNSTABLE));
     static const Tformat::Toption options_mpeg2[]={{IDFF_MOVIE_LAVC,_l("use speedup tricks"),IDFF_fastMpeg2,1},{0,_l("DVD decoding"),IDFF_supDVDdec,1},{0,NULL,0}};
     formats.push_back(Tformat(_l("MPEG2")       ,IDFF_mpg2,movies_mpeg12,_l("MPEG2 codec"),options_mpeg2,LAVC_UNSTABLE));
    }
   formats.push_back(Tformat(_l("MPEG in AVI")   ,IDFF_mpegAVI,movies_mpeg12,_l("MPEG, MPG1, MPG2, 1001, 1002"),options_mpeg12));
   formats.push_back(Tformat(_l("Other MPEG2")   ,IDFF_em2v,movies_mpeg12,_l("EM2V, MMES"),options_mpeg12));
   static const int movies_wmv1[]={IDFF_MOVIE_LAVC,IDFF_MOVIE_WMV9,0};
   formats.push_back(Tformat(_l("WMV1/7")        ,IDFF_wmv1,movies_wmv1,_l("Windows Media Video 7 (WMV1)"),NULL,LAVC_UNSTABLE));
   static const int movies_wmv2[]={IDFF_MOVIE_LAVC,IDFF_MOVIE_WMV9,0};
   formats.push_back(Tformat(_l("WMV2/8")        ,IDFF_wmv2,movies_wmv2,_l("Windows Media Video 8 (WMV2)"),NULL,LAVC_UNSTABLE));
   static const int movies_wmv3[]={IDFF_MOVIE_LAVC,IDFF_MOVIE_WMV9,0};
   formats.push_back(Tformat(_l("WMV3/9")        ,IDFF_wmv3,movies_wmv3,_l("Windows Media Video 9 Simple & Main Profiles (WMV3)"),NULL,LAVC_UNSTABLE));
   static const int movies_wvc1[]={IDFF_MOVIE_LAVC,IDFF_MOVIE_WMV9,0};
   static const Tformat::Toption options_vc1[]={{0,_l("discard telecine"),IDFF_autodetect24P,1},{0,NULL,0,0}};
   formats.push_back(Tformat(_l("VC-1")          ,IDFF_wvc1,movies_wvc1,_l("Windows Media Video 9 Advanced Profile (WVC1, WMVA)"),options_vc1,LAVC_UNSTABLE));
   formats.push_back(Tformat(_l("WMVP")          ,IDFF_wvp2,IDFF_MOVIE_WMV9,_l("Windows Media Video 9 Image (WMVP, WVP2)")));
   formats.push_back(Tformat(_l("MSS1/2")        ,IDFF_mss2,IDFF_MOVIE_WMV9,_l("Windows Screen Codecs (MSS1, MSS2)")));
   formats.push_back(Tformat(_l("H.261")         ,IDFF_h261,IDFF_MOVIE_LAVC,_l("H261, M261")));
   formats.push_back(Tformat(_l("Huffyuv")       ,IDFF_hfyu,IDFF_MOVIE_LAVC,_l("Huffyuv codec for libavcodec (HFYU, FFVH)")));
   formats.push_back(Tformat(_l("MJPEG")         ,IDFF_mjpg,IDFF_MOVIE_LAVC,_l("MJPG, LJPG, MJLS")));
   formats.push_back(Tformat(_l("Other MJPEG")   ,IDFF_avrn,IDFF_MOVIE_LAVC,_l("AVRN, MJPA, SP5X, AMVV")));
   formats.push_back(Tformat(_l("SVQ1")          ,IDFF_svq1,IDFF_MOVIE_LAVC,_l("Sorenson Video (SVQ1)")));
   formats.push_back(Tformat(_l("SVQ3")          ,IDFF_svq3,IDFF_MOVIE_LAVC,_l("Sorenson Video (SVQ3)")));
   formats.push_back(Tformat(_l("8BPS")          ,IDFF_8bps,IDFF_MOVIE_LAVC,_l("Quicktime Planar RGB Video (8BPS)")));
   formats.push_back(Tformat(_l("QTRLE")         ,IDFF_qtrle,IDFF_MOVIE_LAVC,_l("Quicktime Animation Video (QTRLE)")));
   formats.push_back(Tformat(_l("QRPZA")         ,IDFF_qtrpza,IDFF_MOVIE_LAVC,_l("Quicktime Video Video (QRPZA)")));
   formats.push_back(Tformat(_l("FFV1")          ,IDFF_ffv1,IDFF_MOVIE_LAVC,_l("FFV1 codec for libavcodec (FFV1)")));
   formats.push_back(Tformat(_l("DV")            ,IDFF_dvsd,IDFF_MOVIE_LAVC,_l("dvsd, DVSD, dv25, DV25, dv50, DV50")));
   formats.push_back(Tformat(_l("Other DV")      ,IDFF_cdvc,IDFF_MOVIE_LAVC,_l("CDVC, CDV5, DVIS, PDVC")));
   formats.push_back(Tformat(_l("CamStudio")     ,IDFF_cscd,IDFF_MOVIE_LAVC,_l("CamStudio Video (CSCD)")));
   formats.push_back(Tformat(_l("CorePNG")       ,IDFF_png1,IDFF_MOVIE_LAVC,_l("CorePNG (PNG1, MPNG)")));
   formats.push_back(Tformat(_l("LOCO")          ,IDFF_loco,IDFF_MOVIE_LAVC,_l("LOCO Video (LOCO)")));
   formats.push_back(Tformat(_l("MSZH")          ,IDFF_mszh,IDFF_MOVIE_LAVC,_l("LCL LossLess Codec Library (MSZH)")));
   formats.push_back(Tformat(_l("Techsmith")     ,IDFF_tscc,IDFF_MOVIE_LAVC,_l("TechSmith Camtasia (TSCC)")));
   formats.push_back(Tformat(_l("ZLIB")          ,IDFF_zlib,IDFF_MOVIE_LAVC,_l("ZLIB LossLess Codec Library (ZLIB)")));
   formats.push_back(Tformat(_l("ZMBV")          ,IDFF_zmbv,IDFF_MOVIE_LAVC,_l("Zip Motion Blocks Video (ZMBV)")));
   formats.push_back(Tformat(_l("Autodesc RLE")  ,IDFF_aasc,IDFF_MOVIE_LAVC,_l("Autodesc RLE (AASC)")));
   formats.push_back(Tformat(_l("ASV1/2")        ,IDFF_asv1,IDFF_MOVIE_LAVC,_l("ASUS V1/V2 (ASV1, ASV2)")));
   formats.push_back(Tformat(_l("ATI VCR1")      ,IDFF_vcr1,IDFF_MOVIE_LAVC,_l("ATI (VCR1)")));
   formats.push_back(Tformat(_l("Cinepak")       ,IDFF_cvid,IDFF_MOVIE_LAVC,_l("Cinepak (CVID)")));
   formats.push_back(Tformat(_l("Chinese AVS")   ,IDFF_cavs,IDFF_MOVIE_LAVC,_l("Chinese AVS video (AVS1-P2, JiZhun profile)")));
   formats.push_back(Tformat(_l("Creative YUV")  ,IDFF_cyuv,IDFF_MOVIE_LAVC,_l("Creative YUV Video (CYUV)")));
   formats.push_back(Tformat(_l("Indeo 2")       ,IDFF_rt21,IDFF_MOVIE_LAVC,_l("Intel Indeo 2 (RT21)")));
   formats.push_back(Tformat(_l("Indeo 3")       ,IDFF_iv32,IDFF_MOVIE_LAVC,_l("Intel Indeo 3 (IV31, IV32)")));
   formats.push_back(Tformat(_l("Fraps")         ,IDFF_fps1,IDFF_MOVIE_LAVC,_l("Fraps v0, v1, v2, v4 & v5 (FPS1)"),NULL,LAVC_UNSTABLE));
   formats.push_back(Tformat(_l("Miro VideoXL")  ,IDFF_vixl,IDFF_MOVIE_LAVC,_l("Miro VideoXL (VIXL)")));
   formats.push_back(Tformat(_l("MS Video 1")    ,IDFF_cram,IDFF_MOVIE_LAVC,_l("MS Video-1 (CRAM)")));
   formats.push_back(Tformat(_l("MS RLE")        ,IDFF_rle ,IDFF_MOVIE_LAVC,_l("MS RLE Video (MRLE)")));
   formats.push_back(Tformat(_l("Q-Team QPEG")   ,IDFF_qpeg,IDFF_MOVIE_LAVC,_l("Q-Team (QPEG, Q1.0, Q1.1)")));
   formats.push_back(Tformat(_l("Real Video 1,2"),IDFF_rv10,IDFF_MOVIE_LAVC,_l("Real Video (RV10, RV20)")));
   formats.push_back(Tformat(_l("Real Video 4")  ,IDFF_rv40,IDFF_MOVIE_LAVC,_l("Real Video (RV40)"),NULL,LAVC_UNSTABLE));
   formats.push_back(Tformat(_l("Truemotion 1,2"),IDFF_duck,IDFF_MOVIE_LAVC,_l("Doesn't support 24-bit compression.")));
   formats.push_back(Tformat(_l("Ultimotion")    ,IDFF_ulti,IDFF_MOVIE_LAVC,_l("IBM Ultimotion Video (ULTI)")));
   formats.push_back(Tformat(_l("Winnov V1")     ,IDFF_wnv1,IDFF_MOVIE_LAVC,_l("Winnov WNV1 Video (WNV1)")));
   //formats.push_back(Tformat(_l("Snow")          ,IDFF_snow,IDFF_MOVIE_LAVC,_l("experimental"),NULL,LAVC_UNSTABLE));
   formats.push_back(Tformat(_l("AviSynth")      ,IDFF_avisV,IDFF_MOVIE_AVIS,_l("AVIS (AviSynth AVI files)")));
   for (Tformats::iterator f=formats.begin();f!=formats.end();f++)
    {
     f->decoderCheck(config);
     f->decoders.insert(f->decoders.begin(),Tformat::Tdecoder::init(_l("disabled"),0));
    }
  }
 Tformat raw(_l("Raw video"),IDFF_rawv);
 static const TcspFcc rawincsps[]=
  {
   {_l("disabled"),0},
   {_l("all supported")   ,FOURCC_MASK_ALL},
   {_l("all YUV")         ,FOURCC_MASK_YUV},
   {_l("all RGB")         ,FOURCC_MASK_RGB},
   {_l("8-bit palletized"),FOURCC_MASK_PAL},
   NULL,0
  };

 static const Tformat::Toption option={FOURCC_MASK_ALL, _l("Enable in WMP11"),IDFF_alternateUncompressed,1};
 raw.options.push_back(option);

 for (int i=0;rawincsps[i].name;i++)
 {
  raw.decoders.push_back(Tformat::Tdecoder::init(rawincsps[i].name,rawincsps[i].fcc));
 }
 for (int i=0;cspFccs[i].name;i++)
 {
  raw.decoders.push_back(Tformat::Tdecoder::init(cspFccs[i].name,cspFccs[i].fcc));
 }
 formats.push_back(raw);
}

//================================= TcodecsPageAudio ===================================
TcodecsPageAudio::TcodecsPageAudio(TffdshowPageDec *Iparent):TcodecsPage(Iparent)
{
}
void TcodecsPageAudio::init(void)
{
 initLV();
}
void TcodecsPageAudio::fillCodecs(void)
{
 if ((filterMode&IDFF_FILTERMODE_AUDIORAW)==0)
  {
   static const int movie_mp23[]={IDFF_MOVIE_MPLAYER,IDFF_MOVIE_LIBMAD,IDFF_MOVIE_LAVC,IDFF_MOVIE_AUDX,0};
   formats.push_back(Tformat(_l("MP3")         ,IDFF_mp3        ,movie_mp23,_l("MPEG-1 Audio Layer 3")));
   formats.push_back(Tformat(_l("MP1,MP2")     ,IDFF_mp2        ,movie_mp23,_l("MPEG-1 Audio Layer 1,2")));
   static const int movie_ac3[]={IDFF_MOVIE_LIBA52,IDFF_MOVIE_LAVC,0};
   formats.push_back(Tformat(_l("AC3")         ,IDFF_ac3        ,movie_ac3,_l("ATSC A-52 (AC3) decoder")));
   formats.push_back(Tformat(_l("E-AC3")       ,IDFF_eac3       ,IDFF_MOVIE_LAVC,_l("Extended AC3 decoder (Dolby Digital Plus). Works only if AC3 decoding is enabled!")));
   formats.push_back(Tformat(_l("MLP")         ,IDFF_mlp        ,IDFF_MOVIE_LAVC,_l("Dolby TrueHD/MLP decoder. Works only if AC3 decoding is enabled!")));
   static const int movie_dts[]={IDFF_MOVIE_LIBDTS,IDFF_MOVIE_LAVC,0};
   static const Tformat::Toption options_dts[]={{0,_l("Check for DTS in WAV"),IDFF_dtsinwav,1},{NULL,0}};
   formats.push_back(Tformat(_l("DTS")         ,IDFF_dts        ,movie_dts,_l("DTS Coherent Acoustics stream decoder"),options_dts));
   static const int movie_aac[]={IDFF_MOVIE_LIBFAAD, IDFF_MOVIE_REALAAC,0};
   formats.push_back(Tformat(_l("AAC")         ,IDFF_aac        ,movie_aac,_l("Advanced Audio Coding (AAC)")));
   static const int movie_vorbis[]={IDFF_MOVIE_LAVC,IDFF_MOVIE_TREMOR,0};
   static const Tformat::Toption options_vorbis[]={{0,_l("VorbisGain"),IDFF_vorbisgain,1},{0,NULL,0}};
   formats.push_back(Tformat(_l("Vorbis")      ,IDFF_vorbis     ,movie_vorbis,_l("High accuracy mode is enabled for Tremor."),options_vorbis));
   formats.push_back(Tformat(_l("AMR")         ,IDFF_amr        ,IDFF_MOVIE_LAVC,_l("Adaptive Multi-Rate in 3gp files (AMR)")));
   Tformat lpcm(_l("LPCM"),IDFF_lpcm,_l("Linear Pulse Code Modulation"));
   lpcm.decoders.push_back(Tformat::Tdecoder::init(_l("enabled"),IDFF_MOVIE_RAW));
   formats.push_back(lpcm);
   formats.push_back(Tformat(_l("WMA 7")       ,IDFF_wma7       ,IDFF_MOVIE_LAVC,_l("Windows Media Audio compatible decoder"),NULL));
   formats.push_back(Tformat(_l("WMA 8/9")     ,IDFF_wma8       ,IDFF_MOVIE_LAVC,_l("not WMA9 pro, voice or lossless"),NULL));
   formats.push_back(Tformat(_l("MS ADPCM")    ,IDFF_msadpcm    ,IDFF_MOVIE_LAVC,_l("MS Adaptive DPCM")));
   formats.push_back(Tformat(_l("IMA ADPCM")   ,IDFF_iadpcm     ,IDFF_MOVIE_LAVC,_l("IMA Adaptive DPCM")));
   formats.push_back(Tformat(_l("Other ADPCM") ,IDFF_otherAdpcm ,IDFF_MOVIE_LAVC,_l("Creative, Yamaha, G726")));
   formats.push_back(Tformat(_l("Mulaw/Alaw")  ,IDFF_law        ,IDFF_MOVIE_LAVC,_l("Mu-law & A-law")));
   formats.push_back(Tformat(_l("MS GSM")      ,IDFF_gsm        ,IDFF_MOVIE_LAVC,_l("MS GSM Audio")));
   formats.push_back(Tformat(_l("FLAC")        ,IDFF_flac       ,IDFF_MOVIE_LAVC,_l("FLAC (Free Lossless Audio Codec). To play .flac files you also need a source filter.")));
   formats.push_back(Tformat(_l("True Audio")  ,IDFF_tta        ,IDFF_MOVIE_LAVC,_l("TTA (Lossless Audio Codec). To play .tta files you also need a source filter.")));
   formats.push_back(Tformat(_l("TrueSpeech")  ,IDFF_truespeech ,IDFF_MOVIE_LAVC,_l("DSP Group TrueSpeech compatible decoder")));
   formats.push_back(Tformat(_l("QDM2")        ,IDFF_qdm2       ,IDFF_MOVIE_LAVC,_l("QDM2 compatible decoder (incomplete)")));
   formats.push_back(Tformat(_l("MACE3,MACE6") ,IDFF_mace       ,IDFF_MOVIE_LAVC,_l("Macintosh Audio Compression/Expansion")));
   formats.push_back(Tformat(_l("Real Audio")  ,IDFF_ra         ,IDFF_MOVIE_LAVC,_l("Real Audio 1.0 (14.4K) & RealAudio 2.0 (28.8K)")));
   formats.push_back(Tformat(_l("IMC")         ,IDFF_imc        ,IDFF_MOVIE_LAVC,_l("IMC compatible decoder")));
   formats.push_back(Tformat(_l("ATRAC3")      ,IDFF_atrac3     ,IDFF_MOVIE_LAVC,_l("ATRAC3 compatible decoder")));
   formats.push_back(Tformat(_l("Nellymoser")  ,IDFF_nellymoser ,IDFF_MOVIE_LAVC,_l("Nellymoser")));
   formats.push_back(Tformat(_l("AVIS")        ,IDFF_avisA      ,IDFF_MOVIE_AVIS,_l("AVIS (AviSynth AVI files)")));
  }
 //Tformat::Tdecoder movie_raw[]={{"any",IDFF_MOVIE_RAW},{NULL,0)};
 for (Tformats::iterator f=formats.begin();f!=formats.end();f++)
  {
   f->decoderCheck(config);
   f->decoders.insert(f->decoders.begin(),Tformat::Tdecoder::init(_l("disabled"),0));
  }
 static const Tformat::Tdecoder movie_raw[]=
  {
   {_l("disabled"),0},
   {_l("all supported") ,IDFF_MOVIE_RAW},
   {_l("all integer")   ,(TsampleFormat::SF_ALLINT|TsampleFormat::SF_PCM8)<<8},
   {_l("all float")     ,TsampleFormat::SF_ALLFLOAT<<8},
   {_l("8-bit integer") ,TsampleFormat::SF_PCM8   <<8},
   {_l("16-bit integer"),TsampleFormat::SF_PCM16  <<8},
   {_l("24-bit integer"),TsampleFormat::SF_PCM24  <<8},
   {_l("32-bit integer"),TsampleFormat::SF_PCM32  <<8},
   {_l("32-bit float")  ,TsampleFormat::SF_FLOAT32<<8},
   {_l("64-bit float")  ,TsampleFormat::SF_FLOAT64<<8},
   {NULL,0}
  };
 formats.push_back(Tformat(_l("Uncompressed"),IDFF_rawa,movie_raw,_l("raw,twos,sowt,fl32,fl64,in24,in32")));
}
