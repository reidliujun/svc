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


//AVC Files
#include "type.h"
#include "data.h"
#include "refill.h"
#include "vlc_cabac.h"
#include "main_data.h"
#include "CabacMbPred.h"
#include "mb_pred_main.h"
#include "CabacMvAndRef.h"
#include "motion_vector.h"
#include "Direct_prediction.h"

/**
This function permits to decode the prediction mode or the image reference. 

@param data The NAL unit.
@param aio_piPosition The current aio_piPosition in the NAL.
@param block Contains all parameters of the current macroblock.
@param slice The slice structure.
@param mb_field_decoding_flag Specifies if the current macroblock is a frame macroblock.
@param intra_pred_mode Contains the prediction mode for the current macroblock.

*/

void mb_pred_intra_cabac ( CABACContext *c, unsigned char *state, short *Mode, short *intra4x4_pred_mode_cache){    

	int luma4x4BlkIdx ; 

	//Recovery of the prediction mode
	for ( luma4x4BlkIdx = 0 ; luma4x4BlkIdx < 16 ; luma4x4BlkIdx++ )   {
		Mode[luma4x4BlkIdx] = intra4x4_pred_mode_cache[SCAN8(luma4x4BlkIdx)] = GetCabacIPredMode(c, state, intra4x4_pred_mode_cache, luma4x4BlkIdx);
	}
}

/**
This function permits to decode the prediction mode or the image reference. 

@param data The NAL unit.
@param aio_piPosition The current aio_piPosition in the NAL.
@param block Contains all parameters of the current macroblock.
@param slice The slice structure.
@param mb_field_decoding_flag Specifies if the current macroblock is a frame macroblock.
@param intra_pred_mode Contains the prediction mode for the current macroblock.

*/

void mb_pred_intra_8x8_cabac ( CABACContext *c, unsigned char *state, short *Mode, short *intra4x4_pred_mode_cache)
{


	int luma4x4BlkIdx ; 

	//Recovery of the prediction mode
	for ( luma4x4BlkIdx = 0 ; luma4x4BlkIdx < 4 ; luma4x4BlkIdx++ )   {
		Mode[(luma4x4BlkIdx << 2)] =  Mode[(luma4x4BlkIdx << 2) + 1] = 
			Mode[(luma4x4BlkIdx << 2) + 2] =  Mode[(luma4x4BlkIdx << 2) + 3] = 
			intra4x4_pred_mode_cache[SCAN8(luma4x4BlkIdx << 2)] = intra4x4_pred_mode_cache[SCAN8(1 + (luma4x4BlkIdx << 2))] =
			intra4x4_pred_mode_cache[SCAN8( (luma4x4BlkIdx << 2) + 2)] = 
			intra4x4_pred_mode_cache[SCAN8((luma4x4BlkIdx << 2) + 3)] = GetCabacIPredMode(c, state, intra4x4_pred_mode_cache, luma4x4BlkIdx << 2);
	}
}




/**
Motion vector and reference list decoding process when cabac entropy for a P8x8 macroblock
*/

