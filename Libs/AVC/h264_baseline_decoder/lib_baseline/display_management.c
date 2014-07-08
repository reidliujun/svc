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

#include "data.h"
#include "string.h"
#include "display_management.h"




/**
This function stores into the frame buffer the input frame.
*/
void StorePicture(POC_PARAMS *Frame, LIST_MMO *Current_pic, SPS *sps, int layer_number){


	//Stock the new frame in reorder buffer
	memcpy(Frame -> Picture , Current_pic, sizeof(LIST_MMO));

	Frame -> x_size = ((sps -> pic_width_in_mbs) << 4) - sps -> CropRight - sps -> CropLeft;
	Frame -> y_size = ((sps -> pic_height_in_map_units) << 4) - sps -> CropBottom - sps -> CropTop;
	Frame -> Crop = (sps -> CropTop << 24) + (sps -> CropBottom << 16) + (sps -> CropLeft << 8) + sps -> CropRight;
	Frame -> LayerId = layer_number;


	//For SVC in case of ghost picture
	if (Current_pic -> displayed == 255){
		Frame -> is_ghost = 1;
	} else {
		Frame -> is_ghost = 0;
	}
}


/**
Finds the next picture to display.
*/
POC_PARAMS* FindBestFrame(MMO *mmo, POC_PARAMS *FrameList, int *BestPos, int MinPoc){

	int j;
	int IDR = -1;
	int best_poc = MAX_POC;
	*BestPos = -1;


	//Find IDR frame with POC == 0
	for(j = 0; j < mmo -> nb_img_display - mmo -> num_free_poc_address ; j++){
		if (FrameList[j] . Picture -> poc == 0){	
			if(j != 0){
				IDR = j;
				break;
			}
		}
	}

	if(IDR != -1){
		//Just take the minus one until the next IDR
		for(j = 0; j < IDR ; j++){
			if ( FrameList[j] . Picture -> poc < best_poc && FrameList[j] . Picture -> poc > MinPoc) {
				best_poc = FrameList[j] . Picture -> poc;
				*BestPos = j;
			}
		}
	}else {
		for(j = 0; j < mmo -> nb_img_display - mmo -> num_free_poc_address ; j++){
			//Test if exists a frame with a POC inferior to the first in the head.
			//This frame must have a POC superior to the previous one displayed
			//Or in case of multiple IDR with the JSVM, POC can be negative
			if ( FrameList[j] . Picture -> poc < best_poc && FrameList[j] . Picture -> poc > MinPoc){
				best_poc = FrameList[j] . Picture -> poc;
				*BestPos = j;
			}
		}
	}

	if(*BestPos == -1){
		//Best poc not found
		//Just take the minus one
		for(j = 0; j < mmo -> nb_img_display - mmo -> num_free_poc_address ; j++){
			if ( FrameList[j] . Picture -> poc < best_poc) {
				best_poc = FrameList[j] . Picture -> poc;
				*BestPos = j;
			}
		}
	}

	return &FrameList [*BestPos];
}



/**
This function is used to set as displayed, frames dismissed from the output renderer 
which come from a different layer.
*/
char SetAsDisplayed(LAYER_MMO *LayerMmo, int best_poc, int address_pic)
{
	int i;

	for( i = 0; i < LayerMmo -> ShortRefCount; i++){
		if ((best_poc == LayerMmo -> ShortRef[i] . poc) &&
			(LayerMmo -> ShortRef[i] . reference == 3) &&
			(LayerMmo -> ShortRef[i] . MemoryAddress == address_pic)){
				LayerMmo -> ShortRef[i] . displayed = 1;
				return 1;
		}
	}

	for( i = 0; i < MAX_REF; i++){
		if ((best_poc == LayerMmo -> LongRef[i] . poc) && 
			(LayerMmo -> LongRef[i] . reference == 3) && 
			(LayerMmo -> LongRef[i] . MemoryAddress == address_pic) ){
				LayerMmo -> LongRef[i] . displayed = 1;
				return 1;
		}
	}
	return 0;
}



/**
Determines if the memory of frame to display has to be released.
*/
void CurrentDisplay(MMO *CurrMmo, LAYER_MMO *LayerMmo, LIST_MMO *Frame, int *address_pic){

	int keep = 0;
	const int best_poc = Frame -> poc;
	*address_pic = Frame -> MemoryAddress;

	keep = SetAsDisplayed(LayerMmo, best_poc, *address_pic);

	if (!keep){
		CurrMmo -> FreeMemoryAddress [CurrMmo -> NumFreeAddress] = *address_pic;
		CurrMmo -> NumFreeAddress++; 
	}
}
