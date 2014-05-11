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


#ifndef RES_8x8_H
#define RES_8x8_H


#include "svc_type.h"

//Prototypes



void StoreRes8x8		( short *image, const RESIDU *CurrResidu, const short PicWidthInPix, const short W8x8_inter [6][64]);
void AddRes8x8  ( short *image, const RESIDU *CurrResidu, const short PicWidthInPix, const short W8x8_inter [6][64]);

void SCoeffIntra8x8   ( unsigned char *Y, const RESIDU *CurrResidu, const short PicWidthInPix, const short W8x8_inter [6][64]);
void SCoeff8x8AddPic ( unsigned char *Y, const RESIDU *CurrResidu, const short PicWidthInPix);
void SCoeff8x8AddRes ( short *image, const RESIDU *CurrResidu, const short PicWidthInPix);
#endif
