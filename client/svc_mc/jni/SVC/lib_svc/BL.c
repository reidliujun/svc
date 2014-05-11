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
#include "data.h"

//SVC Files
#include "svc_type.h"
#include "svc_data.h"
#include "BL.h"
#include "I_BL.h"
#include "mb_padding.h"




void PaddIPart(const NAL *Nal, REFPART *RefPartition, unsigned char *Lpix, unsigned char *Cbpix, unsigned char *Crpix){


	const short Lstride = Nal -> base_Width + 32;
	const short Cstride = Lstride >> 1;
	merge(8, 8, -Lstride, &Lpix[Lstride], Lpix, RefPartition[10] . RefIdc == -2, -Lstride, -1);
	merge(4, 4, -Cstride, &Cbpix[Cstride], Cbpix, RefPartition[10] . RefIdc == -2, -Cstride, -1);
	merge(4, 4, -Cstride, &Crpix[Cstride], Crpix, RefPartition[10] . RefIdc == -2, -Cstride, -1);
	
	//We have to merge the third block 8x8 with the second and third one if both are present.
	if (RefPartition[10] . RefIdc != -2){
		merge(8, 8, Lstride, &Lpix[1], &Lpix[1 + Lstride], 0, Lstride, 1);
		merge(4, 4, Cstride, &Cbpix[1], &Cbpix[1 + Cstride], 0, Cstride, 1);
		merge(4, 4, Cstride, &Crpix[1], &Crpix[1 + Cstride], 0, Cstride, 1);
	}
}



