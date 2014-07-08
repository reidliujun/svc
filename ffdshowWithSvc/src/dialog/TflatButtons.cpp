/*
 * Copyright (c) 2005,2006 Milan Cutka
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
#include "TflatButtons.h"
#include "ffdebug.h"

VOID CALLBACK TflatButton::trackMouseTimerProc(HWND hwnd,UINT uMsg,UINT idEvent,DWORD dwTime)
{
 RECT rect;
 POINT pt;
 GetClientRect(hwnd,&rect);
 MapWindowPoints(hwnd,NULL,(LPPOINT)&rect,2);
 GetCursorPos(&pt);
 if (!PtInRect(&rect,pt) || (WindowFromPoint(pt)!=hwnd))
  {
   KillTimer(hwnd,idEvent);
   TflatButton *self=(TflatButton*)getDlgItem(hwnd);
   self->isMouse=false;
   InvalidateRect(hwnd,NULL,FALSE);
  }
}

LRESULT TflatButton::onMouseMove(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
 if (!isMouse)
  {
   RECT rect;
   POINT pt;
   GetClientRect(hwnd,&rect);
   MapWindowPoints(hwnd,NULL,(LPPOINT)&rect,2);
   GetCursorPos(&pt);
   if (PtInRect(&rect,pt) && (WindowFromPoint(pt)==hwnd))
    {
     isMouse=true;
     InvalidateRect(hwnd,NULL,FALSE);
     SetTimer(hwnd,UINT_PTR(hwnd),100,(TIMERPROC)trackMouseTimerProc);
    }
  }
 return TwindowWidget::onMouseMove(hwnd,uMsg,wParam,lParam);
}
void TflatButton::paint(DRAWITEMSTRUCT *dis)
{
 RECT &r=dis->rcItem;
 FillRect(dis->hDC,&r,GetSysColorBrush(COLOR_BTNFACE));
 SetTextAlign(dis->hDC,TA_LEFT|TA_BASELINE);
 char_t pomS[256];
 GetWindowText(dis->hwndItem,pomS,255);
 SIZE sz;
 GetTextExtentPoint32(dis->hDC,pomS,(int)strlen(pomS),&sz);
 TextOut(dis->hDC,(r.right-sz.cx)/2,r.bottom-sz.cy/2,pomS,(int)strlen(pomS));
 if (isMouse)
  DrawEdge(dis->hDC,&r,(dis->itemState&ODS_SELECTED)?EDGE_SUNKEN:EDGE_RAISED,BF_RECT);
 else
  DrawEdge(dis->hDC,&r,EDGE_SUNKEN,BF_FLAT|BF_RECT);
}
