/*****************************************************************************
	 *
	 *	Open SVC Decoder developped in IETR image lab
	 *
	 *
	 *
	 *			Médéric BLESTEL <mblestel@insa-rennes.Fr>
	 *			Mickael RAULET <mraulet@insa-rennes.Fr>
	 *			http://www.ietr.org/
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
	 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the GNU
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
#include <stdio.h>
#include <string.h>

#include "type.h"
#include "clip.h"
#include "bitstream.h"

void init_sei_vectors(SEI *decoder_svc_sei) {

	memset(decoder_svc_sei, 0, sizeof(SEI));
}

/**
This function permits to display the SEI message. 

@param data The NAL unit.
@param pos_bits The current aio_piPosition in the NAL unit.
@param payloadSize The size of the SEI payload.
*/

int user_data_unregistered (unsigned char *data, int payloadSize, int *aio_piPosition)
{
	int	i;
	//unsigned char	user_data [500];

		
	if (payloadSize < 16) 
		 return -1;

	for (i = 0; i < 500 - 1 && i <= payloadSize; i++) {
	 /*	user_data [i] = */getNbits(data, aio_piPosition, 8);
	}
	//user_data [i] = 0;
	
	for (; i < payloadSize; i++) {
		 *aio_piPosition+=8;
	}

	// printf("\n");
	return 0;
}

void buffering_period(unsigned char *data, SPS sps[], int *pos_bits) {

	int SchedSelIdx;
	/*short initial_cpb_removal_delay[32];
	short initial_cpb_removal_delay_offset[32];*/
	int seq_parameter_set_id = read_ue(data, pos_bits);

	unsigned char SeqParameterSetId = CLIP3(0, SPS_BUF_SIZE - 1, seq_parameter_set_id);

	SPS *pt_sps = &sps[SeqParameterSetId];

	if(pt_sps -> nal_hrd_parameters_present_flag) {
		for(SchedSelIdx = 0; SchedSelIdx <= sps -> cpb_cnt; SchedSelIdx++) {
		/*	initial_cpb_removal_delay[ SchedSelIdx ] =*/ getNbits(data, pos_bits, sps->initial_cpb_removal_delay_length);
		/*	initial_cpb_removal_delay_offset[ SchedSelIdx ] =*/ getNbits(data, pos_bits, sps->initial_cpb_removal_delay_length);
		}
	}
	if(pt_sps->vlc_hrd_parameters_present_flag) {
		for(SchedSelIdx = 0; SchedSelIdx <= sps -> cpb_cnt; SchedSelIdx++) {
		/*	initial_cpb_removal_delay[ SchedSelIdx ] =*/ getNbits(data, pos_bits, sps->cpb_removal_delay_length);
		/*	initial_cpb_removal_delay_offset[ SchedSelIdx ] =*/ getNbits(data, pos_bits, sps->cpb_removal_delay_length);
		}
	}
}

