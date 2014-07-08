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
#include <math.h>
#ifndef POCKET_PC
#ifdef WIN32
#include "windows.h"
#endif
#endif

//AVC Files
#include "data.h"
#include "clip.h"
#include "bitstream.h"
#include "init_data.h"
#include "AVCdisplay.h"
#include "FinishFrame.h"
#include "slice_header.h"
#include "quantization.h"
#include "SliceGroupMap.h"
#include "slice_data_vlc.h"
#include "memory_management.h"



/**
This function permits to initialize a table factor for direct prediction.


@param dist_scale_factor Table which contains the factors.
@param Mmo MMO structure which contains all information about memory management of the video.
@param Current_pic LIST_MMO structure with contains information on the current picture.
@param ref_listl0 Refernce list l0.
@param ref_listl1 Refernce list l1.
@param poc Curretn poc of the picture.
*/
void direct_dist_scale_factor(short dist_scale_factor[], LAYER_MMO *LayerMmo, 
							  LIST_MMO *ref_listl0,  LIST_MMO * ref_listl1, int poc)
{


	const int poc1 = ref_listl1[0] . poc;
	int i;

	for(i = 0; i < LayerMmo -> RefCountL0; i++){
		int poc0 = ref_listl0[i].poc;
		int td = CLIP3( -128, 127, poc1 - poc0);
		if(td == 0 /* FIXME || pic0 is a long-term ref */){
			dist_scale_factor[i] = 256;
		}else{
			int tb = CLIP3( -128, 127, poc - poc0);
			int tx = (16384 + (abs(td) >> 1)) / td;
			dist_scale_factor[i] = CLIP3( -1024, 1023, (tb*tx + 32) >> 6);
		}
	}
}





/**
This function permits to recover the Slice parameters. 
All the parameters decoded will be stored in the slice structure.


@param data The NAL unit.
@param Position The current Position in the NAL.
@param Slice The slice structure contains all information used to decode the next slice
@param sps The SPS structure.
@param Mmo MMO structure which contains all information about memory management of the video.
@param Current_pic LIST_MMO structure with contains information on the current picture.
@param RefPicListL0 Refernce list l0.
@param RefPicListL1 Refernce list l1.
*/
void ref_pic_list_reordering ( unsigned char *data, int *Position, SLICE *Slice, SPS *sps, 
							  LAYER_MMO *LayerMmo, LIST_MMO *Current_pic , LIST_MMO RefPicListL0[], LIST_MMO RefPicListL1[] )
{


	int i, index, list;


	if(Slice -> slice_type != SLICE_TYPE_I && Slice -> slice_type != SLICE_TYPE_SI){
		LIST_MMO ref[1];
		LIST_MMO *ref_list = RefPicListL0;
		int ref_count = Slice -> num_RefIdxL0_active_minus1 + 1;

		for(list = 0; list < 2; list++){
			if(list){
				ref_list = RefPicListL1;
				ref_count = Slice -> num_RefIdxL1_active_minus1 + 1;
			}
			*ref = ref_list[0];


			if ( getNbits(data, Position, 1)){  /*ref_pic_list_reordering_flag_l0 */ 
				int pred = LayerMmo -> frame_num;

				for(index = 0; index <= ref_count; index++){
					int reordering_of_pic_nums_idc = read_ue(data, Position);	
					if(reordering_of_pic_nums_idc == 3)
						break;

					if(reordering_of_pic_nums_idc < 3){
						if(reordering_of_pic_nums_idc < 2){
							int abs_diff_pic_num_minus1 = read_ue(data, Position) + 1; 

							if(reordering_of_pic_nums_idc == 0){
								pred -= abs_diff_pic_num_minus1;
							}else{
								pred += abs_diff_pic_num_minus1;
							}

							pred &= sps -> max_pic_num - 1;
							for(i = LayerMmo -> ShortRefCount - 1; i >= 0; i--){
								*ref = LayerMmo -> ShortRef [i];
								if(ref -> frame_num == pred){
									// ignore non existing pictures
									break;
								}
							}

							if(i >= 0){
								ref -> pic_id = ref -> frame_num;
							}

						}else{
							int long_term_pic_num = read_ue(data, Position);
							*ref = LayerMmo -> LongRef [long_term_pic_num];
							ref -> pic_id = long_term_pic_num;
							i = 0;
						}

						for(i = index; i < ref_count - 1; i++){
							if( ref -> pic_id == ref_list[i] . pic_id){
								break;
							}
						}
						for(; i > index; i--){
							ref_list[i]= ref_list[i - 1];
						}

						ref_list[index]= *ref;
					}
				}
			}


			if(Slice -> slice_type != SLICE_TYPE_B) {
				break;
			}
		}
	}
	if ( Slice -> slice_type == SLICE_TYPE_B && !Slice -> direct_spatial_mv_pred_flag){
		direct_dist_scale_factor(Slice -> dist_scale_factor, LayerMmo, RefPicListL0,  RefPicListL1, Current_pic -> poc);
	}
}




