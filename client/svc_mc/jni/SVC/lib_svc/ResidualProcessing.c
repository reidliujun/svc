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
#include "clip.h"
#include "decode_mb_P.h"

//SVC Files
#include "svc_type.h"
#include "blk4x4.h"
#include "svc_data.h"
#include "BL_residual.h"
#include "residual_4x4.h"
#include "residual_8x8.h"
#include "quantization.h"
#include "residual_chroma.h"
#include "ResidualProcessing.h"
#include "CoeffLevelPrediction.h"



/**
Add the current residual to the residual picture, in order to calculate the right residual for the current layer.
*/
void base_layer_residual(RESIDU *residu, short *Luma, short *Chroma_Cb, short *Chroma_Cr, 
						 const short PicWidthInPix, const PPS *Pps, const W_TABLES *quantif){


	if ( !residu -> Transform8x8){
		AddRes4x4(Luma, residu, PicWidthInPix, quantif -> W4x4_inter);
	} else {
		AddRes8x8(Luma, residu, PicWidthInPix, quantif -> W8x8_inter);
	}

	//Decode the chrominance
	AddResChroma(Chroma_Cb, Chroma_Cr, residu, Pps, PicWidthInPix >> 1, quantif);
}




/**
Compute the residual of the current frame.
*/
void ResidualPrediction(const NAL *Nal, const PPS *Pps, const W_TABLES *Quantif, RESIDU *CurrResidu, 
						RESIDU *BaseResidu, const short PicWidthInPix, 
						short *Residu_Luma, short *Residu_Cb, short *Residu_Cr)
{
	//Check the type of prediction
	if ( Nal -> SCoeffPrediction){
		//Add coefficients from the base layer to those of the current layer
		SCoeffResidualAddRes(Nal, CurrResidu, BaseResidu, Pps, PicWidthInPix, Quantif, Residu_Luma, Residu_Cb, Residu_Cr);
	}else if (Nal -> TCoeffPrediction && !IS_I(BaseResidu -> MbType)){
		//===== modify QP values (as specified in G.8.1.5.1.2) =====
		//cbp == 0 && (Scoeff || Tcoeff) && (I_BL || ResidualPredictionFlag)
		// BaseResidu -> Qp = CurrResidu -> Qp so DeltaQP = 54
		if (CurrResidu -> Cbp == 0){
			CurrResidu -> Qp = BaseResidu -> Qp;
		}
		if ((CurrResidu -> Cbp & 0x0f) == 0){
			CurrResidu -> Transform8x8 = BaseResidu -> Transform8x8;
		}

		TCoeffResidual(CurrResidu, BaseResidu, Pps, PicWidthInPix, Quantif, Residu_Luma, Residu_Cb, Residu_Cr);
	}else if (CurrResidu -> Cbp){
		
		if ( !Nal -> PicToDecode){
			//Just rescale coefficients.
			RescaleCurrentCoeff(CurrResidu, BaseResidu, Pps, Quantif, Nal -> SpatialScalability);
		}else {
			//add the residual of the current layer
			base_layer_residual(CurrResidu, Residu_Luma, Residu_Cb, Residu_Cr, PicWidthInPix, Pps, Quantif);
		}
	}

	//Update the bl4x4 flag in order to know if the 4x4 block are intra coded.
	update_blk4x4Res(Residu_Luma, CurrResidu, PicWidthInPix);

	//Let's try
	//To test with Tcoeff prediction -> have to do UpdateBlk4x4(CurrResidu);
	//We have to reconsider the blk4x4 due to the Tcoeff prediction
	if (Nal -> TCoeffPrediction){
		CurrResidu -> blk4x4 = CurrResidu -> blk4x4_Res;
	}
}



