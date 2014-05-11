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

#include "data.h"
#include "type.h"

#include "motion_vector.h"



//8.4.1

/**
This function permits to determine the motion vector of the current macroblock. 
All the parameters decoded will be stored in mv_cache.



@param mv_cache A cache table where the motion vector are stocked for each 4x4 block of the current macroblock.
@param MvdLX Luminance motion vector of the current macroblock.
@param ref_cache A cache table where the reference is stocked for each 4x4 block of the current macroblock.
@param mvLX Motion vector error to compensate the prediction.
@param refldxLX Image reference of the current macroblock.
@param index8 Index of the block 4x4 in hte current macroblock.
@param partwidth Width of the current block.
*/
void motion_vector(short mv_cache[][2], const short MvdLX[2], const short *ref_cache, short mvLX[2], short refldxLX, const int index8, int partwidth)
{

	const int refldxLA = ref_cache[index8 - 1];
	const int refldxLB = ref_cache[index8 - 8];
	int match_count;
	int refldxLC; 
	if ( ref_cache[index8 - 8 + partwidth] == PART_NOT_AVAILABLE){
		refldxLC = ref_cache[index8 - 8 - 1];
		mvLX[0] = mv_cache[index8 - 8 - 1][0];
		mvLX[1] = mv_cache[index8 - 8 - 1][1];
	}else{
		refldxLC = ref_cache[index8 - 8 + partwidth];
		mvLX[0] = mv_cache[index8 - 8 + partwidth][0];
		mvLX[1] = mv_cache[index8 - 8 + partwidth][1];
	}

	match_count = (refldxLA == refldxLX) + (refldxLB == refldxLX) + (refldxLC == refldxLX);
        
        
	motion_pred(index8, mv_cache, mvLX, match_count, refldxLX, refldxLA, refldxLB, refldxLC);

	mvLX[0] +=  MvdLX[0];
	mvLX[1] +=  MvdLX[1];
}




