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

#include "refill.h"





/**
This function allows to store the motion vector according to the mode of the macroblcok.

@param slice_type Type of the current slice.
@param subMbmode mode of the sub macroblock ( 8x8, 8x4, 4x8, 4x4).
@param mv_cache The table of the motion vector of the current macroblock.
*/
void refill_motion_vector(int slice_type, int subMbmode, short mv_cache [][2]){


	//Be carefull mode = mode + 1;
	if ( !slice_type){
		// 8x8 Mb
		if(subMbmode == 0){
			refill_mv(2, 2, mv_cache[0], &mv_cache[0]); 
		}else if (subMbmode == 1){
			refill_mv(2, 1, mv_cache[0], &mv_cache[0]); 
			refill_mv(2, 1, mv_cache[8], &mv_cache[8]); 
		}else if (subMbmode == 2){
			refill_mv(1, 2, mv_cache[0], &mv_cache[0]); 
			refill_mv(1, 2, mv_cache[1], &mv_cache[1]); 
		}

	}else{
		if(subMbmode < 4){
			refill_mv(2, 2, mv_cache[0], &mv_cache[0]); 
		}else if (subMbmode == 4 || subMbmode == 6 || subMbmode == 8){
			refill_mv(2, 1, mv_cache[0], &mv_cache[0]); 
			refill_mv(2, 1, mv_cache[8], &mv_cache[8]); 
		}else if (subMbmode == 5 || subMbmode == 7 || subMbmode == 9){
			refill_mv(1, 2, mv_cache[0], &mv_cache[0]); 
			refill_mv(1, 2, mv_cache[1], &mv_cache[1]); 
		}
	}			
}
