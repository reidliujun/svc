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

#include "data.h"
#include "Direct_prediction.h"

/*
Compute the motion vector and reference index for a B skip using spatial prediction.
*/
void SpatialPrediction(short mv_cache_l0[][2], short *ref_cache_l0, short *mvl0, short *ref)
{

	short refa = ref_cache_l0[12 - 1];
	short refb = ref_cache_l0[12 - 8];
	short refc = ref_cache_l0[12 - 8 + 4];
	mvl0[0] = mv_cache_l0[12 - 8 + 4][0];
	mvl0[1] = mv_cache_l0[12 - 8 + 4][1];

	if(refc == -2){
		refc = ref_cache_l0[12 - 8 - 1];
		mvl0[0] = mv_cache_l0[12 - 8 - 1][0];
		mvl0[1] = mv_cache_l0[12 - 8 - 1][1];
	}

	ref[0] = refa;
	if(ref[0] < 0 || (refb < ref[0] && refb >= 0))
		ref[0] = refb;

	if(ref[0] < 0 || (refc < ref[0] && refc >= 0))
		ref[0] = refc;

	if(ref[0] < 0)
		ref[0] = -1;

	if(ref[0] >= 0){        
		int match_count = (refa == ref[0]) + (refb == ref[0]) + (refc == ref[0]);
		motion_pred(12, mv_cache_l0, mvl0, match_count, ref[0], refa, refb, refc);
	}else
		mvl0[0] = mvl0[1] = 0;  

}
/**
This funtion allows to calculate the motion vector in case of a B_Skip.
This is a spatial prediction method.



@param mv_cache_l0 Table used to stock the motion vector of the current macroblock.
@param mv_cache_l1 Table used to stock the motion vector of the current macroblock.
@param mv_l0 A table where the motion vector are stocked for each 4x4 block of each macroblock for the prediction l0.
@param mv_l1 A table where the motion vector are stocked for each 4x4 block of each macroblock for the prediction l1.
@param ref_cache_l0 Table used to stock the reference index of the current macroblock.
@param ref_cache_l1 Table used to stock the reference index of the current macroblock.
@param ref_l1_l0 A table of reference of each 4x4 block of each macroblock of the first frame in l1.
@param ref_l1_l1 A table of reference of each 4x4 block of each macroblock of the first frame in l1.
@param mbPartIdx Number of the current Sub_8x8 macroblock.
@param b_stride Stride in the motion vector table.
@param b8_stride Stride in the reference index table.
@param direct_8x8_inference_flag Allows to determine if the prediction 8x8 is allowed or not.
@param long_term Allow to determine if the first frame stored in the reference list l1 is a long term one or not.
@param is_svc Used to determin if the funciton is used in the SVC decoder.
@param slice_type Type of the first frame stored in the reference list l1.
*/
void spatial_direct_prediction(short mv_cache_l0[][2], short mv_cache_l1[][2], short *mv_l0, short mv_l1[], short *ref_cache_l0, 
							   short *ref_cache_l1, short *ref_l1_l0, short *ref_l1_l1, int mbPartIdx, int b_stride, 
							   int b8_stride, int direct_8x8_inference_flag, int long_term, int is_svc, int slice_type)
{


	
	int i4;
	int x8 = mbPartIdx & 1;
	int y8 = mbPartIdx >> 1;
	int index = SCAN8(mbPartIdx << 2);
	short ref[2];
	short mvl0[2];
	short mvl1[2];


	SpatialPrediction(mv_cache_l0, ref_cache_l0, mvl0, ref);
	SpatialPrediction(mv_cache_l1, ref_cache_l1, mvl1, &ref[1]);

	if(ref[0] < 0 && ref[1] < 0)
		ref[0] = ref[1] = 0;

	mv_cache_l0[index][0] = mv_cache_l0[index + 1][0] = mv_cache_l0[index + 8][0] = mv_cache_l0[index + 9][0] = mvl0[0];
	mv_cache_l0[index][1] = mv_cache_l0[index + 1][1] =	mv_cache_l0[index + 8][1] = mv_cache_l0[index + 9][1] = mvl0[1];
	mv_cache_l1[index][0] = mv_cache_l1[index + 1][0] =	mv_cache_l1[index + 8][0] = mv_cache_l1[index + 9][0] = mvl1[0];
	mv_cache_l1[index][1] = mv_cache_l1[index + 1][1] =	mv_cache_l1[index + 8][1] = mv_cache_l1[index + 9][1] = mvl1[1];

	ref_cache_l0[index] =  ref_cache_l0[index + 1] = ref_cache_l0[index + 8] =  ref_cache_l0[index + 9] = ref[0];
	ref_cache_l1[index] =  ref_cache_l1[index + 1] = ref_cache_l1[index + 8] =  ref_cache_l1[index + 9] = ref[1];            


	if ( !long_term){
		int xx8;
		int yy8;
		int index_ref;
		int index_mv;
		for(i4 = 0; i4 < 4; i4++){
			if (direct_8x8_inference_flag){
				xx8 = 3 * x8;
				yy8 = 3 * y8;
			}else{
				xx8 = (x8 << 1) + (i4&1); 
				yy8 = (y8 << 1) + ((i4&2)>>1);
			}

			if (!is_svc){
				index_ref = x8 + y8 * b8_stride;
			}else{
				index_ref = 2*x8 + 2*y8 * b8_stride;
			}

			index_mv = (xx8 << 1) + yy8 * b_stride;
			index = SCAN8((mbPartIdx << 2) + i4);

			if((slice_type != SLICE_TYPE_I && ref_l1_l0[index_ref] == 0 && abs(mv_l0[index_mv]) <= 1 && abs(mv_l0[(index_mv) + 1]) <= 1)
				|| (ref_l1_l0[index_ref] < 0 && (slice_type == SLICE_TYPE_B) && ref_l1_l1[index_ref] == 0 && abs(mv_l1[index_mv]) <= 1 && abs(mv_l1[(index_mv) + 1]) <= 1)){
					if(ref[0] <= 0){
						mv_cache_l0[index][0] = mv_cache_l0[index][1] = 0;
					}
					if(ref[1] <= 0){
						mv_cache_l1[index][0] = mv_cache_l1[index][1] = 0;
					}
			}
		}
	}
}







