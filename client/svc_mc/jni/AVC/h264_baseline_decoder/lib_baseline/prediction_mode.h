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


#ifndef PRED_MODE_8x8_H
#define PRED_MODE_8x8_H


#include "type.h"
#include "data.h"
#include "clip.h"


// 16x16 DC derivation functions
void predict_16x16_dci (unsigned char * predict_sample,	const short PicWidthInPix, int Mask);

// 4x4 DC derivation functions
void predict_4x4_dci  (unsigned char * predict_sample, const short PicWidthInPix, int Mask, const short locx, const short locy);
void predict_4x4_ddli (unsigned char * predict_sample, const short PicWidthInPix, int Mask, const short locx, const short locy);
void predict_4x4_vli  (unsigned char * predict_sample, const short PicWidthInPix, int Mask, const short locx, const short locy);

// Chroma DC derivation function
void predict_chr_dci (unsigned char * predict_sample, const short PicWidthInPix, int Mask);


//-------------------- 4x4
/**
This function allows to do the 4x4 luminance vertical interpolation.

@param predict_sample The sample to be decoded
@param PicWidthInPix Width in pixel of the image.
@param macro Specifies the availability of the neighbouring macroblock.
@param locx Location in x in the current macroblock.
@param locy Location in y in the current macroblock.
*/
static __inline void predict_4x4_v ( unsigned char * RESTRICT sample, const short PicWidthInPix, int Mask, const short locx, const short locy)
{
#ifdef TI_OPTIM
	unsigned int ui_A;
	const unsigned int cui_B = (PicWidthInPix << 1);

	ui_A = _amem4_const(sample-PicWidthInPix);			// Load A,B,C,D in ui_A
	_amem4(sample) = ui_A;								// Copy A,B,C,D on I and J line
	_amem4(sample+PicWidthInPix) = ui_A;		
	_amem4(sample+cui_B) = ui_A;						// Copy A,B,C,D on K and L line
	_amem4(sample+cui_B+PicWidthInPix) = ui_A;
#else
    int  j ;  
	unsigned char uc1 = sample [- PicWidthInPix];
	unsigned char uc2 = sample [1 - PicWidthInPix];
	unsigned char uc3 = sample [2 - PicWidthInPix];
	unsigned char uc4 = sample [3 - PicWidthInPix];


	for ( j = 0 ; j < 4; j++, sample += PicWidthInPix){
		sample [0] = uc1;
		sample [1] = uc2;
		sample [2] = uc3;
		sample [3] = uc4;
	}
#endif
}




/**
This function allows to do the 4x4 horizontal interpolation.

@param predict_sample The sample to be decoded
@param PicWidthInPix Width in pixel of the image.
@param macro Specifies the availability of the neighbouring macroblock.
@param locx Location in x in the current macroblock.
@param locy Location in y in the current macroblock.
*/
static __inline void predict_4x4_h ( unsigned char * RESTRICT sample, const short PicWidthInPix , int Mask, const short locx, const short locy)
{
#ifdef TI_OPTIM
	unsigned char *  puc_tempA = &sample[0];
	unsigned char uc_TempA;
	unsigned int cui_B=(PicWidthInPix<<1);
	unsigned int ui_TempA;

	uc_TempA= *(puc_tempA-1);
	ui_TempA= uc_TempA+(uc_TempA<<8)|(uc_TempA<<16)+(uc_TempA<<24);
	_amem4( puc_tempA )= ui_TempA;
	// Copy the pixel J on all the others pixels of the line
	puc_tempA = sample+PicWidthInPix;
	uc_TempA= *(puc_tempA-1);
	ui_TempA= uc_TempA+(uc_TempA<<8)|(uc_TempA<<16)+(uc_TempA<<24);
	_amem4( puc_tempA )= ui_TempA;
	// Copy the pixel K on all the others pixels of the line
	puc_tempA = (sample+cui_B);
	uc_TempA= *(puc_tempA-1);
	ui_TempA= uc_TempA+(uc_TempA<<8)|(uc_TempA<<16)+(uc_TempA<<24);
	_amem4( puc_tempA )= ui_TempA;
	// Copy the pixel L on all the others pixels of the line
	puc_tempA = sample+cui_B+PicWidthInPix;
	uc_TempA= *(puc_tempA-1);
	ui_TempA= uc_TempA+(uc_TempA<<8)|(uc_TempA<<16)+(uc_TempA<<24);
	_amem4( puc_tempA )= ui_TempA;

#else
    int       j ;
    for ( j = 0 ; j < 4 ; j++, sample += PicWidthInPix ) {
        sample[0] = sample[1] = sample[2] = sample[3] = sample [- 1];
    }
#endif
}







/**
This function allows to do the 4x4 DC interpolation.

@param predict_sample The sample to be decoded
@param macro Specifies the availability of the neighbouring macroblock.
@param left The left block of the current one.
@param top The top block of the current one.
@param PicWidthInPix Width in pixel of the image.
*/
static __inline void predict_4x4_dc ( unsigned char sample [RESTRICT], NEIGHBOUR * RESTRICT left, NEIGHBOUR * RESTRICT top,	const short PicWidthInPix)
{
#ifdef TI_OPTIM
	unsigned char* puc_HorLine= &sample [-1]; // pui_HorLine points on A
	unsigned int ui_tempA,ui_tempB,ui_tempC;
	const unsigned int cui_A=(PicWidthInPix<<1);
	unsigned char s0 =128 ;


	// Load I,J,K,L 		// ui_tempB = I+J+K+L+2
	ui_tempB =	*(puc_HorLine)+ *(puc_HorLine+PicWidthInPix)+
				*(puc_HorLine+cui_A) + *(puc_HorLine+cui_A+PicWidthInPix) + 2;
	// Load A,B,C,D (4*Bytes) in ui_tempB 
	puc_HorLine = &sample [- PicWidthInPix];
	ui_tempC = _amem4_const(puc_HorLine);			// A B | C D
	ui_tempA = _dotpu4(ui_tempC,0x01010101)+2;		// ui_tempA = A+B+C+D+2

	if ( top -> Avail) {
		if ( left -> Avail){
			s0 = ((ui_tempA + ui_tempB) >> 3);		// (A+B+C+D+2 + I+J+K+L+2)/ 8
		}else{
			s0 = (ui_tempA >> 2);					// (A+B+C+D+2)/ 4
		}
    } 
	else if ( left -> Avail ){
			s0 = (ui_tempB>> 2);					// (I+J+K+L+2)/ 4
    } 
	// Duplicates s0 on each byte of ui_tmp_A : 0xs0,s0,s0,s0
	ui_tempA=0x01010101*s0;
	puc_HorLine=&sample [0];
	// Copy ui_tempA on the lines
	_amem4(puc_HorLine) = ui_tempA;
	_amem4(puc_HorLine + PicWidthInPix) = ui_tempA;
	_amem4(puc_HorLine + cui_A) = ui_tempA;
	_amem4(puc_HorLine + cui_A + PicWidthInPix) = ui_tempA;
#else

    unsigned char    s0 = 128 ;
    int    j ;

	if ( top -> Avail) {
		if ( left -> Avail){
			// no edge
			s0 = ( sample [- PicWidthInPix]	+ sample [1 - PicWidthInPix] + sample [2 - PicWidthInPix]	+ sample [3 - PicWidthInPix]
					+ sample [-1]			+ sample [PicWidthInPix - 1] + sample [2*PicWidthInPix - 1]	+ sample [3*PicWidthInPix - 1]	+ 4 ) >> 3 ;
		}else{
			 s0 = ( sample [- PicWidthInPix] + sample [- PicWidthInPix + 1] + sample [- PicWidthInPix + 2]   + sample [- PicWidthInPix + 3] + 2 ) >> 2 ;
		}
    } 
	else if ( left -> Avail ) 
	{
        // upper edge
        s0 = ( sample [-1] + sample [-1 + PicWidthInPix] + sample [-1 + 2*PicWidthInPix] + sample [-1 + 3*PicWidthInPix] + 2 ) >> 2 ;
    } 

    for ( j = 0 ; j < 4 ; j++, sample += PicWidthInPix ) {
        // store DC sample
  		sample [0] = sample [1] = sample [2] = sample [3] = s0;
    }
#endif
}







