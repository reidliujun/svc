

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



#ifndef SVC_WRITE_BACK_H
#define SVC_WRITE_BACK_H

#include "write_back.h"


#ifdef TI_OPTIM
#define write_back_full_ref write_back_full_ref_TI
#define write_back_P_skip_motion_full_ref write_back_P_skip_motion_full_ref_TI
#define write_back_motion_cache_B_full_ref write_back_motion_cache_B_full_ref_TI
#else
#define write_back_full_ref write_back_full_ref_C
#define write_back_P_skip_motion_full_ref write_back_P_skip_motion_full_ref_C
#define write_back_motion_cache_B_full_ref write_back_motion_cache_B_full_ref_C
#endif

void write_back_motion_cache_B_full_ref( const int ai_iB_stride, const int ai_iB8_stride, const int ai_iMb_xy, 
										short ai_tiMv_l0[ ], short ai_tiMv_l1[ ], short  ai_tiMv_cache_l0[ ][2], 
										short  ai_tiMv_cache_l1[ ][2], short ai_tiRef_l0[ ], short ai_tiRef_l1[ ],
										short aio_tiRef_cache_l0[ ], short aio_tiRef_cache_l1[ ],int mb_stride);


#ifdef TI_OPTIM
static __inline void write_back_P_skip_motion_full_ref_TI( const int ai_iB_stride, const int ai_iB8_stride , short MvdL0[ ], short ai_tiMv_cache[2], short ao_tiRef[ ])
{


	long long aux_ai_tiMv_cache;
	aux_ai_tiMv_cache=_mem4(ai_tiMv_cache);
	aux_ai_tiMv_cache=_itoll(aux_ai_tiMv_cache,aux_ai_tiMv_cache);
	//Iteration 1
	_mem8(MvdL0)  =aux_ai_tiMv_cache;
	_mem8(MvdL0+4)=aux_ai_tiMv_cache;
	//Iteration 2
	MvdL0 += ai_iB_stride;
	_mem8(MvdL0)  =aux_ai_tiMv_cache;
	_mem8(MvdL0+4)=aux_ai_tiMv_cache;

	//Iteration 3
	MvdL0 += ai_iB_stride;
	_mem8(MvdL0)  =aux_ai_tiMv_cache;
	_mem8(MvdL0+4)=aux_ai_tiMv_cache;

	//Iteration 4
	MvdL0 += ai_iB_stride;
	_mem8(MvdL0)  =aux_ai_tiMv_cache;
	_mem8(MvdL0+4)=aux_ai_tiMv_cache;

	_mem8(ao_tiRef)=0;
	_mem8(ao_tiRef+ai_iB8_stride)=0;
	_mem8(ao_tiRef+(ai_iB8_stride<<1))=0;
	_mem8(ao_tiRef+(ai_iB8_stride * 3))=0;

}

static __inline void write_back_full_ref_TI(const int ai_iB_stride , const int ai_iB8_stride, short ai_tiMv[ ], short ai_tiMv_cache[ ][2], short *ai_tiRef, short aio_tiRef_cache[ ]){



	//1 iteration
	_mem8(ai_tiMv_cache+12)=_mem8(ai_tiMv);
	_mem8(ai_tiMv_cache+14)=_mem8(ai_tiMv+4);
	ai_tiMv += ai_iB_stride;
	//2 Iteration
	_mem8(ai_tiMv_cache+20)=_mem8(ai_tiMv);
	_mem8(ai_tiMv_cache+22)=_mem8(ai_tiMv+4);
	ai_tiMv += ai_iB_stride;
	//3 iteration
	_mem8(ai_tiMv_cache+28)=_mem8(ai_tiMv);
	_mem8(ai_tiMv_cache+30)=_mem8(ai_tiMv+4);
	ai_tiMv += ai_iB_stride;
	//4 iteration
	_mem8(ai_tiMv_cache+36)=_mem8(ai_tiMv);
	_mem8(ai_tiMv_cache+38)=_mem8(ai_tiMv+4);
	ai_tiMv += ai_iB_stride;

	//Second for
	//1 Iteration
	_mem8(aio_tiRef_cache+12)=_mem8(ai_tiRef);
	ai_tiRef += ai_iB8_stride;
	//2 Iteration
	_mem8(aio_tiRef_cache+20)=_mem8(ai_tiRef);
	ai_tiRef += ai_iB8_stride;
	//3 Iteration
	_mem8(aio_tiRef_cache+28)=_mem8(ai_tiRef);
	ai_tiRef += ai_iB8_stride;
	//4 Iteration
	_mem8(aio_tiRef_cache+36)=_mem8(ai_tiRef);
	ai_tiRef += ai_iB8_stride;
}

