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
#include "Cperspective.h"
#include "TperspectiveSettings.h"
#include "line.h"

LRESULT TperspectivePage::TwidgetPerspective::onLbuttonDown(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
 self->dragx=GET_X_LPARAM(lParam);self->dragy=GET_Y_LPARAM(lParam);
 self->drag=self->bits[self->dragy*self->stride+self->dragx];
 if (self->drag<3 || self->drag>10)
  self->drag=0;
 else
  SetCapture(hwnd);
 return 0;
}
LRESULT TperspectivePage::TwidgetPerspective::onMouseMove(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
 if (self->drag)
  {
   int dragx=GET_X_LPARAM(lParam),dragy=GET_Y_LPARAM(lParam);
   int x=dragx-(self->gx-self->vx)/2,y=dragy-(self->gy-self->vy)/2;
   x=limit(x,0,int(self->vx)-1);y=limit(y,0,int(self->vy)-1);
   x+=1;y+=1;
   x=100*x/self->vx;y=100*y/self->vy;
   int idffx1,idffy1,idffx2=0,idffy2=0;
   switch (self->drag)
    {
     case  3:idffx1=IDFF_perspectiveX1;idffy1=IDFF_perspectiveY1;break;
     case  4:idffx1=IDFF_perspectiveX2;idffy1=IDFF_perspectiveY2;break;
     case  5:idffx1=IDFF_perspectiveX3;idffy1=IDFF_perspectiveY3;break;
     case  6:idffx1=IDFF_perspectiveX4;idffy1=IDFF_perspectiveY4;break;
     case  7:idffx1=IDFF_perspectiveX1;idffy1=IDFF_perspectiveY1;idffx2=IDFF_perspectiveX2;idffy2=IDFF_perspectiveY2;break;
     case  8:idffx1=IDFF_perspectiveX2;idffy1=IDFF_perspectiveY2;idffx2=IDFF_perspectiveX4;idffy2=IDFF_perspectiveY4;break;
     case  9:idffx1=IDFF_perspectiveX4;idffy1=IDFF_perspectiveY4;idffx2=IDFF_perspectiveX3;idffy2=IDFF_perspectiveY3;break;
     case 10:idffx1=IDFF_perspectiveX3;idffy1=IDFF_perspectiveY3;idffx2=IDFF_perspectiveX1;idffy2=IDFF_perspectiveY1;break;
     default:return true;
    }
   if (idffx2==0)
    {
     self->cfgSet(idffx1,x);
     self->cfgSet(idffy1,y);
    }
   else
    {
     int x0=self->dragx-(self->gx-self->vx)/2,y0=self->dragy-(self->gy-self->vy)/2;
     x0=limit(x0,0,int(self->vx)-1);y0=limit(y0,0,int(self->vy)-1);
     x0+=1;y0+=1;
     x0=100*x0/self->vx;y0=100*y0/self->vy;
     int x1=self->deci->getParam2(idffx1)+(x-x0),y1=self->deci->getParam2(idffy1)+(y-y0);
     int x2=self->deci->getParam2(idffx2)+(x-x0),y2=self->deci->getParam2(idffy2)+(y-y0);
     if (isIn(x1,0,100) && isIn(y1,0,100) && isIn(x2,0,100) && isIn(y2,0,100))
      {
       self->cfgSet(idffx1,x1);
       self->cfgSet(idffy1,y1);
       self->cfgSet(idffx2,x2);
       self->cfgSet(idffy2,y2);
      }
     else
      {
       POINT p={dragx=self->dragx,dragy=self->dragy};
       ClientToScreen(hwnd,&p);
       SetCursorPos(p.x,p.y);
      }
    }
   self->dragx=dragx;self->dragy=dragy;
   self->drawCorners();
  }
 return 0;
}
LRESULT TperspectivePage::TwidgetPerspective::onLbuttonUp(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
 if (self->drag) ReleaseCapture();
 self->drag=0;
 return 0;
}

