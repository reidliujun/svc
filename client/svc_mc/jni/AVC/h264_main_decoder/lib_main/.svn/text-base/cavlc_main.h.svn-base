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


#ifndef CAVLC_MAIN_H
#define CAVLC_MAIN_H

#include "data.h"
#include "cavlc.h"
#include "slice_data_vlc.h"
#include "SliceGroupMap.h"


void slice_data_B_cavlc ( RESIDU *picture_residu, const unsigned char *ai_pcData, const int *ai_piNalBytesInNalunit, 
						 const SPS *Sps, const PPS *Pps, DATA *aio_tstBlock, SLICE *Slice, 
						 int *aio_piPosition, const VLC_TABLES *Vlc, unsigned char *SliceTable,  
						 const unsigned char *ai_tiMbToSliceGroupMap, int *EndOfSlice, LIST_MMO *RefListl1, 
						 short *mvl0_io, short *mvl1_io, short *refl0_io, short *refl1_io, 
						 short *mvl1_l0, short *mvl1_l1, short *refl1_l0, short *refl1_l1);

#endif
