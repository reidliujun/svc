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
#include "type.h"
#include "data.h"
#include "clip.h"
#include "cabac.h"
#include "cavlc.h"
#include "refill.h"
#include "init_data.h"
#include "main_data.h"
#include "vlc_cabac.h"
#include "write_back.h"
#include "cabac_table.h"
#include "WriteBackSVC.h"
#include "mb_pred_main.h"
#include "slice_header.h"
#include "SliceGroupMap.h"
#include "neighbourhood.h" 
#include "motion_vector.h"
#include "macroblock_cabac.h"
#include "slice_data_cabac.h"
#include "fill_caches_cabac.h"



//SVC Files
#include "fill_caches_svc.h"
#include "ErrorConcealment.h"
#include "nal_unit_header_svc_extension.h"





/**
This function permits to decode the VLC for an P image. 
All the parameters decoded will be stored in differents structures or tables.

*/
char P_cabac (RESIDU *picture_residu, unsigned char *data,int *NalBytesInNalunit, SPS *sps, PPS *pps, DATA *block, 
			  SLICE *slice, int *aio_piPosition, unsigned char *SliceTable, unsigned char *MbToSliceGroupMap, short *mv_cabac_l0, 
			  short *ref_cabac_l0, int *EndOfSlice, short *mvl0_io, short *refl0_io)
{


	//Initialization of the image parameters
	__declspec(align(64))  unsigned char non_zero_count_cache [48];
	__declspec(align(64))  short mv_cache_l0[48][2];
	__declspec(align(64))  short ref_cache_l0[48];
	__declspec(align(64))  short mvl0_cache[48][2];
	__declspec(align(64))  short refl0_cache[48];


	CABAC_NEIGH param_neigh[1];
	CABACContext cabac[1];


	volatile int moreDataFlag = 1 ;
	int slice_QP = slice -> slice_qp_delta ;
	int mb_skip_flag = 0 ;
	int diff_qp = 0;

	short iCurrMbAddr = slice -> first_mb_in_slice;
	const short PicSizeInMbs = sps -> PicSizeInMbs ;
	const short PicWidthInMbs = sps -> pic_width_in_mbs;
	const short PicHeightInMbs = sps -> pic_height_in_map_units;


	short iMb_x;
	short iMb_y;
	unsigned char Last_Qp = slice -> slice_qp_delta ;
	unsigned char ErrorDetection = 0;

	//initialize macroblock position
	GetMbPosition(iCurrMbAddr, PicWidthInMbs, &iMb_x, &iMb_y);

	//CABAC
	CabacDecoderInitAVC(cabac, data, NalBytesInNalunit, *aio_piPosition, slice_QP, &cabac_context_init_PB[slice -> cabac_init_idc][0]);


	//Decoding process macroblock per macroblock of one slice
	do {

		init_block(&block[iCurrMbAddr]);
		init_residu(&picture_residu[iCurrMbAddr]);
		SliceTable [iCurrMbAddr] = slice -> slice_num ;
		GetNeighbour(iCurrMbAddr, iMb_x, PicWidthInMbs, PicSizeInMbs, SliceTable, picture_residu, 0);

		//Detection of a skipped macroblock
		mb_skip_flag = decode_cabac_mb_skip( cabac, cabac_state, picture_residu[iCurrMbAddr] . AvailMask, 
			SliceTable, picture_residu, slice -> slice_type, iCurrMbAddr, PicWidthInMbs);
		if (mb_skip_flag){
			diff_qp = 0;
			//Updating the skipped macroblock parameters
			SliceTable [iCurrMbAddr] = slice -> slice_num ;
			init_P_skip_mb(&picture_residu[iCurrMbAddr], Last_Qp,  slice);


			init_mb_skip_mv_ref(&mv_cabac_l0[iMb_x * 8 + iMb_y * (PicWidthInMbs << 5)], 
				&ref_cabac_l0[iMb_x * 2 + iMb_y * (PicWidthInMbs << 2)], PicWidthInMbs << 3, PicWidthInMbs << 1);
			init_skip_non_zero_count(iMb_x, iMb_y, slice -> mb_stride, PicSizeInMbs,block );



			//Motion vector calculation
			init_ref_cache(refl0_cache);
			init_mv_cache(mvl0_cache);
			fill_caches_motion_vector_full_ref(slice, slice -> b_stride, slice -> b8_stride, 
				refl0_cache, mvl0_cache, SliceTable , picture_residu, 
				&mvl0_io[(iMb_x << 3) + iMb_y * (slice -> b_stride << 2)], 
				&refl0_io[(iMb_x << 2) + iMb_y * (slice -> b_stride << 1)], iMb_x, iMb_y);



			motion_vector_skip( mvl0_cache, refl0_cache);
			write_back_P_skip_motion_full_ref( slice -> b_stride, slice -> b8_stride, 
				&mvl0_io [(iMb_x << 3) + iMb_y * (slice -> b_stride << 2)], mvl0_cache[12], 
				&refl0_io[(iMb_x << 2) + iMb_y * (slice -> b_stride << 1)]);

		}
		else{
			//In case of there is more data in the NAL
			init_ref_cache(ref_cache_l0);
			init_ref_cache(refl0_cache);
			init_mv_cache(mvl0_cache);
			init_mv_cache(mv_cache_l0);

			fill_caches_motion_vector( slice, PicWidthInMbs << 3, PicWidthInMbs << 1, ref_cache_l0, 
				mv_cache_l0, SliceTable, &picture_residu[iCurrMbAddr], 
				&mv_cabac_l0[iMb_x * 8 + iMb_y * (PicWidthInMbs << 5)], 
				&ref_cabac_l0[iMb_x * 2 + iMb_y * (PicWidthInMbs << 2)],   iMb_x,iMb_y);

			fill_caches_motion_vector_full_ref(slice, slice -> b_stride, slice -> b8_stride, refl0_cache, 
				mvl0_cache, SliceTable , picture_residu, &mvl0_io[(iMb_x << 3) + iMb_y * (slice -> b_stride << 2)], 
				&refl0_io[(iMb_x << 2) + iMb_y * (slice -> b_stride << 1)], iMb_x, iMb_y);


			//Decoding process of the current macroblock
			ErrorDetection = macroblock_layer_P_cabac(&picture_residu[iCurrMbAddr], slice, block, non_zero_count_cache, SliceTable, 
				iCurrMbAddr, cabac, cabac_state, param_neigh, mv_cache_l0, ref_cache_l0, iMb_x, iMb_y, 
				pps -> constrained_intra_pred_flag, pps -> transform_8x8_mode_flag, &Last_Qp, &diff_qp, mvl0_cache, refl0_cache);

			//Recording of the neighbourhood for the decoding of the VLC 
			NonZeroCountManagement(non_zero_count_cache, iMb_x, iMb_y, PicWidthInMbs, PicHeightInMbs, &block[iCurrMbAddr], &picture_residu[iCurrMbAddr]);

			write_back_motion(PicWidthInMbs << 3, PicWidthInMbs << 1, 
				&mv_cabac_l0[iMb_x * 8 + iMb_y * (PicWidthInMbs << 5)], mv_cache_l0, 
				&ref_cabac_l0[iMb_x * 2 + iMb_y * (PicWidthInMbs << 2)], ref_cache_l0);

			//Recording of the motion vector
			write_back_motion_full_ref( slice -> b_stride, slice -> b8_stride, 
				&mvl0_io [(iMb_x << 3) + iMb_y * (slice -> b_stride << 2)], mvl0_cache, 
				&refl0_io[(iMb_x << 2) + iMb_y * (slice -> b_stride << 1)], refl0_cache );
		}
		//Save the last macroblock address
		slice -> mb_xy++;

		//Updating the macroblock adress
		iCurrMbAddr = Next_MbAddress(iCurrMbAddr, PicSizeInMbs, PicWidthInMbs, MbToSliceGroupMap, &iMb_x, &iMb_y);
		moreDataFlag = !get_cabac_terminate(cabac, slice -> mb_xy, PicSizeInMbs);

	} while ( moreDataFlag && !ErrorDetection);
	//To set endofslice according to the error detection.
	*EndOfSlice = EndOfSliceManagement(slice, PicSizeInMbs, ErrorDetection);

	return ErrorDetection;
}






