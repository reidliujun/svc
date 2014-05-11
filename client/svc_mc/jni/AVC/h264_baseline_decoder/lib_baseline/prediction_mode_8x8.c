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




#include "type.h"
#include "prediction_mode_8x8.h"




/**
This function permits to get the vertical prediction samples of a block 8x8. 


@param sample The sample already decoded from the previous macroblock.
@param PicWidthInPix Width in pixel of the image.
@param Avail Parameter which indicates if the corner is available or not.
*/
void predict_8x8_v (unsigned char * sample, const short PicWidthInPix, int Avail)
{
    int j,i ;
	unsigned char l[8];
	int sample_index = - PicWidthInPix - 1; 

  
	for (i = 0; i < 8; i++, sample_index++){
		l[i] = (sample [sample_index] + 2* sample [sample_index + 1] + sample [sample_index + 2] + 2 ) >> 2;
	}

	if( !(Avail & 0x01))
		l[0] = (3 * sample [sample_index - 7] + sample [sample_index - 6] + 2 ) >> 2;

	if( !(Avail & 0x02))
		l[7] = (3 * sample [sample_index] + sample [sample_index - 1] + 2 ) >> 2;
	
	
	for ( j = 0 ; j < 8 ; j++,  sample += PicWidthInPix) {
		for ( i = 0; i < 8; i++)
			sample [i] = l[i];
	}
}





/**
This function permits to get the horizontal prediction samples of a block 8x8. 


@param sample The sample already decoded from the previous macroblock.
@param PicWidthInPix Width in pixel of the image.
@param Avail Parameter which indicates if the corner is available or not.
*/
void predict_8x8_h (unsigned char * sample, const short PicWidthInPix, int Avail)
{
    int      i=0,   j ;
	unsigned char l[8];
	int 	sample_index;

	l[0] = (( Avail & 0x01 ? sample [-1 -PicWidthInPix]: sample [-1]) + 2* sample [-1] + sample [-1 + PicWidthInPix] + 2 ) >> 2;

	sample_index = -1;
	for (i = 0; i < 6; i++, sample_index += PicWidthInPix){
		l[i+1] = (sample [sample_index] + 2* sample [sample_index + PicWidthInPix] + sample [sample_index + 2 * PicWidthInPix] + 2 ) >> 2;
	}

	l[7] = (sample [sample_index] + 3* sample [sample_index + PicWidthInPix] + 2 ) >> 2;

	//To initializate the macroblock
	for ( j = 0 ; j < 8 ; j++,  sample += PicWidthInPix) {
		for ( i = 0; i < 8; i++)
			sample [i] = l[j];
		
	}
}





