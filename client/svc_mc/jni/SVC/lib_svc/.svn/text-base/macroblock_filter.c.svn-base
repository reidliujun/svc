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




#include <math.h>

//AVC Files
#include "type.h"
#include "data.h"
#include "symbol.h"
#include "edge_filter.h"
#include "Loop_Filter_B.h"

//SVC Files
#include "svc_data.h" 
#include "edge_boundary.h"
#include "macroblock_filter.h"



/**
For the upsampling, inter layer deblocking filter.
*/
void UpsamplinfFilterMb( unsigned char *image, unsigned char *image_Cb , unsigned char *image_Cr, const PPS *pps, 
			   short mb_xy, short PicWidthInMbs, const int transform_8x8_size, int spatial_scalable_flg, 
			   int DeblockingFilterIdc, int AlphaOffset, int BetaOffset, const short x, const short y, const RESIDU *residu) 
{


	int dir;
	int edge;

	int qp, qp_mbn_xy;
	int mbm_type;

	const int top_mb_xy = mb_xy - PicWidthInMbs + 2;
	int mb_type = residu[mb_xy] . MbType;
	int qp_mb_xy = residu[mb_xy] . Qp;
	int mbm_xy =  mb_xy - 1;

	char bS[4];

	// dir : 0 -> vertical edge, 1 -> horizontal edge 
	for( dir = 0; dir < 2; dir++ )
	{
		int start = 0;

		if (mbm_xy >= 0){		 
			mbm_type = residu[mbm_xy] . MbType;
		}else{
			mbm_type = 0;
		}

		if ((!dir && (x == 0)) || (dir && y == 0)){
			start = 1;
		}

		if (DeblockingFilterIdc == 2 && residu[mbm_xy] . SliceNum != residu[mb_xy] . SliceNum){
			start = 1;
		}

		// Calculate bS 
		for( edge = start; edge < 4; edge++ ) {

			//In case of  8x8 macroblock, only edge 0 and 2 to be done
			if ( transform_8x8_size && (edge & 1))
				continue;

			/* mbn_xy: neighbor_ macroblock */
			if (edge > 0){
				//In case of a second pass, 
				//we have to filter only boundarie edge and not internal edge
				mbm_xy = mb_xy;
				qp = qp_mbn_xy = qp_mb_xy;
				InterInternalEdge(mb_type, bS, dir, edge, spatial_scalable_flg, &residu[mb_xy]);
			}else{
				qp_mbn_xy = residu[mbm_xy] . Qp;
				qp = (qp_mb_xy + qp_mbn_xy + 1) >> 1;
				InterExternalEdge(mb_type, mbm_type, mb_xy, mbm_xy, bS, dir, edge, spatial_scalable_flg, DeblockingFilterIdc, residu);
			}
			
			if(bS[0] + bS[1] + bS[2] + bS[3] == 0)
				continue;


			//Application of the loop filter on the horizontal direction 
			if(!dir){
				int qPCb = (qP_tab[qp_mb_xy + pps -> chroma_qp_index_offset] + qP_tab[qp_mbn_xy + pps -> chroma_qp_index_offset] + 1) >> 1;
				int qPCr = (qP_tab[qp_mb_xy + pps -> second_chroma_qp_index_offset] + qP_tab[qp_mbn_xy + pps -> second_chroma_qp_index_offset] + 1) >> 1;	
				HorizontalEdgeFilter(DeblockingFilterIdc, edge, PicWidthInMbs, qp, qPCb, qPCr, bS, AlphaOffset, BetaOffset, image, image_Cb, image_Cr);
			}else {
				int qPCb = (qP_tab[qp_mb_xy + pps -> chroma_qp_index_offset] + qP_tab[qp_mbn_xy + pps -> chroma_qp_index_offset] + 1) >> 1;
				int qPCr = (qP_tab[qp_mb_xy + pps -> second_chroma_qp_index_offset] + qP_tab[qp_mbn_xy + pps -> second_chroma_qp_index_offset] + 1) >> 1;
				VerticalEdgeFilter(DeblockingFilterIdc, edge, PicWidthInMbs, qp, qPCb, qPCr, bS, AlphaOffset, BetaOffset, image, image_Cb, image_Cr);
			}
		}
		mbm_xy = top_mb_xy;
	}
}





