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
#include "data.h"
#include "refill.h"
#include "motion_vector.h"
#include "interpolation.h"
#include "interpolation_8x8.h"
#include "Direct_prediction.h"
#include "interpolation_main.h"
#include "interpolation_main_8x8.h"


//SVC files
#include "Mb_P_svc.h"
#include "Mb_B_svc.h"
#include "fill_caches_svc.h"
#include "slice_data_I_svc.h"
#include "motion_vector_svc.h"




/*
4x4 Sub macroblock
*/
void SvcB4x4(const NAL *Nal, int mbPartIdx,	 const char *MotionPredLx, const short MvdLx[][2], const char *RefIdxLx, 
			 REFPART *RefPartition, short MvCacheLx[][2], short *RefCacheLX, short *MvBaseLx, short *RefBaseLx)
{

	short subMbPartIdx ;
	short mvL0 [2];

	for ( subMbPartIdx = 0 ; subMbPartIdx < 4 ; subMbPartIdx++ ) {
		const int ind = (mbPartIdx << 2) + subMbPartIdx ;
		const int index = SCAN8(ind) ;

		if ( MotionPredLx [mbPartIdx] == 1){
			short Idx = (subMbPartIdx & 1) + ((subMbPartIdx & 2) << 1);
			int ref_index = RefPartition [Idx] . RefIndex;
			RefCacheLX[index] = RefBaseLx[ref_index];
			if (!Nal -> non_diadic) {
				int index_mv  = RefPartition [Idx] . MvIndex;
				MvCacheLx [index][0] = MV_CAL(MvBaseLx[index_mv]	, Nal -> MvCalx [Nal -> LayerId]) + MvdLx[(mbPartIdx << 2) + subMbPartIdx][0]; 
				MvCacheLx [index][1] = MV_CAL(MvBaseLx[index_mv + 1], Nal -> MvCaly [Nal -> LayerId]) + MvdLx[(mbPartIdx << 2) + subMbPartIdx][1]; 
			}else{
				MvCacheLx[index][0] += MvdLx[mbPartIdx << 2][0];
				MvCacheLx[index][1] += MvdLx[mbPartIdx << 2][1];
			}
		}else{
			RefCacheLX [index] = RefIdxLx [mbPartIdx];
			motion_vector( MvCacheLx, MvdLx[ind], RefCacheLX, mvL0, RefIdxLx[mbPartIdx], index, 1);

			MvCacheLx [index][0] = mvL0 [0];
			MvCacheLx [index][1] = mvL0 [1];
		}
	}	
}



/*
4x4 Sub macroblock
*/
void Sub_B_svc_4x4 (short MvCache_l0 [][2], short MvCache_l1 [][2], short *RefCache_l0, short *RefCache_l1, 
					const DATA *aio_pBlock, int mbPartIdx, int SubMbType, short *mv_bas_l0, short *mv_bas_l1, 
					short *ref_bas_l0, short *ref_bas_l1, const NAL *Nal, REFPART RefPartitionL0[16])
{


	//B direct Mode
	if ( SubMbType == 0){
		const short index = SCAN8(mbPartIdx << 2);
		short mvL0 [2];
		short mvL1 [2];

		short Ref0 = SvcSkipMotionVector(MvCache_l0, aio_pBlock -> MvdL0[(mbPartIdx << 2)], RefCache_l0, mvL0, index, 2);
		short Ref1 = SvcSkipMotionVector(MvCache_l1, aio_pBlock -> MvdL1[(mbPartIdx << 2)], RefCache_l1, mvL1, index, 2);

		if (Ref0 == LIST_NOT_USED && Ref1 == LIST_NOT_USED){
			Ref0 = Ref1 = 0;
		}

		RefCache_l0 [index] = RefCache_l0 [index + 1] = RefCache_l0 [index + 8] = RefCache_l0 [index + 9] = Ref0;
		RefCache_l1 [index] = RefCache_l1 [index + 1] = RefCache_l1 [index + 8] = RefCache_l1 [index + 9] = Ref1; 


		MvCache_l0 [index + 0][0] = MvCache_l0 [index + 1][0] = MvCache_l0 [index + 8][0] = MvCache_l0 [index + 9][0] = mvL0 [0];
		MvCache_l0 [index + 0][1] = MvCache_l0 [index + 1][1] = MvCache_l0 [index + 8][1] = MvCache_l0 [index + 9][1] = mvL0 [1];

		MvCache_l1 [index + 0][0] = MvCache_l1 [index + 1][0] = MvCache_l1 [index + 8][0] = MvCache_l1 [index + 9][0] = mvL1 [0];
		MvCache_l1 [index + 0][1] = MvCache_l1 [index + 1][1] = MvCache_l1 [index + 8][1] = MvCache_l1 [index + 9][1] = mvL1 [1];

	}else if ( SubMbType == 10){
		//L0 prediction
		SvcB4x4(Nal, mbPartIdx, aio_pBlock -> MotionPredL0, aio_pBlock -> MvdL0, aio_pBlock -> RefIdxL0, RefPartitionL0, 
			MvCache_l0, RefCache_l0, mv_bas_l0, ref_bas_l0);
	}else if ( SubMbType == 11){
		//L1 prediction
		SvcB4x4(Nal, mbPartIdx, aio_pBlock -> MotionPredL1, aio_pBlock -> MvdL1, aio_pBlock -> RefIdxL1, &RefPartitionL0[16], 
			MvCache_l1, RefCache_l1, mv_bas_l1, ref_bas_l1);
	}else {
		//Bi prediction
		SvcB4x4(Nal, mbPartIdx, aio_pBlock -> MotionPredL0, aio_pBlock -> MvdL0, aio_pBlock -> RefIdxL0, RefPartitionL0, 
			MvCache_l0, RefCache_l0, mv_bas_l0, ref_bas_l0);
		SvcB4x4(Nal, mbPartIdx, aio_pBlock -> MotionPredL1, aio_pBlock -> MvdL1, aio_pBlock -> RefIdxL1, &RefPartitionL0[16], 
			MvCache_l1, RefCache_l1, mv_bas_l1, ref_bas_l1);
	}					
}




