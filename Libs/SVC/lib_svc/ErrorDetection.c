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


#include "math.h"
#include "string.h"

//AVC Files
#include "memory_management.h"
#include "display_management.h"

//SVC Files
#include "svc_type.h"
#include "symbol.h"
#include "pic_list.h"
#include "set_layer.h"
#include "ErrorDetection.h"
#include "FinishFrameSVC.h"
#include "picture_management.h"


/**
Detect end of slice error.
Compare slice parameters with previous slice.
*/
void DetectSliceError(NAL *Nal, SPS *sps, PPS *pps, LIST_MMO *Current_pic, MMO *Mmo, 
					  int EndOfSlice, int PrevEndOfSlice, int *Crop, int *ImgToDisplay, 
					  int *AddressPic, int *xsize, int *ysize)
{
	//Detect slice error in the previous frame
	//when slice detected and it was false, and the layer has to be decoded
	if (!EndOfSlice && (PrevEndOfSlice || Nal -> ErrorDetected) && Nal -> LastDqId <= Nal -> DqIdToDisplay){
		//In case of a slice on an upper layer, we don't know when the picture is finished
		//so this is the only way to know.
		NAL LastStructNal[1];
		memcpy(LastStructNal, Nal, sizeof(NAL));
		LastStructNal -> LayerId = Mmo -> LayerId;
		if(Nal -> layer_id_table [Mmo -> LayerId] != -1){
			LastStructNal -> DqId = Nal -> layer_id_table [Mmo -> LayerId];
		}
		else{		
			//Assumes, that this problem occurs only with AVC stream.
			LastStructNal -> DqId = 0;
		}

		//DqIdMax can change
		LastStructNal -> DqIdMax = Nal -> LastDqIdMax;

		FinishFrameSVCError(1, LastStructNal, sps, pps, Current_pic, Crop, ImgToDisplay, AddressPic, Mmo, xsize, ysize);
	} 
}


/**
In case of no place in the display buffer, a frame is set as displayed.
*/
void DetecMissingTopLayer(NAL *Nal, SPS *sps, PPS *pps, LIST_MMO *Current_pic, MMO *Mmo, 
						  int *Crop, int *ImgToDisplay, int *AddressPic, int *xsize, int *ysize)
{
	//Missing top layer, so no frame has been displayed
	//Add this condition Nal -> LastDqId < Nal -> DqIdMax, because when top layer is missing, DqIdMax changes,
	//and this operation is done on the last frame of the top layer, and not on the first missing.
	if(!Mmo -> num_free_poc_address	&& Nal -> DqId == 0 && (Nal -> LastDqId != Nal -> LastDqIdMax && Nal -> LastDqId < Nal -> LastDqIdMax)){
		int AlreadyIn = 0, i;
		for (i = 0; i < Mmo -> nb_img_display - Mmo -> num_free_poc_address; i++){
			if((Current_pic -> poc == Mmo -> poc_params[i] . Picture -> poc) && 
				(Current_pic -> MemoryAddress == Mmo -> poc_params[i] . Picture -> MemoryAddress)){
				AlreadyIn = 1;
				break;
			}
		}
		
		if(!AlreadyIn){
			NAL LastStructNal[1];
			LAYER_MMO *LayerMmo = &Mmo -> LayerMMO [Nal -> LayerId];
			memcpy(LastStructNal, Nal, sizeof(NAL));
			LastStructNal -> DqId = Nal -> LastDqIdMax; // Set the DqId to the DqIdmax to "display" a frame
			LastStructNal -> LayerId = (unsigned char) GetLayerNumber(Nal, Nal -> LastDqIdMax);
			LastStructNal -> DqIdMax = Nal -> LastDqIdMax;
			
			//Provides new addresses
			ProvideMemAddress(Current_pic, Mmo, LayerMmo);

			//Store the new picture
			FinishFrameSVCError(0, LastStructNal, sps, pps, Current_pic, Crop, ImgToDisplay, AddressPic, Mmo, xsize, ysize);
		}
	}
}


/**
Detect difference between POC of different layer.
*/
void DetectPocProblem(NAL *Nal, MMO *Mmo, LIST_MMO *Current_pic, LAYER_MMO *LayerMmo, SPS *sps)
{

	if(Nal -> DqId && Current_pic -> poc != Mmo -> LastDecodedPoc){
		const int MaxPicOrderCntLsb = 1 << (sps -> log2_max_pic_order_cnt_lsb);

		//To save it
		short LastDecodedPoc = Mmo -> LastDecodedPoc;


		//Compare LSB
		if( LayerMmo -> prevPicOrderCntLsb != Mmo -> PrevPicOrderCntLsb){
			 LayerMmo -> prevPicOrderCntLsb = Mmo -> PrevPicOrderCntLsb;
		}

		//Compare MSB
		if( LayerMmo -> prevPicOrderCntMsb != Mmo -> PrevPicOrderCntMsb){
			 LayerMmo -> prevPicOrderCntMsb = Mmo -> PrevPicOrderCntMsb;
		}

		if(abs(LayerMmo -> pic_order_cnt_lsb) > MaxPicOrderCntLsb){
			 LayerMmo -> pic_order_cnt_lsb = Mmo -> LastDecodedPoc - LayerMmo -> prevPicOrderCntMsb;
		}

		//Compute POC again
		Current_pic -> poc = PocComputation(sps, Nal -> IdrFlag * 5, Nal -> NalRefIdc, LayerMmo);
		Mmo -> LastDecodedPoc = LastDecodedPoc;
	}
}