/**
This function permits to determine the motion vector of a macroblock 8x16. 
All the parameters decoded will be stored in mv_cache.



@param mv_cache A cache table where the motion vector are stocked for each 4x4 block of the current macroblock.
@param MvdLX Luminance motion vector of the current macroblock.
@param ref_cache A cache table where the reference is stocked for each 4x4 block of the current macroblock.
@param mvLX Motion vector error to compensate the prediction.
@param refldxLX Image reference of the current macroblock.
@param index8 Index of the block 4x4 in hte current macroblock.
*/
void motion_vector_8x16(short mv_cache[][2], const short MvdLX[2], const short *ref_cache, short mvLX[2], short refldxLX, const int index8)
{


	int match_count; 
	int refldxLC;

	if ( index8 == 12){
		if (ref_cache[11] == refldxLX){
			mvLX[0] = mv_cache[11][0];
			mvLX[1] = mv_cache[11][1];
		}else{
			if ( ref_cache[4 + 2] == PART_NOT_AVAILABLE ){
				refldxLC = ref_cache[4 - 1];
				mvLX[0] = mv_cache[4 - 1][0];
				mvLX[1] = mv_cache[4 - 1][1];
			}else{
				refldxLC = ref_cache[4 + 2];
				mvLX[0] = mv_cache[4 + 2 ][0];
				mvLX[1] = mv_cache[4 + 2 ][1];
			}
			
			match_count = (ref_cache[11] == refldxLX) + (ref_cache[4] == refldxLX) + (refldxLC == refldxLX);
	
			motion_pred(12, mv_cache, mvLX, match_count, refldxLX, ref_cache[11], ref_cache[4],refldxLC);
		}
	}else{
		
		if ( ref_cache[6 + 2] == PART_NOT_AVAILABLE ){
			refldxLC = ref_cache[6 - 1];
			mvLX[0] = mv_cache[6 + - 1][0];
			mvLX[1] = mv_cache[6 + - 1][1];
		}else{
			refldxLC = ref_cache[6 + 2];
			mvLX[0] = mv_cache[6 + 2][0];
			mvLX[1] = mv_cache[6 + 2][1];
		}
		if (refldxLC != refldxLX){	
			match_count = (ref_cache[13] == refldxLX) + (ref_cache[6] == refldxLX) + (refldxLC == refldxLX);
			motion_pred(14, mv_cache, mvLX, match_count, refldxLX, ref_cache[13], ref_cache[6],refldxLC);
		}
	}

	mvLX[0] +=  MvdLX[0];
	mvLX[1] +=  MvdLX[1];
}


		
/**
This function permits to determine the motion vector of a macroblock 16x8. 
All the parameters decoded will be stored in mv_cache.



@param mv_cache A cache table where the motion vector are stocked for each 4x4 block of the current macroblock.
@param MvdLX Luminance motion vector of the current macroblock.
@param ref_cache A cache table where the reference is stocked for each 4x4 block of the current macroblock.
@param mvLX Motion vector error to compensate the prediction.
@param refldxLX Image reference of the current macroblock.
@param index8 Index of the block 4x4 in hte current macroblock.
*/		
void motion_vector_16x8(short mv_cache[][2], const short MvdLX[2], const short *ref_cache, short mvLX[2], short refldxLX, const int index8)
{


	int match_count; 
	int refldxLC;

	if ( index8 == 12){
		if (ref_cache[4] == refldxLX){
			mvLX[0] = mv_cache[4][0];
			mvLX[1] = mv_cache[4][1];
		}else{
			if ( ref_cache[4 + 4] == PART_NOT_AVAILABLE ){
				refldxLC = ref_cache[4 - 1];
				mvLX[0] = mv_cache[4 - 1][0];
				mvLX[1] = mv_cache[4 - 1][1];
			}else{
				refldxLC = ref_cache[4 + 4];
				mvLX[0] = mv_cache[4 + 4 ][0];
				mvLX[1] = mv_cache[4 + 4 ][1];
			}
			
			match_count = (ref_cache[11] == refldxLX) + (ref_cache[4] == refldxLX) + (refldxLC == refldxLX);
	
			motion_pred(12, mv_cache, mvLX, match_count, refldxLX, ref_cache[11], ref_cache[4],refldxLC);
		}
	}else{
		
		if (ref_cache[27] == refldxLX){
			mvLX[0] = mv_cache[27][0];
			mvLX[1] = mv_cache[27][1];
		}else{
			if ( ref_cache[20 + 4] == PART_NOT_AVAILABLE){
				refldxLC = ref_cache[20 - 1];
				mvLX[0] = mv_cache[20 - 1][0];
				mvLX[1] = mv_cache[20 - 1][1];
			}else{
				refldxLC = ref_cache[20 + 4];
				mvLX[0] = mv_cache[20 + 4][0];
				mvLX[1] = mv_cache[20 + 4][1];
			}
			
			match_count = (ref_cache[27] == refldxLX) + (ref_cache[20] == refldxLX) + (refldxLC == refldxLX);
			motion_pred(28, mv_cache, mvLX, match_count, refldxLX, ref_cache[27], ref_cache[20],refldxLC);
		}
	}
	mvLX[0] +=  MvdLX[0];
	mvLX[1] +=  MvdLX[1];
}




/**
This function permits to determine the motion vector of a skipped macroblock. 
All the parameters decoded will be stored in mv_cache.



@param mv_cache A cache table where the motion vector are stocked for each 4x4 block of the current macroblock.
@param ref_cache A cache table where the reference is stocked for each 4x4 block of the current macroblock.
@param refldxLX Image reference of the current macroblock.
*/
void motion_vector_skip(short mv_cache[][2], const short *ref_cache)
{

	const int refldxLA = ref_cache[12 - 1];
	const int refldxLB = ref_cache[12 - 8];
	int Reflx = 0;
	int match_count;
	int refldxLC; 

	if ( refldxLA == PART_NOT_AVAILABLE ||  refldxLB == PART_NOT_AVAILABLE
			|| (refldxLA == 0  && mv_cache[11][0] == 0 && mv_cache[11][1] == 0)
			|| (refldxLB == 0  && mv_cache[4][0] == 0 && mv_cache[4][1] == 0))	{

			mv_cache[12][0] = 0;
			mv_cache[12][1] = 0;

	}else{
		if ( ref_cache[4 + 4] == PART_NOT_AVAILABLE ){
			refldxLC = ref_cache[4 - 1];
			mv_cache[12][0] = mv_cache[4 - 1][0];
			mv_cache[12][1] = mv_cache[4 - 1][1];
		}else{
			refldxLC = ref_cache[4 + 4];
			mv_cache[12][0] = mv_cache[4 + 4][0];
			mv_cache[12][1] = mv_cache[4 + 4][1];
		}

		match_count = (refldxLA == 0) + (refldxLB == 0) + (refldxLC == 0);
        
        motion_pred(12, mv_cache, mv_cache[12], match_count, Reflx, refldxLA, refldxLB, refldxLC);
	}
}

