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



#include "data.h"
#include "cavlc.h"
#include "bitstream.h"
#include "fill_caches.h"
#include "mb_pred_main.h"
#include "macroblock_layer_main.h"
#include "macroblock_partitionning.h"






/**
CAVLC for B type macroblock.
*/
char macroblock_B_partitionning (const PPS *Pps, RESIDU *Current_residu, const unsigned char *ai_pcData, 
								int *aio_piPosition, SLICE *Slice, DATA *aio_pstBlock, const VLC_TABLES * Vlc, 
								unsigned char NonZeroCountCache [ ], unsigned char  *SliceTable, 
								const short ai_iMb_x, const short ai_iMb_y, int direct_8x8_inference_flag, 
								int long_term, int slice_type, int is_svc, unsigned char *last_QP, int iCurrMbAddr, 
								short mv_cache_l0[][2], short mv_cache_l1[][2], short *ref_cache_l0, short *ref_cache_l1, 
								short *mvl1_l0, short *mvl1_l1, short *refl1_l0, short *refl1_l1)
{
					   
	

	int mb_qp_delta;
	int dct_allowed = Pps -> transform_8x8_mode_flag;

#ifdef ERROR_DETECTION
	//Error detection
	if(ErrorsCheckBMbType(Current_residu -> MbType)){
		return 1;
	}
#endif
	
	//Updating the Slice table in order to save in which Slice each macroblock belong to
	Current_residu -> SliceNum = SliceTable [iCurrMbAddr] = Slice -> slice_num;
    
	//Updating the parameter in order to decode the VLC
	fill_caches( Slice, Current_residu, 0, NonZeroCountCache, aio_pstBlock, SliceTable, ai_iMb_x, ai_iMb_y);

		 
	//Recovery of the motion vectors for the sub_macroblock 
	if (aio_pstBlock -> NumMbPart == 4 ) {
		if(sub_mb_pred_B(&dct_allowed, ai_pcData, aio_piPosition, Slice, Current_residu, 
			mv_cache_l0, ref_cache_l0,mv_cache_l1, ref_cache_l1, mvl1_l0, mvl1_l1, 
			refl1_l0, refl1_l1, direct_8x8_inference_flag, long_term, slice_type, is_svc)){
				return 1;
		}
	}
	else if (Current_residu -> MbType != B_direct){ 
		//Recovery of the prediction mode and the motion vectors for the macroblock 
		if(mb_pred_B(ai_pcData, aio_piPosition, Slice, aio_pstBlock, Current_residu, 
			mv_cache_l0, mv_cache_l1, ref_cache_l0, ref_cache_l1)){
				return 1;
		}
	}else {
		motion_vector_B_skip(mv_cache_l0, mv_cache_l1, ref_cache_l0, ref_cache_l1, 
			mvl1_l0, mvl1_l1, refl1_l0, refl1_l1, Slice, 
			direct_8x8_inference_flag, long_term,  slice_type,  is_svc);
	}


	Current_residu -> Cbp = read_me(ai_pcData, aio_piPosition, aio_pstBlock -> MbPartPredMode[0]);
	if ( dct_allowed && (Current_residu -> Cbp & 15)){
		if(getNbits(ai_pcData, aio_piPosition, 1)){
			Current_residu -> Transform8x8 = aio_pstBlock -> Transform8x8 = INTRA_8x8;
		}
	} 
	
	if ( Current_residu -> Cbp > 0 ){
		mb_qp_delta = read_se(ai_pcData, aio_piPosition);
#ifdef TI_OPTIM
		//Initialization of the macroblock neighbourhood
		*last_QP = Current_residu -> Qp = divide(*last_QP + mb_qp_delta + 52, 52) >> 8 ;
#else
		*last_QP = Current_residu -> Qp = (*last_QP + mb_qp_delta + 52) % 52;
#endif

		//Decoding process of the VLC 
		residual(ai_pcData, aio_piPosition, Current_residu, Vlc, NonZeroCountCache);
	} 
	else 
	{
		//In case of there is no error in the prediction
		init_mb_skip(NonZeroCountCache);
		Current_residu -> Qp = *last_QP;
	}
	return 0;
}


