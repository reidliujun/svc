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
#include "Clevels.h"
#include "TlevelsSettings.h"
#include "IimgFilterLevels.h"
#include "ffmpeg/libavutil/bswap.h"

//============================== TlevelsPage::TwidgetLevels =============================
LRESULT TlevelsPage::TwidgetLevels::onLbuttonDown(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
 int x=GET_X_LPARAM(lParam),y=GET_Y_LPARAM(lParam);
 int min=levelsPage->cfgGet(idffMin),max=levelsPage->cfgGet(idffMax);
 int minP=levelsPage->bdx*min/255,maxP=levelsPage->bdx*max/255;
 if (isIn(x,minP-5,minP+5)) dragIdff=idffMin;
 else if (isIn(x,maxP-5,maxP+5)) dragIdff=idffMax;
 else return 0;
 dx=x-levelsPage->cfgGet(dragIdff)*levelsPage->bdx/255;
 InvalidateRect(hwnd,NULL,FALSE);
 InvalidateRect(GetDlgItem(levelsPage->m_hwnd,IDC_BMP_HISTOGRAM),NULL,FALSE);
 SetCapture(hwnd);
 return 0;
}
LRESULT TlevelsPage::TwidgetLevels::onMouseMove(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
 if (dragIdff)
  {
   int x=GET_X_LPARAM(lParam);
   levelsPage->cfgSet(dragIdff,255*(x-dx)/levelsPage->bdx);
   levelsPage->map2dlg();
   InvalidateRect(hwnd,NULL,FALSE);
   InvalidateRect(GetDlgItem(levelsPage->m_hwnd,IDC_BMP_HISTOGRAM),NULL,FALSE);
   return 0;
  }
 return TwindowWidget::onMouseMove(hwnd,uMsg,wParam,lParam);
}
LRESULT TlevelsPage::TwidgetLevels::onLbuttonUp(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
 dragIdff=0;
 ReleaseCapture();
 InvalidateRect(GetDlgItem(levelsPage->m_hwnd,IDC_BMP_HISTOGRAM),NULL,FALSE);
 return 0;
}
void TlevelsPage::TwidgetLevels::draw(LPDRAWITEMSTRUCT dis)
{
 if (dragIdff)
  {
   HPEN pen = CreatePen(PS_DOT,0,RGB(0,0,0));
   HPEN penOld = (HPEN) SelectObject(dis->hDC, pen);
   COLORREF bgColor = SetBkColor(dis->hDC, RGB(255,255,255));
   int x=levelsPage->cfgGet(dragIdff)*levelsPage->bdx/255;
   MoveToEx(dis->hDC, x, 2, NULL);
   LineTo(dis->hDC, x, dis->rcItem.bottom);
   SetBkColor(dis->hDC, bgColor);
   SelectObject(dis->hDC, penOld);
   DeleteObject(pen);
  }
}

//============================== TlevelsPage::TwidgetCurves =============================
const int TlevelsPage::TwidgetCurves::idffs[]=
{
 IDFF_levelsPoint0x,IDFF_levelsPoint0y,
 IDFF_levelsPoint1x,IDFF_levelsPoint1y,
 IDFF_levelsPoint2x,IDFF_levelsPoint2y,
 IDFF_levelsPoint3x,IDFF_levelsPoint3y,
 IDFF_levelsPoint4x,IDFF_levelsPoint4y,
 IDFF_levelsPoint5x,IDFF_levelsPoint5y,
 IDFF_levelsPoint6x,IDFF_levelsPoint6y,
 IDFF_levelsPoint7x,IDFF_levelsPoint7y,
 IDFF_levelsPoint8x,IDFF_levelsPoint8y,
 IDFF_levelsPoint9x,IDFF_levelsPoint9y
};