void pic_timing(unsigned char *data, SPS *sps, int *pos_bits) {	
	
	short i;
	short NumClockTS = 0; 
	short clock_timestamp_flag[3];
	int full_timestamp_flag;
	int hours_flag;
	int minutes_flag;
	int seconds_flag;

	if(sps->nal_hrd_parameters_present_flag || sps->vlc_hrd_parameters_present_flag) {		
	/*	short cpb_removal_delay =*/ getNbits(data, pos_bits, sps->cpb_removal_delay_length); 
	/*	short dpb_output_delay =*/ getNbits(data, pos_bits, sps->cpb_removal_delay_length);
	}		
	if(sps->pic_present_flag) {		
		int pic_struct = getNbits(data, pos_bits, 4); 

		if (pic_struct < 3)
			NumClockTS = 1;
		else if (pic_struct == 3 || pic_struct == 4 || pic_struct == 7)
			NumClockTS = 2;
		else if (pic_struct == 5 || pic_struct == 6 || pic_struct == 8)
			NumClockTS = 3;

		for(i = 0; i < NumClockTS; i++) {
		//	short time_offset = 0;
		/*	clock_timestamp_flag[ i ] =*/ getNbits(data, pos_bits, 1); 
			if(clock_timestamp_flag[i]) {		
			/*	short ct_type=*/ getNbits(data, pos_bits, 2); 
			/*	short nuit_field_based_flag =*/ getNbits(data, pos_bits, 1);	 
			/*	short counting_type =*/ getNbits(data, pos_bits, 5);	 
				full_timestamp_flag = getNbits(data, pos_bits, 1);	 
			/*	short discontinuity_flag = */getNbits(data, pos_bits, 1);	 
			/*	short cnt_dropped_flag =*/ getNbits(data, pos_bits, 1);	 
			/*	short n_frames =*/ getNbits(data, pos_bits, 8);	 
				if(full_timestamp_flag) {		
				/*	short seconds_value =*/ getNbits(data, pos_bits, 6);	/* 0..59 */	
				/*	short minutes_value =*/ getNbits(data, pos_bits, 6);	/* 0..59 */	
				/*	short hours_value = */getNbits(data, pos_bits, 5);	/* 0..23 */
				} else {
					seconds_flag = getNbits(data, pos_bits, 1);
					if(seconds_flag) {		
					/*	short seconds_value = */getNbits(data, pos_bits, 6);	 /* range 0..59 */
					minutes_flag = getNbits(data, pos_bits, 1);
					if(minutes_flag) {
						/*	short minutes_value = */getNbits(data, pos_bits, 6);	/* 0..59 */
						hours_flag = getNbits(data, pos_bits, 1);
						if(hours_flag)
							/*	hours_value =*/ getNbits(data, pos_bits, 5);	/* 0..23 */
						}
					}
				}		
				if(sps->time_offset_length > 0)		
				/*	time_offset	=*/ getNbits(data, pos_bits, sps->time_offset_length); 
			}		
		}		
	}		
}		