/**
This funtion allows to calculate the motion vector in case of a B_Skip.
This is a temporal prediction method.



@param mv_cache_l0 Table used to stock the motion vector of the current macroblock.
@param mv_cache_l1 Table used to stock the motion vector of the current macroblock.
@param mv_l0 A table where the motion vector are stocked for each 4x4 block of each macroblock for the prediction l0.
@param ref_cache_l0 Table used to stock the reference index of the current macroblock.
@param ref_cache_l1 Table used to stock the reference index of the current macroblock.
@param ref_l1_l0 A table of reference of each 4x4 block of each macroblock of the first frame in l1.
@param ref_l1_l1 A table of reference of each 4x4 block of each macroblock of the first frame in l1.
@param map_col_to_list0 Table which contains the frame pocs of the reference list of the first frame in l0.
@param map_col_to_list1 Table which contains the frame pocs of the reference list of the first frame in l1.
@param dist_scale_factor Factor table used to scale the motion vectors when necessary.
@param b_stride Stride in the motion vector table.
@param b8_stride Stride in the reference index table.
@param MbParIdx Number of the current Sub_8x8 macroblock.
@param direct_8x8_inference_flag Allows to determine if the prediction 8x8 is allowed or not.
@param is_svc Used to determin if the funciton is used in the SVC decoder.
@param slice_type Type of the first frame stored in the reference list l1.
*/
void temporal_direct_prediction(short mv_cache_l0[][2], short mv_cache_l1[][2], short *mv_l0, short *ref_cache_l0, short *ref_cache_l1, 
								short *ref_l1_l0, short *ref_l1_l1, const SLICE *Slice, int MbParIdx, 
								int direct_8x8_inference_flag, int is_svc, const int slice_type)
{

	int i4;

	short index, ref0;
	const int b_stride = Slice -> b_stride;
	const int b8_stride = Slice -> b8_stride;
	const int x8 = MbParIdx & 1;
    const int y8 = MbParIdx >> 1;
	short mv_col[2], mvl0[2];

	
	if ( slice_type != SLICE_TYPE_I){
		if(!is_svc){
			ref0 = ref_l1_l0[x8 + y8 * b8_stride];
		}else{
			ref0 = ref_l1_l0[2*x8 + 2*y8 * b8_stride];
		}
	}else{
		ref0 = -1;
	}

	if(ref0 >= 0){
		ref0 = Slice -> MapColList [ref0];
		for(i4 = 0; i4 < 4; i4++){
			int xx8 = (x8 << 1) + (i4&1); 
			int yy8 = (y8 << 1) + ((i4&2)>>1);
			index = SCAN8((MbParIdx << 2) + i4);
			if (direct_8x8_inference_flag){
				xx8 = 3*x8;
				yy8 = 3*y8;
			}

			mv_col[0] = mv_l0[2 * xx8 + yy8 * b_stride];
			mv_col[1] = mv_l0[2 * xx8 + yy8 * b_stride + 1];
			mv_cache_l0[index][0] = mvl0[0] = (Slice -> dist_scale_factor[ref0] * mv_col[0] + 128) >> 8;
			mv_cache_l0[index][1] = mvl0[1] =  (Slice -> dist_scale_factor[ref0] * mv_col[1] + 128) >> 8;
			mv_cache_l1[index][0] = (mvl0[0] - mv_col[0]);
			mv_cache_l1[index][1] = (mvl0[1] - mv_col[1]);
			ref_cache_l0[index] = ref0;
			ref_cache_l1[index] = 0;

		}
	}else{
		ref0 = 0;
		index = SCAN8((MbParIdx << 2));
		// In case of an intra macroblock
		if (slice_type == SLICE_TYPE_B &&  ref_l1_l1[x8 + y8 * b8_stride] != -1){
			ref0 = Slice -> MapColList[16 + ref_l1_l1[x8 + y8 * b8_stride]];
		}
		mv_cache_l0[index][0] = mv_cache_l0[index][1] = 
		mv_cache_l0[index + 1][0] = mv_cache_l0[index + 1][1] =
		mv_cache_l0[index + 8][0] = mv_cache_l0[index + 8][1] =
		mv_cache_l1[index + 9][0] = mv_cache_l1[index + 9][1] = 0;

		ref_cache_l0[index] = ref_cache_l0[index + 1] = ref_cache_l0[index + 8] = ref_cache_l0[index + 9] = ref0;
		ref_cache_l1[index] = ref_cache_l1[index + 1] = ref_cache_l1[index + 8] = ref_cache_l1[index + 9] = 0;

	}
}





