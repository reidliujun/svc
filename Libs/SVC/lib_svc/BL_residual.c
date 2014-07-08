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
#include "symbol.h"
#include "Transform.h"
#include "decode_mb_P.h"
#include "transform_8x8.h"
#include "quantization.h"



//SVC Files
#include "svc_type.h"
#include "BL_residual.h"
#include "residual_4x4.h"
#include "residual_8x8.h"
#include "residual_chroma.h"
#include "ResidualProcessing.h"
#include "CoeffLevelPrediction.h"




/**
Compute the residual for a I macroblock.
*/
void ComputeCurrentResidual(const NAL *Nal, RESIDU *CurrResidu, RESIDU *BaseResidu, const PPS *Pps, const W_TABLES *Quantif, 
							const short PicWidthInPix, unsigned char *Y, unsigned char *U, unsigned char *V)
{

	//If we have to reconstruct the frame, in case of spatial enhancement is based on this layer.
	if (Nal -> PicToDecode){
		//Add the residual
		ComputeResidual(Pps, CurrResidu, PicWidthInPix, Y, U, V, Quantif);
	}else {
		if( CurrResidu -> Cbp){
			// In case of new SNR enhancement based on this one
			RescaleCurrentCoeff(CurrResidu, BaseResidu, Pps, Quantif, Nal -> SpatialScalability);
		}
	}
}




/**
Computes the residual of the current macroblock.
*/
void ComputeResidualImage(const PPS *Pps, RESIDU *CurrResidu, const short PicWidthInPix, 
						  short *Y, short *U, short *V, const W_TABLES *Quantif)
{
	if ( CurrResidu -> Cbp){
		if ( !CurrResidu -> Transform8x8){
			StoreRes4x4(Y, CurrResidu, PicWidthInPix, Quantif -> W4x4_inter);
		} else {	
			StoreRes8x8(Y, CurrResidu, PicWidthInPix, Quantif -> W8x8_inter);
		}
		//Decode the chrominance
		StoreResChroma(U, V, CurrResidu, Pps, PicWidthInPix >> 1, Quantif);

	}else {
		ResetMbResidu(Y, U, V, PicWidthInPix);
	}
}
