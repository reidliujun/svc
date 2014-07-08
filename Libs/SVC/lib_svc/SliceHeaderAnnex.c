
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
#include "clip.h"
#include "bitstream.h"
#include "slice_header.h"
#include "memory_management.h"


//SVC Files
#include "svc_type.h"
#include "svc_data.h"
#include "set_layer.h"
#include "pic_list_svc.h"
#include "ErrorDetection.h"
#include "SliceHeaderExtension.h"
#include "nal_unit_header_svc_extension.h"





/**
Slice header used for parsing the first AU.
*/
int SliceHeaderCut(unsigned char *data, SPS *sps, PPS *pps, NAL *Nal, int *position) 
{

	int PicParameterId;
	int SeqParameterId;
	PPS *pt_pps_id ;
	SPS *pt_sps_id ;	
	SLICE Slice[1];
	MMO Mmo[1];
	LIST_MMO Current_pic[1], RefPicListL0[16], RefPicListL1[16];
	LAYER_MMO *LayerMmo = &Mmo -> LayerMMO [Nal -> LayerId];
	
	LayerMmo -> prevPicOrderCntLsb = LayerMmo -> prevPicOrderCntMsb = 0;
	Slice -> first_mb_in_slice = (short) read_ue(data, position);
	Current_pic -> slice_type = Slice -> slice_type = (unsigned char) read_ue(data, position);
	if ( Slice -> slice_type > 4 ) {
		Current_pic -> slice_type = Slice -> slice_type -= 5 ;
	}

	PicParameterId = read_ue(data, position);
	Nal -> pic_parameter_id[Nal -> LayerId] = PicParameterId = CLIP3(0, PPS_BUF_SIZE - 1, PicParameterId);
	pt_pps_id = &pps[PicParameterId];
	SeqParameterId = pt_pps_id -> seq_parameter_set_id + (Nal -> LayerId ? 16: 0);
	pt_sps_id = &sps[SeqParameterId];
	LayerMmo -> frame_num = getNbits(data, position, pt_sps_id -> log2_max_frame_num);

	if(!pt_sps_id -> PicSizeInMbs){
		Nal -> NalToSkip = 1;
		Nal -> PicToDecode = Nal -> PicToDisplay = 0;
		return 0;
	}

	//Read Frame parameter
	FrameHeaderData(data, position, Nal -> NalRefIdc, Nal -> IdrFlag, Nal -> LayerId, 1, 
		Current_pic, LayerMmo, pt_sps_id, pt_pps_id, Mmo, RefPicListL0, RefPicListL1);

	Current_pic -> poc = PocComputation(pt_sps_id, Nal -> IdrFlag * 5, Nal -> NalRefIdc, LayerMmo);

	if (Nal -> QualityId == 0){
		ReferenceListProcessing(data, position, Slice, pt_pps_id, pt_sps_id, LayerMmo, Current_pic, RefPicListL0, RefPicListL1);
		Slice -> AdaptivePredictionFlag = 0;

		if( Nal -> NalRefIdc != 0 ) {
			dec_ref_pic_marking(data, position, Nal -> IdrFlag * 5, pt_sps_id, LayerMmo);
			if (!pt_sps_id -> slice_header_restriction_flag){
				Nal -> StoreRefBasePicFlag = getNbits(data, position, 1); //
				if (( Nal -> UseRefBasePicFlag || Nal -> StoreRefBasePicFlag) && !Nal -> IdrFlag){ 
					DecRefBasePicMarking(data, position, Mmo, pt_sps_id, Nal);
				}
			}
		}
	}else {
		Nal -> BaseLayerId = Nal -> LastLayerId;
	}

	if( pt_pps_id -> entropy_coding_mode_flag  &&  Slice -> slice_type !=  EI ){
		read_ue(data, position); 
	}

	read_se(data, position);

	if ( pt_pps_id -> deblocking_filter_control_present_flag ) {
		int disable_deblocking_filter_idc = read_ue(data, position);
		if (disable_deblocking_filter_idc != 1 ) {
			read_se(data, position);
			read_se(data, position);
		}
	}


	if (Nal -> QualityId == 0 && pt_pps_id -> num_slice_groups_minus1 > 0  && pt_pps_id -> slice_group_map_type >= 3  &&  pt_pps_id -> slice_group_map_type <= 5){
		int PicSizeInMapUnits = pt_sps_id -> PicSizeInMbs;
		int SliceGroupChangeRate = pt_pps_id -> slice_group_change_rate_minus1;
		int tmp = log_base2(PicSizeInMapUnits / SliceGroupChangeRate + 1);
		getNbits(data, position, tmp);
	}



	if ( !Nal -> no_inter_layer_pred_flag && Nal -> QualityId == 0){
		int ref_layer_dq_id = read_ue(data, position); 
		Nal -> BaseLayerId = GetLayerNumber(Nal, ref_layer_dq_id);
	}
	NalParamInit(Nal, pps, sps, pt_sps_id);
	SetSpatialUsed(Nal);

	return Current_pic -> poc;
}