TlevelsPage::TwidgetCurves::TwidgetCurves(HWND h,TlevelsPage *Iself):
 levelsPage(Iself),
 TwindowWidget(h,Iself),
 h(h),
 curpoint(-1),dragpoint(-1)
{
 allowOwnProc();
 CRect r;
 GetWindowRect(h,&r);
 SetWindowPos(h,NULL,0,0,r.Height(),r.Height(),SWP_NOMOVE|SWP_NOZORDER);
 dxy=r.Height()+1;
 load();
 br=CreateSolidBrush(RGB(0,0,0));
}
TlevelsPage::TwidgetCurves::~TwidgetCurves()
{
 DeleteObject(br);
}
void TlevelsPage::TwidgetCurves::load(void)
{
 pt.resize(levelsPage->cfgGet(IDFF_levelsNumPoints));
 for (size_t i=0;i<pt.size();i++)
  {
   pt[i].x=levelsPage->cfgGet(idffs[2*i+0]);
   pt[i].y=levelsPage->cfgGet(idffs[2*i+1]);
  }
 print();
}
void TlevelsPage::TwidgetCurves::save(void)
{
 levelsPage->cfgSet(IDFF_levelsNumPoints,(int)pt.size());
 for (size_t i=0;i<pt.size();i++)
  {
   levelsPage->cfgSet(idffs[2*i+0],pt[i].x);
   levelsPage->cfgSet(idffs[2*i+1],pt[i].y);
  }
 print();
}
void TlevelsPage::TwidgetCurves::print(void)
{
 levelsPage->lbxClear(IDC_LBX_LEVELS_CURVES);
 for (size_t i=0;i<pt.size();i++)
  {
   char_t s[30];tsprintf(s,_l("%u:[%li,%li]"),i,pt[i].x,pt[i].y);
   levelsPage->lbxAdd(IDC_LBX_LEVELS_CURVES,s,i);
  }
 if (dragpoint!=-1)
  levelsPage->lbxSetCurSel(IDC_LBX_LEVELS_CURVES,dragpoint);
}

void TlevelsPage::TwidgetCurves::draw(LPDRAWITEMSTRUCT dis)
{
 for (size_t i=0;i<pt.size();i++)
  {
   int x=dxy*pt[i].x/256,y=dxy*(255-pt[i].y)/256;
   CRect pr(x-2,y-2,x+2,y+2);
   FillRect(dis->hDC,&pr,br);
  }
}

std::pair<int,int> TlevelsPage::TwidgetCurves::findPoint(int sx,int sy)
{
 size_t mindist=INT_MAX,mindisti=0;
 for (size_t i=0;i<pt.size();i++)
  {
   int x=dxy*pt[i].x/256,y=dxy*(255-pt[i].y)/256;
   size_t dist=sqr(x-sx)+sqr(y-sy);
   if (dist<mindist)
    {
     mindist=dist;
     mindisti=i;
     if (mindist==0)
      break;
    }
  }
 return std::make_pair((int)mindist,(int)mindisti);
}

bool TlevelsPage::TwidgetCurves::setCursor(void)
{
 POINT cp;
 GetCursorPos(&cp);
 ScreenToClient(h,&cp);
 std::pair<int,int> pt=findPoint(cp.x,cp.y);
 return pt.first>40?(curpoint=-1,false):(curpoint=pt.second,true);
}

LRESULT TlevelsPage::TwidgetCurves::onLbuttonDown(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
 if (curpoint==-1 && pt.size()<10)
  {
   int x=GET_X_LPARAM(lParam),y=GET_Y_LPARAM(lParam);
   x=limit_uint8(256*x/dxy);
   CPoints::iterator p=pt.begin();
   for (pt.begin();p!=pt.end()-1;p++)
    if ((p+1)->x>x)
     break;
   p=pt.insert(p+1,CPoint(x,limit_uint8(255-256*y/dxy)));
   save();
   curpoint=int(p-pt.begin());
   levelsPage->map2dlg();//InvalidateRect(hwnd,NULL,FALSE);
  }
 SetCapture(hwnd);SetCursor(LoadCursor(NULL,IDC_SIZEALL));
 dragpoint=curpoint;
 print();
 return 0;
}
LRESULT TlevelsPage::TwidgetCurves::onLbuttonUp(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
 dragpoint=-1;
 print();
 ReleaseCapture();
 return 0;
}
LRESULT TlevelsPage::TwidgetCurves::onMouseMove(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
 if (dragpoint!=-1)
  {
   int x=256*GET_X_LPARAM(lParam)/dxy;
   int y=255-256*GET_Y_LPARAM(lParam)/dxy;
   static const int border=40;
   if (pt.size()>2 && (!isIn(x,0-border,255+border) || !isIn(y,0-border,255+border)))
    {
     pt.erase(pt.begin()+curpoint);
     curpoint=-1;
     onLbuttonUp(hwnd,uMsg,wParam,lParam);
    }
   else
    {
     pt[dragpoint].x=limit(x,int(dragpoint==0?0:pt[dragpoint-1].x),int(dragpoint==(int)pt.size()-1?255:pt[dragpoint+1].x));
     pt[dragpoint].y=limit_uint8(y);
    }
   save();
   levelsPage->map2dlg();
   return 0;
  }
 return TwindowWidget::onMouseMove(hwnd,uMsg,wParam,lParam);
}

