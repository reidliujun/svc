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

#ifndef INTER_LAYER_PRED
#define INTER_LAYER_PRED


void GetBaseSample(unsigned char *Y, unsigned char *U, unsigned char *V, unsigned char *DpbLuma, 
				   unsigned char *DpbCb, unsigned char *DpbCr, short PicWidthInPix);


void GetBaseFilter(const NAL *Nal, const SLICE *Slice, const PPS *Pps, const SPS *Sps,  RESIDU *BaseResidu, 
				   unsigned char *BaseY, unsigned char *BaseU, unsigned char *BaseV);


void GetBasePrediction(const NAL *Nal, const SLICE *Slice, const PPS *Pps, const SPS *Sps, const SPS *Sps_id, 
					   RESIDU *CurrResidu, RESIDU *BaseResidu, const int size_Mb, short iCurrMbAddr, 
					   const unsigned char *ai_tiMbToSliceGroupMap, short *x16, short *y16, short *xk16, 
					   short *xp16, short *yk16, short *yp16, short *Upsampling_tmp, 
					   unsigned char *BaseY, unsigned char *BaseU, unsigned char *BaseV, 
					   short *Luma_Residu, short *Cb_Residu, short *Cr_Residu);

#endif
