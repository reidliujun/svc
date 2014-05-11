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


#include "quantization.h"
#include "type.h"
#include "data.h"

const short  Default_W  [6][3] = {{ 10, 13, 16 }, { 11, 14, 18 }, { 13, 16, 20 }, { 14, 18, 23 }, { 16, 20, 25 }, { 18, 23, 29 }};
static short Default_W8 [6][6]= {{20,18,32,19,25,24}, {22,19,35,21,28,26}, {26,23,42,24,33,31}, {28,25,45,26,35,33}, {32,28,51,30,40,38}, {36,32,58,34,46,43},};
static const short W8_scan[16] = { 0,3,4,3, 3,1,5,1, 4,5,2,5, 3,1,5,1};


const unsigned char DIV6 [72] = { 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 7, 7, 7, 7, 7, 7, 8, 8, 8, 8, 8, 8, 9, 9, 9, 9, 9, 9, 10, 10, 10, 10, 10, 10, 11, 11, 11, 11, 11, 11};

#ifndef TI_OPTIM
const unsigned char div6 [52] = { 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 7, 7, 7, 7, 7, 7, 8, 8, 8, 8 };
const unsigned char rem6 [52] = { 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3 };
#endif


/**
This function initializes quantization tables.

@param quantif W_TABLES structure which contains all the quantizatio tables.
@param scaling_matrix_present Indicates if the quantization tables are customized.
@param scaling4 Default scaling matrix.
@param scaling8 Default scaling matrix.
*/
void init_quantization (W_TABLES *quantif, int scaling_matrix_present, short scaling4[][16], short scaling8[6][64] )
{
    int i, j ;
	if ( scaling_matrix_present){
		for ( i = 0 ; i < 6 ; i++ ) {
			for ( j = 0 ; j < 16 ; j++ ) {
				quantif -> W4x4_intra [i][j] = Default_W [i][(j & 1) + (j >> 2 & 1)] * scaling4 [0][j];
				quantif -> W4x4_inter [i][j] = Default_W [i][(j & 1) + (j >> 2 & 1)] * scaling4 [3][j];
				quantif -> W4x4_Cb_intra [i][j] = Default_W [i][(j & 1) + (j >> 2 & 1)] * scaling4 [1][j];
				quantif -> W4x4_Cb_inter [i][j] = Default_W [i][(j & 1) + (j >> 2 & 1)] * scaling4 [4][j];
				quantif -> W4x4_Cr_intra [i][j] = Default_W [i][(j & 1) + (j >> 2 & 1)] * scaling4 [2][j];
				quantif -> W4x4_Cr_inter [i][j] = Default_W [i][(j & 1) + (j >> 2 & 1)] * scaling4 [5][j];
			}
			for ( j = 0 ; j < 64 ; j++ ) {
				quantif -> W8x8_intra [i][j] = Default_W8 [i][W8_scan[((j>>1)&12) | (j&3)] ]*scaling8 [0][j];
				quantif -> W8x8_inter [i][j] = Default_W8 [i][W8_scan[((j>>1)&12) | (j&3)] ]*scaling8 [1][j];
			}
		}
	} else{	
		for ( i = 0 ; i < 6 ; i++ ) {
			for ( j = 0 ; j < 16 ; j++ ) {
				quantif -> W4x4_intra [i][j] = Default_W [i][(j & 1) + (j >> 2 & 1)] << 4;
				quantif -> W4x4_inter [i][j] = Default_W [i][(j & 1) + (j >> 2 & 1)] << 4;
				quantif -> W4x4_Cb_intra [i][j] = Default_W [i][(j & 1) + (j >> 2 & 1)] << 4;
				quantif -> W4x4_Cb_inter [i][j] = Default_W [i][(j & 1) + (j >> 2 & 1)] << 4;
				quantif -> W4x4_Cr_intra [i][j] = Default_W [i][(j & 1) + (j >> 2 & 1)] << 4;
				quantif -> W4x4_Cr_inter [i][j] = Default_W [i][(j & 1) + (j >> 2 & 1)] << 4;
			}
			for ( j = 0 ; j < 64 ; j++ ) {
				quantif -> W8x8_intra [i][j] = Default_W8 [i][W8_scan[((j>>1)&12) | (j&3)] ] << 4;
				quantif -> W8x8_inter [i][j] = Default_W8 [i][W8_scan[((j>>1)&12) | (j&3)] ] << 4;
			}
		}
	}
	
}
