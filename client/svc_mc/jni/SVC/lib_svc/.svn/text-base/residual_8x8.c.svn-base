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
#include "transform_8x8.h"
#include "quantization.h"



//SVC Files
#include "ict_svc.h"
#include "residual_8x8.h"
#include "ResidualProcessing.h"





/**
This function allow to add the residual to the base layer one in order to be used for upper layer prediction
*/
void AddRes8x8 ( short *image, const RESIDU *CurrResidu, const short PicWidthInPix, const short W8x8_inter [6][64])
{
	int i8x8 ;

	short luma_coeff [64];

	//Boucle sur chaque block 
	for ( i8x8 = 0 ; i8x8 < 4 ; i8x8++ ) {        
		if (CurrResidu -> Cbp & (1 << i8x8)){

			/* each luma 8x8 block */
			int locx =((i8x8 & 1) << 3);
			int locy =((i8x8 & 2) << 2);

			rescale_8x8_residual(CurrResidu -> Qp, &CurrResidu -> LumaLevel [i8x8 << 6], luma_coeff, W8x8_inter);
			SVC8x8Add(luma_coeff, &image[locx + locy * PicWidthInPix], PicWidthInPix );
		}
	}
}




/**
This function allow to store the residual in a short picture in order to be used for upper layer prediction
*/
void StoreRes8x8 ( short *image, const RESIDU *CurrResidu, const short PicWidthInPix, const short W8x8_inter [6][64])
{
	int i8x8, i, j ;

	short luma_coeff [64];

	//Boucle sur chaque block 
	for ( i8x8 = 0 ; i8x8 < 4 ; i8x8++ ) {        
		if (CurrResidu -> Cbp & (1 << i8x8)){
			/* each luma 8x8 block */
			int locx =((i8x8 & 1) << 3);
			int locy =((i8x8 & 2) << 2);

			rescale_8x8_residual(CurrResidu -> Qp, &CurrResidu -> LumaLevel [i8x8 << 6], luma_coeff, W8x8_inter);
			SVC8x8Store(luma_coeff, &image[locx + locy * PicWidthInPix], PicWidthInPix );
		}else{
			int locx =((i8x8 & 1) << 3);
			int locy =((i8x8 & 2) << 2);
			for( i = 0; i < 8; i++)
				for ( j = 0; j < 8; j++){
					image[locx + i + (j + locy) * PicWidthInPix] = 0;
				}
		}
	}
}



/* 
This function is not working at all.
Base layer coefficient have to be added!!!!
*/
void SCoeff8x8AddRes (short *image, const RESIDU *CurrResidu, const short PicWidthInPix)
{
	int i8x8;

	short luma_coeff [64];

	//Boucle sur chaque block 
	for ( i8x8 = 0 ; i8x8 < 4 ; i8x8++ ) {      
		/* each luma 8x8 block */
		int locx =((i8x8 & 1) << 3);
		int locy =((i8x8 & 2) << 2);

		memcpy(luma_coeff, &CurrResidu -> LumaLevel [i8x8 << 6], 64 * sizeof(short));
		SVC8x8Add(luma_coeff, &image[locx + locy * PicWidthInPix], PicWidthInPix );
	}
}


/* 
This function is not working at all.
Base layer coefficient have to be added!!!!
*/
void SCoeff8x8AddPic ( unsigned char *Y, const RESIDU *CurrResidu, const short PicWidthInPix)
{
	int i8x8;

	short luma_coeff [64];

	//Boucle sur chaque block 
	for ( i8x8 = 0 ; i8x8 < 4 ; i8x8++ ) {      
		/* each luma 8x8 block */
		int locx =((i8x8 & 1) << 3);
		int locy =((i8x8 & 2) << 2);

		memcpy(luma_coeff, &CurrResidu -> LumaLevel [i8x8 << 6], 64 * sizeof(short));
		ict_8x8(luma_coeff, &Y[locx + locy * PicWidthInPix], PicWidthInPix );
	}
}



/**
For Scoeff prediction.
In this case, we have to add the lower rescaled coefficient to those of the current layer, in order to determine the firght prediction.
This function is used in intra prediction, and the result is store directly in the picture.

This function is not working at all.
Base layer coefficient have to be added!!!!
*/
void SCoeffIntra8x8 ( unsigned char *Y, const RESIDU *CurrResidu, const short PicWidthInPix, const short W8x8_inter [6][64])
{
	int i8x8;

	short luma_coeff [64];

	//Boucle sur chaque block 
	for ( i8x8 = 0 ; i8x8 < 4 ; i8x8++ ) {    
		if (CurrResidu -> Cbp & (1 << i8x8)){
			// each luma 8x8 block 
			int locx =((i8x8 & 1) << 3);
			int locy =((i8x8 & 2) << 2);

			rescale_8x8_residual(CurrResidu -> Qp, &CurrResidu -> LumaLevel [i8x8 << 6], luma_coeff, W8x8_inter);
			ict_8x8(luma_coeff, &Y[locx + locy * PicWidthInPix], PicWidthInPix );
		}
	}
}
