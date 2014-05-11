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
#include "Transform.h"




//SVC Files
#include "svc_type.h"
#include "blk4x4.h"
#include "BL_residual.h"
#include "residual_4x4.h"
#include "residual_8x8.h"
#include "quantization.h"
#include "residual_chroma.h"
#include "ResidualProcessing.h"
#include "CoeffLevelPrediction.h"


/**
Rescale coefficent for Scoeff prediction for luminance
*/
void ScoeffSRescale4x4(RESIDU *CurrResidu, const short W4x4_inter [6][16])
{

	int i8x8;
	for ( i8x8 = 0 ; i8x8 < 16 ; i8x8++ ) {   
		/* each 4x4 sub-block of block */
		int index = i8x8 << 4;
		rescale_4x4_residual(CurrResidu -> Qp, &CurrResidu -> LumaLevel [index], &CurrResidu -> LumaLevel [index], W4x4_inter);
	}
}





/**
Rescale coefficent for Scoeff prediction for luminance
*/
void ScoeffSRescale8x8(RESIDU *CurrResidu, const short W8x8inter [6][64])
{

	int i8x8;
	for ( i8x8 = 0 ; i8x8 < 4 ; i8x8++ ) {   
		/* each 4x4 sub-block of block */
		int index = i8x8 << 6;
		rescale_8x8_residual(CurrResidu -> Qp, &CurrResidu -> LumaLevel [index], &CurrResidu -> LumaLevel [index], W8x8inter);
	}
}





/**
Rescale coefficent for Scoeff prediction for chroma
*/
void ScoeffSRescaleChr4x4(RESIDU *CurrResidu, const PPS* Pps, const W_TABLES *Quantif)
{


	short BaseDC_coeff [4];
	short BqP = (short) (CurrResidu -> Qp + Pps -> chroma_qp_index_offset);
	short BQPc = qP_tab [BqP];

	ict_4x4_dc_chroma(&CurrResidu -> ChromaDCLevel_Cb [0], BaseDC_coeff);
	rescale_4x4_dc_chr(BQPc,  BaseDC_coeff, BaseDC_coeff, Quantif -> W4x4_Cb_inter);

	rescale_4x4_residual(BQPc,  &CurrResidu -> ChromaACLevel_Cb [0], &CurrResidu -> ChromaACLevel_Cb [0], Quantif -> W4x4_Cb_inter);
	CurrResidu -> ChromaACLevel_Cb [0] = BaseDC_coeff [0];

	rescale_4x4_residual(BQPc,  &CurrResidu -> ChromaACLevel_Cb [16], &CurrResidu -> ChromaACLevel_Cb [16], Quantif -> W4x4_Cb_inter);
	CurrResidu -> ChromaACLevel_Cb [16] = BaseDC_coeff [1];

	rescale_4x4_residual(BQPc,  &CurrResidu -> ChromaACLevel_Cb [32], &CurrResidu -> ChromaACLevel_Cb [32], Quantif -> W4x4_Cb_inter);
	CurrResidu -> ChromaACLevel_Cb [32] = BaseDC_coeff [2];

	rescale_4x4_residual(BQPc,  &CurrResidu -> ChromaACLevel_Cb [48], &CurrResidu -> ChromaACLevel_Cb [48], Quantif -> W4x4_Cb_inter);
	CurrResidu -> ChromaACLevel_Cb [48] = BaseDC_coeff [3];

	//Cr
	BqP = (short) (CurrResidu -> Qp + Pps -> second_chroma_qp_index_offset);
	BQPc = qP_tab [BqP];
	ict_4x4_dc_chroma(&CurrResidu -> ChromaDCLevel_Cr [0], BaseDC_coeff);
	rescale_4x4_dc_chr(BQPc,  BaseDC_coeff, BaseDC_coeff, Quantif -> W4x4_Cr_inter);

	rescale_4x4_residual(BQPc,  &CurrResidu -> ChromaACLevel_Cr [0], &CurrResidu -> ChromaACLevel_Cr [0], Quantif -> W4x4_Cr_inter);
	CurrResidu -> ChromaACLevel_Cr [0] = BaseDC_coeff [0];

	rescale_4x4_residual(BQPc,  &CurrResidu -> ChromaACLevel_Cr [16], &CurrResidu -> ChromaACLevel_Cr [16], Quantif -> W4x4_Cr_inter);
	CurrResidu -> ChromaACLevel_Cr [16] = BaseDC_coeff [1];

	rescale_4x4_residual(BQPc,  &CurrResidu -> ChromaACLevel_Cr [32], &CurrResidu -> ChromaACLevel_Cr [32], Quantif -> W4x4_Cr_inter);
	CurrResidu -> ChromaACLevel_Cr [32] = BaseDC_coeff [2];

	rescale_4x4_residual(BQPc,  &CurrResidu -> ChromaACLevel_Cr [48], &CurrResidu -> ChromaACLevel_Cr [48], Quantif -> W4x4_Cr_inter);
	CurrResidu -> ChromaACLevel_Cr [48] = BaseDC_coeff [3];
}