/**
This function upsample an Intra macroblock to get a part of the current macroblock for non dyadic upsampling.
The result is stored in a temporary table, in order to be saved during the inter prediction.
*/
void DecodeIPart(const NAL *Nal, const PPS *Pps, const SPS *Sps, REFPART *RefPartition, 
				 RESIDU *BaseResidu, const short PicWidthInPix, short mbBaseAddr, 
				 short x, short y, short x_base, short y_base, short x_mbPartIdx, short y_mbPartIdx, 
				 unsigned char BaseY[ ], unsigned char BaseCb[ ], unsigned char BaseCr[ ], 
				 unsigned char Y[ ], unsigned char U[ ], unsigned char V[ ],
				 unsigned char MbPredY[ ], unsigned char MbPredU[ ], unsigned char MbPredV[ ])
{


	//Get the type for each 8x8 block 
	short cX = 8;
	short cY = 8;
	unsigned char *Y_up = Y;
	unsigned char *U_up = U;
	unsigned char *V_up = V;

	//We know, that at least one 8x8 block is predicted from an I base macroblock,
	//but the macroblock can be partially predicted in Intra mode,
	//so we have to find which block and which base macroblock we need.
	if (!IS_I_SVC(BaseResidu [mbBaseAddr] . MbType)){
		//In this case, the first block 8x8 is not intra predicted
		//We look how to get the right prediction
		int done = 0;
		int xdecal = RefPartition[8] . RefIdc != -2;
		int ydecal = RefPartition[2] . RefIdc != -2;

		//If the second 8x8 block is Intra predicted, we initialized all parameters with it's base macroblock
		if ( RefPartition[2] . RefIdc == -2 && xdecal) {
			mbBaseAddr = (short) (RefPartition[2] . PartIdc >> 4);
			x_mbPartIdx = (short) ((RefPartition[2] . PartIdc & 0x0f) & 0x03);
			y_mbPartIdx = (short) ((RefPartition[2] . PartIdc & 0x0f) >> 2);
	
			//initialize macroblock position
			GetMbPosition(mbBaseAddr, Nal -> base_Width >> 4, &x_base, &y_base);
			Y_up += 8;
			U_up += 4;
			V_up += 4;
			x += 8;
			done = 1;
		}
		
		//If the third 8x8 block is Intra predicted, we initialized all parameters with it's base macroblock
		if ( !done && RefPartition[8] . RefIdc == -2 && ydecal) {
			mbBaseAddr = (short) (RefPartition[8] . PartIdc >> 4);
			x_mbPartIdx = (short) ((RefPartition[8] . PartIdc & 0x0f) & 0x03);
			y_mbPartIdx = (short) ((RefPartition[8] . PartIdc & 0x0f) >> 2);

			//initialize macroblock position
			GetMbPosition(mbBaseAddr, Nal -> base_Width >> 4, &x_base, &y_base);
			Y_up += (PicWidthInPix << 3);
			U_up += (PicWidthInPix << 1);
			V_up += (PicWidthInPix << 1);
			y += 8;
			done = 1;
		}

		//If the fourth 8x8 block is Intra predicted, we initialized all parameters with it's base macroblock
		if ( !done && xdecal && ydecal) {
			mbBaseAddr = (short) (RefPartition[10] . PartIdc >> 4);
			x_mbPartIdx = (short) ((RefPartition[10] . PartIdc & 0x0f) & 0x03);
			y_mbPartIdx = (short) ((RefPartition[10] . PartIdc & 0x0f) >> 2);

			//initialize macroblock position
			GetMbPosition(mbBaseAddr, Nal -> base_Width >> 4, &x_base, &y_base);
			Y_up += 8 + (PicWidthInPix << 3);
			U_up += 4 + (PicWidthInPix << 1);
			V_up += 4 + (PicWidthInPix << 1);
			x += 8;
			y += 8;
			done = 1;
		}

		//In case of at least 2 blocks 8x8 are intra predicted but not the first one
		//We know, if this case have to be used, that not the xdecal and ydecal have been done.
		//So the second block and the third one are present.
		if(!done){
			//We have to merge the first block 8x8 with the second and third one if both are present.
			const int LCurrPos = (((x_base << 2) + x_mbPartIdx) << 2) + 7 + ((((y_base << 2) + y_mbPartIdx) << 2) + 7) * (Nal -> base_Width + 32);
			const int CCurrPos = (((x_base << 2) + x_mbPartIdx) << 1) + 3 + ((((y_base << 2) + y_mbPartIdx) << 1) + 3) * ((Nal -> base_Width + 32) >> 1); 

			PaddIPart(Nal, RefPartition, &BaseY[LCurrPos], &BaseCb[CCurrPos], &BaseCr[CCurrPos]);
		}
	}


	//Upsample I base macroblock
	MbIPad(Nal, Pps, Sps, mbBaseAddr, x, y, x_base, y_base, x_mbPartIdx, y_mbPartIdx, BaseResidu, BaseY, BaseCb, BaseCr, Y_up, U_up, V_up);

	//Save the inter layer prediction if necessary
	if ( RefPartition[0] . RefIdc == -2) {
		copy_2d (cY, cX, PicWidthInPix, 16, Y, MbPredY);
		copy_2d (cY >> 1, cX >> 1,PicWidthInPix >> 1, 8, U, MbPredU);
		copy_2d (cY >> 1, cX >> 1,PicWidthInPix >> 1, 8, V, MbPredV);
	}

	if (cX < 16 &&  RefPartition[2] . RefIdc == -2) {
		copy_2d (cY, 16 - cX,PicWidthInPix, 16, &Y[cX], &MbPredY[cX]);
		copy_2d (cY >> 1, 8 - (cX >> 1), PicWidthInPix >> 1, 8, &U[cX >> 1], &MbPredU[cX >> 1]);
		copy_2d (cY >> 1, 8 - (cX >> 1), PicWidthInPix >> 1, 8, &V[cX >> 1], &MbPredV[cX >> 1]);
	}

	if ( RefPartition[8] . RefIdc == -2) {
		copy_2d (16 - cY, cX,PicWidthInPix, 16, &Y[cY * PicWidthInPix], &MbPredY[cY * 16]);
		copy_2d (8 - (cY >> 1), cX >> 1, PicWidthInPix >> 1, 8, &U[(cY >> 1) * (PicWidthInPix >> 1)], &MbPredU[(cY >> 1) * 8] );
		copy_2d (8 - (cY >> 1), cX >> 1, PicWidthInPix >> 1, 8, &V[(cY >> 1) * (PicWidthInPix >> 1)], &MbPredV[(cY >> 1) * 8] );
	}

	if ( RefPartition[10] . RefIdc == -2) {
		copy_2d (16 - cY, 16 - cX, PicWidthInPix, 16, &Y[cX + cY * PicWidthInPix], &MbPredY[cX + cY * 16] );
		copy_2d (8 - (cY >> 1), 8 - (cX >> 1), PicWidthInPix >> 1, 8, &U[(cX >> 1) + (cY >> 1) * (PicWidthInPix >> 1)], &MbPredU[(cX >> 1) + (cY >> 1) * 8] );
		copy_2d (8 - (cY >> 1), 8 - (cX >> 1), PicWidthInPix >> 1, 8, &V[(cX >> 1) + (cY >> 1) * (PicWidthInPix >> 1)], &MbPredV[(cX >> 1) + (cY >> 1) * 8] );
	}
}







void GetNonDiadicSample(const int ResidualPrediction, short RefIdc, short cX, short cY, const short PicWidthInPix
						, unsigned char *Pred, unsigned char *IntraPred, short *BaseResidu, short *CurrResidu){

	
	short i,j;
	// Get back the Intra interpolation when necessary 
	// Or add to residual the base residual if necessary
	// And add the residual to prediction
	if ( RefIdc == -2) {
		copy_2d (cY, cX, cX << 1, PicWidthInPix, IntraPred, Pred);
	} else if (ResidualPrediction){
		for ( i = 0; i < cX; i++){
			for(j = 0; j < cY; j++){
				CurrResidu[(i * (cX << 1)) + j] = CurrResidu[(i * (cX << 1)) + j] + BaseResidu[j + i * PicWidthInPix];
			}
		}
	}
	add_clip_2d (cY, cX, cX << 1, PicWidthInPix, CurrResidu , Pred);
}

