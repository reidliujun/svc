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



#include <stdlib.h>

#ifndef POCKET_PC
#include <string.h>
#include <stdio.h>
#ifdef WIN32
#include <windows.h>
#endif
#endif



//AVC Files
#include "clip.h"
#include "symbol.h"
#include "memory_management.h"
#include "picture_management.h"





/**
This function allows to copy n element from a buffer to an other one with optimizations.

@param ao_pucDest Destination of the copy.
@param ai_pucOrig Source of the copy.
@param ai_iSize Number of element to copy.
*/
void optim_memcpy(	unsigned char* RESTRICT ao_pucDest, const unsigned char* RESTRICT ai_pucOrig, const int ai_iSize)
{
	int i;
	int cnt8 = ai_iSize&(~7);

	for(i = 0; i < cnt8; i++) {
		*(ao_pucDest++) = *(ai_pucOrig++);
	}

	for(i = 0; i < (ai_iSize&7); i++)	{
		*(ao_pucDest++) = *(ai_pucOrig++);
	}
}




/**
This function allows to copy n element from a buffer to an other one with optimizations and with 64 bits aligned.

@param ao_pucDest Destination of the copy.
@param ai_pucOrig Source of the copy.
@param ai_iSize Number of element to copy.
*/
void optim_memcpy_aligned64(unsigned char* RESTRICT ao_pucDest, const unsigned char* RESTRICT ai_pucOrig, const int ai_iSize)
{
	int i;
	int cnt16 = (ai_iSize)&(~15);

#ifdef TI_OPTIM
	_nassert(((unsigned int)ao_pucDest)%8==0);
	_nassert(((unsigned int)ai_pucOrig)%8==0);
	_nassert((cnt16)%16==0);
#pragma MUST_ITERATE(  ,  , 16 );
#endif

	for(i = 0; i < cnt16; i++)
	{
		*(ao_pucDest++) = *(ai_pucOrig++);
	}


	for(i = 0; i < (ai_iSize&15); i++)
	{
		*(ao_pucDest++) = *(ai_pucOrig++);
	}
}





/**
This function allows to initialize the decoded picture buffer address according to the size of the picture.

@param interval Size in pixel of a padded picture.
@param Dpb_depth Depth of the decoded picture buffer.
@param Dpb_display Depth of the display buffer.
@param nb_of_layers Number of layer choosen during the code generation.
@param num_layer Number of layer choosen to be displayed.
@param Mmo MMO structure which contains all information about memory management of the video.
*/

void set_decoding_dpb(int interval, int Dpb_depth, int Dpb_display, int nb_of_layers, MMO *Mmo){

	int i;
	int NumOfPic = MIN(Dpb_depth + Dpb_display + nb_of_layers - 1, 32);


	for (i = 0; i < NumOfPic; i++){
		Mmo -> FreeMemoryAddress[i] = i * interval;
	}
}



/**
This function allows to initialize all the decoded picture buffer address according to the size of the picture.

@param num_layer Number of layer choosen to be displayed.
@param interval Size in pixel of a padded picture.
@param mv_interval Size in pixel of all the motion vector of one picture.
@param Dpb_depth Depth of the decoded picture buffer.
@param nb_of_layers Number of layer choosen during the code generation.
@param Dpb_display Depth of the display buffer.
@param Mmo MMO structure which contains all information about memory management of the video.
*/
void initialize_address(int total_memory, const int mv_memory, int nb_of_layers, int interval, int mv_interval, SPS *sps_id, MMO *Mmo){


	int i, j;
	int max_mv_ref = sps_id -> num_ref_frames < 16 ? sps_id -> num_ref_frames + 1: 16;


	for (i = 0; i < NUM_POC_PARAMS; i++){
		Mmo -> poc_params[i] . Picture -> MemoryAddress = -1;
		Mmo -> poc_params[i] . Picture -> poc = MAX_POC;
	}

	for (i = 0; i < 32; i++){
		Mmo -> FreeMemoryAddress [i] = -1;
		for (j = 0; j < 8; j++)  {
			LAYER_MMO *LayerMmo = &Mmo -> LayerMMO [j];
			LayerMmo -> MvMemoryAddress [i] = -1;
		}
	}

	Mmo -> num_free_poc_address = sps_id -> nb_img_disp;
	Mmo -> NumFreeAddress = MIN(sps_id -> num_ref_frames + sps_id -> nb_img_disp + nb_of_layers - 1, 32);


	for (j = 0; j < nb_of_layers; j++)  {
		LAYER_MMO *LayerMmo = &Mmo -> LayerMMO [j];

		for (i = 0; i < max_mv_ref; i++){
			LayerMmo -> MvMemoryAddress [i] = i * mv_interval + (j * max_mv_ref  * mv_interval);
		}

		LayerMmo -> FreeMvMemory = max_mv_ref;
		LayerMmo -> num_decoded_frame = 0;
	}


	if((sps_id -> num_ref_frames + sps_id -> nb_img_disp + nb_of_layers - 1) * interval <= total_memory && max_mv_ref * nb_of_layers * mv_interval <= mv_memory){
		set_decoding_dpb(interval, sps_id -> num_ref_frames, sps_id -> nb_img_disp, nb_of_layers, Mmo);
	}else{

#ifdef WIN32
#ifndef POCKET_PC
		MessageBoxA(NULL,(LPCSTR)"Not enough memory, the application will be closed", (LPCSTR)"Open SVC Decoder has encountered an error", MB_OK); 
#else 
		MessageBox(NULL,(LPCWSTR)"Not enough memory, the application will be closed", (LPCWSTR)"Open SVC Decoder has encountered an error", MB_OK); 
#endif
#endif
		exit(14);
	}
}