char sub_mb_P_cabac ( int *dct_allowed, CABACContext *c, unsigned char *state, SLICE *slice, RESIDU *Current_residu, 
					short mv_cache_l0[][2], short *ref_cache_l0, short mvl0_cache[][2], short *refl0_cache)
{	

	int mbPartIdx ;
	int subMbPartIdx ;
	char RefIdxL0[4] = {0, 0, 0, 0};


	//Recovery of the sub-macroblock type
	for ( mbPartIdx = 0 ; mbPartIdx < 4 ; mbPartIdx++ ) {
		Current_residu -> SubMbType [mbPartIdx] = decode_cabac_p_mb_sub_type(c, state);
	}

#ifdef ERROR_DETECTION
	//Error detection 
	if(ErrorsCheckSubMbType( Current_residu -> SubMbType, MaxPSubMbType)){
		//should be set to zero because the macroblock will be decoded.
		refl0_cache [12] = refl0_cache [14] = refl0_cache [28] = refl0_cache [30] = 0;
		return 1;
	}
#endif


	//Recovery of the image reference of each sub-macroblock
	if (slice -> num_RefIdxL0_active_minus1 > 0) {
		if(GetCabacPSubRefIdx(c, state, RefIdxL0, ref_cache_l0, RefIdxL0, slice -> num_RefIdxL0_active_minus1)){
			refl0_cache [12] = refl0_cache [14] = refl0_cache [28] = refl0_cache [30] = 0;
			return 1;
		}
	} else {
		ref_cache_l0[12] = ref_cache_l0[14] = ref_cache_l0[28] = ref_cache_l0[30] = 0;
	}


	//Recovery of the motion vector of each sub-macroblock
	for ( mbPartIdx = 0 ; mbPartIdx < 4 ; mbPartIdx++ ) {
		short MvdL0[2];
		int sub_mb_type = Current_residu -> SubMbType [mbPartIdx];
		for ( subMbPartIdx = 0 ; subMbPartIdx < sub_num_p_part[ sub_mb_type] ; subMbPartIdx++ ) {
			int index = SCAN8(( mbPartIdx << 2) + (sub_mb_type == 1 ? 2 * subMbPartIdx: subMbPartIdx));
			//Decode cabac
			ReadCabacSubMotionVector(c, state, mv_cache_l0, MvdL0, mbPartIdx, subMbPartIdx, sub_mb_type, index, slice -> slice_type);

			//Compute Mv
			refill_ref(2 - (sub_mb_type > 1), 2 - (sub_mb_type & 1), RefIdxL0[mbPartIdx], &refl0_cache[index]);  
			motion_vector(mvl0_cache, MvdL0, refl0_cache, &mvl0_cache[index][0], RefIdxL0[mbPartIdx], index, 2 / ( 1 + ( sub_mb_type > 1)));
		}
		refill_motion_vector(0, sub_mb_type, &mvl0_cache[SCAN8(( mbPartIdx << 2))]);
	}

	for ( mbPartIdx = 0;  mbPartIdx < 4 && dct_allowed; mbPartIdx ++){
		if ( !(sub_num_p_part[Current_residu -> SubMbType[mbPartIdx]] == 1) ){
			*dct_allowed = 0;
		}
	}
	return 0;
}





/**
Motion vector and reference list decoding process when cabac entropy for a P16x16 macroblock
*/

char mb_pred_P_cabac ( CABACContext *c, unsigned char *state, SLICE *slice, RESIDU *CurrResidu, 
					 short mv_cache_l0[][2], short *ref_cache_l0, short mvl0_cache[][2], short *refl0_cache)
{


	int index = 14 * CurrResidu -> Mode;
	char RefIdxL0[2] = {0, 0};

	//Get reference index
	if(GetCabacPRefList(c, state, RefIdxL0, ref_cache_l0, RefIdxL0, CurrResidu -> Mode, slice -> num_RefIdxL0_active_minus1, index)){
		refl0_cache [12] = refl0_cache [14] = refl0_cache [28] = refl0_cache [30] = 0;
		return 1;
	}
	
	//Read motion vector according to the macroblock type.
	if (CurrResidu -> Mode == 3){
		ReadAndComputeCabac16x16MotionVector(c, state, RefIdxL0, refl0_cache, mv_cache_l0, mvl0_cache);
	}else if (CurrResidu -> Mode == 1){
		// mode == 1: 8x16
		ReadAndComputeCabac8x16MotionVector(c, state, RefIdxL0, refl0_cache, mv_cache_l0, mvl0_cache, 12);
		ReadAndComputeCabac8x16MotionVector(c, state, &RefIdxL0[1], refl0_cache, mv_cache_l0, mvl0_cache, 14);
	}else{
		ReadAndComputeCabac16x8MotionVector(c, state, RefIdxL0, refl0_cache, mv_cache_l0, mvl0_cache, 12);
		ReadAndComputeCabac16x8MotionVector(c, state, &RefIdxL0[1], refl0_cache, mv_cache_l0, mvl0_cache, 28);
	}
	return 0;
}







