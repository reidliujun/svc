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



#ifndef C_MOTIONV
#define C_MOTIONV

#include "clip.h"
#include "symbol.h"


#define MEDIAN(X,Y,Z) ((X)+(Y)+(Z)-MIN((X), MIN((Y),(Z)))-MAX((X), MAX((Y),(Z))))
#define MEDIAN16(X,Y,Z) ((X)+(Y)+(Z)-MIN16((X), MIN16((Y),(Z)))-MAX16((X), MAX16((Y),(Z))))


void motion_vector_skip(short mv_cache[][2], const short *ref_cache);
void motion_vector	   (short mv_cache[][2], const short MvdLX[2], const short *ref_cache, short mvLX[2], short refldxLX, const int index, int partwidth);
void motion_vector_8x16(short mv_cache[][2], const short MvdLX[2], const short *ref_cache, short mvLX[2], short refldxLX, const int index8);
void motion_vector_16x8(short mv_cache[][2], const short MvdLX[2], const short *ref_cache, short mvLX[2], short refldxLX, const int index8);




//8.4.1.3.1

/**
This function permits to determine the motion vector of the current blcok. 

@param index8 Index of the block 4x4 in hte current macroblock.
@param mv_cache A cache table where the motion vector are stocked for each 4x4 block of the current macroblock.
@param mvpLX Luminance motion vector of the current macroblock.
@param match_count Number of referebce index equal to the current block.
@param refldxLX Image reference of the current macroblock.
@param refldxLA Image reference of the left block.
@param refldxLB Image reference of the top block.
@param refldxLC Image reference of the top left block.
*/
static __inline void motion_pred(int index8, short mv_cache[][2], short mvpLX[2], const int match_count, const short refldxLX, const int refldxLA , const int refldxLB , const int refldxLC){

	if(match_count > 1){ //most common
        mvpLX[0] = MEDIAN16 (mv_cache[index8 - 1][0], mv_cache[index8 - 8][0],  mvpLX[0]); 
		mvpLX[1] = MEDIAN16 (mv_cache[index8 - 1][1], mv_cache[index8 - 8][1],  mvpLX[1]);
    }else if(match_count == 1){
        if(refldxLA == refldxLX){
            mvpLX[0] = mv_cache[index8 - 1][0];
            mvpLX[1] = mv_cache[index8 - 1][1];
        }else if(refldxLB == refldxLX){
            mvpLX[0] = mv_cache[index8 - 8][0];
            mvpLX[1] = mv_cache[index8 - 8][1];
		}
    }else{
        if(refldxLB == PART_NOT_AVAILABLE && refldxLC == PART_NOT_AVAILABLE && refldxLA != PART_NOT_AVAILABLE){
            mvpLX[0] = mv_cache[index8 - 1][0];
            mvpLX[1] = mv_cache[index8 - 1][1];
        }else{
            mvpLX[0] = MEDIAN16 ( mv_cache[index8 - 1][0], mv_cache[index8 - 8][0],  mvpLX[0]); 
			mvpLX[1] = MEDIAN16 ( mv_cache[index8 - 1][1], mv_cache[index8 - 8][1],  mvpLX[1]);
        }
    }
}


#endif
