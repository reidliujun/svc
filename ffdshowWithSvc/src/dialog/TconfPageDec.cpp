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
#include "TconfPageDec.h"
#include "TffdshowPageDec.h"
#include "TfilterSettings.h"

TconfPageDec::TconfPageDec(TffdshowPageDec *Iparent,const TfilterIDFF *idff,int IfilterPageId):TconfPageBase(Iparent,IfilterPageId)
{
 parent=Iparent;
 deciD=deci;
 resInter=0;
 if (!idff)
  idffInter=idffFull=idffHalf=idffShow=0;
 else
  {
   inPreset=1;
   filterID=idff->id;
   idffShow=idff->show;
   if (filterPageID<=1)
    {
     idffInter=idff->is;
     idffFull=idff->full;
     idffHalf=idff->half;
     dialogId=idff->dlgId;
    }
   else
    idffInter=idffFull=idffHalf=0;
  }
 idffOrder=(idff && idff->order && filterPageID<=1)?idff->order:deciD->getMinOrder2()-1;
}

int TconfPageDec::getInter(void)
{
 if (!idffInter) return -1;
 else return cfgGet(idffInter);
}
bool TconfPageDec::invInter(void)
{
 if (idffInter)
  {
   cfgInv(idffInter);
   return true;
  }
 else
  return false;
}
int TconfPageDec::getShow(void)
{
 return !idffShow?-1:cfgGet(idffShow);
}
void TconfPageDec::setShow(int s)
{
 if (idffShow>0) cfgSet(idffShow,s);
}
int TconfPageDec::getProcessFull(void)
{
 return !idffFull?-1:cfgGet(idffFull);
}
void TconfPageDec::setProcessHalf(int half)
{
 if (idffHalf) cfgSet(idffHalf,half);
}
int TconfPageDec::getProcessHalf(void)
{
 return !idffHalf?-1:cfgGet(idffHalf);
}
void TconfPageDec::setProcessFull(int full)
{
 if (idffFull) cfgSet(idffFull,full);
}
void TconfPageDec::interDlg(void)
{
 if (resInter) setCheck(resInter,cfgGet(idffInter));
}
void TconfPageDec::setOrder(int o)
{
 if (idffOrder>0) cfgSet(idffOrder,o);
}
int TconfPageDec::getOrder(void)
{
 return (idffOrder<=0 || idffOrder>=maxOrder)?idffOrder:cfgGet(idffOrder);
}

void TconfPageDec::cfgInv(unsigned int i)
{
 deci->invParam(i);
 interDlg();
}

bool TconfPageDec::hasReset(void)
{
 bool res=false;
 if (filterID) res|=deciD->filterHasResetEx(filterID,filterPageID)==S_OK;
 res|=reset(true);
 return res;
}

INT_PTR TconfPageDec::msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
 switch (uMsg)
  {
   case WM_COMMAND:
    if (idffInter!=0 && resInter!=0 && LOWORD(wParam)==resInter)
     {
      cfgSet(idffInter,getCheck(resInter));
      parent->drawInter();
      return TRUE;
     }
    break;
  }
 return TconfPageBase::msgProc(uMsg,wParam,lParam);
}
