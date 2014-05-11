/* FFmpeg project
 * libavcodec (h264.c)
 * H.26L/H.264/AVC/JVT/14496-10/... encoder/decoder
 * Copyright (c) 2003 Michael Niedermayer <michaelni@gmx.at>
 *
 * This file has been modified for opensvcdecoder.
 
*****************************************************************************
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

#include "symbol.h"
#include "init_data.h"
#include "edge_filter.h"
#include "Loop_Filter_B.h"


#ifdef TI_OPTIM
char GetBoudaryStrenght_TI( short mv_cache_l0[][2], short mv_cache_l1[][2], short *ref_cache_l0, short *ref_cache_l1, 
					   short poc_l0[], short poc_l1[], int b_idx, int bn_idx, int slice_type, int mb_type, int mbm_type)
{

	short	ref_p1;
	short	ref_q1;
	short ref_p0 = ref_cache_l0[b_idx]  < 0 ? -1 : poc_l0[ref_cache_l0[b_idx]];
	short ref_q0 = ref_cache_l0[bn_idx] < 0 ? -1 : poc_l0[ref_cache_l0[bn_idx]];
	unsigned int p0q0,p1q1;

	char v = 1;
	char x,y;



	if(slice_type == SLICE_TYPE_B) {
		ref_p1 = ref_cache_l1[b_idx] < 0 ? -1 : poc_l1[ref_cache_l1[b_idx]];
		ref_q1 = ref_cache_l1[bn_idx] < 0 ? -1 : poc_l1[ref_cache_l1[bn_idx]];
	}else{
		ref_p1 = -1;
		ref_q1 = -1;
	}

//	ref_p1 = ((ref_cache_l1[b_idx] < 0)&&(slice_type != SLICE_TYPE_B)) ? -1 : poc_l1[ref_cache_l1[b_idx]];
//	ref_q1 = ((ref_cache_l1[bn_idx] < 0)&&(slice_type != SLICE_TYPE_B)) ? -1 : poc_l1[ref_cache_l1[bn_idx]];

	p0q0=_pack2(ref_q1,ref_q0);
	p1q1=_pack2(ref_p1,ref_p0);
	x =_cmpeq2(p0q0,p1q1);
	y = x | _cmpeq2(p0q0,_rotl(p1q1,16));


	//if ( ((ref_p0 == ref_q0) && (ref_p1 == ref_q1)) || ((ref_p0 == ref_q1) && (ref_p1 == ref_q0))) {
	if ( y==3) {
		char l0b_l0bn=bSCheckMvShort (mv_cache_l0[b_idx], mv_cache_l0[bn_idx]);
		char l1b_l1bn=bSCheckMvShort (mv_cache_l1[b_idx], mv_cache_l1[bn_idx]);
		char l0b_l1bn=bSCheckMvShort (mv_cache_l0[b_idx], mv_cache_l1[bn_idx]);
		char l1b_l0bn=bSCheckMvShort (mv_cache_l1[b_idx], mv_cache_l0[bn_idx]);
		if (ref_p0 != ref_p1) 	{ 
			// compare MV for the same reference picture
			if (ref_p0 == ref_q0)  {
				v =  l0b_l0bn || (l1b_l1bn && (!(IS_P(mb_type) || IS_P(mbm_type))));
			} else   {
				v =   (l0b_l1bn) ||  (l1b_l0bn);	
			}
		} else { // L0 and L1 reference pictures of p0 are the same; q0 as well
			v =  ( (l0b_l0bn) ||  (l1b_l1bn)) && ( (l0b_l1bn) ||  (l1b_l0bn));
		}
	}

	return v;

}

#endif


char GetBoudaryStrenght_C( short mv_cache_l0[][2], short mv_cache_l1[][2], short *ref_cache_l0, short *ref_cache_l1, 
					   short poc_l0[], short poc_l1[], int b_idx, int bn_idx, int slice_type, int mb_type, int mbm_type)
{

	int	ref_p1;
	int	ref_q1;
	int ref_p0 = ref_cache_l0[b_idx]  < 0 ? -1 : poc_l0[ref_cache_l0[b_idx]];
	int ref_q0 = ref_cache_l0[bn_idx] < 0 ? -1 : poc_l0[ref_cache_l0[bn_idx]];


	char v = 1;

	if(slice_type == SLICE_TYPE_B) {
		ref_p1 = ref_cache_l1[b_idx] < 0 ? -1 : poc_l1[ref_cache_l1[b_idx]];
		ref_q1 = ref_cache_l1[bn_idx] < 0 ? -1 : poc_l1[ref_cache_l1[bn_idx]];
	}else{
		ref_p1 = -1;
		ref_q1 = -1;
	}

	if ( ((ref_p0 == ref_q0) && (ref_p1 == ref_q1)) || ((ref_p0 == ref_q1) && (ref_p1 == ref_q0))) {
		if (ref_p0 != ref_p1) 	{ 
			// compare MV for the same reference picture
			if (ref_p0 == ref_q0)  {
				if (IS_P(mb_type) || IS_P(mbm_type)){
					v =  bSCheckMvShort (mv_cache_l0[b_idx], mv_cache_l0[bn_idx]);
				}else{
					v =  bSCheckMvShort (mv_cache_l0[b_idx], mv_cache_l0[bn_idx]) || bSCheckMvShort (mv_cache_l1[b_idx], mv_cache_l1[bn_idx]);
				}
			} else   {
				v =  bSCheckMvShort (mv_cache_l0[b_idx], mv_cache_l1[bn_idx]) || bSCheckMvShort (mv_cache_l1[b_idx], mv_cache_l0[bn_idx]);	
			}
		} else { // L0 and L1 reference pictures of p0 are the same; q0 as well
			v =  (bSCheckMvShort (mv_cache_l0[b_idx], mv_cache_l0[bn_idx]) || bSCheckMvShort (mv_cache_l1[b_idx], mv_cache_l1[bn_idx])) && 
				(bSCheckMvShort (mv_cache_l0[b_idx], mv_cache_l1[bn_idx]) || bSCheckMvShort (mv_cache_l1[b_idx], mv_cache_l0[bn_idx]));
		}
	}

	return v;
}


/**
This function allows to filter a macroblock in a picture. 


@param image Table of current frame.
@param image_Cb Table of current frame.
@param image_Cr Table of current frame.
@param slice The Slice structure contains all information used to decode the next Slice.
@param SliceTable Specifies in which Slice belongs each macroblock of the picture.
@param non_zero_count_cache Specifies the coeff_token of each blocks 4x4 of a macroblock.
@param mv_cache_l0 A cache table where the motion vector are stocked for each 4x4 block of the current macroblock.
@param mv_cache_l1 A cache table where the motion vector are stocked for each 4x4 block of the current macroblock.
@param ref_cache_l0 A cache table where the reference is stocked for each 4x4 block of the current macroblock.
@param ref_cache_l1 A cache table where the reference is stocked for each 4x4 block of the current macroblock.
@param PicWidthInMbs Width in macroblocks of the current frame.
@param Tab_Block Contains all parameters of each macroblock of the current picture.
@param chroma_qp_index_offset Qp offset for the chrominance.
@param second_chroma_qp_index_offset Qp offset for the chrominance for Cr.
@param poc_l0 Table which contains the poc of the reference list l0. 
@param poc_l1 Table which contains the poc of the reference list l1.
@param transform_8x8_size To know if the macroblock is in 8x8 mode.
@param x The x position of the macroblock in the picture.
@param y The y position of the macroblock in the picture.
@param residu Structure residu of the current macroblock.
*/

