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



#include "data.h"
#include "cabac.h"
#include "vlc_cabac.h"
#include "fill_caches.h"
#include "mb_pred_main.h"
#include "fill_caches_cabac.h"
#include "mb_cabac_partionning.h"
#include "macroblock_partitionning.h"





/**
Macroblock I decoding process with CABAC entropy.
*/
char mb_cabac_I_partionning (RESIDU *picture_residu, SLICE *slice, DATA *Tab_block,	DATA *block, 
							unsigned char *non_zero_count_cache, unsigned char *SliceTable,
							int iCurrMbAddr, CABACContext *cabac, unsigned char *cabac_state,
							CABAC_NEIGH *neigh, short x, short y, int constrained_intra_pred_flag,
							int dct_allowed, unsigned char *last_QP, int *mb_qp_delta)
{  



	int    coded_block_pattern ;
	short intra4x4_pred_mode_cache[40];

#ifdef ERROR_DETECTION
	//Error detection
	if(ErrorsCheckIMbType(picture_residu -> MbType)){
		return 1;
	}
#endif

	//Updating the slice table in order to save in which slice each macroblock belong to
	if ( picture_residu -> MbType == INTRA_PCM ) {
		unsigned char *ptr = cabac -> bytestream;
		int Position = 0;
		if (cabac -> low & 0x1) {
			ptr -= CABAC_BITS >> 3;
		}
		ParseIPCM(ptr, &Position, picture_residu, non_zero_count_cache);
		init_cabac_decoder(cabac, &ptr[384], (int)(cabac -> bytestream_end - ptr));
		picture_residu -> Cbp = 0x1ef;
		*mb_qp_delta = 0;
	} else {
		//Updating the parameter in order to decode the VLC

		fill_caches_I( slice, picture_residu, 1, non_zero_count_cache, block, SliceTable, 
			intra4x4_pred_mode_cache, x, y, constrained_intra_pred_flag);
		fill_caches_cabac (slice, neigh, SliceTable, iCurrMbAddr, *mb_qp_delta, picture_residu );

		//Recovery of the motion vectors for the sub_macroblock 
		if ( dct_allowed && picture_residu -> MbType == INTRA_4x4 && 
			decode_cabac_mb_transform_size(cabac, cabac_state, Tab_block, picture_residu -> AvailMask, iCurrMbAddr, slice -> mb_stride)){
				picture_residu -> MbType = picture_residu -> Transform8x8 = block -> Transform8x8 = block -> MbPartPredMode [0] = INTRA_8x8;
		}

		//Recovery of the prediction mode and the motion vectors for the macroblock 
		if ( block -> MbPartPredMode [0] == INTRA_4x4){
			mb_pred_intra_cabac(cabac, cabac_state, picture_residu -> Intra16x16DCLevel, intra4x4_pred_mode_cache);
			picture_residu -> IntraChromaPredMode = decode_cabac_mb_chroma_pre_mode( cabac, cabac_state, picture_residu -> AvailMask, neigh);
		}else if (block -> Transform8x8){
			mb_pred_intra_8x8_cabac(cabac, cabac_state, picture_residu -> Intra16x16DCLevel, intra4x4_pred_mode_cache);
			picture_residu -> IntraChromaPredMode = decode_cabac_mb_chroma_pre_mode( cabac, cabac_state, picture_residu -> AvailMask, neigh);
		}else if ( picture_residu -> MbType != I_BL){
			picture_residu -> IntraChromaPredMode = decode_cabac_mb_chroma_pre_mode( cabac, cabac_state, picture_residu -> AvailMask,  neigh);
		}

#ifdef ERROR_DETECTION
		//Error detection
		if(ErrorsCheckIntraChromaPredMode(&picture_residu -> IntraChromaPredMode)){
			return 1;
		}
#endif


		if ( block -> MbPartPredMode[0] != INTRA_16x16) {
			picture_residu -> Cbp = decode_cabac_mb_cbp_luma( cabac, cabac_state, picture_residu -> AvailMask, neigh, x, y) ;
			picture_residu -> Cbp |= (decode_cabac_mb_cbp_chroma( cabac, cabac_state, neigh)) << 4;
		}

		coded_block_pattern = picture_residu -> Cbp;

		if ( picture_residu -> Cbp > 0 || block -> MbPartPredMode[0] == INTRA_16x16 ) {
			if (picture_residu -> Cbp & 0x0f && dct_allowed && picture_residu -> MbType == 26 && 
				decode_cabac_mb_transform_size(cabac, cabac_state, Tab_block, picture_residu -> AvailMask, iCurrMbAddr, slice -> mb_stride)){
					picture_residu -> Transform8x8 = block -> Transform8x8 = block -> MbPartPredMode [0] = INTRA_8x8;
			}
			*mb_qp_delta = decode_cabac_mb_dqp( cabac, cabac_state, neigh, picture_residu[- 1] . MbType);
#ifdef TI_OPTIM
			//Initialization of the macroblock neighbourhood
			*last_QP = picture_residu -> Qp = divide(*last_QP + *mb_qp_delta + 52, 52) >> 8;
#else
			*last_QP = picture_residu -> Qp = (*last_QP + *mb_qp_delta + 52) % 52;
#endif

			//Decoding process of the VLC 
			residual_cabac(cabac, cabac_state, picture_residu, neigh, non_zero_count_cache, &coded_block_pattern);
			picture_residu -> Cbp = coded_block_pattern;

		} else {
			//In case of there is no error in the prediction
			init_mb_skip(non_zero_count_cache);
			picture_residu -> Qp = *last_QP;
		}
	}
	return 0;
}






