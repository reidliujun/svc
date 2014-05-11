
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
#ifdef WIN32
#include "windows.h"
#endif



//AVC Files
#include "type.h"
#include "clip.h"
#include "pic_list.h"
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
This functions determines the scalability type according to decoded parameters.
*/
int SpatialScalabilityType(SPS *sps, NAL *Nal ){

	int interlaceFlag = 0; //Coded frame Only
	int baseInterlaceFlag = 0; //Coded frame Only


	// Only one chroma format so chroma_format_idc is equal to BaseChromaFormatIdc 
	if ( sps -> extended_spatial_scalability < 2  && !interlaceFlag && !baseInterlaceFlag && !Nal -> SpatialScalability){
		return 0;
	}

	if (!interlaceFlag && !baseInterlaceFlag && (2 * Nal -> down_Height == Nal -> up_Height) && (2 * Nal -> down_Width == Nal -> up_Width)){//*!sps -> extended_spatial_scalability  && */
		return 1;
	}

	if ( sps -> extended_spatial_scalability  && !interlaceFlag && !baseInterlaceFlag && 
		(2 * Nal -> base_Height == Nal -> curr_Height) && (2 * Nal -> base_Width == Nal -> curr_Width)){
			return 2;
	} 

	return 3;
}




/**
This function is used to initialize if each layer is used by a spatial enhancement
*/
void SetSpatialUsed(NAL *Nal){
	int Layer;

	for (Layer = 0; Layer < Nal -> LayerId ; Layer++){
		if (Nal -> UsedSpatial [Nal -> BaseLayerId] [Layer] == -1){
			Nal -> UsedSpatial [Nal -> BaseLayerId] [Layer] = 0;
		}
	}
	for (Layer = Nal -> LayerId; Layer < 8 ; Layer++){
		Nal -> UsedSpatial [Nal -> BaseLayerId] [Layer] = (unsigned char ) Nal -> SpatialScalability;
	}
}





/**
This functions update reference adresses of a layer by comparising with base layer.
*/
void UpdateReferenceListAddress(LAYER_MMO *LayerMmo, LIST_MMO *RefListL0, LIST_MMO *RefListL1, unsigned char QualityId){

	int i;
	unsigned char L0Count = 0;
	unsigned char L1Count = 0;

	for( i = 0; i < 16; i++){
		if(RefListL0[i] . poc != MIN_POC){
			int MemoryAddress = search_picture_address(RefListL0[i] . poc, LayerMmo, QualityId);
			if ( MemoryAddress != -1){
				RefListL0[i] . MemoryAddress = MemoryAddress;
				L0Count ++;
			}
		}
		if(RefListL1[i] . poc != MIN_POC){
			int MemoryAddress = search_picture_address(RefListL1[i] . poc, LayerMmo, QualityId);
			if ( MemoryAddress != -1){
				RefListL1[i] . MemoryAddress = MemoryAddress;
				L1Count ++;
			}
		}
		if(RefListL0[i] . poc == MIN_POC && RefListL1[i] . poc == MIN_POC){
			break;
		}
	}
	LayerMmo -> RefCountL0 = L0Count;
	LayerMmo -> RefCountL1 = L1Count;
}