/**
This function allows to do the 4x4 diagonal down left interpolation.

@param predict_sample The sample to be decoded
@param macro Specifies the availability of the neighbouring macroblock.
@param top_right The top_right block of the current one.
@param PicWidthInPix Width in pixel of the image.
*/
static __inline void predict_4x4_ddl( unsigned char sample [RESTRICT], NEIGHBOUR * RESTRICT top_right, const short PicWidthInPix)
{
#ifdef TI_OPTIM
	// pui_HorLine points on A
	unsigned char	uc_C,uc_D;
	unsigned char	uc_a,uc_be,uc_ifc,uc_dgjm,uc_ol,uc_nkh ,uc_p;	// I line
	unsigned int	cui_EFGH;
	{
		const unsigned long long cull_A = _mem8_const(sample-PicWidthInPix); // Load A B | C D | E F | G H
		unsigned int cui_ABCD;
		cui_ABCD = (unsigned int) cull_A;
		cui_EFGH = (unsigned int)(cull_A>>32);
		uc_C = cui_ABCD>>16;
		uc_D = cui_ABCD>>24;
		uc_a = (unsigned char) ((_dotpu4(cui_ABCD,0x00010201)+2)>>2);	//uc_a=(uc_A + (uc_B<<1) + uc_C + 2 )>> 2);
		uc_be= (unsigned char) ((_dotpu4(cui_ABCD,0x01020100)+2)>>2); //uc_be=(uc_B + (uc_C<<1) + uc_D + 2 )>> 2) ;	
	}
	if ( !top_right -> Avail ) 	// If E F G H are NOT available
	{
		const unsigned short cus_tmpA= (uc_D<<2) + 2;
		uc_ifc = ( uc_C + cus_tmpA - uc_D)>>2;		// = uc_C+3*uc_D+2
		uc_nkh=uc_ol=uc_dgjm=uc_p= cus_tmpA>>2;		// = 4*uc_D+2
	}
	else	// If E F G H are available first equation of pred4x4L[x,y]
	{		
		const unsigned char	uc_E = cui_EFGH;
		const unsigned char	uc_F = cui_EFGH>>8;
		unsigned short	us_EDp;

		us_EDp	= _extu(cui_EFGH,24,24)+uc_D+2;			// us_EDp = E+D+2
		uc_ol	=(_dotpu4(cui_EFGH,0x01020100)+2)>>2;	// uc_ol = ( uc_F + (uc_G<<1) + uc_H + 2 ) >> 2;
		uc_p	=(_dotpu4(cui_EFGH,0x03010000)+2)>>2;	// uc_p = ( uc_G + (uc_H<<1) + uc_H + 2 ) >> 2 ;
		uc_nkh	=(_dotpu4(cui_EFGH,0x00010201)+2)>>2;	// uc_nkh = ( uc_E + (uc_F<<1) + uc_G + 2 ) >> 2;
		uc_ifc	=( uc_C + uc_D + us_EDp ) >> 2;			// uc_ifc	=(uc_C + uc_D + us_EDp)>> 2;
		uc_dgjm =( uc_E + uc_F + us_EDp ) >> 2;			// uc_dgjm =(us_EDp + uc_E + uc_F)>> 2;
	}
//	uc_i = uc_f = uc_c ; //	uc_d = uc_g = uc_j = uc_m; //	uc_o = uc_l; //	uc_n = uc_k = uc_h;
	{	// Store the data in amemory
		unsigned char* puc_HorLine= &sample [0];
		const unsigned int cui_A=(PicWidthInPix<<1);
		unsigned int ui_A;
		// Load the I line
		ui_A = (uc_be) | (uc_ifc<<8) + (uc_dgjm<<16);
		_amem4(puc_HorLine) = ( uc_a + (ui_A<<8) );		// = ( uc_a + (uc_be<<8) | (uc_ifc<<16) + (uc_dgjm<<24) )
		// Load the J line
		_amem4(puc_HorLine+PicWidthInPix) = ( ui_A + (uc_nkh<<24) );
		// Load the K line
		ui_A = (uc_dgjm) | (uc_nkh<<8) + (uc_ol<<16);
		_amem4(puc_HorLine+cui_A) = ( uc_ifc + (ui_A<<8) );
		// Load the L line
		_amem4(puc_HorLine+cui_A+PicWidthInPix) = ( ui_A + (uc_p<<24) );
	}
#else
	const unsigned char ucTop0 = sample [- PicWidthInPix];
	const unsigned char ucTop1 = sample [1 - PicWidthInPix];
	const unsigned char ucTop2 = sample [2 - PicWidthInPix];
	const unsigned char ucTop3 = sample [3 - PicWidthInPix];

	sample [0] = ( ucTop0 + 2 * ucTop1 + ucTop2 + 2 ) >> 2 ;
	sample [PicWidthInPix] =  sample [1] = ( ucTop1 + 2 * ucTop2 + ucTop3 + 2 ) >> 2 ;

    if ( !top_right -> Avail ) 
	{
			sample [2*PicWidthInPix] = sample [PicWidthInPix+1] = sample [2] = ( ucTop2 + 3 * ucTop3 + 2 ) >> 2 ;

			sample [3] = sample [PicWidthInPix + 2] = sample [PicWidthInPix + 3] = 
			sample [2*PicWidthInPix + 1] = sample [2*PicWidthInPix + 2] = 
			sample [2*PicWidthInPix + 3] = sample [3*PicWidthInPix] =
            sample [3*PicWidthInPix + 1] = sample [3*PicWidthInPix + 2] = 
			sample [3*PicWidthInPix + 3] =( 4*ucTop3 + 2 ) >> 2;
	}
	else
	{		
		int top_right_ind = - PicWidthInPix + 4;
		const unsigned char ucTopRight0 = sample [top_right_ind++];
		const unsigned char ucTopRight1 = sample [top_right_ind++];
		const unsigned char ucTopRight2 = sample [top_right_ind++];
		const unsigned char ucTopRight3 = sample [top_right_ind];

		sample [2*PicWidthInPix] = sample [PicWidthInPix + 1] = sample [2] = ( ucTop2 + ucTopRight0 + 2 * ucTop3 + 2 ) >> 2 ;

	 	sample [3] = sample [PicWidthInPix + 2] = sample [2*PicWidthInPix + 1] = 
		sample [3*PicWidthInPix] = ( ucTop3 + ucTopRight1 + 2 * ucTopRight0 + 2 ) >> 2 ;

		sample [3*PicWidthInPix + 1] = sample [2*PicWidthInPix + 2] = 
		sample [PicWidthInPix + 3] = ( ucTopRight0 + ucTopRight2 + 2 * ucTopRight1 + 2 ) >> 2 ;

		sample [3*PicWidthInPix + 2] = sample [2*PicWidthInPix + 3] = ( ucTopRight1 + ucTopRight3 + 2 * ucTopRight2 + 2 ) >> 2 ;

	 	sample [3*PicWidthInPix + 3] = ( ucTopRight2 + 3 * ucTopRight3 + 2 ) >> 2 ;
	}
 
#endif
}








/**
This function allows to do the 4x4 diagonal down right interpolation.

@param predict_sample The sample to be decoded
@param PicWidthInPix Width in pixel of the image.
@param macro Specifies the availability of the neighbouring macroblock.
@param locx Location in x in the current macroblock.
@param locy Location in y in the current macroblock.
*/
static __inline void predict_4x4_ddr( unsigned char * RESTRICT sample, const short PicWidthInPix, int Mask, const short locx, const short locy)
{
#ifdef TI_OPTIM
	unsigned char * puc_HorLine = &sample [-PicWidthInPix];	
	const unsigned short cus_tmpA=(PicWidthInPix<<1);
	const unsigned char uc_I = sample [-1];							// I
	const unsigned char uc_J = sample [PicWidthInPix - 1];			// J
	const unsigned char uc_K = sample [cus_tmpA - 1];				// K
	const unsigned char uc_L = sample [cus_tmpA+PicWidthInPix - 1];	// L
	const unsigned char uc_M = sample [-PicWidthInPix-1];			// M

	unsigned char uc_A,uc_B,uc_C,uc_D;

	unsigned char uc_afkp,uc_bgl,uc_ch,uc_d;
	unsigned char uc_eoj;
	unsigned char uc_m,uc_ni;
	{	// Load A B C D
		const unsigned int ui_ABCD= _amem4_const(puc_HorLine);
		uc_A = _extu(ui_ABCD,24,24);//ui_ABCD;
		uc_B = _extu(ui_ABCD,16,24);//ui_ABCD>>8;
		uc_C = _extu(ui_ABCD,8,24);//ui_ABCD>>16;
		uc_D = _extu(ui_ABCD,0,24);//ui_ABCD>>24;
	}
	{
	  	const unsigned short us_IJ= (uc_I + uc_J +1);
		const unsigned short us_KL= (uc_K + uc_L +1);
		const unsigned short us_MI= (uc_M + uc_I +1);
		const unsigned short us_JK= (uc_J + uc_K +1);
	  	const unsigned short us_AB= (uc_A + uc_B +1);
		const unsigned short us_CD= (uc_C + uc_D +1);
		const unsigned short us_MA= (uc_M + uc_A +1);
		const unsigned short us_BC= (uc_B + uc_C +1);
		// Case x=y
		uc_afkp	= (us_MA + us_MI) >>2;			//	(A+2*M+I+2)>>2
		// Case x<y && x!=y
		uc_eoj	= (us_MI + us_IJ) >>2;			//	(M+2*I+J+2)>>2
		uc_m	= (us_JK + us_KL) >>2;			//	(J+2*K+L+2)>>2
		uc_ni	= (us_IJ + us_JK) >>2;			//	(I+2*J+K+2)>>2
		// Case x>y && x!=y
		uc_bgl	= (us_MA + us_AB) >>2;			//	(M+2*A+N+2)>>2
		uc_d	= (us_BC + us_CD) >>2;			//	(B+2*C+D+2)>>2
		uc_ch	= (us_AB + us_BC) >>2;			//	(A+2*B+C+2)>>2
	}
	{
		unsigned int ui_A;
		puc_HorLine = &sample [0];
		// Load the I line
		ui_A = uc_afkp + (uc_bgl<<8) | (uc_ch<<16);
		_amem4(puc_HorLine) = ( ui_A + (uc_d<<24) );
		// Load the J line
		_amem4(puc_HorLine+PicWidthInPix) = ( uc_eoj + (ui_A<<8) );
		// Load the K line
		ui_A = uc_ni + (uc_eoj<<8) | (uc_afkp<<16);
		_amem4(puc_HorLine+cus_tmpA) = ( ui_A + (uc_bgl<<24) );
		// Load the L line
		_amem4(puc_HorLine+cus_tmpA+PicWidthInPix) = ( uc_m + (ui_A<<8) );
	}
#else
	unsigned char uc0 = sample [-1];		//I
	unsigned char uc1 = sample [PicWidthInPix - 1];		//J
	unsigned char uc2 = sample [2*PicWidthInPix - 1];	//K
	unsigned char uc3 = sample [3*PicWidthInPix - 1];	//L

	unsigned short tmp1 = uc0 + uc1 + 1;	// I + J
	unsigned short tmp2 = uc1 + uc2 + 1;
	unsigned short tmp3 = uc2 + uc3 + 1;

	sample [2*PicWidthInPix] = sample [3*PicWidthInPix + 1] = (unsigned char) ((tmp1 + tmp2) >> 2);
	sample [3*PicWidthInPix] = (unsigned char) ((tmp2 + tmp3) >> 2);
	

	uc2 = sample [- PicWidthInPix - 1];		//	M
	uc3 = sample [- PicWidthInPix];			// A
	tmp2 = uc0 + uc2 + 1;	// I + M
	tmp3 = uc2 + uc3 + 1;

     
	sample [PicWidthInPix] = sample [2*PicWidthInPix + 1] = sample [3*PicWidthInPix + 2]  = (unsigned char) ((tmp1 + tmp2) >> 2);

	// I + J + I + M
	sample [0] = sample [PicWidthInPix + 1] = sample [2*PicWidthInPix + 2] = sample [3*PicWidthInPix + 3] = (unsigned char) ((tmp2 + tmp3) >> 2); // I + 2*M +A

	uc0 = sample [1 - PicWidthInPix];
	uc1 = sample [2 - PicWidthInPix];
    
	sample [1] = sample [PicWidthInPix + 2] = sample [2*PicWidthInPix + 3] = (uc2 + 2 * uc3 + uc0 + 2) >> 2;
	sample [2] = sample [PicWidthInPix + 3] = (uc3 + 2 * uc0 + uc1 + 2) >> 2;
	sample [3] = (uc0 + 2 * uc1 + sample [3 - PicWidthInPix] + 2) >> 2;

#endif

}




