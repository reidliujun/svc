/*****************************************************************************
   *
   *  Open SVC Decoder developped in IETR image lab
   *
   *
   *
   *              Médéric BLESTEL <mblestel@insa-rennes.Fr>
   *              Mickael RAULET <mraulet@insa-rennes.Fr>
   *			  Maxime  PELCAT <mpelcat@insa-rennes.Fr>
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
   * but WITHOUT ANY WARRANTY; without even the implied warranty off
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

#ifndef MACRO_LAYER_H
#define MACRO_LAYER_H

#include "type.h"

char macroblock_layer_I (const PPS *Pps, RESIDU *picture_residu, const unsigned char *ai_pcData, int *aio_piPosition, 
						SLICE *Slice, DATA *aio_tstBlock, const VLC_TABLES * Vlc, 
						unsigned char NonZeroCountCache [ ], unsigned char *SliceTable, 
						const short ai_iMb_x, const short ai_iMb_y, unsigned char *last_QP);



char macroblock_layer_P (const PPS *Pps, RESIDU *picture_residu, const unsigned char *ai_pcData, int *aio_piPosition, 
						SLICE *Slice, DATA *aio_tstBlock, const VLC_TABLES * Vlc, 
						unsigned char NonZeroCountCache [ ], unsigned char *SliceTable, 
						const short ai_iMb_x, const short ai_iMb_y, unsigned char *last_QP, short mv_cache_l0[][2], short *ref_cache_l0);

#endif

