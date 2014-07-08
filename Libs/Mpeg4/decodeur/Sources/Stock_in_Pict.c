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

#ifdef C64LIB
#include <IMG_pix_sat.h>
#endif
#ifdef QDMA_6416
#include <QDMA.h>
#endif

void block_sat ( const short *RESTRICT src, unsigned char *RESTRICT dst )
{
#ifdef TI_OPTIM
    IMG_pix_sat(32, src, dst);
    IMG_pix_sat(32, src + 32, dst + 32);
#else 
    
    int i, j ;
    
    for ( i = 0 ; i < 8 ; i++ ) {
        for ( j = 0 ; j < 8 ; j++ ) {
            if ( *(src + i * 8 + j) <= 0 ) 
                *(dst + i * 8 + j) = 0 ;
            else if ( *(src + i * 8 + j) >= 255 ) 
                *(dst + i * 8 + j) = 255 ;
            else 
                *(dst + i * 8 + j) = (unsigned char)*(src + i * 8 + j);
        }
    }
#endif
}

void Stock_block_in_pict ( const int width, const int dep_X, const unsigned char *RESTRICT src
    , unsigned char *RESTRICT dst )
{
#ifdef QDMA_6416
    QDMA32copy_block8x8_in_pict(dst + dep_X, src, width);
#else 
    
    int i, j ;
    
    for ( i = 0 ; i < 8 ; i++ ) {
        for ( j = 0 ; j < 8 ; j++ ) 
            *(dst + dep_X + i * width + j) = *(src + i * 8 + j);
    }
#endif
}

void Stock_block_from_prev_pict_in_block ( const int width_src, const int pos_X, const unsigned char *RESTRICT src
    , unsigned char *RESTRICT dst )
{
#ifdef QDMA_6416
    QDMA32copy_block8x8_from_pict_in_block(dst, src + pos_X, width_src);
#else 
    
    int i, j ;
    
    for ( i = 0 ; i < 8 ; i++ ) {
        for ( j = 0 ; j < 8 ; j++ ) 
            *(dst + i * 8 + j) = *(src + pos_X + i * width_src + j);
    }
#endif
}

void Stock_block_from_prev_pict_in_pict ( const int width, const int pos_X, const unsigned char *RESTRICT src
    , unsigned char *RESTRICT dst )
{
#ifdef QDMA_6416
    QDMA32copy_block8x8_from_pict_in_pict(dst + pos_X, src + pos_X, width);
#else 
    
    int i, j ;
    
    for ( i = 0 ; i < 8 ; i++ ) {
        for ( j = 0 ; j < 8 ; j++ ) {
            *(dst + pos_X + i * width + j) = *(src + pos_X + i * width + j);
        }
    }
#endif
}

void block_inter ( short *RESTRICT src1, unsigned char *RESTRICT src2, short *RESTRICT dst )
{
#ifdef C64LIB
    
    int             i ;
    short           *src1_2 = src1 ;
    unsigned char   *src2_2 = src2 ;
    short  *dst2 = dst ;
    
    for ( i = 0 ; i < 8 ; i++, src1_2 += 8, src2_2 += 8, dst2 += 8 ) {
        unsigned int    s1 = _mem4(src2_2);
        unsigned int    s2 = _mem4(src2_2 + 4);
        unsigned int    a = _unpklu4(s1);
        unsigned int    b = _unpkhu4(s1);
        unsigned int    c = _unpklu4(s2);
        unsigned int    d = _unpkhu4(s2);
        unsigned int    e = _mem4(src1_2);
        unsigned int    f = _mem4(src1_2 + 2);
        unsigned int    g = _mem4(src1_2 + 4);
        unsigned int    h = _mem4(src1_2 + 6);
        _mem4(dst2) = _add2(a, e);
        _mem4(dst2 + 2) = _add2(b, f);
        _mem4(dst2 + 4) = _add2(c, g);
        _mem4(dst2 + 6) = _add2(d, h);
    }
#else 
    
    int i, j ;
    
    for ( i = 0 ; i < 8 ; i++ ) {
        for ( j = 0 ; j < 8 ; j++ ) 
            *(dst + i * 8 + j) = (short)(*(src2 + i * 8 + j)) + *(src1 + i * 8 + j);
    }
#endif
}