void TperspectivePage::init(void)
{
 hg=GetDlgItem(m_hwnd,IDC_BMP_PERSPECTIVE);
 drag=0;

 RECT r;GetClientRect(hg,&r);
 gx=r.right-r.left;gy=r.bottom-r.top;stride=(gx/4+1)*4;
 bits=(unsigned char*)calloc(stride,gy);
 memset(&bmp.bmiHeader,0,sizeof(bmp.bmiHeader));
 bmp.bmiHeader.biSize=sizeof(bmp.bmiHeader);
 bmp.bmiHeader.biWidth=stride;
 bmp.bmiHeader.biHeight=-1*int(gy);
 bmp.bmiHeader.biPlanes=1;
 bmp.bmiHeader.biBitCount=8;
 bmp.bmiHeader.biCompression=BI_RGB;
 bmp.bmiHeader.biXPelsPerMeter=100;
 bmp.bmiHeader.biYPelsPerMeter=100;
 memset(bmp.bmiColors,0,256*sizeof(RGBQUAD));
 bmp.bmiColors[1].rgbRed=255;bmp.bmiColors[1].rgbGreen=255;bmp.bmiColors[1].rgbBlue=255; //1 - white
 bmp.bmiColors[2].rgbRed=128;bmp.bmiColors[2].rgbGreen=128;bmp.bmiColors[2].rgbBlue=128; //2 - 50% gray
 bmp.bmiColors[3].rgbRed= 28;bmp.bmiColors[3].rgbGreen=255;bmp.bmiColors[3].rgbBlue=  0; //3,4,5,6 - corners
 bmp.bmiColors[4]=bmp.bmiColors[5]=bmp.bmiColors[6]=bmp.bmiColors[3];
 bmp.bmiColors[7].rgbRed= 28;bmp.bmiColors[7].rgbGreen=192;bmp.bmiColors[7].rgbBlue=  0; //7,8,9,10 - lines
 bmp.bmiColors[8]=bmp.bmiColors[9]=bmp.bmiColors[10]=bmp.bmiColors[7];
}

void TperspectivePage::cfg2dlg(void)
{
 memset(bits,2,stride*gy);
 deciV->getAVIdimensions(&AVIdx,&AVIdy);
 if (AVIdx==0 || AVIdy==0)
  {
   AVIdx=640;AVIdy=480;
  }
 vy=AVIdy*gx/AVIdx;vx=gx;
 if (vy>gy)
  {
   vx=AVIdx*gy/AVIdy;
   vy=gy;
  }
 cbxSetCurSel(IDC_CBX_PERSPECTIVE_INTERPOLATION,cfgGet(IDFF_perspectiveInterpolation));
 setCheck(IDC_CHB_PERSPECTIVE_ISSRC,cfgGet(IDFF_perspectiveIsSrc));
 drawCorners();
}

void TperspectivePage::box3( int x, int y,unsigned char c)
{
 for (int yy=y-1;yy<=y+1;yy++)
  for (int xx=x-1;xx<=x+1;xx++)
   if ((unsigned int)xx<gx && (unsigned int)yy<gy)
    bits[yy*stride+xx]=c;
}

void TperspectivePage::line(int x1,int y1,int x2,int y2,unsigned char color)
{
 drawline<TputColor>(x1,y1,x2,y2,color,bits,stride);
}

void TperspectivePage::drawCorners(void)
{
 memset(bits,2,stride*gy);
 for (unsigned int y=(gy-vy)/2;y<gy-(gy-vy)/2;y++) memset(bits+stride*y+(gx-vx)/2,0,vx);
 int difx=(gx-vx)/2,dify=(gy-vy)/2;
 int x1=(vx-1)*deci->getParam2(IDFF_perspectiveX1)/100+difx;
 int y1=(vy-1)*deci->getParam2(IDFF_perspectiveY1)/100+dify;
 int x2=(vx-1)*deci->getParam2(IDFF_perspectiveX2)/100+difx;
 int y2=(vy-1)*deci->getParam2(IDFF_perspectiveY2)/100+dify;
 int x3=(vx-1)*deci->getParam2(IDFF_perspectiveX3)/100+difx;
 int y3=(vy-1)*deci->getParam2(IDFF_perspectiveY3)/100+dify;
 int x4=(vx-1)*deci->getParam2(IDFF_perspectiveX4)/100+difx;
 int y4=(vy-1)*deci->getParam2(IDFF_perspectiveY4)/100+dify;
 line(x1,y1,x2,y2,7);line(x2,y2,x4,y4,8);line(x4,y4,x3,y3,9);line(x3,y3,x1,y1,10);
 box3(x1,y1,3);box3(x2,y2,4);box3(x3,y3,5);box3(x4,y4,6);
 repaint(hg);
}

