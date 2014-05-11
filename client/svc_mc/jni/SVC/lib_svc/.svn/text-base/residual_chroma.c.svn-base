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
#include "data.h"
#include "Transform.h"
#include "quantization.h"

//SVC Files
#include "ict_svc.h"
#include "residual_chroma.h"
#include "ResidualProcessing.h"





/**
This function allow to store the residual in a short picture in order to be used for upper layer prediction
*/

void StoreResChroma (short *image_Cb, short *image_Cr, const RESIDU *CurrResidu, const PPS *Pps, const short PicWidthInPix, const W_TABLES *Quantif)
{

    short DC_coeff [4];
    short AC_coeff [16];
   

    //for Cb
	if ( CurrResidu -> Cbp & 0x30){
		//Initialisation du QP 
		int QPc = qP_tab [CurrResidu -> Qp + Pps -> chroma_qp_index_offset];
		ict_4x4_dc_chroma(&CurrResidu -> ChromaDCLevel_Cb [0], DC_coeff);
		rescale_4x4_dc_chr(QPc,  DC_coeff, DC_coeff, Quantif -> W4x4_Cb_inter);
	
		rescale_4x4_residual(QPc,  &CurrResidu -> ChromaACLevel_Cb [0], AC_coeff, Quantif -> W4x4_Cb_inter);
		AC_coeff [0] = DC_coeff [0];
		SVC4x4Store(AC_coeff, image_Cb, PicWidthInPix);

		rescale_4x4_residual(QPc,  &CurrResidu -> ChromaACLevel_Cb [16], AC_coeff, Quantif -> W4x4_Cb_inter);
		AC_coeff [0] = DC_coeff [1];
		SVC4x4Store(AC_coeff, &image_Cb[4],PicWidthInPix);

		rescale_4x4_residual(QPc,  &CurrResidu -> ChromaACLevel_Cb [32], AC_coeff, Quantif -> W4x4_Cb_inter);
		AC_coeff [0] = DC_coeff [2];
		SVC4x4Store(AC_coeff, &image_Cb[PicWidthInPix << 2],PicWidthInPix);

		rescale_4x4_residual(QPc,  &CurrResidu -> ChromaACLevel_Cb [48], AC_coeff, Quantif -> W4x4_Cb_inter);
		AC_coeff [0] = DC_coeff [3];
		SVC4x4Store(AC_coeff, &image_Cb[(1 + PicWidthInPix) << 2], PicWidthInPix);

		QPc = qP_tab [CurrResidu -> Qp + Pps ->second_chroma_qp_index_offset];
		ict_4x4_dc_chroma(&CurrResidu -> ChromaDCLevel_Cr [0], DC_coeff);
		rescale_4x4_dc_chr(QPc,  DC_coeff, DC_coeff, Quantif -> W4x4_Cr_inter);


		rescale_4x4_residual(QPc,  &CurrResidu -> ChromaACLevel_Cr [0], AC_coeff, Quantif -> W4x4_Cr_inter);
		AC_coeff [0] = DC_coeff [0];
		SVC4x4Store(AC_coeff, image_Cr,PicWidthInPix);

		rescale_4x4_residual(QPc,  &CurrResidu -> ChromaACLevel_Cr [16], AC_coeff, Quantif -> W4x4_Cr_inter);
		AC_coeff [0] = DC_coeff [1];
		SVC4x4Store(AC_coeff, &image_Cr[4],PicWidthInPix);

		rescale_4x4_residual(QPc,  &CurrResidu -> ChromaACLevel_Cr [32], AC_coeff, Quantif -> W4x4_Cr_inter);
		AC_coeff [0] = DC_coeff [2];
		SVC4x4Store(AC_coeff, &image_Cr[PicWidthInPix << 2],PicWidthInPix);

		rescale_4x4_residual(QPc,  &CurrResidu -> ChromaACLevel_Cr [48], AC_coeff, Quantif -> W4x4_Cr_inter);
		AC_coeff [0] = DC_coeff [3];
		SVC4x4Store(AC_coeff, &image_Cr[(1 + PicWidthInPix) << 2], PicWidthInPix);
	}else{
		int i,j;

		for( i = 0; i < 8; i++){
			for( j = 0; j < 8; j++){
				image_Cb[i * PicWidthInPix + j] = 0;
				image_Cr[i * PicWidthInPix + j] = 0;
			}
		}
	}
}