/*

*/
void scalability_info(unsigned char *data, SPS *sps, int *pos_bits, SEI *decoder_svc_sei) {	
#ifdef ENABLE_PACKET_LOSS_DETECTION
	short i, j;
	int priority_id_setting_uri_idx;

	unsigned char temporal_id_nesting_flag = getNbits(data, pos_bits, 1);
	unsigned char priority_layer_info_present_flag = getNbits(data, pos_bits, 1);
	unsigned char priority_id_setting_flag = getNbits(data, pos_bits, 1);
	int num_layers_minus1 = read_ue(data, pos_bits);

	for(i=0; i<=num_layers_minus1; i++) {

		decoder_svc_sei -> layer_id[i] = read_ue(data, pos_bits);
		decoder_svc_sei -> priority_id[i] = getNbits(data, pos_bits, 6);
		decoder_svc_sei -> discardable_flag[i] = getNbits(data, pos_bits, 1);
		decoder_svc_sei -> dependency_id[i] = getNbits(data, pos_bits, 3);
		decoder_svc_sei -> quality_id[i] = getNbits(data, pos_bits, 4);
		decoder_svc_sei -> temporal_id[i] = getNbits(data, pos_bits, 3);
		decoder_svc_sei -> sub_pic_layer_flag[i] = getNbits(data, pos_bits, 1);
		decoder_svc_sei -> sub_region_layer_flag[i] = getNbits(data, pos_bits, 1);
		decoder_svc_sei -> iroi_division_info_present_flag[i] = getNbits(data, pos_bits, 1);

		decoder_svc_sei -> profile_level_info_present_flag[i] = getNbits(data, pos_bits, 1);
		decoder_svc_sei -> bitrate_info_present_flag[i] = getNbits(data, pos_bits, 1);
		decoder_svc_sei -> frm_rate_info_present_flag[i] = getNbits(data, pos_bits, 1);
		decoder_svc_sei -> frm_size_info_present_flag[i] = getNbits(data, pos_bits, 1);
		decoder_svc_sei -> layer_dependency_info_present_flag[i] = getNbits(data, pos_bits, 1);
		decoder_svc_sei -> parameter_sets_info_present_flag[i] = getNbits(data, pos_bits, 1);
		decoder_svc_sei -> bitstream_restriction_info_present_flag[i] = getNbits(data, pos_bits, 1);
		decoder_svc_sei -> exact_inter_layer_pred_flag[i] = getNbits(data, pos_bits, 1);

		if(decoder_svc_sei -> sub_pic_layer_flag[i] || decoder_svc_sei -> iroi_division_info_present_flag[i]) {
			decoder_svc_sei -> exact_sample_value_match_flag[i] = getNbits(data, pos_bits, 1);
		}
		
		decoder_svc_sei -> layer_conversion_flag[i] = getNbits(data, pos_bits, 1);
		decoder_svc_sei -> layer_output_flag[i] = getNbits(data, pos_bits, 1);

		if(decoder_svc_sei -> profile_level_info_present_flag[i]) {
			decoder_svc_sei -> layer_profile_level_idc[i] = getNbits(data, pos_bits, 24);
		}

		if(decoder_svc_sei -> bitrate_info_present_flag[i]) {
			decoder_svc_sei -> avg_bitrate[i] = getNbits(data, pos_bits, 16);
			decoder_svc_sei -> max_bitrate_layer[i] = getNbits(data, pos_bits, 16);
			decoder_svc_sei -> max_bitrate_layer_representation[i] = getNbits(data, pos_bits, 16);
			decoder_svc_sei -> max_bitrate_calc_window[i] = getNbits(data, pos_bits, 16);
		}

		if(decoder_svc_sei -> frm_rate_info_present_flag[i]) {
			decoder_svc_sei -> constant_frm_rate_idc[i] = getNbits(data, pos_bits, 2);
			decoder_svc_sei -> avg_frm_rate[i] = getNbits(data, pos_bits, 16);
		}

		if(decoder_svc_sei -> frm_rate_info_present_flag[i] || decoder_svc_sei -> iroi_division_info_present_flag[i]) {
			decoder_svc_sei -> frm_width_in_mbs_minus1[i] = read_ue(data, pos_bits);
			decoder_svc_sei -> frm_height_in_mbs_minus1[i] = read_ue(data, pos_bits);
		}

		if(decoder_svc_sei -> sub_region_layer_flag[i]) {
			decoder_svc_sei -> base_region_layer_id[i] = read_ue(data, pos_bits);
			decoder_svc_sei -> dynamic_rect_flag[i] = getNbits(data, pos_bits, 1);
			if(!decoder_svc_sei -> dynamic_rect_flag[i]) {
				decoder_svc_sei -> horizontal_offset[i] = getNbits(data, pos_bits, 16);
				decoder_svc_sei -> vertical_offset[i] = getNbits(data, pos_bits, 16);
				decoder_svc_sei -> region_width[i] = getNbits(data, pos_bits, 16);
				decoder_svc_sei -> region_height[i] = getNbits(data, pos_bits, 16);
			}
		}

		if(decoder_svc_sei -> sub_pic_layer_flag[i]) {
			decoder_svc_sei -> roi_id[i] = read_ue(data, pos_bits);
		}
 
		if(decoder_svc_sei -> iroi_division_info_present_flag[i]) {
			decoder_svc_sei -> iroi_grid_flag[i] = getNbits(data, pos_bits, 1);
			if(decoder_svc_sei -> iroi_grid_flag[i]) {
				decoder_svc_sei -> grid_width_in_mbs_minus1[i] = read_ue(data, pos_bits);
				decoder_svc_sei -> grid_height_in_mbs_minus1[i] = read_ue(data, pos_bits);
			} else {
				decoder_svc_sei -> num_rois_minus1[i] = read_ue(data, pos_bits);
				for(j=0; j<=decoder_svc_sei -> num_rois_minus1[i]; j++) {
					decoder_svc_sei -> first_mb_in_roi[i][j] = read_ue(data, pos_bits);
					decoder_svc_sei -> roi_width_in_mbs_minus1[i][j] = read_ue(data, pos_bits);
					decoder_svc_sei -> roi_height_in_mbs_minus1[i][j] = read_ue(data, pos_bits);
				}
			}
		}

		if(decoder_svc_sei -> layer_dependency_info_present_flag[i]) {
			decoder_svc_sei -> num_directly_dependent_layers[i] = read_ue(data, pos_bits);
			 
			for(j=0; j<decoder_svc_sei -> num_directly_dependent_layers[i]; j++) {
				decoder_svc_sei -> directly_dependent_layer_id_delta_minus1[i][j] = read_ue(data, pos_bits);
			}
		} else {
			decoder_svc_sei -> layer_dependency_info_src_layer_id_delta[i] = read_ue(data, pos_bits);
		}
		
		if(decoder_svc_sei -> parameter_sets_info_present_flag[i]) {
			decoder_svc_sei -> num_seq_parameter_set_minus1[i] = read_ue(data, pos_bits);
			for(j=0; j<=decoder_svc_sei -> num_seq_parameter_set_minus1[i]; j++) {
				decoder_svc_sei -> seq_parameter_set_id_delta[i][j] = read_ue(data, pos_bits);
			}
			decoder_svc_sei -> num_subset_seq_parameter_set_minus1[i] = read_ue(data, pos_bits);
			for(j=0; j<=decoder_svc_sei -> num_subset_seq_parameter_set_minus1[i]; j++) {
				decoder_svc_sei -> subset_seq_parameter_set_id_delta[i][j] = read_ue(data, pos_bits);
			}
			decoder_svc_sei -> num_pic_parameter_set_minus1[i] = read_ue(data, pos_bits);
			for(j=0; j<=decoder_svc_sei -> num_pic_parameter_set_minus1[i]; j++) {
				decoder_svc_sei -> pic_parameter_set_id_delta[i][j] = read_ue(data, pos_bits);
			}
		} else {
			decoder_svc_sei -> parameter_sets_info_src_layer_id_delta[i] = read_ue(data, pos_bits);
		}

		if(decoder_svc_sei -> bitstream_restriction_info_present_flag[i]) {
			decoder_svc_sei -> motion_vectors_over_pic_boundaries_flag[i] = getNbits(data, pos_bits, 1);
			decoder_svc_sei -> max_bytes_per_pic_denom[i] = read_ue(data, pos_bits);
			decoder_svc_sei -> max_bits_per_mb_denom[i] = read_ue(data, pos_bits);
			decoder_svc_sei -> log2_max_mv_length_horizontal[i] = read_ue(data, pos_bits);
			decoder_svc_sei -> log2_max_mv_length_vertical[i] = read_ue(data, pos_bits);
			decoder_svc_sei -> num_reorder_frames[i] = read_ue(data, pos_bits);
			decoder_svc_sei -> max_dec_frame_buffering[i] = read_ue(data, pos_bits);
		}
		
		if(decoder_svc_sei -> layer_conversion_flag[i]) {
			decoder_svc_sei -> conversion_type_idc[i] = read_ue(data, pos_bits);
			for(j=0; j<2; j++) {
				decoder_svc_sei -> rewriting_info_flag[i][j] = getNbits(data, pos_bits, 1);
				if(decoder_svc_sei -> rewriting_info_flag[i][j]) {
					decoder_svc_sei -> rewriting_profile_level_idc[i][j] = getNbits(data, pos_bits, 24);
					decoder_svc_sei -> rewriting_avg_bitrate[i][j] = getNbits(data, pos_bits, 16);
					decoder_svc_sei -> rewriting_max_bitrate[i][j] = getNbits(data, pos_bits, 16);
				}
			}
		}
	}

	if(priority_layer_info_present_flag) {
		decoder_svc_sei -> pr_num_dId_minus1 = read_ue(data, pos_bits);
		for(i=0; i<=decoder_svc_sei -> pr_num_dId_minus1;i++) {
			decoder_svc_sei -> pr_dependency_id[i] = getNbits(data, pos_bits, 3);
			decoder_svc_sei -> pr_num_minus1[i] = read_ue(data, pos_bits);
			for(j=0; j<=decoder_svc_sei -> pr_num_minus1[i]; j++) {
				decoder_svc_sei -> pr_id[i][j] = read_ue(data, pos_bits);
				decoder_svc_sei -> pr_profile_level_idc[i][j] = getNbits(data, pos_bits, 24);
				decoder_svc_sei -> pr_avg_bitrate[i][j] = getNbits(data, pos_bits, 16);
				decoder_svc_sei -> pr_max_bitrate[i][j] = getNbits(data, pos_bits, 16);
			}
		}
	}

	if(priority_id_setting_flag) {
		priority_id_setting_uri_idx = 0;

		do{
			short tmp;
			tmp = getNbits(data, pos_bits, 8);
			decoder_svc_sei -> priority_id_setting_uri[priority_id_setting_uri_idx] = (char) tmp;
			priority_id_setting_uri_idx++;
		}while (decoder_svc_sei -> priority_id_setting_uri[priority_id_setting_uri_idx-1] != 0); 
	} 
#endif
}