/**
This function is in charge to rescale all coefficients of the current marcoblock.
*/
void RescaleCurrentCoeff(RESIDU *CurrResidu, RESIDU *BaseResidu, const PPS *Pps, const W_TABLES *quantif, short SpatialEnhancement){

	//For luminance
	if( CurrResidu -> Cbp & 0xf){
		if (CurrResidu -> Transform8x8){
			ScoeffSRescale8x8(CurrResidu, quantif -> W8x8_inter);
		}else {
			ScoeffSRescale4x4(CurrResidu, quantif -> W4x4_inter);
		}
	}else if(!SpatialEnhancement){
		//When the cbp is equal to zero, we have to update the transform8x8 flag with 
		//the one of the base macroblock.
		//This is only done with SNR enhancement
		CurrResidu -> Transform8x8 = BaseResidu -> Transform8x8;
	}

	//For chrominance
	if( CurrResidu -> Cbp & 0x30){
		ScoeffSRescaleChr4x4(CurrResidu, Pps, quantif);
	}
}


/**
This function computes the coefficient level of the current layer, including the rescaling of the base alyer.
*/
void ComputeCurrentCoeff(const NAL *Nal, RESIDU *CurrResidu, RESIDU *BaseResidu, const PPS *Pps, const W_TABLES *quantif)
{

	//We can't check by testing cbp == 0, because, the coefficient may have been predicted from base layer
	//if ((Nal -> DependencyId == 1 || Nal -> QualityId == 1)){
	if ((Nal -> DependencyId == 1 && Nal -> QualityId == 0) || (Nal -> QualityId == 1 && Nal -> DependencyId == 0)){
		if (BaseResidu -> Transform8x8){
			ScoeffSRescale8x8(BaseResidu, quantif -> W8x8_inter);
		}else {
			ScoeffSRescale4x4(BaseResidu, quantif -> W4x4_inter);
		}
		ScoeffSRescaleChr4x4(BaseResidu, Pps, quantif);
	}
	AddCoeff256(CurrResidu -> LumaLevel, BaseResidu -> LumaLevel);
	AddCoeff64(CurrResidu -> ChromaACLevel_Cb, BaseResidu -> ChromaACLevel_Cb);
	AddCoeff64(CurrResidu -> ChromaACLevel_Cr, BaseResidu -> ChromaACLevel_Cr);
	UpdateBlk4x4(CurrResidu);
}





/**
This function calculates the Scoeff residual of the current macroblock.
Here, base macroblock coefficents are rescaled, when base layer was a AVC one,
and added to the current macroblock coefficient. The sum is stored in the current macroblock,
in order to be used for upper SNR layer.
The residual is also calculated in a short picture in order to be used for upper spatial enhancement, or to add to the prediction.
*/
void SCoeffResidualAddRes(const NAL *Nal, RESIDU *CurrResidu, RESIDU *BaseResidu, const PPS *Pps, const short PicWidthInPix, 
						  const W_TABLES *Quantif, short *Residu_Luma, short *Residu_Cb, short *Residu_Cr)
{

	//We have to keep base and current layer rescale coefficient for Scoeff prediction
	//For upper layer
	if( CurrResidu -> Cbp){
		// In case of new SNR enhancement based on this one
		RescaleCurrentCoeff(CurrResidu, BaseResidu, Pps, Quantif, Nal -> SpatialScalability);
	}else if ( CurrResidu -> ResidualPredictionFlag && !Nal -> SpatialScalability){
		//===== modify QP values (as specified in G.8.1.5.1.2) =====
		//cbp == 0 && (Scoeff || Tcoeff) && (I_BL || ResidualPredictionFlag)
		CurrResidu -> Qp = BaseResidu -> Qp;
		CurrResidu -> Transform8x8 = BaseResidu -> Transform8x8;
	}


	//Test if cbp != 0 and if we are in the first enhancement layer in MGS or CGS
	if( (!IS_I(CurrResidu -> MbType)) && CurrResidu -> ResidualPredictionFlag && (!IS_I(BaseResidu -> MbType))){
		ComputeCurrentCoeff(Nal, CurrResidu, BaseResidu, Pps, Quantif);
	}

	if (Nal -> PicToDecode){
		if (!CurrResidu -> Transform8x8){
			SCoeff4x4AddRes(Residu_Luma, CurrResidu, PicWidthInPix);
		} else {
			SCoeff8x8AddRes(Residu_Luma, CurrResidu, PicWidthInPix);
		}

		//Decode the chrominance
		SCoeffChromaAddRes(Residu_Cb, Residu_Cr, CurrResidu, PicWidthInPix >> 1);
	}
}


