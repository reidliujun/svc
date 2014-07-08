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
#include "./include/mpeg.h"

static int  memorisation_synchro = 0 ;

void SDX_StockBlocksCb ( const int MBpos, const short *RESTRICT BuffX, const struct_VOLsimple *RESTRICT VOLsimple
    , const int resync_marker, const short *RESTRICT stockage_ligne_prec, short *RESTRICT BuffA
    , short *RESTRICT BuffB, short *RESTRICT BuffC, short *RESTRICT stockage_ligne )
{
    int pos_j, pos_i, i, j;
    short nb ;
    
    //  int nb_MB_width; 
    int nb_block_width ;
    
    memcpy(stockage_ligne, stockage_ligne_prec, VOLsimple -> video_object_layer_width / 8 / 2 * 16 * sizeof(short));
    nb_block_width = VOLsimple -> video_object_layer_width / 16 ;
    pos_i = MBpos % nb_block_width ; /*coordonnées du MB dans l'image */ 
    pos_j = MBpos / nb_block_width ;
    nb = (short) (2 << (VOLsimple -> bits_per_pixel + 1)) ;
    if ( resync_marker == 1 ) 
        memorisation_synchro = 0 ;
    if ( MBpos == 0 || resync_marker == 1 ) {
        BuffA [0] = nb ; /* cf 7.4.3.1 DC & AC prediction direction */ 
        BuffB [0] = nb ;
        BuffC [0] = nb ;
        for ( j = 0 ; j < nb_block_width ; j++ ) 
            stockage_ligne [j * 16] = 0 ;
        for ( i = 1 ; i < 16 ; i++ ) {
            BuffA [i] = 0 ;
            BuffB [i] = 0 ;
            BuffC [i] = 0 ;
            
            /* mise a jour */
            for ( j = 0 ; j < nb_block_width ; j++ ) 
                stockage_ligne [j * 16 + i] = 0 ;
        }
    } else if ( pos_j == 0 || memorisation_synchro < nb_block_width ) 
    /* initialisation des valeurs pour un macroblock de la première ligne */
    {
        for ( i = 0 ; i < 16 ; i++ ) {
            BuffA [i] = BuffX [i];
            if ( i == 0 ) {
                BuffB [i] = nb ;
                BuffC [i] = nb ;
            } else {
                BuffB [i] = 0 ;
                BuffC [i] = 0 ;
            }
            if ( pos_i > 0 ) 
                /* normal !!*/
                stockage_ligne [(pos_i - 1) * 16 + i] = BuffX [i];
        }
        if ( pos_i == 0 ) 
            BuffA [0] = nb ;
    }
    else if ( pos_i == 0 ) 
    /* initialisation des valeurs pour un macroblock de la colonne ligne */
    {
        for ( i = 0 ; i < 16 ; i++ ) {
            if ( i == 0 ) {
                BuffA [i] = nb ;
                BuffB [i] = nb ;
            } else {
                BuffA [i] = 0 ;
                BuffB [i] = 0 ;
            }
            BuffC [i] = stockage_ligne [i];
            stockage_ligne [(nb_block_width - 1) * 16 + i] = BuffX [i];
        }
    }
    else 
    {
        for ( i = 0 ; i < 16 ; i++ ) {
            BuffA [i] = BuffX [i];
            BuffB [i] = stockage_ligne [(pos_i - 1) * 16 + i];
            BuffC [i] = stockage_ligne [(pos_i)*16 + i];
            if ( pos_i > 0 ) 
                stockage_ligne [(pos_i - 1) * 16 + i] = BuffX [i];
        }
    }
    memorisation_synchro++ ;
}
