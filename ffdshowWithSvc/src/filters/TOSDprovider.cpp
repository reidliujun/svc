/*
 * Copyright (c) 2006 Milan Cutka
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
#include "TOSDprovider.h"
#include "IffdshowBase.h"
#include "IffdshowDec.h"
#include "TOSDsettings.h"

STDMETHODIMP_(const char_t*) TOSDprovider::getInfoItemName(int type)
{
 return _deci->getInfoItemName(type);
}
STDMETHODIMP TOSDprovider::getInfoItemValue(int id,const char_t* *value,int *wasChange,int *splitline)
{
 return _deci->getInfoItemValue(id,value,wasChange,splitline);
}
STDMETHODIMP_(const char_t*) TOSDprovider::getFormat(void)
{
 return tempcfg?tempcfg->getFormat():_l("");
}
STDMETHODIMP_(const char_t*) TOSDprovider::getStartupFormat(int *duration)
{
 return _deciD->getOSDstartupFormat(duration);
}
STDMETHODIMP_(bool) TOSDprovider::isOSD(void)
{
 return tempcfg?!!tempcfg->is:false;
}
STDMETHODIMP TOSDprovider::getSave(int *isSave,const char_t* *flnm)
{
 if (!tempcfg) return E_UNEXPECTED;
 *isSave=tempcfg->isSave;
 *flnm=tempcfg->saveFlnm;
 return S_OK;
}
STDMETHODIMP_(int) TOSDprovider::getInfoShortcutItem(const char_t *s,int *toklen)
{
 return _deci->getInfoShortcutItem(s,toklen);
}
