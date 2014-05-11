/*************************************************************************
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


#ifndef SLICE_DATA_CABAC_H
#define SLICE_DATA_CABAC_H

#include "type.h"
#include "data.h"
#include "cavlc.h"
#include "cabac.h"
#include "main_data.h"
#include "bitstream.h"
#include "vlc_cabac.h"
#include "fill_caches.h"
#include "SliceGroupMap.h"
#include "slice_data_vlc.h"
#include "macroblock_cabac.h"






char slice_data_I_cabac ( RESIDU *picture_residu, unsigned char *data, int *NalBytesInNalunit, 
						 SPS *sps, PPS *pps, DATA *block, SLICE *slice, int *aio_piPosition, 
						 unsigned char *SliceTable, unsigned char *MbToSliceGroupMap, int *EndOfSlice );


char slice_data_P_cabac (RESIDU *picture_residu, unsigned char *data,int *NalBytesInNalunit, SPS *sps, 
						 PPS *pps, DATA *block, SLICE *slice,int *aio_piPosition, unsigned char *SliceTable, 
						 unsigned char *MbToSliceGroupMap, short *mv_cabac_l0, short *ref_cabac_l0,
						 int *EndOfSlice, short *mvl0_io, short *refl0_io);

	
char slice_data_B_cabac ( RESIDU *picture_residu, unsigned char *data,int *NalBytesInNalunit, SPS *sps, 
						 PPS *pps, DATA *block, SLICE *slice, int *aio_piPosition, unsigned char *SliceTable,
						 unsigned char *MbToSliceGroupMap, short *mv_cabac_l0, short *mv_cabac_l1, short *ref_cabac_l0, 
						 short *ref_cabac_l1, int *EndOfSlice, LIST_MMO * ai_pRefPicListL1, short *mvl0_io, 
						 short *mvl1_io, short *refl0_io, short *refl1_io, int long_term, int slice_type,
						 short *mvl1_l0, short *mvl1_l1, short *refl1_l0, short *refl1_l1);

void CabacDecoderInitAVC(CABACContext *cabac, unsigned char *Data, const int *NalBytesInNalunit, int Pos, int slice_QP, int CabacContext[][2]);
#endif
