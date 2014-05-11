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






#ifndef REFILL_H
#define REFILL_H

void refill_motion_vector(int mode, int subMbmode, short mv_cache [][2]);


/**
This function allows to store the reference index according to the size given in parameters.

@param width Width of the initialization.
@param height height of the initialization.
@param val Value to initialze with.
@param dst The table of the reference index of the current macroblock.
*/
static __inline void refill_ref( int width, int height, short val, short *dst){

	int i,j;
	for (i = 0; i < height; i++){
		for (j = 0; j < width; j++){
			dst[i * 8 + j] =  val;
		}
	}
}



/**
This function allows to store the motion vector according to the size given in parameters.

@param width Width of the initialization.
@param height height of the initialization.
@param val Value to initialze with.
@param dst The table of the motion vector of the current macroblock.
*/
static __inline void refill_mv( int width, int height, short *val, short dst[][2]){

	int i,j;
	for (i = 0; i < height; i++){
		for (j = 0; j < width; j++){
			dst[i * 8 + j][0] =  *val;
			dst[i * 8 + j][1] =  *(val + 1);
		}
	}
}


static __inline void FillMotionVector(short mvdlx[2], short mv_cache[][2], int Mode){


	switch(Mode){
		case 3:
			mv_cache [0][0] = mv_cache [1][0] = mv_cache [2][0] = mv_cache [3][0] = 
			mv_cache [8][0] = mv_cache [9][0] = mv_cache [10][0] = mv_cache [11][0] = 
			mv_cache [16][0] =  mv_cache [17][0] = mv_cache [18][0] = mv_cache [19][0] = 
			mv_cache [24][0] = mv_cache [25][0] = mv_cache [26][0] = mv_cache [27][0] = mvdlx[0];

			mv_cache [0][1] = mv_cache [1][1] = mv_cache [2][1] = mv_cache [3][1] = 
			mv_cache [8][1] = mv_cache [9][1] = mv_cache [10][1] = mv_cache [11][1] = 
			mv_cache [16][1] = mv_cache [17][1] = mv_cache [18][1] = mv_cache [19][1] = 
			mv_cache [24][1] = mv_cache [25][1] = mv_cache [26][1] = mv_cache [27][1] = mvdlx[1];
			break;

		case 2:
			mv_cache [0][0] = mv_cache [1][0] = mv_cache [2][0] = mv_cache [3][0] = 
			mv_cache [8][0] = mv_cache [9][0] = mv_cache [10][0] = mv_cache [11][0] =  mvdlx[0];
			
			mv_cache [0][1] = mv_cache [1][1] = mv_cache [2][1] = mv_cache [3][1] = 
			mv_cache [8][1] = mv_cache [9][1] = mv_cache [10][1] = mv_cache [11][1] = mvdlx[1];
			break;

		case 1:
			mv_cache [0][0] = mv_cache [1][0] = mv_cache [8][0] = mv_cache [9][0] = 
			mv_cache [16][0] = mv_cache [17][0] = mv_cache [24][0] = mv_cache [25][0] =	mvdlx[0];
			

			mv_cache [0][1] = mv_cache [1][1] = mv_cache [8][1] = mv_cache [9][1] = 
			mv_cache [16][1] = mv_cache [17][1] = mv_cache [24][1] = mv_cache [25][1] =	mvdlx[1];
			break;

	}
}



static __inline void FillRefIdx(short RefIdxLx, short *ref_cache, int Mode){
	

	switch(Mode){
		case 3:
			ref_cache[0] = ref_cache[1] = ref_cache[2] = ref_cache[3] = 
			ref_cache[8] = ref_cache[9] = ref_cache[10] = ref_cache[11] =
			ref_cache[16] = ref_cache[17] = ref_cache[18] = ref_cache[19] =
			ref_cache[24] = ref_cache[25] = ref_cache[26] = ref_cache[27] =	RefIdxLx;
			break;
		case 2:
			ref_cache[0] = ref_cache[1] = ref_cache[2] = ref_cache[3] = 
			ref_cache[8] = ref_cache[9] = ref_cache[10] = ref_cache[11] = RefIdxLx;
			break;
		case 1:
			ref_cache[0] = ref_cache[1] = ref_cache[8] = ref_cache[9] =
			ref_cache[16] = ref_cache[17] = ref_cache[24] = ref_cache[25] =	RefIdxLx;
			break;
	}
}



static __inline void FillSubMotionVector(short MvdLX[2], short mv_cache[][2], int sub_mb_type, int slice_type){

	switch(sub_mb_type + (slice_type << 2) ){
		case 0:
		case 5:
		case 6:
		case 7:
			mv_cache [0][0] = mv_cache [1][0] =	mv_cache [8][0] = mv_cache [9][0] =	MvdLX[0];
			mv_cache [0][1] = mv_cache [1][1] = mv_cache [8][1] = mv_cache [9][1] =	MvdLX[1];
			break;
		case 1:
		case 8:
		case 10:
		case 12:
			mv_cache [0][0] = mv_cache [1][0] =	MvdLX[0];
			mv_cache [0][1] = mv_cache [1][1] =	MvdLX[1];
			break;
		case 2:
		case 9:
		case 11:
		case 13:
			mv_cache [0][0] = mv_cache [8][0] =	MvdLX[0];
			mv_cache [0][1] = mv_cache [8][1] =	MvdLX[1];
			break;
		case 3:
		case 4:
		case 14:
		case 15:
		case 16:
			mv_cache [0][0] = MvdLX[0];
			mv_cache [0][1] = MvdLX[1];
			break;
	}
}
#endif