void pan_scan_rect(unsigned char *data, int *pos_bits) {

	short i;
	int pan_scan_rect_cancel_flag;
/*  short pan_scan_rect_left_offset[3];
	short pan_scan_rect_right_offset[3];
	short pan_scan_rect_top_offset[3];
	short pan_scan_rect_bottom_offset[3];
	short pan_scan_rect_repetition_period;*/

	int pan_scan_rect_id = read_ue(data, pos_bits);
	pan_scan_rect_cancel_flag = getNbits(data, pos_bits, 1); 
	if(!pan_scan_rect_cancel_flag) {
		int pan_scan_cnt_minus = read_ue(data, pos_bits) + 1;
			for(i = 0; i <= pan_scan_cnt_minus; i++) {
			/*	pan_scan_rect_left_offset[ i ] =*/ read_se(data, pos_bits); 
			/*	pan_scan_rect_right_offset[ i ] =*/ read_se(data, pos_bits); 
			/*	pan_scan_rect_top_offset[ i ] = */read_se(data, pos_bits); 
			/*	pan_scan_rect_bottom_offset[ i ] = */read_se(data, pos_bits); 
			}
		/*	pan_scan_rect_repetition_period =*/ read_ue(data, pos_bits);
	}
}

void filler_payload(unsigned char *data, int *pos_bits, int payload_size) {

	int payload_cnt = 0;

	while (payload_cnt < payload_size)
	{
	if (showNbits(data, (*pos_bits + payload_cnt) , 8) == 0xFF)
	{
		payload_cnt++;
	}
	}
}


