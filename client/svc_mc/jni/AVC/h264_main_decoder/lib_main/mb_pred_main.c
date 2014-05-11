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

#include "string.h"

//AVC Files
#include "data.h"
#include "refill.h"
#include "mb_pred.h"
#include "bitstream.h"
#include "mb_pred_main.h"
#include "motion_vector.h"
#include "Direct_prediction.h"





const int sub_mb_b_name[13]={B_direct, Pred_L0, Pred_L1, B_Bi, Pred_L0, Pred_L0, Pred_L1, Pred_L1, B_Bi, B_Bi, Pred_L0, Pred_L1, B_Bi};



/**
Decodes motion vector for B macroblocks.
*/
void GetCavlcBMotionVector(const unsigned char * RESTRICT Data, int *Position, int sub_mb_type, 
						   int mbPartIdx, char *RefIdxLx, short *RefCacheLx, short MvCacheLx[][2])
{

	short mvL0[2];
	int subMbPartIdx;


	for ( subMbPartIdx = 0 ; subMbPartIdx < sub_num_b_part[ sub_mb_type] ; subMbPartIdx++ ) 	{
		int xpart = (sub_mb_type < 5 || sub_mb_type == 6 || sub_mb_type == 8) ? 2: 1;
		int ypart = (sub_mb_type < 4 || sub_mb_type == 5 || sub_mb_type == 7  || sub_mb_type == 9) ? 2: 1;
		int index = SCAN8(( mbPartIdx << 2) + subMbPartIdx * xpart);

		refill_ref(xpart, ypart, RefIdxLx[mbPartIdx], &RefCacheLx[index]);  
		mvL0[0] = (short ) read_se(Data, Position);
		mvL0[1] = (short ) read_se(Data, Position);
		motion_vector(MvCacheLx, mvL0, RefCacheLx, &MvCacheLx[index][0], RefIdxLx[mbPartIdx], index, xpart);
	}

	refill_motion_vector(1, sub_mb_type, &MvCacheLx[SCAN8(( mbPartIdx << 2))]);
}




