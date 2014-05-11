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



#include "data.h"


/**
This function allows to store the prediction of an non IDR picture. 
//8.4.2.2


@param aio_tImage Table of the current decoded picture buffer.
@param aio_tImage_Cb Table of the current decoded picture buffer.
@param aio_tImage_Cr Table of the current decoded picture buffer.
@param Luma_l0 Table of current frame.
@param Luma_l1 Table of current frame.
@param Chroma_Cb_l0 Table of current frame.
@param Chroma_Cb_l1 Table of current frame.
@param Chroma_Cr_l0 Table of current frame.
@param Chroma_Cr_l1 Table of current frame.
@param ref_cache_l0 A cache table where the reference is stocked for each 4x4 block of each macroblock.
@param ref_cache_l1 A cache table where the reference is stocked for each 4x4 block of each macroblock.
@param PicWidthInPix Width in pixel of the current frame..
*/
#ifdef TI_OPTIM
void set_image_4x4_GDEM(unsigned char aio_tImage[RESTRICT], unsigned char aio_tImage_Cb[RESTRICT], unsigned char aio_tImage_Cr[RESTRICT], 
						unsigned char  Luma_l0[RESTRICT], unsigned char  Luma_l1[RESTRICT], unsigned char  Chroma_Cb_l0[RESTRICT], 
						unsigned char Chroma_Cb_l1[RESTRICT], unsigned char  Chroma_Cr_l0[RESTRICT], unsigned char Chroma_Cr_l1[RESTRICT], 
						short *ref_cache_l0, short *ref_cache_l1, short  PicWidthInPix)
{

	int h;
	unsigned char  * dir_aio;
	int dir_inter;
	short PicWidthInPix1 = PicWidthInPix >> 1;
	short PicWidthInPix2 = PicWidthInPix >> 2;
	short PicWidthInPix4 = PicWidthInPix << 1; 
	short PicWidthInPixx3 = PicWidthInPix4 + PicWidthInPix; 
	short PicWidthInPixx3_2 = PicWidthInPixx3 >> 2;



	char index8[16] = {0xC,0xD,0x14,0x15,0xE,0xF,0x16,0x17,0x1C,0x1D,0x24,0x25,0x1E,0x1F,0x26,0x27};
	char locx[16]   = {0x0,0x4,0x0,0x4,0x8,0xC,0x8,0xC,0x0,0x4,0x0,0x4,0x8,0xC,0x8,0xC};
	char locy[16]   = {0x0,0x0,0x4,0x4,0x0,0x0,0x4,0x4,0x8,0x8,0xC,0xC,0x8,0x8,0xC,0xC};
	char x_ch[16]   = {0x0,0x2,0x0,0x2,0x4,0x6,0x4,0x6,0x0,0x2,0x0,0x2,0x4,0x6,0x4,0x6};
	char y_ch[16]   = {0x0,0x0,0x2,0x2,0x0,0x0,0x2,0x2,0x4,0x4,0x6,0x6,0x4,0x4,0x6,0x6};
	for( h = 0; h < 16; h++)
	{
		if ( ref_cache_l0[index8[h]] >= 0 )
		{
			if( ref_cache_l1[index8[h]] < 0 )
			{
				/**/unsigned short *ptr_Chroma_Cb_l0 = (unsigned short *) Chroma_Cb_l0;
				unsigned short *ptr_Chroma_Cr_l0 = (unsigned short *) Chroma_Cr_l0;
				unsigned short *ptr_aio_tImage_Cb = (unsigned short *) (&aio_tImage_Cb[y_ch[h] * (PicWidthInPix1) + x_ch[h]]);
				unsigned short *ptr_aio_tImage_Cr = (unsigned short *) (&aio_tImage_Cr[y_ch[h] * (PicWidthInPix1) + x_ch[h]]);

				unsigned int *ptr_Luma_l0 = (unsigned int *) Luma_l0;
				unsigned int *ptr_aio_tImage = (unsigned int *) (&aio_tImage[(locy[h]) * PicWidthInPix + locx[h]]);






				ptr_aio_tImage[0] = *ptr_Luma_l0++;
				ptr_aio_tImage[PicWidthInPix2] = *ptr_Luma_l0++;
				ptr_aio_tImage[PicWidthInPix1] = *ptr_Luma_l0++;
				ptr_aio_tImage[PicWidthInPixx3_2] = *ptr_Luma_l0;

				ptr_aio_tImage_Cb[0] = *ptr_Chroma_Cb_l0++;
				ptr_aio_tImage_Cb[PicWidthInPix2] = *ptr_Chroma_Cb_l0;
				ptr_aio_tImage_Cr[0] = *ptr_Chroma_Cr_l0++;
				ptr_aio_tImage_Cr[PicWidthInPix2] = *ptr_Chroma_Cr_l0;



			}
			else
			{			


				dir_inter = ((locy[h]) * PicWidthInPix + locx[h]);
				//Iteracion 0
				dir_aio=  & aio_tImage[dir_inter];
				_mem4(dir_aio)=_packl4(_shru2(_add2(_add2(_pack2(Luma_l0[3],Luma_l0[2]),_pack2(Luma_l1[3],Luma_l1[2])),0x00010001),0x00010001),_shru2(_add2(_add2(_pack2(Luma_l0[1],Luma_l0[0]),_pack2(Luma_l1[1],Luma_l1[0])),0x00010001),0x00010001));

				//Iteracion 1
				dir_aio=  & aio_tImage[ (dir_inter + PicWidthInPix) ];

				_mem4(dir_aio)=_packl4(_shru2(_add2(_add2(_pack2(Luma_l0[7],Luma_l0[6]),_pack2(Luma_l1[7],Luma_l1[6])),0x00010001),0x00010001),_shru2(_add2(_add2(_pack2(Luma_l0[5],Luma_l0[4]),_pack2(Luma_l1[5],Luma_l1[4])),0x00010001),0x00010001));

				//Iteracion 2
				dir_aio=  & aio_tImage[ (dir_inter + (PicWidthInPix4))];
				_mem4(dir_aio)=_packl4(_shru2(_add2(_add2(_pack2(Luma_l0[11],Luma_l0[10]),_pack2(Luma_l1[11],Luma_l1[10])),0x00010001),0x00010001),_shru2(_add2(_add2(_pack2(Luma_l0[9],Luma_l0[8]),_pack2(Luma_l1[9],Luma_l1[8])),0x00010001),0x00010001));

				//Iteracion 3
				dir_aio=  & aio_tImage[ (dir_inter + (PicWidthInPixx3))];
				_mem4(dir_aio)=_packl4(_shru2(_add2(_add2(_pack2(Luma_l0[15],Luma_l0[14]),_pack2(Luma_l1[15],Luma_l1[14])),0x00010001),0x00010001),_shru2(_add2(_add2(_pack2(Luma_l0[13],Luma_l0[12]),_pack2(Luma_l1[13],Luma_l1[12])),0x00010001),0x00010001));




				/* */

				dir_inter = (y_ch[h]) * (PicWidthInPix1) + x_ch[h];
				//Iteracion 0
				dir_aio= & aio_tImage_Cb[ dir_inter];
				_mem2(dir_aio)=_packl4(0,_shru2(_add2(_add2(_pack2(Chroma_Cb_l0[1],Chroma_Cb_l0[0]),_pack2(Chroma_Cb_l1[1],Chroma_Cb_l1[0])),0x00010001),0x00010001));
				dir_aio= & aio_tImage_Cr[ dir_inter];
				_mem2(dir_aio)=_packl4(0,_shru2(_add2(_add2(_pack2(Chroma_Cr_l0[1],Chroma_Cr_l0[0]),_pack2(Chroma_Cr_l1[1],Chroma_Cr_l1[0])),0x00010001),0x00010001));

				//Iteracion 1
				dir_aio= & aio_tImage_Cb[ dir_inter + PicWidthInPix1];

				_mem2(dir_aio)=_packl4(0,_shru2(_add2(_add2(_pack2(Chroma_Cb_l0[3],Chroma_Cb_l0[2]),_pack2(Chroma_Cb_l1[3],Chroma_Cb_l1[2])),0x00010001),0x00010001));

				dir_aio= & aio_tImage_Cr[ dir_inter + PicWidthInPix1];
				_mem2(dir_aio)=_packl4(0,_shru2(_add2(_add2(_pack2(Chroma_Cr_l0[3],Chroma_Cr_l0[2]),_pack2(Chroma_Cr_l1[3],Chroma_Cr_l1[2])),0x00010001),0x00010001));




			}
		}
		else
		{
			unsigned short *ptr_Chroma_Cb_l1 = (unsigned short *) Chroma_Cb_l1;
			unsigned short *ptr_Chroma_Cr_l1 = (unsigned short *) Chroma_Cr_l1;
			unsigned short *ptr_aio_tImage_Cb = (unsigned short *) (&aio_tImage_Cb[y_ch[h] * (PicWidthInPix1) + x_ch[h]]);
			unsigned short *ptr_aio_tImage_Cr = (unsigned short *) (&aio_tImage_Cr[y_ch[h] * (PicWidthInPix1) + x_ch[h]]);

			unsigned int *ptr_Luma_l1 = (unsigned int *) Luma_l1;
			unsigned int *ptr_aio_tImage = (unsigned int *) (&aio_tImage[(locy[h]) * PicWidthInPix + locx[h]]);

			ptr_aio_tImage[0] = *ptr_Luma_l1++;
			ptr_aio_tImage[PicWidthInPix2] = *ptr_Luma_l1++;
			ptr_aio_tImage[PicWidthInPix1] = *ptr_Luma_l1++;
			ptr_aio_tImage[PicWidthInPixx3_2] = *ptr_Luma_l1;


			ptr_aio_tImage_Cb[0] = *ptr_Chroma_Cb_l1++;
			ptr_aio_tImage_Cb[PicWidthInPix2] = *ptr_Chroma_Cb_l1;
			ptr_aio_tImage_Cr[0] = *ptr_Chroma_Cr_l1++;
			ptr_aio_tImage_Cr[PicWidthInPix2] = *ptr_Chroma_Cr_l1;



		}

		Luma_l1 += 1 << 4;
		Luma_l0 += 1 << 4;
		Chroma_Cb_l0 += 1 << 2;
		Chroma_Cb_l1 += 1 << 2;
		Chroma_Cr_l0 += 1 << 2;
		Chroma_Cr_l1 += 1 << 2;
	}
}
#else





