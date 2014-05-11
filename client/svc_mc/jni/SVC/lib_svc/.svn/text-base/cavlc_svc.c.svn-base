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
// AVC
#include "type.h"
#include "data.h"
#include "cavlc.h"
#include "mb_pred.h"
#include "ErrorsDetection.h"
#include "macroblock_partitionning.h"


// SVC
#include "svc_data.h"
#include "cavlc_svc.h"
#include "Coordonates.h"



static const int sub_mb_type_name[2][13]={{Pred_L0 , Pred_L0, Pred_L0, Pred_L0, Pred_L0, NA, NA, NA, NA, NA, NA, NA, NA}, {B_direct, Pred_L0, Pred_L1, B_Bi   , Pred_L0, Pred_L0, Pred_L1, Pred_L1, B_Bi, B_Bi, Pred_L0, Pred_L1, B_Bi}};
static const int sub_num_part[2][13]={{1, 2, 2, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0},	  {4, 1, 1, 1, 2, 2, 2, 2, 2, 2, 4, 4, 4}};



/**
This function permits to decode the image reference of each sub-macroblock. 

@param data The NAL unit.
@param position The current position in the NAL.
@param block Contains all parameters of the current macroblock.
@param slice The slice structure.
*/
int sub_mb_pred_svc ( const unsigned char *ai_pcData, int *position, const SLICE *ai_pstSlice, 
					 DATA *aio_pstMacroblock, RESIDU *Current_residu)
{	
	int mbPartIdx ;
	int subMbPartIdx ;


	//Recovery of the sub-macroblock type
	GetCavlcSubMbType(ai_pcData, position, Current_residu);

#ifdef ERROR_DETECTION
	//Error detection 
	if(ErrorsCheckSubMbType( Current_residu -> SubMbType, ai_pstSlice -> slice_type? MaxBSubMbType:MaxPSubMbType)){
		return 1;
	}
#endif

	//Recovery of the image reference of each sub-macroblock
	if( ai_pstSlice -> AdaptiveMotionPredictionFlag && Current_residu -> InCropWindow ) {
		for ( mbPartIdx = 0 ; mbPartIdx < 4 ; mbPartIdx++ ) {
			if (sub_mb_type_name[ai_pstSlice -> slice_type][Current_residu -> SubMbType [mbPartIdx]] != Pred_L1 &&
				sub_mb_type_name[ai_pstSlice -> slice_type][Current_residu -> SubMbType [mbPartIdx]] != B_direct) {
					aio_pstMacroblock -> MotionPredL0 [mbPartIdx] = getNbits(ai_pcData, position, 1);
			}
		}

		//Recovery of the image reference of each sub-macroblock
		for ( mbPartIdx = 0 ; mbPartIdx < 4 ; mbPartIdx++ ) {
			if ( sub_mb_type_name[ai_pstSlice -> slice_type][Current_residu -> SubMbType [mbPartIdx]] != Pred_L0 &&
				sub_mb_type_name[ai_pstSlice -> slice_type][Current_residu -> SubMbType [mbPartIdx]] != B_direct) {
					aio_pstMacroblock -> MotionPredL1 [mbPartIdx] = getNbits(ai_pcData, position, 1);
			}
		}
	}else{
		aio_pstMacroblock -> MotionPredL0[0] = aio_pstMacroblock -> MotionPredL0[1] =
			aio_pstMacroblock -> MotionPredL0[2] = aio_pstMacroblock -> MotionPredL0[3] = 
			aio_pstMacroblock -> MotionPredL1[0] = aio_pstMacroblock -> MotionPredL1[1] = 
			aio_pstMacroblock -> MotionPredL1[2] = aio_pstMacroblock -> MotionPredL1[3] = ai_pstSlice -> DefaultMotionPredictionFlag;
	}


	if  (ai_pstSlice -> num_RefIdxL0_active_minus1 != 0){
		for ( mbPartIdx = 0 ; mbPartIdx < 4 ; mbPartIdx++ ) {
			if ((Current_residu -> MbType != P_8x8ref0) &&	(!aio_pstMacroblock -> MotionPredL0[mbPartIdx]) &&
				(sub_mb_type_name[ai_pstSlice -> slice_type][Current_residu -> SubMbType [mbPartIdx]] != B_direct) &&
				(sub_mb_type_name[ai_pstSlice -> slice_type][Current_residu -> SubMbType [mbPartIdx]] != Pred_L1)) 		{
					aio_pstMacroblock -> RefIdxL0 [mbPartIdx] =  read_te(ai_pcData, position, ai_pstSlice -> num_RefIdxL0_active_minus1);
			}
		}
	}
#ifdef ERROR_DETECTION
	//Error Detection
	if(ErrorsCheckSubRefLx(aio_pstMacroblock -> RefIdxL0, ai_pstSlice -> num_RefIdxL0_active_minus1)){
		return 1;
	}
#endif

	//Recovery of the image reference of each sub-macroblock
	if (ai_pstSlice -> num_RefIdxL1_active_minus1 != 0){
		for ( mbPartIdx = 0 ; mbPartIdx < 4 ; mbPartIdx++ ) 	{
			if ((sub_mb_type_name[ai_pstSlice -> slice_type][Current_residu -> SubMbType [mbPartIdx]] != B_direct) &&
				(sub_mb_type_name[ai_pstSlice -> slice_type][Current_residu -> SubMbType [mbPartIdx]] != Pred_L0) &&
				(!aio_pstMacroblock -> MotionPredL1[mbPartIdx])) {
					aio_pstMacroblock -> RefIdxL1 [mbPartIdx] = read_te(ai_pcData, position, ai_pstSlice -> num_RefIdxL1_active_minus1);
			}
		}
	}

#ifdef ERROR_DETECTION
	//Error Detection
	if(ErrorsCheckSubRefLx(aio_pstMacroblock -> RefIdxL1, ai_pstSlice -> num_RefIdxL1_active_minus1)){
		return 1;
	}
#endif

	//Recovery of the motion vector of each sub-macroblock
	for ( mbPartIdx = 0 ; mbPartIdx < 4 ; mbPartIdx++ )  {
		if ( sub_mb_type_name[ai_pstSlice -> slice_type][Current_residu -> SubMbType [mbPartIdx]] != B_direct 
			&& sub_mb_type_name[ai_pstSlice -> slice_type][Current_residu -> SubMbType [mbPartIdx]] != Pred_L1 ) 
		{
			for ( subMbPartIdx = 0 ; subMbPartIdx < sub_num_part[ ai_pstSlice -> slice_type][ Current_residu -> SubMbType [mbPartIdx]] ; subMbPartIdx++ ) {
				int index = ( mbPartIdx << 2) + subMbPartIdx;
				aio_pstMacroblock ->  MvdL0 [index][0] = read_se(ai_pcData, position);
				aio_pstMacroblock ->  MvdL0 [index][1] = read_se(ai_pcData, position);
			}
		}
	}

	//Recovery of the motion vector of each sub-macroblock
	for ( mbPartIdx = 0 ; mbPartIdx < 4 ; mbPartIdx++ )   {
		if ( sub_mb_type_name[ai_pstSlice -> slice_type][Current_residu -> SubMbType [mbPartIdx]] != B_direct 
			&& sub_mb_type_name[ai_pstSlice -> slice_type][Current_residu -> SubMbType [mbPartIdx]] != Pred_L0 ) 
		{
			for ( subMbPartIdx = 0 ; subMbPartIdx < sub_num_part[ ai_pstSlice -> slice_type][ Current_residu -> SubMbType [mbPartIdx]] ; subMbPartIdx++ )
			{
				int index = ( mbPartIdx << 2) + subMbPartIdx;
				aio_pstMacroblock ->  MvdL1 [index][0] = read_se(ai_pcData, position);
				aio_pstMacroblock ->  MvdL1 [index][1] = read_se(ai_pcData, position);
			}
		}
	}
	return 0;
}







