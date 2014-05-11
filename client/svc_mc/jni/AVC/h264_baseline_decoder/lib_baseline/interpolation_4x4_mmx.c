/*****************************************************************************
   *
   *  Open SVC Decoder developped in IETR image lab
   *
   *
   *
   *              Médéric BLESTEL <mblestel@insa-rennes.Fr>
   *              Mickael RAULET <mraulet@insa-rennes.Fr>
   *			  Maxime  PELCAT <mpelcat@insa-rennes.Fr>
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


#ifdef MMXi


#include "interpolation_mmx.h"


// Interpolation n_0

void luma_sample_interp_0_0_MMX(unsigned char *image, unsigned char *refPicLXl,	const short PicWidthSamples, const short stride){

#ifdef POCKET_PC

	int j;
	/* fullpel position */

	for (j = 0; j < 4; j++){

		*((unsigned int*) image) = (refPicLXl[0]) + (refPicLXl[1] << 8) + (refPicLXl[2] << 16) + (refPicLXl[3] << 24);
		image += stride;
		refPicLXl += PicWidthSamples;
	}
#else
	int j;
	for (j = 0; j < 4; j++){
		*((unsigned int *) image) = *((unsigned int *) refPicLXl) ;
		image += stride;
		refPicLXl += PicWidthSamples;
	}

#endif

}



void luma_sample_interp_1_0_MMX(unsigned char* sortie, unsigned char* image, const short PicWidthInPix,const short OutStride){

	
	int i;
	unsigned int *ptr_out = (unsigned int *)sortie;

	for(i = 0; i < 4; i++, image += PicWidthInPix,ptr_out += (OutStride >> 2))
		*ptr_out = interpolhline_mmx_1(image); 
}



void luma_sample_interp_2_0_MMX(unsigned char* sortie, unsigned char* image, const short PicWidthInPix,const short OutStride){


	
	int i;
	unsigned int *ptr_out = (unsigned int  *)sortie;

	for(i = 0; i < 4; i++, image += PicWidthInPix,ptr_out += (OutStride >> 2))
		*ptr_out = interpolhline_mmx_2(image);

}


void luma_sample_interp_3_0_MMX(unsigned char* sortie, unsigned char* image,const short PicWidthInPix,const short OutStride){

	int i;
	unsigned int *ptr_out = (unsigned int  *)sortie;


	for(i = 0; i < 4; i++, image += PicWidthInPix,ptr_out += (OutStride >> 2))
		*ptr_out = interpolhline_mmx_3(image);

}









// Interpolation 0_n


void luma_sample_interp_0_1_MMX(unsigned char* sortie, unsigned char* image,const short PicWidthInPix,const short OutStride){

	int i;
	unsigned int *ptr_out = (unsigned int  *)sortie;

	for(i = 0; i < 4; i++, image += PicWidthInPix,ptr_out += (OutStride >> 2))
		*ptr_out = interpolvline_mmx_1(image,image, PicWidthInPix);

}





void luma_sample_interp_0_2_MMX(unsigned char* sortie, unsigned char* image,const short PicWidthInPix,const short OutStride){

	int i;
	unsigned int *ptr_out = (unsigned int  *)sortie;

	for(i = 0; i < 4; i++, image += PicWidthInPix,ptr_out += (OutStride >> 2))
		*ptr_out = interpolvline_mmx_2(image,PicWidthInPix);

}




void luma_sample_interp_0_3_MMX(unsigned char* sortie, unsigned char* image,const short PicWidthInPix,const short OutStride){

	int i;
	unsigned int *ptr_out = (unsigned int  *)sortie;

	for(i = 0; i < 4; i++, image += PicWidthInPix,ptr_out += (OutStride >> 2))
		*ptr_out = interpolvline_mmx_3(image,PicWidthInPix);
}





//After this, check if it's faster




// Interpolation 2_n

