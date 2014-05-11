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
#include "cavlc.h"
#include "init_data.h"
#include "write_back.h"
#include "mb_pred_main.h"
#include "WriteBackSVC.h"
#include "slice_header.h"
#include "SliceGroupMap.h"
#include "motion_vector.h"
#include "slice_data_vlc.h"
#include "fill_caches_svc.h"
#include "macroblock_layer.h"
#include "fill_caches_cabac.h"
#include "macroblock_layer_main.h"

//SVC Files
#include "ErrorConcealment.h"
#include "nal_unit_header_svc_extension.h"






/**
P VLD using CAVLC.
*/
char P_cavlc(const PPS *pps, RESIDU *picture_residu, const unsigned char *ai_pcData, const int *ai_piNalBytesInNalunit, 
			 const SPS *Sps, DATA *aio_tstBlock, SLICE *Slice, int *aio_piPosition, const VLC_TABLES *Vlc, 
			 unsigned char *SliceTable, const unsigned char *SliceGroupMap, int *EndOfSlice, short *mvl0_io, short *refl0_io) 
{


	int	i;

	//Initialization of the image parameters
	const short PicSizeInMbs = Sps -> PicSizeInMbs;
	const short PicWidthInMbs = Sps -> pic_width_in_mbs;
	const short PicHeightInMbs = Sps -> pic_height_in_map_units;
	volatile int moreDataFlag = 1;

	__declspec(align(64)) unsigned char NonZeroCountCache[48];
	__declspec(align(64)) short    ref_cache_l0 [48];
	__declspec(align(64)) short    mv_cache_l0 [48][2];
	short iCurrMbAddr = Slice -> first_mb_in_slice;

	short iMb_x;
	short iMb_y;
	unsigned char Last_Qp = Slice -> slice_qp_delta;
	char ErrorDetection = 0;



	//initialize macroblock position
	GetMbPosition(iCurrMbAddr, PicWidthInMbs, &iMb_x, &iMb_y);



	//Decoding process macroblock per macroblock of one Slice
	do 
	{
		//Detection of a skipped macroblock
		int mb_skip_run = read_ue(ai_pcData, aio_piPosition);
		if (mb_skip_run){
			for ( i = 0 ; i < mb_skip_run && iCurrMbAddr < PicSizeInMbs; i++ ) {
				//Updating the skipped macroblock parameters
				init_block(&aio_tstBlock[iCurrMbAddr]);
				init_residu(&picture_residu[iCurrMbAddr]);
				SliceTable [iCurrMbAddr] = Slice -> slice_num ;
				init_P_skip_mb(&picture_residu[iCurrMbAddr], Last_Qp, Slice);

				init_skip_non_zero_count(iMb_x, iMb_y,Sps -> pic_width_in_mbs	, PicSizeInMbs, aio_tstBlock);

				//Initialize the cache table
				init_ref_cache(ref_cache_l0);
				init_mv_cache(mv_cache_l0);

				fill_caches_motion_vector_full_ref(Slice, Slice -> b_stride, Slice -> b8_stride, 
					ref_cache_l0, mv_cache_l0, SliceTable , picture_residu, 
					&mvl0_io[(iMb_x << 3) + iMb_y * (Slice -> b_stride << 2)], 
					&refl0_io[(iMb_x << 2) + iMb_y * (Slice -> b_stride << 1)], iMb_x, iMb_y);


				//Calculation and recording of the motion vector
				motion_vector_skip( mv_cache_l0, ref_cache_l0);
				write_back_P_skip_motion_full_ref( Slice -> b_stride, Slice -> b8_stride,
					&mvl0_io [(iMb_x << 3) + iMb_y * (Slice -> b_stride << 2)], mv_cache_l0[12], 
					&refl0_io[(iMb_x << 2) + iMb_y * (Slice -> b_stride << 1)]);

				//Save the last macroblock address
				iCurrMbAddr = Next_MbAddress(iCurrMbAddr, PicSizeInMbs, PicWidthInMbs, SliceGroupMap, &iMb_x, &iMb_y);
				Slice -> mb_xy ++; 
			}

			//Test if there is more information in the NAL
			moreDataFlag = MoreRbsp(ai_pcData, aio_piPosition, ai_piNalBytesInNalunit, iCurrMbAddr, PicSizeInMbs, &ErrorDetection);
		}


		//In case of there is more data in the NAL
		if ( moreDataFlag){		
			init_ref_cache(ref_cache_l0);
			init_mv_cache(mv_cache_l0);
			init_block(&aio_tstBlock[iCurrMbAddr]);
			init_residu(&picture_residu[iCurrMbAddr]);

			fill_caches_motion_vector_full_ref(Slice, Slice -> b_stride, Slice -> b8_stride, 
				ref_cache_l0, mv_cache_l0, SliceTable , picture_residu, 
				&mvl0_io[(iMb_x << 3) + iMb_y * (Slice -> b_stride << 2)], 
				&refl0_io[(iMb_x << 2) + iMb_y * (Slice -> b_stride << 1)], iMb_x, iMb_y);

			//Decoding process of the current macroblock
			ErrorDetection = macroblock_layer_P(pps, &picture_residu[iCurrMbAddr], ai_pcData, aio_piPosition, Slice, 
				aio_tstBlock, Vlc, NonZeroCountCache, SliceTable, iMb_x, iMb_y, &Last_Qp, mv_cache_l0, ref_cache_l0);

			//Recording of the neighbourhood for the decoding of the VLC 
			NonZeroCountManagement(NonZeroCountCache, iMb_x, iMb_y, PicWidthInMbs, PicHeightInMbs, 
				&aio_tstBlock[iCurrMbAddr], &picture_residu[iCurrMbAddr]);

			//Recording of the motion vector
			write_back_motion_full_ref( Slice -> b_stride, Slice -> b8_stride, 
				&mvl0_io [(iMb_x << 3) + iMb_y * (Slice -> b_stride << 2)], mv_cache_l0, 
				&refl0_io[(iMb_x << 2) + iMb_y * (Slice -> b_stride << 1)], ref_cache_l0 );

			//Updating of the macroblock parameters
			Slice -> mb_xy++;
			moreDataFlag = MoreRbsp(ai_pcData, aio_piPosition, ai_piNalBytesInNalunit, iCurrMbAddr, PicSizeInMbs, &ErrorDetection);

			//Updating the macroblock adress
			iCurrMbAddr = Next_MbAddress(iCurrMbAddr, PicSizeInMbs, PicWidthInMbs, SliceGroupMap, &iMb_x, &iMb_y);
		}
	} while ( moreDataFlag);
	//To set endofslice according to the error detection.
	*EndOfSlice = EndOfSliceManagement(Slice, PicSizeInMbs, ErrorDetection);
	return ErrorDetection;
}