/**
This function allows to do the 4x4 vertical-right interpolation.

@param predict_sample The sample to be decoded
@param PicWidthInPix Width in pixel of the image.
@param macro Specifies the availability of the neighbouring macroblock.
@param locx Location in x in the current macroblock.
@param locy Location in y in the current macroblock.
*/
static __inline void predict_4x4_vr ( unsigned char * RESTRICT sample, const short PicWidthInPix, int Mask, const short locx, const short locy)
{
#ifdef TI_OPTIM
	unsigned short us_ABp,us_MIp,us_BCp,us_AMp,us_CDp,us_JKp,us_IJp;
	const unsigned short cus_tmpA=(PicWidthInPix<<1);
	{	// Load A B C D
		const unsigned int cui_ABCD= _amem4_const( &sample [- PicWidthInPix]);
		const unsigned char cuc_tmp0 = cui_ABCD;		// A
		const unsigned char cuc_tmp1 = cui_ABCD>>8;		// B
		const unsigned char cuc_tmp2 = cui_ABCD>>16;	// C
		const unsigned char cuc_tmp3 = cui_ABCD>>24;	// D
		// Load M I J K
		const unsigned char cuc_tmp4 = sample [- 1 - PicWidthInPix];	// M
		const unsigned char cuc_tmp5 = sample [-1];						// I
		const unsigned char cuc_tmp6 = sample [PicWidthInPix - 1];		// J
		const unsigned char cuc_tmp7 = sample [cus_tmpA - 1];			// K
		// Temporary equations
		us_ABp = cuc_tmp0+cuc_tmp1+1;	// A+B+1
		us_BCp = cuc_tmp1+cuc_tmp2+1;	// B+C+1
		us_CDp = cuc_tmp2+cuc_tmp3+1;	// C+D+1
		us_MIp = cuc_tmp4+cuc_tmp5+1;	// M+I+1
		us_AMp = cuc_tmp0+cuc_tmp4+1;	// A+M+1
		us_IJp = cuc_tmp5+cuc_tmp6+1;	// I+J+1
		us_JKp = cuc_tmp6+cuc_tmp7+1;	// J+K+1
	}
	{
		const unsigned char cuc_tmp0 = us_AMp>>1;				// a=j=(A+M+1)>>1
		const unsigned char cuc_tmp1 = us_ABp>>1;				// b=k=(A+B+1)>>1
		const unsigned char cuc_tmp2 = us_BCp>>1;				// c=l=(B+C+1)>>1
		const unsigned char cuc_tmp3 = us_CDp>>1;				// d=(C+D+1)>>1
		const unsigned char cuc_tmp4 = (us_MIp+us_IJp)>>2;		// i=(M+2I+J+1)>>2
		unsigned int	cui_tmpA = 0;
		cui_tmpA = cuc_tmp0 + (cuc_tmp1<<8) | (cuc_tmp2<<16);
		_amem4(sample) = (  cui_tmpA + (cuc_tmp3<<24) );	// Load line I
		_amem4(sample + cus_tmpA) = ( cuc_tmp4 + (cui_tmpA<<8));// Load line K
	}
	{
		const unsigned char cuc_tmp3 = (us_BCp + us_CDp) >>2;	// (B+2C+D+2)>>2
		const unsigned char cuc_tmp0 = (us_AMp + us_MIp) >>2;	// (A+2M+I+2)>>2
		const unsigned char cuc_tmp2 = (us_ABp + us_BCp) >>2;	// (A+2B+C+2)>>2
		const unsigned char cuc_tmp4 = (us_IJp + us_JKp) >>2;	// (I+2J+K+2)>>2
		const unsigned char cuc_tmp1 = (us_AMp + us_ABp) >>2;	// (B+2A+M+2)>>2
		unsigned int	cui_tmpA = 0;
		cui_tmpA =  cuc_tmp0 + (cuc_tmp1<<8) | (cuc_tmp2<<16);
		_amem4(&sample [(cus_tmpA+PicWidthInPix)]) = (cuc_tmp4 + (cui_tmpA<<8) ); // Load line L
		_amem4(&sample [PicWidthInPix]) = (cui_tmpA + (cuc_tmp3<<24) );// Load line J
	}
#else

	unsigned char uc0 = sample [- 1 - PicWidthInPix];
	unsigned char uc1 = sample [- PicWidthInPix];
	unsigned char uc2 = sample [1 - PicWidthInPix];
	unsigned char uc3 = sample [2 - PicWidthInPix];
	unsigned char uc4 = sample [3 - PicWidthInPix];

	short tmp1 = uc0 + uc1 + 1;
	short tmp2 = uc1 + uc2 + 1;
	short tmp3 = uc2 + uc3 + 1;
	short tmp4 = uc3 + uc4 + 1;

	sample [0] = sample [2*PicWidthInPix + 1] = (unsigned char) (tmp1 >> 1);
	sample [1] = sample [2*PicWidthInPix + 2] = (unsigned char) (tmp2 >> 1);
		
	sample [PicWidthInPix + 1] = sample [3*PicWidthInPix + 2] = (unsigned char) ((tmp1 + tmp2) >> 2);
	sample [2] = sample [2*PicWidthInPix + 3] = (unsigned char) (tmp3 >> 1);
	sample [PicWidthInPix + 2] = sample [3*PicWidthInPix + 3] = (unsigned char) ((tmp2 + tmp3) >> 2);
	sample [3] = (unsigned char) (tmp4 >> 1);
	sample [PicWidthInPix + 3] = (unsigned char) ((tmp3 + tmp4) >> 2);

	uc2 = sample [-1];
	uc3 = sample [PicWidthInPix - 1];
	uc4 = sample [2*PicWidthInPix - 1];
	tmp2 = uc0 + uc2 + 1;
	tmp3 = uc2 + uc3 + 1;
	tmp4 = uc3 + uc4 + 1;

	sample [PicWidthInPix] = sample [3*PicWidthInPix + 1] = (unsigned char) ((tmp1 + tmp2) >> 2);
	sample [2*PicWidthInPix] = (unsigned char) ((tmp2 + tmp3) >> 2);
	sample [3*PicWidthInPix] = (unsigned char) ((tmp3 + tmp4) >> 2);
#endif
}



/**
This function allows to do the 4x4 horizontal-down interpolation.

@param predict_sample The sample to be decoded
@param PicWidthInPix Width in pixel of the image.
@param macro Specifies the availability of the neighbouring macroblock.
@param locx Location in x in the current macroblock.
@param locy Location in y in the current macroblock.
*/
static __inline void predict_4x4_hd ( unsigned char * RESTRICT sample, const short PicWidthInPix, int Mask, const short locx, const short locy)
{
#ifdef TI_OPTIM
	unsigned short us_ABp,us_MIp,us_BCp,us_AMp,us_JKp,us_IJp,us_KLp;
	const unsigned short cus_tmpA=(PicWidthInPix<<1);
	{	// Load M A B C 
		const unsigned int	cui_ABCD = _mem4_const( &sample [- PicWidthInPix-1]);
		const unsigned char cuc_tmpM = cui_ABCD;		// M
		const unsigned char cuc_tmpA = cui_ABCD>>8;		// A
		const unsigned char cuc_tmpB = cui_ABCD>>16;	// B
		const unsigned char cuc_tmpC = cui_ABCD>>24;	// C
		// Load I J K L
		const unsigned char cuc_tmpI = sample [-1];							// I
		const unsigned char cuc_tmpJ = sample [PicWidthInPix - 1];			// J
		const unsigned char cuc_tmpK = sample [cus_tmpA - 1];				// K
		const unsigned char cuc_tmpL = sample [cus_tmpA+PicWidthInPix - 1];	// L
		// Temporary equations
		us_ABp = cuc_tmpA+cuc_tmpB+1;	// A+B+1
		us_BCp = cuc_tmpB+cuc_tmpC+1;	// B+C+1
		us_MIp = cuc_tmpM+cuc_tmpI+1;	// M+I+1
		us_IJp = cuc_tmpI+cuc_tmpJ+1;	// I+J+1
		us_JKp = cuc_tmpJ+cuc_tmpK+1;	// I+J+1
		us_KLp = cuc_tmpK+cuc_tmpL+1;	// K+L+1
		us_AMp = cuc_tmpA+cuc_tmpM+1;	// A+M+1
	}
	{
		const unsigned char cuc_ag	= (us_MIp)>>1;				// a=g=(M+I+1)>>1
		const unsigned char cuc_bh	= (us_AMp+us_MIp)>>2;		// b=h=(A+2M+I+2)>>2
		const unsigned char cuc_c	= (us_AMp+us_ABp)>>2;		// c=(M+2A+B+2)>>2
		const unsigned char cuc_d	= (us_ABp+us_BCp)>>2;		// d=(A+2B+C+2)>>2
		const unsigned char cuc_ek	= (us_IJp)>>1;				// e=k=(I+J+1)>>1
		const unsigned char cuc_fl	= (us_MIp+us_IJp)>>2;		// f=l=(M+2I+J+2)>>2
		const unsigned char cuc_io	= (us_JKp>>1);				// i=(J+K+1)>>1
		const unsigned char cuc_jp	= (us_JKp+us_IJp)>>2;		// j=(K+2J+I+2)>>2
		const unsigned char cuc_m	= (us_KLp>>1);				// m=(K+L+1)>>1
		const unsigned char cuc_n	= (us_JKp+us_KLp)>>2;		// n=(J+ 2K+L+2)>>2
		unsigned int	ui_tmpA=0, ui_tmpB=0;
		ui_tmpA = cuc_ag + (cuc_bh<<8);
		ui_tmpB = cuc_ek + (cuc_fl<<8);

		_amem4(sample ) = ( ui_tmpA + (cuc_c<<16) +(cuc_d<<24) ); // Load line I
		_amem4(sample + PicWidthInPix) = ( ui_tmpB + (ui_tmpA<<16) );// Load line J

		ui_tmpA = 0;
		ui_tmpA = cuc_io + (cuc_jp<<8);
		_amem4(sample + cus_tmpA) = ( ui_tmpA + (ui_tmpB<<16) ); // Load line K
		_amem4(sample + cus_tmpA + PicWidthInPix) = ( cuc_m + (cuc_n<<8) + (ui_tmpA<<16) );// Load line L
	}
#else
   
	unsigned char uc0 = sample [ -1 - PicWidthInPix];
	unsigned char uc1 = sample [- PicWidthInPix];
	unsigned char uc2 = sample [1 - PicWidthInPix];
	unsigned char uc3 = sample [2 - PicWidthInPix];

	short tmp1 = uc0 + uc1 + 1;
	short tmp2 = uc1 + uc2 + 1;
	short tmp3 = uc2 + uc3 + 1;

	sample [2] = (unsigned char) ((tmp1 + tmp2) >> 2);
	sample [3] = (unsigned char) ((tmp2 + tmp3) >> 2);

	uc2 = sample [-1];
	uc3 = sample [PicWidthInPix - 1];

	tmp2 = uc0 + uc2 + 1;
	tmp3 = uc2 + uc3 + 1;

	sample [1] = sample [PicWidthInPix + 3] =(unsigned char) ( (tmp1 + tmp2) >> 2);

	sample [0] = sample [PicWidthInPix + 2] = (unsigned char) (tmp2 >> 1);

	sample [PicWidthInPix] = sample [2*PicWidthInPix + 2] = (unsigned char) (tmp3 >> 1);

	sample [PicWidthInPix + 1] = sample [2*PicWidthInPix + 3] = (unsigned char) ((tmp2 + tmp3) >> 2);

	uc0 = sample [2*PicWidthInPix - 1];
	uc1 = sample [3*PicWidthInPix - 1];

	tmp1 = uc3 + uc0 + 1;
	tmp2 = uc0 + uc1 + 1;

	sample [2*PicWidthInPix] = sample [3*PicWidthInPix + 2] = (unsigned char) (tmp1 >> 1);
	sample [2*PicWidthInPix + 1] = sample [3*PicWidthInPix + 3] = (unsigned char) ((tmp1 + tmp3) >> 2);
	sample [3*PicWidthInPix] = (unsigned char) (tmp2 >> 1);
	sample [3*PicWidthInPix + 1] = (unsigned char) ((tmp1 + tmp2) >> 2);

#endif
}