/**
This function allow to add the residual to he base layer one in order to be used for upper layer prediction
*/
void AddResChroma (short *image_Cb, short *image_Cr, const RESIDU *CurrResidu, const PPS *Pps, const short PicWidthInPix, const W_TABLES *Quantif)
{

    short DC_coeff [4];
    short AC_coeff [16];
   

    //for Cb
	if ( CurrResidu -> Cbp & 0x30){
		//Initialisation du QP 
		int qP = CurrResidu -> Qp + Pps -> chroma_qp_index_offset ;
		int QPc = qP_tab [qP];
		ict_4x4_dc_chroma(&CurrResidu -> ChromaDCLevel_Cb [0], DC_coeff);
		rescale_4x4_dc_chr(QPc,  DC_coeff, DC_coeff, Quantif -> W4x4_Cb_inter);
	
		rescale_4x4_residual(QPc,  &CurrResidu -> ChromaACLevel_Cb [0], AC_coeff, Quantif -> W4x4_Cb_inter);
		AC_coeff [0] = DC_coeff [0];
		SVC4x4Add(AC_coeff, image_Cb, PicWidthInPix);

		rescale_4x4_residual(QPc,  &CurrResidu -> ChromaACLevel_Cb [16], AC_coeff, Quantif -> W4x4_Cb_inter);
		AC_coeff [0] = DC_coeff [1];
		SVC4x4Add(AC_coeff, &image_Cb[4],PicWidthInPix);

		rescale_4x4_residual(QPc,  &CurrResidu -> ChromaACLevel_Cb [32], AC_coeff, Quantif -> W4x4_Cb_inter);
		AC_coeff [0] = DC_coeff [2];
		SVC4x4Add(AC_coeff, &image_Cb[PicWidthInPix << 2],PicWidthInPix);

		rescale_4x4_residual(QPc,  &CurrResidu -> ChromaACLevel_Cb [48], AC_coeff, Quantif -> W4x4_Cb_inter);
		AC_coeff [0] = DC_coeff [3];
		SVC4x4Add(AC_coeff, &image_Cb[(1 + PicWidthInPix) << 2], PicWidthInPix);

		qP = CurrResidu -> Qp + Pps -> second_chroma_qp_index_offset ;
		QPc = qP_tab [qP];
		ict_4x4_dc_chroma(&CurrResidu -> ChromaDCLevel_Cr [0], DC_coeff);
		rescale_4x4_dc_chr(QPc,  DC_coeff, DC_coeff, Quantif -> W4x4_Cr_inter);


		rescale_4x4_residual(QPc,  &CurrResidu -> ChromaACLevel_Cr [0], AC_coeff, Quantif -> W4x4_Cr_inter);
		AC_coeff [0] = DC_coeff [0];
		SVC4x4Add(AC_coeff, image_Cr,PicWidthInPix);

		rescale_4x4_residual(QPc,  &CurrResidu -> ChromaACLevel_Cr [16], AC_coeff, Quantif -> W4x4_Cr_inter);
		AC_coeff [0] = DC_coeff [1];
		SVC4x4Add(AC_coeff, &image_Cr[4],PicWidthInPix);

		rescale_4x4_residual(QPc,  &CurrResidu -> ChromaACLevel_Cr [32], AC_coeff, Quantif -> W4x4_Cr_inter);
		AC_coeff [0] = DC_coeff [2];
		SVC4x4Add(AC_coeff, &image_Cr[PicWidthInPix << 2],PicWidthInPix);

		rescale_4x4_residual(QPc,  &CurrResidu -> ChromaACLevel_Cr [48], AC_coeff, Quantif -> W4x4_Cr_inter);
		AC_coeff [0] = DC_coeff [3];
		SVC4x4Add(AC_coeff, &image_Cr[(1 + PicWidthInPix) << 2], PicWidthInPix);
	}
}