/**
This function permits to get the DC prediction samples of a block 8x8. 


@param sample The sample already decoded from the previous macroblock.
@param PicWidthInPix Width in pixel of the image.
@param Avail Parameter which indicates if the corner is available or not.
*/
void predict_8x8_dc (unsigned char * sample, const short PicWidthInPix, int Avail)
{
    int    s0 = 1 << 7;
    int    i,j ;
    unsigned char l[8];
	int sample_index;

	if ( Avail & 0x08){
		l[0] = (( Avail & 0x01 ? sample [-1 -PicWidthInPix]: sample [-1]) + 2* sample [-1] + sample [-1 +PicWidthInPix] + 2 ) >> 2;

		sample_index = -1;
		for (i = 0; i < 6; i++, sample_index += PicWidthInPix){
			l[i+1] = (sample [sample_index] + 2* sample [sample_index + PicWidthInPix] + sample [sample_index + 2 * PicWidthInPix] + 2 ) >> 2;
		}

		l[7] = (sample [sample_index] + 3* sample [sample_index + PicWidthInPix] + 2 ) >> 2;


		if ( Avail & 0x04  ) {
			unsigned char t[8];

			t[0] = (( Avail & 0x01 ? sample [- PicWidthInPix - 1]: sample [- PicWidthInPix ]) + 2* sample [- PicWidthInPix ] + sample [- PicWidthInPix  +1] + 2 ) >> 2;

			sample_index = - PicWidthInPix;
			for (i = 0; i < 6; i++, sample_index ++){
				t[i+1] = (sample [sample_index] + 2* sample [sample_index + 1] + sample [sample_index + 2] + 2 ) >> 2;
			}

			t[7] = (( Avail & 0x02 ? sample [- PicWidthInPix + 8]: sample [- PicWidthInPix + 7]) +
				2*sample [- PicWidthInPix + 7] + sample [- PicWidthInPix +6] + 2 ) >> 2; 			

			// no edge
			s0 = 0;
			for ( i=0;i <8; i++)
				s0 += l[i] + t[i] ; 

			s0 = ( s0 + 8 ) >> 4 ;
		} 
		else 
		{
			s0 = 0;
			// upper edge
			for ( i = 0;i <8; i++)
				s0 += l[i];

			s0 = (s0 + 4) >> 3 ;
		}
	}
	else if ( Avail & 0x04)
	{
		unsigned char t[8];

		t[0] = (( Avail & 0x01 ? sample [- PicWidthInPix - 1]: sample [- PicWidthInPix ]) + 
			2* sample [- PicWidthInPix ] + sample [- PicWidthInPix  +1] + 2 ) >> 2;
		
		sample_index = - PicWidthInPix;
		for (i = 0; i < 6; i++, sample_index ++){
			t[i+1] = (sample [sample_index] + 2* sample [sample_index + 1] + sample [sample_index + 2] + 2 ) >> 2;
		}

		t[7] = (( Avail & 0x02 ? sample [- PicWidthInPix +8]: sample [- PicWidthInPix +7]) +
			2*sample [- PicWidthInPix + 7] + sample [- PicWidthInPix +6] + 2 ) >> 2;  

		// left edge
		s0 = 0;
		for ( i = 0; i < 8; i++)
			s0 += t[i];

		s0 = (s0 + 4 ) >> 3 ;
	}
	
	//To initializate the macroblock
    for ( j = 0 ; j < 8 ; j++, sample += PicWidthInPix ) {
		for ( i = 0; i < 8; i++)
			sample [i] = (unsigned char) s0;
    }
}








/**
This function permits to get the diagonal down left prediction samples of a block 8x8. 


@param sample The sample already decoded from the previous macroblock.
@param PicWidthInPix Width in pixel of the image.
@param Avail Parameter which indicates if the corner is available or not.
*/
void predict_8x8_ddl (unsigned char * sample, const short PicWidthInPix, int Avail)
{
	int i,j;
	unsigned char t[17];
	int sample_index = - PicWidthInPix - 1;

     	
	 for (i = 0; i < 7; i++, sample_index ++){
		 t[i] = (sample [sample_index] + 2* sample [sample_index + 1] + sample [sample_index + 2] + 2 ) >> 2;
	 }

	 if( !(Avail & 0x01))
		 t[0] = (3* sample [- PicWidthInPix] + sample [- PicWidthInPix  + 1] + 2 ) >> 2;
	

  
	 if (Avail & 0x02){
		 t[7] = (sample [- PicWidthInPix +8] + 2*sample [- PicWidthInPix + 7] + sample [- PicWidthInPix +6] + 2 ) >> 2;

		 sample_index = - PicWidthInPix + 7;
		 for (i = 0; i < 7; i++, sample_index ++){
			 t[i+8] = (sample [sample_index] + 2* sample [sample_index + 1] + sample [sample_index + 2] + 2 ) >> 2;
		 }

		 t[16] = t[15] = (sample [- PicWidthInPix +14] + 3*sample [- PicWidthInPix + 15] + 2 ) >> 2; 

	 }else{
		 t[7] = ( 3*sample [- PicWidthInPix + 7] + sample [- PicWidthInPix +6] + 2 ) >> 2;  
		 t[8] = t[9] = t[10] = t[11] = t[12] = t[13] = t[14] = t[15] = t[16] = sample [- PicWidthInPix + 7];
	 }

	 //To initializate the macroblock
	 for ( j=0; j < 8; j++, sample += PicWidthInPix){
		 for( i = 0; i < 8; i++){
			 sample[i] = ( t[i+j] + 2 * t[i+j+1] + t[i+j+2] + 2 ) >> 2 ;
		 }
	 }


}



