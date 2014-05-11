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

#include "main_data.h"
#include "cabac.h"
#include "data.h"
#include "vlc_cabac.h"


static const int scantable [16] = { 0, 1, 4, 8, 5, 2, 3, 6, 9, 12, 13, 10, 7, 11, 14, 15 };
static const int DC_scan[4] = {0,1,2,3};
static const int significant_coeff_flag_field_offset[2] = { 105, 277 };
static const int last_significant_coeff_flag_field_offset[2] = { 166, 338 };
static const int significant_coeff_flag_offset[6] = { 0, 15, 29, 44, 47, 297 };
static const int last_significant_coeff_flag_offset[6] = { 0, 15, 29, 44, 47, 251 };
static const int coeff_abs_level_m1_offset[6] = { 227+0, 227+10, 227+20, 227+30, 227+39, 426 };

static const int significant_coeff_flag_offset_8x8[63] = {
	0, 1, 2, 3, 4, 5, 5, 4, 4, 3, 3, 4, 4, 4, 5, 5,
	4, 4, 4, 4, 3, 3, 6, 7, 7, 7, 8, 9,10, 9, 8, 7,
	7, 6,11,12,13,11, 6, 7, 8, 9,14,10, 9, 8, 6,11,
	12,13,11, 6, 9,14,10, 9,11,12,13,11,14,10,12
};
static const int last_coeff_flag_offset_8x8[63] = {
	0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4,
	5, 5, 5, 5, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8
};




#define DECODE_SIGNIFICANCE( coefs, sig_off, last_off ) \
	for(last= 0; last < coefs; last++) { \
	unsigned char *sig_ctx = significant_coeff_ctx_base + sig_off; \
	if( get_cabac( c, sig_ctx )) { \
	unsigned char *last_ctx = last_coeff_ctx_base + last_off; \
	index[coeff_count++] = last; \
	if( get_cabac( c, last_ctx ) ) { \
	last = max_coeff; \
	break; \
	} \
	} \
	}




int get_cabac_cbf_ctx(unsigned char non_zero_count_cache[],int cat, int idx, int left_cbp, int top_cbp ) {

	int nza, nzb;
	int ctx = 0;

	if( cat == 0 ) {
		nza = left_cbp&0x100;
		nzb = top_cbp&0x100;
	} else if( cat == 1 || cat == 2 ) {
		nza = non_zero_count_cache[SCAN8(idx) - 1];
		nzb = non_zero_count_cache[SCAN8(idx) - 8];
	} else if( cat == 3 ) {
		nza = (left_cbp>>(6+idx))&0x01;
		nzb = (top_cbp>>(6+idx))&0x01;
	} else {
		nza = non_zero_count_cache[SCAN8(idx+16) - 1];
		nzb = non_zero_count_cache[SCAN8(idx+16) - 8];
	}

	if( nza > 0 )
		ctx++;

	if( nzb > 0 )
		ctx += 2;

	return ctx + (cat << 2);
}




