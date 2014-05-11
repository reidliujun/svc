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

#include "clip.h"
#include "data.h"
#include "interpolation_8x8.h"
#include "interpolation_main_8x8.h"






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
void set_image_8x8_C(unsigned char aio_tImage[RESTRICT] , unsigned char aio_tImage_Cb[RESTRICT], unsigned char aio_tImage_Cr[RESTRICT], 
					 unsigned char  Luma_l0[RESTRICT], unsigned char  Luma_l1[RESTRICT], unsigned char  Chroma_Cb_l0[RESTRICT], 
					 unsigned char Chroma_Cb_l1[RESTRICT], unsigned char  Chroma_Cr_l0[RESTRICT], unsigned char Chroma_Cr_l1[RESTRICT], 
					 short *ref_cache_l0, short *ref_cache_l1, short  PicWidthInPix)
{

	int h, i;

	for( h = 0; h < 4; h++){
		int index8 = SCAN8(h << 2);
		int locx =((h & 1) << 3);
		int locy =((h & 2) << 2);
		int x_ch = (locx) >> 1;
		int y_ch = (locy) >> 1;


		if ( ref_cache_l0[index8] > -1 ){
			if( ref_cache_l1[index8] < 0 ){

				for ( i = 0; i < 8; i++){
					aio_tImage[ (locy + i) * PicWidthInPix + locx] = Luma_l0[(i << 3)];
					aio_tImage[ (locy + i) * PicWidthInPix + locx + 1] = Luma_l0[(i << 3) + 1];
					aio_tImage[ (locy + i) * PicWidthInPix + locx + 2] = Luma_l0[(i << 3) + 2];
					aio_tImage[ (locy + i) * PicWidthInPix + locx + 3] = Luma_l0[(i << 3) + 3];
					aio_tImage[ (locy + i) * PicWidthInPix + locx + 4] = Luma_l0[(i << 3) + 4];
					aio_tImage[ (locy + i) * PicWidthInPix + locx + 5] = Luma_l0[(i << 3) + 5];
					aio_tImage[ (locy + i) * PicWidthInPix + locx + 6] = Luma_l0[(i << 3) + 6];
					aio_tImage[ (locy + i) * PicWidthInPix + locx + 7] = Luma_l0[(i << 3) + 7];

				}
				for ( i = 0; i < 4; i++){
					aio_tImage_Cb[ (y_ch + i) * (PicWidthInPix >> 1) + x_ch] = Chroma_Cb_l0[(i << 2)];
					aio_tImage_Cb[ (y_ch + i) * (PicWidthInPix >> 1) + x_ch + 1] = Chroma_Cb_l0[(i << 2) + 1];
					aio_tImage_Cb[ (y_ch + i) * (PicWidthInPix >> 1) + x_ch + 2] = Chroma_Cb_l0[(i << 2) + 2];
					aio_tImage_Cb[ (y_ch + i) * (PicWidthInPix >> 1) + x_ch + 3] = Chroma_Cb_l0[(i << 2) + 3];
					aio_tImage_Cr[ (y_ch + i) * (PicWidthInPix >> 1) + x_ch] = Chroma_Cr_l0[(i << 2)];
					aio_tImage_Cr[ (y_ch + i) * (PicWidthInPix >> 1) + x_ch + 1] = Chroma_Cr_l0[(i << 2) + 1];
					aio_tImage_Cr[ (y_ch + i) * (PicWidthInPix >> 1) + x_ch + 2] = Chroma_Cr_l0[(i << 2) + 2];
					aio_tImage_Cr[ (y_ch + i) * (PicWidthInPix >> 1) + x_ch + 3] = Chroma_Cr_l0[(i << 2) + 3];
				}
			}else{
				for ( i = 0; i < 8; i++){
					aio_tImage[ (locy + i) * PicWidthInPix + locx ] =	( Luma_l0[(i << 3)]	    + Luma_l1[(i << 3)]     + 1) >> 1;
					aio_tImage[ (locy + i) * PicWidthInPix + locx + 1] = ( Luma_l0[(i << 3) + 1] + Luma_l1[(i << 3) + 1] + 1) >> 1;
					aio_tImage[ (locy + i) * PicWidthInPix + locx + 2] = ( Luma_l0[(i << 3) + 2] + Luma_l1[(i << 3) + 2] + 1) >> 1;
					aio_tImage[ (locy + i) * PicWidthInPix + locx + 3] = ( Luma_l0[(i << 3) + 3] + Luma_l1[(i << 3) + 3] + 1) >> 1;
					aio_tImage[ (locy + i) * PicWidthInPix + locx + 4] = ( Luma_l0[(i << 3) + 4] + Luma_l1[(i << 3) + 4] + 1) >> 1;
					aio_tImage[ (locy + i) * PicWidthInPix + locx + 5] = ( Luma_l0[(i << 3) + 5] + Luma_l1[(i << 3) + 5] + 1) >> 1;
					aio_tImage[ (locy + i) * PicWidthInPix + locx + 6] = ( Luma_l0[(i << 3) + 6] + Luma_l1[(i << 3) + 6] + 1) >> 1;
					aio_tImage[ (locy + i) * PicWidthInPix + locx + 7] = ( Luma_l0[(i << 3) + 7] + Luma_l1[(i << 3) + 7] + 1) >> 1;

				} 
				for ( i = 0; i < 4; i++){
					aio_tImage_Cb[ (y_ch + i) * (PicWidthInPix >> 1) + x_ch] =	 ( Chroma_Cb_l1[(i << 2)]	  + Chroma_Cb_l0[(i << 2)]	   +1) >> 1;
					aio_tImage_Cb[ (y_ch + i) * (PicWidthInPix >> 1) + x_ch + 1] = ( Chroma_Cb_l1[(i << 2) + 1] + Chroma_Cb_l0[(i << 2) + 1] +1) >> 1;
					aio_tImage_Cb[ (y_ch + i) * (PicWidthInPix >> 1) + x_ch + 2] = ( Chroma_Cb_l1[(i << 2) + 2] + Chroma_Cb_l0[(i << 2) + 2] +1) >> 1;
					aio_tImage_Cb[ (y_ch + i) * (PicWidthInPix >> 1) + x_ch + 3] = ( Chroma_Cb_l1[(i << 2) + 3] + Chroma_Cb_l0[(i << 2) + 3] +1) >> 1;
					aio_tImage_Cr[ (y_ch + i) * (PicWidthInPix >> 1) + x_ch] =	 ( Chroma_Cr_l1[(i << 2)]	  + Chroma_Cr_l0[(i << 2)]	   +1) >> 1;
					aio_tImage_Cr[ (y_ch + i) * (PicWidthInPix >> 1) + x_ch + 1] = ( Chroma_Cr_l1[(i << 2) + 1] + Chroma_Cr_l0[(i << 2) + 1] +1) >> 1;
					aio_tImage_Cr[ (y_ch + i) * (PicWidthInPix >> 1) + x_ch + 2] = ( Chroma_Cr_l1[(i << 2) + 2] + Chroma_Cr_l0[(i << 2) + 2] +1) >> 1;
					aio_tImage_Cr[ (y_ch + i) * (PicWidthInPix >> 1) + x_ch + 3] = ( Chroma_Cr_l1[(i << 2) + 3] + Chroma_Cr_l0[(i << 2) + 3] +1) >> 1;
				}
			}
		}else{

			for ( i = 0; i < 8; i++){
				aio_tImage[ (locy + i) * PicWidthInPix + locx] =  Luma_l1[(i << 3)];
				aio_tImage[ (locy + i) * PicWidthInPix + locx + 1] =  Luma_l1[(i << 3) + 1];
				aio_tImage[ (locy + i) * PicWidthInPix + locx + 2] =  Luma_l1[(i << 3) + 2];
				aio_tImage[ (locy + i) * PicWidthInPix + locx + 3] =  Luma_l1[(i << 3) + 3];
				aio_tImage[ (locy + i) * PicWidthInPix + locx + 4] =  Luma_l1[(i << 3) + 4];
				aio_tImage[ (locy + i) * PicWidthInPix + locx + 5] =  Luma_l1[(i << 3) + 5];
				aio_tImage[ (locy + i) * PicWidthInPix + locx + 6] =  Luma_l1[(i << 3) + 6];
				aio_tImage[ (locy + i) * PicWidthInPix + locx + 7] =  Luma_l1[(i << 3) + 7];
			}

			for ( i = 0; i < 4; i++){
				aio_tImage_Cb[ (y_ch + i) * (PicWidthInPix >> 1) + x_ch]	  = Chroma_Cb_l1[(i << 2)];
				aio_tImage_Cb[ (y_ch + i) * (PicWidthInPix >> 1) + x_ch + 1] = Chroma_Cb_l1[(i << 2) + 1];
				aio_tImage_Cb[ (y_ch + i) * (PicWidthInPix >> 1) + x_ch + 2] = Chroma_Cb_l1[(i << 2) + 2];
				aio_tImage_Cb[ (y_ch + i) * (PicWidthInPix >> 1) + x_ch + 3] = Chroma_Cb_l1[(i << 2) + 3];
				aio_tImage_Cr[ (y_ch + i) * (PicWidthInPix >> 1) + x_ch]     = Chroma_Cr_l1[(i << 2)];
				aio_tImage_Cr[ (y_ch + i) * (PicWidthInPix >> 1) + x_ch + 1] = Chroma_Cr_l1[(i << 2) + 1];
				aio_tImage_Cr[ (y_ch + i) * (PicWidthInPix >> 1) + x_ch + 2] = Chroma_Cr_l1[(i << 2) + 2];
				aio_tImage_Cr[ (y_ch + i) * (PicWidthInPix >> 1) + x_ch + 3] = Chroma_Cr_l1[(i << 2) + 3];
			}
		}
		Luma_l1 += 1 << 6;
		Luma_l0 += 1 << 6;
		Chroma_Cb_l0 += 1 << 4;
		Chroma_Cb_l1 += 1 << 4;
		Chroma_Cr_l0 += 1 << 4;
		Chroma_Cr_l1 += 1 << 4;
	}
}






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

