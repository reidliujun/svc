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
#include "cavlc.h"
#include "init_data.h"
#include "write_back.h"
#include "BitstreamSkip.h"
#include "SliceGroupMap.h"
#include "slice_data_vlc.h"
#include "macroblock_layer.h"





/**
This function permits to detect the end of the Nal.

@param ai_pcData The NAL unit.
@param aio_piPosition The current aio_piPosition in the NAL.
@param ai_piNalBytesInNalunit The sizes of the NAL unit in bytes.

*/
int more_rbsp_data ( const unsigned char *ai_pcData, const int *aio_piPosition, const int *ai_piNalBytesInNalunit)
{

	
	int byteoffset = *aio_piPosition >> 3 ;


	if ( byteoffset  < *ai_piNalBytesInNalunit - 1 ) {
		return 1 ;
	}else {
		int  cnt = 0 ;
		int localPosition = *aio_piPosition;	
		int bitoffset = 6 - (localPosition & 7);

		// read one bit
		int ctr_bit = showNbits(ai_pcData, localPosition, 1);

		// a stop bit has to be one
		if ( !ctr_bit ) return 1 ;

		localPosition++;

		while (bitoffset >= 0)	{
			ctr_bit = showNbits(ai_pcData, localPosition, 1);   // set up control bit

			if (ctr_bit) cnt++;

			bitoffset--;
			localPosition++;
		}

		return (0 != cnt);
	}
}


/**
Indicates if still more information to decode into NAL.
*/
int MoreRbsp( const unsigned char *ai_pcData, const int *aio_piPosition, const int *ai_piNalBytesInNalunit, 
			 int iCurrMbAddr, int PicSizeInMbs, char *ErrorDetected)
{

	if(!*ErrorDetected){
		int moredataflag = more_rbsp_data (ai_pcData, aio_piPosition, ai_piNalBytesInNalunit);
		if ( iCurrMbAddr >= PicSizeInMbs || ((*aio_piPosition >> 3) > *ai_piNalBytesInNalunit)) {
			if(moredataflag){
				//In case of we have the right number of decoded macroblocks,
				//and there is a lot of data to treat.
				*ErrorDetected = 2;
			}
			return 0 ;
		}

		return moredataflag;
	}
	return 0;
}


/**
This function permits to decode the VLC for an intra image. 
All the parameters decoded will be stored in differents structures or tables.

@param residu Structure which contains all the residu data of the current macroblock.
@param ai_pcData The NAL unit.
@param ai_piNalBytesInNalunit The sizes of the NAL unit in bytes.
@param Sps The sps structure.
@param pps The pps structure.
@param aio_tstBlock Contains all parameters of each macroblock of the current picture.
@param Slice The slice structure contains all information used to decode the next slice
@param aio_piPosition The current aio_piPosition in the NAL.
@param Vlc VLC_TABLES contains all the tables for decoding the vlc.
@param aio_tstNon_zero_count Specifies the coeff_token of each block of the picture.
@param SliceTable Specifies in which Slice belongs each macroblock of the picture.
@param aio_pstSliceGroupMap Specifies in which Slice belongs each macroblock of the picture.
@param ao_pstIntra_pred_mode Contains the prediction mode for each macroblock.
@param ao_piEndOfSlice Specifies if the current picture is totally decoded.
*/
char slice_data_I_cavlc(RESIDU *picture_residu, const unsigned char *ai_pcData,  const int *ai_piNalBytesInNalunit, 
						const SPS *Sps, const PPS *Pps, DATA *aio_tstBlock, SLICE *Slice, 
						int *aio_piPosition,const VLC_TABLES *Vlc, unsigned char *SliceTable, 
						const unsigned char *aio_pstSliceGroupMap, int *EndOfSlice) 
{

	//Initialization of the image parameters
	volatile int  moreDataFlag;
	const short PicSizeInMbs = Sps -> PicSizeInMbs ;
	const short PicWidthInMbs = Sps -> pic_width_in_mbs;
	const short PicHeightInMbs = Sps -> pic_height_in_map_units;
	short iCurrMbAddr = Slice -> first_mb_in_slice;

	__declspec(align(64)) unsigned char NonZeroCountCache[48];

	short iMb_x;
	short iMb_y;

	char ErrorDetection = 0;
	unsigned char Last_Qp = Slice -> slice_qp_delta ;
	
	//initialize macroblock position
	GetMbPosition(iCurrMbAddr, PicWidthInMbs, &iMb_x, &iMb_y);

	//Decoding process macroblock per macroblock of one Slice
	do 	{

		init_block(&aio_tstBlock[iCurrMbAddr]);
		init_residu(&picture_residu[iCurrMbAddr]);

		//Decoding process of the current macroblock
		ErrorDetection = macroblock_layer_I(Pps, &picture_residu[iCurrMbAddr], ai_pcData, aio_piPosition, Slice, aio_tstBlock, 
			Vlc, NonZeroCountCache, SliceTable, iMb_x, iMb_y, &Last_Qp);

		NonZeroCountManagement(NonZeroCountCache, iMb_x, iMb_y, PicWidthInMbs, PicHeightInMbs, 
			&aio_tstBlock[iCurrMbAddr], &picture_residu[iCurrMbAddr]);

		//Updating of the macroblock parameters
		Slice -> mb_xy ++;
		moreDataFlag = MoreRbsp(ai_pcData, aio_piPosition, ai_piNalBytesInNalunit, iCurrMbAddr, PicSizeInMbs, &ErrorDetection);

		//Updating of the macroblock adress
		iCurrMbAddr = Next_MbAddress(iCurrMbAddr, PicSizeInMbs, PicWidthInMbs, aio_pstSliceGroupMap, &iMb_x, &iMb_y);

	} while ( moreDataFlag);

	//To set endofslice according to the error detection.
	*EndOfSlice = EndOfSliceManagement(Slice, PicSizeInMbs, ErrorDetection);
	return ErrorDetection;
}





