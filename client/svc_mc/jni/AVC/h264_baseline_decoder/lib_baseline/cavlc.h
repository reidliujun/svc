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

#ifndef CAVLC_H
#define CAVLC_H
#include <string.h>


#include "type.h"
#include "data.h"
#include "bitstream.h"
#include "fill_caches.h"
#include "decode_mb_I.h"



#define residual_block_cavlc_16 residual_block_cavlc_16_C
#define residual_block_cavlc_4 residual_block_cavlc_4_C




#ifdef MMX
#undef residual_block_cavlc_16
#define residual_block_cavlc_16 residual_block_cavlc_16_MMX

//Do not compil with INTEL
#undef residual_block_cavlc_4
#define residual_block_cavlc_4 residual_block_cavlc_4_MMX
#endif


#ifndef TI_OPTIM
extern const int ietr_log2_tab [];
extern const int coeff_token_table_index [];
#endif



/**
This function permits to read the level_prefix. 

@param data The NAL unit.
@param aio_piPosition The current aio_piPosition in the NAL.
@param ao_piRestBits 

*/
static __inline unsigned int read_levelprefix ( const unsigned char * ai_poData,  int *aio_piPosition,  int *ao_piRestBits)
{
#ifdef TI_OPTIM
	unsigned int buf = show32bits(ai_poData, *aio_piPosition);
	// Get rid of the sign
    int log = _norm(buf >> 1);
    *ao_piRestBits = buf << (log + 1);
    *aio_piPosition += (log + 1);

	
    return log;
#else
	
	unsigned int outbuf;
	unsigned int buf = outbuf = show32bits(ai_poData, *aio_piPosition);

	int flag = ((buf & 0xffff0000) != 0) << 4;
	int log = flag;

	buf >>= flag;

	flag = ((buf & 0xff00) != 0) << 3;
	buf >>= flag;
	log += flag;

	log += ietr_log2_tab [buf];
	log = 32 - log ;
	*aio_piPosition += log ;
	*ao_piRestBits = outbuf << log;
	return log - 1;
#endif
}






/**
This function gets the CoeffToken table index. 

@param ai_iPred_non_zero_count.
*/
static  __inline int getCoeffTokenIndex(const int ai_iPred_non_zero_count)
{
#ifdef TI_OPTIM
 return(31 - _norm((_sshl(ai_iPred_non_zero_count, 27)) >> 28));
#else
 return(coeff_token_table_index [ai_iPred_non_zero_count]);
#endif
}




/**
This function permits to determine the coeff_token and the total_zeros. 
// light version for ai_iMax_depth = 1

@param ai_pcData The NAL unit.
@param aio_piPosition The current aio_piPosition in the NAL.
@param ai_pstTable VLC table where each parameter is stocked.
@param ai_iBits Number of bits to read in the NAL.
*/

static  __inline int read_ceMax_depth1 ( const unsigned char *  ai_pcData,  int * const aio_piPosition, const VLC_TYPE ai_pstTable [ ], const int ai_iBits)
{

	
	unsigned int index = showNbits_9max(ai_pcData, *aio_piPosition, ai_iBits);

    int code = ai_pstTable [index] . code;
    int n = ai_pstTable [index] . len;

    *aio_piPosition += n ;

    return code ;
}





/**
This function permits to determine the coeff_token and the total_zeros. 
// light version for ai_iMax_depth = 2 && ai_iBits == RUN7_VLC_BITS

@param ai_pcData The NAL unit.
@param aio_piPosition The current aio_piPosition in the NAL.
@param ai_pstTable VLC table where each parameter is stocked.
*/

static  __inline short read_ceMax_depth2 ( const unsigned char *  ai_pcData, int *aio_piPosition, const VLC_TYPE ai_pstTable [ ])
{

    unsigned int index2 = showNbits(ai_pcData, *aio_piPosition, 24);
	unsigned int index = index2 >> (24 - RUN7_VLC_BITS);

    
    int n = ai_pstTable [index].len ;
	short code = ai_pstTable [index].code ;

    if ( n < 0 ) {
        int nb_bits = -n ;
		*aio_piPosition += RUN7_VLC_BITS ;
    
		index2 <<= (8 + RUN7_VLC_BITS);
        index = (index2 >> (32 - nb_bits)) + code ;

        code = ai_pstTable [index] . code ;
        n = ai_pstTable [index] . len ;
    }

    *aio_piPosition += n;

    return code;
}






/**
This function maps an unsigned int to an int:
0 -> 1
1 -> -1
2 -> 2
3 -> -2
4 -> 3

@param levelCode unsigned to convert.
*/
static __inline short MapUnsignedToShort (const int levelCode){

#ifdef TI_OPTIM
	short val = (levelCode + 2) >> 1;
	if(levelCode&1) val = -val;
#else
	int mask = -(levelCode & 1);
	short val = (short)((((2 + levelCode) >> 1) ^ mask) - mask) ;
#endif

	return val;
}




/**
This function permits to determine a context for the next decoding process. 
THe function returns the number of the vlc table where the decoding process will have to get back the coeff_token.

@param n Index of the current block 4x4.
@param non_zero_count_cache A cache table where coeff_token is stocked for each 4x4 block of the current macroblock.
*/
static __inline int pred_non_zero_count ( const short ai_iN, const unsigned char ai_piNon_zero_count_cache [ ] )
{
    const unsigned char index8 = (unsigned char) SCAN8(ai_iN);
    const unsigned char left = ai_piNon_zero_count_cache [index8 - 1];
    const unsigned char top = ai_piNon_zero_count_cache [index8 - 8];
    unsigned char i = left + top ;
    unsigned char flag = -(int)( i < 64 ) ;
    
    i = (i&~flag) | (((i + 1) >> 1) & flag) ;

    return i & 31;
}




/**
This function permits to determine the TrailingOnes. 

@param ai_iCoeff_token Specifies the total number of non-zero transform coefficient levels.
*/
static __inline int TrailingOnes ( const int ai_iCoeff_token ){
    return ai_iCoeff_token & 3 ;
}




/**
This function permits to determine the TotalCoeff. 
This functoin determines the number of non-zero transform coefficient levels.

@param ai_iCoeff_token Specifies the total number of non-zero transform coefficient levels.
*/

static __inline int TotalCoeff ( const int ai_iCoeff_token ){
	
	return ai_iCoeff_token >> 2 ;
}


//Prototypes

void residual_block_cavlc_16 ( short *   ao_piCoeffLevel, const int ai_piMaxNumCoeff, const unsigned char *   ai_poData, int *   aio_piPosition
							  , const int ai_iN, const VLC_TABLES *vlc, unsigned char aio_piNon_zero_count_cache [ ]);

void residual_block_cavlc_4 ( short * ao_piCoeffLevel, const unsigned char *ai_poData, int * aio_piPosition , const VLC_TABLES *vlc);

void residual (const unsigned char *ai_pcData,  int * aio_piPosition, RESIDU *residu , const VLC_TABLES *Vlc,  unsigned char NonZeroCountCache [ ]);


void init_coeff_token_vlc_adress ( int coeff_token_vlc_adress [] );
int build_table ( VLC_TYPE *vlc, int table_nb_bits, int nb_codes, const int *bits, const int *codes , int code_prefix, int n_prefix, int aio_piPosition, int *table );
int init_vlc ( VLC_TYPE *vlc, int nb_bits, int nb_codes, const int *bits, const int *codes );
void decode_init_vlc(VLC_TABLES *Vlc);



#endif