/*
8x4 Sub macroblock
*/
void SvcB8x4(const NAL *Nal, int mbPartIdx,	 const char *MotionPredLx, const short MvdLx[][2], const char *RefIdxLx, 
			 REFPART RefPartition[16], short MvCacheLx[][2], short *RefCacheLX, short *MvBaseLx, short *RefBaseLx)
{

	short subMbPartIdx ;
	short mvL0 [2];

	for ( subMbPartIdx = 0 ; subMbPartIdx < 2 ; subMbPartIdx++ ) {
		const int ind = (mbPartIdx << 2) + (subMbPartIdx << 1);
		const int index = SCAN8(ind) ;

		if ( MotionPredLx [mbPartIdx] == 1){
			short Idx = (subMbPartIdx & 1) << 2;
			int ref_index = RefPartition [Idx] . RefIndex;
			RefCacheLX [index] = RefCacheLX [index + 1] = RefBaseLx[ref_index];

			if ( !Nal -> non_diadic){
				int index_mv  = RefPartition [Idx] . MvIndex;
				mvL0 [0] = MV_CAL(MvBaseLx[index_mv]	, Nal -> MvCalx [Nal -> LayerId]) + MvdLx[( mbPartIdx << 2) + subMbPartIdx][0]; 
				mvL0 [1] = MV_CAL(MvBaseLx[index_mv + 1], Nal -> MvCaly [Nal -> LayerId]) + MvdLx[( mbPartIdx << 2) + subMbPartIdx][1]; 
			}else{
				mvL0 [0] = MvCacheLx [index][0] + MvdLx[( mbPartIdx << 2) + subMbPartIdx][0]; 
				mvL0 [1] = MvCacheLx [index][1] + MvdLx[( mbPartIdx << 2) + subMbPartIdx][1]; 
			}
		}else{
			RefCacheLX [index] = RefCacheLX [index + 1] = RefIdxLx [mbPartIdx];
			motion_vector(MvCacheLx, MvdLx[( mbPartIdx << 2) + subMbPartIdx], RefCacheLX, mvL0, RefIdxLx[mbPartIdx], index, 2);
		}

		MvCacheLx [index][0] = MvCacheLx [index + 1][0] = mvL0 [0];
		MvCacheLx [index][1] = MvCacheLx [index + 1][1] = mvL0 [1];	
	}
}


/*
8x4 Sub macroblock
*/
void Sub_B_svc_8x4 ( short MvCache_l0 [][2], short MvCache_l1 [][2], short *RefCache_l0, short *RefCache_l1,
					const DATA *aio_pBlock, int mbPartIdx, int SubMbType, short *mv_bas_l0, short *mv_bas_l1, 
					short *ref_bas_l0, short *ref_bas_l1, const NAL *Nal, REFPART RefPartitionL0[16])
{



	if (  SubMbType == 4 ){
		//L0 prediction
		SvcB8x4(Nal, mbPartIdx, aio_pBlock -> MotionPredL0, aio_pBlock -> MvdL0, aio_pBlock -> RefIdxL0, RefPartitionL0, 
			MvCache_l0, RefCache_l0, mv_bas_l0, ref_bas_l0);
	}else if ( SubMbType == 6 ){
		//L1 prediction
		SvcB8x4(Nal, mbPartIdx, aio_pBlock -> MotionPredL1, aio_pBlock -> MvdL1, aio_pBlock -> RefIdxL1, &RefPartitionL0[16], 
			MvCache_l1, RefCache_l1, mv_bas_l1, ref_bas_l1);
	}else{
		//Bi prediction
		SvcB8x4(Nal, mbPartIdx, aio_pBlock -> MotionPredL0, aio_pBlock -> MvdL0, aio_pBlock -> RefIdxL0, RefPartitionL0, 
			MvCache_l0, RefCache_l0, mv_bas_l0, ref_bas_l0);
		SvcB8x4(Nal, mbPartIdx, aio_pBlock -> MotionPredL1, aio_pBlock -> MvdL1, aio_pBlock -> RefIdxL1, &RefPartitionL0[16], 
			MvCache_l1, RefCache_l1, mv_bas_l1, ref_bas_l1);
	}
}






