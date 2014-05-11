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

#ifndef I_BL_H
#define I_BL_H

#include "svc_type.h"



void DecodeITCoeff(RESIDU *CurrResidu, const RESIDU *BaseResidu, const PPS *pps, const short PicWidthInPix, const W_TABLES *quantif, const STRUCT_PF * Allpf_struct
			, unsigned char *Y, unsigned char *U, unsigned char *V);

void MbIPad(const NAL *Nal, const PPS *Pps, const SPS *Sps, short BaseMbAddr
			, short x, short y, short x_base, short y_base, short x_mbPartIdx, short y_mbPartIdx
			, const RESIDU *BaseResidu, unsigned char *BaseY, unsigned char *BaseU, unsigned char *BaseV
			, unsigned char *Y, unsigned char *U, unsigned char *V);
#endif
