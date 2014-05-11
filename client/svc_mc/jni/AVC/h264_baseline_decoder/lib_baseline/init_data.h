/*****************************************************************************
   *
   *  Open SVC Decoder developped in IETR image lab
   *
   *
   *
   *              Médéric BLESTEL <mblestel@insa-rennes.Fr>
   *              Mickael RAULET <mraulet@insa-rennes.Fr>
   *			  Maxime  PELCAT <mpelcat@insa-rennes.Fr>
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
   * but WITHOUT ANY WARRANTY; without even the implied warranty off
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

#ifndef INIT_DATA_H
#define INIT_DATA_H

#include <string.h>
#include "type.h"
#include "symbol.h"


void init_non_zero_count_cache ( unsigned char non_zero_count_cache [], unsigned char val );
void init_intra_pred( int *ao_pstIntra);
void init_int_tab (const int ai_iSize, int ao_tiPicture_mb_type [ ] );
void InitUcharTab (const int ai_iSize, unsigned char ao_tiPicture_mb_type [ ] );
void init_data_block (const int ai_iSize, DATA ao_tstBlock[ ]);
void init_data_residu (const int ai_iSize, RESIDU *Block);



/**
This function permits to initialize the ref_cache table. 

@param ref_cache A cache table where the reference is stocked for each 4x4 block of the current macroblock.
*/
static __inline void init_ref_cache(short *ref_cache){

	memset(ref_cache,-1, 48 * sizeof(short));
}


/**
This function permits to initialize the mv_cache table. 

@param mv_cache A cache table where the motion vector are stocked for each 4x4 block of the current macroblock.
*/
static __inline void init_mv_cache(short mv_cache[][2]){

	memset(mv_cache,0, 96 * sizeof(short));
}


/**
This function permits to initialize the structure Block. 

@param ao_pstBlock The structure whxih contains all the data of each macroblock.

*/
static __inline void init_block (DATA *ao_pstBlock){

	memset (ao_pstBlock,'\0',sizeof(DATA));
}


static __inline void init_residu(RESIDU *Residu){

	memset (Residu,'\0',sizeof(RESIDU));
}

/**
This function permits to initialize the structure Block for a P skipped macroblock. 


@param ao_pstBlock The structure whxih contains all the data of the current macroblock.
@param residu The structure whxih contains all the residual data of the current macroblock.
*/
static __inline void init_P_skip_mb(RESIDU *residu, unsigned char LastQp, SLICE *Slice)
{
	residu -> Qp = LastQp;
	residu -> Mode = 4;
	residu -> MbType = P_Skip;
	residu -> SliceNum = Slice -> slice_num;
	residu -> ResidualPredictionFlag = Slice -> DefaultResidualPredictionFlag;
}



/**
This function permits to initialize the structure Block for a B skipped macroblock. 


@param ao_pstBlock The structure whxih contains all the data of the current macroblock.
@param residu The structure whxih contains all the residual data of the current macroblock.
*/
static __inline void init_B_skip_mb( RESIDU *residu, unsigned char LastQp, SLICE *Slice)
{
	residu -> Mode = 0;
	residu -> MbType = B_Skip;	
	residu -> Qp = LastQp;
	residu -> SliceNum = Slice -> slice_num;
	residu -> ResidualPredictionFlag = Slice -> DefaultResidualPredictionFlag;
}

#endif
