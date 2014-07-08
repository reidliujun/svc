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
#include "mpeg.h"

/* pour une image CIF, il y a 22 MB de largeur, soit 44 blocks sur une ligne */
static short  stockage_ligne [XSIZE / 8 + 3][16]; /* stockage de la ligne précédente avec 44 blocs max sur une ligne */ 

//static int  memorisation_synchro = 0 ;
void StockBlocksLum ( const int pos_i, const int resync_marker, const short *RESTRICT BuffX2
    , const short *RESTRICT BuffX3, const short *RESTRICT BuffX4, const struct_VOLsimple *RESTRICT VOLsimple
    , short *RESTRICT BuffA, short *RESTRICT BuffB, short *RESTRICT BuffC, short *RESTRICT BuffD, short *RESTRICT BuffE )
{
    int         i, j ;
    const int   nb_MB_width = VOLsimple -> video_object_layer_width >> 4 ;
    const int   nb_block_width = nb_MB_width * 2 ;
    const short   nb = (short)(2 << (VOLsimple -> bits_per_pixel + 1)) ;
    
    if ( resync_marker == 0 ) {
        for ( i = 0 ; i < 16 ; i++ ) {
            BuffB [i] = stockage_ligne [pos_i * 2][i];
            BuffC [i] = stockage_ligne [pos_i * 2 + 1][i];
            BuffD [i] = stockage_ligne [pos_i * 2 + 2][i];
        }
        if ( pos_i != 0 ) {
            for ( i = 0 ; i < 16 ; i++ ) {
                BuffA [i] = BuffX2 [i];
                BuffE [i] = BuffX4 [i];
                stockage_ligne [pos_i * 2 - 1][i] = BuffX3 [i];
                stockage_ligne [pos_i * 2][i] = BuffX4 [i];
            }
        } else {
            
            // initialisation des valeurs pour un macroblock de la colonne ligne
			#ifdef C64LIB
            for ( i = 0 ; i < 16 ; i+=4 )
			{
				_mem8(BuffA+i)=0;
				_mem8(BuffE+i)=0;
            }
            BuffA [0] = nb ;
            BuffE [0] = nb ;

			#else
            for ( i = 0 ; i < 16 ; i++ ) {
                BuffA [i] = 0 ;
            }
            for ( i = 0 ; i < 16 ; i++ ) {
                BuffE [i] = 0 ;
            }
            BuffA [0] = nb ;
            BuffE [0] = nb ;

			#endif
            for ( i = 0 ; i < 16 ; i++ ) {
                stockage_ligne [nb_block_width - 1][i] = BuffX3 [i];
                stockage_ligne [nb_block_width][i] = BuffX4 [i];
            }
        }
    } else {
	        for ( j = 0 ; j < nb_block_width + 1 ; j++ ) 
	            for ( i = 1 ; i < 16 ; i++ ) 
	                stockage_ligne [j][i] = 0 ;
	    for ( j = 0 ; j < nb_block_width + 1 ; j++ ) 
	        stockage_ligne [j][0] = nb ;
		#ifdef C64LIB
	        for ( i = 0 ; i < 16 ; i+=4 ) 
	        {
	        	_mem8(BuffA+i)=0;
				_mem8(BuffB+i)=0;
				_mem8(BuffC+i)=0;
				_mem8(BuffD+i)=0;
				_mem8(BuffE+i)=0;
	        }
		#else
	        
	        for ( i = 1 ; i < 16 ; i++ ) 
	        {
	            BuffA [i] = 0 ;
	            BuffB [i] = 0 ;
	            BuffC [i] = 0 ;
	            BuffD [i] = 0 ;
	            BuffE [i] = 0 ;
	        }
	    #endif
	        BuffA [0] = nb ;
	        BuffE [0] = nb ;
	        BuffB [0] = nb ;
	        BuffC [0] = nb ;
	        BuffD [0] = nb ;
		
    }
}

void StockBlocksLum_init ( const struct_VOLsimple *RESTRICT VOLsimple, short *RESTRICT BuffX3, short *RESTRICT BuffX4 )
{
    int         i, j ;
    const int   nb_MB_width = VOLsimple -> video_object_layer_width >> 4 ;
    const int   nb_block_width = nb_MB_width * 2 ;
    const short   nb = (short)(2 << (VOLsimple -> bits_per_pixel + 1)) ;
	
	    for ( j = 0 ; j < nb_block_width + 1 ; j++ ) 
	        for ( i = 1 ; i < 16 ; i++ ) 
	            stockage_ligne [j][i] = 0 ;
	    for ( j = 0 ; j < nb_block_width + 1 ; j++ ) 
	        stockage_ligne [j][0] = nb ;
	#ifdef C64LIB
	    for ( i = 0 ; i < 16 ; i+=4 ) 
	    {
	    	_mem8(BuffX3+i)=0;
			_mem8(BuffX4+i)=0;
	    }
	#else
	    for ( i = 1 ; i < 16 ; i++ ) {
	        BuffX3 [i] = 0 ;
	        BuffX4 [i] = 0 ;
	    }
	#endif
	    BuffX3 [0] = 1024 ;
	    BuffX4 [0] = 1024 ;
}
