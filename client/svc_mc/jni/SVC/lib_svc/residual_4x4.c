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
#include "Transform.h"
#include "quantization.h"


//SVC Files
#include "ict_svc.h"
#include "residual_4x4.h"
#include "ResidualProcessing.h"



/**
This function allows to add the residual to the base layer one in order to be used for upper layer prediction
*/
void AddRes4x4 ( short *image, const RESIDU *CurrResidu, const short PicWidthInPix, const short W4x4_inter [6][16])
{
    int i8x8 ;
    short luma_coeff [16];
    
    //Boucle sur chaque block 
    for ( i8x8 = 0 ; i8x8 < 4 ; i8x8++ ) {
        if (CurrResidu -> Cbp & (1 << i8x8)){    
			/* each luma 8x8 block */
            int locx = ((i8x8 & 1) << 3);
            int locy = ((i8x8 & 2) << 2);
            
            /* each 4x4 sub-block of block */
            int index = i8x8 << 6;
            rescale_4x4_residual(CurrResidu -> Qp, &CurrResidu -> LumaLevel [index], luma_coeff, W4x4_inter);
            SVC4x4Add(luma_coeff, &image[locx + locy * PicWidthInPix],  PicWidthInPix);

            index += 16;
            rescale_4x4_residual(CurrResidu -> Qp, &CurrResidu -> LumaLevel [index], luma_coeff, W4x4_inter);
            SVC4x4Add(luma_coeff, &image[locx + 4 + locy * PicWidthInPix],  PicWidthInPix);

            index += 16;
            rescale_4x4_residual(CurrResidu -> Qp, &CurrResidu -> LumaLevel [index], luma_coeff, W4x4_inter);
            SVC4x4Add(luma_coeff, &image[locx + (locy + 4) * PicWidthInPix],  PicWidthInPix);

            index += 16;
            rescale_4x4_residual(CurrResidu -> Qp, &CurrResidu -> LumaLevel [index], luma_coeff, W4x4_inter);
            SVC4x4Add(luma_coeff, &image[4 + locx + (locy + 4) * PicWidthInPix],  PicWidthInPix);
		}
    }
}



/**
This function allows to store the residual in a short picture in order to be used for upper layer prediction
*/
void StoreRes4x4 ( short *image, const RESIDU *CurrResidu,  const short PicWidthInPix, const short W4x4_inter [6][16])
{
	int i8x8, i, j ;
	int index ;
	short luma_coeff [16];

	//Boucle sur chaque block 
	for ( i8x8 = 0 ; i8x8 < 4 ; i8x8++ ) {        
		if (CurrResidu -> Cbp & (1 << i8x8)) {    
			/* each luma 8x8 block */
			int locx = ((i8x8 & 1) << 3);
			int locy = ((i8x8 & 2) << 2);

			/* each 4x4 sub-block of block */
			index = i8x8 << 6;
			rescale_4x4_residual(CurrResidu -> Qp, &CurrResidu -> LumaLevel [index], luma_coeff, W4x4_inter);
			SVC4x4Store(luma_coeff, &image[locx + locy * PicWidthInPix], PicWidthInPix);

			index += 16;
			rescale_4x4_residual(CurrResidu -> Qp, &CurrResidu -> LumaLevel [index], luma_coeff, W4x4_inter);
			SVC4x4Store(luma_coeff, &image[locx + 4 + locy * PicWidthInPix], PicWidthInPix);

			index += 16;
			rescale_4x4_residual(CurrResidu -> Qp, &CurrResidu -> LumaLevel [index], luma_coeff, W4x4_inter);
			SVC4x4Store(luma_coeff, &image[locx + (locy + 4) * PicWidthInPix], PicWidthInPix);

			index += 16;
			rescale_4x4_residual(CurrResidu -> Qp, &CurrResidu -> LumaLevel [index], luma_coeff, W4x4_inter);
			SVC4x4Store(luma_coeff, &image[4 + locx + (locy + 4) * PicWidthInPix], PicWidthInPix);

		}else{
			int locx = ((i8x8 & 1) << 3);
			int locy = ((i8x8 & 2) << 2);
			for( i = 0; i < 8; i++){
				for ( j = 0; j < 8; j++){
					image[locx + i + (j + locy) * PicWidthInPix] = 0;
				}
			}
		}
	}
}


/**
For Scoeff prediction.
In this case, we have to add the lower rescaled coefficient to those of the current layer, in order to determine the firght prediction.
This function is used in inter prediction, and the resukt is stored in the picture of residual.
*/
void SCoeff4x4AddRes (short *image, RESIDU *CurrResidu, const short PicWidthInPix)
{
    short i8x8;
   
    //Boucle sur chaque block 
    for ( i8x8 = 0 ; i8x8 < 4 ; i8x8++ ) {   
		/* each luma 8x8 block */
		short locx = ((i8x8 & 1) << 3);
		short locy = ((i8x8 & 2) << 2);

		/* each 4x4 sub-block of block */
		short index = i8x8 << 6;
		SVC4x4Add(&CurrResidu -> LumaLevel [index], &image[locx + locy * PicWidthInPix], PicWidthInPix);

		//next block 4x4
		index += 16;
		SVC4x4Add(&CurrResidu -> LumaLevel [index], &image[locx + 4 + locy * PicWidthInPix], PicWidthInPix);

		index += 16;
		SVC4x4Add(&CurrResidu -> LumaLevel [index], &image[locx + (locy + 4) * PicWidthInPix], PicWidthInPix);

		index += 16;
		SVC4x4Add(&CurrResidu -> LumaLevel [index], &image[4 + locx + (locy + 4) * PicWidthInPix], PicWidthInPix);
	}
}



/**
For Scoeff prediction.
In this case, we have to add the lower rescaled coefficient to those of the current layer, in order to determine the firght prediction.
This function is used in inter prediction, and the result is added in the picture of residual.
*/
void SCoeff4x4AddPic ( unsigned char *image, RESIDU *CurrResidu, const short PicWidthInPix)
{
    short i8x8;

    //Boucle sur chaque block 
    for ( i8x8 = 0 ; i8x8 < 4 ; i8x8++ ) {   
		/* each luma 8x8 block */
		short locx = ((i8x8 & 1) << 3);
		short locy = ((i8x8 & 2) << 2);

		/* each 4x4 sub-block of block */
		short index = i8x8 << 6;
		ict_4x4_residual(&CurrResidu -> LumaLevel [index], &image[locx + locy * PicWidthInPix], PicWidthInPix);

		//next block 4x4
		index += 16;
		ict_4x4_residual(&CurrResidu -> LumaLevel [index], &image[locx + 4 + locy * PicWidthInPix], PicWidthInPix);

		index += 16;
		ict_4x4_residual(&CurrResidu -> LumaLevel [index], &image[locx + (locy + 4) * PicWidthInPix], PicWidthInPix);

		index += 16;
		ict_4x4_residual(&CurrResidu -> LumaLevel [index], &image[4 + locx + (locy + 4) * PicWidthInPix], PicWidthInPix);
	}
}
