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


#ifndef MB_PRED_H
#define MB_PRED_H


#include "type.h"
#include "data.h"
#include "symbol.h" 
#include "refill.h"
#include "bitstream.h"
#include "ErrorsDetection.h"

void ReadCavlc16x16MotionVector(const unsigned char *ai_pcData, int *aio_piPosition, char *RefLx, short *RefCacheLx, short MvCacheLx[][2]);
void ReadCavlc8x16MotionVector(const unsigned char *ai_pcData, int *aio_piPosition, char *RefLx, short *RefCacheLx, short MvCacheLx[][2], int Index);
void ReadCavlc16x8MotionVector(const unsigned char *ai_pcData, int *aio_piPosition, char *RefLx, short *RefCacheLx, short MvCacheLx[][2], int Index);
int GetCavlcPRefList(const unsigned char *ai_pcData, int *aio_piPosition, char *refLx, int Mode, int NumRefLx);

int mb_pred_I( const unsigned char *ai_pcData, int *aio_piPosition, RESIDU *Residu, 
			  short *Mode, short *intra4x4_pred_mode_cache);
int sub_mb_pred_P(int *dct_allowed, const unsigned char *ai_pcData, int *aio_piPosition, 
				  const SLICE *Slice, RESIDU * residu, short mv_cache_l0[][2], short *ref_cache_l0);
int mb_pred_P ( const unsigned char *ai_pcData, int *aio_piPosition, const SLICE *Slice, 
				RESIDU *CurrResidu, short mv_cache_l0[][2], short *ref_cache_l0);



/**
Decodes Sub Macroblock type.
*/
static __inline void GetCavlcSubMbType(const unsigned char *Data,  int *Position, RESIDU *CurrResidu ){

	CurrResidu -> SubMbType [0] = (char ) read_ue(Data, Position);
	CurrResidu -> SubMbType [1] = (char ) read_ue(Data, Position);
	CurrResidu -> SubMbType [2] = (char ) read_ue(Data, Position);
	CurrResidu -> SubMbType [3] = (char ) read_ue(Data, Position);
}

/**
Decodes reference index for P macroblock.
*/
static __inline int GetCavlcPSubRefLx(const unsigned char *Data,  int *Position, char *RefIdxLx, int NumRefIdxLx){

	int mbPartIdx;
	for ( mbPartIdx = 0 ; mbPartIdx < 4 ; mbPartIdx++ ){
		RefIdxLx [mbPartIdx] = (char ) read_te(Data, Position,  NumRefIdxLx);
	}
#ifdef ERROR_DETECTION
	//Error detection
	if(ErrorsCheckSubRefLx(RefIdxLx, NumRefIdxLx)){
		return 1;
	}
#endif
	return 0;
}





/** 
This function allows to determine the most probable prediction mode of the current block 4x4.

@param intra4x4_pred_mode_cache A cache table where  prediction mode is stocked for each 4x4 block of the current macroblock.
@param index Index of the current block 4x4.
*/
static __inline short get_predicted ( short intra4x4_pred_mode_cache [], int index )
{
	//Récupération du mode de prediction du voisinage
	short min ;
	short index8 = SCAN8(index);
	short left = intra4x4_pred_mode_cache [index8 - 1];
	short top = intra4x4_pred_mode_cache [index8 - 8];

	min = left < top ? left : top ;
	return min < 0 ? INTRA_4x4_DC : min ;
}


/**
Decodes INTRA prediction mode.
*/
static __inline short GetCavlcIPredMode(const unsigned char *Data,  int *Position, short *intra4x4_pred_mode_cache, int luma4x4BlkIdx)
{

	short predicted = get_predicted(intra4x4_pred_mode_cache, luma4x4BlkIdx);

	if (!getNbits(Data, Position, 1)) 
	{
		short rem_intra4x4_pred_mode = (short) getNbits(Data, Position, 3);
		if ( rem_intra4x4_pred_mode < predicted){
			return rem_intra4x4_pred_mode;
		}else {
			return  rem_intra4x4_pred_mode + 1;
		}
	}

	return predicted;
}


#endif