/*
4x8 Sub macroblock
*/
void SvcB4x8(const NAL *Nal, int mbPartIdx, const char *MotionPredLx, const short MvdLx[][2], const char *RefIdxLx, 
			 REFPART *RefPartition, short MvCacheLx[][2], short *RefCacheLX, short *MvBaseLx, short *RefBaseLx)
{

	int subMbPartIdx ;
	short mvL0 [2];

	for ( subMbPartIdx = 0 ; subMbPartIdx < 2 ; subMbPartIdx++ ) {
		const int ind = (mbPartIdx << 2) + subMbPartIdx ;
		const int index = SCAN8(ind) ;

		if ( MotionPredLx [mbPartIdx] == 1){
			int ref_index = RefPartition [subMbPartIdx] . RefIndex;
			RefCacheLX [index] = RefCacheLX [index + 8] = RefBaseLx[ref_index];
			if( !Nal -> non_diadic){
				int index_mv  = RefPartition [subMbPartIdx] . MvIndex;
				mvL0 [0] = MV_CAL(MvBaseLx[index_mv]	, Nal -> MvCalx [Nal -> LayerId]) + MvdLx[( mbPartIdx << 2) + subMbPartIdx][0]; 
				mvL0 [1] = MV_CAL(MvBaseLx[index_mv + 1], Nal -> MvCaly [Nal -> LayerId]) + MvdLx[( mbPartIdx << 2) + subMbPartIdx][1];
			}else {
				mvL0 [0] = MvCacheLx [index][0] + MvdLx[( mbPartIdx << 2) + subMbPartIdx][0];
				mvL0 [1] = MvCacheLx [index][1] + MvdLx[( mbPartIdx << 2) + subMbPartIdx][1];
			}
		}else{
			RefCacheLX [index] = RefCacheLX [index + 8] = RefIdxLx [mbPartIdx];
			motion_vector(MvCacheLx, MvdLx[ind], RefCacheLX, mvL0, RefIdxLx[mbPartIdx], index, 1);

		}
		MvCacheLx [index][0] = MvCacheLx [index + 8][0] = mvL0 [0];
		MvCacheLx [index][1] = MvCacheLx [index + 8][1] = mvL0 [1];
	}
}





/*
4x8 Sub macroblock
*/
void Sub_B_svc_4x8( short MvCache_l0 [][2], short MvCache_l1 [][2], short *RefCache_l0, short *RefCache_l1, 
				   const DATA *aio_pBlock, int mbPartIdx, int SubMbType, short *mv_bas_l0, short *mv_bas_l1, 
				   short *ref_bas_l0, short *ref_bas_l1, const NAL *Nal, REFPART RefPartitionL0[16])
{


	if ( SubMbType == 5 ){
		//L0 prediction
		SvcB4x8(Nal, mbPartIdx, aio_pBlock -> MotionPredL0, aio_pBlock -> MvdL0, aio_pBlock -> RefIdxL0, RefPartitionL0, 
			MvCache_l0, RefCache_l0, mv_bas_l0, ref_bas_l0);
	}else if (SubMbType == 7 ){
		//L1 prediction
		SvcB4x8(Nal, mbPartIdx, aio_pBlock -> MotionPredL1, aio_pBlock -> MvdL1, aio_pBlock -> RefIdxL1, &RefPartitionL0[16], 
			MvCache_l1, RefCache_l1, mv_bas_l1, ref_bas_l1);
	}else{
		//Bi prediction
		SvcB4x8(Nal, mbPartIdx, aio_pBlock -> MotionPredL0, aio_pBlock -> MvdL0, aio_pBlock -> RefIdxL0, RefPartitionL0, 
			MvCache_l0, RefCache_l0, mv_bas_l0, ref_bas_l0);
		SvcB4x8(Nal, mbPartIdx, aio_pBlock -> MotionPredL1, aio_pBlock -> MvdL1, aio_pBlock -> RefIdxL1, &RefPartitionL0[16], 
			MvCache_l1, RefCache_l1, mv_bas_l1, ref_bas_l1);
	}
}




