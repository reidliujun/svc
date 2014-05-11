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
#include "memory_management.h"

//SVC Files
#include "svc_type.h"
#include "set_layer.h"

void svc_calculate_dpb(const int total_memory, const int mv_memory, short nb_of_layers, MMO *mmo_struct, SPS *sps, NAL *Nal){


	short i;
	short changed = 0;
	short sps_id = 0;
	short NumLayers = GetLayerNumber(Nal, Nal -> DqIdMax) + 1;
	short SeqNumLayers = 0;
	short max_width  = mmo_struct -> MaxWidth;
	short max_height = mmo_struct -> MaxHeight;
	unsigned char max_depth_wait = 0;
	
	for ( i = 0; i < 32; i ++){
		if ( max_width < (sps[i] . pic_width_in_mbs << 4)){
			//Get the higher horizontal resolution
			max_width = mmo_struct -> MaxWidth = (sps[i] . pic_width_in_mbs << 4);
			sps[i] . b_stride = (max_width) >> 1;
			sps[i] . b8_stride = (max_width) >> 2;
			changed = 1;
			sps_id = i;
		}

		if ( max_height < (sps[i] . pic_height_in_map_units << 4)){
			//Get the higher vertical resolution
			max_height = mmo_struct -> MaxHeight = (sps[i] . pic_height_in_map_units << 4);
		}

		if ( sps[i] . nb_img_disp > max_depth_wait){
			//Get the higher delay
			max_depth_wait = sps[i] . nb_img_disp;
			sps_id = i;
		}

		if (sps[i] . pic_width_in_mbs > 0){
			//Get the num of layer of the sequence
			SeqNumLayers ++;
			if(sps[i]. num_ref_frames > sps[sps_id] . num_ref_frames){
				sps_id = i;
			}
		}
	}

	for ( i = 0; i < 32; i ++){
		//Avoid to loose addresses when we change of layer
		sps[i] . nb_img_disp = max_depth_wait;

		//Allow to have the same partitionning between each layer
		sps[i] . b_stride = (max_width) >> 1;
		sps[i] . b8_stride = (max_width) >> 2;
	}



	if ( changed){
		if(SeqNumLayers > NumLayers){
			//In case of all layers are not present into the first AU.
			//Comparison between the number of sps and the number of layer detected in the first AU
			NumLayers = SeqNumLayers;
		}
		if ( NumLayers > nb_of_layers){
			//To avoid allocating more than the decoder provides
			NumLayers = nb_of_layers;
		}
		initialize_address(total_memory, mv_memory, NumLayers, (max_width + 32) * (max_height + 32), max_width * max_height >> 3, &sps[sps_id], mmo_struct);
		mmo_struct -> MemoryAllocation = 1;
	}
}
