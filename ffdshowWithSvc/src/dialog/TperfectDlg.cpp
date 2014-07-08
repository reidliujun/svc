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
#include "Tperfect.h"
#include "TperfectDlg.h"
#include "2pass.h"
#include "CsecondPass.h"
#include "TencStats.h"
#include "IffdshowEnc.h"
#include "Ttranslate.h"

void TperfectDlg::init(void)
{
 translate();
 const char_t *capt=tr->translate(m_hwnd,dialogId,0,NULL);
 if (capt && capt[0]) setWindowText(m_hwnd,capt);
 _drawgraphframe=_(-IDD_PERFECT,_l("%i%%: %i frame(s)"));
 _quantpercentage=_(-IDD_PERFECT,_l("quantizer:%i, percentage:%i"));
 _quantcount=_(-IDD_PERFECT,_l("quantizer:%i, count:%i"));

 hg=GetDlgItem(m_hwnd,IDC_BMP_PERFECT_GRAPH);
 RECT r;GetWindowRect(hg,&r);
 gx=r.right-r.left;gy=r.bottom-r.top;stride=(gx/4+1)*4;
 graphBits=(unsigned char*)calloc(stride,gy);
 memset(&graphBmp.bmiHeader,0,sizeof(graphBmp.bmiHeader));
 graphBmp.bmiHeader.biSize=sizeof(graphBmp.bmiHeader);
 graphBmp.bmiHeader.biWidth=stride;
 graphBmp.bmiHeader.biHeight=gy;
 graphBmp.bmiHeader.biPlanes=1;
 graphBmp.bmiHeader.biBitCount=8;
 graphBmp.bmiHeader.biCompression=BI_RGB;
 graphBmp.bmiHeader.biXPelsPerMeter=100;
 graphBmp.bmiHeader.biYPelsPerMeter=100;
 memset(graphBmp.bmiColors,0,256*sizeof(RGBQUAD));
 graphBmp.bmiColors[0].rgbRed=255;graphBmp.bmiColors[0].rgbGreen=255;graphBmp.bmiColors[0].rgbBlue=255; //0 - background
 graphBmp.bmiColors[1].rgbRed=  0;graphBmp.bmiColors[1].rgbGreen=  0;graphBmp.bmiColors[1].rgbBlue=  0; //1 - percent bars
 graphBmp.bmiColors[2].rgbRed=  0;graphBmp.bmiColors[2].rgbGreen=255;graphBmp.bmiColors[2].rgbBlue=  0; //2 - average

 hqp=GetDlgItem(m_hwnd,IDC_BMP_PERFECT_QUANT_PERCENTS);
 GetWindowRect(hqp,&r);
 qpy=r.bottom-r.top;
 qpBits=(unsigned char*)calloc(stride,qpy);
 memset(&qpBmp.bmiHeader,0,sizeof(qpBmp.bmiHeader));
 qpBmp.bmiHeader.biSize=sizeof(qpBmp.bmiHeader);
 qpBmp.bmiHeader.biWidth=stride;
 qpBmp.bmiHeader.biHeight=qpy;
 qpBmp.bmiHeader.biPlanes=1;
 qpBmp.bmiHeader.biBitCount=8;
 qpBmp.bmiHeader.biCompression=BI_RGB;
 qpBmp.bmiHeader.biXPelsPerMeter=100;
 qpBmp.bmiHeader.biYPelsPerMeter=100;
 memset(qpBmp.bmiColors,0,256*sizeof(RGBQUAD));
 qpBmp.bmiColors[0].rgbRed=255;qpBmp.bmiColors[0].rgbGreen=255;qpBmp.bmiColors[0].rgbBlue=255; //0 - background
 qpBmp.bmiColors[1].rgbRed=  0;qpBmp.bmiColors[1].rgbGreen=  0;qpBmp.bmiColors[1].rgbBlue=  0; //1 - percent bars
 qpBmp.bmiColors[2].rgbRed=  0;qpBmp.bmiColors[2].rgbGreen=  0;qpBmp.bmiColors[1].rgbBlue=255; //2 - horiz line

 hqcnt=GetDlgItem(m_hwnd,IDC_BMP_PERFECT_QUANT_CNT);
 GetWindowRect(hqcnt,&r);
 qcnty=r.bottom-r.top;
 qcntBits=(unsigned char*)calloc(stride,qpy);
 memset(&qcntBmp.bmiHeader,0,sizeof(qcntBmp.bmiHeader));
 qcntBmp.bmiHeader.biSize=sizeof(qcntBmp.bmiHeader);
 qcntBmp.bmiHeader.biWidth=stride;
 qcntBmp.bmiHeader.biHeight=qcnty;
 qcntBmp.bmiHeader.biPlanes=1;
 qcntBmp.bmiHeader.biBitCount=8;
 qcntBmp.bmiHeader.biCompression=BI_RGB;
 qcntBmp.bmiHeader.biXPelsPerMeter=100;
 qcntBmp.bmiHeader.biYPelsPerMeter=100;
 memset(qcntBmp.bmiColors,0,256*sizeof(RGBQUAD));
 qcntBmp.bmiColors[0].rgbRed=255;qcntBmp.bmiColors[0].rgbGreen=255;qcntBmp.bmiColors[0].rgbBlue=255; //0 - background
 qcntBmp.bmiColors[1].rgbRed=  0;qcntBmp.bmiColors[1].rgbGreen=  0;qcntBmp.bmiColors[1].rgbBlue=  0; //1 - percent bars

 hTipgraph=createHintWindow(hg,10000,10,0);
 hTipqp=createHintWindow(hqp,10000,10,0);
 hTipqcnt=createHintWindow(hqcnt,10000,10,0);

 int barDx=gx/30;
 TOOLINFO ti;
 ti.cbSize=sizeof(TOOLINFO);
 ti.uFlags=TTF_SUBCLASS;
 ti.hinst=hi;

 ti.hwnd=hqp;
 GetClientRect(hqp,&r);
 const char_t *_nosimulation=_(-IDD_PERFECT,_l("no simulation yet"));
 for (int i=1,x=r.left;i<=31;i++,x+=barDx)
  {
   ti.uId=i;
   ti.lpszText=LPTSTR(_nosimulation);
   ti.rect.left=x;
   ti.rect.right=x+barDx-1;
   ti.rect.top=r.top;
   ti.rect.bottom=r.bottom;
   SendMessage(hTipqp,TTM_ADDTOOL,0,(LPARAM)&ti);
  }

 ti.hwnd=hqcnt;
 GetClientRect(hqcnt,&r);
 for (int i=1,x=r.left;i<=31;i++,x+=barDx)
  {
   ti.uId=i;
   ti.lpszText=LPTSTR(_nosimulation);
   ti.rect.left=x;
   ti.rect.right=x+barDx-1;
   ti.rect.top=r.top;
   ti.rect.bottom=r.bottom;
   SendMessage(hTipqcnt,TTM_ADDTOOL,0,(LPARAM)&ti);
  }

 ti.hwnd=hg;
 GetClientRect(hg,&r);
 for (int x=0;x<gx;x++)
  {
   ti.uId=x;
   ti.lpszText=LPTSTR(_nosimulation);
   ti.rect.left=x;
   ti.rect.right=x+1;
   ti.rect.top=r.top;
   ti.rect.bottom=r.bottom;
   SendMessage(hTipgraph,TTM_ADDTOOL,0,(LPARAM)&ti);
  }

 if (!fileexists(cfgGetStr(IDFF_enc_stats1flnm)))
  enable(0,IDC_BT_PERFECT_ANALYZE);
 analyze();
}

