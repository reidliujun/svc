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

#include "decode_mb_I.h"
#include "decode_mb_P.h"
#include "fill_caches.h"
#include "SliceGroupMap.h"
#include "neighbourhood.h"
#include "interpolation.h"
#include "residual_chroma.h"
#include "fill_caches_svc.h"
#include "interpolation_8x8.h"

//SVC Files
#include "blk4x4.h"
#include "BL_residual.h"
#include "residual_4x4.h"
#include "residual_8x8.h"
#include "WriteBackSVC.h"
#include "Slice_data_avc.h"
#include "ResidualProcessing.h"


/**
This function rebuilds the frame according to datas decoded from the VLD for a P AVC slice.
*/
void Decode_P_avc( const SPS *Sps, const PPS *Pps, const SLICE *ai_pstSlice, const unsigned char *SliceTable, 
				  RESIDU *picture_residu, const STRUCT_PF *ai_struct_pf, LIST_MMO *ai_pstRefPicListL0, 
				  LIST_MMO *Current_pic, const W_TABLES *Quantif, const NAL *Nal, short *aio_tiMv, short *aio_tiRef, 
				  unsigned char *DpbLuma, unsigned char *DpbCb, unsigned char *DpbCr, 
				  short *Residu_Luma, short *Residu_Cb, short *Residu_Cr)
{

	if (!Nal -> NalToSkip){
		__declspec(align(64)) short ref_cache_l0 [48];
		__declspec(align(64)) short mv_cache_l0 [48][2];
		const short PicSizeInMbs = Sps -> PicSizeInMbs ;
		const short PicWiInMbs =  Sps -> pic_width_in_mbs;
		const short PicWidthInMbs = Sps -> pic_width_in_mbs + 2;
		const short PicHeightInMbs = Sps -> pic_height_in_map_units + 2; 
		const short PicWidthInPix = (PicWidthInMbs << 4);
		const short offset_L = (PicWidthInMbs << 8) + 16;
		const short offset_C = (PicWidthInMbs << 6) + 8; 

		unsigned char *Y = &DpbLuma[offset_L + Current_pic -> MemoryAddress];
		unsigned char *U = &DpbCb[offset_C + (Current_pic -> MemoryAddress >> 2)];
		unsigned char *V = &DpbCr[offset_C + (Current_pic -> MemoryAddress >> 2)];

		short iCurrMbAddr = ai_pstSlice -> first_mb_in_slice;

		short iMb_x;
		short iMb_y;

		
		//initialize macroblock position
		GetMbPosition(iCurrMbAddr, PicWiInMbs, &iMb_x, &iMb_y);

		//Loop on all macroblocks
		for ( ; iCurrMbAddr < PicSizeInMbs ;) {

			//Construction process for one macroblock
			// I_PCM || INTRA_4x4 || INTRA_16x16
			if ( IS_I(picture_residu [iCurrMbAddr] . MbType) ) {
				//Initalization of the neighbourhood
				GetNeighbour(iCurrMbAddr, iMb_x, PicWiInMbs, PicSizeInMbs, SliceTable, picture_residu, Pps -> constrained_intra_pred_flag);

				decode_MB_I(&Y[((iMb_x + ( iMb_y * PicWidthInPix)) << 4)], &U[((iMb_x + ( iMb_y * PicWidthInPix >> 1)) << 3)], 
					&V[((iMb_x + ( iMb_y * PicWidthInPix >> 1)) << 3)], Pps, &picture_residu[iCurrMbAddr], PicWidthInPix, Quantif, ai_struct_pf);

				ResetMbResidu(&Residu_Luma[((iMb_x + (iMb_y * PicWidthInMbs << 4)) << 4)], 
					&Residu_Cb[((iMb_x + (iMb_y * PicWidthInMbs << 3)) << 3)], 
					&Residu_Cr[((iMb_x + (iMb_y * PicWidthInMbs << 3)) << 3)], PicWidthInMbs << 4);
			} 
			else {
				//Calculate the motion vector
				if (Nal -> PicToDisplay){
					if( picture_residu [iCurrMbAddr] . Mode == 0){
						write_back_full_ref( ai_pstSlice -> b_stride, ai_pstSlice -> b8_stride, 
							&aio_tiMv[iMb_x * 8 + iMb_y * (ai_pstSlice -> b_stride << 2)],  mv_cache_l0, 
							&aio_tiRef[iMb_x * 4 + iMb_y * (ai_pstSlice -> b_stride << 1)], ref_cache_l0);

						//Recovery of  the prediction
						sample_interpolation(&DpbLuma[offset_L], &DpbCb[offset_C],&DpbCr[offset_C], 
							&Y[((iMb_x + ( iMb_y * PicWidthInPix)) << 4)], &U[((iMb_x + ( iMb_y * PicWidthInPix >> 1)) << 3)], 
							&V[((iMb_x + ( iMb_y * PicWidthInPix >> 1)) << 3)], mv_cache_l0, ref_cache_l0, iMb_x << 4 , iMb_y << 4, 
							PicWidthInMbs << 4, PicHeightInMbs << 4, ai_pstRefPicListL0, ai_struct_pf -> Ptr_interpol_4x4);
					}else{
						write_back_full_ref_8x8(ai_pstSlice -> b_stride, ai_pstSlice -> b8_stride, 
							&aio_tiMv[iMb_x * 8 + iMb_y * (ai_pstSlice -> b_stride << 2)], mv_cache_l0, 
							&aio_tiRef[iMb_x * 4 + iMb_y * (ai_pstSlice -> b_stride << 1)], ref_cache_l0);


						SampleInterpolation8x8(&DpbLuma[offset_L], &DpbCb[offset_C], &DpbCr[offset_C], 
							&Y[((iMb_x + ( iMb_y * PicWidthInPix)) << 4)], &U[((iMb_x + ( iMb_y * PicWidthInPix >> 1)) << 3)], 
							&V[((iMb_x + ( iMb_y * PicWidthInPix >> 1)) << 3)], mv_cache_l0, ref_cache_l0, iMb_x << 4 , iMb_y << 4, 
							PicWidthInMbs << 4, PicHeightInMbs << 4, ai_pstRefPicListL0, ai_struct_pf -> Ptr_interpol_8x8);
					}

				
					//Add of the residual
					ComputeResidual(Pps, &picture_residu [iCurrMbAddr], PicWidthInPix, 
						&Y[((iMb_x + ( iMb_y * PicWidthInPix)) << 4)], 
						&U[((iMb_x + ( iMb_y * PicWidthInPix >> 1)) << 3)], 
						&V[((iMb_x + ( iMb_y * PicWidthInPix >> 1)) << 3)], Quantif);
				}else if (Nal -> PicToDecode){
					ComputeResidualImage(Pps, &picture_residu [iCurrMbAddr], PicWidthInPix, 
						&Residu_Luma[((iMb_x + (iMb_y * PicWidthInMbs << 4)) << 4)],
						&Residu_Cb[((iMb_x + (iMb_y * PicWidthInMbs << 3)) << 3)], 
						&Residu_Cr[((iMb_x + (iMb_y * PicWidthInMbs << 3)) << 3)], Quantif);


				   update_blk4x4Res(&Residu_Luma[((iMb_x + (iMb_y * PicWidthInMbs << 4)) << 4)], &picture_residu [iCurrMbAddr], PicWidthInMbs << 4);
				}else{
					ResetMbResidu(&Residu_Luma[((iMb_x + (iMb_y * PicWidthInMbs << 4)) << 4)], 
						&Residu_Cb[((iMb_x + (iMb_y * PicWidthInMbs << 3)) << 3)], 
						&Residu_Cr[((iMb_x + (iMb_y * PicWidthInMbs << 3)) << 3)], PicWidthInMbs << 4);
				}
			}

			//Updating the macroblock address
			iCurrMbAddr = Next_MbAddress(iCurrMbAddr, PicSizeInMbs, PicWiInMbs, SliceTable, &iMb_x, &iMb_y);
			if ( SliceTable [iCurrMbAddr] == 255 ) {
				iCurrMbAddr = PicSizeInMbs ;
			}
		}
	}
}

