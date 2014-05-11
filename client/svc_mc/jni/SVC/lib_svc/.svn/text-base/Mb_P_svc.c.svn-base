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
#include "symbol.h"
#include "data.h"
#include "clip.h"
#include "refill.h"
#include "motion_vector.h"
#include "interpolation.h"
#include "neighbourhood.h"
#include "interpolation_8x8.h"

//SVC files
#include "Mb_P_svc.h"
#include "Mb_B_svc.h"
#include "motion_vector_svc.h"
#include "slice_data_I_svc.h"
#include "fill_caches_svc.h"
#include "svc_neighbourhood.h"



void init_svc_vectors(SVC_VECTORS *svc)
{
	svc -> P_vectors [0] = P_svc_8x8 ;
	svc -> P_vectors [1] = P_svc_8x16 ;
	svc -> P_vectors [2] = P_svc_16x8 ;
	svc -> P_vectors [3] = P_svc_16x16 ;
	svc -> P_vectors [4] = P_svc_skip;
	svc -> P_sub_vectors [0] = Sub_P_svc_8x8; 
	svc -> P_sub_vectors [1] = Sub_P_svc_8x4;
	svc -> P_sub_vectors [2] = Sub_P_svc_4x8;
	svc -> P_sub_vectors [3] = Sub_P_svc_4x4;


	svc -> B_vectors [0] = B_svc_8x8 ;
	svc -> B_vectors [1] = B_svc_8x16 ;
	svc -> B_vectors [2] = B_svc_16x8 ;
	svc -> B_vectors [3] = B_svc_16x16 ;
	svc -> B_vectors [4] = B_svc_Skip ;

	svc -> B_sub_vectors [0] = Sub_B_svc_4x4; 
	svc -> B_sub_vectors  [1] = Sub_B_svc_8x8;
	svc -> B_sub_vectors  [2] = Sub_B_svc_8x8;
	svc -> B_sub_vectors  [3] = Sub_B_svc_8x8;
	svc -> B_sub_vectors  [4] = Sub_B_svc_8x4; 
	svc -> B_sub_vectors  [5] = Sub_B_svc_4x8;
	svc -> B_sub_vectors  [6] = Sub_B_svc_8x4;
	svc -> B_sub_vectors  [7] = Sub_B_svc_4x8;
	svc -> B_sub_vectors  [8] = Sub_B_svc_8x4; 
	svc -> B_sub_vectors  [9] = Sub_B_svc_4x8;
	svc -> B_sub_vectors  [10] = Sub_B_svc_4x4;
	svc -> B_sub_vectors  [11] = Sub_B_svc_4x4;
	svc -> B_sub_vectors  [12] = Sub_B_svc_4x4;
}