void filter_mb_B( unsigned char *image, unsigned char *image_Cb, unsigned char *image_Cr, SLICE *slice, PPS *pps, 
				 unsigned char *SliceTable,  short mv_cache_l0[][2], short mv_cache_l1[][2], 
				 short *ref_cache_l0, short *ref_cache_l1, int mb_xy, short PicWidthInMbs, 
				 short poc_l0[], short poc_l1[], const int transform_8x8_size, 
				 const int x, const int y, RESIDU *residu) 
{


    int dir;
	int edge;
	int i;
    int mv_done;
	int qp;
	int mbm_type;
	int mode_mbm;
	int mbn_type;
	int qp_mbn_xy;
	int mask_par1;

	const int mode = residu [mb_xy] . Mode;
	const int mb_type = residu [mb_xy] . MbType;
	const int top_mb_xy = mb_xy - slice -> mb_stride ;
	const int qp_mb_xy = residu [mb_xy] . Qp;
	int mbm_xy =  mb_xy -1;

	char bS[4];

    /* dir : 0 -> vertical edge, 1 -> horizontal edge */
    for( dir = 0; dir < 2; dir++ )
    {
        int start = 0;

		// how often to recheck mv-based bS when iterating between edges
		const int cond = (( mode == 1 && !dir) || (mode == 2 && dir));
		const int mask_edge = (mode == 3 || (mode == 2 && !dir) || (mode == 1 && dir)) ? 3 :cond ? 1 : 0;

		// how often to recheck mv-based bS when iterating along each edge
	    const int  mask_par0 = (mode == 3) * (1 << 3);

	   	 if (mbm_xy >= 0){
	   	 	mbm_type = residu [mbm_xy] . MbType;
			mode_mbm = residu[mbm_xy] . Mode;
		}else{
			mbm_type = 0;
			mode_mbm = 0;
		}


		if ((!dir && (x == 0)) || (dir && y == 0)){
			start = 1;
		}

		if (slice -> disable_deblocking_filter_idc  == 2 && SliceTable[mbm_xy] != SliceTable[mb_xy]){
			start = 1;
		}

        /* Calculate bS */
        for( edge = start; edge < 4; edge++ ) {

			//In case of  8x8 macroblock, only edge 0 and 2 to be done
			if ( transform_8x8_size && ( edge&1))
				continue;

            /* mbn_xy: neighbor_ macroblock */
			if (edge > 0){
				mbn_type = mb_type;
				mbm_xy = mb_xy;
				qp = qp_mbn_xy = qp_mb_xy;
			}else{
				mbn_type = mbm_type;
				qp_mbn_xy = residu[mbm_xy].Qp;
				qp = (qp_mb_xy + qp_mbn_xy + 1) >> 1;
			}

			// if it's an intra macroblock
			if( IS_I(mb_type) ||  IS_I(mbn_type)){
				if ((edge == 0)) {
					bS[0] = bS[1] = bS[2] = bS[3] = 4;
				} else {
					bS[0] = bS[1] = bS[2] = bS[3] = 3;
				}
			}else{
				
				mask_par1 = (IS_B(mbn_type) && (mode_mbm == 3)) * (1 << 3);
				if( edge & mask_edge ) {
					bS[0] = bS[1] = bS[2] = bS[3] = 0;
					mv_done = 1;

				} else if( mask_par0 && (edge || mask_par1)){
					int b_idx = 8 + 4 + edge * (dir ? 8:1);
					int bn_idx = b_idx - (dir ? 8:1);

					bS[0] = bS[1] = bS[2] = bS[3] = GetBoudaryStrenght(mv_cache_l0, mv_cache_l1, ref_cache_l0, ref_cache_l1, 
						poc_l0, poc_l1, b_idx, bn_idx, slice -> slice_type, mb_type, mbm_type);
					mv_done = 1;

				}else{
					mv_done = 0;
				}

				for( i = 0; i < 4; i++ ) {
					int x_pos = dir == 0 ? edge : i;
					int y_pos = dir == 0 ? i    : edge;
					int b_idx = 8 + 4 + x_pos + (y_pos << 3);
					int bn_idx = b_idx - (dir ? 8:1);
					int xn_pos = dir == 0 ? (x_pos + 4 - 1) % 4 : x_pos;
					int yn_pos = dir == 0 ? y_pos: (y_pos + 4 - 1) % 4;
					int BMask = (1 << (x_pos + 4 * y_pos));
					int BnMask = (1 << (xn_pos + 4 * yn_pos));

					if( (residu[mb_xy] . blk4x4 & BMask) || (residu[mbm_xy] . blk4x4 & BnMask)) {
						bS[i] = 2;
					}else if(!mv_done) {
						bS[i] = GetBoudaryStrenght(mv_cache_l0, mv_cache_l1, ref_cache_l0, ref_cache_l1, 
							poc_l0, poc_l1, b_idx, bn_idx, slice -> slice_type, mb_type, mbm_type);
					}
				}
				if(bS[0] + bS[1] + bS[2] + bS[3] == 0)
					continue;
			}

           
			//Application of the loop filter on the horizontal direction 
			if(!dir){
				int qPCb = (qP_tab[qp_mb_xy + pps -> chroma_qp_index_offset] + qP_tab[qp_mbn_xy + pps -> chroma_qp_index_offset] + 1) >> 1;
				int qPCr = (qP_tab[qp_mb_xy + pps -> second_chroma_qp_index_offset] + qP_tab[qp_mbn_xy + pps -> second_chroma_qp_index_offset] + 1) >> 1;	
				HorizontalEdgeFilter(slice -> disable_deblocking_filter_idc, edge, PicWidthInMbs, qp, qPCb, qPCr, bS, pps -> AlphaOffset, pps -> BetaOffset, image, image_Cb, image_Cr);
			}else {
				int qPCb = (qP_tab[qp_mb_xy + pps -> chroma_qp_index_offset] + qP_tab[qp_mbn_xy + pps -> chroma_qp_index_offset] + 1) >> 1;
				int qPCr = (qP_tab[qp_mb_xy + pps -> second_chroma_qp_index_offset] + qP_tab[qp_mbn_xy + pps -> second_chroma_qp_index_offset] + 1) >> 1;
				VerticalEdgeFilter(slice -> disable_deblocking_filter_idc, edge, PicWidthInMbs, qp, qPCb, qPCr, bS, pps -> AlphaOffset, pps -> BetaOffset, image, image_Cb, image_Cr);
			}
		}
		mbm_xy = top_mb_xy;
    }
}








