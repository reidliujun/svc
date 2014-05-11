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

//AVC Files
#include "clip.h"
#include "interpolation_mmx.h"



//For n_0

unsigned int interpolhline_mmx_1(unsigned char* image){

	__m64 mm_A = _mm_set_pi16(image[1],image[0],image[-1],image[-2]); 
	__m64 mm_B = _mm_set_pi16(image[2],image[1],image[0],image[-1]); 
	__m64 mm_C = _mm_set_pi16(image[3],image[2],image[1],image[0]); 
	__m64 mm_D = _mm_set_pi16(image[4],image[3],image[2],image[1]);
	__m64 mm_E = _mm_set_pi16(image[5],image[4],image[3],image[2]);
	__m64 mm_F = _mm_set_pi16(image[6],image[5],image[4],image[3]);
	__m64 mm_zero = _mm_setzero_si64();

	__m64 mm_AF = _mm_add_pi16(mm_A,mm_F);//A + F
	__m64 mm_inter0 = _mm_add_pi16(mm_AF,_mm_set_pi16(16,16,16,16));//A + F + 16
	__m64 mm_BE = _mm_add_pi16(mm_B,mm_E);//B + E
	__m64 mm_CD = _mm_add_pi16(mm_C,mm_D);//C + D
	__m64 mm_CDS = _mm_slli_pi16(mm_CD,2);//(C + D) << 2
	__m64 mm_inter1 = _mm_sub_pi16(mm_CDS,mm_BE);//((C + D) << 2)-(B + E)
	__m64 mm_5 = _mm_set_pi16(5,5,5,5);
	__m64 mm_inter_3 = _mm_mullo_pi16(mm_inter1, mm_5);//(((C + D) << 2)-(B + E))*5

	__m64 mm_result = _mm_add_pi16(mm_inter_3,mm_inter0);//A + F + 16 + (((C + D) << 2)-(B + E))*5
	__m64 mm_clip = _mm_max_pi16(mm_result,mm_zero);//Clip with 0
	__m64 mm_ret = _mm_srai_pi16(mm_clip,5);
	__m64 mm_clip1 = _mm_min_pi16(mm_ret,_mm_set_pi16(255,255,255,255)); //Clip with 255


	__m64 test = _mm_avg_pu8(mm_clip1,mm_C);//(ptr_img[0] + ptr_rf[0] + 1) >> 1
	__m64 test1 =_mm_packs_pu16(test,mm_zero);
	unsigned int ret = _mm_cvtsi64_si32(test1);
	
	empty(); 
	return ret;

}


unsigned int interpolhline_mmx_2(unsigned char* image){

	__m64 mm_A = _mm_set_pi16(image[1],image[0],image[-1],image[-2]); 
	__m64 mm_B = _mm_set_pi16(image[2],image[1],image[0],image[-1]); 
	__m64 mm_C = _mm_set_pi16(image[3],image[2],image[1],image[0]); 
	__m64 mm_D = _mm_set_pi16(image[4],image[3],image[2],image[1]);
	__m64 mm_E = _mm_set_pi16(image[5],image[4],image[3],image[2]);
	__m64 mm_F = _mm_set_pi16(image[6],image[5],image[4],image[3]);

	__m64 mm_AF = _mm_add_pi16(mm_A,mm_F);//A + F
	__m64 mm_inter0 = _mm_add_pi16(mm_AF,_mm_set_pi16(16,16,16,16));//A + F + 16
	__m64 mm_BE = _mm_add_pi16(mm_B,mm_E);//B + E
	__m64 mm_CD = _mm_add_pi16(mm_C,mm_D);//C + D
	__m64 mm_CDS = _mm_slli_pi16(mm_CD,2);//(C + D) << 2
	__m64 mm_inter1 = _mm_sub_pi16(mm_CDS,mm_BE);//((C + D) << 2)-(B + E)
	__m64 mm_5 = _mm_set_pi16(5,5,5,5);
	__m64 mm_inter_3 = _mm_mullo_pi16(mm_inter1, mm_5);//(((C + D) << 2)-(B + E))*5

	__m64 mm_result = _mm_add_pi16(mm_inter_3,mm_inter0);//A + F + 16 + (((C + D) << 2)-(B + E))*5
	__m64 mm_zero = _mm_setzero_si64();
	__m64 mm_clip = _mm_max_pi16(mm_result,mm_zero);//Clip with 0
	__m64 mm_ret = _mm_srai_pi16(mm_clip,5);
	__m64 mm_clip1 = _mm_min_pi16(mm_ret,_mm_set_pi16(255,255,255,255)); //Clip with 255
	
	__m64 result =_mm_packs_pu16(mm_clip1,mm_zero);
	unsigned int ret = _mm_cvtsi64_si32(result);

	 empty(); 
	return ret;

}


unsigned int interpolhline_mmx_3(unsigned char* image){

	__m64 mm_A = _mm_set_pi16(image[1],image[0],image[-1],image[-2]); 
	__m64 mm_B = _mm_set_pi16(image[2],image[1],image[0],image[-1]); 
	__m64 mm_C = _mm_set_pi16(image[3],image[2],image[1],image[0]); 
	__m64 mm_D = _mm_set_pi16(image[4],image[3],image[2],image[1]);
	__m64 mm_E = _mm_set_pi16(image[5],image[4],image[3],image[2]);
	__m64 mm_F = _mm_set_pi16(image[6],image[5],image[4],image[3]);

	__m64 mm_AF = _mm_add_pi16(mm_A,mm_F);//A + F
	__m64 mm_inter0 = _mm_add_pi16(mm_AF,_mm_set_pi16(16,16,16,16));//A + F + 16
	__m64 mm_BE = _mm_add_pi16(mm_B,mm_E);//B + E
	__m64 mm_CD = _mm_add_pi16(mm_C,mm_D);//C + D
	__m64 mm_CDS = _mm_slli_pi16(mm_CD,2);//(C + D) << 2
	__m64 mm_inter1 = _mm_sub_pi16(mm_CDS,mm_BE);//((C + D) << 2)-(B + E)
	__m64 mm_5 = _mm_set_pi16(5,5,5,5);
	__m64 mm_inter_3 = _mm_mullo_pi16(mm_inter1, mm_5);//(((C + D) << 2)-(B + E))*5

	__m64 mm_result = _mm_add_pi16(mm_inter_3,mm_inter0);//A + F + 16 + (((C + D) << 2)-(B + E))*5
	__m64 mm_zero = _mm_setzero_si64();
	__m64 mm_clip = _mm_max_pi16(mm_result,mm_zero);//Clip with 0
	__m64 mm_ret = _mm_srai_pi16(mm_clip,5);
	__m64 mm_clip1 = _mm_min_pi16(mm_ret,_mm_set_pi16(255,255,255,255)); //Clip with 255
	
	
	__m64 test =  _mm_avg_pu8(mm_clip1,mm_D);//(ptr_img[0] + ptr_rf[0] + 1) >> 1
	__m64 test1 =_mm_packs_pu16(test,mm_zero);
	unsigned int ret = _mm_cvtsi64_si32(test1);

	 empty(); 
	return ret;

}








