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
#include "TfilterSettings.h"
#include "Tfilters.h"

void TfilterIDFFs::copy(const TfilterIDFFs *orig)
{
 const_iterator src=orig->begin();
 iterator dst=begin();
 for (;dst!=end();src++,dst++)
  if (src->cfg->deepcopy)
   dst->cfg->copy(src->cfg);
  else
   *dst->cfg=*src->cfg;
}

void TfilterSettings::idffOnChange(const TfilterIDFF &idffs,Tfilters *filters,bool temporary) const
{
 if (!temporary)
  {
   if (idffs.is) setOnChange(idffs.is,filters,&Tfilters::onQueueChange);
   if (idffs.show) setOnChange(idffs.show,filters,&Tfilters::onQueueChange);
   if (idffs.order) setOnChange(idffs.order,filters,&Tfilters::onQueueChange);
  }
}

bool TfilterSettings::hasReset(unsigned int pageId)
{
 return !!getResets(pageId);
}
bool TfilterSettings::reset(unsigned int pageId)
{
 if (const int *idResets=getResets(pageId))
  {
   resetValues(idResets);
   return true;
  }
 else
  return false;
}
