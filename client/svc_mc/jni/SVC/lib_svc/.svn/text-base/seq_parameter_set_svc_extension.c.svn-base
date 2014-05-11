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

#include <string.h>

//AVC Files
#include "type.h"
#include "sps.h"
#include "clip.h"
#include "bitstream.h"
#include "scaling_matrix.h"
#include "slice_data_vlc.h"


//SVC Files
#include "set_layer.h"


/**
SPS extension parsing.
*/
void seq_parameter_set_svc_extension (unsigned char *data, int *position, SPS *sps_id)
{
    // to change
    sps_id -> InterLayerDeblockingFilterFlag = getNbits(data, position, 1) ;// u(1)  interlayer_deblocking_filter_control_present_flag =
    sps_id -> extended_spatial_scalability = getNbits(data, position, 2) ;// u(2)

    if ( sps_id -> chroma_format_idc > 0 )  {
        sps_id -> chroma_phase_x = (char) (getNbits(data, position, 1) - 1);// u(1)
        sps_id -> chroma_phase_y = (char) (getNbits(data, position, 2) - 1);// u(2)
    }else	{
		sps_id -> chroma_phase_x = - 1;
        sps_id -> chroma_phase_y = - 1;
	}
		


    if( sps_id -> extended_spatial_scalability == 1 ) {
        if( sps_id -> chroma_format_idc > 0 ) { //assume that separate_colour_plane_flag is equal to 0.
			getNbits(data, position, 1);// + 1; int seq_ref_layer_chroma_phase_x = 
			getNbits(data, position, 2);// + 1; int seq_ref_layer_chroma_phase_y = 
		}

		sps_id -> scaled_base_left_offset = (short ) (read_se(data, position));
        sps_id -> scaled_base_top_offset = (short ) (read_se(data, position));
        sps_id -> scaled_base_right_offset = (short ) (read_se(data, position));
        sps_id -> scaled_base_bottom_offset = (short ) (read_se(data, position));
	}

	if(getNbits(data, position, 1)) {//seq_tcoeff_level_prediction_flag 0 u(1)
		sps_id -> tcoeff_level_prediction_flag = 1;
		sps_id -> adaptive_tcoeff_level_prediction_flag = getNbits(data, position, 1);
	}
	
	sps_id -> slice_header_restriction_flag = getNbits(data, position, 1); //slice_header_restriction_flag 
}




/**
VUI extension parsing.
*/

void svc_vui_parameters_extension(unsigned char *data, int *position, SPS *sps_id ) {
	
	int i;
	int nal_hrd_parameters_present_flag, vcl_hrd_parameters_present_flag;
	int num_layers_minus1 = read_ue(data, position) + 1;

	for( i = 0; i <= num_layers_minus1; i++ ) {
		getNbits(data, position, 3);//DependencyId[ i ] = 
		getNbits(data, position, 4);//quality_id[ i ] = 
		getNbits(data, position, 3);//temporal_id[ i ] = 
		if( getNbits(data, position, 1)) {// timing_info_present_flag[ i ]
			getNbits(data, position, 32);//num_units_in_tick[ i ] 0 u(32)
			getNbits(data, position, 32);//	time_scale[ i ] 0 u(32)
			getNbits(data, position, 1);//	fixed_frame_rate_flag[ i ] 0 u(1)
		}
		
		nal_hrd_parameters_present_flag = getNbits(data, position, 1); //nal_hrd_parameters_present_flag[ i ] 0 u(1)
		if(nal_hrd_parameters_present_flag ){
			hrd_parameters(data, position, sps_id);
		}

		vcl_hrd_parameters_present_flag = getNbits(data, position, 1);
		if(vcl_hrd_parameters_present_flag){
			hrd_parameters(data, position, sps_id);
		}
					
		if( nal_hrd_parameters_present_flag ||	vcl_hrd_parameters_present_flag){
			getNbits(data, position, 1); //low_delay_hrd_flag[ i ] 0 u(1)
		}
		
		getNbits(data, position, 1); //pic_struct_present_flag[ i ] 0 u(1)
	}
}





/**
Subset Sps parsing.
*/
void subset_sps (unsigned char *data, int *NalInRbsp, SPS *sps, NAL *Nal)
{

	int position = 8;
	unsigned char svc_vui_parameters_present_flag;
	unsigned char profile_idc = getNbits(data, &position, 8);
    unsigned char constraint = getNbits(data, &position, 8);
    unsigned char level_idc = getNbits(data, &position, 8); //level_idc =

	if (profile_idc && !(constraint & 0xf)){
		unsigned char Sps_id = read_ue(data, &position) + 16;
		unsigned char seq_parameter_set_id = CLIP3(0, SPS_BUF_SIZE - 1, Sps_id);
		SPS *pt_sps = &sps [seq_parameter_set_id];

		//To save parameter before resetting
		short b_stride = pt_sps -> b_stride; 
		short b8_stride = pt_sps -> b8_stride; 
		unsigned char MaxDpbWait = pt_sps -> nb_img_disp;

		//Initialization of the maximun number of sps
		if(seq_parameter_set_id > sps[0] . MaxSpsId){
			sps[0] . MaxSpsId = seq_parameter_set_id;
		}

		//Intilaize the SPS to zero
		memset(pt_sps, 0, sizeof(SPS));
		pt_sps -> b_stride = b_stride; 
		pt_sps -> b8_stride = b8_stride;
		pt_sps -> profile_idc = profile_idc;
		pt_sps -> level_idc = level_idc;

		seq_parameter_set_data(data, &position, pt_sps);

		if( pt_sps -> profile_idc == 83 || pt_sps -> profile_idc == 86 ) {
			seq_parameter_set_svc_extension(data, &position, pt_sps );
			svc_vui_parameters_present_flag = getNbits(data, &position, 1);
			if( svc_vui_parameters_present_flag == 1 )
				svc_vui_parameters_extension(data, &position, pt_sps );
		}
		if(MaxDpbWait){//Must not changed during the decoding process
			pt_sps -> nb_img_disp = MaxDpbWait;
		}

		if( getNbits(data, &position, 1) ){ //additional_extension2_flag
			while( more_rbsp_data(data, &position,  NalInRbsp)){
				getNbits(data, &position, 1); //int additional_extension2_data_flag = 
			}
		}
	}
}