//===================================== TlevelsPage =====================================
void TlevelsPage::init(void)
{
 deciD->queryFilterInterface(IID_IimgFilterLevels,(void**)&filter);
 memset(&histogramBmp.bmiHeader,0,sizeof(histogramBmp.bmiHeader));
 histogramBmp.bmiHeader.biSize=sizeof(histogramBmp.bmiHeader);
 histogramBmp.bmiHeader.biWidth=256;
 histogramBmp.bmiHeader.biHeight=64;
 histogramBmp.bmiHeader.biPlanes=1;
 histogramBmp.bmiHeader.biBitCount=8;
 histogramBmp.bmiHeader.biCompression=BI_RGB;
 histogramBmp.bmiHeader.biXPelsPerMeter=100;
 histogramBmp.bmiHeader.biYPelsPerMeter=100;
 memset(histogramBmp.bmiColors,0,256*sizeof(RGBQUAD));
 histogramBmp.bmiColors[1].rgbRed=histogramBmp.bmiColors[1].rgbGreen=histogramBmp.bmiColors[1].rgbBlue=255;
 histogramBmp.bmiColors[2].rgbRed=histogramBmp.bmiColors[2].rgbGreen=histogramBmp.bmiColors[2].rgbBlue=192;
 memset(histogramBits,1,sizeof(histogramBits));

 curvesBmp=histogramBmp;
 curvesBmp.bmiHeader.biHeight=256;
 hcurves=GetDlgItem(m_hwnd,IDC_BMP_LEVELS_CURVES);

 bmpGradient=LoadBitmap(hi,MAKEINTRESOURCE(IDB_GRADIENT));
 HDC hdc=GetDC(m_hwnd);
 hdcGradient=CreateCompatibleDC(hdc);
 oldHDCgradient=SelectObject(hdcGradient,bmpGradient);
 LOGFONT oldFont;
 HFONT hf=(HFONT)GetCurrentObject(hdc,OBJ_FONT);
 GetObject(hf,sizeof(LOGFONT),&oldFont);
 strcpy(oldFont.lfFaceName,_l("Courier"));
 oldFont.lfWeight=FW_LIGHT;
 oldFont.lfHeight=-11;
 oldFont.lfWidth=0;
 fontCurier=CreateFontIndirect(&oldFont);
 SendMessage(GetDlgItem(m_hwnd,IDC_LBX_LEVELS_CURVES),WM_SETFONT,WPARAM(fontCurier),FALSE);
 strcpy(oldFont.lfFaceName,_l("Small fonts"));
 fontGradient=CreateFontIndirect(&oldFont);
 ReleaseDC(m_hwnd,hdc);
 isMap=false;
 hIn=GetDlgItem(m_hwnd,IDC_BMP_LEVELS_IN);
 hOut=GetDlgItem(m_hwnd,IDC_BMP_LEVELS_OUT);
 RECT r;GetWindowRect(hIn,&r);
 bdx=r.right-r.left-2;
 tbrSetRange(IDC_TBR_LEVELS_GAMMA,1,400,40);
 tbrSetRange(IDC_TBR_LEVELS_POSTERIZE,1,255);
 SendDlgItemMessage(m_hwnd,IDC_UD_Y_MAX_DELTA ,UDM_SETRANGE,0,MAKELONG(50,1));
 SendDlgItemMessage(m_hwnd,IDC_UD_Y_THRESHOLD ,UDM_SETRANGE,0,MAKELONG(150,1));
 SendDlgItemMessage(m_hwnd,IDC_UD_Y_TEMPORAL ,UDM_SETRANGE,0,MAKELONG(20,1));
 startup=true;
}

