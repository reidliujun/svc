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
//AVC Files
#include "cabac.h"
#include "type.h"
#include "symbol.h"
#include "write_back.h"
#include "fill_caches.h"
#include "fill_caches_cabac.h"
 





/**
This function permits to update the neighbourhood of the current macroblock in a slice.
This function stock in table the data calculated for the macroblock which belong to the neighbourhood of the current macroblock. 

@param slice The Slice strucutre.
@param Tab_Block Contains all parameters of each macroblock of the current picture.
@param neigh Structure which contains all the data for the CABAC neighbouring.
@param macro Specifies the availability of the neighbouring macroblock.
@param SliceTable A table which resume in which Slice each macroblock belongs to.
@param mb_xy Number of the current macroblock.
@param qp_delta Delta qp of the previous macroblock.
@param residu Structure which contains all the residu data of the current macroblock..
*/

void fill_caches_cabac ( SLICE *slice, CABAC_NEIGH *Neigh, unsigned char *SliceTable, int mb_xy, int qp_delta, RESIDU *residu )
{



	int	top_type;
    int left_type = 0;
    
	//Updating the adress of the macroblock which belong to the neighbourhood.
    int top_xy = mb_xy - slice -> mb_stride;
    int left_xy = mb_xy - 1 ;
	

	if ( top_xy >= 0 && SliceTable [top_xy] == slice -> slice_num) {
        top_type =  residu [- slice -> mb_stride] . MbType;
		Neigh -> intra_chroma_pred_mode_top = residu [- slice -> mb_stride] . IntraChromaPredMode;
	}else{
		top_type = 0;
	}


	if ( left_xy  >= 0 ) {
		left_type = SliceTable [left_xy] == slice -> slice_num ? residu [-1] . MbType : 0 ;
		Neigh -> intra_chroma_pred_mode_left = residu [-1] . IntraChromaPredMode;
		Neigh -> last_cbp = residu [-1] . Cbp;
		Neigh -> last_qp_diff = qp_delta;
	}else{
		Neigh -> last_cbp = 0;
		Neigh -> last_qp_diff = 0;
	}

	if ( mb_xy > 0 && !(mb_xy % (slice -> mb_stride ))){
		left_type =  0;
	}	
	

	Neigh -> intra_chroma_pred_mode_left = residu [-1] . IntraChromaPredMode;
	Neigh -> intra_chroma_pred_mode_top = residu [- slice -> mb_stride] . IntraChromaPredMode;


    // top_cbp
    if(top_type) {
        Neigh -> Cbp_b = residu[- slice -> mb_stride] . Cbp;
	} else if (IS_I(residu -> MbType )){
		Neigh -> Cbp_b = 0x1c0;
	}else{
       Neigh -> Cbp_b = 0;
    }

    // left_cbp
    if (left_type) {
        Neigh -> Cbp_a = residu[-1] . Cbp & 0xffA;
    } else if (IS_I(residu -> MbType )){
		 Neigh -> Cbp_a = 0x1c0;
	}else {
        Neigh -> Cbp_a = 0;
    }
}







/**
This function permits to update the neighbourhood of the current macroblock.
This function stock in table the motion vector for the macroblock which belong to the neighbourhood of the current macroblock. 

@param slice The Slice strucutre.
@param b_stride The width of sample in the ai_tiMv table.
@param b8_stride The width of sample in the ai_tiRef table.
@param ref_cache_l0 A cache table where the reference is stocked for each 4x4 block of the current macroblock.
@param ref_cache_l1 A cache table where the reference is stocked for each 4x4 block of the current macroblock.
@param mv_cache_l0 A cache table where the motion vector are stocked for each 4x4 block of the current macroblock.
@param mv_cache_l1 A cache table where the motion vector are stocked for each 4x4 block of the current macroblock.
@param SliceTable A table which resume in which Slice each macroblock belongs to.
@param Tab_Block Contains all parameters of each macroblock of the current picture. 
@param mv_l0 A table where each motion vector calculated is stocked for each macroblock decoded.
@param mv_l1 A table where each motion vector calculated is stocked for each macroblock decoded.
@param ref_l0 A table where each reference mode calculated is stocked for each macroblock decoded.
@param ref_l1 A table where each reference mode calculated is stocked for each macroblock decoded.
@param iCurrMbAddr Address of the current macroblock.
@param ai_iMb_x The x position of the macroblock in the picture.
*/
void fill_caches_motion_vector_B(SLICE *slice, int b_stride, int b8_stride, short ref_cache_l0 [], short ref_cache_l1 [], 
								 short mv_cache_l0[][2], short  mv_cache_l1[][2], unsigned char *SliceTable, RESIDU *CurrResidu, 
								 short *mv_l0, short mv_l1[], short ref_l0[], short ref_l1[], int iCurrMbAddr, int ai_iX)

