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

//AVC files
#include "type.h"
#include "data.h"
#include "cavlc.h"
#include "cabac.h"
#include "main_data.h"
#include "init_data.h"
#include "vlc_cabac.h"
#include "bitstream.h"
#include "write_back.h"
#include "cabac_table.h"
#include "slice_header.h"
#include "quantization.h"
#include "SliceGroupMap.h"
#include "neighbourhood.h"
#include "slice_data_vlc.h"
#include "fill_caches_cabac.h"



//SVC files
#include "svc_data.h"
#include "blk4x4.h"
#include "cavlc_svc.h"
#include "cabac_svc.h"
#include "Coordonates.h"
#include "slice_data_svc.h"



/**
This function decodes the stream using CAVLC encotry decoding.
*/
char slice_data_cavlc_svc(const NAL *Nal, RESIDU *Current_residu, RESIDU *BaseResidu, const unsigned char *ai_pcData,  
						 const int *ai_piNalBytesInNalunit, const SPS *Sps, const PPS *Pps, DATA *aio_tstBlock, 
						 SLICE *Slice, int *position, const VLC_TABLES *vlc,  unsigned char *SliceTable, 
						 const  unsigned char *MbToSliceGroupMap, int *EndOfSlice)
{	



	//Initialization of the image parameters
	volatile int moreDataFlag = 1;
	int	i;
	const short PicSizeInMbs = Sps -> PicSizeInMbs;
	const short PicWidthInMbs = Sps -> pic_width_in_mbs;
	const short PicHeightInMbs = Sps -> pic_height_in_map_units;
	short iCurrMbAddr = Slice -> first_mb_in_slice;
	unsigned char Last_Qp = Slice -> slice_qp_delta;
	char ErrorDetection = 0;


	__declspec(align(64))  unsigned char NonZeroCountCache [48];

	short iMb_x;
	short iMb_y;

	//initialize macroblock position
	GetMbPosition(iCurrMbAddr, PicWidthInMbs, &iMb_x, &iMb_y);


	//Decoding process macroblock per macroblock of one slice
	do {

		if( Slice -> slice_type  !=  EI ){
			//Detection of a skipped macroblock
			short mb_skip_run = (short) read_ue(ai_pcData, position);

			if (mb_skip_run){
				for ( i = 0 ; i < mb_skip_run && iCurrMbAddr < PicSizeInMbs; i++ ){
					//Updating the skipped macroblock parameters
					SliceTable [iCurrMbAddr] = Slice -> slice_num ;
					init_block(&aio_tstBlock[iCurrMbAddr]);
					init_residu(&Current_residu[iCurrMbAddr]);
					if ( Slice -> slice_type == SLICE_TYPE_P){
						init_P_skip_mb(&Current_residu[iCurrMbAddr], Last_Qp, Slice);
					}else{
						init_B_skip_mb(&Current_residu[iCurrMbAddr], Last_Qp, Slice);
						Current_residu[iCurrMbAddr] . Mode = 4;
					}
					Current_residu[iCurrMbAddr] . InCropWindow = is_crop_in_window(Sps, iMb_x, iMb_y);
					//Initialize base macroblock address
					GetBaseMbAddr(Nal, &aio_tstBlock[iCurrMbAddr], iMb_x << 4, iMb_y << 4);

					init_mb_skip(NonZeroCountCache);
					write_back_non_zero_count(NonZeroCountCache, iMb_x, iMb_y, PicWidthInMbs, PicSizeInMbs, &aio_tstBlock[iCurrMbAddr]);

					//Updating of the macroblock adress
					iCurrMbAddr = Next_MbAddress(iCurrMbAddr, PicSizeInMbs, PicWidthInMbs, MbToSliceGroupMap, &iMb_x, &iMb_y);
					Slice -> mb_xy ++; 
				}

				//Test if there is more information in the NAL
				moreDataFlag = MoreRbsp(ai_pcData, position, ai_piNalBytesInNalunit, iCurrMbAddr, PicSizeInMbs, &ErrorDetection);
			}
		}


		//In case of there is more data in the NAL
		if ( moreDataFlag ) {
			init_block(&aio_tstBlock[iCurrMbAddr]);
			init_residu(&Current_residu[iCurrMbAddr]);
			Current_residu[iCurrMbAddr] . InCropWindow = is_crop_in_window(Sps, iMb_x, iMb_y);

			//Decoding process of the current macroblock
			ErrorDetection = macroblock_layer_in_scalable_extension(Nal, Pps, &Current_residu[iCurrMbAddr], BaseResidu, ai_pcData, 
				position, Slice, aio_tstBlock, vlc, NonZeroCountCache, SliceTable, iMb_x, iMb_y, 
				Sps -> direct_8x8_inference_flag, &Last_Qp);

			//Recording of the neighbourhood for the decoding of the VLC 
			NonZeroCountManagement(NonZeroCountCache, iMb_x, iMb_y, PicWidthInMbs, PicHeightInMbs, &aio_tstBlock[iCurrMbAddr], &Current_residu[iCurrMbAddr]);

			//Updating of the macroblock parameters
			Slice -> mb_xy++;
			moreDataFlag = MoreRbsp(ai_pcData, position, ai_piNalBytesInNalunit, iCurrMbAddr, PicSizeInMbs, &ErrorDetection);

			//Updating of the macroblock adress
			iCurrMbAddr = Next_MbAddress(iCurrMbAddr, PicSizeInMbs, PicWidthInMbs, MbToSliceGroupMap, &iMb_x, &iMb_y);
		}
	} while ( moreDataFlag);
	//To set endofslice according to the error detection.
	*EndOfSlice = EndOfSliceManagement(Slice, PicSizeInMbs, ErrorDetection);

	return ErrorDetection;
}



