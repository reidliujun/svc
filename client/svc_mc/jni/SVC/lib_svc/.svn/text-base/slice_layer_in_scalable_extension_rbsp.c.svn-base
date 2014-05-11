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
#include "data.h"
#include "cavlc.h"
#include "init_data.h"
#include "slice_header.h"
#include "SliceGroupMap.h"

//SVC files
#include "svc_type.h"
#include "svc_data.h"
#include "Coordonates.h"
#include "slice_data_svc.h"
#include "fill_caches_svc.h"
#include "ErrorConcealment.h"
#include "nal_unit_header_svc_extension.h"


/**
This functions initializes the RESIDU structure of a macroblock when a slice is skipped.
*/
void SkipSliceMacroInit (RESIDU *CurrResidu, SLICE *Slice, int BaseMbType)
{

	init_residu(CurrResidu);
	if (Slice -> slice_type == SLICE_TYPE_P){ 
		init_P_skip_mb(CurrResidu, Slice -> slice_qp_delta, Slice);
		if(IS_I_SVC(BaseMbType)){
			CurrResidu -> MbType = I_BL;
		}else {
			CurrResidu -> MbType = P_BL;
		}
	}else{
		init_B_skip_mb(CurrResidu, Slice -> slice_qp_delta, Slice);
		if(IS_I_SVC(BaseMbType)){
			CurrResidu -> MbType = I_BL;
		}else {
			CurrResidu -> MbType = P_BL;
		}
	}

	CurrResidu -> InCropWindow = CurrResidu -> BaseModeFlag = CurrResidu -> ResidualPredictionFlag = 1;
}




/**
This functions initializes the RESIDU structure of a skipped slice.
*/
void SkipSliceDecoding(NAL *Nal, const SPS *Sps, SLICE *Slice, RESIDU *CurrResidu, RESIDU *BaseResidu, 
					   DATA *Block, unsigned char *MbToSliceGroupMap, unsigned char *SliceTable)
{

	short iMb_x;
	short iMb_y;
	short iCurrMbAddr = Slice -> first_mb_in_slice;
	const short PicSizeInMbs = Sps -> PicSizeInMbs ;
	const short PicWidthInMbs = Sps -> pic_width_in_mbs;

	//initialize macroblock position
	GetMbPosition(iCurrMbAddr, PicWidthInMbs, &iMb_x, &iMb_y);

	for (; iCurrMbAddr < PicSizeInMbs ;) {
		//Initialize base macroblock address
		short BaseAddr = GetBaseMbAddr(Nal, &Block[iCurrMbAddr], iMb_x << 4, iMb_y << 4);
		SkipSliceMacroInit(&CurrResidu [iCurrMbAddr], Slice, BaseResidu[BaseAddr] . MbType);
		SliceTable[iCurrMbAddr] = 0;

		//Updating the macroblock address
		iCurrMbAddr = Next_MbAddress(iCurrMbAddr, PicSizeInMbs, PicWidthInMbs, MbToSliceGroupMap, &iMb_x, &iMb_y);
	}
}


/**
CAVLC decoding process for SVC layers.
*/
void slice_data_in_scalable_extension_cavlc ( const int size_mb, const unsigned char *Data, const int *NalBytesInNalunit, 
											 const int *Position, NAL *Nal, const SPS *Sps, PPS *Pps, const VLC_TABLES *vlc, 
											 unsigned char *SliceGroupId, SLICE *Slice, unsigned char *MbToSliceGroupMap, 
											 unsigned char *SliceTable, DATA *TabBlock, RESIDU *TabResidu, int *EndOfSlice)
{



	if( !Nal -> NalToSkip){
		int pos = *Position;
		const int MbMacro = Sps -> PicSizeInMbs;

		//Parameters initialization Initialisation in case of a new picture
		if ( *EndOfSlice == 0 ) {
			reinit_slice(Slice);
			Slice -> mb_stride = Sps -> pic_width_in_mbs;
			Nal -> b_stride = Sps -> b_stride;
			Nal -> b8_stride = Sps -> b8_stride;
			mapUnitToSlice(Sps, Pps, MbToSliceGroupMap, Slice -> slice_group_change_cycle, SliceGroupId);
			InitUcharTab(MbMacro, SliceTable);
		}else{
			SliceManagement(Pps, Slice);
		}

		//Entropy decoding if present
		if ( Nal -> SliceSkipFlag){
			SkipSliceDecoding(Nal, Sps, Slice, &TabResidu[Nal -> LayerId * size_mb], 
				&TabResidu[Nal -> BaseLayerId * size_mb], TabBlock, MbToSliceGroupMap, SliceTable);
		}else {
			Nal -> ErrorDetection = slice_data_cavlc_svc(Nal, &TabResidu[Nal -> LayerId * size_mb], &TabResidu[Nal -> BaseLayerId * size_mb], 
				Data, NalBytesInNalunit, Sps, Pps, TabBlock, Slice, &pos, vlc, SliceTable, 
				MbToSliceGroupMap, EndOfSlice);
		}
	}

#ifdef ERROR_CONCEALMENT
	//Error concealment
	SVCErrorConcealment(Nal, Sps, Slice, SliceTable, &TabResidu[Nal -> LayerId * size_mb], TabBlock);
#endif
}






