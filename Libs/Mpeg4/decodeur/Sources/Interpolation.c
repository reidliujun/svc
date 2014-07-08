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
#include "mpeg.h"

#ifdef QDMA_6416
#include <QDMA.h>
#endif

void transfer8x8_copy ( const int stride_src, const unsigned char *RESTRICT src
    , unsigned char *RESTRICT dst )
{
#ifdef QDMA_6416
    
    int             i, j ;
    unsigned char   tab_interne [12 * 8];
    unsigned char   *pt_tab_interne ;
    
    pt_tab_interne = tab_interne ;
    *(volatile unsigned int *)QDMA_OPT = (unsigned int)(0x25300000 | QDMA_TCC << 16);
    *(volatile unsigned int *)QDMA_SRC = (unsigned int)src ;
    *(volatile unsigned int *)QDMA_CNT = (unsigned int)(7 << 16) | 3 ;
    *(volatile unsigned int *)QDMA_DST = (unsigned int)pt_tab_interne ;
    *(volatile unsigned int *)QDMA_S_IDX = (unsigned int)(stride_src - 12 << 16);
    do {} while ( (*(volatile Uint32*) EDMA_intTest(QDMA_TCC)) == 0 );
    EDMA_intClear(QDMA_TCC);
    pt_tab_interne += (unsigned int)(src) % 4 ;
    for ( j = 0 ; j < 8 ; j++, pt_tab_interne += 4 ) {
        for ( i = 0 ; i < 8 ; i++ ) 
            *dst++ = *pt_tab_interne++ ;
    }
#else 
    
    int i, j ;
    
    for ( j = 0 ; j < 8 ; j++ ) {
        for ( i = 0 ; i < 8 ; i++ ) {
            dst [j * 8 + i] = src [j * stride_src + i];
        }
    }
#endif
}

void interpolate8x8_halfpel_h ( const int stride_src, const int rounding
    , const unsigned char *RESTRICT src, unsigned char *RESTRICT dst )
{
#ifdef QDMA_6416
    
    int             i, j ;
    unsigned char   tab_interne [12 * 8];
    unsigned char   *pt_tab_interne ;
    const int       rnd = 1 - rounding ;
    
    pt_tab_interne = tab_interne ;
    *(volatile unsigned int *)QDMA_OPT = (unsigned int)(0x25300000 | QDMA_TCC << 16);
    *(volatile unsigned int *)QDMA_SRC = (unsigned int)src ;
    *(volatile unsigned int *)QDMA_CNT = (unsigned int)(((7 << 16) | 3)) ;
    *(volatile unsigned int *)QDMA_DST = (unsigned int)pt_tab_interne ;
    *(volatile unsigned int *)QDMA_S_IDX = (unsigned int)(stride_src - 12 << 16);
    do {} while ( (*(volatile Uint32*) EDMA_intTest(QDMA_TCC)) == 0 );
    EDMA_intClear(QDMA_TCC);
    pt_tab_interne += (unsigned int)(src) % 4 ;
    for ( j = 0 ; j < 8 ; j++, pt_tab_interne += 4 ) {
        for ( i = 0 ; i < 8 ; i++ ) {
            int tot ;
            tot = *(pt_tab_interne) + *(pt_tab_interne + 1);
            pt_tab_interne++ ;
            tot = tot + rnd >> 1 ;
            *dst++ = (unsigned char)tot ;
        }
    }
#else 
    
    int         i, j ;
    int         tot ;
    const int   rnd = 1 - rounding ;
    
    for ( j = 0 ; j < 8 ; j++ ) {
        for ( i = 0 ; i < 8 ; i++ ) {
            tot = (int)src [j * stride_src + i] + (int)src [j * stride_src + i + 1];
            tot = (tot + rnd) >> 1 ;
            dst [j * 8 + i] = (unsigned char)tot ;
        }
    }
#endif
}

