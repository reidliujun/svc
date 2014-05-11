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
*  Optimized version for Texas Instrument DSP 
*			   GDEM(Grupo de Diseño Electrónico y Microelectrónico)
*			   Universidad Politecnica de Madrid. 
*		       http://www.sec.upm.es/gdem
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


#ifndef INTERPOL_MAIN_8x8_H
#define INTERPOL_MAIN_8x8_H

#include "type.h"
#include "data.h"



#if defined (CHIP_DM642) || defined (CHIP_DM6437) || defined (CHIP_DM648) || defined (CHIP_OMAP3530)
#ifdef TI_OPTIM
	#define set_image_8x8 set_image_8x8_GDEM      //ESB
#else
	#define set_image_8x8 set_image_8x8_C 
#endif
#elif  MMXi
	#define set_image_8x8 set_image_8x8_MMX
#else
	#define set_image_8x8 set_image_8x8_C 
#endif


void set_image_8x8(unsigned char *aio_tImage, unsigned char aio_tImage_Cb[], unsigned char aio_tImage_Cr[], 
				   unsigned char  *Luma_l0, unsigned char  *Luma_l1, unsigned char  *Chroma_Cb_l0, 
				   unsigned char *Chroma_Cb_l1, unsigned char  *Chroma_Cr_l0, unsigned char *Chroma_Cr_l1, 
				   short *ref_cache_l0, short *ref_cache_l1, short  PicWidthInPix);

void interpolation_main_8x8(unsigned char *DpbLuma, unsigned char *DpbCb,unsigned char *DpbCr, 
							unsigned char *image , unsigned char *image_Cb, unsigned char *image_Cr, 
							short mvL0[2], short PicWidthInPix, const interpol_8x8 * interpol);

void chroma_SampleInterpolationMain8x8(unsigned char *image_Cb, unsigned char *image_Cr, unsigned char *refPicLXCb, 
									   unsigned char *refPicLXCr, short xFracl, short yFracl, short PicWidthSamples);


void SampleInterpolationMain8x8(unsigned char *DpbLuma, unsigned char *DpbCb,unsigned char *DpbCr, 
								unsigned char *Luma_l0 , unsigned char *Chroma_Cb_l0, unsigned char *Chroma_Cr_l0, 
								short mvL0[][2], short *ref_cache_l0, int x, int y, short PicWidthInPix, 
								short PicHeightInPix , const LIST_MMO *   RefPicListL0, const interpol_8x8 *interpol);

#endif