/**
Decode P sub 8x4 macroblock.
*/
void Sub_P_svc_8x4(short mv_cache [][2], short *ref_cache, const DATA *Block, int mbPartIdx, 
				   short *mv_l0,  short *ref_bas_l0, const NAL *Nal, REFPART RefPartition[16]){


	short mvL0 [2];
	int index = SCAN8((mbPartIdx << 2)) ;

	if ( Block -> MotionPredL0 [mbPartIdx] == 1){ 
		int ref_index_0 = RefPartition [0] . RefIndex;
		int ref_index_1 = RefPartition [4] . RefIndex;
		if (!Nal -> non_diadic){
			//First Block 8x4
			int index_mv = RefPartition [0] . MvIndex;
			SetInterParamHori(Nal, &mv_cache[index], &ref_cache[index], Block -> MvdL0[mbPartIdx << 2], &mv_l0[index_mv], &ref_bas_l0[ref_index_0]);

			//Second Block 8x4
			index_mv  = RefPartition [4] . MvIndex;
			SetInterParamHori(Nal, &mv_cache[index + 8], &ref_cache[index + 8], Block -> MvdL0[(mbPartIdx << 2) + 1], &mv_l0[index_mv], &ref_bas_l0[ref_index_1]);
		}else{
			//To check
			//First Block 8x4
			ref_cache[index] = ref_cache[index + 1] = ref_bas_l0[ref_index_0];
			mv_cache [index][0] = mv_cache [index + 1][0] = mv_cache [index][0] + Block -> MvdL0[mbPartIdx << 2][0];
			mv_cache [index][1] = mv_cache [index + 1][1] = mv_cache [index][1] + Block -> MvdL0[mbPartIdx << 2][1];

			ref_cache[index + 8] = ref_cache[index + 9] = ref_bas_l0[ref_index_1];
			mv_cache [index + 8][0] = mv_cache [index + 9][0] = mv_cache [index + 8] [0] + Block -> MvdL0[(mbPartIdx << 2) + 1][0];
			mv_cache [index + 8][1] = mv_cache [index + 9][1] = mv_cache [index + 8] [1] + Block -> MvdL0[(mbPartIdx << 2) + 1][1];
		}

	}else{
		ref_cache [index]	  = ref_cache [index + 1] = 
		ref_cache [index + 8] = ref_cache [index + 9] = Block -> RefIdxL0 [mbPartIdx];

		//First Block 8x4
		motion_vector(mv_cache, Block -> MvdL0[(mbPartIdx << 2) ], ref_cache, mvL0, Block -> RefIdxL0[mbPartIdx], index, 2);
		mv_cache [index][0] = mv_cache [index + 1][0] = mvL0 [0];
		mv_cache [index][1] = mv_cache [index + 1][1] = mvL0 [1];

		//Second Block 8x4
		motion_vector( mv_cache, Block -> MvdL0[(mbPartIdx << 2) + 1], ref_cache, mvL0, Block -> RefIdxL0[mbPartIdx], index+8, 2);
		mv_cache [index + 8][0] = mv_cache [index + 9][0] = mvL0 [0];
		mv_cache [index + 8][1] = mv_cache [index + 9][1] = mvL0 [1];

	}
}



/**
Decode P sub 4x8 macroblock.
*/
void Sub_P_svc_4x8(short mv_cache [][2], short *ref_cache, const DATA * Block, int mbPartIdx, 
				   short *mv_l0, short *ref_bas_l0,const NAL *Nal, REFPART RefPartition[16]){


	short mvL0 [2];
	int  index = SCAN8((mbPartIdx << 2)) ;

	if ( Block -> MotionPredL0 [mbPartIdx] == 1){ 
		int ref_index_0 = RefPartition [0] . RefIndex;
		int ref_index_1 = RefPartition [1] . RefIndex;
		if ( !Nal -> non_diadic){
			//First Block 4x8
			int index_mv = RefPartition [0] . MvIndex;
			SetInterParamVert(Nal, &mv_cache[index], &ref_cache[index], Block -> MvdL0[mbPartIdx << 2], &mv_l0[index_mv], &ref_bas_l0[ref_index_0]);

			//Second Block 8x4
			index_mv  = RefPartition [1] . MvIndex;
			SetInterParamVert(Nal, &mv_cache[index + 1], &ref_cache[index + 1], Block -> MvdL0[(mbPartIdx << 2) + 1], &mv_l0[index_mv], &ref_bas_l0[ref_index_1]);
		}else{
			//First Block 8x4
			ref_cache[index] = ref_cache[index + 8] = ref_bas_l0[ref_index_0];
			mv_cache [index][0] = mv_cache [index + 8][0] = mv_cache [index][0] + Block -> MvdL0[(mbPartIdx << 2)][0];
			mv_cache [index][1] = mv_cache [index + 8][1] = mv_cache [index][1] + Block -> MvdL0[(mbPartIdx << 2)][1];

			//Second Block 8x4
			ref_cache[index + 1] = ref_cache[index + 9] = ref_bas_l0[ref_index_1];
			mv_cache [index + 1][0] = mv_cache [index + 9][0] = mv_cache [index + 1][0] + Block -> MvdL0[(mbPartIdx << 2) + 1][0];
			mv_cache [index + 1][1] = mv_cache [index + 9][1] = mv_cache [index + 1][1] + Block -> MvdL0[(mbPartIdx << 2) + 1][1];
		}

	}else{
		ref_cache [index] = ref_cache [index + 1] = 
		ref_cache [index + 8] = ref_cache [index + 9] = Block -> RefIdxL0 [mbPartIdx];

		//First Block 8x4
		motion_vector(mv_cache,  Block -> MvdL0[(mbPartIdx << 2)], ref_cache, mvL0, Block -> RefIdxL0[mbPartIdx], index, 1);

		mv_cache [index][0] = mv_cache [index + 8][0] = mvL0 [0];
		mv_cache [index][1] = mv_cache [index + 8][1] = mvL0 [1];


		//Second Block 8x4
		motion_vector( mv_cache,  Block -> MvdL0[(mbPartIdx << 2) + 1], ref_cache, mvL0, Block -> RefIdxL0[mbPartIdx], index + 1, 1);

		mv_cache [index + 1][0] = mv_cache [index + 9][0] = mvL0 [0];
		mv_cache [index + 1][1] = mv_cache [index + 9][1] = mvL0 [1];
	}
}