{

	int    top_type = 0 ;
    int    topleft_type;
    int    topright_type;
    int    left_type;

	
	//Updating the adress of the macroblock which belong to the neighbourhood.
    int top_xy = iCurrMbAddr - slice -> mb_stride ;
    int topleft_xy = top_xy - 1 ;
	int topright_xy = top_xy >= 0? top_xy + 1: -1 ;
    int left_xy = iCurrMbAddr - 1 ;


	//Updating their availability according to the slice.
	//Updating their availability according to the slice.
	if ( top_xy >= 0 )    {
		top_type = SliceTable [top_xy] == slice -> slice_num ? CurrResidu [- slice -> mb_stride] . MbType : 0 ;

		if ( topleft_xy >= 0 && ai_iX != 0 && SliceTable [topleft_xy] == slice -> slice_num){
				topleft_type = CurrResidu [- slice -> mb_stride - 1] . MbType;
		}else{
			topleft_type = 0;
			ref_cache_l0[12 - 1 - 8] = ref_cache_l1[12 - 1 - 8] = PART_NOT_AVAILABLE;
		}

		
		if ( topright_xy >= 0 && (ai_iX != slice -> mb_stride - 1) && SliceTable [topright_xy] == slice -> slice_num){
				topright_type = CurrResidu [- slice -> mb_stride + 1] . MbType;
		}else{
			topright_type = 0;
			ref_cache_l0[12 + 4 - 8] = ref_cache_l1[12 + 4 - 8] = PART_NOT_AVAILABLE;
		}

	}else{
		top_type = 0;
		topleft_type = 0;
		topright_type = 0;

		//Update top right and top left reference
		 ref_cache_l0[12 - 1 - 8] = ref_cache_l1[12 - 1 - 8] = PART_NOT_AVAILABLE;
		 ref_cache_l0[12 + 4 - 8] = ref_cache_l1[12 + 4 - 8] = PART_NOT_AVAILABLE;
	}


   if ( left_xy >= 0 && ai_iX != 0 && SliceTable [left_xy] == slice -> slice_num){
			left_type = CurrResidu [-1] . MbType;
	}else{
		left_type = 0;

		//Update left reference
		FillRefCacheLeftPosition(ref_cache_l0, PART_NOT_AVAILABLE, PART_NOT_AVAILABLE);
		FillRefCacheLeftPosition(ref_cache_l1, PART_NOT_AVAILABLE, PART_NOT_AVAILABLE);
	}


            
	//In case of a skipped or a P macroblock
	if( top_type && !IS_I(top_type)){
		FillMvCacheTopPosition(mv_cache_l0, &mv_l0[-b_stride]);
		FillMvCacheTopPosition(mv_cache_l1, &mv_l1[-b_stride]);
		FillRefCacheTopPosition(ref_cache_l0, ref_l0 [- b8_stride], ref_l0[1 - b8_stride]);
		FillRefCacheTopPosition(ref_cache_l1, ref_l1 [- b8_stride], ref_l1[1 - b8_stride]);

	}else if (!top_type){
		FillRefCacheTopPosition(ref_cache_l0, PART_NOT_AVAILABLE, PART_NOT_AVAILABLE);
		FillRefCacheTopPosition(ref_cache_l1, PART_NOT_AVAILABLE, PART_NOT_AVAILABLE); ;
	}

	//In case of a skipped or a P macroblock
	if(left_type && !IS_I(left_type)){
		FillMvCacheLeftPosition(mv_cache_l0, &mv_l0[-2], b_stride);
		FillMvCacheLeftPosition(mv_cache_l1, &mv_l1[-2], b_stride);
		FillRefCacheLeftPosition (ref_cache_l0, ref_l0[-1], ref_l0[-1 + b8_stride]);
		FillRefCacheLeftPosition (ref_cache_l1, ref_l1[-1], ref_l1[-1 + b8_stride]);
	}


	//In case of a skipped or a P macroblock
	if(topleft_type && !IS_I(topleft_type)){
		mv_cache_l0 [12 - 1 - 8][0] = mv_l0[-2 - b_stride];
		mv_cache_l0 [12 - 1 - 8][1] = mv_l0[-1 - b_stride];
		mv_cache_l1 [12 - 1 - 8][0] = mv_l1[-2 - b_stride];
		mv_cache_l1 [12 - 1 - 8][1] = mv_l1[-1 - b_stride];
		ref_cache_l0[12 - 1 - 8] =  ref_l0[-1 - b8_stride];
		ref_cache_l1[12 - 1 - 8] =  ref_l1[-1 - b8_stride];
	}

	//In case of a skipped or a P macroblock
	if(topright_type && !IS_I(topright_type)){
		mv_cache_l0 [12 + 4 - 8][0] = mv_l0[8 - b_stride];
		mv_cache_l0 [12 + 4 - 8][1] = mv_l0[9 - b_stride];
		mv_cache_l1 [12 + 4 - 8][0] = mv_l1[8 - b_stride];
		mv_cache_l1 [12 + 4 - 8][1] = mv_l1[9 - b_stride];
		ref_cache_l0[12 + 4 - 8]=  ref_l0[2 - b8_stride];
		ref_cache_l1[12 + 4 - 8]=  ref_l1[2 - b8_stride];

	}

	ref_cache_l0[15+1] = ref_cache_l0[23+1] = ref_cache_l0[31+1] = ref_cache_l0[14] = ref_cache_l0[30] = 
	ref_cache_l1[15+1] = ref_cache_l1[23+1] = ref_cache_l1[31+1] = ref_cache_l1[14] = ref_cache_l1[30] = PART_NOT_AVAILABLE;
}






