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

#ifndef RES_4x4_H
#define RES_4x4_H

#include "svc_type.h"

//Prototypes




void StoreRes4x4		( short *image, const RESIDU *CurrResidu, const short PicWidthInPix, const short W4x4_inter [6][16]);
void AddRes4x4  ( short *image, const RESIDU *CurrResidu, const short PicWidthInPix, const short W4x4_inter [6][16]);
void SCoeff4x4AddRes	( short *image, RESIDU *CurrResidu, const short PicWidthInPix);
void SCoeff4x4AddPic	( unsigned char *image, RESIDU *CurrResidu, const short PicWidthInPix);
#endif
