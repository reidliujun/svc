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
/* gestion des resync_marker et passage dans Video_Packet_Header 6.2.5.1 du standard */
/* attention, si on passe plusieurs fois dans cette fonction, il y aura p'tre des pb de mémorisations ... 
   dans ce cas, mettre la structure en variable globale et ne prendre les paramètres de VOP que pour un nouveau VOP !! */
#include <string.h>
#include "mpeg.h"

/* mémorisation du macrobloc courant */
//static int MB_courant=0;
//static int                  dernier_MB = 1 ;
static const unsigned int   intra_dc_threshold_table [] = { 32, /* never use */ 13, 15, 17, 19, 21, 23, 1, };

void VideoPacketHeaderI ( const unsigned char *RESTRICT data, const int pos_i, struct_VOLsimple *RESTRICT VOLsimple
    , struct_VOP *RESTRICT VOP, const int *RESTRICT MB_courant, struct_VOP *RESTRICT newVOP, int *RESTRICT pos_o
    , int *RESTRICT resync_marker, int *RESTRICT MB_num )
{
    
	/* variables locales */
	int posbytealigned, valbytealigned, MB_num_change ;
	int macroblock_number, macroblock_number_length, val, comp, bits, bits2 ;
	int header_extension_code ;
	unsigned int temp ;

	MB_num_change = 0 ;
	*pos_o = pos_i ;
	*newVOP = *VOP ;
	while ( showNbits(data, *pos_o, 9) == 1 ) 
		*pos_o += 9 ;

	*resync_marker = 0 ;
	newVOP -> resync_marker = 0 ;
	if ( *MB_courant != 0 ) {
		if ( VOLsimple -> resync_marker_disable == 0 ) {
			posbytealigned = (*pos_o / 8 + 1) * 8 ;
			if (showNbits(data, *pos_o, posbytealigned - *pos_o)== (((unsigned int)1 << (posbytealigned - *pos_o - 1)) - 1) )
			{	
				valbytealigned = showNbits(data, posbytealigned, 17);
				posbytealigned += 17 ;
				if ( valbytealigned == 1 ) {
					*resync_marker = 1 ;
					newVOP -> resync_marker = 1 ;
					*pos_o = posbytealigned ;

					/* video_packet_header 6.2.5.2 */
					val = (VOLsimple -> video_object_layer_width + 15) / 16
						* ((VOLsimple -> video_object_layer_height + 15) / 16);

					/* implantation optimisée de la table 6.23 */
					macroblock_number_length = 0 ;
					comp = 1 ;
					do {
						macroblock_number_length++ ;
						comp = comp * 2 ;
					} while ( val >= comp );
					macroblock_number = showNbits(data, *pos_o, macroblock_number_length);
					*pos_o += macroblock_number_length ;
					newVOP -> bound = macroblock_number ;
					*MB_num = macroblock_number ;
					MB_num_change = 1 ;
					newVOP->quant = showNbits(data, *pos_o, 5); 
					*pos_o += 5;
					header_extension_code = showNbits(data, *pos_o, 1);
					*pos_o += 1 ;
					if ( header_extension_code ) {
						do {
							newVOP -> modulo_time_base = showNbits(data, *pos_o, 1);
							*pos_o += 1 ;

							/* on récupère les bits mais on ne les traite pas ... */
						} while ( newVOP -> modulo_time_base != 0 );
						bits2 = VOLsimple -> vop_time_increment_resolution ;
						bits = log_base2(bits2);
						if ( bits < 1 ) 
							bits = 1 ;
						*pos_o += 1 ; // MARKER_BIT
						showNbits(data, *pos_o, bits);//vop_time_increment
						*pos_o += bits ;
						*pos_o += 1 ; //MARKER_BIT
						temp = showNbits(data, *pos_o, 8);
						//newVOP -> vop_coding_type = showNbits(data, *pos_o, 2);
						newVOP -> vop_coding_type = temp >> 6 ;
						//*pos_o += 2 ;
						//newVOP -> intra_dc_vlc_thr = intra_dc_threshold_table [showNbits(data, *pos_o, 3)];
						newVOP -> intra_dc_vlc_thr = intra_dc_threshold_table [temp << 26 >> 29];
						//*pos_o += 3 ;
						*pos_o += 5 ;
						if ( newVOP -> vop_coding_type != 0 ) {

							/* 0 == I_VOP */
							*pos_o += 3 ; /*fcode_forward*/ 
						}
						if ( newVOP -> vop_coding_type == 2 ) {

							/* 2 == B_VOP */
							*pos_o += 3 ; /*fcode_backward*/ 
						}
					}
				}
			}
		}
	}
	if ( MB_num_change == 0 ) 
		*MB_num = *MB_courant ;
}

