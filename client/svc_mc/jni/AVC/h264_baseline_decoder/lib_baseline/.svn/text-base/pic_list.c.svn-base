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

#include <string.h>
#include <stdio.h>

//AVC Files
#include "type.h"
#include "symbol.h"
#include "pic_list.h"
#include "picture_management.h"
#include "memory_management.h"




/**
This function allows to reorder the reference list when there is a gap in the decoding process.

@param mmo MMO structure which contains all information about memory management of the video.
@param MaxFrameNum Maximal number which can have a frame in the video.
@param num_ref_frames Number of reference frame in the current video.
@param RefPicListL0 Refernce list l0.
@param RefPicListL1 Refernce list l1.
*/
void fill_frame_num_gap(MMO *Mmo, LAYER_MMO *LayerMmo, int MaxFrameNum, int num_ref_frames, 
						LIST_MMO *RefPicListL0, LIST_MMO *RefPicListL1)
{



	LIST_MMO Current_pic[1];
	int UnusedShortTermFrameNum = (LayerMmo -> prev_frame_num + 1) % MaxFrameNum;
	int i = (LayerMmo -> frame_num - LayerMmo -> prev_frame_num - 1 + MaxFrameNum) % MaxFrameNum;		
	memset(Current_pic, 0, sizeof(LIST_MMO));

	for (; i > 0; i--){
		Current_pic -> pic_id = UnusedShortTermFrameNum;
		Current_pic -> frame_num = UnusedShortTermFrameNum;
		Current_pic -> reference = 3;
		if (Mmo -> NumFreeAddress <= 0){
			remove_short(Mmo, LayerMmo, LayerMmo -> ShortRef[ LayerMmo -> ShortRefCount - 1] . frame_num);
		}

		remove_short(Mmo, LayerMmo, LayerMmo -> frame_num);  
		if(LayerMmo -> ShortRefCount){
			memmove(&LayerMmo -> ShortRef[1], &LayerMmo -> ShortRef[0], LayerMmo -> ShortRefCount * sizeof(LIST_MMO));
		}

		ProvideMemAddress(Current_pic, Mmo, LayerMmo);

		LayerMmo -> ShortRef [0] = *Current_pic;
		LayerMmo -> ShortRef [0] . displayed = 1;
		LayerMmo -> ShortRefCount++;
		LayerMmo -> prev_frame_num = UnusedShortTermFrameNum;
		UnusedShortTermFrameNum = (LayerMmo -> prev_frame_num + 1) % MaxFrameNum;
		if( LayerMmo -> ShortRefCount + LayerMmo -> LongRefCount >= num_ref_frames + 1){ 
			sliding_window(Mmo, LayerMmo);               
		}
		fill_default_ref_list( LayerMmo, RefPicListL0, RefPicListL1, Current_pic, 1);
		if( LayerMmo -> RefCountL0 == num_ref_frames + 1){ 
			sliding_window(Mmo, LayerMmo);               
		}
	}
}



