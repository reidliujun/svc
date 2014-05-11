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


//AVC files
#include "type.h"
#include "init_data.h"
#include "Loop_Filter_B.h"


//SVC files
#include "svc_type.h"
#include "svc_data.h"
#include "blk4x4.h"
#include "WriteBackSVC.h"
#include "Loop_filter_ext.h"
#include "macroblock_filter.h"






/**
This funciton applies the deblocking filter on an AVC layer.
*/
void Loop_filter_avc(const int size_mb, int layer_id, SPS *Sps, PPS *pps, SLICE *slice, LIST_MMO *Current_pic, 
					 unsigned char *SliceTable, DATA *Tab_Block, short *aio_tiMv_l0, short *aio_tiMv_l1, short *aio_tiRef_l0, 
					 short *aio_tiRef_l1, RESIDU *all_residu, unsigned char Y[], unsigned char U[], unsigned char V[])
{



	__declspec(align(64)) short    mv_cache_l0[48][2];
	__declspec(align(64)) short    mv_cache_l1[48][2];
	__declspec(align(64)) short    ref_cache_l0[48];
	__declspec(align(64)) short    ref_cache_l1[48];
	


	if(slice -> disable_deblocking_filter_idc != 1){
		const short PicSizeInMbs = (short) Sps -> PicSizeInMbs ;
		const short PicWiInMbs = (short) Sps -> pic_width_in_mbs;
		const short PicWidthInMbs = (short) Sps -> pic_width_in_mbs +2;
 		const int offset_L = (PicWidthInMbs << 8) + 16;
		const int offset_C = (PicWidthInMbs << 6) + 8;  
		RESIDU *residu = &all_residu[layer_id * size_mb];

		short iMb_x = 0;
		short iMb_y = 0;
		short    iCurrMbAddr = 0;//(short) slice -> first_mb_in_slice ;


		Y += offset_L;
		U += offset_C;
		V += offset_C;

		//Loop filter process applied on the whole image
		for (iCurrMbAddr = 0 ; iCurrMbAddr < PicSizeInMbs ;iCurrMbAddr++) {

			write_back_motion_cache_B_full_ref(slice -> b_stride, slice -> b8_stride, iCurrMbAddr, 
				&aio_tiMv_l0[iMb_x * 8 + iMb_y * (slice -> b_stride << 2)], &aio_tiMv_l1[iMb_x * 8 + iMb_y * (slice -> b_stride << 2)], 
				mv_cache_l0, mv_cache_l1, &aio_tiRef_l0[iMb_x * 4 + iMb_y * (slice -> b_stride << 1)], 
				&aio_tiRef_l1[iMb_x * 4 + iMb_y * (slice -> b_stride << 1)]	, ref_cache_l0, ref_cache_l1, slice -> mb_stride);


			filter_mb_B(&Y [((iMb_x + (PicWidthInMbs << 4) * iMb_y) << 4)], &U [((iMb_x + (PicWidthInMbs << 3) * iMb_y) <<3)], 
				&V [((iMb_x + (PicWidthInMbs << 3) * iMb_y) <<3)], slice, pps, SliceTable, mv_cache_l0, mv_cache_l1, 
				ref_cache_l0, ref_cache_l1, iCurrMbAddr, PicWidthInMbs, Current_pic -> ref_poc_l0, 
				Current_pic -> ref_poc_l1, Tab_Block[iCurrMbAddr] . Transform8x8, iMb_x, iMb_y, residu);

			iMb_x++;
			if( iMb_x == PicWiInMbs){
				iMb_x = 0;
				iMb_y++;
			}
		}
	}
}

 