/**
This function permits to decode the image reference of each sub-macroblock. 
Only for B slice

@param data The NAL unit.
@param aio_piPosition The current aio_piPosition in the NAL.
@param block Contains all parameters of the current macroblock.
@param slice The slice structure.
@param mb_field_decoding_flag Specifies if the current macroblock is a frame macroblock.
*/

char sub_mb_pred_cabac ( int *dct_allowed, CABACContext *c, unsigned char *state, SLICE *slice, RESIDU *Current_residu, 
						short mv_cache_l0[][2], short mv_cache_l1[][2], short *ref_cache_l0, short *ref_cache_l1, 
						short mvl0_cache[][2], short mvl1_cache[][2], short *refl0_cache, short *refl1_cache, 
						short *mvl1_l0, short *mvl1_l1, short *refl1_l0, short *refl1_l1, 
						int direct_8x8_inference_flag, int long_term, int slice_type, int is_svc)
{	

	int mbPartIdx ;
	char RefIdxL0[4] = {0, 0, 0, 0};
	char RefIdxL1[4] = {0, 0, 0, 0};


	//Recovery of the sub-macroblock type
	for ( mbPartIdx = 0 ; mbPartIdx < 4 ; mbPartIdx++ ) {
		Current_residu -> SubMbType [mbPartIdx] = decode_cabac_b_mb_sub_type( c, state);
	}

#ifdef ERROR_DETECTION
	//Error detection
	if(ErrorsCheckSubMbType( Current_residu -> SubMbType, MaxBSubMbType)){
		//should be set to zero because the macroblock will be decoded.
		refl0_cache [12] = refl0_cache [14] = refl0_cache [28] = refl0_cache [30] = 0;
		refl1_cache [12] = refl1_cache [14] = refl1_cache [28] = refl1_cache [30] = 0;
		return 1;
	}
#endif

	//L0 and L1 reference index
	if(GetCabacBSubRefList(c, state, RefIdxL0, ref_cache_l0, RefIdxL0, Current_residu -> SubMbType, slice -> num_RefIdxL0_active_minus1, Pred_L1)){
		refl0_cache [12] = refl0_cache [14] = refl0_cache [28] = refl0_cache [30] = 0;
		refl1_cache [12] = refl1_cache [14] = refl1_cache [28] = refl1_cache [30] = 0;
		return 1;
	}
	if(GetCabacBSubRefList(c, state, RefIdxL1, ref_cache_l1, RefIdxL1, Current_residu -> SubMbType, slice -> num_RefIdxL1_active_minus1, Pred_L0)){
		refl1_cache [12] = refl1_cache [14] = refl1_cache [28] = refl1_cache [30] = 0;
		return 1;
	}

	
	//Recovery of the motion vector of each sub-macroblock
	for ( mbPartIdx = 0 ; mbPartIdx < 4 ; mbPartIdx++ ) {
		int sub_mb_type = Current_residu -> SubMbType [mbPartIdx];
		if ( sub_mb_b_name[sub_mb_type] == B_direct){
			int ind = SCAN8(mbPartIdx << 2);
			short mv[2] = {0, 0};
			refill_mv(2, 2, mv, &mv_cache_l0[ind]);

			if (slice -> direct_spatial_mv_pred_flag){
				spatial_direct_prediction_l0(mvl0_cache, mvl1_l0, mvl1_l1, refl0_cache, refl1_cache, refl1_l0, refl1_l1, 
					mbPartIdx, slice -> b_stride, slice -> b8_stride, direct_8x8_inference_flag, long_term, is_svc, slice_type);
			}else{
				temporal_direct_prediction_l0(mvl0_cache, mvl1_l0, refl0_cache, refl1_l0, refl1_l1, slice, 
					mbPartIdx, direct_8x8_inference_flag, is_svc, slice_type);
			}

		}else if (sub_mb_b_name[sub_mb_type] != Pred_L1 ) {
			ReadAndComputeCabac8x8MotionVector( c, state, mbPartIdx, sub_mb_type, RefIdxL0, refl0_cache, mv_cache_l0, mvl0_cache, 1);
		}else {
			int ind = SCAN8(mbPartIdx << 2);
			short mv[2] = {0, 0};
			refill_mv(2, 2, mv, &mv_cache_l0[ind]);
			refill_ref(2, 2, -1, &refl0_cache[ind]); 
		}
	}


	//Recovery of the motion vector of each sub-macroblock
	for ( mbPartIdx = 0 ; mbPartIdx < 4 ; mbPartIdx++ ) {
		int sub_mb_type = Current_residu -> SubMbType [mbPartIdx];
		if ( sub_mb_b_name[ sub_mb_type] == B_direct){
			int ind = SCAN8(mbPartIdx << 2);
			short mv[2] = {0, 0};
			refill_mv(2, 2, mv, &mv_cache_l1[ind]);

			if (slice -> direct_spatial_mv_pred_flag){
				spatial_direct_prediction_l1(mvl1_cache, mvl1_l0, mvl1_l1, refl0_cache, refl1_cache, refl1_l0, refl1_l1
					, mbPartIdx, slice -> b_stride, slice -> b8_stride, direct_8x8_inference_flag, long_term, is_svc , slice_type);
			}else{
				temporal_direct_prediction_l1(mvl1_cache, mvl1_l0, refl1_cache, refl1_l0, refl1_l1,slice
					, mbPartIdx, direct_8x8_inference_flag, is_svc, slice_type);
			}


		}else if (sub_mb_b_name[sub_mb_type] != Pred_L0 ) {
			ReadAndComputeCabac8x8MotionVector( c, state, mbPartIdx, sub_mb_type, RefIdxL1, refl1_cache, mv_cache_l1, mvl1_cache, 1);
		}else{
			int ind = SCAN8(mbPartIdx << 2);
			short mv[2] = {0, 0};
			refill_mv(2, 2, mv, &mv_cache_l1[ind]);
			refill_ref(2, 2, -1, &refl1_cache[ind]); 
		}
	}

	for ( mbPartIdx = 0;  mbPartIdx < 4 && *dct_allowed; mbPartIdx ++){
		if ( !((sub_num_b_part[Current_residu -> SubMbType[mbPartIdx]] == 1) || ( direct_8x8_inference_flag && sub_mb_b_name[Current_residu -> SubMbType[mbPartIdx]] == B_direct))){
			*dct_allowed = 0;
		}
	}
	return 0;
}





