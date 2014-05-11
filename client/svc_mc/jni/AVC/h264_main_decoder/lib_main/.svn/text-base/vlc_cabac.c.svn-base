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
#include "refill.h"
#include "vlc_cabac.h"
#include "CabacMbPred.h"
#include "mb_pred_main.h"
#include "motion_vector.h"
#include "Direct_prediction.h"


static const int locx_8x8[16] = { 0, 1, 0, 1, 2, 3, 2, 3, 0, 1, 0, 1, 2, 3, 2, 3};
static const int locy_8x8[16] = {0, 0, 1, 1, 0, 0, 1, 1, 2, 2, 3, 3, 2, 2, 3, 3};



static const int block_idx_xy[4][4] = {
	{ 0, 2, 8,  10},
	{ 1, 3, 9,  11},
	{ 4, 6, 12, 14},
	{ 5, 7, 13, 15}
};



/**
Loa for CABAC motion vector neighbourhood.
*/
void get_mv_neighbourhood( int *mva, int *mvb, short mv_cache[][2], int sub_mb_type, int mbPartIdx, int SubMbPart, int compIdx, int slice_type){

	int ind = SCAN8((mbPartIdx << 2) + SubMbPart);
	if (slice_type == SLICE_TYPE_P){
		if (sub_mb_type == 1){
			ind = SCAN8(((mbPartIdx << 1) + SubMbPart) << 1);
			*mva = mv_cache[ ind - 1][compIdx];
			*mvb = mv_cache[ ind - 8][compIdx];
		}else{
			*mva = mv_cache[ ind - 1][compIdx];
			*mvb = mv_cache[ ind - 8][compIdx];
		}
	}else{
		if (sub_mb_type == 4 || sub_mb_type == 6 || sub_mb_type == 8){
			ind = SCAN8(((mbPartIdx << 1) + SubMbPart) << 1);
			*mva = mv_cache[ ind - 1][compIdx];
			*mvb = mv_cache[ ind - 8][compIdx];
		}else{
			*mva = mv_cache[ ind - 1][compIdx];
			*mvb = mv_cache[ ind - 8][compIdx];
		}
	}
}




/**
CABAC parsing for skip flag.
*/
int decode_cabac_mb_skip( CABACContext *c, unsigned char *state, int AvailMask, unsigned char *SliceTable, 
						 RESIDU *CurrResidu, int slice_type, int mb_xy, int mb_stride) 
{


	const int mba_xy = mb_xy - 1;
	const int mbb_xy = mb_xy - mb_stride;
	int ctx = 0;

	if( (AvailMask & 0x01) && SliceTable[mba_xy] == SliceTable[mb_xy] 
	&& CurrResidu [mba_xy] . MbType != P_Skip && CurrResidu [mba_xy] . MbType != B_Skip  ){
		ctx++;
	}

	if( ((AvailMask >> 1) & 0x01) && SliceTable[mbb_xy] == SliceTable[mb_xy] 
	&& CurrResidu [mbb_xy] . MbType != P_Skip && CurrResidu [mbb_xy] . MbType != B_Skip){
		ctx++;
	}


	if (slice_type == SLICE_TYPE_P){
		return get_cabac( c, &state[11 + ctx] );
	} else {	
		return get_cabac( c, &state[24 + ctx] );
	}
}

/**
CABAC parsing for chrominance prediction mode.
*/
unsigned char decode_cabac_mb_chroma_pre_mode( CABACContext *c, unsigned char *state, int AvailMask, CABAC_NEIGH *neigh) {



	int intra_chroma_pred_mode_top = neigh -> intra_chroma_pred_mode_top;
	int intra_chroma_pred_mode_left = neigh -> intra_chroma_pred_mode_left;

	int ctx = 0;

	/* No need to test for IS_INTRA4x4 and IS_INTRA16x16, as we set chroma_pred_mode_table to 0 */
	if( (AvailMask & 0x01) && intra_chroma_pred_mode_left!= 0){
		ctx++;
	}

	if(((AvailMask >> 1) & 0x01) && intra_chroma_pred_mode_top != 0 ){
		ctx++;
	}

	if( get_cabac( c, &state[64 + ctx] ) == 0 ){
		return 0;
	}

	if( get_cabac( c, &state[64 + 3] ) == 0 ){
		return 1;
	}
	if( get_cabac( c, &state[64 + 3] ) == 0 ){
		return 2;
	}else {
		return 3;
	}
}

