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
* but WITHOUT ANY WARRANTY; without even the implied warranty off
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


#ifndef CABAC_MB_PRED_H
#define CABAC_MB_PRED_H


#include "main_data.h"
#include "cabac.h"
#include "mb_pred.h"


static __inline short GetCabacIPredMode(CABACContext *c, unsigned char *state, short *intra4x4_pred_mode_cache, int luma4x4BlkIdx)
{


	const short predicted = get_predicted(intra4x4_pred_mode_cache, luma4x4BlkIdx);

	if ( !get_cabac( c, &state[68] ))  {
		short rem_intra4x4_pred_mode = (short) get_cabac( c, &state[69] );
		rem_intra4x4_pred_mode += (short) (get_cabac( c, &state[69] ) << 1);
		rem_intra4x4_pred_mode += (short) (get_cabac( c, &state[69] ) << 2);
		if ( rem_intra4x4_pred_mode < predicted){
			return rem_intra4x4_pred_mode;
		}else {
			return rem_intra4x4_pred_mode + 1;
		}
	}
	return predicted;
}



static __inline unsigned char decode_cabac_p_mb_sub_type(  CABACContext *c, unsigned char *state) {
	if( get_cabac( c, &state[21] ) ){
		return 0;   /* 8x8 */
	}

	if( !get_cabac( c, &state[22] ) ){
		return 1;   /* 8x4 */
	}
	if( get_cabac( c, &state[23] ) ){
		return 2;   /* 4x8 */
	}
	return 3;       /* 4x4 */
}



/**
Decode B macroblock type.
*/
static __inline unsigned char decode_cabac_b_mb_sub_type( CABACContext *c, unsigned char *state) {

	unsigned char type;

	if( !get_cabac( c, &state[36] ) ){
		return 0;   /* B_Direct_8x8 */
	}

	if( !get_cabac( c, &state[37] ) ){
		type = (unsigned char) get_cabac( c, &state[39] ); /* B_L0_8x8, B_L1_8x8 */
		return type + 1;
	}

	type = 3;
	if( get_cabac( c, &state[38] ) ) {
		if( get_cabac(c, &state[39] ) ){
			type = (unsigned char) get_cabac( c, &state[39] ); /* B_L1_4x4, B_Bi_4x4 */
			return 11 + type;
		}
		type += 4;
	}

	type += (unsigned char) (get_cabac( c, &state[39] ) << 1);
	type += (unsigned char) get_cabac( c, &state[39] );
	return type;
}



#endif