/**
This funtion allows to calculate the motion vector in case of a direct prediction.
This is a temporal prediction method.



@param mv_cache_l0 Table used to stock the motion vector of the current macroblock.
@param mv_l0 A table where the motion vector are stocked for each 4x4 block of each macroblock for the prediction l0.
@param ref_cache_l0 Table used to stock the reference index of the current macroblock.
@param ref_l1_l0 A table of reference of each 4x4 block of each macroblock of the first frame in l1.
@param ref_l1_l1 A table of reference of each 4x4 block of each macroblock of the first frame in l1.
@param map_col_to_list0 Table which contains the frame pocs of the reference list of the first frame in l0.
@param map_col_to_list1 Table which contains the frame pocs of the reference list of the first frame in l1.
@param dist_scale_factor Factor table used to scale the motion vectors when necessary.
@param b_stride Stride in the motion vector table.
@param b8_stride Stride in the reference index table.
@param MbParIdx Number of the current Sub_8x8 macroblock.
@param direct_8x8_inference_flag Allows to determine if the prediction 8x8 is allowed or not.
@param is_svc Used to determin if the funciton is used in the SVC decoder.
@param slice_type Type of the first frame stored in the reference list l1.
*/
void temporal_direct_prediction_l0(short mv_cache_l0[][2], short *mv_l0, short *ref_cache_l0, 
								   short *ref_l1_l0, short *ref_l1_l1, const SLICE *Slice, 
								   int MbParIdx, int direct_8x8_inference_flag, 
								   int is_svc, const int slice_type)
{

	int i4;

	const int b_stride = Slice -> b_stride;
	const int b8_stride = Slice -> b8_stride;
	const int x8 = MbParIdx & 1;
    const int y8 = MbParIdx >> 1;
	short mv_col[2];
	short ref0;
	const short *dist_scale_factor = Slice -> dist_scale_factor;

	
	if ( slice_type != SLICE_TYPE_I){
		if(!is_svc){
			ref0 = ref_l1_l0[x8 + y8 * b8_stride];
		}else{
			ref0 = ref_l1_l0[2*x8 + 2*y8 * b8_stride];
		}
	}else{
		ref0 = -1;
	}

	if(ref0 >= 0){
		ref0 = Slice -> MapColList [ref0];
		if (direct_8x8_inference_flag){
			int xx8 = 3*x8;
			int yy8 = 3*y8;
			int index = SCAN8((MbParIdx << 2));
			mv_col[0] = mv_l0[2 * xx8 + yy8 * b_stride];
			mv_col[1] = mv_l0[2 * xx8 + yy8 * b_stride + 1];
			mv_cache_l0[index][0] = mv_cache_l0[index + 1][0] = mv_cache_l0[index + 8][0] = mv_cache_l0[index + 9][0] = (dist_scale_factor[ref0] * mv_col[0] + 128) >> 8;
			mv_cache_l0[index][1] = mv_cache_l0[index + 1][1] = mv_cache_l0[index + 8][1] = mv_cache_l0[index + 9][1] = (dist_scale_factor[ref0] * mv_col[1] + 128) >> 8;
			ref_cache_l0[index] = ref_cache_l0[index + 1] = ref_cache_l0[index + 8] = ref_cache_l0[index + 9] = ref0;

		}else{
		
			for(i4 = 0; i4 < 4; i4++){
				int xx8 = (x8 << 1) + (i4&1); 
				int yy8 = (y8 << 1) + ((i4&2)>>1);
				int index = SCAN8((MbParIdx << 2) + i4);
				

				mv_col[0] = mv_l0[2 * xx8 + yy8 * b_stride];
				mv_col[1] = mv_l0[2 * xx8 + yy8 * b_stride + 1];
				mv_cache_l0[index][0] = (dist_scale_factor[ref0] * mv_col[0] + 128) >> 8;
				mv_cache_l0[index][1] = (dist_scale_factor[ref0] * mv_col[1] + 128) >> 8;
				ref_cache_l0[index] = ref0;
			}
		}
	}else{
		int index = SCAN8((MbParIdx << 2));
		ref0 = 0;

		// In case of an intra macroblock
		if (slice_type == SLICE_TYPE_B &&  ref_l1_l1[x8 + y8 * b8_stride] != -1){
			ref0 = Slice -> MapColList[16 + ref_l1_l1[x8 + y8 * b8_stride]];
		}
		mv_cache_l0[index][0] = mv_cache_l0[index][1] = 
		mv_cache_l0[index + 1][0] = mv_cache_l0[index + 1][1] =
		mv_cache_l0[index + 8][0] = mv_cache_l0[index + 8][1] = 0;

		ref_cache_l0[index] = ref_cache_l0[index + 1] = ref_cache_l0[index + 8] = ref_cache_l0[index + 9] = ref0;
	}
}