/**
This function permits to decode the prediction mode or the image reference. 

@param ai_pcData The NAL unit.
@param position The current position in the NAL.
@param block Contains all parameters of the current macroblock.
@param ai_pstSlice The slice structure.
@param ao_pstIntra_pred_mode Contains the prediction mode for the current macroblock.

*/
int mb_pred_svc( const unsigned char *ai_pcData, int *position, const SLICE *ai_pstSlice, 
				DATA *aio_pstMacroblock, RESIDU *Current_residu, short *intra4x4_pred_mode_cache)
{



	int mbPartIdx ;



	//Recovery of the prediction mode
	if(IS_I(aio_pstMacroblock -> MbPartPredMode [0])){
		if(mb_pred_I(ai_pcData, position, Current_residu, Current_residu -> Intra16x16DCLevel, intra4x4_pred_mode_cache)){
			return 1;
		}
	}
	else if ( aio_pstMacroblock -> MbPartPredMode [0] != B_direct ) {
		if( ai_pstSlice -> AdaptiveMotionPredictionFlag && Current_residu -> InCropWindow ) {
			if(ReadMotionPredSVC(ai_pcData, position, aio_pstMacroblock -> MotionPredL0, aio_pstMacroblock, Pred_L1)) return 1;
			if(ReadMotionPredSVC(ai_pcData, position, aio_pstMacroblock -> MotionPredL1, aio_pstMacroblock, Pred_L0)) return 1;
		}else{
			aio_pstMacroblock -> MotionPredL0[0] = aio_pstMacroblock -> MotionPredL0[1] = 
				aio_pstMacroblock -> MotionPredL0[2] = aio_pstMacroblock -> MotionPredL0[3] = 
				aio_pstMacroblock -> MotionPredL1[0] = aio_pstMacroblock -> MotionPredL1[1] = 
				aio_pstMacroblock -> MotionPredL1[2] = aio_pstMacroblock -> MotionPredL1[3] = ai_pstSlice -> DefaultMotionPredictionFlag;
		}



		if ( ai_pstSlice -> num_RefIdxL0_active_minus1 != 0 ){
			for ( mbPartIdx = 0 ; mbPartIdx < aio_pstMacroblock -> NumMbPart ; mbPartIdx++ ) {
				if ( (aio_pstMacroblock -> MbPartPredMode [mbPartIdx] != Pred_L1 ) && (!aio_pstMacroblock -> MotionPredL0[mbPartIdx])) {
					aio_pstMacroblock -> RefIdxL0 [mbPartIdx] = read_te(ai_pcData, position, ai_pstSlice -> num_RefIdxL0_active_minus1);
				}
			}
		}

#ifdef ERROR_DETECTION
		//Error detection
		if(ErrorsCheckRefLx(aio_pstMacroblock -> RefIdxL0, ai_pstSlice -> num_RefIdxL0_active_minus1)){
			return 1;
		}
#endif

		//Recovery of of the image reference for the frame 
		if ( ai_pstSlice -> num_RefIdxL1_active_minus1 != 0 ){
			for ( mbPartIdx = 0 ; mbPartIdx < aio_pstMacroblock -> NumMbPart ; mbPartIdx++ )  {
				if ((aio_pstMacroblock -> MbPartPredMode [mbPartIdx] != Pred_L0) && (!aio_pstMacroblock -> MotionPredL1[mbPartIdx])) {
					aio_pstMacroblock -> RefIdxL1 [mbPartIdx] = read_te(ai_pcData, position, ai_pstSlice -> num_RefIdxL1_active_minus1);
				}
			}
		}

#ifdef ERROR_DETECTION
		//Error detection
		if(ErrorsCheckRefLx(aio_pstMacroblock -> RefIdxL1, ai_pstSlice -> num_RefIdxL1_active_minus1)){
			return 1;
		}
#endif

		//Recovery of of the motion vector for the frame P
		for ( mbPartIdx = 0 ; mbPartIdx < aio_pstMacroblock -> NumMbPart ; mbPartIdx++ ) {
			/* Pour eviter de traiter les Images B*/
			if ( aio_pstMacroblock -> MbPartPredMode [mbPartIdx] != Pred_L1 )	{
				aio_pstMacroblock -> MvdL0 [mbPartIdx << 2][0] = read_se(ai_pcData, position);
				aio_pstMacroblock -> MvdL0 [mbPartIdx << 2][1] = read_se(ai_pcData, position);
			}
		}

		//Recovery of of the motion vector for the frame B
		for ( mbPartIdx = 0 ; mbPartIdx < aio_pstMacroblock -> NumMbPart ; mbPartIdx++ ) {
			if ( aio_pstMacroblock -> MbPartPredMode [mbPartIdx] != Pred_L0 ) 	{
				aio_pstMacroblock -> MvdL1 [ mbPartIdx << 2][0] = read_se(ai_pcData, position);
				aio_pstMacroblock -> MvdL1 [ mbPartIdx << 2][1] = read_se(ai_pcData, position);
			}
		}
	}else{
		Current_residu -> Mode = 4;
		for (mbPartIdx = 0; mbPartIdx < 4; mbPartIdx++){
			Current_residu -> SubMbType [mbPartIdx] = 3;
		}
	}
	return 0;
}




