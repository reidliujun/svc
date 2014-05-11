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
#include <time.h>


//AVC Files
#include "data.h"
#include "init_data.h"
#include "decode_mb_I.h"
#include "slice_data_B.h"
#include "fill_caches_cabac.h"
#include "interpolation_main_8x8.h"






/**
This function allows to build of a B picture. 

@param image_ref_luma Table of reference frames.
@param image_ref_Cb Table of reference frames.
@param image_ref_Cr Table of reference frames.
@param image Table of current frame.
@param image_Cb Table of current frame.
@param image_Cr Table of current frame.
@param slice The slice structure contains all information used to decode the next slice.
@param sps The sps structure.
@param pps The pps structure.
@param Tab_block Contains all parameters of each macroblock of the current picture.
@param SliceTable Specifies in which slice belongs each macroblock of the picture.
@param picture_mb_type Specifies the type of each macroblock of the picture.
@param MbToSliceGroupMap Specifies in which slice belongs each macroblock of the picture.
@param mv A cache table where the motion vector are stocked for each 4x4 block of each macroblock.
@param ref A cache table where the reference is stocked for each 4x4 block of each macroblock.
@param pic_intra_dc Contains the DC luma level of a decoded macroblock.
@param picture_luma Contains the AC luma level of a 4x4 decoded macroblock.
@param picture_chroma_dc Contains the AC chroma level of a decoded macroblock.
@param picture_chroma_ac Contains the DC chroma level of a decoded macroblock.
@param intra4x4_pred_mode_cache A cache table where  prediction mode is stocked for each 4x4 block of the current macroblock.
@param intra4x4_pred_mode A table where each prediction mode calculated is stocked for each macroblock decoded.
@param non_zero_count Specifies the coeff_token of each block of the picture.
@param predict_4x4 Function pointer for intra 4x4.
@param predict16x16 Function pointer for intra 16x16.
@param predict_chr Function pointer for chrominance.
@param decode Function pointer for predicted frame.
@param decode_sub Function pointer for sub-macroblockk of predicted frame.
*/
void slice_data_B (const SPS *ai_stSps, const PPS *ai_stPps, SLICE *ai_stSlice, const unsigned char *ai_tSliceTable, 
				   RESIDU *picture_residu, MAIN_STRUCT_PF *main_vectors, LIST_MMO *ai_pRefPicListL0, 
				   LIST_MMO *ai_pRefPicListL1, short *aio_tMv_l0, short *aio_tMv_l1, short *aio_tref_l0, 
				   short *aio_tref_l1, unsigned char *aio_tDpb_luma, unsigned char *aio_tDpb_Cb, unsigned char *aio_tDpb_Cr,
				   unsigned char *Image, unsigned char *Image_Cb, unsigned char *Image_Cr, const W_TABLES *quantif)
{


	__declspec(align(64)) unsigned char image_l0[256];
	__declspec(align(64)) unsigned char image_l1[256];
	__declspec(align(64)) unsigned char image_Cb_l0[64];
	__declspec(align(64)) unsigned char image_Cb_l1[64];
	__declspec(align(64)) unsigned char image_Cr_l0[64];
	__declspec(align(64)) unsigned char image_Cr_l1[64];
	__declspec(align(64)) short ref_cache_l0 [48];
	__declspec(align(64)) short ref_cache_l1 [48];
	__declspec(align(64)) short mv_cache_l0 [48][2];
	__declspec(align(64)) short mv_cache_l1 [48][2];

	const short PicSizeInMbs = ai_stSps -> PicSizeInMbs ;
	const short PicWiInMbs = ai_stSps -> pic_width_in_mbs;
	const short PicWidthInMbs = ai_stSps -> pic_width_in_mbs + 2;
	const short PicWidthInPix = PicWidthInMbs << 4 ;
	const short PicHeightInPix =  (ai_stSps -> pic_height_in_map_units + 2) << 4;


	const short offset_L = (PicWidthInMbs << 8) + 16;
	const short offset_C = (PicWidthInMbs << 6) + 8;  	
	short iCurrMbAddr = ai_stSlice -> first_mb_in_slice;

	short iMb_x;
	short iMb_y;
	
	//initialize macroblock position
	GetMbPosition(iCurrMbAddr, PicWiInMbs, &iMb_x, &iMb_y);


	aio_tDpb_luma += offset_L;
	aio_tDpb_Cb += offset_C;
	aio_tDpb_Cr += offset_C;

	//Loop on all macroblocks
	for ( ; iCurrMbAddr < PicSizeInMbs ;) {



		unsigned char *Y = &Image[offset_L + ((iMb_x + ( iMb_y * PicWidthInPix)) << 4)];
		unsigned char *U = &Image_Cb[offset_C + ((iMb_x + ( iMb_y * PicWidthInPix >> 1)) << 3)];
		unsigned char *V = &Image_Cr[offset_C + ((iMb_x + ( iMb_y * PicWidthInPix >> 1)) << 3)];


		if (  IS_I(picture_residu [iCurrMbAddr] . MbType)) {
			//Initalization of the neighbourhood;
			GetNeighbour(iCurrMbAddr, iMb_x, PicWiInMbs, PicSizeInMbs, ai_tSliceTable, picture_residu, ai_stPps -> constrained_intra_pred_flag);
			decode_MB_I(Y, U, V	, ai_stPps, &picture_residu[iCurrMbAddr], PicWidthInPix, quantif, main_vectors -> baseline_vectors);

		} else {

		
			if( picture_residu [iCurrMbAddr] . Mode == 0){
				write_back_main_interpolation( ai_stSlice -> b_stride, ai_stSlice -> b8_stride,
					&aio_tMv_l0[iMb_x * 8 + iMb_y * (ai_stSlice -> b_stride << 2)], 
					&aio_tMv_l1[iMb_x * 8 + iMb_y * (ai_stSlice -> b_stride << 2)],
					mv_cache_l0,  mv_cache_l1, &aio_tref_l0[iMb_x * 2 + iMb_y * (ai_stSlice -> b8_stride << 1)], 
					&aio_tref_l1[iMb_x * 2 + iMb_y * (ai_stSlice -> b8_stride << 1)], ref_cache_l0, ref_cache_l1);

				//Recovery of  the prediction
				sample_interpolation_main(aio_tDpb_luma, aio_tDpb_Cb, aio_tDpb_Cr, 
					image_l0, image_l1, image_Cb_l0, image_Cb_l1, image_Cr_l0, image_Cr_l1, 
					mv_cache_l0, mv_cache_l1, ref_cache_l0, ref_cache_l1, iMb_x << 4, iMb_y << 4, 
					PicWidthInPix, PicHeightInPix, ai_pRefPicListL0, ai_pRefPicListL1, 
					main_vectors -> baseline_vectors -> Ptr_interpol_4x4);

				set_image_4x4(Y, U, V, image_l0, image_l1, image_Cb_l0, image_Cb_l1, image_Cr_l0, image_Cr_l1, ref_cache_l0, ref_cache_l1, PicWidthInPix);
			}else{
				write_back_main_interpolation_8x8( ai_stSlice -> b_stride, ai_stSlice -> b8_stride, 
					&aio_tMv_l0[iMb_x * 8 + iMb_y * (ai_stSlice -> b_stride << 2)], 
					&aio_tMv_l1[iMb_x * 8 + iMb_y * (ai_stSlice -> b_stride << 2)], mv_cache_l0,  mv_cache_l1, 
					&aio_tref_l0[iMb_x * 2 + iMb_y * ( ai_stSlice -> b8_stride << 1)], 
					&aio_tref_l1[iMb_x * 2 + iMb_y * ( ai_stSlice -> b8_stride << 1)], ref_cache_l0, ref_cache_l1);


				SampleInterpolationMain8x8(aio_tDpb_luma, aio_tDpb_Cb,aio_tDpb_Cr, image_l0, image_Cb_l0, image_Cr_l0, 
					mv_cache_l0, ref_cache_l0, iMb_x << 4 , iMb_y << 4, PicWidthInMbs << 4, PicHeightInPix, 
					ai_pRefPicListL0, main_vectors -> baseline_vectors -> Ptr_interpol_8x8);
				SampleInterpolationMain8x8(aio_tDpb_luma, aio_tDpb_Cb,aio_tDpb_Cr, image_l1, image_Cb_l1, image_Cr_l1,
					mv_cache_l1, ref_cache_l1, iMb_x << 4 , iMb_y << 4, PicWidthInMbs << 4, PicHeightInPix, 
					ai_pRefPicListL1, main_vectors -> baseline_vectors -> Ptr_interpol_8x8);

				set_image_8x8(Y, U, V, image_l0, image_l1, image_Cb_l0, image_Cb_l1, image_Cr_l0, image_Cr_l1, 
					ref_cache_l0, ref_cache_l1, PicWidthInPix);
			}
			

			

			//Add the residual
			ComputeResidual(ai_stPps, &picture_residu [iCurrMbAddr], PicWidthInPix, Y, U, V, quantif);
		}

		//Updating the macroblock address
		iCurrMbAddr = Next_MbAddress(iCurrMbAddr, PicSizeInMbs, PicWiInMbs, ai_tSliceTable, &iMb_x, &iMb_y);
		if ( ai_tSliceTable [iCurrMbAddr] == 255 ) {
            iCurrMbAddr = PicSizeInMbs ;
        }
	}
}