/**
This functions create the reference list of the current frame.
*/
void SvcReadReferenceListProcessing(unsigned char *data, int *position, NAL *Nal, SLICE *Slice, SPS *pt_sps_id, 
									PPS *pt_pps_id, LAYER_MMO *LayerMmo, LIST_MMO *Current_pic, MMO *Mmo, 
									LIST_MMO *RefPicListL0, LIST_MMO *RefPicListL1)
{

	if (Nal -> QualityId == 0){
		ReferenceListProcessing(data, position, Slice, pt_pps_id, pt_sps_id, LayerMmo, Current_pic, RefPicListL0, RefPicListL1);
		Slice -> AdaptivePredictionFlag = 0;

		//Read weighted table
		PredWeightTable(data, position, pt_pps_id, Slice);

		//Update reference lists with current layer pictures.
		if ((Nal -> DqId != Nal -> DqIdToDisplay) && ((Nal -> DqId >> 4) == (Nal -> DqIdToDisplay >> 4))){
			//In case of SNR refinement, we set the reference listes with the referenceframe of the right layer
			int WantedLayer = GetLayerNumber(Nal, Nal -> DqIdToDisplay);
			LAYER_MMO *Layer = &Mmo -> LayerMMO [WantedLayer];
			UpdateReferenceListAddress(Layer, RefPicListL0, RefPicListL1, Nal -> DqIdToDisplay - Nal -> DqId);
		}else if ((Nal -> DqIdMax < Nal -> DqIdToDisplay) && ((Nal -> DqId >> 4) == (Nal -> DqIdMax >> 4))){
			//In case of SNR refinement, we set the reference listes with the referenceframe of the right layer
			int WantedLayer = GetLayerNumber(Nal, Nal -> DqIdMax);
			LAYER_MMO *Layer = &Mmo -> LayerMMO [WantedLayer];
			UpdateReferenceListAddress(Layer, RefPicListL0, RefPicListL1, Nal -> DqIdToDisplay - Nal -> DqId);
		}

		if( Nal -> NalRefIdc != 0 ) {
			dec_ref_pic_marking(data, position, Nal -> IdrFlag * 5, pt_sps_id, LayerMmo);
			if (!pt_sps_id -> slice_header_restriction_flag){
				Nal -> StoreRefBasePicFlag = getNbits(data, position, 1); //
				if (( Nal -> UseRefBasePicFlag || Nal -> StoreRefBasePicFlag) && !Nal -> IdrFlag){ 
					DecRefBasePicMarking(data, position, Mmo, pt_sps_id, Nal);
				}
			}
			SetDecRefLayer(Nal, Mmo, LayerMmo, Nal -> DependencyId);
		}else {
			LayerMmo -> index = 0;
		}
	}else {
		Slice -> InterLayerDeblockingFilterIdc = 1;
	}
}




/**
This function gets from the bitstream the layer prediction information and deblocking filter information.
*/
void SvcReadLayerPrediction(unsigned char *data, int *position, NAL *Nal, SPS *pt_sps_id, PPS *pt_pps_id, SLICE *Slice, 
							LIST_MMO *Current_pic, MMO *Mmo, int EndOfSlice)
{

	int  RealBaseLayer = 0;

	if ( !Nal -> no_inter_layer_pred_flag && Nal -> QualityId == 0){
		int ref_layer_dq_id = read_ue(data, position); 
		RealBaseLayer = Nal -> BaseLayerId = GetLayerNumber(Nal, ref_layer_dq_id);

		if ( pt_sps_id -> InterLayerDeblockingFilterFlag){
			Slice -> InterLayerDeblockingFilterIdc = (unsigned char) read_ue(data, position);

			if(Slice -> InterLayerDeblockingFilterIdc != 1 ) {
				pt_pps_id -> AlphaOffsetUpsapmpling = (unsigned char) (read_se(data, position) << 1); //int inter_layer_slice_alpha_c0_offset_div2 = 
				pt_pps_id -> BetaOffsetUpsapmpling = ((unsigned char) read_se(data, position) << 1);  //int inter_layer_slice_beta_offset_div2 = 
			}

		}else {
			Slice -> InterLayerDeblockingFilterIdc = 1;
			pt_pps_id -> AlphaOffsetUpsapmpling = 0;
			pt_pps_id -> BetaOffsetUpsapmpling = 0;
		}

		pt_pps_id -> constrained_intra_upsampling_flag = (unsigned char) getNbits(data, position, 1); //u(1) 

		if(  pt_sps_id -> extended_spatial_scalability  ==  2 ) {
			if ( (pt_sps_id -> chroma_format_idc) > 0 ) {
				getNbits(data, position, 2) ; // int ref_layer_chroma_phase_x_plus1_flag = u(2) - 1
				getNbits(data, position, 2) ; // int ref_layer_chroma_phase_y_plus1_flag = u(2) - 1
			}
		}
	} else 	if (!EndOfSlice){
		if(Nal -> DqId){
			int BaseLayerDqId = Nal -> layer_id_table [Nal -> LastDecodedId];
			Nal -> BaseLayerId = RealBaseLayer = GetLayerNumber(Nal, BaseLayerDqId);
		}else {
			Nal -> BaseLayerId = RealBaseLayer;
		}
	}else {
		RealBaseLayer = Nal -> BaseLayerId;
	}


	//Initialize memory address
	Nal -> BaseMvMemoryAddress = search_mv_address(Current_pic -> poc, &Mmo -> LayerMMO [Nal -> BaseLayerId]);
	Nal -> BaseMemoryAddress = search_picture_address(Current_pic -> poc, &Mmo -> LayerMMO [RealBaseLayer], 0);
}




