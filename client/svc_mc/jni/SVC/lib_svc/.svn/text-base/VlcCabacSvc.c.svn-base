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

//AVC Files
#include "type.h"
#include "main_data.h"
#include "vlc_cabac.h"
#include "CabacMbPred.h"
#include "CabacMvAndRef.h"

//SVC Files
#include "VlcCabacSvc.h"




/**
This function is used to decode the sub macroblock information in a P slice.
*/
char SubMbPCabacSVC ( int *dct_allowed, CABACContext *c, unsigned char *state, SLICE *slice, DATA *macroblock, 
					 RESIDU *CurrentResidu, short mv_cache_l0[][2], short *ref_cache_l0)
{	

	int mbPartIdx ;
	int subMbPartIdx ;
	int compIdx ;
	int mva;
	int mvb;


    //Recovery of the sub-macroblock type
	for ( mbPartIdx = 0 ; mbPartIdx < 4 ; mbPartIdx++ ) {
		CurrentResidu -> SubMbType [mbPartIdx] = decode_cabac_p_mb_sub_type( c, state);
	}

#ifdef ERROR_DETECTION
	//Error detection 
	if(ErrorsCheckSubMbType( CurrentResidu -> SubMbType, MaxPSubMbType)){
		//should be set to zero because the macroblock will be decoded.
		return 1;
	}
#endif

	if(GetCabacSVCPMotionPred(c, state, slice, macroblock, macroblock -> MotionPredL0, CurrentResidu -> InCropWindow, Pred_L1, 0)){
		return 1;
	}
		
	//Recovery of the image reference of each sub-macroblock
	if (slice -> num_RefIdxL0_active_minus1 > 0) {
		if(GetCabacPSubRefIdx(c, state, macroblock -> RefIdxL0, ref_cache_l0, macroblock -> MotionPredL0,
			slice -> num_RefIdxL0_active_minus1)){
				return 1;
		}
	}else {
		ref_cache_l0[12] = ref_cache_l0[13] = ref_cache_l0[14] = ref_cache_l0[15] = 
		ref_cache_l0[20] = ref_cache_l0[21] = ref_cache_l0[22] = ref_cache_l0[23] = 
		ref_cache_l0[28] = ref_cache_l0[29] = ref_cache_l0[30] = ref_cache_l0[21] = 
		ref_cache_l0[36] = ref_cache_l0[37] = ref_cache_l0[38] = ref_cache_l0[39] = 0;
	}


	//Recovery of the motion vector of each sub-macroblock
	for ( mbPartIdx = 0 ; mbPartIdx < 4 ; mbPartIdx++ ) {
		short MvdL0[2];
		int sub_mb_type = CurrentResidu -> SubMbType [mbPartIdx];
		for ( subMbPartIdx = 0 ; subMbPartIdx < sub_num_p_part[ sub_mb_type] ; subMbPartIdx++ ) {
			int index = SCAN8(( mbPartIdx << 2) + (sub_mb_type == 1 ? 2 * subMbPartIdx: subMbPartIdx));
			for ( compIdx = 0 ; compIdx < 2 ; compIdx++ ) {
				get_mv_neighbourhood(&mva, &mvb, mv_cache_l0, sub_mb_type, mbPartIdx , subMbPartIdx , compIdx, slice -> slice_type);
				macroblock -> MvdL0 [(mbPartIdx << 2) + subMbPartIdx][compIdx] = MvdL0[compIdx] = decode_cabac_mb_mvd( c, state, mva, mvb, compIdx);
			}
			FillSubMotionVector(MvdL0, &mv_cache_l0[index], sub_mb_type, slice -> slice_type);
		}
	}

	for ( mbPartIdx = 0;  mbPartIdx < 4 && *dct_allowed; mbPartIdx ++){
		if ( !(sub_num_p_part[CurrentResidu -> SubMbType[mbPartIdx]] == 1) ){
			*dct_allowed = 0;
		}
	}
	return 0;
}



