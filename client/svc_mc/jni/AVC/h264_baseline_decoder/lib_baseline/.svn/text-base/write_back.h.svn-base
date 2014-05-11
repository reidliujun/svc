
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


#ifndef WRITE_BACK_H
#define WRITE_BACK_H
#include "type.h"

#ifdef TI_OPTIM
#undef write_back_motion
#define write_back_motion write_back_motion_TI
#else
#undef write_back_motion
#define write_back_motion write_back_motion_C
#endif



void write_back_motion_cache( const int ai_iB_stride, const int ai_iB8_stride, const int ai_iMb_xy, short ai_tiMv[ ], 
							 short ai_tiMv_cache[ ][2], short *ai_tiRef, short aio_tiRef_cache[ ], int mb_stride);

void NonZeroCountManagement( const unsigned char *Non_zero_count_cache, const short iMb_x, const short iMb_y, const int PicWidthInMbs,
							const int PicHeightInMbs,DATA *aio_tstBlock, RESIDU *CurrentResidu);
void WriteBackMotionCacheCurrPosition(const int ai_iB_stride	, const int ai_iB8_stride, short ai_tiMv[ ], short  ai_tiMv_cache[ ][2], short *ai_tiRef, short aio_tiRef_cache[ ]);
void write_back_motion_cache_interpolation_8x8(const int ai_iB_stride, const int ai_iB8_stride, short ai_tiMv[ ], short  ai_tiMv_cache[ ][2], short *ai_tiRef, short aio_tiRef_cache[ ]);
void write_back_motion( const int ai_iB_stride, const int ai_iB8_stride	, short MvdL0[ ], short  ai_tiMv_cache[ ][2], short ao_tiRef[ ], short ai_tiRef_cache[]);
void write_back_P_skip_motion( const int ai_iB_stride, const int ai_iB8_stride, short MvdL0[ ], short ai_tiMv_cache[2], short ao_tiRef[ ], const short ai_tiRef_cache);




/**
According to the non zero coefficient level, bl4x4 is set.
Only when transform 8x8 flag is activated
*/
static __inline void write_back_cbp_8x8(unsigned char *tab, RESIDU *residu)
{
	
	if(tab[12]){
		if (!(residu -> blk4x4 & 1))
			residu -> blk4x4 += 1;
		if (!(residu -> blk4x4 & 2))
			residu -> blk4x4 += 2;
		if (!(residu -> blk4x4 & 16))
			residu -> blk4x4 += 16;
		if (!(residu -> blk4x4 & 32))
			residu -> blk4x4 += 32;
	}

	if(tab[14]){
		if (!(residu -> blk4x4 & 4))
			residu -> blk4x4 += 4;
		if (!(residu -> blk4x4 & 8))
			residu -> blk4x4 += 8;
		if (!(residu -> blk4x4 & 64))
			residu -> blk4x4 += 64;
		if (!(residu -> blk4x4 & 128))
			residu -> blk4x4 += 128;
	}


	if(tab[28]){
		if (!(residu -> blk4x4 & 256))
			residu -> blk4x4 += 256;
		if (!(residu -> blk4x4 & 512))
			residu -> blk4x4 += 512;
		if (!(residu -> blk4x4 & 4096))
			residu -> blk4x4 += 4096;
		if (!(residu -> blk4x4 & 8192))
			residu -> blk4x4 += 8192;
	}


	if(tab[30]){
		if (!(residu -> blk4x4 & 1024))
			residu -> blk4x4 += 1024;
		if (!(residu -> blk4x4 & 2048))
			residu -> blk4x4 += 2048;
		if (!(residu -> blk4x4 & 16384))
			residu -> blk4x4 += 16384;
		if (!(residu -> blk4x4 & 32768))
			residu -> blk4x4 += 32768;
	}
}