/**
SVC default parameters parsing from the picture header.
*/
void SvcReadDefaultParameters(unsigned char *data, int *position, NAL *Nal, SLICE *Slice, SPS *pt_sps_id)
{

	if ( !Nal -> no_inter_layer_pred_flag){ 
		unsigned char default_base_mode_flag = 0;
		unsigned char default_motion_prediction_flag = 0;
		unsigned char default_residual_prediction_flag = 0;

		Nal -> SliceSkipFlag = (short) getNbits(data, position, 1);
		if( Nal -> SliceSkipFlag ){
			read_ue(data, position); //num_mbs_in_slice_minus1 = + 1;
		}
		else {
			Slice -> AdaptiveBaseModeFlag = (unsigned char) getNbits(data, position, 1); 
			if( !Slice -> AdaptiveBaseModeFlag ){//adaptive_base_mode_flag
				default_base_mode_flag = (unsigned char) getNbits(data, position, 1);
			}
			Slice -> DefaultBaseModeFlag = (unsigned char) default_base_mode_flag;

			if( !default_base_mode_flag  ) {
				Slice -> AdaptiveMotionPredictionFlag = (unsigned char) getNbits(data, position, 1); 
				if( !Slice -> AdaptiveMotionPredictionFlag )
					default_motion_prediction_flag = (unsigned char) getNbits(data, position, 1); 
			}else{
				Slice -> AdaptiveMotionPredictionFlag = 0;
			}

			Slice -> DefaultMotionPredictionFlag = default_motion_prediction_flag;

			Slice -> AdaptiveResidualPredictionFlag = (unsigned char) getNbits(data, position, 1); 
			if( !Slice -> AdaptiveResidualPredictionFlag ){ //change adaptive_residual_prediction_flag by !adaptive_residual_prediction_flag
				default_residual_prediction_flag = (unsigned char) getNbits(data, position, 1); 
			}

			Slice -> DefaultResidualPredictionFlag = default_residual_prediction_flag;

			if( pt_sps_id -> adaptive_tcoeff_level_prediction_flag == 1 ){
				Nal -> TCoeffPrediction = (short) getNbits(data, position, 1); 
			}else{
				Nal -> TCoeffPrediction = (short) pt_sps_id -> tcoeff_level_prediction_flag;
			}
		}

	}else{
		Slice -> AdaptiveBaseModeFlag = 0;
		Slice -> AdaptiveMotionPredictionFlag = 0;
		Slice -> AdaptiveResidualPredictionFlag = 0;
		Slice -> DefaultBaseModeFlag = 0;
		Nal -> TCoeffPrediction = 0;
	}


	//SliceHeader.cpp ln 344
	if (!Nal -> TCoeffPrediction && !Nal -> no_inter_layer_pred_flag && !Nal -> SpatialScalability){
		Nal -> SCoeffPrediction = 1;
	} else {
		Nal -> SCoeffPrediction = 0;
	}

	//To check with new sequences
	if( !pt_sps_id -> slice_header_restriction_flag && !Nal -> SliceSkipFlag) {
		int scan_idx_start = getNbits(data, position, 4); //scan_idx_start 2 u(4)
		int scan_idx_end = getNbits(data, position, 4); //scan_idx_end 2 u(4)
		Slice -> notDefaultScanIdx = !(scan_idx_start == 0 && scan_idx_end == 15);

		if(Slice -> notDefaultScanIdx && !Nal -> NalToSkip){
/*
#ifdef WIN32
#ifndef POCKET_PC
			MessageBoxA(NULL,(LPCSTR)"A layer is using a different scan for residual block", (LPCSTR)"Open SVC Decoder has encountered an error", MB_OK); 
#else 
			MessageBox(NULL,(LPCWSTR)"A layer is using a different scan for residual block", (LPCWSTR)"Open SVC Decoder has encountered an error", MB_OK); 
#endif
			exit(15);
#endif*/
			Nal -> NalToSkip = 1;
		}
	}
}