/**
SVC macroblock deblocking filter.
*/
void filter_mb_svc( unsigned char *image, unsigned char *image_Cb, unsigned char *image_Cr, SLICE *slice, PPS *pps, 
				   unsigned char *SliceTable, short mv_cache_l0[][2], short mv_cache_l1[][2], short *ref_cache_l0, 
				   short *ref_cache_l1, short mb_xy, short PicWidthInMbs, short *poc_l0, short *poc_l1, 
				   const short x, const short y, RESIDU *residu, const NAL *Nal, short Pass, unsigned char ProfileIdc)
{


	int dir;

	int qp;
	int mbm_type;
	int qp_mbn_xy;

	const int top_mb_xy = mb_xy - slice -> mb_stride ;
	const int mb_type = residu [mb_xy] . MbType;
	const int qp_mb_xy = residu [mb_xy] . QpLf;
	int mbm_xy =  mb_xy - 1;

	/* dir : 0 -> vertical edge, 1 -> horizontal edge */
	for( dir = 0; dir < 2; dir++ ){
		int edge;
		int start = 0;
		char bS[4];

		if (mbm_xy >= 0){
			mbm_type = residu [mbm_xy] . MbType;
		}else{
			mbm_type = 0;
		}

		if ((!dir && (x == 0)) || (dir && y == 0)){
			start = 1;
		}

		if ((slice -> disable_deblocking_filter_idc  == 2 || slice -> disable_deblocking_filter_idc == 5 ) && SliceTable[mbm_xy] != SliceTable[mb_xy]){
			start = 1;
		}

		if ((Pass == 1) && SliceTable[mbm_xy] != SliceTable[mb_xy]){
			start = 1;
		}

		/* Calculate bS */
		for( edge = start; edge < 4; edge++ ) {

			//In case of  8x8 macroblock, only edge 0 and 2 to be done
			if ( residu [mb_xy] . Transform8x8 && (edge & 1))
				continue;

			/* mbn_xy: neighbor_ macroblock */
			if (edge > 0){
				//In case of a second pass, 
				//we have to filter only boundarie edge and not internal edge
				if( Pass == 2){
					bS[0] = bS[1] = bS[2] = bS[3] = 0;
				}else{ 
					qp = qp_mbn_xy = qp_mb_xy;
					internal_edge(mb_type, bS, dir, edge, slice -> slice_type, Nal -> SpatialScalability, ProfileIdc,
						&residu[mb_xy], mv_cache_l0, ref_cache_l0, poc_l0, mv_cache_l1, ref_cache_l1, poc_l1);
				}
			}else{
				if ((Pass == 2) && SliceTable[mbm_xy] == SliceTable[mb_xy]){
					bS[0] = bS[1] = bS[2] = bS[3] = 0;
				} else {
					qp_mbn_xy = residu[mbm_xy] . QpLf;
					qp = (qp_mb_xy + qp_mbn_xy + 1) >> 1;
					external_edge(mb_type, mbm_type, mb_xy, mbm_xy, bS, ProfileIdc, dir, edge, slice -> slice_type, Nal -> SpatialScalability, 
						residu, mv_cache_l0, ref_cache_l0, poc_l0, mv_cache_l1, ref_cache_l1, poc_l1);
				}
			}


			if(bS[0] + bS[1] + bS[2] + bS[3] == 0)
				continue;

			//Application of the loop filter on the horizontal direction 
			if(!dir){
				int qPCb = (qP_tab[qp_mb_xy + pps -> chroma_qp_index_offset] + qP_tab[qp_mbn_xy + pps -> chroma_qp_index_offset] + 1) >> 1;
				int qPCr = (qP_tab[qp_mb_xy + pps -> second_chroma_qp_index_offset] + qP_tab[qp_mbn_xy + pps -> second_chroma_qp_index_offset] + 1) >> 1;	
				HorizontalEdgeFilter(slice -> disable_deblocking_filter_idc, edge, PicWidthInMbs, qp, qPCb, qPCr, 
					bS, pps -> AlphaOffset, pps -> BetaOffset, image, image_Cb, image_Cr);
			}else {
				int qPCb = (qP_tab[qp_mb_xy + pps -> chroma_qp_index_offset] + qP_tab[qp_mbn_xy + pps -> chroma_qp_index_offset] + 1) >> 1;
				int qPCr = (qP_tab[qp_mb_xy + pps -> second_chroma_qp_index_offset] + qP_tab[qp_mbn_xy + pps -> second_chroma_qp_index_offset] + 1) >> 1;
				VerticalEdgeFilter(slice -> disable_deblocking_filter_idc, edge, PicWidthInMbs, qp, qPCb, qPCr, 
					bS, pps -> AlphaOffset, pps -> BetaOffset, image, image_Cb, image_Cr);
			}
		}
		mbm_xy = top_mb_xy;
	}
}
