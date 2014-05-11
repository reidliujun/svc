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
#include "type.h"
#include "data.h"
#include "decode_mb_I.h"
#include "SliceGroupMap.h"
#include "neighbourhood.h"



//SVC Files
#include "svc_type.h"




/**
This function rebuilds the frame according to datas decoded from the VLD for a P AVC slice.
*/
void Decode_I_avc( const SPS *Sps, const PPS *Pps, const SLICE *ai_pstSlice, const unsigned char *SliceTable, 
				  RESIDU *picture_residu, const STRUCT_PF *ai_struct_pf, 
				  const W_TABLES *quantif, NAL *Nal, unsigned char *Y, unsigned char *U, unsigned char *V)
{

	if (!Nal -> NalToSkip){
		const short PicSizeInMbs = Sps -> PicSizeInMbs ;
		const short PicWiInMbs = Sps -> pic_width_in_mbs;
		const short PicWidthInMbs = Sps -> pic_width_in_mbs + 2;
		const short offset_L = (PicWidthInMbs << 8) + 16;
		const short offset_C = (PicWidthInMbs << 6) + 8;  

		short iCurrMbAddr = ai_pstSlice -> first_mb_in_slice ;

		short iMb_x;
		short iMb_y;
		
		//initialize macroblock position
		GetMbPosition(iCurrMbAddr, PicWiInMbs, &iMb_x, &iMb_y);

		Y += offset_L;
		U += offset_C;
		V += offset_C;

		//Loop on all macroblocks
		for ( ; iCurrMbAddr < PicSizeInMbs ;) {

			//Initialization of the macroblock neighbourhood
			GetNeighbour(iCurrMbAddr, iMb_x, PicWiInMbs, PicSizeInMbs, SliceTable, picture_residu, Pps -> constrained_intra_pred_flag);

			//Construction process for one macroblock
			decode_MB_I(&Y[(iMb_x + (iMb_y * PicWidthInMbs << 4)) << 4], &U[(iMb_x + (iMb_y * PicWidthInMbs << 3)) << 3], 
				&V[(iMb_x + (iMb_y * PicWidthInMbs << 3)) << 3], Pps, &picture_residu[iCurrMbAddr], PicWidthInMbs << 4, quantif, ai_struct_pf);

			//Updating the macroblock address
			iCurrMbAddr = Next_MbAddress(iCurrMbAddr, PicSizeInMbs, PicWiInMbs, SliceTable, &iMb_x, &iMb_y);
			if ( SliceTable [iCurrMbAddr] == 255 ) {
				iCurrMbAddr = PicSizeInMbs ;
			}
		}
	}
}