/**
This function permits to get the diagonal down right prediction samples of a block 8x8. 


@param sample The sample already decoded from the previous macroblock.
@param PicWidthInPix Width in pixel of the image.
@param Avail Parameter which indicates if the corner is available or not.
*/
void predict_8x8_ddr (unsigned char * sample, const short PicWidthInPix, int Avail)
{
	int i,j;
	unsigned char y[17];
	int sample_index = -1;
    
	y[0] = (sample [-1 + 6 * PicWidthInPix] + 3 * sample [-1 + 7 * PicWidthInPix] + 2 ) >> 2;
	
	for (i = 0; i < 6; i++, sample_index += PicWidthInPix){
		y[6 - i] = (sample [sample_index] + 2 * sample [sample_index + PicWidthInPix] + sample [sample_index + 2 * PicWidthInPix] + 2 ) >> 2;
	}
	y[8] = ( sample [-1] + 2 * sample [-1 -PicWidthInPix ] + sample [-PicWidthInPix] +2 ) >> 2;
   

	if(Avail & 0x01){
		y[7] = (sample [-1 - PicWidthInPix] +  2* sample [-1] + sample [-1 + PicWidthInPix] + 2 ) >> 2;
		y[9] = (sample [-PicWidthInPix - 1] +  2* sample [-PicWidthInPix] + sample [-PicWidthInPix  +1] + 2 ) >> 2;
	}else{
		y[7] = (3 * sample [-1] + sample [-1 + PicWidthInPix] + 2) >> 2;
		y[9] = (3 * sample [-PicWidthInPix ] + sample [-PicWidthInPix + 1] + 2 ) >> 2;
	}
	
    
	sample_index = - PicWidthInPix;
	for (i = 0; i < 7; i++, sample_index ++){
		 y[i + 10] = (sample [sample_index] + 2* sample [sample_index + 1] + sample [sample_index + 2] + 2 ) >> 2;
	 }

    if(!(Avail & 0x02))
		y[16] = (3 * sample [-PicWidthInPix + 7] + sample [-PicWidthInPix + 6] + 2 ) >> 2;  

   
	//To initializate the macroblock
	 sample += 7 * PicWidthInPix;
	 for ( j = 0; j < 8; j++, sample -= PicWidthInPix){
		 for( i = 0; i < 8; i++){
			 sample[i] = ( y[i + j] + 2 * y[i + j + 1] + y[i + j + 2] + 2 ) >> 2 ;
		 }
	 }
}