void avc_calculate_dpb(const int total_memory, const int mv_memory, MMO *mmo_struct, SPS *sps){


	int i;
	int changed = 0;
	int sps_id = 0;
	int max_width  = mmo_struct -> MaxWidth;
	int max_height = mmo_struct -> MaxHeight;


	for ( i = 0; i < 32; i ++){
		if ( max_width < (sps[i] . pic_width_in_mbs << 4)){
			max_width = mmo_struct -> MaxWidth = (sps[i] . pic_width_in_mbs << 4);
			sps[i] . b_stride = (max_width) >> 1;
			sps[i] . b8_stride = (max_width) >> 3;
			changed = 1;
			sps_id = i;
		}

		if ( max_height < (sps[i] . pic_height_in_map_units << 4)){
			max_height = mmo_struct -> MaxHeight = (sps[i] . pic_height_in_map_units << 4);
		}
	}

	if ( changed){
		initialize_address(total_memory, mv_memory, 1, (max_width + 32) * (max_height + 32), max_width * max_height >> 3, &sps[sps_id], mmo_struct);
		mmo_struct -> MemoryAllocation = 1;
	}
}





/**
This function allows to initialize all the decoded picture buffer address according to the size of the picture.

@param Nb_Layer Number of layer choosen during the code generation.
@param Mmo MMO structure which contains all information about memory management of the video.
@param ai_pstRefPicListL0 Refernce list l0.
@param ai_pstRefPicListL1 Refernce list l1.
*/
void idr_flush_dpb(int Nb_Layer, MMO *Mmo, LIST_MMO *RefPicListL0, LIST_MMO *RefPicListL1){

	int i,j;


	for(i = 0; i < 16; i++){
		RefPicListL0[i] . frame_num = -1;
		RefPicListL0[i] . poc = MIN_POC;
		RefPicListL0[i] . ref_count_l0 = -1;
		RefPicListL0[i] . reference = -1;
		RefPicListL1[i] . frame_num = -1;
		RefPicListL1[i] . poc = MIN_POC;
		RefPicListL1[i] . ref_count_l0 = -1;
		RefPicListL1[i] . reference = -1;
	}

	for(j = 0; j < Nb_Layer; j++){
		LAYER_MMO *LayerMmo = &Mmo -> LayerMMO [j];
		remove_all(Mmo, LayerMmo);
		LayerMmo -> RefCountL0 = 0;
		LayerMmo -> prev_frame_num = 0;
		LayerMmo -> prev_frame_num_offset = 0;
	}
}

/**
This function allows to initialize all the decoded picture buffer address according to the size of the picture.

@param Nb_Layer Number of layer choosen during the code generation.
@param Mmo MMO structure which contains all information about memory management of the video.
@param ai_pstRefPicListL0 Refernce list l0.
@param ai_pstRefPicListL1 Refernce list l1.
*/
void IdrFlushDpb(int Nb_Layer, MMO *Mmo, LIST_MMO *RefPicListL0, LIST_MMO *RefPicListL1){

	int i;

	LAYER_MMO *LayerMmo = &Mmo -> LayerMMO [Nb_Layer];


	for(i = 0; i < 16; i++){
		RefPicListL0[i] . frame_num = -1;
		RefPicListL0[i] . poc = MIN_POC;
		RefPicListL0[i] . ref_count_l0 = -1;
		RefPicListL0[i] . reference = -1;
		RefPicListL1[i] . frame_num = -1;
		RefPicListL1[i] . poc = MIN_POC;
		RefPicListL1[i] . ref_count_l0 = -1;
		RefPicListL1[i] . reference = -1;
	}

	remove_all(Mmo, LayerMmo);
	LayerMmo -> RefCountL0 = 0;
	LayerMmo -> prev_frame_num = 0;
	LayerMmo -> prev_frame_num_offset = 0;
}



/**
This function provides memory addresses for a new picture.
*/
void ProvideMemAddress(LIST_MMO *Current_pic, MMO *Mmo, LAYER_MMO *LayerMmo)
{
	Current_pic -> MvMemoryAddress = LayerMmo -> MvMemoryAddress [LayerMmo -> FreeMvMemory - 1];
	Current_pic -> MemoryAddress = Mmo -> FreeMemoryAddress [Mmo -> NumFreeAddress - 1];
	Mmo -> FreeMemoryAddress  [Mmo -> NumFreeAddress - 1] = -1;
	LayerMmo -> MvMemoryAddress [LayerMmo -> FreeMvMemory - 1] = -1;
	Current_pic -> displayed = 0;
	Mmo -> NumFreeAddress--;
	LayerMmo -> FreeMvMemory--;
}