void TlevelsPage::cfg2dlg(void)
{
 repaint(GetDlgItem(m_hwnd,IDC_BMP_LEVELS_IN ));
 repaint(GetDlgItem(m_hwnd,IDC_BMP_LEVELS_OUT));
 setCheck(IDC_CHB_LEVELS_SHOW_HISTOGRAM,cfgGet(IDFF_buildHistogram));
 setCheck(IDC_CHB_LEVELS_SHOW_HISTOGRAM_FULL,cfgGet(IDFF_levelsFullY));
 cbxSetCurSel(IDC_CBX_LEVELS_MODE,cfgGet(IDFF_levelsMode));
 levels2dlg();
 setCheck(IDC_CHB_LEVELS_INPUT_AUTO,cfgGet(IDFF_levelsInAuto));
}
void TlevelsPage::levels2dlg(void)
{
 setCheck(IDC_CHB_LEVELS_ONLYLUMA,cfgGet(IDFF_levelsOnlyLuma));
 int x=cfgGet(IDFF_levelsGamma);
 tbrSet(IDC_TBR_LEVELS_GAMMA,x);
 char_t pomS[256];
 tsnprintf_s(pomS, countof(pomS), _TRUNCATE, _l("%s %3.2f"), _(IDC_LBL_LEVELS_GAMMA), float(x/100.0));
 if (x==100)
  strncatf(pomS, countof(pomS), _l(" (%s)"),_(IDC_LBL_LEVELS_GAMMA,_l("off")));
 setDlgItemText(m_hwnd,IDC_LBL_LEVELS_GAMMA,pomS);
 tbrSet(IDC_TBR_LEVELS_POSTERIZE,cfgGet(IDFF_levelsPosterize),IDC_LBL_LEVELS_POSTERIZE);
 SetDlgItemInt(m_hwnd,IDC_ED_Y_MAX_DELTA ,deci->getParam2(IDFF_levelsYmaxDelta ),FALSE);
 SetDlgItemInt(m_hwnd,IDC_ED_Y_THRESHOLD ,deci->getParam2(IDFF_levelsYthreshold ),FALSE);
 SetDlgItemInt(m_hwnd,IDC_ED_Y_TEMPORAL ,deci->getParam2(IDFF_levelsYtemporal ),FALSE);
 mode2dlg();
}
void TlevelsPage::mode2dlg(void)
{
 int mode=cfgGet(IDFF_levelsMode);
 static const int idnCurves[]={IDC_CHB_LEVELS_SHOW_HISTOGRAM,IDC_CHB_LEVELS_SHOW_HISTOGRAM_FULL,IDC_BMP_HISTOGRAM,IDC_LBL_LEVELS_INPUT,IDC_CHB_LEVELS_INPUT_AUTO,IDC_BMP_LEVELS_IN,IDC_LBL_LEVELS_OUTPUT,IDC_BMP_LEVELS_OUT,IDC_LBL_LEVELS_GAMMA,IDC_TBR_LEVELS_GAMMA,IDC_LBL_LEVELS_POSTERIZE,IDC_TBR_LEVELS_POSTERIZE,0};
 show(mode!=5,idnCurves);
 static const int idCurves[]={IDC_BMP_LEVELS_CURVES,IDC_LBX_LEVELS_CURVES,IDC_BT_LEVELS_CURVES_LOAD,0};
 show(mode==5,idCurves);
 if(mode==6)
  {
   static const int idnCurves[]={IDC_CHB_LEVELS_SHOW_HISTOGRAM_FULL,IDC_LBL_LEVELS_POSTERIZE,IDC_TBR_LEVELS_POSTERIZE,0};
   show(false,idnCurves);
   static const int idCurves[]={IDC_LBL_Y_MAX_DELTA,IDC_ED_Y_MAX_DELTA,IDC_UD_Y_MAX_DELTA, IDC_LBL_Y_THRESHOLD,IDC_ED_Y_THRESHOLD,IDC_UD_Y_THRESHOLD, IDC_LBL_Y_TEMPORAL,IDC_ED_Y_TEMPORAL,IDC_UD_Y_TEMPORAL,0};
   show(true,idCurves);
   setCheck(IDC_CHB_LEVELS_SHOW_HISTOGRAM_FULL,true);
   tbrSet(IDC_TBR_LEVELS_POSTERIZE,255,IDC_LBL_LEVELS_POSTERIZE);
  }
 else
 {
   static const int idnCurves[]={IDC_LBL_Y_MAX_DELTA,IDC_ED_Y_MAX_DELTA,IDC_UD_Y_MAX_DELTA, IDC_LBL_Y_THRESHOLD,IDC_ED_Y_THRESHOLD,IDC_UD_Y_THRESHOLD, IDC_LBL_Y_TEMPORAL,IDC_ED_Y_TEMPORAL,IDC_UD_Y_TEMPORAL,0};
   show(false,idnCurves);
 }
 map2dlg();
}
void TlevelsPage::map2dlg(void)
{
 isMap=SUCCEEDED(deciV->getLevelsMap(map));
 onFrame();
}