/**
This funtion allows to calculate the motion vector in case of a direct prediction.
This is a temporal prediction method.


@param mv_cache_l1 Table used to stock the motion vector of the current macroblock.
@param mv_l0 A table where the motion vector are stocked for each 4x4 block of each macroblock for the prediction l0.
@param ref_cache_l0 Table used to stock the reference index of the current macroblock.
@param ref_cache_l1 Table used to stock the reference index of the current macroblock.
@param ref_l1_l0 A table of reference of each 4x4 block of each macroblock of the first frame in l1.
@param ref_l1_l1 A table of reference of each 4x4 block of each macroblock of the first frame in l1.
@param map_col_to_list0 Table which contains the frame pocs of the reference list of the first frame in l0.
@param map_col_to_list1 Table which contains the frame pocs of the reference list of the first frame in l1.
@param dist_scale_factor Factor table used to scale the motion vectors when necessary.
@param b_stride Stride in the motion vector table.
@param b8_stride Stride in the reference index table.
@param MbParIdx Number of the current Sub_8x8 macroblock.
@param direct_8x8_inference_flag Allows to determine if the prediction 8x8 is allowed or not.
@param is_svc Used to determin if the funciton is used in the SVC decoder.
@param slice_type Type of the first frame stored in the reference list l1.
*/
void temporal_direct_prediction_l1(short mv_cache_l1[][2], short *mv_l0, short *ref_cache_l1, 
								   short *ref_l1_l0, short *ref_l1_l1, const SLICE *Slice, 
								   int MbParIdx, int direct_8x8_inference_flag, 
								   int is_svc, const int slice_type)
{

	int i4;
	const int b_stride = Slice -> b_stride;
	const int b8_stride = Slice -> b8_stride;
	const int x8 = MbParIdx & 1;
    const int y8 = MbParIdx >> 1;
	short mv_col[2], mvl0[2];
	short index, ref0;

	const short *dist_scale_factor = Slice -> dist_scale_factor;

	
	if ( slice_type != SLICE_TYPE_I){
		if(!is_svc){
			ref0 = ref_l1_l0[x8 + y8 * b8_stride];
		}else{
			ref0 = ref_l1_l0[2*x8 + 2*y8 * b8_stride];
		}
	}else{
		ref0 = -1;
	}

	if(ref0 >= 0){
		ref0 = Slice -> MapColList [ref0];
		if (direct_8x8_inference_flag){
			int xx8 = 3*x8;
			int yy8 = 3*y8;
			index = SCAN8((MbParIdx << 2));
			mv_col[0] = mv_l0[2 * xx8 + yy8 * b_stride];
			mv_col[1] = mv_l0[2 * xx8 + yy8 * b_stride + 1];
			mvl0[0] = (dist_scale_factor[ref0] * mv_col[0] + 128) >> 8;
			mvl0[1] = (dist_scale_factor[ref0] * mv_col[1] + 128) >> 8;
			mv_cache_l1[index][0] = mv_cache_l1[index + 1][0] = mv_cache_l1[index + 8][0] = mv_cache_l1[index + 9][0] = mvl0[0] - mv_col[0];
			mv_cache_l1[index][1] = mv_cache_l1[index + 1][1] = mv_cache_l1[index + 8][1] = mv_cache_l1[index + 9][1] =  mvl0[1] - mv_col[1];
			ref_cache_l1[index] = ref_cache_l1[index + 1] =  ref_cache_l1[index + 8] =  ref_cache_l1[index + 9] = 0;

		}else {
			for(i4 = 0; i4 < 4; i4++){
				int xx8 = (x8 << 1) + (i4&1); 
				int yy8 = (y8 << 1) + ((i4&2)>>1);
				index = SCAN8((MbParIdx << 2) + i4);
				

				mv_col[0] = mv_l0[2 * xx8 + yy8 * b_stride];
				mv_col[1] = mv_l0[2 * xx8 + yy8 * b_stride + 1];
				mvl0[0] = (dist_scale_factor[ref0] * mv_col[0] + 128) >> 8;
				mvl0[1] = (dist_scale_factor[ref0] * mv_col[1] + 128) >> 8;
				mv_cache_l1[index][0] = mvl0[0] - mv_col[0];
				mv_cache_l1[index][1] = mvl0[1] - mv_col[1];
				ref_cache_l1[index] = 0;
			}
		}
	}else{
		ref0 = 0;
		index = SCAN8((MbParIdx << 2));
		// In case of an intra macroblock
		if (slice_type == SLICE_TYPE_B &&  ref_l1_l1[x8 + y8 * b8_stride] != -1){
			ref0 = Slice -> MapColList [16 + ref_l1_l1[x8 + y8 * b8_stride]];
		}

		mv_cache_l1[index + 9][0] = mv_cache_l1[index + 9][1] = 0;
		ref_cache_l1[index] = ref_cache_l1[index + 1] = ref_cache_l1[index + 8] = ref_cache_l1[index + 9] = 0;

	}
}