/**
This function initializes tje CABAC context.
*/
void CabacDecoderInitSVC(CABACContext *cabac, unsigned char *Data, const int *NalBytesInNalunit, int Pos, int slice_QP, int CabacContext[][2]){

	int i;

	//CABAC
	init_cabac_states(cabac, lps_range,	mps_state, lps_state, 64);    
	init_cabac_decoder(cabac, Data +(Pos >> 3), *NalBytesInNalunit - ((Pos + 7) >> 3));
	for( i = 0; i < 467; i++ ) {
		int pre = ((CabacContext[i][0] * slice_QP) >> 4) + CabacContext[i][1];
		pre = CLIP3(1, 126, pre);       

		if( pre <= 63 ){
			cabac_state[i] = (unsigned char) (( 63 - pre ) << 1);
		} else{
			cabac_state[i] = (unsigned char) ((( pre - 64 ) << 1) + 1);
		}
	}
}






/**
This function decodes the stream using CABAC encotry decoding for I slices.
*/
char SliceDataICabacSvc(unsigned char *Data,  const int *NalBytesInNalunit, int Pos, const NAL *Nal, const SPS *Sps, 
					   const PPS *Pps, SLICE *Slice, RESIDU *CurrResidu, RESIDU *BaseResidu, DATA *Block, 
					   unsigned char *SliceTable, unsigned char *MbToSliceGroupMap, int *EndOfSlice)
{	



	//Initialization of the image parameters
	__declspec(align(64)) unsigned char NonZeroCountCache [48];

	CABAC_NEIGH param_neigh[1];
	CABACContext cabac[1];
	volatile int moreDataFlag = 1 ;
	int diff_qp = 0;

	const short PicSizeInMbs = Sps -> PicSizeInMbs ;
	const short PicWidthInMbs = Sps -> pic_width_in_mbs;
	const short PicHeightInMbs = Sps -> pic_height_in_map_units;
	const short slice_QP = Slice -> slice_qp_delta ;
	short iCurrMbAddr = Slice -> first_mb_in_slice;

	short iMb_x;
	short iMb_y;
	char ErrorDetection = 0;
	unsigned char Last_Qp = Slice -> slice_qp_delta;

	//initialize macroblock position
	GetMbPosition(iCurrMbAddr, PicWidthInMbs, &iMb_x, &iMb_y);

	//CABAC
	CabacDecoderInitSVC(cabac, Data, NalBytesInNalunit, Pos, slice_QP, cabac_context_init_I);


	//Decoding process macroblock per macroblock of one slice
	do {
		init_block(&Block[iCurrMbAddr]);
		init_residu(&CurrResidu[iCurrMbAddr]);
		CurrResidu[iCurrMbAddr] . SliceNum = SliceTable [iCurrMbAddr] = Slice -> slice_num ;
		GetNeighbour(iCurrMbAddr, iMb_x, PicWidthInMbs, PicSizeInMbs, SliceTable, CurrResidu, 0);
		CurrResidu[iCurrMbAddr] . InCropWindow = is_crop_in_window(Sps, iMb_x, iMb_y);

		ErrorDetection = CabacIMacroblockLayer(cabac, cabac_state, Nal, Slice, CurrResidu, BaseResidu, Block, param_neigh, SliceTable, NonZeroCountCache,
			iMb_x, iMb_y, Pps -> constrained_intra_pred_flag, Pps -> transform_8x8_mode_flag, &Last_Qp, &diff_qp);

		//Recording of the neighbourhood for the decoding of the VLC 
		NonZeroCountManagement(NonZeroCountCache, iMb_x, iMb_y, PicWidthInMbs, PicHeightInMbs, &Block[iCurrMbAddr], &CurrResidu[iCurrMbAddr]);

		//Updating of the macroblock parameters
		Slice -> mb_xy++;
		moreDataFlag = !get_cabac_terminate(cabac, Slice -> mb_xy, PicSizeInMbs);

		//Updating of the macroblock adress
		iCurrMbAddr = Next_MbAddress(iCurrMbAddr, PicSizeInMbs, PicWidthInMbs, MbToSliceGroupMap, &iMb_x, &iMb_y);

	} while ( moreDataFlag && !ErrorDetection);
	//To set endofslice according to the error detection.
	*EndOfSlice = EndOfSliceManagement(Slice, PicSizeInMbs, ErrorDetection);
	return ErrorDetection;
}