/**
This functions update the up-sampling parameters according to the layer size.
*/
void NalParamInit(NAL *Nal, PPS *pps, SPS *sps, SPS *pt_sps_id)
{


	//Get base parameters
	PPS *base_pps = &pps[Nal -> pic_parameter_id[Nal -> BaseLayerId]];
	SPS *base_sps = &sps[base_pps -> seq_parameter_set_id + (Nal -> BaseLayerId ? 16: 0)];


	//Initialization of Nal's different parameters 
	Nal -> curr_Width = (short) pt_sps_id -> pic_width_in_mbs << 4;
	Nal -> curr_Height = (short) pt_sps_id -> pic_height_in_map_units << 4;
	Nal -> base_Width = (short) base_sps -> pic_width_in_mbs << 4;
	Nal -> base_Height = (short) base_sps -> pic_height_in_map_units << 4;

	//In case of cropping
	Nal -> up_Width = Nal -> curr_Width - ((pt_sps_id -> scaled_base_left_offset + pt_sps_id -> scaled_base_right_offset) << 1);
	Nal -> up_Height = Nal -> curr_Height - ((pt_sps_id -> scaled_base_top_offset + pt_sps_id -> scaled_base_bottom_offset) << 1);

	if( Nal -> QualityId){
		Nal -> down_Width = Nal -> up_Width;
		Nal -> down_Height = Nal -> up_Height;
	}else{
		Nal -> down_Width = Nal -> base_Width;
		Nal -> down_Height = Nal -> base_Height;
	}

	if(!Nal -> MvCalx[Nal -> LayerId] || !Nal -> MvCaly[Nal -> LayerId]){
		Nal -> MvCalx[Nal -> LayerId] = RESAMPLE_CAL(Nal -> up_Width, Nal -> down_Width);
		Nal -> MvCaly[Nal -> LayerId] = RESAMPLE_CAL(Nal -> up_Height, Nal -> down_Height);
	}



	//Allow to determine if the last layer (actually, must be the base layer) was a spatial or an SNR enhancement.
	Nal -> LoopFilterSpatialScalability = Nal -> SpatialScalability;

	//Help to detemrine if we have to do an upsample or not
	if(Nal -> DqId){
		Nal -> SpatialScalability = Nal -> up_Width != Nal -> down_Width && Nal -> up_Height != Nal -> down_Height;
	}else{
		Nal -> SpatialScalability = 0;
	}

	//If we have a spatial enhancement, we have to activate the inter-layer deblocking filter.
	if ( !pt_sps_id -> InterLayerDeblockingFilterFlag && Nal -> SpatialScalability){
		Nal -> LoopFilterForcedIdc = 1;
	}else{
		Nal -> LoopFilterForcedIdc = 0;
	}

	Nal -> non_diadic = (Nal -> up_Width % Nal -> down_Width) || ( Nal -> up_Height % Nal -> down_Height);
	Nal -> is_cropping = pt_sps_id -> scaled_base_left_offset || pt_sps_id -> scaled_base_bottom_offset || pt_sps_id -> scaled_base_top_offset || pt_sps_id -> scaled_base_right_offset;

	Nal -> left_offset = pt_sps_id -> scaled_base_left_offset << 1;
	Nal -> top_offset = pt_sps_id -> scaled_base_top_offset << 1;
}
