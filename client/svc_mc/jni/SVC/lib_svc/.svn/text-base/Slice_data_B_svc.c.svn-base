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



#include <stdio.h>


//AVC Files
#include "main_data.h"
#include "init_data.h"
#include "decode_mb_I.h"
#include "neighbourhood.h"
#include "SliceGroupMap.h"



//SVC Files
#include "svc_type.h"
#include "I_BL.h"
#include "P_BL.h"
#include "B_BL.h"
#include "Mb_I_svc.h"
#include "svc_data.h"
#include "Coordonates.h"
#include "WriteBackSVC.h"
#include "fill_caches_svc.h"
#include "ResidualProcessing.h"
#include "InterLayerPrediction.h"





/**
Decode B frames using SVC standard.
*/
void Decode_B_svc ( const int size_Mb, SPS *ai_pstSps, PPS * ai_pstPps, SLICE *Slice, const NAL * Nal, unsigned char *SliceTable, 
				   DATA *TabBlock, RESIDU *AllResidu, const MAIN_STRUCT_PF *vectors, LIST_MMO *RefPicListL0,	
				   LIST_MMO *RefPicListL1, LIST_MMO *Current_pic, const W_TABLES *Quantif, SVC_VECTORS *svc_vectors, short *px, 
				   short *py, short *Upsampling_tmp, short *xk16, short *xp16, short *yk16, short *yp16, short *aio_tiMv_l0, 
				   short *aio_tiMv_l1, short *aio_tiRef_l0, short *aio_tiRef_l1, unsigned char *DpbLuma, 
				   unsigned char *DpbCb, unsigned char *DpbCr, short *Luma_Residu, short *Cb_Residu, short *Cr_Residu)
{


	if (!Nal -> NalToSkip){
		__declspec(align(64)) short  ref_cache_l0 [48];
		__declspec(align(64)) short  ref_cache_l1 [48];
		__declspec(align(64)) short  mv_cache_l0 [48][2];
		__declspec(align(64)) short  mv_cache_l1 [48][2];
		short  x_mbPartIdx, y_mbPartIdx;
		short  x_base, y_base, mbBaseAddr;

		int pic_parameter_id = Nal -> pic_parameter_id[Nal -> LayerId];
		const PPS * Pps = &ai_pstPps[pic_parameter_id];
		const SPS *Sps = &ai_pstSps[Pps -> seq_parameter_set_id + 16];

		const short PicSizeInMbs = Sps -> PicSizeInMbs ;
		const short PicWiInMbs = Sps -> pic_width_in_mbs;
		const short PicWidthInMbs = Sps -> pic_width_in_mbs + 2;
		const short PicHeightInMbs = Sps -> pic_height_in_map_units + 2; 
		const short PicWidthInPix = (PicWidthInMbs << 4);
		const short offset_L = (PicWidthInMbs << 8) + 16;
		const short offset_C = (PicWidthInMbs << 6) + 8;  

		unsigned char *Y = &DpbLuma[offset_L + Current_pic -> MemoryAddress];
		unsigned char *U = &DpbCb[offset_C + (Current_pic -> MemoryAddress >> 2)];
		unsigned char *V = &DpbCr[offset_C + (Current_pic -> MemoryAddress >> 2)];
		unsigned char *BaseY = &DpbLuma[(Nal -> BaseMemoryAddress) + 16 + ((Nal -> base_Width + 32) << 4)];
		unsigned char *BaseU = &DpbCb[(Nal -> BaseMemoryAddress >> 2) + 8 + ((Nal -> base_Width + 32) << 2)];
		unsigned char *BaseV = &DpbCr[(Nal -> BaseMemoryAddress >> 2) + 8 + ((Nal -> base_Width + 32) << 2)];

		int MvMemoryAddress = Nal -> BaseMvMemoryAddress; 

		RESIDU *CurrResidu = &AllResidu[Nal -> LayerId * size_Mb];
		RESIDU *BaseResidu = &AllResidu[Nal -> BaseLayerId * size_Mb];
		REFPART RefPartitionL0[32];

		//Parameters of the current slice
		short *mv_l0 = &aio_tiMv_l0[Current_pic -> MvMemoryAddress];
		short *mv_l1 = &aio_tiMv_l1[Current_pic -> MvMemoryAddress];
		short *ref_l0 = &aio_tiRef_l0[Current_pic -> MvMemoryAddress >> 1];
		short *ref_l1 = &aio_tiRef_l1[Current_pic -> MvMemoryAddress >> 1];

		//Parameters of the colocated slice in the base layer
		short *mv_base_l0 = &aio_tiMv_l0[MvMemoryAddress];
		short *mv_base_l1 = &aio_tiMv_l1[MvMemoryAddress];
		short *ref_base_l0 = &aio_tiRef_l0[MvMemoryAddress >> 1];
		short *ref_base_l1 = &aio_tiRef_l1[MvMemoryAddress >> 1];

		short *Residu_Luma = &Luma_Residu[(size_Mb << 8) * Nal -> LayerId]; 
		short *Residu_Cb = &Cb_Residu[(size_Mb << 6) * Nal -> LayerId];
		short *Residu_Cr = &Cr_Residu[(size_Mb << 6) * Nal -> LayerId];

		short iCurrMbAddr = Slice -> first_mb_in_slice ;

		short iMb_x;
		short iMb_y;

		//initialize macroblock position
		GetMbPosition(iCurrMbAddr, PicWiInMbs, &iMb_x, &iMb_y);


		GetBasePrediction(Nal, Slice, ai_pstPps, ai_pstSps, Sps, CurrResidu, BaseResidu, size_Mb, 
			iCurrMbAddr, SliceTable, px, py, xk16, xp16, yk16, yp16, Upsampling_tmp, 
			BaseY, BaseU, BaseV, Luma_Residu, Cb_Residu, Cr_Residu);

		for (; iCurrMbAddr < PicSizeInMbs ;) {
			init_ref_cache(ref_cache_l0);
			init_ref_cache(ref_cache_l1);
			init_mv_cache(mv_cache_l0);
			init_mv_cache(mv_cache_l1);


			// Search the base layer prediction.
			mbBaseAddr = GetBasePositionB(Sps -> direct_8x8_inference_flag, iMb_x << 4,iMb_y << 4, &x_base, &y_base, 
				&x_mbPartIdx, &y_mbPartIdx, Nal, RefPartitionL0, &TabBlock [iCurrMbAddr], &CurrResidu[iCurrMbAddr], 
				BaseResidu, mv_base_l0, ref_base_l0,  mv_base_l1, ref_base_l1, mv_cache_l0, mv_cache_l1 );

			// I_PCM || INTRA_4x4 || INTRA_16x16 || I_BL
			if ( IS_I_SVC(CurrResidu [iCurrMbAddr] . MbType)) {
				//Get neighbourhood
				GetNeighbour(iCurrMbAddr, iMb_x, PicWiInMbs, PicSizeInMbs, SliceTable, CurrResidu, Pps -> constrained_intra_pred_flag);

				//Call I decoding process
				MbISVCDecoding(Sps, Pps, Nal, Quantif, vectors -> baseline_vectors, &CurrResidu [iCurrMbAddr], BaseResidu, 
					&TabBlock [iCurrMbAddr], PicWidthInPix, iMb_x, iMb_y,
					&Y[((iMb_x + (iMb_y * PicWidthInPix)) << 4)], 
					&U[((iMb_x + (iMb_y * PicWidthInPix >> 1)) << 3)], 
					&V[((iMb_x + (iMb_y * PicWidthInPix >> 1)) << 3)], BaseY, BaseU, BaseV);
			}
			else	{
				//Bring back motion vectors and reference indexes needed.
				fill_caches_motion_vector_B_full_ref(Slice, Nal -> b_stride, Nal -> b8_stride, 
					ref_cache_l0, ref_cache_l1, mv_cache_l0, mv_cache_l1, SliceTable, CurrResidu, 
					&mv_l0[iMb_x * 8 + iMb_y * (Nal -> b_stride << 2)], &mv_l1[iMb_x * 8 + iMb_y * (Nal -> b_stride << 2)], 
					&ref_l0[iMb_x * 4 + iMb_y * (Nal -> b_stride << 1)], &ref_l1[iMb_x * 4 + iMb_y * (Nal -> b_stride << 1)], iCurrMbAddr, iMb_x);

				//In case of an BL macroblock
				if (IS_BL(CurrResidu [iCurrMbAddr] . MbType)){ 
					Decode_B_BL(Pps, Sps, Nal, &CurrResidu[iCurrMbAddr], BaseResidu, vectors -> baseline_vectors, Quantif, RefPicListL0, 
						RefPicListL1, iMb_x, iMb_y, x_base, y_base, x_mbPartIdx, y_mbPartIdx, PicWidthInMbs << 4, RefPartitionL0, 
						ref_cache_l0, ref_cache_l1, mv_cache_l0, mv_cache_l1, mv_base_l0, mv_base_l1, ref_base_l0, ref_base_l1, 
						DpbLuma, DpbCb, DpbCr, &Y[((iMb_x + ( iMb_y * PicWidthInPix)) << 4)], 
						&U[((iMb_x + (iMb_y * PicWidthInMbs << 3))<< 3)], &V[((iMb_x + (iMb_y * PicWidthInMbs << 3))<< 3)], 
						&Residu_Luma[((iMb_x + ( iMb_y * PicWidthInPix)) << 4)]	, &Residu_Cb[((iMb_x + (iMb_y * PicWidthInMbs << 3))<< 3)], 
						&Residu_Cr[((iMb_x + (iMb_y * PicWidthInMbs << 3))<< 3)]);

				}else{
					//For B tiMb_ype macroblock
					svc_vectors -> B_vectors [CurrResidu [iCurrMbAddr] . Mode](mv_cache_l0, mv_cache_l1, ref_cache_l0, ref_cache_l1, 
						&CurrResidu [iCurrMbAddr], &TabBlock [iCurrMbAddr], svc_vectors -> B_sub_vectors, 
						&DpbLuma[offset_L], &DpbCb[offset_C], &DpbCr[offset_C], 
						&Y[((iMb_x + ( iMb_y * PicWidthInPix)) << 4)], &U[((iMb_x + (iMb_y * PicWidthInMbs << 3))<< 3)], 
						&V[((iMb_x + (iMb_y * PicWidthInMbs << 3))<< 3)], iMb_x << 4 , iMb_y << 4, PicWidthInMbs << 4, 
						PicHeightInMbs << 4, Nal, RefPicListL0, RefPicListL1, mv_base_l0, mv_base_l1, 
						ref_base_l0, ref_base_l1, vectors -> baseline_vectors, RefPartitionL0);


					// Add CurrResidual to the prediction
					AddResidual(Nal, Pps, &CurrResidu[iCurrMbAddr], &BaseResidu[mbBaseAddr], PicWidthInMbs << 4, Quantif, 
						&Y[((iMb_x + (iMb_y * PicWidthInPix)) << 4)], &U[((iMb_x + (iMb_y * PicWidthInMbs << 3))<< 3)], 
						&V[((iMb_x + (iMb_y * PicWidthInMbs << 3))<< 3)], &Residu_Luma[((iMb_x + (iMb_y * PicWidthInPix)) << 4)], 
						&Residu_Cb[((iMb_x + (iMb_y * PicWidthInMbs << 3))<< 3)], &Residu_Cr[((iMb_x + (iMb_y * PicWidthInMbs << 3))<< 3)]);
				}
			}

			//Recording of the motion vector
			write_back_motion_full_ref(Nal -> b_stride, Nal -> b8_stride, &mv_l0[iMb_x * 8 + iMb_y * (Nal -> b_stride << 2)], mv_cache_l0, 
				&ref_l0[iMb_x * 4 + iMb_y * (Nal -> b_stride << 1)], ref_cache_l0 );
			write_back_motion_full_ref(Nal -> b_stride, Nal -> b8_stride, &mv_l1[iMb_x * 8 + iMb_y * (Nal -> b_stride << 2)], mv_cache_l1, 
				&ref_l1[iMb_x * 4 + iMb_y * (Nal -> b_stride << 1)], ref_cache_l1 );

			//Updating the macroblock address
			iCurrMbAddr = Next_MbAddress(iCurrMbAddr, PicSizeInMbs,PicWiInMbs, SliceTable, &iMb_x, &iMb_y);
			if ( SliceTable [iCurrMbAddr] == 255 ) {
				iCurrMbAddr = PicSizeInMbs ;
			}
		}
	}
}

