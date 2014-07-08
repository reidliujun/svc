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

#define EDGE_SIZE2 EDGE_SIZE/2

void SDX_image_setedges_compute ( const unsigned char *RESTRICT ref_y, const unsigned char *RESTRICT ref_u
	, const unsigned char *RESTRICT ref_v, unsigned char *RESTRICT y, unsigned char *RESTRICT u
	, unsigned char *RESTRICT v	, const unsigned int edged_width, const unsigned int edged_height
	, const unsigned int width, const unsigned int height );

void copy_image ( const unsigned int *RESTRICT src, unsigned int *RESTRICT dst, const int height, const int stride_src
    , const int stride_dst );
void image_setedges ( const unsigned char *ref_y, const unsigned char *ref_u, const unsigned char *ref_v
    , unsigned char *y, unsigned char *u, unsigned char *v, const int stride_dst, const int height );


void SDX_image_setedges ( const struct_VOLsimple *RESTRICT VOLsimple, const unsigned char *RESTRICT Y_prec
	, const unsigned char *RESTRICT U_prec, const unsigned char *RESTRICT V_prec, unsigned char *RESTRICT refLum
	, unsigned char *RESTRICT refCb, unsigned char *RESTRICT refCr )
{
    
    SDX_image_setedges_compute(Y_prec, U_prec, V_prec, refLum, refCb, refCr, VOLsimple -> video_object_layer_width + 2 * EDGE_SIZE
        , VOLsimple -> video_object_layer_height + 2 * EDGE_SIZE, VOLsimple -> video_object_layer_width
        , VOLsimple -> video_object_layer_height);
}


void SDX_image_setedges_compute ( const unsigned char *RESTRICT ref_y, const unsigned char *RESTRICT ref_u
	, const unsigned char *RESTRICT ref_v, unsigned char *RESTRICT y, unsigned char *RESTRICT u
	, unsigned char *RESTRICT v	, const unsigned int edged_width, const unsigned int edged_height
	, const unsigned int width, const unsigned int height )
{
    unsigned int    edged_width2  = edged_width/2;
    unsigned int    width2 = width/2;
    unsigned int    height2 = height/2;
    unsigned char   *dst ;
    unsigned char   *src ;
    
    /*Y*/
    /******************************* Middle***********************************/
    dst = y  + (EDGE_SIZE * edged_width + EDGE_SIZE);
    src = (unsigned char *)ref_y ;
    copy_image((unsigned int *)src, (unsigned int *)dst, height, width, edged_width);

    /******************************* Middle***********************************/
    dst = u + (EDGE_SIZE2 * edged_width2 + EDGE_SIZE2);
    src = (unsigned char *)ref_u ;
    copy_image((unsigned int *)src, (unsigned int *)dst, height2, width2, edged_width2);

    /* V */
    /******************************* Middle***********************************/
    dst = v + (EDGE_SIZE2 * edged_width2 + EDGE_SIZE2);
    src = (unsigned char *)ref_v ;
    copy_image((unsigned int *)src, (unsigned int *)dst, height2, width2, edged_width2);

    image_setedges (y, u, v, y, u, v, edged_width, height );
    



}

void copy_image ( const unsigned int *RESTRICT src, unsigned int *RESTRICT dst, const int height, const int stride_src
    , const int stride_dst )
{
#ifdef QDMA_6416
	volatile Uint32 test;
    *(volatile unsigned int *)QDMA_OPT = (unsigned int)(0x21B00000 | QDMA_TCC << 16);
    *(volatile unsigned int *)QDMA_SRC = (unsigned int)src ;
    *(volatile unsigned int *)QDMA_CNT = (unsigned int)(height - 1 << 16) | stride_src >> 2 ;
    *(volatile unsigned int *)QDMA_DST = (unsigned int)dst ;
    *(volatile unsigned int *)QDMA_S_IDX = (stride_dst - stride_src << 16);
    do {} while ( (*(volatile Uint32*) EDMA_intTest(QDMA_TCC)) == 0 );
    EDMA_intClear(QDMA_TCC);
#else 
    
    int i, j ;
    
#ifdef TI_OPTIM
    _nassert(((int)src & 0x3) == 0); /* src word aligned */ 
    _nassert(((int)dst & 0x3) == 0); /* dst word aligned */ 
#endif
#ifdef TI_OPTIM
#pragma MUST_ITERATE(4,,4)
#endif
    for ( i = 0 ; i < height ; i++ ) {
#ifdef TI_OPTIM
#pragma MUST_ITERATE(4,,4)
#endif
        for ( j = 0 ; j < stride_src / 4 ; j++ ) 
            *(dst + j) = *(src + j);
        dst += stride_dst / 4 ;
        src += stride_src / 4 ;
    }
#endif
}
