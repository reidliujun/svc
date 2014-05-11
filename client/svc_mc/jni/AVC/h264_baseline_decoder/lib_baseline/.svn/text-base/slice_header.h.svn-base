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




#ifndef SLICE_HEADER_H
#define SLICE_HEADER_H

#include "type.h"
#include "pic_list.h"

void SliceManagement(PPS *Pps, SLICE *Slice);
void ref_pic_list_reordering ( unsigned char *data,int *aio_piPosition, SLICE *Slice,
							  SPS *sps, LAYER_MMO *LayerMmo ,LIST_MMO *Current_pic , 
							  LIST_MMO RefPicListL0[], LIST_MMO RefPicListL1[] );

void GetMMCOOperationCode(SPS *sps, LAYER_MMO *LayerMmo, unsigned char *data, int *Position);
void direct_dist_scale_factor(short dist_scale_factor[], LAYER_MMO *LayerMmo, 
							  LIST_MMO *ref_listl0, LIST_MMO * ref_listl1, int poc);


void dec_ref_pic_marking ( unsigned char *data, int *aio_piPosition, int nal_unit_type, 
						  SPS *sps, LAYER_MMO *LayerMmo);

void rbsp_aio_pstSlice_trailing_bits ( unsigned char *data,int *aio_piPosition, int NalBytesInNalunit, PPS *pps_ );
void init_slice ( SLICE *Slice );
void reinit_slice ( SLICE *Slice );

void FrameHeaderData(unsigned char *data, int *Position, int NalRefIdc, int IdrFlag, int NumOfLayer, int EndOfSlice, 
					 LIST_MMO *Current_pic, LAYER_MMO *LayerMmo, SPS *sps_id, PPS *pps_id, 
					 MMO *mmo, LIST_MMO *RefPicListL0, LIST_MMO *RefPicListL1);

void ReferenceListProcessing(unsigned char *data, int *Position, SLICE *Slice, 
							 PPS *pps_id, SPS *sps_id, LAYER_MMO *LayerMmo, 
							 LIST_MMO *Current_pic, LIST_MMO *RefPicListL0, LIST_MMO *RefPicListL1);

void PredWeightTable(unsigned char *data, int *Position, PPS *pps_id, SLICE *Slice);
void QpAndDeblockProcessing(unsigned char *data, int *Position, SLICE *Slice, PPS *pps_id, SPS *sps_id);

void slice_header ( unsigned char *data, int *nal_unit_type, int *nal_ref_idc, 
				   PPS *pps, int *Position, SPS *sps_id, PPS *pps_id, 
				   int *EntropyCodingType, W_TABLES *quantif, LIST_MMO *Current_pic, 
				   SPS *sps, SLICE *Slice, MMO *mmo, LIST_MMO RefPicListL0[], 
				   LIST_MMO RefPicListL1[], int *EndOfSlice,
				   int *ImgToDisplay, int *xsize, int *ysize, int *AddressPic, int *Crop);

void Slice_layer_without_partitioning_rbsp ( unsigned char *ai_pcData, const int *ai_piNalBytesInNalunit, const int *aio_piPosition, 
											const SPS *Sps, PPS *Pps, VLC_TABLES *Vlc, int NonZeroCountCache [ ],
											STRUCT_PF *pf_struct, int *ai_aio_pstSlice_group_id, SLICE *Slice, 
											int *aio_tiMbToSliceGroupMap, int *SliceTable, DATA aio_tstBlock [ ], 
											RESIDU *picture_residu,  int *   aio_piEnd_of_slice , int *mv_io, int *ref_io) ;

#endif