/**
 @param top_right The top_right macroblock of the current one
 Prediction Mode 7: vertical-left
*/

/**
This function allows to do the 4x4 top right interpolation.

@param predict_sample The sample to be decoded
@param macro Specifies the availability of the neighbouring macroblock.
@param top_right The top_right block of the current one.
@param PicWidthInPix Width in pixel of the image.
*/
static __inline void predict_4x4_vl ( unsigned char sample [RESTRICT], NEIGHBOUR *RESTRICT top_right, const short PicWidthInPix)
{
#ifdef TI_OPTIM
	unsigned short us_ABp,us_BCp,us_CDp,us_DEp,us_EFp,us_FGp;
	unsigned char uc_a,uc_bi,uc_cj,uc_e,uc_mf,uc_ng,uc_ho,uc_dk,uc_p,uc_l;
	unsigned char uc_D ;	// D
	{	// Load A B C D
		const unsigned int	cui_ABCD = _amem4_const( &sample [-PicWidthInPix]);
		const unsigned char cuc_tmpA = cui_ABCD;		// A
		const unsigned char cuc_tmpB = cui_ABCD>>8;		// B
		const unsigned char cuc_tmpC = cui_ABCD>>16;	// C
		uc_D= cui_ABCD>>24;	// D
		// Temporary equations
		us_ABp = cuc_tmpA+cuc_tmpB+1;	// A+B+1
		us_BCp = cuc_tmpB+cuc_tmpC+1;	// B+C+1
		us_CDp = cuc_tmpC+uc_D+1;		// C+D+1
	}
	{
		// Load E F G H 
		const unsigned int	cui_EFGH = _amem4_const( &sample [-PicWidthInPix+4]);
		const unsigned char cuc_tmpE = cui_EFGH;		// E
		const unsigned char cuc_tmpF = cui_EFGH>>8;		// F
		const unsigned char cuc_tmpG = cui_EFGH>>16;	// G
		us_DEp = uc_D+cuc_tmpE+1;		// D+E+1
		us_EFp = cuc_tmpE+cuc_tmpF+1;	// E+F+1
		us_FGp = cuc_tmpF+cuc_tmpG+1;	// F+G+1
	}
    uc_a = us_ABp >>1 ;  	 		// a= (A+B+1)>>1
	uc_bi= us_BCp>>1;				// b=i= (B+C+1)>>1
	uc_cj= us_CDp>>1;				// c=j= (C+D+1)>>1
	uc_e = (us_ABp+us_BCp)>>2;	 	// e= (A+2B+C+2)>>2
	uc_mf= (us_BCp+us_CDp)>>2;		// m=f= (B+2C+D+2)>>2
    if ( !top_right -> Avail ) 
	{
		const unsigned short cus_DD= (uc_D<<1)+1;
		uc_p=uc_ho=uc_l=uc_dk= (cus_DD)>>1;	// p=h=o=l=d=k= (2*D+1)>>1
		uc_ng = (us_CDp+cus_DD)>>2;			// g=n= (C+D+1 +2*D +1)>>2
	}
	else
	{			
		uc_ng	= (us_CDp+us_DEp)>>2;	// n=g=(C+2D+E+2)>>2
		uc_ho	= (us_EFp+us_DEp)>>2;	// h=o= (F+2E+D+2)>>2
		uc_dk	= us_DEp>>1;			// d=k=(D+E+1)>>1
		uc_p	= (us_EFp+us_FGp)>>2;	// p=( E+2F+G+2)>>2
		uc_l	= us_EFp>>1;			// l=(E+F+1)>>1
	}
	{
		unsigned int ui_tempA=0;
		const unsigned short cus_tmpA=(PicWidthInPix<<1);
		ui_tempA = uc_bi + (uc_cj<<8) +(uc_dk<<16);
		_amem4(sample) = ( uc_a + (ui_tempA<<8) );				// Load line I
		_amem4(sample + cus_tmpA) = ( ui_tempA + (uc_l<<24) );				// Load line K

		ui_tempA=0;
		ui_tempA = uc_mf + (uc_ng<<8) + (uc_ho<<16);
		_amem4(sample + PicWidthInPix		) = ( uc_e + (ui_tempA<<8) );		// Load line J
		_amem4(sample + cus_tmpA+PicWidthInPix) = ( ui_tempA + (uc_p<<24));	// Load line L
	}
#else
	unsigned char uc0 = sample [ - PicWidthInPix];	// A
	unsigned char uc1 = sample [1 - PicWidthInPix];	// B
	unsigned char uc2 = sample [2 - PicWidthInPix];	// C
	unsigned char uc3 = sample [3 - PicWidthInPix];	// D

	short tmp1 = uc0 + uc1 + 1;	//	(A+B+1)
	short tmp2 = uc1 + uc2 + 1;	//	(B+C+1)
	short tmp3 = uc2 + uc3 + 1;	//	(C+D+1)
    
    sample [0] = (unsigned char) (tmp1 >> 1);   	//	a=(A+B+1)>>1
	sample [1]  = sample [2* PicWidthInPix] = (unsigned char) (tmp2 >> 1);	// b=i= (B+C+1)>>1
	sample [2] = sample [2* PicWidthInPix + 1] = (unsigned char) (tmp3 >> 1);	// c=j= (C+D+1)>>1
	sample [PicWidthInPix] = (unsigned char) ((tmp1 + tmp2) >> 2);				// e= (A+2B+C+2)>>2
	sample [3* PicWidthInPix] =	sample [PicWidthInPix + 1] = (unsigned char) ((tmp2 + tmp3) >> 2);	// m= f=(B+2C+D+2)>>2


    if ( !top_right -> Avail ) 
	{
		sample [3* PicWidthInPix + 3] = sample [PicWidthInPix + 3] = sample [3* PicWidthInPix + 2] = 
		sample [2* PicWidthInPix + 3] = sample [3] = sample [2* PicWidthInPix + 2] = (unsigned char) ((2 * uc3 + 1) >> 1);
		// p=h=o=l=d=k= (2*D+1)>>1
		sample [1* PicWidthInPix + 2]=	sample [3* PicWidthInPix + 1] = (unsigned char) ((tmp3 + 2 * uc3 + 1) >> 2);
		// g=n= (C+D+1 +2*D +1)>>2 
	}
	else
	{			
		
		uc0 = sample [- PicWidthInPix + 4];	// E
		uc1 = sample [- PicWidthInPix + 5];	// F

		tmp1 = uc0 + uc1 + 1;	// ( E+F+1) 
		tmp2 = uc3 + uc0 + 1;	// ( D+E+1)

		sample [3* PicWidthInPix + 1] = sample [1* PicWidthInPix + 2] = (unsigned char) ((tmp2 + tmp3) >> 2);	//n=g=(C+2D+E+2)>>2
		sample [3]  = sample [2* PicWidthInPix + 2]	= (unsigned char) (tmp2 >> 1);		// d=k=(D+E+1)>>1
		sample [PicWidthInPix + 3] = sample [3* PicWidthInPix + 2] = (unsigned char) ((tmp1 + tmp2) >> 2); // h=o= (F+2E+D+2)>>2
		sample [2* PicWidthInPix + 3] = (unsigned char) (tmp1 >> 1);	// l=(E+F+1)>>1
		sample [3* PicWidthInPix + 3] = (unsigned char) ((tmp1 + uc1 + sample [- PicWidthInPix + 6] + 1) >> 2);
				 // p= ( E+F+1)+F+G +1)>>2
	}
#endif
}


