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
#include "bitstream.h"
#include "ErrorsDetection.h"
#include "macroblock_layer.h"
#include "macroblock_partitionning.h"



/**
This function permits to decode the VLC. 
All the parameters decoded will be stored in differents structures or tables.

@param Pps PPS structure of the current video.
@param picture_residu Structure whixh contains information about the macroblock.
@param data The NAL unit.
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
@param last_QP Give the QP of the last decoded macroblock.
*/
char macroblock_layer_I(const PPS *Pps, RESIDU *picture_residu,  const unsigned char *ai_pcData,  int *aio_piPosition, 
					   SLICE *Slice, DATA  *aio_tstBlock, const VLC_TABLES * Vlc, 
					   unsigned char NonZeroCountCache [ ], unsigned char *SliceTable, 
					   const short ai_iMb_x, const short ai_iMb_y, unsigned char *last_QP)
{  


	const int iCurrMbAddr = ai_iMb_x + ai_iMb_y * (short)(Slice -> mb_stride);
	DATA * aio_pstBlock = &aio_tstBlock[iCurrMbAddr];

    int mb_type = read_ue(ai_pcData, aio_piPosition);

#ifdef ERROR_DETECTION
	//Error detection
	if(ErrorsCheckMbType(mb_type, 27)){
		return 1;
	}
#endif

	//According to the macroblock type, the parameters are adjusted
	aio_pstBlock -> MbPartPredMode [0] = i_mb_type_info[mb_type] . type ;
	picture_residu -> MbType = i_mb_type_info[mb_type] . type ;
	picture_residu -> Cbp = i_mb_type_info[mb_type] . Cbp;
	picture_residu -> Intra16x16PredMode = i_mb_type_info[mb_type] . pred_mode;
          
	
	return macroblock_I_partitionning(Pps, picture_residu, ai_pcData, aio_piPosition,
		Slice, aio_pstBlock, Vlc, NonZeroCountCache, SliceTable, 
		ai_iMb_x, ai_iMb_y, last_QP, iCurrMbAddr);
}






/**
This function permits to decode the VLC. 
All the parameters decoded will be stored in differents structures or tables.

@param Pps PPS structure of the current video.
@param picture_residu Structure whixh contains information about the macroblock.
@param data The NAL unit.
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
@param direct_8x8_inference_flag Enables to determine if 8x8 mode is allowed or not.
@param last_QP Give the QP of the last decoded macroblock.
@param mv_cache_l0 Table used to stock the motion vector of the current macroblock.
@param ref_cache_l0 Table used to stock the reference index of the current macroblock.
*/
char macroblock_layer_P (const PPS *Pps, RESIDU *picture_residu,  const unsigned char *ai_pcData,  int *aio_piPosition, 
						SLICE *Slice, DATA *aio_tstBlock, const VLC_TABLES * Vlc, 
						unsigned char NonZeroCountCache [ ], unsigned char *SliceTable, const short ai_iMb_x, 
						const short ai_iMb_y, unsigned char *last_QP, short mv_cache_l0[][2], short *ref_cache_l0)
{  

      

	const int iCurrMbAddr = ai_iMb_x + ai_iMb_y * (short)(Slice -> mb_stride);
	DATA * aio_pstBlock = &aio_tstBlock[iCurrMbAddr];
    int mb_type = read_ue(ai_pcData, aio_piPosition);

#ifdef ERROR_DETECTION
	//Error detection
	if(ErrorsCheckMbType(mb_type, 5 + 27)){
		return 1;
	}
#endif

	//According to the Slice type and the macroblock type, the parameters are adjusted

	if (mb_type < 5)	{
		aio_pstBlock -> NumMbPart = p_mb_type_info[mb_type].partcount ;
		aio_pstBlock -> MbPartPredMode [0] = p_mb_type_info[mb_type] . type_0 ;
		aio_pstBlock -> MbPartPredMode [1] = p_mb_type_info[mb_type] . type_1 ;
		picture_residu -> MbType =  p_mb_type_info[mb_type] . name ;
		picture_residu -> Mode = p_mb_type_info[mb_type] . Mode ;

		return macroblock_P_partitionning(Pps, picture_residu, ai_pcData, aio_piPosition, Slice, 
			aio_pstBlock, Vlc, NonZeroCountCache, SliceTable, ai_iMb_x, ai_iMb_y, last_QP, 
			iCurrMbAddr, mv_cache_l0, ref_cache_l0);

	} 
	else	{
		mb_type -= 5; 
		aio_pstBlock -> MbPartPredMode [0] = i_mb_type_info[mb_type] . type ;
		picture_residu -> MbType = i_mb_type_info[mb_type] . type ;
		picture_residu -> Cbp = i_mb_type_info[mb_type] . Cbp;
		picture_residu -> Intra16x16PredMode =  i_mb_type_info[mb_type] . pred_mode;	

		ref_cache_l0[14] = ref_cache_l0[30] = -1;

		return macroblock_I_partitionning(Pps, picture_residu, ai_pcData, aio_piPosition, Slice, 
			aio_pstBlock, Vlc, NonZeroCountCache, SliceTable, ai_iMb_x, ai_iMb_y, last_QP, iCurrMbAddr);
	}
}