/**
This function permits to initialize the table used for the CABAC when a skip macroblock is detected.


@param MvdL0 A cache table where the motion vector are stocked for each 4x4 block of the current macroblock.
@param ref A cache table where the reference is stocked for each 4x4 block of the current macroblock.
@param b_stride The width of sample in the ai_tiMv table.
@param b8_stride The width of sample in the ai_tiRef table.

*/
void init_mb_skip_mv_ref(short MvdL0[], short ref[],int b_stride, int b8_stride){

	int y;
	
	for( y = 0; y < 4; y++, MvdL0 += b_stride -7){
		*(MvdL0++) = 0;
		*(MvdL0++) = 0;
		*(MvdL0++) = 0;
		*(MvdL0++) = 0;
		*(MvdL0++) = 0;
		*(MvdL0++) = 0;
		*(MvdL0++) = 0;
		*(MvdL0) = 0;
	}

	ref[0] = -1;
	ref[1] = -1;
	ref[b8_stride] = -1;
	ref[1 + b8_stride] = -1;
}






/**
This function enables to write back the reference index and the motio nvector of a macroblock.

@param ai_iB_stride The width of sample in the ao_tiMv table.
@param ai_iB8_stride The width of sample in the ao_tiRef table.
@param ai_iMb_xy Adress of the current macroblock.
@param ai_tiMv_l0 A table where each motion vector calculated is stocked for each macroblock decoded.
@param ai_tiMv_l1 A table where each motion vector calculated is stocked for each macroblock decoded.
@param ai_tiMv_cache_l0 A cache table where the motion vector are stocked for each 4x4 block of the current macroblock. 
@param ai_tiMv_cache_l1 A cache table where the motion vector are stocked for each 4x4 block of the current macroblock. 
@param ai_tiRef_l0 A table where each reference mode calculated is stocked for each macroblock decoded.
@param ai_tiRef_l1 A table where each reference mode calculated is stocked for each macroblock decoded.
@param aio_tiRef_cache_l0 A cache table where the reference is stocked for each 4x4 block of the current macroblock.
@param aio_tiRef_cache_l1 A cache table where the reference is stocked for each 4x4 block of the current macroblock.
@param mb_stride Stride i macroblock of the picture.

*/
void write_back_motion_cache_B( const int ai_iB_stride, const int ai_iB8_stride, const int ai_iMb_xy
				, short ai_tiMv_l0[ ], short ai_tiMv_l1[ ], short ai_tiMv_cache_l0[ ][2]
				, short ai_tiMv_cache_l1[ ][2], short ai_tiRef_l0[ ], short ai_tiRef_l1[ ], short aio_tiRef_cache_l0[ ]
				, short aio_tiRef_cache_l1[ ],int mb_stride)
{
	
	//Updating the adress of the macroblock which belong to the neighbourhood.
    const int top_xy = ai_iMb_xy - mb_stride  ;
    const int topleft_xy = top_xy - 1 ;
	const int topright_xy = top_xy >= 0 ? top_xy + 1:-1 ;
    const int left_xy = ai_iMb_xy - 1 ;

	WriteBackMotionCacheCurrPosition(ai_iB_stride, ai_iB8_stride, ai_tiMv_l0, ai_tiMv_cache_l0, ai_tiRef_l0, aio_tiRef_cache_l0);
	WriteBackMotionCacheCurrPosition(ai_iB_stride, ai_iB8_stride, ai_tiMv_l1, ai_tiMv_cache_l1, ai_tiRef_l1, aio_tiRef_cache_l1);

	//In case of a skipped or a P macroblock
    if(top_xy >= 0)   {
		FillMvCacheTopPosition(ai_tiMv_cache_l0, &ai_tiMv_l0[- ai_iB_stride]);
		FillMvCacheTopPosition(ai_tiMv_cache_l1, &ai_tiMv_l1[- ai_iB_stride]);
		FillRefCacheTopPosition(aio_tiRef_cache_l0, ai_tiRef_l0 [- ai_iB8_stride], ai_tiRef_l0[1 - ai_iB8_stride]);
		FillRefCacheTopPosition(aio_tiRef_cache_l1, ai_tiRef_l1 [- ai_iB8_stride], ai_tiRef_l1[1 - ai_iB8_stride]);
	}
    else    {
		FillRefCacheTopPosition(aio_tiRef_cache_l0, PART_NOT_AVAILABLE, PART_NOT_AVAILABLE);;
		FillRefCacheTopPosition(aio_tiRef_cache_l1, PART_NOT_AVAILABLE, PART_NOT_AVAILABLE);;
    }

	//In case of a skipped or a P macroblock
    if(left_xy >= 0)    {
		FillMvCacheLeftPosition(ai_tiMv_cache_l0, &ai_tiMv_l0[-2], ai_iB_stride);
		FillMvCacheLeftPosition(ai_tiMv_cache_l1, &ai_tiMv_l1[-2], ai_iB_stride);
		FillRefCacheLeftPosition(aio_tiRef_cache_l0, ai_tiRef_l0[-1], ai_tiRef_l0[-1 + ai_iB8_stride]);
		FillRefCacheLeftPosition(aio_tiRef_cache_l1, ai_tiRef_l1[-1], ai_tiRef_l1[-1 + ai_iB8_stride]);
    
	}
    else    {
		FillRefCacheLeftPosition(aio_tiRef_cache_l1, PART_NOT_AVAILABLE, PART_NOT_AVAILABLE);
		FillRefCacheLeftPosition(aio_tiRef_cache_l1, PART_NOT_AVAILABLE, PART_NOT_AVAILABLE);
    }


	//In case of a skipped or a P macroblock
    if(topleft_xy >= 0){
        ai_tiMv_cache_l0 [12 - 1 - 8][0] = ai_tiMv_l0[-2 - ai_iB_stride];
		ai_tiMv_cache_l0 [12 - 1 - 8][1] = ai_tiMv_l0[-1 - ai_iB_stride];

		ai_tiMv_cache_l1 [12 - 1 - 8][0] = ai_tiMv_l1[-2 - ai_iB_stride];
		ai_tiMv_cache_l1 [12 - 1 - 8][1] = ai_tiMv_l1[-1 - ai_iB_stride];

		aio_tiRef_cache_l0[12 - 1 - 8] = ai_tiRef_l0[-1 - ai_iB8_stride];
		aio_tiRef_cache_l1[12 - 1 - 8] = ai_tiRef_l1[-1 - ai_iB8_stride];
    }
	else     {
        aio_tiRef_cache_l0[12 - 1 - 8] = PART_NOT_AVAILABLE;
		aio_tiRef_cache_l1[12 - 1 - 8] = PART_NOT_AVAILABLE;
    }


	//In case of a skipped or a P macroblock
    if(topright_xy >= 0)    {
        ai_tiMv_cache_l0 [12 + 4 - 8][0] = ai_tiMv_l0[8 - ai_iB_stride];
		ai_tiMv_cache_l0 [12 + 4 - 8][1] = ai_tiMv_l0[9 - ai_iB_stride];

		ai_tiMv_cache_l1 [12 + 4 - 8][0] = ai_tiMv_l1[8 - ai_iB_stride];
		ai_tiMv_cache_l1 [12 + 4 - 8][1] = ai_tiMv_l1[9 - ai_iB_stride];

        aio_tiRef_cache_l0[12 + 4 - 8] =  ai_tiRef_l0[2 - ai_iB8_stride];
        aio_tiRef_cache_l1[12 + 4 - 8] =  ai_tiRef_l1[2 - ai_iB8_stride];
    }
    else     {
        aio_tiRef_cache_l0[12 + 4 - 8] = PART_NOT_AVAILABLE;
		aio_tiRef_cache_l1[12 + 4 - 8] = PART_NOT_AVAILABLE;
    }
  
}






