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

#include "string.h"

//AVC Files
#include "type.h"
#include "decode_mb_I.h"


//SVC Files 
#include "I_BL.h"
#include "mb_padding.h"
#include "svc_neighbourhood.h"
#include "mb_upsample_filter.h"
#include "CoeffLevelPrediction.h"



/**
This function allow to treate a I macroblock when the residu is predicted with the base macroblock.
*/
void DecodeITCoeff(RESIDU *CurrResidu, const RESIDU *BaseResidu, const PPS *Pps, const short PicWidthInPix, 
				 const W_TABLES *quantif, const STRUCT_PF * Allpf_struct, 
				 unsigned char *Y, unsigned char *U, unsigned char *V)
{


	//First, we have to rescale the coefficient
	const short DeltaQP = (short) (BaseResidu -> Qp - CurrResidu -> Qp + 54);

	//Use base macroblock to determine current macroblock parameters.
	CurrResidu -> MbType = BaseResidu -> MbType;
	CurrResidu -> Intra16x16PredMode = BaseResidu -> Intra16x16PredMode;
	CurrResidu -> IntraChromaPredMode = BaseResidu -> IntraChromaPredMode;


	//Check with cbp, if there is a residu
	if (BaseResidu -> MbType == INTRA_16x16){
		TCoeffRescaleDCLuma(DeltaQP, BaseResidu -> Intra16x16DCLevel, CurrResidu -> Intra16x16DCLevel);
		TCoeffRescaleACLuma(DeltaQP, BaseResidu -> LumaLevel, CurrResidu -> LumaLevel);
		UpdateLevel(CurrResidu);
	}


	if (BaseResidu -> MbType == INTRA_4x4){
		//In case of an INTRA_4x4, we have to copy intra_4x4 mode because there are not in the current layer
		memcpy(CurrResidu -> Intra16x16DCLevel, BaseResidu -> Intra16x16DCLevel, 16 * sizeof(short));
		if (BaseResidu -> Cbp){
			TCoeffRescaleACLuma(DeltaQP, BaseResidu -> LumaLevel, CurrResidu -> LumaLevel);
			UpdateLumaCbp(CurrResidu, BaseResidu);
		}
	}

	if( BaseResidu -> Cbp > 0x0f){
		short BaseQP = qP_tab[BaseResidu -> Qp + Pps -> chroma_qp_index_offset];
		short CurrQP = qP_tab[CurrResidu -> Qp + Pps -> chroma_qp_index_offset];
		short Delta = BaseQP - CurrQP + 54;
		TCoeffRescaleDCChr(Delta, BaseResidu -> ChromaDCLevel_Cb, CurrResidu -> ChromaDCLevel_Cb);
		TCoeffRescaleDCChr(Delta, BaseResidu -> ChromaDCLevel_Cr, CurrResidu -> ChromaDCLevel_Cr);
		TCoeffRescaleACChr(Delta, BaseResidu -> ChromaACLevel_Cb, CurrResidu -> ChromaACLevel_Cb);
		TCoeffRescaleACChr(Delta, BaseResidu -> ChromaACLevel_Cr, CurrResidu -> ChromaACLevel_Cr);
		UpdateChrCbp(CurrResidu, BaseResidu);
	}



	//Just do now the Intra prediction
	decode_MB_I(Y, U, V, Pps, CurrResidu, PicWidthInPix, quantif, Allpf_struct );
}



/**
Upsample macroblock samples.
*/
void MbIPad(const NAL *Nal, const PPS *Pps, const SPS *Sps, short BaseMbAddr, short x, short y, 
			short x_base, short y_base, short x_mbPartIdx, short y_mbPartIdx, const RESIDU *BaseResidu, 
			unsigned char *BaseY, unsigned char *BaseU, unsigned char *BaseV, 
			unsigned char *Y, unsigned char *U, unsigned char *V)
{

	unsigned char tmp_luma[LumaStride * LumaStride];
	unsigned char tmp_Cb[ChromaStride * ChromaStride];
	unsigned char tmp_Cr[ChromaStride * ChromaStride];

	short AvailMask;
	//Get the reference macroblock neighbourhood
	get_base_neighbourhood(&AvailMask, BaseMbAddr, x_base, Nal -> base_Width >> 4, BaseResidu, 
		(Nal -> base_Height * Nal -> base_Width) >> 8, Pps -> constrained_intra_upsampling_flag);
	x_base = x_base << 4;
	y_base = y_base << 4;


	edges_limit(Nal, AvailMask, x_mbPartIdx, y_mbPartIdx, tmp_luma, &BaseY[x_base + y_base * (Nal -> base_Width + 32)], 
		tmp_Cb, &BaseU[(x_base >> 1) + (y_base >> 1) * ((Nal -> base_Width + 32) >> 1)], 
		tmp_Cr, &BaseV[(x_base >> 1) + (y_base >> 1) * ((Nal -> base_Width + 32) >> 1)]);

	x_base += x_mbPartIdx << 2;
	y_base += y_mbPartIdx << 2;

	//Upsample
	upsample_mb_luninance(Sps, Nal, Y, &tmp_luma[LeftBorder + (x_mbPartIdx << 2) + (y_mbPartIdx << 2) * LumaStride], x_base, y_base, x, y);
	upsample_mb_chroma(Sps, Nal, U, &tmp_Cb[(LeftBorder >> 1) + (x_mbPartIdx << 1) + (y_mbPartIdx << 1) * ChromaStride], x_base >> 1, y_base >> 1, x >> 1, y >> 1);
	upsample_mb_chroma(Sps, Nal, V, &tmp_Cr[(LeftBorder >> 1) + (x_mbPartIdx << 1) + (y_mbPartIdx << 1) * ChromaStride], x_base >> 1, y_base >> 1, x >> 1, y >> 1);
}
