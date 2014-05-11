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


#ifndef MB_I_SVC_H
#define MB_I_SVC_H

//AVC Files
#include "type.h"


//SVC Files
#include "svc_type.h"

void MbISnr (const PPS *Pps, const NAL* Nal, const W_TABLES *Quantif, const STRUCT_PF *BaselineVectors, 
			 RESIDU *CurrResidu, RESIDU *BaseResidu, const short PicWidthInMbs, 
			 unsigned char *Y, unsigned char *U, unsigned char *V,
			 unsigned char *BaseY, unsigned char *BaseU, unsigned char *BaseV);


void MbISpatial (const NAL *Nal, const SPS *Sps, const PPS *Pps, const short PicWidthInPix, const W_TABLES *quantif, 
				 short x, short y, DATA *CurrBlock, RESIDU *CurrResidu, RESIDU *BaseResidu, 
				 unsigned char *Y, unsigned char *U, unsigned char *V, 
				 unsigned char *BaseY, unsigned char *BaseU, unsigned char *BaseV);

void MbISVCDecoding(const SPS *Sps, const PPS *Pps, const NAL* Nal, const W_TABLES *Quantif, const STRUCT_PF *BaselineVectors, 
			 RESIDU *CurrResidu, RESIDU *BaseResidu, DATA *Block,
			 const short PicWidthInPix, short iMb_x, short iMb_y,
			 unsigned char *Y, unsigned char *U, unsigned char *V, 
			 unsigned char *BaseY, unsigned char *BaseU, unsigned char *BaseV);
#endif
