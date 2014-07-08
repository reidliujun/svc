/*****************************************************************************
*
*  SVC4DSP developped in IETR image lab
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

#include "SVCDecoder_ietr_api.h"

/**
This function is used to give the command to the decoder.
*/
void SetCommandLayer(int *Command, int DqIdMax, int CurrDqId, int *TemporalCom, int TemporalId)
{

	Command[0] = DqIdMax;
	Command[1] = CurrDqId;
	Command[2] = *TemporalCom;
	Command[3] = TemporalId;

	//Once set, the command can be reset.
	*TemporalCom = 0;
}

/**
Used by players in order to change of layer.
*/
void UpdateLayer(int *DqIdTable, int *CurrDqId, int *TemporalCom, int *TemporalId, int MaxDqId, int Command){

	int i;
	
	//Get current layer and max dqDid
	for( i = 0; i < 8 && DqIdTable [i] != -1; i++){
		if(*CurrDqId == DqIdTable [i]){
			break;
		}
	}

	switch(Command){
		case 0://Set layer Down
			if(i > 0){
				*CurrDqId = DqIdTable [i - 1];
			}
			break;
		case 1://Set layer up
			if(i < 7 && DqIdTable [i + 1] != -1){
				*CurrDqId = DqIdTable [i + 1];
			}
			break;
		case 2:
			//Set layer max
			*CurrDqId = MaxDqId;
			break;	
		case 4:
			*TemporalCom = 1;
			break;
		case 5:
			*TemporalCom = 2;
			break;
		case 6:
			//only mplayer
			//to start with a specific temporal layer
			if(MaxDqId < 255 && MaxDqId >= 0){
				*TemporalId = MaxDqId;
				*TemporalCom = 3;
			}
			break;
	}
}