/**
This function is used to decode the macroblock information in a P slice.
*/
char MbPredPCabacSvc (CABACContext *c, unsigned char *state, SLICE *slice, RESIDU *CurrResidu, DATA *macroblock, 
					 short mv_cache_l0[][2], short *ref_cache_l0){

	int index = 14 * CurrResidu -> Mode;


	//Get MotionPred flag
	if(GetCabacSVCPMotionPred(c, state, slice, macroblock, macroblock -> MotionPredL0, CurrResidu -> InCropWindow, Pred_L1, 0)){
		//Error detected
		ref_cache_l0 [12] = ref_cache_l0 [14] = ref_cache_l0 [28] = ref_cache_l0 [30] = 0;
		return 1;
	}

	//Get reference index
	if(GetCabacPRefList(c, state, macroblock -> RefIdxL0, ref_cache_l0, macroblock -> MotionPredL0, 
		CurrResidu -> Mode, slice -> num_RefIdxL0_active_minus1, index)){
		return 1;
	}
	
	//Read motion vector according to the macroblock type.
	if (CurrResidu -> Mode == 3){
		ReadCabacMotionVector(c, state, mv_cache_l0, macroblock -> MvdL0[0], 3, 12);
	}else if (CurrResidu -> Mode == 1){
		// mode == 1: 8x16
		ReadCabacMotionVector(c, state, mv_cache_l0, macroblock -> MvdL0[0], 1, 12);
		ReadCabacMotionVector(c, state, mv_cache_l0, macroblock -> MvdL0[4], 1, 14);
	}else{
		ReadCabacMotionVector(c, state, mv_cache_l0, macroblock -> MvdL0[0], 2, 12);
		ReadCabacMotionVector(c, state, mv_cache_l0, macroblock -> MvdL0[4], 2, 28);
	}
	return 0;
}


/**
This function decode the motion vector of a macroblock with 8x8 partition
*/
void ReadCabac8x8MotionVector(CABACContext *c, unsigned char *state, RESIDU *CurrentResidu, SLICE *slice, 
							  short mv_cache_lx[][2], short MvdLx[][2], int Pred_Lx)
{

	int mbPartIdx, subMbPartIdx;

	for ( mbPartIdx = 0 ; mbPartIdx < 4 ; mbPartIdx++ ) {
		int SubMbType = CurrentResidu -> SubMbType [mbPartIdx];
		if ( (sub_mb_b_name [SubMbType] != B_direct) && (sub_mb_b_name [SubMbType] != Pred_Lx )) {
			for ( subMbPartIdx = 0 ; subMbPartIdx < sub_num_b_part[SubMbType] ; subMbPartIdx++ ) {
				int xpart = (SubMbType < 5 || SubMbType == 6 || SubMbType == 8) ? 2:1;
				int index = SCAN8(( mbPartIdx << 2) + subMbPartIdx * xpart);

				ReadCabacSubMotionVector(c, state, mv_cache_lx, &MvdLx [(mbPartIdx << 2) + subMbPartIdx][0], 
					mbPartIdx, subMbPartIdx, SubMbType, index, slice -> slice_type);
			}
		}
	}
}



/**
This function is used to decode the sub macroblock information in a B slice.
*/
char SubMbBCabacSVC ( int *dct_allowed, int Dir8x8infFlag, CABACContext *c, unsigned char *state, SLICE *slice, DATA *macroblock, 
					 RESIDU *CurrentResidu, short mv_cache_l0[][2], short *ref_cache_l0, short mv_cache_l1[][2], short *ref_cache_l1)
{	

	int mbPartIdx ;


    //Recovery of the sub-macroblock type
	for ( mbPartIdx = 0 ; mbPartIdx < 4 ; mbPartIdx++ ) {
		CurrentResidu -> SubMbType [mbPartIdx] = decode_cabac_b_mb_sub_type( c, state);
	}
#ifdef ERROR_DETECTION
	//Error detection 
	if(ErrorsCheckSubMbType( CurrentResidu -> SubMbType, MaxBSubMbType)){
		//should be set to zero because the macroblock will be decoded.
		return 1;
	}
#endif

	if(GetCabacSVCBSubMotionPred(c, state, slice, macroblock -> MotionPredL0, CurrentResidu -> SubMbType,  
		4,  CurrentResidu -> InCropWindow, 0, Pred_L1)){
			return 1;
	}
	if(GetCabacSVCBSubMotionPred(c, state, slice, macroblock -> MotionPredL1, CurrentResidu -> SubMbType,  
		4,  CurrentResidu -> InCropWindow, 1, Pred_L0)){
			return 1;
	}


	//L0 and L1 reference index
	if(GetCabacBSubRefList(c, state, macroblock -> RefIdxL0, ref_cache_l0, macroblock -> MotionPredL0, 
		CurrentResidu -> SubMbType, slice -> num_RefIdxL0_active_minus1, Pred_L1)){
		return 1;
	}
	if(GetCabacBSubRefList(c, state, macroblock -> RefIdxL1, ref_cache_l1, macroblock -> MotionPredL1, 
		CurrentResidu -> SubMbType, slice -> num_RefIdxL1_active_minus1, Pred_L0)){
		return 1;
	}

	//Recovery of the motion vector of each sub-macroblock
	ReadCabac8x8MotionVector(c, state, CurrentResidu, slice, mv_cache_l0, macroblock -> MvdL0, Pred_L1);
	ReadCabac8x8MotionVector(c, state, CurrentResidu, slice, mv_cache_l1, macroblock -> MvdL1, Pred_L0);

	for ( mbPartIdx = 0;  mbPartIdx < 4 && *dct_allowed; mbPartIdx ++){
		if ( !((sub_num_b_part[ CurrentResidu -> SubMbType[mbPartIdx]] == 1) ||  
			(Dir8x8infFlag && 	sub_mb_b_name[CurrentResidu -> SubMbType[mbPartIdx]] == B_direct))){
			*dct_allowed = 0;
		}
	}
	return 0;
}