/**
Decode P sub 4x4 macroblock.
*/
void Sub_P_svc_4x4(short mv_cache [][2], short *ref_cache, const DATA * Block, int mbPartIdx, 
				   short *mv_l0, short *ref_bas_l0,const NAL *Nal, REFPART RefPartition[16]){

	int index = SCAN8((mbPartIdx << 2));

	if ( Block -> MotionPredL0 [mbPartIdx] == 1){
		int ref_index_0 = RefPartition [0] . RefIndex;
		int ref_index_1 = RefPartition [1] . RefIndex;
		int ref_index_2 = RefPartition [4] . RefIndex;
		int ref_index_3 = RefPartition [5] . RefIndex;
		if ( !Nal -> non_diadic){
			//First Block 4x4
			int index_mv  = RefPartition [0] . MvIndex;
			SetInterParam(Nal, mv_cache[index], &ref_cache[index], Block -> MvdL0[mbPartIdx << 2], &mv_l0[index_mv], &ref_bas_l0[ref_index_0]);

			//Second Block 4x4
			index_mv  = RefPartition [1] . MvIndex;
			SetInterParam(Nal, mv_cache[index + 1], &ref_cache[index + 1], Block -> MvdL0[(mbPartIdx << 2) + 1], &mv_l0[index_mv], &ref_bas_l0[ref_index_1]);

			//Third Block 4x4
			index_mv  = RefPartition [4] . MvIndex;
			SetInterParam(Nal, mv_cache[index + 8], &ref_cache[index + 8], Block -> MvdL0[(mbPartIdx << 2) + 2], &mv_l0[index_mv], &ref_bas_l0[ref_index_2]);

			//Fourth Block 4x4
			index_mv  = RefPartition [5] . MvIndex;
			SetInterParam(Nal, mv_cache[index + 9], &ref_cache[index + 9], Block -> MvdL0[(mbPartIdx << 2) + 3], &mv_l0[index_mv], &ref_bas_l0[ref_index_3]);
		}else{
			//All Blocks 4x4
			ref_cache[index] = ref_bas_l0[ref_index_0];
			mv_cache[index][0] += Block -> MvdL0[mbPartIdx << 2][0];
			mv_cache[index][1] += Block -> MvdL0[mbPartIdx << 2][1];

			ref_cache[index + 1] = ref_bas_l0[ref_index_1];
			mv_cache[index + 1][0] += Block -> MvdL0[(mbPartIdx << 2) + 1][0];
			mv_cache[index + 1][1] += Block -> MvdL0[(mbPartIdx << 2) + 1][1];

			ref_cache[index + 8] = ref_bas_l0[ref_index_2];
			mv_cache[index + 8][0] += Block -> MvdL0[(mbPartIdx << 2) + 2][0];
			mv_cache[index + 8][1] += Block -> MvdL0[(mbPartIdx << 2) + 2][1];

			ref_cache[index + 9] = ref_bas_l0[ref_index_3];
			mv_cache[index + 9][0] += Block -> MvdL0[(mbPartIdx << 2) + 3][0];
			mv_cache[index + 9][1] += Block -> MvdL0[(mbPartIdx << 2) + 3][1];
		}
	}else{
		short mvL0[2];
		ref_cache [index]	  = ref_cache [index + 1] = 
			ref_cache [index + 8] = ref_cache [index + 9] = Block -> RefIdxL0 [mbPartIdx];

		//First Block 4x4
		motion_vector( mv_cache,Block -> MvdL0[(mbPartIdx << 2)],  ref_cache, mvL0, Block -> RefIdxL0[mbPartIdx]	, index, 1);
		mv_cache [index][0] = mvL0 [0];
		mv_cache [index][1] = mvL0 [1];

		//Second Block 4x4
		index += 1;
		motion_vector(mv_cache,Block -> MvdL0[(mbPartIdx << 2) + 1],  ref_cache, mvL0, Block -> RefIdxL0[mbPartIdx]	, index, 1);
		mv_cache [index][0] = mvL0 [0];
		mv_cache [index][1] = mvL0 [1];

		//Third Block 4x4
		index += 7;
		motion_vector( mv_cache,Block -> MvdL0[(mbPartIdx << 2) + 2],  ref_cache, mvL0, Block -> RefIdxL0[mbPartIdx], index, 1);
		mv_cache [index][0] = mvL0 [0];
		mv_cache [index][1] = mvL0 [1];

		//Fourth Block 4x4
		index += 1;
		motion_vector( mv_cache,Block -> MvdL0[(mbPartIdx << 2) + 3],  ref_cache, mvL0, Block -> RefIdxL0[mbPartIdx]	, index, 1);
		mv_cache [index][0] = mvL0 [0];
		mv_cache [index][1] = mvL0 [1];
	}
}