/**
This function parses the MMCO operation code.
*/
void GetMMCOOperationCode(SPS *sps, LAYER_MMO *LayerMmo, unsigned char *data, int *Position)
{
	int i;
	MMCO *Mmco = LayerMmo -> Mmco;
	for(i = 0; i < MAX_MMCO_COUNT; i++) {
		Mmco[i] . opcode = read_ue(data, Position);

		if(! Mmco[i] . opcode){
			break;
		}

		if (  Mmco[i] . opcode == 1 ||  Mmco[i] . opcode == 3 ) {
			Mmco[i] . short_frame_num = (LayerMmo -> frame_num - read_ue(data, Position) - 1) & ((1 << (sps -> log2_max_frame_num)) - 1) ;// difference_of_pic_nums_minus1 =
		}
		if (  Mmco[i] . opcode == 2 ) {
			Mmco[i] .  long_term_frame_idx = read_ue(data, Position);// long_term_pic_num =
		}
		if (  Mmco[i] . opcode == 3 ||  Mmco[i] . opcode == 6 ) {
			Mmco[i] . long_term_frame_idx = read_ue(data, Position);// long_term_frame_idx =
		}
		if (  Mmco[i] . opcode == 4 ) {
			Mmco[i] . long_term_frame_idx = read_ue(data, Position);//max_long_term_frame_idx_plus1 =
		}

	}
	LayerMmo -> index = i;
}

/**
This function permits to recover the Slice parameters. 
All the parameters decoded will be stored in the slice structure.

@param data The NAL unit.
@param Position The current Position in the NAL.
@param nal_unit_type The type of the NAL unit.
@param sps The SPS structure.
@param Mmo MMO structure which contains all information about memory management of the video.
@param mmco MMCO structure which contains all information to execute on the reference list.
*/
void dec_ref_pic_marking ( unsigned char *data, int *Position, int nal_unit_type, 
						  SPS *sps, LAYER_MMO *LayerMmo)
{


	if ( nal_unit_type == 5 ) {	
		int long_term_reference_flag;
		getNbits(data, Position, 1) /*-1*/;/*const int no_output_of_prior_pics_flag = */
		long_term_reference_flag = getNbits(data, Position, 1) - 1;
		if(long_term_reference_flag  == -1)
			LayerMmo -> index = 0;
		else{
			LayerMmo -> index = 1;
			LayerMmo -> Mmco[0] . opcode = 6;
			LayerMmo -> Mmco[0] . long_term_frame_idx = 0;
		}
	} else {
		if ( getNbits(data, Position, 1) ) {
			GetMMCOOperationCode(sps, LayerMmo, data, Position);
		}else{
			LayerMmo -> index = 0;
			if(LayerMmo -> LongRefCount + LayerMmo -> ShortRefCount == sps -> num_ref_frames){ 
				LayerMmo -> Mmco[0] . opcode = 7;
				LayerMmo -> index = 1;					
			}else{
				LayerMmo -> index = 0;
			}
		}
	}
}



