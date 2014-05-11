

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

#ifndef RESIDUAL_PROCESS_H
#define RESIDUAL_PROCESS_H

//AVC Files
#include "clip.h"

//SVC Files
#include "svc_type.h"

//Prototypes
void base_layer_residual(RESIDU *residu, short *Luma, short *Chroma_Cb, short *Chroma_Cr, const short PicWidthInPix, const PPS *Pps, const  W_TABLES *quantif);
void AddResidualPic(const NAL *Nal,const PPS * Pps  , RESIDU *residu, RESIDU *BaseResidu,const short PicWidthInMbs , const W_TABLES *quantif, 
					unsigned char *Y,unsigned char *U, unsigned char *V, short *Residu_img_lum, short *Residu_img_Cb, short *Residu_img_Cr);

void AddResidualRes(const NAL *Nal, const PPS *Pps, RESIDU *CurrResidu, RESIDU *BaseResidu, const short PicWidthInPix, 
					const W_TABLES *quantif, short *Residu_Luma, short *Residu_Cb, short *Residu_Cr);
void AddResidual(const NAL *Nal, const PPS * Pps, RESIDU *CurrResidu, RESIDU *BaseResidu, const short PicWidthInPix, 
				 const W_TABLES *quantif, unsigned char *Y,unsigned char *U, unsigned char *V, 
				 short *Residu_Luma, short *Residu_Cb, short *Residu_Cr);

void SCoeffPic(RESIDU *residu, const RESIDU *BaseResidu, unsigned char *Y, unsigned char *Cb, unsigned char *Cr,
			   const short PicWidthInPix, const PPS *Pps, const W_TABLES *quantif);


#ifdef TI_OPTIM
#define add_compensed_residual add_compensed_residual_TI
#define ResetMbResidu ResetMbResidu_TI
#define AddCoeff AddCoeff_TI
#define AddCoeff64 AddCoeff64_TI
#define AddCoeff256 AddCoeff256_TI
#else
#define add_compensed_residual add_compensed_residual_C
#define ResetMbResidu ResetMbResidu_C
#define AddCoeff AddCoeff_C
#define AddCoeff64 AddCoeff64_C
#define AddCoeff256 AddCoeff256_C
#endif


#ifdef TI_OPTIM
//Definitions

/**
Set the residual picture to zero for the current macroblock
*/
static __inline void ResetMbResidu_TI(short *Residu, short *Residu_Cb, short *Residu_Cr,const short PicWidthInPix)
{
	int i;
	int PicWidthInPixCr=PicWidthInPix>>1;

	for ( i = 0; i < 16; i++){
		_mem8(Residu+ i * PicWidthInPix+0 ) = 0;
		_mem8(Residu+ i * PicWidthInPix+4 ) = 0;
		_mem8(Residu+ i * PicWidthInPix+8 ) = 0;
		_mem8(Residu+ i * PicWidthInPix+12) = 0;
		//for ( j = 0; j < 4; j++){
		//Residu[j + i * PicWidthInPix] = 0;
		//}
	}

	for ( i = 0; i < 8; i++){
		_mem8(Residu_Cb+ i * PicWidthInPixCr+0 ) = 0;
		_mem8(Residu_Cb+ i * PicWidthInPixCr+4 ) = 0;
		_mem8(Residu_Cr+ i * PicWidthInPixCr+0 ) = 0;
		_mem8(Residu_Cr+ i * PicWidthInPixCr+4 ) = 0;

		//for ( j = 0; j < 8; j++){
		//	Residu_Cb[j + i * (PicWidthInPix >> 1)] = Residu_Cr[j + i * (PicWidthInPix >> 1)] = 0;
		//}
	}
}



static __inline void AddCoeff_TI(short *Coeff, const short *BaseLumaCoeff){
	int i;

	for ( i = 0; i < 16; i=i+2){
		_mem4(Coeff+ i) = _add2(_mem4(Coeff+ i), _mem4((void *)(BaseLumaCoeff+i)));
	}
}



static __inline void AddCoeff64_TI(short *Coeff, short *BaseLumaCoeff){
	int i;

	for ( i = 0; i < 64; i=i+2){
		_mem4(Coeff+ i) = _add2(_mem4(Coeff+ i), _mem4(BaseLumaCoeff+i));
	}
}

static __inline void AddCoeff256_TI(short *Coeff, short *BaseLumaCoeff){
	int i;

	for ( i = 0; i < 256; i=i+2){
		_mem4(Coeff+ i) = _add2(_mem4(Coeff+ i), _mem4(BaseLumaCoeff+i));
	}
}