void TperfectDlg::lineX(unsigned char *bits,int x1,int x2,int y,unsigned char c)
{
 memset(bits+y*stride+x1,c,x2-x1+1);
}
void TperfectDlg::lineY(unsigned char *bits,int x,int y1,int y2,unsigned char c)
{
 if (y1>y2) std::swap(y1,y2);
 unsigned char *dst=bits+y1*stride+x;
 for (int y=y1;y<y2;y++,dst+=stride)
  *dst=c;
}

void TperfectDlg::drawGraph(void)
{
 memset(graphBits,0,stride*gy);
 if (perfect.ready)
  {
   double barWidth=(perfect.avgmax-perfect.avgmin)/gx;
   double percent=perfect.avgmin;
   int *cnts=new int[gx];int maxcnt=-1;
   char_t pomS[256];
   TOOLINFO ti;
   memset(&ti,0,sizeof(ti));
   ti.cbSize=sizeof(ti);
   ti.lpszText=pomS;
   ti.hwnd=hg;
   for (int x=0;x<gx;x++,percent+=barWidth)
    {
     unsigned int framesizeMin=(unsigned int)((percent         )*perfect.avg/100.0);
     unsigned int framesizeMax=(unsigned int)((percent+barWidth)*perfect.avg/100.0);
     int cnt=0;
     for (unsigned int i=framesizeMin;i<framesizeMax;i++)
      cnt+=perfect.getFramesCount(i);
     if (cnt>maxcnt) maxcnt=cnt;
     if (percent<=100 && 100<percent+barWidth)
      cnt=-cnt;
     cnts[x]=cnt;
     ti.uId=x;
     tsnprintf_s(pomS, countof(pomS), _TRUNCATE, _drawgraphframe, int(percent+0.5+barWidth/2-100), abs(cnts[x]));
     SendMessage(hTipgraph,TTM_UPDATETIPTEXT,0,LPARAM(&ti));
    }
   for (int x=0;x<gx;x++)
    {
     int y=(gy-1)*abs(cnts[x])/maxcnt;
     if (cnts[x]<0) lineY(graphBits,x,0,gy-1,2);
     lineY(graphBits,x,0,y,1);
    }
   delete []cnts;
  }
 repaint(GetDlgItem(m_hwnd,IDC_BMP_PERFECT_GRAPH));
}

