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



#ifndef C_INTERPOLATION
#define C_INTERPOLATION

#include "type.h"
#include "data.h"



#define chroma_sample_interpolation chroma_sample_interpolation_C

#ifdef TI_OPTIM
#undef chroma_sample_interpolation
#define chroma_sample_interpolation chroma_sample_interpolation_TI
#endif

#ifdef MMX____
#undef chroma_sample_interpolation
#define chroma_sample_interpolation chroma_sample_interpolation_MMX
#endif

void chroma_sample_interpolation(unsigned char *image_Cb,	unsigned char *image_Cr, unsigned char *refPicLXCb, 
								 unsigned char *refPicLXCr, const short xFracl, const short yFracl, 
								 const short PicWidthSamples, const short stride);

void sample_interpolation(unsigned char *DpbLuma, unsigned char *DpbCb, unsigned char *DpbCr, 
						  unsigned char *image, unsigned char *image_Cb, unsigned char *image_Cr, 
						  short mvLX[][2], short *ref_cache, const short x, const short y, const short PicWidthInPix, 
						  const short PicHeightInPix, const LIST_MMO *RefPicListL0, const interpol_4x4 *interpol);

void Interpolate(int Ref, short Currentx, short Currenty, short *mvLX, const short OutStride, const short PicWidthInPix, 
				 const short PicHeightInPix, const LIST_MMO *RefPicListL0, unsigned char *Y, unsigned char*U, unsigned char *V, 
				 unsigned char *DpbLuma, unsigned char*DpbCb, unsigned char *DpbCr, const interpol_4x4 *interpol);
#endif


