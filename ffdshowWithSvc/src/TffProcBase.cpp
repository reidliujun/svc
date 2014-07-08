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
#include "TffProcBase.h"
#include "IffdshowBase.h"
#include "IffdshowDec.h"
#include "ffdshow_constants.h"
#include "reg.h"
#include "IffdshowParamsEnum.h"

TffProcBase::TffProcBase(int Ifiltermode):filtermode(Ifiltermode)
{
}
TffProcBase::~TffProcBase()
{
}

void TffProcBase::initDeci(IUnknown *unk)
{
 deci=unk;deciD=unk;
 deci->putParam(IDFF_filterMode,filtermode|IDFF_FILTERMODE_PROC);
 deciD->initPresets();
}

STDMETHODIMP TffProcBase::loadPreset(const char *presetName)
{
 deciD->setActivePreset(text<char_t>(presetName),true);
 return S_OK;
}
STDMETHODIMP TffProcBase::setTempPreset(const char *tempPresetName)
{
 deciD->createTempPreset(text<char_t>(tempPresetName));
 deciD->setActivePreset(text<char_t>(tempPresetName),false);
 return S_OK;
}
STDMETHODIMP TffProcBase::setActivePreset(const char *presetName)
{
 return deciD->setActivePreset(text<char_t>(presetName),false);
}
STDMETHODIMP TffProcBase::saveBytestreamConfig(void *buf,size_t len)
{
 return deci->savePresetMem(buf,len);
}
STDMETHODIMP TffProcBase::loadBytestreamConfig(const void *buf,size_t len)
{
 return deci->loadPresetMem(buf,len);
}

void TffProcBase::saveConfig(TregOpIDstreamWrite &t)
{
 int procLen=0;
 saveBytestreamConfig(&procLen,0);
 if (procLen>0)
  {
   void *procBuf=malloc(procLen);
   if (saveBytestreamConfig(procBuf,procLen)==S_OK)
    {
     t.append((char)0);
     t.append((short int)0);
     t.append(procBuf,procLen);
    }
   free(procBuf);
  }
}
void TffProcBase::loadConfig(bool notreg,const unsigned char *buf,size_t len)
{
 setActivePreset(notreg?"ffdshow proc temporary preset":"ffdshow proc");
 loadBytestreamConfig(buf,len);
}

STDMETHODIMP TffProcBase::config(HWND owner)
{
 return deci->showCfgDlg(owner);
}

STDMETHODIMP TffProcBase::putStringParam(const char *param,char sep)
{
 return deciD->putStringParams(text<char_t>(param),sep,false);
}

STDMETHODIMP TffProcBase::getParamsEnum(IffdshowParamsEnum* *enumPtr)
{
 if (!enumPtr) return E_POINTER;
 return deci->QueryInterface(IID_IffdshowParamsEnum,(void**)enumPtr);
}

STDMETHODIMP TffProcBase::notifyParamsChanged(void)
{
 return deci->notifyParamsChanged();
}
STDMETHODIMP TffProcBase::saveActivePreset(const char *name)
{
 return deciD->saveActivePreset(text<char_t>(name));
}
STDMETHODIMP TffProcBase::putParam(unsigned int paramID,int val)
{
 return deci->putParam(paramID,val);
}
STDMETHODIMP TffProcBase::setBasePageSite(IPropertyPage *page)
{
 return page->SetObjects(1,(IUnknown**)&deci);
}