/**
This function allows to do the 4x4 horizontal-up interpolation.

@param predict_sample The sample to be decoded
@param PicWidthInPix Width in pixel of the image.
@param macro Specifies the availability of the neighbouring macroblock.
@param locx Location in x in the current macroblock.
@param locy Location in y in the current macroblock.
*/
static __inline void predict_4x4_hu ( unsigned char * RESTRICT sample, const short PicWidthInPix, int Mask, const short locx, const short locy)
{
#ifdef TI_OPTIM
	unsigned short us_IJp,us_JKp,us_KLp;
	unsigned char uc_a,uc_b,uc_ce,uc_gi,uc_df,uc_hj,uc_L;
	const unsigned short cus_tmpA=(PicWidthInPix<<1);
	{
		// Load I J K L
		const unsigned char cuc_tmpI = sample [-1];					// I
		const unsigned char cuc_tmpJ = sample [PicWidthInPix - 1];	// J
		const unsigned char cuc_tmpK = sample [cus_tmpA - 1];		// K
		uc_L = sample [cus_tmpA+PicWidthInPix - 1];					// L
		// Temporary equations
		us_IJp = cuc_tmpI+cuc_tmpJ+1;		// I+J+1
		us_JKp = cuc_tmpJ+cuc_tmpK+1;		// I+J+1
		us_KLp = cuc_tmpK+uc_L+1;			// K+L+1
	}
	uc_a = us_IJp >> 1;		 				// a= (I+J+1)>>1
	uc_b = (us_IJp + us_JKp) >>2;			// b=(I+2J+K+2)>>2
	uc_ce = us_JKp >> 1; 					// c=e= (J+K+1)>>1
	uc_gi = us_KLp >>1;						// g=i=(K+L+1)>>1
	uc_df = (us_KLp + us_JKp) >>2;			// d=f=(2K+L+J+2)>>2
	uc_hj = (us_KLp + (uc_L<<1) + 1) >>2;	// h=j=(K+L+1 +2*L+1)>>2
//	uc_zHU5 = uc_L;			  				// zHU >5, zHU=x+2*y, l=k=m=n=o=p= L
	{
		unsigned int ui_tempA=0,ui_tempB=0;

		ui_tempA= uc_ce + (uc_df<<8);
		ui_tempB= uc_gi + (uc_hj<<8);
		_amem4(sample) = uc_a + (uc_b<<8) +(ui_tempA<<16); // Load line I
		_amem4(sample + PicWidthInPix) = ui_tempA + (ui_tempB<<16);// Load line J

		ui_tempA=0;
		ui_tempA= (uc_L + (uc_L<<8));
		_amem4(sample + cus_tmpA) = ui_tempB +(ui_tempA<<16);// Load line K
		_amem4(sample + cus_tmpA + PicWidthInPix) = ui_tempA+(ui_tempA<<16);// Load line L
	}
#else	
	unsigned char uc0 = sample [- 1];
	unsigned char uc1 = sample [PicWidthInPix - 1];
	unsigned char uc2 = sample [2*PicWidthInPix - 1];
	unsigned char uc3 = sample [3*PicWidthInPix - 1];
	short tmp1,tmp2;

	tmp1 = (uc0 + uc1 + 1) ;
	tmp2 = (uc1 + uc2 + 1) ;

	sample [0] = (unsigned char) (tmp1 >> 1);
	sample [2] = sample [PicWidthInPix] = (unsigned char) (tmp2 >> 1);
	sample [1] = (unsigned char) ((tmp1 + tmp2) >> 2);

	tmp1 = (uc2 + uc3 + 1);

	sample [PicWidthInPix + 2] = sample [2* PicWidthInPix] = (unsigned char) (tmp1 >> 1);
	sample [3] = sample [PicWidthInPix + 1] = (unsigned char) ((tmp1 + tmp2) >> 2);
	sample [PicWidthInPix + 3] = sample [2* PicWidthInPix + 1] = (unsigned char) ((tmp1 + 2*uc3 + 1) >> 2);


	sample [2* PicWidthInPix + 3] = sample [3* PicWidthInPix + 1] = 
	sample [3* PicWidthInPix] =	sample [2* PicWidthInPix + 2] = 
	sample [3* PicWidthInPix + 2] = sample [3* PicWidthInPix + 3] = (unsigned char) uc3;
#endif
}
//*------------------ 16x16


/**
This function allows to do the 16x16 vertical interpolation.

@param predict_sample The sample to be decoded
@param PicWidthInPix Width in pixel of the image.
@param macro Specifies the availability of the neighbouring macroblock.
*/
static __inline void predict_16x16_v( unsigned char sample [RESTRICT], const short PicWidthInPix, int Mask)
{
#ifdef TI_OPTIM
 	unsigned char j ;
	unsigned char * current_ligne= & sample [0];		//Current ligne 
	const unsigned long long cull_tempA=_amem8_const( & sample [-PicWidthInPix]);	// LSB Ref. ligne 
	const unsigned long long cull_tempB=_amem8_const( & sample [-PicWidthInPix+8]);	// MSB Ref. line

#pragma UNROLL(16);
    for ( j=16 ; j !=0 ; j--)
     {
		_amem8(current_ligne)= cull_tempA;
		_amem8(current_ligne+8)= cull_tempB;
		current_ligne += PicWidthInPix;
    }
#else
    int         i, j ;
    
    for ( j = 0 ; j < 16 ; j ++, sample += PicWidthInPix) {
        for ( i = 0 ; i < 16 ; i++) {
            	sample [i] = sample [i - PicWidthInPix];
        }
    }
#endif
}




/**
This function allows to do the 16x16 horizontal interpolation.

@param predict_sample The sample to be decoded
@param PicWidthInPix Width in pixel of the image.
@param macro Specifies the availability of the neighbouring macroblock.
*/
static __inline void predict_16x16_h( unsigned char sample [RESTRICT], const short PicWidthInPix, int Mask)
{
#ifdef TI_OPTIM
 	unsigned char j,uc_B;
	unsigned char * puc_Pix= & sample [-1];
	unsigned long long ull_A;

	unsigned int ui_A;
#pragma UNROLL(16);
	for ( j=16 ; j!=0 ; j--)
	{
		uc_B=*(puc_Pix); // Load the reference pix
		ui_A= 0x01010101*uc_B;
		ull_A=_itoll(ui_A,ui_A);
		_amem8(puc_Pix+1)=ull_A;
		_amem8(puc_Pix+9)=ull_A;
		puc_Pix += PicWidthInPix;
   }
#else
    int  i ;
    int   j ;

	for ( j = 0 ; j < 16 ; j++, sample += PicWidthInPix) {
		for ( i = 0 ; i < 16 ; i++ ) {
           sample [i] = sample [-1];
        }
    }
#endif
}


/**
This function allows to do the 16x16 DC interpolation.

@param predict_sample The sample to be decoded
@param PicWidthInPix Width in pixel of the image.
*/
static __inline void predict_16x16_dc( unsigned char sample [RESTRICT],	const short PicWidthInPix)
{
#ifdef TI_OPTIM
	unsigned char * puc_Pix= & sample [- PicWidthInPix];
	unsigned int ui_tempA,ui_tempB,ui_tempC;
	const unsigned short us_tmpA = (PicWidthInPix<<1);
	unsigned long long ull_A;
	unsigned char  i;
	/* sum horizontal Ref pix*/
	ui_tempC = _amem4_const(puc_Pix);		// Load 4 pixels 
	ui_tempB = _amem4_const(puc_Pix+4);		// Load 4 pixels
	ui_tempA = _dotpu4(ui_tempC,0x01010101)+ _dotpu4(ui_tempB,0x01010101);

	ui_tempC = _amem4_const(puc_Pix+8);		// Load 4 pixels 
	ui_tempB = _amem4_const(puc_Pix+12);	// Load 4 pixels 
	ui_tempA += _dotpu4(ui_tempC,0x01010101)+ _dotpu4(ui_tempB,0x01010101);
	/* sum vertical Ref pix*/
	puc_Pix= & sample [- 1];
	ui_tempB=0;
	#pragma UNROLL(8);
    for ( i = 8 ; i !=0 ; i--) 
	{
        ui_tempB += ( *(puc_Pix)+ *(puc_Pix+PicWidthInPix) );
		puc_Pix	 += us_tmpA;
    }
	// ui_tempC = mean
    ui_tempC = 0x01010101 * ((ui_tempA + ui_tempB + 16) >> 5);	// duplication of the mean on all the line
	ull_A = _itoll(ui_tempC,ui_tempC);
	// store ui_tempC on the 16 lines
	puc_Pix= & sample [0];
	#pragma UNROLL(16);
    for ( i=16 ; i!=0 ; i--)
	{
		_amem8(puc_Pix)= ull_A;
		_amem8(puc_Pix+8)= ull_A;
		 puc_Pix+=PicWidthInPix;
	}
#else
    int i, j, s0, s1, s2 ;

    s1 = s2 = 0 ;
    for ( i = 0 ; i < 16 ; i++) {
        s1 += sample [i - PicWidthInPix]; // sum hor pix
    }
	
    for ( i = 0 ; i < (PicWidthInPix << 4) ; i += PicWidthInPix) 	{
        s2 += sample [i - 1]; // sum vert pix
    }

    s0 = (s1 + s2 + 16) >> 5 ;
	for ( j = 0 ; j < 16 ; j++ ,sample += PicWidthInPix) {
        for ( i = 0 ; i < 16 ; i++) {
			sample [i] = (unsigned char) s0 ;
        }
	}
#endif
}




//-----------------------16x16
/**
*/


/**
This function allows to do the 16x16 DC left interpolation.

@param predict_sample The sample to be decoded
@param PicWidthInPix Width in pixel of the image.
*/