void luma_sample_interp_2_1_MMX(unsigned char* sortie, unsigned char* image,const short PicWidthInPix,const short OutStride){

	int i;
	__m64 temp[9];
	unsigned char *ptr_ref = &image[-((PicWidthInPix + 1) << 1)];


	for(i = 0; i < 9; i++, ptr_ref += PicWidthInPix){
		temp[i] = interpolhline64_mmx(ptr_ref);
		empty(); 
	}


	for(i = 0; i < 4; i++, sortie += OutStride)	{
		*((unsigned int *) sortie) = interpolvline64_1_mmx(temp + i);
	}
}





void luma_sample_interp_2_2_MMX(unsigned char* sortie, unsigned char* image,const short PicWidthInPix,const short OutStride){

	int i;
	unsigned char *ptr_ref = &image[-((PicWidthInPix + 1) << 1)];
	__m64 temp[9];

	for(i = 0; i < 9; i++, ptr_ref += PicWidthInPix){
		temp[i] = interpolhline64_mmx(ptr_ref);
		empty(); 
	}


	for(i = 0; i < 4; i++, sortie += OutStride)	{
		*((unsigned int *) sortie) = interpolvline64_2_mmx(temp + i);
	}

}





void luma_sample_interp_2_3_MMX(unsigned char* sortie, unsigned char* image,const short PicWidthInPix,const short OutStride){

	int i;
	unsigned char *ptr_ref = &image[-((PicWidthInPix + 1) << 1)];
	__m64 temp[9];

	for(i = 0; i < 9; i++, ptr_ref += PicWidthInPix){
		temp[i] = interpolhline64_mmx(ptr_ref);
		empty(); 
	}


	for(i = 0; i < 4; i++, sortie += OutStride)	{
		*((unsigned int *) sortie) = interpolvline64_3_mmx(temp + i);
	}

}







// Interpolation n_2

void luma_sample_interp_1_2_MMX(unsigned char* sortie, unsigned char* image,const short PicWidthInPix,const short OutStride){
	
	__m64 temp[9];
	__m64 img[4];
	__m64 temp0, temp1,temp2,temp3;
	
	int i;
	for(i = 0; i < 9;i++){
		temp[i] = interpolvline64_mmx(image - 2 +  i,PicWidthInPix);
		empty(); 
	}

	for(i = 0; i < 4; i++)	{
		 img[i] = interpolhline64_1_mmx(&temp[i]);
		empty(); 
	}
	

	temp0 = _mm_set_pi8(0,0,0,0,_mm_extract_pi16(img[3],0),_mm_extract_pi16(img[2],0),_mm_extract_pi16(img[1],0),_mm_extract_pi16(img[0],0));
	*((unsigned int *) sortie) = _mm_cvtsi64_si32(temp0);
	temp1 = _mm_set_pi8(0,0,0,0,_mm_extract_pi16(img[3],1),_mm_extract_pi16(img[2],1),_mm_extract_pi16(img[1],1),_mm_extract_pi16(img[0],1));
	*((unsigned int *) (&sortie[OutStride])) = _mm_cvtsi64_si32(temp1);
	temp2 = _mm_set_pi8(0,0,0,0,_mm_extract_pi16(img[3],2),_mm_extract_pi16(img[2],2),_mm_extract_pi16(img[1],2),_mm_extract_pi16(img[0],2));
	*((unsigned int *) (&sortie[OutStride << 1])) = _mm_cvtsi64_si32(temp2);
	temp3 = _mm_set_pi8(0,0,0,0,_mm_extract_pi16(img[3],3),_mm_extract_pi16(img[2],3),_mm_extract_pi16(img[1],3),_mm_extract_pi16(img[0],3));
	*((unsigned int *) (&sortie[OutStride * 3])) = _mm_cvtsi64_si32(temp3);

	empty(); 
}