/**
Decode P sub 8x8 macroblock.
*/
void Sub_P_svc_8x8(short mv_cache [][2], short *ref_cache, const DATA * Block, int mbPartIdx, 
				   short *mv_l0, short *ref_bas_l0,const NAL *Nal, REFPART RefPartition[16]){

	short mvL0 [2];
	int index = SCAN8(mbPartIdx << 2);

	if ( Block -> MotionPredL0 [mbPartIdx] == 1){
		int ref_index = RefPartition [0] . RefIndex;
		if ( !Nal -> non_diadic ){
			int index_mv  = RefPartition [0] . MvIndex;
			SetInterParam(Nal, &mvL0[0], &ref_cache[index], Block -> MvdL0[mbPartIdx << 2], &mv_l0[index_mv], &ref_bas_l0[ref_index]);
		}else{
			ref_cache[index] = ref_bas_l0[ref_index];
			mvL0[0] = mv_cache [index][0] + Block -> MvdL0[mbPartIdx << 2][0];
			mvL0[1] = mv_cache [index][1] + Block -> MvdL0[mbPartIdx << 2][1];
		}
	}else{
		ref_cache [index] =  Block -> RefIdxL0 [mbPartIdx];
		motion_vector( mv_cache, Block -> MvdL0[mbPartIdx << 2], ref_cache, mvL0, Block -> RefIdxL0[mbPartIdx], index, 2);
	}
	//Set to the rest of the block 8x8
	ref_cache [index + 1] = ref_cache [index + 8] = ref_cache [index + 9] = ref_cache [index];
	mv_cache [index][0]	= mv_cache [index + 1][0] = mv_cache [index + 8][0]	= mv_cache [index + 9][0] = mvL0 [0];
	mv_cache [index][1]	= mv_cache [index + 1][1] = mv_cache [index + 8][1]	= mv_cache [index + 9][1] = mvL0 [1];
}




/**
Decode P 8x8 macroblock.
*/
void P_svc_8x8( short mv_cache [][2],	short *ref_cache, const RESIDU *CurrResidu, const DATA *Block, const Sub_P_svc decode_sub[], 
			   unsigned char *DpbLuma, unsigned char *DpbCb, unsigned char *DpbCr, 
			   unsigned char *image, unsigned char *image_Cb, unsigned char *image_Cr, 
			   const short x, const short y, const short PicWidthInPix, const short PicHeightInPix, 
			   const NAL *Nal, LIST_MMO *RefPicListL0, short *mv_l0, short *ref_bas_l0, 
			   const STRUCT_PF *vectors, REFPART RefPartition[16])
{

	int mbPartIdx;

	for ( mbPartIdx = 0 ; mbPartIdx < 4 ; mbPartIdx++ ) {
		const int mode = CurrResidu -> SubMbType [mbPartIdx];
		decode_sub [mode](mv_cache, ref_cache, Block, mbPartIdx, mv_l0, ref_bas_l0, Nal, &RefPartition[((mbPartIdx & 2) << 2) + ((mbPartIdx & 1) << 1)]);
	}

	if ( Nal -> PicToDisplay ){
		//Recovery of  the prediction
		sample_interpolation(DpbLuma, DpbCb,DpbCr, image, image_Cb, image_Cr, mv_cache, ref_cache, x , y, PicWidthInPix, PicHeightInPix, RefPicListL0, vectors -> Ptr_interpol_4x4);			
	}		
}