void TperfectDlg::analyze(void)
{
 initStatsRead();
 perfect.analyze(statsRead);
 if (perfect.ready)
  {
   SetDlgItemInt(m_hwnd,IDC_LBL_PERFECT_AVGMIN,UINT(-(perfect.avgmin-100)),FALSE);
   SetDlgItemInt(m_hwnd,IDC_LBL_PERFECT_AVGMAX,UINT(  perfect.avgmax-100 ),FALSE);
  }
 else
  {
   setDlgItemText(m_hwnd,IDC_LBL_PERFECT_AVGMIN,_l(""));
   setDlgItemText(m_hwnd,IDC_LBL_PERFECT_AVGMAX,_l(""));
  }
 enable(perfect.ready,IDC_BT_PERFECT_SIMULATE);
 drawGraph();
}

void TperfectDlg::initStatsRead(void)
{
 if (!statsRead || stricmp(oldStats1flnm,cfgGetStr(IDFF_enc_stats1flnm))!=0)
  {
   if (statsRead) delete statsRead;
   statsRead=new TxvidStats(ff_strncpy(oldStats1flnm, cfgGetStr(IDFF_enc_stats1flnm), countof(oldStats1flnm)),false);
  }
 statsRead->resetRead();
}
void TperfectDlg::simulate(void)
{
 if (!perfect.ready) return;
 initStatsRead();
 TxvidStats statsWrite(NULL,true);
 T2passSecond pass(deciE,statsRead,NULL,&statsWrite,true);
 uint64_t movielen=0;
 int oldquanttype=0;
 TencFrameParams params;
 TencStats encstats;
 for (params.framenum=0;;params.framenum++)
  {
   if (!pass.getQuantSecond(params)) break;
   params.quant&=Txvid_2pass::NNSTATS_QUANTMASK;
   int quant1=pass.getQuantFirst();
   int outlen;
   unsigned int inlen=pass.getBytesFirst();
   if (params.quant==quant1)
    outlen=inlen;
   else
    {
     outlen=int(quant1*inlen/double(params.quant));
    }
   int quanttype=deciE->getQuantType2(params.quant);
   if (quanttype!=oldquanttype)
    {
     outlen+=140;
     oldquanttype=quanttype;
    }
   movielen+=outlen;
   params.length=outlen;
   params.quanttype=quanttype;
   encstats.add(params);
   if (pass.update(params)==false) break;
  }
 memset(qpBits,0,stride*qpy);
 int x,q;
 q=1;
 int qtmin=1000000,qtmax=-1000;
 for (q=1;q<=31;q++)
  {
   int qt=pass.quant_threshs[q].percent;
   if (qt<qtmin) qtmin=qt;
   else if (qt>qtmax) qtmax=qt;
  }
 int bardx=gx/30;
 if (qtmin!=qtmax)
  {
   q=1;
   int y1=qpy*(0-qtmin)/(qtmax-qtmin);
   for (x=0;q<=31;q++,x+=bardx)
    {
     int y2=y1+qpy*pass.quant_threshs[q].percent/(qtmax-qtmin);
     y1=limit(y1,0,qpy-1);y2=limit(y2,0,qpy-1);
     for (int xx=x;xx<x+bardx;xx++)
      lineY(qpBits,xx,y1,y2,1);
    }
   lineX(qpBits,0,gx,y1,2);
  }

 memset(qcntBits,0,stride*qcnty);
 unsigned int qcntmax=0;for (q=1;q<=31;q++) if (encstats.quantCount[q]>qcntmax) qcntmax=encstats.quantCount[q];
 q=1;
 if (qcntmax!=0)
  {
   for (x=0;q<=31;q++,x+=bardx)
    {
     int y=(qpy-1)*encstats.quantCount[q]/qcntmax+2;
     y=limit(y,0,qpy-1);
     for (int xx=x;xx<x+bardx;xx++)
      lineY(qcntBits,xx,2,y,1);
    }
   char_t pomS[256];
   TOOLINFO ti;
   memset(&ti,0,sizeof(ti));
   ti.cbSize=sizeof(ti);
   ti.lpszText=pomS;
   unsigned int quantsum=0,quantcnt=0;
   for (q=1;q<=31;q++)
    {
     ti.uId=q;
     ti.hwnd=hqp;
     tsnprintf_s(pomS, countof(pomS), _TRUNCATE, _quantpercentage, q,pass.quant_threshs[q].percent);
     SendMessage(hTipqp,TTM_UPDATETIPTEXT,0,LPARAM(&ti));
     ti.hwnd=hqcnt;
     tsnprintf_s(pomS, countof(pomS), _TRUNCATE, _quantcount, q, encstats.quantCount[q]);
     quantsum+=q*encstats.quantCount[q];quantcnt+=encstats.quantCount[q];
     SendMessage(hTipqcnt,TTM_UPDATETIPTEXT,0,LPARAM(&ti));
    }
   if (quantcnt>0)
    {
     setText(IDC_LBL_PERFECT_BYTES,_l("%s %I64i"),_(IDC_LBL_PERFECT_BYTES),movielen);
     setText(IDC_LBL_PERFECT_NUMFRAMES,_l("%s %i"),_(IDC_LBL_PERFECT_NUMFRAMES),quantcnt);
     setText(IDC_LBL_PERFECT_BITRATE,_l("%s %i %s"),_(IDC_LBL_PERFECT_BITRATE),int(8*movielen/(quantcnt/25))/1000,_(IDC_LBL_PERFECT_BITRATE,_l("kbps (at 25 fps)")));
     setText(IDC_LBL_PERFECT_AVGFRAMESIZE,_l("%s %i"),_(IDC_LBL_PERFECT_AVGFRAMESIZE),int(movielen/quantcnt));
     setText(IDC_LBL_PERFECT_AVGQUANT,_l("%s %5.2f"),_(IDC_LBL_PERFECT_AVGQUANT),float(quantsum)/quantcnt);
    }
  }
 repaint(hqp);repaint(hqcnt);
 if (cfgGet(IDFF_enc_xvid2pass_use_write))
  *statsRead=statsWrite;
}