//For 0_n
unsigned int interpolvline_mmx_1(unsigned char* image, unsigned char * ref,	int PicWidthInPix){

	__m64 mm_ref = _mm_set_pi16(ref[3],ref[2],ref[1],ref[0]);
	__m64 mm_A = _mm_set_pi16(image[-2 * PicWidthInPix + 3],image[-2 * PicWidthInPix + 2],image[-2 * PicWidthInPix + 1],image[-2 * PicWidthInPix]); 
	__m64 mm_B = _mm_set_pi16(image[-1 * PicWidthInPix + 3],image[-1 * PicWidthInPix + 2],image[-1 * PicWidthInPix + 1],image[-1 * PicWidthInPix]); 
	__m64 mm_C = _mm_set_pi16(image[3],image[2],image[1],image[0]); 
	__m64 mm_D = _mm_set_pi16(image[1 * PicWidthInPix + 3],image[1 * PicWidthInPix + 2],image[1 * PicWidthInPix + 1],image[1 * PicWidthInPix]);
	__m64 mm_E = _mm_set_pi16(image[2 * PicWidthInPix + 3],image[2 * PicWidthInPix + 2],image[2 * PicWidthInPix + 1],image[2 * PicWidthInPix]);
	__m64 mm_F = _mm_set_pi16(image[3 * PicWidthInPix + 3],image[3 * PicWidthInPix + 2],image[3 * PicWidthInPix + 1],image[3 * PicWidthInPix]);

	__m64 mm_AF = _mm_add_pi16(mm_A,mm_F);//A + F
	__m64 mm_inter0 = _mm_add_pi16(mm_AF,_mm_set_pi16(16,16,16,16));//A + F + 16
	__m64 mm_BE = _mm_add_pi16(mm_B,mm_E);//B + E
	__m64 mm_CD = _mm_add_pi16(mm_C,mm_D);//C + D
	__m64 mm_CDS = _mm_slli_pi16(mm_CD,2);//(C + D) << 2
	__m64 mm_inter1 = _mm_sub_pi16(mm_CDS,mm_BE);//((C + D) << 2)-(B + E)
	__m64 mm_5 = _mm_set_pi16(5,5,5,5);
	__m64 mm_inter_3 = _mm_mullo_pi16(mm_inter1, mm_5);//(((C + D) << 2)-(B + E))*5

	__m64 mm_result = _mm_add_pi16(mm_inter_3,mm_inter0);//A + F + 16 + (((C + D) << 2)-(B + E))*5
	__m64 mm_zero = _mm_setzero_si64();
	__m64 mm_clip = _mm_max_pi16(mm_result,mm_zero);//Clip with 0
	__m64 mm_ret = _mm_srai_pi16(mm_clip,5);
	__m64 mm_clip1 = _mm_min_pi16(mm_ret,_mm_set_pi16(255,255,255,255)); //Clip with 255
	
	
	__m64 test =  _mm_avg_pu8(mm_clip1,mm_ref);//(ptr_img[0] + ptr_rf[0] + 1) >> 1
	__m64 test1 =_mm_packs_pu16(test,mm_zero);
	unsigned int ret = _mm_cvtsi64_si32(test1);

	 empty(); 
	return ret;

}




unsigned int interpolvline_mmx_2(unsigned char* image,	int PicWidthInPix){

	__m64 mm_A = _mm_set_pi16(image[-2 * PicWidthInPix + 3],image[-2 * PicWidthInPix + 2],image[-2 * PicWidthInPix + 1],image[-2 * PicWidthInPix]); 
	__m64 mm_B = _mm_set_pi16(image[-1 * PicWidthInPix + 3],image[-1 * PicWidthInPix + 2],image[-1 * PicWidthInPix + 1],image[-1 * PicWidthInPix]); 
	__m64 mm_C = _mm_set_pi16(image[3],image[2],image[1],image[0]); 
	__m64 mm_D = _mm_set_pi16(image[1 * PicWidthInPix + 3],image[1 * PicWidthInPix + 2],image[1 * PicWidthInPix + 1],image[1 * PicWidthInPix]);
	__m64 mm_E = _mm_set_pi16(image[2 * PicWidthInPix + 3],image[2 * PicWidthInPix + 2],image[2 * PicWidthInPix + 1],image[2 * PicWidthInPix]);
	__m64 mm_F = _mm_set_pi16(image[3 * PicWidthInPix + 3],image[3 * PicWidthInPix + 2],image[3 * PicWidthInPix + 1],image[3 * PicWidthInPix]);

	__m64 mm_AF = _mm_add_pi16(mm_A,mm_F);//A + F
	__m64 mm_inter0 = _mm_add_pi16(mm_AF,_mm_set_pi16(16,16,16,16));//A + F + 16
	__m64 mm_BE = _mm_add_pi16(mm_B,mm_E);//B + E
	__m64 mm_CD = _mm_add_pi16(mm_C,mm_D);//C + D
	__m64 mm_CDS = _mm_slli_pi16(mm_CD,2);//(C + D) << 2
	__m64 mm_inter1 = _mm_sub_pi16(mm_CDS,mm_BE);//((C + D) << 2)-(B + E)
	__m64 mm_5 = _mm_set_pi16(5,5,5,5);
	__m64 mm_inter_3 = _mm_mullo_pi16(mm_inter1, mm_5);//(((C + D) << 2)-(B + E))*5

	__m64 mm_result = _mm_add_pi16(mm_inter_3,mm_inter0);//A + F + 16 + (((C + D) << 2)-(B + E))*5
	__m64 mm_zero = _mm_setzero_si64();
	__m64 mm_clip = _mm_max_pi16(mm_result,mm_zero);//Clip with 0
	__m64 mm_ret = _mm_srai_pi16(mm_clip,5);
	__m64 mm_clip1 = _mm_min_pi16(mm_ret,_mm_set_pi16(255,255,255,255)); //Clip with 255
	
	__m64 test1 =_mm_packs_pu16(mm_clip1,mm_zero);
	unsigned int ret = _mm_cvtsi64_si32(test1);

	  empty(); 
	return ret;

}




