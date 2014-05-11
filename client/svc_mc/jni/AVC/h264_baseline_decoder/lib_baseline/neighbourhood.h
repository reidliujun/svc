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



#ifndef NEIGHBOURHOOD_H
#define NEIGHBOURHOOD_H

#include <string.h>
#include "type.h"

/**
derivation process of the availability for macroblock addresses: 6.4.4

@param mbAddr Neighbouring macroblock address.
@param CurrentAddr Current macroblock address.
@param SliceTable Specifies in which Slice belongs each macroblock of the picture.
*/
static __inline int NeighbourAvail ( const int ai_iMbAddr, const int PicSizeInMBs, int ASliceNum, int BSliceNum)
{
    if ( ai_iMbAddr < 0 || ai_iMbAddr >= PicSizeInMBs ) 
		return 0 ;

    if ( ASliceNum != BSliceNum )
        return 0 ;

    return 1 ;
}





/**
Process for neihbouring locations: 6.4.8.1

@param macro Specifies the availability of the neighbouring macroblock.
@param Block Macroblock parameters.
*/
static __inline void neighbouring_locations_luma ( int AvailMask, NEIGHBOUR * block){
	//Récupératoin du voisin et de son adresse

	const int bLeftx = -(block -> xN < 0);  
	const int bTopy = -(block -> yN < 0);
	const int bBottomy = -(block -> yN >= 15); 
	const int bRightx = -(block -> xN >= 15); 
	const int bMiddlex = ~(bLeftx | bRightx); 
	const int bMiddley = ~(bTopy | bBottomy); 

	block -> Avail = (bMiddlex && bMiddley) + ((bLeftx & bMiddley) && (AvailMask & 0x01)) + (bTopy && ((bMiddlex &&  ((AvailMask >> 1) & 0x01)) + 
		(bLeftx && ((AvailMask >> 3) & 0x01)) + (bRightx && ((AvailMask >> 2) & 0x01))));
}



void GetNeighbour(short iCurrAddress, short iAddr_x, const short PicWidthInMbs, const short PicSizeInMbs, 
				  const unsigned char *SliceTable, RESIDU CurrResidu [], const int constained_pred_flag);
void mbAddrN_8x8(unsigned char *predict_sample, const short PicWidthInMbs, int Mask, const short locx, const short locy, const int mode);

#endif 