/*
8x8 Sub macroblock
*/
void SvcB8x8(const NAL *Nal, int mbPartIdx, const char *MotionPredLx, const short MvdLx[][2], const char *RefIdxLx, 
			 REFPART *RefPartition, short MvCacheLx[][2], short *RefCacheLX, short *MvBaseLx, short *RefBaseLx)
{

	short ref;
	short mvL0 [2];
	const int index = SCAN8(mbPartIdx << 2);

	if ( MotionPredLx [mbPartIdx] == 1){
		int ref_index = RefPartition [0] . RefIndex;
		ref = RefBaseLx[ref_index];

		if ( !Nal -> non_diadic){
			int index_mv  = RefPartition [0] . MvIndex;

			mvL0[0] = MV_CAL(MvBaseLx [index_mv]	, Nal -> MvCalx [Nal -> LayerId]) + MvdLx[mbPartIdx << 2][0]; 
			mvL0[1] = MV_CAL(MvBaseLx [index_mv + 1], Nal -> MvCaly [Nal -> LayerId]) + MvdLx[mbPartIdx << 2][1]; 
		}else {
			mvL0[0] = MvCacheLx [index][0] + MvdLx[mbPartIdx << 2][0]; 
			mvL0[1] = MvCacheLx [index][1] + MvdLx[mbPartIdx << 2][1]; 
		}			 
	}else{
		ref = RefIdxLx [mbPartIdx];
		motion_vector( MvCacheLx, MvdLx[mbPartIdx << 2], RefCacheLX, mvL0, RefIdxLx[mbPartIdx], index, 2);
	}

	RefCacheLX [index] = RefCacheLX [index + 1] = RefCacheLX [index + 8] = RefCacheLX [index + 9] = ref;

	MvCacheLx [index][0] = MvCacheLx [index + 1][0] = MvCacheLx [index + 8][0] = MvCacheLx [index + 9][0] = mvL0 [0];
	MvCacheLx [index][1] = MvCacheLx [index + 1][1] = MvCacheLx [index + 8][1] = MvCacheLx [index + 9][1] = mvL0 [1];
}







/*
8x8 Sub macroblock
*/
void Sub_B_svc_8x8 ( short MvCache_l0 [][2], short MvCache_l1 [][2], short *RefCache_l0, short *RefCache_l1, 
					const DATA *aio_pBlock, int mbPartIdx, int SubMbType, short *mv_bas_l0, short *mv_bas_l1, 
					short *ref_bas_l0, short *ref_bas_l1, const NAL *Nal, REFPART RefPartitionL0[16])
{


	if (SubMbType == 1 ){
		SvcB8x8(Nal, mbPartIdx, aio_pBlock -> MotionPredL0, aio_pBlock -> MvdL0, aio_pBlock -> RefIdxL0, RefPartitionL0, 
			MvCache_l0, RefCache_l0, mv_bas_l0, ref_bas_l0);
	}else if( SubMbType == 2){
		SvcB8x8(Nal, mbPartIdx, aio_pBlock -> MotionPredL1, aio_pBlock -> MvdL1, aio_pBlock -> RefIdxL1, &RefPartitionL0[16], 
			MvCache_l1, RefCache_l1, mv_bas_l1, ref_bas_l1);
	}else{
		SvcB8x8(Nal, mbPartIdx, aio_pBlock -> MotionPredL0, aio_pBlock -> MvdL0, aio_pBlock -> RefIdxL0, RefPartitionL0, 
			MvCache_l0, RefCache_l0, mv_bas_l0, ref_bas_l0);
		SvcB8x8(Nal, mbPartIdx, aio_pBlock -> MotionPredL1, aio_pBlock -> MvdL1, aio_pBlock -> RefIdxL1, &RefPartitionL0[16], 
			MvCache_l1, RefCache_l1, mv_bas_l1, ref_bas_l1);
	}
}







/*
8x8 macroblock
*/
void B_svc_8x8(short MvCache_l0 [][2], short MvCache_l1 [][2], short *RefCache_l0, short *RefCache_l1, 
			   const RESIDU *CurrResidu, const DATA *aio_pBlock, Sub_B_svc aio_tdecode_sub[], 
			   unsigned char *DpbLuma, unsigned char *DpbCb, unsigned char *DpbCr, 
			   unsigned char *image, unsigned char *image_Cb, unsigned char *image_Cr, 
			   const short x, const short y, const short PicWidthInPix, const short PicHeightInPix, 
			   const NAL *Nal, LIST_MMO *RefPicListL0, LIST_MMO *RefPicListL1, short *mv_bas_l0, 
			   short *mv_bas_l1, short *ref_bas_l0, short *ref_bas_l1, const STRUCT_PF *vectors, REFPART RefPartitionL0[16])
{



	int mbPartIdx ;
	unsigned char image_l0[256];
	unsigned char image_l1[256];
	unsigned char image_Cb_l0[64];
	unsigned char image_Cb_l1[64];
	unsigned char image_Cr_l0[64];
	unsigned char image_Cr_l1[64];


	for ( mbPartIdx = 0 ; mbPartIdx < 4 ; mbPartIdx++ ) {
		int mode = CurrResidu -> SubMbType [mbPartIdx] ;

		RefCache_l0 [SCAN8(mbPartIdx << 2)] = RefCache_l1 [SCAN8(mbPartIdx << 2)] = -1;

		aio_tdecode_sub [mode](MvCache_l0, MvCache_l1, RefCache_l0, RefCache_l1, aio_pBlock, mbPartIdx, 
			mode, mv_bas_l0, mv_bas_l1, ref_bas_l0, ref_bas_l1, Nal, &RefPartitionL0[(((mbPartIdx & 2) << 1) + (mbPartIdx & 1)) << 1]);						
	}

	if ( Nal -> PicToDisplay ){
		sample_interpolation_main(DpbLuma, DpbCb, DpbCr, image_l0, image_l1, image_Cb_l0, image_Cb_l1, 
			image_Cr_l0, image_Cr_l1, MvCache_l0, MvCache_l1, RefCache_l0, RefCache_l1, x, y, 
			PicWidthInPix, PicHeightInPix , RefPicListL0, RefPicListL1, vectors -> Ptr_interpol_4x4);

		set_image_4x4(image, image_Cb, image_Cr, image_l0, image_l1, image_Cb_l0, image_Cb_l1, image_Cr_l0, 
			image_Cr_l1, RefCache_l0, RefCache_l1, PicWidthInPix);
	}
}