int residual_block_cabac( CABACContext *c, unsigned char *state,  short *coeffLevel, unsigned char non_zero_count_cache[], 
						 const int *scan_table, CABAC_NEIGH *neigh, int cat, int n, int max_coeff, int *cbp, int offset )
{

	int index[64];

	int i, last;
	int abslevel1 = 1;
	int abslevelgt1 = 0;
	unsigned char coeff_count = 0;
	unsigned char *significant_coeff_ctx_base;
	unsigned char *last_coeff_ctx_base;
	unsigned char *abs_level_m1_ctx_base;


	int left_cbp = neigh -> Cbp_a;
	int top_cbp = neigh -> Cbp_b;


	/* cat: 0 ->  DC 16x16  n = 0
	*      1 ->  AC 16x16  n = luma4x4idx
	*      2 ->  Luma4x4   n = luma4x4idx
	*      3 ->  DC Chroma n = iCbCr
	*      4 ->  AC Chroma n = 4 * iCbCr + chroma4x4idx
	*      5 ->  Luma8x8   n = 4 * luma8x8idx
	*/

	/* read coded block flag */
	if( cat != 5 ) {
		if( get_cabac( c, &state[85 + get_cabac_cbf_ctx(non_zero_count_cache, cat, n, left_cbp, top_cbp ) ] ) == 0 ) {
			if( cat == 1 || cat == 2 )
				non_zero_count_cache[SCAN8(n)] = 0;
			else if( cat == 4 )
				non_zero_count_cache[SCAN8(16+n)] = 0;
			return 0;
		}
	}

	significant_coeff_ctx_base = state + significant_coeff_flag_offset[cat] + significant_coeff_flag_field_offset[0];

	last_coeff_ctx_base = state + last_significant_coeff_flag_offset[cat] + last_significant_coeff_flag_field_offset[0];

	abs_level_m1_ctx_base = state + coeff_abs_level_m1_offset[cat];



	if( cat == 5 ) {
		DECODE_SIGNIFICANCE(63, significant_coeff_flag_offset_8x8[last], last_coeff_flag_offset_8x8[last] );
	} else {
		DECODE_SIGNIFICANCE(max_coeff - 1, last, last);
	}
	if( last == max_coeff -1 ) {
		index[coeff_count++] = last;
	}

	if( cat == 0 )
		*cbp |= 0x100;
	else if( cat == 1 || cat == 2 )
		non_zero_count_cache[SCAN8(n)] = coeff_count;
	else if( cat == 3 )
		*cbp |= 0x40 << n;
	else if( cat == 4 )
		non_zero_count_cache[SCAN8(16 + n)] = coeff_count;
	else {
		non_zero_count_cache[SCAN8(n)] = non_zero_count_cache[SCAN8(n + 1)] = 
		non_zero_count_cache[SCAN8(n + 2)] = non_zero_count_cache[SCAN8(n + 3)] = coeff_count;
	}

	for( i = coeff_count - 1; i >= 0; i-- ) {
		unsigned char *ctx = (abslevelgt1 != 0 ? 0 : 4 >abslevel1 ? abslevel1:4 ) + abs_level_m1_ctx_base;
		short j = (short) scan_table[index[i] + offset] ;

		if( get_cabac( c, ctx ) == 0 ) {
			if( get_cabac_bypass( c ) ){
				coeffLevel[j] = -1;
			}else {                        
				coeffLevel[j] =  1;
			}

			abslevel1++;
		} else {
			short coeff_abs = 2;
			ctx = 5 + (4 > abslevelgt1 ? abslevelgt1: 4) + abs_level_m1_ctx_base;
			while( coeff_abs < 15 && get_cabac( c, ctx ) ) {
				coeff_abs++;
			}

			if( coeff_abs >= 15 ) {
				short j = 0;
				while( get_cabac_bypass( c ) ) {
					coeff_abs += 1 << j;
					j++;
				}

				while( j-- ) {
					if( get_cabac_bypass( c ) )
						coeff_abs += 1 << j ;
				}
			}


			if( get_cabac_bypass( c ) )
				coeffLevel[j] = - coeff_abs;
			else      
				coeffLevel[j] =  coeff_abs;

			abslevelgt1++;
		}
	}

	return 0;
}





