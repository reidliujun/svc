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


#ifndef BL_H
#define BL_H

void DecodeIPart(const NAL *Nal, const PPS *Pps, const SPS *Sps, REFPART *RefPartition, 
				 RESIDU *BaseResidu, const short PicWidthInPix, short mbBaseAddr, 
				 short x, short y, short x_base, short y_base, short x_mbPartIdx, short y_mbPartIdx, 
				 unsigned char BaseY[ ], unsigned char BaseCb[ ], unsigned char BaseCr[ ], 
				 unsigned char Y[ ], unsigned char U[ ], unsigned char V[ ],
				 unsigned char MbPredY[ ], unsigned char MbPredU[ ], unsigned char MbPredV[ ]);


void GetNonDiadicSample(const int ResidualPrediction, short RefIdc, short cX, short cY, const short PicWidthInPix, 
						unsigned char *Pred, unsigned char *IntraPred, short *BaseResidu, short *CurrResidu);
#endif
