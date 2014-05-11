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
#define C_MB_I 
#ifndef POCKET_PC
#include <stdio.h>
#endif
#include "type.h"
#include "data.h"
#include "quantization.h"
#include "Transform.h"
#include "transform_8x8.h"
#include "fill_caches.h"
#include "decode_mb_I.h"
#include "neighbourhood.h"



/**
This function permits to decode all intra4x4 luminance blocks in a MB.

@param image Table of current frame.
@param LumaLevel Contains the AC luma level of a 4x4 decoded macroblock.
@param PicWidthInMbs Width in macroblocks of the current frame.
@param mode_tab Specifies the mode of prediction of each block 4x4.
@param residu Structure which contains all the residu data of the current macroblock.
@param W4x4_intra is the scaling matrix.
@param mb4x4_mode is the function vector for intra prediction

*/
void decode_I_Intra4x4_lum ( unsigned char *image, const short PicWidthInPix, const RESIDU *residu, const short W4x4_intra [6][16],const mb4x4_mode_pf *mb4x4_mode)
{


	short i ;

	// each luma 4x4 block
	for ( i = 0 ; i < 16 ; i ++ ) {       
		short locx = LOCX(i);
		short locy = LOCY(i);
		short mode = residu -> Intra16x16DCLevel[i]; 
		unsigned char *ptr_img = &image[locx + locy * PicWidthInPix];


		/* Chose the right prediction mode functions */
		mb4x4_mode [mode](ptr_img, PicWidthInPix, residu -> AvailMask, locx, locy);

		if (residu -> Cbp & 1 << (i >> 2)){
			short luma_coeff [16];
			rescale_4x4_residual(residu -> Qp, &residu -> LumaLevel [i << 4], luma_coeff, W4x4_intra);
			ict_4x4_residual(luma_coeff, ptr_img, PicWidthInPix);
		}
	}
}







/**
This function permits to decode an intra16x16 luminance MB.


@param image Table of current frame.
@param PicWidthInMbs Width in macroblocks of the current frame.
@param Block Data of the current macroblock.
@param QP Quantification parameter.
@param W4x4_intra is the scaling matrix.
@param mb16x16_mode is the function vector for intra prediction.

*/
void decode_I_Intra16x16_lum (unsigned char *image, const short PicWidthInPix, const RESIDU *CurrResidu,
							  const short W4x4_intra [6][16], const mb16x16_mode_pf *mb16x16_mode)
{
	short luma_coeff [16];
	short luma_dc_coeff [16];
	unsigned int i ;

	mb16x16_mode[CurrResidu -> Intra16x16PredMode](image, PicWidthInPix, CurrResidu -> AvailMask);		

	ict_4x4_dc_lum(CurrResidu -> Intra16x16DCLevel, luma_dc_coeff);
	rescale_4x4_dc_lum(CurrResidu -> Qp, luma_dc_coeff, luma_dc_coeff, W4x4_intra);

	// each 4x4 sub-block of block
	for ( i = 0 ; i < 16 ; i++ ) {
		int locx = LOCX(i);
		int locy = LOCY(i);
		int index_lum = (locy >> 2) + locx;
		rescale_4x4_residual(CurrResidu -> Qp, &CurrResidu -> LumaLevel [i << 4], luma_coeff, W4x4_intra);
		luma_coeff [0] = luma_dc_coeff [index_lum];
		ict_4x4_residual(luma_coeff, &image[locx + locy * PicWidthInPix], PicWidthInPix);
	}
}





/**
This function permits to decode all intra8x8 luminance blocks in a MB.

@param image Table of current frame.
@param LumaLevel Contains the AC luma level of a 4x4 decoded macroblock.
@param PicWidthInMbs Width in macroblocks of the current frame.
@param mode_tab Specifies the mode of prediction of each block 4x4.
@param residu Structure which contains all the residu data of the current macroblock.
@param W8x8_intra is the scaling matrix.
*/