/**
This functions tries to release reference frame from a layer which is not anymore decoded.
*/
void DetectUnusedReferenceFrame(NAL *Nal, MMO *Mmo)
{
	int i;
	
	//Get the highest layerId
	int LayerIdMax = GetLayerNumber(Nal, Nal -> DqIdMax);

	for (i = LayerIdMax + 1; i < 8; i++){
		LAYER_MMO *LayerMmo = &Mmo -> LayerMMO [i];
		if(LayerMmo -> ShortRefCount){
			remove_all(Mmo, LayerMmo);
		}
	}
}


/**
Detect execute ref pic not done.
*/
void DetectUnDoneExecuteRefPic(NAL *Nal, MMO *Mmo)
{
	int i;
	//Get the highest layerId
	int LayerIdMax = GetLayerNumber(Nal, Nal -> DqIdMax);

	for (i = LayerIdMax + 1; i < 8; i++){
		LAYER_MMO *LayerMmo = &Mmo -> LayerMMO [i];
		if( LayerMmo -> index){
			remove_all(Mmo, LayerMmo);
			LayerMmo -> index = 0;
		}
	}
}


/*
This functions homogeneises the video renderer and removes one bad picture.
*/
void RemoveDisplayError(MMO *Mmo, NAL *Nal)
{
	short Pos = Mmo -> nb_img_display;
	short LayerId = Mmo -> poc_params[Pos] . LayerId;
	short DqId = Nal -> layer_id_table [LayerId];
	short Poc = Mmo -> poc_params[Pos] . Picture -> poc;

	//Look at the last stored picture
	if ( DqId > Nal -> DqIdToDisplay){
		//If we don't want to display it, we just remove it.
		Mmo -> AuMemoryAddress[Mmo -> AuMemory] = Mmo -> poc_params[Pos] . Picture -> MemoryAddress;
		Mmo -> AuMemory++;

		//Reset the Poc params
		memmove(&Mmo -> poc_params[Pos], &Mmo -> poc_params[Pos + 1],  1 * sizeof(POC_PARAMS));
		Mmo -> num_free_poc_address++;
	}else{
		LAYER_MMO *LayerMmo;
		//We have to search the wrong frame.
		//The wrong POC should be the only poc_params with a different num_of_layer
		int i, Found = 0;
		for ( i = 0; i < NUM_POC_PARAMS; i++){
			POC_PARAMS *CurrFrame = &Mmo -> poc_params[i];
			if (CurrFrame -> LayerId != Nal -> LayerId){
				Poc = CurrFrame -> Picture -> poc;
				LayerId = CurrFrame -> LayerId;
				Found = 1;
				break;
			}
		}
		
		// A frame with the wrong layerId has been detected.
		//We remove it
		if (Found){
			LayerMmo = &Mmo -> LayerMMO [Mmo -> poc_params[i] . LayerId];

			//Free memory address
			Mmo -> FreeMemoryAddress [Mmo -> NumFreeAddress] = Mmo -> poc_params[i] . Picture -> MemoryAddress;
			Mmo -> NumFreeAddress++; 

			//Free Mv memory address
			LayerMmo -> MvMemoryAddress [LayerMmo -> FreeMvMemory] = Mmo -> poc_params[i] . Picture -> MvMemoryAddress;
			LayerMmo -> FreeMvMemory++;


			//Reset the Poc params
   			memmove(&Mmo -> poc_params[i], &Mmo -> poc_params[i + 1], (Mmo -> nb_img_display - i) * sizeof(POC_PARAMS));
			Mmo -> num_free_poc_address++;

			Mmo -> poc_params[Mmo -> nb_img_display] . Picture -> poc = MAX_POC;

			LayerMmo = &Mmo -> LayerMMO [LayerId];

			//Search from the short or long term picture the wrong poc
			for (i = 0; i < 16; i++){
				if( LayerMmo -> ShortRef [i] . poc == Poc){
					LayerMmo -> ShortRef [i] . poc = Mmo -> poc_params[Pos] . Picture -> poc;
					break;
				}
				if( LayerMmo -> LongRef [i] . poc == Poc){
					LayerMmo -> ShortRef [i] . poc = Mmo -> poc_params[Pos] . Picture -> poc;
					break;
				}
			}
		}else {
			//No frame of another layer has been detected.
			//No problem is somewhere else.
			//As there is no more free place, we remove one.
			//Find the best picture with the correct poc
			int address_pic;
			POC_PARAMS *Frame = FindBestFrame(Mmo, Mmo -> poc_params, &i, MIN_POC);
			Mmo -> num_free_poc_address ++;

			//Remove from short or long ref the current picture
			CurrentDisplay(Mmo, &Mmo -> LayerMMO[0], Frame -> Picture, &address_pic);
			for (; i < Mmo -> nb_img_display - Mmo -> num_free_poc_address; i++){
				memmove(&Mmo -> poc_params[i], &Mmo -> poc_params[i + 1],  1 * sizeof(POC_PARAMS));
			}
		}
	}
}
