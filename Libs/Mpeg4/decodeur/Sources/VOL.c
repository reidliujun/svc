/*****************************************************************************
   *
   *  MPEG4DSP developped in IETR image lab
   *
   *
   *
   *              Jean-Francois NEZAN <jnezan@insa-rennes.Fr>
   *              Mickael Raulet <mraulet@insa-rennes.Fr>
   *              http://www.ietr.org/gro/IMA/th3/temp/index.htm
   *
   *  Based on the XviD MPEG-4 video codec
   *
   *
   *
   *  This program is free software; you can redistribute it and/or modify
   *  it under the terms of the GNU General Public License as published by
   *  the Free Software Foundation; either version 2 of the License, or
   *  (at your option) any later version.
   *
   *  This program is distributed in the hope that it will be useful,
   *  but WITHOUT ANY WARRANTY; without even the implied warranty of
   *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   *  GNU General Public License for more details.
   *
   *  You should have received a copy of the GNU General Public License
   *  along with this program; if not, write to the Free Software
   *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
   *
   * $Id$
   *
   **************************************************************************/
#include <stdio.h>
#include <string.h>
#include "mpeg.h"

#define VIDOBJ_START_CODE 0x00000100	/* ..0x0000011f  */
#define VIDOBJLAY_START_CODE 0x00000120	/* ..0x0000012f */
#define VISOBJSEQ_START_CODE 0x000001b0
#define VISOBJSEQ_STOP_CODE 0x000001b1	/* ??? */
#define USERDATA_START_CODE 0x000001b2
#define GRPOFVOP_START_CODE 0x000001b3
/*#define VIDSESERR_ERROR_CODE  0x000001b4 */
#define VISOBJ_START_CODE 0x000001b5
/*#define SLICE_START_CODE      0x000001b7 */
/*#define EXT_START_CODE        0x000001b8 */
#define VISOBJ_TYPE_VIDEO 1
/*#define VISOBJ_TYPE_STILLTEXTURE      2 */
/*#define VISOBJ_TYPE_MESH              3 */
/*#define VISOBJ_TYPE_FBA               4 */
/*#define VISOBJ_TYPE_3DMESH            5 */
#define VIDOBJLAY_TYPE_SIMPLE 1
/*#define VIDOBJLAY_TYPE_SIMPLE_SCALABLE    2 */
#define VIDOBJLAY_TYPE_CORE 3
#define VIDOBJLAY_TYPE_MAIN 4
#define VOP_START_CODE 0x1b6
#define VIDOBJ_START_CODE_MASK 0x0000001f
#define VIDOBJLAY_START_CODE_MASK 0x0000000f
#define SPRITE_NONE 0
#define SPRITE_STATIC 1
#define SPRITE_GMC 2

void    read_vol_complexity_estimation_header (const unsigned char *RESTRICT data, int *RESTRICT position) ;

