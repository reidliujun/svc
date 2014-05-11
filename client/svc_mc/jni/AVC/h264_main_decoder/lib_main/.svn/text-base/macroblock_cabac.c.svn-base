/* FFmpeg project
* libavcodec (h264.c)
* H.26L/H.264/AVC/JVT/14496-10/... encoder/decoder
* Copyright (c) 2003 Michael Niedermayer <michaelni@gmx.at>
*
* This file has been modified for opensvcdecoder.


*****************************************************************************
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


#include "macroblock_cabac.h"
#include "mb_cabac_partionning.h"



/**
This function determine the type of the current intra macroblock.



@param c Contains the data concernig the CABAC for decoding the VLC.
@param state State of the current slice.
@param Tab_Block Contains all parameters of the current macroblock.
@param macro Specifies the availability of the neighbouring macroblock.
@param mb_xy CUrrent address of the macroblock.
@param intra_slice == 1 if it's an intra slice.
@param stride Width of the picture in macroblock.
*/
int  decode_cabac_intra_mb_type(CABACContext *c, unsigned char *state, RESIDU *Residu, int intra_slice, const int mb_stride){

	int mb_type ;
	if(intra_slice){
		int AvailMask = Residu -> AvailMask;
		const int mba_xy = - 1 ;
		const int mbb_xy = - mb_stride;
		int ctx = 0;
		if( (AvailMask & 0x01) && !IS_NxN(Residu [mba_xy] . MbType)){
			ctx++;
		}

		if( ((AvailMask >> 1) & 0x01)  && !IS_NxN(Residu [mbb_xy] . MbType) ){
			ctx++;
		}

		if( get_cabac( c, &state[ctx] ) == 0 ){
			return 0;   /* I4x4 */
		}
		state += 2;
	}else{
		if( get_cabac( c, &state[0] ) == 0 )
			return 0;   /* I4x4 */
	}

	if( get_cabac_terminate( c, 0, 1 ) )
		return 25;  /* PCM */

	mb_type = 1; /* I16x16 */
	if( get_cabac( c, &state[1] ) )
		mb_type += 12;  /* cbp_luma != 0 */

	if( get_cabac( c, &state[2] ) ) {
		if( get_cabac( c, &state[2 + intra_slice] ) )
			mb_type += 4 * 2;   /* cbp_chroma == 2 */
		else
			mb_type += 4 * 1;   /* cbp_chroma == 1 */
	}
	if( get_cabac( c, &state[3+intra_slice] ) )
		mb_type += 2;
	if( get_cabac( c, &state[3+2*intra_slice] ) )
		mb_type += 1;
	return mb_type;

}





/**
This function determine the type of the current macroblock.

@param c Contains the data concernig the CABAC for decoding the VLC.
@param state State of the current slice.
@param Tab_Block Contains all parameters of the current macroblock.
@param SliceTable Specifies in which Slice belongs each macroblock of the picture.
@param macro Specifies the availability of the neighbouring macroblock.
@param mb_xy CUrrent address of the macroblock.
@param slice_type Slice type of the current slice.
@param mb_stride Width of the picture in macroblock.
*/
int decode_cabac_mb_type( CABACContext *c, unsigned char *state, RESIDU *CurrResidu, 
						 unsigned char *SliceTable, int mb_xy, int slice_type, const int mb_stride)
{




	if( slice_type == SLICE_TYPE_P ) {
		if( get_cabac( c, &state[14] ) == 0 ) {
			/* P-type */
			if( get_cabac( c, &state[15] ) == 0 ) {
				if( get_cabac( c, &state[16] ) == 0 )
					return 0; /* P_L0_D16x16; */
				else
					return 3; /* P_8x8; */
			} else {
				if( get_cabac( c, &state[17] ) == 0 )
					return 2; /* P_L0_D8x16; */
				else
					return 1; /* P_L0_D16x8; */
			}
		} else {
			return decode_cabac_intra_mb_type(c, &state[17], CurrResidu, 0, mb_stride) + 5;
		}
	} else  {
		const int mba_xy = mb_xy - 1;
		const int mbb_xy = mb_xy - mb_stride;
		int ctx = 0;
		int bits;

		if( (CurrResidu -> AvailMask & 0x01) && SliceTable[mba_xy] == SliceTable[mb_xy] && CurrResidu [-1] . MbType != B_Skip
			&& CurrResidu [-1] . MbType != B_direct)
			ctx++;
		if( ((CurrResidu -> AvailMask >> 1) & 0x01) && SliceTable[mbb_xy] == SliceTable[mb_xy] && CurrResidu [- mb_stride] . MbType != B_Skip
			&& CurrResidu [- mb_stride] . MbType != B_direct )
			ctx++;

		if( !get_cabac( c, &state[27 + ctx] ) )
			return 0; /* B_Direct_16x16 */

		if( !get_cabac( c, &state[27 + 3] ) ) {
			return 1 + get_cabac( c, &state[27 + 5] ); /* B_L[01]_16x16 */
		}

		bits = get_cabac( c, &state[27 + 4] ) << 3;
		bits|= get_cabac( c, &state[27 + 5] ) << 2;
		bits|= get_cabac( c, &state[27 + 5] ) << 1;
		bits|= get_cabac( c, &state[27 + 5] );

		if( bits < 8 )
			return bits + 3; /* B_Bi_16x16 through B_L1_L0_16x8 */
		else if( bits == 13 ) {
			return decode_cabac_intra_mb_type(c, &state[32], CurrResidu, 0, mb_stride) + 23;
		} else if( bits == 14 )
			return 11; /* B_L1_L0_8x16 */
		else if( bits == 15 )
			return 22; /* B_8x8 */

		bits = ( bits << 1 ) | get_cabac( c, &state[27 + 5] );
		return bits - 4; /* B_L0_Bi_* through B_Bi_Bi_* */
	}
}