/**
This function decodes the stream using CABAC encotry decoding for P slices.
*/
char SliceDataPCabacSvc(unsigned char *Data,  const int *NalBytesInNalunit, int Pos, const NAL *Nal, const SPS *Sps, 
					   const PPS *Pps, SLICE *Slice, RESIDU *CurrResidu, RESIDU *BaseResidu, DATA *Block, 
					   unsigned char *SliceTable, unsigned char *MbToSliceGroupMap, int *EndOfSlice, short *MvCabacL0, short *RefCabacL0)
{	

	//Initialization of the image parameters	
	__declspec(align(64)) unsigned char NonZeroCountCache [48];
	__declspec(align(64)) short mv_cache_l0[48][2];
	__declspec(align(64)) short ref_cache_l0[48];


	CABAC_NEIGH Neigh[1];
	CABACContext cabac[1];
	volatile int moreDataFlag = 1 ;
	int diff_qp = 0;
	int MbSkipFlag;

	const short PicSizeInMbs = Sps -> PicSizeInMbs ;
	const short PicWidthInMbs = Sps -> pic_width_in_mbs;
	const short PicHeightInMbs = Sps -> pic_height_in_map_units;
	const short slice_QP = Slice -> slice_qp_delta ;

	short iCurrMbAddr = Slice -> first_mb_in_slice;

	short iMb_x;
	short iMb_y;
	char ErrorDetection = 0;
	unsigned char Last_Qp = Slice -> slice_qp_delta;


	//initialize macroblock position
	GetMbPosition(iCurrMbAddr, PicWidthInMbs, &iMb_x, &iMb_y);


	//CABAC
	CabacDecoderInitSVC(cabac, Data, NalBytesInNalunit, Pos, slice_QP, &cabac_context_init_PB[Slice -> cabac_init_idc][0]);



	//Decoding process macroblock per macroblock of one slice
	do {
		init_block(&Block[iCurrMbAddr]);
		init_residu(&CurrResidu[iCurrMbAddr]);
		SliceTable [iCurrMbAddr] = Slice -> slice_num ;
		GetNeighbour(iCurrMbAddr, iMb_x, PicWidthInMbs, PicSizeInMbs, SliceTable, CurrResidu, 0);
		CurrResidu[iCurrMbAddr] . InCropWindow = is_crop_in_window(Sps, iMb_x, iMb_y);


		//Detection of a skipped macroblock
		MbSkipFlag = decode_cabac_mb_skip( cabac, cabac_state, CurrResidu[iCurrMbAddr] . AvailMask, SliceTable, 
			CurrResidu, Slice -> slice_type, iCurrMbAddr, PicWidthInMbs);
		if (MbSkipFlag){
			diff_qp = 0;
			//Updating the skipped macroblock parameters
			SliceTable [iCurrMbAddr] = Slice -> slice_num ;
			init_P_skip_mb(&CurrResidu[iCurrMbAddr], Last_Qp,  Slice);

			init_mb_skip_mv_ref(&MvCabacL0[iMb_x * 8 + iMb_y * (PicWidthInMbs << 5)], 
				&RefCabacL0[iMb_x * 2 + iMb_y * (PicWidthInMbs << 2)], PicWidthInMbs << 3, PicWidthInMbs << 1);
			init_skip_non_zero_count(iMb_x, iMb_y, Slice -> mb_stride, PicSizeInMbs, Block );	
			//Initialize base macroblock address
			GetBaseMbAddr(Nal, &Block[iCurrMbAddr], iMb_x << 4, iMb_y << 4);
		}
		else{
			//In case of there is more data in the NAL
			init_ref_cache(ref_cache_l0);
			init_mv_cache(mv_cache_l0);

			fill_caches_motion_vector( Slice, PicWidthInMbs << 3, PicWidthInMbs << 1, ref_cache_l0, mv_cache_l0, 
				SliceTable, &CurrResidu[iCurrMbAddr], &MvCabacL0[iMb_x * 8 + iMb_y * (PicWidthInMbs << 5)], 
				&RefCabacL0[iMb_x * 2 + iMb_y * (PicWidthInMbs << 2)], iMb_x, iMb_y);

			//Decoding process of the current macroblock
			ErrorDetection = CabacPMacroblockLayer(cabac, cabac_state, Nal, Slice, CurrResidu, BaseResidu, Block, Neigh, 
				SliceTable, NonZeroCountCache, mv_cache_l0, ref_cache_l0, iMb_x, iMb_y, 
				Pps -> constrained_intra_pred_flag, Pps -> transform_8x8_mode_flag, &Last_Qp, &diff_qp);

			//Recording of the neighbourhood for the decoding of the VLC 
			NonZeroCountManagement(NonZeroCountCache, iMb_x, iMb_y, PicWidthInMbs, PicHeightInMbs, &Block[iCurrMbAddr], &CurrResidu[iCurrMbAddr]);

			write_back_motion(PicWidthInMbs << 3, PicWidthInMbs << 1, 
				&MvCabacL0[iMb_x * 8 + iMb_y * (PicWidthInMbs << 5)], mv_cache_l0, 
				&RefCabacL0[iMb_x * 2 + iMb_y * (PicWidthInMbs << 2)], ref_cache_l0);			
		}
		//Save the last macroblock address
		Slice -> mb_xy++;

		//Updating the macroblock adress
		iCurrMbAddr = Next_MbAddress(iCurrMbAddr, PicSizeInMbs, PicWidthInMbs, MbToSliceGroupMap, &iMb_x, &iMb_y);
		moreDataFlag = !get_cabac_terminate(cabac, Slice -> mb_xy, PicSizeInMbs);

	} while ( moreDataFlag && !ErrorDetection);
	//To set endofslice according to the error detection.
	*EndOfSlice = EndOfSliceManagement(Slice, PicSizeInMbs, ErrorDetection);
	return ErrorDetection;
}