/**
Reads and decodes frame header.
*/
void FrameHeaderData(unsigned char *data, int *Position, int NalRefIdc, int IdrFlag, int LayerId, 
					 int EndOfSlice, LIST_MMO *Current_pic, LAYER_MMO *LayerMmo, SPS *sps_id, PPS *pps_id, 
					 MMO *Mmo, LIST_MMO *RefPicListL0, LIST_MMO *RefPicListL1)
{



	if ( !sps_id -> frame_mbs_only_flag ) {
		sps_id -> field_pic_flag = getNbits(data, Position, 1);
#ifdef WIN32
		if (!sps_id -> field_pic_flag && sps_id -> MbAdaptiveFrameFieldFlag){
#ifndef POCKET_PC
			MessageBoxA(NULL,(LPCSTR)"A layer is using a field macroblock", (LPCSTR)"Open SVC Decoder has encountered an error", MB_OK); 
#else 
			MessageBox(NULL,(LPCWSTR)"A layer is using a field macroblock", (LPCWSTR)"Open SVC Decoder has encountered an error", MB_OK);  
#endif
			exit(16);
		}
#endif
		if ( sps_id -> field_pic_flag ) {
			getNbits(data, Position, 1);//bottom_field_flag
		}
	}

	if(!sps_id -> field_pic_flag){
		Current_pic -> frame_num = LayerMmo -> frame_num;
		Current_pic -> long_term = 0;
		sps_id -> max_pic_num = 1 << (sps_id -> log2_max_frame_num);
		Current_pic -> reference  = 3;
		if (!NalRefIdc){
			Current_pic -> reference  = 0;
		}
	}else{
		Current_pic -> frame_num = 2 * LayerMmo -> frame_num;
		sps_id -> max_pic_num = 1 << (sps_id -> log2_max_frame_num + 1);
	}

	if ( IdrFlag == 1) {
		read_ue(data, Position);//idr_pic_id =
		if (Current_pic -> slice_type == SLICE_TYPE_I){
			Current_pic -> KeyFrame = 1;
		}
		if(!EndOfSlice){
			IdrFlushDpb(LayerId, Mmo, RefPicListL0, RefPicListL1);
		}
	}else {
		Current_pic -> KeyFrame = 0;
	}


	if ( sps_id -> pic_order_cnt_type == 0 ) {
		LayerMmo -> pic_order_cnt_lsb = getNbits(data, Position, sps_id -> log2_max_pic_order_cnt_lsb ); 
		if ( pps_id -> pic_order_present_flag && !sps_id -> field_pic_flag ) 
			sps_id -> delta_pic_order_cnt_bottom = read_se(data, Position);
	}

	if ( sps_id -> pic_order_cnt_type == 1 && !sps_id -> delta_pic_order_always_zero_flag ) {
		sps_id -> delta_pic_order_cnt [0] = read_se(data, Position); 
		if ( pps_id -> pic_order_present_flag && !sps_id -> field_pic_flag ) 
			sps_id -> delta_pic_order_cnt [1] = read_se(data, Position);
	}

	if ( pps_id -> redundant_pic_cnt_present_flag ) {
		read_ue(data, Position);//redundant_pic_cnt =
	}
}


/**
Reads reference list parameters.
*/
void ReferenceListProcessing(unsigned char *data, int *Position, SLICE *Slice, 
							 PPS *pps_id, SPS *sps_id, LAYER_MMO *LayerMmo, 
							 LIST_MMO *Current_pic, LIST_MMO *RefPicListL0, LIST_MMO *RefPicListL1)
{

	if ( Slice -> slice_type == SLICE_TYPE_B ) {
		Slice -> direct_spatial_mv_pred_flag = getNbits(data, Position, 1);
	}

	if ( Slice -> slice_type == SLICE_TYPE_P
		|| Slice -> slice_type == SLICE_TYPE_B
		|| Slice -> slice_type == SLICE_TYPE_SP ) {
			int num_ref_idx_active_override_flag = getNbits(data, Position, 1);

			if ( num_ref_idx_active_override_flag ) {
				Slice -> num_RefIdxL0_active_minus1 = read_ue(data, Position);
				if ( Slice -> slice_type == SLICE_TYPE_B ) {
					Slice -> num_RefIdxL1_active_minus1 = read_ue(data, Position);
				}
			} else {
				Slice -> num_RefIdxL0_active_minus1 = pps_id -> num_RefIdxL0_active_minus1 ;
				if ( Slice -> slice_type == SLICE_TYPE_B ) {
					Slice -> num_RefIdxL1_active_minus1 = pps_id -> num_RefIdxL1_active_minus1 ;
				}
			}
	} else {
		Slice -> num_RefIdxL0_active_minus1 = pps_id -> num_RefIdxL0_active_minus1 ;
		Slice -> num_RefIdxL1_active_minus1 = pps_id -> num_RefIdxL1_active_minus1 ;
	}

	LayerMmo -> RefCountL0 = Slice -> num_RefIdxL0_active_minus1 + 1;
	LayerMmo -> RefCountL1 = Slice -> num_RefIdxL1_active_minus1 + 1;


	if (Slice -> slice_type != SLICE_TYPE_I){
		fill_default_ref_list(LayerMmo, RefPicListL0, RefPicListL1, Current_pic, Slice -> slice_type);
	}


	ref_pic_list_reordering(data, Position, Slice, sps_id, LayerMmo, Current_pic, RefPicListL0, RefPicListL1);
}