int VideoObjectLayer ( const int pos_i, int length, const unsigned char *RESTRICT data, struct_VOLsimple *RESTRICT VOLsimple
    , unsigned char *RESTRICT vop_complexity, int *RESTRICT pos_o, int *xsize, int *ysize )
{
    int             position = pos_i ;
    unsigned int    start_code ;
    //int             interlaced ;
    int             time_inc_bits = 0 ;
    int             vol_ver_id ;
    //int             low_delay ;
    int   video_object_layer_shape ;
    int             video_signal_type ;
    
    //BitstreamByteAlign(bs);
    do {
        if ( position % 8 != 0 ) 
            position = (position / 8 + 1) * 8 ;
        start_code = showNbits(data, position, 16) << 16 ;
        start_code += showNbits(data, position + 16, 16);
        
        // printf("temp %x\n", temp);
        // start_code = BitstreamShowBits(bs, 32);
        if ( start_code == VISOBJSEQ_START_CODE ) {
            
            // printf("<visual_object_sequence>\n");
            showNbits(data, position, 16); /* visual_object_sequence_start_code */ 
            position += 16 ;
            showNbits(data, position, 16); /* visual_object_sequence_start_code */ 
            position += 16 ;
            showNbits(data, position, 8); /* profile_and_level_indication */ 
            position += 8 ;
            
            // printf("profile_and_level_indication %i\n", profile);
        } else if ( start_code == VISOBJSEQ_STOP_CODE ) {
            showNbits(data, position, 16); /* visual_object_sequence_stop_code */ 
            position += 16 ;
            showNbits(data, position, 16); /* visual_object_sequence_stop_code */ 
            position += 16 ;
            
            // printf("</visual_object_sequence>\n");
        } else if ( start_code == VISOBJ_START_CODE ) {
            showNbits(data, position, 16); /* visual_object_start_code */ 
            position += 16 ;
            showNbits(data, position, 16); /* visual_object_start_code */ 
            position += 16 ;
            
            // printf("<visual_object>\n");
            if ( showNbits(data, position, 1) ) 
            /* is_visual_object_identified */
            {
                position += 1 ;
                vol_ver_id = showNbits(data, position, 4); /* visual_object_ver_id */ 
                position += 4 ;
                
                // printf("ver_id %i\n", vol_ver_id);
                showNbits(data, position, 3); /* visual_object_priority */ 
                position += 3 ;
            }
            else 
            {
                position += 1 ;
                vol_ver_id = 1 ;
            }
            if ( showNbits(data, position, 4) != VISOBJ_TYPE_VIDEO ) 
            /* visual_object_type */
            {
                
                // printf("visual_object_type != video\n");
                *pos_o = -1 ;
                return -1 ;
            }
            showNbits(data, position, 4);
            position += 4 ;
            
            /* video_signal_type */
            video_signal_type = showNbits(data, position, 1);
            position += 1 ;
            if ( video_signal_type ) 
            /* video_signal_type */
            {
                int color_description ;
                
                // printf("+ video_signal_type\n");
                showNbits(data, position, 3); /* video_format */ 
                position += 3 ;
                showNbits(data, position, 1); /* video_range */ 
                position += 1 ;
                color_description = showNbits(data, position, 1);
                position += 1 ;
                if ( color_description ) 
                /* color_description */
                {
                    
                    // printf("+ color_description\n");
                    showNbits(data, position, 8); /* color_primaries */ 
                    position += 8 ;
                    showNbits(data, position, 8); /* transfer_characteristics */ 
                    position += 8 ;
                    showNbits(data, position, 8); /* matrix_coefficients */ 
                    position += 8 ;
                }
            }
        } else if ( (start_code & ~VIDOBJ_START_CODE_MASK) == VIDOBJ_START_CODE ) {
            
            // printf("<video_object>\n");
            // printf("vo id %i\n", start_code & VIDOBJ_START_CODE_MASK);
            showNbits(data, position, 16); /* video_object_start_code */ 
            position += 16 ;
            showNbits(data, position, 16); /* video_object_start_code */ 
            position += 16 ;
        } else if ( (start_code & ~VIDOBJLAY_START_CODE_MASK) == VIDOBJLAY_START_CODE ) {
            int is_object_layer_identifier ;
            int aspect_ratio_info ;
            int vol_control_parameters ;
            int fixed_vop_rate ;
            int scalability ;
            
            // printf("<video_object_layer>\n");
            // printf("vol id %i\n", start_code & VIDOBJLAY_START_CODE_MASK);
            showNbits(data, position, 16); /* video_object_layer_start_code */ 
            position += 16 ;
            showNbits(data, position, 16); /* video_object_layer_start_code */ 
            position += 16 ;
            showNbits(data, position, 1); /* random_accessible_vol */ 
            position += 1 ;
            showNbits(data, position, 8);
            position += 8 ;
            is_object_layer_identifier = showNbits(data, position, 1);
            position += 1 ;
            if ( is_object_layer_identifier ) 
            /* is_object_layer_identifier */
            {
                
                // printf("+ is_object_layer_identifier\n");
                vol_ver_id = showNbits(data, position, 4); /* video_object_layer_verid */ 
                position += 4 ;
                
                // printf("ver_id %i\n", vol_ver_id);
                showNbits(data, position, 3); /* video_object_layer_priority */ 
                position += 3 ;
            }
            else 
            {
                vol_ver_id = 1 ;
            }
            aspect_ratio_info = showNbits(data, position, 4);
            position += 4 ;
            if ( aspect_ratio_info == 0x0F ) 
            /* aspect_ratio_info */
            {
                
                // printf("+ aspect_ratio_info\n");
                showNbits(data, position, 8); /* par_width */ 
                position += 8 ;
                showNbits(data, position, 8); /* par_height */ 
                position += 8 ;
            }
            vol_control_parameters = showNbits(data, position, 1);
            position += 1 ;
            if ( vol_control_parameters ) 
            /* vol_control_parameters */
            {
                int vbv_parameters ;
                
                // printf("+ vol_control_parameters\n");
                showNbits(data, position, 2); /* chroma_format */ 
                position += 2 ;
                
                // poke dec->low_delay = (uint8_t)BitstreamGetBit(bs);	/* low_delay */
                showNbits(data, position, 1); /* low_delay */ 
                position += 1 ;
                vbv_parameters = showNbits(data, position, 1);
                position += 1 ;
                
                // printf("low_delay %i\n", low_delay);
                if ( vbv_parameters ) 
                /* vbv_parameters */
                {
                    
                    // printf("+ vbv_parameters\n");
                    showNbits(data, position, 15); /* first_half_bitrate */ 
                    position += 15 ;
                    showNbits(data, position, 1);
                    position += 1 ;
                    showNbits(data, position, 15); /* latter_half_bitrate */ 
                    position += 15 ;
                    showNbits(data, position, 1);
                    position += 1 ;
                    showNbits(data, position, 15); /* first_half_vbv_buffer_size */ 
                    position += 15 ;
                    showNbits(data, position, 1);
                    position += 1 ;
                    showNbits(data, position, 3); /* latter_half_vbv_buffer_size */ 
                    position += 3 ;
                    showNbits(data, position, 11); /* first_half_vbv_occupancy */ 
                    position += 11 ;
                    showNbits(data, position, 1);
                    position += 1 ;
                    showNbits(data, position, 15); /* latter_half_vbv_occupancy */ 
                    position += 15 ;
                    showNbits(data, position, 1);
                    position += 1 ;
                }
            }
            video_object_layer_shape = showNbits(data, position, 2); /* video_object_layer_shape */ 
            position += 2 ;
            
            // printf("shape %i\n", video_object_layer_shape);
            if ( video_object_layer_shape == 3 && vol_ver_id != 1 ) {
                showNbits(data, position, 4); /* video_object_layer_shape_extension */ 
                position += 4 ;
            }
            showNbits(data, position, 1);
            position += 1 ;
            
            /* *************************** for decode B-frame time *********************** */
            VOLsimple -> vop_time_increment_resolution = showNbits(data, position, 16); /* vop_time_increment_resolution */ 
            position += 16 ;
            
            // printf("vop_time_increment_resolution %i\n", VOLsimple -> vop_time_increment_resolution);
            if ( VOLsimple -> vop_time_increment_resolution > 0 ) {
                time_inc_bits = VOLsimple -> time_inc_bits = MAX(log_base2(VOLsimple -> vop_time_increment_resolution),1);;
            } else {
                time_inc_bits = VOLsimple -> time_inc_bits = 1 ;
            }
            showNbits(data, position, 1);
            position += 1 ;
            fixed_vop_rate = showNbits(data, position, 1);
            position += 1 ;
            if ( fixed_vop_rate ) 
            /* fixed_vop_rate */
            {
                
                // printf("+ fixed_vop_rate\n");
                showNbits(data, position, time_inc_bits); /* fixed_vop_time_increment */ 
                position += time_inc_bits ;
            }
            if ( video_object_layer_shape != 2 ) 
            {
                if ( video_object_layer_shape == 0 ) {
                    showNbits(data, position, 1);
                    position += 1 ;
                    *xsize = VOLsimple -> video_object_layer_width = showNbits(data, position, 13); /* video_object_layer_width */
                    VOLsimple -> video_object_layer_width = ((VOLsimple -> video_object_layer_width + 15) /16) * 16;
                    position += 13 ;
                    showNbits(data, position, 1);
                    position += 1 ;
                    *ysize = VOLsimple -> video_object_layer_height = showNbits(data, position, 13); /* video_object_layer_height */
                    VOLsimple -> video_object_layer_height = ((VOLsimple -> video_object_layer_height + 15) /16) * 16;
                    position += 13 ;
                    showNbits(data, position, 1);
                    position += 1 ;
                    
                    // printf("width %i\n", VOLsimple -> video_object_layer_width);
                    // printf("height %i\n", VOLsimple -> video_object_layer_height);
                }
                showNbits(data, position, 1); //interlaced
                position += 1 ;
                
                // printf("interlace\n", interlaced);
                if ( !showNbits(data, position, 1) ) 
                /* obmc_disable */
                {
                    
                    // printf("obmc_disabled==false not supported\n");
                    /* TODO */
                    /* fucking divx4.02 has this enabled */
                }
                position += 1 ; // obmc_disable
                VOLsimple -> sprite_enable = showNbits(data, position, (vol_ver_id == 1 ? 1 : 2)); /* sprite_enable */ 
                position += vol_ver_id == 1 ? 1 : 2 ;
                if ( VOLsimple -> sprite_enable == SPRITE_STATIC || VOLsimple -> sprite_enable == SPRITE_GMC ) {
                    //int low_latency_sprite_enable ;
                    //int sprite_warping_points ;
                    //int sprite_warping_accuracy ;
                    //int sprite_brightness_change ;
                    if ( VOLsimple -> sprite_enable != SPRITE_GMC ) {
                        //int sprite_width ;
                        //int sprite_height ;
                        //int sprite_left_coord ;
                        //int sprite_top_coord ;
                        showNbits(data, position, 13); /* sprite_width */ 
                        position += 13 ;
                        showNbits(data, position, 1);
                        position += 1 ;
                        showNbits(data, position, 13); /* sprite_height */ 
                        position += 13 ;
                        showNbits(data, position, 1);
                        position += 1 ;
                        showNbits(data, position, 13); /* sprite_left_coordinate */ 
                        position += 13 ;
                        showNbits(data, position, 1);
                        position += 1 ;
                        showNbits(data, position, 13); /* sprite_top_coordinate */ 
                        position += 13 ;
                        showNbits(data, position, 1);
                        position += 1 ;
                    }
                    showNbits(data, position, 6); /* no_of_sprite_warping_points */ 
                    position += 6 ;
                    showNbits(data, position, 2); /* sprite_warping_accuracy */ 
                    position += 2 ;
                    showNbits(data, position, 1); /* brightness_change */ 
                    position += 1 ;
                    if ( VOLsimple -> sprite_enable != SPRITE_GMC ) {
                        showNbits(data, position, 1); /* low_latency_sprite_enable */ 
                        position += 1 ;
                    }
                }
                if ( vol_ver_id != 1 && video_object_layer_shape != 0 ) {
                    showNbits(data, position, 1); /* sadct_disable */ 
                    position += 1 ;
                }
                if ( showNbits(data, position, 1) ) 
                /* not_8_bit */
                {
                    position += 1 ;
                    
                    // printf("not_8_bit==true (ignored)\n");
                    VOLsimple -> quant_precision = showNbits(data, position, 4); /* quant_precision */ 
                    position += 4 ;
                    VOLsimple -> bits_per_pixel = showNbits(data, position, 4); /* bits_per_pixel */ 
                    position += 4 ;
                }
                else 
                {
                    position += 1 ;
                    VOLsimple -> quant_precision = 5 ;
                    VOLsimple -> bits_per_pixel = 8 ;
                }
                if ( video_object_layer_shape == 3 ) {
                    showNbits(data, position, 1); /* no_gray_quant_update */ 
                    position += 1 ;
                    showNbits(data, position, 1); /* composition_method */ 
                    position += 1 ;
                    showNbits(data, position, 1); /* linear_composition */ 
                    position += 1 ;
                }
                VOLsimple -> quant_type = showNbits(data, position, 1); /* quant_type */ 
                position += 1 ;
                
                // printf("quant_type %i\n", VOLsimple -> quant_type);
                if ( VOLsimple -> quant_type ) {
                    int load_intra_quant_mat = showNbits(data, position, 1);
                    position += 1 ;
                    if ( load_intra_quant_mat ) 
                    /* load_intra_quant_mat */
                    {
                        
                        // unsigned char   matrix [64];
                        // printf("load_intra_quant_mat\n");
                        // bs_get_matrix(bs, matrix);
                        // set_intra_matrix(matrix);
                    }
                    else 
                    {
                        
                        // set_intra_matrix(get_default_intra_matrix());
                    }
                    if ( showNbits(data, position, 1) ) 
                    /* load_inter_quant_mat */
                    {
                        
                        // unsigned char   matrix [64];
                        position += 1 ;
                        
                        // printf("load_inter_quant_mat\n");
                        // bs_get_matrix(bs, matrix);
                        // set_inter_matrix(matrix);
                    }
                    else 
                    {
                        position += 1 ;
                        
                        // set_inter_matrix(get_default_inter_matrix());
                    }
                    if ( video_object_layer_shape == 0x03 ) {
                        
                        // printf("greyscale matrix not supported\n");
                        *pos_o = -1 ;
                        return -1 ;
                    }
                }
                if ( vol_ver_id != 1 ) {
                    
                    // printf("QUARTERPEL BITSTREAM\n");
                    VOLsimple -> quarterpel = showNbits(data, position, 1);
                    position += 1 ;
                    
                    // printf("quaterpel %d\n", VOLsimple -> quarterpel);
                } else {
                    VOLsimple -> quarterpel = 0 ;
                }
                VOLsimple -> complexity_estimation_disable = showNbits(data, position, 1);
                position += 1 ;
                if ( !VOLsimple -> complexity_estimation_disable ) 
                /* complexity_estimation_disable */
                {
                    read_vol_complexity_estimation_header(data, &position);
                }
                VOLsimple -> resync_marker_disable = showNbits(data, position, 1); /* resync_marker_disable */ 
                position += 1 ;
                VOLsimple -> data_partitioned = showNbits(data, position, 1);
                position += 1 ;
                if ( VOLsimple -> data_partitioned ) 
                /* data_partitioned */
                {
                    
                    // printf("data_partitioned not supported\n");
                    VOLsimple -> reversible_vlc = showNbits(data, position, 1); /* reversible_vlc */ 
                    position += 1 ;
                }
                if ( vol_ver_id != 1 ) {
                    VOLsimple -> newpred_enable = showNbits(data, position, 1);
                    position += 1 ;
                    if ( VOLsimple -> newpred_enable ) 
                    /* newpred_enable */
                    {
                        
                        // printf("+ newpred_enable\n");
                        showNbits(data, position, 2); /* requested_upstream_message_type */ 
                        position += 2 ;
                        showNbits(data, position, 1); /* newpred_segment_type */ 
                        position += 1 ;
                    }
                    VOLsimple -> reduced_resolution_enable = showNbits(data, position, 1); /* reduced_resolution_vop_enable */ 
                    position += 1 ;
                    
                    // printf("reduced_resolution_enable %i\n", VOLsimple -> reduced_resolution_enable);
                } else {
                    VOLsimple -> newpred_enable = 0 ;
                    VOLsimple -> reduced_resolution_enable = 0 ;
                }
                scalability = (unsigned char)showNbits(data, position, 1);
                position += 1 ;
                if ( scalability ) 
                /* scalability */
                {
                    
                    // printf("scalability not supported\n");
                    *pos_o = -1 ;
                    return -1 ;
                }
            } else 
            /* dec->shape == BINARY_ONLY */
            {
                if ( vol_ver_id != 1 ) {
                    VOLsimple -> scalability = showNbits(data, position, 1);
                    position += 1 ;
                    if ( VOLsimple -> scalability ) 
                    /* scalability */
                    {
                        
                        // printf("scalability not supported\n");
                        *pos_o = -1 ;
                        return -1 ;
                    }
                }
                showNbits(data, position, 1); /* resync_marker_disable */ 
                position += 1 ;
            }
            
            // printf("VOL\n");
            //			return (-2 );	/* VOL */
        } else if ( start_code == GRPOFVOP_START_CODE ) {
            
            // printf("<group_of_vop>\n");
            showNbits(data, position, 16); /* visual_object_sequence_start_code */ 
            position += 16 ;
            showNbits(data, position, 16); /* visual_object_sequence_start_code */ 
            position += 16 ;
            {
                //int hours, minutes, seconds ;
                showNbits(data, position, 5); //hours
                position += 5 ;
                showNbits(data, position, 6); //minutes
                position += 6 ;
                showNbits(data, position, 1);
                position += 1 ;
                showNbits(data, position, 6); //seconds
                position += 6 ;
                
                // printf("time %ih%im%is\n", hours, minutes, seconds);
            }
            showNbits(data, position, 1); /* closed_gov */ 
            position += 1 ;
            showNbits(data, position, 1); /* broken_link */ 
            position += 1 ;
        } else if ( start_code == VOP_START_CODE ) {
            
            // printf("<vop>\n");
            showNbits(data, position, 16); /* visual_object_sequence_start_code */ 
            position += 16 ;
            showNbits(data, position, 16); /* visual_object_sequence_start_code */ 
            position += 16 ;
            *pos_o = position ;
            return position ;
        } else if ( start_code == USERDATA_START_CODE ) 
        {
            char    tmp [256];
            int     i ;
            
            // printf("<user_data>\n");
            showNbits(data, position, 16); /* user_data_start_code */ 
            position += 16 ;
            showNbits(data, position, 16); /* user_data_start_code */ 
            position += 16 ;
            memset(tmp, 0, 256);
            tmp [0] = (char) showNbits(data, position, 8);
            for ( i = 1 ; i < 256 ; i++ ) {
                tmp [i] = (char) showNbits(data, position, 16);
                if ( tmp [i] == 0 ) {
                    break ;
                }
                showNbits(data, position, 8);
                position += 8 ;
            }
            
            // printf("<user_data>: %s\n", tmp);
            /* read xvid bitstream version */
            if ( strncmp(tmp, "XviD", 4) == 0 ) {
                if ( tmp [strlen(tmp) - 1] == 'C' ) {
                    //					sscanf(tmp, "XviD%dC", version);
                } else {
                    //					sscanf(tmp, "XviD%d", version);
                }
            }
            
            /* divx detection */
            /*            i = sscanf(tmp, "DivX%dBuild%d%c", &version, &build, &packed);
               if ( i < 2 ) 
                   i = sscanf(tmp, "DivX%db%d%c", &version, &build, &packed);
               if ( i >= 2 ) {
                   (i == 3 && packed == 'p');
                   // printf("divx version=%i, build=%i packed=%i\n", version, build, packed);
               }
               */
        } else 
        /* start_code == ? */
        {
            int temp = 0 ;
            temp = showNbits(data, position, 8) << 16 ;
            position += 8 ;
            temp |= showNbits(data, position, 16);
            position += 16 ;
            if ( temp != 1 ) {
                position -= 24 ;
            }
            showNbits(data, position, 8);
            position += 8 ;
        }
    } while ( position<length );
    return(0);
}