INT_PTR TperspectivePage::msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
 switch (uMsg)
  {
   case WM_DESTROY:
    //SetWindowLong(hg,GWL_WNDPROC,LONG(oldBmpWndProc));
    if (bits) free(bits);bits=NULL;
    break;
   case WM_DRAWITEM:
    switch (wParam)
     {
      case IDC_BMP_PERSPECTIVE:
       {
        LPDRAWITEMSTRUCT dis=LPDRAWITEMSTRUCT(lParam);
        SetDIBitsToDevice(dis->hDC,0,0,gx,gy,0,0,0,gy,bits,(BITMAPINFO*)&bmp,DIB_RGB_COLORS);
        return TRUE;
       }
     }
    break;
   case WM_SETCURSOR:
    if (HWND(wParam)==hg)
     {
      POINT sm;GetCursorPos(&sm);
      ScreenToClient(hg,&sm);
      if (sm.x<0 || sm.x>=(int)gx || sm.y<0 || sm.y>=(int)gy) return FALSE;
      unsigned char c=bits[sm.x+sm.y*stride];
      if (c>=3 && c<=6)
       {
        SetCursor(LoadCursor(NULL,IDC_SIZEALL));
        setDlgResult(TRUE);
       }
      else if (c>=7 && c<=10)
       {
        int idffx1,idffy1,idffx2,idffy2;
        switch (c)
         {
          case  7:idffx1=IDFF_perspectiveX1;idffy1=IDFF_perspectiveY1;idffx2=IDFF_perspectiveX2;idffy2=IDFF_perspectiveY2;break;
          case  8:idffx1=IDFF_perspectiveX2;idffy1=IDFF_perspectiveY2;idffx2=IDFF_perspectiveX4;idffy2=IDFF_perspectiveY4;break;
          case  9:idffx1=IDFF_perspectiveX4;idffy1=IDFF_perspectiveY4;idffx2=IDFF_perspectiveX3;idffy2=IDFF_perspectiveY3;break;
          case 10:idffx1=IDFF_perspectiveX3;idffy1=IDFF_perspectiveY3;idffx2=IDFF_perspectiveX1;idffy2=IDFF_perspectiveY1;break;
          default:return FALSE;
         }
        int x1=cfgGet(idffx1),y1=cfgGet(idffy1);
        int x2=cfgGet(idffx2),y2=cfgGet(idffy2);
        double t=atan2(double(y2-y1),double(x2-x1));
        t=16*t/M_PI;
        if (t>=8) t-=16;
        LPCTSTR cursor;
        if      (isIn(t,-8.0,-7.0)) cursor=IDC_SIZEWE;
        else if (isIn(t,-7.0,-2.0)) cursor=IDC_SIZENWSE;
        else if (isIn(t,-2.0, 2.0)) cursor=IDC_SIZENS;
        else cursor=IDC_SIZENESW;
        SetCursor(LoadCursor(NULL,cursor));
        setDlgResult(TRUE);
       }
      return TRUE;
     }
    break;
  }
 return TconfPageDecVideo::msgProc(uMsg,wParam,lParam);
}
Twidget* TperspectivePage::createDlgItem(int id,HWND h)
{
 if (id==IDC_BMP_PERSPECTIVE)
  return new TwidgetPerspective(h,this);
 else
  return TconfPageDecVideo::createDlgItem(id,h);
}

void TperspectivePage::translate(void)
{
 TconfPageBase::translate();

 cbxTranslate(IDC_CBX_PERSPECTIVE_INTERPOLATION,TperspectiveSettings::interpolationNames);
}

TperspectivePage::TperspectivePage(TffdshowPageDec *Iparent,const TfilterIDFF *idff):TconfPageDecVideo(Iparent,idff)
{
 resInter=IDC_CHB_PERSPECTIVE;
 bits=NULL;
 static const TbindCheckbox<TperspectivePage> chb[]=
  {
   IDC_CHB_PERSPECTIVE_ISSRC,IDFF_perspectiveIsSrc,NULL,
   0,NULL,NULL
  };
 bindCheckboxes(chb);
 static const TbindCombobox<TperspectivePage> cbx[]=
  {
   IDC_CBX_PERSPECTIVE_INTERPOLATION,IDFF_perspectiveInterpolation,BINDCBX_SEL,NULL,
   0
  };
 bindComboboxes(cbx);
}
