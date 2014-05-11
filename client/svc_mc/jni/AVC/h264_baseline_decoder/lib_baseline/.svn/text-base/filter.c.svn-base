/* FFmpeg project
* libavcodec (h264.c)
* H.26L/H.264/AVC/JVT/14496-10/... encoder/decoder
* Copyright (c) 2003 Michael Niedermayer <michaelni@gmx.at>
*
* This file has been modified for opensvcdecoder.
*/

/*****************************************************************************
*
*  Open SVC Decoder developped in IETR image lab
*
*
*
*              Médéric BLESTEL <mblestel@insa-rennes.Fr>
*              Mickael RAULET <mraulet@insa-rennes.Fr>
*			  Alain  MACCARI <alain.maccari@ens.insa-rennes.fr>
*              http://www.ietr.org/
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
* License along with this not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
*
*
* $Id$
*
**************************************************************************/
#include <math.h>

//AVC Files
#include "type.h"
#include "data.h"
#include "filter.h"
#include "edge_filter.h"
#include "init_data.h"
#include "fill_caches.h"
#include "write_back.h"


static const char ref2frm[18] = {-1,-1,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};



void HorizontalEdgeFilter(int FilterIdc, int edge, short PicWidthInMbs, int qp, int qPCb, int qPCr,
						  char *bS, int AlphaOffset, int BetaOffset, 
						  unsigned char *image, unsigned char *image_Cb, unsigned char *image_Cr)
{

	filter_mb_edgev( &image[edge << 2], PicWidthInMbs << 4, bS, qp + AlphaOffset, qp + BetaOffset);
	if(FilterIdc < 4 &&  ((edge&1) == 0 )) {
		//Application of the loop filter on the chrominance
		filter_mb_edgecv( &image_Cb[edge << 1], qPCb + AlphaOffset, qPCb + BetaOffset, PicWidthInMbs << 3, bS);
		filter_mb_edgecv( &image_Cr[edge << 1], qPCr + AlphaOffset, qPCr + BetaOffset, PicWidthInMbs << 3, bS);
	}
}




void VerticalEdgeFilter(int FilterIdc, int edge, short PicWidthInMbs, int qp, int qpCb, int qpCr,
						char *bS, int AlphaOffset, int BetaOffset, 
						unsigned char *image, unsigned char *image_Cb, unsigned char *image_Cr)
{
	//Application of the loop filter on the vertical direction 
	filter_mb_edgeh( &image[edge * PicWidthInMbs << 6], PicWidthInMbs << 4, bS,  qp + AlphaOffset, qp + BetaOffset);
	if(FilterIdc < 4 &&  ((edge&1) == 0 )) {
		//Application of the loop filter on the chrominance
		filter_mb_edgech( &image_Cb[edge * PicWidthInMbs << 4], qpCb + AlphaOffset, qpCb + BetaOffset, PicWidthInMbs << 3, bS);
		filter_mb_edgech( &image_Cr[edge * PicWidthInMbs << 4], qpCr + AlphaOffset, qpCr + BetaOffset, PicWidthInMbs << 3, bS);
	}
}





/**
This function allows to filter a macroblock in a picture. 


@param image Table of current frame.
@param image_Cb Table of current frame.
@param image_Cr Table of current frame.
@param Slice The Slice structure contains all information used to decode the next Slice.
@param SliceTable Specifies in which Slice belongs each macroblock of the picture.
@param non_zero_count_cache Specifies the coeff_token of each blocks 4x4 of a macroblock.
@param mv_cache_l0 A cache table where the motion vector are stocked for each 4x4 block of the current macroblock.
@param ref_cache_l0 A cache table where the reference is stocked for each 4x4 block of the current macroblock.
@param PicWidthInMbs Width in macroblocks of the current frame.
@param aio_tstBlock Contains all parameters of each macroblock of the current picture.
@param chroma_qp_index_offset Qp offset for the chrominance.
@param second_chroma_qp_index_offset Qp offset for the chrominance for Cr.
@param x The x position of the macroblock in the picture.
@param y The y position of the macroblock in the picture.
@param residu Structure residu of the current macroblock.
*/


