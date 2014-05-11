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

//AVC
#include "type.h"
#include "data.h"
#include "init_data.h"
#include "fill_caches.h"

//SVC
#include "svc_data.h"
#include "fill_caches_svc.h"




/**
Fill tables to be used for the decoding process.
*/
void fill_caches_svc ( const SLICE *Slice, const RESIDU *residu, const RESIDU *BaseResidu, const int EntropyCodingModeFlog, 
					  unsigned char NonZeroCountCache [ RESTRICT ], const unsigned char *SliceTable, const DATA *CurrBlock,  
					  short aio_tiIntra4x4_pred_mode_cache[], const short ai_iMb_x, const short ai_iMb_y ,	
					  const int ai_iConstrained_intra_pred, const int level_prediction_flag)
{
	short top_type = 0 ;
	short left_type = 0;
	short top_xy = 0;
	short left_xy = 0;
	short pred;
	const short	iMb_xy = ai_iMb_x + ai_iMb_y * (short)(Slice -> mb_stride);
	const short	ai_iMb_type_name = residu -> MbType;
	const unsigned char val_init = EntropyCodingModeFlog && !(IS_I(ai_iMb_type_name)) ? 0 : 64 ;

	init_non_zero_count_cache(NonZeroCountCache, val_init);

	if ( ai_iMb_y > 0 ) {
		top_xy = iMb_xy - Slice -> mb_stride;
		top_type = (SliceTable [top_xy] == Slice -> slice_num) ? residu [- Slice -> mb_stride] . MbType : 0 ;
	}else {
		top_type = 0;
	}

	if ( iMb_xy > 0 && ai_iMb_x != 0 )  {
		left_xy = iMb_xy - 1 ;
		left_type = SliceTable [left_xy] == Slice -> slice_num ? residu [-1] . MbType : 0 ;
	}else {
		left_type = 0;
	}

	FillNonZeroCount(CurrBlock, Slice -> mb_stride, top_type, left_type, NonZeroCountCache);


	//Updating the aio_tiIntra4x4_pred_mode_cache table from the current macroblock.
	if(IS_NxN(top_type)){
		aio_tiIntra4x4_pred_mode_cache[4] = residu[- Slice -> mb_stride] . Intra16x16DCLevel[10];
		aio_tiIntra4x4_pred_mode_cache[5] = residu[- Slice -> mb_stride] . Intra16x16DCLevel[11];
		aio_tiIntra4x4_pred_mode_cache[6] = residu[- Slice -> mb_stride] . Intra16x16DCLevel[14];
		aio_tiIntra4x4_pred_mode_cache[7] = residu[- Slice -> mb_stride] . Intra16x16DCLevel[15];
	}
	else if (level_prediction_flag && top_type == I_BL && IS_NxN(BaseResidu [top_xy] . MbType)){
		aio_tiIntra4x4_pred_mode_cache[4] = BaseResidu[top_xy] . Intra16x16DCLevel[10];
		aio_tiIntra4x4_pred_mode_cache[5] = BaseResidu[top_xy] . Intra16x16DCLevel[11];
		aio_tiIntra4x4_pred_mode_cache[6] = BaseResidu[top_xy] . Intra16x16DCLevel[14];
		aio_tiIntra4x4_pred_mode_cache[7] = BaseResidu[top_xy] . Intra16x16DCLevel[15];
	}
	else
	{
		if( top_type && (!ai_iConstrained_intra_pred || (level_prediction_flag && IS_I_SVC(top_type)) || ((ai_iConstrained_intra_pred || !level_prediction_flag) && IS_I(top_type)))){
			pred = 2;
		}else{
			pred = -1;
		}

		aio_tiIntra4x4_pred_mode_cache[4] = aio_tiIntra4x4_pred_mode_cache[5] =
		aio_tiIntra4x4_pred_mode_cache[6] = aio_tiIntra4x4_pred_mode_cache[7] = pred;
	}

	if(IS_NxN(left_type))    {
		aio_tiIntra4x4_pred_mode_cache[3 + 8] = residu[-1] . Intra16x16DCLevel[5];
		aio_tiIntra4x4_pred_mode_cache[3 + 16] = residu[-1] . Intra16x16DCLevel[7];
		aio_tiIntra4x4_pred_mode_cache[3 + 24] = residu[-1] . Intra16x16DCLevel[13];
		aio_tiIntra4x4_pred_mode_cache[3 + 32] = residu[-1] . Intra16x16DCLevel[15];
	}
	else if(level_prediction_flag && left_type == I_BL && IS_NxN(BaseResidu [left_xy] . MbType)){
		aio_tiIntra4x4_pred_mode_cache[3 + 8] = BaseResidu[left_xy] . Intra16x16DCLevel[5];
		aio_tiIntra4x4_pred_mode_cache[3 + 16] = BaseResidu[left_xy] . Intra16x16DCLevel[7];
		aio_tiIntra4x4_pred_mode_cache[3 + 24] = BaseResidu[left_xy] . Intra16x16DCLevel[13];
		aio_tiIntra4x4_pred_mode_cache[3 + 32] = BaseResidu[left_xy] . Intra16x16DCLevel[15];
	}
	else   {
		if( left_type && (!ai_iConstrained_intra_pred || (level_prediction_flag && IS_I_SVC(left_type)) || ((ai_iConstrained_intra_pred || !level_prediction_flag) && IS_I(left_type)))){
			pred = 2;
		}else{
			pred = -1;
		}

		aio_tiIntra4x4_pred_mode_cache[3 + 8] = aio_tiIntra4x4_pred_mode_cache[3 + 24] =
		aio_tiIntra4x4_pred_mode_cache[3 + 16] = aio_tiIntra4x4_pred_mode_cache[3 + 32] = pred;
	}

	if (ai_iMb_x == 0) {
		aio_tiIntra4x4_pred_mode_cache [11] = aio_tiIntra4x4_pred_mode_cache [27] = 
		aio_tiIntra4x4_pred_mode_cache [19] = aio_tiIntra4x4_pred_mode_cache [35] = -1 ;
	}
}










