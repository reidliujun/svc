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




#include "data.h"
#include "init_data.h"
#include "fill_caches.h"



void FillNonZeroCount(const DATA *CurrBlock, const int MbStride,const int top_type, 
					  const int  left_type, unsigned char *NonZeroCountCache){


	//Updating the NonZeroCountCache table from the current macroblock.
	if ( top_type ){
		NonZeroCountCache [4] = CurrBlock [-MbStride] . NonZeroCount[36];
		NonZeroCountCache [5] = CurrBlock [-MbStride] . NonZeroCount[37];
		NonZeroCountCache [6] = CurrBlock [-MbStride] . NonZeroCount[38];
		NonZeroCountCache [7] = CurrBlock [-MbStride] . NonZeroCount[39];
		NonZeroCountCache [1] = CurrBlock [-MbStride] . NonZeroCount[17];
		NonZeroCountCache [2] = CurrBlock [-MbStride] . NonZeroCount[18];
		NonZeroCountCache [1 + 24] = CurrBlock [-MbStride] . NonZeroCount[41];
		NonZeroCountCache [2 + 24] = CurrBlock [-MbStride] . NonZeroCount[42];
	}


	//Updating the adress of the macroblock which belong to the neighbourhood.
	if (left_type){
		NonZeroCountCache [11] = CurrBlock [-1] . NonZeroCount[15];
		NonZeroCountCache [19] = CurrBlock [-1] . NonZeroCount[23];
		NonZeroCountCache [8] = CurrBlock [-1] . NonZeroCount[10];
		NonZeroCountCache [32] = CurrBlock [-1] . NonZeroCount[34];

		NonZeroCountCache [27] = CurrBlock [-1] . NonZeroCount[31];
		NonZeroCountCache [35] = CurrBlock [-1] . NonZeroCount[39];
		NonZeroCountCache [16] = CurrBlock [-1] . NonZeroCount[18];
		NonZeroCountCache [40] = CurrBlock [-1] . NonZeroCount[42];
	}
}




/**
This function permits to update the neighbourhood of the current macroblock in a I slice.
This function stock in table the data calculated for the macroblock which belong to the neighbourhood of the current macroblock.

@param Slice The Slice strucutre.
@param aio_tstBlock Contains all parameters of each macroblock of the current picture.
@param EntropyCodingModeFlog The type of the VLC coding, CABAC or CAVLC.
@param NonZeroCountCache A cache table where coeff_token is stocked for each 4x4 block of the current macroblock.
@param ai_tstNon_zero_count A table where each coeff_token calculated is stocked for each macroblock decoded.
@param SliceTable A table which resume in which Slice each macroblock belongs to.
@param intra_pred_mod_cache Contains the prediction mode for each block 4x4 of the current macroblock.
@param intra_pred_mod Contains the prediction mode for each macroblock.
@param ai_iMb_x The x position of the macroblock in the picture.
@param ai_iMb_y The y position of the macroblock in the picture.
@param constrained_intra_pred_flag Enables to determine if the intra decoding process is contrained.
*/
void fill_caches_I (const SLICE *Slice, const RESIDU *CurrResidu, const int EntropyCodingModeFlog, 
					unsigned char NonZeroCountCache [ RESTRICT ], const DATA *CurrBlock, 
					const unsigned char *SliceTable, short *aio_tiIntra4x4_pred_mode_cache, 
					const short ai_iMb_x , const short ai_iMb_y, const int ai_iConstrained_intra_pred)
{
    int		top_type;
    int     left_type;
    const int iMb_xy = ai_iMb_x + ai_iMb_y * (short)(Slice -> mb_stride);
	const int ai_iMb_type_name = CurrResidu -> MbType;
	const int val_init = EntropyCodingModeFlog && !(IS_I(ai_iMb_type_name)) ? 0 : 64 ;
	init_non_zero_count_cache(NonZeroCountCache, val_init);

	if ( ai_iMb_y > 0 ) {
		short top_xy = iMb_xy - Slice -> mb_stride;
		top_type = (SliceTable [top_xy] == Slice -> slice_num) ? CurrResidu [-  Slice -> mb_stride] . MbType : 0;
	}else {
		top_type = 0;
	}

	if ( iMb_xy > 0 && ai_iMb_x != 0 )  {
		short left_xy = iMb_xy - 1 ;
		left_type = SliceTable [left_xy] == Slice -> slice_num ? CurrResidu [-1] . MbType : 0 ;
	}else {
		left_type = 0;
	}

	FillNonZeroCount(CurrBlock, Slice -> mb_stride, top_type, left_type, NonZeroCountCache);

	//Updating the aio_tiIntra4x4_pred_mode_cache table from the current macroblock.
	if(IS_NxN(top_type)){
		aio_tiIntra4x4_pred_mode_cache[4] = CurrResidu [- Slice -> mb_stride] . Intra16x16DCLevel[10];
		aio_tiIntra4x4_pred_mode_cache[5] = CurrResidu [- Slice -> mb_stride] . Intra16x16DCLevel[11];
		aio_tiIntra4x4_pred_mode_cache[6] = CurrResidu [- Slice -> mb_stride] . Intra16x16DCLevel[14];
		aio_tiIntra4x4_pred_mode_cache[7] = CurrResidu [- Slice -> mb_stride] . Intra16x16DCLevel[15];
    }
    else{
		if(top_type  && (!ai_iConstrained_intra_pred || IS_I(top_type))){
            aio_tiIntra4x4_pred_mode_cache[4] = aio_tiIntra4x4_pred_mode_cache[5] =
			aio_tiIntra4x4_pred_mode_cache[6] = aio_tiIntra4x4_pred_mode_cache[7] = 2;
		}else{
			aio_tiIntra4x4_pred_mode_cache[4] = aio_tiIntra4x4_pred_mode_cache[5] =
			aio_tiIntra4x4_pred_mode_cache[6] = aio_tiIntra4x4_pred_mode_cache[7] = -1;
		}
    }

    if(IS_NxN(left_type))  {
        aio_tiIntra4x4_pred_mode_cache[3 + 8] = CurrResidu [- 1] . Intra16x16DCLevel[5];
        aio_tiIntra4x4_pred_mode_cache[3 + 16] = CurrResidu [- 1] . Intra16x16DCLevel[7];
        aio_tiIntra4x4_pred_mode_cache[3 + 24] = CurrResidu [- 1] . Intra16x16DCLevel[13];
        aio_tiIntra4x4_pred_mode_cache[3 + 32] = CurrResidu [- 1] . Intra16x16DCLevel[15];
    }
    else   {
		if(left_type && (!ai_iConstrained_intra_pred || IS_I(left_type))){
            aio_tiIntra4x4_pred_mode_cache[3 + 8] = aio_tiIntra4x4_pred_mode_cache[3 + 24] =
			aio_tiIntra4x4_pred_mode_cache[3 + 16]= aio_tiIntra4x4_pred_mode_cache[3 + 32] = 2;

		}else{
			aio_tiIntra4x4_pred_mode_cache[3 + 8] = aio_tiIntra4x4_pred_mode_cache[3 + 24] =
			aio_tiIntra4x4_pred_mode_cache[3 + 16]= aio_tiIntra4x4_pred_mode_cache[3 + 32] = -1;
		}
    }
}







