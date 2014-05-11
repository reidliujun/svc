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

#ifndef LOOP_FILTER_EXT_H
#define LOOP_FILTER_EXT_H

#include "svc_type.h"

void Activ_base_filter(const int nb_num,SLICE *slice,int *Activ_filter);
void Activ_layer_filter(SLICE *slice, NAL *Nal,int *Activ_filter);

void Deblocking_base_layer(const SPS *Sps,const PPS *pps,const RESIDU *residu, const NAL *Nal, int InterLayerDeblockingFilterIdc, 
						   int AlphaOffset, int BetaOffset, unsigned char Y[], unsigned char U[], unsigned char V[]);

void Loop_filter_svc(const int size_mb, const NAL *Nal, SPS *Sps, PPS *pps, SLICE *Slice, LIST_MMO *Current_pic, 
					 unsigned char *SliceTable, short *aio_tiMv_l0, short *aio_tiMv_l1, short *aio_tiRef_l0, 
					 short *aio_tiRef_l1, RESIDU *all_residu, unsigned char Y[], unsigned char U[], unsigned char V[]);


void Loop_filter_avc(const int size_mb, int layer_id, SPS *Sps, PPS *pps, SLICE *slice, LIST_MMO *Current_pic, 
					 unsigned char *SliceTable, DATA *Tab_Block, short *aio_tiMv_l0, short *aio_tiMv_l1, short *aio_tiRef_l0, 
					 short *aio_tiRef_l1, RESIDU *all_residu, unsigned char Y[], unsigned char U[], unsigned char V[]);




static __inline void SetLoopFilterQp(RESIDU *CurrResidu, const NAL *Nal, short iCurrMbAddr){



	if ( (iCurrMbAddr != 0) && Nal -> TCoeffPrediction && !Nal -> no_inter_layer_pred_flag && (CurrResidu -> Cbp == 0) && (CurrResidu -> MbType != INTRA_16x16)){
		CurrResidu -> QpLf = CurrResidu [-1] . QpLf;
	}else{
		CurrResidu -> QpLf = CurrResidu -> Qp;
	}
}
#endif