/**
Decode P 8x16 macroblock.
*/
void P_svc_8x16 ( short mv_cache [][2], short *ref_cache, const RESIDU *CurrResidu, const DATA * Block, const Sub_P_svc decode_sub[], 
				 unsigned char *DpbLuma, unsigned char *DpbCb, unsigned char *DpbCr, 
				 unsigned char *image, unsigned char *image_Cb, unsigned char *image_Cr, 
				 const short x, const short y, const short PicWidthInPix, const short PicHeightInPix, 
				 const NAL *Nal, LIST_MMO *RefPicListL0, short *mv_l0, short *ref_bas_l0, 
				 const STRUCT_PF *vectors, REFPART RefPartition[16])
{

	short mbPartIdx ;
	short mvL0 [2];
	short ref;

	for ( mbPartIdx = 0 ; mbPartIdx < 2 ; mbPartIdx++ ) {
		int index = SCAN8(mbPartIdx << 2) ;

		if ( Block -> MotionPredL0[mbPartIdx] == 1){
			int ref_index =  RefPartition [mbPartIdx << 1] . RefIndex;
			if (!Nal -> non_diadic){
				int index_mv = RefPartition [mbPartIdx << 1] . MvIndex;
				SetInterParam(Nal, &mvL0[0], &ref, Block -> MvdL0[mbPartIdx << 2], &mv_l0[index_mv], &ref_bas_l0[ref_index]);

			}else{
				ref = ref_bas_l0[ref_index];
				mvL0[0] = mv_cache [index][0] + Block -> MvdL0[mbPartIdx << 2][0];
				mvL0[1] = mv_cache [index][1] + Block -> MvdL0[mbPartIdx << 2][1];
			}
		}else{
			ref = Block -> RefIdxL0 [mbPartIdx];
			motion_vector_8x16(mv_cache, Block -> MvdL0[mbPartIdx << 2], ref_cache, mvL0, ref,index);
		}

		FillRefIdx(ref, &ref_cache [index], 1);
		FillMotionVector(mvL0, &mv_cache [index], 1);
	}	
	if ( Nal -> PicToDisplay ){
		//Recovery of  the prediction
		SampleInterpolation8x8(DpbLuma, DpbCb, DpbCr, image, image_Cb, image_Cr, mv_cache, ref_cache, x, y,  PicWidthInPix, PicHeightInPix, RefPicListL0, vectors -> Ptr_interpol_8x8);
	}
}





/**
Decode P 16x8 macroblock.
*/
void P_svc_16x8 ( short mv_cache [][2], short *ref_cache, const RESIDU *CurrResidu, const DATA *Block, const Sub_P_svc decode_sub[], 
				 unsigned char *DpbLuma, unsigned char *DpbCb, unsigned char *DpbCr, 
				 unsigned char *image, unsigned char *image_Cb, unsigned char *image_Cr, 
				 const short x, const short y, const short PicWidthInPix, const short PicHeightInPix, 
				 const NAL *Nal, LIST_MMO *RefPicListL0, short *mv_l0, short *ref_bas_l0,
				 const STRUCT_PF *vectors, REFPART RefPartition[16])
{

	short mbPartIdx ;
	short mvL0 [2];
	short ref;

	for ( mbPartIdx = 0 ; mbPartIdx < 2 ; mbPartIdx++ ) 	{
		int index = SCAN8(mbPartIdx << 3) ;

		if ( Block -> MotionPredL0[mbPartIdx] == 1){
			int ref_index =  RefPartition [mbPartIdx << 3] . RefIndex;
			if ( !Nal -> non_diadic){
				int index_mv = RefPartition [mbPartIdx << 3] . MvIndex;
				SetInterParam(Nal, &mvL0[0], &ref, Block -> MvdL0[mbPartIdx << 2], &mv_l0[index_mv], &ref_bas_l0[ref_index]);
			}else{
				ref = ref_bas_l0[ref_index];
				mvL0[0] = mv_cache [index][0] + Block -> MvdL0[mbPartIdx << 2][0];
				mvL0[1] = mv_cache [index][1] + Block -> MvdL0[mbPartIdx << 2][1];
			}
		}else{
			ref = Block -> RefIdxL0 [mbPartIdx];
			motion_vector_16x8(mv_cache, Block -> MvdL0[mbPartIdx << 2], ref_cache, mvL0, ref,index);
		}


		FillRefIdx(ref, &ref_cache [index], 2);
		FillMotionVector(mvL0 , &mv_cache [index], 2);
	}	
	if ( Nal -> PicToDisplay ){
		//Recovery of  the prediction
		SampleInterpolation8x8(DpbLuma, DpbCb, DpbCr, image, image_Cb, image_Cr, mv_cache, ref_cache, x, y,  PicWidthInPix, PicHeightInPix, RefPicListL0, vectors -> Ptr_interpol_8x8);
	}
}