/**
This funtion allows to calculate the motion vector in case of a direct prediction.
This is a spatial prediction method.



@param mv_cache_l0 Table used to stock the motion vector of the current macroblock.
@param mv_l0 A table where the motion vector are stocked for each 4x4 block of each macroblock for the prediction l0.
@param mv_l1 A table where the motion vector are stocked for each 4x4 block of each macroblock for the prediction l1.
@param ref_cache_l0 Table used to stock the reference index of the current macroblock.
@param ref_cache_l1 Table used to stock the reference index of the current macroblock.
@param ref_l1_l0 A table of reference of each 4x4 block of each macroblock of the first frame in l1.
@param ref_l1_l1 A table of reference of each 4x4 block of each macroblock of the first frame in l1.
@param mbPartIdx Number of the current Sub_8x8 macroblock.
@param b_stride Stride in the motion vector table.
@param b8_stride Stride in the reference index table.
@param direct_8x8_inference_flag Allows to determine if the prediction 8x8 is allowed or not.
@param long_term Allow to determine if the first frame stored in the reference list l1 is a long term one or not.
@param is_svc Used to determin if the funciton is used in the SVC decoder.
@param slice_type Type of the first frame stored in the reference list l1.
*/
void spatial_direct_prediction_l0(short mv_cache_l0[][2], short *mv_l0, short mv_l1[], short *ref_cache_l0, short *ref_cache_l1, 
								  short *ref_l1_l0, short *ref_l1_l1, int mbPartIdx, int b_stride, int b8_stride, 
								  int direct_8x8_inference_flag, int long_term, int is_svc, int slice_type)
{


	short ref[2];
	short mvl0[2];
	int i4;
	int x8 = mbPartIdx & 1;
	int y8 = mbPartIdx >> 1;
	int index = SCAN8(mbPartIdx << 2);

	short refa = ref_cache_l1[12 - 1];
	short refb = ref_cache_l1[12 - 8];
	short refc = ref_cache_l1[12 - 8 + 4];
	
	SpatialPrediction(mv_cache_l0, ref_cache_l0, mvl0, ref);



	if(refc == -2){
		refc = ref_cache_l1[12 - 8 - 1];
	}

	ref[1] = refa;
	if(ref[1] < 0 || (refb < ref[1] && refb >= 0)){
		ref[1] = refb;
	}

	if(ref[1] < 0 || (refc < ref[1] && refc >= 0)){
		ref[1] = refc;
	}

	if(ref[1] < 0){
		ref[1] = -1;
	}

	if(ref[0] < 0 && ref[1] < 0){
		ref[0] = ref[1] = 0;
	}

	mv_cache_l0[index][0] = mv_cache_l0[index + 1][0] = mv_cache_l0[index + 8][0] = mv_cache_l0[index + 9][0] =  mvl0[0];
	mv_cache_l0[index][1] = mv_cache_l0[index + 1][1] =	mv_cache_l0[index + 8][1] = mv_cache_l0[index + 9][1] =  mvl0[1];
	
	ref_cache_l0[index] =  ref_cache_l0[index + 1] = ref_cache_l0[index + 8] =  ref_cache_l0[index + 9] = ref[0];
          


	if ( !long_term){
		int xx8;
		int yy8;
		int index_ref;
		int index_mv;
		for(i4 = 0; i4 < 4; i4++){
			if (direct_8x8_inference_flag){
				xx8 = 3 * x8;
				yy8 = 3 * y8;
			}else{
				xx8 = (x8 << 1) + (i4&1); 
				yy8 = (y8 << 1) + ((i4&2)>>1);
			}

			if (!is_svc){
				index_ref = x8 + y8 * b8_stride;
			}else{
				index_ref = 2*x8 + 2*y8 * b8_stride;
			}

			index_mv = (xx8 << 1) + yy8 * b_stride;
			index = SCAN8((mbPartIdx << 2) + i4);



			if((slice_type != SLICE_TYPE_I && ref_l1_l0[index_ref] == 0 && abs(mv_l0[index_mv]) <= 1 	&& abs(mv_l0[(index_mv) + 1]) <= 1)
				|| (ref_l1_l0[index_ref] < 0 && (slice_type == SLICE_TYPE_B) && ref_l1_l1[index_ref] == 0	&& abs(mv_l1[index_mv]) <= 1 && abs(mv_l1[(index_mv) + 1]) <= 1)){
					if(ref[0] <= 0){
						mv_cache_l0[index][0] = mv_cache_l0[index][1] = 0;
					}
			}
		}
	}
}