/**
Fill tables to be used for the decoding process concerning inter prediction.
*/
void fill_caches_motion_vector_full_ref(  const SLICE *Slice, const int ai_iB_stride, const int ai_iB8_stride, 
										short aio_tiRef_cache_l0 [ RESTRICT ], short aio_tiMv_cache_l0[ RESTRICT ][2], 
										const unsigned char *SliceTable, const RESIDU CurrResidu [ ], short ai_tiMv[ RESTRICT ], 
										const short *ai_tiRef,  const int ai_iX, const int ai_iY)
{

	int    top_type = 0;
	int    topleft_type;
	int    topright_type;
	int    left_type;
	const int iCurrMbAddr = ai_iX + Slice -> mb_stride * ai_iY;
	const int top_xy = iCurrMbAddr - Slice -> mb_stride ;
	const int topleft_xy = top_xy - 1 ;
	const int topright_xy = (top_xy >= 0)? top_xy + 1:-1 ;
	const int left_xy = iCurrMbAddr - 1;

	//Updating their availability according to the slice.
	if ( top_xy >= 0 )     {
		top_type = SliceTable [top_xy] == Slice -> slice_num ? CurrResidu [top_xy] . MbType : 0 ;

		if ( topleft_xy >= 0 && ai_iX != 0 && SliceTable [topleft_xy] == Slice -> slice_num){
			topleft_type =  CurrResidu [topleft_xy] . MbType;
		}else{
			topleft_type = 0;
			aio_tiRef_cache_l0[3] = PART_NOT_AVAILABLE;
		}


		if ( topright_xy >= 0 && (ai_iX != Slice -> mb_stride - 1) && SliceTable [topright_xy] == Slice -> slice_num){
			topright_type = CurrResidu [topright_xy] . MbType;
		}else{
			topright_type = 0;
			aio_tiRef_cache_l0[8] = PART_NOT_AVAILABLE;
		}

	}else{
		top_type = 0;
		topleft_type = 0;
		topright_type = 0;

		//Update top right and top left reference
		aio_tiRef_cache_l0[3] = PART_NOT_AVAILABLE;
		aio_tiRef_cache_l0[8] = PART_NOT_AVAILABLE;
	}


	if ( left_xy >= 0 && ai_iX != 0 && SliceTable [left_xy] == Slice -> slice_num){
		left_type = CurrResidu [left_xy] . MbType ;
	}else{
		left_type = 0;
		//Update left reference
		FillRefCacheLeftPosition(aio_tiRef_cache_l0, PART_NOT_AVAILABLE, PART_NOT_AVAILABLE);
	}


	//In case of a skipped or a P macroblock
	if(IS_P(top_type))    {
		FillMvCacheTopPosition(aio_tiMv_cache_l0, &ai_tiMv[-ai_iB_stride]);
		aio_tiRef_cache_l0[4] = ai_tiRef [- ai_iB8_stride];
		aio_tiRef_cache_l0[5] = ai_tiRef [1 - ai_iB8_stride];
		aio_tiRef_cache_l0[6] = ai_tiRef [2 - ai_iB8_stride];
		aio_tiRef_cache_l0[7] = ai_tiRef [3 - ai_iB8_stride];
	}
	else if (!top_type){
		FillRefCacheTopPosition(aio_tiRef_cache_l0, PART_NOT_AVAILABLE, PART_NOT_AVAILABLE);
	}

	//In case of a skipped or a P macroblock
	if(IS_P(left_type))    {
		FillMvCacheLeftPosition(aio_tiMv_cache_l0, &ai_tiMv[-2], ai_iB_stride);
		aio_tiRef_cache_l0[11] = ai_tiRef[-1];
		aio_tiRef_cache_l0[19] = ai_tiRef[-1 + ai_iB8_stride];
		aio_tiRef_cache_l0[27] = ai_tiRef[-1 + 2 * ai_iB8_stride];
		aio_tiRef_cache_l0[35] = ai_tiRef[-1 + 3 * ai_iB8_stride];
	}


	//In case of a skipped or a P macroblock
	if(IS_P(topleft_type ))	{	
		aio_tiMv_cache_l0 [3][0] = ai_tiMv[-2 - ai_iB_stride];
		aio_tiMv_cache_l0 [3][1] = ai_tiMv[-1 - ai_iB_stride];
		aio_tiRef_cache_l0[3] = ai_tiRef[-1 - ai_iB8_stride];
	}

	//In case of a skipped or a P macroblock
	if(IS_P(topright_type ))  {

		aio_tiMv_cache_l0 [8][0] = ai_tiMv[8 - ai_iB_stride];
		aio_tiMv_cache_l0 [8][1] = ai_tiMv[9 - ai_iB_stride];
		aio_tiRef_cache_l0[8] =  ai_tiRef[4 - ai_iB8_stride];

	}

	aio_tiRef_cache_l0[14] = aio_tiRef_cache_l0[15 + 1] = 
	aio_tiRef_cache_l0[23 + 1] = aio_tiRef_cache_l0[30] = aio_tiRef_cache_l0[31 + 1] = PART_NOT_AVAILABLE;

}




