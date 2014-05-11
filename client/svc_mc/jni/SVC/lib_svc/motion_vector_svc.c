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
#include "motion_vector.h"
#include "interpolation.h"
#include "interpolation_main.h"

//SVC Files
#include "svc_data.h"
#include "motion_vector_svc.h"


#define SIGN(X) (X > 0 ? 1: -1)







/**
Predict from base layer motion vectors and reference indexes for sample interpolation.
*/
void get_base_P_dyadic(short mv_cache [][2], short *ref_cache, unsigned char *DpbLuma, unsigned char *DpbCb, unsigned char *DpbCr, 
					   unsigned char *Y, unsigned char *U, unsigned char *V, short x, short y, short PicWidthInPix, short PicHeightInPix, 
					   LIST_MMO *RefPicListL0, short *mv_l0, short *ref_base, REFPART *RefPartition, const NAL *Nal, const interpol_4x4 *interpol_4x4)
{


	int Idx;
	for ( Idx = 0 ; Idx < 16 ; Idx++ ) {
		int index_mv =  RefPartition [Idx] . MvIndex;
		int ref_index = RefPartition [Idx] . RefIndex;
		int index = 12 + (Idx & 0x03) + ((Idx >> 2) << 3);	
		ref_cache [index] = ref_base[ref_index];
		mv_cache [index][0] = MV_CAL(mv_l0[index_mv]	, Nal -> MvCalx [Nal -> LayerId]); 
		mv_cache [index][1] = MV_CAL(mv_l0[index_mv + 1], Nal -> MvCaly [Nal -> LayerId]); 
	}


	if ( Nal -> PicToDisplay){
		//Recovery of  the prediction
		sample_interpolation(DpbLuma, DpbCb, DpbCr, Y, U, V, mv_cache, ref_cache, x, y, PicWidthInPix, PicHeightInPix, RefPicListL0, interpol_4x4);	
	}
}





/**
Predict from base layer motion vectors and reference indexes for sample interpolation
when up-scaling is not dyadic.
*/
void get_base_P_non_dyadic(short mv_cache [][2], short *ref_cache, unsigned char *DpbLuma, unsigned char *DpbCb, unsigned char *DpbCr, 
						   unsigned char *image, unsigned char *image_Cb, unsigned char *image_Cr, short x, short y, 
						   short PicWidthInPix, short PicHeightInPix, LIST_MMO *RefPicListL0, short *ref_base, const NAL *Nal, 
						   const interpol_4x4 *interpol_4x4, REFPART *RefPartition)
{



	int Idx;
	//TODO with the scale factor
	for ( Idx = 0 ; Idx < 16 ; Idx++ ) {
		int ref_index = RefPartition [Idx] . RefIndex;
		int index = 12 + (Idx & 0x03) + ((Idx >> 2) << 3);	
		ref_cache [index] = ref_base[ref_index]; 
	}

	if ( Nal -> PicToDisplay){
		//Recovery of  the prediction
		sample_interpolation(DpbLuma, DpbCb,DpbCr, image, image_Cb, image_Cr, mv_cache, ref_cache, x , y, PicWidthInPix, PicHeightInPix, RefPicListL0, interpol_4x4);
	}
}





