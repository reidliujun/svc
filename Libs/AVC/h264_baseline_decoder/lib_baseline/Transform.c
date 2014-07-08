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

#define C_TRANSFORM
#include "Transform.h"





/**
   This function permits to transform from frequential to spatial domain.
 @param block Table of transform coefficients.
 @param dst Table of transform coefficients.
 */

void ict_4x4_dc_lum ( const short * RESTRICT  block, short * RESTRICT  dst)
{

	__declspec(align(16)) short BlockTemp[16];

#ifdef TI_OPTIM


	short i = 0 ;
	short zz[16];
	short za[4],zb[4],zc[4],zd[4];

	for ( i = 0 ; i < 4 ; i++ ) {

		_amem4(&zz[i*4]) = _add2(_amem4_const(&block [0 + i*4]),_amem4_const(&block [2 + i*4]));  
		_amem4(&zz[i*4+2]) = _sub2(_amem4_const(&block [0 + i*4]),_amem4_const(&block [2 + i*4]));     
       	BlockTemp [i*4 + 0] = zz[i*4] + zz[i*4+1] ;
		BlockTemp [i*4 + 1] = zz[i*4+2] + zz[i*4+3] ;
		BlockTemp [i*4 + 3] = zz[i*4] - zz[i*4+1] ;		
        BlockTemp [i*4 + 2] = zz[i*4+2] - zz[i*4+3] ;	 
        	 
    }

	_amem4(&za[0]) = _add2(_amem4_const(&BlockTemp[0]),_amem4_const(&BlockTemp[4])); 
	_amem4(&zb[0]) = _add2(_amem4_const(&BlockTemp[8]),_amem4_const(&BlockTemp[12])); 
	_amem4(&zc[0]) = _sub2(_amem4_const(&BlockTemp[0]),_amem4_const(&BlockTemp[4])); 
	_amem4(&zd[0]) = _sub2(_amem4_const(&BlockTemp[12]),_amem4_const(&BlockTemp[8])); 	
	_amem4(&za[2]) = _add2(_amem4_const(&BlockTemp [2]),_amem4_const(&BlockTemp[6])); 
	_amem4(&zb[2]) = _add2(_amem4_const(&BlockTemp [10]),_amem4_const(&BlockTemp[14])); 
	_amem4(&zc[2]) = _sub2(_amem4_const(&BlockTemp [2]),_amem4_const(&BlockTemp[6])); 
	_amem4(&zd[2]) = _sub2(_amem4_const(&BlockTemp [14]),_amem4_const(&BlockTemp[10])); 


	dst [0] = za[0] + zb[0] ;
    dst [1] = za[0] - zb[0] ;

    dst [2] = zc[0] + zd[0] ;
    dst [3] = zc[0] - zd[0] ;	

	dst [4] = za[1] + zb[1] ;
    dst [5] = za[1] - zb[1] ;

    dst [6] = zc[1] + zd[1] ;
    dst [7] = zc[1] - zd[1] ;

	dst [8] = za[2] + zb[2] ;
    dst [9] = za[2] - zb[2] ;

    dst [10] = zc[2] + zd[2] ;
    dst [11] = zc[2] - zd[2] ;	

	dst [12] = za[3] + zb[3] ;
    dst [13] = za[3] - zb[3] ;

    dst [14] = zc[3] + zd[3] ;
    dst [15] = zc[3] - zd[3] ;


	// a tenter lorsque l'on aura un C64x+
	//_amem4(&dst[0]) = _addsub(_amem4_const(&za[0],_amem4_const(&zb[0])); 	
	//_amem4(&dst[2]) = _addsub(_amem4_const(&zc[0],_amem4_const(&zd[0])); 	
	//_amem4(&dst[4]) = _addsub(_amem4_const(&za[1],_amem4_const(&zb[1]));
	//_amem4(&dst[6]) = _addsub(_amem4_const(&zc[1],_amem4_const(&zd[1])); 
	//_amem4(&dst[8]) = _addsub(_amem4_const(&za[2],_amem4_const(&zb[2])); 	
	//_amem4(&dst[10]) = _addsub(_amem4_const(&zc[2],_amem4_const(&zd[2])); 
	//_amem4(&dst[12]) = _addsub(_amem4_const(&za[3],_amem4_const(&zb[3])); 
	//_amem4(&dst[14]) = _addsub(_amem4_const(&zc[3],_amem4_const(&zd[3])); 


#else

	short   i;

    for ( i = 0 ; i < 4 ; i++ ) {
	  	const short z0 = block [0 + (i << 2)] + block [2 + (i << 2)]; 
		const short z1 = block [1 + (i << 2)] + block [3 + (i << 2)];	
        const short z2 = block [0 + (i << 2)] - block [2 + (i << 2)];
        const short z3 = block [1 + (i << 2)] - block [3 + (i << 2)];

		BlockTemp [0 + (i << 2)] = z0 + z1 ;
		BlockTemp [1 + (i << 2)] = z2 + z3 ;
        BlockTemp [2 + (i << 2)] = z2 - z3 ;	
		BlockTemp [3 + (i << 2)] = z0 - z1 ;
    }


    for ( i = 0 ; i < 4 ; i++ ) {
        const short   z0 = BlockTemp [i] + BlockTemp [i + 8];
        const short   z1 = BlockTemp [i] - BlockTemp [i + 8];
        const short   z2 = BlockTemp [i + 4] - BlockTemp [i + 12];
        const short   z3 = BlockTemp [i + 4] + BlockTemp [i + 12];
        dst [0 + (i << 2)] = z0 + z3 ;
        dst [1 + (i << 2)] = z1 + z2 ;
        dst [2 + (i << 2)] = z1 - z2 ;
        dst [3 + (i << 2)] = z0 - z3 ;
    }

#endif
}



//Faster to let it here instead of declare it in inline
void ict_4x4_residual_C ( short * RESTRICT  block, unsigned char * RESTRICT  image, const short PicWidthInPix){

    short  i, j = 0;
	__declspec(align(16)) short BlockTemp[16];
	block [0] += 32;
 
    for ( i = 0 ; i < 4 ; i++ ) {
        const short   z0 = block [j] + block [2 + j];
        const short   z1 = block [j] - block [2 + j];
        const short   z2 = (block [1 + j] >> 1) - block [3 + j];
        const short   z3 = block [1 + j] + (block [3 + j] >> 1);
        BlockTemp [j + 0] = z0 + z3 ;
        BlockTemp [j + 1] = z1 + z2 ;
        BlockTemp [j + 2] = z1 - z2 ;
        BlockTemp [j + 3] = z0 - z3 ;
        j += 4 ;
    }

    for ( i = 0 ; i < 4 ; i++ ) {
        const short   z0 = BlockTemp [i] + BlockTemp [8 + i];
        const short   z1 = BlockTemp [i] - BlockTemp [8 + i];
        const short   z2 = (BlockTemp [4 + i] >> 1) - BlockTemp [12 + i];
        const short   z3 = BlockTemp [4 + i] + (BlockTemp [12 + i] >> 1);

        image [i]						 = (unsigned char) (CLIP255(((z0 + z3) >> 6) + image [i]));
        image [PicWidthInPix + i]		 = (unsigned char) (CLIP255(((z1 + z2) >> 6) + image [PicWidthInPix + i]));
        image [(PicWidthInPix << 1) + i] = (unsigned char) (CLIP255(((z1 - z2) >> 6) + image [(PicWidthInPix << 1) + i]));
        image [3 * PicWidthInPix + i]	 = (unsigned char) (CLIP255(((z0 - z3) >> 6) + image [3 * PicWidthInPix + i]));
    }
}