/**
Fill tables to be used for the decoding process concerning inter prediction.
*/
void fill_caches_full_ref(  const SLICE *Slice, const int ai_iB_stride, const int ai_iB8_stride, short aio_tiRef_cache_l0 [ RESTRICT ], 
						  short aio_tiMv_cache_l0[ RESTRICT ][2], const unsigned char *SliceTable, const RESIDU CurrResidu [ ], 
						  short ai_tiMv[ RESTRICT ], const short *ai_tiRef, const int ai_iX, const int ai_iY)
{


	int    top_type = 0;
	int    topleft_type;
	int    topright_type;
	int    left_type;

	const int iCurrMbAddr = ai_iX + Slice -> mb_stride * ai_iY;
	const int top_xy = iCurrMbAddr - Slice -> mb_stride ;
	const int topleft_xy = top_xy - 1 ;
	const int topright_xy = (top_xy >= 0)? top_xy + 1:-1 ;
	const int left_xy = iCurrMbAddr - 1;

	//Updating their availability according to the slice.
	if (top_xy >= 0 ){
		top_type = SliceTable [top_xy] == Slice -> slice_num ? CurrResidu [top_xy] . MbType : 0 ;

		if ( topleft_xy >= 0 && ai_iX != 0 && SliceTable [topleft_xy] == Slice -> slice_num){
			topleft_type =  CurrResidu [topleft_xy] . MbType;
		}else{
			topleft_type = 0;
			aio_tiRef_cache_l0[3] = PART_NOT_AVAILABLE;
		}


		if ( topright_xy >= 0 && (ai_iX != Slice -> mb_stride - 1) && SliceTable [topright_xy] == Slice -> slice_num){
			topright_type = CurrResidu [topright_xy] . MbType;
		}else{
			topright_type = 0;
			aio_tiRef_cache_l0[8] = PART_NOT_AVAILABLE;
		}

	}else{
		top_type = 0;
		topleft_type = 0;
		topright_type = 0;

		//Update top right and top left reference
		aio_tiRef_cache_l0[3] = PART_NOT_AVAILABLE;
		aio_tiRef_cache_l0[8] = PART_NOT_AVAILABLE;
	}


	if ( left_xy >= 0 && ai_iX != 0 && SliceTable [left_xy] == Slice -> slice_num){
		left_type = CurrResidu [left_xy] . MbType ;
	}else{
		left_type = 0;

		//Update left reference
		FillRefCacheLeftPosition(aio_tiRef_cache_l0, PART_NOT_AVAILABLE, PART_NOT_AVAILABLE);
	}


	//In case of a skipped or a P macroblock
	if(top_type)   {
		FillMvCacheTopPosition(aio_tiMv_cache_l0, &ai_tiMv[-ai_iB_stride]);

		aio_tiRef_cache_l0[4] = ai_tiRef [- ai_iB8_stride];
		aio_tiRef_cache_l0[5] = ai_tiRef [1 - ai_iB8_stride];
		aio_tiRef_cache_l0[6] = ai_tiRef [2 - ai_iB8_stride];
		aio_tiRef_cache_l0[7] = ai_tiRef [3 - ai_iB8_stride];

	}
	else if (!top_type){
		FillRefCacheTopPosition(aio_tiRef_cache_l0, PART_NOT_AVAILABLE, PART_NOT_AVAILABLE);;
	}

	//In case of a skipped or a P macroblock
	if(left_type)    {
		FillMvCacheLeftPosition(aio_tiMv_cache_l0, &ai_tiMv[-2], ai_iB_stride);

		aio_tiRef_cache_l0[11] = ai_tiRef[-1];
		aio_tiRef_cache_l0[19] = ai_tiRef[-1 + ai_iB8_stride];
		aio_tiRef_cache_l0[27] = ai_tiRef[-1 + 2 * ai_iB8_stride];
		aio_tiRef_cache_l0[35] = ai_tiRef[-1 + 3 * ai_iB8_stride];
	}


	//In case of a skipped or a P macroblock
	if(topleft_type)	{	
		aio_tiMv_cache_l0 [3][0] = ai_tiMv[-2 - ai_iB_stride];
		aio_tiMv_cache_l0 [3][1] = ai_tiMv[-1 - ai_iB_stride];
		aio_tiRef_cache_l0[3] = ai_tiRef[-1 - ai_iB8_stride];
	}

	//In case of a skipped or a P macroblock
	if(topright_type)   {
		aio_tiMv_cache_l0 [8][0] = ai_tiMv[8 - ai_iB_stride];
		aio_tiMv_cache_l0 [8][1] = ai_tiMv[9 - ai_iB_stride];
		aio_tiRef_cache_l0[8] =  ai_tiRef[4 - ai_iB8_stride];
	}

	aio_tiRef_cache_l0[14] = aio_tiRef_cache_l0[15 + 1] = 
	aio_tiRef_cache_l0[23 + 1] = aio_tiRef_cache_l0[30] = aio_tiRef_cache_l0[31 + 1] = PART_NOT_AVAILABLE;
}