/**
This function permits to update the neighbourhood of the current macroblock in a P or B slice.
This function stock in table the data calculated for the macroblock which belong to the neighbourhood of the current macroblock.

@param Slice The Slice strucutre.
@param aio_tstBlock Contains all parameters of each macroblock of the current picture.
@param EntropyCodingModeFlog The type of the VLC coding, CABAC or CAVLC.
@param NonZeroCountCache A cache table where coeff_token is stocked for each 4x4 block of the current macroblock.
@param ai_tstNon_zero_count A table where each coeff_token calculated is stocked for each macroblock decoded.
@param SliceTable A table which resume in which Slice each macroblock belongs to.
@param ai_iMb_x The x position of the macroblock in the picture.
@param ai_iMb_y The y position of the macroblock in the picture.
@param constrained_intra_pred_flag Enables to determine if the intra decoding process is contrained.
*/
void fill_caches (const SLICE *Slice, const RESIDU *CurrResidu, const int EntropyCodingModeFlog, 
				  unsigned char NonZeroCountCache [ RESTRICT ], const DATA *CurrBlock, 
				  const unsigned char *SliceTable, const short ai_iMb_x, const short ai_iMb_y)
{


    int		top_type;
    int     left_type;
    const int iMb_xy = ai_iMb_x + ai_iMb_y * (short)(Slice -> mb_stride);
	const int ai_iMb_type_name = CurrResidu -> MbType;
	const int val_init = EntropyCodingModeFlog && !(IS_I(ai_iMb_type_name)) ? 0 : 64;
	init_non_zero_count_cache(NonZeroCountCache, val_init);

	if ( ai_iMb_y > 0 ) {
		short top_xy = iMb_xy - Slice -> mb_stride;
		top_type = (SliceTable [top_xy] == Slice -> slice_num) ? CurrResidu [-  Slice -> mb_stride] . MbType : 0 ;
	}else {
		top_type = 0;
	}

	if ( iMb_xy > 0 && ai_iMb_x != 0 )  {
		short left_xy = iMb_xy - 1 ;
		left_type = SliceTable [left_xy] == Slice -> slice_num ? CurrResidu [-1] . MbType : 0 ;
	}else {
		left_type = 0;
	}

	FillNonZeroCount(CurrBlock, Slice -> mb_stride, top_type, left_type, NonZeroCountCache);
}








