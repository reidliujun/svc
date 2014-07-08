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
#include "mpeg.h"

/*#include<math.h>*/
/*recupère N bits (1<N<8) dans le tableau de caractères et les mets dans un caractère, position est incrémenté */
/*fonction 6.2.5.1 de la norme non faite encore et qui rempli la structure read_vop_complexity*/
char                read_vop_complexity_estimation_header (const unsigned char *RESTRICT data, int *RESTRICT position
    , const unsigned char *RESTRICT vop_complexity, struct_read_vop_complexity *RESTRICT read_vop_complexity) ;
static const int    intra_dc_threshold_table [] = { 32, /* never use */ 13, 15, 17, 19, 21, 23, 1, };

/* static : VideoObjectPlane -> 1 seul appel */
static int          time = 0 ;            //xvid : long
static int          time_base = 0 ;       //xvid : long
static int          last_time_base = 0 ;  //xvid : long
static int          last_non_b_time = 0 ; //xvid : long

void VideoObjectPlaneI ( int position_vol, const unsigned char *RESTRICT data, const struct_VOLsimple *RESTRICT VOLsimple
    , const unsigned char *RESTRICT vop_complexity, int *RESTRICT pos, struct_VOP *RESTRICT VOP
    , int *RESTRICT vop_coding_type )
{
    
    /* mes variables locales */
    //int position;
    
    int vop_time_increment ;
    int vop_time_increment_size ;
    int time_incr = 0 ;
    
    /*intra_dc_vlc_thr */
    /***********************/
    /* Function's Body     */
    /***********************/
    /*  LOG_printf("I'm in function: VideoObjectPlaneI_c generated from method: VideoObjectPlaneI.update\n");*/
    /* cf 6.2.5 de la norme */
    *pos = position_vol ;
    VOP -> bound = 0 ;
    VOP -> vop_coding_type = showNbits(data, *pos, 2); /* vérification du codage intra */ 
    *pos += 2 ;
    
    // LOG_printf("coding_type %d\n", VOP -> vop_coding_type);
    /*  if ( VOP -> vop_coding_type == 0 ) 
       {
       */
    /* LOG_printf("\ndécodage d'un VOP codé Intra\n"); */
    do {
        VOP -> modulo_time_base = showNbits(data, *pos, 1);
        *pos += 1 ;
        if ( VOP -> modulo_time_base != 0 ) 
            time_incr++ ;
        
        /* on récupère les bits mais on ne les traite pas ... */
    } while ( VOP -> modulo_time_base != 0 );
    showNbits(data, *pos, 1); //marker_bit
    *pos += 1 ;
	vop_time_increment_size = VOLsimple -> time_inc_bits;
    if ( vop_time_increment_size < 1 && vop_time_increment_size > 16 ) {
        
        // LOG_printf("vop_time_increment à une taille entre 1 et 16 bits ... erreur !!\n");
    } else {
        vop_time_increment = showNbits(data, *pos, vop_time_increment_size);
        *pos += vop_time_increment_size ;
    }
    if ( VOP -> vop_coding_type != 2 ) {
        last_time_base = time_base ;
        time_base += time_incr ;
        time = time_base * VOLsimple -> vop_time_increment_resolution + vop_time_increment ;
        VOP -> time_pp = time - last_non_b_time ;
        last_non_b_time = time ;
    } else {
        time = (last_time_base + time_incr) * VOLsimple -> vop_time_increment_resolution + vop_time_increment ;
        VOP -> time_bp = VOP -> time_pp - (last_non_b_time - time);
    }
    showNbits(data, *pos, 1); //marker_bit
    *pos += 1 ;
    VOP -> vop_coded = showNbits(data, *pos, 1);
    *pos += 1 ;
    if ( VOP -> vop_coded == 0 ) {
        VOP -> vop_coding_type = 4 ;
        *vop_coding_type = (int)VOP -> vop_coding_type ;
        
        /*LOG_printf("vop non codé\n"); */
    } else {
        if ( VOP -> vop_coding_type == 1 ) {
            VOP -> rounding = showNbits(data, *pos, 1); /* rounding_type pour les P*/ 
            *pos += 1 ;
        }
        /*if ( !VOLsimple->complexity_estimation_disable ) ;
           read_vop_complexity_estimation_header(data, pos, vop_complexity, read_vop_complexity);*/
        VOP -> intra_dc_vlc_thr = intra_dc_threshold_table [showNbits(data, *pos, 3)]; /*cf table*/ 
        *pos += 3 ;
        VOP -> quant = showNbits(data, *pos, VOLsimple -> quant_precision);
        *pos += VOLsimple -> quant_precision ;
        
        // LOG_printf("quant %d\n", VOP -> quant);
        if ( VOP -> vop_coding_type != 0 ) 
        /*pour les P*/
        {
            VOP -> fcode_forward = showNbits(data, *pos, 3); /* fcode_forward */ 
            *pos += 3 ;
        }
        if ( VOP -> vop_coding_type == 2 ) 
        /*pour les B*/
        {
            VOP -> fcode_backward = showNbits(data, *pos, 3); /* fcode_forward */ 
            *pos += 3 ;
        }
        *vop_coding_type = (int)VOP -> vop_coding_type ;
        
        //    }
        /*  }
           else 
             LOG_printf("le VOP n'est pas codé INTRA \n");
           */
        //return(1);
    }
}
