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

#include "data.h"
#include "symbol.h"
#include "refill.h"
#include "mb_pred.h"
#include "bitstream.h"
#include "decode_mb_I.h"
#include "motion_vector.h"




/**
Decodes P reference list indexes.
*/
int GetCavlcPRefList(const unsigned char *ai_pcData, int *aio_piPosition, char *refLx,
					 int Mode, int NumRefLx)
{

	//Read one reference list and a second one for 16x8 or 8x16 block
	if ( NumRefLx > 0) {
		refLx [0] = (char) read_te(ai_pcData, aio_piPosition, NumRefLx);
		if (Mode != 3){
			refLx [1] = (char) read_te(ai_pcData, aio_piPosition, NumRefLx);
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
Decodes 16x16 motion vectors.
*/
void ReadCavlc16x16MotionVector(const unsigned char *ai_pcData, int *aio_piPosition, char *RefLx, short *RefCacheLx, short MvCacheLx[][2])
{
	short mvLx[2];

	refill_ref(4, 4, RefLx[0], &RefCacheLx[12]);
	mvLx[0] = (short) read_se(ai_pcData, aio_piPosition);
	mvLx[1] = (short) read_se(ai_pcData, aio_piPosition);
	motion_vector(MvCacheLx, mvLx, RefCacheLx, &MvCacheLx[12][0], RefCacheLx[12], 12, 4);
	refill_mv(4, 4, MvCacheLx[12], &MvCacheLx[12]);
}

/**
Decodes 8x16 motion vectors.
*/
void ReadCavlc8x16MotionVector(const unsigned char *ai_pcData, int *aio_piPosition, char *RefLx, short *RefCacheLx, short MvCacheLx[][2], int Index)
{
	short mvLx[2];

	refill_ref(2, 4, RefLx[0], &RefCacheLx[Index]); 
	mvLx[0] = (short) read_se(ai_pcData, aio_piPosition);
	mvLx[1] = (short) read_se(ai_pcData, aio_piPosition);
	motion_vector_8x16(MvCacheLx, mvLx, RefCacheLx, &MvCacheLx[Index][0], RefLx [0], Index);
	refill_mv(2, 4, MvCacheLx[Index], &MvCacheLx[Index]); 

}

/**
Decodes 16x8 motion vectors.
*/
void ReadCavlc16x8MotionVector(const unsigned char *ai_pcData, int *aio_piPosition, char *RefLx, short *RefCacheLx, short MvCacheLx[][2], int Index)
{
	short mvLx[2];

	refill_ref(4, 2, RefLx[0], &RefCacheLx[Index]); 
	mvLx[0] = (short) read_se(ai_pcData, aio_piPosition);
	mvLx[1] = (short) read_se(ai_pcData, aio_piPosition);
	motion_vector_16x8(MvCacheLx, mvLx, RefCacheLx, &MvCacheLx[Index][0], RefLx[0], Index);
	refill_mv(4, 2, &MvCacheLx[Index][0], &MvCacheLx[Index]); 
}


/**
This function permits to decode the prediction mode for an I macroblock. 

@param ai_pcData The NAL unit.
@param aio_piPosition The current aio_piPosition in the NAL.
@param aio_pstMacroblock Contains all parameters of the current macroblock.
@param ao_pstIntra_pred_mode Contains the prediction mode for the current macroblock.
@param intra4x4_pred_mode_cache Table containing the mode of the 4x4 neighbourhood.
*/
int mb_pred_I( const unsigned char *ai_pcData, int *aio_piPosition, RESIDU *Residu, short *Mode, short *intra4x4_pred_mode_cache)
{


	int luma4x4BlkIdx ;  

	//Recovery of the prediction mode
	if ( Residu -> MbType == INTRA_4x4 )	{
		for ( luma4x4BlkIdx = 0 ; luma4x4BlkIdx < 16 ; luma4x4BlkIdx++ )   {
			Mode[luma4x4BlkIdx] = intra4x4_pred_mode_cache[SCAN8(luma4x4BlkIdx)] = GetCavlcIPredMode(ai_pcData, aio_piPosition, intra4x4_pred_mode_cache, luma4x4BlkIdx);
		}


	}else if(  Residu -> MbType == INTRA_8x8 ) {
		for ( luma4x4BlkIdx = 0 ; luma4x4BlkIdx < 4 ; luma4x4BlkIdx ++ )   {
			Mode[(luma4x4BlkIdx << 2)] = Mode[(luma4x4BlkIdx << 2) + 1] = 
				Mode[(luma4x4BlkIdx << 2) + 2] =  Mode[(luma4x4BlkIdx << 2) + 3] = 
				intra4x4_pred_mode_cache[SCAN8(luma4x4BlkIdx << 2)] = intra4x4_pred_mode_cache[SCAN8(1 + (luma4x4BlkIdx << 2))] =
				intra4x4_pred_mode_cache[SCAN8( (luma4x4BlkIdx << 2) + 2)] = intra4x4_pred_mode_cache[SCAN8((luma4x4BlkIdx << 2) + 3)] = GetCavlcIPredMode(ai_pcData, aio_piPosition, intra4x4_pred_mode_cache, luma4x4BlkIdx << 2);
		}
	}


	//Reading the chrominance prediction mode
	Residu -> IntraChromaPredMode = (char) read_ue(ai_pcData, aio_piPosition);

#ifdef ERROR_DETECTION
	//Error detection
	if(ErrorsCheckIntraChromaPredMode(&Residu -> IntraChromaPredMode)){
		return 1;
	}
#endif

	return 0;
}




/**
This function permits to decode the image reference of each sub-macroblock for a P macroblock. 

@param data The NAL unit.
@param aio_piPosition The current aio_piPosition in the NAL.
@param Slice The Slice structure.
@param block Contains all parameters of the current macroblock.
@param mv_cache_l0 Table used to stock the motion vector of the current macroblock.
@param ref_cache_l0 Table used to stock the reference index of the current macroblock.

*/
int sub_mb_pred_P(int *dct_allowed, const unsigned char *RESTRICT Data, int *Position, const SLICE *Slice, 
				  RESIDU *Current_residu, short mv_cache_l0[][2], short *ref_cache_l0)
{	
	short mvL0[2];
	int mbPartIdx ;
	int subMbPartIdx ;
	char RefIdxL0[4];


	//Recovery of the sub-macroblock type
	GetCavlcSubMbType(Data, Position, Current_residu);

#ifdef ERROR_DETECTION
	//Error detection 
	if(ErrorsCheckSubMbType(Current_residu -> SubMbType, MaxPSubMbType)){
		//should be set to zero because the macroblock will be decoded.
		ref_cache_l0 [12] = ref_cache_l0 [14] = ref_cache_l0 [28] = ref_cache_l0 [30] = 0;
		return 1;
	}
#endif

	//Recovery of the image reference of each sub-macroblock
	if (Slice -> num_RefIdxL0_active_minus1 && (Current_residu -> MbType != P_8x8ref0)) {
		if(GetCavlcPSubRefLx(Data, Position, RefIdxL0, Slice -> num_RefIdxL0_active_minus1)){
			ref_cache_l0 [12] = ref_cache_l0 [14] = ref_cache_l0 [28] = ref_cache_l0 [30] = 0;
			return 1;
		}
	}else{
		RefIdxL0 [0] = RefIdxL0 [1] = RefIdxL0 [2] = RefIdxL0 [3] = 0;
	}


	//Recovery of the motion vector of each sub-macroblock
	for ( mbPartIdx = 0 ; mbPartIdx < 4 ; mbPartIdx++ )   {   
		int sub_mb_type = Current_residu -> SubMbType [mbPartIdx];
		for ( subMbPartIdx = 0 ; subMbPartIdx < sub_num_p_part[sub_mb_type] ; subMbPartIdx++ ) {
			int index = SCAN8(( mbPartIdx << 2) + (sub_mb_type == 1? 2 * subMbPartIdx: subMbPartIdx));
			refill_ref(2 - (sub_mb_type > 1), 2 - (sub_mb_type & 1), RefIdxL0[mbPartIdx], &ref_cache_l0[index]);  
			mvL0[0] = (short) read_se(Data, Position);
			mvL0[1] = (short) read_se(Data, Position);
			motion_vector(mv_cache_l0, mvL0, ref_cache_l0, &mv_cache_l0[index][0], RefIdxL0[mbPartIdx], index, 2 / ( 1 + (sub_mb_type > 1)));
		}
		refill_motion_vector(0, sub_mb_type, &mv_cache_l0[SCAN8(( mbPartIdx << 2))]);
	}


	for ( mbPartIdx = 0;  mbPartIdx < 4 && *dct_allowed; mbPartIdx ++){
		if (!(sub_num_p_part[Current_residu -> SubMbType[mbPartIdx]] == 1)){
			*dct_allowed = 0;
		}
	}

	return 0;
}




/**
This function permits to decode the image reference for a P macroblock. 

@param data The NAL unit.
@param aio_piPosition The current aio_piPosition in the NAL.
@param Slice The Slice structure.
@param block Contains all parameters of the current macroblock.
@param mv_cache_l0 Table used to stock the motion vector of the current macroblock.
@param ref_cache_l0 Table used to stock the reference index of the current macroblock.

*/
int mb_pred_P ( const unsigned char *ai_pcData, int *aio_piPosition, const SLICE *Slice, 
			   RESIDU *CurrResidu, short mv_cache_l0[][2], short *ref_cache_l0)
{


	char RefIdxL0[2] = {0, 0};

	if(GetCavlcPRefList(ai_pcData, aio_piPosition, RefIdxL0, CurrResidu -> Mode, 
		Slice -> num_RefIdxL0_active_minus1)){
			ref_cache_l0 [12] = ref_cache_l0 [14] = ref_cache_l0 [28] = ref_cache_l0 [30] = 0;
			return 1;
	}


	//According to the size block, we calculate the real motion vector.
	if (CurrResidu -> Mode == 3){
		//Recovery of the image reference for the frame P
		ReadCavlc16x16MotionVector(ai_pcData, aio_piPosition, RefIdxL0, ref_cache_l0, mv_cache_l0);
	}else {
		//Read Motion vectors

		//Recovery of of the motion vector for the frame P
		if (CurrResidu -> Mode == 1){
			// mode == 1: 8x16
			ReadCavlc8x16MotionVector(ai_pcData, aio_piPosition, RefIdxL0, ref_cache_l0, mv_cache_l0, 12);
			ReadCavlc8x16MotionVector(ai_pcData, aio_piPosition, &RefIdxL0[1], ref_cache_l0, mv_cache_l0, 14);
		}else{
			ReadCavlc16x8MotionVector(ai_pcData, aio_piPosition, RefIdxL0, ref_cache_l0, mv_cache_l0, 12);
			ReadCavlc16x8MotionVector(ai_pcData, aio_piPosition, &RefIdxL0[1], ref_cache_l0, mv_cache_l0, 28);
		}
	} 
	return 0;
}
