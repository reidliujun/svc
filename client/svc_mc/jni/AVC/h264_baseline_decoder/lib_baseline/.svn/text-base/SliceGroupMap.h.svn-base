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

#ifndef C_SLICE_GROUPMAP
#define C_SLICE_GROUPMAP


#include "type.h"
#include "data.h"



void mapUnitToSlice (const SPS *Sps, const PPS *Pps,  unsigned char ao_tiMapUnitToSliceGroupMap [ ], 
					 const int ai_islice_group_change_cycle, unsigned char *ai_Slice_group_id);


/**
This function permits to determine the next macroblock address. 
This function returns the adress of the next macroblock in the slice

@param n The adress of the current macroblock.
@param PicSizeInMbs The size in macroblock of the image.
@param MbToaio_pstSliceGroupMap A table which contains the aio_piPosition of each macroblock in a slice 
*/
static __inline short NextMbAddress ( const short ai_iN,  const int ai_iPicSizeInMbs, const int aio_pstSliceGroupMap [ ]){
    short i;
  
	for(i = ai_iN + 1 ; (i < ai_iPicSizeInMbs) && (aio_pstSliceGroupMap [i] != aio_pstSliceGroupMap [ai_iN]) ; i++);

    return i ;
}


/**
According to the SliceGroupMap, this function returns the next macroblock address.
*/
static __inline short Next_MbAddress ( const short ai_iN,  const short ai_iPicSizeInMbs, const short PicWiInMbs, 
									  const unsigned char aio_pstSliceGroupMap[ ], short *Mb_x, short * Mb_y){

	short i;
	
	if(aio_pstSliceGroupMap[ai_iN + 1] != aio_pstSliceGroupMap[ai_iN]){
		for(i = ai_iN + 2 ; (i < ai_iPicSizeInMbs) && (aio_pstSliceGroupMap [i] != aio_pstSliceGroupMap [ai_iN]) ; i++);

	
		//initialize macroblock position
		GetMbPosition(i, PicWiInMbs, Mb_x, Mb_y);
		return i;
	}else{
		*Mb_x += 1;
		if (*Mb_x == PicWiInMbs){
			*Mb_y += 1;
			*Mb_x = 0;
		}

		return ai_iN + 1;
	}
}

#endif