/**
Compute POC of each frame.
*/
short PocComputation(SPS *sps, int IdrPicFlag, int nal_ref_idc, LAYER_MMO *LayerMmo)
{

	int PicOrderCntMsb, TopFieldOrderCnt, BottomFieldOrderCnt;

	if(sps -> pic_order_cnt_type == 0){
		const int MaxPicOrderCntLsb = 1 << (sps -> log2_max_pic_order_cnt_lsb);

		if(IdrPicFlag == 5){
			LayerMmo -> prevPicOrderCntMsb = LayerMmo -> prevPicOrderCntLsb = 0;
		}

		if  ((LayerMmo -> pic_order_cnt_lsb < LayerMmo -> prevPicOrderCntLsb) &&
			((LayerMmo -> prevPicOrderCntLsb - LayerMmo -> pic_order_cnt_lsb) >= (MaxPicOrderCntLsb >> 1))){
				PicOrderCntMsb =  LayerMmo -> prevPicOrderCntMsb + MaxPicOrderCntLsb;
				LayerMmo -> prevPicOrderCntMsb = PicOrderCntMsb;
				LayerMmo -> prevPicOrderCntLsb = 0;

		}else if( (LayerMmo -> pic_order_cnt_lsb > LayerMmo -> prevPicOrderCntLsb) && 
			((LayerMmo -> pic_order_cnt_lsb - LayerMmo -> prevPicOrderCntLsb) > (MaxPicOrderCntLsb >> 1))){
				PicOrderCntMsb = LayerMmo -> prevPicOrderCntMsb - MaxPicOrderCntLsb;
				LayerMmo -> prevPicOrderCntMsb = PicOrderCntMsb;
		}else{
			PicOrderCntMsb = LayerMmo -> prevPicOrderCntMsb;
		}

		TopFieldOrderCnt = BottomFieldOrderCnt = PicOrderCntMsb + LayerMmo -> pic_order_cnt_lsb;

		if(sps -> field_pic_flag){
			BottomFieldOrderCnt += sps -> delta_pic_order_cnt_bottom;
		}
	}else if(sps -> pic_order_cnt_type == 1){
		int abs_frame_num, expected_delta_per_poc_cycle, expectedpoc;
		int i;

		if(sps -> num_ref_frames_in_pic_order_cnt_cycle != 0){
			abs_frame_num = LayerMmo -> frame_num_offset + LayerMmo -> frame_num;
		}else{
			abs_frame_num = 0;
		}
		if(nal_ref_idc == 0 && abs_frame_num > 0){
			abs_frame_num--;
		}

		expected_delta_per_poc_cycle = 0;
		for(i = 0; i < sps -> num_ref_frames_in_pic_order_cnt_cycle; i++){
			expected_delta_per_poc_cycle += sps -> offset_for_ref_frame[ i ]; //FIXME integrate during sps parse
		}

		if(abs_frame_num > 0){
			int poc_cycle_cnt = (abs_frame_num - 1) / sps -> num_ref_frames_in_pic_order_cnt_cycle;
			int frame_num_in_poc_cycle = (abs_frame_num - 1) % sps -> num_ref_frames_in_pic_order_cnt_cycle;

			expectedpoc = poc_cycle_cnt * expected_delta_per_poc_cycle;
			for(i = 0; i <= frame_num_in_poc_cycle; i++)
				expectedpoc = expectedpoc + sps -> offset_for_ref_frame[ i ];
		} else{
			expectedpoc = 0;
		}
		if(nal_ref_idc == 0){
			expectedpoc = expectedpoc + sps -> offset_for_non_ref_pic;
		}

		TopFieldOrderCnt = expectedpoc + sps -> delta_pic_order_cnt[0];
		BottomFieldOrderCnt = TopFieldOrderCnt + sps -> offset_for_top_to_bottom_field + sps -> delta_pic_order_cnt[1];

		if(sps -> field_pic_flag){
			BottomFieldOrderCnt += sps -> delta_pic_order_cnt[1];
		}
	}else{
		int tempPicOrderCnt;
		if(IdrPicFlag == 5){
			tempPicOrderCnt = 0;
		}else{
			if(nal_ref_idc) {
				tempPicOrderCnt = 2*(LayerMmo -> frame_num_offset + LayerMmo -> frame_num);
			}else {
				tempPicOrderCnt = 2*(LayerMmo -> frame_num_offset + LayerMmo -> frame_num) - 1;
			}
		}
		TopFieldOrderCnt = tempPicOrderCnt;
		BottomFieldOrderCnt = tempPicOrderCnt;
	}

	//Save the last decoded POC for comparison
	return ((TopFieldOrderCnt < BottomFieldOrderCnt)? TopFieldOrderCnt: BottomFieldOrderCnt);
}



/**
This function is used to determine initial picture orderings for reference picture in the decoding of B slices.


@param sps SPS structure of the current video.
@param nal_unit_type The type of the NAL unit.
@param nal_ref_idc Specifies if the current picture is a reference one.
@param mmo MMO structure which contains all information about memory management of the video.
@param Current_pic LIST_MMO structure with contains information on the current picture.
@param RefPicListL0 Refernce list l0.
@param RefPicListL1 Refernce list l1.
*/