unsigned int interpolvline_mmx_3(unsigned char* image,	int PicWidthInPix){

	__m64 mm_A = _mm_set_pi16(image[-2 * PicWidthInPix + 3],image[-2 * PicWidthInPix + 2],image[-2 * PicWidthInPix + 1],image[-2 * PicWidthInPix]); 
	__m64 mm_B = _mm_set_pi16(image[-1 * PicWidthInPix + 3],image[-1 * PicWidthInPix + 2],image[-1 * PicWidthInPix + 1],image[-1 * PicWidthInPix]); 
	__m64 mm_C = _mm_set_pi16(image[3],image[2],image[1],image[0]); 
	__m64 mm_D = _mm_set_pi16(image[1 * PicWidthInPix + 3],image[1 * PicWidthInPix + 2],image[1 * PicWidthInPix + 1],image[1 * PicWidthInPix]);
	__m64 mm_E = _mm_set_pi16(image[2 * PicWidthInPix + 3],image[2 * PicWidthInPix + 2],image[2 * PicWidthInPix + 1],image[2 * PicWidthInPix]);
	__m64 mm_F = _mm_set_pi16(image[3 * PicWidthInPix + 3],image[3 * PicWidthInPix + 2],image[3 * PicWidthInPix + 1],image[3 * PicWidthInPix]);

	__m64 mm_AF = _mm_add_pi16(mm_A,mm_F);//A + F
	__m64 mm_inter0 = _mm_add_pi16(mm_AF,_mm_set_pi16(16,16,16,16));//A + F + 16
	__m64 mm_BE = _mm_add_pi16(mm_B,mm_E);//B + E
	__m64 mm_CD = _mm_add_pi16(mm_C,mm_D);//C + D
	__m64 mm_CDS = _mm_slli_pi16(mm_CD,2);//(C + D) << 2
	__m64 mm_inter1 = _mm_sub_pi16(mm_CDS,mm_BE);//((C + D) << 2)-(B + E)
	__m64 mm_5 = _mm_set_pi16(5,5,5,5);
	__m64 mm_inter_3 = _mm_mullo_pi16(mm_inter1, mm_5);//(((C + D) << 2)-(B + E))*5

	__m64 mm_result = _mm_add_pi16(mm_inter_3,mm_inter0);//A + F + 16 + (((C + D) << 2)-(B + E))*5
	__m64 mm_zero = _mm_setzero_si64();
	__m64 mm_clip = _mm_max_pi16(mm_result,mm_zero);//Clip with 0
	__m64 mm_ret = _mm_srai_pi16(mm_clip,5);
	__m64 mm_clip1 = _mm_min_pi16(mm_ret,_mm_set_pi16(255,255,255,255)); //Clip with 255
	
	
	__m64 test =  _mm_avg_pu8(mm_clip1,mm_D);//(ptr_img[0] + ptr_rf[0] + 1) >> 1
	__m64 test1 =_mm_packs_pu16(test,mm_zero);
	unsigned int ret = _mm_cvtsi64_si32(test1);

	empty(); 
	return ret;

}







//For 2_0

 __m64 interpolhline64_mmx(unsigned char* image){

 
	__m64 mm_A = _mm_set_pi16(image[3],image[2],image[1],image[0]); 
	__m64 mm_B = _mm_set_pi16(image[4],image[3],image[2],image[1]);
	__m64 mm_C = _mm_set_pi16(image[5],image[4],image[3],image[2]);
	__m64 mm_D = _mm_set_pi16(image[6],image[5],image[4],image[3]);
	__m64 mm_E = _mm_set_pi16(image[7],image[6],image[5],image[4]);
	__m64 mm_F = _mm_set_pi16(image[8],image[7],image[6],image[5]);


	__m64 mm_AF = _mm_add_pi16(mm_A,mm_F);//A + F
	__m64 mm_BE = _mm_add_pi16(mm_B,mm_E);//B + E
	__m64 mm_CD = _mm_add_pi16(mm_C,mm_D);//C + D
	__m64 mm_CDS = _mm_slli_pi16(mm_CD,2);//(C + D) << 2
	__m64 mm_inter1 = _mm_sub_pi16(mm_CDS,mm_BE);//((C + D) << 2)-(B + E)
	__m64 mm_5 = _mm_set_pi16(5,5,5,5);
	__m64 mm_inter_3 = _mm_mullo_pi16(mm_inter1, mm_5);//(((C + D) << 2)-(B + E))*5

	__m64 mm_result = _mm_add_pi16(mm_inter_3,mm_AF);//A + F + 16 + (((C + D) << 2)-(B + E))*5
	 

	return(mm_result);


}




