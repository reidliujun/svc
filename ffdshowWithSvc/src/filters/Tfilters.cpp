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
#include "Tfilters.h"
#include "IffdshowBase.h"
#include "IffdshowDec.h"
#include "ffdshow_constants.h"
#include "TpresetSettings.h"

Tfilters::Tfilters(IffdshowBase *Ideci):deci(Ideci),deciD(Ideci)
{
 deci->getConfig(&config);
 queueChanged=1;
 isdvdproc=!!deci->getParam2(IDFF_dvdproc);
}

Tfilters::~Tfilters()
{
 for (iterator i=begin();i!=end();i++)
  {
   i->second->done();
   delete i->second;
  }
}

void Tfilters::makeQueue(const Tpreset *cfg,TfilterQueue &queue)
{
 queue.clear();
 cfg->createFilters(this,queue);
}

void Tfilters::onSeek(void)
{
 for (iterator f=begin();f!=end();f++)
  f->second->onSeek();
}

void Tfilters::onFlush(void)
{
 for (iterator f=begin();f!=end();f++)
  f->second->onFlush();
}

void Tfilters::onStop(void)
{
 for (iterator f=begin();f!=end();f++)
  f->second->onStop();
}

void Tfilters::onDisconnect(PIN_DIRECTION dir)
{
 for (iterator f=begin();f!=end();f++)
  f->second->onDisconnect(dir);
}

void Tfilters::onQueueChange(int id,int val)
{
 InterlockedIncrement((LONG*)&queueChanged);
}

HRESULT Tfilters::queryFilterInterface(const IID &iid,void **ptr) const
{
 for (const_iterator i=begin();i!=end();i++)
  if (i->second->queryInterface(iid,ptr)==S_OK)
   return S_OK;
 return E_NOINTERFACE;
}