/**
According to the non zero coefficient level, bl4x4 is set.
Only when transform 8x8 flag is not activated
*/
static __inline void write_back_cbp(unsigned char *tab, RESIDU *residu)
{

	int i, j;
	for(i = 0; i < 4; i++){
		for(j = 0; j < 4; j++){
			if(tab[12 + j + 8 * i] && !(residu -> blk4x4 & (1 << (j + 4 * i)))){
				residu -> blk4x4 += 1 << (j + 4 *i);
			}
		}
	}
}



/**
This function permits to write back the coeff_token of the current macroblock.


@param non_zero_count_cache A cache table where coeff_token is stocked for each 4x4 block of the current macroblock.
@param non_zero_count A table where each coeff_token calculated is stocked for each macroblock decoded.
@param ai_iMb_x The x position of the macroblock in the picture.
@param ai_iMb_y The y position of the macroblock in the picture.
@param ai_iMb_stride The width in macroblocks of the image.
@param ai_iPicWidthInMbs Number in macroblocks of the image.
@param aio_tstBlock Contains all parameters of each macroblock of the current picture. 
*/
	
static __inline void write_back_non_zero_count( const unsigned char *ai_tiNon_zero_count_cache, const short ai_iMb_x, const short ai_iMb_y, 
											   const int ai_iMb_stride, const int PicHeightInMbs, DATA *aio_tstBlock)
{

	
	//Luminance
	aio_tstBlock -> NonZeroCount[12] = ai_tiNon_zero_count_cache [4 + 8 * 1];
	aio_tstBlock -> NonZeroCount[13] = ai_tiNon_zero_count_cache [5 + 8 * 1];
    aio_tstBlock -> NonZeroCount[14] = ai_tiNon_zero_count_cache [6 + 8 * 1];
	aio_tstBlock -> NonZeroCount[15] = ai_tiNon_zero_count_cache [7 + 8 * 1];

	aio_tstBlock -> NonZeroCount[20] = ai_tiNon_zero_count_cache [4 + 8 * 2];
	aio_tstBlock -> NonZeroCount[21] = ai_tiNon_zero_count_cache [5 + 8 * 2];
	aio_tstBlock -> NonZeroCount[22] = ai_tiNon_zero_count_cache [6 + 8 * 2];
	aio_tstBlock -> NonZeroCount[23] = ai_tiNon_zero_count_cache [7 + 8 * 2];

	aio_tstBlock -> NonZeroCount[28] = ai_tiNon_zero_count_cache [4 + 8 * 3];
	aio_tstBlock -> NonZeroCount[29] = ai_tiNon_zero_count_cache [5 + 8 * 3];
	aio_tstBlock -> NonZeroCount[30] = ai_tiNon_zero_count_cache [6 + 8 * 3];
	aio_tstBlock -> NonZeroCount[31] = ai_tiNon_zero_count_cache [7 + 8 * 3];

	aio_tstBlock -> NonZeroCount[36] = ai_tiNon_zero_count_cache [4 + 8 * 4];
	aio_tstBlock -> NonZeroCount[37] = ai_tiNon_zero_count_cache [5 + 8 * 4];
	aio_tstBlock -> NonZeroCount[38] = ai_tiNon_zero_count_cache [6 + 8 * 4];
	aio_tstBlock -> NonZeroCount[39] = ai_tiNon_zero_count_cache [7 + 8 * 4];


	//Chrominance
	aio_tstBlock -> NonZeroCount[17] = ai_tiNon_zero_count_cache [1 + 8 * 2];
	aio_tstBlock -> NonZeroCount[18] = ai_tiNon_zero_count_cache [2 + 8 * 2];
	aio_tstBlock -> NonZeroCount[9] = ai_tiNon_zero_count_cache [1 + 8 * 1];
	aio_tstBlock -> NonZeroCount[10] = ai_tiNon_zero_count_cache [2 + 8 * 1];

	aio_tstBlock -> NonZeroCount[41] = ai_tiNon_zero_count_cache [1 + 8 * 5];
    aio_tstBlock -> NonZeroCount[42] = ai_tiNon_zero_count_cache [2 + 8 * 5];
	aio_tstBlock -> NonZeroCount[33] = ai_tiNon_zero_count_cache [1 + 8 * 4];
	aio_tstBlock -> NonZeroCount[34] = ai_tiNon_zero_count_cache [2 + 8 * 4];




	// not first column
	if( ai_iMb_x && aio_tstBlock[- 1] . Transform8x8){
		aio_tstBlock -> NonZeroCount[11] = aio_tstBlock -> NonZeroCount[19] = aio_tstBlock [- 1] . NonZeroCount[14];
		aio_tstBlock -> NonZeroCount[27] = aio_tstBlock -> NonZeroCount[35] = aio_tstBlock [- 1] . NonZeroCount[30];
	}else if( ai_iMb_x){
		aio_tstBlock -> NonZeroCount[11] = aio_tstBlock [-1] . NonZeroCount[15];
		aio_tstBlock -> NonZeroCount[19] = aio_tstBlock [-1] . NonZeroCount[23];
		aio_tstBlock -> NonZeroCount[27] = aio_tstBlock [-1] . NonZeroCount[31];
		aio_tstBlock -> NonZeroCount[35] = aio_tstBlock [-1] . NonZeroCount[39];
	}

	// not last column
	if( ai_iMb_x != ai_iMb_stride - 1){
		aio_tstBlock [1] . NonZeroCount[11] = aio_tstBlock -> NonZeroCount[15];
		aio_tstBlock [1] . NonZeroCount[19] = aio_tstBlock -> NonZeroCount[23];
		aio_tstBlock [1] . NonZeroCount[27] = aio_tstBlock -> NonZeroCount[31];
		aio_tstBlock [1] . NonZeroCount[35] = aio_tstBlock -> NonZeroCount[39];
	}

	// not first line
	if(ai_iMb_y && aio_tstBlock[- ai_iMb_stride] . Transform8x8){
		aio_tstBlock -> NonZeroCount[4] = aio_tstBlock -> NonZeroCount[5] = aio_tstBlock [- ai_iMb_stride] . NonZeroCount[28];
		aio_tstBlock -> NonZeroCount[6] = aio_tstBlock -> NonZeroCount[7] = aio_tstBlock [- ai_iMb_stride] . NonZeroCount[30];
		aio_tstBlock -> NonZeroCount[1] = aio_tstBlock [- ai_iMb_stride] . NonZeroCount[17];
		aio_tstBlock -> NonZeroCount[2] = aio_tstBlock [- ai_iMb_stride] . NonZeroCount[18];
		aio_tstBlock -> NonZeroCount[25] = aio_tstBlock [- ai_iMb_stride] . NonZeroCount[41];
		aio_tstBlock -> NonZeroCount[26] = aio_tstBlock [- ai_iMb_stride] . NonZeroCount[42];
	}else if(ai_iMb_y){
		aio_tstBlock -> NonZeroCount[4] = aio_tstBlock [- ai_iMb_stride] . NonZeroCount[36];
		aio_tstBlock -> NonZeroCount[5] = aio_tstBlock [- ai_iMb_stride] . NonZeroCount[37];
		aio_tstBlock -> NonZeroCount[6] = aio_tstBlock [- ai_iMb_stride] . NonZeroCount[38];
		aio_tstBlock -> NonZeroCount[7] = aio_tstBlock [- ai_iMb_stride] . NonZeroCount[39];
		aio_tstBlock -> NonZeroCount[1] = aio_tstBlock [- ai_iMb_stride] . NonZeroCount[17];
		aio_tstBlock -> NonZeroCount[2] = aio_tstBlock [- ai_iMb_stride] . NonZeroCount[18];
		aio_tstBlock -> NonZeroCount[25] = aio_tstBlock [- ai_iMb_stride] . NonZeroCount[41];
		aio_tstBlock -> NonZeroCount[26] = aio_tstBlock [- ai_iMb_stride] . NonZeroCount[42];
	}

	// not last line
	if( (ai_iMb_y + 1) < PicHeightInMbs){
		aio_tstBlock [ai_iMb_stride] . NonZeroCount[4] = aio_tstBlock -> NonZeroCount[36];
		aio_tstBlock [ai_iMb_stride] . NonZeroCount[5] = aio_tstBlock -> NonZeroCount[37];
		aio_tstBlock [ai_iMb_stride] . NonZeroCount[6] = aio_tstBlock -> NonZeroCount[38];
		aio_tstBlock [ai_iMb_stride] . NonZeroCount[7] = aio_tstBlock -> NonZeroCount[39];
		aio_tstBlock [ai_iMb_stride] . NonZeroCount[1] = aio_tstBlock -> NonZeroCount[17];
		aio_tstBlock [ai_iMb_stride] . NonZeroCount[2] = aio_tstBlock -> NonZeroCount[18];
		aio_tstBlock [ai_iMb_stride] . NonZeroCount[25] = aio_tstBlock -> NonZeroCount[41];
		aio_tstBlock [ai_iMb_stride] . NonZeroCount[26] = aio_tstBlock -> NonZeroCount[42];
	}
}




