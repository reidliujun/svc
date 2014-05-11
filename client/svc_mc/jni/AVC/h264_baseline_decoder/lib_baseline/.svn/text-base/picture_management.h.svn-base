/*****************************************************************************
   *
   *  Open SVC Decoder developped in IETR image lab
   *
   *
   *
   *              Médéric BLESTEL <mblestel@insa-rennes.Fr>
   *              Mickael RAULET <mraulet@insa-rennes.Fr>
   *              http://www.ietr.org/
   *
   *
   *
   *
   *
   * This library is free software; you can redistribute it and/or
   * modify it under the terms of the GNU Lesser General Public
   * License as published by the Free Software Foundation; either
   * version 2 of the License, or (at your option) any later version.
   *
   * This library is distributed in the hope that it will be useful,
   * but WITHOUT ANY WARRANTY; without even the implied warranty of
   * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   * Lesser General Public License for more details.
   *
   * You should have received a copy of the GNU Lesser General Public
   * License along with this library; if not, write to the Free Software
   * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
   *
   *
   * $Id$
   *
   **************************************************************************/

#ifndef PIC_MANAG_H
#define PIC_MANAG_H

#include "type.h"



void remove_short(MMO *mmo, LAYER_MMO *LayerMmo, int frame_num);
void remove_unused(MMO *mmo, LAYER_MMO *LayerMmo);
void remove_long(MMO *mmo, LAYER_MMO *LayerMmo,  int long_term_pic_num);
void remove_all(MMO *mmo, LAYER_MMO *LayerMmo);
void sliding_window(MMO *mmo, LAYER_MMO *LayerMmo);
void short_to_long(MMO *mmo, LAYER_MMO *LayerMmo, int frame_num, int Long_Term_Idx);
#endif
