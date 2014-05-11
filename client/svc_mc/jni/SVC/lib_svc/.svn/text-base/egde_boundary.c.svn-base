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

#include "math.h"


//AVC
#include "symbol.h"
#include "type.h"
#include "Loop_Filter_B.h"





//SVC 
#include "svc_data.h"
#include "edge_boundary.h"


/**
Determine boudary strengh by detecting if the 4x4 block or a 4x4 block neighbour are intra coded, or if the residual is different from zero.
The way to determine it depends of block1 and block2.
*/
static __inline void CheckNeighbourBlock(int dir, int edge, unsigned char add, int block1, int block2, char bS[4]){

	int i;
	for( i = 0; i < 4; i++ ) {
		int x_pos = dir == 0 ? edge : i;
		int y_pos = dir == 0 ? i : edge;
		int xn_pos = dir == 0 ? (x_pos + 4 - 1) % 4 : x_pos;
		int yn_pos = dir == 0 ? y_pos: (y_pos + 4 - 1) % 4;

		bS[i] = add;
		if(((block1 & (1 << (x_pos + 4 * y_pos))) != 0) || ((block2 & (1 << (xn_pos + 4 * yn_pos))) != 0)) {
			bS[i] ++;
		}
	}
}


/**
Determine the boudary strength of a boudary edge.
*/
void external_edge(const int mb_type, const int mbn_type, const int mb_xy, const int mbn_xy, char bS[4], 
				   unsigned char ProfileIdc, int dir, int edge,int slice_type, int spatial_scalability, 
				   RESIDU *residu, short mv_cache_l0[][2], short *ref_cache_l0, short poc_l0[], 
				   short mv_cache_l1[][2], short *ref_cache_l1, short poc_l1[])
{


	int i;

	if ( spatial_scalability && ( IS_I_SVC(mb_type) || IS_I_SVC(mbn_type))){
		// if it's an intra macroblock
		if( IS_I(mb_type) || IS_I(mbn_type)){
			bS[0] = bS[1] = bS[2] = bS[3] = 4;
		}else if (IS_I_SVC(mb_type) && IS_I_SVC(mbn_type)){
			CheckNeighbourBlock(dir, edge, 0, residu[mb_xy] . blk4x4, residu[mbn_xy] . blk4x4, bS);
		}else {
			CheckNeighbourBlock(dir, edge, 1, residu[mb_xy] . blk4x4_Res, residu[mbn_xy] . blk4x4_Res, bS);
		}
	}else{
		if( IS_I_SVC(mb_type) || IS_I_SVC(mbn_type)){
				bS[0] = bS[1] = bS[2] = bS[3] = 4;
		}else{
			for( i = 0; i < 4; i++ ) {
				int x_pos = dir == 0 ? edge : i;
				int y_pos = dir == 0 ? i : edge;
				int xn_pos = dir == 0 ? (x_pos + 4 - 1) % 4 : x_pos;
				int yn_pos = dir == 0 ? y_pos: (y_pos + 4 - 1) % 4;
				int BMask = (1 << (x_pos + 4 * y_pos));
				int BnMask = (1 << (xn_pos + 4 * yn_pos));


				if((ProfileIdc == 83 && ((residu[mb_xy] . blk4x4 & BMask) || (residu[mbn_xy] . blk4x4 & BnMask))) || residu[mb_xy] . blk4x4_Res & BMask || residu[mbn_xy] . blk4x4_Res & BnMask) {
					bS[i] = 2;
				}else {
					int b_idx = 8 + 4 + x_pos + (y_pos << 3);
					int bn_idx = b_idx - (dir ? 8: 1);
					bS[i] = GetBoudaryStrenght(mv_cache_l0, mv_cache_l1, ref_cache_l0, ref_cache_l1, poc_l0, poc_l1, b_idx, bn_idx, slice_type, mb_type, mbn_type);
				}
			}
		}
	}
}






/**
Determine the boudary strength of a internal edge.
*/
void internal_edge(const int mb_type, char bS[4], int dir, int edge, int slice_type, int spatial_scalability,
				   unsigned char ProfileIdc, RESIDU *residu, short mv_cache_l0[][2], short *ref_cache_l0, 
				   short poc_l0[], short mv_cache_l1[][2], short *ref_cache_l1, short poc_l1[])
{		
				

	int i;

	// if it's an intra macroblock
	if( IS_I(mb_type)){
		bS[0] = bS[1] = bS[2] = bS[3] = 3;
	}else if (IS_I_SVC(mb_type) ){
		//ok for internal
		if(spatial_scalability  ){
			CheckNeighbourBlock(dir, edge, 0, residu -> blk4x4, residu -> blk4x4, bS);
		}else{
			bS[0] = bS[1] = bS[2] = bS[3] = 3;
		}
	
	}else{
		for( i = 0; i < 4; i++ ) {
			int x_pos = dir == 0 ? edge : i;
			int y_pos = dir == 0 ? i : edge;
			int xn_pos = dir == 0 ? (x_pos + 4 - 1) % 4 : x_pos;
			int yn_pos = dir == 0 ? y_pos: (y_pos + 4 - 1) % 4;
			int BMask = (1 << (x_pos + 4 * y_pos));
			int BnMask = (1 << (xn_pos + 4 * yn_pos));

			if((ProfileIdc == 83 && ((residu -> blk4x4 & BMask) || (residu -> blk4x4 & BnMask))) || (residu -> blk4x4_Res & BMask) || (residu -> blk4x4_Res & BnMask)) {
				bS[i] = 2;
			}else {
				int b_idx = 8 + 4 + x_pos + (y_pos << 3);
				int bn_idx = b_idx - (dir ? 8:1);
				bS[i] = GetBoudaryStrenght(mv_cache_l0, mv_cache_l1, ref_cache_l0, ref_cache_l1, poc_l0, poc_l1, b_idx, bn_idx, slice_type, mb_type, mb_type);
			}
		}
	}
}