static __inline void  add_compensed_residual_TI(unsigned char *Y, unsigned char *U, unsigned char *V, short *luma_residu, 
												short *Cb_residu, short *Cr_residu, const int PicWidthInPix)
{

	int i;
	unsigned int A,B,C,D;
	unsigned int PicWidthInPixCr=PicWidthInPix>>1;

	for ( i = 0; i < 16; i++){
		A=_mem4(Y+i*PicWidthInPix);
		B=_mem4(Y+i*PicWidthInPix+4);
		C=_mem4(Y+i*PicWidthInPix+8);
		D=_mem4(Y+i*PicWidthInPix+12);
		_mem4(Y+i*PicWidthInPix+0 )=_spacku4(_add2(_unpkhu4(A),_mem4(luma_residu+i * PicWidthInPix+2 )) ,_add2(_unpklu4(A),_mem4(luma_residu+i * PicWidthInPix+0 )) );
		_mem4(Y+i*PicWidthInPix+4 )=_spacku4(_add2(_unpkhu4(B),_mem4(luma_residu+i * PicWidthInPix+6 )) ,_add2(_unpklu4(B),_mem4(luma_residu+i * PicWidthInPix+4 )) );
		_mem4(Y+i*PicWidthInPix+8 )=_spacku4(_add2(_unpkhu4(C),_mem4(luma_residu+i * PicWidthInPix+10)) ,_add2(_unpklu4(C),_mem4(luma_residu+i * PicWidthInPix+8 )) );
		_mem4(Y+i*PicWidthInPix+12)=_spacku4(_add2(_unpkhu4(D),_mem4(luma_residu+i * PicWidthInPix+14)) ,_add2(_unpklu4(D),_mem4(luma_residu+i * PicWidthInPix+12)) );

		//for ( j = 0; j < 16; j++){
		//	Y [i*PicWidthInPix + j] = (unsigned char ) CLIP255(Y[i * PicWidthInPix + j] + luma_residu[i * PicWidthInPix + j]) ;
		//}
	}

	for ( i = 0; i < 8; i++){
		A=_mem4(U+i*PicWidthInPixCr);
		B=_mem4(U+i*PicWidthInPixCr+4);
		_mem4(U+i*PicWidthInPixCr+0 )=_spacku4(_add2(_unpkhu4(A),_mem4(Cb_residu+i * PicWidthInPixCr+2 )) ,_add2(_unpklu4(A),_mem4(Cb_residu+i * PicWidthInPixCr+0 )) );
		_mem4(U+i*PicWidthInPixCr+4 )=_spacku4(_add2(_unpkhu4(B),_mem4(Cb_residu+i * PicWidthInPixCr+6 )) ,_add2(_unpklu4(B),_mem4(Cb_residu+i * PicWidthInPixCr+4 )) );

		C=_mem4(V+i*PicWidthInPixCr);
		D=_mem4(V+i*PicWidthInPixCr+4);
		_mem4(V+i*PicWidthInPixCr+0 )=_spacku4(_add2(_unpkhu4(C),_mem4(Cr_residu+i * PicWidthInPixCr+2 )) ,_add2(_unpklu4(C),_mem4(Cr_residu+i * PicWidthInPixCr+0 )) );
		_mem4(V+i*PicWidthInPixCr+4 )=_spacku4(_add2(_unpkhu4(D),_mem4(Cr_residu+i * PicWidthInPixCr+6 )) ,_add2(_unpklu4(D),_mem4(Cr_residu+i * PicWidthInPixCr+4 )) );

		//for ( j = 0; j < 8; j++){
		//	U [i * (PicWidthInPix >> 1) + j] = (unsigned char ) CLIP255(U[i * (PicWidthInPix >> 1) + j] + Cb_residu[i*(PicWidthInPix >> 1) + j]) ;
		//	V [i * (PicWidthInPix >> 1) + j] = (unsigned char ) CLIP255(V[i * (PicWidthInPix >> 1) + j] + Cr_residu[i*(PicWidthInPix >> 1) + j]) ;
		//}    
	}
}

#endif



/**
Set the residual picture to zero for the current macroblock
*/
static __inline void ResetMbResidu_C(short *Residu, short *Residu_Cb, short *Residu_Cr,const short PicWidthInPix)
{
	int i,j;

	for ( i = 0; i < 16; i++){
		for ( j = 0; j < 16; j++){
			Residu[j + i * PicWidthInPix] = 0;
		}
	}

	for ( i = 0; i < 8; i++){
		for ( j = 0; j < 8; j++){
			Residu_Cb[j + i * (PicWidthInPix >> 1)] = Residu_Cr[j + i * (PicWidthInPix >> 1)] = 0;
		}
	}
}

/**
Add residual to the prediction in order to get the picture
*/
static __inline void  add_compensed_residual_C(unsigned char *Y, unsigned char *U, unsigned char *V, short *luma_residu, 
											   short *Cb_residu, short *Cr_residu, const int PicWidthInPix)
{

	int i,j;

	for ( i = 0; i < 16; i++){
		for ( j = 0; j < 16; j++){
			Y [i*PicWidthInPix + j] = (unsigned char ) CLIP255(Y[i * PicWidthInPix + j] + luma_residu[i * PicWidthInPix + j]) ;
		}
	}

	for ( i = 0; i < 8; i++){
		for ( j = 0; j < 8; j++){
			U [i * (PicWidthInPix >> 1) + j] = (unsigned char ) CLIP255(U[i * (PicWidthInPix >> 1) + j] + Cb_residu[i*(PicWidthInPix >> 1) + j]) ;
			V [i * (PicWidthInPix >> 1) + j] = (unsigned char ) CLIP255(V[i * (PicWidthInPix >> 1) + j] + Cr_residu[i*(PicWidthInPix >> 1) + j]) ;
		}    
	}
}




/**
Allow to add 16 coefficents.
Used for Scoeff prediction
*/
static __inline void AddCoeff_C(short *Coeff, const short *BaseLumaCoeff){
	int i;

	for ( i = 0; i < 16; i++){
		Coeff [i] = Coeff [i] + BaseLumaCoeff[i];
	}
}

/**
Allow to add 64 coefficents.
Used for Scoeff prediction
*/
static __inline void AddCoeff64_C(short *Coeff, short *BaseLumaCoeff){
	int i;

	for ( i = 0; i < 64; i++){
		Coeff [i] = Coeff [i] + BaseLumaCoeff[i];
	}
}



/**
Allow to add 256 coefficents.
Used for Scoeff prediction
*/
static __inline void AddCoeff256_C(short *Coeff, short *BaseLumaCoeff){
	int i;

	for ( i = 0; i < 256; i++){
		Coeff [i] = Coeff [i] + BaseLumaCoeff[i];
	}
}
#endif