void pic_order_process(SPS *sps, int IdrPicFlag, int nal_ref_idc, MMO *Mmo, LAYER_MMO *LayerMmo, 
					   LIST_MMO *Current_pic, LIST_MMO RefPicListL0[], LIST_MMO RefPicListL1[])
{

	int max_frame_num = 1 << (sps -> log2_max_frame_num);

	if(IdrPicFlag == 5){
		LayerMmo -> frame_num_offset = 0;
		LayerMmo -> prev_frame_num = 0;
	}else{
		if(LayerMmo -> frame_num < LayerMmo -> prev_frame_num)
			LayerMmo -> frame_num_offset = LayerMmo -> prev_frame_num_offset + max_frame_num;
		else
			LayerMmo -> frame_num_offset = LayerMmo -> prev_frame_num_offset;
	}


	//Fill gap only on the first representation
	if (sps -> GapInFrameNumAllowFlag && (!Mmo -> LayerId) && (LayerMmo -> frame_num != LayerMmo -> prev_frame_num) && (LayerMmo -> frame_num != (LayerMmo -> prev_frame_num + 1) % max_frame_num)){
		fill_frame_num_gap(Mmo, LayerMmo, max_frame_num, sps -> num_ref_frames, RefPicListL0, RefPicListL1);
	}


	if (LayerMmo -> ShortRefCount + LayerMmo -> LongRefCount == sps -> num_ref_frames + 1){
		remove_unused(Mmo, LayerMmo);
	}

#ifdef ERROR_DETECTION
	//Avoid crashing when bitstream errors.
	//Error detection
	if(LayerMmo -> FreeMvMemory <= 0 || Mmo -> NumFreeAddress <= 0){
		//printf("Missing free mv place\n");
		remove_unused(Mmo, LayerMmo);
	}
#endif
	Mmo -> LastDecodedPoc = Current_pic -> poc;
	Current_pic -> poc = PocComputation(sps, IdrPicFlag, nal_ref_idc, LayerMmo);
}





/**
This function is used to initialize two tables for direct prediction for B slices.

@param Slice The slice structure contains all information used to decode the next slice
@param RefPicListL0 Refernce list l0.
@param RefPicListL1 Refernce list l1.
@param Current_pic LIST_MMO structure with contains information on the current picture.
@param mmo MMO structure which contains all information about memory management of the video.
@param map_col_to_list0 Table used to determine the poc of the colocated macroblock.
@param map_col_to_list1 Table used to determine the poc of the colocated macroblock.
*/
void direct_ref_list_init( SLICE *Slice, LIST_MMO RefPicListL0[], LIST_MMO RefPicListL1[], 
						  LIST_MMO *Current_pic, LAYER_MMO *LayerMmo)
{


	int i, j;

	if(Slice -> slice_type == SLICE_TYPE_I){
		Current_pic -> ref_count_l0 = 0;
	}

	if(Slice -> slice_type != SLICE_TYPE_B){
		Current_pic -> ref_count_l1 = 0;
	}

	for(j = 0; j < LayerMmo -> RefCountL0; j++){
		Current_pic -> ref_poc_l0[j] = RefPicListL0[j] . poc;
	}

	Current_pic -> ref_count_l0 = LayerMmo -> RefCountL0;


	for(j = 0; j < LayerMmo -> RefCountL1; j++){
		Current_pic -> ref_poc_l1[j] = RefPicListL1[j] . poc;
	}

	Current_pic -> ref_count_l1 = LayerMmo -> RefCountL1;


	if(Slice -> slice_type != SLICE_TYPE_B || Slice -> direct_spatial_mv_pred_flag)
		return;


	for(i = 0; i < RefPicListL1 -> ref_count_l0; i++){
		const int poc = RefPicListL1[0].ref_poc_l0[i];
		Slice -> MapColList[i] = PART_NOT_AVAILABLE;
		for(j = 0; j < LayerMmo -> RefCountL0; j++){
			if (RefPicListL0[j] . poc == poc){
				Slice -> MapColList [i] = j;
				break;
			}
		}
	}



	for(i = 0; i < RefPicListL1 -> ref_count_l1; i++){
		const int poc = RefPicListL1[0].ref_poc_l1[i];
		Slice -> MapColList[16 + i] = PART_NOT_AVAILABLE;
		for(j = 0; j < LayerMmo -> RefCountL1; j++){
			if(RefPicListL1[j] . poc == poc){
				Slice -> MapColList [16 + i] = j;
				break;
			}
		}
	}	
}