void read_vol_complexity_estimation_header ( const unsigned char *RESTRICT data, int *RESTRICT position)
{
    int             method ;
    int             shape_complexity_estimation_disable ;
    int             texture_complexity_estimation_set_2_disable ;
    int             motion_compensation_complexity_disable ;
    int             texture_complexity_estimation_set_1_disable ;
    
    method = showNbits(data, *position, 2); /* estimation_method */ 
    *position += 2 ;
    if ( method == 0 || method == 1 ) {
        shape_complexity_estimation_disable = showNbits(data, *position, 1);
        *position += 1 ;
        if ( !shape_complexity_estimation_disable ) 
        /* shape_complexity_estimation_disable */
        {
            //int   opaque ;
            //int   transparent ;
            //int   intra_cae ;
            //int   inter_cae ;
            //int   no_update ;
            //int   upsampling ;
            showNbits(data, *position, 1); /* opaque */ 
            *position += 1 ;
            showNbits(data, *position, 1); /* transparent */ 
            *position += 1 ;
            showNbits(data, *position, 1); /* intra_cae */ 
            *position += 1 ;
            showNbits(data, *position, 1); /* inter_cae */ 
            *position += 1 ;
            showNbits(data, *position, 1); /* no_update */ 
            *position += 1 ;
            showNbits(data, *position, 1); /* upsampling */ 
            *position += 1 ;
        }
        texture_complexity_estimation_set_1_disable = showNbits(data, *position, 1); 
        *position += 1 ;
        if ( !texture_complexity_estimation_set_1_disable ) 
        /* texture_complexity_estimation_set_1_disable */
        {
            //int   intra_blocks ;
            //int   inter_blocks ;
            //int   inter4v_blocks ;
            //int   not_coded_blocks ;
            showNbits(data, *position, 1); /* intra_blocks */ 
            *position += 1 ;
            showNbits(data, *position, 1); /* inter_blocks */ 
            *position += 1 ;
            showNbits(data, *position, 1); /* inter4v_blocks */ 
            *position += 1 ;
            showNbits(data, *position, 1); /* not_coded_blocks */ 
            *position += 1 ;
        }
    }
    showNbits(data, *position, 1);
    *position += 1 ;
    texture_complexity_estimation_set_2_disable = showNbits(data, *position, 1);
    *position += 1 ;
    if ( !texture_complexity_estimation_set_2_disable ) 
    /* texture_complexity_estimation_set_2_disable */
    {
        //int   dct_coefs ;
        //int   dct_lines ;
        //int   vlc_symbols ;
        //int   vlc_bits ;
        showNbits(data, *position, 1); /* dct_coefs */ 
        *position += 1 ;
        showNbits(data, *position, 1); /* dct_lines */ 
        *position += 1 ;
        showNbits(data, *position, 1); /* vlc_symbols */ 
        *position += 1 ;
        showNbits(data, *position, 1); /* vlc_bits */ 
        *position += 1 ;
    }
    motion_compensation_complexity_disable = showNbits(data, *position, 1);
    *position += 1 ;
    if ( !motion_compensation_complexity_disable ) 
    /* motion_compensation_complexity_disable */
    {
        //int   apm ;
        //int   npm ;
        //int   interpolate_mc_q ;
        //int   forw_back_mc_q ;
        //int   halfpel2 ;
        //int   halfpel4 ;
        showNbits(data, *position, 1); /* apm */ 
        *position += 1 ;
        showNbits(data, *position, 1); /* npm */ 
        *position += 1 ;
        showNbits(data, *position, 1); /* interpolate_mc_q */ 
        *position += 1 ;
        showNbits(data, *position, 1); /* forw_back_mc_q */ 
        *position += 1 ;
        showNbits(data, *position, 1); /* halfpel2 */ 
        *position += 1 ;
        showNbits(data, *position, 1); /* halfpel4 */ 
        *position += 1 ;
    }
    showNbits(data, *position, 1);
    *position += 1 ;
    if ( method == 1 ) {
        int version2_complexity_estimation_disable = showNbits(data, *position, 1);
        *position += 1 ;
        if ( !version2_complexity_estimation_disable ) 
        /* version2_complexity_estimation_disable */
        {
            //int   sadct ;
            //int   quarterpel ;
            showNbits(data, *position, 1); /* sadct */ 
            *position += 1 ;
            showNbits(data, *position, 1); /* quarterpel */ 
            *position += 1 ;
        }
    }
}