/**
This function permits to write back the motion vector of the current macroblock.

@param ai_iB_stride The width of sample in the ao_tiMv table.
@param ai_iB8_stride The width of sample in the ao_tiRef table.
@param ai_tiMv_l0 A table where each motion vector calculated is stocked for each macroblock decoded.
@param ai_tiMv_l1 A table where each motion vector calculated is stocked for each macroblock decoded.
@param ai_tiMv_cache_l0 A cache table where the motion vector are stocked for each 4x4 block of the current macroblock. 
@param ai_tiMv_cache_l1 A cache table where the motion vector are stocked for each 4x4 block of the current macroblock. 
@param ai_tiRef_l0 A table where each reference mode calculated is stocked for each macroblock decoded.
@param ai_tiRef_l1 A table where each reference mode calculated is stocked for each macroblock decoded.
@param aio_tiRef_cache_l0 A cache table where the reference is stocked for each 4x4 block of the current macroblock.
@param aio_tiRef_cache_l1 A cache table where the reference is stocked for each 4x4 block of the current macroblock.

*/
void write_back_main_interpolation(const int ai_iB_stride, const int ai_iB8_stride
				, short ai_tiMv_l0[ ], short ai_tiMv_l1[ ], short  ai_tiMv_cache_l0[ ][2], short  ai_tiMv_cache_l1[ ][2]
				, short ai_tiRef_l0[ ], short ai_tiRef_l1[ ], short aio_tiRef_cache_l0[ ], short aio_tiRef_cache_l1[ ]){


	WriteBackMotionCacheCurrPosition(ai_iB_stride, ai_iB8_stride, ai_tiMv_l0, ai_tiMv_cache_l0, ai_tiRef_l0, aio_tiRef_cache_l0);
	WriteBackMotionCacheCurrPosition(ai_iB_stride, ai_iB8_stride, ai_tiMv_l1, ai_tiMv_cache_l1, ai_tiRef_l1, aio_tiRef_cache_l1);
}






