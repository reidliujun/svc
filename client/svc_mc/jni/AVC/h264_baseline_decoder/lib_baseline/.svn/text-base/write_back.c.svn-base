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


#include "symbol.h"
#include "write_back.h"
#include "fill_caches.h"



void NonZeroCountManagement( const unsigned char *Non_zero_count_cache, const short iMb_x, const short iMb_y, 
							const int PicWidthInMbs, const int PicHeightInMbs,DATA *aio_tstBlock, RESIDU *CurrentResidu)
{

	//Recording of the neighbourhood for the decoding of the VLC 
	write_back_non_zero_count(Non_zero_count_cache, iMb_x, iMb_y, PicWidthInMbs, PicHeightInMbs, aio_tstBlock);

	if ( aio_tstBlock -> Transform8x8){
		write_back_cbp_8x8(aio_tstBlock -> NonZeroCount, CurrentResidu);
	}else{
		write_back_cbp(aio_tstBlock -> NonZeroCount, CurrentResidu);
	}
}

/**
This function permits to write back the motion vector of the current macroblock.

@param ai_iB_stride The width of sample in the ao_tiMv table.
@param ai_iB8_stride The width of sample in the ao_tiRef table.
@param ai_tiMv A table where each motion vector calculated is stocked for each macroblock decoded.
@param ai_tiMv_cache A cache table where the motion vector are stocked for each 4x4 block of the current macroblock. 
@param ao_tiRef A table where each reference mode calculated is stocked for each macroblock decoded.
@param ai_tiRef_cache A cache table where the reference is stocked for each 4x4 block of the current macroblock.

*/
void WriteBackMotionCacheCurrPosition(const int ai_iB_stride, const int ai_iB8_stride, short ai_tiMv[ ], 
									  short ai_tiMv_cache[ ][2], short *ai_tiRef, short aio_tiRef_cache[ ])
{


	short y;


	//Write back the macroblock's parameters
	for(y = 0; y < 4; y++, ai_tiMv += ai_iB_stride){
		FillMvCacheCurrPosition(&ai_tiMv_cache[8 * y], ai_tiMv);
	}


	FillRefCacheCurrPosition(aio_tiRef_cache, ai_tiRef);
	FillRefCacheCurrPosition(&aio_tiRef_cache[16], &ai_tiRef[ai_iB8_stride]);
}




/**
This function enables to write back the reference index and the motio nvector of a macroblock.

@param ai_iB_stride The width of sample in the ao_tiMv table.
@param ai_iB8_stride The width of sample in the ao_tiRef table.
@param ai_iMb_xy Adress of the current macroblock.
@param ai_tiMv A table where each motion vector calculated is stocked for each macroblock decoded.
@param ai_tiMv_cache A cache table where the motion vector are stocked for each 4x4 block of the current macroblock. 
@param ai_tiRef A table where each reference mode calculated is stocked for each macroblock decoded.
@param ai_tiRef_cache A cache table where the reference is stocked for each 4x4 block of the current macroblock.
@param mb_stride Stride i macroblock of the picture.

*/

void write_back_motion_cache(const int ai_iB_stride, const int ai_iB8_stride, const int ai_iMb_xy, short ai_tiMv[ ], 
							 short ai_tiMv_cache[ ][2], short *ai_tiRef, short aio_tiRef_cache[ ], int mb_stride)
{

	const int top_xy = ai_iMb_xy - mb_stride  ;
	const int topleft_xy = top_xy - 1 ;
	const int topright_xy = top_xy >= 0 ? top_xy + 1:-1 ;
	const int left_xy = ai_iMb_xy - 1 ;


	WriteBackMotionCacheCurrPosition(ai_iB_stride, ai_iB8_stride, ai_tiMv, ai_tiMv_cache, ai_tiRef, aio_tiRef_cache);

	//In case of a skipped or a P macroblock
	if(top_xy >= 0)    {
		FillMvCacheTopPosition(ai_tiMv_cache, &ai_tiMv[- ai_iB_stride]);
		FillRefCacheTopPosition(aio_tiRef_cache, ai_tiRef [- ai_iB8_stride], ai_tiRef[1 - ai_iB8_stride]);
	}
	else     {
		FillRefCacheTopPosition(aio_tiRef_cache, PART_NOT_AVAILABLE, PART_NOT_AVAILABLE);
	}

	//In case of a skipped or a P macroblock
	if(left_xy >= 0)   {
		FillMvCacheLeftPosition(ai_tiMv_cache, &ai_tiMv[-2], ai_iB_stride);
		FillRefCacheLeftPosition(aio_tiRef_cache, ai_tiRef[-1], ai_tiRef[-1 + ai_iB8_stride]);    }
	else   {
		FillRefCacheLeftPosition(aio_tiRef_cache, PART_NOT_AVAILABLE, PART_NOT_AVAILABLE);
	}


	//In case of a skipped or a P macroblock
	if(topleft_xy >= 0){
		ai_tiMv_cache [12 - 1 - 8][0] = ai_tiMv[-2 - ai_iB_stride];
		ai_tiMv_cache [12 - 1 - 8][1] = ai_tiMv[-1 - ai_iB_stride];
		aio_tiRef_cache[12 - 1 - 8] = ai_tiRef[-1 - ai_iB8_stride];
	}
	else   {
		aio_tiRef_cache[12 - 1 - 8] = PART_NOT_AVAILABLE;
	}


	//In case of a skipped or a P macroblock
	if(topright_xy >= 0)    {
		ai_tiMv_cache [12 + 4 - 8][0] = ai_tiMv[8 - ai_iB_stride];
		ai_tiMv_cache [12 + 4 - 8][1] = ai_tiMv[9 - ai_iB_stride];
		aio_tiRef_cache[12 + 4 - 8] =  ai_tiRef[2 - ai_iB8_stride];
	}
	else    {
		aio_tiRef_cache[12 + 4 - 8] = PART_NOT_AVAILABLE;
	}
}