/**
Decoding process of a macroblock from a B slice
*/
char macroblock_layer_B_cabac (RESIDU *picture_residu, SLICE *slice, DATA *Tab_block, unsigned char *non_zero_count_cache, 
							   unsigned char *SliceTable, int iCurrMbAddr, CABACContext *cabac, unsigned char *cabac_state, 
							   CABAC_NEIGH *neigh, short mv_cache_l0[][2], short mv_cache_l1[][2], short *ref_cache_l0, 
							   short *ref_cache_l1, short x, short y, int constrained_intra_pred_flag, int direct_8x8_inference_flag, 
							   int dct_allowed, unsigned char *last_QP, int *mb_qp_delta, int is_svc, short mvl0_cache[][2], 
							   short mvl1_cache[][2], short *refl0_cache, short *refl1_cache, int long_term, int slice_type, 
							   short *mvl1_l0, short *mvl1_l1, short *refl1_l0, short *refl1_l1)
{  


	DATA *block = &Tab_block[iCurrMbAddr];
	int mb_type = decode_cabac_mb_type(cabac, cabac_state, picture_residu, SliceTable, iCurrMbAddr,  slice -> slice_type, slice -> mb_stride);

#ifdef ERROR_DETECTION
	//Error detection
	if(ErrorsCheckMbType(mb_type, 23 + 27)){
		return 1;
	}
#endif


	//According to the slice type and the macroblock type  
	//the parameters are adjusted
	if (mb_type < 23){
		block -> NumMbPart = b_mb_type_info[mb_type] . partcount ;
		block -> MbPartPredMode [0] = b_mb_type_info[mb_type] . type_0 ;
		block -> MbPartPredMode [1] = b_mb_type_info[mb_type] . type_1 ;
		picture_residu -> MbType = b_mb_type_info[mb_type] . name ;
		picture_residu -> Mode = b_mb_type_info[mb_type] . Mode ;

		return mb_cabac_B_partionning(picture_residu,  slice, Tab_block, non_zero_count_cache, SliceTable, 
			iCurrMbAddr, cabac, cabac_state, neigh, mv_cache_l0, mv_cache_l1, ref_cache_l0, ref_cache_l1, 
			x, y, direct_8x8_inference_flag, dct_allowed, last_QP, mb_qp_delta, is_svc, mvl0_cache, 
			mvl1_cache, refl0_cache, refl1_cache, long_term, slice_type, mvl1_l0, mvl1_l1, refl1_l0, refl1_l1);

	}else{
		mb_type -= 23; 
		block -> MbPartPredMode [0] = i_mb_type_info[mb_type] . type ;
		picture_residu -> MbType = i_mb_type_info[mb_type] . type ;
		picture_residu -> Cbp = i_mb_type_info[mb_type] . Cbp;
		picture_residu -> Intra16x16PredMode =  i_mb_type_info[mb_type] . pred_mode;	

		ref_cache_l0[14] = ref_cache_l0[30] = -1;
		ref_cache_l1[14] = ref_cache_l1[30] = -1;

		return mb_cabac_I_partionning(picture_residu, slice, Tab_block, block, non_zero_count_cache, SliceTable, 
			iCurrMbAddr, cabac, cabac_state, neigh, x, y, constrained_intra_pred_flag, dct_allowed, last_QP, mb_qp_delta);
	}

}