static __inline void predict_16x16_dc_left(	unsigned char sample [RESTRICT], const short PicWidthInPix)
{
#ifdef TI_OPTIM
	unsigned char * puc_Pix= & sample [- 1];
	unsigned int ui_tempA= 0;
	const unsigned int cui_tmpA = (PicWidthInPix<<1);
	unsigned char  i;
	unsigned long long ull_A;

	/* sum vertical Ref pix*/
	#pragma UNROLL(8);
    for ( i = 8 ; i !=0 ; i--) 
	{
        ui_tempA += ( *(puc_Pix)+ *(puc_Pix+PicWidthInPix) );
		puc_Pix+=cui_tmpA;		// Jump to the next pixel
    }

	ui_tempA = ((ui_tempA+8)>>4);
	ui_tempA =(0x01010101*ui_tempA);	// duplication of the mean on all the line
	ull_A = _itoll(ui_tempA,ui_tempA);

	// store ui_tempA on the 16 lines
	puc_Pix= & sample [0];
	// store ui_tempA on the 16 lines
	#pragma UNROLL(16);
    for ( i=16 ; i!=0 ; i--)
	{
		_amem8(puc_Pix)=ull_A;
		_amem8(puc_Pix+8)=ull_A;
		puc_Pix+=PicWidthInPix;
	}
#else   
    int i, j;
	int s0 = 0 ;

    for ( i = 0 ; i < 16 ; i++ ) {
        s0 += sample [(i * PicWidthInPix) - 1]; // sum vert pix
    }

    s0 = (s0 + 8) >> 4 ;
    for ( j = 0 ; j < 16 ; j++ , sample += PicWidthInPix)	{
        for ( i = 0 ; i < 16 ; i++) {
			sample [i] = (unsigned char) s0 ;
        }
    }
#endif
}







/**
This function allows to do the 16x16 DC top interpolation.

@param predict_sample The sample to be decoded
@param PicWidthInPix Width in pixel of the image.
*/
static __inline void predict_16x16_dc_top( unsigned char sample [RESTRICT],	const short PicWidthInPix)
{
#ifdef TI_OPTIM
	unsigned char * puc_Pix= & sample [- PicWidthInPix];
	unsigned int ui_tempA,ui_tempB,ui_tempC;
	unsigned long long ull_A;
	unsigned char i;
	/* sum horizontal Ref pix*/
	ui_tempC = _amem4_const(puc_Pix);		// Load 4 pixels 
	ui_tempB = _amem4_const(puc_Pix+4);		// Load 4 pixels
	ui_tempA = _dotpu4(ui_tempC,0x01010101) + _dotpu4(ui_tempB,0x01010101);

	ui_tempC = _amem4_const(puc_Pix+8);		// Load 4 pixels 
	ui_tempB = _amem4_const(puc_Pix+12);	// Load 4 pixels 
	ui_tempA += _dotpu4(ui_tempC,0x01010101) + _dotpu4(ui_tempB,0x01010101);
	ui_tempA = (ui_tempA+8)>>4;
	// duplication of the mean on all the line
    ui_tempA =(0x01010101*ui_tempA);

	ull_A = _itoll(ui_tempA,ui_tempA);

	puc_Pix= & sample [0];
	// store ui_tempA on the 16 lines
	#pragma UNROLL(16);
    for ( i=16 ; i!=0 ; i--)
	{
		_amem8(puc_Pix)=ull_A;
		_amem8(puc_Pix+8)=ull_A;
		puc_Pix+=PicWidthInPix;
	}
#else
    int         i, j;
    int s0 = 0 ;

    for ( i = 0 ; i < 16 ; i++ ) {
        s0 += sample [i - PicWidthInPix]; // sum hor pix
    }

    s0 = (s0 + 8) >> 4 ;
    for ( j = 0 ; j < 16 ; j++ ,sample += PicWidthInPix) 
        for ( i = 0 ; i < 16 ; i++ ){
             sample [i] = (unsigned char) s0 ;
        }
#endif
}



/**
This function allows to do the 16x16 DC 128 interpolation.

@param predict_sample The sample to be decoded
@param PicWidthInPix Width in pixel of the image.
@param macro Specifies the availability of the neighbouring macroblock.
*/
static __inline void predict_16x16_dc_128( unsigned char sample [RESTRICT],	const short PicWidthInPix)
{
#ifdef TI_OPTIM
	unsigned char i;
	unsigned char * puc_Pix= &sample [0];
	#pragma UNROLL(16);
    for ( i=16 ; i!=0 ; i--)
	{
		_amem8(puc_Pix)	= 0x8080808080808080;
		_amem8(puc_Pix+8)= 0x8080808080808080;
		puc_Pix += PicWidthInPix;
	}
#else
	int i, j;
	unsigned int *ptr_img;
    const int stride = PicWidthInPix >> 2; 
	ptr_img = (unsigned int *) sample;
    
	for ( j = 0 ; j < 16 ; j++ ) {
		for ( i = 0 ; i < 4 ; i++ ) {     
               	*(ptr_img + i) = 0x80808080;
        }
		ptr_img += stride;
    }
#endif
}



/**
 Prediction Mode 3 : plane
*/
static __inline void predict_16x16_p( unsigned char sample [RESTRICT], const short PicWidthInPix, int Mask)
{
#ifdef TI_OPTIM
    int a, b, c,H = 0,V = 0;
	int	i00,j,index=0,i;
	unsigned char * puc_CurPix= &sample [- 1 - PicWidthInPix];
	unsigned char  uc_lastHPix,uc_firstHPix;
	const unsigned int cui_by2 = (PicWidthInPix<<1);
	const unsigned int cui_by4 = (PicWidthInPix<<2);
	const unsigned int cui_by8 = (PicWidthInPix<<3);
	int ui_tempA,ui_tempB,ui_tempC;
 	const unsigned char cuc_lastVPix= sample [ -1 +15*PicWidthInPix];
//----H----
	ui_tempA = _mem4_const(puc_CurPix);			// ui_tempA = lo (-1 | 0 | 1 | 2) hi
	uc_firstHPix = ui_tempA;					// uc_firstHPix = pixel(-1,-1)
	ui_tempB = _dotpu4(ui_tempA,0x05060708 );	//
	ui_tempA = _mem4_const(puc_CurPix+4);		// ui_tempB = lo (3 | 4 | 5 | 6) hi
	ui_tempB += _dotpu4(ui_tempA,0x01020304);	//
	ui_tempA = _mem4_const(puc_CurPix+9);		// ui_tempB = lo (8  | 9 | 10 | 11) hi
	ui_tempC = _dotpu4(ui_tempA,0x04030201);	//
	ui_tempA = _mem4_const(puc_CurPix+13);		// ui_tempA = lo (12 | 13 | 14 | 15) hi
	ui_tempC += _dotpu4(ui_tempA,0x08070605);
	H= ui_tempC-ui_tempB;
	uc_lastHPix = (ui_tempA>>24);				// uc_lastHPix = pixel(15,-1)
//----V----
	puc_CurPix= (sample -1 + cui_by8+cui_by4);					//(ligne 12)
	ui_tempA=_shrmb( cuc_lastVPix,ui_tempA	);				//15
	ui_tempA=_shrmb( *(puc_CurPix+cui_by2),ui_tempA	);		//14
	ui_tempA=_shrmb( *(puc_CurPix+PicWidthInPix),ui_tempA);	//13
	ui_tempA=_shrmb( *(puc_CurPix),ui_tempA	);				//12
	ui_tempB= _dotpu4(ui_tempA,0x05060708 );

	puc_CurPix= (sample -1 + cui_by8);		//(ligne 8)		
	ui_tempA=_shrmb( *(puc_CurPix+cui_by2+PicWidthInPix),ui_tempA);	//11
	ui_tempA=_shrmb( *(puc_CurPix+cui_by2),ui_tempA);				//10
	ui_tempA=_shrmb( *(puc_CurPix+PicWidthInPix),ui_tempA);			//9 
	ui_tempA=_shrmb( *(puc_CurPix),ui_tempA	);						//8
	ui_tempB += _dotpu4(ui_tempA,0x01020304 );

	puc_CurPix= (sample -1 + cui_by2+PicWidthInPix);	//(ligne 3)
	ui_tempA=_shrmb( *(puc_CurPix+cui_by2+PicWidthInPix),ui_tempA);	//6
	ui_tempA=_shrmb( *(puc_CurPix+cui_by2),ui_tempA);				//5
	ui_tempA=_shrmb( *(puc_CurPix+PicWidthInPix),ui_tempA);			//4
	ui_tempA=_shrmb( *(puc_CurPix),ui_tempA	);						//3
	ui_tempC = _dotpu4(ui_tempA,0x04030201 );

	puc_CurPix= &sample [- 1];			//(ligne 0)
	ui_tempA=_shrmb( *(puc_CurPix+cui_by2)	,ui_tempA	);			//2
	ui_tempA=_shrmb( *(puc_CurPix+PicWidthInPix),ui_tempA);			//1
	ui_tempA=_shrmb( *(puc_CurPix),	ui_tempA);						//0 
	ui_tempA=_shrmb( uc_firstHPix,ui_tempA	);						//-1
	ui_tempC += _dotpu4(ui_tempA,0x08070605 );

	V= ui_tempB-ui_tempC;
//----abc----
    a = (uc_lastHPix + cuc_lastVPix) <<4;
    b = (5 * H + 32) >> 6 ;
    c = (5 * V + 32) >> 6 ;
//----abc----
	i00 = a + -7*b -7*c + 16 ;

    for ( j = 0 ; j < 16 ; j++, i00+=(c-(b<<4)), index += (PicWidthInPix-16) ) 
	{
        for ( i = 0 ; i < 16 ; i++, i00+=b ) 
		{
               	sample [index++] = _spacku4(0,(i00 >> 5)) ;
        }
    }
#else
    int         i, j ;
    int         a, b, c ;
	int         i00 ;
    int         H = 0 ;
    int         V = 0 ;

    a = (sample [15 - PicWidthInPix] + sample [-1 + 15 * PicWidthInPix]) << 4;

    for ( i = 0 ; i < 8 ; i++ ) {
        H += (i + 1) * (sample [8 + i - PicWidthInPix] - sample [6 - i - PicWidthInPix]);
		V += (i + 1) * (sample [-1 + (8 + i) * PicWidthInPix]  - sample [-1 + (6 - i) * PicWidthInPix]);
    }
    b = (5 * H + 32) >> 6 ;
    c = (5 * V + 32) >> 6 ;
	i00 = a + -7*b -7*c + 16 ;

    for ( j = 0 ; j < 16 ; j++, i00 +=(c - (b << 4)), sample += PicWidthInPix ) 	{
        for ( i = 0 ; i < 16 ; i++, i00 += b ) {
			sample [i] = (unsigned char) CLIP255_16(i00 >> 5) ;
        }
    }
#endif
}