#endif

/**
Save motion vectors and reference indexes of the current macroblock.
All reference indexes are saved.
*/
static __inline void write_back_P_skip_motion_full_ref_C( const int ai_iB_stride, const int ai_iB8_stride, 
														 short *MvdL0, short *ai_tiMv_cache, short *ao_tiRef)
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


	ao_tiRef[0] = ao_tiRef[1] = ao_tiRef[2] = ao_tiRef[3] =
	ao_tiRef[ai_iB8_stride] = ao_tiRef[1 + ai_iB8_stride] = 
	ao_tiRef[2 + ai_iB8_stride] = ao_tiRef[3 + ai_iB8_stride] = 
	ao_tiRef[2 * ai_iB8_stride] = ao_tiRef[1 + 2 * ai_iB8_stride] = 
	ao_tiRef[2 + 2 * ai_iB8_stride] = ao_tiRef[3 + 2 * ai_iB8_stride] = 0;
	ao_tiRef[3 * ai_iB8_stride] = ao_tiRef[1 + 3 * ai_iB8_stride] = 
	ao_tiRef[2 + 3 * ai_iB8_stride] = ao_tiRef[3 + 3 * ai_iB8_stride] = 0;
}



/**
Load motion vectors and reference indexes into local tables.
*/
static __inline void write_back_full_ref_C(const int ai_iB_stride, const int ai_iB8_stride, short *ai_tiMv,
										   short ai_tiMv_cache[][2], short *ai_tiRef, short *aio_tiRef_cache){



	short y;


	//Write back the macroblock's parameters
	for(y = 0; y < 4; y++, ai_tiMv += ai_iB_stride, ai_tiRef += ai_iB8_stride){
		FillMvCacheCurrPosition(&ai_tiMv_cache[8 * y], ai_tiMv);
		FillFullRefCacheCurrPosition(&aio_tiRef_cache[12 + 8 * y], ai_tiRef);
	}
}




/**
Load motion vectors and reference indexes into local tables.
*/
static __inline void write_back_full_ref_8x8(const int ai_iB_stride, const int ai_iB8_stride, short *ai_tiMv, 
											 short ai_tiMv_cache[][2], short *ai_tiRef, short *aio_tiRef_cache){


	//Write back the macroblock's parameters

	ai_tiMv_cache[12][0] = ai_tiMv[0];
	ai_tiMv_cache[12][1] = ai_tiMv[1];
	ai_tiMv_cache[14][0] = ai_tiMv[4];
	ai_tiMv_cache[14][1] = ai_tiMv[5];

	ai_tiMv_cache[28][0] = ai_tiMv[2 * ai_iB_stride];
	ai_tiMv_cache[28][1] = ai_tiMv[2 * ai_iB_stride + 1];
	ai_tiMv_cache[30][0] = ai_tiMv[2 * ai_iB_stride + 4];
	ai_tiMv_cache[30][1] = ai_tiMv[2 * ai_iB_stride + 5];


	aio_tiRef_cache[12] =  ai_tiRef[0];
	aio_tiRef_cache[14 + 0] = ai_tiRef[2];

	aio_tiRef_cache[12 + 0 + 16] = ai_tiRef[2 * ai_iB8_stride];
	aio_tiRef_cache[14 + 0 + 16] = ai_tiRef[2 * ai_iB8_stride + 2];
}



/**
save motion vectors and reference indexes into local tables.
*/
static __inline void write_back_motion_full_ref( const int ai_iB_stride, const int ai_iB8_stride, short *MvdL0, 
												short ai_tiMv_cache[][2], short *ao_tiRef, const short *ai_tiRef_cache)
{

	short y;

	for(y = 0; y < 4; y++, MvdL0 += ai_iB_stride, ao_tiRef += ai_iB8_stride){
		FillMvCurrPosition(MvdL0, &ai_tiMv_cache[8 * y]); 
		FillFullRefCurrPosition(ao_tiRef, &ai_tiRef_cache[12 + 8 * y]);
	}
}


#endif