void set_image_4x4_C(unsigned char *aio_tImage, unsigned char aio_tImage_Cb[], unsigned char aio_tImage_Cr[], 
					 unsigned char  *Luma_l0, unsigned char  *Luma_l1, unsigned char  *Chroma_Cb_l0, unsigned char *Chroma_Cb_l1, 
					 unsigned char  *Chroma_Cr_l0, unsigned char *Chroma_Cr_l1, short *ref_cache_l0, 
					 short *ref_cache_l1, short  PicWidthInPix)
{

	int h, i;

	for( h = 0; h < 16; h++){
		int locx = LOCX(h);
		int locy = LOCY(h);
		int x_ch = (locx) >> 1;
		int y_ch = (locy) >> 1;
		short index8 = SCAN8(h);


		if ( ref_cache_l0[index8] >= 0 ){
			if( ref_cache_l1[index8] < 0 ){
				unsigned short *ptr_Chroma_Cb_l0 = (unsigned short *) Chroma_Cb_l0;
				unsigned short *ptr_Chroma_Cr_l0 = (unsigned short *) Chroma_Cr_l0;
				unsigned short *ptr_aio_tImage_Cb = (unsigned short *) (&aio_tImage_Cb[y_ch * (PicWidthInPix >> 1) + x_ch]);
				unsigned short *ptr_aio_tImage_Cr = (unsigned short *) (&aio_tImage_Cr[y_ch * (PicWidthInPix >> 1) + x_ch]);

				unsigned int *ptr_Luma_l0 = (unsigned int *) Luma_l0;
				unsigned int *ptr_aio_tImage = (unsigned int *) (&aio_tImage[(locy) * PicWidthInPix + locx]);

				ptr_aio_tImage[0] = *ptr_Luma_l0++;
				ptr_aio_tImage[PicWidthInPix >> 2] = *ptr_Luma_l0++;
				ptr_aio_tImage[PicWidthInPix >> 1] = *ptr_Luma_l0++;
				ptr_aio_tImage[(3 * PicWidthInPix) >> 2] = *ptr_Luma_l0;

				ptr_aio_tImage_Cb[0] = *ptr_Chroma_Cb_l0++;
				ptr_aio_tImage_Cb[PicWidthInPix >> 2] = *ptr_Chroma_Cb_l0;
				ptr_aio_tImage_Cr[0] = *ptr_Chroma_Cr_l0++;
				ptr_aio_tImage_Cr[PicWidthInPix >> 2] = *ptr_Chroma_Cr_l0;

			}else{			

				for ( i = 0; i < 4; i++){
					aio_tImage[ (locy + i) * PicWidthInPix + locx ] = ( Luma_l0[ (i << 2)] + Luma_l1[ (i << 2)] + 1) >> 1;
					aio_tImage[ (locy + i) * PicWidthInPix + locx+ 1 ] = ( Luma_l0[ (i << 2) + 1] + Luma_l1[ (i << 2) + 1] + 1) >> 1;
					aio_tImage[ (locy + i) * PicWidthInPix + locx+ 2 ] = ( Luma_l0[ (i << 2) + 2] + Luma_l1[ (i << 2) + 2] + 1) >> 1;
					aio_tImage[ (locy + i) * PicWidthInPix + locx+ 3 ] = ( Luma_l0[ (i << 2) + 3] + Luma_l1[ (i << 2) + 3] + 1) >> 1;
				} 
				for ( i = 0; i < 2; i++){
					aio_tImage_Cb[ (y_ch + i) * (PicWidthInPix>>1) + x_ch] =	 ( Chroma_Cb_l1[(i << 1)]	  + Chroma_Cb_l0[(i << 1)]	   + 1) >> 1;
					aio_tImage_Cb[ (y_ch + i) * (PicWidthInPix>>1) + x_ch + 1] = ( Chroma_Cb_l1[(i << 1) + 1] + Chroma_Cb_l0[(i << 1) + 1] + 1) >> 1;
					aio_tImage_Cr[ (y_ch + i) * (PicWidthInPix>>1) + x_ch] =	 ( Chroma_Cr_l1[(i << 1)]	  + Chroma_Cr_l0[(i << 1)]	   + 1) >> 1;
					aio_tImage_Cr[ (y_ch + i) * (PicWidthInPix>>1) + x_ch + 1] = ( Chroma_Cr_l1[(i << 1) + 1] + Chroma_Cr_l0[(i << 1) + 1] + 1) >> 1;
				}
			}
		}else{
			unsigned short *ptr_Chroma_Cb_l1 = (unsigned short *) Chroma_Cb_l1;
			unsigned short *ptr_Chroma_Cr_l1 = (unsigned short *) Chroma_Cr_l1;
			unsigned short *ptr_aio_tImage_Cb = (unsigned short *) (&aio_tImage_Cb[y_ch * (PicWidthInPix >> 1) + x_ch]);
			unsigned short *ptr_aio_tImage_Cr = (unsigned short *) (&aio_tImage_Cr[y_ch * (PicWidthInPix >> 1) + x_ch]);

			unsigned int *ptr_Luma_l1 = (unsigned int *) Luma_l1;
			unsigned int *ptr_aio_tImage = (unsigned int *) (&aio_tImage[(locy) * PicWidthInPix + locx]);

			ptr_aio_tImage[0] = *ptr_Luma_l1++;
			ptr_aio_tImage[PicWidthInPix >> 2] = *ptr_Luma_l1++;
			ptr_aio_tImage[PicWidthInPix >> 1] = *ptr_Luma_l1++;
			ptr_aio_tImage[(3 * PicWidthInPix) >> 2] = *ptr_Luma_l1;


			ptr_aio_tImage_Cb[0] = *ptr_Chroma_Cb_l1++;
			ptr_aio_tImage_Cb[PicWidthInPix >> 2] = *ptr_Chroma_Cb_l1;
			ptr_aio_tImage_Cr[0] = *ptr_Chroma_Cr_l1++;
			ptr_aio_tImage_Cr[PicWidthInPix >> 2] = *ptr_Chroma_Cr_l1;

		}

		Luma_l1 += 1 << 4;
		Luma_l0 += 1 << 4;
		Chroma_Cb_l0 += 1 << 2;
		Chroma_Cb_l1 += 1 << 2;
		Chroma_Cr_l0 += 1 << 2;
		Chroma_Cr_l1 += 1 << 2;
	}
}
#endif


