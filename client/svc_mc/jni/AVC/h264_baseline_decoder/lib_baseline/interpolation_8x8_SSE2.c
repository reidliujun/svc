/*****************************************************************************
   *
   *  Open SVC Decoder developped in IETR image lab
   *
   *
   *
   *              Médéric BLESTEL <mblestel@insa-rennes.Fr>
   *              Mickael RAULET <mraulet@insa-rennes.Fr>
   *			  Alain MACCARI <alain.maccari@ens.insa-rennes.fr>
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

#ifdef SSE2

#include "type.h"


///////////////////////// SSE2 interpolation //////////////////////////////
///////////////////////////////////////////////////////////////////////////


 __m64 interpolhline_1(unsigned char* image){

	__m128i xmm0,xmm1,xmm2,xmm3,xmm4,xmm5,xmm6,xmm7;
	unsigned char* imagetmp = image - 2;
	__m64 ret;

	xmm7 = _mm_setzero_si128();
	xmm6 = _mm_loadu_si128(((__m128i*)imagetmp));


	xmm0 = _mm_unpacklo_epi8(xmm6,xmm7);
	xmm6 = _mm_srli_si128(xmm6,1);
	xmm1 = _mm_unpacklo_epi8(xmm6,xmm7);
	xmm6 = _mm_srli_si128(xmm6,1);
	xmm2 = _mm_unpacklo_epi8(xmm6,xmm7);
	xmm6 = _mm_srli_si128(xmm6,1);
	xmm3 = _mm_unpacklo_epi8(xmm6,xmm7);
	xmm6 = _mm_srli_si128(xmm6,1);
	xmm4 = _mm_unpacklo_epi8(xmm6,xmm7);
	xmm6 = _mm_srli_si128(xmm6,1);
	xmm5 = _mm_unpacklo_epi8(xmm6,xmm7);

// filter on 8 values
	xmm6 = _mm_add_epi16(xmm2,xmm3);//(C + D)
	xmm6 = _mm_slli_epi16(xmm6,2);//(C + D) << 2
	xmm6 = _mm_sub_epi16(xmm6,xmm1);//((C + D) << 2) - B
	xmm6 = _mm_sub_epi16(xmm6,xmm4);//((C + D) << 2) - B - E

	xmm1 = _mm_set_epi32(0x00050005,0x00050005,0x00050005,0x00050005);
	xmm6 = _mm_mullo_epi16(xmm6,xmm1);//(((C + D) << 2) - B - E) * 5
	xmm6 = _mm_add_epi16(xmm6,xmm0);//((((C + D) << 2) - B - E) * 5) + A
	xmm6 = _mm_add_epi16(xmm6,xmm5);//((((C + D) << 2) - B - E) * 5) + A + F
	xmm6 = _mm_add_epi16(xmm6,_mm_set_epi32(0x00100010,0x00100010,0x00100010,0x00100010));//((((C + D) << 2) - B - E) * 5) + A + F + 16
	xmm6 = _mm_max_epi16(xmm6, xmm7); // preventing negative values Clip255_16
	xmm6 = _mm_srli_epi16(xmm6,5); // result0 >> 5

	xmm2 = _mm_packus_epi16(xmm2,xmm7);
	xmm3 = _mm_packus_epi16(xmm3,xmm7);
	xmm6 = _mm_packus_epi16(xmm6,xmm7);

	xmm5 = _mm_unpacklo_epi8(xmm2,xmm6);
	xmm4 = _mm_unpacklo_epi8(xmm6,xmm3);
	xmm6 = _mm_avg_epu8(xmm4,xmm5);

	xmm6 = _mm_slli_epi16(xmm6,8);
	xmm6 = _mm_srli_epi16(xmm6,8);
	xmm6 = _mm_packus_epi16(xmm6,xmm7);

	ret = _mm_movepi64_pi64(xmm6);
	_mm_empty(); 

	return(ret);
}



 __m64 interpolhline_2(unsigned char* image){

	__m128i xmm0,xmm1,xmm2,xmm3,xmm4,xmm5,xmm6,xmm7;
	unsigned char* imagetmp = image - 2;
	__m64 ret;

	xmm7 = _mm_setzero_si128();
	xmm6 =  _mm_loadu_si128(((__m128i*)imagetmp));


	xmm0 = _mm_unpacklo_epi8(xmm6,xmm7);
	xmm6 = _mm_srli_si128(xmm6,1);
	xmm1 = _mm_unpacklo_epi8(xmm6,xmm7);
	xmm6 = _mm_srli_si128(xmm6,1);
	xmm2 = _mm_unpacklo_epi8(xmm6,xmm7);
	xmm6 = _mm_srli_si128(xmm6,1);
	xmm3 = _mm_unpacklo_epi8(xmm6,xmm7);
	xmm6 = _mm_srli_si128(xmm6,1);
	xmm4 = _mm_unpacklo_epi8(xmm6,xmm7);
	xmm6 = _mm_srli_si128(xmm6,1);
	xmm5 = _mm_unpacklo_epi8(xmm6,xmm7);

// filter on 8 values
	xmm6 = _mm_add_epi16(xmm2,xmm3);
	xmm6 = _mm_slli_epi16(xmm6,2);
	xmm6 = _mm_sub_epi16(xmm6,xmm1);
	xmm6 = _mm_sub_epi16(xmm6,xmm4);

	xmm1 = _mm_set_epi32(0x00050005,0x00050005,0x00050005,0x00050005);
	xmm6 = _mm_mullo_epi16(xmm6,xmm1);
	xmm6 = _mm_add_epi16(xmm6,xmm0);
	xmm6 = _mm_add_epi16(xmm6,xmm5);
	xmm6 = _mm_add_epi16(xmm6,_mm_set_epi32(0x00100010,0x00100010,0x00100010,0x00100010));
	xmm6 = _mm_max_epi16(xmm6, xmm7); // preventing negative values
	xmm6 = _mm_srli_epi16(xmm6,5);

	xmm6 = _mm_packus_epi16(xmm6,xmm7);
	
	ret = _mm_movepi64_pi64(xmm6);
	_mm_empty(); 

	return(ret);
}



 __m64 interpolhline_3(unsigned char* image){

	__m128i xmm0,xmm1,xmm2,xmm3,xmm4,xmm5,xmm6,xmm7;
	unsigned char* imagetmp = image - 2;
	__m64 ret;

	xmm7 = _mm_setzero_si128();
	xmm6 =  _mm_loadu_si128(((__m128i*)imagetmp));


	xmm0 = _mm_unpacklo_epi8(xmm6,xmm7);
	xmm6 = _mm_srli_si128(xmm6,1);
	xmm1 = _mm_unpacklo_epi8(xmm6,xmm7);
	xmm6 = _mm_srli_si128(xmm6,1);
	xmm2 = _mm_unpacklo_epi8(xmm6,xmm7);
	xmm6 = _mm_srli_si128(xmm6,1);
	xmm3 = _mm_unpacklo_epi8(xmm6,xmm7);
	xmm6 = _mm_srli_si128(xmm6,1);
	xmm4 = _mm_unpacklo_epi8(xmm6,xmm7);
	xmm6 = _mm_srli_si128(xmm6,1);
	xmm5 = _mm_unpacklo_epi8(xmm6,xmm7);

// filter on 8 values
	xmm6 = _mm_add_epi16(xmm2,xmm3);
	xmm6 = _mm_slli_epi16(xmm6,2);
	xmm6 = _mm_sub_epi16(xmm6,xmm1);
	xmm6 = _mm_sub_epi16(xmm6,xmm4);

	xmm1 = _mm_set_epi32(0x00050005,0x00050005,0x00050005,0x00050005);
	xmm6 = _mm_mullo_epi16(xmm6,xmm1);
	xmm6 = _mm_add_epi16(xmm6,xmm0);
	xmm6 = _mm_add_epi16(xmm6,xmm5);
	xmm6 = _mm_add_epi16(xmm6,_mm_set_epi32(0x00100010,0x00100010,0x00100010,0x00100010));
	xmm6 = _mm_max_epi16(xmm6, xmm7); // preventing negative values
	xmm6 = _mm_srli_epi16(xmm6,5);

	xmm2 = _mm_packus_epi16(xmm2,xmm7);
	xmm3 = _mm_packus_epi16(xmm3,xmm7);
	xmm6 = _mm_packus_epi16(xmm6,xmm7);

	xmm5 = _mm_unpacklo_epi8(xmm2,xmm6);
	xmm4 = _mm_unpacklo_epi8(xmm6,xmm3);
	xmm6 = _mm_avg_epu8(xmm4,xmm5);

	xmm6 = _mm_srli_epi16(xmm6,8);
	xmm6 = _mm_packus_epi16(xmm6,xmm7);
		
	ret = _mm_movepi64_pi64(xmm6);
	_mm_empty(); 

	return(ret);
}






 __m128i interpolhline128(unsigned char* image){

	__m128i xmm0,xmm1,xmm2,xmm3,xmm4,xmm5,xmm6,xmm7;
	unsigned char* imagetmp = image - 2;

	xmm7 = _mm_setzero_si128();
	xmm6 =  _mm_loadu_si128(((__m128i*)imagetmp));


	xmm0 = _mm_unpacklo_epi8(xmm6,xmm7);
	xmm6 = _mm_srli_si128(xmm6,1);
	xmm1 = _mm_unpacklo_epi8(xmm6,xmm7);
	xmm6 = _mm_srli_si128(xmm6,1);
	xmm2 = _mm_unpacklo_epi8(xmm6,xmm7);
	xmm6 = _mm_srli_si128(xmm6,1);
	xmm3 = _mm_unpacklo_epi8(xmm6,xmm7);
	xmm6 = _mm_srli_si128(xmm6,1);
	xmm4 = _mm_unpacklo_epi8(xmm6,xmm7);
	xmm6 = _mm_srli_si128(xmm6,1);
	xmm5 = _mm_unpacklo_epi8(xmm6,xmm7);

// filter on 8 values
	xmm6 = _mm_add_epi16(xmm2,xmm3);
	xmm6 = _mm_slli_epi16(xmm6,2);
	xmm6 = _mm_sub_epi16(xmm6,xmm1);
	xmm6 = _mm_sub_epi16(xmm6,xmm4);

	xmm1 = _mm_set_epi32(0x00050005,0x00050005,0x00050005,0x00050005);
	xmm6 = _mm_mullo_epi16(xmm6,xmm1);
	xmm6 = _mm_add_epi16(xmm6,xmm0);
	xmm6 = _mm_add_epi16(xmm6,xmm5);
	//xmm6 = _mm_max_epi16(xmm6, xmm7); // preventing negative values

	_mm_empty(); 
	return(xmm6);
}




 __m64 interpolvline_1(	unsigned char* image,	int PicWidthInPix){
	
	 
	 
	__m128i xmm0,xmm1,xmm2,xmm3,xmm4,xmm5,xmm6,xmm7;

	__m64 ret;

	xmm7 = _mm_setzero_si128();

	xmm0 = _mm_movpi64_epi64(*((__m64*)(image - 2*PicWidthInPix)));
	xmm0 = _mm_unpacklo_epi8(xmm0,xmm7);
	xmm1 = _mm_movpi64_epi64(*((__m64*)(image - 1*PicWidthInPix)));
	xmm1 = _mm_unpacklo_epi8(xmm1,xmm7);
	xmm2 = _mm_movpi64_epi64(*((__m64*)(image - 0*PicWidthInPix)));
	xmm2 = _mm_unpacklo_epi8(xmm2,xmm7);
	xmm3 = _mm_movpi64_epi64(*((__m64*)(image + 1*PicWidthInPix)));
	xmm3 = _mm_unpacklo_epi8(xmm3,xmm7);
	xmm4 = _mm_movpi64_epi64(*((__m64*)(image + 2*PicWidthInPix)));
	xmm4 = _mm_unpacklo_epi8(xmm4,xmm7);
	xmm5 = _mm_movpi64_epi64(*((__m64*)(image + 3*PicWidthInPix)));
	xmm5 = _mm_unpacklo_epi8(xmm5,xmm7);

// filter on 8 values
	xmm6 = _mm_add_epi16(xmm2,xmm3);
	xmm6 = _mm_slli_epi16(xmm6,2);
	xmm6 = _mm_sub_epi16(xmm6,xmm1);
	xmm6 = _mm_sub_epi16(xmm6,xmm4);

	xmm1 = _mm_set_epi32(0x00050005,0x00050005,0x00050005,0x00050005);
	xmm6 = _mm_mullo_epi16(xmm6,xmm1);
	xmm6 = _mm_add_epi16(xmm6,xmm0);
	xmm6 = _mm_add_epi16(xmm6,xmm5);
	xmm6 = _mm_add_epi16(xmm6,_mm_set_epi32(0x00100010,0x00100010,0x00100010,0x00100010));
	xmm6 = _mm_max_epi16(xmm6, xmm7); // preventing negative values
	xmm6 = _mm_srli_epi16(xmm6,5);

	xmm2 = _mm_packus_epi16(xmm2,xmm7);
	xmm3 = _mm_packus_epi16(xmm3,xmm7);
	xmm6 = _mm_packus_epi16(xmm6,xmm7);

	xmm5 = _mm_unpacklo_epi8(xmm2,xmm6);
	xmm4 = _mm_unpacklo_epi8(xmm6,xmm3);
	xmm6 = _mm_avg_epu8(xmm4,xmm5);

	xmm6 = _mm_slli_epi16(xmm6,8);
	xmm6 = _mm_srli_epi16(xmm6,8);
	xmm6 = _mm_packus_epi16(xmm6,xmm7);
	
	ret = _mm_movepi64_pi64(xmm6);
	_mm_empty(); 

	return(ret);
}





__m64 interpolvline_2(	unsigned char* image,	int PicWidthInPix){
	
	
	__m128i xmm0,xmm1,xmm2,xmm3,xmm4,xmm5,xmm6,xmm7;

	__m64 ret;

	xmm7 = _mm_setzero_si128();

	xmm0 = _mm_movpi64_epi64(*((__m64*)(image - 2*PicWidthInPix)));
	xmm0 = _mm_unpacklo_epi8(xmm0,xmm7);
	xmm1 = _mm_movpi64_epi64(*((__m64*)(image - 1*PicWidthInPix)));
	xmm1 = _mm_unpacklo_epi8(xmm1,xmm7);
	xmm2 = _mm_movpi64_epi64(*((__m64*)(image - 0*PicWidthInPix)));
	xmm2 = _mm_unpacklo_epi8(xmm2,xmm7);
	xmm3 = _mm_movpi64_epi64(*((__m64*)(image + 1*PicWidthInPix)));
	xmm3 = _mm_unpacklo_epi8(xmm3,xmm7);
	xmm4 = _mm_movpi64_epi64(*((__m64*)(image + 2*PicWidthInPix)));
	xmm4 = _mm_unpacklo_epi8(xmm4,xmm7);
	xmm5 = _mm_movpi64_epi64(*((__m64*)(image + 3*PicWidthInPix)));
	xmm5 = _mm_unpacklo_epi8(xmm5,xmm7);

// filter on 8 values
	xmm6 = _mm_add_epi16(xmm2,xmm3);
	xmm6 = _mm_slli_epi16(xmm6,2);
	xmm6 = _mm_sub_epi16(xmm6,xmm1);
	xmm6 = _mm_sub_epi16(xmm6,xmm4);

	xmm1 = _mm_set_epi32(0x00050005,0x00050005,0x00050005,0x00050005);
	xmm6 = _mm_mullo_epi16(xmm6,xmm1);
	xmm6 = _mm_add_epi16(xmm6,xmm0);
	xmm6 = _mm_add_epi16(xmm6,xmm5);
	xmm6 = _mm_add_epi16(xmm6,_mm_set_epi32(0x00100010,0x00100010,0x00100010,0x00100010));
	xmm6 = _mm_max_epi16(xmm6, xmm7); // preventing negative values
	xmm6 = _mm_srli_epi16(xmm6,5);

	xmm6 = _mm_packus_epi16(xmm6,xmm7);
	
	ret = _mm_movepi64_pi64(xmm6);

	_mm_empty(); 

	return(ret);
}





__m64 interpolvline_3(unsigned char* image,	int PicWidthInPix){
	
	
	
	__m128i xmm0,xmm1,xmm2,xmm3,xmm4,xmm5,xmm6,xmm7;

	__m64 ret;

	xmm7 = _mm_setzero_si128();

	xmm0 = _mm_movpi64_epi64(*((__m64*)(image - 2*PicWidthInPix)));
	xmm0 = _mm_unpacklo_epi8(xmm0,xmm7);
	xmm1 = _mm_movpi64_epi64(*((__m64*)(image - 1*PicWidthInPix)));
	xmm1 = _mm_unpacklo_epi8(xmm1,xmm7);
	xmm2 = _mm_movpi64_epi64(*((__m64*)(image - 0*PicWidthInPix)));
	xmm2 = _mm_unpacklo_epi8(xmm2,xmm7);
	xmm3 = _mm_movpi64_epi64(*((__m64*)(image + 1*PicWidthInPix)));
	xmm3 = _mm_unpacklo_epi8(xmm3,xmm7);
	xmm4 = _mm_movpi64_epi64(*((__m64*)(image + 2*PicWidthInPix)));
	xmm4 = _mm_unpacklo_epi8(xmm4,xmm7);
	xmm5 = _mm_movpi64_epi64(*((__m64*)(image + 3*PicWidthInPix)));
	xmm5 = _mm_unpacklo_epi8(xmm5,xmm7);

// filter on 8 values
	xmm6 = _mm_add_epi16(xmm2,xmm3);
	xmm6 = _mm_slli_epi16(xmm6,2);
	xmm6 = _mm_sub_epi16(xmm6,xmm1);
	xmm6 = _mm_sub_epi16(xmm6,xmm4);

	xmm1 = _mm_set_epi32(0x00050005,0x00050005,0x00050005,0x00050005);
	xmm6 = _mm_mullo_epi16(xmm6,xmm1);
	xmm6 = _mm_add_epi16(xmm6,xmm0);
	xmm6 = _mm_add_epi16(xmm6,xmm5);
	xmm6 = _mm_add_epi16(xmm6,_mm_set_epi32(0x00100010,0x00100010,0x00100010,0x00100010));
	xmm6 = _mm_max_epi16(xmm6, xmm7); // preventing negative values
	xmm6 = _mm_srli_epi16(xmm6,5);

	xmm2 = _mm_packus_epi16(xmm2,xmm7);
	xmm3 = _mm_packus_epi16(xmm3,xmm7);
	xmm6 = _mm_packus_epi16(xmm6,xmm7);

	xmm5 = _mm_unpacklo_epi8(xmm2,xmm6);
	xmm4 = _mm_unpacklo_epi8(xmm6,xmm3);
	xmm6 = _mm_avg_epu8(xmm4,xmm5);

	xmm6 = _mm_srli_epi16(xmm6,8);
	xmm6 = _mm_packus_epi16(xmm6,xmm7);
	ret = _mm_movepi64_pi64(xmm6);

	_mm_empty(); 

	return(ret);
}




__m64 interpolvline128_1(	__m128i* temp){
	__m128i xmm6;

	__m64 ret;

	__m128i xmm7 = _mm_setzero_si128();

	__m128i xmm0 = _mm_load_si128(temp++);
	__m128i xmm1 = _mm_load_si128(temp++);
	__m128i xmm2 = _mm_load_si128(temp++);
	__m128i xmm3 = _mm_load_si128(temp++);
	__m128i xmm4 = _mm_load_si128(temp++);
	__m128i xmm5 = _mm_load_si128(temp);

	xmm1 = _mm_add_epi16(xmm1,xmm4);
	xmm0 = _mm_add_epi16(xmm0,xmm5);

	xmm6 = _mm_set_epi32(0xFFFBFFFB,0xFFFBFFFB,0xFFFBFFFB,0xFFFBFFFB);

	xmm4 = _mm_mullo_epi16(xmm1, xmm6);
	xmm5 = _mm_mulhi_epi16(xmm1, xmm6);

	xmm1 = _mm_unpacklo_epi16(xmm4, xmm5);
	xmm6 = _mm_unpackhi_epi16(xmm4, xmm5);

	xmm7 = _mm_set_epi32(0x00140014,0x00140014,0x00140014,0x00140014);
	xmm5 = _mm_add_epi16(xmm2,xmm3);

	xmm4 = _mm_mullo_epi16(xmm5, xmm7);
	xmm5 = _mm_mulhi_epi16(xmm5, xmm7);

	xmm7 = _mm_unpacklo_epi16(xmm4, xmm5);
	xmm4 = _mm_unpackhi_epi16(xmm4, xmm5);

	xmm7 = _mm_add_epi32(xmm7,xmm1);
	xmm4 = _mm_add_epi32(xmm4,xmm6); 

	xmm6 = _mm_set_epi32(0x00010001,0x00010001,0x00010001,0x00010001);
	xmm6 = _mm_mulhi_epi16(xmm0, xmm6);

	xmm1 = _mm_unpacklo_epi16(xmm0, xmm6);
	xmm6 = _mm_unpackhi_epi16(xmm0, xmm6);

	xmm7 = _mm_add_epi32(xmm7,xmm1);
	xmm4 = _mm_add_epi32(xmm4,xmm6); 
	
	xmm1 = _mm_set_epi32(0x00000200,0x00000200,0x00000200,0x00000200);
	
	xmm7 = _mm_add_epi32(xmm7,xmm1);
	xmm4 = _mm_add_epi32(xmm4,xmm1);
	
	xmm5 = _mm_setzero_si128();

	xmm7 = _mm_srli_epi32(xmm7, 10);
	xmm7 = _mm_max_epi16(xmm7, xmm5); // preventing negative values
	xmm7 = _mm_slli_epi32(xmm7,16);
	xmm7 = _mm_srli_epi32(xmm7,16);

	xmm4 = _mm_srli_epi32(xmm4, 10);
	xmm4 = _mm_max_epi16(xmm4, xmm5); // preventing negative values
	xmm4 = _mm_slli_epi32(xmm4,16);
	xmm4 = _mm_srli_epi32(xmm4,16);

	xmm6 = _mm_packs_epi32(xmm7, xmm4);

	xmm1 = _mm_set_epi32(0x00100010,0x00100010,0x00100010,0x00100010);
	xmm2 = _mm_add_epi16(xmm2,xmm1);
	xmm2 = _mm_max_epi16(xmm2, xmm5); // preventing negative values
	xmm2 = _mm_srli_epi16(xmm2,5);

	
	xmm3 = _mm_add_epi16(xmm3,xmm1);
	xmm3 = _mm_max_epi16(xmm3, xmm5); // preventing negative values
	xmm3 = _mm_srli_epi16(xmm3,5);

	xmm2 = _mm_packus_epi16(xmm2,xmm5);
	xmm3 = _mm_packus_epi16(xmm3,xmm5);
	xmm6 = _mm_packus_epi16(xmm6,xmm5);

	xmm7 = _mm_unpacklo_epi8(xmm2,xmm6);
	xmm4 = _mm_unpacklo_epi8(xmm6,xmm3);
	xmm6 = _mm_avg_epu8(xmm4,xmm7);

	xmm6 = _mm_slli_epi16(xmm6,8);
	xmm6 = _mm_srli_epi16(xmm6,8);
	xmm6 = _mm_packus_epi16(xmm6,xmm5);
	
	ret = _mm_movepi64_pi64(xmm6);

	_mm_empty(); 

	return(ret);
}




__m64 interpolvline128_2(__m128i* temp){
	
	__m128i xmm6;

	__m64 ret;

	__m128i xmm7 = _mm_setzero_si128();

	__m128i xmm0 = _mm_load_si128(temp++);
	__m128i xmm1 = _mm_load_si128(temp++);
	__m128i xmm2 = _mm_load_si128(temp++);
	__m128i xmm3 = _mm_load_si128(temp++);
	__m128i xmm4 = _mm_load_si128(temp++);
	__m128i xmm5 = _mm_load_si128(temp);

	xmm1 = _mm_add_epi16(xmm1,xmm4);
	xmm0 = _mm_add_epi16(xmm0,xmm5);

	xmm6 = _mm_set_epi32(0xFFFBFFFB,0xFFFBFFFB,0xFFFBFFFB,0xFFFBFFFB);

	xmm4 = _mm_mullo_epi16(xmm1, xmm6);
	xmm5 = _mm_mulhi_epi16(xmm1, xmm6);

	xmm1 = _mm_unpacklo_epi16(xmm4, xmm5);
	xmm6 = _mm_unpackhi_epi16(xmm4, xmm5);

	xmm7 = _mm_set_epi32(0x00140014,0x00140014,0x00140014,0x00140014);
	xmm5 = _mm_add_epi16(xmm2,xmm3);

	xmm4 = _mm_mullo_epi16(xmm5, xmm7);
	xmm5 = _mm_mulhi_epi16(xmm5, xmm7);

	xmm7 = _mm_unpacklo_epi16(xmm4, xmm5);
	xmm4 = _mm_unpackhi_epi16(xmm4, xmm5);

	xmm7 = _mm_add_epi32(xmm7,xmm1);
	xmm4 = _mm_add_epi32(xmm4,xmm6); 

	xmm6 = _mm_set_epi32(0x00010001,0x00010001,0x00010001,0x00010001);
	xmm6 = _mm_mulhi_epi16(xmm0, xmm6);

	xmm1 = _mm_unpacklo_epi16(xmm0, xmm6);
	xmm6 = _mm_unpackhi_epi16(xmm0, xmm6);

	xmm7 = _mm_add_epi32(xmm7,xmm1);
	xmm4 = _mm_add_epi32(xmm4,xmm6); 
	
	xmm1 = _mm_set_epi32(0x00000200,0x00000200,0x00000200,0x00000200);
	
	xmm7 = _mm_add_epi32(xmm7,xmm1);
	xmm4 = _mm_add_epi32(xmm4,xmm1);
	
	xmm5 = _mm_setzero_si128();

	xmm7 = _mm_srli_epi32(xmm7, 10);
	xmm7 = _mm_max_epi16(xmm7, xmm5); // preventing negative values
	xmm7 = _mm_slli_epi32(xmm7,16);
	xmm7 = _mm_srli_epi32(xmm7,16);

	xmm4 = _mm_srli_epi32(xmm4, 10);
	xmm4 = _mm_max_epi16(xmm4, xmm5); // preventing negative values
	xmm4 = _mm_slli_epi32(xmm4,16);
	xmm4 = _mm_srli_epi32(xmm4,16);

	xmm6 = _mm_packs_epi32(xmm7, xmm4);
	
	xmm6 = _mm_packus_epi16(xmm6,xmm5);
	
	ret = _mm_movepi64_pi64(xmm6);

	_mm_empty(); 

	return(ret);
}




 __m64 interpolvline128_3(__m128i* temp){



	__m128i xmm6;

	__m64 ret;

	__m128i xmm7 = _mm_setzero_si128();

	__m128i xmm0 = _mm_load_si128(temp++);
	__m128i xmm1 = _mm_load_si128(temp++);
	__m128i xmm2 = _mm_load_si128(temp++);
	__m128i xmm3 = _mm_load_si128(temp++);
	__m128i xmm4 = _mm_load_si128(temp++);
	__m128i xmm5 = _mm_load_si128(temp);

	xmm1 = _mm_add_epi16(xmm1,xmm4);
	xmm0 = _mm_add_epi16(xmm0,xmm5);

	xmm6 = _mm_set_epi32(0xFFFBFFFB,0xFFFBFFFB,0xFFFBFFFB,0xFFFBFFFB);

	xmm4 = _mm_mullo_epi16(xmm1, xmm6);
	xmm5 = _mm_mulhi_epi16(xmm1, xmm6);

	xmm1 = _mm_unpacklo_epi16(xmm4, xmm5);
	xmm6 = _mm_unpackhi_epi16(xmm4, xmm5);

	xmm7 = _mm_set_epi32(0x00140014,0x00140014,0x00140014,0x00140014);
	xmm5 = _mm_add_epi16(xmm2,xmm3);

	xmm4 = _mm_mullo_epi16(xmm5, xmm7);
	xmm5 = _mm_mulhi_epi16(xmm5, xmm7);

	xmm7 = _mm_unpacklo_epi16(xmm4, xmm5);
	xmm4 = _mm_unpackhi_epi16(xmm4, xmm5);

	xmm7 = _mm_add_epi32(xmm7,xmm1);
	xmm4 = _mm_add_epi32(xmm4,xmm6); 

	xmm6 = _mm_set_epi32(0x00010001,0x00010001,0x00010001,0x00010001);
	xmm6 = _mm_mulhi_epi16(xmm0, xmm6);

	xmm1 = _mm_unpacklo_epi16(xmm0, xmm6);
	xmm6 = _mm_unpackhi_epi16(xmm0, xmm6);

	xmm7 = _mm_add_epi32(xmm7,xmm1);
	xmm4 = _mm_add_epi32(xmm4,xmm6); 
	
	xmm1 = _mm_set_epi32(0x00000200,0x00000200,0x00000200,0x00000200);
	
	xmm7 = _mm_add_epi32(xmm7,xmm1);
	xmm4 = _mm_add_epi32(xmm4,xmm1);
	
	xmm5 = _mm_setzero_si128();

	xmm7 = _mm_srli_epi32(xmm7, 10);
	xmm7 = _mm_max_epi16(xmm7, xmm5); // preventing negative values
	xmm7 = _mm_slli_epi32(xmm7,16);
	xmm7 = _mm_srli_epi32(xmm7,16);

	xmm4 = _mm_srli_epi32(xmm4, 10);

	xmm4 = _mm_max_epi16(xmm4, xmm5); // preventing negative values
	xmm4 = _mm_slli_epi32(xmm4,16);
	xmm4 = _mm_srli_epi32(xmm4,16);

	xmm6 = _mm_packs_epi32(xmm7, xmm4);
	
	xmm1 = _mm_set_epi32(0x00100010,0x00100010,0x00100010,0x00100010);
	xmm2 = _mm_add_epi16(xmm2,xmm1);
	xmm2 = _mm_max_epi16(xmm2, xmm5); // preventing negative values
	xmm2 = _mm_srli_epi16(xmm2,5);

	
	xmm3 = _mm_add_epi16(xmm3,xmm1);
	xmm3 = _mm_max_epi16(xmm3, xmm5); // preventing negative values
	xmm3 = _mm_srli_epi16(xmm3,5);

	xmm2 = _mm_packus_epi16(xmm2,xmm5);
	xmm3 = _mm_packus_epi16(xmm3,xmm5);
	xmm6 = _mm_packus_epi16(xmm6,xmm5);

	xmm7 = _mm_unpacklo_epi8(xmm2,xmm6);
	xmm4 = _mm_unpacklo_epi8(xmm6,xmm3);
	xmm6 = _mm_avg_epu8(xmm4,xmm7);

	xmm6 = _mm_srli_epi16(xmm6,8);
	xmm6 = _mm_packus_epi16(xmm6,xmm5);
	ret = _mm_movepi64_pi64(xmm6);

	_mm_empty(); 

	return(ret);
}






void luma_sample_interp_8x8_0_0_SSE2(unsigned char* sortie, unsigned char* image,const short PicWidthInPix, const short OutStride){

	int i;
	__m64* ptr_img = (__m64*) image;
	__m64* ptr_out = (__m64*) sortie;
	for(i = 0;i < 8; i++, ptr_img += (PicWidthInPix >> 3), ptr_out += (OutStride >> 3))
		*ptr_out = *ptr_img;

	_mm_empty(); 
}



void luma_sample_interp_8x8_1_0_SSE2(unsigned char* sortie, unsigned char* image,const short PicWidthInPix,const short OutStride){

	int i;
	__m64 *ptr_out = (__m64 *)sortie;

	for(i = 0;i < 8; i++, image += PicWidthInPix,ptr_out += (OutStride >> 3))
		*ptr_out = interpolhline_1(image);

	_mm_empty(); 
}


void luma_sample_interp_8x8_2_0_SSE2(unsigned char* sortie, unsigned char* image,const short PicWidthInPix,const short OutStride){
	
	
	int i;
	__m64 *ptr_out = (__m64 *)sortie;

	for(i = 0;i < 8; i++, image += PicWidthInPix,ptr_out += (OutStride >> 3))
			*ptr_out = interpolhline_2(image);

	_mm_empty(); 
}

void luma_sample_interp_8x8_3_0_SSE2(unsigned char* sortie, unsigned char* image,const short PicWidthInPix,const short OutStride){

	int i;
	__m64 *ptr_out = (__m64 *)sortie;

	for(i = 0;i < 8; i++, image += PicWidthInPix,ptr_out += (OutStride >> 3))
		*ptr_out = interpolhline_3(image);

	_mm_empty(); 
}



void luma_sample_interp_8x8_0_1_SSE2(unsigned char* sortie, unsigned char* image,const short PicWidthInPix,const short OutStride){

	int i;
	__m64 *ptr_out = (__m64 *)sortie;

	for(i = 0; i < 8; i++, image += PicWidthInPix,ptr_out += (OutStride >> 3))
			*ptr_out = interpolvline_1(image,PicWidthInPix);


	_mm_empty(); 
}


void luma_sample_interp_8x8_0_2_SSE2(unsigned char* sortie, unsigned char* image,const short PicWidthInPix,const short OutStride){

	int i;
	__m64 *ptr_out = (__m64 *)sortie;

	for(i = 0; i < 8; i++, image += PicWidthInPix,ptr_out += (OutStride >> 3))
		*ptr_out = interpolvline_2(image,PicWidthInPix);


	_mm_empty(); 
}

void luma_sample_interp_8x8_0_3_SSE2(unsigned char* sortie, unsigned char* image,const short PicWidthInPix,const short OutStride){

	int i;
	__m64 *ptr_out = (__m64 *)sortie;

	for(i = 0; i < 8; i++, image += PicWidthInPix,ptr_out += (OutStride >> 3))
		*ptr_out = interpolvline_3(image,PicWidthInPix);


	_mm_empty(); 
}


void luma_sample_interp_8x8_2_1_SSE2(unsigned char* sortie, unsigned char* image,const short PicWidthInPix,const short OutStride){

	int i;
	__m64 *ptr_out = (__m64 *)sortie;
	__m128i temp[13];

	for(i = 0; i < 13; i++){
		_mm_store_si128(temp + i, interpolhline128(image + PicWidthInPix * (i - 2)));
	}


	for(i = 0; i < 8; i++, ptr_out += (OutStride >> 3))	{
		*ptr_out = interpolvline128_1(temp+i);
		_mm_empty(); 
	}


}



void luma_sample_interp_8x8_2_2_SSE2(unsigned char* sortie, unsigned char* image,const short PicWidthInPix,const short OutStride){

	int i;
	__m64 *ptr_out = (__m64 *)sortie;
	__m128i temp[13];

	for(i = 0; i < 13; i++){
		_mm_store_si128(temp + i, interpolhline128(image + PicWidthInPix * (i - 2)));
	}

	for(i = 0; i < 8; i++, ptr_out += (OutStride >> 3))	{
		*ptr_out = interpolvline128_2(temp+i);
	}

	_mm_empty(); 
}


void luma_sample_interp_8x8_2_3_SSE2(unsigned char* sortie, unsigned char* image,const short PicWidthInPix,const short OutStride){

	int i;
	__m64 *ptr_out = (__m64 *)sortie;
	__m128i temp[13];

	for(i = 0; i < 13; i++){
		_mm_store_si128(temp + i, interpolhline128(image + PicWidthInPix * (i - 2)));
	}

	
	for(i = 0; i < 8; i++, ptr_out += (OutStride >> 3))	{
		*ptr_out = interpolvline128_3(temp + i);
		_mm_empty(); 
	}

}



void luma_sample_interp_8x8_1_1_SSE2(unsigned char* sortie, unsigned char* image,const short PicWidthInPix,const short OutStride){


	unsigned char* tmpimg = image;
	unsigned char* tmpimg2 = image;
	__m64 *ptr_out = (__m64 *)sortie;

	int i;
	for(i = 0; i < 8 * PicWidthInPix; i += PicWidthInPix, ptr_out += (OutStride >> 3))
	{
		__m128i verthalf = _mm_movpi64_epi64(interpolvline_2(tmpimg + i,PicWidthInPix));
		__m128i horizhalf = _mm_movpi64_epi64(interpolhline_2(tmpimg2 + i));
		*ptr_out = _mm_movepi64_pi64(_mm_avg_epu8(verthalf,horizhalf));
	}

	_mm_empty(); 
}



void luma_sample_interp_8x8_3_1_SSE2(unsigned char* sortie, unsigned char* image,const short PicWidthInPix,const short OutStride){


	unsigned char* tmpimg = image + 1;
	unsigned char* tmpimg2 = image;
	__m64 *ptr_out = (__m64 *)sortie;

	int i;
	for(i = 0; i < 8 * PicWidthInPix; i += PicWidthInPix, ptr_out += (OutStride >> 3))
	{
		__m128i verthalf = _mm_movpi64_epi64(interpolvline_2(tmpimg + i,PicWidthInPix));
		__m128i horizhalf = _mm_movpi64_epi64(interpolhline_2(tmpimg2 + i));
		*ptr_out = _mm_movepi64_pi64(_mm_avg_epu8(verthalf,horizhalf));
	}

	_mm_empty(); 
}



void luma_sample_interp_8x8_1_3_SSE2(unsigned char* sortie, unsigned char* image,const short PicWidthInPix,const short OutStride){


	unsigned char* tmpimg = image;
	unsigned char* tmpimg2 = image + PicWidthInPix;
	__m64 *ptr_out = (__m64 *)sortie;

	int i;
	for(i = 0; i < 8 * PicWidthInPix; i += PicWidthInPix, ptr_out += (OutStride >> 3))
	{
		__m128i verthalf = _mm_movpi64_epi64(interpolvline_2(tmpimg + i,PicWidthInPix));
		__m128i horizhalf = _mm_movpi64_epi64(interpolhline_2(tmpimg2 + i));
		*ptr_out = _mm_movepi64_pi64(_mm_avg_epu8(verthalf,horizhalf));
	}

	_mm_empty(); 
}


void luma_sample_interp_8x8_3_3_SSE2(unsigned char* sortie, unsigned char* image,const short PicWidthInPix,const short OutStride){


	unsigned char* tmpimg = image + 1;
	unsigned char* tmpimg2 = image + PicWidthInPix;
	__m64 *ptr_out = (__m64 *)sortie;

	int i;
	for(i = 0; i < 8 * PicWidthInPix; i += PicWidthInPix, ptr_out += (OutStride >> 3))
	{
		__m128i verthalf = _mm_movpi64_epi64(interpolvline_2(tmpimg + i,PicWidthInPix));
		__m128i horizhalf = _mm_movpi64_epi64(interpolhline_2(tmpimg2 + i));
		*ptr_out = _mm_movepi64_pi64(_mm_avg_epu8(verthalf,horizhalf));
	}

	_mm_empty(); 
}



void luma_sample_interp_8x8_1_2_SSE2(unsigned char* sortie, unsigned char* image,const short PicWidthInPix,const short OutStride){
	
	__m64 *ptr_out = (__m64 *)sortie;
	unsigned char* tmpimg = image;
	__m128i temp[13];
	int i;
	for(i = 0; i < 13;i++)
	{
		_mm_store_si128(temp + i, interpolhline128(image + PicWidthInPix * (i - 2)));
	}

	for(i = 0; i < 8;i++, ptr_out += (OutStride >> 3))	{

		__m128i verthalf = _mm_movpi64_epi64(interpolvline_2(tmpimg + i*PicWidthInPix,PicWidthInPix));
		__m128i centerpix = _mm_movpi64_epi64(interpolvline128_2(temp+i));

		*ptr_out = _mm_movepi64_pi64(_mm_avg_epu8(verthalf,centerpix));
	}

	_mm_empty(); 
}


void luma_sample_interp_8x8_3_2_SSE2(unsigned char* sortie, unsigned char* image,const short PicWidthInPix,const short OutStride){
	
	__m64 *ptr_out = (__m64 *)sortie;
	unsigned char* tmpimg = image + 1;
	__m128i temp[13];
	int i;
	for(i = 0; i < 13;i++)	{
		_mm_store_si128(temp + i, interpolhline128(image + PicWidthInPix * (i - 2)));
	}

	for(i = 0; i < 8;i++, ptr_out += (OutStride >> 3))
	{

		__m128i verthalf = _mm_movpi64_epi64(interpolvline_2(tmpimg + i*PicWidthInPix,PicWidthInPix));
		__m128i centerpix = _mm_movpi64_epi64(interpolvline128_2(temp+i));

		*ptr_out = _mm_movepi64_pi64(_mm_avg_epu8(verthalf,centerpix));
	}

	_mm_empty(); 
}

#endif