void VideoPacketHeaderP ( const unsigned char *RESTRICT data, const int pos_i, struct_VOLsimple *RESTRICT VOLsimple
    , struct_VOP *RESTRICT VOP, const int *RESTRICT MB_courant, struct_VOP *RESTRICT newVOP, int *RESTRICT pos_o
    , int *RESTRICT resync_marker, int *RESTRICT MB_num )
{
    
    /* variables locales */
    int posbytealigned, valbytealigned, MB_num_change ;
    int macroblock_number, macroblock_number_length, val, comp, bits, bits2 ;
    int header_extension_code ;
    int temp ;
    
    MB_num_change = 0 ;
    *pos_o = pos_i ;
    *newVOP = *VOP ;
    while ( showNbits(data, *pos_o, 10) == 1 ) 
        *pos_o += 10 ;
    
    *resync_marker = 0 ;
    newVOP -> resync_marker = 0 ;
    if ( *MB_courant != 0 ) {
		if ( VOLsimple -> resync_marker_disable == 0 ) {
			posbytealigned = (*pos_o / 8 + 1) * 8 ;
			if ( showNbits(data, *pos_o, posbytealigned - *pos_o) == (((unsigned int)1 << (posbytealigned - *pos_o - 1)) - 1) )
			{	
				valbytealigned = showNbits(data, posbytealigned, 17 + VOP->fcode_forward - 1);
				posbytealigned += 17 + VOP->fcode_forward - 1 ;
				if ( valbytealigned == 1 ) {
					*resync_marker = 1 ;
					newVOP -> resync_marker = 1 ;
					*pos_o = posbytealigned ;

					/* video_packet_header 6.2.5.2 */
					val = (VOLsimple -> video_object_layer_width + 15) / 16
						* ((VOLsimple -> video_object_layer_height + 15) / 16);

					macroblock_number_length = 0 ;
					comp = 1 ;
					do {
						macroblock_number_length++ ;
						comp = comp * 2 ;
					} while ( val >= comp );
					macroblock_number = showNbits(data, *pos_o, macroblock_number_length);
					*pos_o += macroblock_number_length ;
					newVOP -> bound = macroblock_number ;
					*MB_num = macroblock_number ;
					MB_num_change = 1 ;
					newVOP->quant = showNbits(data, *pos_o, 5); 
					*pos_o += 5;
					header_extension_code = showNbits(data, *pos_o, 1);
					*pos_o += 1 ;
					if ( header_extension_code ) {
						do {
							newVOP -> modulo_time_base = showNbits(data, *pos_o, 1);
							*pos_o += 1 ;

							/* on récupère les bits mais on ne les traite pas ... */
						} while ( newVOP -> modulo_time_base != 0 );
						bits2 = VOLsimple -> vop_time_increment_resolution ;
						bits = log_base2(bits2);
						if ( bits < 1 ) 
							bits = 1 ;
						*pos_o += 1 ; // MARKER_BIT
						showNbits(data, *pos_o, bits); //vop_time_increment
						*pos_o += bits ;
						*pos_o += 1 ; //MARKER_BIT
						temp = showNbits(data, *pos_o, 8);
						//newVOP -> vop_coding_type = showNbits(data, *pos_o, 2);
						newVOP -> vop_coding_type = temp >> 6 ;
						//*pos_o += 2 ;
						//newVOP -> intra_dc_vlc_thr = intra_dc_threshold_table [showNbits(data, *pos_o, 3)];
						newVOP -> intra_dc_vlc_thr = intra_dc_threshold_table [temp << 26 >> 29];
						//*pos_o += 3 ;
						*pos_o += 5 ;
						if ( newVOP -> vop_coding_type != 0 ) {

							/* 0 == I_VOP */
							*pos_o += 3 ; /*fcode_forward*/ 
						}
						if ( newVOP -> vop_coding_type == 2 ) {

							/* 2 == B_VOP */
							*pos_o += 3 ; /*fcode_backward*/ 
						}
					}
				}
			}
		}
	}
    if ( MB_num_change == 0 ) 
        *MB_num = *MB_courant ;
}

