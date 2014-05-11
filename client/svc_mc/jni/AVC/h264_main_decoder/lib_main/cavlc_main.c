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
#include "refill.h"
#include "init_data.h"
#include "main_data.h"
#include "write_back.h"
#include "cavlc_main.h"
#include "mb_pred_main.h"
#include "slice_header.h"
#include "BitstreamSkipB.h"
#include "init_Ptr_Fction.h"
#include "slice_data_cabac.h"
#include "macroblock_layer_main.h"




/**
This function permits to decode the VLC for an B image. 
All the parameters decoded will be stored in differents structures or tables.


@param residu Structure which contains all the residu data of the current macroblock.
@param ai_pcData The NAL unit.
@param ai_piNalBytesInNalunit The sizes of the NAL unit in bytes.
@param Sps The SPS structure.
@param pps The PPS structure.
@param aio_tstBlock Contains all parameters of each macroblock of the current picture.
@param Slice The slice structure contains all information used to decode the next slice
@param aio_piPosition The current aio_piPosition in the NAL.
@param Vlc VLC_TABLES contains all the tables for decoding the vlc.
@param aio_tstNon_zero_count Specifies the coeff_token of each block of the picture.
@param SliceTable Specifies in which Slice belongs each macroblock of the picture.
@param aio_pstSliceGroupMap Specifies in which Slice belongs each macroblock of the picture.
@param ao_pstIntra_pred_mode Contains the prediction mode for each macroblock.
@param ao_piEnd_of_aio_pstSlice Specifies if the current picture is totally decoded.
@param RefListl1 Reference list l1.
@param mvl0_io A table where the motion vector are stocked for each 4x4 block of each macroblock for the prediction l0.
@param mvl1_io A table where the motion vector are stocked for each 4x4 block of each macroblock for the prediction l1.
@param refl0_io A table where the reference is stocked for each 4x4 block of each macroblock for the prediction l0.
@param refl1_io A table where the reference is stocked for each 4x4 block of each macroblock for the prediction l1.
@param mapcolList Table which contains the frame pocs of the reference list of the first frame in l1.
@param mvl1_l0 A table of motion vectors of each 4x4 block of each macroblock of the first frame in l1.
@param mvl1_l1 A table of motion vectors of each 4x4 block of each macroblock of the first frame in l1.
@param refl1_l0 A table of reference of each 4x4 block of each macroblock of the first frame in l1.
@param refl1_l1 A table of reference of each 4x4 block of each macroblock of the first frame in l1.
*/
void slice_data_B_cavlc ( RESIDU *picture_residu, const unsigned char *ai_pcData, const int *ai_piNalBytesInNalunit, 
						 const SPS *Sps, const PPS *Pps,DATA *aio_tstBlock, SLICE *Slice, 
						 int *aio_piPosition,  const VLC_TABLES *Vlc, unsigned char *SliceTable,  
						 const unsigned char *ai_tiMbToSliceGroupMap, int *EndOfSlice, LIST_MMO *RefListl1, 
						 short *mvl0_io, short *mvl1_io, short *refl0_io, short *refl1_io, 
						 short *mvl1_l0, short *mvl1_l1, short *refl1_l0, short *refl1_l1)
{

   
	//Initialization of the image parameters
	volatile int  moreDataFlag = 1;
	char ErrorDetection = 0;


	__declspec(align(64)) short ref_cache_l0 [48];
	__declspec(align(64)) short ref_cache_l1 [48];
    __declspec(align(64)) short mv_cache_l0 [48][2];
    __declspec(align(64)) short mv_cache_l1 [48][2];
	__declspec(align(64)) unsigned char NonZeroCountCache [48];



	const short PicSizeInMbs = Sps -> PicSizeInMbs ;
	const short PicWidthInMbs = Sps -> pic_width_in_mbs;
	const short PicHeightInMbs = Sps -> pic_height_in_map_units;
	short i;
	short iCurrMbAddr = Slice -> first_mb_in_slice;

	short iMb_x;
	short iMb_y;
	
	unsigned char Last_Qp = Slice -> slice_qp_delta;

	//initialize macroblock position
	GetMbPosition(iCurrMbAddr, PicWidthInMbs, &iMb_x, &iMb_y);


	//Decoding process macroblock per macroblock of one slice
    do 
    {

		//Detection of a skipped macroblock
		short mb_skip_run = (short) read_ue(ai_pcData, aio_piPosition);
		if (mb_skip_run){
			for ( i = 0 ; i < mb_skip_run && iCurrMbAddr < PicSizeInMbs; i++ ) {		
				//Updating the skipped macroblock parameters
				init_block(&aio_tstBlock[iCurrMbAddr]);
				init_residu(&picture_residu[iCurrMbAddr]);
				SliceTable [iCurrMbAddr] = Slice -> slice_num ;

				SkipB(Slice, Sps, picture_residu, aio_tstBlock, SliceTable, iMb_x, iMb_y, Last_Qp, mv_cache_l0, ref_cache_l0, 
					&mvl0_io [(iMb_x << 3) + iMb_y * (Slice -> b_stride << 2)], &refl0_io[(iMb_x << 1) + iMb_y * (Slice -> b8_stride << 1)]	, 
					mv_cache_l1, ref_cache_l1, 
					&mvl1_io [(iMb_x << 3) + iMb_y * (Slice -> b_stride << 2)], &refl1_io[(iMb_x << 1) + iMb_y * (Slice -> b8_stride << 1)], 
					&mvl1_l0[(iMb_x << 3) + iMb_y * (Slice -> b_stride << 2)], &mvl1_l1[(iMb_x << 3) + iMb_y * (Slice -> b_stride << 2)], 
					&refl1_l0[(iMb_x << 1) + iMb_y * (Slice -> b8_stride << 1)], &refl1_l1[(iMb_x << 1) + iMb_y * (Slice -> b8_stride << 1)], RefListl1);
					
			
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


			fill_caches_motion_vector_B(Slice, Slice -> b_stride, Slice -> b8_stride, 
				ref_cache_l0, ref_cache_l1, mv_cache_l0, mv_cache_l1, 
				SliceTable , &picture_residu [iCurrMbAddr], 
				&mvl0_io [(iMb_x << 3) + iMb_y * (Slice -> b_stride << 2)], 
				&mvl1_io [(iMb_x << 3) + iMb_y * (Slice -> b_stride << 2)], 
				&refl0_io[(iMb_x << 1) + iMb_y * (Slice -> b8_stride << 1)], 
				&refl1_io[(iMb_x << 1) + iMb_y * (Slice -> b8_stride << 1)], iCurrMbAddr, iMb_x);
				
				
			//Decoding process of the current macroblock
            ErrorDetection = macroblock_layer_B(Pps, &picture_residu[iCurrMbAddr], ai_pcData, aio_piPosition, 
				Slice, aio_tstBlock, Vlc, NonZeroCountCache, SliceTable, iMb_x, iMb_y, 
				Sps -> direct_8x8_inference_flag, RefListl1[0] . long_term, RefListl1[0] . slice_type, 
				0, &Last_Qp	, mv_cache_l0, mv_cache_l1, ref_cache_l0, ref_cache_l1,
				&mvl1_l0[(iMb_x << 3) + iMb_y * (Slice -> b_stride << 2)], &mvl1_l1[(iMb_x << 3) + iMb_y * (Slice -> b_stride << 2)], &
				refl1_l0[(iMb_x << 1) + iMb_y * (Slice -> b8_stride << 1)], &refl1_l1[(iMb_x << 1) + iMb_y * (Slice -> b8_stride << 1)]);
			
			//Recording of the neighbourhood for the decoding of the VLC 
			NonZeroCountManagement(NonZeroCountCache, iMb_x, iMb_y, PicWidthInMbs, 
				PicHeightInMbs, &aio_tstBlock[iCurrMbAddr], &picture_residu[iCurrMbAddr]);
			
			//Recording of the motion vector
			write_back_motion( Slice -> b_stride, Slice -> b8_stride, 
				&mvl0_io[(iMb_x << 3) + iMb_y * (Slice -> b_stride << 2)], mv_cache_l0, 
				&refl0_io[(iMb_x << 1) + iMb_y * (Slice -> b8_stride << 1)], ref_cache_l0 );

			write_back_motion( Slice -> b_stride, Slice -> b8_stride, 
				&mvl1_io[(iMb_x << 3) + iMb_y * (Slice -> b_stride << 2)], mv_cache_l1, 
				&refl1_io[(iMb_x << 1) + iMb_y * (Slice -> b8_stride << 1)], ref_cache_l1 );


			//Updating of the macroblock parameters
			Slice -> mb_xy++;
            moreDataFlag = MoreRbsp(ai_pcData, aio_piPosition,  ai_piNalBytesInNalunit, iCurrMbAddr, PicSizeInMbs, &ErrorDetection);
         
			//Updating the macroblock adress
			iCurrMbAddr = Next_MbAddress(iCurrMbAddr, PicSizeInMbs, PicWidthInMbs, ai_tiMbToSliceGroupMap, &iMb_x, &iMb_y);
		}
	} while ( moreDataFlag);
	//To set endofslice according to the error detection.
	*EndOfSlice = EndOfSliceManagement(Slice, PicSizeInMbs, ErrorDetection);
}




/**
This function permits to initialize the function pointer. 

@param aio_pf The structure which contains all function pointer.
*/
void vector_main_init ( MAIN_STRUCT_PF pf [3] )
{
	init_vectors(pf -> baseline_vectors);
		
}