/**
For Scoeff prediction.
In this case, we have to add the lower rescaled coefficient to those of the current layer, in order to determine the firght prediction.
This function is used in inter prediction, and the resukt is stored in the picture of residual.
*/
void SCoeffChromaAddRes (short *image_Cb, short *image_Cr, const RESIDU *CurrResidu, const short PicWidthInPix)
{

    short AC_coeff [16];

	memcpy(AC_coeff, &CurrResidu -> ChromaACLevel_Cb [0], 16 * sizeof(short));
	SVC4x4Add(AC_coeff, image_Cb, PicWidthInPix);

	//Second  block 4x4
	memcpy(AC_coeff, &CurrResidu -> ChromaACLevel_Cb [16], 16 * sizeof(short));
	SVC4x4Add(AC_coeff, &image_Cb[4],PicWidthInPix);

	memcpy(AC_coeff, &CurrResidu -> ChromaACLevel_Cb [32], 16 * sizeof(short));
	SVC4x4Add(AC_coeff, &image_Cb[PicWidthInPix << 2],PicWidthInPix);

	memcpy(AC_coeff, &CurrResidu -> ChromaACLevel_Cb [48], 16 * sizeof(short));
	SVC4x4Add(AC_coeff, &image_Cb[(1 + PicWidthInPix) << 2], PicWidthInPix);

	//Cr
	memcpy(AC_coeff, &CurrResidu -> ChromaACLevel_Cr [0], 16 * sizeof(short));
	SVC4x4Add(AC_coeff, image_Cr, PicWidthInPix);

	//Second  block 4x4
	memcpy(AC_coeff, &CurrResidu -> ChromaACLevel_Cr [16], 16 * sizeof(short));
	SVC4x4Add(AC_coeff, &image_Cr[4],PicWidthInPix);

	memcpy(AC_coeff, &CurrResidu -> ChromaACLevel_Cr [32], 16 * sizeof(short));
	SVC4x4Add(AC_coeff, &image_Cr[PicWidthInPix << 2],PicWidthInPix);

	memcpy(AC_coeff, &CurrResidu -> ChromaACLevel_Cr [48], 16 * sizeof(short));
	SVC4x4Add(AC_coeff, &image_Cr[(1 + PicWidthInPix) << 2], PicWidthInPix);
}




/**
For Scoeff prediction.
In this case, we have to add the lower rescaled coefficient to those of the current layer, in order to determine the firght prediction.
This function is used in inter prediction, and the resukt is added in the picture of residual.
*/
void SCoeffChromaAddPic (unsigned char *image_Cb, unsigned char *image_Cr, const RESIDU *CurrResidu, const short PicWidthInPix)
{

    short AC_coeff [16];

	memcpy(AC_coeff, &CurrResidu -> ChromaACLevel_Cb [0], 16 * sizeof(short));
	ict_4x4_residual(AC_coeff, image_Cb, PicWidthInPix);

	//Second  block 4x4
	memcpy(AC_coeff, &CurrResidu -> ChromaACLevel_Cb [16], 16 * sizeof(short));
	ict_4x4_residual(AC_coeff, &image_Cb[4],PicWidthInPix);

	memcpy(AC_coeff, &CurrResidu -> ChromaACLevel_Cb [32], 16 * sizeof(short));
	ict_4x4_residual(AC_coeff, &image_Cb[PicWidthInPix << 2],PicWidthInPix);

	memcpy(AC_coeff, &CurrResidu -> ChromaACLevel_Cb [48], 16 * sizeof(short));
	ict_4x4_residual(AC_coeff, &image_Cb[(1 + PicWidthInPix) << 2], PicWidthInPix);

	//Cr
	memcpy(AC_coeff, &CurrResidu -> ChromaACLevel_Cr [0], 16 * sizeof(short));
	ict_4x4_residual(AC_coeff, image_Cr, PicWidthInPix);

	//Second  block 4x4
	memcpy(AC_coeff, &CurrResidu -> ChromaACLevel_Cr [16], 16 * sizeof(short));
	ict_4x4_residual(AC_coeff, &image_Cr[4],PicWidthInPix);

	memcpy(AC_coeff, &CurrResidu -> ChromaACLevel_Cr [32], 16 * sizeof(short));
	ict_4x4_residual(AC_coeff, &image_Cr[PicWidthInPix << 2],PicWidthInPix);

	memcpy(AC_coeff, &CurrResidu -> ChromaACLevel_Cr [48], 16 * sizeof(short));
	ict_4x4_residual(AC_coeff, &image_Cr[(1 + PicWidthInPix) << 2], PicWidthInPix);
}
