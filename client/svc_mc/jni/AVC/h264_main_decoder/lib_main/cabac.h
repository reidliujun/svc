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
*  Optimized version for Texas Instrument DSP 
*			   GDEM(Grupo de Diseño Electrónico y Microelectrónico)
*			   Universidad Politecnica de Madrid. 
*		       http://www.sec.upm.es/gdem    
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

#ifndef CABAC_H
#define CABAC_H

#include "main_data.h"




#ifdef TI_OPTIM
#define get_cabac_bypass get_cabac_bypass_TI
#define get_cabac get_cabac_TI
#define get_cabac_terminate get_cabac_terminate_TI
#else
#define get_cabac_bypass get_cabac_bypass_c
#define get_cabac get_cabac_c
#define get_cabac_terminate get_cabac_terminate_c
#endif



void init_cabac_states(CABACContext *c, unsigned char (*lps_range)[4],  unsigned char *mps_state, unsigned char *lps_state, int state_count);
void init_cabac_decoder(CABACContext *c, unsigned char *buf, int buf_size);



/**
This function permits to refill the CABAC state when it's became to low.. 
@param c Contains the data concernig the CABAC for decoding the VLC.
*/
static __inline void refill(CABACContext *c){

	//Should be always inferior to the last element
	//if(c -> bytestream <= c -> bytestream_end){
	c -> low += (c -> bytestream[0] << 1);
	
	c -> low -= CABAC_MASK;
	c -> bytestream += (CABAC_BITS >> 3);
}



/**
This function permits to update the CABAC data one time in function of the data previously decoded. 

@param c Contains the data concernig the CABAC for decoding the VLC.
*/
static __inline void renorm_cabac_decoder_once(CABACContext *c){
	int mask = (c -> range - (0x200 << CABAC_BITS) ) >> 31;
	c -> range += c -> range & mask;
	c -> low  += c -> low & mask;
	if(!(c -> low & CABAC_MASK)){
		refill(c);
	}
}


/**
This function permits to update the CABAC data in function of the data previously decoded. 

@param c Contains the data concernig the CABAC for decoding the VLC.
*/
static __inline void renorm_cabac_decoder(CABACContext *c){
	while(c -> range < (0x200 << CABAC_BITS)){
		c -> range += c -> range;
		c -> low += c -> low;
		if(!(c -> low & CABAC_MASK)){
			refill(c);
		}
	}
}


/**
This function permits to decode the VLC and to return the correct bit according to the state. 

@param c Contains the data concernig the CABAC for decoding the VLC.
*/
static __inline  int get_cabac_bypass_c(CABACContext *c){
	c -> low += c -> low;

	if(!(c -> low & CABAC_MASK)){
		refill(c);
	}

	if(c -> low < c -> range){
		return 0;
	}else{
		c -> low -= c -> range;
		return 1;
	}
}




/**
This function permits to determine if the NAL is finished.
@return the number of bytes read or 0 if no end

@param c Contains the data concernig the CABAC for decoding the VLC.
*/
static __inline int get_cabac_terminate_c(CABACContext *c, int iCurrMbAddr, int PicSizeInMbs){

	if ( iCurrMbAddr >= PicSizeInMbs) {
		return 1;
	}

	c -> range -= (4 << CABAC_BITS);
	if(c -> low < c -> range){
		renorm_cabac_decoder_once(c);
		return 0;
	}else{
		return (int )(c -> bytestream - c -> bytestream_start);
	}
}



/**
This function permits to decode the VLC and to return the correct bit according to the state. 

@param c Contains the data concernig the CABAC for decoding the VLC.
@param state State of the current slice.
*/
static __inline int get_cabac_c(CABACContext *c, unsigned char *state){

	int RangeLPS = c -> lps_range[*state][c -> range >> (CABAC_BITS + 7)] << (CABAC_BITS + 1);
	int bit;

	c -> range -= RangeLPS;
	if(c -> low < c -> range){
		bit = (*state) & 1;
		*state = c -> mps_state[*state];
		renorm_cabac_decoder_once(c);
	}else{
		bit = ((*state) & 1)^1;
		c -> low -= c -> range;
		*state = c -> lps_state[*state];
		c -> range = RangeLPS;
		renorm_cabac_decoder(c);
	}
	return bit;
}


