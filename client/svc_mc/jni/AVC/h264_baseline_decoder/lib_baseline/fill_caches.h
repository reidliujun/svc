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




#ifndef FILL_CACHES_H
#define FILL_CACHES_H

#include "type.h"
void FillNonZeroCount(const DATA *CurrBlock, const int MbStride,const int top_type, 
					  const int  left_type, unsigned char *NonZeroCountCache);

void fill_caches (const SLICE *Slice, const RESIDU *Residu, const int EntropyCodingModeFlog, unsigned char NonZeroCountCache [ ], 
				  const DATA *CurrBlock, const unsigned char *SliceTable,  const short ai_iMb_x, const short ai_iMb_y);


void fill_caches_I (const SLICE *Slice, const RESIDU *Residu, const int EntropyCodingModeFlog, 
					unsigned char NonZeroCountCache [ ], const DATA *CurrBlock, 
					const unsigned char *SliceTable, short aio_tiIntra4x4_pred_mode_cache[], 
					const short ai_iMb_x, const short ai_iMb_y, const int ai_iConstrained_intra_pred);


void fill_caches_motion_vector(  const SLICE *Slice, const int ai_iB_stride, const int ai_iB8_stride, short aio_tiRef_cache_l0 [  ], 
							   short aio_tiMv_cache_l0[  ][2], const unsigned char *SliceTable, const RESIDU *Residu, 
							   short ai_tiMv[  ], const short *ai_tiRef,  const int ai_iX, const int ai_iY);


/**
This function permits to update the neighbourhood of the current macroblock in case of a skipped macroblock.

@param NonZeroCountCache A cache table where coeff_token is stocked for each 4x4 block of the current macroblock.
*/
static __inline void init_mb_skip ( unsigned char NonZeroCountCache [] )
{

	NonZeroCountCache [4 + 8 * 1] = NonZeroCountCache [5 + 8 * 1] = NonZeroCountCache [6 + 8 * 1] = 
	NonZeroCountCache [7 + 8 * 1] = NonZeroCountCache [4 + 8 * 2] = NonZeroCountCache [5 + 8 * 2] = 
	NonZeroCountCache [6 + 8 * 2] = NonZeroCountCache [7 + 8 * 2] = NonZeroCountCache [4 + 8 * 3] = 
	NonZeroCountCache [5 + 8 * 3] = NonZeroCountCache [6 + 8 * 3] = NonZeroCountCache [7 + 8 * 3] = 
	NonZeroCountCache [4 + 8 * 4] = NonZeroCountCache [5 + 8 * 4] = NonZeroCountCache [6 + 8 * 4] = 
	NonZeroCountCache [7 + 8 * 4] = 0;

	NonZeroCountCache [1 + 8 * 1] = NonZeroCountCache [2 + 8 * 1] = NonZeroCountCache [1 + 8 * 2] = 
	NonZeroCountCache [2 + 8 * 2] = NonZeroCountCache [1 + 8 * 4] = NonZeroCountCache [2 + 8 * 4] =
	NonZeroCountCache [1 + 8 * 5] = NonZeroCountCache [2 + 8 * 5] = 0;
}



/**
This function initialises the tab NonZeroCountCache when the macroblock is coded in I_PCM way.

@param non_zero_count_cache Specifies the coeff_token of each blocks 4x4 of a macroblock.
*/
static __inline void init_mb_IPCM ( unsigned char NonZeroCountCache [ ] )
{	
	NonZeroCountCache [4 + 8] = NonZeroCountCache [5 + 8] = NonZeroCountCache [6 + 8] = 
	NonZeroCountCache [7 + 8] = NonZeroCountCache [4 + 16] = NonZeroCountCache [5 + 16] = 
	NonZeroCountCache [6 + 16] = NonZeroCountCache [7 + 16] = NonZeroCountCache [4 + 24] = 
	NonZeroCountCache [5 + 24] = NonZeroCountCache [6 + 24] = NonZeroCountCache [7 + 24] = 
	NonZeroCountCache [4 + 32] = NonZeroCountCache [5 + 32] = NonZeroCountCache [6 + 32] = 
	NonZeroCountCache [7 + 32] = NonZeroCountCache [1 + 8] = NonZeroCountCache [2 + 8] = 
	NonZeroCountCache [1 + 16] = NonZeroCountCache [2 + 16] = NonZeroCountCache [1 + 32] = 
	NonZeroCountCache [2 + 32] = NonZeroCountCache [1 + 40] = NonZeroCountCache [2 + 40] = 16;
}

/**
Load into MvCache table Top macroblock motion vectors.
*/
static __inline void FillMvCacheTopPosition(short MvCache[][2], short *MvdL0){
	MvCache [4][0] = *(MvdL0++);
	MvCache [4][1] = *(MvdL0++);
	MvCache [5][0] = *(MvdL0++);
	MvCache [5][1] = *(MvdL0++);
	MvCache [6][0] = *(MvdL0++);
	MvCache [6][1] = *(MvdL0++);
	MvCache [7][0] = *(MvdL0++);
	MvCache [7][1] = *(MvdL0++);
}

/**
Load into MvCache table Left macroblock motion vectors.
*/
static __inline void FillMvCacheLeftPosition(short MvCache[][2], short *MvdL0, int Stride){

	MvCache[11][0] = *(MvdL0++);
	MvCache[11][1] = *MvdL0;

	MvdL0 += Stride - 1;
	MvCache[19][0] = *(MvdL0++);
	MvCache[19][1] = *MvdL0;

	MvdL0 += Stride - 1;
	MvCache [27][0] = *(MvdL0++);
	MvCache [27][1] = *MvdL0;

	MvdL0 += Stride - 1;
	MvCache [35][0] = *(MvdL0++);
	MvCache [35][1] = *MvdL0;
}

/**
Load into RefCache table left macroblock reference indexes.
*/
static __inline void FillRefCacheLeftPosition(short *RefCache, short FirstBlock, short ThirdBlock){
	RefCache[11] = RefCache[19] = FirstBlock;
    RefCache[27] = RefCache[35] = ThirdBlock;
}

/**
Load into RefCache table top macroblock reference indexes.
*/
static __inline void FillRefCacheTopPosition(short *RefCache, short FirstBlock, short SecondBlock){
	RefCache[4] = RefCache[5] = FirstBlock;
    RefCache[6] = RefCache[7] = SecondBlock;
}

#endif
