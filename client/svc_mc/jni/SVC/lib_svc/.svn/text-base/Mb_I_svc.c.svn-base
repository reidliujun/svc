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



//AVC files
#include "type.h"
#include "symbol.h"
#include "decode_mb_I.h"
#include "decode_mb_P.h"
#include "neighbourhood.h"


//SVC files
#include "svc_type.h"
#include "svc_data.h"
#include "I_BL.h"
#include "Mb_I_svc.h"
#include "BL_residual.h"
#include "Coordonates.h"
#include "ResidualProcessing.h"
#include "InterLayerPrediction.h"
#include "CoeffLevelPrediction.h"




/**
Decode a I_BL marcoblock using SNR prediction.
*/
void MbISnr (const PPS *Pps, const NAL* Nal, const W_TABLES *Quantif, const STRUCT_PF *BaselineVectors, 
			 RESIDU *CurrResidu, RESIDU *BaseResidu, const short PicWidthInPix, 
			 unsigned char *Y, unsigned char *U, unsigned char *V,
			 unsigned char *BaseY, unsigned char *BaseU, unsigned char *BaseV)
{

	//Construction process for one macroblock
	if (IS_I(CurrResidu -> MbType)){
		//I type macroblock decoding process
		decode_MB_I(Y, U, V, Pps, CurrResidu, PicWidthInPix, Quantif, BaselineVectors);
	}else if (Nal -> TCoeffPrediction){
		//Decoding proces of the macroblock, as an INTRA_BL
		DecodeITCoeff(CurrResidu, BaseResidu, Pps, PicWidthInPix, Quantif, BaselineVectors, Y, U, V);
	}else if (Nal -> SCoeffPrediction){
		if ((CurrResidu -> Cbp & 0x0f) == 0){
			//===== modify QP values (as specified in G.8.1.5.1.2) =====
			//cbp == 0 && (Scoeff || Tcoeff) && (I_BL || ResidualPredictionFlag)
			CurrResidu -> Transform8x8 = BaseResidu -> Transform8x8;
			if (!CurrResidu -> Cbp){
				CurrResidu -> Qp = BaseResidu -> Qp;
			}
		}

		//Bring back base layeer sample into current layer
		GetBaseSample(Y, U, V, BaseY, BaseU, BaseV, PicWidthInPix);


		if ( IS_BL (BaseResidu -> MbType)){
			//In case of two SNR enchancement
			//We have to get the zero quality prediction
			//And to add to this all Scoeff 
			SCoeffResidualAddPic(Nal, CurrResidu, BaseResidu, Pps, PicWidthInPix, Quantif, Y, U, V);
		}else if (Nal -> PicToDecode){
			//We have only one layer prediction, so we can just add the residual to it.
			ComputeResidual(Pps, CurrResidu, PicWidthInPix, Y, U, V, Quantif);
		}else{
			if( CurrResidu -> Cbp){
				// In case of new SNR enhancement based on this one
				RescaleCurrentCoeff(CurrResidu, BaseResidu, Pps, Quantif, Nal -> SpatialScalability);
			}
		}
	}
}




/**
Decode a I_BL marcoblock using spatial prediction.
*/
void MbISpatial (const NAL *Nal, const SPS *Sps, const PPS *Pps, const short PicWidthInPix, const W_TABLES *Quantif, 
				 short x, short y, DATA *CurrBlock, RESIDU *CurrResidu, RESIDU *BaseResidu, 
				 unsigned char *Y, unsigned char *U, unsigned char *V, 
				 unsigned char *BaseY, unsigned char *BaseU, unsigned char *BaseV)
{


	// For all other case, spatial layer
	short x_base, y_base, x_mbPartIdx, y_mbPartIdx;
	
	//Get coordonate in the lower layer
	short BaseMbAddr = CurrBlock -> MbBaseAddr;
	GetBaseMbCoord(CurrBlock, &x_base, &y_base, &x_mbPartIdx, &y_mbPartIdx);
	
	//Get the prediction by upsampling
	MbIPad(Nal, Pps, Sps, BaseMbAddr, x << 4, y << 4, x_base, y_base, x_mbPartIdx, y_mbPartIdx, BaseResidu, BaseY, BaseU, BaseV, Y, U, V);

	//Compute residual
	ComputeCurrentResidual(Nal, CurrResidu, BaseResidu, Pps, Quantif, PicWidthInPix, Y, U, V);
}


/**
This function deocdes an I macroblock into an P or B slice.
*/
void MbISVCDecoding(const SPS *Sps, const PPS *Pps, const NAL* Nal, const W_TABLES *Quantif, const STRUCT_PF *BaselineVectors, 
			 RESIDU *CurrResidu, RESIDU *BaseResidu, DATA *Block,
			 const short PicWidthInPix, short iMb_x, short iMb_y,
			 unsigned char *Y, unsigned char *U, unsigned char *V, 
			 unsigned char *BaseY, unsigned char *BaseU, unsigned char *BaseV)
{
	if(!Nal -> SpatialScalability || IS_I(CurrResidu -> MbType)){
		//AVC I macroblock or SNR I macroblock
		const short iMb_xy = iMb_x + iMb_y * ((PicWidthInPix - 32) >> 4);
		MbISnr(Pps, Nal, Quantif, BaselineVectors, CurrResidu, &BaseResidu[iMb_xy], PicWidthInPix, 
			Y, U, V, &BaseY[((iMb_x + (iMb_y * PicWidthInPix)) << 4)], 
			&BaseU[((iMb_x + (iMb_y * PicWidthInPix >> 1)) << 3)], 
			&BaseV[((iMb_x + (iMb_y * PicWidthInPix >> 1)) << 3)]);
	}else{
		//Only SVC I macroblock with spatial enhancement
		MbISpatial (Nal, Sps, Pps, PicWidthInPix, Quantif, iMb_x, iMb_y, Block, CurrResidu, BaseResidu,
			Y, U, V, BaseY, BaseU, BaseV);
	}
}