void decode_I_Intra8x8_lum ( unsigned char *image, const short PicWidthInPix, const RESIDU *residu, const short W8x8_intra [6][64] )
{
	short i;
	short luma_coeff [64];

	for ( i = 0 ; i < 4 ; i++ ) {       
		short locx = LOCX(i << 2);
		short locy = LOCY(i << 2);
		int mode = residu -> Intra16x16DCLevel[i << 2];
		unsigned char *ptr_img = &image[locx + locy * PicWidthInPix];

		// Update the prediction
		mbAddrN_8x8(ptr_img, PicWidthInPix, residu -> AvailMask, locx, locy, mode);
		if (residu -> Cbp & 1 << i)	{
			rescale_8x8_residual(residu -> Qp, &residu -> LumaLevel [i << 6], luma_coeff, W8x8_intra);
			ict_8x8(luma_coeff, ptr_img, PicWidthInPix);
		}
	}
}





/**
This function permits to decode all intra chrominances in a MB.

@param image_Cb Table of current frame.
@param image_Cr Table of current frame.
@param Block Data of the current macroblock.
@param PicWidth Width in macroblocks of the current frame.
@param pps PPS structure of the current video.
@param residu Structure which contains all the residu data of the current macroblock.
@param quantif Table which contains the quantization table.
@param mb4x4_mode is the function vector for intra prediction
*/

void decode_I_chroma ( unsigned char * image_Cb, unsigned char *image_Cr, short PicWidth, const PPS *pps, 
					  const RESIDU *residu, const W_TABLES *quantif, const mbChroma_mode_pf * mbChroma_mode_pf) 
{

	
	short DC_coeff [4];
	short AC_coeff [16];


	// prediction mode functions called by the pointer
	mbChroma_mode_pf[residu -> IntraChromaPredMode](image_Cb, PicWidth, residu -> AvailMask);
	mbChroma_mode_pf[residu -> IntraChromaPredMode](image_Cr, PicWidth, residu -> AvailMask);


	if ( residu -> Cbp & 0x30){
		//Initialization of QP 

		int QPc = qP_tab [residu -> Qp + pps -> chroma_qp_index_offset];

		// Compute DC coefficients
		ict_4x4_dc_chroma(&residu -> ChromaDCLevel_Cb [0], DC_coeff);
		rescale_4x4_dc_chr(QPc, DC_coeff, DC_coeff, quantif -> W4x4_Cb_intra);

		// Compute AC coefficients
		rescale_4x4_residual(QPc, &residu -> ChromaACLevel_Cb [0], AC_coeff,  quantif -> W4x4_Cb_intra);
		AC_coeff [0] = DC_coeff [0];
		ict_4x4_residual(AC_coeff, image_Cb, PicWidth);

		rescale_4x4_residual(QPc, &residu -> ChromaACLevel_Cb [16], AC_coeff, quantif -> W4x4_Cb_intra);
		AC_coeff [0] = DC_coeff [1];
		ict_4x4_residual(AC_coeff, &image_Cb[4], PicWidth);

		rescale_4x4_residual(QPc, &residu -> ChromaACLevel_Cb [32], AC_coeff, quantif -> W4x4_Cb_intra);
		AC_coeff [0] = DC_coeff [2];
		ict_4x4_residual(AC_coeff, &image_Cb[PicWidth << 2], PicWidth);

		rescale_4x4_residual(QPc, &residu -> ChromaACLevel_Cb [48], AC_coeff, quantif -> W4x4_Cb_intra);
		AC_coeff [0] = DC_coeff [3];
		ict_4x4_residual(AC_coeff, &image_Cb[(1 + PicWidth) << 2], PicWidth);



		// Cr decoding process	  
		QPc = qP_tab [residu -> Qp + pps -> second_chroma_qp_index_offset];

		// Compute DC coefficients
		ict_4x4_dc_chroma(&residu -> ChromaDCLevel_Cr [0], DC_coeff);
		rescale_4x4_dc_chr(QPc, DC_coeff, DC_coeff, quantif -> W4x4_Cr_intra);

		// Compute AC coefficients
		rescale_4x4_residual(QPc, &residu -> ChromaACLevel_Cr [0], AC_coeff, quantif -> W4x4_Cr_intra);
		AC_coeff [0] = DC_coeff [0];
		ict_4x4_residual(AC_coeff, image_Cr, PicWidth);

		rescale_4x4_residual(QPc, &residu -> ChromaACLevel_Cr [16], AC_coeff, quantif -> W4x4_Cr_intra);
		AC_coeff [0] = DC_coeff [1];
		ict_4x4_residual(AC_coeff, &image_Cr[4], PicWidth);

		rescale_4x4_residual(QPc, &residu -> ChromaACLevel_Cr [32], AC_coeff, quantif -> W4x4_Cr_intra);
		AC_coeff [0] = DC_coeff [2];
		ict_4x4_residual(AC_coeff, &image_Cr[PicWidth << 2], PicWidth);

		rescale_4x4_residual(QPc, &residu -> ChromaACLevel_Cr [48], AC_coeff, quantif -> W4x4_Cr_intra);
		AC_coeff [0] = DC_coeff [3];
		ict_4x4_residual(AC_coeff, &image_Cr[(1 + PicWidth) << 2], PicWidth);
	}
}