unsigned int interpolvline64_1_mmx(__m64* temp){


	__m64 res,res1;
	__m64 ptr = _mm_setzero_si64();
	__m64 mm_16 = _mm_set_pi16(16,16,16,16);
	
	short A = _mm_extract_pi16(temp[0],0);
	short B = _mm_extract_pi16(temp[1],0);
	short C = _mm_extract_pi16(temp[2],0);
	short D = _mm_extract_pi16(temp[3],0);
	short E = _mm_extract_pi16(temp[4],0);
	short F = _mm_extract_pi16(temp[5],0);

	unsigned int result = A + F - 5 * (short)(B + E) + 20 * (short)(C + D) + 512;
	ptr = _mm_insert_pi16(ptr,CLIP255_16(result >> 10),0);

	 A = _mm_extract_pi16(temp[0],1);
	 B = _mm_extract_pi16(temp[1],1);
	 C = _mm_extract_pi16(temp[2],1);
	 D = _mm_extract_pi16(temp[3],1);
	 E = _mm_extract_pi16(temp[4],1);
	 F = _mm_extract_pi16(temp[5],1);

	result = A + F - 5 * (short)(B + E) + 20 * (short)(C + D) + 512;
	ptr = _mm_insert_pi16(ptr,CLIP255_16(result >> 10),1);

	 A = _mm_extract_pi16(temp[0],2);
	 B = _mm_extract_pi16(temp[1],2);
	 C = _mm_extract_pi16(temp[2],2);
	 D = _mm_extract_pi16(temp[3],2);
	 E = _mm_extract_pi16(temp[4],2);
	 F = _mm_extract_pi16(temp[5],2);

	result = A + F - 5 * (short)(B + E) + 20 * (short)(C + D) + 512;
	ptr = _mm_insert_pi16(ptr,CLIP255_16(result >> 10),2);

	 A = _mm_extract_pi16(temp[0],3);
	 B = _mm_extract_pi16(temp[1],3);
	 C = _mm_extract_pi16(temp[2],3);
	 D = _mm_extract_pi16(temp[3],3);
	 E = _mm_extract_pi16(temp[4],3);
	 F = _mm_extract_pi16(temp[5],3);

	result = A + F - 5 * (short)(B + E) + 20 * (short)(C + D) + 512;
	ptr = _mm_insert_pi16(ptr,CLIP255_16(result >> 10),3);
	

	res = _mm_add_pi16(temp[2],mm_16);
	res1 = _mm_srai_pi16(res,5);
	res1 = _mm_max_pi16(res1,_mm_set_pi16(0,0,0,0));
	res1 = _mm_min_pi16(res1,_mm_set_pi16(255,255,255,255)); //Clip

	res = _mm_set_pi8(0,0,0,0,_mm_extract_pi16(ptr,3),_mm_extract_pi16(ptr,2),_mm_extract_pi16(ptr,1),_mm_extract_pi16(ptr,0));
	res1 =_mm_set_pi8(0,0,0,0,_mm_extract_pi16(res1,3),_mm_extract_pi16(res1,2),_mm_extract_pi16(res1,1),_mm_extract_pi16(res1,0));
	res =  _mm_avg_pu8(res,res1);//(ptr_img[0] + ptr_rf[0] + 1) >> 1

	result = _mm_cvtsi64_si32(res); 
	empty();
	return result;

}



unsigned int interpolvline64_2_mmx(__m64* temp){

	__m64 res;
	__m64 ptr = _mm_setzero_si64();
	

	short A = _mm_extract_pi16(temp[0],0);
	short B = _mm_extract_pi16(temp[1],0);
	short C = _mm_extract_pi16(temp[2],0);
	short D = _mm_extract_pi16(temp[3],0);
	short E = _mm_extract_pi16(temp[4],0);
	short F = _mm_extract_pi16(temp[5],0);

	unsigned int result = A + F - 5 * (short)(B + E) + 20 * (short)(C + D) + 512;
	ptr = _mm_insert_pi16(ptr,CLIP255_16(result >> 10),0);

	 A = _mm_extract_pi16(temp[0],1);
	 B = _mm_extract_pi16(temp[1],1);
	 C = _mm_extract_pi16(temp[2],1);
	 D = _mm_extract_pi16(temp[3],1);
	 E = _mm_extract_pi16(temp[4],1);
	 F = _mm_extract_pi16(temp[5],1);

	result = A + F - 5 * (short)(B + E) + 20 * (short)(C + D) + 512;
	ptr = _mm_insert_pi16(ptr,CLIP255_16(result >> 10),1);

	 A = _mm_extract_pi16(temp[0],2);
	 B = _mm_extract_pi16(temp[1],2);
	 C = _mm_extract_pi16(temp[2],2);
	 D = _mm_extract_pi16(temp[3],2);
	 E = _mm_extract_pi16(temp[4],2);
	 F = _mm_extract_pi16(temp[5],2);

	result = A + F - 5 * (short)(B + E) + 20 * (short)(C + D) + 512;
	ptr = _mm_insert_pi16(ptr,CLIP255_16(result >> 10),2);

	 A = _mm_extract_pi16(temp[0],3);
	 B = _mm_extract_pi16(temp[1],3);
	 C = _mm_extract_pi16(temp[2],3);
	 D = _mm_extract_pi16(temp[3],3);
	 E = _mm_extract_pi16(temp[4],3);
	 F = _mm_extract_pi16(temp[5],3);

	result = A + F - 5 * (short)(B + E) + 20 * (short)(C + D) + 512;
	ptr = _mm_insert_pi16(ptr,CLIP255_16(result >> 10),3);
	
	res = _mm_set_pi8(0,0,0,0,_mm_extract_pi16(ptr,3),_mm_extract_pi16(ptr,2),_mm_extract_pi16(ptr,1),_mm_extract_pi16(ptr,0));
	result = _mm_cvtsi64_si32(res); 
	empty();
	return result;

}