void interpolate8x8_halfpel_v ( const int stride_src, const int rounding
    , const unsigned char *RESTRICT src, unsigned char *RESTRICT dst )
{
#ifdef QDMA_6416
    
    int             i, j ;
    unsigned char   tab_interne [12 * 9];
    unsigned char   *pt_tab_interne ;
    const int       rnd = 1 - rounding ;
    
    pt_tab_interne = tab_interne ;
    *(volatile unsigned int *)QDMA_OPT = (unsigned int)(0x25300000 | QDMA_TCC << 16);
    *(volatile unsigned int *)QDMA_SRC = (unsigned int)src ;
    *(volatile unsigned int *)QDMA_CNT = (unsigned int)(8 << 16) | 3 ;
    *(volatile unsigned int *)QDMA_DST = (unsigned int)pt_tab_interne ;
    *(volatile unsigned int *)QDMA_S_IDX = (unsigned int)(stride_src - 12 << 16);
    do {} while ( (*(volatile Uint32*) EDMA_intTest(QDMA_TCC)) == 0 );
    EDMA_intClear(QDMA_TCC);
    pt_tab_interne += (unsigned int)(src) % 4 ;
    for ( j = 0 ; j < 8 ; j++, pt_tab_interne += 4 ) {
        for ( i = 0 ; i < 8 ; i++ ) {
            int tot ;
            tot = *(pt_tab_interne) + *(pt_tab_interne + 12);
            pt_tab_interne++ ;
            tot = tot + rnd >> 1 ;
            *dst++ = (unsigned char)tot ;
        }
    }
#else 
    
    int         i, j ;
    int         tot ;
    const int   rnd = 1 - rounding ;
    
    for ( j = 0 ; j < 8 ; j++ ) {
        for ( i = 0 ; i < 8 ; i++ ) {
            tot = (int)src [j * stride_src + i] + (int)src [j * stride_src + i + stride_src];
            tot = (tot + rnd) >> 1 ;
            dst [j * 8 + i] = (unsigned char)tot ;
        }
    }
#endif
}

void interpolate8x8_halfpel_hv ( const int stride_src,const int rounding
    , const unsigned char *RESTRICT src, unsigned char *RESTRICT dst )
{
#ifdef QDMA_6416
    
    int             i, j ;
    unsigned char   tab_interne [12 * 9];
    unsigned char   *pt_tab_interne ;
    const int       rnd = 2 - rounding ;
    
    pt_tab_interne = tab_interne ;
    *(volatile unsigned int *)QDMA_OPT = (unsigned int)(0x25300000 | QDMA_TCC << 16);
    *(volatile unsigned int *)QDMA_SRC = (unsigned int)src ;
    *(volatile unsigned int *)QDMA_CNT = (unsigned int)(8 << 16) | 3 ;
    *(volatile unsigned int *)QDMA_DST = (unsigned int)pt_tab_interne ;
    *(volatile unsigned int *)QDMA_S_IDX = (unsigned int)(stride_src - 12 << 16);
    do {} while ( (*(volatile Uint32*) EDMA_intTest(QDMA_TCC)) == 0 );
    EDMA_intClear(QDMA_TCC);
    pt_tab_interne += (unsigned int)(src) % 4 ;
    for ( j = 0 ; j < 8 ; j++, pt_tab_interne += 4 ) {
        for ( i = 0 ; i < 8 ; i++ ) {
            int tot ;
            tot = *(pt_tab_interne) + *(pt_tab_interne + 1) + *(pt_tab_interne + 12) + *(pt_tab_interne + 13);
            pt_tab_interne++ ;
            tot = tot + rnd >> 2 ;
            *dst++ = (unsigned char)tot ;
        }
    }
#else 
    
    int j, k ;
    
    k = 0 ;
    if ( rounding ) {
        for ( j = 0 ; j < 8 * 8 ; j += 8 ) {
            dst [j + 0]
                = (unsigned char)((src [k + 0] + src [k + 1] + src [k + stride_src + 0] + src [k + stride_src + 1] + 1)
                    >> 2);
            dst [j + 1]
                = (unsigned char)((src [k + 1] + src [k + 2] + src [k + stride_src + 1] + src [k + stride_src + 2] + 1)
                    >> 2);
            dst [j + 2]
                = (unsigned char)((src [k + 2] + src [k + 3] + src [k + stride_src + 2] + src [k + stride_src + 3] + 1)
                    >> 2);
            dst [j + 3]
                = (unsigned char)((src [k + 3] + src [k + 4] + src [k + stride_src + 3] + src [k + stride_src + 4] + 1)
                    >> 2);
            dst [j + 4]
                = (unsigned char)((src [k + 4] + src [k + 5] + src [k + stride_src + 4] + src [k + stride_src + 5] + 1)
                    >> 2);
            dst [j + 5]
                = (unsigned char)((src [k + 5] + src [k + 6] + src [k + stride_src + 5] + src [k + stride_src + 6] + 1)
                    >> 2);
            dst [j + 6]
                = (unsigned char)((src [k + 6] + src [k + 7] + src [k + stride_src + 6] + src [k + stride_src + 7] + 1)
                    >> 2);
            dst [j + 7]
                = (unsigned char)((src [k + 7] + src [k + 8] + src [k + stride_src + 7] + src [k + stride_src + 8] + 1)
                    >> 2);
            k += stride_src ;
        }
    } else {
        for ( j = 0 ; j < 8 * 8 ; j += 8 ) {
            dst [j + 0]
                = (unsigned char)((src [k + 0] + src [k + 1] + src [k + stride_src + 0] + src [k + stride_src + 1] + 2)
                    >> 2);
            dst [j + 1]
                = (unsigned char)((src [k + 1] + src [k + 2] + src [k + stride_src + 1] + src [k + stride_src + 2] + 2)
                    >> 2);
            dst [j + 2]
                = (unsigned char)((src [k + 2] + src [k + 3] + src [k + stride_src + 2] + src [k + stride_src + 3] + 2)
                    >> 2);
            dst [j + 3]
                = (unsigned char)((src [k + 3] + src [k + 4] + src [k + stride_src + 3] + src [k + stride_src + 4] + 2)
                    >> 2);
            dst [j + 4]
                = (unsigned char)((src [k + 4] + src [k + 5] + src [k + stride_src + 4] + src [k + stride_src + 5] + 2)
                    >> 2);
            dst [j + 5]
                = (unsigned char)((src [k + 5] + src [k + 6] + src [k + stride_src + 5] + src [k + stride_src + 6] + 2)
                    >> 2);
            dst [j + 6]
                = (unsigned char)((src [k + 6] + src [k + 7] + src [k + stride_src + 6] + src [k + stride_src + 7] + 2)
                    >> 2);
            dst [j + 7]
                = (unsigned char)((src [k + 7] + src [k + 8] + src [k + stride_src + 7] + src [k + stride_src + 8] + 2)
                    >> 2);
            k += stride_src ;
        }
    }
    
    /*    for ( j = 0 ; j < 8 ; j++ ) {
           for ( i = 0 ; i < 8 ; i++ ) {
               tot = (int)src [j * stride_src + i] + (int)src [j * stride_src + i + 1]
                   + (int)src [j * stride_src + i + stride_src] + (int)src [j * stride_src + i + stride_src + 1];
               tot = tot + 2 - rounding >> 2 ;
               dst [j * stride_dst + i] = (unsigned char)tot ;
           }
       }
       */
#endif
}

