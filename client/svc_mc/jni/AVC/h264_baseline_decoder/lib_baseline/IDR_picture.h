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


#ifndef IDR_PICTURE_H
#define IDR_PICTURE_H
#include "type.h"


void IDR_picture( const SPS * Sps, const PPS * Pps, const SLICE *Slice, const unsigned char *SliceTable, 
				 RESIDU *picture_residu, W_TABLES *quantif, const STRUCT_PF * ai_STRUCT_PF, 
				 unsigned char *aio_tucImage, unsigned char *aio_tucImage_Cb, unsigned char *aio_tucImage_Cr);


#endif
