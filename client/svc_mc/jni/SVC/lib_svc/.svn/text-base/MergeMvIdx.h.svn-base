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

#ifndef MERGEMVIDX_H
#define MERGEMVIDX_H

#include "math.h"

static __inline short MvDiff (short *MvA, short *MvB){
	
	return (short) (abs (MvA[0] - MvB[0]) + abs (MvA[1] - MvB[1]));
}



static __inline short MinRef(short RefA, short RefB){
	
	if (RefA < 0) return RefB; 
	if (RefB < 0) return RefA; 
	return MIN(RefA, RefB);
}

static __inline short MinRef4x4(short RefInded4x4[]){

	short Min = MinRef(RefInded4x4[0], RefInded4x4[1]);
	Min = MinRef(Min, RefInded4x4[4]);
	Min = MinRef(Min, RefInded4x4[5]);
	return Min;
}


void SetPartPred(REFPART RefPartition[16], short *ref_l0);
void UpdateRefPartition(const NAL *Nal, REFPART RefPartition[16]);
void CombineMv(char Mode8x8[4], short mv_cache_l0[][2], const char *MotionPred);
void MergeMv(const NAL *Nal, const int Direct8x8Inference, REFPART RefPartition[16], const char *MotionPred, short mv_cache_l0[][2], char Mode[4], short *mv_l0);
#endif
