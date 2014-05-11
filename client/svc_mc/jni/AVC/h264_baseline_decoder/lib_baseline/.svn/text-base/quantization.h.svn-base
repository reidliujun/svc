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





#ifndef QUANTIZE_H
#define QUANTIZE_H
#include "type.h"
#include "clip.h"
#include "data.h"





#ifdef TI_OPTIM
#define SUBC(a,b) _subc(a,b)

// divides num by 6 and returns remainder (num >> 7) and result (num & 3f)
static __inline int div_6 ( int num ) // num range is 0 - 52 (6 bits)
{
    int den = 6 << 6 ; // align denominator 
    num = SUBC(num, den);
    num = SUBC(num, den);
    num = SUBC(num, den);
    num = SUBC(num, den);
    num = SUBC(num, den);
    num = SUBC(num, den);
    num = SUBC(num, den);

    return (num);
}
#else

extern const unsigned char div6 [52];
extern const unsigned char rem6 [52];

#endif


void init_quantization (W_TABLES *quantif, int scaling_matrix_present, short scaling4[][16], short scaling8[6][64] );




/**
This function quantizes a block 4x4 of residual values.

@param qp QP of the current block.
@param in Buffer to rescale.
@param out Out buffer rescaled. 
@param W4x4 Quantization table.
*/
static __inline void rescale_4x4_residual ( const int qp, const short * RESTRICT in, short * RESTRICT out,	const short W4x4[][16])
{
    int     i ;
	short f;

#ifdef TI_OPTIM
	unsigned short  shift = div_6 ( qp );
   const short *  pt_qmul = W4x4 [shift >> 7];
	shift = shift & ((1<<6) - 1);
#else
	unsigned short shift = div6[qp];
    const short * pt_qmul = W4x4[rem6 [qp]];
#endif

	if ( shift < 4){
		shift = 4 - shift;
		f = 1 << ( shift - 1);
		for ( i = 0 ; i < 16 ; i++ ) 
		{
			out [i] = ( in [i] * pt_qmul [i] + f) >> shift ;
		}
	}else{
		shift -= 4;
		for ( i = 0 ; i < 16 ; i++ ) 
		{
			out [i] = in [i] * pt_qmul [i] << shift ;
		}
	}
}


/**
This function quantizes 4 blocks 4x4 of residual values.

@param qp QP of the current block.
@param in Buffer to rescale.
@param out Out buffer rescaled. 
@param W4x4 Quantization table.
*/
static __inline void rescale_residual ( const int qp, const short * RESTRICT in, short * RESTRICT out,	const short W4x4[][16])
{
    short i;
	short f;

#ifdef TI_OPTIM
	unsigned short  shift = div_6 ( qp );
   const short *  pt_qmul = W4x4 [shift >> 7];
	shift = shift & ((1<<6) - 1);
#else
	unsigned char shift = div6[qp];
    const short * pt_qmul = W4x4[rem6 [qp]];
#endif

	if ( shift < 4){
		shift = 4 - shift;
		f = 1 << ( shift - 1);
		for ( i = 0 ; i < 16 ; i++ ){
			out [i] = ( in [i] * pt_qmul [i] + f) >> shift ;
			out [i + 16] = ( in [i + 16] * pt_qmul [i] + f) >> shift ;
			out [i + 32] = ( in [i + 32] * pt_qmul [i] + f) >> shift ;
			out [i + 48] = ( in [i + 48] * pt_qmul [i] + f) >> shift ;
		}
	}else{
		shift -= 4;
		for ( i = 0 ; i < 16 ; i++ ) 
		{
			out [i] = in [i] * pt_qmul [i] << shift ;
			out [i + 16] = in [i + 16] * pt_qmul [i] << shift ;
			out [i + 32] = in [i + 32] * pt_qmul [i] << shift ;
			out [i + 48] = in [i + 48] * pt_qmul [i] << shift ;
		}
	}
}





/**
This function quantizes a block 4x4 of DC luminances.

@param qp QP of the current block.
@param in Buffer to rescale.
@param out Out buffer rescaled. 
@param W4x4 Quantization table.  
*/
static __inline void rescale_4x4_dc_lum ( const int qp, const short * RESTRICT in, short * RESTRICT out,const short W4x4[][16])
{
    short     i ;
    const short   *pt_qmul ;
    unsigned short  shift;
    
#ifdef TI_OPTIM
	shift = div_6 ( qp );
    pt_qmul = W4x4 [shift >> 7];
	shift = shift & ((1<<6) - 1);
#else
	shift = div6[qp];
    pt_qmul = W4x4 [rem6 [qp]];
#endif

    if ( shift < 6 ) 
	{
        const short   f = 1 << ( 5 - shift ) ;
        const short   i_shift = 6 - shift ;

        for ( i = 0 ; i < 16 ; i++ ) 
		{
            out [i] = ( in [i] * pt_qmul [0] + f ) >> i_shift ;
        }
    } 
	else 
	{
        const short  i_shift = shift - 6 ;

        for ( i = 0 ; i < 16 ; i++ ) 
		{
            out [i] = in [i] * pt_qmul [0] << i_shift ;
        }
    }
}

/**
   This function quantizes a block 4x4 of DC chrominances.
   
@param qp QP of the current block.
@param in Buffer to rescale.
@param out Out buffer rescaled. 
@param W4x4 Quantization table.

*/
static __inline void rescale_4x4_dc_chr ( const int qp, const short * RESTRICT in, short * RESTRICT out,const short W4x4[][16])
{
    int     i ;
    const short   *pt_qmul ;
    unsigned short   shift;
    
#ifdef TI_OPTIM
	shift = div_6 ( qp );
    pt_qmul = W4x4[shift >> 7];
	shift = shift & ((1<<6) - 1);
#else
	shift = div6[qp];
    pt_qmul = W4x4[rem6 [qp]];
#endif

    if ( shift < 5 ) 
	{
        const short   i_shift = 5 - shift;		
        for ( i = 0 ; i < 4 ; i++ ) 
		{
            out [i] = in [i] * pt_qmul [0] >> i_shift ;
        }
    } 
	else 
	{
        const short   i_shift = shift - 5 ;

        for ( i = 0 ; i < 4 ; i++ ) 
		{
            out [i] = in [i] * pt_qmul [0] << i_shift ;
        }
    }
}

#endif



/**
This function quantizes a block 8x8 of residual values.

@param qp QP of the current block.
@param in Buffer to rescale.
@param out Out buffer rescaled. 
@param W8x8 Quantization table.
*/
static __inline void rescale_8x8_residual ( const int qp, const short * RESTRICT in, short * RESTRICT out, const short W8x8[][64])
{
    int     i ;
    const short   *pt_qmul ;
    unsigned short  shift;
    
#ifdef TI_OPTIM
	shift = div_6 ( qp );
    pt_qmul = W8x8 [shift >> 7];
	shift = shift & ((1<<6) - 1);
#else
	shift = div6[qp];
    pt_qmul = W8x8[rem6 [qp]];
#endif

    if ( shift <= 5 ) 
	{
		const short   i_shift = 6 - shift; 
		const short   f = 1 << ( i_shift - 1 ) ;

        for ( i = 0 ; i < 64 ; i++ ){
            out [i] = ( in [i] * pt_qmul [i] + f ) >> i_shift ;
        }
    } 
	else 
	{
        const short   i_shift = shift - 6;//2 ;

        for ( i = 0 ; i < 64 ; i++ ) {
            out [i] = in [i] * pt_qmul [i] << i_shift ;
        }
    }
}
