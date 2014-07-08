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

static short  stockage_ligne [XSIZE / 8 / 2 + 1][16];  /* stockage de la ligne précédente avec 22 blocs max sur une ligne */ 
static int  memorisation_synchro = 0 ;

void StockBlocksCb ( const int MBpos, const short *RESTRICT BuffX, const struct_VOLsimple *RESTRICT VOLsimple
    , const int resync_marker, short *RESTRICT BuffA, short *RESTRICT BuffB, short *RESTRICT BuffC )
{
    int pos_i, i, j;
    short nb ;
    
    //  int nb_MB_width; 
    int nb_block_width ;
    
    nb_block_width = VOLsimple -> video_object_layer_width / 16 ;
    pos_i = MBpos % nb_block_width ; /*coordonnées du MB dans l'image */ 
    nb = (short) (2 << (VOLsimple -> bits_per_pixel + 1)) ;
    if ( MBpos == 0 || resync_marker == 1 ) {
        BuffA [0] = nb ; 
        BuffB [0] = nb ;
        BuffC [0] = nb ;
	#ifdef C64LIB
	    for ( i = 0 ; i < 16 ; i+=4 ) 
	    {
	    	_mem8(BuffA+i)=0;
			_mem8(BuffB+i)=0;
			_mem8(BuffC+i)=0;
	    }
        for ( j = 0 ; j < nb_block_width + 1 ; j++ ) 
		{	
            stockage_ligne [j][0] = nb ;
			for ( i = 1 ; i < 16 ; i++ )
                stockage_ligne [j][i] = 0 ;
		}
		BuffA [0] = nb ;  
        BuffB [0] = nb ;
        BuffC [0] = nb ;
	#else
        for ( i = 0 ; i < 16 ; i++ ) {
            BuffA [i] = 0 ;
            BuffB [i] = 0 ;
            BuffC [i] = 0 ;
		}
        /* update */
        for ( j = 0 ; j < nb_block_width + 1 ; j++ ) 
		{	
            stockage_ligne [j][0] = nb ;
			for ( i = 1 ; i < 16 ; i++ )
                stockage_ligne [j][i] = 0 ;
		}
        BuffA [0] = nb ;  
        BuffB [0] = nb ;
        BuffC [0] = nb ;
	#endif
    } else if ( pos_i == 0 ) 
    /* first row */
    {
	#ifdef CHIP_DM642
	    for ( i = 0 ; i < 16 ; i+=4 ) 
	    {
			_mem8(BuffA+i)=0;
			_mem8(BuffB+i)=0;
	    }
        BuffA [0] = nb ;
        BuffB [0] = nb ;
        for ( i = 0 ; i < 16 ; i++ ) {
            BuffC [i] = stockage_ligne [pos_i + 1][i];
            stockage_ligne [nb_block_width][i] = BuffX [i];
		}
	#else
		for ( i = 0 ; i < 16 ; i++ ) {
			BuffA [i] = 0 ;
			BuffB [i] = 0 ;
		}
		BuffA [0] = nb ;
		BuffB [0] = nb ;
		for ( i = 0 ; i < 16 ; i++ ) {
            BuffC [i] = stockage_ligne [pos_i + 1][i];
            stockage_ligne [nb_block_width][i] = BuffX [i];
        }
	#endif
    }
    else 
    {
        for ( i = 0 ; i < 16 ; i++ ) {
            BuffA [i] = BuffX [i];
            BuffB [i] = stockage_ligne [pos_i][i];
            BuffC [i] = stockage_ligne [pos_i + 1][i];
            stockage_ligne [pos_i][i] = BuffX [i];
        }
    }
}