/**
This function permits to write back the motion vector of the current macroblock.

@param ai_iB_stride The width of sample in the ao_tiMv table.
@param ai_iB8_stride The width of sample in the ao_tiRef table.
@param ai_tiMv_l0 A table where each motion vector calculated is stocked for each macroblock decoded.
@param ai_tiMv_l1 A table where each motion vector calculated is stocked for each macroblock decoded.
@param ai_tiMv_cache_l0 A cache table where the motion vector are stocked for each 4x4 block of the current macroblock. 
@param ai_tiMv_cache_l1 A cache table where the motion vector are stocked for each 4x4 block of the current macroblock. 
@param ai_tiRef_l0 A table where each reference mode calculated is stocked for each macroblock decoded.
@param ai_tiRef_l1 A table where each reference mode calculated is stocked for each macroblock decoded.
@param aio_tiRef_cache_l0 A cache table where the reference is stocked for each 4x4 block of the current macroblock.
@param aio_tiRef_cache_l1 A cache table where the reference is stocked for each 4x4 block of the current macroblock.

*/
void write_back_main_interpolation_8x8(const int ai_iB_stride, const int ai_iB8_stride
			, short ai_tiMv_l0[ ], short ai_tiMv_l1[ ], short ai_tiMv_cache_l0[ ][2], short ai_tiMv_cache_l1[ ][2]
			, short ai_tiRef_l0[ ], short ai_tiRef_l1[ ], short aio_tiRef_cache_l0[ ], short aio_tiRef_cache_l1[ ]){



	//Write back the macroblock's parameters

	ai_tiMv_cache_l0[12][0] = ai_tiMv_l0[0];
	ai_tiMv_cache_l0[12][1] = ai_tiMv_l0[1];
	ai_tiMv_cache_l0[14][0] = ai_tiMv_l0[4];
	ai_tiMv_cache_l0[14][1] = ai_tiMv_l0[5];
		
	ai_tiMv_cache_l0[28][0] = ai_tiMv_l0[2 * ai_iB_stride];
	ai_tiMv_cache_l0[28][1] = ai_tiMv_l0[2 * ai_iB_stride + 1];
	ai_tiMv_cache_l0[30][0] = ai_tiMv_l0[2 * ai_iB_stride + 4];
	ai_tiMv_cache_l0[30][1] = ai_tiMv_l0[2 * ai_iB_stride + 5];

	ai_tiMv_cache_l1[12][0] = ai_tiMv_l1[0];
	ai_tiMv_cache_l1[12][1] = ai_tiMv_l1[1];
	ai_tiMv_cache_l1[14][0] = ai_tiMv_l1[4];
	ai_tiMv_cache_l1[14][1] = ai_tiMv_l1[5];
		
	ai_tiMv_cache_l1[28][0] = ai_tiMv_l1[2 * ai_iB_stride];
	ai_tiMv_cache_l1[28][1] = ai_tiMv_l1[2 * ai_iB_stride + 1];
	ai_tiMv_cache_l1[30][0] = ai_tiMv_l1[2 * ai_iB_stride + 4];
	ai_tiMv_cache_l1[30][1] = ai_tiMv_l1[2 * ai_iB_stride + 5];


	FillRefCacheCurrPosition(aio_tiRef_cache_l0, ai_tiRef_l0);
	FillRefCacheCurrPosition(&aio_tiRef_cache_l0[16], &ai_tiRef_l0[ai_iB8_stride]);
	FillRefCacheCurrPosition(aio_tiRef_cache_l1, ai_tiRef_l1);
	FillRefCacheCurrPosition(&aio_tiRef_cache_l1[16], &ai_tiRef_l1[ai_iB8_stride]);
}