/**
This function permits to initialize the coeff_token of a skipped macroblock.


@param non_zero_count A NonZeroCountle where each coeff_token calculated is stocked for each macroblock decoded.
@param ai_iMb_x The x position of the macroblock in the picture.
@param ai_iMb_y The y position of the macroblock in the picture.
@param ai_iMb_stride The width in macroblocks of the image.
@param ai_iPicWidthInMbs Number in macroblocks of the image.
@param aio_tstBlock Contains all parameters of each macroblock of the current picture. 
*/
static __inline void init_skip_non_zero_count( const short ai_iMb_x, const short ai_iMb_y, const int ai_iMb_stride,
											  const int ai_iPicWidthInMbs, DATA *aio_tstBlock
)
{

    const int	iMb_xy = ai_iMb_x + ai_iMb_y * (short)(ai_iMb_stride);
	
	//Luminance
	aio_tstBlock[iMb_xy] . NonZeroCount[12] = 0;
	aio_tstBlock[iMb_xy] . NonZeroCount[13] = 0;
    aio_tstBlock[iMb_xy] . NonZeroCount[14] = 0;
	aio_tstBlock[iMb_xy] . NonZeroCount[15] = 0;

	aio_tstBlock[iMb_xy] . NonZeroCount[20] = 0;
	aio_tstBlock[iMb_xy] . NonZeroCount[21] = 0;
	aio_tstBlock[iMb_xy] . NonZeroCount[22] = 0;
	aio_tstBlock[iMb_xy] . NonZeroCount[23] = 0;

	aio_tstBlock[iMb_xy] . NonZeroCount[28] = 0;
	aio_tstBlock[iMb_xy] . NonZeroCount[29] = 0;
	aio_tstBlock[iMb_xy] . NonZeroCount[30] = 0;
	aio_tstBlock[iMb_xy] . NonZeroCount[31] = 0;

	aio_tstBlock[iMb_xy] . NonZeroCount[36] = 0;
	aio_tstBlock[iMb_xy] . NonZeroCount[37] = 0;
	aio_tstBlock[iMb_xy] . NonZeroCount[38] = 0;
	aio_tstBlock[iMb_xy] . NonZeroCount[39] = 0;


	//Chrominance
	aio_tstBlock[iMb_xy] . NonZeroCount[17] = 0;
	aio_tstBlock[iMb_xy] . NonZeroCount[18] = 0;
	aio_tstBlock[iMb_xy] . NonZeroCount[9] = 0;
	aio_tstBlock[iMb_xy] . NonZeroCount[10] = 0;

	aio_tstBlock[iMb_xy] . NonZeroCount[41] = 0;
    aio_tstBlock[iMb_xy] . NonZeroCount[42] = 0;
	aio_tstBlock[iMb_xy] . NonZeroCount[33] = 0;
	aio_tstBlock[iMb_xy] . NonZeroCount[34] = 0;

// not first column
	if( ai_iMb_x && aio_tstBlock[iMb_xy - 1] . Transform8x8){
		aio_tstBlock[iMb_xy] . NonZeroCount[11] = aio_tstBlock[iMb_xy] . NonZeroCount[19] = aio_tstBlock [iMb_xy - 1] . NonZeroCount[14];
		aio_tstBlock[iMb_xy] . NonZeroCount[27] = aio_tstBlock[iMb_xy] . NonZeroCount[35] = aio_tstBlock [iMb_xy - 1] . NonZeroCount[30];
	}else if( ai_iMb_x){
		aio_tstBlock[iMb_xy] . NonZeroCount[11] = aio_tstBlock [iMb_xy-1] . NonZeroCount[15];
		aio_tstBlock[iMb_xy] . NonZeroCount[19] = aio_tstBlock [iMb_xy-1] . NonZeroCount[23];
		aio_tstBlock[iMb_xy] . NonZeroCount[27] = aio_tstBlock [iMb_xy-1] . NonZeroCount[31];
		aio_tstBlock[iMb_xy] . NonZeroCount[35] = aio_tstBlock [iMb_xy-1] . NonZeroCount[39];
	}

// not last column
	if( ai_iMb_x != ai_iMb_stride - 1){
		aio_tstBlock [iMb_xy+1] . NonZeroCount[11] = 0;
		aio_tstBlock [iMb_xy+1] . NonZeroCount[19] = 0;
		aio_tstBlock [iMb_xy+1] . NonZeroCount[27] = 0;
		aio_tstBlock [iMb_xy+1] . NonZeroCount[35] = 0;
	}

// not first line
	if(ai_iMb_y && aio_tstBlock[iMb_xy - ai_iMb_stride] . Transform8x8){
		aio_tstBlock[iMb_xy] . NonZeroCount[4] = aio_tstBlock[iMb_xy] . NonZeroCount[5] = aio_tstBlock [iMb_xy - ai_iMb_stride] . NonZeroCount[28];
		aio_tstBlock[iMb_xy] . NonZeroCount[6] = aio_tstBlock[iMb_xy] . NonZeroCount[7] = aio_tstBlock [iMb_xy - ai_iMb_stride] . NonZeroCount[30];
		aio_tstBlock[iMb_xy] . NonZeroCount[1] = aio_tstBlock [iMb_xy - ai_iMb_stride] . NonZeroCount[17];
		aio_tstBlock[iMb_xy] . NonZeroCount[2] = aio_tstBlock [iMb_xy - ai_iMb_stride] . NonZeroCount[18];
		aio_tstBlock[iMb_xy] . NonZeroCount[25] = aio_tstBlock [iMb_xy - ai_iMb_stride] . NonZeroCount[41];
		aio_tstBlock[iMb_xy] . NonZeroCount[26] = aio_tstBlock [iMb_xy - ai_iMb_stride] . NonZeroCount[42];
	}else if(ai_iMb_y){
		aio_tstBlock[iMb_xy] . NonZeroCount[4] = aio_tstBlock [iMb_xy - ai_iMb_stride] . NonZeroCount[36];
		aio_tstBlock[iMb_xy] . NonZeroCount[5] = aio_tstBlock [iMb_xy - ai_iMb_stride] . NonZeroCount[37];
		aio_tstBlock[iMb_xy] . NonZeroCount[6] = aio_tstBlock [iMb_xy - ai_iMb_stride] . NonZeroCount[38];
		aio_tstBlock[iMb_xy] . NonZeroCount[7] = aio_tstBlock [iMb_xy - ai_iMb_stride] . NonZeroCount[39];
		aio_tstBlock[iMb_xy] . NonZeroCount[1] = aio_tstBlock [iMb_xy - ai_iMb_stride] . NonZeroCount[17];
		aio_tstBlock[iMb_xy] . NonZeroCount[2] = aio_tstBlock [iMb_xy - ai_iMb_stride] . NonZeroCount[18];
		aio_tstBlock[iMb_xy] . NonZeroCount[25] = aio_tstBlock [iMb_xy - ai_iMb_stride] . NonZeroCount[41];
		aio_tstBlock[iMb_xy] . NonZeroCount[26] = aio_tstBlock [iMb_xy - ai_iMb_stride] . NonZeroCount[42];
	}

// not last line
	if( (iMb_xy + ai_iMb_stride) < ai_iPicWidthInMbs){
		aio_tstBlock [iMb_xy + ai_iMb_stride] . NonZeroCount[4] = 0;
		aio_tstBlock [iMb_xy + ai_iMb_stride] . NonZeroCount[5] = 0;
		aio_tstBlock [iMb_xy + ai_iMb_stride] . NonZeroCount[6] = 0;
		aio_tstBlock [iMb_xy + ai_iMb_stride] . NonZeroCount[7] = 0;
		aio_tstBlock [iMb_xy + ai_iMb_stride] . NonZeroCount[1] = 0;
		aio_tstBlock [iMb_xy + ai_iMb_stride] . NonZeroCount[2] = 0;
		aio_tstBlock [iMb_xy + ai_iMb_stride] . NonZeroCount[25] = 0;
		aio_tstBlock [iMb_xy + ai_iMb_stride] . NonZeroCount[26] = 0;
	}
}


