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
#include "data.h"
#include "init_data.h"
#include "main_data.h"
#include "decode_mb_I.h"
#include "decode_mb_P.h"
#include "fill_caches.h"
#include "SliceGroupMap.h"
#include "neighbourhood.h"
#include "fill_caches_svc.h"
#include "residual_chroma.h"
#include "fill_caches_cabac.h"
#include "interpolation_main.h"
#include "interpolation_main_8x8.h"


//SVC Files
#include "blk4x4.h"
#include "BL_residual.h"
#include "WriteBackSVC.h"
#include "Slice_data_avc.h"
#include "ResidualProcessing.h"




void Decode_B_avc ( SPS *ai_stSps, PPS *Pps, SLICE *ai_stSlice, unsigned char *ai_tSliceTable, 
				   RESIDU *picture_residu, MAIN_STRUCT_PF *main_vectors, LIST_MMO *ai_pRefPicListL0, 
				   LIST_MMO *ai_pRefPicListL1, LIST_MMO *Current_pic, const W_TABLES *Quantif, 
				   const NAL *Nal, short Mv_l0[], short Mv_l1[], short ref_l0[], short ref_l1[], 
				   unsigned char *aio_tDpb_luma, unsigned char *aio_tDpb_Cb, unsigned char *aio_tDpb_Cr, 
				   short *Residu_Luma, short *Residu_Cb, short *Residu_Cr)
{

	if(!Nal -> NalToSkip){
		__declspec(align(64)) short ref_cache_l0 [48];
		__declspec(align(64)) short ref_cache_l1 [48];
		__declspec(align(64)) short mv_cache_l0 [48][2];
		__declspec(align(64)) short mv_cache_l1 [48][2];

		__declspec(align(64)) unsigned char image_l0[256];
		__declspec(align(64)) unsigned char image_l1[256];
		__declspec(align(64)) unsigned char image_Cb_l0[64];
		__declspec(align(64)) unsigned char image_Cb_l1[64];
		__declspec(align(64)) unsigned char image_Cr_l0[64];
		__declspec(align(64)) unsigned char image_Cr_l1[64];


		const short PicSizeInMbs = (short)ai_stSps -> PicSizeInMbs ;
		const short PicWiInMbs = (short)ai_stSps -> pic_width_in_mbs;
		const short PicWidthInMbs = (short)ai_stSps -> pic_width_in_mbs + 2;
		const short PicWidthInPix = (short)PicWidthInMbs << 4 ;
		const short PicHeightInPix =  (short)(ai_stSps -> pic_height_in_map_units + 2) << 4;

		short *aio_tMv_l0 = &Mv_l0[Current_pic -> MvMemoryAddress];
		short *aio_tMv_l1 = &Mv_l1[Current_pic -> MvMemoryAddress];
		short *aio_tref_l0 = &ref_l0[Current_pic -> MvMemoryAddress >> 1];
		short *aio_tref_l1 = &ref_l1[Current_pic -> MvMemoryAddress >> 1];

		const short offset_L = (PicWidthInMbs << 8) + 16;
		const short offset_C = (PicWidthInMbs << 6) + 8;  

		unsigned char *Y = &aio_tDpb_luma[offset_L + Current_pic -> MemoryAddress];
		unsigned char *U = &aio_tDpb_Cb[offset_C + (Current_pic -> MemoryAddress >> 2)];
		unsigned char *V = &aio_tDpb_Cr[offset_C + (Current_pic -> MemoryAddress >> 2)];

		short iCurrMbAddr = ai_stSlice -> first_mb_in_slice ;

		short iMb_x;
		short iMb_y;

		//initialize macroblock position
		GetMbPosition(iCurrMbAddr, PicWiInMbs, &iMb_x, &iMb_y);


		aio_tDpb_luma += offset_L;
		aio_tDpb_Cb += offset_C;
		aio_tDpb_Cr += offset_C; 

		//Loop on all macroblocks
		for ( ; iCurrMbAddr < PicSizeInMbs ;) {

			if (  IS_I(picture_residu [iCurrMbAddr] . MbType)) {
				//Initalization of the neighbourhood
				GetNeighbour(iCurrMbAddr, iMb_x, PicWiInMbs, PicSizeInMbs, ai_tSliceTable, picture_residu, Pps -> constrained_intra_pred_flag);

				decode_MB_I(&Y[((iMb_x + (iMb_y * PicWidthInPix)) << 4) ], &U[((iMb_x + (iMb_y * PicWidthInPix >> 1)) << 3)], 
					&V[((iMb_x + (iMb_y * PicWidthInPix >> 1)) << 3)], Pps, &picture_residu[iCurrMbAddr], 
					PicWidthInPix, Quantif, main_vectors -> baseline_vectors);

				ResetMbResidu(&Residu_Luma[((iMb_x + (iMb_y * PicWidthInMbs << 4))<< 4)], 
					&Residu_Cb[((iMb_x + (iMb_y * PicWidthInMbs << 3))<< 3)], 
					&Residu_Cr[((iMb_x + (iMb_y * PicWidthInMbs << 3))<< 3)], PicWidthInMbs << 4);
			} else {

				if (Nal -> PicToDisplay){
					if( picture_residu [iCurrMbAddr] . Mode == 0){
						write_back_full_ref( ai_stSlice -> b_stride, ai_stSlice -> b8_stride, 
							&aio_tMv_l0[iMb_x * 8 + iMb_y * (ai_stSlice -> b_stride << 2)],  mv_cache_l0, 
							&aio_tref_l0[iMb_x * 4 + iMb_y * (ai_stSlice -> b_stride << 1)], ref_cache_l0);

						write_back_full_ref( ai_stSlice -> b_stride, ai_stSlice -> b8_stride, 
							&aio_tMv_l1[iMb_x * 8 + iMb_y * (ai_stSlice -> b_stride << 2)],  mv_cache_l1, 
							&aio_tref_l1[iMb_x * 4 + iMb_y * (ai_stSlice -> b_stride << 1)], ref_cache_l1);

						//Recovery of  the prediction
						sample_interpolation_main(aio_tDpb_luma, aio_tDpb_Cb, aio_tDpb_Cr, image_l0, image_l1, image_Cb_l0, image_Cb_l1, 
							image_Cr_l0, image_Cr_l1, mv_cache_l0, mv_cache_l1, ref_cache_l0, ref_cache_l1, iMb_x << 4, iMb_y << 4, 
							PicWidthInPix, PicHeightInPix , ai_pRefPicListL0, ai_pRefPicListL1, main_vectors -> baseline_vectors -> Ptr_interpol_4x4);

						set_image_4x4(&Y[((iMb_x + (iMb_y * PicWidthInPix)) << 4)], &U[((iMb_x + (iMb_y * PicWidthInPix >> 1)) << 3)],
							&V[((iMb_x + (iMb_y * PicWidthInPix >> 1)) << 3)], image_l0, image_l1, image_Cb_l0, image_Cb_l1, 
							image_Cr_l0, image_Cr_l1, ref_cache_l0, ref_cache_l1, PicWidthInPix);
					}else{
						write_back_full_ref_8x8(ai_stSlice -> b_stride, ai_stSlice -> b8_stride, 
							&aio_tMv_l0[iMb_x * 8 + iMb_y * (ai_stSlice -> b_stride << 2)],  mv_cache_l0, 
							&aio_tref_l0[iMb_x * 4 + iMb_y * (ai_stSlice -> b_stride << 1)], ref_cache_l0);

						write_back_full_ref_8x8(ai_stSlice -> b_stride, ai_stSlice -> b8_stride, 
							&aio_tMv_l1[iMb_x * 8 + iMb_y * (ai_stSlice -> b_stride << 2)],  mv_cache_l1, 
							&aio_tref_l1[iMb_x * 4 + iMb_y * (ai_stSlice -> b_stride << 1)], ref_cache_l1);


						SampleInterpolationMain8x8(aio_tDpb_luma, aio_tDpb_Cb,aio_tDpb_Cr, image_l0, image_Cb_l0, image_Cr_l0, mv_cache_l0, ref_cache_l0, 
							iMb_x << 4 , iMb_y << 4, PicWidthInMbs << 4, PicHeightInPix, ai_pRefPicListL0, main_vectors -> baseline_vectors -> Ptr_interpol_8x8);
						SampleInterpolationMain8x8(aio_tDpb_luma, aio_tDpb_Cb,aio_tDpb_Cr, image_l1, image_Cb_l1, image_Cr_l1, mv_cache_l1, ref_cache_l1, 
							iMb_x << 4 , iMb_y << 4, PicWidthInMbs << 4, PicHeightInPix, ai_pRefPicListL1, main_vectors -> baseline_vectors -> Ptr_interpol_8x8);

						set_image_8x8(&Y[((iMb_x + (iMb_y * PicWidthInPix)) << 4) ], &U[((iMb_x + (iMb_y * PicWidthInPix >> 1)) << 3)], 
							&V[((iMb_x + (iMb_y * PicWidthInPix >> 1)) << 3)], image_l0, image_l1, image_Cb_l0, image_Cb_l1, 
							image_Cr_l0, image_Cr_l1, ref_cache_l0, ref_cache_l1, PicWidthInPix);
					}



					ComputeResidual(Pps, &picture_residu [iCurrMbAddr], PicWidthInPix, 
						&Y[((iMb_x + ( iMb_y * PicWidthInPix)) << 4)], 
						&U[((iMb_x + ( iMb_y * PicWidthInPix >> 1)) << 3)], 
						&V[((iMb_x + ( iMb_y * PicWidthInPix >> 1)) << 3)], Quantif);

				}else if ( Nal -> PicToDecode){
					//Calculate the residual
					ComputeResidualImage(Pps, &picture_residu [iCurrMbAddr], PicWidthInPix, 
						&Residu_Luma[((iMb_x + (iMb_y * PicWidthInMbs << 4))<< 4)],
						&Residu_Cb[((iMb_x + (iMb_y * PicWidthInMbs << 3))<< 3)], 
						&Residu_Cr[((iMb_x + (iMb_y * PicWidthInMbs << 3))<< 3)], Quantif);

					update_blk4x4Res(&Residu_Luma[((iMb_x + (iMb_y * PicWidthInMbs << 4)) << 4)], &picture_residu [iCurrMbAddr], PicWidthInMbs << 4);
				}else{
					ResetMbResidu(&Residu_Luma[((iMb_x + (iMb_y * PicWidthInMbs << 4)) << 4)], 
						&Residu_Cb[((iMb_x + (iMb_y * PicWidthInMbs << 3)) << 3)], 
						&Residu_Cr[((iMb_x + (iMb_y * PicWidthInMbs << 3)) << 3)], PicWidthInMbs << 4);
				}
			}

			//Updating the macroblock address
			iCurrMbAddr = Next_MbAddress(iCurrMbAddr, PicSizeInMbs, PicWiInMbs, ai_tSliceTable, &iMb_x, &iMb_y);
			if ( ai_tSliceTable [iCurrMbAddr] == 255 ) 	{
				iCurrMbAddr = PicSizeInMbs ;
			}
		}
	}
}