/**
This function permits to decode the VLC for an B image. 
All the parameters decoded will be stored in differents structures or tables.
*/
unsigned char B_cabac ( RESIDU *picture_residu, unsigned char *data,int *NalBytesInNalunit, SPS *sps, PPS *pps, DATA *block, 
					   SLICE *slice, int *aio_piPosition, unsigned char *SliceTable, unsigned char *MbToSliceGroupMap, short *mv_cabac_l0, 
					   short *mv_cabac_l1, short *ref_cabac_l0, short *ref_cabac_l1, int *EndOfSlice, LIST_MMO * ai_pRefPicListL1, 
					   short *mvl0_io, short *mvl1_io, short *refl0_io, short *refl1_io, unsigned char long_term, 
					   unsigned char slice_type, short *mvl1_l0, short *mvl1_l1, short *refl1_l0, short *refl1_l1)
{

	//Initialization of the image parameters
	__declspec(align(64)) unsigned char non_zero_count_cache [48];
	__declspec(align(64)) short mv_cache_l0[48][2];
	__declspec(align(64)) short mv_cache_l1[48][2];
	__declspec(align(64)) short ref_cache_l0[48];
	__declspec(align(64)) short ref_cache_l1[48];
	__declspec(align(64)) short mvl0_cache[48][2];
	__declspec(align(64)) short mvl1_cache[48][2];
	__declspec(align(64)) short refl0_cache[48];
	__declspec(align(64)) short refl1_cache[48];

	//Initialization of the image parameters
	CABAC_NEIGH param_neigh[1];
	CABACContext cabac[1];

	volatile int moreDataFlag = 1 ;
	int mb_skip_flag = 0 ;
	int diff_qp = 0;
	int slice_QP = slice -> slice_qp_delta ;


	const short PicSizeInMbs = sps -> PicSizeInMbs ;
	const short PicWidthInMbs = sps -> pic_width_in_mbs;
	const short PicHeightInMbs = sps -> pic_height_in_map_units;
	short iCurrMbAddr = slice -> first_mb_in_slice;
	short iMb_x;
	short iMb_y;

	unsigned char Last_Qp = slice -> slice_qp_delta;
	unsigned char ErrorDetection = 0;

	//initialize macroblock position
	GetMbPosition(iCurrMbAddr, PicWidthInMbs, &iMb_x, &iMb_y);

	//CABAC
	CabacDecoderInitAVC(cabac, data, NalBytesInNalunit, *aio_piPosition, slice_QP, &cabac_context_init_PB[slice -> cabac_init_idc][0]);

	//Decoding process macroblock per macroblock of one slice
	do {
		init_block(&block[iCurrMbAddr]);
		init_residu(&picture_residu[iCurrMbAddr]);
		picture_residu[iCurrMbAddr] . SliceNum = SliceTable [iCurrMbAddr] = slice -> slice_num ;
		GetNeighbour(iCurrMbAddr, iMb_x, PicWidthInMbs, PicSizeInMbs, SliceTable, picture_residu, 0);

		//Detection of a skipped macroblock
		mb_skip_flag = decode_cabac_mb_skip( cabac, cabac_state, picture_residu[iCurrMbAddr] . AvailMask, 
			SliceTable, picture_residu, slice -> slice_type, iCurrMbAddr, PicWidthInMbs);

		if (mb_skip_flag){
			//Updating the skipped macroblock parameters
			init_B_skip_mb(&picture_residu[iCurrMbAddr], Last_Qp, slice);

			//For CABAC decoding process
			diff_qp = 0;

			init_mb_skip_mv_ref(&mv_cabac_l0[iMb_x * 8 + iMb_y * (PicWidthInMbs << 5)]	, 
				&ref_cabac_l0[iMb_x * 2 + iMb_y * (PicWidthInMbs << 2)], PicWidthInMbs << 3, PicWidthInMbs << 1);
			init_mb_skip_mv_ref(&mv_cabac_l1[iMb_x * 8 + iMb_y * (PicWidthInMbs << 5)]	, 
				&ref_cabac_l1[iMb_x * 2 + iMb_y * (PicWidthInMbs << 2)], PicWidthInMbs << 3, PicWidthInMbs << 1);
			init_skip_non_zero_count(iMb_x, iMb_y, slice -> mb_stride, PicSizeInMbs, block);


			//For motion vector decoding process
			init_ref_cache(refl0_cache);
			init_ref_cache(refl1_cache);
			init_mv_cache(mvl0_cache);
			init_mv_cache(mvl1_cache);


			fill_caches_motion_vector_B_full_ref(slice, slice -> b_stride, slice -> b8_stride, 
				refl0_cache, refl1_cache, mvl0_cache, mvl1_cache, SliceTable , picture_residu, 
				&mvl0_io [(iMb_x << 3) + iMb_y * (slice -> b_stride << 2)], 
				&mvl1_io [(iMb_x << 3) + iMb_y * (slice -> b_stride << 2)], 
				&refl0_io[(iMb_x << 2) + iMb_y * (slice -> b_stride << 1)], 
				&refl1_io[(iMb_x << 2) + iMb_y * (slice -> b_stride << 1)], iCurrMbAddr, iMb_x);


			motion_vector_B_skip(mvl0_cache, mvl1_cache, refl0_cache, refl1_cache, 
				&mvl1_l0[(iMb_x << 3) + iMb_y * (slice -> b_stride << 2)], 
				&mvl1_l1[(iMb_x << 3) + iMb_y * (slice -> b_stride << 2)], 
				&refl1_l0[(iMb_x << 2) + iMb_y * (slice -> b_stride << 1)], 
				&refl1_l1[(iMb_x << 2) + iMb_y * (slice -> b_stride << 1)], 
				slice, sps -> direct_8x8_inference_flag, ai_pRefPicListL1 -> long_term, ai_pRefPicListL1 -> slice_type, 1);


			//Recording of the motion vector
			write_back_motion_full_ref( slice -> b_stride, slice -> b8_stride, 
				&mvl0_io[(iMb_x << 3) + iMb_y * (slice -> b_stride << 2)], mvl0_cache, 
				&refl0_io[(iMb_x << 2) + iMb_y * (slice -> b_stride << 1)], refl0_cache);
			write_back_motion_full_ref( slice -> b_stride, slice -> b8_stride, 
				&mvl1_io[(iMb_x << 3) + iMb_y * (slice -> b_stride << 2)], mvl1_cache, 
				&refl1_io[(iMb_x << 2) + iMb_y * (slice -> b_stride << 1)], refl1_cache);

		} else {
			//In case of there is more data in the NAL
			init_ref_cache(ref_cache_l0);
			init_ref_cache(ref_cache_l1);
			init_ref_cache(refl0_cache);
			init_ref_cache(refl1_cache);
			init_mv_cache( mv_cache_l0);
			init_mv_cache( mv_cache_l1);
			init_mv_cache( mvl0_cache);
			init_mv_cache( mvl1_cache);


			fill_caches_motion_vector_B( slice, PicWidthInMbs << 3, PicWidthInMbs << 1, 
				ref_cache_l0, ref_cache_l1, mv_cache_l0, mv_cache_l1, SliceTable, &picture_residu[iCurrMbAddr], 
				&mv_cabac_l0[iMb_x * 8 + iMb_y * (PicWidthInMbs << 5)], &mv_cabac_l1[iMb_x * 8 + iMb_y * (PicWidthInMbs << 5)], 
				&ref_cabac_l0[iMb_x * 2 + iMb_y * (PicWidthInMbs << 2)], &ref_cabac_l1[iMb_x * 2 + iMb_y * (PicWidthInMbs << 2)], 
				iCurrMbAddr,iMb_x);



			fill_caches_motion_vector_B_full_ref(slice, slice -> b_stride, slice -> b8_stride, 
				refl0_cache, refl1_cache, mvl0_cache, mvl1_cache, SliceTable , picture_residu,
				&mvl0_io [(iMb_x << 3) + iMb_y * (slice -> b_stride << 2)],
				&mvl1_io [(iMb_x << 3) + iMb_y * (slice -> b_stride << 2)],
				&refl0_io[(iMb_x << 2) + iMb_y * (slice -> b_stride << 1)],
				&refl1_io[(iMb_x << 2) + iMb_y * (slice -> b_stride << 1)], iCurrMbAddr, iMb_x);



			//Decoding process of the current macroblock
			ErrorDetection = macroblock_layer_B_cabac(&picture_residu[iCurrMbAddr], slice, block, non_zero_count_cache, 
				SliceTable, iCurrMbAddr, cabac, cabac_state, param_neigh, mv_cache_l0, mv_cache_l1, ref_cache_l0, 
				ref_cache_l1, iMb_x, iMb_y, pps -> constrained_intra_pred_flag , sps -> direct_8x8_inference_flag, 
				pps -> transform_8x8_mode_flag, &Last_Qp, &diff_qp, 1, mvl0_cache, mvl1_cache, refl0_cache, 
				refl1_cache, long_term, slice_type, &mvl1_l0[(iMb_x << 3) + iMb_y * (slice -> b_stride << 2)], 
				&mvl1_l1[(iMb_x << 3) + iMb_y * (slice -> b_stride << 2)], &refl1_l0[(iMb_x << 2) + iMb_y * (slice -> b_stride << 1)], 
				&refl1_l1[(iMb_x << 2) + iMb_y * (slice -> b_stride << 1)]);


			//Recording of the neighbourhood for the decoding of the VLC 
			NonZeroCountManagement(non_zero_count_cache, iMb_x, iMb_y, PicWidthInMbs, PicHeightInMbs, &block[iCurrMbAddr], &picture_residu[iCurrMbAddr]);

			write_back_motion(PicWidthInMbs << 3, PicWidthInMbs << 1, &mv_cabac_l0[iMb_x * 8 + iMb_y * (PicWidthInMbs << 5)], 
				mv_cache_l0, &ref_cabac_l0[iMb_x * 2 + iMb_y * (PicWidthInMbs << 2)], ref_cache_l0);
			write_back_motion(PicWidthInMbs << 3, PicWidthInMbs << 1, &mv_cabac_l1[iMb_x * 8 + iMb_y * (PicWidthInMbs << 5)], 
				mv_cache_l1, &ref_cabac_l1[iMb_x * 2 + iMb_y * (PicWidthInMbs << 2)], ref_cache_l1);

			//Recording of the motion vector
			write_back_motion_full_ref( slice -> b_stride, slice -> b8_stride,
				&mvl0_io[(iMb_x << 3) + iMb_y * (slice -> b_stride << 2)], mvl0_cache,
				&refl0_io[(iMb_x << 2) + iMb_y * (slice -> b_stride << 1)], refl0_cache );
			write_back_motion_full_ref( slice -> b_stride, slice -> b8_stride,
				&mvl1_io[(iMb_x << 3) + iMb_y * (slice -> b_stride << 2)], mvl1_cache,
				&refl1_io[(iMb_x << 2) + iMb_y * (slice -> b_stride << 1)], refl1_cache );
		}

		slice -> mb_xy++;
		//Updating the macroblock adress
		iCurrMbAddr = Next_MbAddress(iCurrMbAddr, PicSizeInMbs, PicWidthInMbs, MbToSliceGroupMap, &iMb_x, &iMb_y);
		moreDataFlag = !get_cabac_terminate(cabac, slice -> mb_xy, PicSizeInMbs);


	} while ( moreDataFlag && !ErrorDetection);
	//To set endofslice according to the error detection.
	*EndOfSlice = EndOfSliceManagement(slice, PicSizeInMbs, ErrorDetection);

	return ErrorDetection;
}




