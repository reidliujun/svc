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
* but WITHOUT ANY WARRANTY; without even the implied warranty off
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
#include "type.h"
#include "refill.h"
#include "init_data.h"
#include "write_back.h"
#include "fill_caches.h"
#include "BitstreamSkip.h"
#include "motion_vector.h"
#include "slice_data_vlc.h"



void SkipP(SLICE *Slice, const SPS *Sps, RESIDU *CurrResidu, DATA *Block, unsigned char *SliceTable, 
		   short iMb_x, short iMb_y, int PicSizeInMbs, int Last_Qp, short mv_cache_l0[][2], 
		   short *ref_cache_l0, short *mvl0_io, short *refl0_io)
{



	const int iCurrMbAddr = iMb_x + iMb_y * Slice -> mb_stride;
	init_P_skip_mb(&CurrResidu[iCurrMbAddr], Last_Qp, Slice);
	init_skip_non_zero_count(iMb_x, iMb_y, Sps -> pic_width_in_mbs, PicSizeInMbs, Block);

	//Initialize the cache table
	init_ref_cache(ref_cache_l0);
	init_mv_cache(mv_cache_l0);

	fill_caches_motion_vector(Slice, Slice -> b_stride, Slice -> b8_stride, ref_cache_l0, mv_cache_l0, 
		SliceTable , &CurrResidu[iCurrMbAddr], mvl0_io, refl0_io, iMb_x, iMb_y);


	//Calculation and recording of the motion vector
	motion_vector_skip( mv_cache_l0, ref_cache_l0);
	write_back_P_skip_motion( Slice -> b_stride, Slice -> b8_stride, mvl0_io, mv_cache_l0[12], refl0_io, 0 );
}