/**
This function is used to decode the macroblock information in a B slice.
*/
char MbPredBCabacSvc ( CABACContext *c, unsigned char *state, SLICE *Slice, RESIDU *CurrResidu, DATA *macroblock, 
					  short mv_cache_l0[][2], short *ref_cache_l0 , short mv_cache_l1[][2], short *ref_cache_l1)
{

	int index = 14 * CurrResidu -> Mode;

	//Get MotionPred flag
	if(GetCabacSVCPMotionPred(c, state, Slice, macroblock, macroblock -> MotionPredL0, CurrResidu -> InCropWindow, Pred_L1, 0)){
		//Error detected
		return 1;
	}
	
	//Get MotionPred flag
	if(GetCabacSVCPMotionPred(c, state, Slice, macroblock, macroblock -> MotionPredL1, CurrResidu -> InCropWindow, Pred_L0, 1)){
		//Error detected
		return 1;
	}
	

	if(GetCabacBRefList(c, state, macroblock -> RefIdxL0, ref_cache_l0, macroblock -> MotionPredL0, CurrResidu -> Mode, 
		Slice -> num_RefIdxL0_active_minus1, macroblock -> MbPartPredMode, Pred_L1, index)){
			return 1;
	}

	if(GetCabacBRefList(c, state, macroblock -> RefIdxL1, ref_cache_l1, macroblock -> MotionPredL1, CurrResidu -> Mode, 
		Slice -> num_RefIdxL1_active_minus1, macroblock -> MbPartPredMode, Pred_L0, index)){
			return 1;
	}

	if (CurrResidu -> Mode == 3){
		// Pred_L0 prediction
		if ( macroblock -> MbPartPredMode [0] != Pred_L1 ) {
			ReadCabacMotionVector(c, state, mv_cache_l0, &macroblock -> MvdL0[0][0], 3, 12);
		}

		// Pred_L1 prediction
		if ( macroblock -> MbPartPredMode [0] != Pred_L0 ) {
			ReadCabacMotionVector(c, state, mv_cache_l1, &macroblock -> MvdL1[0][0], 3, 12);
		}
	}else if (CurrResidu -> Mode == 1){
		//Recovery of of the motion vector for the frame P
		// mode == 1: 8x16
	
		//L0 prediction
		if ( macroblock -> MbPartPredMode [0] != Pred_L1 ) {
			ReadCabacMotionVector(c, state, mv_cache_l0, &macroblock -> MvdL0[0][0], 1, 12);
		}	

		if ( macroblock -> MbPartPredMode [1] != Pred_L1 ) {
			ReadCabacMotionVector(c, state, mv_cache_l0, &macroblock -> MvdL0[4][0], 1, 14);
		}


		//L1 prediction
		if ( macroblock -> MbPartPredMode [0] != Pred_L0 ) {
			ReadCabacMotionVector(c, state, mv_cache_l1, &macroblock -> MvdL1[0][0], 1, 12);
		}

		if ( macroblock -> MbPartPredMode [1] != Pred_L0 ) {
			ReadCabacMotionVector(c, state, mv_cache_l1, &macroblock -> MvdL1[4][0], 1, 14);
		}
	}else{
		//L0 prediction
		if ( macroblock -> MbPartPredMode [0] != Pred_L1 ) {
			ReadCabacMotionVector(c, state, mv_cache_l0, &macroblock -> MvdL0[0][0], 2, 12);
		}

		if ( macroblock -> MbPartPredMode [1] != Pred_L1 ) {
			ReadCabacMotionVector(c, state, mv_cache_l0, &macroblock -> MvdL0[4][0], 2, 28);		
		}

		//L1 prediction
		if ( macroblock -> MbPartPredMode [0] != Pred_L0 ) {
			ReadCabacMotionVector(c, state, mv_cache_l1, &macroblock -> MvdL1[0][0], 2, 12);
		}
		if ( macroblock -> MbPartPredMode [1] != Pred_L0 ) {
			ReadCabacMotionVector(c, state, mv_cache_l1, &macroblock -> MvdL1[4][0], 2, 28);		
		}
	}

	return 0;
}