void set_image_8x8_GDEM(unsigned char aio_tImage[RESTRICT] , unsigned char aio_tImage_Cb[RESTRICT], unsigned char aio_tImage_Cr[RESTRICT], 
					   unsigned char  Luma_l0[RESTRICT], unsigned char  Luma_l1[RESTRICT], unsigned char  Chroma_Cb_l0[RESTRICT], 
					   unsigned char Chroma_Cb_l1[RESTRICT], unsigned char  Chroma_Cr_l0[RESTRICT], unsigned char Chroma_Cr_l1[RESTRICT], 
					   short *ref_cache_l0, short *ref_cache_l1, short  PicWidthInPix)
{


	int h;
	int dir_inter;
	unsigned char  * dir_aio;

	char index8[16] = {0xC,0xE,0x1C,0x1E};
	char locx[16]   = {0x0,0x8,0x0,0x8};
	char locy[16]   = {0x0,0x0,0x8,0x8};
	char x_ch[16]   = {0x0,0x4,0x0,0x4};
	char y_ch[16]   = {0x0,0x0,0x4,0x4};

	short  PicWidthInPix1 = PicWidthInPix >> 1;

	short  PicWidthInPix2 = PicWidthInPix << 1;
	short  PicWidthInPix3 = PicWidthInPix + PicWidthInPix2;
	short  PicWidthInPix4 = PicWidthInPix << 2;
	short  PicWidthInPix5 = PicWidthInPix + PicWidthInPix4;
	short  PicWidthInPix6 = PicWidthInPix + PicWidthInPix5;
	short  PicWidthInPix7 = PicWidthInPix + PicWidthInPix6;

	short  dato_inter2 = PicWidthInPix1 << 1;
	short  dato_inter3 = dato_inter2 + PicWidthInPix1; 

	for( h = 0; h < 4; h++)
	{

		if ( ref_cache_l0[index8[h]] > -1 )
		{
			if( ref_cache_l1[index8[h]] < 0 )
			{

				dir_inter = (locy[h]) * PicWidthInPix + locx[h];
				_mem8(& aio_tImage[ dir_inter])  = _itoll(_packl4(_pack2(Luma_l0[7],Luma_l0[6]),_pack2(Luma_l0[5],Luma_l0[4])),_packl4(_pack2(Luma_l0[3],Luma_l0[2]),_pack2(Luma_l0[1],Luma_l0[0])));


				_mem8(& aio_tImage[ (dir_inter + PicWidthInPix)])  = _itoll(_packl4(_pack2(Luma_l0[15],Luma_l0[14]),_pack2(Luma_l0[13],Luma_l0[12])),_packl4(_pack2(Luma_l0[11],Luma_l0[10]),_pack2(Luma_l0[9],Luma_l0[8])));


				_mem8(& aio_tImage[ (dir_inter + (PicWidthInPix2))])  = _itoll(_packl4(_pack2(Luma_l0[23],Luma_l0[22]),_pack2(Luma_l0[21],Luma_l0[20])),_packl4(_pack2(Luma_l0[19],Luma_l0[18]),_pack2(Luma_l0[17],Luma_l0[16])));

				_mem8(& aio_tImage[ (dir_inter + (PicWidthInPix3))])  = _itoll(_packl4(_pack2(Luma_l0[31],Luma_l0[30]),_pack2(Luma_l0[29],Luma_l0[28])),_packl4(_pack2(Luma_l0[27],Luma_l0[26]),_pack2(Luma_l0[25],Luma_l0[24])));

				_mem8(& aio_tImage[ (dir_inter + (PicWidthInPix4))])  = _itoll(_packl4(_pack2(Luma_l0[39],Luma_l0[38]),_pack2(Luma_l0[37],Luma_l0[36])),_packl4(_pack2(Luma_l0[35],Luma_l0[34]),_pack2(Luma_l0[33],Luma_l0[32])));

				_mem8(& aio_tImage[ (dir_inter + (PicWidthInPix5))])  = _itoll(_packl4(_pack2(Luma_l0[47],Luma_l0[46]),_pack2(Luma_l0[45],Luma_l0[44])),_packl4(_pack2(Luma_l0[43],Luma_l0[42]),_pack2(Luma_l0[41],Luma_l0[40])));

				_mem8(& aio_tImage[ (dir_inter + (PicWidthInPix6))])  = _itoll(_packl4(_pack2(Luma_l0[55],Luma_l0[54]),_pack2(Luma_l0[53],Luma_l0[52])),_packl4(_pack2(Luma_l0[51],Luma_l0[50]),_pack2(Luma_l0[49],Luma_l0[48])));

				_mem8(& aio_tImage[ (dir_inter + (PicWidthInPix7))])  = _itoll(_packl4(_pack2(Luma_l0[63],Luma_l0[62]),_pack2(Luma_l0[61],Luma_l0[60])),_packl4(_pack2(Luma_l0[59],Luma_l0[58]),_pack2(Luma_l0[57],Luma_l0[56])));

				dir_inter = y_ch[h] * PicWidthInPix1 + x_ch[h];

				dir_aio=  & aio_tImage_Cb[ dir_inter];
				_mem4(dir_aio)= _packl4(_pack2(Chroma_Cb_l0[3],Chroma_Cb_l0[2]),_pack2(Chroma_Cb_l0[1],Chroma_Cb_l0[0]));
				dir_aio= & aio_tImage_Cr[ dir_inter];
				_mem4(dir_aio)= _packl4(_pack2(Chroma_Cr_l0[3],Chroma_Cr_l0[2]),_pack2(Chroma_Cr_l0[1],Chroma_Cr_l0[0]));

				//valor_inter = 4;
				dir_aio=  & aio_tImage_Cb[ dir_inter + PicWidthInPix1];
				_mem4(dir_aio)= _packl4(_pack2(Chroma_Cb_l0[7],Chroma_Cb_l0[6]),_pack2(Chroma_Cb_l0[5],Chroma_Cb_l0[4]));
				dir_aio= & aio_tImage_Cr[ dir_inter + PicWidthInPix1];
				_mem4(dir_aio)= _packl4(_pack2(Chroma_Cr_l0[7],Chroma_Cr_l0[6]),_pack2(Chroma_Cr_l0[5],Chroma_Cr_l0[4]));

				//valor_inter = 8;
				dir_aio=  & aio_tImage_Cb[ dir_inter + (dato_inter2)];
				_mem4(dir_aio)= _packl4(_pack2(Chroma_Cb_l0[11],Chroma_Cb_l0[10]),_pack2(Chroma_Cb_l0[9],Chroma_Cb_l0[8]));
				dir_aio= & aio_tImage_Cr[ dir_inter + (dato_inter2)];
				_mem4(dir_aio)= _packl4(_pack2(Chroma_Cr_l0[11],Chroma_Cr_l0[10]),_pack2(Chroma_Cr_l0[9],Chroma_Cr_l0[8]));

				//valor_inter = 12;
				dir_aio=  & aio_tImage_Cb[ dir_inter + (dato_inter3)];
				_mem4(dir_aio)= _packl4(_pack2(Chroma_Cb_l0[15],Chroma_Cb_l0[14]),_pack2(Chroma_Cb_l0[13],Chroma_Cb_l0[12]));
				dir_aio= & aio_tImage_Cr[ dir_inter + (dato_inter3)];
				_mem4(dir_aio)= _packl4(_pack2(Chroma_Cr_l0[15],Chroma_Cr_l0[14]),_pack2(Chroma_Cr_l0[13],Chroma_Cr_l0[12]));


			}
			else 
			{

				/**/
				dir_inter = ((locy[h]) * PicWidthInPix + locx[h]);

				_mem8(& aio_tImage[ dir_inter])  = _itoll(_packl4(_shru2(_add2(_add2(_pack2(Luma_l0[7],Luma_l0[6]),_pack2(Luma_l1[7],Luma_l1[6])),0x00010001),0x00010001),_shru2(_add2(_add2(_pack2(Luma_l0[5],Luma_l0[4]),_pack2(Luma_l1[5],Luma_l1[4])),0x00010001),0x00010001)),_packl4(_shru2(_add2(_add2(_pack2(Luma_l0[3],Luma_l0[2]),_pack2(Luma_l1[3],Luma_l1[2])),0x00010001),0x00010001),_shru2(_add2(_add2(_pack2(Luma_l0[1],Luma_l0[0]),_pack2(Luma_l1[1],Luma_l1[0])),0x00010001),0x00010001)));					

				_mem8(& aio_tImage[ (dir_inter + PicWidthInPix) ])  = _itoll(_packl4(_shru2(_add2(_add2(_pack2(Luma_l0[15],Luma_l0[14]),_pack2(Luma_l1[15],Luma_l1[14])),0x00010001),0x00010001),_shru2(_add2(_add2(_pack2(Luma_l0[13],Luma_l0[12]),_pack2(Luma_l1[13],Luma_l1[12])),0x00010001),0x00010001)),_packl4(_shru2(_add2(_add2(_pack2(Luma_l0[11],Luma_l0[10]),_pack2(Luma_l1[11],Luma_l1[10])),0x00010001),0x00010001),_shru2(_add2(_add2(_pack2(Luma_l0[9],Luma_l0[8]),_pack2(Luma_l1[9],Luma_l1[8])),0x00010001),0x00010001)));

				_mem8(& aio_tImage[ (dir_inter + (PicWidthInPix <<1)) ])  = _itoll(_packl4(_shru2(_add2(_add2(_pack2(Luma_l0[23],Luma_l0[22]),_pack2(Luma_l1[23],Luma_l1[22])),0x00010001),0x00010001),_shru2(_add2(_add2(_pack2(Luma_l0[21],Luma_l0[20]),_pack2(Luma_l1[21],Luma_l1[20])),0x00010001),0x00010001)),_packl4(_shru2(_add2(_add2(_pack2(Luma_l0[19],Luma_l0[18]),_pack2(Luma_l1[19],Luma_l1[18])),0x00010001),0x00010001),_shru2(_add2(_add2(_pack2(Luma_l0[17],Luma_l0[16]),_pack2(Luma_l1[17],Luma_l1[16])),0x00010001),0x00010001)));

				_mem8(& aio_tImage[ (dir_inter + (PicWidthInPix3))])  = _itoll(_packl4(_shru2(_add2(_add2(_pack2(Luma_l0[31],Luma_l0[30]),_pack2(Luma_l1[31],Luma_l1[30])),0x00010001),0x00010001),_shru2(_add2(_add2(_pack2(Luma_l0[29],Luma_l0[28]),_pack2(Luma_l1[29],Luma_l1[28])),0x00010001),0x00010001)),_packl4(_shru2(_add2(_add2(_pack2(Luma_l0[27],Luma_l0[26]),_pack2(Luma_l1[27],Luma_l1[26])),0x00010001),0x00010001),_shru2(_add2(_add2(_pack2(Luma_l0[25],Luma_l0[24]),_pack2(Luma_l1[25],Luma_l1[24])),0x00010001),0x00010001)));

				_mem8(& aio_tImage[ (dir_inter + (PicWidthInPix4 ))])  = _itoll(_packl4(_shru2(_add2(_add2(_pack2(Luma_l0[39],Luma_l0[38]),_pack2(Luma_l1[39],Luma_l1[38])),0x00010001),0x00010001),_shru2(_add2(_add2(_pack2(Luma_l0[37],Luma_l0[36]),_pack2(Luma_l1[37],Luma_l1[36])),0x00010001),0x00010001)),_packl4(_shru2(_add2(_add2(_pack2(Luma_l0[35],Luma_l0[34]),_pack2(Luma_l1[35],Luma_l1[34])),0x00010001),0x00010001),_shru2(_add2(_add2(_pack2(Luma_l0[33],Luma_l0[32]),_pack2(Luma_l1[33],Luma_l1[32])),0x00010001),0x00010001)));

				_mem8(& aio_tImage[ (dir_inter + (PicWidthInPix5))])  = _itoll(_packl4(_shru2(_add2(_add2(_pack2(Luma_l0[47],Luma_l0[46 ]),_pack2(Luma_l1[47],Luma_l1[46])),0x00010001),0x00010001),_shru2(_add2(_add2(_pack2(Luma_l0[45],Luma_l0[44]),_pack2(Luma_l1[45],Luma_l1[44])),0x00010001),0x00010001)),_packl4(_shru2(_add2(_add2(_pack2(Luma_l0[43],Luma_l0[42]),_pack2(Luma_l1[43],Luma_l1[42])),0x00010001),0x00010001),_shru2(_add2(_add2(_pack2(Luma_l0[41],Luma_l0[40]),_pack2(Luma_l1[41],Luma_l1[40])),0x00010001),0x00010001)));

				_mem8(& aio_tImage[ (dir_inter + (PicWidthInPix6))])  = _itoll(_packl4(_shru2(_add2(_add2(_pack2(Luma_l0[55],Luma_l0[54]),_pack2(Luma_l1[55],Luma_l1[54])),0x00010001),0x00010001),_shru2(_add2(_add2(_pack2(Luma_l0[53],Luma_l0[52]),_pack2(Luma_l1[53],Luma_l1[52])),0x00010001),0x00010001)),_packl4(_shru2(_add2(_add2(_pack2(Luma_l0[51],Luma_l0[50]),_pack2(Luma_l1[51],Luma_l1[50])),0x00010001),0x00010001),_shru2(_add2(_add2(_pack2(Luma_l0[49],Luma_l0[48]),_pack2(Luma_l1[49],Luma_l1[48])),0x00010001),0x00010001)));

				_mem8(& aio_tImage[ (dir_inter + (PicWidthInPix7))])  = _itoll(_packl4(_shru2(_add2(_add2(_pack2(Luma_l0[63],Luma_l0[62]),_pack2(Luma_l1[63],Luma_l1[62])),0x00010001),0x00010001),_shru2(_add2(_add2(_pack2(Luma_l0[61],Luma_l0[60]),_pack2(Luma_l1[61],Luma_l1[60])),0x00010001),0x00010001)),_packl4(_shru2(_add2(_add2(_pack2(Luma_l0[59],Luma_l0[58]),_pack2(Luma_l1[59],Luma_l1[58])),0x00010001),0x00010001),_shru2(_add2(_add2(_pack2(Luma_l0[57],Luma_l0[56]),_pack2(Luma_l1[57],Luma_l1[56])),0x00010001),0x00010001)));

				//CHROMA
				/**/

				dir_inter = (y_ch[h]) * (PicWidthInPix1) + x_ch[h];

				//valor_inter = 0;
				dir_aio= & aio_tImage_Cb[ dir_inter];
				_mem4(dir_aio)=_packl4(_shru2(_add2(_add2(_pack2(Chroma_Cb_l0[3],Chroma_Cb_l0[2]),_pack2(Chroma_Cb_l1[3],Chroma_Cb_l1[2])),0x00010001),0x00010001),_shru2(_add2(_add2(_pack2(Chroma_Cb_l0[1],Chroma_Cb_l0[0]),_pack2(Chroma_Cb_l1[1],Chroma_Cb_l1[0])),0x00010001),0x00010001));
				dir_aio= & aio_tImage_Cr[ dir_inter];
				_mem4(dir_aio)=_packl4(_shru2(_add2(_add2(_pack2(Chroma_Cr_l0[3],Chroma_Cr_l0[2]),_pack2(Chroma_Cr_l1[3],Chroma_Cr_l1[2])),0x00010001),0x00010001),_shru2(_add2(_add2(_pack2(Chroma_Cr_l0[1],Chroma_Cr_l0[0]),_pack2(Chroma_Cr_l1[1],Chroma_Cr_l1[0])),0x00010001),0x00010001));


				//valor_inter = 4;
				dir_aio= & aio_tImage_Cb[ dir_inter + PicWidthInPix1];
				_mem4(dir_aio)=_packl4(_shru2(_add2(_add2(_pack2(Chroma_Cb_l0[7],Chroma_Cb_l0[6]),_pack2(Chroma_Cb_l1[7],Chroma_Cb_l1[6])),0x00010001),0x00010001),_shru2(_add2(_add2(_pack2(Chroma_Cb_l0[5],Chroma_Cb_l0[4]),_pack2(Chroma_Cb_l1[5],Chroma_Cb_l1[4])),0x00010001),0x00010001));
				dir_aio= & aio_tImage_Cr[ dir_inter + PicWidthInPix1];
				_mem4(dir_aio)=_packl4(_shru2(_add2(_add2(_pack2(Chroma_Cr_l0[7],Chroma_Cr_l0[6]),_pack2(Chroma_Cr_l1[7],Chroma_Cr_l1[6])),0x00010001),0x00010001),_shru2(_add2(_add2(_pack2(Chroma_Cr_l0[5],Chroma_Cr_l0[4]),_pack2(Chroma_Cr_l1[5],Chroma_Cr_l1[4])),0x00010001),0x00010001));

				//valor_inter = 8;
				dir_aio= & aio_tImage_Cb[ dir_inter + (dato_inter2)];
				_mem4(dir_aio)=_packl4(_shru2(_add2(_add2(_pack2(Chroma_Cb_l0[11],Chroma_Cb_l0[10]),_pack2(Chroma_Cb_l1[11],Chroma_Cb_l1[10])),0x00010001),0x00010001),_shru2(_add2(_add2(_pack2(Chroma_Cb_l0[9],Chroma_Cb_l0[8]),_pack2(Chroma_Cb_l1[9],Chroma_Cb_l1[8])),0x00010001),0x00010001));
				dir_aio= & aio_tImage_Cr[ dir_inter + (dato_inter2)];
				_mem4(dir_aio)=_packl4(_shru2(_add2(_add2(_pack2(Chroma_Cr_l0[11],Chroma_Cr_l0[10]),_pack2(Chroma_Cr_l1[11],Chroma_Cr_l1[10])),0x00010001),0x00010001),_shru2(_add2(_add2(_pack2(Chroma_Cr_l0[9],Chroma_Cr_l0[8]),_pack2(Chroma_Cr_l1[9],Chroma_Cr_l1[8])),0x00010001),0x00010001));

				//valor_inter = 12;
				dir_aio= & aio_tImage_Cb[ dir_inter + (dato_inter3)];
				_mem4(dir_aio)=_packl4(_shru2(_add2(_add2(_pack2(Chroma_Cb_l0[15],Chroma_Cb_l0[14]),_pack2(Chroma_Cb_l1[15],Chroma_Cb_l1[14])),0x00010001),0x00010001),_shru2(_add2(_add2(_pack2(Chroma_Cb_l0[13],Chroma_Cb_l0[12]),_pack2(Chroma_Cb_l1[13],Chroma_Cb_l1[12])),0x00010001),0x00010001));
				dir_aio= & aio_tImage_Cr[ dir_inter + (dato_inter3)];
				_mem4(dir_aio)=_packl4(_shru2(_add2(_add2(_pack2(Chroma_Cr_l0[15],Chroma_Cr_l0[14]),_pack2(Chroma_Cr_l1[15],Chroma_Cr_l1[14])),0x00010001),0x00010001),_shru2(_add2(_add2(_pack2(Chroma_Cr_l0[13],Chroma_Cr_l0[12]),_pack2(Chroma_Cr_l1[13],Chroma_Cr_l1[12])),0x00010001),0x00010001));

			}
		}
		else
		{			
			dir_inter = (locy[h]) * PicWidthInPix + locx[h];
			_mem8(& aio_tImage[ dir_inter])=_itoll(_packl4(_pack2(Luma_l1[7],Luma_l1[6]),_pack2(Luma_l1[5],Luma_l1[4])),_packl4(_pack2(Luma_l1[3],Luma_l1[2]),_pack2(Luma_l1[1],Luma_l1[0])));

			_mem8(& aio_tImage[ (dir_inter + PicWidthInPix)])=_itoll(_packl4(_pack2(Luma_l1[15],Luma_l1[14]),_pack2(Luma_l1[13],Luma_l1[12])), _packl4(_pack2(Luma_l1[11],Luma_l1[10]),_pack2(Luma_l1[9],Luma_l1[8])));
			_mem8(& aio_tImage[ (dir_inter + (PicWidthInPix2))])=_itoll(_packl4(_pack2(Luma_l1[23],Luma_l1[22]),_pack2(Luma_l1[21],Luma_l1[20])),_packl4(_pack2(Luma_l1[19],Luma_l1[18]),_pack2(Luma_l1[17],Luma_l1[16])));
			_mem8(& aio_tImage[ (dir_inter + (PicWidthInPix3))])=_itoll(_packl4(_pack2(Luma_l1[31],Luma_l1[30]),_pack2(Luma_l1[29],Luma_l1[28])),_packl4(_pack2(Luma_l1[27],Luma_l1[26]),_pack2(Luma_l1[25],Luma_l1[24])));
			_mem8(& aio_tImage[ (dir_inter + (PicWidthInPix4))])=_itoll(_packl4(_pack2(Luma_l1[39],Luma_l1[38]),_pack2(Luma_l1[37],Luma_l1[36])),_packl4(_pack2(Luma_l1[35],Luma_l1[34]),_pack2(Luma_l1[33],Luma_l1[32])));
			_mem8(& aio_tImage[ (dir_inter + (PicWidthInPix5))])=_itoll(_packl4(_pack2(Luma_l1[47],Luma_l1[46]),_pack2(Luma_l1[45],Luma_l1[44])),_packl4(_pack2(Luma_l1[43],Luma_l1[42]),_pack2(Luma_l1[41],Luma_l1[40])));
			_mem8(& aio_tImage[ (dir_inter + (PicWidthInPix6))])=_itoll(_packl4(_pack2(Luma_l1[55],Luma_l1[54]),_pack2(Luma_l1[53],Luma_l1[52])),_packl4(_pack2(Luma_l1[51],Luma_l1[50]),_pack2(Luma_l1[49],Luma_l1[48])));
			_mem8(& aio_tImage[ (dir_inter + (PicWidthInPix7))])=_itoll(_packl4(_pack2(Luma_l1[63],Luma_l1[62]),_pack2(Luma_l1[61],Luma_l1[60])),_packl4(_pack2(Luma_l1[59],Luma_l1[58]),_pack2(Luma_l1[57],Luma_l1[56])));

			//CHROMA

			dir_inter = y_ch[h] * PicWidthInPix1 + x_ch[h];
			//valor_inter = 0;
			dir_aio=  & aio_tImage_Cb[ dir_inter];
			_mem4(dir_aio)= _packl4(_pack2(Chroma_Cb_l1[3],Chroma_Cb_l1[2]),_pack2(Chroma_Cb_l1[1],Chroma_Cb_l1[0]));
			dir_aio= & aio_tImage_Cr[ dir_inter];
			_mem4(dir_aio)= _packl4(_pack2(Chroma_Cr_l1[3],Chroma_Cr_l1[2]),_pack2(Chroma_Cr_l1[1],Chroma_Cr_l1[0]));

			//valor_inter = 4;
			dir_aio=  & aio_tImage_Cb[ dir_inter + PicWidthInPix1];
			_mem4(dir_aio)= _packl4(_pack2(Chroma_Cb_l1[7],Chroma_Cb_l1[6]),_pack2(Chroma_Cb_l1[5],Chroma_Cb_l1[4]));
			dir_aio= & aio_tImage_Cr[ dir_inter + PicWidthInPix1];
			_mem4(dir_aio)= _packl4(_pack2(Chroma_Cr_l1[7],Chroma_Cr_l1[6]),_pack2(Chroma_Cr_l1[5],Chroma_Cr_l1[4]));

			//valor_inter = 8;
			dir_aio=  & aio_tImage_Cb[ dir_inter + (dato_inter2)];
			_mem4(dir_aio)= _packl4(_pack2(Chroma_Cb_l1[11],Chroma_Cb_l1[10]),_pack2(Chroma_Cb_l1[9],Chroma_Cb_l1[8]));
			dir_aio= & aio_tImage_Cr[ dir_inter + (dato_inter2)];
			_mem4(dir_aio)= _packl4(_pack2(Chroma_Cr_l1[11],Chroma_Cr_l1[10]),_pack2(Chroma_Cr_l1[9],Chroma_Cr_l1[8]));

			//valor_inter = 12;
			dir_aio=  & aio_tImage_Cb[ dir_inter + (dato_inter3)];
			_mem4(dir_aio)= _packl4(_pack2(Chroma_Cb_l1[15],Chroma_Cb_l1[14]),_pack2(Chroma_Cb_l1[13],Chroma_Cb_l1[12]));
			dir_aio= & aio_tImage_Cr[ dir_inter + (dato_inter3)];
			_mem4(dir_aio)= _packl4(_pack2(Chroma_Cr_l1[15],Chroma_Cr_l1[14]),_pack2(Chroma_Cr_l1[13],Chroma_Cr_l1[12]));

		}
		Luma_l1 += 1 << 6;
		Luma_l0 += 1 << 6;
		Chroma_Cb_l0 += 1 << 4;
		Chroma_Cb_l1 += 1 << 4;
		Chroma_Cr_l0 += 1 << 4;
		Chroma_Cr_l1 += 1 << 4;
	}


}