void TlevelsPage::drawTriangle(DRAWITEMSTRUCT *dis,int cl,bool fade)
{
 int x=(cl*dis->rcItem.right)/255;
 if (fade) cl=127;
 HBRUSH br=CreateSolidBrush(RGB(cl,cl,cl));
 HPEN pen=CreatePen(PS_SOLID,1,RGB(255-cl,255-cl,255-cl));
 HGDIOBJ oldBr=SelectObject(dis->hDC,br);
 HGDIOBJ oldPen=SelectObject(dis->hDC,pen);
 if (!fade)
  {
   HGDIOBJ oldFont=SelectObject(dis->hDC,fontGradient);
   SetTextAlign(dis->hDC,TA_LEFT|TA_TOP);
   char_t pomS[10];_itoa(cl,pomS,10);
   SIZE sz;
   GetTextExtentPoint32(dis->hDC,pomS,(int)strlen(pomS),&sz);
   int tx=x;
   if (tx-sz.cx/2<0) tx=0;
   else if (tx+sz.cx/2>dis->rcItem.right) tx=dis->rcItem.right-sz.cx;
   else tx-=sz.cx/2;
   TextOut(dis->hDC,tx,22,pomS,(int)strlen(pomS));
   SelectObject(dis->hDC,oldFont);
  }
 int trsz=fade?6:10;
 POINT tri[]={{x,10},{x-trsz/2,10+trsz},{x+trsz/2,10+trsz}};
 Polygon(dis->hDC,tri,3);
 SelectObject(dis->hDC,oldBr);
 SelectObject(dis->hDC,oldPen);
 DeleteObject(pen);
 DeleteObject(br);
}

INT_PTR TlevelsPage::msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
 switch (uMsg)
  {
   case WM_DESTROY:
    SelectObject(hdcGradient,oldHDCgradient);DeleteDC(hdcGradient);
    DeleteBitmap(bmpGradient);
    DeleteFont(fontGradient);
    SendMessage(GetDlgItem(m_hwnd,IDC_LBX_LEVELS_CURVES),WM_SETFONT,NULL,FALSE);DeleteFont(fontCurier);
    break;
   case WM_DRAWITEM:
    switch (wParam)
     {
      case IDC_BMP_HISTOGRAM:
       {
        LPDRAWITEMSTRUCT dis=LPDRAWITEMSTRUCT(lParam);
        int full=cfgGet(IDFF_levelsFullY);
        StretchDIBits(dis->hDC,0,0,dis->rcItem.right,dis->rcItem.bottom,full?0:16,0,full?256:234-16+1,64,histogramBits,(BITMAPINFO*)&histogramBmp,DIB_RGB_COLORS,SRCCOPY);
        wIn->draw(dis);
        return TRUE;
       }
      case IDC_BMP_LEVELS_IN:
       {
        LPDRAWITEMSTRUCT dis=LPDRAWITEMSTRUCT(lParam);
        StretchBlt(dis->hDC,0,0,dis->rcItem.right,10,hdcGradient,0,0,256,1,SRCCOPY);
        RECT r=dis->rcItem;r.top=10;
        FillRect(dis->hDC,&r,GetSysColorBrush(COLOR_BTNFACE));
        if (filter && getCheck(IDC_CHB_LEVELS_INPUT_AUTO))
         {
          int min,max;
          filter->getInAuto(&min,&max);
          drawTriangle(dis,min,true);
          drawTriangle(dis,max,true);
         }
        drawTriangle(dis,cfgGet(IDFF_levelsInMin));
        drawTriangle(dis,cfgGet(IDFF_levelsInMax));
        return TRUE;
       }
      case IDC_BMP_LEVELS_OUT:
       {
        LPDRAWITEMSTRUCT dis=LPDRAWITEMSTRUCT(lParam);
        StretchBlt(dis->hDC,0,0,dis->rcItem.right,10,hdcGradient,0,0,256,1,SRCCOPY);
        RECT r=dis->rcItem;r.top=10;
        FillRect(dis->hDC,&r,GetSysColorBrush(COLOR_BTNFACE));
        drawTriangle(dis,cfgGet(IDFF_levelsOutMin));
        drawTriangle(dis,cfgGet(IDFF_levelsOutMax));
        return TRUE;
       }
      case IDC_BMP_LEVELS_CURVES:
       {
        LPDRAWITEMSTRUCT dis=LPDRAWITEMSTRUCT(lParam);
        StretchDIBits(dis->hDC,0,0,dis->rcItem.right,dis->rcItem.bottom,0,0,256,256,histogramBits,(BITMAPINFO*)&curvesBmp,DIB_RGB_COLORS,SRCCOPY);
        wCurves->draw(dis);
        return TRUE;
       }
     }
    break;
   case WM_SETCURSOR:
    if (HWND(wParam)==hcurves)
     {
      if (wCurves->setCursor())
       {
        SetCursor(LoadCursor(NULL,IDC_SIZEALL));
        setDlgResult(TRUE);
       }
      return TRUE;
     }
    break;
  }
 return TconfPageDecVideo::msgProc(uMsg,wParam,lParam);
}