void filter_mb_P( unsigned char *image, unsigned char *image_Cb, unsigned char *image_Cr, const  SLICE *Slice, 
				 unsigned char *SliceTable, short mv_cache_l0[][2], short *ref_cache_l0, int PicWidthInMbs, 
				 const PPS *pps, const int x, const int y, const RESIDU *residu)
{
	int dir, edge, i, mv_done, qp, mbm_type;
	int mbn_type, qp_mbn_xy, edges;
	const int mb_xy = x + y * Slice -> mb_stride;
	const int mb_type = residu[mb_xy] . MbType;
	const int mode = residu[mb_xy] . Mode;
	const int qp_mb_xy = residu[mb_xy] . Qp;
	int mbm_xy = mb_xy - 1; 
	char bS[4];

	/* dir : 0 -> vertical edge, 1 -> horizontal edge */
	for( dir = 0; dir < 2; dir++ ){
		int start = 0;

		// how often to recheck mv-based bS when iterating between edges
		// 8x16 and vertical filter or 16x8 and horizontal filter 
		const int cond = (( mode == 1 && !dir) || (mode == 2 && dir));
		const int mask_edge = ( mode == 3 || (mode == 2 && !dir) || (mode == 1 && dir)) ? 3 :	cond ? 1 : 0;

		// how often to recheck mv-based bS when iterating along each edge
		const int mask_par0 = (mode == 3 || cond) * (1 << (6 - mode));

		if (mbm_xy >= 0){
			mbm_type = residu [mbm_xy] . MbType;
		}
		else{
			mbm_type = 0; 
		}

		edges = ((mb_type == P_Skip) && (mbm_type == P_Skip))? 1: 4;

		if ((!dir && (x == 0)) || (dir && y == 0)){
			start = 1;	
		}

		if (Slice -> disable_deblocking_filter_idc  == 2 && SliceTable[mbm_xy] != SliceTable[mb_xy]){
			start = 1;	
		}

		/* Calculate bS */
		for( edge = start; edge < edges; edge++ ) {
			/* mbn_xy: neighbour_ macroblock */
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
				} 
				else {
					bS[0] = bS[1] = bS[2] = bS[3] = 3;
				}

			}else{
				if( edge & mask_edge ) {
					bS[0] = bS[1] = bS[2] = bS[3] = 0;
					mv_done = 1;
				} else if( mask_par0 && (edge || (mbn_type == P_Skip))){
					int b_idx= 8 + 4 + edge * (dir ? 8:1);
					int bn_idx= b_idx - (dir ? 8:1);
					int v = ref2frm[ref_cache_l0[b_idx] + 2] != ref2frm[ref_cache_l0[bn_idx] + 2] || bSCheckMvShort( mv_cache_l0[b_idx],  mv_cache_l0[bn_idx]);

					bS[0] = bS[1] = bS[2] = bS[3] = v;
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
						bS[i] = 0;
						if ( ref2frm[ref_cache_l0[b_idx] + 2] != ref2frm[ref_cache_l0[bn_idx] + 2] || bSCheckMvShort( mv_cache_l0[b_idx],  mv_cache_l0[bn_idx])){
							bS[i] = 1;
						}
					}
				}

				if(bS[0] + bS[1] + bS[2] + bS[3] == 0)
					continue;
			}

			//Application of the loop filter on the horizontal direction 
			if(!dir){
				int qPCb = (qP_tab[qp_mb_xy + pps -> chroma_qp_index_offset] + qP_tab[qp_mbn_xy + pps -> chroma_qp_index_offset] + 1) >> 1;
				int qPCr = (qP_tab[qp_mb_xy + pps -> second_chroma_qp_index_offset] + qP_tab[qp_mbn_xy + pps -> second_chroma_qp_index_offset] + 1) >> 1;	
				HorizontalEdgeFilter( Slice -> disable_deblocking_filter_idc, edge, PicWidthInMbs, qp, qPCb, qPCr, bS, pps -> AlphaOffset, pps -> BetaOffset, image, image_Cb, image_Cr);
			}else {
				int qPCb = (qP_tab[qp_mb_xy + pps -> chroma_qp_index_offset] + qP_tab[qp_mbn_xy + pps -> chroma_qp_index_offset] + 1) >> 1;
				int qPCr = (qP_tab[qp_mb_xy + pps -> second_chroma_qp_index_offset] + qP_tab[qp_mbn_xy + pps -> second_chroma_qp_index_offset] + 1) >> 1;
				VerticalEdgeFilter(Slice -> disable_deblocking_filter_idc, edge, PicWidthInMbs, qp, qPCb, qPCr, bS, pps -> AlphaOffset, pps -> BetaOffset, image, image_Cb, image_Cr);
			}
		}
		mbm_xy = mb_xy - Slice -> mb_stride;
	}
}