/**
This function permits to get the vertical left prediction samples of a block 8x8. 


@param sample The sample already decoded from the previous macroblock.
@param PicWidthInPix Width in pixel of the image.
@param Avail Parameter which indicates if the corner is available or not.
*/
void predict_8x8_vr (unsigned char * sample, const short PicWidthInPix, int Avail){


	unsigned int i,j;
    unsigned char y[16];
	int sample_index = -1;


	for (i = 0; i < 6; i++, sample_index += PicWidthInPix){
		y[5 - i] = (sample [sample_index] + 2* sample [sample_index + PicWidthInPix] + sample [sample_index + 2 * PicWidthInPix] + 2 ) >> 2;
	}

	if (Avail & 0x01){
		y[6] = (sample [-1 -PicWidthInPix] + 2* sample [-1] + sample [-1 + PicWidthInPix] + 2) >> 2;	
		y[7] = ( sample [-1] + 2* sample [-1 - PicWidthInPix] + sample [-PicWidthInPix] +2) >> 2;
		y[8] = (sample [-PicWidthInPix - 1] + 2* sample [-PicWidthInPix ] + sample [-PicWidthInPix  +1] + 2) >> 2;
	}else{
		y[6] = (3* sample [-1] + sample [-1 + PicWidthInPix] + 2) >> 2;	
		y[7] = ( sample [-1] + 2* sample [-1 - PicWidthInPix] + sample [-PicWidthInPix] + 2) >> 2;
		y[8] = (3* sample [-PicWidthInPix] + sample [-PicWidthInPix  +1] + 2) >> 2;
	}
	

	sample_index = -PicWidthInPix;
	for (i = 0; i < 7; i++, sample_index ++){
		y[9 + i] = (sample [sample_index] + 2* sample [sample_index + 1] + sample [sample_index + 2] + 2 ) >> 2;
	}

	if(!(Avail & 0x02))
		y[15] = ( 3*sample [-PicWidthInPix + 7] + sample [-PicWidthInPix +6] + 2 ) >> 2;  



	//To initializate the top left to the bottom left of the macroblock

	//2
	sample_index = 2*PicWidthInPix;
	for (j = 0; j < 6; j++){
			sample[sample_index + j * PicWidthInPix] = (y[5 - j] + 2 * y[6 - j] + y[7 - j] + 2) >>2 ;
	}

	//4+1
	sample_index += 2 * PicWidthInPix + 1;
	for (j = 0; j < 4; j++){
			sample[sample_index + j * PicWidthInPix] = (y[5 - j] + 2 * y[6 - j] + y[7 - j] + 2) >>2 ;
	}
	
	//6+2
	sample_index += 2 * PicWidthInPix + 1;
	for (j = 0; j < 2; j++){
			sample[sample_index + j * PicWidthInPix] = (y[5 - j] + 2 * y[6 - j] + y[7 - j] + 2) >>2 ;
	}

	
	for ( j=0; j < 8; j++){
		sample[j] = (y[7 + j] + y[8 + j] + 1) >> 1;
	}
	
	//1
	sample += PicWidthInPix;
	for ( j = 0; j < 8; j++){
		sample[j] = (y[6 + j] + 2 * y[7 + j] + y[8 + j] + 2) >> 2;
	}
	

	//2+1
	sample += PicWidthInPix + 1;
	for (j = 0; j < 7; j++){
		sample[j] = (y[7 + j] + y[8 + j] + 1) >> 1;
	}
	
	//3+1
	sample += PicWidthInPix;
	for ( j = 0; j < 7; j++){
		sample[j] = (y[6 + j] + 2 *  y[7 + j] + y[8 + j] + 2) >> 2;
	}


	//4+2
	sample += PicWidthInPix + 1;
	for ( j = 0; j < 6; j++){
		sample[j] = (y[7 + j] + y[8 + j] + 1) >> 1;
	}

	//5+2
	sample += PicWidthInPix;
	for ( j = 0; j < 6; j++){
		sample[j] = (y[6 + j] + 2 * y[7 + j] + y[8 + j] + 2) >> 2;
	}
	
	//6+3
	sample += PicWidthInPix + 1;
	for ( j = 0; j < 5; j++){
		sample[j] = (y[7 + j] + y[8 + j] + 1) >> 1;
	}


	//7+3
	sample += PicWidthInPix;
	for ( j = 0; j < 5; j++){
		sample[j] = (y[6 + j] + 2*y[7 + j] + y[8 +j ] + 2) >> 2;
	}
}