/**
Macroblock P decoding process with CABAC entropy.
*/
char mb_cabac_P_partionning (RESIDU *picture_residu, SLICE *slice, DATA *Tab_block, DATA *block, 
							 unsigned char *non_zero_count_cache, unsigned char *SliceTable, int iCurrMbAddr, 
							 CABACContext *cabac, unsigned char *cabac_state, CABAC_NEIGH *neigh, 
							 short mv_cache_l0[][2], short *ref_cache_l0, short x, short y, 
							 int dct_allowed, unsigned char *last_QP, int *mb_qp_delta, short mvl0_cache[][2], short *refl0_cache)
{  



	int coded_block_pattern;

#ifdef ERROR_DETECTION
	//Error detection
	if(ErrorsCheckPMbType(picture_residu -> MbType)){
		return 1;
	}
#endif

	//Updating the parameter in order to decode the VLC
	fill_caches( slice, picture_residu, 1, non_zero_count_cache, block, SliceTable, x, y);
	fill_caches_cabac (slice, neigh, SliceTable, iCurrMbAddr, *mb_qp_delta, picture_residu);

	//Recovery of the motion vectors for the sub_macroblock 
	if ( block -> NumMbPart == 4 ) {
		if(sub_mb_P_cabac(&dct_allowed, cabac, cabac_state, slice, picture_residu, mv_cache_l0, ref_cache_l0, mvl0_cache, refl0_cache)){
			return 1;
		}
	}else{ 
		if(mb_pred_P_cabac(cabac, cabac_state, slice, picture_residu, mv_cache_l0, ref_cache_l0, mvl0_cache, refl0_cache)){
			return 1;
		}	
	}


	picture_residu -> Cbp = decode_cabac_mb_cbp_luma( cabac, cabac_state, picture_residu -> AvailMask, neigh, x, y) ;
	picture_residu -> Cbp |= (decode_cabac_mb_cbp_chroma( cabac, cabac_state,neigh)) << 4 ;

	if ( dct_allowed && (picture_residu -> Cbp & 15)){
		if( decode_cabac_mb_transform_size(cabac, cabac_state, Tab_block, picture_residu -> AvailMask, iCurrMbAddr, slice -> mb_stride)){
			picture_residu -> Transform8x8 = block -> Transform8x8 = INTRA_8x8;
		}
	}


	coded_block_pattern = picture_residu -> Cbp;

	if ( picture_residu -> Cbp > 0 || block -> MbPartPredMode[0] == INTRA_16x16 ) {
		*mb_qp_delta = decode_cabac_mb_dqp( cabac, cabac_state, neigh, picture_residu[- 1] . MbType);
#ifdef TI_OPTIM
		//Initialization of the macroblock neighbourhood
		*last_QP = picture_residu -> Qp = divide(*last_QP + *mb_qp_delta + 52, 52) >> 8;
#else
		*last_QP = picture_residu -> Qp = (*last_QP + *mb_qp_delta + 52) % 52;
#endif

		//Decoding process of the VLC 
		residual_cabac(cabac, cabac_state, picture_residu, neigh ,non_zero_count_cache, &coded_block_pattern);
		picture_residu -> Cbp = coded_block_pattern;

	} else {
		//In case of there is no error in the prediction
		init_mb_skip(non_zero_count_cache);
		picture_residu -> Qp = *last_QP;
	}
	return 0;
}