/**
Load motion vectors into MvCache table.
*/
static __inline void FillMvCacheCurrPosition(short MvCache[][2], short *MvdL0)
{

	MvCache[12 + 0][0] = *(MvdL0++);
	MvCache[12 + 0][1] = *(MvdL0++);
	MvCache[12 + 1][0] = *(MvdL0++);
	MvCache[12 + 1][1] = *(MvdL0++);
	MvCache[12 + 2][0] = *(MvdL0++);
	MvCache[12 + 2][1] = *(MvdL0++);
	MvCache[12 + 3][0] = *(MvdL0++);
	MvCache[12 + 3][1] = *MvdL0;
}

/**
Save motion vector.
*/
static __inline void FillMvCurrPosition(short *MvdL0, short MvCache[][2])
{

	*(MvdL0++) = MvCache[12 + 0][0];
	*(MvdL0++) = MvCache[12 + 0][1];
	*(MvdL0++) = MvCache[12 + 1][0];
	*(MvdL0++) = MvCache[12 + 1][1];
	*(MvdL0++) = MvCache[12 + 2][0];
	*(MvdL0++) = MvCache[12 + 2][1];
	*(MvdL0++) = MvCache[12 + 3][0];
	*(MvdL0)	= MvCache[12 + 3][1];
}