unsigned int interpolvline64_3_mmx(__m64* temp){


	__m64 res,res1;
	__m64 ptr = _mm_setzero_si64();
	__m64 mm_16 = _mm_set_pi16(16,16,16,16);
	
	short A = _mm_extract_pi16(temp[0],0);
	short B = _mm_extract_pi16(temp[1],0);
	short C = _mm_extract_pi16(temp[2],0);
	short D = _mm_extract_pi16(temp[3],0);
	short E = _mm_extract_pi16(temp[4],0);
	short F = _mm_extract_pi16(temp[5],0);

	unsigned int result = A + F - 5 * (short)(B + E) + 20 * (short)(C + D) + 512;
	ptr = _mm_insert_pi16(ptr,CLIP255_16(result >> 10),0);

	 A = _mm_extract_pi16(temp[0],1);
	 B = _mm_extract_pi16(temp[1],1);
	 C = _mm_extract_pi16(temp[2],1);
	 D = _mm_extract_pi16(temp[3],1);
	 E = _mm_extract_pi16(temp[4],1);
	 F = _mm_extract_pi16(temp[5],1);

	result = A + F - 5 * (short)(B + E) + 20 * (short)(C + D) + 512;
	ptr = _mm_insert_pi16(ptr,CLIP255_16(result >> 10),1);

	 A = _mm_extract_pi16(temp[0],2);
	 B = _mm_extract_pi16(temp[1],2);
	 C = _mm_extract_pi16(temp[2],2);
	 D = _mm_extract_pi16(temp[3],2);
	 E = _mm_extract_pi16(temp[4],2);
	 F = _mm_extract_pi16(temp[5],2);

	result = A + F - 5 * (short)(B + E) + 20 * (short)(C + D) + 512;
	ptr = _mm_insert_pi16(ptr,CLIP255_16(result >> 10),2);

	 A = _mm_extract_pi16(temp[0],3);
	 B = _mm_extract_pi16(temp[1],3);
	 C = _mm_extract_pi16(temp[2],3);
	 D = _mm_extract_pi16(temp[3],3);
	 E = _mm_extract_pi16(temp[4],3);
	 F = _mm_extract_pi16(temp[5],3);

	result = A + F - 5 * (short)(B + E) + 20 * (short)(C + D) + 512;
	ptr = _mm_insert_pi16(ptr,CLIP255_16(result >> 10),3);
	

	res = _mm_add_pi16(temp[3],mm_16);
	res1 = _mm_srai_pi16(res,5);
	res1 = _mm_max_pi16(res1,_mm_set_pi16(0,0,0,0));
	res1 = _mm_min_pi16(res1,_mm_set_pi16(255,255,255,255)); //Clip

	res = _mm_set_pi8(0,0,0,0,_mm_extract_pi16(ptr,3),_mm_extract_pi16(ptr,2),_mm_extract_pi16(ptr,1),_mm_extract_pi16(ptr,0));
	res1 =_mm_set_pi8(0,0,0,0,_mm_extract_pi16(res1,3),_mm_extract_pi16(res1,2),_mm_extract_pi16(res1,1),_mm_extract_pi16(res1,0));
	res =  _mm_avg_pu8(res,res1);//(ptr_img[0] + ptr_rf[0] + 1) >> 1



	result = _mm_cvtsi64_si32(res); 
	empty();
	return result;

}






//n_2
 __m64 interpolvline64_mmx(unsigned char* image, const unsigned short PicWidthInPix){

 
	__m64 mm_A = _mm_set_pi16(image[1 * PicWidthInPix],image[0],image[-1 * PicWidthInPix],image[-2 * PicWidthInPix]); 
	__m64 mm_B = _mm_set_pi16(image[2 * PicWidthInPix],image[1 * PicWidthInPix],image[0],image[-1 * PicWidthInPix]); 
	__m64 mm_C = _mm_set_pi16(image[3 * PicWidthInPix],image[2 * PicWidthInPix],image[1 * PicWidthInPix],image[0]); 
	__m64 mm_D = _mm_set_pi16(image[4 * PicWidthInPix],image[3 * PicWidthInPix],image[2 * PicWidthInPix],image[1 * PicWidthInPix]);
	__m64 mm_E = _mm_set_pi16(image[5 * PicWidthInPix],image[4 * PicWidthInPix],image[3 * PicWidthInPix],image[2 * PicWidthInPix]);
	__m64 mm_F = _mm_set_pi16(image[6 * PicWidthInPix],image[5 * PicWidthInPix],image[4 * PicWidthInPix],image[3 * PicWidthInPix]);


	__m64 mm_AF = _mm_add_pi16(mm_A,mm_F);//A + F
	__m64 mm_BE = _mm_add_pi16(mm_B,mm_E);//B + E
	__m64 mm_CD = _mm_add_pi16(mm_C,mm_D);//C + D
	__m64 mm_CDS = _mm_slli_pi16(mm_CD,2);//(C + D) << 2
	__m64 mm_inter1 = _mm_sub_pi16(mm_CDS,mm_BE);//((C + D) << 2)-(B + E)
	__m64 mm_5 = _mm_set_pi16(5,5,5,5);
	__m64 mm_inter_3 = _mm_mullo_pi16(mm_inter1, mm_5);//(((C + D) << 2)-(B + E))*5

	__m64 mm_result = _mm_add_pi16(mm_inter_3,mm_AF);//A + F + 16 + (((C + D) << 2)-(B + E))*5

	 empty();  
	return(mm_result);
}