INT_PTR TperfectDlg::msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
 switch (uMsg)
  {
   case WM_INITDIALOG:
    resizeDialog();
    init();
    if (cfgGet(IDFF_dlgPerfectDlgX)!=-1)
     SetWindowPos(m_hwnd,NULL,cfgGet(IDFF_dlgPerfectDlgX),cfgGet(IDFF_dlgPerfectDlgY),0,0,SWP_NOSIZE|SWP_NOREPOSITION);
    return TRUE;
   case WM_DESTROY:
    {
     WINDOWPLACEMENT wpl;
     wpl.length=sizeof(wpl);
     GetWindowPlacement(m_hwnd,&wpl);
     cfgSet(IDFF_dlgPerfectDlgX,wpl.rcNormalPosition.left);
     cfgSet(IDFF_dlgPerfectDlgY,wpl.rcNormalPosition.top );
     deci->saveDialogSettings();
     if (graphBits) free(graphBits);graphBits=NULL;
     if (qpBits) free(qpBits);qpBits=NULL;
     DestroyWindow(hTipgraph);DestroyWindow(hTipqp);DestroyWindow(hTipqcnt);
     break;
    }
   case WM_DRAWITEM:
    switch (wParam)
     {
      case IDC_BMP_PERFECT_GRAPH:
       {
        LPDRAWITEMSTRUCT dis=LPDRAWITEMSTRUCT(lParam);
        SetDIBitsToDevice(dis->hDC,0,0,gx,gy,0,0,0,gy,graphBits,(BITMAPINFO*)&graphBmp,DIB_RGB_COLORS);
        return TRUE;
       }
      case IDC_BMP_PERFECT_QUANT_PERCENTS:
       {
        LPDRAWITEMSTRUCT dis=LPDRAWITEMSTRUCT(lParam);
        SetDIBitsToDevice(dis->hDC,0,0,gx,qpy,0,0,0,gy,qpBits,(BITMAPINFO*)&qpBmp,DIB_RGB_COLORS);
        return TRUE;
       }
      case IDC_BMP_PERFECT_QUANT_CNT:
       {
        LPDRAWITEMSTRUCT dis=LPDRAWITEMSTRUCT(lParam);
        SetDIBitsToDevice(dis->hDC,0,0,gx,qcnty,0,0,0,gy,qcntBits,(BITMAPINFO*)&qcntBmp,DIB_RGB_COLORS);
        return TRUE;
       }
     }
    break;
   case WM_COMMAND:
    switch (LOWORD(wParam))
     {
      case IDCANCEL:
       parent->onPerfectDestroy();
       return TRUE;
     }
    break;
  }
 return TdlgWindow::msgProc(uMsg,wParam,lParam);
}