/**
B VLD using CAVLC.
*/
char B_cavlc ( const PPS *pps, RESIDU *picture_residu, const unsigned char *ai_pcData,  const int *ai_piNalBytesInNalunit, 
			  const SPS *Sps , DATA *aio_tstBlock, SLICE *Slice, int *aio_piPosition,  const VLC_TABLES *Vlc, 
			  unsigned char *SliceTable, const unsigned char *ai_tiMbToSliceGroupMap , int *EndOfSlice, LIST_MMO *RefListl1, 
			  short *mvl0_io, short *mvl1_io, short *refl0_io, short *refl1_io, short *mvl1_l0, short *mvl1_l1, short *refl1_l0, short *refl1_l1)
{

	//Initialization of the image parameters
	const short PicSizeInMbs = Sps -> PicSizeInMbs ;
	const short PicWidthInMbs = Sps -> pic_width_in_mbs;
	const short PicHeightInMbs = Sps -> pic_height_in_map_units;
	volatile int moreDataFlag = 1 ;

	__declspec(align(64)) unsigned char NonZeroCountCache[48];
	__declspec(align(64)) short    ref_cache_l0 [48];
	__declspec(align(64)) short    ref_cache_l1 [48];
	__declspec(align(64)) short    mv_cache_l0 [48][2];
	__declspec(align(64)) short    mv_cache_l1 [48][2];
	int mb_skip_run;
	short  i;
	short iCurrMbAddr =  Slice -> first_mb_in_slice;

	short iMb_x;
	short iMb_y;
	char ErrorDetection = 0;
	unsigned char Last_Qp = Slice -> slice_qp_delta;

	//initialize macroblock position
	GetMbPosition(iCurrMbAddr, PicWidthInMbs, &iMb_x, &iMb_y);


	//Decoding process macroblock per macroblock of one slice
	do 
	{
		//Detection of a skipped macroblock
		mb_skip_run = read_ue(ai_pcData, aio_piPosition);
		if (mb_skip_run){
			for ( i = 0 ; i < mb_skip_run && iCurrMbAddr < PicSizeInMbs; i++ ) {		

				//Updating the skipped macroblock parameters
				init_block(&aio_tstBlock[iCurrMbAddr]);
				init_residu(&picture_residu[iCurrMbAddr]);
				SliceTable [iCurrMbAddr] = Slice -> slice_num ;
				init_B_skip_mb(&picture_residu[iCurrMbAddr], Last_Qp, Slice);

				init_skip_non_zero_count(iMb_x, iMb_y, Slice -> mb_stride, PicSizeInMbs,aio_tstBlock );

				init_ref_cache(ref_cache_l0);
				init_ref_cache(ref_cache_l1);
				init_mv_cache(mv_cache_l0);
				init_mv_cache(mv_cache_l1);

				fill_caches_motion_vector_B_full_ref(Slice, Slice -> b_stride, Slice -> b8_stride, 
					ref_cache_l0, ref_cache_l1, mv_cache_l0, mv_cache_l1, SliceTable , picture_residu,
					&mvl0_io [(iMb_x << 3) + iMb_y * (Slice -> b_stride << 2)],
					&mvl1_io [(iMb_x << 3) + iMb_y * (Slice -> b_stride << 2)],
					&refl0_io[(iMb_x << 2) + iMb_y * (Slice -> b_stride << 1)],
					&refl1_io[(iMb_x << 2) + iMb_y * (Slice -> b_stride << 1)], iCurrMbAddr, iMb_x);

				motion_vector_B_skip(mv_cache_l0, mv_cache_l1, ref_cache_l0, ref_cache_l1, 
					&mvl1_l0[(iMb_x << 3) + iMb_y * (Slice -> b_stride << 2)], 
					&mvl1_l1[(iMb_x << 3) + iMb_y * (Slice -> b_stride << 2)],
					&refl1_l0[(iMb_x << 2) + iMb_y * (Slice -> b_stride << 1)],
					&refl1_l1[(iMb_x << 2) + iMb_y * (Slice -> b_stride << 1)],
					Slice, Sps -> direct_8x8_inference_flag, RefListl1 -> long_term, RefListl1 -> slice_type, 1);



				//Recording of the motion vector
				write_back_motion_full_ref( Slice -> b_stride, Slice -> b8_stride,
					&mvl0_io[(iMb_x << 3) + iMb_y * (Slice -> b_stride << 2)], mv_cache_l0,
					&refl0_io[(iMb_x << 2) + iMb_y * (Slice -> b_stride << 1)], ref_cache_l0 );
				write_back_motion_full_ref( Slice -> b_stride, Slice -> b8_stride,
					&mvl1_io[(iMb_x << 3) + iMb_y * (Slice -> b_stride << 2)], mv_cache_l1,
					&refl1_io[(iMb_x << 2) + iMb_y * (Slice -> b_stride << 1)], ref_cache_l1 );

				//Save the last macroblock address
				iCurrMbAddr = Next_MbAddress(iCurrMbAddr, PicSizeInMbs, PicWidthInMbs, ai_tiMbToSliceGroupMap, &iMb_x, &iMb_y);
				Slice -> mb_xy ++;
			}

			//Test if there is more information in the NAL
			moreDataFlag = MoreRbsp(ai_pcData, aio_piPosition, ai_piNalBytesInNalunit, iCurrMbAddr, PicSizeInMbs, &ErrorDetection);
		}


		//In case of there is more data in the NAL
		if ( moreDataFlag ) {		
			init_ref_cache(ref_cache_l0);
			init_ref_cache(ref_cache_l1);
			init_mv_cache(mv_cache_l0);
			init_mv_cache(mv_cache_l1);
			init_block(&aio_tstBlock[iCurrMbAddr]);
			init_residu(&picture_residu[iCurrMbAddr]);


			fill_caches_motion_vector_B_full_ref(Slice, Slice -> b_stride, Slice -> b8_stride, 
				ref_cache_l0, ref_cache_l1, mv_cache_l0, mv_cache_l1, SliceTable, picture_residu,
				&mvl0_io [(iMb_x << 3) + iMb_y * (Slice -> b_stride << 2)],
				&mvl1_io [(iMb_x << 3) + iMb_y * (Slice -> b_stride << 2)],
				&refl0_io[(iMb_x << 2) + iMb_y * (Slice -> b_stride << 1)],
				&refl1_io[(iMb_x << 2) + iMb_y * (Slice -> b_stride << 1)], iCurrMbAddr, iMb_x);


			//Decoding process of the current macroblock
			ErrorDetection = macroblock_layer_B(pps, &picture_residu[iCurrMbAddr], ai_pcData, aio_piPosition, Slice, aio_tstBlock, 
				Vlc,  NonZeroCountCache, SliceTable, iMb_x, iMb_y, Sps -> direct_8x8_inference_flag,
				RefListl1[0] . long_term, RefListl1[0] . slice_type, 1, &Last_Qp, mv_cache_l0, mv_cache_l1, ref_cache_l0, ref_cache_l1, 
				&mvl1_l0[(iMb_x << 3) + iMb_y * (Slice -> b_stride << 2)], &mvl1_l1[(iMb_x << 3) + iMb_y * (Slice -> b_stride << 2)], 
				&refl1_l0[(iMb_x << 2) + iMb_y * (Slice -> b_stride << 1)], &refl1_l1[(iMb_x << 2) + iMb_y * (Slice -> b_stride << 1)]);

			//Recording of the neighbourhood for the decoding of the VLC 
			NonZeroCountManagement(NonZeroCountCache, iMb_x, iMb_y, PicWidthInMbs, PicHeightInMbs, 
				&aio_tstBlock[iCurrMbAddr], &picture_residu[iCurrMbAddr]);

			//Recording of the motion vector
			write_back_motion_full_ref( Slice -> b_stride, Slice -> b8_stride, 
				&mvl0_io[(iMb_x << 3) + iMb_y * (Slice -> b_stride << 2)], mv_cache_l0, 
				&refl0_io[(iMb_x << 2) + iMb_y * (Slice -> b_stride << 1)], ref_cache_l0 );

			write_back_motion_full_ref( Slice -> b_stride, Slice -> b8_stride, 
				&mvl1_io[(iMb_x << 3) + iMb_y * (Slice -> b_stride << 2)], mv_cache_l1, 
				&refl1_io[(iMb_x << 2) + iMb_y * (Slice -> b_stride << 1)], ref_cache_l1 );


			//Updating of the macroblock parameters
			Slice -> mb_xy++;
			moreDataFlag = MoreRbsp(ai_pcData, aio_piPosition,  ai_piNalBytesInNalunit, iCurrMbAddr, PicSizeInMbs, &ErrorDetection);

			//Updating the macroblock adress
			iCurrMbAddr = Next_MbAddress(iCurrMbAddr, PicSizeInMbs, PicWidthInMbs, ai_tiMbToSliceGroupMap, &iMb_x, &iMb_y);
		}
	} while ( moreDataFlag);
	//To set endofslice according to the error detection.
	*EndOfSlice = EndOfSliceManagement(Slice, PicSizeInMbs, ErrorDetection);
	return ErrorDetection;
}