#endif



#ifdef MMXi
//mbl test with Pocket_PC			
void set_image_8x8_MMX(unsigned char *aio_tImage , unsigned char aio_tImage_Cb[], unsigned char aio_tImage_Cr[], 
					   unsigned char  *Luma_l0, unsigned char  *Luma_l1, unsigned char  *Chroma_Cb_l0, 
					   unsigned char *Chroma_Cb_l1, unsigned char  *Chroma_Cr_l0, unsigned char *Chroma_Cr_l1, 
					   short *ref_cache_l0, short *ref_cache_l1, short  PicWidthInPix)
{

	int h;

	for( h = 0; h < 4; h++){
		int index8 = SCAN8(h << 2);
		int locx =((h & 1) << 3);
		int locy =((h & 2) << 2);
		int x_ch = (locx) >> 1;
		int y_ch = (locy) >> 1;


		if ( ref_cache_l0[index8] > -1 ){
			if( ref_cache_l1[index8] < 0 ){
				unsigned short *ptr_Chroma_Cb_l0 = (unsigned short *) Chroma_Cb_l0;
				unsigned short *ptr_Chroma_Cr_l0 = (unsigned short *) Chroma_Cr_l0;
				unsigned short *ptr_aio_tImage_Cb = (unsigned short *) (&aio_tImage_Cb[y_ch * (PicWidthInPix >> 1) + x_ch]);
				unsigned short *ptr_aio_tImage_Cr = (unsigned short *) (&aio_tImage_Cr[y_ch * (PicWidthInPix >> 1) + x_ch]);

				unsigned int *ptr_Luma_l0 = (unsigned int *) Luma_l0;
				unsigned int *ptr_aio_tImage = (unsigned int *) (&aio_tImage[(locy) * PicWidthInPix + locx]);

				ptr_aio_tImage[0] = *ptr_Luma_l0++;
				ptr_aio_tImage[1] = *ptr_Luma_l0++;
				ptr_aio_tImage[PicWidthInPix >> 2] = *ptr_Luma_l0++;
				ptr_aio_tImage[1 + (PicWidthInPix >> 2)] = *ptr_Luma_l0++;
				ptr_aio_tImage[PicWidthInPix >> 1] = *ptr_Luma_l0++;
				ptr_aio_tImage[1 + (PicWidthInPix >> 1)] = *ptr_Luma_l0++;
				ptr_aio_tImage[(3 * PicWidthInPix) >> 2] = *ptr_Luma_l0++;
				ptr_aio_tImage[1 + ((3 * PicWidthInPix) >> 2)] = *ptr_Luma_l0++;

				ptr_aio_tImage[PicWidthInPix] = *ptr_Luma_l0++;
				ptr_aio_tImage[1 + PicWidthInPix] = *ptr_Luma_l0++;
				ptr_aio_tImage[(5 * PicWidthInPix) >> 2] = *ptr_Luma_l0++;
				ptr_aio_tImage[1 + ((5 * PicWidthInPix) >> 2)] = *ptr_Luma_l0++;
				ptr_aio_tImage[(3 * PicWidthInPix) >> 1] = *ptr_Luma_l0++;
				ptr_aio_tImage[1 + ((3 * PicWidthInPix) >> 1)] = *ptr_Luma_l0++;
				ptr_aio_tImage[(7 * PicWidthInPix) >> 2] = *ptr_Luma_l0++;
				ptr_aio_tImage[1 + ((7 * PicWidthInPix) >> 2)] = *ptr_Luma_l0++;




				ptr_aio_tImage_Cb[0] = *ptr_Chroma_Cb_l0++;
				ptr_aio_tImage_Cb[1] = *ptr_Chroma_Cb_l0++;
				ptr_aio_tImage_Cb[PicWidthInPix >> 2] = *ptr_Chroma_Cb_l0++;
				ptr_aio_tImage_Cb[1 + (PicWidthInPix >> 2)] = *ptr_Chroma_Cb_l0++;
				ptr_aio_tImage_Cb[PicWidthInPix >> 1] = *ptr_Chroma_Cb_l0++;
				ptr_aio_tImage_Cb[1 + (PicWidthInPix >> 1)] = *ptr_Chroma_Cb_l0++;
				ptr_aio_tImage_Cb[(3 * PicWidthInPix) >> 2] = *ptr_Chroma_Cb_l0++;
				ptr_aio_tImage_Cb[1 + ((3 * PicWidthInPix) >> 2)] = *ptr_Chroma_Cb_l0;


				ptr_aio_tImage_Cr[0] = *ptr_Chroma_Cr_l0++;
				ptr_aio_tImage_Cr[1] = *ptr_Chroma_Cr_l0++;
				ptr_aio_tImage_Cr[PicWidthInPix >> 2] = *ptr_Chroma_Cr_l0++;
				ptr_aio_tImage_Cr[1 + (PicWidthInPix >> 2)] = *ptr_Chroma_Cr_l0++;
				ptr_aio_tImage_Cr[PicWidthInPix >> 1] = *ptr_Chroma_Cr_l0++;
				ptr_aio_tImage_Cr[1 + (PicWidthInPix >> 1)] = *ptr_Chroma_Cr_l0++;
				ptr_aio_tImage_Cr[(3 * PicWidthInPix) >> 2] = *ptr_Chroma_Cr_l0++;
				ptr_aio_tImage_Cr[1 + ((3 * PicWidthInPix) >> 2)] = *ptr_Chroma_Cr_l0;

			}else{
				__m128i l_l0_0 = _mm_loadu_si128((__m128i *) Luma_l0);
				__m128i l_l0_1 = _mm_loadu_si128((__m128i *) &Luma_l0[16]);
				__m128i l_l0_2 = _mm_loadu_si128((__m128i *) &Luma_l0[32]);
				__m128i l_l0_3 = _mm_loadu_si128((__m128i *) &Luma_l0[48]);

				__m128i l_l1_0 = _mm_loadu_si128((__m128i *) Luma_l1);
				__m128i l_l1_1 = _mm_loadu_si128((__m128i *) &Luma_l1[16]);
				__m128i l_l1_2 = _mm_loadu_si128((__m128i *) &Luma_l1[32]);
				__m128i l_l1_3 = _mm_loadu_si128((__m128i *) &Luma_l1[48]);

				__m128i l_res_0 = _mm_avg_epu8 (l_l0_0, l_l1_0);
				__m128i l_res_1 = _mm_avg_epu8 (l_l0_1, l_l1_1);
				__m128i l_res_2 = _mm_avg_epu8 (l_l0_2, l_l1_2);
				__m128i l_res_3 = _mm_avg_epu8 (l_l0_3, l_l1_3);

				__m128i Cb_l0 = _mm_loadu_si128((__m128i *) Chroma_Cb_l0);
				__m128i Cb_l1 = _mm_loadu_si128((__m128i *) Chroma_Cb_l1);
				__m128i Cb_res = _mm_avg_epu8 (Cb_l0, Cb_l1);
				__m128i Cr_l0 = _mm_loadu_si128((__m128i *) Chroma_Cr_l0);
				__m128i Cr_l1 = _mm_loadu_si128((__m128i *) Chroma_Cr_l1);
				__m128i Cr_res = _mm_avg_epu8 (Cr_l0, Cr_l1);

				unsigned int *ptr_aio_tImag = (unsigned int *) (&aio_tImage[(locy) * PicWidthInPix + locx]);
				unsigned int *ptr_aio_tImage_Cb = (unsigned int *) (&aio_tImage_Cb[y_ch * (PicWidthInPix >> 1) + x_ch]);
				unsigned int *ptr_aio_tImage_Cr = (unsigned int *) (&aio_tImage_Cr[y_ch * (PicWidthInPix >> 1) + x_ch]);

				ptr_aio_tImag[0] = _mm_cvtsi128_si32(l_res_0); 
				ptr_aio_tImag[1] = _mm_cvtsi128_si32(_mm_shuffle_epi32(l_res_0,_MM_SHUFFLE(0,3,2,1))); 
				ptr_aio_tImag[PicWidthInPix >> 2] = _mm_cvtsi128_si32(_mm_shuffle_epi32(l_res_0,_MM_SHUFFLE(1,0,3,2)));
				ptr_aio_tImag[1 + (PicWidthInPix >> 2)] = _mm_cvtsi128_si32(_mm_shuffle_epi32(l_res_0,_MM_SHUFFLE(2,1,0,3)));

				ptr_aio_tImag[PicWidthInPix >> 1] = _mm_cvtsi128_si32(l_res_1); 
				ptr_aio_tImag[1 + (PicWidthInPix >> 1)] = _mm_cvtsi128_si32(_mm_shuffle_epi32(l_res_1,_MM_SHUFFLE(0,3,2,1))); 
				ptr_aio_tImag[(3 * PicWidthInPix) >> 2] = _mm_cvtsi128_si32(_mm_shuffle_epi32(l_res_1,_MM_SHUFFLE(1,0,3,2)));
				ptr_aio_tImag[1 + ((3 * PicWidthInPix) >> 2)] = _mm_cvtsi128_si32(_mm_shuffle_epi32(l_res_1,_MM_SHUFFLE(2,1,0,3)));

				ptr_aio_tImag[PicWidthInPix] = _mm_cvtsi128_si32(l_res_2); 
				ptr_aio_tImag[1 + PicWidthInPix] = _mm_cvtsi128_si32(_mm_shuffle_epi32(l_res_2,_MM_SHUFFLE(0,3,2,1)));  
				ptr_aio_tImag[(5 * PicWidthInPix) >> 2] = _mm_cvtsi128_si32(_mm_shuffle_epi32(l_res_2,_MM_SHUFFLE(1,0,3,2)));
				ptr_aio_tImag[1 + ((5 * PicWidthInPix) >> 2)] = _mm_cvtsi128_si32(_mm_shuffle_epi32(l_res_2,_MM_SHUFFLE(2,1,0,3)));

				ptr_aio_tImag[(3 * PicWidthInPix) >> 1] = _mm_cvtsi128_si32(l_res_3); 
				ptr_aio_tImag[1 + ((3 * PicWidthInPix) >> 1)] = _mm_cvtsi128_si32(_mm_shuffle_epi32(l_res_3,_MM_SHUFFLE(0,3,2,1)));  
				ptr_aio_tImag[(7 * PicWidthInPix) >> 2] = _mm_cvtsi128_si32(_mm_shuffle_epi32(l_res_3,_MM_SHUFFLE(1,0,3,2)));
				ptr_aio_tImag[1 + ((7 * PicWidthInPix) >> 2)] = _mm_cvtsi128_si32(_mm_shuffle_epi32(l_res_3,_MM_SHUFFLE(2,1,0,3)));


				ptr_aio_tImage_Cb[0] = _mm_cvtsi128_si32(Cb_res);
				ptr_aio_tImage_Cb[PicWidthInPix >> 3] = _mm_cvtsi128_si32(_mm_shuffle_epi32(Cb_res,_MM_SHUFFLE(0,3,2,1))); 
				ptr_aio_tImage_Cb[PicWidthInPix >> 2] = _mm_cvtsi128_si32(_mm_shuffle_epi32(Cb_res,_MM_SHUFFLE(1,0,3,2))); 
				ptr_aio_tImage_Cb[(3 * PicWidthInPix) >> 3] = _mm_cvtsi128_si32(_mm_shuffle_epi32(Cb_res,_MM_SHUFFLE(2,1,0,3))); 

				ptr_aio_tImage_Cr[0] = _mm_cvtsi128_si32(Cr_res);
				ptr_aio_tImage_Cr[PicWidthInPix >> 3] = _mm_cvtsi128_si32(_mm_shuffle_epi32(Cr_res,_MM_SHUFFLE(0,3,2,1))); 
				ptr_aio_tImage_Cr[PicWidthInPix >> 2] = _mm_cvtsi128_si32(_mm_shuffle_epi32(Cr_res,_MM_SHUFFLE(1,0,3,2))); 
				ptr_aio_tImage_Cr[(3 * PicWidthInPix) >> 3] = _mm_cvtsi128_si32(_mm_shuffle_epi32(Cr_res,_MM_SHUFFLE(2,1,0,3))); 

				empty();    
			}

		}else{

			unsigned short *ptr_Chroma_Cb_l1 = (unsigned short *) Chroma_Cb_l1;
			unsigned short *ptr_Chroma_Cr_l1 = (unsigned short *) Chroma_Cr_l1;
			unsigned short *ptr_aio_tImage_Cb = (unsigned short *) (&aio_tImage_Cb[y_ch * (PicWidthInPix >> 1) + x_ch]);
			unsigned short *ptr_aio_tImage_Cr = (unsigned short *) (&aio_tImage_Cr[y_ch * (PicWidthInPix >> 1) + x_ch]);

			unsigned int *ptr_Luma_l1 = (unsigned int *) Luma_l1;
			unsigned int *ptr_aio_tImage = (unsigned int *) (&aio_tImage[(locy) * PicWidthInPix + locx]);

			ptr_aio_tImage[0] = *ptr_Luma_l1++;
			ptr_aio_tImage[1] = *ptr_Luma_l1++;
			ptr_aio_tImage[PicWidthInPix >> 2] = *ptr_Luma_l1++;
			ptr_aio_tImage[1 + (PicWidthInPix >> 2)] = *ptr_Luma_l1++;
			ptr_aio_tImage[PicWidthInPix >> 1] = *ptr_Luma_l1++;
			ptr_aio_tImage[1 + (PicWidthInPix >> 1)] = *ptr_Luma_l1++;
			ptr_aio_tImage[(3 * PicWidthInPix) >> 2] = *ptr_Luma_l1++;
			ptr_aio_tImage[1 + ((3 * PicWidthInPix) >> 2)] = *ptr_Luma_l1++;

			ptr_aio_tImage[PicWidthInPix] = *ptr_Luma_l1++;
			ptr_aio_tImage[1 + PicWidthInPix] = *ptr_Luma_l1++;
			ptr_aio_tImage[(5 * PicWidthInPix) >> 2] = *ptr_Luma_l1++;
			ptr_aio_tImage[1 + ((5 * PicWidthInPix) >> 2)] = *ptr_Luma_l1++;
			ptr_aio_tImage[(3 * PicWidthInPix) >> 1] = *ptr_Luma_l1++;
			ptr_aio_tImage[1 + ((3 * PicWidthInPix) >> 1)] = *ptr_Luma_l1++;
			ptr_aio_tImage[(7 * PicWidthInPix) >> 2] = *ptr_Luma_l1++;
			ptr_aio_tImage[1 + ((7 * PicWidthInPix) >> 2)] = *ptr_Luma_l1++;




			ptr_aio_tImage_Cb[0] = *ptr_Chroma_Cb_l1++;
			ptr_aio_tImage_Cb[1] = *ptr_Chroma_Cb_l1++;
			ptr_aio_tImage_Cb[PicWidthInPix >> 2] = *ptr_Chroma_Cb_l1++;
			ptr_aio_tImage_Cb[1 + (PicWidthInPix >> 2)] = *ptr_Chroma_Cb_l1++;
			ptr_aio_tImage_Cb[PicWidthInPix >> 1] = *ptr_Chroma_Cb_l1++;
			ptr_aio_tImage_Cb[1 + (PicWidthInPix >> 1)] = *ptr_Chroma_Cb_l1++;
			ptr_aio_tImage_Cb[(3 * PicWidthInPix) >> 2] = *ptr_Chroma_Cb_l1++;
			ptr_aio_tImage_Cb[1 + ((3 * PicWidthInPix) >> 2)] = *ptr_Chroma_Cb_l1;


			ptr_aio_tImage_Cr[0] = *ptr_Chroma_Cr_l1++;
			ptr_aio_tImage_Cr[1] = *ptr_Chroma_Cr_l1++;
			ptr_aio_tImage_Cr[PicWidthInPix >> 2] = *ptr_Chroma_Cr_l1++;
			ptr_aio_tImage_Cr[1 + (PicWidthInPix >> 2)] = *ptr_Chroma_Cr_l1++;
			ptr_aio_tImage_Cr[PicWidthInPix >> 1] = *ptr_Chroma_Cr_l1++;
			ptr_aio_tImage_Cr[1 + (PicWidthInPix >> 1)] = *ptr_Chroma_Cr_l1++;
			ptr_aio_tImage_Cr[(3 * PicWidthInPix) >> 2] = *ptr_Chroma_Cr_l1++;
			ptr_aio_tImage_Cr[1 + ((3 * PicWidthInPix) >> 2)] = *ptr_Chroma_Cr_l1;


		}
		Luma_l1 += 1 << 6;
		Luma_l0 += 1 << 6;
		Chroma_Cb_l0 += 1 << 4;
		Chroma_Cb_l1 += 1 << 4;
		Chroma_Cr_l0 += 1 << 4;
		Chroma_Cr_l1 += 1 << 4;
	}
}

#endif
