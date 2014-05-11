/*****************************************************************************
   *
   *  Open SVC Decoder developped in IETR image lab
   *
   *
   *
   *              Médéric BLESTEL <mblestel@insa-rennes.Fr>
   *              Mickael RAULET <mraulet@insa-rennes.Fr>
   *              Alain Maccari <amaccari@insa-rennes.Fr>
   *              http://www.ietr.org/
   *
   *
   *
   *
   *
   * This library is free software; you can redistribute it and/or
   * modify it under the terms of the GNU Lesser General Public
   * License as published by the Free Software Foundation; either
   * version 2 of the License, or (at your option) any later version.
   *
   * This library is distributed in the hope that it will be useful,
   * but WITHOUT ANY WARRANTY; without even the implied warranty of
   * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   * Lesser General Public License for more details.
   *
   * You should have received a copy of the GNU Lesser General Public
   * License along with this library; if not, write to the Free Software
   * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
   *
   *
   * $Id$
   *
   **************************************************************************/

#include "string.h"

#ifdef QDMA_6416

#include <QDMA.h>
#endif
#define EDGE_SIZE 16
#define EDGE_SIZE2 EDGE_SIZE/2

#ifdef TI_OPTIM________
#include <csl_cache.h>
#define RESTRICT restrict
#else
#define RESTRICT 
#endif

#ifdef MMX
#ifdef POCKET_PC
#include <mmintrin.h>
#include <cmnintrin.h> 
#else
#include <emmintrin.h>
#include <mmintrin.h>
#endif
#endif

__inline void set_value_to_buff_side ( const unsigned char * RESTRICT src, const unsigned char * RESTRICT dst
							, const int height, const unsigned int stride_dst
							, const unsigned char edge_size );

__inline void set_value_to_top ( const unsigned char * RESTRICT src, unsigned char * RESTRICT dst
			, const unsigned int stride_dst, const unsigned char edge_size );

__inline void set_value_to_bottom  ( const unsigned char * RESTRICT src, unsigned char * RESTRICT dst
						, unsigned int stride_dst, const unsigned char edge_size  );







/**
This function dublicate the edge pixels by 16 all around the luma picture and by 8 all around the chroma picture 

@param y Luminance buffer
@param u Chrominance Cb buffer.
@param v Chrominance Cr buffer.
@param stride_dst  Width in pixel of the image
@param height		height  Width in pixel of the image

*/

void ImageSetEdges ( unsigned char * RESTRICT y,  unsigned char * RESTRICT u, unsigned char *RESTRICT v
			, const unsigned int stride_dst, const unsigned int height )
{

    unsigned char  * uc_dst1, * uc_dst2 ;
    unsigned char  * uc_src1, * uc_src2 ;
    const unsigned int stride_dst2 = stride_dst / 2 ;
    const unsigned int height2 = height / 2 ;

    /*Y*/
    // Left 
    uc_dst1 =  (y + (EDGE_SIZE * stride_dst)) ;
    uc_src1 = (unsigned char *) (y + (EDGE_SIZE * stride_dst) + EDGE_SIZE) ;
	set_value_to_buff_side (uc_src1,uc_dst1,height,stride_dst,EDGE_SIZE);
	// Right 
    uc_dst2 = y + (EDGE_SIZE* stride_dst) + stride_dst  - EDGE_SIZE;
    uc_src2 = (unsigned char *)(y + (EDGE_SIZE * stride_dst) + stride_dst - EDGE_SIZE - 1);
	set_value_to_buff_side (uc_src2,uc_dst2,height,stride_dst,EDGE_SIZE);
   // Top 
    uc_dst1 = y + (EDGE_SIZE * stride_dst)- stride_dst ;
    uc_src1 = (unsigned char *)y + EDGE_SIZE * stride_dst;
	set_value_to_top (uc_src1,uc_dst1,stride_dst,EDGE_SIZE);   
    // Bottom  
    uc_dst2 = y + (EDGE_SIZE * stride_dst) + stride_dst * height  ;
    uc_src2 = (unsigned char *) (y + stride_dst * (height + EDGE_SIZE - 1));
	set_value_to_bottom (uc_src2,uc_dst2,stride_dst,EDGE_SIZE);   

    /*U*/
    // Left 
    uc_dst1 = u + EDGE_SIZE2 * stride_dst2 ;
    uc_src1 = (unsigned char *)u + EDGE_SIZE2 + EDGE_SIZE2 * stride_dst2 ;
	set_value_to_buff_side (uc_src1,uc_dst1,height2,stride_dst2,EDGE_SIZE2);
    // Right 
    uc_dst2 = u + (EDGE_SIZE2+1) * stride_dst2 - EDGE_SIZE2;
    uc_src2 = (unsigned char *)(u + (EDGE_SIZE2+1) * stride_dst2 - EDGE_SIZE2 - 1);
	set_value_to_buff_side (uc_src2,uc_dst2,height2,stride_dst2,EDGE_SIZE2);
	 // Top 
    uc_dst1 = u + (EDGE_SIZE2-1) * stride_dst2;
    uc_src1 = (unsigned char *)u + EDGE_SIZE2 * stride_dst2;
	set_value_to_top (uc_src1,uc_dst1,stride_dst2,EDGE_SIZE2);   
    // Bottom  
    uc_dst2 = u + stride_dst2 * (height2 + EDGE_SIZE2);
    uc_src2 = (unsigned char *) (u + stride_dst2 * (height2 + EDGE_SIZE2 - 1));
	set_value_to_bottom (uc_src2,uc_dst2,stride_dst2,EDGE_SIZE2);   

    /*V*/
    // Left 
    uc_dst1 = v + EDGE_SIZE2 * stride_dst2 ;
    uc_src1 = (unsigned char *)v + EDGE_SIZE2 + EDGE_SIZE2 * stride_dst2 ;
	set_value_to_buff_side (uc_src1,uc_dst1,height2,stride_dst2,EDGE_SIZE2);
    // Right 
    uc_dst2 = v + (EDGE_SIZE2+1) * stride_dst2 - EDGE_SIZE2;
    uc_src2 = (unsigned char *)(v + (EDGE_SIZE2+1) * stride_dst2 - EDGE_SIZE2 - 1);
	set_value_to_buff_side (uc_src2,uc_dst2,height2,stride_dst2,EDGE_SIZE2);
    // Top 
    uc_dst1 = v + (EDGE_SIZE2-1) * stride_dst2;
    uc_src1 = (unsigned char *)(v + EDGE_SIZE2 * stride_dst2);
	set_value_to_top (uc_src1,uc_dst1,stride_dst2,EDGE_SIZE2);   
    // Bottom 
    uc_dst2 = v + stride_dst2 * (height2 + EDGE_SIZE2);
    uc_src2 = (unsigned char *) (v + stride_dst2 * (height2 + EDGE_SIZE2 - 1));
	set_value_to_bottom (uc_src2,uc_dst2,stride_dst2,EDGE_SIZE2);   
}