void non_IDR_main ( SPS *ai_stSps, PPS *ai_stPps , SLICE *ai_stSlice,  unsigned char *ai_tSliceTable, 
				   RESIDU *picture_residu, MAIN_STRUCT_PF *main_vector, LIST_MMO *ai_pRefPicListL0, LIST_MMO *ai_pRefPicListL1, 
				   LIST_MMO *Current_pic, const W_TABLES *quantif, short *aio_tMv_l0, short *aio_tMv_l1, short *aio_tref_l0, 
				   short *aio_tref_l1, unsigned char *aio_tDpb_luma, unsigned char *aio_tDpb_Cb,unsigned char *aio_tDpb_Cr)
{


	if (ai_stSps -> PicSizeInMbs){ 
		slice_data_B (ai_stSps,ai_stPps, ai_stSlice, ai_tSliceTable,  picture_residu, main_vector, ai_pRefPicListL0, 
			ai_pRefPicListL1, &aio_tMv_l0[Current_pic -> MvMemoryAddress], &aio_tMv_l1[Current_pic -> MvMemoryAddress], 
			&aio_tref_l0[Current_pic -> MvMemoryAddress >> 3], &aio_tref_l1[Current_pic -> MvMemoryAddress >> 3], 
			aio_tDpb_luma, aio_tDpb_Cb, aio_tDpb_Cr	, &aio_tDpb_luma[Current_pic -> MemoryAddress], 
			&aio_tDpb_Cb[Current_pic -> MemoryAddress >> 2], &aio_tDpb_Cr[Current_pic -> MemoryAddress >> 2], quantif);
	}
}