Twidget* TlevelsPage::createDlgItem(int id,HWND h)
{
 if (id==IDC_BMP_LEVELS_IN)
  return wIn=new TwidgetLevels(h,this,IDFF_levelsInMin,IDFF_levelsInMax);
 else if (id==IDC_BMP_LEVELS_OUT)
  return new TwidgetLevels(h,this,IDFF_levelsOutMin,IDFF_levelsOutMax);
 else if (id==IDC_BMP_LEVELS_CURVES)
  return wCurves=new TwidgetCurves(h,this);
 else
  return TconfPageDecVideo::createDlgItem(id,h);
}

void TlevelsPage::onFrame(void)
{
 if (!startup && !IsWindowVisible(m_hwnd)) return;
 startup=false;
 int mode=cfgGet(IDFF_levelsMode);
 memset(histogramBits,1,sizeof(histogramBits));
 if (mode==5)
  for (int i=32;i<256;i+=32)
   for (int j=0;j<256;j+=2)
    {
     histogramBits[i*256+j]=2;
     histogramBits[j*256+i]=2;
    }
 repaint(GetDlgItem(m_hwnd,IDC_BMP_HISTOGRAM));
 unsigned int histogram[256];
 int isHist=getCheck(IDC_CHB_LEVELS_SHOW_HISTOGRAM);
 if (isHist && filter)
  filter->getHistogram(histogram);
 else
  memset(histogram,0,sizeof(histogram));
 unsigned int min,max;
 if (mode==5 || cfgGet(IDFF_levelsFullY))
  {
   min=0;max=255;
  }
 else
  {
   min=16;max=234;
  }
 unsigned int histogramMax=0;
 histogramMax=*std::max_element(histogram+min,histogram+max+1);
 int dy=mode==5?256:64;
 if (histogramMax>1 || isMap || !isHist || mode==5)
  {
   for (unsigned int x=min;x<=max;x++)
    {
     if (histogramMax>0)
      {
       unsigned int y=histogram[x]*dy/histogramMax;
       for (unsigned char *dst=histogramBits+x,*dstEnd=dst+y*256;dst<dstEnd;dst+=256)
        *dst=2;
      }
     if (isMap)
      histogramBits[x+256*(map[x]/(256/dy))]=0;
    }
   repaint(mode==5?hcurves:GetDlgItem(m_hwnd,IDC_BMP_HISTOGRAM));
  }
 repaint(GetDlgItem(m_hwnd,IDC_BMP_LEVELS_IN));
}