/**
This function decodes the VLC of the base layer.
*/
void slice_base_layer_cabac (unsigned char *data, int *NalBytesInNalunit, int *aio_piPosition, SPS *sps, PPS *pps, 
							 unsigned char *ai_slice_group_id, LIST_MMO *Current_pic, LIST_MMO *ai_pRefPicListL1, NAL *Nal, 
							 short *mv_cabac_l0, short *mv_cabac_l1, short *ref_cabac_l0, short *ref_cabac_l1, 
							 SLICE *slice, unsigned char *MbToSliceGroupMap, unsigned char *SliceTable, DATA *Tab_block, RESIDU *picture_residu, 
							 int *end_of_slice, short *mvl0_io, short *mvl1_io, short *refl0_io, short *refl1_io) 
{



	if (!Nal -> NalToSkip){
		int pos = *aio_piPosition;

		//Parameters initialization Initialisation in case of a new picture
		if ( *end_of_slice == 0 ) {
			reinit_slice(slice);
			slice -> mb_stride = sps -> pic_width_in_mbs;
			slice -> b_stride = sps -> b_stride;
			slice -> b8_stride = sps -> b8_stride;
			mapUnitToSlice(sps, pps, MbToSliceGroupMap, slice -> slice_group_change_cycle, ai_slice_group_id);
			InitUcharTab(sps -> PicSizeInMbs, SliceTable);
		}else{
			SliceManagement(pps, slice);
		}



		//CABAC decoding process
		if(( pos & 7)!= 0)  {
			pos =((pos >> 3) + 1) << 3;
		}

		if ( slice -> slice_type == SLICE_TYPE_B){
			Nal -> ErrorDetection = B_cabac (picture_residu, data, NalBytesInNalunit, sps, pps, Tab_block, slice, &pos, SliceTable, 
				MbToSliceGroupMap, mv_cabac_l0, mv_cabac_l1, ref_cabac_l0, ref_cabac_l1, end_of_slice, ai_pRefPicListL1, 
				&mvl0_io[Current_pic -> MvMemoryAddress], &mvl1_io[Current_pic -> MvMemoryAddress], 
				&refl0_io[Current_pic -> MvMemoryAddress >> 1], &refl1_io[Current_pic -> MvMemoryAddress >> 1], 
				ai_pRefPicListL1[0] . long_term, ai_pRefPicListL1[0] . slice_type, &mvl0_io[ai_pRefPicListL1[0] . MvMemoryAddress], 
				&mvl1_io[ai_pRefPicListL1[0] . MvMemoryAddress], &refl0_io[ai_pRefPicListL1[0] . MvMemoryAddress >> 1], 
				&refl1_io[ai_pRefPicListL1[0] . MvMemoryAddress >> 1]);

		}else if ( slice -> slice_type == SLICE_TYPE_P){
			Nal -> ErrorDetection = P_cabac(picture_residu, data, NalBytesInNalunit, sps, pps, Tab_block, slice, &pos,  
				SliceTable, MbToSliceGroupMap, mv_cabac_l0, ref_cabac_l0, end_of_slice, 
				&mvl0_io[Current_pic -> MvMemoryAddress], &refl0_io[Current_pic -> MvMemoryAddress >> 1]);
		}else {
			Nal -> ErrorDetection = slice_data_I_cabac (picture_residu, data, NalBytesInNalunit, sps, pps, Tab_block, slice, 
				&pos, SliceTable, MbToSliceGroupMap, end_of_slice);
		}
	}

#ifdef ERROR_CONCEALMENT
	//Error concealment
	AVCErrorConcealment(Nal, sps, slice, SliceTable, picture_residu, Tab_block,
		&mvl0_io[Current_pic -> MvMemoryAddress], &mvl1_io[Current_pic -> MvMemoryAddress], 
		&refl0_io[Current_pic -> MvMemoryAddress >> 1], &refl1_io[Current_pic -> MvMemoryAddress >> 1]);
#endif
}