/**
Read weighted prediction parameters.
Data are readen but not used in oder to decoder the bitstream.
*/
void PredWeightTable(unsigned char *data, int *Position, PPS *pps_id, SLICE *Slice)
{
	if((pps_id -> WeightedPredFlag && Slice -> slice_type == SLICE_TYPE_P) ||
		(pps_id -> WeightedBipredIdc == 1 && Slice -> slice_type == SLICE_TYPE_B)){
			int i;
			read_ue(data, Position); //luma_log2
			read_ue(data, Position); //chroma_log2
			
			for(i = 0; i <= Slice -> num_RefIdxL0_active_minus1; i++){
				if(getNbits(data, Position, 1)){	//luma_weight_flag
					read_se(data, Position);		//luma_weight_l0
					read_se(data, Position);		//luma_offset_l0
				}		
				if(getNbits(data, Position, 1)){	//Chroma_weight_flag
					read_se(data, Position);		//Chroma_weight_l0
					read_se(data, Position);		//Chroma_offset_l0
					read_se(data, Position);		//Chroma_weight_l0
					read_se(data, Position);		//Chroma_offset_l0
				}
			}

			if( Slice -> slice_type == SLICE_TYPE_B){
				for(i = 0; i <= Slice -> num_RefIdxL1_active_minus1; i++){
					if(getNbits(data, Position, 1)){	//luma_weight_flag
						read_se(data, Position);		//luma_weight_l1
						read_se(data, Position);		//luma_offset_l1
					}		
					if(getNbits(data, Position, 1)){	//Chroma_weight_flag
						read_se(data, Position);		//Chroma_weight_l1
						read_se(data, Position);		//Chroma_offset_l1
						read_se(data, Position);		//Chroma_weight_l1
						read_se(data, Position);		//Chroma_offset_l1
					}
				}
			}
	}
}




/**
Read Qp and deblocking filter parameters.
*/
void QpAndDeblockProcessing(unsigned char *data, int *Position, SLICE *Slice, PPS *pps_id, SPS *sps_id)
{
	if ( pps_id -> entropy_coding_mode_flag && Slice -> slice_type != SLICE_TYPE_I && Slice -> slice_type != SLICE_TYPE_SI ){
		Slice -> cabac_init_idc = read_ue(data, Position); 
	}

	Slice -> slice_qp_delta = read_se(data, Position) + pps_id -> pic_init_qp;

	if ( Slice -> slice_type == SLICE_TYPE_SP || Slice -> slice_type == SLICE_TYPE_SI ) {
		if ( Slice -> slice_type == SLICE_TYPE_SP ) {
			getNbits(data, Position, 1);//sp_for_switch_flag =
		}
		read_se(data, Position);//aio_pstSlice_qs_delta = 
	}

	Slice -> disable_deblocking_filter_idc = 0;
	pps_id -> AlphaOffset = 0;
	pps_id -> BetaOffset = 0;

	if ( pps_id -> deblocking_filter_control_present_flag ) {
		Slice -> disable_deblocking_filter_idc = read_ue(data, Position);
		if ( Slice -> disable_deblocking_filter_idc != 1 ) {
			pps_id -> AlphaOffset = (read_se(data, Position) << 1);
			pps_id -> BetaOffset = (read_se(data, Position) << 1);
		}
	}


	if ( pps_id -> num_slice_groups_minus1 > 0 && pps_id -> slice_group_map_type >= 3 && pps_id -> slice_group_map_type <= 5 ) {
		short PicSizeInMapUnits = sps_id -> PicSizeInMbs;
		short aio_pstSliceGroupChangeRate = pps_id -> slice_group_change_rate_minus1;
		short tmp = log_base2(PicSizeInMapUnits / aio_pstSliceGroupChangeRate + 1);
		Slice -> slice_group_change_cycle = getNbits(data, Position, tmp);
	}
}





/**
This function permits to recover the Slice parameters. 
All the parameters decoded will be stored in the slice .

@param ai_pcData The NAL unit.
@param ai_piNalBytesInNalunit The sizes of the NAL unit in bytes.
@param nal_unit_type The type of the NAL unit.
@param nal_ref_idc Specifies if the current picture is a reference one.
@param pps The PPS structure.
@param Position The current Position in the NAL.
@param sps_id The SPS structure of the curretn slice.
@param pps_id The PPS structure of the curretn slice.
@param Current_pic LIST_MMO structure with contains information on the current picture.
@param entropy_coding_flag Entropy coding used.
@param quantif Table which contains the quantization table
@param sps The SPS structure.
@param Slice The slice structure contains all information used to decode the next slice
@param Mmo MMO structure which contains all information about memory management of the video.
@param mmco MMCO structure which contains the operatio to do on the short and long refernce list.
@param RefPicListL0 Refernce list l0.
@param RefPicListL1 Refernce list l1.
@param ao_piEndOfSlice Specifies if the current picture is totally decoded.
*/