#ifdef MMXi


//mbl test with Pocket_PC		
void set_image_4x4_MMX(unsigned char *aio_tImage , unsigned char aio_tImage_Cb[], unsigned char aio_tImage_Cr[], 
					   unsigned char  *Luma_l0, unsigned char  *Luma_l1, unsigned char  *Chroma_Cb_l0, unsigned char *Chroma_Cb_l1, 
					   unsigned char  *Chroma_Cr_l0, unsigned char *Chroma_Cr_l1, short *ref_cache_l0, 
					   short *ref_cache_l1, short  PicWidthInPix)
{

	int h;

	for( h = 0; h < 16; h++){
		int locx = LOCX(h);
		int locy = LOCY(h);
		int x_ch = (locx) >> 1;
		int y_ch = (locy) >> 1;
		short index8 = SCAN8(h);


		if ( ref_cache_l0[index8] >= 0 ){
			if( ref_cache_l1[index8] < 0 ){
				unsigned short *ptr_Chroma_Cb_l0 = (unsigned short *) Chroma_Cb_l0;
				unsigned short *ptr_Chroma_Cr_l0 = (unsigned short *) Chroma_Cr_l0;
				unsigned short *ptr_aio_tImage_Cb = (unsigned short *) (&aio_tImage_Cb[y_ch * (PicWidthInPix >> 1) + x_ch]);
				unsigned short *ptr_aio_tImage_Cr = (unsigned short *) (&aio_tImage_Cr[y_ch * (PicWidthInPix >> 1) + x_ch]);

				unsigned int *ptr_Luma_l0 = (unsigned int *) Luma_l0;
				unsigned int *ptr_aio_tImage = (unsigned int *) (&aio_tImage[(locy) * PicWidthInPix + locx]);

				ptr_aio_tImage[0] = *ptr_Luma_l0++;
				ptr_aio_tImage[PicWidthInPix >> 2] = *ptr_Luma_l0++;
				ptr_aio_tImage[PicWidthInPix >> 1] = *ptr_Luma_l0++;
				ptr_aio_tImage[(3 * PicWidthInPix) >> 2] = *ptr_Luma_l0;

				ptr_aio_tImage_Cb[0] = *ptr_Chroma_Cb_l0++;
				ptr_aio_tImage_Cb[PicWidthInPix >> 2] = *ptr_Chroma_Cb_l0;
				ptr_aio_tImage_Cr[0] = *ptr_Chroma_Cr_l0++;
				ptr_aio_tImage_Cr[PicWidthInPix >> 2] = *ptr_Chroma_Cr_l0;

			}else{
				__m128i l_l0 = _mm_loadu_si128((__m128i *) Luma_l0);
				__m128i l_l1 = _mm_loadu_si128((__m128i *) Luma_l1);
				__m128i l_res = _mm_avg_epu8 (l_l0, l_l1);
				__m128i Cb_l0 = _mm_loadu_si128((__m128i *) Chroma_Cb_l0);
				__m128i Cb_l1 = _mm_loadu_si128((__m128i *) Chroma_Cb_l1);
				__m128i Cb_res = _mm_avg_epu8 (Cb_l0, Cb_l1);
				__m128i Cr_l0 = _mm_loadu_si128((__m128i *) Chroma_Cr_l0);
				__m128i Cr_l1 = _mm_loadu_si128((__m128i *) Chroma_Cr_l1);
				__m128i Cr_res = _mm_avg_epu8 (Cr_l0, Cr_l1);

				unsigned int *ptr_aio_tImage = (unsigned int *) (&aio_tImage[(locy) * PicWidthInPix + locx]);
				unsigned short *ptr_aio_tImage_Cb = (unsigned short *) (&aio_tImage_Cb[y_ch * (PicWidthInPix >> 1) + x_ch]);
				unsigned short *ptr_aio_tImage_Cr = (unsigned short *) (&aio_tImage_Cr[y_ch * (PicWidthInPix >> 1) + x_ch]);

				ptr_aio_tImage[0] = _mm_cvtsi128_si32(l_res);
				ptr_aio_tImage[PicWidthInPix >> 2] = _mm_cvtsi128_si32(_mm_shuffle_epi32(l_res,_MM_SHUFFLE(0,3,2,1)));
				ptr_aio_tImage[PicWidthInPix >> 1] = _mm_cvtsi128_si32(_mm_shuffle_epi32(l_res,_MM_SHUFFLE(1,0,3,2)));
				ptr_aio_tImage[(3 * PicWidthInPix) >> 2] = _mm_cvtsi128_si32(_mm_shuffle_epi32(l_res,_MM_SHUFFLE(2,1,0,3)));

				ptr_aio_tImage_Cb[0] = (short) _mm_extract_epi16(Cb_res,0); 
				ptr_aio_tImage_Cb[PicWidthInPix >> 2] = (short) _mm_extract_epi16(Cb_res,1); 
				ptr_aio_tImage_Cr[0] = (short) _mm_extract_epi16(Cr_res,0); 
				ptr_aio_tImage_Cr[PicWidthInPix >> 2] = (short) _mm_extract_epi16(Cr_res,1); 



			}
		}else{
			unsigned short *ptr_Chroma_Cb_l1 = (unsigned short *) Chroma_Cb_l1;
			unsigned short *ptr_Chroma_Cr_l1 = (unsigned short *) Chroma_Cr_l1;
			unsigned short *ptr_aio_tImage_Cb = (unsigned short *) (&aio_tImage_Cb[y_ch * (PicWidthInPix >> 1) + x_ch]);
			unsigned short *ptr_aio_tImage_Cr = (unsigned short *) (&aio_tImage_Cr[y_ch * (PicWidthInPix >> 1) + x_ch]);

			unsigned int *ptr_Luma_l1 = (unsigned int *) Luma_l1;
			unsigned int *ptr_aio_tImage = (unsigned int *) (&aio_tImage[(locy) * PicWidthInPix + locx]);

			ptr_aio_tImage[0] = *ptr_Luma_l1++;
			ptr_aio_tImage[PicWidthInPix >> 2] = *ptr_Luma_l1++;
			ptr_aio_tImage[PicWidthInPix >> 1] = *ptr_Luma_l1++;
			ptr_aio_tImage[(3 * PicWidthInPix) >> 2] = *ptr_Luma_l1;


			ptr_aio_tImage_Cb[0] = *ptr_Chroma_Cb_l1++;
			ptr_aio_tImage_Cb[PicWidthInPix >> 2] = *ptr_Chroma_Cb_l1;
			ptr_aio_tImage_Cr[0] = *ptr_Chroma_Cr_l1++;
			ptr_aio_tImage_Cr[PicWidthInPix >> 2] = *ptr_Chroma_Cr_l1;

		}

		Luma_l1 += 1 << 4;
		Luma_l0 += 1 << 4;
		Chroma_Cb_l0 += 1 << 2;
		Chroma_Cb_l1 += 1 << 2;
		Chroma_Cr_l0 += 1 << 2;
		Chroma_Cr_l1 += 1 << 2;
	}
}
#endif