__m64 interpolhline64_1_mmx(__m64* temp){


	__m64 res,res1;
	__m64 ptr = _mm_setzero_si64();
	__m64 mm_16 = _mm_set_pi16(16,16,16,16);
	
	short A = _mm_extract_pi16(temp[0],0);
	short B = _mm_extract_pi16(temp[1],0);
	short C = _mm_extract_pi16(temp[2],0);
	short D = _mm_extract_pi16(temp[3],0);
	short E = _mm_extract_pi16(temp[4],0);
	short F = _mm_extract_pi16(temp[5],0);

	unsigned int result = A + F - 5 * (short)(B + E) + 20 * (short)(C + D) + 512;
	ptr = _mm_insert_pi16(ptr,CLIP255_16(result >> 10),0);

	 A = _mm_extract_pi16(temp[0],1);
	 B = _mm_extract_pi16(temp[1],1);
	 C = _mm_extract_pi16(temp[2],1);
	 D = _mm_extract_pi16(temp[3],1);
	 E = _mm_extract_pi16(temp[4],1);
	 F = _mm_extract_pi16(temp[5],1);

	result = A + F - 5 * (short)(B + E) + 20 * (short)(C + D) + 512;
	ptr = _mm_insert_pi16(ptr,CLIP255_16(result >> 10),1);

	 A = _mm_extract_pi16(temp[0],2);
	 B = _mm_extract_pi16(temp[1],2);
	 C = _mm_extract_pi16(temp[2],2);
	 D = _mm_extract_pi16(temp[3],2);
	 E = _mm_extract_pi16(temp[4],2);
	 F = _mm_extract_pi16(temp[5],2);

	result = A + F - 5 * (short)(B + E) + 20 * (short)(C + D) + 512;
	ptr = _mm_insert_pi16(ptr,CLIP255_16(result >> 10),2);

	 A = _mm_extract_pi16(temp[0],3);
	 B = _mm_extract_pi16(temp[1],3);
	 C = _mm_extract_pi16(temp[2],3);
	 D = _mm_extract_pi16(temp[3],3);
	 E = _mm_extract_pi16(temp[4],3);
	 F = _mm_extract_pi16(temp[5],3);

	result = A + F - 5 * (short)(B + E) + 20 * (short)(C + D) + 512;
	ptr = _mm_insert_pi16(ptr,CLIP255_16(result >> 10),3);
	

	res = _mm_add_pi16(temp[2],mm_16);
	res1 = _mm_srai_pi16(res,5);
	res1 = _mm_max_pi16(res1,_mm_set_pi16(0,0,0,0));
	res1 = _mm_min_pi16(res1,_mm_set_pi16(255,255,255,255)); //Clip

	res =  _mm_avg_pu16(ptr,res1);//(ptr_img[0] + ptr_rf[0] + 1) >> 1*/
	
	return res;

}


__m64 interpolhline64_3_mmx(__m64* temp){


	__m64 res,res1;
	__m64 ptr = _mm_setzero_si64();
	__m64 mm_16 = _mm_set_pi16(16,16,16,16);
	
	short A = _mm_extract_pi16(temp[0],0);
	short B = _mm_extract_pi16(temp[1],0);
	short C = _mm_extract_pi16(temp[2],0);
	short D = _mm_extract_pi16(temp[3],0);
	short E = _mm_extract_pi16(temp[4],0);
	short F = _mm_extract_pi16(temp[5],0);

	unsigned int result = A + F - 5 * (short)(B + E) + 20 * (short)(C + D) + 512;
	ptr = _mm_insert_pi16(ptr,CLIP255_16(result >> 10),0);

	 A = _mm_extract_pi16(temp[0],1);
	 B = _mm_extract_pi16(temp[1],1);
	 C = _mm_extract_pi16(temp[2],1);
	 D = _mm_extract_pi16(temp[3],1);
	 E = _mm_extract_pi16(temp[4],1);
	 F = _mm_extract_pi16(temp[5],1);

	result = A + F - 5 * (short)(B + E) + 20 * (short)(C + D) + 512;
	ptr = _mm_insert_pi16(ptr,CLIP255_16(result >> 10),1);

	 A = _mm_extract_pi16(temp[0],2);
	 B = _mm_extract_pi16(temp[1],2);
	 C = _mm_extract_pi16(temp[2],2);
	 D = _mm_extract_pi16(temp[3],2);
	 E = _mm_extract_pi16(temp[4],2);
	 F = _mm_extract_pi16(temp[5],2);

	result = A + F - 5 * (short)(B + E) + 20 * (short)(C + D) + 512;
	ptr = _mm_insert_pi16(ptr,CLIP255_16(result >> 10),2);

	 A = _mm_extract_pi16(temp[0],3);
	 B = _mm_extract_pi16(temp[1],3);
	 C = _mm_extract_pi16(temp[2],3);
	 D = _mm_extract_pi16(temp[3],3);
	 E = _mm_extract_pi16(temp[4],3);
	 F = _mm_extract_pi16(temp[5],3);

	result = A + F - 5 * (short)(B + E) + 20 * (short)(C + D) + 512;
	ptr = _mm_insert_pi16(ptr,CLIP255_16(result >> 10),3);
	

	res = _mm_add_pi16(temp[3],mm_16);
	res1 = _mm_srai_pi16(res,5);
	res1 = _mm_max_pi16(res1,_mm_set_pi16(0,0,0,0));
	res1 = _mm_min_pi16(res1,_mm_set_pi16(255,255,255,255)); //Clip

	res =  _mm_avg_pu16(ptr,res1);//(ptr_img[0] + ptr_rf[0] + 1) >> 1*/
	
	return res;

}

#endif