/*
16x16 macroblock
*/
void B_svc_16x16( short MvCache_l0 [][2], short MvCache_l1 [][2], short *RefCache_l0, short *RefCache_l1, 
				 const RESIDU *CurrResidu, const DATA *aio_pBlock,  Sub_B_svc aio_tdecode_sub[], 
				 unsigned char *DpbLuma, unsigned char *DpbCb, unsigned char *DpbCr, 
				 unsigned char *image, unsigned char *image_Cb, unsigned char *image_Cr,
				 const short x, const short y, const short PicWidthInPix, const short PicHeightInPix, 
				 const NAL *Nal, LIST_MMO *RefPicListL0, LIST_MMO *RefPicListL1, short *mv_bas_l0, 
				 short *mv_bas_l1, short *ref_bas_l0, short *ref_bas_l1, const STRUCT_PF *vectors, REFPART RefPartitionL0[16])
{

	short mvL0 [2];
	short ref;
	unsigned char image_l0[256];
	unsigned char image_l1[256];
	unsigned char image_Cb_l0[64];
	unsigned char image_Cb_l1[64];
	unsigned char image_Cr_l0[64];
	unsigned char image_Cr_l1[64];

	//mbl check
	RefCache_l0 [12 + 2] = RefCache_l0 [12 + 18] = 
	RefCache_l1 [12 + 2] = RefCache_l1 [12 + 18] =  -1;


	if ( aio_pBlock -> MbPartPredMode[0] == Pred_L0 || aio_pBlock -> MbPartPredMode[0] == B_Bi	){
		if ( aio_pBlock -> MotionPredL0 [0] == 1){
			int ref_index = RefPartitionL0 [0] . RefIndex;
			ref = ref_bas_l0[ref_index];
			if ( !Nal -> non_diadic){
				int index_mv = RefPartitionL0 [0] . MvIndex;		
				mvL0[0] = MV_CAL(mv_bas_l0[index_mv]	, Nal -> MvCalx [Nal -> LayerId]) + aio_pBlock -> MvdL0[0][0]; 
				mvL0[1] = MV_CAL(mv_bas_l0[index_mv + 1], Nal -> MvCaly [Nal -> LayerId]) + aio_pBlock -> MvdL0[0][1]; 
			}else{
				mvL0[0] = MvCache_l0 [12 + 0][0] + aio_pBlock -> MvdL0[0][0]; 
				mvL0[1] = MvCache_l0 [12 + 0][1] + aio_pBlock -> MvdL0[0][1]; 
			}
		}else {
			ref = aio_pBlock -> RefIdxL0 [0];
			motion_vector(MvCache_l0, aio_pBlock -> MvdL0[0], RefCache_l0, mvL0, aio_pBlock -> RefIdxL0[0], 12, 4);
		}

		FillRefIdx(ref, &RefCache_l0 [12], 3);
		FillMotionVector(mvL0 , &MvCache_l0 [12], 3);
	}



	if (aio_pBlock -> MbPartPredMode[0] == Pred_L1 || aio_pBlock -> MbPartPredMode[0] == B_Bi){
		if ( aio_pBlock -> MotionPredL1 [0] == 1){
			int ref_index = RefPartitionL0 [16] . RefIndex;
			ref = ref_bas_l1[ref_index];
			if (!Nal -> non_diadic){
				int index_mv = RefPartitionL0 [0] . MvIndex;
				mvL0[0] = MV_CAL(mv_bas_l1[index_mv]	, Nal -> MvCalx [Nal -> LayerId]) + aio_pBlock -> MvdL1[0][0]; 
				mvL0[1] = MV_CAL(mv_bas_l1[index_mv + 1], Nal -> MvCaly [Nal -> LayerId]) + aio_pBlock -> MvdL1[0][1]; 
			}else{
				mvL0[0] = MvCache_l1 [12 + 0][0] + aio_pBlock -> MvdL1[0][0]; 
				mvL0[1] = MvCache_l1 [12 + 0][1] + aio_pBlock -> MvdL1[0][1];  
			}
		}else { 
			ref = aio_pBlock -> RefIdxL1 [0];
			motion_vector(MvCache_l1, aio_pBlock -> MvdL1[0], RefCache_l1, mvL0, aio_pBlock -> RefIdxL1[0], 12, 4);
		}

		FillRefIdx(ref, &RefCache_l1 [12], 3);
		FillMotionVector(mvL0, &MvCache_l1 [12], 3);
	}

	if ( Nal -> PicToDisplay ){
		//L0 interpolation
		SampleInterpolationMain8x8(DpbLuma, DpbCb, DpbCr, image_l0, image_Cb_l0, image_Cr_l0, MvCache_l0, 
			RefCache_l0, x, y, PicWidthInPix, PicHeightInPix, RefPicListL0, vectors -> Ptr_interpol_8x8);

		//L0 interpolation
		SampleInterpolationMain8x8(DpbLuma, DpbCb, DpbCr, image_l1, image_Cb_l1, image_Cr_l1, MvCache_l1, 
			RefCache_l1, x, y, PicWidthInPix, PicHeightInPix, RefPicListL1, vectors -> Ptr_interpol_8x8  );

		//Final prediction
		set_image_8x8(image, image_Cb, image_Cr, image_l0, image_l1, image_Cb_l0, image_Cb_l1, image_Cr_l0, 
			image_Cr_l1, RefCache_l0, RefCache_l1, PicWidthInPix);
	}
}







