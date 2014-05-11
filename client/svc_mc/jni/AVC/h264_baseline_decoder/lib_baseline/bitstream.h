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

#ifndef HEADER_H
#define HEADER_H

#include "symbol.h"



#define read_ue read_ue_C
#define show32bits show32bits_C 

#ifdef TI_OPTIM
#undef read_ue
#define read_ue read_ue_TI

#undef show32bits
#define show32bits show32bits_TI
#else 
int read_ue ( const unsigned char *  tab,  int *pos );
#endif

#ifdef MMX
#undef show32bits
#define show32bits show32bits_MMX
#endif





extern const char    golomb_to_intra4x4 [48];
extern const char    golomb_to_inter [48];



/**
This function permits to get the n next bits without incrementing the position..

@param tab The NAL unit.
@param position The current aio_piPosition in the NAL.
@param n The number of bits to read (< 9).

*/
static  __inline unsigned int showNbits_9max ( const unsigned char *tab, const int position, const int n )
{
    const unsigned int  pos_bit = position & 0x7 ; /* position courante dans le caractère courant */ 
    const unsigned int  pos_char = position >> 3 ; /* caractère courant dans le tableau */ 
    
    unsigned int  c = (unsigned int)(*(tab + pos_char) << 8) | *(tab + pos_char + 1);
    c = (unsigned int)(c << (pos_bit + 16));
    c = (unsigned int)(c >> (32 - n));

    return (c);
}




/**
This function permits to get the n next bits without incrementing the position..

@param tab The NAL unit.
@param position The current aio_piPosition in the NAL.
@param n The number of bits to read.
*/
static  __inline unsigned int showNbits ( const unsigned char *  tab, const int position, const int n )
{

#ifdef TI_OPTIM

	const int       pos_bit = position & 0x7 ; // position courante dans le caractère courant 
    const int       pos_char = position >> 3 ; // caractère courant dans le tableau  
    unsigned int    res;
    
    res = _mem4((void *)(tab + pos_char));
    res = _packlh2(res, res);
    res = _swap4(res);
    res = _extu(res, pos_bit, 32 - n);

    return (res);
#else 
    
    const unsigned int  pos_bit = position & 0x7 ; /* position courante dans le caractère courant */ 
    const unsigned int  pos_char = position >> 3 ; /* caractère courant dans le tableau */ 
    unsigned int        c;
    
    c = (unsigned int)(*(tab + pos_char) << 24) | *(tab + pos_char + 1) << 16 | *(tab + pos_char + 2) << 8 | *(tab + pos_char + 3);
    c = (unsigned int)(c << pos_bit);
    c = (unsigned int)(c >> (32 - n));

    return (c);
#endif
}

/**
This function permits to get the 3 next bytes without incrementing the position.

@param tab The NAL unit.
@param position The current aio_piPosition in the NAL.
@param n The number of bits to read.
*/
static  __inline unsigned int show3Bytes_Nal ( const unsigned char *tab, const int pos_char){
#ifdef TI_OPTIM
    unsigned int    res ;
    
    res = _mem4((void *)(tab + pos_char));
    res = _packlh2(res, res);
    res = _swap4(res);
    res = (unsigned int)(res >> 8);

    return (res);

#else
    unsigned int    c = (*(tab + pos_char) << 16 | *(tab + pos_char + 1) << 8 | *(tab + pos_char + 2)) & 0xffffff ;
    
    return (c);
#endif
}




/**
This function permits to get the n next bits.

@param tab The NAL unit.
@param position The current aio_piPosition in the NAL.
@param n The number of bits to read.
*/
static  __inline unsigned int getNbits ( const unsigned char *tab,  int *position, const int n )
{
    
    unsigned int tmp = showNbits(tab, *position, n);
    *position += n ;
    return tmp;
}


static  __inline int show32bits_C ( const unsigned char *  tab,  const int position )
{

    
    unsigned int tmp_msb = showNbits(tab, position, 16);
    unsigned int tmp_lsb = showNbits(tab, position + 16, 16);
    unsigned int c = (tmp_msb << 16 | tmp_lsb) & 0xffffffff ;

    return c ;
}

/**
This function permits to get the n next bits without incrementing the position..

@param tab The NAL unit.
@param position The current aio_piPosition in the NAL.
@param n The number of bits to read.
*/
#ifdef MMX
static  __inline __m64 showNbits_64 (const unsigned char *tab, const int position, const int n)
{
    const unsigned int  pos_bit = position & 0x7 ; /* position courante dans le caractère courant */ 
    const unsigned int  pos_char = position >> 3 ; /* caractère courant dans le tableau */ 

#ifdef POCKET_PC
	unsigned char A,B,C,D,E,F,G,H;
	__m64 res64;

	A = *(tab + pos_char);
	B = *(tab + pos_char + 1);
	C = *(tab + pos_char + 2);
	D = *(tab + pos_char + 3);
	E = *(tab + pos_char + 4);
	F = *(tab + pos_char + 5);
	G = *(tab + pos_char + 6);
	H = *(tab + pos_char + 7);
    
	res64 = _mm_setr_pi8 (H,G,F,E,D,C,B,A);
	res64 = _mm_slli_si64(res64, pos_bit);
    res64 = _mm_srli_si64(res64, (64 - n));
    
#else
	__m64 res64 = _mm_setr_pi8 (tab[pos_char + 7],tab[pos_char + 6],tab[pos_char + 5],tab[pos_char + 4],tab[pos_char + 3],tab[pos_char + 2],tab[pos_char + 1],tab[pos_char]);
	
	res64 = _mm_slli_si64(res64, pos_bit);
    res64 = _mm_srli_si64(res64, (64 - n));
#endif
	empty();

	return (res64);
}