/**
This function allows to do the Intra Chroma Vertical interpolation.

@param predict_sample The sample to be decoded
@param PicWidthInPix Width in pixel of the image.
@param macro Specifies the availability of the neighbouring macroblock.
*/
static __inline void predict_chr_v( unsigned char * RESTRICT sample, const short PicWidthInPix, int Mask)
{
#ifdef TI_OPTIM
	const unsigned long long cui_tempA=_amem8_const( & sample [-PicWidthInPix]);	// Ref. ligne 
	const unsigned int b=(PicWidthInPix<<2);// b= 4*PicWidthInPix
	const unsigned int d=(PicWidthInPix<<1);// d= 2*PicWidthInPix
	const unsigned int a= PicWidthInPix+d;	// a= 3*PicWidthInPix
	// Copy cui_tempA on all the Lines
	_amem8(sample)=cui_tempA;
	_amem8(sample+PicWidthInPix)=cui_tempA;
	_amem8(sample+d)=cui_tempA;
	_amem8(sample+a)=cui_tempA;
	_amem8(sample+b)=cui_tempA;
	_amem8(sample+PicWidthInPix+b)=cui_tempA;
	_amem8(sample+d+b)=cui_tempA;
	_amem8(sample+a+b)=cui_tempA;
#else
    int i;
    int j;  
	int index = 0;

    for ( j = 0 ; j < 8 ; j++, index +=(PicWidthInPix-8) ) 
	{
        for ( i = 0 ; i < 8 ; i++ ) 
		{
           sample [index++] = sample [i - PicWidthInPix];
        }
    }
#endif
}




/**
This function allows to do the Intra Chroma Horizontal interpolation.

@param predict_sample The sample to be decoded
@param PicWidthInPix Width in pixel of the image.
@param macro Specifies the availability of the neighbouring macroblock.
*/
static __inline void predict_chr_h( unsigned char * RESTRICT sample, const short PicWidthInPix, int Mask)
{
#ifdef TI_OPTIM
	unsigned char * puc_Pix= & sample [-1];
	unsigned long long * pull_Pix= (unsigned long long *) & sample [0];
	unsigned long long ull_A;
	unsigned int ui_A;
	unsigned char uc_B,j;

	#pragma UNROLL(8);
	for ( j=8 ; j!=0 ; j--)
	{
		uc_B=*(puc_Pix);
		ui_A= 0x01010101*uc_B;
		ull_A = _itoll(ui_A,ui_A);
		_amem8(pull_Pix)=ull_A;
		puc_Pix += PicWidthInPix;
		pull_Pix = (unsigned long long *)(puc_Pix+1);
   }
#else
    int         i ;
    int         j ;    

    for ( j = 0 ; j < 8 ; j++, sample += PicWidthInPix){
        for ( i = 0 ; i < 8 ; i++ ) {
           sample [i] = sample [- 1];
        }
    }
#endif
}





/**
This function allows to do the Intra Chroma plane interpolation.

@param predict_sample The sample to be decoded
@param PicWidthInPix Width in pixel of the image.
@param macro Specifies the availability of the neighbouring macroblock.
*/
static __inline void predict_chr_p( unsigned char * RESTRICT sample, const short PicWidthInPix, int Mask)
{
#ifdef TI_OPTIM
//	-1	0	1	2	3	4	5	6	7
//	0
//	1
//	2
//	3	
//	4
//	5
//	6
//	7
    int a, b, c,H = 0,V = 0;
	int	i00,j,index=0,i;
	unsigned char * puc_CurPix= &sample [- 1 - PicWidthInPix];
	const unsigned char uc_V7 =  sample [- 1 +7*PicWidthInPix];	// uc_V7 = pixel(-1,7)
	const unsigned int cui_by2 = (PicWidthInPix<<1);
	const unsigned int cui_by4= (PicWidthInPix<<2);
	int ui_tempA;
	unsigned char uc_H7,uc_O;
//----H----
	ui_tempA = _mem4_const(puc_CurPix+5);		// ui_tempB = lo (4 | 5 | 6 | 7) hi
	H = _dotpu4(ui_tempA,0x04030201);			//
	uc_H7 =(unsigned char)(ui_tempA>>24);		// uc_H7 = pixel(7,-1)

	ui_tempA = _mem4_const(puc_CurPix);			// ui_tempA = lo (-1 | 0 | 1 | 2) hi
	uc_O =(unsigned char)(ui_tempA);
	H -= _dotpu4(ui_tempA,0x01020304 );			//
//----V----
	puc_CurPix= (sample -1 + cui_by4 );
	ui_tempA=_shrmb(	uc_V7,ui_tempA);								//7
	ui_tempA=_shrmb(	*(puc_CurPix+cui_by2),ui_tempA);				//6
	ui_tempA=_shrmb(	*(puc_CurPix+PicWidthInPix),ui_tempA);			//5
	ui_tempA=_shrmb(	*puc_CurPix,ui_tempA	);						//4
	V= _dotpu4(ui_tempA,0x01020304 );	// ui_tempB = lo (7 | 6 | 5 | 4) hi

	puc_CurPix= &sample [- 1 ];
	ui_tempA=_shrmb(	*(puc_CurPix+cui_by2),ui_tempA);				//2
	ui_tempA=_shrmb(	*(puc_CurPix+PicWidthInPix),ui_tempA);			//1
	ui_tempA=_shrmb(	*puc_CurPix,ui_tempA);							//0
	ui_tempA=_shrmb(	uc_O,ui_tempA);									//-1
	V -= _dotpu4(ui_tempA,0x04030201 );		// ui_tempB = lo (2 | 1| 0| -1) hi
//----abc----
    a = (uc_V7+uc_H7) <<4;
    b = (17 * H + 16) >> 5 ;
    c = (17 * V + 16) >> 5 ;
//------------
	i00 = a - 3*b - 3*c + 16;
    for ( j = 0 ; j < 8 ; j++ , i00 += (c-(b<<3)), index += (PicWidthInPix-8) )
	{
        for ( i = 0 ; i < 8 ; i++, i00 += b ) 
		{
            	 	sample [index++] =  _spacku4(0,( i00 >> 5));
        }
    }
#else
    int         i, j ;
    int         a, b, c ;
    int         H;
    int         V;
	int			i00;
	int offset = (PicWidthInPix << 2);
    
    H = sample [4 - PicWidthInPix] - sample [2 - PicWidthInPix];
    H += (sample [5 - PicWidthInPix] - sample [1 - PicWidthInPix]) << 1;
    H += 3*(short)(sample [6 - PicWidthInPix] - sample [-PicWidthInPix]);
    H += (sample [7 - PicWidthInPix] - sample [- 1 - PicWidthInPix]) << 2;

    V = (sample [offset - 1]- sample [-1 + (PicWidthInPix<<1)]);
	offset += PicWidthInPix;
    V += (sample [offset - 1]- sample [PicWidthInPix - 1]) << 1;
	offset += PicWidthInPix;
    V += 3*(sample [offset - 1]- sample [-1]);
	offset += PicWidthInPix;
    V += (sample [offset - 1]- sample [-1 - PicWidthInPix]) << 2;

    b = (17 * H + 16) >> 5 ;
    c = (17 * V + 16) >> 5 ;
    a = (sample [7 - PicWidthInPix]+ sample [-1 + 7*PicWidthInPix]) << 4;

	i00 = a - 3*b - 3*c + 16;
    for ( j = 0 ; j < 8 ; j++ , i00 += (c - (b << 3)), sample += PicWidthInPix )	{
        for ( i = 0 ; i < 8 ; i++, i00 += b ) 		{
            sample [i] = (unsigned char) CLIP255_16( i00 >> 5);
        }
    }
#endif
}





//-------------------- Chroma
//		a	 b	
//	 ____________
//c	|  A  |  B  |
//	|_____|_____|
//d |  C  |  D  |
//  | ____|_____|

/*
 Prediction mode 0: Intra_Chroma_DC
*/