/**
This function allows to filter a picture. 


@param sps The sps structure which contains all parameters decoded in this NAL.
@param pps The pps structure which contains all parameters decoded in this NAL.
@param slice The Slice structure contains all information used to decode the next Slice.
@param Current_pic Data which concerns the current frame.
@param SliceTable Specifies in which Slice belongs each macroblock of the picture.
@param non_zero_count Specifies the coeff_token of each block of the picture.
@param Tab_Block Contains all parameters of each macroblock of the current picture.
@param aio_tiMv_l0 A table where the motion vector are stocked for each 4x4 block of each macroblock.
@param aio_tiMv_l1 A table where the motion vector are stocked for each 4x4 block of each macroblock.
@param aio_tiRef_l0 A table where the reference is stocked for each 4x4 block of each macroblock.
@param aio_tiRef_l1 A table where the reference is stocked for each 4x4 block of each macroblock.
@param residu Structure residu of the current macroblock.
@param image Table of current frame.
@param image_Cb Table of current frame.
@param image_Cr Table of current frame.
*/
void filter_B( SPS *Sps, PPS *pps,SLICE *slice, LIST_MMO *Current_pic, unsigned char *SliceTable, DATA *Tab_Block, 
			  short *aio_tiMv_l0, short *aio_tiMv_l1, short *aio_tiRef_l0, short *aio_tiRef_l1, RESIDU *residu, 
			  unsigned char *aio_tImage, unsigned char *image_Cb, unsigned char *image_Cr)
{




	__declspec(align(64)) short    mv_cache_l0[48][2];
	__declspec(align(64)) short    mv_cache_l1[48][2];
	__declspec(align(64)) short    ref_cache_l0[48];
	__declspec(align(64)) short    ref_cache_l1[48];

	const short PicSizeInMbs = Sps -> PicSizeInMbs ;
    const short PicWiInMbs = Sps -> pic_width_in_mbs;
	const short PicWidthInMbs = Sps -> pic_width_in_mbs + 2;
	short iCurrMbAddr;
	const int offset_L = (PicWidthInMbs << 8) + 16;
	const int offset_C = (PicWidthInMbs << 6) + 8;  
	short x = 0;
	short y = 0;

   //Loop filter process applied on the whole image
	for ( iCurrMbAddr = 0; iCurrMbAddr < PicSizeInMbs ;iCurrMbAddr++) {


		write_back_motion_cache_B( slice -> b_stride, slice -> b8_stride, iCurrMbAddr, 
			&aio_tiMv_l0[x * 8 + y * (slice -> b_stride << 2)], &aio_tiMv_l1[x * 8 + y * (slice -> b_stride << 2)], 
			mv_cache_l0, mv_cache_l1, &aio_tiRef_l0[x * 2 + y * ( slice -> b8_stride << 1)], 
			&aio_tiRef_l1[x * 2 + y * ( slice -> b8_stride << 1)], ref_cache_l0,ref_cache_l1, slice -> mb_stride);


		filter_mb_B(&aio_tImage [offset_L +  ((x + (PicWidthInMbs << 4) * y) << 4)], 
			&image_Cb [offset_C + ((x + (PicWidthInMbs << 3) * y) <<3)], 
			&image_Cr [offset_C + ((x + (PicWidthInMbs << 3) * y) <<3)], slice, pps, SliceTable, 
			mv_cache_l0, mv_cache_l1, ref_cache_l0, ref_cache_l1, iCurrMbAddr, PicWidthInMbs, Current_pic -> ref_poc_l0,
			Current_pic -> ref_poc_l1, Tab_Block[iCurrMbAddr] . Transform8x8, x, y, residu);

		x++;
		if ( x == PicWiInMbs){
			x = 0;
			y++;
		}
	}
}