/**
Decodes variable length using SVC prediction.
*/
char macroblock_layer_in_scalable_extension(const NAL *Nal, const PPS *Pps, RESIDU *Current_residu, RESIDU *BaseResidu, 
										   const unsigned char *ai_pcData, int *position, const SLICE *ai_pstSlice, 
										   DATA *Tab_Block, const VLC_TABLES *vlc, unsigned char *NonZeroCountCache, 
										   unsigned char *SliceTable, const short iMb_x, const short iMb_y, 
										   int direct_8x8_inference_flag, unsigned char *last_QP)

{  




	int  noSubMbPartSizeLessThan8x8;
	int BaseModeFlag = 0;
	const int iCurrMbAddr = iMb_x + iMb_y * (short)(ai_pstSlice -> mb_stride);
	short  intra4x4_pred_mode_cache[40];
	DATA * aio_pstBlock = &Tab_Block[iCurrMbAddr];

	//TODO
	if( ai_pstSlice -> AdaptiveBaseModeFlag && Current_residu -> InCropWindow)  {
		Current_residu -> BaseModeFlag = BaseModeFlag = getNbits(ai_pcData, position, 1); //u(1)   
	}else if (ai_pstSlice -> DefaultBaseModeFlag){
		Current_residu -> BaseModeFlag = BaseModeFlag = 1;
	}

	if(!BaseModeFlag){
		int mb_type = read_ue(ai_pcData, position);

		//According to the slice type and the macroblock type  
		//the parameters are adjusted
		switch ( ai_pstSlice -> slice_type ) 
		{
		case EI : 
#ifdef ERROR_DETECTION
			//Error detection
			if(ErrorsCheckMbType(mb_type, I_BL)){
				return 1;
			}
#endif
			aio_pstBlock -> MbPartPredMode [0] = i_mb_type_info[mb_type] . type;
			Current_residu -> MbType = i_mb_type_info[mb_type] . type;
			Current_residu -> Cbp = i_mb_type_info[mb_type] . Cbp;
			Current_residu -> Intra16x16PredMode = i_mb_type_info[mb_type] . pred_mode;
			break ;
		case EP :
			if (mb_type < 5){
#ifdef ERROR_DETECTION
				//Error detection
				if(ErrorsCheckMbType(mb_type, P_BL)){
					return 1;
				}
#endif
				aio_pstBlock -> NumMbPart = p_mb_type_info[mb_type] . partcount;
				aio_pstBlock -> MbPartPredMode [0] = p_mb_type_info[mb_type] . type_0;
				aio_pstBlock -> MbPartPredMode [1] = p_mb_type_info[mb_type] . type_1;
				Current_residu -> MbType = p_mb_type_info[mb_type] . name;
				Current_residu -> Mode = p_mb_type_info[mb_type] . Mode;
			} 
			else {
				mb_type -= 5;
#ifdef ERROR_DETECTION
				//Error detection
				if(ErrorsCheckMbType(mb_type, I_BL)){
					return 1;
				}
#endif
				aio_pstBlock -> MbPartPredMode [0] = i_mb_type_info[mb_type] . type;
				Current_residu -> MbType = i_mb_type_info[mb_type] . type;
				Current_residu -> Cbp = i_mb_type_info[mb_type] . Cbp;
				Current_residu -> Intra16x16PredMode = i_mb_type_info[mb_type] . pred_mode;		
			}
			break ;
		case EB :
			if (mb_type < 23)	{
#ifdef ERROR_DETECTION
				//Error detection
				if(ErrorsCheckMbType(mb_type, B_BL)){
					return 1;
				}
#endif
				aio_pstBlock -> NumMbPart = b_mb_type_info[mb_type] . partcount;
				aio_pstBlock -> MbPartPredMode [0] = b_mb_type_info[mb_type] . type_0;
				aio_pstBlock -> MbPartPredMode [1] = b_mb_type_info[mb_type] . type_1;
				Current_residu -> MbType = b_mb_type_info[mb_type] . name;
				Current_residu -> Mode = b_mb_type_info[mb_type] . Mode;
			}
			else	{
				mb_type -= 23;
#ifdef ERROR_DETECTION
			//Error detection
			if(ErrorsCheckMbType(mb_type, I_BL)){
				return 1;
			}
#endif
				aio_pstBlock -> MbPartPredMode [0] = i_mb_type_info[mb_type] . type;
				Current_residu -> MbType = i_mb_type_info[mb_type] . type;
				Current_residu -> Cbp = i_mb_type_info[mb_type] . Cbp;
				Current_residu -> Intra16x16PredMode = i_mb_type_info[mb_type] . pred_mode;		
			}
			break;
		}
		//Initialize base macroblock address
		GetBaseMbAddr(Nal, aio_pstBlock, iMb_x << 4, iMb_y << 4);
	}
	else{
		aio_pstBlock -> MbPartPredMode [0] = Current_residu -> MbType = GetBaseType(Nal, BaseResidu, aio_pstBlock, iMb_x, iMb_y);
	}


	//Updating the slice table in order to save in which slice each macroblock belong to
	Current_residu -> SliceNum = SliceTable [iCurrMbAddr] = ai_pstSlice -> slice_num ;


	if ( !BaseModeFlag && Current_residu -> MbType == INTRA_PCM )   {
		while ( !bytes_aligned(*position) ) {
			getNbits(ai_pcData, position, 1);//pcm_alignment_zero_bit = 
		}
		ParseIPCM(ai_pcData, position, Current_residu, NonZeroCountCache);
	} 
	else     {
		//Updating the parameter in order to decode the VLC
		fill_caches_svc(ai_pstSlice, Current_residu, BaseResidu, 0, NonZeroCountCache, SliceTable, 
			aio_pstBlock, intra4x4_pred_mode_cache, iMb_x, iMb_y, Pps -> constrained_intra_pred_flag, Nal -> TCoeffPrediction);

		if (!BaseModeFlag){
			//Recovery of the motion vectors for the sub_macroblock 
			if ( !IS_I(Current_residu -> MbType) && (aio_pstBlock -> NumMbPart == 4))    {
				int mbPartIdx;
				if(sub_mb_pred_svc(ai_pcData, position, ai_pstSlice, aio_pstBlock, Current_residu)){
					return 1;
				}
				noSubMbPartSizeLessThan8x8 = 0;
				if ( direct_8x8_inference_flag ){
					noSubMbPartSizeLessThan8x8 = 1;
				}
				for ( mbPartIdx = 0;  mbPartIdx < 4; mbPartIdx ++){
					if ( sub_mb_type_name[ai_pstSlice -> slice_type][Current_residu -> SubMbType[mbPartIdx]] != B_direct){
						if ( sub_num_part[ai_pstSlice -> slice_type][Current_residu -> SubMbType[mbPartIdx]] > 1 )
							noSubMbPartSizeLessThan8x8 = 0;
					}
				}
			}
			else
			{ 
				noSubMbPartSizeLessThan8x8 = 1;
				if ( Pps -> transform_8x8_mode_flag && Current_residu -> MbType == INTRA_4x4 && getNbits(ai_pcData, position, 1)){
					Current_residu -> MbType = Current_residu -> Transform8x8 = aio_pstBlock -> Transform8x8 = aio_pstBlock -> MbPartPredMode[0] = INTRA_8x8;
				}

				//Recovery of the prediction mode and the motion vectors for the macroblock 
				if(mb_pred_svc(ai_pcData, position, ai_pstSlice, aio_pstBlock, Current_residu, intra4x4_pred_mode_cache)){
					return 1;
				}
			}
		}

		//Decoding process of the VLC 
		if( ai_pstSlice -> AdaptiveResidualPredictionFlag && ai_pstSlice -> slice_type != EI && (BaseModeFlag || ( !IS_I( Current_residu -> MbType) && Current_residu -> InCropWindow ))){
			Current_residu -> ResidualPredictionFlag = getNbits(ai_pcData, position, 1);
		}else{
			Current_residu -> ResidualPredictionFlag = ai_pstSlice -> DefaultResidualPredictionFlag;
		}


		if ( BaseModeFlag || aio_pstBlock -> MbPartPredMode[0] != INTRA_16x16){
			if(read_me_svc(ai_pcData, position, aio_pstBlock -> MbPartPredMode[0], Current_residu, iCurrMbAddr, ai_pstSlice, iMb_x)){
				return 1;
			}

			if ( (Current_residu -> Cbp & 15) && Pps -> transform_8x8_mode_flag && (BaseModeFlag || (!IS_I(Current_residu -> MbType) &&
				noSubMbPartSizeLessThan8x8 && ( Current_residu -> MbType == B_direct || direct_8x8_inference_flag))) && 
				getNbits(ai_pcData, position, 1)){
					Current_residu -> Transform8x8 = aio_pstBlock -> Transform8x8 = INTRA_8x8;
			}
		} 


		if ( Current_residu -> Cbp > 0 || (Current_residu -> MbType == INTRA_16x16)) {
			int mb_qp_delta = read_se(ai_pcData, position);

			residual(ai_pcData, position, Current_residu, vlc, NonZeroCountCache);

			//In case of cbp is equal to zéro, we check if there is a DC level 
			if (Current_residu -> Cbp == 0 && NonZeroCountCache[0] != 0){
				Current_residu -> Cbp = 15;
			}

			if ( Current_residu -> MbType == INTRA_16x16 && !(Current_residu -> Cbp & 15) && (Current_residu -> Cbp & 0x30) && NonZeroCountCache[0]){
				Current_residu -> Cbp += 15;
			}

#ifdef TI_OPTIM
			*last_QP = Current_residu -> Qp = divide(*last_QP + mb_qp_delta + 52, 52) >> 8 ;
#else
			*last_QP = Current_residu -> Qp = (*last_QP + mb_qp_delta + 52) % 52;
#endif

		}else { 
			NonZeroCountCache [12]	= NonZeroCountCache [13] = NonZeroCountCache [14] = NonZeroCountCache [15] = 
				NonZeroCountCache [20] = NonZeroCountCache [21] = NonZeroCountCache [22] = NonZeroCountCache [23] = 
				NonZeroCountCache [28] = NonZeroCountCache [29] = NonZeroCountCache [30] = NonZeroCountCache [31] = 
				NonZeroCountCache [36] = NonZeroCountCache [37] = NonZeroCountCache [38] = NonZeroCountCache [39] = 
				NonZeroCountCache [ 9] = NonZeroCountCache [10] = NonZeroCountCache [17] = NonZeroCountCache [18] = 
				NonZeroCountCache [33] = NonZeroCountCache [34] =	NonZeroCountCache [41] = NonZeroCountCache [42] = 0;
			Current_residu -> Qp = *last_QP;
		}
	}
	return 0;
}
