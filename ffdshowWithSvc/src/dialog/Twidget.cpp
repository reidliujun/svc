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
#include "Twidget.h"
#include "Twindow.h"
#include "Ttranslate.h"

Twidget::Twidget(HWND Ih):h(Ih),ownproc(false)
{
 if (h)
  {
   SetWindowLongPtr(h,GWLP_USERDATA,LONG_PTR(this));
   id=Twindow::getId(h);
  }
 else
  id=0;
}

LRESULT TwindowWidget::onContextMenu(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
 Twindow *self=window;
 if (self->tr->translateMode)
  {
   int id=(hwnd==self->m_hwnd)?-self->dialogId:Twindow::getId(hwnd);
   if (id!=-1)
    {
     const char_t *oldtrans=self->tr->translate(self->m_hwnd,self->dialogId,id,NULL);
     Twindow::TtranslateCbxs::const_iterator translateCbx=self->translateCbxs.find(id);
     int mode=-1;
     if (oldtrans && translateCbx!=self->translateCbxs.end())
      {
       HMENU hmn=LoadMenu(self->hi,MAKEINTRESOURCE(IDR_MENU_TRANSLATE)),hmn2=GetSubMenu(hmn,0);
       RECT r;
       GetWindowRect(hwnd,&r);
       int cmd=TrackPopupMenu(hmn2,TPM_LEFTALIGN|TPM_TOPALIGN|TPM_RETURNCMD,r.left-1,r.bottom,0,hwnd,0);
       DestroyMenu(hmn);
       switch (cmd)
        {
         case ID_TRANSLATE_CONTROLCAPTION:mode=0;break;
         case ID_TRANSLATE_ASSOCIATEDSTRINGS:mode=1;break;
        }
      }
     else if (oldtrans)
      mode=0;
     else if (translateCbx!=self->translateCbxs.end())
      mode=1;
     switch (mode)
      {
       case 0:
        {
         char_t newtrans[1024];
         ff_strncpy(newtrans, Ttranslate::eolnString2file(oldtrans).c_str(), countof(newtrans));
         if (self->inputString(_l("Translate"),oldtrans,newtrans,1024))
          {
           self->tr->newTranslation(self->dialogId,id,Ttranslate::eolnFile2string(newtrans));
           SetWindowText(hwnd,newtrans);
          }
         return TRUE;
        }
       case 1:
        {
         TdlgPropsEdit::TstringProps props;
         for (strings::const_iterator s=translateCbx->second.begin();s!=translateCbx->second.end();s++)
          props.push_back(std::make_pair(Ttranslate::eolnString2file(*s),Ttranslate::eolnString2file(self->_(0,s->c_str(),false))));
         TdlgPropsEdit dlgProps(props,self->deci);
         if (dlgProps.execute(self->m_hwnd))
          {
           for (TdlgPropsEdit::TstringProps::const_iterator p=props.begin();p!=props.end();p++)
            if (p->first!=p->second)
             self->tr->newTranslation(Ttranslate::eolnFile2string(p->first),Ttranslate::eolnFile2string(p->second));
           self->translate();
          }
         return TRUE;
        }
      }
    }
  }
 return Twidget::onContextMenu(hwnd,uMsg,wParam,lParam);
}
