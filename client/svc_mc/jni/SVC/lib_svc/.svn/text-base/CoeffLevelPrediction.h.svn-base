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

#ifndef COEFF_LEVEL_PRED_H
#define COEFF_LEVEL_PRED_H


//SVC Files
#include "svc_type.h"



extern const unsigned char DIV6 [72];

/**
This function quantizes a block 4x4 of residual values.

@param qp QP of the current block.
@param in Buffer to rescale.
@param out Out buffer rescaled. 
@param W4x4 Quantization table.
*/
static __inline void TCoeffRescaleDCLuma ( const int qp, const short * RESTRICT in, short * RESTRICT out){

	
	int i;
	unsigned short shift = DIV6[qp];
	const short aiScaleFactor[6]  = { 8, 9, 10, 11, 13, 14 };
    const short Scale = aiScaleFactor[qp % 6];


	for ( i = 0 ; i < 16 ; i++ ) {
		out [i] += (in [i] * Scale << shift) >> 12;
	}
}

static __inline void TCoeffRescaleDCChr ( const int qp, const short * RESTRICT in, short * RESTRICT out){

	
	int i;
	unsigned short shift = DIV6[qp];
	const short aiScaleFactor[6]  = { 8, 9, 10, 11, 13, 14 };
    const short Scale = aiScaleFactor[qp % 6];


	for ( i = 0 ; i < 4 ; i++ ) {
		out [i] += (in [i] * Scale << shift) >> 12;
	}
}


static __inline void TCoeffRescaleACLuma  ( const int qp, const short * RESTRICT in, short * RESTRICT out){

	
	int i;
	unsigned short shift = DIV6[qp];
	const short aiScaleFactor[6]  = { 8, 9, 10, 11, 13, 14 };
    const short Scale = aiScaleFactor[qp % 6];


	for ( i = 0 ; i < 256 ; i++ ) {
		out [i] += (in [i] * Scale << shift) >> 12;
	}
}


static __inline void TCoeffRescaleACChr  ( const int qp, const short * RESTRICT in, short * RESTRICT out){

	
	int i;
	unsigned short shift = DIV6[qp];
	const short aiScaleFactor[6]  = { 8, 9, 10, 11, 13, 14 };
    const short Scale = aiScaleFactor[qp % 6];


	for ( i = 0 ; i < 64 ; i++ ) {
		out [i] += (in [i] * Scale << shift) >> 12;
	}
}


static __inline void UpdateLumaCbp(RESIDU *CurrResidu, const RESIDU *BaseResidu){

	
	if ( !(CurrResidu -> Cbp & 0x01)){
		CurrResidu -> Cbp += BaseResidu -> Cbp & 0x01;
	}

	if ( !(CurrResidu -> Cbp & 0x02)){
		CurrResidu -> Cbp += BaseResidu -> Cbp & 0x02;
	}

	if ( !(CurrResidu -> Cbp & 0x04)){
		CurrResidu -> Cbp += BaseResidu -> Cbp & 0x04;
	}

	if ( !(CurrResidu -> Cbp & 0x08)){
		CurrResidu -> Cbp += BaseResidu -> Cbp & 0x08;
	}
}


static __inline void UpdateChrCbp(RESIDU *CurrResidu, const RESIDU *BaseResidu){

	
	if ( !(CurrResidu -> Cbp & 0x30)){
		CurrResidu -> Cbp += BaseResidu -> Cbp & 0x30;
	}

	if ( !(CurrResidu -> Cbp & 0x20)){
		CurrResidu -> Cbp += BaseResidu -> Cbp & 0x20;
	}
}