/**
This function permits to decode the image reference of each sub-macroblock. 

@param data The NAL unit.
@param aio_piPosition The current aio_piPosition in the NAL.
@param Slice The Slice structure.
@param block Contains all parameters of the current macroblock.

*/
char sub_mb_pred_B ( int *dct_allowed, const unsigned char *RESTRICT Data, int *Position, 
				   const SLICE *Slice, RESIDU *Current_residu, short mv_cache_l0[][2], 
				   short *ref_cache_l0, short mv_cache_l1[][2], short *ref_cache_l1, 
				   short *mvl1_l0, short *mvl1_l1, short *refl1_l0, short *refl1_l1, 
				   int direct_8x8_inference_flag, int long_term, int slice_type, int is_svc)
{	



	int mbPartIdx ;
	char RefIdxL0[4];
	char RefIdxL1[4];


	//Recovery of the sub-macroblock type
	GetCavlcSubMbType(Data, Position, Current_residu);

#ifdef ERROR_DETECTION
	//Error detection
	if(ErrorsCheckSubMbType( Current_residu -> SubMbType, MaxBSubMbType)){
		//should be set to zero because the macroblock will be decoded.
		ref_cache_l0 [12] = ref_cache_l0 [14] = ref_cache_l0 [28] = ref_cache_l0 [30] = 0;
		ref_cache_l1 [12] = ref_cache_l1 [14] = ref_cache_l1 [28] = ref_cache_l1 [30] = 0;
		return 1;
	}
#endif

	//Recovery of the image reference of each sub-macroblock
	if ( Slice -> num_RefIdxL0_active_minus1 > 0){
		if(GetCavlcBRefLx(Data, Position, Current_residu -> SubMbType, RefIdxL0, Slice -> num_RefIdxL0_active_minus1, Pred_L1)){
			ref_cache_l0 [12] = ref_cache_l0 [14] = ref_cache_l0 [28] = ref_cache_l0 [30] = 0;
			ref_cache_l1 [12] = ref_cache_l1 [14] = ref_cache_l1 [28] = ref_cache_l1 [30] = 0;
			return 1;
		}
	} else {
		RefIdxL0 [0] = RefIdxL0 [1] = RefIdxL0 [2] = RefIdxL0 [3] = 0;
	}


	if ( Slice -> num_RefIdxL1_active_minus1 > 0){
		if(GetCavlcBRefLx(Data, Position, Current_residu -> SubMbType, RefIdxL1, Slice -> num_RefIdxL1_active_minus1, Pred_L0)){
			ref_cache_l1 [12] = ref_cache_l1 [14] = ref_cache_l1 [28] = ref_cache_l1 [30] = 0;
			return 1;
		}
	} else {
		RefIdxL1 [0] = RefIdxL1 [1] = RefIdxL1 [2] = RefIdxL1 [3] = 0;
	}


	//Recovery of the motion vector of each sub-macroblock
	for ( mbPartIdx = 0 ; mbPartIdx < 4 ; mbPartIdx++ )  {
		char sub_mb_type = Current_residu -> SubMbType[mbPartIdx];
		if ( sub_mb_b_name[sub_mb_type] == B_direct ){
			if (Slice -> direct_spatial_mv_pred_flag){
				spatial_direct_prediction_l0(mv_cache_l0, mvl1_l0, mvl1_l1, ref_cache_l0, ref_cache_l1, 
					refl1_l0, refl1_l1, mbPartIdx, Slice -> b_stride, Slice -> b8_stride, 
					direct_8x8_inference_flag, long_term, is_svc, slice_type);
			}else{
				temporal_direct_prediction_l0(mv_cache_l0, mvl1_l0, ref_cache_l0, refl1_l0, refl1_l1, Slice, 
					mbPartIdx, direct_8x8_inference_flag, is_svc, slice_type);
			}
		}else if( sub_mb_b_name[sub_mb_type] != Pred_L1 )   {
			GetCavlcBMotionVector(Data, Position, sub_mb_type, mbPartIdx, RefIdxL0, ref_cache_l0, mv_cache_l0);
		}else{
			int index = SCAN8(( mbPartIdx << 2));
			ref_cache_l0[index] = -1;
		}
	}

	//Recovery of the motion vector of each sub-macroblock
	for ( mbPartIdx = 0 ; mbPartIdx < 4 ; mbPartIdx++ )     {
		char sub_mb_type = Current_residu -> SubMbType[mbPartIdx];
		if ( sub_mb_b_name[sub_mb_type] == B_direct ){
			if (Slice -> direct_spatial_mv_pred_flag){
				spatial_direct_prediction_l1(mv_cache_l1, mvl1_l0, mvl1_l1, ref_cache_l0, ref_cache_l1,
					refl1_l0, refl1_l1, mbPartIdx, Slice -> b_stride, Slice -> b8_stride, 
					direct_8x8_inference_flag, long_term, is_svc, slice_type);
			}else{
				temporal_direct_prediction_l1(mv_cache_l1, mvl1_l0, ref_cache_l1, refl1_l0, refl1_l1, Slice, 
					mbPartIdx, direct_8x8_inference_flag, is_svc, slice_type);
			}
		}
		else if( sub_mb_b_name[sub_mb_type] != Pred_L0 ){
			GetCavlcBMotionVector(Data, Position, sub_mb_type, mbPartIdx, RefIdxL1, ref_cache_l1, mv_cache_l1);
		}else{
			int index = SCAN8(( mbPartIdx << 2));
			ref_cache_l1[index] = -1;
		}
	}

	for ( mbPartIdx = 0;  mbPartIdx < 4 && *dct_allowed; mbPartIdx ++){
		if ( !((sub_num_b_part[ Current_residu -> SubMbType[mbPartIdx]] == 1) ||  
			( direct_8x8_inference_flag && sub_mb_b_name[Current_residu -> SubMbType[mbPartIdx]] == B_direct))){
				*dct_allowed = 0;
		}
	}
	return 0;
}