/**
This function calculates the residual and store it in the prediction, when it's the Nal to display.
*/
void AddResidualPic(const NAL *Nal, const PPS *Pps, RESIDU *CurrResidu, RESIDU *BaseResidu, 
					const short PicWidthInPix, const W_TABLES *Quantif, 
					unsigned char *Y,unsigned char *U, unsigned char *V, 
					short *Residu_Luma, short *Residu_Cb, short *Residu_Cr)
{

	//Determine if current residu is predicted from the base layer
	if (CurrResidu -> ResidualPredictionFlag){
		ResidualPrediction(Nal, Pps, Quantif, CurrResidu, BaseResidu, PicWidthInPix, Residu_Luma, Residu_Cb, Residu_Cr);

		//add the residual to the prediction
		add_compensed_residual(Y, U, V, Residu_Luma, Residu_Cb, Residu_Cr, PicWidthInPix);
	}else{
		//Calculate the residual
		CurrResidu -> blk4x4_Res = CurrResidu -> blk4x4;
		if (CurrResidu -> Cbp){
			//Add of the residual
			ComputeResidual(Pps, CurrResidu, PicWidthInPix, Y, U, V, Quantif);
		} else if ( (Nal -> SCoeffPrediction || Nal -> TCoeffPrediction) && CurrResidu -> ResidualPredictionFlag){
			//===== modify QP values (as specified in G.8.1.5.1.2) =====
			//cbp == 0 && (Scoeff || Tcoeff) && (I_BL || ResidualPredictionFlag)
			CurrResidu -> Qp = BaseResidu -> Qp;
			CurrResidu -> Transform8x8 = BaseResidu -> Transform8x8;
		}
	}
}




/**
This function calculates the residual and store it, when it's not the Nal to display.
*/
void AddResidualRes(const NAL *Nal, const PPS *Pps, RESIDU *CurrResidu, RESIDU *BaseResidu, 
					const short PicWidthInPix, const W_TABLES *Quantif, 
					short *Residu_Luma, short *Residu_Cb, short *Residu_Cr)
{

	//Determine if current residu is predicted from the base layer
	if (CurrResidu -> ResidualPredictionFlag){
		ResidualPrediction(Nal, Pps, Quantif, CurrResidu, BaseResidu, PicWidthInPix, Residu_Luma, Residu_Cb, Residu_Cr);
	}else if (!Nal -> PicToDecode){
		RescaleCurrentCoeff(CurrResidu, BaseResidu, Pps, Quantif, Nal -> SpatialScalability);
	}else{
		//Calculate the residual
		CurrResidu -> blk4x4_Res = CurrResidu -> blk4x4;
		ComputeResidualImage(Pps, CurrResidu, PicWidthInPix, Residu_Luma, Residu_Cb, Residu_Cr, Quantif);
	}
}


/**
This function calculates the residual and store it directly in the picture, when it's the Nal to display, 
or store it in an intermediary place
*/
void AddResidual(const NAL *Nal, const PPS * Pps, RESIDU *CurrResidu, RESIDU *BaseResidu, 
					const short PicWidthInPix, const W_TABLES *quantif, 
					unsigned char *Y,unsigned char *U, unsigned char *V, 
					short *Residu_Luma, short *Residu_Cb, short *Residu_Cr)
{

	//Add residual to prediction
	if(Nal -> PicToDisplay){
		AddResidualPic(Nal, Pps, CurrResidu, BaseResidu, PicWidthInPix, quantif, Y, U, V, Residu_Luma, Residu_Cb, Residu_Cr);
	}else if (Nal -> SCoeffPrediction){
		// In case of spatial enhancement based on this one
		//Add coefficients from the base layer to those of the current layer
		SCoeffResidualAddRes(Nal, CurrResidu, BaseResidu, Pps, PicWidthInPix, quantif, Residu_Luma, Residu_Cb, Residu_Cr);
	}	else{
		//Or just store the residual for upper layer
		AddResidualRes(Nal, Pps, CurrResidu, BaseResidu, PicWidthInPix, quantif, Residu_Luma, Residu_Cb, Residu_Cr);
	}
}
