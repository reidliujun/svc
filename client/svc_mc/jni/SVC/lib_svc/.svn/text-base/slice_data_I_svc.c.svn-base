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


#include <stdio.h>
#include <string.h>

//AVC files
#include "type.h"
#include "neighbourhood.h"
#include "SliceGroupMap.h"


//SVC files
#include "svc_data.h"
#include "Mb_I_svc.h"
#include "Loop_filter_ext.h"
#include "slice_data_I_svc.h"
#include "svc_neighbourhood.h"
#include "InterLayerPrediction.h"



/**
This function decodes an SVC I frame.
*/
void Decode_I_svc (const int size_Mb, const SPS *ai_pstSps, const PPS *ai_pstPps, const SLICE *Slice, 
				   const NAL * Nal, const unsigned char *SliceTable, DATA *Block, RESIDU *all_residu, 
				   STRUCT_PF *baseline_vector, LIST_MMO *Current_pic, const W_TABLES *quantif, 
				   unsigned char *DpbLuma, unsigned char *DpbCb, unsigned char  *DpbCr)
{



	if (!Nal -> NalToSkip){
		RESIDU *CurrResidu = &all_residu[Nal -> LayerId * size_Mb];
		RESIDU *BaseResidu = &all_residu[Nal -> BaseLayerId * size_Mb];

		int pic_parameter_id = Nal -> pic_parameter_id[Nal -> LayerId];
		const PPS *Pps = &ai_pstPps[pic_parameter_id];
		const SPS *Sps = &ai_pstSps[Pps -> seq_parameter_set_id + 16];
		int MemoryAddress = Nal -> BaseMemoryAddress;
		short iCurrMbAddr = Slice -> first_mb_in_slice;
		const short PicSizeInMbs = Sps -> PicSizeInMbs;
		const short PicWiInMbs = Sps -> pic_width_in_mbs;
		const short PicWidthInMbs = Sps -> pic_width_in_mbs + 2;
		const short PicWidthInPix = PicWidthInMbs * 16 ;
		const short offset_L = (PicWidthInMbs << 8) + 16;
		const short offset_C = (PicWidthInMbs << 6) + 8;  

		short iMb_x;
		short iMb_y;

		unsigned char *Y = &DpbLuma[offset_L + Current_pic -> MemoryAddress];
		unsigned char *U = &DpbCb[offset_C + (Current_pic -> MemoryAddress >> 2)];
		unsigned char *V = &DpbCr[offset_C + (Current_pic -> MemoryAddress >> 2)];
		unsigned char *BaseY = &DpbLuma[MemoryAddress + 16 + ((Nal -> base_Width + 32) << 4)];
		unsigned char *BaseU = &DpbCb[(MemoryAddress >> 2) + 8 + ((Nal -> base_Width + 32) << 2)];
		unsigned char *BaseV = &DpbCr[(MemoryAddress >> 2) + 8 + ((Nal -> base_Width + 32) << 2)];


		//initialize macroblock position
		GetMbPosition(iCurrMbAddr, PicWiInMbs, &iMb_x, &iMb_y);

		//Applies deblocking filter on the base layer if necessary
		GetBaseFilter(Nal, Slice, ai_pstPps, ai_pstSps, BaseResidu, BaseY, BaseU, BaseV);

		//Loop on all macroblocks
		for ( ; iCurrMbAddr < PicSizeInMbs ;) {

			if (IS_I(CurrResidu [iCurrMbAddr] . MbType) || !Nal -> SpatialScalability) {
				//For all SNR enhancement
				// I macroblock, or Scoeff and Tcoeff prediction are treated
				GetNeighbour(iCurrMbAddr, iMb_x, PicWiInMbs, PicSizeInMbs, SliceTable, CurrResidu, Pps -> constrained_intra_pred_flag);

				MbISnr(Pps, Nal, quantif, baseline_vector, &CurrResidu [iCurrMbAddr], &BaseResidu [iCurrMbAddr], PicWidthInPix, 
					&Y[((iMb_x + (iMb_y * PicWidthInPix)) << 4)], 
					&U[((iMb_x + (iMb_y * PicWidthInPix >> 1)) << 3)], 
					&V[((iMb_x + (iMb_y * PicWidthInPix >> 1)) << 3)], 
					&BaseY[((iMb_x + (iMb_y * PicWidthInPix)) << 4)], 
					&BaseU[((iMb_x + (iMb_y * PicWidthInPix >> 1)) << 3)], 
					&BaseV[((iMb_x + (iMb_y * PicWidthInPix >> 1)) << 3)]);

			}else{
				MbISpatial (Nal, Sps, Pps, PicWidthInPix, quantif, iMb_x, iMb_y, &Block [iCurrMbAddr], &CurrResidu [iCurrMbAddr], BaseResidu, 
					&Y[((iMb_x + (iMb_y * PicWidthInPix)) << 4)], &U[((iMb_x + (iMb_y * PicWidthInPix >> 1)) << 3)], 
					&V[((iMb_x + (iMb_y * PicWidthInPix >> 1)) << 3)], BaseY, BaseU, BaseV);
			}

			//Updating the macroblock address
			iCurrMbAddr = Next_MbAddress(iCurrMbAddr, PicSizeInMbs, PicWiInMbs, SliceTable, &iMb_x, &iMb_y);
			//Used to stop the deconding process when there is slices.
			if ( SliceTable [iCurrMbAddr] == 255 ) {
				iCurrMbAddr = PicSizeInMbs ;
			}
		}
		
	}
}


