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

#ifndef BLK4X4_H
#define BLK4X4_H

static int PuissanceBlock16 [16] = {0, 1, 4, 5, 2, 3, 6, 7, 8, 9, 12, 13, 10, 11, 14, 15};
static int PuissanceBlock64[4] = {0, 2, 8, 10};

#ifdef TI_OPTIM
	#define blk4x4_coded blk4x4_coded_TI
	#define blk8x8_coded blk8x8_coded_TI
	#define UpdateCoeff16 UpdateCoeff16_TI
	#define UpdateCoeff64 UpdateCoeff64_TI


/**
Return true if a non zero value is found in a 4x4 table
*/
static __inline int blk4x4_coded_TI(short *residu, const int PicWidthinPix){

	if (_mem8(residu)) return 1;
	if (_mem8(residu+ PicWidthinPix)) return 1;
	if (_mem8(residu+ PicWidthinPix*2)) return 1;
	if (_mem8(residu+ PicWidthinPix*3)) return 1;

	return 0;
}

/**
Return true when there is at least one non zero count in the current blk8x8.
*/
static __inline int UpdateCoeff64_TI(short *CoeffLevel){

	if (_mem8(CoeffLevel)) return 1;
	if (_mem8(CoeffLevel+4)) return 1;
	if (_mem8(CoeffLevel+8)) return 1;
	if (_mem8(CoeffLevel+12)) return 1;

	if (_mem8(CoeffLevel+16)) return 1;
	if (_mem8(CoeffLevel+20)) return 1;
	if (_mem8(CoeffLevel+24)) return 1;
	if (_mem8(CoeffLevel+28)) return 1;

	if (_mem8(CoeffLevel+32)) return 1;
	if (_mem8(CoeffLevel+36)) return 1;
	if (_mem8(CoeffLevel+40)) return 1;
	if (_mem8(CoeffLevel+44)) return 1;

	if (_mem8(CoeffLevel+48)) return 1;
	if (_mem8(CoeffLevel+52)) return 1;
	if (_mem8(CoeffLevel+56)) return 1;
	if (_mem8(CoeffLevel+60)) return 1;


	return 0;
}

/**
Return true if a non zero value is found in a 8x8 table
*/
static __inline int blk8x8_coded_TI(short *residu, const int PicWidthinPix){

	if (_mem8(residu)) return 1;
	if (_mem8(residu+ 4)) return 1;
	if (_mem8(residu+ PicWidthinPix)) return 1;
	if (_mem8(residu+ PicWidthinPix+4)) return 1;
	if (_mem8(residu+ PicWidthinPix*2)) return 1;
	if (_mem8(residu+ PicWidthinPix*2+4)) return 1;
	if (_mem8(residu+ PicWidthinPix*3)) return 1;
	if (_mem8(residu+ PicWidthinPix*3+4)) return 1;

	return 0;
}


/**
Return true when there is at least one non zero count in the current blk4x4.
*/
static __inline int UpdateCoeff16_TI(short *CoeffLevel){

	
	if (_mem8(CoeffLevel)) return 1;
	if (_mem8(CoeffLevel+4)) return 1;
	if (_mem8(CoeffLevel+8)) return 1;
	if (_mem8(CoeffLevel+12)) return 1;

	return 0;
}


#else
	#define blk4x4_coded blk4x4_coded_C
	#define blk8x8_coded blk8x8_coded_C
	#define UpdateCoeff16 UpdateCoeff16_C   
	#define UpdateCoeff64 UpdateCoeff64_C   



/**
Return true if a non zero value is found in a 4x4 table
*/
static __inline int blk4x4_coded_C(short *residu, const int PicWidthinPix){

	int i,j;
	for ( i = 0; i < 4; i++){
		for ( j = 0; j < 4; j++){
			if (residu[j + i * PicWidthinPix]){
				return 1;
			}
		}
	}

	return 0;
}

/**
Return true if a non zero value is found in a 8x8 table
*/
static __inline int blk8x8_coded_C(short *residu, const int PicWidthinPix){

	int i,j;
	for ( i = 0; i < 8; i++){
		for ( j = 0; j < 8; j++){
			if (residu[j + i * PicWidthinPix]){
				return 1;
			}
		}
	}

	return 0;
}


/**
Return true when there is at least one non zero count in the current blk4x4.
*/
static __inline int UpdateCoeff16_C(short *CoeffLevel){

	int i;
	for ( i = 0; i < 16; i++){
		if( CoeffLevel[i] != 0){
			return 1;
		}
	}
	return 0;
}

/**
Return true when there is at least one non zero count in the current blk8x8.
*/
static __inline int UpdateCoeff64_C(short *CoeffLevel){

	int i;
	for ( i = 0; i < 64; i++){
		if( CoeffLevel[i] != 0){
			return 1;
		}
	}
	return 0;
}

#endif


/**
The function updates the blk4x4 variable of the current macroblock according to the decoded residual.
*/
static __inline void update_blk4x4Res(short *residu, RESIDU *CurrResidu, const int PicWidthinPix){


	int i;
	if(!CurrResidu -> Transform8x8){
		for (i = 0 ; i < 16; i++){
			short *ptr = &residu[((i & 0x03) << 2) +  ((i >> 2) << 2) * PicWidthinPix];
			CurrResidu -> blk4x4_Res += (blk4x4_coded(ptr, PicWidthinPix)) << i;
		}
	}else{
		for (i = 0 ; i < 4; i++){
			short *ptr = &residu[((i & 0x01) << 3) +  ((i >> 1) << 3) * PicWidthinPix];
			if(blk8x8_coded(ptr, PicWidthinPix)){
				CurrResidu -> blk4x4_Res += (0x33 << PuissanceBlock64 [i]);
			}
		}
	}
}



/**
Update the blk4x4 variable according to the non zero coefficient.
*/
static __inline void UpdateBlk4x4(RESIDU *CurrResidu){

	int i;
	CurrResidu -> blk4x4 = 0;
	if ( CurrResidu -> Transform8x8){
		for (i = 0 ; i < 4; i++){
			if( UpdateCoeff64(&CurrResidu -> LumaLevel [i << 6])){
				CurrResidu -> blk4x4 += (0x33 << PuissanceBlock64 [i]);
			}
		}
	}else{
		for (i = 0 ; i < 16; i++){
			if( UpdateCoeff16(&CurrResidu -> LumaLevel [i << 4])){
				CurrResidu -> blk4x4 += 1 << PuissanceBlock16 [i];
			}
		}
	}
}

#endif