void slice_header ( unsigned char *data, int *nal_unit_type, int *nal_ref_idc, 
				   PPS *pps, int *Position, SPS *sps_id, PPS *pps_id, 
				   int *EntropyCodingType, W_TABLES *quantif, LIST_MMO *Current_pic, 
				   SPS *sps, SLICE *Slice, MMO *Mmo, LIST_MMO RefPicListL0[], 
				   LIST_MMO RefPicListL1[], int *EndOfSlice,
				   int *ImgToDisplay, int *xsize, int *ysize, int *AddressPic, int *Crop)

{
	if( Mmo -> MemoryAllocation){
		LAYER_MMO *LayerMmo = &Mmo -> LayerMMO [0];
		int PicParameterId,seq_parameter_set_id;
		//For Error detection
		int LastFrameNum = LayerMmo -> frame_num;
		int PrevEndOfSlice = *EndOfSlice;
		int FirstMbInSlice = Slice -> first_mb_in_slice;

		//Get frame parameters
		*Position = 8;	

		Slice -> AdaptiveBaseModeFlag = 0;//mbl check
		Slice -> first_mb_in_slice = read_ue(data, Position);
		Current_pic -> slice_type = Slice -> slice_type = read_ue(data, Position);
		if ( Slice -> slice_type > 4 ){
			Current_pic -> slice_type = Slice -> slice_type -= 5 ;
		}

		PicParameterId = read_ue(data, Position);
		PicParameterId = CLIP3(0, PPS_BUF_SIZE - 1, PicParameterId);
		*pps_id = pps [PicParameterId];

		seq_parameter_set_id = pps_id -> seq_parameter_set_id;
		*sps_id = sps[seq_parameter_set_id];
		LayerMmo -> frame_num = getNbits(data, Position, sps_id -> log2_max_frame_num );
		*EntropyCodingType = pps_id -> entropy_coding_mode_flag;

		//EndOfSlice is set to one, if the VLD has detected that not enough decoded macroblock,
		//And that the the frame_num did not change
		*EndOfSlice = PrevEndOfSlice && LayerMmo -> frame_num == LastFrameNum && FirstMbInSlice != Slice -> first_mb_in_slice? 1: 0;

#ifdef ERROR_DETECTION
		//Detect slice error in the previous frame
		//If PrevEndOfSlice is equals to 1 and *EndOfSlice is equals to 0, 
		//the previous frame has not been stored in the poc managment, we have to do it.
		if (PrevEndOfSlice && !*EndOfSlice){
			FinishFrameError(sps_id, Current_pic, Crop, ImgToDisplay, AddressPic, Mmo, xsize, ysize);
		}	
#endif

		//Get frame paraters like framenum and POC
		FrameHeaderData(data, Position, *nal_ref_idc, *nal_unit_type == 5, 0, *EndOfSlice, 
			Current_pic, LayerMmo, sps_id, pps_id, Mmo, RefPicListL0, RefPicListL1);


		if ( !*EndOfSlice){
			pic_order_process(sps_id, *nal_unit_type, *nal_ref_idc, Mmo, LayerMmo, Current_pic, RefPicListL0, RefPicListL1);
			Current_pic -> MemoryAddress =  Mmo -> FreeMemoryAddress [Mmo -> NumFreeAddress - 1];
			Current_pic -> MvMemoryAddress =  LayerMmo -> MvMemoryAddress [LayerMmo -> FreeMvMemory - 1];
			Mmo -> FreeMemoryAddress  [Mmo -> NumFreeAddress - 1] = -1;
			LayerMmo -> MvMemoryAddress [LayerMmo -> FreeMvMemory - 1] = -1;
			Current_pic -> displayed = 0;
			Mmo -> NumFreeAddress--;
			LayerMmo -> FreeMvMemory--;
		}


		//Get the number of reference frames and initialize the reference lists.
		ReferenceListProcessing(data, Position, Slice, pps_id, sps_id, LayerMmo, Current_pic, RefPicListL0, RefPicListL1);

		if (!*EndOfSlice){
			direct_ref_list_init(Slice, RefPicListL0, RefPicListL1, Current_pic, LayerMmo);
		}

		//Read weighted table
		PredWeightTable(data, Position, pps_id, Slice);

		if ( *nal_ref_idc != 0 ) {
			dec_ref_pic_marking(data, Position, *nal_unit_type, sps_id, LayerMmo);
		}

		//Parse last parameters of the slice header, concerning qp, loop filter and slice group.
		QpAndDeblockProcessing(data, Position, Slice, pps_id, sps_id);

		//Intialize the quantization tables
		init_quantization (quantif, sps_id -> scaling_matrix_present, pps_id -> scaling_matrix4, pps_id -> scaling_matrix8 );

		Mmo -> nb_img_display = sps_id -> nb_img_disp;
		Slice -> slice_num++ ;
	}
}






