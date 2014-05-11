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
#include "decode_mb_P.h"




//SVC Files
#include "svc_type.h"
#include "BL.h"
#include "I_BL.h"
#include "P_BL.h"
#include "blk4x4.h"
#include "svc_data.h"
#include "Mb_P_svc.h"
#include "mb_padding.h"
#include "BL_residual.h"
#include "residual_4x4.h"
#include "residual_8x8.h"
#include "residual_chroma.h"
#include "svc_neighbourhood.h"
#include "motion_vector_svc.h"
#include "ResidualProcessing.h"
#include "mb_upsample_filter.h"
#include "CoeffLevelPrediction.h"
#include "InterLayerPrediction.h"






/**
This functions decodes an P macroblock based on a lower layer.
*/
void Decode_P_BL(const PPS * Pps, const SPS * Sps, const NAL *Nal, RESIDU *CurrResidu, RESIDU *BaseResidu, 
				 const STRUCT_PF *baseline_vector, const W_TABLES *Quantif, LIST_MMO *ai_pstRefPicListL0, 
				 short x, short y, short x_base, short y_base, short x_mbPartIdx, short y_mbPartIdx, short PicWidthInPix, 
				 REFPART *RefPartition, short *ref_cache_l0,  short mv_cache_l0 [][2], short *mv, short *ref_base, 
				 unsigned char *DpbLuma, unsigned char *DpbCb, unsigned char *DpbCr, 
				 unsigned char Y[ ], unsigned char U[ ], unsigned char V[ ], 
				 short CurrResidu_Luma[], short CurrResidu_Cb[], short CurrResidu_Cr[])
{



	const short PicHeightInMbs = Sps -> pic_height_in_map_units + 2; 
	const short mbBaseAddr = x_base + y_base * (Nal -> base_Width >> 4);




	if(!Nal -> non_diadic){
		//Inter prediction in diadic mode
		//So we are sure that 's all macroblock is predicted in inter mode
		int offset_Lum = 16 + 16 * (Nal -> curr_Width + 32);
		int offset_Cb = 8 + 4 * (Nal ->  curr_Width + 32);
		DpbLuma += offset_Lum;
		DpbCb += offset_Cb;
		DpbCr += offset_Cb;


		get_base_P_dyadic(mv_cache_l0, ref_cache_l0, DpbLuma, DpbCb, DpbCr, 
			Y, U, V, x << 4, y << 4, PicWidthInPix, PicHeightInMbs << 4, ai_pstRefPicListL0, 
			mv, ref_base, RefPartition, Nal, baseline_vector -> Ptr_interpol_4x4);

		AddResidual(Nal, Pps, CurrResidu, &BaseResidu[mbBaseAddr], PicWidthInPix, Quantif, Y, U, V, CurrResidu_Luma, CurrResidu_Cb, CurrResidu_Cr);

	}else {
		short Y_Res[256];
		short U_Res[64];
		short V_Res[64];
		unsigned char MbPredY[32*32];
		unsigned char MbPredU[256];
		unsigned char MbPredV[256];


		//Inter prediction in diadic mode
		//The macroblock  can be inter and intra predicted.
		int cX = 8; 
		int cY = 8; 

		int offset_Lum = 16 + 16 * (Nal -> curr_Width + 32);
		int offset_Cb = 8 + 4 * (Nal ->  curr_Width + 32);
		int BaseoffsetLum = 16 + 16 * (Nal ->  base_Width + 32);
		int BaseOffsetCb = 8 + 4 * (Nal ->  base_Width + 32);

		short one_I = (RefPartition[0] . RefIdc == -2) || (RefPartition[2] . RefIdc == -2) || (RefPartition[8] . RefIdc == -2) || (RefPartition[10] . RefIdc == -2);

		//Calculate the residual
		ComputeResidualImage(Pps, CurrResidu, 16, Y_Res, U_Res, V_Res, Quantif);


		if( one_I){ 
			DecodeIPart(Nal, Pps, Sps, RefPartition, BaseResidu, PicWidthInPix, mbBaseAddr, x << 4, y << 4, 
				x_base, y_base, x_mbPartIdx, y_mbPartIdx, &DpbLuma[Nal -> BaseMemoryAddress + BaseoffsetLum], 
				&DpbCb[(Nal -> BaseMemoryAddress >> 2) + BaseOffsetCb], 
				&DpbCr[(Nal -> BaseMemoryAddress >> 2) + BaseOffsetCb], 
				Y, U, V, MbPredY, MbPredU, MbPredV);
		}

		DpbLuma += offset_Lum;
		DpbCb += offset_Cb;
		DpbCr += offset_Cb;

		// Get inter interpolation
		get_base_P_non_dyadic(mv_cache_l0, ref_cache_l0	, DpbLuma, DpbCb, DpbCr, 
			Y, U, V, x << 4, y << 4, PicWidthInPix, PicHeightInMbs << 4, ai_pstRefPicListL0, ref_base, 
			Nal, baseline_vector -> Ptr_interpol_4x4, RefPartition);


		//First block 8x8
		GetNonDiadicSample(CurrResidu -> ResidualPredictionFlag, RefPartition[0] . RefIdc, 8, 8, 
			PicWidthInPix, Y, MbPredY, CurrResidu_Luma, Y_Res);
		GetNonDiadicSample(CurrResidu -> ResidualPredictionFlag, RefPartition[0] . RefIdc, 4, 4, 
			PicWidthInPix >> 1, U, MbPredU, CurrResidu_Cb, U_Res);
		GetNonDiadicSample(CurrResidu -> ResidualPredictionFlag, RefPartition[0] . RefIdc, 4, 4, 
			PicWidthInPix >> 1, V, MbPredV, CurrResidu_Cr, V_Res);

		//Second block 8x8
		GetNonDiadicSample(CurrResidu -> ResidualPredictionFlag, RefPartition[2] . RefIdc, 8, 8, 
			PicWidthInPix, &Y[cX], &MbPredY[cX], &CurrResidu_Luma[cX], &Y_Res[cX]);
		GetNonDiadicSample(CurrResidu -> ResidualPredictionFlag, RefPartition[2] . RefIdc, 4, 4, 
			PicWidthInPix >> 1, &U[cX >> 1], &MbPredU[cX >> 1], &CurrResidu_Cb[cX >> 1], &U_Res[cX >> 1]);
		GetNonDiadicSample(CurrResidu -> ResidualPredictionFlag, RefPartition[2] . RefIdc, 4, 4, 
			PicWidthInPix >> 1, &V[cX >> 1], &MbPredV[cX >> 1], &CurrResidu_Cr[cX >> 1], &V_Res[cX >> 1]);

		//Third block 8x8
		GetNonDiadicSample(CurrResidu -> ResidualPredictionFlag, RefPartition[8] . RefIdc, 8, 8, 
			PicWidthInPix, &Y[cY * PicWidthInPix], &MbPredY[cY << 4], &CurrResidu_Luma[cY * PicWidthInPix], &Y_Res[cY << 4]);
		GetNonDiadicSample(CurrResidu -> ResidualPredictionFlag, RefPartition[8] . RefIdc, 4, 4, 
			PicWidthInPix >> 1, &U[(cY >> 1) * (PicWidthInPix >> 1)], &MbPredU[cY << 2], &CurrResidu_Cb[(cY >> 1) * (PicWidthInPix >> 1)], &U_Res[cY << 2]);
		GetNonDiadicSample(CurrResidu -> ResidualPredictionFlag, RefPartition[8] . RefIdc, 4, 4, 
			PicWidthInPix >> 1, &V[(cY >> 1) * (PicWidthInPix >> 1)], &MbPredV[cY << 2], &CurrResidu_Cr[(cY >> 1) * (PicWidthInPix >> 1)], &V_Res[cY << 2]);


		//Fourth block 8x8
		GetNonDiadicSample(CurrResidu -> ResidualPredictionFlag, RefPartition[10] . RefIdc, 8, 8, 
			PicWidthInPix, &Y[cX + cY * PicWidthInPix], &MbPredY[cX + (cY << 4)], &CurrResidu_Luma[cX + cY * PicWidthInPix], &Y_Res[cX + (cY << 4)]);
		GetNonDiadicSample(CurrResidu -> ResidualPredictionFlag, RefPartition[10] . RefIdc, 4, 4, 
			PicWidthInPix >> 1, &U[(cX >> 1) + (cY >> 1) * (PicWidthInPix >> 1)], &MbPredU[(cX >> 1) + (cY << 2)], 
			&CurrResidu_Cb[(cX >> 1) + (cY >> 1) * (PicWidthInPix >> 1)], &U_Res[(cX >> 1) + (cY << 2)]);
		GetNonDiadicSample(CurrResidu -> ResidualPredictionFlag, RefPartition[10] . RefIdc, 4, 4, 
			PicWidthInPix >> 1, &V[(cX >> 1) + (cY >> 1) * (PicWidthInPix >> 1)], &MbPredV[(cX >> 1) + (cY << 2)], 
			&CurrResidu_Cr[(cX >> 1) + (cY >> 1) * (PicWidthInPix >> 1)], &V_Res[(cX >> 1) + (cY << 2)]);

		//Update the bl4x4 flag in order to know if the 4x4 block are intra coded.
		update_blk4x4Res(Y_Res, CurrResidu, 16);
	}
}