/*
unsigned int interpolvline64_2_mmx(	__m64* temp){

	__m64 mm_A = temp[0]; 
	__m64 mm_B = temp[1];
	__m64 mm_C = temp[2];
	__m64 mm_D = temp[3];
	__m64 mm_E = temp[4];
	__m64 mm_F = temp[5];

	__m64 mm_512 = _mm_set1_pi16(512);
	__m64 mm_AF = _mm_add_pi16(mm_A,mm_F);//A + F
	__m64 mm_inter = _mm_add_pi16(mm_AF,mm_512);//A + F + 512
	__m64 mm_BE = _mm_add_pi16(mm_B,mm_E);//B + E
	__m64 mm_CD = _mm_add_pi16(mm_C,mm_D);//C + D


	__m64 mm_CDS = _mm_slli_pi16(mm_CD,2);//(C + D) << 2
	__m64 mm_inter1 = _mm_sub_pi16(mm_CDS,mm_BE);//((C + D) << 2)-(B + E)
	__m64 mm_0 = _mm_set1_pi16(0);
	__m64 mmm_l = _mm_unpacklo_pi16(mm_inter1,mm_0);
	__m64 mmm_h = _mm_unpackhi_pi16(mm_inter1,mm_0);
	__m64 mm_5 = _mm_set_pi16(0,5,0,5);
	__m64 mm_inter0_l = _mm_mullo_pi16(mmm_l, mm_5); //(((C + D) << 2)-(B + E)) * 5 result on 32 bits so 4 int
    __m64 mm_inter0_h = _mm_mulhi_pu16(mmm_l, mm_5);
	__m64 mm_inter1_l = _mm_mullo_pi16(mmm_h, mm_5);
    __m64 mm_inter1_h = _mm_mulhi_pu16(mmm_h, mm_5);

	//can not work with a pi32 because we have to be in unsigned short
	__m64 mm_mul_l = _mm_setr_pi16(_mm_extract_pi16(mm_inter0_l,0),_mm_extract_pi16(mm_inter0_h,0),_mm_extract_pi16(mm_inter0_l,2),_mm_extract_pi16(mm_inter0_h,2));
	__m64 mm_mul_h = _mm_setr_pi16(_mm_extract_pi16(mm_inter1_l,0),_mm_extract_pi16(mm_inter1_h,0),_mm_extract_pi16(mm_inter1_l,2),_mm_extract_pi16(mm_inter1_h,2)); //reorder in the right order the result


	__m64 mm_inter_32_l = _mm_setr_pi32(_mm_extract_pi16(mm_inter,0),_mm_extract_pi16(mm_inter,1));//Put (A + F + 512) on 32 bits
	__m64 mm_inter_32_h = _mm_setr_pi32(_mm_extract_pi16(mm_inter,2),_mm_extract_pi16(mm_inter,3));

	__m64 mm_result_l = _mm_add_pi32(mm_mul_l,mm_inter_32_l);//A + F + 512 + (((C + D) << 2)-(B + E))*5
	__m64 mm_result_h = _mm_add_pi32(mm_mul_h,mm_inter_32_h);//A + F + 512 + (((C + D) << 2)-(B + E))*5
	

	__m64 mm_retl = _mm_srai_pi32(mm_result_l,10);//result >> 10
	__m64 mm_reth = _mm_srai_pi32(mm_result_h,10);//result >> 10


	__m64 result =_mm_set_pi8(0,0,0,0,_mm_extract_pi16(mm_reth,2),_mm_extract_pi16(mm_reth,0),_mm_extract_pi16(mm_retl,2),_mm_extract_pi16(mm_retl,0));

	unsigned int ret = _mm_cvtsi64_si32(result);

	_mm_empty();  

	return ret;

}


unsigned int interpolvline64_3_mmx(	__m64* temp){

	__m64 mm_A = temp[0]; 
	__m64 mm_B = temp[1];
	__m64 mm_C = temp[2];
	__m64 mm_D = temp[3];
	__m64 mm_E = temp[4];
	__m64 mm_F = temp[5];

	__m64 mm_512 = _mm_set1_pi16(512);
	__m64 mm_16 = _mm_set_pi16(16,16,16,16);
	__m64 mm_AF = _mm_add_pi16(mm_A,mm_F);//A + F
	__m64 mm_inter = _mm_add_pi16(mm_AF,mm_512);//A + F + 512
	__m64 mm_BE = _mm_add_pi16(mm_B,mm_E);//B + E
	__m64 mm_CD = _mm_add_pi16(mm_C,mm_D);//C + D

	__m64 mm_I16 = _mm_add_pi16(mm_D,mm_16);//tmp_res[Offset] + 16

	__m64 mm_CDS = _mm_slli_pi16(mm_CD,2);//(C + D) << 2
	__m64 mm_inter1 = _mm_sub_pi16(mm_CDS,mm_BE);//((C + D) << 2)-(B + E)
	__m64 mm_0 = _mm_set1_pi16(0);
	__m64 mmm_l = _mm_unpacklo_pi16(mm_inter1,mm_0);
	__m64 mmm_h = _mm_unpackhi_pi16(mm_inter1,mm_0);
	__m64 mm_5 = _mm_set_pi16(0,5,0,5);
	__m64 mm_inter0_l = _mm_mullo_pi16(mmm_l, mm_5); //(((C + D) << 2)-(B + E)) * 5 result on 32 bits so 4 int
    __m64 mm_inter0_h = _mm_mulhi_pu16(mmm_l, mm_5);
	__m64 mm_inter1_l = _mm_mullo_pi16(mmm_h, mm_5);
    __m64 mm_inter1_h = _mm_mulhi_pu16(mmm_h, mm_5);

	//can not work with a pi32 because we have to be in unsigned short
	__m64 mm_mul_l = _mm_setr_pi16(_mm_extract_pi16(mm_inter0_l,0),_mm_extract_pi16(mm_inter0_h,0),_mm_extract_pi16(mm_inter0_l,2),_mm_extract_pi16(mm_inter0_h,2));
	__m64 mm_mul_h = _mm_setr_pi16(_mm_extract_pi16(mm_inter1_l,0),_mm_extract_pi16(mm_inter1_h,0),_mm_extract_pi16(mm_inter1_l,2),_mm_extract_pi16(mm_inter1_h,2)); //reorder in the right order the result


	__m64 mm_inter_32_l = _mm_setr_pi32(_mm_extract_pi16(mm_inter,0),_mm_extract_pi16(mm_inter,1));//Put (A + F + 512) on 32 bits
	__m64 mm_inter_32_h = _mm_setr_pi32(_mm_extract_pi16(mm_inter,2),_mm_extract_pi16(mm_inter,3));

	__m64 mm_result_l = _mm_add_pi32(mm_mul_l,mm_inter_32_l);//A + F + 512 + (((C + D) << 2)-(B + E))*5
	__m64 mm_result_h = _mm_add_pi32(mm_mul_h,mm_inter_32_h);//A + F + 512 + (((C + D) << 2)-(B + E))*5
	

	__m64 mm_retl = _mm_srai_pi32(mm_result_l,10);//result >> 10
	__m64 mm_reth = _mm_srai_pi32(mm_result_h,10);//result >> 10

	__m64 mm_I5 = _mm_srai_pi16(mm_I16,5);

	__m64 res0 =_mm_set_pi8(0,0,0,0,_mm_extract_pi16(mm_I5,3),_mm_extract_pi16(mm_I5,2),_mm_extract_pi16(mm_I5,1),_mm_extract_pi16(mm_I5,0));
	__m64 res1 =_mm_set_pi8(0,0,0,0,_mm_extract_pi16(mm_reth,2),_mm_extract_pi16(mm_reth,0),_mm_extract_pi16(mm_retl,2),_mm_extract_pi16(mm_retl,0));
	__m64 result =  _mm_avg_pu8(res0,res1);//(ptr_img[0] + ptr_rf[0] + 1) >> 1

	unsigned int ret = _mm_cvtsi64_si32(result);

	 _mm_empty();  

	return ret;

}






__m64 interpolvline_mmx(__m64* temp){

	

	__m64 mm_A = temp[0]; 
	__m64 mm_B = temp[1];
	__m64 mm_C = temp[2];
	__m64 mm_D = temp[3];
	__m64 mm_E = temp[4];
	__m64 mm_F = temp[5];

	__m64 mm_512 = _mm_set1_pi16(512);
	__m64 mm_16 = _mm_set_pi16(16,16,16,16);
	__m64 mm_AF = _mm_add_pi16(mm_A,mm_F);//A + F
	__m64 mm_inter = _mm_add_pi16(mm_AF,mm_512);//A + F + 512
	__m64 mm_BE = _mm_add_pi16(mm_B,mm_E);//B + E
	__m64 mm_CD = _mm_add_pi16(mm_C,mm_D);//C + D


	__m64 mm_0 = _mm_set1_pi16(0);
	__m64 mm_CDl = _mm_unpacklo_pi16(mm_CD,mm_0);
	__m64 mm_BEl = _mm_unpacklo_pi16(mm_BE,mm_0);
	__m64 mm_CDh = _mm_unpackhi_pi16(mm_CD,mm_0);
	__m64 mm_BEh = _mm_unpackhi_pi16(mm_BE,mm_0);


	__m64 mm_CDSl = _mm_slli_pi16(mm_CDl,2);//(C + D) << 2
	__m64 mm_CDSh = _mm_slli_pi16(mm_CDh,2);//(C + D) << 2

	__m64 mm_I16 = _mm_add_pi16(mm_C,mm_16);//tmp_res[Offset] + 16
	__m64 mm_5 = _mm_set_pi16(0,5,0,5);

	__m64 mm_interCD_ll = _mm_mullo_pi16(mm_CDSl, mm_5); //(((C + D) << 2)) * 5 result on 32 bits so 4 int
	__m64 mm_interCD_hl = _mm_mullo_pi16(mm_CDSh, mm_5);
	__m64 mm_interBE_ll = _mm_mullo_pi16(mm_BEl, mm_5); //(B + F) * 5 result on 32 bits so 4 int
	__m64 mm_interBE_hl = _mm_mullo_pi16(mm_BEh, mm_5);

	__m64 mm_interCD_lh = _mm_mulhi_pi16(mm_CDSl, mm_5);
    __m64 mm_interCD_hh = _mm_mulhi_pi16(mm_CDSh, mm_5);
	__m64 mm_interBE_lh = _mm_mulhi_pi16(mm_BEl, mm_5);
    __m64 mm_interBE_hh = _mm_mulhi_pi16(mm_BEh, mm_5);

	__m64 mm_CD_l =  _mm_add_pi16(mm_interCD_ll,_mm_shuffle_pi16(mm_interCD_lh,_MM_SHUFFLE(2,3,0,1)));
	__m64 mm_CD_h =  _mm_add_pi16(mm_interCD_hl,_mm_shuffle_pi16(mm_interCD_hh,_MM_SHUFFLE(2,3,0,1)));
	__m64 mm_BE_l =  _mm_add_pi16(mm_interBE_ll,_mm_shuffle_pi16(mm_interBE_lh,_MM_SHUFFLE(2,3,0,1)));
	__m64 mm_BE_h =  _mm_add_pi16(mm_interBE_hl,_mm_shuffle_pi16(mm_interBE_hh,_MM_SHUFFLE(2,3,0,1)));

	__m64 mm_inter_32_l = _mm_setr_pi32(_mm_extract_pi16(mm_inter,0),_mm_extract_pi16(mm_inter,1));//Put (A + F + 512) on 32 bits
	__m64 mm_inter_32_h = _mm_setr_pi32(_mm_extract_pi16(mm_inter,2),_mm_extract_pi16(mm_inter,3));

	__m64 mm_inter_l = _mm_sub_pi32(mm_CD_l,mm_BE_l);
	__m64 mm_inter_h = _mm_sub_pi32(mm_CD_h,mm_BE_h);

	__m64 mm_add_l = _mm_add_pi32(mm_inter_l,mm_inter_32_l);
	__m64 mm_add_h = _mm_add_pi32(mm_inter_h,mm_inter_32_h);


	__m64 mm_retl = _mm_srai_pi32(mm_add_l,10);//result >> 10
	__m64 mm_reth = _mm_srai_pi32(mm_add_h,10);//result >> 10
	__m64 mm_I5 = _mm_srai_pi16(mm_I16,5);

	__m64 res =_mm_set_pi16(_mm_extract_pi16(mm_reth,2),_mm_extract_pi16(mm_reth,0),_mm_extract_pi16(mm_retl,2),_mm_extract_pi16(mm_retl,0));
	__m64 mm_255 = _mm_set_pi16(255,255,255,255);
	__m64 mm_retCl = _mm_max_pi16(_mm_min_pi16(res,mm_255),mm_0);
	__m64 mm_I5Cl = _mm_max_pi16(_mm_min_pi16(mm_I5,mm_255),mm_0);
	__m64 result = _mm_avg_pu8(mm_I5Cl, mm_retCl);



	return result;
	// _mm_empty();  
}

*/