/**
This function permits to decode the VLC. 
All the parameters decoded will be stored in differents structures or tables.


@param picture_residu Structure whixh contains information about the macroblock.
@param data The NAL unit.
@param NalBytesInNalunit The sizes of the NAL unit in bytes.
@param aio_piPosition The current aio_piPosition in the NAL.
@param Slice The Slice structure.
@param block Contains all parameters of the current macroblock.
@param vlc The VLC tables in order to decode the Nal Unit.
@param non_zero_count_cache Specifies the coeff_token of each blocks 4x4 of a macroblock.
@param non_zero_count Specifies the coeff_token of each block of the picture.
@param SliceTable Specifies in which Slice belongs each macroblock of the picture.
@param intra_pred_mod Contains the prediction mode for each macroblock.
@param ai_iMb_x The x position of the macroblock in the picture.
@param ai_iMb_y The y position of the macroblock in the picture.
@param constrained_intra_pred_flag Enables to determine if the intra decoding process is contrained.
@param direct_8x8_inference_flag Enables to determine if 8x8 mode is allowed or not.
@param transform_8x8_mode_flag Determines if the macroblock is in 8x8 mode.
@param last_QP Give the QP of the last decoded macroblock.
*/
char macroblock_layer_B (const PPS *Pps, RESIDU *picture_residu,  const unsigned char *ai_pcData,  int *aio_piPosition, 
						SLICE *Slice, DATA *aio_tstBlock, const VLC_TABLES * Vlc, unsigned char *NonZeroCountCache, 
						unsigned char  *SliceTable, const short ai_iMb_x, const short ai_iMb_y, int direct_8x8_inference_flag, 
						int long_term, int slice_type, int is_svc, unsigned char *last_QP, short mv_cache_l0[][2], short mv_cache_l1[][2], 
						short *ref_cache_l0, short *ref_cache_l1, short *mvl1_l0, short *mvl1_l1, short *refl1_l0, short *refl1_l1)
{  


	const int iCurrMbAddr = ai_iMb_x + ai_iMb_y * (short)(Slice -> mb_stride);
	DATA * aio_pstBlock = &aio_tstBlock[iCurrMbAddr];
    int mb_type = read_ue(ai_pcData, aio_piPosition);


#ifdef ERROR_DETECTION
	//Error detection
	if(ErrorsCheckMbType(mb_type, 23 + 27)){
		return 1;
	}
#endif

	//According to the Slice type and the macroblock type, the parameters are adjusted
	if (mb_type < 23)	{
		aio_pstBlock -> NumMbPart = b_mb_type_info[mb_type] . partcount ;
		aio_pstBlock -> MbPartPredMode [0] = b_mb_type_info[mb_type] . type_0 ;
		aio_pstBlock -> MbPartPredMode [1] = b_mb_type_info[mb_type] . type_1 ;
		picture_residu -> MbType = b_mb_type_info[mb_type] . name ;
		picture_residu -> Mode = b_mb_type_info[mb_type] . Mode ;

		return macroblock_B_partitionning(Pps, picture_residu, ai_pcData, aio_piPosition, Slice, 
			aio_pstBlock, Vlc, NonZeroCountCache, SliceTable, ai_iMb_x, ai_iMb_y, 
			direct_8x8_inference_flag, long_term, slice_type, is_svc, last_QP, iCurrMbAddr, 
			mv_cache_l0, mv_cache_l1, ref_cache_l0, ref_cache_l1, mvl1_l0, mvl1_l1, refl1_l0, refl1_l1);
	}
	else
	{
		mb_type -= 23; 
		aio_pstBlock -> MbPartPredMode [0] = i_mb_type_info[mb_type] . type;
		picture_residu -> MbType = i_mb_type_info[mb_type] . type ;
		picture_residu -> Cbp = i_mb_type_info[mb_type] . Cbp;
		picture_residu -> Intra16x16PredMode =  i_mb_type_info[mb_type] . pred_mode;		

		ref_cache_l0[14] = ref_cache_l0[30] = -1;
		ref_cache_l1[14] = ref_cache_l1[30] = -1;

		return macroblock_I_partitionning(Pps, picture_residu, ai_pcData, aio_piPosition, 
			Slice, aio_pstBlock, Vlc, NonZeroCountCache, SliceTable, 
			ai_iMb_x, ai_iMb_y, last_QP, iCurrMbAddr);
	}
}