/*
8x16 macroblock
*/
void B_svc_8x16(short MvCache_l0 [][2], short MvCache_l1 [][2], short *RefCache_l0, short *RefCache_l1, 
				const RESIDU *CurrResidu, const DATA *aio_pBlock,  Sub_B_svc aio_tdecode_sub[], 
				unsigned char *DpbLuma, unsigned char *DpbCb, unsigned char *DpbCr, 
				unsigned char *image, unsigned char *image_Cb, unsigned char *image_Cr, const short x, 
				const short y, const short PicWidthInPix, const short PicHeightInPix, const NAL *Nal, 
				LIST_MMO *RefPicListL0, LIST_MMO *RefPicListL1, short *mv_bas_l0, short *mv_bas_l1, 
				short *ref_bas_l0, short *ref_bas_l1, const STRUCT_PF *vectors, REFPART RefPartitionL0[16])
{


	short mbPartIdx ;
	short mvL0 [2];
	short ref;
	unsigned char image_l0[256];
	unsigned char image_l1[256];
	unsigned char image_Cb_l0[64];
	unsigned char image_Cb_l1[64];
	unsigned char image_Cr_l0[64];
	unsigned char image_Cr_l1[64];

	for (mbPartIdx = 0;  mbPartIdx < 2; mbPartIdx++){
		const int index = 	SCAN8 (mbPartIdx << 2);	
		RefCache_l0 [index] =  RefCache_l0 [index + 16] = 
		RefCache_l1 [index] =  RefCache_l1 [index + 16] = -1;

		if ( aio_pBlock -> MbPartPredMode[mbPartIdx] == Pred_L0 || aio_pBlock -> MbPartPredMode[mbPartIdx] == B_Bi){
			if ( aio_pBlock -> MotionPredL0 [mbPartIdx] == 1){
				int ref_index =  RefPartitionL0 [mbPartIdx << 1] . RefIndex;
				if (!Nal -> non_diadic){
					int index_mv = RefPartitionL0 [mbPartIdx << 1] . MvIndex;
					SetInterParam(Nal, &mvL0[0], &ref, aio_pBlock -> MvdL0[mbPartIdx << 2], &mv_bas_l0[index_mv], &ref_bas_l0[ref_index]);
				}else{
					ref = ref_bas_l0[ref_index];
					mvL0[0] = MvCache_l0 [index][0] + aio_pBlock -> MvdL0[mbPartIdx << 2][0];
					mvL0[1] = MvCache_l0 [index][1] + aio_pBlock -> MvdL0[mbPartIdx << 2][1];
				}
			}else{
				ref = aio_pBlock -> RefIdxL0 [mbPartIdx];
				motion_vector_8x16(MvCache_l0, aio_pBlock -> MvdL0[mbPartIdx << 2], RefCache_l0, mvL0, aio_pBlock -> RefIdxL0[mbPartIdx], index);
			}


			FillRefIdx(ref, &RefCache_l0 [index], 1);
			FillMotionVector(mvL0 , &MvCache_l0 [index], 1);
		}

		if (aio_pBlock -> MbPartPredMode[mbPartIdx] == Pred_L1 || aio_pBlock -> MbPartPredMode[mbPartIdx] == B_Bi){
			REFPART *RefPartitionL1 = &RefPartitionL0[16];
			if ( aio_pBlock -> MotionPredL1 [mbPartIdx] == 1){
				int ref_index =  RefPartitionL1 [mbPartIdx << 1] . RefIndex;
				if (!Nal -> non_diadic){
					int index_mv = RefPartitionL1 [mbPartIdx << 1] . MvIndex;
					SetInterParam(Nal, &mvL0[0], &ref, aio_pBlock -> MvdL1[mbPartIdx << 2], &mv_bas_l1[index_mv], &ref_bas_l1[ref_index]);
				}else{
					ref = ref_bas_l1[ref_index];
					mvL0[0] = MvCache_l1 [index][0] + aio_pBlock -> MvdL1[mbPartIdx << 2][0];
					mvL0[1] = MvCache_l1 [index][1] + aio_pBlock -> MvdL1[mbPartIdx << 2][1];
				}
			}else{
				ref = aio_pBlock -> RefIdxL1 [mbPartIdx];
				motion_vector_8x16(MvCache_l1, aio_pBlock -> MvdL1[mbPartIdx << 2], RefCache_l1, mvL0, aio_pBlock -> RefIdxL1[mbPartIdx], index);
			}

			FillRefIdx(ref, &RefCache_l1 [index], 1);
			FillMotionVector(mvL0 , &MvCache_l1 [index], 1);
		}	
	}

	if ( Nal -> PicToDisplay ){
		//L0 interpolation
		SampleInterpolationMain8x8(DpbLuma, DpbCb, DpbCr, image_l0, image_Cb_l0, image_Cr_l0, MvCache_l0, 
			RefCache_l0, x, y, PicWidthInPix, PicHeightInPix, RefPicListL0, vectors -> Ptr_interpol_8x8  );

		//L0 interpolation
		SampleInterpolationMain8x8(DpbLuma, DpbCb, DpbCr, image_l1, image_Cb_l1, image_Cr_l1, MvCache_l1, 
			RefCache_l1, x, y,  PicWidthInPix, PicHeightInPix, RefPicListL1, vectors -> Ptr_interpol_8x8);

		//Final prediction
		set_image_8x8(image, image_Cb, image_Cr, image_l0, image_l1, image_Cb_l0, image_Cb_l1, image_Cr_l0, 
			image_Cr_l1, RefCache_l0, RefCache_l1, PicWidthInPix);
	}
}