/**
This function permits to get the horizontal down prediction samples of a block 8x8. 


@param sample The sample already decoded from the previous macroblock.
@param PicWidthInPix Width in pixel of the image.
@param Avail Parameter which indicates if the corner is available or not.
*/
void predict_8x8_hd (unsigned char * sample, const short PicWidthInPix, int Avail)
{
	unsigned int i,j;
	int sample_index;
    unsigned char y[17];
	
	y[0] = (sample [-1 +6*PicWidthInPix] + 3* sample [-1 +7*PicWidthInPix] + 2 ) >> 2;

	sample_index = -1;
	for (i = 0; i < 6; i++, sample_index += PicWidthInPix){
		y[6-i] = (sample [sample_index] + 2* sample [sample_index + PicWidthInPix] + sample [sample_index + 2 * PicWidthInPix] + 2 ) >> 2;
	}

	y[8] = ( sample [-1] + 2 * sample [-1 - PicWidthInPix ] + sample [-PicWidthInPix] + 2) >> 2;

	if ( Avail & 0x01){
		y[9] = (sample [-PicWidthInPix - 1] + 2* sample [-PicWidthInPix] + sample [-PicWidthInPix + 1] + 2 ) >> 2;
		y[7] = (sample [-1 - PicWidthInPix] +  2* sample [-1] + sample [-1+PicWidthInPix] + 2 ) >> 2;
	}
	else{
		y[9] = (3* sample [-PicWidthInPix] + sample [-PicWidthInPix + 1] + 2 ) >> 2;
		y[7] = (3* sample [-1] + sample [-1 + PicWidthInPix] + 2 ) >> 2;
	}
   

	sample_index = - PicWidthInPix;
	for (i = 0; i < 6; i++, sample_index ++){
		y[10+i] = (sample [sample_index] + 2 * sample [sample_index + 1] + sample [sample_index + 2] + 2) >> 2;
	}

    y[16] = (sample [-PicWidthInPix + 5] + 2 * sample [-PicWidthInPix + 6] + sample [-PicWidthInPix + 7] + 2 ) >> 2;


	//To initializate the top left to the bottom right of the macroblock every two lines
	sample_index = 0;
	for(j = 0; j < 8; j++, sample_index += PicWidthInPix){
		sample [sample_index] =(y[8 - j] + y[7 - j] + 1) >> 1;
	}

	sample_index = PicWidthInPix + 2;
	for(j = 0; j < 7; j++, sample_index += PicWidthInPix){
		sample [sample_index] =(y[8 - j] + y[7 - j] + 1) >> 1;
	}
	sample_index = 2 * PicWidthInPix + 4;
	for(j = 0; j < 6; j++, sample_index += PicWidthInPix){
		sample [sample_index] =(y[8 - j] + y[7 - j] + 1) >> 1;
	}
	sample_index = 3 * PicWidthInPix + 6;
	for(j = 0; j < 5; j++, sample_index += PicWidthInPix){
		sample [sample_index] =(y[8 - j] + y[7 - j] + 1) >> 1;
	}



	//To initializate the top left to the bottom right of the macroblock every other two lines
	sample_index = PicWidthInPix + 1;
	for(j = 0; j < 7; j++, sample_index += PicWidthInPix){
		sample [sample_index] = (y[8 - j] + 2 * y[7 - j] + y[6 - j] + 2) >> 2;
	}

	sample_index = 2 * PicWidthInPix + 3;
	for(j = 0; j < 6; j++, sample_index += PicWidthInPix){
		sample [sample_index] = (y[8 - j] + 2 * y[7 - j] + y[6 - j] + 2) >> 2;
	}

	sample_index = 3 * PicWidthInPix + 5;
	for(j = 0; j < 5; j++, sample_index += PicWidthInPix){
		sample [sample_index] = (y[8 - j] + 2 * y[7 - j] + y[6 - j] + 2) >> 2;
	}

	sample_index = 4 * PicWidthInPix + 7;
	for(j = 0; j < 4; j++, sample_index += PicWidthInPix){
		sample [sample_index] = (y[8 - j] + 2 * y[7 - j] + y[6 - j] + 2) >> 2;
	}
	
	
	//To initializate the top left to the top right of the macroblock
	sample_index = 1;
	for(j = 0; j < 7; j++, sample_index ++){
		sample [sample_index] = (y[9 + j] + 2 * y[8 + j] + y[7 + j] + 2) >> 2;
	}

	sample_index = 3 + PicWidthInPix;
	for(j = 0; j < 5; j++, sample_index ++){
		sample [sample_index] = (y[9 + j] + 2 * y[8 + j] + y[7 + j] + 2) >> 2;
	}

	sample_index = 5 + 2 * PicWidthInPix;
	for(j = 0; j < 3; j++, sample_index ++){
		sample [sample_index] = (y[9 + j] + 2 * y[8 + j] + y[7 + j] + 2) >> 2;
	}

	sample [7 + 3 * PicWidthInPix] = (y[9] + 2*y[8] + y[7] +2) >> 2;
	
	
		
}




/**
This function permits to get the vertical left prediction samples of a block 8x8. 


@param sample The sample already decoded from the previous macroblock.
@param PicWidthInPix Width in pixel of the image.
@param Avail Parameter which indicates if the corner is available or not.
*/
void predict_8x8_vl (unsigned char * sample, const short PicWidthInPix, int Avail)
{

	int i,j;
	int sample_index = - PicWidthInPix - 1;
	unsigned char t[13];
   
	
	for (i = 0; i < 7; i++, sample_index ++){
		t[i] = (sample [sample_index] + 2* sample [sample_index + 1] + sample [sample_index + 2] + 2 ) >> 2;
	}

	if (!(Avail & 0x01))
		t[0] = (3 * sample [-PicWidthInPix] + sample [-PicWidthInPix + 1] + 2 ) >> 2;


	if (Avail & 0x02){
		sample_index = - PicWidthInPix + 6;
		for(i = 0; i < 6; i++, sample_index++){
			t[7+i] = (sample[sample_index] + 2 * sample[sample_index + 1] + sample[sample_index + 2] + 2) >>2;
		}

	}else{
		t[7] = (3 * sample [-PicWidthInPix + 7] + sample [-PicWidthInPix + 6] + 2) >> 2; 
		t[8] = t[9] = t[10] = t[11] = t[12] = sample [-PicWidthInPix + 7];
	}

	//To initializate all the macroblock
	for (i = 0; i < 4; i++, sample += 2* PicWidthInPix){
		for (j = 0; j < 8; j++){
			sample[j] = (t[i + j] + t[i + j + 1] + 1) >> 1;
			sample[PicWidthInPix + j] = (t[i + j] + 2 * t[i + j + 1] +t[i + j + 2] + 2) >> 2;
		}
	}

}