void slice_base_layer_cavlc ( unsigned char *ai_pcData, const int *ai_piNalBytesInNalunit, const int *aio_piPosition, const SPS *Sps, 
							 PPS *Pps, VLC_TABLES *Vlc, unsigned char *ai_Slice_group_id, LIST_MMO *Current_pic, LIST_MMO *ai_pRefPicListL1, 
							 NAL *Nal, SLICE *Slice, unsigned char *aio_tiMbToSliceGroupMap, unsigned char *SliceTable, DATA *Block, 
							 RESIDU *picture_residu, int *EndOfSlice, short *mvl0_io, short *mvl1_io, short *refl0_io, short *refl1_io) 
{
	
	
	if (!Nal -> NalToSkip){
		int pos = *aio_piPosition;


		//Parameters initialization Initialisation in case of a new picture
		if ( *EndOfSlice == 0 ) {
			reinit_slice(Slice);
			Slice -> b_stride = Sps -> b_stride;
			Slice -> b8_stride = Sps -> b8_stride;
			Slice -> mb_stride = Sps -> pic_width_in_mbs;
			mapUnitToSlice(Sps, Pps, aio_tiMbToSliceGroupMap, Slice -> slice_group_change_cycle, ai_Slice_group_id);
			InitUcharTab(Sps -> PicSizeInMbs, SliceTable);
		}else{
			SliceManagement(Pps, Slice);
		}



		if(Slice -> slice_type == SLICE_TYPE_B){
			Nal -> ErrorDetection = B_cavlc (Pps, picture_residu, ai_pcData, ai_piNalBytesInNalunit, Sps, Block, 
				Slice, &pos, Vlc, SliceTable, aio_tiMbToSliceGroupMap, EndOfSlice, ai_pRefPicListL1, 
				&mvl0_io[Current_pic -> MvMemoryAddress], &mvl1_io[Current_pic -> MvMemoryAddress], 
				&refl0_io[Current_pic -> MvMemoryAddress >> 1], &refl1_io[Current_pic -> MvMemoryAddress >> 1], 
				&mvl0_io[ai_pRefPicListL1[0] . MvMemoryAddress], &mvl1_io[ai_pRefPicListL1[0] . MvMemoryAddress], 
				&refl0_io[ai_pRefPicListL1[0] . MvMemoryAddress >> 1], &refl1_io[ai_pRefPicListL1[0] . MvMemoryAddress >> 1]);

		}else if(Slice -> slice_type == SLICE_TYPE_P){
			Nal -> ErrorDetection = P_cavlc (Pps, picture_residu, ai_pcData, ai_piNalBytesInNalunit, Sps, Block, 
				Slice, &pos, Vlc, SliceTable, aio_tiMbToSliceGroupMap, EndOfSlice, 
				&mvl0_io[Current_pic -> MvMemoryAddress], &refl0_io[Current_pic -> MvMemoryAddress >> 1]);

		}else if (Slice -> slice_type == SLICE_TYPE_I){
			Nal -> ErrorDetection = slice_data_I_cavlc (picture_residu, ai_pcData, ai_piNalBytesInNalunit, Sps, Pps, Block, 
				Slice, &pos, Vlc, SliceTable, aio_tiMbToSliceGroupMap, EndOfSlice);
		}
	}

#ifdef ERROR_CONCEALMENT
	//Error concealment
	AVCErrorConcealment(Nal, Sps, Slice, SliceTable, picture_residu, Block,
		&mvl0_io[Current_pic -> MvMemoryAddress], &mvl1_io[Current_pic -> MvMemoryAddress], 
		&refl0_io[Current_pic -> MvMemoryAddress >> 1], &refl1_io[Current_pic -> MvMemoryAddress >> 1]);
#endif
}
