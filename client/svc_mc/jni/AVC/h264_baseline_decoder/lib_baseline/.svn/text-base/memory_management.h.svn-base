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

#ifndef MEMORY_MANAG_H
#define MEMORY_MANAG_H

#include "type.h"

void IdrFlushDpb(int Nb_Layer, MMO *mmo, LIST_MMO *RefPicListL0, LIST_MMO *RefPicListL1);
void idr_flush_dpb( int Nb_Layer, MMO *mmo, LIST_MMO *RefPicListL0, LIST_MMO *RefPicListL1);
void initialize_address(int total_memory, const int mv_memory,int nb_of_layers, int interval, int mv_interval, SPS *sps_id, MMO *mmo);
void avc_calculate_dpb(const int total_memory,const int mv_memory, MMO *mmo_struct, SPS *sps);
void ProvideMemAddress(LIST_MMO *Current_pic, MMO *Mmo, LAYER_MMO *LayerMmo);
#endif