void VideoPacketHeaderB ( const unsigned char *RESTRICT data, const int pos_i, struct_VOLsimple *RESTRICT VOLsimple
    , struct_VOP *RESTRICT VOP, const int *RESTRICT MB_courant, struct_VOP *RESTRICT newVOP, int *RESTRICT pos_o
    , int *RESTRICT resync_marker, int *RESTRICT MB_num )
{
    
    /* variables locales */
    int posbytealigned, valbytealigned, MB_num_change ;
    int macroblock_number, macroblock_number_length, val, comp, bits, bits2 ;
    int header_extension_code ;
    int temp ;
    
    MB_num_change = 0 ;
    *pos_o = pos_i ;
    *newVOP = *VOP ;
    
    *resync_marker = 0 ;
    newVOP -> resync_marker = 0 ;
    if ( *MB_courant != 0 ) {
        if ( VOLsimple -> resync_marker_disable == 0 ) {
			posbytealigned = (*pos_o / 8 + 1) * 8 ;
			if (showNbits(data, *pos_o, posbytealigned - *pos_o)== (((unsigned int)1 << (posbytealigned - *pos_o - 1)) - 1) )
			{   
				const int fcode_max = (VOP -> fcode_forward > VOP -> fcode_backward) ? VOP->fcode_forward : VOP -> fcode_backward;

				valbytealigned = showNbits(data, posbytealigned, 17 + fcode_max - 1);
				posbytealigned += 17 + fcode_max - 1 ;
				if ( valbytealigned == 1 ) {
					*resync_marker = 1 ;
					newVOP -> resync_marker = 1 ;
					*pos_o = posbytealigned ;

					/* video_packet_header 6.2.5.2*/
					val = (VOLsimple -> video_object_layer_width + 15) / 16
						* ((VOLsimple -> video_object_layer_height + 15) / 16);

					macroblock_number_length = 0 ;
					comp = 1 ;
					do {
						macroblock_number_length++ ;
						comp = comp * 2 ;
					} while ( val >= comp );
					macroblock_number = showNbits(data, *pos_o, macroblock_number_length);
					*pos_o += macroblock_number_length ;
					newVOP -> bound = macroblock_number ;
					//*MB_courant = macroblock_number ;
					*MB_num = macroblock_number ;
					MB_num_change = 1 ;
					newVOP->quant = showNbits(data, *pos_o, 5); 
					*pos_o += 5;
					header_extension_code = showNbits(data, *pos_o, 1);
					*pos_o += 1 ;
					if ( header_extension_code ) {
						do {
							newVOP -> modulo_time_base = showNbits(data, *pos_o, 1);
							*pos_o += 1 ;

						} while ( newVOP -> modulo_time_base != 0 );
						bits2 = VOLsimple -> vop_time_increment_resolution ;
						bits = log_base2(bits2);
						if ( bits < 1 ) 
							bits = 1 ;
						*pos_o += 1 ; // MARKER_BIT
						showNbits(data, *pos_o, bits); //vop_time_increment
						*pos_o += bits ;
						*pos_o += 1 ; //MARKER_BIT
						temp = showNbits(data, *pos_o, 8);
						//newVOP -> vop_coding_type = showNbits(data, *pos_o, 2);
						newVOP -> vop_coding_type = temp >> 6 ;
						//*pos_o += 2 ;
						//newVOP -> intra_dc_vlc_thr = intra_dc_threshold_table [showNbits(data, *pos_o, 3)];
						newVOP -> intra_dc_vlc_thr = intra_dc_threshold_table [temp << 26 >> 29];
						//*pos_o += 3 ;
						*pos_o += 5 ;
						if ( newVOP -> vop_coding_type != 0 ) {

							/* 0 == I_VOP */
							*pos_o += 3 ; /*fcode_forward*/ 
						}
						if ( newVOP -> vop_coding_type == 2 ) {

							/* 2 == B_VOP */
							*pos_o += 3 ; /*fcode_backward*/ 
						}
					}
				}
			}
		}
	}
    if ( MB_num_change == 0 ) 
        *MB_num = *MB_courant ;
}