/**
This funciton applies the deblocking filter on an SVC layer.
*/
void Loop_filter_svc(const int size_mb, const NAL *Nal, SPS *Sps, PPS *pps, SLICE *Slice, LIST_MMO *Current_pic, 
					 unsigned char *SliceTable, short *aio_tiMv_l0, short *aio_tiMv_l1, short *aio_tiRef_l0, 
					 short *aio_tiRef_l1, RESIDU *all_residu, unsigned char Y[], unsigned char U[], unsigned char V[])
{


	__declspec(align(64)) short    mv_cache_l0[48][2];
	__declspec(align(64)) short    mv_cache_l1[48][2];
	__declspec(align(64)) short    ref_cache_l0[48];
	__declspec(align(64)) short    ref_cache_l1[48];
	const short PicSizeInMbs = Sps -> PicSizeInMbs ;
    const short PicWiInMbs = Sps -> pic_width_in_mbs;
	const short PicWidthInMbs = Sps -> pic_width_in_mbs + 2;
	const short offset_L = (PicWidthInMbs << 8) + 16;
	const short offset_C = (PicWidthInMbs << 6) + 8;  
	RESIDU *CurrResidu = &all_residu[Nal -> LayerId * size_mb];

	if(Slice -> disable_deblocking_filter_idc != 1){
		short Pass;
		short NbPass = (short ) ((Slice -> disable_deblocking_filter_idc == 3 || Slice -> disable_deblocking_filter_idc == 6) + 1);

		Y += offset_L;
		U += offset_C;
		V += offset_C; 

		for (Pass = 0; Pass < NbPass; Pass ++){
			short iMb_x = 0;
			short iMb_y = 0;
			short iCurrMbAddr = 0;
			//Mode == 0 => Only one pass
			//Mode == 1 => First pass by two pass to do
			//Mode == 3 => Second and last pass
			short Mode = NbPass == 2 ? Pass + 1: Pass;

			//Loop filter process applied on the whole image
			for ( iCurrMbAddr = 0; iCurrMbAddr < PicSizeInMbs ; iCurrMbAddr++) {
	
				//We use a different QP for loopfiler
				SetLoopFilterQp(&CurrResidu [iCurrMbAddr], Nal, iCurrMbAddr); 

				write_back_motion_cache_B_full_ref(Nal -> b_stride, Nal -> b8_stride, iCurrMbAddr, 
					&aio_tiMv_l0[iMb_x * 8 + iMb_y * (Nal -> b_stride << 2)], 
					&aio_tiMv_l1[iMb_x * 8 + iMb_y * (Nal -> b_stride << 2)], 
					mv_cache_l0, mv_cache_l1,
					&aio_tiRef_l0[iMb_x * 4 + iMb_y * (Nal -> b_stride << 1)], 
					&aio_tiRef_l1[iMb_x * 4 + iMb_y * (Nal -> b_stride << 1)], 
					ref_cache_l0, ref_cache_l1, Slice -> mb_stride);

				filter_mb_svc(&Y [((iMb_x + (PicWidthInMbs << 4) * iMb_y) << 4)], 
					&U [((iMb_x + (PicWidthInMbs << 3) * iMb_y) << 3)], 
					&V [((iMb_x + (PicWidthInMbs << 3) * iMb_y) << 3)], Slice, pps, SliceTable, 
					mv_cache_l0, mv_cache_l1, ref_cache_l0, ref_cache_l1, iCurrMbAddr,
					PicWidthInMbs, Current_pic -> ref_poc_l0, Current_pic -> ref_poc_l1, iMb_x, iMb_y, 
					CurrResidu, Nal, Mode, Sps -> profile_idc);

				iMb_x++;
				if( iMb_x == PicWiInMbs){
					iMb_x = 0;
					iMb_y++;
				}
			}
		}
	}
}






/**
This funciton applies the interlayer deblocking filter.
*/
void Deblocking_base_layer(const SPS *Sps, const PPS *pps, const RESIDU *residu, const NAL *Nal, int DeblockingFilterIdc, 
						   int AlphaOffset, int BetaOffset, unsigned char Y[], unsigned char U[], unsigned char V[])
{




	const short PicSizeInMbs = Sps -> PicSizeInMbs ;
    const short PicWiInMbs = Sps -> pic_width_in_mbs;
	const short PicWidthInMbs = Sps -> pic_width_in_mbs + 2;

	short  iMb_x = 0;
	short  iMb_y = 0;
	short  iCurrMbAddr = 0;
	
	for ( ; iCurrMbAddr < PicSizeInMbs ;iCurrMbAddr++) {

		if (IS_I_SVC(residu[iCurrMbAddr] . MbType)){
			UpsamplinfFilterMb(&Y [((iMb_x + (PicWidthInMbs << 4) * iMb_y) << 4)], &U [((iMb_x + (PicWidthInMbs << 3) * iMb_y) << 3)], 
				&V [((iMb_x + (PicWidthInMbs << 3) * iMb_y) << 3)], pps, iCurrMbAddr, PicWidthInMbs, 
				residu[iCurrMbAddr] . Transform8x8, Nal -> LoopFilterSpatialScalability, DeblockingFilterIdc, 
				AlphaOffset, BetaOffset, iMb_x, iMb_y, residu);
		}
		iMb_x++;
		if( iMb_x == PicWiInMbs){
			iMb_x = 0;
			iMb_y++;
		}
	}
}