void TlevelsPage::onCurveLoad(void)
{
 if (dlgGetFile(false,m_hwnd,_(-IDD_LEVELS,_l("Select curve file")),_l("Adobe Photoshop Curves (*.acv)\0*.acv\0All files (*.*)\0*.*\0"),_l("acv"),curvesflnm,_l("."),0))
  {
   FILE *f=fopen(curvesflnm,_l("rb"));if (!f) return;
   int16_t w;
   if (fread(&w,1,2,f)==2 && be2me_16(w)==4)
    if (fread(&w,1,2,f)==2 && be2me_16(w)==5)
     {
      int16_t cnt;
      if (fread(&cnt,1,2,f)==2 && (cnt=be2me_16(cnt))>0)
       {
        cnt=std::min(cnt,int16_t(10));
        cfgSet(IDFF_levelsNumPoints,cnt);
        std::pair<uint8_t,uint8_t> pts[10];
        int numpoints=0;
        for (int i=0;i<cnt;i++)
         {
          int16_t x,y;
          if (fread(&y,1,2,f)!=2 || fread(&x,1,2,f)!=2)
           break;
          pts[numpoints].first=limit_uint8(be2me_16(x));
          pts[numpoints].second=limit_uint8(be2me_16(y));
          numpoints++;
         }
        std::sort(pts,pts+numpoints);
        for (int i=0;i<numpoints;i++)
         {
          cfgSet(TwidgetCurves::idffs[2*i+0],pts[i].first);
          cfgSet(TwidgetCurves::idffs[2*i+1],pts[i].second);
         }
        wCurves->reset();
        map2dlg();
       }
     }
   fclose(f);
  }
}

bool TlevelsPage::reset(bool testonly)
{
 if (!testonly)
  {
   repaint(GetDlgItem(m_hwnd,IDC_BMP_LEVELS_IN));
   repaint(GetDlgItem(m_hwnd,IDC_BMP_LEVELS_OUT));
   wCurves->reset();
  }
 return true;
}

void TlevelsPage::translate(void)
{
 TconfPageDecVideo::translate();

 cbxTranslate(IDC_CBX_LEVELS_MODE,TlevelsSettings::modes);
}

TlevelsPage::TlevelsPage(TffdshowPageDec *Iparent,const TfilterIDFF *idff):TconfPageDecVideo(Iparent,idff),filter(NULL)
{
 resInter=IDC_CHB_LEVELS;
 helpURL=_l("levels.html");
 curvesflnm[0]='\0';
 static const TbindCheckbox<TlevelsPage> chb[]=
  {
   IDC_CHB_LEVELS_ONLYLUMA,IDFF_levelsOnlyLuma,&TlevelsPage::levels2dlg,
   IDC_CHB_LEVELS_SHOW_HISTOGRAM,IDFF_buildHistogram,NULL,
   IDC_CHB_LEVELS_SHOW_HISTOGRAM_FULL,IDFF_levelsFullY,&TlevelsPage::map2dlg,
   IDC_CHB_LEVELS_INPUT_AUTO,IDFF_levelsInAuto,NULL,
   0,NULL,NULL
  };
 bindCheckboxes(chb);
 static const TbindTrackbar<TlevelsPage> htbr[]=
  {
   IDC_TBR_LEVELS_GAMMA,IDFF_levelsGamma,&TlevelsPage::levels2dlg,
   IDC_TBR_LEVELS_POSTERIZE,IDFF_levelsPosterize,&TlevelsPage::levels2dlg,
   0,0,NULL
  };
 bindHtracks(htbr);
 static const TbindCombobox<TlevelsPage> cbx[]=
  {
   IDC_CBX_LEVELS_MODE,IDFF_levelsMode,BINDCBX_SEL,&TlevelsPage::mode2dlg,
   0
  };
 bindComboboxes(cbx);
 static const TbindButton<TlevelsPage> bt[]=
  {
   IDC_BT_LEVELS_CURVES_LOAD,&TlevelsPage::onCurveLoad,
   0,NULL
  };
 bindButtons(bt);
 static const TbindEditInt<TlevelsPage> edInt[]=
  {
   IDC_ED_Y_MAX_DELTA ,1,50,IDFF_levelsYmaxDelta,NULL,
   IDC_ED_Y_THRESHOLD ,1,150,IDFF_levelsYthreshold,NULL,
   IDC_ED_Y_TEMPORAL ,1,20,IDFF_levelsYtemporal,NULL,
   0
  };
 bindEditInts(edInt);
}