/**
This function permits to decode the VLC for an P image. 
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
@param ao_piEndOfSlice Specifies if the current picture is totally decoded.
@param mvl0_io A table where the motion vector are stocked for each 4x4 block of each macroblock.
@param refl0_io A table where the reference is stocked for each 4x4 block of each macroblock.
*/
char slice_data_P_cavlc(RESIDU *picture_residu, const unsigned char *Data, const int *ai_piNalBytesInNalunit, 
						const SPS *Sps, const PPS *Pps, DATA *Block, SLICE *Slice, int *Position, 
						const VLC_TABLES *Vlc, unsigned char *SliceTable, const unsigned char *SliceGroupMap, 
						int *EndOfSlice , short *mvl0_io, short *refl0_io)
{


	int		i ;
	volatile int  moreDataFlag = 1;
	int mb_skip_run;


	__declspec(align(64)) unsigned char Non_zero_count_cache[48];
	__declspec(align(64)) short ref_cache_l0 [48];
	__declspec(align(64)) short mv_cache_l0 [48][2];


	//Initialization of the image parameters
	const short PicSizeInMbs = Sps -> PicSizeInMbs ;
	const short PicWidthInMbs = Sps -> pic_width_in_mbs;
	const short PicHeightInMbs = Sps -> pic_height_in_map_units;
	short iCurrMbAddr = Slice -> first_mb_in_slice;

	short iMb_x;
	short iMb_y;
	char ErrorDetection = 0;
	unsigned char Last_Qp = Slice -> slice_qp_delta ;
	
	//initialize macroblock position
	GetMbPosition(iCurrMbAddr, PicWidthInMbs, &iMb_x, &iMb_y);



	//Decoding process macroblock per macroblock of one Slice
	do 
	{
		//Detection of a skipped macroblock
		mb_skip_run = read_ue(Data, Position);
		if (mb_skip_run){
			for ( i = 0 ; i < mb_skip_run && iCurrMbAddr < PicSizeInMbs; i++ ) {
				//Updating the skipped macroblock parameters
				init_block(&Block[iCurrMbAddr]);
				init_residu(&picture_residu[iCurrMbAddr]);
				picture_residu[iCurrMbAddr] . SliceNum = SliceTable [iCurrMbAddr] = Slice -> slice_num ;

				SkipP(Slice, Sps, picture_residu, Block, SliceTable, 
					iMb_x, iMb_y, PicSizeInMbs, Last_Qp, mv_cache_l0, ref_cache_l0, 
					&mvl0_io [(iMb_x << 3) + iMb_y * (Slice -> b_stride << 2)], 
					&refl0_io[(iMb_x << 1) + iMb_y * (Slice -> b8_stride << 1)]);

				//Save the last macroblock address
				iCurrMbAddr = Next_MbAddress(iCurrMbAddr, PicSizeInMbs, PicWidthInMbs, SliceGroupMap, &iMb_x, &iMb_y);
				Slice -> mb_xy ++; 
			}

			//Test if there is more information in the NAL
			moreDataFlag = MoreRbsp(Data, Position, ai_piNalBytesInNalunit, iCurrMbAddr, PicSizeInMbs, &ErrorDetection);
		}


		//In case of there is more data in the NAL
		if ( moreDataFlag ) {		
			init_ref_cache(ref_cache_l0);
			init_mv_cache(mv_cache_l0);
			init_block(&Block[iCurrMbAddr]);
			init_residu(&picture_residu[iCurrMbAddr]);

			fill_caches_motion_vector(Slice, Slice -> b_stride, Slice -> b8_stride, 
				ref_cache_l0, mv_cache_l0, SliceTable , &picture_residu[iCurrMbAddr], 
				&mvl0_io [(iMb_x << 3) + iMb_y * (Slice -> b_stride << 2)], 
				&refl0_io[(iMb_x << 1) + iMb_y * (Slice -> b8_stride << 1)], iMb_x, iMb_y);


			//Decoding process of the current macroblock
			ErrorDetection = macroblock_layer_P(Pps, &picture_residu[iCurrMbAddr], Data, 
				Position, Slice, Block , Vlc, Non_zero_count_cache, 
				SliceTable, iMb_x, iMb_y, &Last_Qp, mv_cache_l0, ref_cache_l0);

			//Recording of the neighbourhood for the decoding of the VLC 
			NonZeroCountManagement(Non_zero_count_cache, iMb_x, iMb_y, PicWidthInMbs, PicHeightInMbs, &Block[iCurrMbAddr], &picture_residu[iCurrMbAddr]);

			//Recording of the motion vector
			write_back_motion( Slice -> b_stride, Slice -> b8_stride, 
				&mvl0_io[(iMb_x << 3) + iMb_y * (Slice -> b_stride << 2)], mv_cache_l0, 
				&refl0_io[(iMb_x << 1) + iMb_y * (Slice -> b8_stride << 1)], ref_cache_l0 );

			//Updating of the macroblock parameters
			Slice -> mb_xy++;
			moreDataFlag = MoreRbsp(Data, Position, ai_piNalBytesInNalunit, iCurrMbAddr, PicSizeInMbs, &ErrorDetection);

			//Updating the macroblock adress
			iCurrMbAddr = Next_MbAddress(iCurrMbAddr, PicSizeInMbs, PicWidthInMbs, SliceGroupMap, &iMb_x, &iMb_y);
		}
	} while ( moreDataFlag);
	//To set endofslice according to the error detection.
	*EndOfSlice = EndOfSliceManagement(Slice, PicSizeInMbs, ErrorDetection);
	return ErrorDetection;
}