void luma_sample_interp_3_2_MMX(unsigned char* sortie, unsigned char* image,const short PicWidthInPix,const short OutStride){
	
	__m64 temp[9];
	__m64 img[4];
	__m64 temp0, temp1,temp2,temp3;
	
	int i;
	for(i = 0; i < 9;i++){
		temp[i] = interpolvline64_mmx(image - 2 +  i,PicWidthInPix);
		empty(); 
	}

	for(i = 0; i < 4; i++)	{
		 img[i] = interpolhline64_3_mmx(&temp[i]);
		empty(); 
	}
	

	temp0 = _mm_set_pi8(0,0,0,0,_mm_extract_pi16(img[3],0),_mm_extract_pi16(img[2],0),_mm_extract_pi16(img[1],0),_mm_extract_pi16(img[0],0));
	*((unsigned int *) sortie) = _mm_cvtsi64_si32(temp0);
	temp1 = _mm_set_pi8(0,0,0,0,_mm_extract_pi16(img[3],1),_mm_extract_pi16(img[2],1),_mm_extract_pi16(img[1],1),_mm_extract_pi16(img[0],1));
	*((unsigned int *) (&sortie[OutStride])) = _mm_cvtsi64_si32(temp1);
	temp2 = _mm_set_pi8(0,0,0,0,_mm_extract_pi16(img[3],2),_mm_extract_pi16(img[2],2),_mm_extract_pi16(img[1],2),_mm_extract_pi16(img[0],2));
	*((unsigned int *) (&sortie[OutStride << 1])) = _mm_cvtsi64_si32(temp2);
	temp3 = _mm_set_pi8(0,0,0,0,_mm_extract_pi16(img[3],3),_mm_extract_pi16(img[2],3),_mm_extract_pi16(img[1],3),_mm_extract_pi16(img[0],3));
	*((unsigned int *) (&sortie[OutStride * 3])) = _mm_cvtsi64_si32(temp3);

	empty(); 
}






void luma_sample_interp_1_1_MMX(unsigned char* sortie, unsigned char* image,const short PicWidthInPix,const short OutStride){


	int i;
	
	unsigned int *ptr = (unsigned int  *)sortie;
	for(i = 0; i < 4; i++, image += PicWidthInPix,ptr += (OutStride >> 2)){
		*ptr = interpolhline_mmx_2(image);
		*ptr = interpolvline_mmx_1(image, (unsigned char *)ptr, PicWidthInPix);
	}
}


void luma_sample_interp_1_3_MMX(unsigned char* sortie, unsigned char* image,const short PicWidthInPix,const short OutStride){


	int i;
	
	unsigned int *ptr = (unsigned int  *)sortie;
	for(i = 0; i < 4; i++, image += PicWidthInPix,ptr += (OutStride >> 2)){
		*ptr = interpolhline_mmx_2(image + PicWidthInPix);
		*ptr = interpolvline_mmx_1(image, (unsigned char *)ptr, PicWidthInPix);
	}
}


void luma_sample_interp_3_1_MMX(unsigned char* sortie, unsigned char* image,const short PicWidthInPix,const short OutStride){


	int i;
	
	unsigned int *ptr = (unsigned int  *)sortie;
	for(i = 0; i < 4; i++, image += PicWidthInPix,ptr += (OutStride >> 2)){
		*ptr = interpolhline_mmx_2(image);
		*ptr = interpolvline_mmx_1(image + 1, (unsigned char *)ptr, PicWidthInPix);
	}
}

void luma_sample_interp_3_3_MMX(unsigned char* sortie, unsigned char* image,const short PicWidthInPix,const short OutStride){


	int i;
	
	unsigned int *ptr = (unsigned int  *)sortie;
	for(i = 0; i < 4; i++, image += PicWidthInPix,ptr += (OutStride >> 2)){
		*ptr = interpolhline_mmx_2(image + PicWidthInPix);
		*ptr = interpolvline_mmx_1(image + 1, (unsigned char *)ptr, PicWidthInPix);
	}
}
#endif
