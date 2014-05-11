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
#include "init_data.h"
#include "write_back.h"
#include "mb_pred_main.h"
#include "BitstreamSkipB.h"
#include "fill_caches_cabac.h"




void SkipB(SLICE *Slice, const SPS *Sps, RESIDU *CurrResidu, DATA *Block, unsigned char *SliceTable, short iMb_x, short iMb_y, 
		   unsigned char Last_Qp, short mv_cache_l0[][2], short *ref_cache_l0, short *mvl0_io, short *refl0_io, 
		   short mv_cache_l1[][2], short *ref_cache_l1, short *mvl1_io, short *refl1_io, 
		   short *mvl1_l0, short *mvl1_l1, short *refl1_l0, short *refl1_l1, LIST_MMO *RefListl1)
{


	const int iCurrMbAddr = iMb_x + iMb_y * Slice -> mb_stride;
	
	
	init_B_skip_mb(&CurrResidu[iCurrMbAddr], Last_Qp, Slice);
	init_skip_non_zero_count(iMb_x, iMb_y, Slice -> mb_stride, Sps -> PicSizeInMbs, Block);

	//to check if necessary
	init_ref_cache(ref_cache_l0);
	init_ref_cache(ref_cache_l1);
	init_mv_cache(mv_cache_l0);
	init_mv_cache(mv_cache_l1);

	fill_caches_motion_vector_B(Slice, Slice -> b_stride, Slice -> b8_stride, ref_cache_l0, ref_cache_l1, 
		mv_cache_l0, mv_cache_l1, SliceTable, &CurrResidu [iCurrMbAddr], mvl0_io, mvl1_io, 
		refl0_io, refl1_io, iCurrMbAddr, iMb_x);

	motion_vector_B_skip(mv_cache_l0, mv_cache_l1, ref_cache_l0, ref_cache_l1, mvl1_l0, mvl1_l1, refl1_l0, refl1_l1, 
		Slice, Sps -> direct_8x8_inference_flag, RefListl1 -> long_term, RefListl1 -> slice_type,0);



	//Recording of the motion vector
	write_back_motion( Slice -> b_stride, Slice -> b8_stride, mvl0_io, mv_cache_l0, refl0_io, ref_cache_l0 );
	write_back_motion( Slice -> b_stride, Slice -> b8_stride, mvl1_io, mv_cache_l1, refl1_io, ref_cache_l1 );
}