static  __inline int show32bits_MMX ( const unsigned char *  tab,  const int position )
{

	int result = _mm_cvtsi64_si32 (showNbits_64 ( tab, position, 32));

	empty();

	return result;
}


#endif



#ifdef TI_OPTIM
static  __inline int show32bits_TI ( const unsigned char *  tab,  const int position )
{



	const int       pos_bit = position & 0x7 ; // position courante dans le caractère courant 
    const int       pos_char = position >> 3 ; // caractère courant dans le tableau 

	unsigned int high;
	unsigned int low;
	double p = _memd8((void*)(tab+pos_char));	
	high = _hi (p);
	low = _lo (p);

    low = _packlh2(low, low);
    high = _packlh2(high, high);
    low = _swap4(low);
    high = _swap4(high);

    low = (unsigned int) (low << pos_bit);
    high = (unsigned int) (high >> (32-pos_bit));
	low = low + high;

    return low ;
}


static __inline int read_ue_TI ( const unsigned char *  tab,  int *pos )
{

	int buf = showNbits(tab, *pos, 24);
	int leadingzeroBits = _norm(buf) - 7;

	if(!leadingzeroBits)
	{
		*pos += 1;
		return 0;
	}
	else if(leadingzeroBits < 12)
	{
		*pos += (2 * leadingzeroBits) + 1;
		buf >>= (23 - (leadingzeroBits << 1));

		return(buf - 1);
	}
	else
	{
		// we calculate another time leadingzeroBits this time up to 32 bits
		int b = 0 ;
		int leadingzeroBits = 0 ;
		b = getNbits(tab, pos, 1);
	    
		for (  ; !b ; leadingzeroBits++ ) {
			b = getNbits(tab, pos, 1);
		}

		return leadingzeroBits? (1 << leadingzeroBits) - 1 + getNbits(tab, pos, leadingzeroBits):0 ;
	}
}

#endif




/**
This function permits to get the 32 next bits.

@param tab The NAL unit.
@param position The current aio_piPosition in the NAL.
*/
static  __inline unsigned int get32bits ( const unsigned char *  tab,  int *position )
{
    
    int tmp = show32bits(tab, *position);
    *position += 32 ;
    return ((unsigned int)tmp) ;
}





/**
This function permits to decode the coded block pattern. 

me(v): mapped integer Exp-Golomb-coded syntax element with the left bit first.

@param data The NAL unit.
@param aio_piPosition The current aio_piPosition in the NAL.
@param mode The prediction mode of the macroblock.

*/
static __inline int read_me ( const unsigned char *tab,  int *pos, int mode )
{
    int val = read_ue(tab, pos);
    
    if ( mode < INTRA_PCM) 
        return golomb_to_intra4x4 [val];
    else 
        return golomb_to_inter [val];

}


/**
This function permits to decode an Exp-Golomb-coded syntax;

se(v): signed integer Exp-Golomb-coded syntax element with the left bit first. 

@param tab The NAL unit.
@param position The current aio_piPosition in the NAL.
*/
static __inline int read_se ( const unsigned char *  tab,  int *pos )
{
    int val = read_ue(tab, pos);
    return val & 0x01 ? ((val >> 1) + 1) : -(val >> 1);
}





/**
This function permits to return the logg_base2 of a number .

// for a > 0
*/

static __inline int log_base2 ( int a )
{
#ifdef TI_OPTIM
    return (31-_norm(a));
#else
    int n ;
    
    a-- ;
    n = 0 ;
    while ( a > 0 ) {
        a = a >> 1 ;
        n++ ;
    }
    return (n);
#endif
}




/**
This function permits to decode an Exp-Golomb-coded syntax;

te(v): truncated Exp-Golomb-coded syntax element with left bit first

@param tab The NAL unit.
@param position The current aio_piPosition in the NAL.
*/
static __inline int read_te ( const unsigned char *  tab,  int *pos, int x )
{
    if ( x == 1 ) {
        return 1 - getNbits(tab, pos, 1);
    } else if ( x > 1 ) {
        return read_ue(tab, pos);
    }
    return 0 ;
}


/**
This function allows to determine if the position is aligned on a byte.


@param position The current aio_piPosition in the NAL.
*/
static __inline int bytes_aligned (  int position )
{
    return position & 0x07 ? 0 : 1 ;
}



/**
This function allows to finish the Nal in order to be bytes aligned.


@param position The current aio_piPosition in the NAL.
*/
static __inline void rbsp_trailing_bits (int *position )
{
    
	int n;
	(*position)++;

	if(!bytes_aligned(*position))
	{	n = (8 -((*position) & 0x7));
		(*position) += n;
	}
}






#endif