void user_data_registered_itu_t_t35(unsigned char *data, int *pos_bits, int payload_size) {
	short i;
//	short itu_t_t35_payload_byte;

	int itu_t_t35_country_code = getNbits(data, pos_bits, 8); 
	if(itu_t_t35_country_code	!= 0xFF)
		i = 1;
	else {
		/*int itu_t_t35_country_code_extension_byte =*/ getNbits(data, pos_bits, 8);	
		i = 2;
	}
	do {
	/*	itu_t_t35_payload_byte =*/ getNbits(data, pos_bits, 8); 
		i++;
	} while(i < payload_size);
}

void recovery_point(unsigned char *data, int *pos_bits) {
	/*int recovery_frame_cnt = */read_ue(data, pos_bits);
	/*int exact_match_flag = */getNbits(data, pos_bits, 1);	
	/*int broken_link_flag = */getNbits(data, pos_bits, 1);	
	/*int changing_aio_pstSlice_group_idc = */getNbits(data, pos_bits, 2);	
}

void dec_ref_pic_marking_repetition(unsigned char *data, int *pos_bits, SPS *sps) {
//	short original_bottom_field_flag;
	
	/*short original_idr_flag = */getNbits(data, pos_bits, 1);	
	/*short original_frame_num = */read_ue(data, pos_bits);
	if(!sps->frame_mbs_only_flag) {
		if(getNbits(data, pos_bits, 1))
		/*	original_bottom_field_flag =*/ getNbits(data, pos_bits, 1);	
	}
	//dec_ref_pic_marking()
}

void spare_pic(unsigned char *data, int *pos_bits, SPS *sps) {

	short i,j;
//	short target_bottom_field_flag;
	int num_spare_pics;
//	short delta_spare_frame_num[ 16 ];
//	short spare_bottom_field_flag[ 16 ];
	int spare_area_idc[ 16 ];
//	short spare_unit_flag[ 16 ][PIC_WIDTH_MB_MAX * PIC_HEIGHT_MB_MAX];
//	short zero_run_length[ 16 ][PIC_WIDTH_MB_MAX * PIC_HEIGHT_MB_MAX];
	int spare_field_flag;
	
	/*int target_frame_num = */read_ue(data, pos_bits);
	spare_field_flag = getNbits(data, pos_bits, 1);	

	if(spare_field_flag)
	/*	target_bottom_field_flag =*/ getNbits(data, pos_bits, 1);	
	num_spare_pics	= read_ue(data, pos_bits) + 1; 
	for(i = 0; i < num_spare_pics; i++) {
	/*		delta_spare_frame_num[ i ]	=*/ read_ue(data, pos_bits); 

		if(spare_field_flag)
		/*	spare_bottom_field_flag[ i ] =*/ getNbits(data, pos_bits, 1); 

		spare_area_idc[ i ]	= read_ue(data, pos_bits);

		if(spare_area_idc[ i ]	==  1)
			for(j = 0; j < sps->PicSizeInMbs; j++) 
				/*spare_unit_flag[ i ][ j ] =*/ getNbits(data, pos_bits, 1); 

		else if(spare_area_idc[ i ]	== 2) {
			short mapUnitCnt = 0;
			for(j=0; mapUnitCnt < sps->PicSizeInMbs; j++) {
			/*	zero_run_length[ i ][ j ] =*/ read_ue(data, pos_bits);
			/*	mapUnitCnt += zero_run_length[ i ][ j ] + 1;*/
			}
		}

	} 
}