/**
Motion vector and reference list decoding process when cabac entropy for a B16x16 macroblock
*/
char mb_pred_cabac ( CABACContext *c, unsigned char *state, SLICE *slice, RESIDU *CurrResidu, DATA *macroblock, 
					short mv_cache_l0[][2], short mv_cache_l1[][2], short *ref_cache_l0, short *ref_cache_l1, 
					short mvl0_cache[][2], short mvl1_cache[][2], short *refl0_cache, short *refl1_cache)
{

	int index = 14 * CurrResidu -> Mode;
	char RefIdxL0[2] = {0, 0};
	char RefIdxL1[2] = {0, 0};

	//Reference index for L0
	if(GetCabacBRefList(c, state, RefIdxL0, ref_cache_l0, RefIdxL0, CurrResidu -> Mode, slice -> num_RefIdxL0_active_minus1, 
		macroblock -> MbPartPredMode, Pred_L1, index)){
			refl0_cache [12] = refl0_cache [14] = refl0_cache [28] = refl0_cache [30] = 0;
			refl1_cache [12] = refl1_cache [14] = refl1_cache [28] = refl1_cache [30] = 0;
			return 1;
	}

	//Reference index for L0
	if(GetCabacBRefList(c, state, RefIdxL1, ref_cache_l1, RefIdxL1, CurrResidu -> Mode, slice -> num_RefIdxL1_active_minus1, 
		macroblock -> MbPartPredMode, Pred_L0, index)){
			refl1_cache [12] = refl1_cache [14] = refl1_cache [28] = refl1_cache [30] = 0;
			return 1;
	}


	if (CurrResidu -> Mode == 3){
		// Pred_L0 prediction
		if ( macroblock -> MbPartPredMode [0] != Pred_L1 ) {
			ReadAndComputeCabac16x16MotionVector(c, state, RefIdxL0, refl0_cache, mv_cache_l0, mvl0_cache);
		}else {
			refl0_cache[14] = refl0_cache[30] = -1;
		}

		// Pred_L1 prediction
		if ( macroblock -> MbPartPredMode [0] != Pred_L0 ) {
			ReadAndComputeCabac16x16MotionVector(c, state, RefIdxL1, refl1_cache, mv_cache_l1, mvl1_cache);
		}else {
			refl1_cache[14] = refl1_cache[30] = -1;
		}
	}else if (CurrResidu -> Mode == 1){
		//Recovery of of the motion vector for the frame P
		// mode == 1: 8x16
	
		//L0 prediction
		if ( macroblock -> MbPartPredMode [0] != Pred_L1 ) {
			ReadAndComputeCabac8x16MotionVector(c, state, RefIdxL0, refl0_cache, mv_cache_l0, mvl0_cache, 12);
		}	

		if ( macroblock -> MbPartPredMode [1] != Pred_L1 ) {
			ReadAndComputeCabac8x16MotionVector(c, state, &RefIdxL0[1], refl0_cache, mv_cache_l0, mvl0_cache, 14);
		}else{
			refl0_cache[14] = refl0_cache[30] = -1;
		}


		//L1 prediction
		if ( macroblock -> MbPartPredMode [0] != Pred_L0 ) {
			ReadAndComputeCabac8x16MotionVector(c, state, RefIdxL1, refl1_cache, mv_cache_l1, mvl1_cache, 12);
		}

		if ( macroblock -> MbPartPredMode [1] != Pred_L0 ) {
			ReadAndComputeCabac8x16MotionVector(c, state, &RefIdxL1[1], refl1_cache, mv_cache_l1, mvl1_cache, 14);
		}else{
			refl1_cache[14] = refl1_cache[30] = -1;
		}
	}else{
		//L0 prediction
		if ( macroblock -> MbPartPredMode [0] != Pred_L1 ) {
			ReadAndComputeCabac16x8MotionVector(c, state, RefIdxL0, refl0_cache, mv_cache_l0, mvl0_cache, 12);
		}else{
			refl0_cache[14] = -1;
		}

		if ( macroblock -> MbPartPredMode [1] != Pred_L1 ) {
			ReadAndComputeCabac16x8MotionVector(c, state, &RefIdxL0[1], refl0_cache, mv_cache_l0, mvl0_cache, 28);		
		}else{
			refl0_cache[30] = -1;
		}

		//L1 prediction
		if ( macroblock -> MbPartPredMode [0] != Pred_L0 ) {
			ReadAndComputeCabac16x8MotionVector(c, state, RefIdxL1, refl1_cache, mv_cache_l1, mvl1_cache, 12);
		}else{
			refl1_cache[14] = -1;
		}
		if ( macroblock -> MbPartPredMode [1] != Pred_L0 ) {
			ReadAndComputeCabac16x8MotionVector(c, state, &RefIdxL1[1], refl1_cache, mv_cache_l1, mvl1_cache, 28);		
		}else{
			refl1_cache[30] = -1;
		}
	}
	return 0;
}




