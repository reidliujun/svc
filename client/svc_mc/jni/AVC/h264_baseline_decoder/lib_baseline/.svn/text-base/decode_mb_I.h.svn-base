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


#ifndef MB_I_H
#define MB_I_H 


#include "symbol.h"
#include "data.h"







void decode_I_Intra4x4_lum    ( unsigned char *image, const short PicWidthInPix, const RESIDU *residu, const short W4x4_intra [6][16], const mb4x4_mode_pf *mb4x4_mode);
void decode_I_Intra8x8_lum	  ( unsigned char *image, const short PicWidthInPix, const RESIDU *residu, const short W8x8_intra [6][64] );
void decode_I_Intra16x16_lum  ( unsigned char *image, const short PicWidthInPix, const RESIDU *Curr_Residu, const short W4x4_intra [6][16] , const mb16x16_mode_pf * mb16x16_mode) ;

void decode_I_chroma ( unsigned char *image_Cb, unsigned char *image_Cr, short PicWidth, const PPS *pps, const RESIDU *residu, const W_TABLES *quantif, const mbChroma_mode_pf * mbChroma_mode_pf) ;




void decode_MB_I ( unsigned char * image, unsigned char * image_Cb, unsigned char * image_Cr, const PPS *pps, 
				  const RESIDU *residu, const short PicWidthInPix, const W_TABLES *quantif, const STRUCT_PF * Allpf_struct);

#endif