void scene_info(unsigned char *data, int *pos_bits) {
	
//	short second_scene_id;
	int scene_transition_type;

	if(getNbits(data, pos_bits, 1)   ) {
		read_ue(data, pos_bits);
		scene_transition_type = read_ue(data, pos_bits);
		if(scene_transition_type > 3)
		/*	second_scene_id =*/ read_ue(data, pos_bits);
	}
}

void sub_seq_info(unsigned char *data, int *pos_bits   ) {

//	short sub_seq_frame_num;

	/*int sub_seq_layer_num = */read_ue(data, pos_bits);
	/*int sub_seq_id = */read_ue(data, pos_bits);
	/*int first_ref_pic_flag = */getNbits(data, pos_bits, 1);
	/*int leading_non_ref_pic_flag = */getNbits(data, pos_bits, 1);
	/*int last_pic_flag = */getNbits(data, pos_bits, 1);
	if(getNbits(data, pos_bits, 1))
	/*	sub_seq_frame_num = */read_ue(data, pos_bits);
}

void sub_seq_layer_characteristics(unsigned char *data, int *pos_bits) {

	short layer;
	int num_sub_seq_layers = read_ue(data, pos_bits) + 1;
	for(layer = 0; layer <= num_sub_seq_layers; layer++) {
		/*int accurate_statistics_flag = */getNbits(data, pos_bits, 1);
		/*int average_bit_rate = */getNbits(data, pos_bits, 16);
		/*int average_frame_rate =*/ getNbits(data, pos_bits, 16);
	}
}


void sub_seq_characteristics(unsigned char *data, int *pos_bits) {

	short n;
//	short sub_seq_duration;
//	short average_rate_flag;
//	short accurate_statistics_flag;
//	short average_bit_rate;
//	short average_frame_rate;
//	short ref_sub_seq_layer_num;
//	short ref_sub_seq_id;
//	short ref_sub_seq_direction;
	int num_referenced_subseqs;


	/*int sub_seq_layer_num = */read_ue(data, pos_bits);
	/*int sub_seq_id = */read_ue(data, pos_bits);


	if(getNbits(data, pos_bits, 1))
	/*	sub_seq_duration =*/ getNbits(data, pos_bits, 32);
/*	average_rate_flag = */getNbits(data, pos_bits, 1);
	if(getNbits(data, pos_bits, 1)) {
	/*	accurate_statistics_flag = */getNbits(data, pos_bits, 1);
	/*	average_bit_rate =*/ getNbits(data, pos_bits, 16);
	/*	average_frame_rate =*/ getNbits(data, pos_bits, 16);
	}
	num_referenced_subseqs = read_ue(data, pos_bits);
	for(n = 0; n < num_referenced_subseqs; n++) {
	/*	ref_sub_seq_layer_num =*/ read_ue(data, pos_bits);
	/*	ref_sub_seq_id = */read_ue(data, pos_bits);
	/*	ref_sub_seq_direction = */getNbits(data, pos_bits, 1);
	}
}
void full_frame_freeze(unsigned char *data, int *pos_bits) {

	/*short full_frame_freeze_repetition_period = */read_ue(data, pos_bits);
}
void full_frame_freeze_release() {

}
void full_frame_snapshot(unsigned char *data, int *pos_bits   ) {
	
	/*short snapshot_id =*/ read_ue(data, pos_bits);
}

void progressive_refinement_segment_start(unsigned char *data, int *pos_bits   ) {

	/*short progressive_refinement_id = */read_ue(data, pos_bits);
	/*short num_refinement_steps =*/ read_ue(data, pos_bits) /*+ 1*/;
}

void progressive_refinement_segment_end(unsigned char *data, int *pos_bits) {
	
	/*short progressive_refinement_id = */read_ue(data, pos_bits);
}