/**
This funtion allows to calculate the motion vector in case of a direct prediction.
This is a spatial prediction method.



@param mv_cache_l0 Table used to stock the motion vector of the current macroblock.
@param mv_l0 A table where the motion vector are stocked for each 4x4 block of each macroblock for the prediction l0.
@param mv_l1 A table where the motion vector are stocked for each 4x4 block of each macroblock for the prediction l1.
@param ref_cache_l0 Table used to stock the reference index of the current macroblock.
@param ref_cache_l1 Table used to stock the reference index of the current macroblock.
@param ref_l1_l0 A table of reference of each 4x4 block of each macroblock of the first frame in l1.
@param ref_l1_l1 A table of reference of each 4x4 block of each macroblock of the first frame in l1.
@param mbPartIdx Number of the current Sub_8x8 macroblock.
@param b_stride Stride in the motion vector table.
@param b8_stride Stride in the reference index table.
@param direct_8x8_inference_flag Allows to determine if the prediction 8x8 is allowed or not.
@param long_term Allow to determine if the first frame stored in the reference list l1 is a long term one or not.
@param is_svc Used to determin if the funciton is used in the SVC decoder.
@param slice_type Type of the first frame stored in the reference list l1.
*/
void spatial_direct_prediction_l1(short mv_cache_l1[][2], short *mv_l0, short mv_l1[], short *ref_cache_l0, short *ref_cache_l1, 
								  short *ref_l1_l0, short *ref_l1_l1, int mbPartIdx, int b_stride, int b8_stride, 
								  int direct_8x8_inference_flag, int long_term, int is_svc, int slice_type)
{


	short ref[2];
	short mvl1[2];
	int i4;
	int x8 = mbPartIdx & 1;
	int y8 = mbPartIdx >> 1;
	int index = SCAN8(mbPartIdx << 2);


	short refa = ref_cache_l0[12 - 1];
	short refb = ref_cache_l0[12 - 8];
	short refc = ref_cache_l0[12 - 8 + 4];

	if(refc == -2){
		refc = ref_cache_l0[12 - 8 - 1];
	}
	ref[0] = refa;
	if(ref[0] < 0 || (refb < ref[0] && refb >= 0))
		ref[0] = refb;

	if(ref[0] < 0 || (refc < ref[0] && refc >= 0))
		ref[0] = refc;

	if(ref[0] < 0)
		ref[0] = -1;

	
	SpatialPrediction(mv_cache_l1, ref_cache_l1, mvl1, &ref[1]);

	if(ref[0] < 0 && ref[1] < 0)
		ref[0] = ref[1] = 0;
	
	mv_cache_l1[index][0] = mv_cache_l1[index + 1][0] =	mv_cache_l1[index + 8][0] = mv_cache_l1[index + 9][0] =  mvl1[0];
	mv_cache_l1[index][1] = mv_cache_l1[index + 1][1] =	mv_cache_l1[index + 8][1] = mv_cache_l1[index + 9][1] =  mvl1[1];
	ref_cache_l1[index] =  ref_cache_l1[index + 1] = ref_cache_l1[index + 8] =  ref_cache_l1[index + 9] = ref[1];            


	if ( !long_term){
		int xx8;
		int yy8;
		int index_ref;
		int index_mv;
		for(i4 = 0; i4 < 4; i4++){
			if (direct_8x8_inference_flag){
				xx8 = 3 * x8;
				yy8 = 3 * y8;
			}else{
				xx8 = (x8 << 1) + (i4&1); 
				yy8 = (y8 << 1) + ((i4&2)>>1);
			}

			if (!is_svc){
				index_ref = x8 + y8 * b8_stride;
			}else{
				index_ref = 2*x8 + 2*y8 * b8_stride;
			}

			index_mv = (xx8 << 1) + yy8 * b_stride;
			index = SCAN8((mbPartIdx << 2) + i4);



			if((slice_type != SLICE_TYPE_I && ref_l1_l0[index_ref] == 0 && abs(mv_l0[index_mv]) <= 1 	&& abs(mv_l0[(index_mv) + 1]) <= 1)
				|| (ref_l1_l0[index_ref] < 0 && (slice_type == SLICE_TYPE_B) && ref_l1_l1[index_ref] == 0	&& abs(mv_l1[index_mv]) <= 1 && abs(mv_l1[(index_mv) + 1]) <= 1)){
					
					if(ref[1] <= 0){
						mv_cache_l1[index][0] = mv_cache_l1[index][1] = 0;
					}
			}
		}
	}
}