/**
To calculate the motion vector when the macroblock type is B_Skip
*/
void motion_vector_B_skip(short aio_tmv_cache_l0 [][2], short aio_tmv_cache_l1 [][2], short *aio_tref_cache_l0, 
						  short *aio_tref_cache_l1, short *aio_tMv_l0, short *aio_tMv_l1, short *ref_l1_l0, 
						  short *ref_l1_l1, const SLICE *slice, int direct_8x8_inference_flag, int long_term, int slice_type, int is_svc)
{



	int mbPartIdx;

	if (slice -> direct_spatial_mv_pred_flag){
		for ( mbPartIdx = 0 ; mbPartIdx < 4 ; mbPartIdx++ ) {
			spatial_direct_prediction(aio_tmv_cache_l0, aio_tmv_cache_l1, aio_tMv_l0, aio_tMv_l1, 
				aio_tref_cache_l0, aio_tref_cache_l1, ref_l1_l0, ref_l1_l1, mbPartIdx, 
				slice -> b_stride, slice -> b8_stride , direct_8x8_inference_flag, long_term, is_svc, slice_type );
		}
	}else{
		for ( mbPartIdx = 0 ; mbPartIdx < 4 ; mbPartIdx++ ) {
			temporal_direct_prediction(aio_tmv_cache_l0, aio_tmv_cache_l1, aio_tMv_l0, 
				aio_tref_cache_l0, aio_tref_cache_l1, ref_l1_l0, ref_l1_l1, slice, 
				mbPartIdx, direct_8x8_inference_flag, is_svc, slice_type);
		}
	}
}



char GetCavlcBRefList(const unsigned char *ai_pcData, int *aio_piPosition, char *refLx,
					 int Mode, unsigned char NumRefLx, unsigned char *MbPartMode, short Pred_Lx)
{

	//Read one reference list and a second one for 16x8 or 8x16 block
	if ( NumRefLx > 0) {
		if(MbPartMode [0] != Pred_Lx){
			refLx [0] = (char ) read_te(ai_pcData, aio_piPosition, NumRefLx);
		}else {
			refLx [0] = -1; //mbl check
		}
		if (Mode != 3 && MbPartMode [1] != Pred_Lx){
			refLx [1] = (char ) read_te(ai_pcData, aio_piPosition, NumRefLx);
		}else {
			refLx [1] = -1;
		}
#ifdef ERROR_DETECTION
		//Error detection
		if(ErrorsCheckRefLx(refLx, NumRefLx)){
			return 1;
		}
#endif
	}
	return 0;
}