/*
16x8 macroblock
*/
void B_svc_16x8 (short MvCache_l0 [][2], short MvCache_l1 [][2], short *RefCache_l0, short *RefCache_l1, 
				 const RESIDU *CurrResidu, const DATA *aio_pBlock,  Sub_B_svc aio_tdecode_sub[], 
				 unsigned char *DpbLuma, unsigned char *DpbCb, unsigned char *DpbCr, 
				 unsigned char *image, unsigned char *image_Cb, unsigned char *image_Cr, 
				 const short x, const short y, const short PicWidthInPix, const short PicHeightInPix, 
				 const NAL *Nal, LIST_MMO * RefPicListL0, LIST_MMO *  RefPicListL1, short *mv_bas_l0, 
				 short *mv_bas_l1, short *ref_bas_l0, short *ref_bas_l1, const STRUCT_PF *vectors, REFPART RefPartitionL0[16])
{




	short mbPartIdx ;
	short mvL0 [2];
	short ref;
	unsigned char image_l0[256];
	unsigned char image_l1[256];
	unsigned char image_Cb_l0[64];
	unsigned char image_Cb_l1[64];
	unsigned char image_Cr_l0[64];
	unsigned char image_Cr_l1[64];

	for (mbPartIdx = 0;  mbPartIdx < 2; mbPartIdx++){
		const int index = SCAN8 (mbPartIdx << 3);	
		RefCache_l0 [index + 2] =	RefCache_l1 [index + 2] = -1;

		if ( aio_pBlock -> MbPartPredMode[mbPartIdx] == Pred_L0 || aio_pBlock -> MbPartPredMode[mbPartIdx] == B_Bi){
			if ( aio_pBlock -> MotionPredL0 [mbPartIdx] == 1){
				int ref_index = RefPartitionL0 [mbPartIdx << 3] . RefIndex;
				if ( !Nal -> non_diadic){
					int index_mv = RefPartitionL0 [mbPartIdx << 3] . MvIndex;
					SetInterParam(Nal, &mvL0[0], &ref, aio_pBlock -> MvdL0[mbPartIdx << 2], &mv_bas_l0[index_mv], &ref_bas_l0[ref_index]);
				}else{
					ref = ref_bas_l0[ref_index];
					mvL0[0] = MvCache_l0 [index][0] + aio_pBlock -> MvdL0[mbPartIdx << 2][0];
					mvL0[1] = MvCache_l0 [index][1] + aio_pBlock -> MvdL0[mbPartIdx << 2][1];
				}
			}else{
				ref = aio_pBlock -> RefIdxL0 [mbPartIdx];
				motion_vector_16x8(MvCache_l0,aio_pBlock -> MvdL0[mbPartIdx << 2], RefCache_l0, mvL0, aio_pBlock -> RefIdxL0[mbPartIdx], index);
			}

			FillRefIdx(ref, &RefCache_l0 [index], 2);
			FillMotionVector(mvL0 , &MvCache_l0 [index], 2);
		}



		if (aio_pBlock -> MbPartPredMode[mbPartIdx] == Pred_L1 || aio_pBlock -> MbPartPredMode[mbPartIdx] == B_Bi){
			REFPART *RefPartitionL1 = &RefPartitionL0[16];
			if ( aio_pBlock -> MotionPredL1 [mbPartIdx] == 1){
				int ref_index = RefPartitionL1 [mbPartIdx << 3] . RefIndex;
				if ( !Nal -> non_diadic){
					int index_mv = RefPartitionL1 [mbPartIdx << 3] . MvIndex;
					SetInterParam(Nal, &mvL0[0], &ref, aio_pBlock -> MvdL1[mbPartIdx << 2], &mv_bas_l1[index_mv], &ref_bas_l1[ref_index]);

				}else{
					ref = ref_bas_l1[ref_index];
					mvL0[0] = MvCache_l1 [index][0] + aio_pBlock -> MvdL1[mbPartIdx << 2][0];
					mvL0[1] = MvCache_l1 [index][1] + aio_pBlock -> MvdL1[mbPartIdx << 2][1];
				}
			}else{
				ref = aio_pBlock -> RefIdxL1 [mbPartIdx];
				motion_vector_16x8(MvCache_l1, aio_pBlock -> MvdL1[mbPartIdx << 2], RefCache_l1, mvL0, aio_pBlock -> RefIdxL1[mbPartIdx], index);
			}

			FillRefIdx(ref, &RefCache_l1 [index], 2);
			FillMotionVector(mvL0, &MvCache_l1 [index], 2);
		}
	}

	if ( Nal -> PicToDisplay ){
		//L0 interpolation
		SampleInterpolationMain8x8(DpbLuma, DpbCb, DpbCr, image_l0, image_Cb_l0, image_Cr_l0, MvCache_l0, RefCache_l0, 
			x, y,  PicWidthInPix, PicHeightInPix, RefPicListL0, vectors -> Ptr_interpol_8x8);

		//L0 interpolation
		SampleInterpolationMain8x8(DpbLuma, DpbCb, DpbCr, image_l1, image_Cb_l1, image_Cr_l1, MvCache_l1, RefCache_l1, 
			x, y,  PicWidthInPix, PicHeightInPix, RefPicListL1, vectors -> Ptr_interpol_8x8);

		//Final prediction
		set_image_8x8(image, image_Cb, image_Cr, image_l0, image_l1, image_Cb_l0, image_Cb_l1, image_Cr_l0, image_Cr_l1, 
			RefCache_l0, RefCache_l1, PicWidthInPix);
	}
}