/**
CABAC decoding process for SVC layers.
*/
void SliceCabac(const int size_mb, unsigned char *data, int *NalBytesInNalunit, int *position, NAL *Nal, const SPS *Sps, PPS *Pps, 
				unsigned char *ai_slice_group_id, short *mv_cabac_l0, short *mv_cabac_l1, short *ref_cabac_l0, short *ref_cabac_l1, 
				SLICE *Slice, unsigned char *MbToSliceGroupMap,  unsigned char *SliceTable, DATA *Tab_block, RESIDU *TabResidu, int *EndOfSlice)
{



	if( !Nal -> NalToSkip){
		//Frame with TemporalId >= WantedTemporalid are not decoded
		int pos = *position;
		const int MbMacro = Sps -> PicSizeInMbs;

		//Parameters initialization Initialisation in case of a new picture
		if ( *EndOfSlice == 0 ) {
			reinit_slice(Slice);
			Slice -> mb_stride = Sps -> pic_width_in_mbs;
			Nal -> b_stride = Sps -> b_stride;
			Nal -> b8_stride = Sps -> b8_stride;
			mapUnitToSlice(Sps, Pps, MbToSliceGroupMap, Slice -> slice_group_change_cycle, ai_slice_group_id);
			InitUcharTab(MbMacro, SliceTable);
		}else{
			SliceManagement(Pps, Slice);
		}


		//CABAC decoding process
		if((pos & 7) != 0)  {
			pos =((pos >> 3) + 1) << 3;
		}


		//Entropy decoding if present
		if ( Nal -> SliceSkipFlag){
			SkipSliceDecoding(Nal, Sps, Slice, &TabResidu[Nal -> LayerId * size_mb], &TabResidu[Nal -> BaseLayerId * size_mb], 
				Tab_block, MbToSliceGroupMap, SliceTable); 
		}else {
			if ( Slice -> slice_type == SLICE_TYPE_B){
				Nal -> ErrorDetection = SliceDataBCabacSvc(data, NalBytesInNalunit, pos, Nal, Sps, Pps, Slice, &TabResidu[Nal -> LayerId * size_mb], 
					&TabResidu[Nal -> BaseLayerId * size_mb], Tab_block, SliceTable, MbToSliceGroupMap, EndOfSlice, 
					mv_cabac_l0, ref_cabac_l0, mv_cabac_l1, ref_cabac_l1);

			}else if ( Slice -> slice_type == SLICE_TYPE_P){
				Nal -> ErrorDetection = SliceDataPCabacSvc(data, NalBytesInNalunit, pos, Nal, Sps, Pps, Slice, &TabResidu[Nal -> LayerId * size_mb], 
					&TabResidu[Nal -> BaseLayerId * size_mb], Tab_block, SliceTable, MbToSliceGroupMap, EndOfSlice, 
					mv_cabac_l0, ref_cabac_l0);

			}else {
				Nal -> ErrorDetection = SliceDataICabacSvc(data, NalBytesInNalunit, pos, Nal, Sps, Pps, Slice, &TabResidu[Nal -> LayerId * size_mb], 
					&TabResidu[Nal -> BaseLayerId * size_mb], Tab_block, SliceTable, MbToSliceGroupMap, EndOfSlice);
			}
		}
	}

#ifdef ERROR_CONCEALMENT
	//Error concealment
	SVCErrorConcealment(Nal, Sps, Slice, SliceTable, &TabResidu[Nal -> LayerId * size_mb], Tab_block);
#endif
}