/**
This function permits to get the horizontal up prediction samples of a block 8x8. 


@param sample The sample already decoded from the previous macroblock.
@param PicWidthInPix Width in pixel of the image.
@param Avail Parameter which indicates if the corner is available or not.
*/
void predict_8x8_hu (unsigned char * sample, const short PicWidthInPix, int Avail)
{
	int i,j;
	unsigned char l[9];
	int sample_index;
  
    l[0] = (( Avail & 0x01 ? sample [-1 - PicWidthInPix]: sample [-1 ]) +      2* sample [-1] + sample [-1 + PicWidthInPix] + 2 ) >> 2;
   
	sample_index = -1;
	for (i = 0; i < 6; i++, sample_index += PicWidthInPix){
		l[i+1] = (sample [sample_index] + 2* sample [sample_index + PicWidthInPix] + sample [sample_index + 2 * PicWidthInPix] + 2 ) >> 2;
	}
    l[8] = l[7] = (sample [-1 +6*PicWidthInPix] + 3* sample [-1 +7*PicWidthInPix] + 2 ) >> 2;


	//To initializate the first line and every two lines
	sample_index = 0;
	for(j = 0; j < 8; j++, sample_index += PicWidthInPix){
		sample[sample_index] = (l[j] + l[j + 1] + 1) >> 1;
	}

	sample_index = 2;
	for(j = 0; j < 7; j++,sample_index += PicWidthInPix){
		sample[sample_index] = (l[j + 1] + l[j + 2] + 1) >> 1;
	}

	sample_index = 4;
	for(j = 0; j < 6; j++, sample_index += PicWidthInPix){
			sample[sample_index] = (l[2 + j] + l[j + 3] + 1) >> 1;
	}
	sample_index = 6;
	for(j = 0; j < 5; j++, sample_index += PicWidthInPix){
			sample[sample_index] = (l[3 + j] + l[j + 4] + 1) >> 1;
	}
	
	//To initializate the second line and every two lines
	sample_index = 1;
	for(j = 0; j < 8;  j++, sample_index += PicWidthInPix){
		sample[sample_index] = (l[j] + 2*l[j + 1] + l[j + 2] + 2) >> 2;
	}
	sample_index = 3;
	for(j = 0; j < 7; j++, sample_index += PicWidthInPix){
		sample[sample_index] = (l[1 + j] + 2*l[2 + j] + l[j + 3] + 2) >> 2;
	}
	sample_index = 5;
	for(j = 0; j < 6; j++, sample_index += PicWidthInPix){
		sample[sample_index] = (l[2 + j] + 2*l[j + 3] + l[j + 4] + 2) >> 2;
	}
	sample_index = 7;
	for(j = 0; j < 5; j++, sample_index += PicWidthInPix){
		sample[sample_index] = (l[3 + j] + 2*l[j + 4] + l[j + 5] + 2) >> 2;
	}
	
	//To initializate the bottom right of the macroblock
	sample_index = 4 * PicWidthInPix + 6;
	for(j = 0; j < 2; j++){
		sample[sample_index + j] = l[7];
	}

	sample_index += PicWidthInPix - 2;
	for(j = 0; j < 4; j++){
		sample[sample_index + j] = l[7];
	}

	sample_index += PicWidthInPix - 2;
	for(j = 0; j < 6; j++){
		sample[sample_index + j] = l[7];
	}

	sample_index += PicWidthInPix - 2;
	for(j = 0; j < 8; j++){
		sample[sample_index + j] = l[7];
	}
}
