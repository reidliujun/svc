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
#include "Tfilter.h"
#include "IffdshowBase.h"
#include "IffdshowDec.h"
#include "TpresetSettings.h"

Tfilter::Tfilter(IffdshowBase *Ideci):deci(Ideci),deciD(Ideci)
{
}
Tfilter::~Tfilter()
{
}

void TfilterQueueItem::copyCfg(const Tpreset *cfgsrc)
{
 if (owncfg.size())
  memcpy(owncfg.begin(),cfgsrc->indexGetSettings(index),owncfg.size());
}

void TfilterQueue::copyCfg(const Tpreset *cfg)
{
 for (iterator f=begin();f!=end();f++)
  f->copyCfg(cfg);
}