/**
Determine the boudary strength of a boudary edge.
*/
void InterExternalEdge(const int mb_type, const int mbn_type, const int mb_xy, const int mbn_xy, 
					   char bS[4], int dir, int edge, int spatial_scalability, 
					   int DeblockingFilterIdc, const RESIDU *residu ){


	int i;

	if (!IS_I_SVC(mbn_type)){
		bS[0] = bS[1] = bS[2] = bS[3] = 0;
	}else {
		if ( DeblockingFilterIdc == 1 && (!IS_I_SVC(mb_type) || !IS_I_SVC(mbn_type))){
			bS[0] = bS[1] = bS[2] = bS[3] = 0;
		}else if ( spatial_scalability && ( IS_I_SVC(mb_type) || IS_I_SVC(mbn_type))){
			// if it's an intra macroblock
			if( IS_I(mb_type) || IS_I(mbn_type)){
				bS[0] = bS[1] = bS[2] = bS[3] = 4;
			}else if (IS_I_SVC(mb_type) && IS_I_SVC(mbn_type)){
				CheckNeighbourBlock(dir, edge, 0, residu[mb_xy] . blk4x4, residu[mbn_xy] . blk4x4, bS);
			}else {
				CheckNeighbourBlock(dir, edge, 1, residu[mb_xy] . blk4x4_Res, residu[mbn_xy] . blk4x4_Res, bS);
			}
		}
		else{
			if( IS_I_SVC(mb_type) || IS_I_SVC(mbn_type)){
					bS[0] = bS[1] = bS[2] = bS[3] = 4;
			}else{
				for( i = 0; i < 4; i++ ) {
					int x_pos = dir == 0 ? edge : i;
					int y_pos = dir == 0 ? i : edge;
					int xn_pos = dir == 0 ? (x_pos + 4 - 1) % 4 : x_pos;
					int yn_pos = dir == 0 ? y_pos: (y_pos + 4 - 1) % 4;
					int BMask = (1 << (x_pos + 4 * y_pos));
					int BnMask = (1 << (xn_pos + 4 * yn_pos));

					if((residu[mb_xy] . blk4x4 & BMask) || (residu[mbn_xy] . blk4x4 & BnMask) || (residu[mb_xy] . blk4x4_Res & BMask) || (residu[mbn_xy] . blk4x4_Res & BnMask)) {
						bS[i] = 2;
					}else {
						bS[i] = 0;
					}
				}
			}
		}
	}
}





/**
Determine the boudary strength of a internal edge.
*/
void InterInternalEdge(const int mb_type, char bS[4], int dir, int edge, int spatial_scalability, const RESIDU *residu){		
				
	// if it's an intra macroblock
	if( IS_I(mb_type)){
		bS[0] = bS[1] = bS[2] = bS[3] = 3;
	}else if (IS_I_SVC(mb_type) ){
		//ok for internal
		if(spatial_scalability  ){
			CheckNeighbourBlock(dir, edge, 0, residu -> blk4x4, residu -> blk4x4, bS);
		}else {//check this else{
			bS[0] = bS[1] = bS[2] = bS[3] = 3;
		}
	}else{
		int i;
		for( i = 0; i < 4; i++ ) {
			int x_pos = dir == 0 ? edge : i;
			int y_pos = dir == 0 ? i : edge;
			int xn_pos = dir == 0 ? (x_pos + 4 - 1) % 4 : x_pos;
			int yn_pos = dir == 0 ? y_pos: (y_pos + 4 - 1) % 4;
			int BMask = (1 << (x_pos + 4 * y_pos));
			int BnMask = (1 << (xn_pos + 4 * yn_pos));

			if((residu -> blk4x4 & BMask)|| (residu -> blk4x4 & BnMask) || (residu -> blk4x4_Res & BMask) || (residu -> blk4x4_Res & BnMask)) {
					bS[i] = 2;
			}else {
				bS[i] = 0;
			}
		}
	}
}
