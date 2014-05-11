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

//AVC Files
#include "symbol.h"

//SVC Files
#include "svc_type.h"
#include "PartitionType.h"

//Determine in which configuration a 8x8 block is
char Determine8x8Part(REFPART RefPartition[16]){

	char index = 0;
	//For 8x4
	if ( RefPartition [0] . PartIdc == RefPartition [1] . PartIdc && RefPartition [4] . PartIdc == RefPartition [5] . PartIdc){
		index += 2;
	}

	//for 4x8 
	if ( RefPartition [0] . PartIdc == RefPartition [4] . PartIdc && RefPartition [1] . PartIdc == RefPartition [5] . PartIdc){
		index ++;
	}

	return 3 - index;
}



//Determine in which configuration a macroblock is
void DetermineMacroPart(int SliceType, RESIDU *Residu, char Mode8x8[4], REFPART RefPartition[16]){
	
	char index = 0;

	//For 16x8
	if ( RefPartition [0] . PartIdc == RefPartition [2] . PartIdc && Mode8x8 [0] == Mode8x8 [1] && 
		 RefPartition [8] . PartIdc == RefPartition [10] . PartIdc && Mode8x8 [2] == Mode8x8 [3]){
		index += 2;
	} 

	//For 8x16
	if ( RefPartition [0] . PartIdc == RefPartition [8] . PartIdc && Mode8x8 [0] == Mode8x8 [2] && 
		 RefPartition [2] . PartIdc == RefPartition [10] . PartIdc && Mode8x8 [1] == Mode8x8 [3]){
		index ++;
	}

	Residu -> Mode = index;
	//In case of 8x8 mode, initialize sub macroblock
	if ( !index){
		for (index = 0; index < 4; index++){
			if ( SliceType == SLICE_TYPE_B){
				/*initialize the partionning according to Mode.
				*Mode == 3, SubMbType like 4x4 biPred
				*Mode == 2, SubMbType like 4x8 biPred
				*Mode == 1, SubMbType like 8x4 biPred
				*Mode == 0, SubMbType like 8x8 biPred*/
				Residu -> SubMbType [index] = Mode8x8 [index] == 3 ? 12: Mode8x8 [index] == 2 ? 9: Mode8x8 [index] == 1 ? 8 : 3;
			}else {
				Residu -> SubMbType [index] = Mode8x8 [index];
			}
		}
	}		
}



//Determine in which configuration a macroblock is
void SetPartionning(int SliceType, RESIDU *Residu, REFPART RefPartition[16]){

	char Mode8x8[4];
	short i;

	//Loop over each block 8x8 to know the type.
	for ( i = 0; i < 4; i++){
		Mode8x8[i] = Determine8x8Part(&RefPartition[((i & 1) << 1) + ((i & 2) << 2)]);
	}

	DetermineMacroPart(SliceType, Residu, Mode8x8, RefPartition);
}
