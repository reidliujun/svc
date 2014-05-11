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


#include "IDR_picture.h"
#include "neighbourhood.h"
#include "decode_mb_I.h"
#include "SliceGroupMap.h"
#include "data.h"


/**
This function allows to start the reconstruction of an IDR picture. 


@param Sps The sps structure.
@param Pps The pps structure.
@param Slice The slice structure contains all information used to decode the next slice
@param aio_pstSliceGroupMap Specifies in which Slice belongs each macroblock of the picture.
@param SliceTable Specifies in which Slice belongs each macroblock of the picture.
@param aio_tstBlock Contains all parameters of each macroblock of the current picture.
@param intra_pre Table which contains all datas need to obtain the intra 4x4 prediction mode.
@param picture_residu The structure which contains all the residual data of the macroblocks.
@param quantif W_TABLES which contains all the quantization table.
@param ai_struct_pf Function pointer for interpolation
@param image Table of current frame.
@param image_Cb Table of current frame.
@param image_Cr Table of current frame.
*/

void IDR_picture( const SPS *Sps, const PPS * Pps, const SLICE *Slice, const unsigned char *SliceTable, 
				 RESIDU *picture_residu, W_TABLES *quantif , const STRUCT_PF * ai_struct_pf,
				 unsigned char *aio_tucImage, unsigned char *aio_tucImage_Cb, unsigned char  *aio_tucImage_Cr)
{
	
	if ( Sps -> PicSizeInMbs){

		const short PicSizeInMbs = Sps -> PicSizeInMbs ;
		const short PicWiInMbs = Sps -> pic_width_in_mbs;
		const short PicWidthInMbs = Sps -> pic_width_in_mbs + 2;
		const short offset_L = (PicWidthInMbs << 8) + 16;
		const short offset_C = (PicWidthInMbs << 6) + 8;  

		short iCurrMbAddr = Slice -> first_mb_in_slice ;


		short iMb_x;
		short iMb_y;
		
		//initialize macroblock position
		GetMbPosition(iCurrMbAddr, PicWiInMbs, &iMb_x, &iMb_y);

		aio_tucImage += offset_L;
		aio_tucImage_Cb += offset_C;
		aio_tucImage_Cr += offset_C;

		//Loop on all macroblocks
		for ( ; iCurrMbAddr < PicSizeInMbs ;) {

			//Initialization of the macroblock neighbourhood
			GetNeighbour(iCurrMbAddr, iMb_x, PicWiInMbs, PicSizeInMbs, SliceTable, picture_residu, Pps -> constrained_intra_pred_flag);

			//Construction process for one macroblock
			decode_MB_I(&aio_tucImage[((iMb_x + (iMb_y * PicWidthInMbs << 4)) << 4)], 
				&aio_tucImage_Cb[((iMb_x + (iMb_y * PicWidthInMbs << 3)) << 3)], 
				&aio_tucImage_Cr[((iMb_x + (iMb_y * PicWidthInMbs << 3)) << 3)], Pps, 
				&picture_residu[iCurrMbAddr], PicWidthInMbs << 4, quantif, ai_struct_pf);

			//Updating the macroblock address
			iCurrMbAddr = Next_MbAddress(iCurrMbAddr, PicSizeInMbs, PicWiInMbs, SliceTable, &iMb_x, &iMb_y);
			if ( SliceTable [iCurrMbAddr] == 255 ) {
				iCurrMbAddr = PicSizeInMbs ;
			}
		}
	}
}