/**
This function build the macroblock according to the coefficient levels.
*/
void SCoeffResidualAddPic(const NAL *Nal, RESIDU *CurrResidu, RESIDU *BaseResidu, const PPS *Pps, const short PicWidthInPix, 
						  const W_TABLES *Quantif, unsigned char *Y, unsigned char *U, unsigned char *V)
{



	//We have to keep base and current layer rescale coefficient for Scoeff prediction
	//For upper layer
	if( CurrResidu -> Cbp){
		// In case of new SNR enhancement based on this one
		RescaleCurrentCoeff(CurrResidu, BaseResidu, Pps, Quantif, Nal -> SpatialScalability);
	}

	ComputeCurrentCoeff(Nal, CurrResidu, BaseResidu, Pps, Quantif);

	if (Nal -> PicToDecode){
		if ( !CurrResidu -> Transform8x8){
			SCoeff4x4AddPic(Y, CurrResidu, PicWidthInPix);
		} else {
			SCoeff8x8AddPic(Y, CurrResidu, PicWidthInPix);
		}

		//Decode the chrominance
		SCoeffChromaAddPic(U, V, CurrResidu, PicWidthInPix >> 1);
	}
}


/**
This function build the macroblock according to the coefficient levels.
*/
void TCoeffResidual(RESIDU *CurrResidu, RESIDU *BaseResidu, const PPS *Pps, const short PicWidthInPix, 
					const W_TABLES *Quantif, short *Residu_Y, short *Residu_Cb, short *Residu_Cr)
{




	if (BaseResidu -> Cbp){
		//In case of an INTRA_4x4, we don't have to copy intra_4x4 mode because there are still in the mode_tab.
		short BaseQP = qP_tab[BaseResidu -> Qp + Pps -> chroma_qp_index_offset];
		short CurrQP = qP_tab[CurrResidu -> Qp + Pps -> chroma_qp_index_offset];
		short Delta = BaseQP - CurrQP + 54;
		short DeltaQP = (short) (BaseResidu -> Qp - CurrResidu -> Qp + 54);

		TCoeffRescaleACLuma(DeltaQP, BaseResidu -> LumaLevel, CurrResidu -> LumaLevel);
		UpdateLumaCbp(CurrResidu, BaseResidu);

		TCoeffRescaleDCChr(Delta, BaseResidu -> ChromaDCLevel_Cb, CurrResidu -> ChromaDCLevel_Cb);
		TCoeffRescaleDCChr(Delta, BaseResidu -> ChromaDCLevel_Cr, CurrResidu -> ChromaDCLevel_Cr);
		TCoeffRescaleACChr(Delta, BaseResidu -> ChromaACLevel_Cb, CurrResidu -> ChromaACLevel_Cb);
		TCoeffRescaleACChr(Delta, BaseResidu -> ChromaACLevel_Cr, CurrResidu -> ChromaACLevel_Cr);
		UpdateChrCbp(CurrResidu, BaseResidu);
	}

	if (CurrResidu -> Cbp){
		//Calculate the residual
		ComputeResidualImage(Pps, CurrResidu, PicWidthInPix, Residu_Y, Residu_Cb, Residu_Cr, Quantif);

	}else {
		ResetMbResidu(Residu_Y, Residu_Cb, Residu_Cr, PicWidthInPix);
	}
}