/**
This function permits to update the neighbourhood of the current macroblock.
This function stock in table the motion vector for the macroblock which belong to the neighbourhood of the current macroblock.

@param Slice The Slice strucutre.
@param ai_iB_stride The width of sample in the ai_tiMv table.
@param ai_iB8_stride The width of sample in the ai_tiRef table.
@param aio_tiRef_cache_l0 A cache table where the reference is stocked for each 4x4 block of the current macroblock.
@param aio_tiMv_cache_l0 A cache table where the motion vector are stocked for each 4x4 block of the current macroblock.
@param SliceTable A table which resume in which Slice each macroblock belongs to.
@param aio_tstBlock Contains all parameters of each macroblock of the current picture.
@param ai_tiMv A table where each motion vector calculated is stocked for each macroblock decoded.
@param ai_tiRef A table where each reference mode calculated is stocked for each macroblock decoded.
@param ai_iMb_x The x position of the macroblock in the picture.
@param ai_iMb_y The y position of the macroblock in the picture.
*/
void fill_caches_motion_vector( const SLICE *Slice, const int ai_iB_stride, const int ai_iB8_stride, 
							   short aio_tiRef_cache_l0 [ RESTRICT ], short aio_tiMv_cache_l0[ RESTRICT ][2], 
							   const unsigned char *SliceTable, const RESIDU *CurrResidu, 
							   short ai_tiMv[ RESTRICT ], const short *ai_tiRef,  const int ai_iX, const int ai_iY)
{


	int    top_type = 0;
    int    topleft_type;
    int    topright_type;
    int    left_type;

	const int iCurrMbAddr = ai_iX + Slice -> mb_stride * ai_iY;
	const int top_xy = iCurrMbAddr - Slice -> mb_stride;
    const int topleft_xy = top_xy - 1 ;
    const int topright_xy = (top_xy >= 0)? top_xy + 1:-1 ;
    const int left_xy = iCurrMbAddr - 1;

	//Updating their availability according to the slice
	if ( top_xy >= 0 )  {
		top_type = SliceTable [top_xy] == Slice -> slice_num ? CurrResidu [- Slice -> mb_stride] . MbType : 0 ;

		if ( topleft_xy >= 0 && ai_iX != 0 && SliceTable [topleft_xy] == Slice -> slice_num){
			topleft_type = CurrResidu [- Slice -> mb_stride - 1] . MbType;
		}else{
			topleft_type = 0;
			aio_tiRef_cache_l0[3] = PART_NOT_AVAILABLE;
		}


		if ( topright_xy >= 0 && (ai_iX != Slice -> mb_stride - 1) && SliceTable [topright_xy] == Slice -> slice_num){
			topright_type = CurrResidu [- Slice -> mb_stride + 1] . MbType;
		}else{
			topright_type = 0;
			aio_tiRef_cache_l0[8] = PART_NOT_AVAILABLE;
		}

	}else{
		top_type = 0;
		topleft_type = 0;
		topright_type = 0;

		//Update top right and top left reference
		 aio_tiRef_cache_l0[3] = PART_NOT_AVAILABLE;
		 aio_tiRef_cache_l0[8] = PART_NOT_AVAILABLE;
	}


    if ( left_xy >= 0 && ai_iX != 0 && SliceTable [left_xy] == Slice -> slice_num){
		left_type = CurrResidu [-1] . MbType ;
	}else{
		left_type = 0;
		//Update left reference
		FillRefCacheLeftPosition(aio_tiRef_cache_l0, PART_NOT_AVAILABLE, PART_NOT_AVAILABLE);
	}


	//In case of a skipped or a P macroblock
    if(IS_P(top_type)){
        FillMvCacheTopPosition(aio_tiMv_cache_l0, &ai_tiMv[-ai_iB_stride]);
		FillRefCacheTopPosition(aio_tiRef_cache_l0, ai_tiRef [- ai_iB8_stride], ai_tiRef[1 - ai_iB8_stride]);
    }
	else if (!top_type){
        FillRefCacheTopPosition(aio_tiRef_cache_l0, PART_NOT_AVAILABLE, PART_NOT_AVAILABLE);
    }

	//In case of a skipped or a P macroblock
    if(IS_P(left_type)){
       FillMvCacheLeftPosition(aio_tiMv_cache_l0, &ai_tiMv[-2], ai_iB_stride);
	   FillRefCacheLeftPosition(aio_tiRef_cache_l0, ai_tiRef[-1], ai_tiRef[-1 + ai_iB8_stride]);
    }


	//In case of a skipped or a P macroblock
    if(IS_P(topleft_type ))	{
        aio_tiMv_cache_l0 [3][0] = ai_tiMv[-2 - ai_iB_stride];
		aio_tiMv_cache_l0 [3][1] = ai_tiMv[-1 - ai_iB_stride];
		aio_tiRef_cache_l0[3] = ai_tiRef[-1 - ai_iB8_stride];
    }

	//In case of a skipped or a P macroblock
    if(IS_P(topright_type ))  {
		aio_tiMv_cache_l0 [8][0] = ai_tiMv[8 - ai_iB_stride];
		aio_tiMv_cache_l0 [8][1] = ai_tiMv[9 - ai_iB_stride];
        aio_tiRef_cache_l0[8] =  ai_tiRef[2 - ai_iB8_stride];
    }

    aio_tiRef_cache_l0[14] = aio_tiRef_cache_l0[15 + 1] = aio_tiRef_cache_l0[23 + 1] =
	aio_tiRef_cache_l0[30] = aio_tiRef_cache_l0[31 + 1] = PART_NOT_AVAILABLE;
}
