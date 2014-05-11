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




#ifndef VLC_CABAC_SVC_H
#define VLC_CABAC_SVC_H


#include "mb_pred_main.h"


char SubMbPCabacSVC ( int *dct_allowed, CABACContext *c, unsigned char *state, SLICE *slice, DATA *macroblock, RESIDU *Current_residu, short mv_cache_l0[][2], short  *ref_cache_l0);
char SubMbBCabacSVC ( int *dct_allowed, int Dir8x8infFlag, CABACContext *c, unsigned char *state, SLICE *slice, DATA *macroblock, 
					 RESIDU *CurrentResidu, short mv_cache_l0[][2], short  *ref_cache_l0, short mv_cache_l1[][2], short  *ref_cache_l1);
char MbPredPCabacSvc ( CABACContext *c, unsigned char *state, SLICE *slice, RESIDU *CurrResidu, DATA *macroblock, short mv_cache_l0[][2], short *ref_cache_l0);
char MbPredBCabacSvc ( CABACContext *c, unsigned char *state, SLICE *Slice, RESIDU *CurrResidu, DATA *macroblock, short mv_cache_l0[][2], 
					  short *ref_cache_l0, short mv_cache_l1[][2], short *ref_cache_l1);



/**
Decode MotionPred flag for CABAC.
*/
static __inline unsigned char CabacGetMotionPredFlag(CABACContext *c, unsigned char *state, int Idx)
{

	return (unsigned char) get_cabac( c, &state[463 + Idx]);
}



/**
Decode MotionPred flag for CABAC for a SVC slice.
*/
static __inline int GetCabacSVCPMotionPred(CABACContext *c, unsigned char *state, SLICE *slice, DATA *macroblock, 
										   char *MotionPred, int InCropInWindow, int Pred_Lx, int Comptx)
{
	if( slice -> AdaptiveMotionPredictionFlag && InCropInWindow) {
		//Recovery of of the image reference for the frame P
		int mbPartIdx;
		for ( mbPartIdx = 0 ; mbPartIdx < macroblock -> NumMbPart ; mbPartIdx++ ) {
			if (macroblock -> MbPartPredMode [mbPartIdx] != Pred_Lx) {
				MotionPred [mbPartIdx] = CabacGetMotionPredFlag( c, state, Comptx);
			}
		}
	}else{
		MotionPred[0] = MotionPred[1] = MotionPred[2] = MotionPred[3] = slice -> DefaultMotionPredictionFlag;
	}
#ifdef ERROR_DETECTION
	//error detection
	if(ErrorsCheckSubRefLx(MotionPred, 1)){
		return 1;
	}
#endif
	return 0;
}



/**
Decode sub macroblock motion vectors for CABAC.
*/
static __inline int GetCabacSVCBSubMotionPred(CABACContext *c, unsigned char *state, SLICE *slice, char *MotionPred, 
										   unsigned char *SubMbType, int MbPart, int InCropInWindow, int Index, int PredLx)
{
	if( slice -> AdaptiveMotionPredictionFlag && InCropInWindow) {
		//Recovery of of the image reference for the frame P
		int mbPartIdx;
		for ( mbPartIdx = 0 ; mbPartIdx < MbPart ; mbPartIdx++ ) {
			int SubType = sub_mb_b_name [SubMbType [mbPartIdx]];
			if ( (SubType != B_direct) && (SubType != PredLx )) {
				MotionPred [mbPartIdx] = CabacGetMotionPredFlag( c, state, Index);
			}
		}
	}else{
		MotionPred[0] = MotionPred[1] = MotionPred[2] = MotionPred[3] = slice -> DefaultMotionPredictionFlag;
	}
#ifdef ERROR_DETECTION
	//error detection
	if(ErrorsCheckSubRefLx(MotionPred, 1)){
		return 1;
	}
#endif
	return 0;
}



#endif