/**
This function permits to write back the motion vector of the current macroblock.

@param ai_iB_stride The width of sample in the ao_tiMv table.
@param ai_iB8_stride The width of sample in the ao_tiRef table.
@param ai_tiMv A table where each motion vector calculated is stocked for each macroblock decoded.
@param ai_tiMv_cache A cache table where the motion vector are stocked for each 4x4 block of the current macroblock. 
@param ao_tiRef A table where each reference mode calculated is stocked for each macroblock decoded.
@param ai_tiRef_cache A cache table where the reference is stocked for each 4x4 block of the current macroblock.

*/
void write_back_motion_cache_interpolation_8x8(const int ai_iB_stride, const int ai_iB8_stride, short ai_tiMv[ ], 
											   short ai_tiMv_cache[ ][2], short *ai_tiRef, short aio_tiRef_cache[ ])
{



	//Write back the macroblock's parameters
	ai_tiMv_cache[12][0] = ai_tiMv[0];
	ai_tiMv_cache[12][1] = ai_tiMv[1];
	ai_tiMv_cache[14][0] = ai_tiMv[4];
	ai_tiMv_cache[14][1] = ai_tiMv[5];

	ai_tiMv_cache[28][0] = ai_tiMv[2 * ai_iB_stride];
	ai_tiMv_cache[28][1] = ai_tiMv[2 * ai_iB_stride + 1];
	ai_tiMv_cache[30][0] = ai_tiMv[2 * ai_iB_stride + 4];
	ai_tiMv_cache[30][1] = ai_tiMv[2 * ai_iB_stride + 5];


	FillRefCacheCurrPosition(aio_tiRef_cache, ai_tiRef);
	FillRefCacheCurrPosition(&aio_tiRef_cache[16], &ai_tiRef[ai_iB8_stride]);
}





void write_back_P_skip_motion( const int ai_iB_stride, const int ai_iB8_stride, short MvdL0[ ], short ai_tiMv_cache[2], short ao_tiRef[ ], const short ai_tiRef_cache)
{

	short y;

	for(y = 0; y < 4; y++, MvdL0 += ai_iB_stride - 7){
		*(MvdL0++) = ai_tiMv_cache[0];
		*(MvdL0++) = ai_tiMv_cache[1];
		*(MvdL0++) = ai_tiMv_cache[0];
		*(MvdL0++) = ai_tiMv_cache[1];
		*(MvdL0++) = ai_tiMv_cache[0];
		*(MvdL0++) = ai_tiMv_cache[1];
		*(MvdL0++) = ai_tiMv_cache[0];
		*(MvdL0)   = ai_tiMv_cache[1];
	}

	ao_tiRef[0] = ao_tiRef[1] = ao_tiRef[ai_iB8_stride] = ao_tiRef[1 + ai_iB8_stride] = ai_tiRef_cache;
}


/**
This function permits to write back the motion vector of the current macroblock.

@param ai_iB_stride The width of sample in the ao_tiMv table.
@param ai_iB8_stride The width of sample in the ao_tiRef table.
@param MvdL0 A table where each motion vector calculated is stocked for each macroblock decoded.
@param ai_tiMv_cache A cache table where the motion vector are stocked for each 4x4 block of the current macroblock. 
@param ao_tiRef A table where each reference mode calculated is stocked for each macroblock decoded.
@param ai_tiRef_cache A cache table where the reference is stocked for each 4x4 block of the current macroblock.

*/
#ifdef TI_OPTIM
void write_back_motion_TI( const int ai_iB_stride, const int ai_iB8_stride , short MvdL0[ ],short  ai_tiMv_cache[ ][2], 
						  short ao_tiRef[ ], short ai_tiRef_cache[])
{
	//1 Iteration
	_mem8(MvdL0)  =_mem8(ai_tiMv_cache+12);
	_mem8(MvdL0+4)=_mem8(ai_tiMv_cache+14);
	MvdL0 += ai_iB_stride;
	//2 iteration

	_mem8(MvdL0)  =_mem8(ai_tiMv_cache+20);
	_mem8(MvdL0+4)=_mem8(ai_tiMv_cache+22);
	MvdL0 += ai_iB_stride;

	//3 iteration
	_mem8(MvdL0)  =_mem8(ai_tiMv_cache+28);
	_mem8(MvdL0+4)=_mem8(ai_tiMv_cache+30);
	MvdL0 += ai_iB_stride;

	//4 iteration
	_mem8(MvdL0)  =_mem8(ai_tiMv_cache+36);
	_mem8(MvdL0+4)=_mem8(ai_tiMv_cache+38);
	MvdL0 += ai_iB_stride;
	_mem4(ao_tiRef)=(ai_tiRef_cache[14]<<16|ai_tiRef_cache[12]);
	_mem4(ao_tiRef+ai_iB8_stride)=(ai_tiRef_cache[30]<<16|ai_tiRef_cache[28]);
}
#else


void write_back_motion_C( const int ai_iB_stride, const int ai_iB8_stride, short MvdL0[ ], short ai_tiMv_cache[ ][2], short ao_tiRef[ ], short ai_tiRef_cache[])
{

	short y;

	for(y = 0; y < 4; y++, MvdL0 += ai_iB_stride){
		FillMvCurrPosition(MvdL0, &ai_tiMv_cache[8 * y]);


		FillRefCurrPosition(ao_tiRef, ai_tiRef_cache);
		FillRefCurrPosition(&ao_tiRef[ai_iB8_stride], &ai_tiRef_cache[16]);	
	}
}
#endif