/**
Fill tables to be used for the decoding process concerning inter prediction.
*/
void fill_caches_motion_vector_B_full_ref(  SLICE *slice, int b_stride, int b8_stride, short ref_cache_l0 [], short ref_cache_l1 [], 
										  short  mv_cache_l0[][2], short  mv_cache_l1[][2], unsigned char *SliceTable, const RESIDU CurrResidu [ ], 
										  short *mv_l0, short mv_l1[], short ref_l0[], short ref_l1[], int iCurrMbAddr, int ai_iX)
{

	int    top_type = 0 ;
	int    topleft_type;
	int    topright_type;
	int    left_type;


	//Updating the adress of the macroblock which belong to the neighbourhood.
	int top_xy = iCurrMbAddr - slice -> mb_stride ;
	int topleft_xy = top_xy - 1 ;
	int topright_xy = top_xy >= 0? top_xy + 1: -1 ;
	int left_xy = iCurrMbAddr - 1 ;



	//Updating their availability according to the slice.
	if ( top_xy >= 0 )   {
		top_type = SliceTable [top_xy] == slice -> slice_num ? CurrResidu [top_xy] . MbType : 0 ;

		if ( topleft_xy >= 0 && ai_iX != 0 && SliceTable [topleft_xy] == slice -> slice_num){
			topleft_type = CurrResidu [topleft_xy] . MbType ;
		}else{
			topleft_type = 0;
			ref_cache_l0[12 - 1 - 8] = ref_cache_l1[12 - 1 - 8] = PART_NOT_AVAILABLE;
		}


		if ( topright_xy >= 0 && (ai_iX != slice -> mb_stride - 1) && SliceTable [topright_xy] == slice -> slice_num){
			topright_type = CurrResidu [topright_xy] . MbType;
		}else{
			topright_type = 0;
			ref_cache_l0[12 + 4 - 8] = ref_cache_l1[12 + 4 - 8] = PART_NOT_AVAILABLE;
		}

	}else{
		top_type = 0;
		topleft_type = 0;
		topright_type = 0;

		//Update top right and top left reference
		ref_cache_l0[12 - 1 - 8] = ref_cache_l1[12 - 1 - 8] = PART_NOT_AVAILABLE;
		ref_cache_l0[12 + 4 - 8] = ref_cache_l1[12 + 4 - 8] = PART_NOT_AVAILABLE;
	}


	if ( left_xy >= 0 && ai_iX != 0 && SliceTable [left_xy] == slice -> slice_num){
		left_type = CurrResidu [left_xy] . MbType;
	}else{
		left_type = 0;

		//Update left reference
		FillRefCacheLeftPosition(ref_cache_l0, PART_NOT_AVAILABLE, PART_NOT_AVAILABLE);
		FillRefCacheLeftPosition(ref_cache_l1, PART_NOT_AVAILABLE, PART_NOT_AVAILABLE);
	}

	//Updating the mv_cache_l0 table from the current macroblock.


	//In case of a skipped or a P macroblock
	if( top_type && !IS_I(top_type)){
		FillMvCacheTopPosition(mv_cache_l0, &mv_l0[-b_stride]);
		FillMvCacheTopPosition(mv_cache_l1, &mv_l1[-b_stride]);

		ref_cache_l0[4] = ref_l0 [- b8_stride];
		ref_cache_l0[5] = ref_l0 [1 - b8_stride];
		ref_cache_l0[6] = ref_l0 [2 - b8_stride];
		ref_cache_l0[7] = ref_l0 [3 - b8_stride];

		ref_cache_l1[4] = ref_l1 [- b8_stride];
		ref_cache_l1[5] = ref_l1 [1 - b8_stride];
		ref_cache_l1[6] = ref_l1 [2 - b8_stride];
		ref_cache_l1[7] = ref_l1 [3 - b8_stride];

	}else if (!top_type){
		FillRefCacheTopPosition(ref_cache_l0, PART_NOT_AVAILABLE, PART_NOT_AVAILABLE);
		FillRefCacheTopPosition(ref_cache_l1, PART_NOT_AVAILABLE, PART_NOT_AVAILABLE);;
	}

	//In case of a skipped or a P macroblock
	if(left_type && !IS_I(left_type)){
		FillMvCacheLeftPosition(mv_cache_l0, &mv_l0[-2], b_stride);
		FillMvCacheLeftPosition(mv_cache_l1, &mv_l1[-2], b_stride);

		ref_cache_l0[11] = ref_l0[-1];
		ref_cache_l0[19] = ref_l0[-1 + b8_stride];
		ref_cache_l0[27] = ref_l0[-1 + 2 * b8_stride];
		ref_cache_l0[35] = ref_l0[-1 + 3 * b8_stride];

		ref_cache_l1[11] = ref_l1[-1];
		ref_cache_l1[19] = ref_l1[-1 + b8_stride];
		ref_cache_l1[27] = ref_l1[-1 + 2 * b8_stride];
		ref_cache_l1[35] = ref_l1[-1 + 3 * b8_stride];
	}


	//In case of a skipped or a P macroblock
	if(topleft_type && !IS_I(topleft_type)){

		mv_cache_l0 [12 - 1 - 8][0] = mv_l0[-2 - b_stride];
		mv_cache_l0 [12 - 1 - 8][1] = mv_l0[-1 - b_stride];
		mv_cache_l1 [12 - 1 - 8][0] = mv_l1[-2 - b_stride];
		mv_cache_l1 [12 - 1 - 8][1] = mv_l1[-1 - b_stride];

		ref_cache_l0[12 - 1 - 8] =  ref_l0[-1 - b8_stride];
		ref_cache_l1[12 - 1 - 8] =  ref_l1[-1 - b8_stride];

	}

	//In case of a skipped or a P macroblock
	if(topright_type && !IS_I(topright_type)){

		mv_cache_l0 [12 + 4 - 8][0] = mv_l0[8 - b_stride];
		mv_cache_l0 [12 + 4 - 8][1] = mv_l0[9 - b_stride];
		mv_cache_l1 [12 + 4 - 8][0] = mv_l1[8 - b_stride];
		mv_cache_l1 [12 + 4 - 8][1] = mv_l1[9 - b_stride];
		ref_cache_l0[12 + 4 - 8] =  ref_l0[4 - b8_stride];
		ref_cache_l1[12 + 4 - 8] =  ref_l1[4 - b8_stride];

	}

	ref_cache_l0[15+1] = ref_cache_l0[23+1] = ref_cache_l0[31+1] = ref_cache_l0[14] = ref_cache_l0[30] = 
	ref_cache_l1[15+1] = ref_cache_l1[23+1] = ref_cache_l1[31+1] = ref_cache_l1[14] = ref_cache_l1[30] = PART_NOT_AVAILABLE;
}