#ifdef TI_OPTIM
/**
This part of functions has been optimized by the GDEM-UPM team.
*/
static int get_cabac_bypass_TI(CABACContext *c){
	unsigned short ret;
	unsigned int cond_1, cond_2, cond_3, bytestream_0;
	unsigned char *bytestream, *bytestream_end;
	int range, low, mask;
	double long_temp,long_temp2;

	long_temp2= _memd8((double*)c+99);
	bytestream_end= (unsigned char*)_hi(long_temp2);
	bytestream= (unsigned char*)_lo(long_temp2);
	long_temp= _amemd8(c);
	range= _hi(long_temp);
	low= _lo(long_temp);
	bytestream_0= *bytestream; //LDBU

	ret= 1;
	mask= CABAC_MASK;

	low += low;
	cond_1= (low & CABAC_MASK);
	cond_3= (bytestream > bytestream_end);

	if(!cond_3) bytestream_0<<= 1;
	if( cond_3) bytestream_0= 0;
	if(!cond_3) mask= mask-bytestream_0;
	if(!cond_1) low -= mask;
	if(!cond_1) bytestream+= 1;
	if(!cond_1) _mem4((int*)c+198)= (unsigned int)bytestream;

	cond_2= (low < range);
	if(!cond_2) low -= range;
	if( cond_2) ret= 0;

	//STORES
	_mem4(c)= (unsigned int)low;
	return ret;
}


/****************************************************************************************************
* FUNCTION NAME: get_cabac                                             						    
* "C+instrinsics" version   
*  Optimized version for Texas Instrument DSP                                                         
*****************************************************************************************************/
static __inline int get_cabac_terminate_TI(CABACContext *c, int iCurrMbAddr, int PicSizeInMbs)
{

	if ( iCurrMbAddr >= PicSizeInMbs) {
		return 1;
	} else {
		int range,low,mask;
		unsigned char *bytestream=(unsigned char*)_lo(_memd8((double*)c+99));
		range= _hi(_amemd8(c));
		low= _lo(_amemd8(c));



		range -= (4 << CABAC_BITS);  


		if(low <range){
			mask = (range - (0x200 << CABAC_BITS) ) >> 31;
			range += (range & mask);
			low +=  (low & mask);
			if(!(low & CABAC_MASK))
			{
				low= (unsigned int)((low+((*bytestream)<<1))-CABAC_MASK);
				_mem4((int*)c+198)=(unsigned int)(bytestream+(CABAC_BITS >> 3));
			}
			//Stores
			_memd8(c)= _itod(range,low);
			return 0;
		}
		else	{	
			_mem4((int*)c + 1)=range;
			return (int )(bytestream - c -> bytestream_start);
		}
	}
}



/****************************************************************************************************
* FUNCTION NAME: get_cabac                                             						    
* "C+instrinsics" version   
*  Optimized version for Texas Instrument DSP                                                         
*****************************************************************************************************/
static __inline int get_cabac_TI(CABACContext *c, unsigned char * const state)
{

#define SHL_CABAC_BITS__0x200_	0x20000
#define CABAC_MASK_				0xFF

	int RangeLPS;
	int bit, lps_refill_mult, mask ;
	unsigned int cond_1, cond_2, cond_3, cond_refill, bytestream_0;
	unsigned char lps_state;
	int low_lps;
	unsigned char state_cont;
	unsigned char *bytestream, *bytestream_end;
	int range, low,  loop_lps_count,lmbd_low ;

	//LOADS

	range= _hi(_amemd8(c));
	low= _lo(_amemd8(c));

	state_cont= *state; 

	bytestream_end= (unsigned char*)_hi(_memd8((double*)c+99));
	bytestream= (unsigned char*)_lo(_memd8((double*)c+99));

	RangeLPS= *((unsigned char*)c+12+((state_cont>>1)<<2)+_extu(range,15,30) );

	RangeLPS= RangeLPS<<9;

	lps_state= *((unsigned char*)c+532+state_cont);

	bytestream_0= *bytestream; 
	bytestream_0<<= 1;


	//PROLOG

	bit= (state_cont)&1;
	cond_1= (state_cont<124); 

	low_lps= (low - range) + RangeLPS;
	lmbd_low=8-_lmbd(0x1, _bitr (low - range));

	range -= RangeLPS; 

	loop_lps_count= _lmbd(0x1, RangeLPS)-14;

	//MPS PROLOG & MPS



	cond_2= (low < range);
	mask= (range < SHL_CABAC_BITS__0x200_);
	range= range<<mask; 
	low= low<<mask;     



	if(cond_1) state_cont+= 2;


	//REFILL PROLOG
	lps_refill_mult= 0;
	cond_refill= (low<<24);  
	cond_3= (bytestream > bytestream_end);

	if(!cond_2){		
		bit^= 1;
		lps_refill_mult= (loop_lps_count-lmbd_low); 
		range= RangeLPS<<loop_lps_count;
		low= low_lps<<loop_lps_count;
		state_cont= lps_state;
		cond_refill= ((unsigned int)loop_lps_count<(unsigned int)lmbd_low);
	}

	//REFILL
	if(!cond_3) bytestream_0= bytestream_0<<lps_refill_mult;
	if( cond_3) bytestream_0= 0;
	if(!cond_refill)
	{

		low+= bytestream_0;
		low -= (CABAC_MASK<<lps_refill_mult);
		bytestream+= 1;
	}

	//STORES
	*state= state_cont;
	_memd8(c)= _itod(range,low);
	if(!cond_refill) _mem4((int*)c+198)= (unsigned int)bytestream;

	return bit; 

}

#endif

#endif