/**
Decode P 16x16 macroblock.
*/
void P_svc_16x16 ( short mv_cache [][2], short *ref_cache, const RESIDU *CurrResidu, const DATA *Block, const Sub_P_svc decode_sub[], 
				  unsigned char *DpbLuma, unsigned char *DpbCb, unsigned char *DpbCr, 
				  unsigned char *image, unsigned char *image_Cb, unsigned char *image_Cr, 
				  const short x, const short y, const short PicWidthInPix, const short PicHeightInPix, 
				  const NAL *Nal, LIST_MMO *RefPicListL0, short *mv_l0, short *ref_bas_l0, 
				  const STRUCT_PF *vectors, REFPART RefPartition[16])
{


	short mvL0 [2];
	short ref;


	if ( Block -> MotionPredL0[0] == 1){
		int ref_index = RefPartition [0] . RefIndex;
		if ( !Nal -> non_diadic){
			int index_mv = RefPartition [0] . MvIndex;
			SetInterParam(Nal, &mvL0[0], &ref, Block -> MvdL0[0], &mv_l0[index_mv], &ref_bas_l0[ref_index]);
		}else{
			ref = ref_bas_l0[ref_index];
			mvL0[0] = mv_cache [12][0] + Block -> MvdL0[0][0];
			mvL0[1] = mv_cache [12][1] + Block -> MvdL0[0][1];
		}
	}else{
		ref = Block -> RefIdxL0 [0];
		motion_vector( mv_cache, Block -> MvdL0[0], ref_cache, mvL0, ref, 12, 4);
	}

	FillRefIdx(ref, &ref_cache [12], 3);
	FillMotionVector(mvL0, &mv_cache [12], 3);


	if ( Nal -> PicToDisplay ){
		//Recovery of  the prediction
		SampleInterpolation8x8(DpbLuma, DpbCb, DpbCr, image, image_Cb, image_Cr, mv_cache, ref_cache, x, y, PicWidthInPix, PicHeightInPix, RefPicListL0, vectors -> Ptr_interpol_8x8);
	}
}



/**
Decode P skip macroblock.
*/
void P_svc_skip( short mv_cache [][2], short *ref_cache, const RESIDU *CurrResidu, const DATA *Block, const Sub_P_svc decode_sub[], 
				unsigned char *DpbLuma, unsigned char *DpbCb, unsigned char *DpbCr, 
				unsigned char *image, unsigned char *image_Cb, unsigned char *image_Cr, 
				const short x, const short y, const short PicWidthInPix, const short PicHeightInPix, 
				const NAL *Nal, LIST_MMO * RefPicListL0, short *mv_l0, short *ref_bas_l0, 
				const STRUCT_PF *vectors, REFPART RefPartition[16])
{



	short ref = Block -> RefIdxL0 [0];
	FillRefIdx(ref, &ref_cache [12], 3);
	motion_vector_skip(mv_cache, ref_cache);
	FillMotionVector(&mv_cache [12][0], &mv_cache [12], 3);

	if ( Nal -> PicToDisplay ){
		//Recovery of the prediction
		SampleInterpolation8x8(DpbLuma, DpbCb, DpbCr, image, image_Cb, image_Cr, mv_cache, ref_cache, x, y, PicWidthInPix, PicHeightInPix, RefPicListL0, vectors -> Ptr_interpol_8x8);
	}
}