/**
This fuction allows to execute the memory management for a right decoding of the current video.

@param mmco MMCO structure which contains the operatio to do on the short and long refernce list.
@param Curr LIST_MMO structure with contains information on the current picture.
@param mmo MMO structure which contains all information about memory management of the video.
*/
void execute_ref_pic_marking(LIST_MMO *Curr, MMO *Mmo, int LayerId)
{

	int i, j;
	int current_is_long = 0;
	LAYER_MMO *LayerMmo = &Mmo -> LayerMMO [LayerId];
	MMCO *Mmco = LayerMmo -> Mmco;
	
	Mmo -> PrevPicOrderCntLsb = LayerMmo -> prevPicOrderCntLsb;
	Mmo -> PrevPicOrderCntMsb = LayerMmo -> prevPicOrderCntMsb;
	 
	LayerMmo -> prev_frame_num_offset = LayerMmo -> frame_num_offset;
	LayerMmo -> prev_frame_num = LayerMmo -> frame_num;
	LayerMmo -> prevPicOrderCntLsb = LayerMmo -> pic_order_cnt_lsb;

	for(i = 0; i < LayerMmo -> index; i++){
		switch(Mmco[i] . opcode){
		case 1:
			remove_short(Mmo, LayerMmo, Mmco[i] . short_frame_num);
			break;

		case 2:
			remove_long(Mmo, LayerMmo, Mmco[i] . long_term_frame_idx);
			break;

		case 3:
			remove_long(Mmo, LayerMmo, Mmco[i] . long_term_frame_idx);
			short_to_long(Mmo, LayerMmo, Mmco[i] . short_frame_num, Mmco[i] . long_term_frame_idx);
			break;

		case 4:
			// just remove the long term which index is greater than new max
			for(j = Mmco[i] . long_term_frame_idx; j < 16; j++){
				remove_long(Mmo, LayerMmo, j);
			}
			break;

		case 5:
			remove_all(Mmo, LayerMmo);
			Curr -> frame_num = 0;
			Curr -> poc = 0;
			LayerMmo -> prev_frame_num = 0;
			break;

		case 6:
			remove_long(Mmo, LayerMmo, Mmco[i] . long_term_frame_idx);
			Curr -> long_term = 1;
			LayerMmo -> LongRef[Mmco[i] . long_term_frame_idx]= *Curr;
			LayerMmo -> LongRef[Mmco[i] . long_term_frame_idx] . pic_id = Mmco[i] . long_term_frame_idx;
			LayerMmo -> LongRefCount++;
			current_is_long = 1;
			break;

		case 7:
			remove_unused(Mmo, LayerMmo);
		}
	}

	LayerMmo -> index = 0;
	if(!current_is_long){
		remove_short(Mmo, LayerMmo, LayerMmo -> frame_num);
		if(LayerMmo -> ShortRefCount){
			memmove(&LayerMmo -> ShortRef[1], &LayerMmo -> ShortRef[0], LayerMmo -> ShortRefCount * sizeof(LIST_MMO));
		}

		LayerMmo -> ShortRef[0] = *Curr;
		LayerMmo -> ShortRefCount++;
	}
}