/**
This function allows to filter a picture. 


@param sps The sps structure which contains all parameters decoded in this NAL.
@param pps The pps structure which contains all parameters decoded in this NAL.
@param Slice The Slice structure contains all information used to decode the next Slice.
@param SliceTable Specifies in which Slice belongs each macroblock of the picture.
@param aio_tstNon_zero_count Specifies the coeff_token of each block of the picture.
@param aio_tstBlock Contains all parameters of each macroblock of the current picture.
@param aio_tiMv_l0 A table where the motion vector are stocked for each 4x4 block of each macroblock.
@param aio_tiRef_l0 A table where the reference is stocked for each 4x4 block of each macroblock.
@param residu Structure residu of the current macroblock.
@param image Table of current frame.
@param image_Cb Table of current frame.
@param image_Cr Table of current frame.
*/
void filter_P(const  SPS *aipstsps, const PPS *aipstpps, const SLICE *Slice,  unsigned char *SliceTable,
			  short *aio_tiMv_l0, short *aio_tiRef_l0,  const RESIDU *residu,
			  unsigned char *image, unsigned char *image_Cb, unsigned char *image_Cr)
{
	__declspec(align(64)) short mv_cache_l0[48][2];
	__declspec(align(64)) short ref_cache_l0[48];
	const int PicSizeInMbs = aipstsps -> PicSizeInMbs ;
	const int PicWiInMbs = aipstsps -> pic_width_in_mbs;
	const int PicWidthInMbs = aipstsps -> pic_width_in_mbs + 2;
	const short PicWidthInPix = (short) PicWidthInMbs << 4 ;
	const int offset_L = (PicWidthInMbs << 8) + 16;
	const int offset_C = (PicWidthInMbs << 6) + 8; 

	int iCurrMbAddr = 0 ;
	short x = 0;
	short y = 0;


	image += offset_L;
	image_Cb += offset_C;
	image_Cr += offset_C;

	//Loop filter process applied on the whole image
	for ( ; iCurrMbAddr < PicSizeInMbs ;iCurrMbAddr++) 	{

		unsigned char *Y = &image[((x + ( y * PicWidthInPix)) << 4)];
		unsigned char *U = &image_Cb[((x + ( y * PicWidthInPix >> 1)) << 3)];
		unsigned char *V = &image_Cr[((x + ( y * PicWidthInPix >> 1)) << 3)];


		write_back_motion_cache(Slice -> b_stride, Slice -> b8_stride, iCurrMbAddr, 
			&aio_tiMv_l0[x * 8 + y * (Slice -> b_stride << 2)],  mv_cache_l0, 
			&aio_tiRef_l0[x * 2 + y * ( Slice -> b8_stride << 1)], ref_cache_l0, Slice -> mb_stride);

		filter_mb_P(Y, U, V, Slice, SliceTable, mv_cache_l0, ref_cache_l0, PicWidthInMbs, aipstpps, x, y, residu);

		x++;
		if( x == PicWiInMbs){
			x = 0;
			y++;
		}
	}
}