/*
Skip macroblock
*/
void B_svc_Skip(short MvCache_l0 [][2], short MvCache_l1 [][2], short *RefCache_l0, short *RefCache_l1, 
				const RESIDU *CurrResidu, const DATA *aio_pBlock, Sub_B_svc aio_tdecode_sub[], 
				unsigned char *DpbLuma, unsigned char *DpbCb, unsigned char *DpbCr, 
				unsigned char *image, unsigned char *image_Cb, unsigned char *image_Cr, 
				const short x, const short y, const short PicWidthInPix, const short PicHeightInPix, 
				const NAL *Nal, LIST_MMO *RefPicListL0, LIST_MMO *RefPicListL1, short *mv_bas_l0, short *mv_bas_l1, 
				short *ref_bas_l0, short *ref_bas_l1, const STRUCT_PF *vectors, REFPART RefPartitionL0[16])
{



	short mvL0 [2];
	short mvL1 [2];
	unsigned char image_l0[256];
	unsigned char image_l1[256];
	unsigned char image_Cb_l0[64];
	unsigned char image_Cb_l1[64];
	unsigned char image_Cr_l0[64];
	unsigned char image_Cr_l1[64];


	int Ref0 = SvcSkipMotionVector(MvCache_l0, aio_pBlock -> MvdL0[0], RefCache_l0, mvL0, 12, 4);
	int Ref1 = SvcSkipMotionVector(MvCache_l1, aio_pBlock -> MvdL1[0], RefCache_l1, mvL1, 12, 4);

	if (Ref0 == LIST_NOT_USED && Ref1 == LIST_NOT_USED){
		Ref0 = Ref1 = 0;
	}

	FillRefIdx(Ref0, &RefCache_l0 [12], 3);
	FillRefIdx(Ref1, &RefCache_l1 [12], 3);
	FillMotionVector(mvL0, &MvCache_l0 [12], 3);
	FillMotionVector(mvL1, &MvCache_l1 [12], 3);


	if ( Nal -> PicToDisplay ){
		//L0 interpolation
		SampleInterpolationMain8x8(DpbLuma, DpbCb, DpbCr, image_l0, image_Cb_l0, image_Cr_l0, MvCache_l0, 
			RefCache_l0, x, y, PicWidthInPix, PicHeightInPix, RefPicListL0, vectors -> Ptr_interpol_8x8);

		//L0 interpolation
		SampleInterpolationMain8x8(DpbLuma, DpbCb, DpbCr, image_l1, image_Cb_l1, image_Cr_l1, MvCache_l1, 
			RefCache_l1, x, y, PicWidthInPix, PicHeightInPix, RefPicListL1, vectors -> Ptr_interpol_8x8  );

		//Final prediction
		set_image_8x8(image, image_Cb, image_Cr, image_l0, image_l1, image_Cb_l0, image_Cb_l1, image_Cr_l0, 
			image_Cr_l1, RefCache_l0, RefCache_l1, PicWidthInPix);
	}
}