/**
This function allows to do the Intra Chroma DC interpolation.

@param predict_sample The sample to be decoded
@param PicWidthInPix Width in pixel of the image.
*/
static __inline void predict_chr_dc( unsigned char * RESTRICT sample, const short PicWidthInPix)
{
#ifdef TI_OPTIM
	unsigned char * puc_Pix= & sample [- PicWidthInPix];
	const unsigned int b=(PicWidthInPix<<2);// b= 4*PicWidthInPix
	const unsigned int d=(PicWidthInPix<<1);// d= 2*PicWidthInPix
	const unsigned int a= PicWidthInPix+d;	// a= 3*PicWidthInPix
	unsigned char uc_tempA;
	unsigned int ui_a,ui_b,ui_c,ui_d,ui_tempA,ui_tempB,ui_tempC=0,ui_tempD=0;
	unsigned long long ull_A;

	/* sum all horizontal Ref pix in ui_a and ui_b*/
	ull_A= _amem8_const(puc_Pix);
	ui_tempA = (unsigned int)ull_A;		// Load 4 Ref pixels 
	ui_tempB = ull_A>>32;
	ui_a= _dotpu4(ui_tempA,0x01010101) +2;// ui_tempA=sum of all the hor pixels
	ui_b= _dotpu4(ui_tempB,0x01010101) +2;// ui_tempA=sum of all the hor pixels
	/* sum the vertical Ref pix in ui_c and ui_d*/
	puc_Pix= & sample [- 1];
    ui_c = ( *(puc_Pix)	 + *(puc_Pix+PicWidthInPix)	 + *(puc_Pix+d)+ *(puc_Pix+a)+2);
    ui_d = ( *(puc_Pix+b)+ *(puc_Pix+b+PicWidthInPix)+ *(puc_Pix+b+d)+ *(puc_Pix+b+a)+2);

	uc_tempA = (ui_a+ui_c)>>3 ;		// BLOCK A
	ui_tempA = uc_tempA + (uc_tempA<<8) + (uc_tempA<<16)+ (uc_tempA<<24);
	uc_tempA =	ui_b>>2 ;			// BLOCK B
	ui_tempB = uc_tempA + (uc_tempA<<8) + (uc_tempA<<16)+ (uc_tempA<<24);
	// Copy BLOCK A and B
	puc_Pix= & sample [0];
	// Copy ui_tempA on the lines of the block
	ull_A=_itoll(ui_tempB,ui_tempA);
	_amem8(puc_Pix)= ull_A;
	_amem8(puc_Pix+PicWidthInPix)= ull_A;
	_amem8(puc_Pix+d)= ull_A;
	_amem8(puc_Pix+a)= ull_A;

	uc_tempA =	ui_d>>2 ;			// BLOCK C
	ui_tempC = uc_tempA + (uc_tempA<<8) + (uc_tempA<<16)+ (uc_tempA<<24);
	uc_tempA = (ui_d+ui_b)>>3 ;		// BLOCK D
	ui_tempD = uc_tempA + (uc_tempA<<8) + (uc_tempA<<16)+ (uc_tempA<<24);
	// Copy BLOCK C and D
	ull_A=_itoll(ui_tempD,ui_tempC);
	puc_Pix+= b;
	_amem8(puc_Pix)= ull_A;
	_amem8(puc_Pix+PicWidthInPix)= ull_A;
	_amem8(puc_Pix+d)= ull_A;
	_amem8(puc_Pix+a)= ull_A;
#else
    int         i, j,k;
    int         s0 [4] = { 0, 0, 0, 0 };
    int         s1 [4] = { 0, 0, 0, 0 };    
	int index = 0;
	

	for ( i = 0 ; i < 4 ; i++ ) {
		s0 [0] += sample [i - PicWidthInPix];
		s0 [3] = s0 [1] += sample [i + 4 - PicWidthInPix];
		s1 [0] += sample [-1 + i * PicWidthInPix];
		s1 [2] += sample [-1 + (i + 4) * PicWidthInPix];
	}

    s0 [0] = (s0 [0] + s1 [0] + 4) >> 3 ;
    s0 [1] = (s0 [1] + 2) >> 2 ;
    s0 [2] = (s1 [2] + 2) >> 2 ;
    s0 [3] = (s0 [3] + s1 [2] + 4) >> 3 ;

	for ( j = 0 ; j < 2; j++, index = j*4 ) {
		for ( i = 0 ; i < 4 ; i++, index += PicWidthInPix) {
			for(k = 0; k < 4 ; k++)
				sample [index + k] =  (unsigned char) s0[j];
		}
	}

	index = 4 * PicWidthInPix;
	for ( j = 0 ; j < 2; j++, index = j*4 + 4*PicWidthInPix) {
		for ( i = 0 ; i < 4 ; i++, index += PicWidthInPix ) {
			for ( k = 0; k < 4 ; k++)
				sample [index + k] = (unsigned char) s0[j + 2];
		}
	}
#endif
}


/*---- predict_chr_dc_left ----*/

/**
This function allows to do the Intra Chroma DC left interpolation.

@param predict_sample The sample to be decoded
@param PicWidthInPix Width in pixel of the image.
*/
static __inline void predict_chr_dc_left( unsigned char * RESTRICT sample, const short PicWidthInPix)
{
#ifdef TI_OPTIM
	unsigned char * puc_Pix= & sample [- PicWidthInPix];
	const unsigned int b=(PicWidthInPix<<2);// b= 4*PicWidthInPix
	const unsigned int d=(PicWidthInPix<<1);// d= 2*PicWidthInPix
	const unsigned int a= PicWidthInPix+d;	// a= 3*PicWidthInPix
	unsigned char ui_c,ui_d;
	unsigned int ui_tempA,ui_tempB,ui_tempC;
	unsigned long long ull_A;
	/* sum the vertical Ref pix in ui_c and ui_d*/
	puc_Pix= & sample [- 1];
    ui_c = ( *(puc_Pix)	 + *(puc_Pix+PicWidthInPix)	 + *(puc_Pix+d)+ *(puc_Pix+a)+2)>>2;
    ui_d = ( *(puc_Pix+b)+ *(puc_Pix+b+PicWidthInPix)+ *(puc_Pix+b+d)+ *(puc_Pix+b+a)+2)>>2;

	// Copy BLOCK A and B
	ui_tempA = ui_tempC = ui_c + (ui_c<<8);
	ui_tempA +=  (ui_tempC<<16);
	// Copy BLOCK C and D
	ui_tempB = ui_tempC = ui_d + (ui_d<<8);
	ui_tempB +=  (ui_tempC<<16);

	ull_A=_itoll(ui_tempA,ui_tempA);
	puc_Pix= & sample [0];
	_amem8(puc_Pix)= ull_A;
	_amem8(puc_Pix+PicWidthInPix) = ull_A;
	_amem8(puc_Pix+d) = ull_A;
	_amem8(puc_Pix+a) = ull_A;

	ull_A=_itoll(ui_tempB,ui_tempB);
	puc_Pix+= b;
	_amem8(puc_Pix)=ull_A;
	_amem8(puc_Pix+PicWidthInPix) = ull_A;
	_amem8(puc_Pix+d) = ull_A;
	_amem8(puc_Pix+a) = ull_A;
#else
    int i, k;
    int s0 [2] = {0, 0};

    
	for ( i = 0 ; i < 4 ; i++ ) {
		s0 [0] += sample [-1 + i * PicWidthInPix];
		s0 [1] += sample [-1 + (4 + i) * PicWidthInPix];
	}
    
	s0 [0] = (s0 [0] + 2) >> 2 ;
	s0 [1] = (s0 [1] + 2) >> 2 ;
	
	for ( i = 0 ; i < 4 ; i++, sample += PicWidthInPix) {
		for(k = 0; k < 8 ; k++){
			sample [k] =  (unsigned char) s0[0];
		}
	}

	for ( i = 0 ; i < 4 ; i++, sample += PicWidthInPix ) {
		for (k = 0; k < 8 ; k++){
			sample [k] = (unsigned char) s0[1];
		}
	}
#endif
}


/**
This function allows to do the Intra Chroma DC top interpolation.

@param predict_sample The sample to be decoded
@param PicWidthInPix Width in pixel of the image.
*/
static __inline void predict_chr_dc_top( unsigned char * RESTRICT sample, const short PicWidthInPix)
{
#ifdef TI_OPTIM
	unsigned char * puc_Pix= & sample [- PicWidthInPix];
	const unsigned int b=(PicWidthInPix<<2);// b= 4*PicWidthInPix
	const unsigned int d=(PicWidthInPix<<1);// d= 2*PicWidthInPix
	const unsigned int a= PicWidthInPix+d;	// a= 3*PicWidthInPix
	unsigned int ui_a,ui_b,ui_tempA,ui_tempB;

	unsigned long long ull_temp;

 	/* sum all horizontal Ref pix in ui_a and ui_b*/
	ull_temp= _amem8_const(puc_Pix);
	ui_tempA = (unsigned int)ull_temp;		// Load 4 Ref pixels 
	ui_tempB = ull_temp>>32;
	ui_a = (_dotpu4(ui_tempA,0x01010101) +2)>>2 ;	// 
	ui_b = (_dotpu4(ui_tempB,0x01010101) +2)>>2 ;	//
	ui_tempA = 0x01010101*ui_a;
	ui_tempB = 0x01010101*ui_b;
	// Copy BLOCK A and B
	ull_temp = _itoll(ui_tempB,ui_tempA);
	puc_Pix= & sample [0];
	_amem8(puc_Pix)=ull_temp;
	_amem8(puc_Pix+PicWidthInPix)=ull_temp;
	_amem8(puc_Pix+d)=ull_temp;
	_amem8(puc_Pix+a)= ull_temp;
	// Copy BLOCK C and D
	puc_Pix+= b;
	_amem8(puc_Pix)=ull_temp;
	_amem8(puc_Pix+PicWidthInPix)=ull_temp;
	_amem8(puc_Pix+d)=ull_temp;
	_amem8(puc_Pix+a)= ull_temp;
#else

    int i, k;
    int s0 [2] = { 0, 0};
	int index = 0;
  
	for ( i = 0 ; i < 4 ; i++ ) {
		s0 [0] += sample [-PicWidthInPix + i];
		s0 [1] += sample [-PicWidthInPix + i + 4];
	}
       
	s0 [0] = (s0 [0] + 2) >> 2 ;
	s0 [1] = (s0 [1] + 2) >> 2 ;
	

	for ( i = 0 ; i < 8 ; i++, index += PicWidthInPix) {
		for(k = 0; k < 4 ; k++){
			sample [index + k] =  (unsigned char) s0[0];
			sample [index + k + 4] = (unsigned char) s0[1];
        }
    }
#endif
}







/**
This function allows to do the Intra Chroma 128 left interpolation.

@param predict_sample The sample to be decoded
@param PicWidthInPix Width in pixel of the image.
*/
static __inline void predict_chr_dc_128(unsigned char * RESTRICT sample,const short PicWidthInPix)
{
#ifdef TI_OPTIM
	const unsigned int cui_tempA = PicWidthInPix<<1;
	const unsigned int cui_tempB = PicWidthInPix<<2;

	_amem8(sample)=0x8080808080808080;
	_amem8(sample+PicWidthInPix)=0x8080808080808080;
	_amem8(sample+cui_tempA)=0x8080808080808080;
	_amem8(sample+cui_tempA+PicWidthInPix)=0x8080808080808080;
	_amem8(sample+cui_tempB)=0x8080808080808080;
	_amem8(sample+cui_tempB+PicWidthInPix)=0x8080808080808080;
	_amem8(sample+cui_tempA+cui_tempB)=0x8080808080808080;
	_amem8(sample+cui_tempA+cui_tempB+PicWidthInPix)= 0x8080808080808080;
#else
	int i, j;

    const int stride = PicWidthInPix >> 2; 
	unsigned int *ptr_img = (unsigned int *) sample;
    
	for ( j = 0 ; j < 8 ; j++ ) {
		for ( i = 0 ; i < 2 ; i++ ) {
			*(ptr_img + i) = 0x80808080;
        }
		ptr_img += stride;
    }
#endif
}
#endif