/**
Fill tables to be used for the decoding process concerning inter prediction.
*/
void mergeMotionData(int *mb_mode, int macroblock_mode[4], short *mv_l0, short *ref_l0, short base_index_l0[], 
					 int mv_inc, short mv_cache_l0[][2])
{


	short i;
	short *mv_0; 
	short *mv_1;
	short *mv_2; 
	short *mv_3;
	short ref_0;
	short ref_1;
	short ref_2;
	short ref_3;
	short bC1, bC2, bC3, bC4;
	short change[4] = {0,0,0,0};


	//To do all the mode
	switch(*mb_mode){
		case MBLK_8x8:
			for( i = 0; i < 4; i++){
				int cache_index = ((i & 2) << 3) + ((i & 1) << 1);
				int block_index = ((i & 2) << 2) + ((i & 1) << 1);

				if( macroblock_mode[i] == BLK_8x4){
					mv_0 = &mv_l0[(base_index_l0[block_index] >> 4) * mv_inc + (base_index_l0[block_index] & 0x07)];
					mv_1 = &mv_l0[(base_index_l0[block_index + 4] >> 4) * mv_inc + (base_index_l0[block_index + 4] & 0x07)];
					bC1 = ABS_MV(mv_0, mv_1) <= 1;

					if ( bC1){
						short mv_x = (mv_0[0] + mv_1[0] + 1) >> 1;
						short mv_y = (mv_0[1] + mv_1[1] + 1) >> 1;
						change[i] = 1;

						mv_cache_l0[12 + cache_index][0] = mv_cache_l0[13 + cache_index][0] = mv_cache_l0[20 + cache_index][0] = mv_cache_l0[21 + cache_index][0] = mv_x;
						mv_cache_l0[12 + cache_index][1] = mv_cache_l0[13 + cache_index][1] = mv_cache_l0[20 + cache_index][1] = mv_cache_l0[21 + cache_index][1] = mv_y;
						macroblock_mode[i] = BLK_8x8;
					}else {
						mv_cache_l0[12 + cache_index][0] = mv_cache_l0[13 + cache_index][0] = mv_0[0];
						mv_cache_l0[12 + cache_index][1] = mv_cache_l0[13 + cache_index][1] = mv_0[1];
						mv_cache_l0[20 + cache_index][0] = mv_cache_l0[21 + cache_index][0] = mv_1[0];
						mv_cache_l0[20 + cache_index][1] = mv_cache_l0[21 + cache_index][1] = mv_1[1];
					}

				}else if( macroblock_mode[i] == BLK_4x8){
					mv_0 = &mv_l0[(base_index_l0[block_index] >> 4) * mv_inc + (base_index_l0[block_index] & 0x07)];
					mv_1 = &mv_l0[(base_index_l0[block_index + 1] >> 4) * mv_inc + (base_index_l0[block_index + 1] & 0x07)];
					bC1 = ABS_MV(mv_0, mv_1) <= 1;

					if ( bC1){
						short mv_x = (mv_0[0] + mv_1[0] + 1) >> 1;
						short mv_y = (mv_0[1] + mv_1[1] + 1) >> 1;
						change[i] = 1;

						mv_cache_l0[12 + cache_index][0] = mv_cache_l0[13 + cache_index][0] = mv_cache_l0[20 + cache_index][0] = mv_cache_l0[21 + cache_index][0] = mv_x;
						mv_cache_l0[12 + cache_index][1] = mv_cache_l0[13 + cache_index][1] = mv_cache_l0[20 + cache_index][1] = mv_cache_l0[21 + cache_index][1] = mv_y;
						macroblock_mode[i] = BLK_8x8;
					} else {
						mv_cache_l0[12 + cache_index][0] = mv_cache_l0[20 + cache_index][0] = mv_0[0];
						mv_cache_l0[12 + cache_index][1] = mv_cache_l0[20 + cache_index][1] = mv_0[1];
						mv_cache_l0[13 + cache_index][0] = mv_cache_l0[21 + cache_index][0] = mv_1[0];
						mv_cache_l0[13 + cache_index][1] = mv_cache_l0[21 + cache_index][1] = mv_1[1];
					}
				}else 	if( macroblock_mode[i] == BLK_4x4){
					mv_0 = &mv_l0[(base_index_l0[block_index] >> 4) * mv_inc + (base_index_l0[block_index] & 0x07)];
					mv_1 = &mv_l0[(base_index_l0[block_index + 1] >> 4) * mv_inc + (base_index_l0[block_index + 1] & 0x07)];
					mv_2 = &mv_l0[(base_index_l0[block_index + 4] >> 4) * mv_inc + (base_index_l0[block_index + 4] & 0x07)];
					mv_3 = &mv_l0[(base_index_l0[block_index + 5] >> 4) * mv_inc + (base_index_l0[block_index + 5] & 0x07)];

					bC1 = ABS_MV(mv_0, mv_1) <= 1;
					bC2 = ABS_MV(mv_2, mv_3) <= 1;
					bC3 = ABS_MV(mv_0, mv_2) <= 1;
					bC4 = ABS_MV(mv_1, mv_3) <= 1;

					if ( bC1 && bC2)
						macroblock_mode[i] = BLK_8x4;
					if ( bC3 && bC4)
						macroblock_mode[i] = ( macroblock_mode[i] == BLK_8x4) ? BLK_8x8: BLK_4x8;

					change[i] = 1;

					if ( macroblock_mode[i] == BLK_8x8){
						short mv_x = (mv_0[0] + mv_1[0] + mv_2[0] + mv_3[0] + 2) >> 2;
						short mv_y = (mv_0[1] + mv_1[1] + mv_2[1] + mv_3[1] + 2) >> 2;

						mv_cache_l0[12 + cache_index][0] = mv_cache_l0[13 + cache_index][0] = 
							mv_cache_l0[20 + cache_index][0] = mv_cache_l0[21 + cache_index][0] = mv_x;
						mv_cache_l0[12 + cache_index][1] = mv_cache_l0[13 + cache_index][1] = 
							mv_cache_l0[20 + cache_index][1] = mv_cache_l0[21 + cache_index][1] = mv_y;

					}else if ( macroblock_mode[i] == BLK_8x4){
						short mv_x = (mv_0[0] + mv_1[0] + 1) >> 1;
						short mv_y = (mv_0[1] + mv_1[1] + 1) >> 1;

						mv_cache_l0[12 + cache_index][0] = mv_cache_l0[13 + cache_index][0] = mv_x;
						mv_cache_l0[12 + cache_index][1] = mv_cache_l0[13 + cache_index][1] = mv_y;

						mv_x = (mv_2[0] + mv_3[0] + 1) >> 1;
						mv_y = (mv_2[1] + mv_3[1] + 1) >> 1;

						mv_cache_l0[20 + cache_index][0] = mv_cache_l0[21 + cache_index][0] = mv_x;
						mv_cache_l0[20 + cache_index][1] = mv_cache_l0[21 + cache_index][1] = mv_y;

					}else if ( macroblock_mode[i] == BLK_4x8){
						short mv_x = (mv_0[0] + mv_2[0] + 1) >> 1;
						short mv_y = (mv_0[1] + mv_2[1] + 1) >> 1;

						mv_cache_l0[12 + cache_index][0] = mv_cache_l0[20 + cache_index][0] = mv_x;
						mv_cache_l0[12 + cache_index][1] = mv_cache_l0[20 + cache_index][1] = mv_y;

						mv_x = (mv_1[0] + mv_3[0] + 1) >> 1;
						mv_y = (mv_1[1] + mv_3[1] + 1) >> 1;

						mv_cache_l0[13 + cache_index][0] = mv_cache_l0[21 + cache_index][0] = mv_x;
						mv_cache_l0[13 + cache_index][1] = mv_cache_l0[21 + cache_index][1] = mv_y;
					} else {
						mv_cache_l0[12 + cache_index][0] = mv_0[0];
						mv_cache_l0[12 + cache_index][1] = mv_0[1];
						mv_cache_l0[13 + cache_index][0] = mv_1[0];
						mv_cache_l0[13 + cache_index][1] = mv_1[1];
						mv_cache_l0[20 + cache_index][0] = mv_2[0];
						mv_cache_l0[20 + cache_index][1] = mv_2[1];
						mv_cache_l0[21 + cache_index][0] = mv_3[0];
						mv_cache_l0[21 + cache_index][1] = mv_3[1];

					}

				}else {
					mv_cache_l0[12 + cache_index][0] = mv_cache_l0[13 + cache_index][0] = 
					mv_cache_l0[20 + cache_index][0] = mv_cache_l0[21 + cache_index][0] = mv_l0[(base_index_l0[block_index] >> 4) * mv_inc + (base_index_l0[block_index] & 0x07)];
					mv_cache_l0[12 + cache_index][1] = mv_cache_l0[13 + cache_index][1] = 
					mv_cache_l0[20 + cache_index][1] = mv_cache_l0[21 + cache_index][1] = mv_l0[(base_index_l0[block_index] >> 4) * mv_inc + (base_index_l0[block_index] & 0x07) + 1];
				}


			}

			if ( macroblock_mode[0] == BLK_8x8 && macroblock_mode[1] == BLK_8x8 && macroblock_mode[2] == BLK_8x8 && macroblock_mode[3] == BLK_8x8){
				ref_0 = ref_l0[(base_index_l0[0] >> 4) * (mv_inc >> 1) + ((base_index_l0[0] & 0x07) >> 1)];
				ref_1 = ref_l0[(base_index_l0[2] >> 4) * (mv_inc >> 1) + ((base_index_l0[2] & 0x07) >> 1)];
				ref_2 = ref_l0[(base_index_l0[8] >> 4) * (mv_inc >> 1) + ((base_index_l0[8] & 0x07) >> 1)];
				ref_3 = ref_l0[(base_index_l0[10] >> 4) * (mv_inc >> 1) + ((base_index_l0[10] & 0x07) >> 1)];

				bC1 = mv_cache_l0[12][0] == mv_cache_l0[14][0] && mv_cache_l0[12][1] == mv_cache_l0[14][1] && ref_0 == ref_1 && change[0] == change[1];
				bC2 = mv_cache_l0[28][0] == mv_cache_l0[30][0] && mv_cache_l0[28][1] == mv_cache_l0[30][1] && ref_2 == ref_3 && change[2] == change[3];
				bC3 = mv_cache_l0[12][0] == mv_cache_l0[28][0] && mv_cache_l0[12][1] == mv_cache_l0[28][1] && ref_0 == ref_2 && change[0] == change[2];
				bC4 = mv_cache_l0[14][0] == mv_cache_l0[30][0] && mv_cache_l0[14][1] == mv_cache_l0[30][1] && ref_1 == ref_3 && change[1] == change[3];

				if ( bC1 && bC2){
					*mb_mode = MBLK_16x8;
				}
				if ( bC3 && bC4){
					*mb_mode = (*mb_mode == MBLK_16x8 ? MBLK_16x16: MBLK_8x16);
				}
			}
			break;	


		case MBLK_8x16:
			mv_0 = &mv_l0[(base_index_l0[0] >> 4) * mv_inc + (base_index_l0[0] & 0x07)];
			mv_1 = &mv_l0[(base_index_l0[2] >> 4) * mv_inc + (base_index_l0[2] & 0x07)];
			bC1 = (mv_0[0] == mv_1[0]) && (mv_0[1] == mv_1[1]);

			if ( bC1) {
				*mb_mode = MBLK_16x16;
				for ( i = 0; i < 16; i ++){
					base_index_l0[i] = base_index_l0[0];
				}
				mv_cache_l0[12][0] = mv_cache_l0[13][0] = mv_cache_l0[14][0] = mv_cache_l0[15][0] = 
				mv_cache_l0[20][0] = mv_cache_l0[21][0] = mv_cache_l0[22][0] = mv_cache_l0[23][0] = 
				mv_cache_l0[28][0] = mv_cache_l0[29][0] = mv_cache_l0[30][0] = mv_cache_l0[31][0] = 
				mv_cache_l0[36][0] = mv_cache_l0[37][0] = 
				mv_cache_l0[38][0] = mv_cache_l0[39][0] = mv_0[0];
				mv_cache_l0[12][1] = mv_cache_l0[13][1] = mv_cache_l0[14][1] = mv_cache_l0[15][1] = 
				mv_cache_l0[20][1] = mv_cache_l0[21][1] = mv_cache_l0[22][1] = mv_cache_l0[23][1] = 
				mv_cache_l0[28][1] = mv_cache_l0[29][1] = mv_cache_l0[30][1] = mv_cache_l0[31][1] = 
				mv_cache_l0[36][1] = mv_cache_l0[37][1] = 
				mv_cache_l0[38][1] = mv_cache_l0[39][1] = mv_0[1];
			} else {
				for ( i = 0; i < 4; i ++){
					base_index_l0[4 * i] = base_index_l0[4 * i + 1] = base_index_l0[0];
					base_index_l0[4 * i + 2] = base_index_l0[4 * i + 3] = base_index_l0[2];
				}

				mv_cache_l0[12][0] = mv_cache_l0[13][0] = mv_cache_l0[28][0] = mv_cache_l0[29][0] = 
				mv_cache_l0[20][0] = mv_cache_l0[21][0] = mv_cache_l0[36][0] = mv_cache_l0[37][0] = mv_0[0];
				mv_cache_l0[12][1] = mv_cache_l0[13][1] = mv_cache_l0[28][1] = mv_cache_l0[29][1] = 
				mv_cache_l0[20][1] = mv_cache_l0[21][1] = mv_cache_l0[36][1] = mv_cache_l0[37][1] = mv_0[1];
				mv_cache_l0[14][0] = mv_cache_l0[15][0] = mv_cache_l0[30][0] = mv_cache_l0[31][0] = 
				mv_cache_l0[22][0] = mv_cache_l0[23][0] = mv_cache_l0[38][0] = mv_cache_l0[39][0] = mv_1[0];
				mv_cache_l0[14][1] = mv_cache_l0[15][1] = mv_cache_l0[30][1] = mv_cache_l0[31][1] = 
				mv_cache_l0[22][1] = mv_cache_l0[23][1] = mv_cache_l0[38][1] = mv_cache_l0[39][1] = mv_1[1];
			}

			break;


		case MBLK_16x8:
			mv_0 = &mv_l0[(base_index_l0[0] >> 4) * mv_inc + (base_index_l0[0] & 0x07)];
			mv_1 = &mv_l0[(base_index_l0[8] >> 4) * mv_inc + (base_index_l0[8] & 0x07)];
			bC1 = (mv_0[0] == mv_1[0]) && (mv_0[1] == mv_1[1]);

			if ( bC1) {
				*mb_mode = MBLK_16x16;
				for ( i = 0; i < 16; i ++){
					base_index_l0[i] = base_index_l0[0];
				}
				mv_cache_l0[12][0] = mv_cache_l0[13][0] = mv_cache_l0[14][0] = mv_cache_l0[15][0] = 
				mv_cache_l0[20][0] = mv_cache_l0[21][0] = mv_cache_l0[22][0] = mv_cache_l0[23][0] = 
				mv_cache_l0[28][0] = mv_cache_l0[29][0] = mv_cache_l0[30][0] = mv_cache_l0[31][0] = 
				mv_cache_l0[36][0] = mv_cache_l0[37][0] = 
				mv_cache_l0[38][0] = mv_cache_l0[39][0] = mv_0[0];
				mv_cache_l0[12][1] = mv_cache_l0[13][1] = mv_cache_l0[14][1] = mv_cache_l0[15][1] = 
				mv_cache_l0[20][1] = mv_cache_l0[21][1] = mv_cache_l0[22][1] = mv_cache_l0[23][1] = 
				mv_cache_l0[28][1] = mv_cache_l0[29][1] = mv_cache_l0[30][1] = mv_cache_l0[31][1] = 
				mv_cache_l0[36][1] = mv_cache_l0[37][1] = 
				mv_cache_l0[38][1] = mv_cache_l0[39][1] = mv_0[1];
			}else {
				for ( i = 0; i < 8; i ++){
					base_index_l0[i] = base_index_l0[0];
					base_index_l0[8 + i] = base_index_l0[8];
				}
				mv_cache_l0[12][0] = mv_cache_l0[13][0] = mv_cache_l0[14][0] = mv_cache_l0[15][0] = 
				mv_cache_l0[20][0] = mv_cache_l0[21][0] = mv_cache_l0[22][0] = mv_cache_l0[23][0] = mv_0[0];
				mv_cache_l0[12][1] = mv_cache_l0[13][1] = mv_cache_l0[14][1] = mv_cache_l0[15][1] = 
				mv_cache_l0[20][1] = mv_cache_l0[21][1] = mv_cache_l0[22][1] = mv_cache_l0[23][1] = mv_0[1];
				mv_cache_l0[28][0] = mv_cache_l0[29][0] = mv_cache_l0[30][0] = mv_cache_l0[31][0] = 
				mv_cache_l0[36][0] = mv_cache_l0[37][0] = mv_cache_l0[38][0] = mv_cache_l0[39][0] = mv_1[0];
				mv_cache_l0[28][1] = mv_cache_l0[29][1] = mv_cache_l0[30][1] = mv_cache_l0[31][1] = 
				mv_cache_l0[36][1] = mv_cache_l0[37][1] = mv_cache_l0[38][1] = mv_cache_l0[39][1] = mv_1[1];
			}



			break;

		case MBLK_16x16:
			for ( i = 0; i < 16; i ++){
				base_index_l0[i] = base_index_l0[0];
			}

			mv_cache_l0[12][0] = mv_cache_l0[13][0] = mv_cache_l0[14][0] = mv_cache_l0[15][0] = 
			mv_cache_l0[20][0] = mv_cache_l0[21][0] = mv_cache_l0[22][0] = mv_cache_l0[23][0] = 
			mv_cache_l0[28][0] = mv_cache_l0[29][0] = mv_cache_l0[30][0] = mv_cache_l0[31][0] = 
			mv_cache_l0[36][0] = mv_cache_l0[37][0] = 
			mv_cache_l0[38][0] = mv_cache_l0[39][0] = mv_l0[(base_index_l0[0] >> 4) * mv_inc + (base_index_l0[0] & 0x07)];
			mv_cache_l0[12][1] = mv_cache_l0[13][1] = mv_cache_l0[14][1] = mv_cache_l0[15][1] = 
			mv_cache_l0[20][1] = mv_cache_l0[21][1] = mv_cache_l0[22][1] = mv_cache_l0[23][1] = 
			mv_cache_l0[28][1] = mv_cache_l0[29][1] = mv_cache_l0[30][1] = mv_cache_l0[31][1] = 
			mv_cache_l0[36][1] = mv_cache_l0[37][1] = 
			mv_cache_l0[38][1] = mv_cache_l0[39][1] = mv_l0[(base_index_l0[0] >> 4) * mv_inc + (base_index_l0[0] & 0x07) + 1];

			break;
	}
}
