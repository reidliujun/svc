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



#ifndef C_FILTER
#define C_FILTER

#include "type.h"
#include <math.h>

int bSCheckMvShort(short mva[2], short mvb[2]);
void HorizontalEdgeFilter(int FilterIdc, int edge, short PicWidthInMbs, int qp, int qpCb, int qpCr, char *bS, int AlphaOffset, 
						  int BetaOffset, unsigned char *image, unsigned char *image_Cb, unsigned char *image_Cr);

void VerticalEdgeFilter(int FilterIdc, int edge, short PicWidthInMbs, int qp, int qpCb, int qpCr, char *bS, int AlphaOffset, 
						int BetaOffset, unsigned char *image, unsigned char *image_Cb, unsigned char *image_Cr);

void filter_mb_P( unsigned char *image, unsigned char *image_Cb, unsigned char *image_Cr, const SLICE *Slice, 
				 unsigned char *SliceTable, short mv_cache_l0[][2], short *ref_cache_l0, 
				 int PicWidthInMbs, const PPS *pps, const int x, const int y, const RESIDU *residu);

void filter_P( const SPS *aipstsps, const PPS *aipstpps, const SLICE *Slice, unsigned char *SliceTable, 
			  short *aio_tiMv_l0, short *aio_tiRef_l0, const RESIDU *residu, 
			  unsigned char *image, unsigned char *image_Cb, unsigned char *image_Cr);


#ifdef TI_OPTIM
#undef bSCheckMvShort
#define bSCheckMvShort bSCheckMvShort_TI

static int __inline bSCheckMvShort_TI(short mva[2], short mvb[2])
{

	return ((_cmpgt2(_abs2(_sub2(*(int*)mva,*(int*)mvb)),0x00030003))!=0);
}

#else
#undef bSCheckMvShort
#define bSCheckMvShort bSCheckMvShort_C


static unsigned char __inline bSCheckMvShort_C(short mva[2], short mvb[2]){

	return (abs(mva[0] - mvb[0]) >= 4) || (abs(mva[1] - mvb[1]) >= 4);
}
#endif
#endif