/**
This function dublicate the edge pixels on the side of the picture 

@param src Source buffer.
@param dst Destination buffer.
@param height height in pixel of the image.
@param stride_dst  Width in pixel of the image.
@param edge_size Size of the edge.

*/
void set_value_to_buff_side ( const unsigned char * RESTRICT src, const unsigned char * RESTRICT dst
							, const int height, const unsigned int stride_dst
							, const unsigned char edge_size )
{
#ifdef TI_OPTIM

	unsigned short i;
	unsigned int *pull_dst= (unsigned int *)dst;
	unsigned char uc_val,j;
	unsigned int ui_temp;
	const unsigned char cuc_TheEdge = (edge_size>>3);

	for (i=0; i<height; i++) //12-14 cycles
	{
		pull_dst=(unsigned int *)(dst + (i*stride_dst));
		uc_val = src[i*stride_dst];
		ui_temp = (uc_val<<24)+(uc_val<<16)+(uc_val<<8)+uc_val;

#pragma UNROLL(2);
		for(j = cuc_TheEdge ; j!=0 ; j--)
		{
			_amem4(pull_dst++) = ui_temp;
			_amem4(pull_dst++) = ui_temp;
		}
	}
#elif MMX

#ifdef SSE2

	int i;
	unsigned char value;

	__m64 *dst_tmp64 =(__m64 *) dst;
	__m128i *dst_tmp128 =(__m128i *) dst;
	unsigned char *src_tmp =(unsigned char *) src;

	__m64 temp64;
	__m128i temp128;

	if(edge_size == 8)
		for (i = 0; i < height; i++, src_tmp += stride_dst, dst_tmp64 += (stride_dst >> 3))	
		{
			value = *src_tmp;
			temp64 = _mm_set1_pi8 (value);
			*dst_tmp64 = temp64;
		}
	else if(edge_size == 16)
		for (i = 0; i < height; i++, src_tmp += stride_dst, dst_tmp128 += (stride_dst >> 4))	
		{
			value = *src_tmp;

			temp128 = _mm_set1_epi8 ((char)value);
			_mm_storeu_si128(dst_tmp128, temp128);
		}

		_mm_empty();    

#elif POCKET_PC

	unsigned int temp;
	int i;
	unsigned char value;
	unsigned int *dst_tmp =(unsigned int *) dst;

	if(edge_size == 8)
		for (i = 0; i < height; i++)	{
			value = src[i * stride_dst];
			temp = (value << 24) + (value << 16) + (value << 8) + value;
			dst_tmp[i * (stride_dst >> 2)] = temp;
			dst_tmp[i * (stride_dst >> 2) +1] = temp;
		}
	else if(edge_size == 16)
		for (i = 0; i < height; i++)	{
			value = src[i * stride_dst];
			temp = (value << 24) + (value << 16) + (value << 8) + value;
			dst_tmp[i * (stride_dst >> 2)] = temp;
			dst_tmp[i * (stride_dst >> 2) +1] = temp;
			dst_tmp[i * (stride_dst >> 2) +2] = temp;
			dst_tmp[i * (stride_dst >> 2) +3] = temp;
		}

#else

	int i;
	unsigned char value;
	__m64 *dst_tmp64 =(__m64 *) dst;
	unsigned char *src_tmp =(unsigned char *) src;
	__m64 temp64;

	if(edge_size == 8)
		for (i = 0; i < height; i++, src_tmp += stride_dst, dst_tmp64 += (stride_dst >> 3))	
		{
			value = *src_tmp;
			temp64 = _mm_set1_pi8 (value);
			*dst_tmp64 = temp64;
		}
	else if(edge_size == 16)
		for (i = 0; i < height; i++, src_tmp += stride_dst, dst_tmp64 += (stride_dst >> 3))	
		{
			value = *src_tmp;
			temp64 = _mm_set1_pi8 (value);
			*(dst_tmp64+1) = *dst_tmp64 = temp64;
		}

		_mm_empty();    

#endif

#else

	unsigned int temp;
	int i,j;
	unsigned char value;
	unsigned int *dst_tmp =(unsigned int *) dst;

	for (i = 0; i < height; i++)	{
		value = src[i * stride_dst];
		temp = (value << 24) + (value << 16) + (value << 8) + value;
		for(j = 0; j < (edge_size >> 2) ;j++)
			dst_tmp[j + i * (stride_dst >> 2)] = temp;
	}

#endif


}