/**
CABAC parsing for Luminance cbp.
*/
int decode_cabac_mb_cbp_luma( CABACContext *c, unsigned char *state, int AvailMask, CABAC_NEIGH *neigh, int mb_x, int mb_y) {


	int cbp = 0;
	int i8x8;

	for( i8x8 = 0; i8x8 < 4; i8x8++ ) {
		int cbp_a = -1;
		int cbp_b = -1;
		int ctx = 0;

		int x = locx_8x8[i8x8 << 2];
		int y = locy_8x8[i8x8 << 2];

		if( x > 0 )
			cbp_a = cbp;
		else if( mb_x > 0 && (AvailMask & 0x01)) {
			cbp_a = neigh -> Cbp_a & 15;
		}

		if( y > 0 )
			cbp_b = cbp;
		else if( mb_y > 0 && ((AvailMask >> 1) & 0x01)) {
			cbp_b = neigh -> Cbp_b & 15;
		}

		/* No need to test for skip as we put 0 for skip block */
		/* No need to test for IPCM as we put 1 for IPCM block */
		if( cbp_a >= 0 ) {
			int i8x8a = (block_idx_xy[(x - 1) & 0x03][y]) >> 2;
			if( ((cbp_a >> i8x8a)&0x01) == 0 )
				ctx++;
		}

		if( cbp_b >= 0 ) {
			int i8x8b = (block_idx_xy[x][(y - 1)& 0x03]) >> 2;
			if( ((cbp_b >> i8x8b)&0x01) == 0 )
				ctx += 2;
		}

		if( get_cabac( c, &state[73 + ctx] ) ) {
			cbp |= 1 << i8x8;
		}
	}
	return cbp;
}


/**
CABAC parsing for chrominance cbp.
*/
int decode_cabac_mb_cbp_chroma( CABACContext *c, unsigned char *state, CABAC_NEIGH *neigh) {

	int cbp_a = (neigh -> Cbp_a >> 4) & 0x03;
	int cbp_b = (neigh -> Cbp_b >> 4) & 0x03;

	int ctx = 0;
	if( cbp_a > 0 ) ctx++;
	if( cbp_b > 0 ) ctx += 2;
	if( get_cabac( c, &state[77 + ctx] ) == 0 ){
		return 0;
	}

	ctx = 4;
	if( cbp_a == 2 ) ctx++;
	if( cbp_b == 2 ) ctx += 2;
	return 1 + get_cabac( c, &state[77 + ctx] );
}

/**
CABAC parsing for delta qp.
*/
int decode_cabac_mb_dqp(  CABACContext *c, unsigned char *state, CABAC_NEIGH *neigh	, int mb_type_left) {

	int   ctx = 0;
	int   val = 0;

	if( neigh -> last_qp_diff != 0 && ((mb_type_left == INTRA_16x16 ) || (neigh -> last_cbp &0x3f)  ))
		ctx++;

	while( get_cabac( c, &state[60 + ctx] ) ) {
		if( ctx < 2 ){
			ctx = 2;
		} else{
			ctx = 3;
		}
		val++;
	}

	if( val&0x01 )
		return (val + 1) >> 1;
	else
		return -((val + 1) >> 1);
}


/**
CABAC parsing for reference list index.
*/
unsigned char decode_cabac_mb_ref(  CABACContext *c, unsigned char *state, int refa, int refb) {

	unsigned char ref  = 0;
	unsigned char ctx  = 0;

	if( refa > 0 )
		ctx++;
	if( refb > 0 )
		ctx += 2;


	while( get_cabac( c, &state[54 + ctx] ) ) {
		ref++;
		if( ctx < 4 )
			ctx = 4;
		else
			ctx = 5;
	}
	return ref;
}


/**
CABAC parsing for motion vector.
*/
short decode_cabac_mb_mvd(CABACContext *c, unsigned char *state, int mva, int mvb, int l) {

	int amvd = abs(mva) + abs (mvb);
	int ctxbase = (l == 0) ? 40 : 47;
	short ctx, mvd;

	if( amvd < 3 ){
		ctx = 0;
	}else if( amvd > 32 ){
		ctx = 2;
	} else {
		ctx = 1;
	}

	if(!get_cabac(c, &state[ctxbase + ctx])){
		return 0;
	}

	mvd = 1;
	ctx = 3;
	while( mvd < 9 && get_cabac( c, &state[ctxbase + ctx] ) ) {
		mvd++;
		if( ctx < 6 ){
			ctx++;
		}
	}

	if( mvd >= 9 ) {
		int k = 3;
		while( get_cabac_bypass( c ) ) {
			mvd += 1 << k;
			k++;
		}
		while( k-- ) {
			if( get_cabac_bypass( c ) )
				mvd += 1 << k;
		}
	}

	if( get_cabac_bypass( c ) )  {
		return -mvd;
	}else  {
		return  mvd;
	}
}