/**
This function decodes the stream using CABAC encotry decoding for B slices.
*/
char SliceDataBCabacSvc(unsigned char *Data,  const int *NalBytesInNalunit, int Pos, const NAL *Nal, const SPS *Sps, 
					   const PPS *Pps, SLICE *Slice, RESIDU *CurrResidu, RESIDU *BaseResidu, DATA *Block, 
					   unsigned char *SliceTable, unsigned char *MbToSliceGroupMap, int *EndOfSlice, short *MvCabacL0, 
					   short *RefCabacL0, short *MvCabacL1, short *RefCabacL1)
{	

	//Initialization of the image parameters	
	__declspec(align(64)) unsigned char NonZeroCountCache [48];
	__declspec(align(64)) short mv_cache_l0[48][2];
	__declspec(align(64)) short mv_cache_l1[48][2];
	__declspec(align(64)) short ref_cache_l0[48];
	__declspec(align(64)) short ref_cache_l1[48];


	CABAC_NEIGH Neigh[1];
	CABACContext cabac[1];
	volatile int moreDataFlag = 1 ;
	int diff_qp = 0;
	int MbSkipFlag;

	const short PicSizeInMbs = Sps -> PicSizeInMbs ;
	const short PicWidthInMbs = Sps -> pic_width_in_mbs;
	const short PicHeightInMbs = Sps -> pic_height_in_map_units;
	const short slice_QP = Slice -> slice_qp_delta ;
	short iCurrMbAddr = Slice -> first_mb_in_slice;


	short iMb_x;
	short iMb_y;
	char  ErrorDetection = 0;
	unsigned char Last_Qp = Slice -> slice_qp_delta;


	//initialize macroblock position
	GetMbPosition(iCurrMbAddr, PicWidthInMbs, &iMb_x, &iMb_y);


	//CABAC
	CabacDecoderInitSVC(cabac, Data, NalBytesInNalunit, Pos, slice_QP, &cabac_context_init_PB[Slice -> cabac_init_idc][0]);


	//Decoding process macroblock per macroblock of one slice
	do {	
		init_block(&Block[iCurrMbAddr]);
		init_residu(&CurrResidu[iCurrMbAddr]);
		SliceTable [iCurrMbAddr] = Slice -> slice_num ;
		GetNeighbour(iCurrMbAddr, iMb_x, PicWidthInMbs, PicSizeInMbs, SliceTable, CurrResidu, 0);
		CurrResidu[iCurrMbAddr] . InCropWindow = is_crop_in_window(Sps, iMb_x, iMb_y);

		//Detection of a skipped macroblock
		MbSkipFlag = decode_cabac_mb_skip( cabac, cabac_state, CurrResidu[iCurrMbAddr] . AvailMask, SliceTable, CurrResidu, Slice -> slice_type, iCurrMbAddr, PicWidthInMbs);
		if (MbSkipFlag){
			diff_qp = 0;
			//Updating the skipped macroblock parameters
			SliceTable [iCurrMbAddr] = Slice -> slice_num ;
			init_B_skip_mb(&CurrResidu[iCurrMbAddr], Last_Qp,  Slice);
			CurrResidu[iCurrMbAddr] . Mode = 4;

			//Initialize base macroblock address
			GetBaseMbAddr(Nal, &Block[iCurrMbAddr], iMb_x << 4, iMb_y << 4);

			init_mb_skip_mv_ref(&MvCabacL0[iMb_x * 8 + iMb_y * (PicWidthInMbs << 5)], 
				&RefCabacL0[iMb_x * 2 + iMb_y * (PicWidthInMbs << 2)], PicWidthInMbs << 3, PicWidthInMbs << 1);
			init_mb_skip_mv_ref(&MvCabacL1[iMb_x * 8 + iMb_y * (PicWidthInMbs << 5)], 
				&RefCabacL1[iMb_x * 2 + iMb_y * (PicWidthInMbs << 2)], PicWidthInMbs << 3, PicWidthInMbs << 1);
			init_skip_non_zero_count(iMb_x, iMb_y, Slice -> mb_stride, PicSizeInMbs, Block );			
		}
		else{
			//In case of there is more data in the NAL
			init_ref_cache(ref_cache_l0);
			init_ref_cache(ref_cache_l1);
			init_mv_cache(mv_cache_l0);
			init_mv_cache(mv_cache_l1);

			fill_caches_motion_vector_B( Slice, PicWidthInMbs << 3, PicWidthInMbs << 1, ref_cache_l0, ref_cache_l1,
				mv_cache_l0, mv_cache_l1, SliceTable,  &CurrResidu[iCurrMbAddr], 
				&MvCabacL0[iMb_x * 8 + iMb_y * (PicWidthInMbs << 5)], &MvCabacL1[iMb_x * 8 + iMb_y * (PicWidthInMbs << 5)], 
				&RefCabacL0[iMb_x * 2 + iMb_y * (PicWidthInMbs << 2)], &RefCabacL1[iMb_x * 2 + iMb_y * (PicWidthInMbs << 2)], 
				iCurrMbAddr, iMb_x);


			//Decoding process of the current macroblock
			ErrorDetection = CabacBMacroblockLayer(cabac, cabac_state, Nal, Slice, CurrResidu, BaseResidu, Block, Neigh, SliceTable,
				NonZeroCountCache, mv_cache_l0, ref_cache_l0, mv_cache_l1, ref_cache_l1, iMb_x, iMb_y, 
				Pps -> constrained_intra_pred_flag, Pps -> transform_8x8_mode_flag, Sps -> direct_8x8_inference_flag, &Last_Qp, &diff_qp);

			//Recording of the neighbourhood for the decoding of the VLC 
			NonZeroCountManagement(NonZeroCountCache, iMb_x, iMb_y, PicWidthInMbs, PicHeightInMbs, &Block[iCurrMbAddr], &CurrResidu[iCurrMbAddr]);

			write_back_motion(PicWidthInMbs << 3, PicWidthInMbs << 1, 
				&MvCabacL0[iMb_x * 8 + iMb_y * (PicWidthInMbs << 5)], mv_cache_l0, 
				&RefCabacL0[iMb_x * 2 + iMb_y * (PicWidthInMbs << 2)], ref_cache_l0);

			write_back_motion(PicWidthInMbs << 3, PicWidthInMbs << 1, 
				&MvCabacL1[iMb_x * 8 + iMb_y * (PicWidthInMbs << 5)], mv_cache_l1, 
				&RefCabacL1[iMb_x * 2 + iMb_y * (PicWidthInMbs << 2)], ref_cache_l1);		
		}
		//Save the last macroblock address
		Slice -> mb_xy++;

		//Updating the macroblock adress
		iCurrMbAddr = Next_MbAddress(iCurrMbAddr, PicSizeInMbs, PicWidthInMbs, MbToSliceGroupMap, &iMb_x, &iMb_y);
		moreDataFlag = !get_cabac_terminate(cabac, Slice -> mb_xy, PicSizeInMbs);

	} while ( moreDataFlag && !ErrorDetection);
	//To set endofslice according to the error detection.
	*EndOfSlice = EndOfSliceManagement(Slice, PicSizeInMbs, ErrorDetection);
	return ErrorDetection;
}