/**
This function permits to decode the prediction mode or the image reference. 

@param ai_pcData The NAL unit.
@param aio_piPosition The current aio_piPosition in the NAL.
@param Slice The Slice structure.
@param aio_pstMacroblock Contains all parameters of the current macroblock.
@param ao_pstIntra_pred_mode Contains the prediction mode for the current macroblock.
@param intra4x4_pred_mode_cache Table containing the mode of the 4x4 neighbourhood.
@param vlc The VLC tables in order to decode the Nal Unit.
*/
char mb_pred_B ( const unsigned char *ai_pcData, int *aio_piPosition, const SLICE *Slice, DATA *aio_pstMacroblock, 
			   RESIDU *CurrResidu, short mv_cache_l0[][2], short mv_cache_l1[][2], short *ref_cache_l0, 
			   short *ref_cache_l1)
{



	char RefIdxL0[2] = {0, 0};
	char RefIdxL1[2] = {0, 0};

	if(GetCavlcBRefList(ai_pcData, aio_piPosition, RefIdxL0, CurrResidu -> Mode, 
		Slice -> num_RefIdxL0_active_minus1, aio_pstMacroblock -> MbPartPredMode, Pred_L1)){
			ref_cache_l0 [12] = ref_cache_l0 [14] = ref_cache_l0 [28] = ref_cache_l0 [30] = 0;
			ref_cache_l1 [12] = ref_cache_l1 [14] = ref_cache_l1 [28] = ref_cache_l1 [30] = 0;
			return 1;
	}


	if(GetCavlcBRefList(ai_pcData, aio_piPosition, RefIdxL1, CurrResidu -> Mode, 
		Slice -> num_RefIdxL1_active_minus1, aio_pstMacroblock -> MbPartPredMode, Pred_L0)){
			ref_cache_l1 [12] = ref_cache_l1 [14] = ref_cache_l1 [28] = ref_cache_l1 [30] = 0;
			return 1;
	}


	if (CurrResidu -> Mode == 3){
		if ( aio_pstMacroblock -> MbPartPredMode [0] != Pred_L1 )  {
			ReadCavlc16x16MotionVector(ai_pcData, aio_piPosition, RefIdxL0, ref_cache_l0, mv_cache_l0);
		}else{
			//To remove the -2
			ref_cache_l0[14] = ref_cache_l0[30] = -1;
		}


		//Recovery of of the motion vector for the frame B
		if ( aio_pstMacroblock -> MbPartPredMode [0] != Pred_L0 ) {
			ReadCavlc16x16MotionVector(ai_pcData, aio_piPosition, RefIdxL1, ref_cache_l1, mv_cache_l1);
		}else{
			ref_cache_l1[14] = ref_cache_l1[30] = -1;
		}

	}else {
		//Recovery of of the motion vector for the frame P
		if (CurrResidu -> Mode == 1){
			// mode == 1: 8x16

			if ( aio_pstMacroblock -> MbPartPredMode [0] != Pred_L1 )  {
				ReadCavlc8x16MotionVector(ai_pcData, aio_piPosition, RefIdxL0, ref_cache_l0, mv_cache_l0, 12);
			}

			if ( aio_pstMacroblock -> MbPartPredMode [1] != Pred_L1 )  {
				ReadCavlc8x16MotionVector(ai_pcData, aio_piPosition, &RefIdxL0[1], ref_cache_l0, mv_cache_l0, 14);
			}else{
				ref_cache_l0[14] = ref_cache_l0[30] = -1;
			}

			//Recovery of of the motion vector for the frame B
			if ( aio_pstMacroblock -> MbPartPredMode [0] != Pred_L0 ) {
				ReadCavlc8x16MotionVector(ai_pcData, aio_piPosition, RefIdxL1, ref_cache_l1, mv_cache_l1, 12);
			}

			if ( aio_pstMacroblock -> MbPartPredMode [1] != Pred_L0 ) {
				ReadCavlc8x16MotionVector(ai_pcData, aio_piPosition, &RefIdxL1[1], ref_cache_l1, mv_cache_l1, 14);
			}
			else{
				ref_cache_l1[14] = ref_cache_l1[30] = -1;
			}
		}else{
			// mode == 2; 16x8

			if ( aio_pstMacroblock -> MbPartPredMode [0] != Pred_L1 )  {
				ReadCavlc16x8MotionVector(ai_pcData, aio_piPosition, RefIdxL0, ref_cache_l0, mv_cache_l0, 12);
			}else{
				ref_cache_l0[14] = -1;
			}

			if ( aio_pstMacroblock -> MbPartPredMode [1] != Pred_L1 )  {
				ReadCavlc16x8MotionVector(ai_pcData, aio_piPosition, &RefIdxL0[1], ref_cache_l0, mv_cache_l0, 28);
			}else{
				ref_cache_l0[30] = -1;
			}


			//Recovery of of the motion vector for the frame B
			if ( aio_pstMacroblock -> MbPartPredMode [0] != Pred_L0 ) {
				ReadCavlc16x8MotionVector(ai_pcData, aio_piPosition, RefIdxL1, ref_cache_l1, mv_cache_l1, 12);
			}else{
				ref_cache_l1[14] = -1;
			}

			if ( aio_pstMacroblock -> MbPartPredMode [1] != Pred_L0 ) {
				ReadCavlc16x8MotionVector(ai_pcData, aio_piPosition, &RefIdxL1[1], ref_cache_l1, mv_cache_l1, 28);
			}else{
				ref_cache_l1[30] = -1;
			}
		}
	}
	return 0;
}