void motion_constrained_aio_pstSlice_group_set(unsigned char *data, int *pos_bits) {

//	short aio_pstSlice_group_id(8];
/*	short num_aio_pstSlice_groups_in_set	= */read_ue(data, pos_bits);

/*	for(i = 0; i <= num_aio_pstSlice_groups_in_set; i++)
		aio_pstSlice_group_id[ i ] = getNbits(data, &pos_bits, log2_num_slice_groups_minus1);
	exact_sample_value_match_flag
		pan_scan_rect_flag
		if(pan_scan_rect_flag)
			pan_scan_rect_id*/
}

void reserved_sei_message(unsigned char *data, int *pos_bits, int payloadSize) {
	short i;
//	short reserved_sei_message_payload_byte;
	for(i = 0; i < payloadSize; i++)
	/*	reserved_sei_message_payload_byte	=*/ getNbits(data, pos_bits, 8);

}


/**
This function permits to decode the SEI message. 

@param data The NAL unit.
@param pos_bits The current aio_piPosition in the NAL unit.
@param payloadType The type of the SEI payload.
@param payloadSize The size of the SEI payload.
*/
void sei_payload (unsigned char *data, int payloadType, int payloadSize, int *pos_bits, SPS sps[], SEI *decoder_svc_sei)
{
	// int tmp;
	switch(payloadType){
	case 0:
		buffering_period (data, sps, pos_bits);	break;
	case 1:
		pic_timing (data, sps, pos_bits);	break;
	case 2:
		pan_scan_rect (data, pos_bits);	break;
	case 3:
		filler_payload(data, pos_bits, payloadSize);break;
	case 4:
		user_data_registered_itu_t_t35 (data, pos_bits, payloadSize);	break;
	case 5:
		user_data_unregistered(data, payloadSize, pos_bits);	break;
	case 6:
		recovery_point (data, pos_bits);	break;
	case 7:
		dec_ref_pic_marking_repetition (data, pos_bits, sps);	break;
	case 8:
		spare_pic (data, pos_bits,sps);	break;
	case 9:
		scene_info (data, pos_bits);	break;
	case 10:
		sub_seq_info (data, pos_bits);	break;
	case 11:
		sub_seq_layer_characteristics (data, pos_bits);	break;
	case 12:
		sub_seq_characteristics (data, pos_bits);	break;
	case 13:
		full_frame_freeze (data, pos_bits);	break;
	case 14:
		full_frame_freeze_release ();	break;
	case 15:
		full_frame_snapshot (data,pos_bits);	break;
	case 16:
		progressive_refinement_segment_start (data,pos_bits);	break;
	case 17:
		progressive_refinement_segment_end (data,pos_bits);	break;
	case 18:
		motion_constrained_aio_pstSlice_group_set (data,pos_bits);	break;
	case 24:
		scalability_info(data,sps,pos_bits, decoder_svc_sei);	break;
	// todo: handle payloadtype 19 - 44
	default:
		//reserved_sei_message (data, pos_bits, payloadSize);
		break;
	}


	// pointeur moved to a bytealign aio_piPosition
	if (!bytes_aligned(*pos_bits)) {
		do{
			getNbits(data, pos_bits, 1);
		}while (!bytes_aligned(*pos_bits)); 
	}
}


/**
This function permits to decode from the bitstream the Supplemental Enhancement Information. 

@param data The NAL unit.
@param aio_piPosition The current aio_piPosition in the NAL.
@param NalBytesInNalunit The sizes of the NAL unit in bytes.
*/
void sei_rbsp (unsigned char *data, int NalBytesInNalunit, SPS sps[], SEI *decoder_svc_sei)
{
	int i = 0;
	int payloadType = 0;
	int payloadSize = 0;	 
	int aio_piPosition = 8;

	memset(decoder_svc_sei, 0, sizeof(SEI));

	do{
		// sei_message();

		do{
			payloadType += showNbits(data, aio_piPosition , 8);
		}while(getNbits(data, &aio_piPosition, 8) == 255);

		payloadSize = 0;
		do{
			payloadSize += showNbits(data, aio_piPosition, 8);
		}while(getNbits(data, &aio_piPosition, 8)== 255);

		sei_payload(data, payloadType, payloadSize, &aio_piPosition, sps, decoder_svc_sei);
		i++;
	} while(aio_piPosition / 8 < NalBytesInNalunit);

	rbsp_trailing_bits(&aio_piPosition);
}