/**
This function is used to generate the to reference lists.


@param refPicLXl0 Refernce list l0.
@param refPicLXl1 Refernce list l1.
@param Current_pic LIST_MMO structure with contains information on the current picture.
@param slice_type Slice type of the current slice.
*/
void fill_default_ref_list(LAYER_MMO *LayerMmo, LIST_MMO *refPicLXl0, LIST_MMO *refPicLXl1, 
						   LIST_MMO *Current_pic, int slice_type)
{

	int i, ref_count;
	int smallest_poc_greater_than_current = -1;
	LIST_MMO sorted_short_ref[32];
	LIST_MMO *ref_list;
	unsigned char *ref_count_lX;

	if(slice_type == SLICE_TYPE_B){
		int out_i;
		int limit = MIN_POC;///INT_MIN;

		// sort frame according to poc in B Slice 
		for(out_i = 0; out_i < LayerMmo -> ShortRefCount; out_i++){
			int best_i = 0;//INT_MIN;
			int best_poc = MAX_POC;//= ;//INT_MAX;

			for(i = 0; i < LayerMmo -> ShortRefCount; i++){
				const int poc =  LayerMmo -> ShortRef[i].poc;
				if(poc > limit && poc < best_poc){
					best_poc = poc;
					best_i = i;
				}
			}


			limit = best_poc;
			sorted_short_ref[out_i] = LayerMmo -> ShortRef[best_i];
			if (-1 == smallest_poc_greater_than_current) {
				if (LayerMmo -> ShortRef[best_i] . poc >= Current_pic -> poc) {
					smallest_poc_greater_than_current = out_i;
				}
			}
		}
	}

	if(slice_type == SLICE_TYPE_B){
		int list;

		// find the largest poc
		for(list = 0; list < 2; list++){
			int index = 0;
			int j = -99;
			int step = list ? -1 : 1;

			if (list){
				ref_count =  LayerMmo -> RefCountL1 ;
				ref_count_lX = &LayerMmo -> RefCountL1;
				ref_list = &refPicLXl1[0];
			}else {
				ref_count = LayerMmo -> RefCountL0;
				ref_count_lX = &LayerMmo -> RefCountL0;
				ref_list = &refPicLXl0[0];
			} 

			for(i = 0; i < LayerMmo -> ShortRefCount && index < ref_count; i++, j+=step) {
				while(j < 0 || j >= LayerMmo -> ShortRefCount){
					if(j != -99 && step == (list ? -1 : 1)){
						for (i = LayerMmo -> ShortRefCount - 1; i >= 0; i--){
							if(sorted_short_ref[i] . reference == 3){
								ref_list[index  ] = sorted_short_ref[i];
								ref_list[index++] . pic_id = sorted_short_ref[i] . frame_num;
							}
						}
						break;
					}
					step = -step;
					j = smallest_poc_greater_than_current + (step >> 1);
				}

				if(sorted_short_ref[j] . reference != 3) 
					continue;

				ref_list[index  ] = sorted_short_ref[j];
				ref_list[index++] . pic_id = sorted_short_ref[j] . frame_num;
			}

			for(i = 0; i < 16 && index < ref_count; i++){
				if(LayerMmo -> LongRef[i] . reference != 3) 
					continue;

				ref_list[index  ] = LayerMmo -> LongRef[i];
				ref_list[index++] . pic_id = i;;
			}


			if(list && (smallest_poc_greater_than_current <= 0 || smallest_poc_greater_than_current >= LayerMmo -> ShortRefCount) && (1 < index)){
				// swap the two first elements of L1 when
				// L0 and L1 are identical
				LIST_MMO temp = refPicLXl1[0];
				refPicLXl1[0] = refPicLXl1[1];
				refPicLXl1[1] = temp;
			}
			*ref_count_lX = index;
		}
	}else{
		int index = 0;

		for(i = 0; i < LayerMmo -> ShortRefCount && index < LayerMmo -> RefCountL0; i++){
			if(LayerMmo -> ShortRef[i] . reference != 3){
				continue; //FIXME refernce field shit
			}
			refPicLXl0[index] = LayerMmo -> ShortRef[i];
			refPicLXl0[index++] . pic_id = LayerMmo -> ShortRef[i] . frame_num;
		}

		for(i = 0; i < 16 && index < LayerMmo -> RefCountL0; i++){
			if(LayerMmo -> LongRef[i] . reference != 3){
				continue;
			}
			refPicLXl0 [index] = LayerMmo -> LongRef[i];
			refPicLXl0 [index++] . pic_id = i;
		}
		LayerMmo -> RefCountL0 = index;
		if(index < LayerMmo -> RefCountL0){
			memset(&refPicLXl0[index], 0, sizeof(LIST_MMO) * (LayerMmo -> RefCountL0 - index));
		}
	}
}