/**
Decoding process of a macroblock from a I slice
*/
char  macroblock_layer_I_cabac (RESIDU *picture_residu, SLICE *slice, DATA *Tab_block, unsigned char *non_zero_count_cache, 
							   unsigned char *SliceTable, int iCurrMbAddr, CABACContext *cabac, unsigned char *cabac_state,
							   CABAC_NEIGH *neigh, short x, short y, int constrained_intra_pred_flag, int dct_allowed, 
							   unsigned char *last_QP, int *mb_qp_delta)
{  



	int mb_type = decode_cabac_intra_mb_type(cabac, &cabac_state[3], picture_residu, 1, slice -> mb_stride);
	DATA *block = &Tab_block[iCurrMbAddr];

#ifdef ERROR_DETECTION
	//Error detection
	if(ErrorsCheckMbType(mb_type, 27)){
		return 1;
	}
#endif

	block -> MbPartPredMode [0] = i_mb_type_info[mb_type] . type ;
	picture_residu -> MbType = i_mb_type_info[mb_type] . type ;
	picture_residu -> Cbp = i_mb_type_info[mb_type] . Cbp;
	picture_residu -> Intra16x16PredMode =  i_mb_type_info[mb_type] . pred_mode;

	return mb_cabac_I_partionning(picture_residu, slice, Tab_block, block,  non_zero_count_cache, SliceTable, 
		iCurrMbAddr, cabac, cabac_state, neigh, x, y, constrained_intra_pred_flag, dct_allowed, last_QP, mb_qp_delta);
}





/**
Decoding process of a macroblock from a P slice
*/
char macroblock_layer_P_cabac (RESIDU *picture_residu, SLICE *slice, DATA *Tab_block, unsigned char *non_zero_count_cache, 
							   unsigned char *SliceTable, int iCurrMbAddr, CABACContext *cabac, unsigned char *cabac_state, 
							   CABAC_NEIGH *neigh, short mv_cache_l0[][2], short *ref_cache_l0, short x, short y, 
							   int constrained_intra_pred_flag, int dct_allowed, unsigned char *last_QP, int *mb_qp_delta, 
							   short mvl0_cache[][2], short *refl0_cache)
{  



	DATA *block = &Tab_block[iCurrMbAddr];
	int mb_type = decode_cabac_mb_type(cabac, cabac_state, picture_residu, SliceTable, iCurrMbAddr,  slice -> slice_type, slice -> mb_stride);

#ifdef ERROR_DETECTION
	//Error detection
	if(ErrorsCheckMbType(mb_type, 5 + 27)){
		return 1;
	}
#endif

	if (mb_type < 5){
		block -> NumMbPart = p_mb_type_info[mb_type] . partcount ;
		block -> MbPartPredMode [0] = p_mb_type_info[mb_type] . type_0 ;
		block -> MbPartPredMode [1] = p_mb_type_info[mb_type] . type_1 ;
		picture_residu -> MbType = p_mb_type_info[mb_type] . name ;
		picture_residu -> Mode = p_mb_type_info[mb_type] . Mode ;

		return mb_cabac_P_partionning(picture_residu,  slice, Tab_block, block,  non_zero_count_cache, SliceTable, 
			iCurrMbAddr, cabac, cabac_state, neigh, mv_cache_l0, ref_cache_l0, x, y, dct_allowed, 
			last_QP, mb_qp_delta, mvl0_cache, refl0_cache);

	} else{
		mb_type -= 5; 
		block -> MbPartPredMode [0] = i_mb_type_info[mb_type] . type ;
		picture_residu -> MbType = i_mb_type_info[mb_type] . type ;
		picture_residu -> Cbp = i_mb_type_info[mb_type] . Cbp;
		picture_residu -> Intra16x16PredMode =  i_mb_type_info[mb_type] . pred_mode;	

		ref_cache_l0[14] = ref_cache_l0[30] = -1;

		return mb_cabac_I_partionning(picture_residu, slice, Tab_block, block, non_zero_count_cache, SliceTable, 
			iCurrMbAddr, cabac, cabac_state, neigh, x, y, constrained_intra_pred_flag, dct_allowed, 
			last_QP, mb_qp_delta);
	}
}
