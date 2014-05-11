/*****************************************************************************
   *
   *  Open SVC Decoder developped in IETR image lab
   *
   *
   *
   *              Médéric BLESTEL <mblestel@insa-rennes.Fr>
   *              Mickael RAULET <mraulet@insa-rennes.Fr>
   *			  Maxime  PELCAT <mpelcat@insa-rennes.Fr>
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

#ifndef INTERPOL_MMX_H
#define INTERPOL_MMX_H


#include "type.h"

//n_0
unsigned int interpolhline_mmx_1(unsigned char* image);
unsigned int interpolhline_mmx_2(unsigned char* image);
unsigned int interpolhline_mmx_3(unsigned char* image);

//0_n
unsigned int interpolvline_mmx_1(unsigned char* image, unsigned char * ref,	int PicWidthInPix);
unsigned int interpolvline_mmx_2(unsigned char* image,	int PicWidthInPix);
unsigned int interpolvline_mmx_3(unsigned char* image,	int PicWidthInPix);

//2_n
__m64 interpolhline64_mmx(unsigned char* image);
unsigned int interpolvline64_1_mmx(	__m64* temp);
unsigned int interpolvline64_2_mmx(	__m64* temp);
unsigned int interpolvline64_3_mmx(	__m64* temp);

//n_2
__m64 interpolvline64_mmx(unsigned char* image, const unsigned short PicWidthInPix);
__m64 interpolhline64_1_mmx(__m64* temp);
__m64 interpolhline64_3_mmx(__m64* temp);



#endif
