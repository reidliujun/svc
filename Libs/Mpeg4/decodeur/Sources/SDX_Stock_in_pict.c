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

void    Stock_block_in_pict (const int width, const int dep_X, const unsigned char *RESTRICT src
    , unsigned char *RESTRICT dst) ;
void    Stock_block_from_prev_pict_in_pict (const int width, const int pos_X, const unsigned char *RESTRICT src
    , unsigned char *RESTRICT dst) ;
void    Stock_block_from_prev_pict_in_block (const int width_src, const int pos_X, const unsigned char *RESTRICT src
    , unsigned char *RESTRICT dst) ;

void SDX_Stock_block_in_pict ( const int NB_MB, const int width_dst, const int *pos_X
    , const unsigned char *RESTRICT src, unsigned char *RESTRICT dst )
{
    int k ;
    
    for ( k = 0 ; k < NB_MB ; k++ ) {
        Stock_block_in_pict(width_dst, *(pos_X + k), src + k * 64, dst);
    }
}

void SDX_Stock_block_from_prev_pict_in_pict ( const int NB_MB, const int width, const int *pos_X
    , const unsigned char *RESTRICT src, unsigned char *RESTRICT dst )
{
    int k ;
    
    for ( k = 0 ; k < NB_MB ; k++ ) {
        Stock_block_from_prev_pict_in_pict(width, *(pos_X + k), src, dst);
    }
}

void SDX_Stock_blockLum_in_pict ( const int NB_MB, const int width_dst, const int *pos_X1, const int *pos_X2
    , const int *pos_X3, const int *pos_X4, const unsigned char *RESTRICT src_X1, const unsigned char *RESTRICT src_X2
    , const unsigned char *RESTRICT src_X3, const unsigned char *RESTRICT src_X4, unsigned char *RESTRICT dst )
{
    SDX_Stock_block_in_pict(NB_MB, width_dst, pos_X1, src_X1, dst);
    SDX_Stock_block_in_pict(NB_MB, width_dst, pos_X2, src_X2, dst);
    SDX_Stock_block_in_pict(NB_MB, width_dst, pos_X3, src_X3, dst);
    SDX_Stock_block_in_pict(NB_MB, width_dst, pos_X4, src_X4, dst);
}

void SDX_Stock_block_from_prev_pict_in_block ( const int k, const int width, const int *MB_pos
    , const unsigned char *RESTRICT src, unsigned char *RESTRICT dst )
{
    int x_pos, y_pos, pos_X ;
    
    switch ( k ) {
        case 0 : 
            
            // X1
            x_pos = *MB_pos % (width / 16);
            y_pos = *MB_pos / (width / 16);
            pos_X = x_pos * 16 + 16 * y_pos * width ;
            break ;
        case 1 : 
            
            // X2
            x_pos = *MB_pos % (width / 16);
            y_pos = *MB_pos / (width / 16);
            pos_X = x_pos * 16 + 8 + 16 * y_pos * width ;
            break ;
        case 2 : 
            
            // X3
            x_pos = *MB_pos % (width / 16);
            y_pos = *MB_pos / (width / 16);
            pos_X = x_pos * 16 + (16 * y_pos + 8) * width ;
            break ;
        case 3 : 
            
            // X4
            x_pos = *MB_pos % (width / 16);
            y_pos = *MB_pos / (width / 16);
            pos_X = x_pos * 16 + 8 + (16 * y_pos + 8) * width ;
            break ;
        case 4 : 
            
            // XCb
            x_pos = *MB_pos % (width / 8);
            y_pos = *MB_pos / (width / 8);
            pos_X = x_pos * 8 + 8 * y_pos * width ;
            break ;
        case 5 : 
            
            // XCr
            x_pos = *MB_pos % (width / 8);
            y_pos = *MB_pos / (width / 8);
            pos_X = x_pos * 8 + 8 * y_pos * width ;
            break ;
        default : break ;
    }
    Stock_block_from_prev_pict_in_block(width, pos_X, src, dst);
}

void init_blkXn ( short *blkXn )
{
    memset(blkXn, 0, 16 * sizeof(short));
    *blkXn = 1024 ;
}

void calcul_cbp ( const struct_VOP *RESTRICT VOP, int *RESTRICT cbpX1, int *RESTRICT cbpX2, int *RESTRICT cbpX3
    , int *RESTRICT cbpX4, int *RESTRICT cbpXCb, int *RESTRICT cbpXCr )
{
    *cbpX1 = VOP -> cbp >> 5 & 1 ;
    *cbpX2 = VOP -> cbp >> 4 & 1 ;
    *cbpX3 = VOP -> cbp >> 3 & 1 ;
    *cbpX4 = VOP -> cbp >> 2 & 1 ;
    *cbpXCb = VOP -> cbp >> 1 & 1 ;
    *cbpXCr = VOP -> cbp >> 0 & 1 ;
}
