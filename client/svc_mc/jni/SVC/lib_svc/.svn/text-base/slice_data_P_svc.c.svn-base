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
#include "data.h"
#include "init_data.h"
#include "decode_mb_I.h"
#include "neighbourhood.h"
#include "SliceGroupMap.h"



//SVC Files
#include "svc_type.h"
#include "I_BL.h"
#include "P_BL.h"
#include "Mb_I_svc.h"
#include "svc_data.h"
#include "Coordonates.h"
#include "WriteBackSVC.h"
#include "fill_caches_svc.h"
#include "ResidualProcessing.h"
#include "InterLayerPrediction.h"





/**
Decode P slice using SVC predictions.
*/
void Decode_P_svc ( const int size_Mb, const SPS *ai_pstSps, const PPS *ai_pstPps,const SLICE *Slice, const NAL *Nal, 
				   const unsigned char *SliceTable, DATA *TabBlock, RESIDU *all_residu, 
				   const STRUCT_PF *baseline_vector, LIST_MMO *ai_pstRefPicListL0, LIST_MMO *Current_pic, 
				   const W_TABLES *Quantif, SVC_VECTORS *svc_vectors, short *px, short *py, short *Upsampling_tmp, short *xk16, 
				   short *xp16, short *yk16, short *yp16, short *aio_tiMv, short *aio_tiRef, 
				   unsigned char *DpbLuma, unsigned char *DpbCb, unsigned char *DpbCr, 
				   short *Luma_Residu, short *Cb_Residu, short *Cr_Residu)
{


	if ( !Nal -> NalToSkip){
		REFPART RefPartition[16];
		short x_mbPartIdx, y_mbPartIdx;
		short x_base, y_base, mbBaseAddr;	

		__declspec(align(64)) short ref_cache_l0 [48];
		__declspec(align(64)) short mv_cache_l0 [48][2];

		int pic_parameter_id = Nal -> pic_parameter_id[Nal -> LayerId];
		const PPS * Pps = &ai_pstPps[pic_parameter_id];
		const SPS * Sps = &ai_pstSps[Pps -> seq_parameter_set_id + 16];

		const short PicSizeInMbs = Sps -> PicSizeInMbs ;
		const short PicWiInMbs = Sps -> pic_width_in_mbs;
		const short PicWidthInMbs = Sps -> pic_width_in_mbs + 2;
		const short PicHeightInMbs = Sps -> pic_height_in_map_units + 2; 
		const short PicWidthInPix = (PicWidthInMbs << 4);

		const short offset_L = (PicWidthInMbs << 8) + 16;
		const short offset_C = (PicWidthInMbs << 6) + 8;   

		short *Residu_Luma = &Luma_Residu[(size_Mb << 8) * Nal -> LayerId]; 
		short *Residu_Cb = &Cb_Residu[(size_Mb << 6) * Nal -> LayerId];
		short *Residu_Cr = &Cr_Residu[(size_Mb << 6) * Nal -> LayerId];

		short iCurrMbAddr = Slice -> first_mb_in_slice ;

		RESIDU *CurrResidu = &all_residu[Nal -> LayerId * size_Mb];
		RESIDU *BaseResidu = &all_residu[Nal -> BaseLayerId * size_Mb];

		short *aio_tiMv_l0 = &aio_tiMv [Current_pic -> MvMemoryAddress];
		short *aio_tiRef_l0 = &aio_tiRef [Current_pic -> MvMemoryAddress >> 1];


		unsigned char *Y = &DpbLuma[offset_L + Current_pic -> MemoryAddress];
		unsigned char *U = &DpbCb[offset_C + (Current_pic -> MemoryAddress >> 2)];
		unsigned char *V = &DpbCr[offset_C + (Current_pic -> MemoryAddress >> 2)];
		unsigned char *BaseY = &DpbLuma[(Nal -> BaseMemoryAddress) + 16 + ((Nal -> base_Width + 32) << 4)];
		unsigned char *BaseU = &DpbCb[(Nal -> BaseMemoryAddress >> 2) + 8 + ((Nal -> base_Width + 32) << 2)];
		unsigned char *BaseV = &DpbCr[(Nal -> BaseMemoryAddress >> 2) + 8 + ((Nal -> base_Width + 32) << 2)];



		short *mv;
		short *ref_base;

		short iMb_x;
		short iMb_y;

		//initialize macroblock position
		GetMbPosition(iCurrMbAddr, PicWiInMbs, &iMb_x, &iMb_y);


		//mbl check
		//Not done in B slice 
		if ( Nal -> BaseMemoryAddress != -1){
			mv = &aio_tiMv[Nal -> BaseMvMemoryAddress];
			ref_base = &aio_tiRef[Nal -> BaseMvMemoryAddress >> 1];
		}else {
			//Took the address zéro to initialize the motion vector.
			//We don't know which motions vector are stored at this address.
			mv = &aio_tiMv[0];
			ref_base = &aio_tiRef[0];
		}


		GetBasePrediction(Nal, Slice, ai_pstPps, ai_pstSps, Sps, CurrResidu, BaseResidu, size_Mb, iCurrMbAddr, 
			SliceTable, px, py, xk16, xp16, yk16, yp16, Upsampling_tmp, 
			BaseY, BaseU, BaseV, Luma_Residu, Cb_Residu, Cr_Residu);


		for (; iCurrMbAddr < PicSizeInMbs ;) {
			init_ref_cache(ref_cache_l0);
			init_mv_cache(mv_cache_l0);

			// Search the base layer prediction.
			mbBaseAddr = GetBasePosition(iMb_x << 4, iMb_y << 4, &x_base, &y_base, &x_mbPartIdx, &y_mbPartIdx, Nal, 
				RefPartition, &TabBlock [iCurrMbAddr], &CurrResidu[iCurrMbAddr], BaseResidu, mv, ref_base, mv_cache_l0);

			// I_PCM || INTRA_4x4 || INTRA_16x16 || I_BL
			if ( IS_I_SVC(CurrResidu [iCurrMbAddr] . MbType)) {
				//Get neighbourhood
				GetNeighbour(iCurrMbAddr, iMb_x, PicWiInMbs, PicSizeInMbs, SliceTable, CurrResidu, Pps -> constrained_intra_pred_flag);

				//Call I decoding process
				MbISVCDecoding(Sps, Pps, Nal, Quantif, baseline_vector, &CurrResidu [iCurrMbAddr], BaseResidu, 
					&TabBlock [iCurrMbAddr], PicWidthInPix, iMb_x, iMb_y,
					&Y[((iMb_x + (iMb_y * PicWidthInPix)) << 4)], 
					&U[((iMb_x + (iMb_y * PicWidthInPix >> 1)) << 3)], 
					&V[((iMb_x + (iMb_y * PicWidthInPix >> 1)) << 3)], BaseY, BaseU, BaseV);
			}
			else	{
				//Bring back motion vectors and reference indexes needed.
				fill_caches_full_ref(Slice, Nal -> b_stride, Nal -> b8_stride, ref_cache_l0, mv_cache_l0, 
					SliceTable, CurrResidu, &aio_tiMv_l0[iMb_x * 8 + iMb_y * (Nal -> b_stride << 2)], 
					&aio_tiRef_l0[iMb_x * 4 + iMb_y * (Nal -> b_stride << 1)], iMb_x, iMb_y);



				//In case of an BL macroblock
				if (IS_BL(CurrResidu [iCurrMbAddr] . MbType)){
					Decode_P_BL(Pps, Sps, Nal, &CurrResidu[iCurrMbAddr], BaseResidu, baseline_vector, 
						Quantif, ai_pstRefPicListL0, iMb_x, iMb_y, x_base, y_base, x_mbPartIdx, y_mbPartIdx, 
						PicWidthInMbs << 4, RefPartition, ref_cache_l0, mv_cache_l0, mv, ref_base, 
						DpbLuma, DpbCb, DpbCr, &Y[((iMb_x + ( iMb_y * PicWidthInPix)) << 4)], 
						&U[((iMb_x + ( iMb_y * PicWidthInPix >> 1)) << 3)], &V[((iMb_x + ( iMb_y * PicWidthInPix >> 1)) << 3)], 
						&Residu_Luma[((iMb_x + ( iMb_y * PicWidthInPix)) << 4)], &Residu_Cb[((iMb_x + ( iMb_y * PicWidthInPix >> 1)) << 3)]	, 
						&Residu_Cr[((iMb_x + ( iMb_y * PicWidthInPix >> 1)) << 3)]);
				}

				//For P type macroblock
				else{
					//Calculate the motion vector
					svc_vectors -> P_vectors [CurrResidu [iCurrMbAddr] . Mode](mv_cache_l0, ref_cache_l0, &CurrResidu [iCurrMbAddr], 
						&TabBlock [iCurrMbAddr], svc_vectors -> P_sub_vectors, &DpbLuma[offset_L], 
						&DpbCb[offset_C], &DpbCr[offset_C], &Y[((iMb_x + (iMb_y * PicWidthInPix)) << 4)], 
						&U[((iMb_x + (iMb_y * PicWidthInMbs << 3)) << 3)], &V[((iMb_x + (iMb_y * PicWidthInMbs << 3)) << 3)], 
						iMb_x << 4 , iMb_y << 4, PicWidthInMbs << 4, PicHeightInMbs << 4, Nal, 
						ai_pstRefPicListL0, mv, ref_base, baseline_vector, RefPartition);


					// Add residual to the prediction
					AddResidual (Nal, Pps, &CurrResidu[iCurrMbAddr], &BaseResidu[mbBaseAddr], PicWidthInMbs << 4, Quantif, 
						&Y[((iMb_x + (iMb_y * PicWidthInPix)) << 4)], &U[((iMb_x + (iMb_y * PicWidthInMbs << 3)) << 3)], 
						&V[((iMb_x + (iMb_y * PicWidthInMbs << 3)) << 3)], &Residu_Luma[((iMb_x + (iMb_y * PicWidthInPix)) << 4)], 
						&Residu_Cb[((iMb_x + (iMb_y * PicWidthInMbs << 3)) << 3)], &Residu_Cr[((iMb_x + (iMb_y * PicWidthInMbs << 3)) << 3)]);
				}
			}

			//Recording of the motion vector
			write_back_motion_full_ref(Nal -> b_stride, Nal -> b8_stride, 
				&aio_tiMv_l0[iMb_x * 8 + iMb_y * (Nal -> b_stride << 2)], mv_cache_l0, 
				&aio_tiRef_l0[iMb_x * 4 + iMb_y * (Nal -> b_stride << 1)], ref_cache_l0 );

			//Updating the macroblock address
			iCurrMbAddr = Next_MbAddress(iCurrMbAddr, PicSizeInMbs,PicWiInMbs, SliceTable, &iMb_x, &iMb_y);
			//Used to stop the deconding process when there is slices.
			if ( SliceTable [iCurrMbAddr] == 255 ) {
				iCurrMbAddr = PicSizeInMbs ;
			}
		}
	}
}