/**
This function dublicate the edge pixels on the bottom of the picture 

@param src Source buffer.
@param dst Destination buffer.
@param stride_dst  Width in pixel of the image.
@param edge_size Size of the edge.
*/
void set_value_to_bottom  ( const unsigned char * RESTRICT src, unsigned char * RESTRICT dst
						, unsigned int stride_dst, const unsigned char edge_size  )
{
#ifdef TI_OPTIM
{
	unsigned int j;
	unsigned long long * RESTRICT pull_src = (unsigned long long *)src;
	unsigned long long * RESTRICT pull_dst = (unsigned long long *)dst; //Beginning of the dst
	unsigned long long * RESTRICT pull_dst2 = (unsigned long long *)(dst + (edge_size >> 1) * stride_dst);//half of the dst
	const unsigned int cui_StrideDiv8 = (stride_dst >> 3);			// Nbr of char divided by 8
	unsigned long long ull_A;

	for( j = ( (edge_size >> 1) * cui_StrideDiv8 ); j!=0  ; j-- )// deplacement sur la hauteur
	{
		ull_A= *pull_src++;
		_amem8(pull_dst++) = ull_A;
		_amem8(pull_dst2++) = ull_A;
	}
}

#else
	int i;
	for(i = 0; i < edge_size; i++)
		memcpy(dst + i * stride_dst, src, stride_dst);
#endif
}



/**
This function dublicate the edge pixels on the top of the picture 

@param src Source buffer.
@param dst Destination buffer.
@param stride_dst  Width in pixel of the image.
@param edge_size Size of the edge.

*/
void set_value_to_top ( const unsigned char * RESTRICT src, unsigned char * RESTRICT dst
					, const unsigned int stride_dst, const unsigned char edge_size  )
{
#ifdef TI_OPTIM
{

	unsigned int j;
	const unsigned int full_edge_area= (edge_size*stride_dst);
	unsigned long long * RESTRICT pull_src = (unsigned long long *)src;
	unsigned long long * RESTRICT pull_dst  = (unsigned long long *)(src-full_edge_area); //Top of the dst
	unsigned long long * RESTRICT pull_dst2 = (unsigned long long *)(src- (full_edge_area>>1));//half of the dst
	const unsigned int cui_StrideDiv8 = (stride_dst >> 3);			// Nbr of char divided by 8
	unsigned long long ull_A;
	for( j = cui_StrideDiv8 ; j!=0  ; j-- )//
	{
		ull_A= *pull_src++;
		_amem8(pull_dst++) = ull_A;		// On top
		_amem8(pull_dst2++) = ull_A;	// On half
	}
	pull_src  = (unsigned long long *)(src- full_edge_area);	 //On Top 
	pull_dst  = (unsigned long long *)(src- full_edge_area+stride_dst); //Top +1 line
	pull_dst2 = (unsigned long long *)(src-(full_edge_area>>1)+stride_dst);//half +1 line

	for( j = ( ((edge_size>>1)-1)*cui_StrideDiv8 ); j!=0  ; j-- )// 
	{
		ull_A= *pull_src++;
		_amem8(pull_dst++) = ull_A;
		_amem8(pull_dst2++) = ull_A;
	}
}
#else
// MMX does not work well
/*
	int i,j;

	for(i = 0; i < edge_size * stride_dst; i+=stride_dst)
		for(j = 0; j < stride_dst; j+=8)
		*((__m64*)(dst + j - i)) =  *((__m64*)(src + j));
*/

	int i;

	for(i = 0; i < edge_size; i++)
		memcpy(dst - i * stride_dst, src, stride_dst);

#endif
}


