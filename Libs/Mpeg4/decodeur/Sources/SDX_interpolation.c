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

void    interpolate8x8_switch (const int width, const int height, const int edge, const int stride_src, const int x, const int y, const int dx
    , const int dy, const struct_VOP *RESTRICT VOP, const unsigned char *RESTRICT refn, unsigned char *RESTRICT cur) ;

void SDX_interpolation8x8 ( const int block, const int x_size, const int y_size, const int *RESTRICT MB_pos
    , const vector *RESTRICT current_mv, const int *RESTRICT uv_dx, const int *RESTRICT uv_dy
    , const struct_VOP *RESTRICT VOP, const unsigned char *RESTRICT refLum, const unsigned char *RESTRICT refCb
    , const unsigned char *RESTRICT refCr, unsigned char *RESTRICT block_interpolate )
{
    const unsigned char *imgRef ;
    int                 x_pos, y_pos, x_pos_inter, y_pos_inter, dx, dy, width, height, width_edged ;
    
    x_pos = *MB_pos % (x_size / 16);
    y_pos = *MB_pos / (x_size / 16);
    switch ( block ) {
        case 0 : 
            
            // X1
            width = x_size ;
            height = y_size ;
            width_edged = x_size + 2 * EDGE_SIZE ;
            imgRef = refLum + EDGE_SIZE + EDGE_SIZE * width_edged ;
            x_pos_inter = 16 * x_pos ;
            y_pos_inter = 16 * y_pos ;
            dx = current_mv -> mvs [0].x ;
            dy = current_mv -> mvs [0].y ;
            break ;
        case 1 : 
            
            // X2
            width = x_size ;
            height = y_size ;
            width_edged = x_size + 2 * EDGE_SIZE ;
            imgRef = refLum + EDGE_SIZE + EDGE_SIZE * width_edged ;
            x_pos_inter = 16 * x_pos + 8 ;
            y_pos_inter = 16 * y_pos ;
            dx = current_mv -> mvs [1].x ;
            dy = current_mv -> mvs [1].y ;
            break ;
        case 2 : 
            
            // X3
            width = x_size ;
            height = y_size ;
            width_edged = x_size + 2 * EDGE_SIZE ;
            imgRef = refLum + EDGE_SIZE + EDGE_SIZE * width_edged ;
            x_pos_inter = 16 * x_pos ;
            y_pos_inter = 16 * y_pos + 8 ;
            dx = current_mv -> mvs [2].x ;
            dy = current_mv -> mvs [2].y ;
            break ;
        case 3 : 
            
            // X4
            width = x_size ;
            height = y_size ;
            width_edged = x_size + 2 * EDGE_SIZE ;
            imgRef = refLum + EDGE_SIZE + EDGE_SIZE * width_edged ;
            x_pos_inter = 16 * x_pos + 8 ;
            y_pos_inter = 16 * y_pos + 8 ;
            dx = current_mv -> mvs [3].x ;
            dy = current_mv -> mvs [3].y ;
            break ;
        case 4 : 
            
            // XCb
            width = x_size / 2 ;
            height = y_size / 2 ;
            width_edged = (x_size + 2 * EDGE_SIZE) / 2 ;
            imgRef = refCb + EDGE_SIZE / 2 + EDGE_SIZE / 2 * width_edged ;
            x_pos_inter = 8 * x_pos ;
            y_pos_inter = 8 * y_pos ;
            dx = uv_dx [0];
            dy = uv_dy [0];
            break ;
        case 5 : 
            
            // XCr
            width = x_size / 2 ;
            height = y_size / 2 ;
            width_edged = (x_size + 2 * EDGE_SIZE) / 2 ;
            imgRef = refCr + EDGE_SIZE / 2 + EDGE_SIZE / 2 * width_edged ;
            x_pos_inter = 8 * x_pos ;
            y_pos_inter = 8 * y_pos ;
            dx = uv_dx [0];
            dy = uv_dy [0];
            break ;
        default : break ;
    }
    interpolate8x8_switch(width, height,  (width_edged - width)/2, width_edged, x_pos_inter, y_pos_inter, dx, dy, VOP, imgRef, block_interpolate);
}