void TperfectDlg::onAnalyze(void)
{
 enable(0,IDC_BT_PERFECT_ANALYZE);
 analyze();
 enable(1,IDC_BT_PERFECT_ANALYZE);
}
void TperfectDlg::onSimulate(void)
{
 enable(0,IDC_BT_PERFECT_SIMULATE);
 simulate();
 enable(1,IDC_BT_PERFECT_SIMULATE);
}

static LRESULT CALLBACK MainWndProc(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
 switch (uMsg)
  {
   case WM_CREATE:
    return 0;
   case WM_PAINT:
    return 0;
   case WM_SIZE:
    return 0;
   case WM_DESTROY:
    return 0;
   default:
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
  }
}

TperfectDlg::TperfectDlg(TsecondPassPage *Iparent):parent(Iparent),TdlgWindow(IDD_PERFECT,Iparent->deci)
{
 deciE=deci;

 static const TbindButton<TperfectDlg> bt[]=
  {
   IDC_BT_PERFECT_ANALYZE,&TperfectDlg::onAnalyze,
   IDC_BT_PERFECT_SIMULATE,&TperfectDlg::onSimulate,
   0,NULL
  };
 bindButtons(bt);

 statsRead=NULL;

 WNDCLASSEX wcx;
 wcx.cbSize = sizeof(wcx);
 wcx.style = CS_HREDRAW | CS_VREDRAW;
 wcx.lpfnWndProc = MainWndProc;
 wcx.cbClsExtra = 0;
 wcx.cbWndExtra = 0;
 wcx.hInstance = hi;
 wcx.hIcon = LoadIcon(NULL,IDI_APPLICATION);
 wcx.hCursor = LoadCursor(NULL, IDC_ARROW);
 wcx.hbrBackground = (HBRUSH)GetStockObject( WHITE_BRUSH);
 wcx.lpszMenuName =  NULL;
 wcx.lpszClassName = _l("TperfectDlg");
 wcx.hIconSm = (HICON)LoadImage(hi,MAKEINTRESOURCE(IDI_FFVFW),IMAGE_ICON, GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), LR_DEFAULTCOLOR);
 RegisterClassEx(&wcx);

 hwndMain = CreateWindowEx(0,
                           _l("TperfectDlg"),
                           _l("Second pass simulation"),
                           WS_OVERLAPPEDWINDOW |WS_TABSTOP,
                           CW_USEDEFAULT,
                           CW_USEDEFAULT,
                           CW_USEDEFAULT,
                           CW_USEDEFAULT,
                           (HWND) NULL,
                           (HMENU) NULL,
                           hi,
                           NULL
                          );

 if (!hwndMain) return;
 ShowWindow(hwndMain,SW_HIDE);
 UpdateWindow(hwndMain);
}
TperfectDlg::~TperfectDlg()
{
 if (statsRead) delete statsRead;
 DestroyWindow(m_hwnd);
 DestroyWindow(hwndMain);
}

void TperfectDlg::show(void)
{
 createDialog(dialogId,hwndMain);
}