/**
Load reference indexes into RefCache table.
*/
static __inline void FillRefCacheCurrPosition(short *RefCache, short *Ref)
{
	
	RefCache[12] = RefCache[12 + 1] = RefCache[12 + 8] = RefCache[12 + 9] = Ref[0];
	RefCache[14] = RefCache[14 + 1] = RefCache[14 + 8] = RefCache[14 + 9] = Ref[1];
}

/**
Load reference indexes into RefCache table.
All reference are stored.
*/
static __inline void FillFullRefCacheCurrPosition(short *RefCache, short *Ref)
{
	
	RefCache[0] = Ref[0];
	RefCache[1] = Ref[1];
	RefCache[2] = Ref[2];
	RefCache[3] = Ref[3];
}



/**
Save reference indexes.
*/
static __inline void FillRefCurrPosition(short *Ref, short *RefCache)
{
	
	Ref[0] = RefCache[12];
	Ref[1] = RefCache[12 + 2];
}

/**
Save reference indexes.
All reference are saved.
*/
static __inline void FillFullRefCurrPosition(short *Ref, const short *RefCache)
{
	
	Ref[0] = RefCache[0];
	Ref[1] = RefCache[1];
	Ref[2] = RefCache[2];
	Ref[3] = RefCache[3];
}
#endif
