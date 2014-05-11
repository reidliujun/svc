/*****************************************************************************
   *
   *  Open SVC Decoder developped in IETR image lab
   *
   *
   *
   *              Médéric BLESTEL <mblestel@insa-rennes.Fr>
   *              Mickael RAULET <mraulet@insa-rennes.Fr>
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





#ifndef C_EDGE_FILTER
#define C_EDGE_FILTER

#include "type.h"

#define weak_vertical_luma weak_vertical_luma_C
#define strong_vertical_luma strong_vertical_luma_C

#define weak_horizontal_luma weak_horizontal_luma_C
#define strong_horizontal_luma strong_horizontal_luma_C

#define weak_vertical_chroma weak_vertical_chroma_C
#define strong_vertical_chroma strong_vertical_chroma_C

#define weak_horizontal_chroma weak_horizontal_chroma_C
#define strong_horizontal_chroma strong_horizontal_chroma_C




#ifdef TI_OPTIM
#undef weak_vertical_luma 
#undef strong_vertical_luma 
#undef weak_horizontal_luma 
#undef strong_horizontal_luma 
#undef weak_vertical_chroma 
#undef strong_vertical_chroma 
#undef weak_horizontal_chroma 
#undef strong_horizontal_chroma 

#define weak_vertical_luma weak_vertical_luma_TI
#define strong_vertical_luma strong_vertical_luma_TI

#define weak_horizontal_luma weak_horizontal_luma_TI
#define strong_horizontal_luma strong_horizontal_luma_TI

#define weak_vertical_chroma weak_vertical_chroma_TI
#define strong_vertical_chroma strong_vertical_chroma_TI

#define weak_horizontal_chroma weak_horizontal_chroma_TI
#define strong_horizontal_chroma strong_horizontal_chroma_TI

#endif



#ifdef MMXi_
#undef weak_horizontal_luma 
#undef weak_vertical_luma 
#undef weak_horizontal_chroma


#define weak_horizontal_chroma weak_horizontal_chroma_MMX
#define weak_vertical_luma weak_vertical_luma_MMX
#define weak_horizontal_luma weak_horizontal_luma_MMX
#endif





void filter_mb_edgev( unsigned char pix[], const int stride, char bS[4], int index_a, int index_b);
void filter_mb_edgeh( unsigned char pix[], const int stride, char bS[4], int index_a, int index_b);
void filter_mb_edgecv(unsigned char pix[], int Slice_alpha_c0_offset_div2, int Slice_beta_offset_div2, const  int stride, char bS[4]) ;
void filter_mb_edgech(unsigned char pix[], int Slice_alpha_c0_offset_div2, int Slice_beta_offset_div2, const int stride, char bS[4]) ;




/* Vertical Luma */
void weak_vertical_luma(unsigned char pix[], const int ystride, const unsigned char alpha, const unsigned char beta, const unsigned char tc0);
void strong_vertical_luma(unsigned char pix[], const unsigned int ystride, const unsigned char alpha, const unsigned char beta);

/* Horizontal Luma */
void weak_horizontal_luma(unsigned char pix[], const int xstride, const unsigned char alpha, const unsigned char beta, const unsigned char tc0);
void strong_horizontal_luma(unsigned char pix[],const  int xstride, const unsigned char alpha, const unsigned char beta);

/* Vertical Chroma */
void weak_vertical_chroma(unsigned char pix[], const int ystride, const unsigned char alpha, const unsigned char beta, const unsigned char tc0);
void strong_vertical_chroma(unsigned char pix[],const unsigned int ystride, const unsigned char alpha, const unsigned char beta);

/* Horizontal Chroma */
void weak_horizontal_chroma(unsigned char pix[], const int xstride, const unsigned char alpha	, const unsigned char beta, const unsigned char tc0);
void strong_horizontal_chroma(unsigned char pix[], const int xstride, const unsigned char alpha, const unsigned char beta);
#endif
