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



#include "type.h"
#include "data.h"
#include "picture_management.h"


/**
This function permits to remove the oldest image in the reference list.

@param Mmo MMO structure which contains all information about memory management of the video.
*/
void sliding_window(MMO *Mmo, LAYER_MMO *LayerMmo){

	int pos_best = LayerMmo -> ShortRefCount - 1;
	//Avoid pb when bits stream error
	if(pos_best >= 0){
		remove_short(Mmo, LayerMmo, LayerMmo -> ShortRef[pos_best] . frame_num);
	}
}


/**
This function permits to remove the oldest image in the reference list.

@param Mmo MMO structure which contains all information about memory management of the video.
*/
void sliding_window_forced(MMO *Mmo, LAYER_MMO *LayerMmo){

	int pos_best = LayerMmo -> ShortRefCount - 1;
	//Avoid pb when bits stream error
	if(pos_best >= 0){
		remove_short(Mmo, LayerMmo, LayerMmo -> ShortRef[pos_best] . frame_num);
		/*if(!Mmo -> NumFreeAddress){
			int i;
			for(i = pos_best - 1; i >=0 && !Mmo -> NumFreeAddress; i--){
				remove_short(Mmo, LayerMmo, LayerMmo -> ShortRef[i] . frame_num);
			}
		}*/
	}
}



/**
This function permits to remove the unused image in the short and long reference list.

@param Mmo MMO structure which contains all information about memory management of the video.
*/
void remove_unused(MMO *Mmo, LAYER_MMO *LayerMmo){
	int i;
	int removed_flag = 0;

	for(i = 0 ; i < LayerMmo -> ShortRefCount ; i++){
		if ( LayerMmo -> ShortRef[i] . reference == 0){
			remove_short(Mmo,  LayerMmo, LayerMmo -> ShortRef[i] . frame_num);
			removed_flag = 1;
			break;
		}
	}
	if (!removed_flag){
		sliding_window(Mmo, LayerMmo);
	}
}





/**
This function permits to remove an image in the short reference list.

@param Mmo MMO structure which contains all information about memory management of the video.
@param frame_num The number of the frame to eliminate.
*/
void remove_short(MMO *Mmo, LAYER_MMO *LayerMmo, int frame_num){

	if ( LayerMmo -> ShortRefCount){
		int i;
		LIST_MMO pic;
		for(i = 0; i < LayerMmo -> ShortRefCount; i++){
			pic = LayerMmo -> ShortRef[i];
			if(pic . frame_num == frame_num){
				if ( pic . displayed == 1){
					Mmo -> FreeMemoryAddress[Mmo -> NumFreeAddress++] = LayerMmo -> ShortRef[i] . MemoryAddress;
				}

				LayerMmo -> MvMemoryAddress[LayerMmo -> FreeMvMemory++] = LayerMmo -> ShortRef[i] . MvMemoryAddress;

				for (; i < LayerMmo -> ShortRefCount; i++){
					LayerMmo -> ShortRef[i] = LayerMmo -> ShortRef[i + 1];
				}
				LayerMmo -> ShortRefCount--;
				break;
			}
		}
	}
}




/**
This function permits to transform an short reference image in a long one.

@param Mmo MMO structure which contains all information about memory management of the video.
@param frame_num The number of the frame to covert.
@param Long_Term_Idx Index to set to the image to convert.
*/
void short_to_long(MMO *Mmo, LAYER_MMO *LayerMmo, int frame_num, int Long_Term_Idx){

	int i;
	LIST_MMO pic;

	if (LayerMmo -> ShortRefCount){
		for(i = 0; i < LayerMmo -> ShortRefCount; i++){
			pic = LayerMmo -> ShortRef[i];
			if(pic . frame_num == frame_num){
				LayerMmo -> LongRef [Long_Term_Idx] = pic;
				remove_short(Mmo, LayerMmo, pic . frame_num);
				
				if(pic.displayed == 1){
					Mmo -> NumFreeAddress--;
					Mmo -> FreeMemoryAddress[Mmo -> NumFreeAddress] = 0;
				}

				LayerMmo -> LongRef[Long_Term_Idx] . MvMemoryAddress = LayerMmo -> MvMemoryAddress[LayerMmo -> FreeMvMemory - 1];
				LayerMmo -> FreeMvMemory--;
				LayerMmo -> MvMemoryAddress[LayerMmo -> FreeMvMemory] = 0;

				LayerMmo -> LongRef[Long_Term_Idx] . pic_id = Long_Term_Idx;
				LayerMmo -> LongRefCount++;
				break;
			}
		}
	}
}





/**
This function permits to remove an image in the long reference list.

@param Mmo MMO structure which contains all information about memory management of the video.
@param long_term_pic_num The picture number of the frame to eliminate.
*/
void remove_long(MMO *Mmo, LAYER_MMO *LayerMmo, int long_term_pic_num){


	if (LayerMmo -> LongRef[long_term_pic_num].reference == 3){
		if (LayerMmo -> LongRef[long_term_pic_num] . displayed == 1){
			Mmo -> FreeMemoryAddress[Mmo -> NumFreeAddress++] = LayerMmo -> LongRef[long_term_pic_num] . MemoryAddress;
		}

		LayerMmo -> MvMemoryAddress[LayerMmo -> FreeMvMemory] = LayerMmo -> LongRef[long_term_pic_num] . MvMemoryAddress;
		LayerMmo -> FreeMvMemory++;
		LayerMmo -> LongRef[long_term_pic_num] . reference = 0;
		LayerMmo -> LongRefCount--;
	}
}




/**
This function permits to remove all image.

@param Mmo MMO structure which contains all information about memory management of the video.
*/
void remove_all(MMO *Mmo, LAYER_MMO *LayerMmo){
	int i;
	int count = LayerMmo -> ShortRefCount;
	
	for(i = 0; i < count; i++){
		remove_short(Mmo, LayerMmo, LayerMmo -> ShortRef[0] . frame_num);
	}

	for (i = 0 ; i < 16;  i++){
		if (LayerMmo -> LongRef[i] . reference == 3){
			if(LayerMmo -> LongRef[i] . displayed == 1){
				Mmo -> FreeMemoryAddress[Mmo -> NumFreeAddress++] = LayerMmo -> LongRef[i] . MemoryAddress;
			}
			LayerMmo -> MvMemoryAddress[LayerMmo -> FreeMvMemory] = LayerMmo -> LongRef[i] . MvMemoryAddress;
			LayerMmo -> FreeMvMemory++;
			LayerMmo -> LongRef[i] . reference = 0;
		}
	}
	LayerMmo -> LongRefCount = 0;
}
