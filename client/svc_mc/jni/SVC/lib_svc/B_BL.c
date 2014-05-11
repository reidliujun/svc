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
#include "symbol.h"
#include "clip.h"
#include "decode_mb_P.h"
#include "residual_chroma.h"




//SVC Files
#include "svc_type.h"
#include "BL.h"
#include "I_BL.h"
#include "B_BL.h"
#include "P_BL.h"
#include "svc_data.h"
#include "blk4x4.h"
#include "Mb_P_svc.h"
#include "mb_padding.h"
#include "BL_residual.h"
#include "fill_caches_svc.h"
#include "motion_vector_svc.h"
#include "svc_neighbourhood.h"
#include "mb_upsample_filter.h"
#include "ResidualProcessing.h"



/**
Decodes B macroblock using base mode flag.
*/
void Decode_B_BL(const PPS * Pps, const SPS * Sps, const NAL *Nal, RESIDU *CurrResidu,  RESIDU *base_residu, const STRUCT_PF *baseline_vector, 
				 const W_TABLES *Quantif, LIST_MMO * ai_pstRefPicListL0, LIST_MMO * ai_pstRefPicListL1, short x, short y, 
				 short x_base, short y_base, short x_mbPartIdx, short y_mbPartIdx,  short PicWidthInPix, REFPART RefPartitionL0[16], 
				 short *ref_cache_l0, short *ref_cache_l1, short mv_cache_l0[][2], short mv_cache_l1[][2], 
				 short *mv_l0, short *mv_l1, short *ref_base_l0, short *ref_base_l1, 
				 unsigned char *DpbLuma, unsigned char *DpbCb, unsigned char *DpbCr, 
				 unsigned char *Y, unsigned char *U, unsigned char *V, short *Residu_Luma, short *Residu_Cb, short *Residu_Cr)
{



	short mbBaseAddr = x_base + y_base * (Nal -> base_Width >> 4);
	const short PicHeightInMbs =  (short) Sps -> pic_height_in_map_units + 2; 

	if ( !Nal -> non_diadic){
		int offset_Lum = 16 + 16 * (Nal -> curr_Width + 32);
		int offset_Cb = 8 + 4 * (Nal ->  curr_Width + 32);

		DpbLuma += offset_Lum;
		DpbCb += offset_Cb;
		DpbCr += offset_Cb;

		get_base_B_mv(mv_cache_l0, mv_cache_l1, ref_cache_l0, ref_cache_l1, 
			DpbLuma, DpbCb, DpbCr, Y, U, V, 
			x << 4, y << 4, PicWidthInPix, PicHeightInMbs << 4, ai_pstRefPicListL0, 
			ai_pstRefPicListL1, RefPartitionL0, mv_l0, mv_l1, ref_base_l0, ref_base_l1, 
			Nal, baseline_vector -> Ptr_interpol_4x4);


		AddResidual(Nal, Pps, CurrResidu, &base_residu[mbBaseAddr], PicWidthInPix, Quantif, Y, U, V, Residu_Luma, Residu_Cb, Residu_Cr);
	}else {
		short Y_Res[256];
		short U_Res[64];
		short V_Res[64];
		unsigned char MbPredY[32*32];
		unsigned char MbPredU[256];
		unsigned char MbPredV[256];

		int cX = 8; 
		int cY = 8; 
		int offset_Lum = 16 + 16 * (Nal -> curr_Width + 32);
		int offset_Cb = 8 + 4 * (Nal ->  curr_Width + 32);
		int BaseoffsetLum = 16 + 16 * (Nal ->  base_Width + 32);
		int BaseOffsetCb = 8 + 4 * (Nal ->  base_Width + 32);


		//Get the type for each 8x8 block 
		short one_I = (RefPartitionL0[0] . RefIdc == -2) || (RefPartitionL0[2] . RefIdc == -2) || (RefPartitionL0[8] . RefIdc == -2) || (RefPartitionL0[10] . RefIdc == -2);

		//Calculate the residual
		ComputeResidualImage(Pps, CurrResidu, 16, Y_Res, U_Res, V_Res, Quantif);

		if( one_I){ 
			DecodeIPart(Nal, Pps, Sps, RefPartitionL0, base_residu, PicWidthInPix, mbBaseAddr, x, y, 
				x_base, y_base, x_mbPartIdx, y_mbPartIdx, &DpbLuma[Nal -> BaseMemoryAddress + BaseoffsetLum], 
				&DpbCb[(Nal -> BaseMemoryAddress >> 2) + BaseOffsetCb], 
				&DpbCr[(Nal -> BaseMemoryAddress >> 2) + BaseOffsetCb], 
				Y, U, V, MbPredY, MbPredU, MbPredV);
		}


		DpbLuma += offset_Lum;
		DpbCb += offset_Cb;
		DpbCr += offset_Cb;

		// Get inter interpolation
		GetBaseBMvNonDiadic(mv_cache_l0, mv_cache_l1, ref_cache_l0, ref_cache_l1, 
			DpbLuma, DpbCb, DpbCr, Y, U, V, 
			x << 4, y << 4, PicWidthInPix, PicHeightInMbs << 4, ai_pstRefPicListL0,
			ai_pstRefPicListL1, RefPartitionL0, ref_base_l0, ref_base_l1, Nal, baseline_vector -> Ptr_interpol_4x4);


		//First block 8x8
		GetNonDiadicSample(CurrResidu -> ResidualPredictionFlag, RefPartitionL0[0] . RefIdc, 8, 8, PicWidthInPix, Y, MbPredY, Residu_Luma, Y_Res);
		GetNonDiadicSample(CurrResidu -> ResidualPredictionFlag, RefPartitionL0[0] . RefIdc, 4, 4, PicWidthInPix >> 1, U, MbPredU, Residu_Cb, U_Res);
		GetNonDiadicSample(CurrResidu -> ResidualPredictionFlag, RefPartitionL0[0] . RefIdc, 4, 4, PicWidthInPix >> 1, V, MbPredV, Residu_Cr, V_Res);

		//Second block 8x8
		GetNonDiadicSample(CurrResidu -> ResidualPredictionFlag, RefPartitionL0[2] . RefIdc, 8, 8, PicWidthInPix, &Y[cX], &MbPredY[cX], &Residu_Luma[cX], &Y_Res[cX]);
		GetNonDiadicSample(CurrResidu -> ResidualPredictionFlag, RefPartitionL0[2] . RefIdc, 4, 4, PicWidthInPix >> 1, &U[cX >> 1], &MbPredU[cX >> 1], &Residu_Cb[cX >> 1], &U_Res[cX >> 1]);
		GetNonDiadicSample(CurrResidu -> ResidualPredictionFlag, RefPartitionL0[2] . RefIdc, 4, 4, PicWidthInPix >> 1, &V[cX >> 1], &MbPredV[cX >> 1], &Residu_Cr[cX >> 1], &V_Res[cX >> 1]);

		//Third block 8x8
		GetNonDiadicSample(CurrResidu -> ResidualPredictionFlag, RefPartitionL0[8] . RefIdc, 8, 8, PicWidthInPix, &Y[cY * PicWidthInPix], &MbPredY[cY << 4], &Residu_Luma[cY * PicWidthInPix], &Y_Res[cY << 4]);
		GetNonDiadicSample(CurrResidu -> ResidualPredictionFlag, RefPartitionL0[8] . RefIdc, 4, 4, PicWidthInPix >> 1, &U[(cY >> 1) * (PicWidthInPix >> 1)], &MbPredU[cY << 2], &Residu_Cb[(cY >> 1) * (PicWidthInPix >> 1)], &U_Res[cY << 2]);
		GetNonDiadicSample(CurrResidu -> ResidualPredictionFlag, RefPartitionL0[8] . RefIdc, 4, 4, PicWidthInPix >> 1, &V[(cY >> 1) * (PicWidthInPix >> 1)], &MbPredV[cY << 2], &Residu_Cr[(cY >> 1) * (PicWidthInPix >> 1)], &V_Res[cY << 2]);


		//Fourth block 8x8
		GetNonDiadicSample(CurrResidu -> ResidualPredictionFlag, RefPartitionL0[10] . RefIdc, 8, 8, PicWidthInPix, &Y[cX + cY * PicWidthInPix], &MbPredY[cX + (cY << 4)], &Residu_Luma[cX + cY * PicWidthInPix], &Y_Res[cX + (cY << 4)]);
		GetNonDiadicSample(CurrResidu -> ResidualPredictionFlag, RefPartitionL0[10] . RefIdc, 4, 4, PicWidthInPix >> 1, &U[(cX >> 1) + (cY >> 1) * (PicWidthInPix >> 1)], &MbPredU[(cX >> 1) + (cY << 2)], &Residu_Cb[(cX >> 1) + (cY >> 1) * (PicWidthInPix >> 1)], &U_Res[(cX >> 1) + (cY << 2)]);
		GetNonDiadicSample(CurrResidu -> ResidualPredictionFlag, RefPartitionL0[10] . RefIdc, 4, 4, PicWidthInPix >> 1, &V[(cX >> 1) + (cY >> 1) * (PicWidthInPix >> 1)], &MbPredV[(cX >> 1) + (cY << 2)], &Residu_Cr[(cX >> 1) + (cY >> 1) * (PicWidthInPix >> 1)], &V_Res[(cX >> 1) + (cY << 2)]);

		//Update the bl4x4 flag in order to know if the 4x4 block are intra coded.
		update_blk4x4Res(Y_Res, CurrResidu, 16);
	}
}