void residual_cabac (CABACContext *c, unsigned char *state, RESIDU *CurrResidu, CABAC_NEIGH *neigh, unsigned char non_zero_count_cache [], int *cbp){


	int i, i8x8, i4x4 ;

	//Recovery of the Intra 16x16 luminance DC
	if ( CurrResidu -> MbType == INTRA_16x16 ){
		residual_block_cabac( c, state, CurrResidu -> Intra16x16DCLevel, non_zero_count_cache, scantable, neigh, 0, 0, 16, cbp, 0) ;

		if( CurrResidu -> Cbp & 15 ) {
			for( i = 0; i < 16; i++ ) {
				residual_block_cabac( c, state, &CurrResidu -> LumaLevel [i << 4], non_zero_count_cache, scantable, neigh, 1, i, 15, cbp, 1) ;
			}
		} else {

			//Updating the non_zero_count_cache table for the neighbourhood
			non_zero_count_cache [12] = non_zero_count_cache [12 + 1] = non_zero_count_cache [12 + 8] = 
				non_zero_count_cache [12 + 9] = non_zero_count_cache [14] = non_zero_count_cache [14 + 1] = 
				non_zero_count_cache [14 + 8] = non_zero_count_cache [14 + 9] = non_zero_count_cache [28] = 
				non_zero_count_cache [28 + 1] = non_zero_count_cache [28 + 8] = non_zero_count_cache [28 + 9] = 
				non_zero_count_cache [30] = non_zero_count_cache [30 + 1] = non_zero_count_cache [30 + 8] = non_zero_count_cache [30 + 9] = 0 ;
		}

	}else if ( CurrResidu -> Transform8x8 == INTRA_8x8){
		//Recovery of the luminance AC
		for ( i8x8 = 0 ; i8x8 < 4 ; i8x8++ ) {
			if ( CurrResidu -> Cbp & 1 << i8x8 ) {
				// each luma 8x8 block 
				//Recovery of the Intra 4x4 luminance AC
				residual_block_cabac( c, state, &CurrResidu -> LumaLevel [i8x8 << 6], non_zero_count_cache, zigzag_scan8x8, neigh, 5, i8x8 << 2, 64, cbp, 0) ;
			} else {
				//Updating the non_zero_count_cache table for the neighbourhood
				non_zero_count_cache [SCAN8(i8x8 << 2)] = non_zero_count_cache [SCAN8(i8x8 << 2) + 1] = 
				non_zero_count_cache [SCAN8(i8x8 << 2) + 8] = non_zero_count_cache [SCAN8(i8x8 << 2) + 9] = 0 ;
			}
		}
	}
	else{
		//Recovery of the luminance AC
		for ( i8x8 = 0 ; i8x8 < 4 ; i8x8++ ) {
			if ( CurrResidu -> Cbp & 1 << i8x8 ) {
				// each luma 8x8 block 
				for ( i4x4 = 0 ; i4x4 < 4 ; i4x4++ ) {
					// each 4x4 sub-block of block 
					//Recovery of the Intra 4x4 luminance AC
					const int   index = i4x4 + (i8x8 << 2) ;
					residual_block_cabac( c, state, &CurrResidu -> LumaLevel [index << 4], non_zero_count_cache, scantable, neigh, 2, index, 16, cbp, 0) ;
				}
			} else {

				//Updating the non_zero_count_cache table for the neighbourhood
				non_zero_count_cache [SCAN8(i8x8 << 2)] = non_zero_count_cache [SCAN8(i8x8 << 2) + 1] = 
				non_zero_count_cache [SCAN8(i8x8 << 2) + 8] = non_zero_count_cache [SCAN8(i8x8 << 2) + 9] = 0 ;
			}
		}
	}

	//Recovery of the Chrominance DC
	if ( CurrResidu -> Cbp & 0x30 ) {
		//Chrominance Cb
		residual_block_cabac( c,state,  CurrResidu -> ChromaDCLevel_Cb, non_zero_count_cache, DC_scan, neigh, 3 , 0, 4, cbp, 0) ;

		//Chrominance Cr
		residual_block_cabac( c,state,  CurrResidu -> ChromaDCLevel_Cr, non_zero_count_cache, DC_scan, neigh, 3 , 1, 4, cbp, 0) ;
	}

	//Recovery of the Chrominance AC
	if ( CurrResidu -> Cbp & 0x20 ) {
		for ( i4x4 = 0 ; i4x4 < 4 ; i4x4++ ) {
			//Chrominance Cb
			residual_block_cabac( c,state,  &CurrResidu -> ChromaACLevel_Cb [i4x4 << 4], non_zero_count_cache, scantable, neigh, 4, i4x4, 15, cbp, 1) ;
		}
		for ( i4x4 = 0 ; i4x4 < 4 ; i4x4++ ) {
			//Chrominance Cr
			residual_block_cabac( c,state,  &CurrResidu -> ChromaACLevel_Cr [i4x4 << 4], non_zero_count_cache,scantable, neigh, 4, i4x4+4, 15, cbp, 1) ;
		}
	} else{
		//Updating the non_zero_count_cache table for the neighbourhood
		non_zero_count_cache [9 + 0] = non_zero_count_cache [9 + 1] = non_zero_count_cache [9 + 8] = non_zero_count_cache [9 + 9] =
		non_zero_count_cache [33 + 0] = non_zero_count_cache [33 + 1] = non_zero_count_cache [33 + 8] = non_zero_count_cache [33 + 9] = 0 ;
	}
}

