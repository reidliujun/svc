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

#define EDGE_SIZE2 EDGE_SIZE/2

__inline void set_value_to_buff_side ( const unsigned char *RESTRICT src, unsigned char *RESTRICT dst
	, const int height, const int stride_dst, const int edge_size );
    
	
__inline int set_value_to_top ( const unsigned char *RESTRICT src, unsigned char *RESTRICT dst, const int stride_dst, const int edge_size );

__inline int set_value_to_bottom ( const unsigned char *RESTRICT src, unsigned char *RESTRICT dst, const int stride_dst, const int edge_size );


void image_setedges ( unsigned char *ref_y, unsigned char *ref_u, unsigned char *ref_v
    , unsigned char *y, unsigned char *u, unsigned char *v, const int stride_dst, const int height )
{
    unsigned char   *dst ;
    unsigned char   *src ;
    const unsigned int stride_dst2 = stride_dst / 2 ;
    const unsigned int height2 = height / 2 ;

    /*Y*/
    // Left 
    dst = y + EDGE_SIZE * stride_dst ;
    src = (unsigned char *)ref_y + EDGE_SIZE + EDGE_SIZE * stride_dst ;
	set_value_to_buff_side (src,dst,height,stride_dst,EDGE_SIZE);
    
    // Right 
    dst = y + (EDGE_SIZE+1) * stride_dst - EDGE_SIZE;
    src = (unsigned char *)(ref_y + (EDGE_SIZE+1) * stride_dst - EDGE_SIZE - 1);
	set_value_to_buff_side (src,dst,height,stride_dst,EDGE_SIZE);
    
    // Top 
    dst = y + (EDGE_SIZE-1) * stride_dst;
    src = (unsigned char *)ref_y + EDGE_SIZE * stride_dst;
	set_value_to_top (src,dst,stride_dst,EDGE_SIZE);

    // Bottom  
    dst = y + stride_dst * (height + EDGE_SIZE);
    src = (unsigned char *) (ref_y + stride_dst * (height + EDGE_SIZE - 1));
	set_value_to_bottom (src,dst,stride_dst,EDGE_SIZE);
    
    
    //U 
    // Left 
    dst = u + EDGE_SIZE2 * stride_dst2 ;
    src = (unsigned char *)ref_u + EDGE_SIZE2 + EDGE_SIZE2 * stride_dst2 ;
	set_value_to_buff_side (src,dst,height2,stride_dst2,EDGE_SIZE2);
    
    // Right 
    dst = u + (EDGE_SIZE2+1) * stride_dst2 - EDGE_SIZE2;
    src = (unsigned char *)(ref_u + (EDGE_SIZE2+1) * stride_dst2 - EDGE_SIZE2 - 1);
	set_value_to_buff_side (src,dst,height2,stride_dst2,EDGE_SIZE2);
    
    // Top 
    dst = u + (EDGE_SIZE2-1) * stride_dst2;
    src = (unsigned char *)ref_u + EDGE_SIZE2 * stride_dst2;
	set_value_to_top (src,dst,stride_dst2,EDGE_SIZE2);

    // Bottom  
    dst = u + stride_dst2 * (height2 + EDGE_SIZE2);
    src = (unsigned char *) (ref_u + stride_dst2 * (height2 + EDGE_SIZE2 - 1));
	set_value_to_bottom (src,dst,stride_dst2,EDGE_SIZE2);
    
    // V 
    // Left 
    dst = v + EDGE_SIZE2 * stride_dst2 ;
    src = (unsigned char *)ref_v + EDGE_SIZE2 + EDGE_SIZE2 * stride_dst2 ;
	set_value_to_buff_side (src,dst,height2,stride_dst2,EDGE_SIZE2);
    
    // Right 
    dst = v + (EDGE_SIZE2+1) * stride_dst2 - EDGE_SIZE2;
    src = (unsigned char *)(ref_v + (EDGE_SIZE2+1) * stride_dst2 - EDGE_SIZE2 - 1);
	set_value_to_buff_side (src,dst,height2,stride_dst2,EDGE_SIZE2);
    
    // Top 
    dst = v + (EDGE_SIZE2-1) * stride_dst2;
    src = (unsigned char *)ref_v + EDGE_SIZE2 * stride_dst2;
	set_value_to_top (src,dst,stride_dst2,EDGE_SIZE2);


    // Bottom  
    dst = v + stride_dst2 * (height2 + EDGE_SIZE2);
    src = (unsigned char *) (ref_v + stride_dst2 * (height2 + EDGE_SIZE2 - 1));
	set_value_to_bottom (src,dst,stride_dst2,EDGE_SIZE2);
	


}


void set_value_to_buff_side ( const unsigned char *RESTRICT src, unsigned char *RESTRICT dst
	, const int height, const int stride_dst, const int edge_size )
{
	int temp,i,j;
	unsigned int *dst_tmp;

	dst_tmp=(unsigned int *) dst;
#ifdef TI_OPTIM
	//int i;
	for (i=0;i<height;i++)
	{
		unsigned char value = src[i*stride_dst];
		temp = _packl4( _pack2 (value,value), _pack2 (value,value));
		for(j=0;j<(edge_size>>2);j++)
			dst_tmp[j+i*(stride_dst>>2)] = temp;
  	
	}
#else
	for (i=0;i<height;i++)
	{
		temp = (*(src + i*stride_dst) << 24) + (*(src + i*stride_dst) << 16) + (*(src + i*stride_dst) << 8) + *(src + i*stride_dst) ;
		for(j=0;j<(edge_size>>2);j++)
			dst_tmp[j+i*(stride_dst>>2)] = temp;
  	}
#endif
}

int set_value_to_bottom ( const unsigned char *RESTRICT src, unsigned char *RESTRICT dst, const int stride_dst, const int edge_size  )
{
	int i;
	for(i=0;i<edge_size;i++)
		memcpy(dst+i*stride_dst,src,stride_dst);
	return 0;

}


int set_value_to_top ( const unsigned char *RESTRICT src, unsigned char *RESTRICT dst, const int stride_dst, const int edge_size  )
{
	int i;
	for(i=0;i<edge_size;i++)
		memcpy(dst-i*stride_dst,src,stride_dst);
	return 0;

}