static __inline void UpdateLevel(RESIDU *CurrResidu){

		CurrResidu -> Intra16x16DCLevel [0] = CurrResidu -> Intra16x16DCLevel [0] + CurrResidu -> LumaLevel [0];
		CurrResidu -> Intra16x16DCLevel [1] = CurrResidu -> Intra16x16DCLevel [1] + CurrResidu -> LumaLevel [16];
		CurrResidu -> Intra16x16DCLevel [2] = CurrResidu -> Intra16x16DCLevel [2] + CurrResidu -> LumaLevel [64];
		CurrResidu -> Intra16x16DCLevel [3] = CurrResidu -> Intra16x16DCLevel [3] + CurrResidu -> LumaLevel [80];
		CurrResidu -> Intra16x16DCLevel [4] = CurrResidu -> Intra16x16DCLevel [4] + CurrResidu -> LumaLevel [32];
		CurrResidu -> Intra16x16DCLevel [5] = CurrResidu -> Intra16x16DCLevel [5] + CurrResidu -> LumaLevel [48];
		CurrResidu -> Intra16x16DCLevel [6] = CurrResidu -> Intra16x16DCLevel [6] + CurrResidu -> LumaLevel [96];
		CurrResidu -> Intra16x16DCLevel [7] = CurrResidu -> Intra16x16DCLevel [7] + CurrResidu -> LumaLevel [112];
		CurrResidu -> Intra16x16DCLevel [8] = CurrResidu -> Intra16x16DCLevel [8] + CurrResidu -> LumaLevel [128];
		CurrResidu -> Intra16x16DCLevel [9] = CurrResidu -> Intra16x16DCLevel [9] + CurrResidu -> LumaLevel [144];
		CurrResidu -> Intra16x16DCLevel [10] = CurrResidu -> Intra16x16DCLevel [10] + CurrResidu -> LumaLevel [192];
		CurrResidu -> Intra16x16DCLevel [11] = CurrResidu -> Intra16x16DCLevel [11] + CurrResidu -> LumaLevel [208];
		CurrResidu -> Intra16x16DCLevel [12] = CurrResidu -> Intra16x16DCLevel [12] + CurrResidu -> LumaLevel [160];
		CurrResidu -> Intra16x16DCLevel [13] = CurrResidu -> Intra16x16DCLevel [13] + CurrResidu -> LumaLevel [176];
		CurrResidu -> Intra16x16DCLevel [14] = CurrResidu -> Intra16x16DCLevel [14] + CurrResidu -> LumaLevel [224];
		CurrResidu -> Intra16x16DCLevel [15] = CurrResidu -> Intra16x16DCLevel [15] + CurrResidu -> LumaLevel [240];
}


void ScoeffSRescale4x4(RESIDU *CurrResidu, const short W4x4_inter [6][16]);
void ScoeffSRescaleChr4x4(RESIDU *CurrResidu, const PPS* Pps, const W_TABLES *Quantif);
void ScoeffSRescale8x8(RESIDU *CurrResidu, const short W8x8inter [6][64]);

void ComputeCurrentCoeff(const NAL *Nal, RESIDU *CurrResidu, RESIDU *BaseResidu, const PPS *Pps, const W_TABLES *quantif);
void RescaleCurrentCoeff(RESIDU *CurrResidu, RESIDU *BaseResidu, const PPS *Pps, const W_TABLES *quantif, short SnrEnhancement);


void SCoeffResidualAddRes(const NAL *Nal,RESIDU *CurrResidu, RESIDU *BaseResidu, const PPS *Pps, const short PicWidthInPix, const W_TABLES *quantif
					, short *Residu_img_lum, short *Residu_img_Cb, short *Residu_img_Cr	);
void SCoeffResidualAddPic(const NAL *Nal, RESIDU *CurrResidu, RESIDU *BaseResidu, const PPS *Pps, const short PicWidthInPix, const W_TABLES *quantif
					, unsigned char *Y, unsigned char *U, unsigned char *V);
void TCoeffResidual(RESIDU *CurrResidu, RESIDU *BaseResidu, const PPS *Pps, const short PicWidthInPix, const W_TABLES *quantif
					, short *Residu_Y, short *Residu_Cb, short *Residu_Cr);
#endif