/**
This function permits to decode an intra MB.

@param image Table of current luma frame.
@param image_Cb Table of current chroma frame.
@param image_Cr Table of current chroma frame.
@param pps PPS structure of the current video.
@param aio_tstBlock Contains all parameters of each macroblock of the current picture.
@param mode_tab Specifies the mode of prediction of each block 4x4.
@param residu Structure which contains all the residu data of the current macroblock.
@param PicWidthInPix Width in pixel of the current frame.
@param quantif is the scaling matrix.
@param Allpf_struct is the function vector for all intra prediction
*/
void decode_MB_I (unsigned char *image, unsigned char *image_Cb, unsigned char *image_Cr, const PPS *pps, 
				  const RESIDU *residu, const short PicWidthInPix, const W_TABLES *quantif, const STRUCT_PF * Allpf_struct){    


	// Test on the mode of the current MB 
	if ( residu -> MbType == INTRA_4x4 ) {
		// Decode all intra 4x4 luminance blocks in a MB
		decode_I_Intra4x4_lum(image, PicWidthInPix, residu, quantif -> W4x4_intra, Allpf_struct -> Ptr_mb4x4_mode);

		// Decode chrominances
		decode_I_chroma(image_Cb, image_Cr, PicWidthInPix >> 1,  pps, residu, quantif, Allpf_struct -> Ptr_mbChroma_mode_pf);
	} 
	else if (residu -> MbType == INTRA_16x16 ) {   
		// Intra16x16 MB
		// Decode luminances
		decode_I_Intra16x16_lum(image, PicWidthInPix, residu, quantif -> W4x4_intra, Allpf_struct -> Ptr_mb16x16_mode);

		// Decode chrominances
		decode_I_chroma(image_Cb, image_Cr, PicWidthInPix >> 1, pps, residu, quantif, Allpf_struct -> Ptr_mbChroma_mode_pf);
	}
	else if (residu -> MbType == INTRA_8x8 ){
		// Intra16x16 MB
		// Decode luminances
		decode_I_Intra8x8_lum(image, PicWidthInPix, residu, quantif -> W8x8_intra);
		// Decode chrominances
		decode_I_chroma(image_Cb, image_Cr, PicWidthInPix >> 1, pps, residu, quantif, Allpf_struct -> Ptr_mbChroma_mode_pf);

	}
	else if (residu -> MbType == INTRA_PCM )
	{
		int i,j; 

		for (i = 0 ; i < 16; i++){
			for ( j = 0 ; j < 16; j++){
				image[PicWidthInPix * i + j] =  (unsigned char) residu -> LumaLevel[(i << 4) + j];
			}
		}

		for (i = 0 ; i < 8; i++){
			for ( j = 0 ; j < 8; j++){
				image_Cb[(PicWidthInPix >> 1) * i + j] =  (unsigned char) residu -> ChromaACLevel_Cb[(i << 3) + j];
				image_Cr[(PicWidthInPix >> 1) * i + j] =  (unsigned char) residu -> ChromaACLevel_Cr[(i << 3) + j];
			}
		}
	}
}