void interpolate8x8_switch ( const int width, const int height, const int edge, const int stride_src, const int x, const int y, const int dx
    , const int dy, const struct_VOP *RESTRICT VOP, const unsigned char *RESTRICT refn, unsigned char *RESTRICT cur )
{
    int ddx, ddy ;
    int posx ;
    int posy ;
    
    switch ( ((dx & 1) << 1) + (dy & 1) ) {
        case 0 : 
            ddx = dx / 2 ;
            ddy = dy / 2 ;
            posy = y + ddy ;
            if ( posy < -edge ) 
                posy = -edge ;
            if ( posy >= height ) 
                posy = height ;
            posx = x + ddx ;
            if ( posx < -edge ) 
                posx = -edge ;
            if ( posx >= width ) 
                posx = width ;
            transfer8x8_copy(stride_src, refn + (int)(posy * stride_src + posx), cur);
            break ;
        case 1 : 
            ddx = dx / 2 ;
            ddy = (dy - 1) / 2 ;
            posy = y + ddy ;
            if ( posy < -edge ) 
                posy = -edge ;
            if ( posy >= height ) 
                posy = height ;
            posx = x + ddx ;
            if ( posx < -edge ) 
                posx = -edge ;
            if ( posx >= width ) 
                posx = width ;
            interpolate8x8_halfpel_v(stride_src, VOP -> rounding, refn + (int)(posy * stride_src + posx), cur);
            break ;
        case 2 : 
            ddx = (dx - 1) / 2 ;
            ddy = dy / 2 ;
            posy = y + ddy ;
            if ( posy < -edge ) 
                posy = -edge ;
            if ( posy >= height ) 
                posy = height ;
            posx = x + ddx ;
            if ( posx < -edge ) 
                posx = -edge ;
            if ( posx >= width ) 
                posx = width ;
            interpolate8x8_halfpel_h(stride_src, VOP -> rounding, refn + (int)(posy * stride_src + posx), cur);
            break ;
        default : 
            ddx = (dx - 1) / 2 ;
            ddy = (dy - 1) / 2 ;
            posy = y + ddy ;
            if ( posy < -edge ) 
                posy = -edge ;
            if ( posy >= height ) 
                posy = height ;
            posx = x + ddx ;
            if ( posx < -edge ) 
                posx = -edge ;
            if ( posx >= width ) 
                posx = width ;
            interpolate8x8_halfpel_hv(stride_src, VOP -> rounding, refn + (int)(posy * stride_src + posx), cur);
            break ;
    }
}