/**
Macroblock B decoding process with CABAC entropy.
*/
char mb_cabac_B_partionning (RESIDU *picture_residu, SLICE *slice, DATA *Tab_block, unsigned char *non_zero_count_cache, 
							 unsigned char *SliceTable, int iCurrMbAddr, CABACContext *cabac, unsigned char *cabac_state, CABAC_NEIGH *neigh,
							 short mv_cache_l0[][2], short mv_cache_l1[][2], short *ref_cache_l0, short *ref_cache_l1, 
							 short x, short y, int direct_8x8_inference_flag, int dct_allowed, unsigned char *last_QP, int *mb_qp_delta, 
							 int is_svc, short mvl0_cache[][2], short mvl1_cache[][2], short *refl0_cache, short *refl1_cache, 
							 int long_term, int slice_type, short *mvl1_l0, short *mvl1_l1, short *refl1_l0, short *refl1_l1)
{  





	int    coded_block_pattern ;
	DATA *block = &Tab_block[iCurrMbAddr];

#ifdef ERROR_DETECTION
	//Error detection
	if(ErrorsCheckBMbType(picture_residu -> MbType)){
		return 1;
	}
#endif

	//Updating the parameter in order to decode the VLC
	fill_caches( slice, picture_residu, 1, non_zero_count_cache, block, SliceTable, x, y);
	fill_caches_cabac (slice, neigh, SliceTable, iCurrMbAddr, *mb_qp_delta, picture_residu);


	//Recovery of the motion vectors for the sub_macroblock 
	if ( block -> NumMbPart == 4 ) {
		if(sub_mb_pred_cabac(&dct_allowed, cabac, cabac_state, slice, picture_residu,  mv_cache_l0, mv_cache_l1, 
			ref_cache_l0, ref_cache_l1, mvl0_cache, mvl1_cache, refl0_cache, refl1_cache, mvl1_l0, mvl1_l1, 
			refl1_l0, refl1_l1, direct_8x8_inference_flag, long_term, slice_type, is_svc)){
				return 1;
		}
	}else{ 
		if ( picture_residu -> MbType != B_direct){
			if(mb_pred_cabac(cabac, cabac_state, slice, picture_residu, block, mv_cache_l0, mv_cache_l1, 
				ref_cache_l0, ref_cache_l1, mvl0_cache, mvl1_cache, refl0_cache, refl1_cache)){
					return 1;
			}
		}else if (picture_residu -> MbType == B_direct){
			dct_allowed &= direct_8x8_inference_flag;
			motion_vector_B_skip(mvl0_cache, mvl1_cache, refl0_cache, refl1_cache
				, mvl1_l0, mvl1_l1, refl1_l0, refl1_l1, slice, direct_8x8_inference_flag, long_term,  slice_type, is_svc);
		}
	}


	picture_residu -> Cbp = decode_cabac_mb_cbp_luma( cabac, cabac_state, picture_residu -> AvailMask, neigh, x, y) ;
	picture_residu -> Cbp |= (decode_cabac_mb_cbp_chroma( cabac, cabac_state,neigh)) << 4 ;

	if ( dct_allowed && (picture_residu -> Cbp & 15)){
		if( decode_cabac_mb_transform_size(cabac, cabac_state, Tab_block, picture_residu -> AvailMask, iCurrMbAddr, slice -> mb_stride)){
			picture_residu -> Transform8x8 = block -> Transform8x8 = INTRA_8x8;
		}
	}

	coded_block_pattern = picture_residu -> Cbp;

	if ( picture_residu -> Cbp > 0 || block -> MbPartPredMode[0] == INTRA_16x16 ) {
		*mb_qp_delta = decode_cabac_mb_dqp( cabac, cabac_state, neigh, picture_residu [- 1] . MbType);
#ifdef TI_OPTIM
		//Initialization of the macroblock neighbourhood
		*last_QP = picture_residu -> Qp = divide(*last_QP + *mb_qp_delta + 52, 52) >> 8;
#else
		*last_QP = picture_residu -> Qp = (*last_QP + *mb_qp_delta + 52) % 52;
#endif

		//Decoding process of the VLC 
		residual_cabac(cabac, cabac_state, picture_residu, neigh ,non_zero_count_cache, &coded_block_pattern);
		picture_residu -> Cbp = coded_block_pattern;

	} else {
		//In case of there is no error in the prediction
		init_mb_skip(non_zero_count_cache);
		picture_residu -> Qp = *last_QP;
	}

	return 0;
}