/**
Predict from base layer motion vectors and reference indexes for sample interpolation.
*/
void get_base_B_mv(short mv_cache_l0 [][2], short mv_cache_l1 [][2], short ref_cache_l0 [], short ref_cache_l1 [], 
				   unsigned char *DpbLuma, unsigned char *DpbCb, unsigned char *DpbCr, 
				   unsigned char *image, unsigned char *image_Cb, unsigned char *image_Cr, 
				   const short x, const short y, const short PicWidthInPix, const short PicHeightInPix, 
				   LIST_MMO *RefPicListL0, LIST_MMO *RefPicListL1, REFPART *RefPartitionL0, short *mv_l0, short mv_l1[], 
				   short *ref_l0, short *ref_l1, const NAL *Nal, const interpol_4x4 *interpol_4x4)
{



	unsigned char image_l0[256];
	unsigned char image_l1[256];
	unsigned char image_Cb_l0[64];
	unsigned char image_Cb_l1[64];
	unsigned char image_Cr_l0[64];
	unsigned char image_Cr_l1[64];
	int Idx, Sub_Idx;

	REFPART *RefPartitionL1 = &RefPartitionL0[16];

	for ( Idx = 0 ; Idx < 4 ; Idx++ ) {
		for (Sub_Idx = 0; Sub_Idx < 4; Sub_Idx++){
			int index = 12 + 8 * Idx + Sub_Idx;	
			int index_mv_l0 =  RefPartitionL0 [Sub_Idx + (Idx << 2)] . MvIndex;
			int index_mv_l1 =  RefPartitionL1 [Sub_Idx + (Idx << 2)] . MvIndex;
			int ref_index_l1 = RefPartitionL1 [Sub_Idx + (Idx << 2)] . RefIndex;
			int ref_index_l0 = RefPartitionL0 [Sub_Idx + (Idx << 2)] . RefIndex;
			ref_cache_l0 [index] = ref_l0[ref_index_l0];
			ref_cache_l1 [index] = ref_l1[ref_index_l1];
			if (ref_cache_l0 [index] >= 0){
				mv_cache_l0 [index][0] = MV_CAL(mv_l0[index_mv_l0]	  , Nal -> MvCalx [Nal -> LayerId]); 
				mv_cache_l0 [index][1] = MV_CAL(mv_l0[index_mv_l0 + 1], Nal -> MvCaly [Nal -> LayerId]); 
			}
			//mbl check
			if (ref_cache_l1 [index] >= 0){
				mv_cache_l1 [index][0] = MV_CAL(mv_l1[index_mv_l1]	  , Nal -> MvCalx [Nal -> LayerId]); 
				mv_cache_l1 [index][1] = MV_CAL(mv_l1[index_mv_l1 + 1], Nal -> MvCaly [Nal -> LayerId]); 
			}
		}
	}


	if ( Nal -> PicToDisplay){
		sample_interpolation_main(DpbLuma, DpbCb, DpbCr, image_l0, image_l1, image_Cb_l0, image_Cb_l1, 
			image_Cr_l0, image_Cr_l1, mv_cache_l0, mv_cache_l1, ref_cache_l0, ref_cache_l1, 
			x, y, PicWidthInPix, PicHeightInPix , RefPicListL0, RefPicListL1, interpol_4x4);

		set_image_4x4(image, image_Cb, image_Cr, image_l0, image_l1, image_Cb_l0, image_Cb_l1,
			image_Cr_l0, image_Cr_l1, ref_cache_l0, ref_cache_l1, PicWidthInPix);
	}
}



		
/**
Predict from base layer motion vectors and reference indexes for sample interpolation
when up-scaling is not dyadic.
*/
void GetBaseBMvNonDiadic(short mv_cache_l0 [][2], short mv_cache_l1 [][2], short ref_cache_l0 [], short ref_cache_l1 [], 
						 unsigned char *DpbLuma, unsigned char *DpbCb, unsigned char *DpbCr, 
						 unsigned char *image, unsigned char *image_Cb, unsigned char *image_Cr, 
						 const short x, const short y, const short PicWidthInPix, const short PicHeightInPix, 
						 LIST_MMO *RefPicListL0, LIST_MMO *RefPicListL1, REFPART *RefPartitionL0, 
						 short *ref_l0, short *ref_l1, const NAL *Nal, const interpol_4x4 *interpol_4x4)
{



	unsigned char image_l0[256];
	unsigned char image_l1[256];
	unsigned char image_Cb_l0[64];
	unsigned char image_Cb_l1[64];
	unsigned char image_Cr_l0[64];
	unsigned char image_Cr_l1[64];
	int Idx, Sub_Idx;

	REFPART *RefPartitionL1 = &RefPartitionL0[16];

	for ( Idx = 0 ; Idx < 4 ; Idx++ ) {
		for (Sub_Idx = 0; Sub_Idx < 4; Sub_Idx++){
			int index = 12 + 8 * Idx + Sub_Idx;	
			int ref_index_l1 = RefPartitionL1 [Sub_Idx + (Idx << 2)] . RefIndex;
			int ref_index_l0 = RefPartitionL0 [Sub_Idx + (Idx << 2)] . RefIndex;
			ref_cache_l0 [index] = ref_l0[ref_index_l0];
			ref_cache_l1 [index] = ref_l1[ref_index_l1];
		}
	}


	if ( Nal -> PicToDisplay){
		sample_interpolation_main(DpbLuma, DpbCb, DpbCr, image_l0, image_l1, image_Cb_l0, image_Cb_l1, 
			image_Cr_l0, image_Cr_l1, mv_cache_l0, mv_cache_l1, ref_cache_l0, ref_cache_l1, 
			x, y, PicWidthInPix, PicHeightInPix , RefPicListL0, RefPicListL1, interpol_4x4);

		set_image_4x4(image	, image_Cb, image_Cr, image_l0, image_l1, image_Cb_l0, image_Cb_l1, image_Cr_l0, image_Cr_l1, ref_cache_l0, ref_cache_l1, PicWidthInPix);
	}
}




/**
Compute Motion vector for a skip macroblock.
*/
short SvcSkipMotionVector(short mv_cache[][2], const short MvdLX[2], const short *ref_cache, short mvLX[2], int index8, int partwidth)
{

	short RefC, RefX;
	short RefA = ref_cache[index8 - 1] > LIST_NOT_USED ? ref_cache[index8 - 1]: 255;
	short RefB = ref_cache[index8 - 8] > LIST_NOT_USED ? ref_cache[index8 - 8]: 255;

	if ( ref_cache[index8 - 8 + partwidth] == PART_NOT_AVAILABLE){
		RefC = ref_cache[index8 - 8 - 1] > LIST_NOT_USED ? ref_cache[index8 - 8 - 1]: 255;
	}else{
		RefC = ref_cache[index8 - 8 + partwidth] > LIST_NOT_USED ? ref_cache[index8 - 8 + partwidth]: 255;
	}

	RefX = MIN(RefA, MIN(RefB, RefC));

	if (RefX != 255){
		motion_vector(mv_cache, MvdLX, ref_cache, mvLX, RefX, index8, partwidth);
		return RefX;
	} else {
		mvLX [0] = mvLX [1] = 0;
		return -1;
	}
}