/**
Slice management to correct bit stream error.
This fonction should detect error in the variable mb_xy from the structure SLICE
according to the Slice group map.
*/
void SliceManagement(PPS *Pps, SLICE *Slice){

	//
	if(!Pps -> num_slice_groups_minus1 && Slice -> mb_xy != Slice -> first_mb_in_slice){
		Slice -> mb_xy = Slice -> first_mb_in_slice;
	}
}


/**
This function permits to decode the VLC. 
All the parameters decoded will be stored in differents structures or tables.


@param ai_pcData The NAL unit.
@param ai_piNalBytesInNalunit The sizes of the NAL unit in bytes.
@param Position The current Position in the NAL.
@param Sps The SPS structure.
@param pps The PPS structure.
@param Vlc VLC_TABLES contains all the tables for decoding the vlc.
@param ai_slice_group_id Table for determine the slice decoding map.
@param Slice The slice structure contains all information used to decode the next slice
@param aio_pstSliceGroupMap Specifies in which Slice belongs each macroblock of the picture.
@param SliceTable Specifies in which Slice belongs each macroblock of the picture.
@param aio_tstNon_zero_count Specifies the coeff_token of each block of the picture.
@param aio_tstBlock Contains all parameters of each macroblock of the current picture.
@param residu Structure which contains all the residu data of the current macroblock.
@param ao_pstIntra_pred_mode Contains the prediction mode for each macroblock.
@param ao_piEndOfSlice Specifies if the current picture is totally decoded.
@param mvl0_io A table where the motion vector are stocked for each 4x4 block of each macroblock.
@param refl0_io A table where the reference is stocked for each 4x4 block of each macroblock.

*/
void slice_layer_without_partitioning_rbsp ( unsigned char *ai_pcData, const int *ai_piNalBytesInNalunit, 
											const int *ai_piPosition, const SPS *Sps, PPS *Pps, VLC_TABLES *Vlc, 
											unsigned char *ai_slice_group_id, SLICE *Slice, unsigned char *aio_tiMbToSliceGroupMap, 
											unsigned char *SliceTable, DATA *aio_tstBlock, RESIDU *picture_residu, 
											int *EndOfSlice, short *mvl0_io, short *refl0_io) 
{
	int pos = *ai_piPosition;


	//Parameters initialization Initialisation in case of a new picture
	if ( *EndOfSlice == 0 ) {
		reinit_slice(Slice);
		Slice -> mb_stride = Sps -> pic_width_in_mbs;
		Slice -> b_stride = Sps -> b_stride;
		Slice -> b8_stride = Sps -> b8_stride;
		mapUnitToSlice(Sps, Pps, aio_tiMbToSliceGroupMap, Slice -> slice_group_change_cycle, ai_slice_group_id);
		InitUcharTab(Sps -> PicSizeInMbs, SliceTable);
	}else{
		SliceManagement(Pps, Slice);
	}

	if(Slice -> slice_type == SLICE_TYPE_P){
		slice_data_P_cavlc (picture_residu, ai_pcData, ai_piNalBytesInNalunit, 
			Sps,Pps, aio_tstBlock, Slice, &pos, Vlc, SliceTable, 
			aio_tiMbToSliceGroupMap, EndOfSlice,  mvl0_io, refl0_io);
	}else if (Slice -> slice_type == SLICE_TYPE_I){
		slice_data_I_cavlc (picture_residu, ai_pcData, ai_piNalBytesInNalunit, Sps,
			Pps, aio_tstBlock	, Slice, &pos, Vlc, SliceTable, 
			aio_tiMbToSliceGroupMap, EndOfSlice);
	}
}


