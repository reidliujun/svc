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

#ifndef CABACMVREF_H
#define CABACMVREF_H

/**
Decode CABAC sub macroblock reference lists for a P slice.
*/
static __inline int GetCabacPSubRefIdx(CABACContext *c, unsigned char *state, char *RefIdxLx, short *CabacRefCacheLx, char *MotionPred, int NumRefLx)
{
	int mbPartIdx;
	for ( mbPartIdx = 0 ; mbPartIdx < 4 ; mbPartIdx++ ) {
		if (!MotionPred[mbPartIdx]){
			const int ind = SCAN8(mbPartIdx << 2);
			int refa = CabacRefCacheLx[ind - 1];
			int refb = CabacRefCacheLx[ind - 8];
			CabacRefCacheLx[ind] = CabacRefCacheLx[ind + 1] =	
			CabacRefCacheLx[ind + 8] = CabacRefCacheLx[ind + 9] = RefIdxLx [mbPartIdx] = decode_cabac_mb_ref(c, state, refa, refb); 
		}
	}
#ifdef ERROR_DETECTION
	//error detection
	if(ErrorsCheckSubRefLx(RefIdxLx, NumRefLx)){
		return 1;
	}
#endif
	return 0;
}



/**
Decode CABAC sub macroblock motion vectors.
*/
static __inline void ReadCabacSubMotionVector(CABACContext *c, unsigned char *state, short MvCacheLx[][2], short *MvdLX, 
											  int mbPartIdx, int subMbPartIdx, int sub_mb_type, int index, int SliceType)
{
	int compIdx;
	
	for ( compIdx = 0 ; compIdx < 2 ; compIdx++ ) {
		int mva, mvb;
		get_mv_neighbourhood(&mva, &mvb, MvCacheLx, sub_mb_type, mbPartIdx, subMbPartIdx, compIdx, SliceType);
		MvdLX[compIdx] = decode_cabac_mb_mvd(c, state, mva, mvb, compIdx);
	}
	FillSubMotionVector(MvdLX, &MvCacheLx[index], sub_mb_type, SliceType);
}



/**
Decode CABAC macroblock motion vectors.
*/
static __inline void ReadCabacMotionVector(CABACContext *c, unsigned char *state, short MvCacheLx[][2], 
										   short *MvdLX, int Mode, int index)
{
	int compIdx;
	for ( compIdx = 0 ; compIdx < 2 ; compIdx++ ) {
		int mva = MvCacheLx[index - 1][compIdx];
		int mvb = MvCacheLx[index - 8][compIdx];
		MvdLX[compIdx] = decode_cabac_mb_mvd(c, state, mva, mvb, compIdx);
	}
	FillMotionVector(MvdLX, &MvCacheLx[index], Mode);
}


int GetCabacPRefList(CABACContext *c, unsigned char *state, char *refLx, short *RefCacheLx,
					 char *MotionPred, int Mode, int NumRefLx, int index);

int GetCabacBSubRefList(CABACContext *c, unsigned char *state, char *refLx, short *RefCacheLx,
						char *MotionPred, unsigned char *SubMbType, int NumRefLx, int Pred_Lx);

int GetCabacBRefList(CABACContext *c, unsigned char *state, char *refLx, short *RefCacheLx, char *MotionPred,
					 int Mode, int NumRefLx, unsigned char *MbPartMode, short Pred_Lx, int index);

void ReadAndComputeCabac8x8MotionVector( CABACContext *c, unsigned char *state, int mbPartIdx, int sub_mb_type, char *RefLx, 
							  short *RefLxCache, short MvCacheLx[][2], short MvLxCache[][2], int SliceType);

void ReadAndComputeCabac16x16MotionVector( CABACContext *c, unsigned char *state, char *RefLx, short *RefLxCache,
								short MvCacheLx[][2], short mvLx_cache[][2]);

void ReadAndComputeCabac8x16MotionVector( CABACContext *c, unsigned char *state, char *RefLx, short *RefLxCache,
							   short MvCacheLx[][2], short mvLx_cache[][2], int index);

void ReadAndComputeCabac16x8MotionVector( CABACContext *c, unsigned char *state, char *RefLx, short *RefLxCache,
							   short MvCacheLx[][2], short mvLx_cache[][2], int index);
#endif
