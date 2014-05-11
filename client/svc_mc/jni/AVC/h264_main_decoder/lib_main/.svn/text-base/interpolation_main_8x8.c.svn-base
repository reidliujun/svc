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

#include "clip.h"
#include "data.h"
#include "interpolation_8x8.h"
#include "interpolation_main_8x8.h"





/**
This function allows to get the chrominance prediction of an non IDR picture. 
//8.4.2.2


@param image_Cb Table of current frame.
@param image_Cr Table of current frame.
@param refPicLXCb Table of the reference decoded picture buffer.
@param refPicLXCr Table of the reference decoded picture buffer.
@param xFracl Horizontal fractionnal part.
@param yFracl Vertical fractionnal part.
@param PicWidthSamples Stride of the reference buffer.
*/
#ifdef TI_OPTIM
void chroma_SampleInterpolationMain8x8(unsigned char *image_Cb, unsigned char *image_Cr, unsigned char *refPicLXCb, 
									   unsigned char *refPicLXCr , short xFracl, short yFracl, short PicWidthSamples)
{


	short cst1 = xFracl * yFracl;
	int cst1cst4 = _pack2((cst1),((yFracl << 3) - cst1));
	int cst3cst2 = _pack2(((xFracl << 3) - cst1),(cst1 - ((xFracl + yFracl) << 3) + 64));	

	//short j,i;
	unsigned char *ptr_img_Cb = refPicLXCb;
	unsigned char *ptr_img_Cr = refPicLXCr;



	int P1P2P3P4= _mem4(ptr_img_Cb);

	int P2P3P4B3= _mem4(ptr_img_Cb + 1);
	int P1P2= _unpklu4(P1P2P3P4);
	int P3P4= _unpkhu4(P1P2P3P4);
	int P2P3 = _unpklu4(P2P3P4B3);
	int P4B3 = _unpkhu4(P2P3P4B3);


	int Q1Q2Q3Q4= _mem4(ptr_img_Cb + PicWidthSamples);
	int Q2Q3Q4D3= _mem4(ptr_img_Cb + PicWidthSamples+1);
	int Q1Q2 = _unpklu4(Q1Q2Q3Q4);
	int Q3Q4 = _unpkhu4(Q1Q2Q3Q4);
	int Q2Q3 = _unpklu4(Q2Q3Q4D3);
	int Q4D3 = _unpkhu4(Q2Q3Q4D3);

	//CR
	int R1R2R3R4= _mem4(ptr_img_Cr);
	int R2R3R4F3= _mem4(ptr_img_Cr+1);
	int R1R2= _unpklu4(R1R2R3R4);
	int R3R4= _unpkhu4(R1R2R3R4);
	int R2R3= _unpklu4(R2R3R4F3);
	int R4F3= _unpkhu4(R2R3R4F3);

	int S1S2S3S4= _mem4(ptr_img_Cr + PicWidthSamples);
	int S2S3S4H3= _mem4(ptr_img_Cr + PicWidthSamples+1);	
	int S1S2 = _unpklu4(S1S2S3S4);
	int S3S4 = _unpkhu4(S1S2S3S4);
	int S2S3 = _unpklu4(S2S3S4H3);
	int S4H3 = _unpkhu4(S2S3S4H3);



	_mem4(image_Cb)=_packl4(_shru2(_add2(_pack2((_dotp2(P4B3,cst3cst2)+_dotp2(Q4D3,cst1cst4)),(_dotp2(P3P4,cst3cst2)+_dotp2(Q3Q4,cst1cst4))),0x00200020),0x00060006),_shru2(_add2(_pack2((_dotp2(P2P3,cst3cst2)+_dotp2(Q2Q3,cst1cst4)),(_dotp2(P1P2,cst3cst2)+_dotp2(Q1Q2,cst1cst4))),0x00200020),0x00060006));


	_mem4(image_Cr)=_packl4(_shru2(_add2(_pack2((_dotp2(R4F3,cst3cst2)+_dotp2(S4H3,cst1cst4)),(_dotp2(R3R4,cst3cst2)+_dotp2(S3S4,cst1cst4))),0x00200020),0x00060006),_shru2(_add2(_pack2((_dotp2(R2R3,cst3cst2)+_dotp2(S2S3,cst1cst4)),(_dotp2(R1R2,cst3cst2)+_dotp2(S1S2,cst1cst4))),0x00200020),0x00060006));

	//cr

	//Iteracion 2
	ptr_img_Cb += (PicWidthSamples<<1); 
	P1P2P3P4 = _mem4(ptr_img_Cb);
	P2P3P4B3= _mem4(ptr_img_Cb + 1);
	P1P2= _unpklu4(P1P2P3P4);
	P3P4= _unpkhu4(P1P2P3P4);
	P2P3 = _unpklu4(P2P3P4B3);
	P4B3 = _unpkhu4(P2P3P4B3);


	_mem4(image_Cb+4)=_packl4(_shru2(_add2(_pack2((_dotp2(Q4D3,cst3cst2)+_dotp2(P4B3,cst1cst4)),(_dotp2(Q3Q4,cst3cst2)+_dotp2(P3P4,cst1cst4))),0x00200020),0x00060006),_shru2(_add2(_pack2((_dotp2(Q2Q3,cst3cst2)+_dotp2(P2P3,cst1cst4)),(_dotp2(Q1Q2,cst3cst2)+_dotp2(P1P2,cst1cst4))),0x00200020),0x00060006));


	//cr
	ptr_img_Cr += (PicWidthSamples<<1);



	R1R2R3R4= _mem4(ptr_img_Cr);
	R2R3R4F3= _mem4(ptr_img_Cr+1);
	R1R2= _unpklu4(R1R2R3R4);
	R3R4= _unpkhu4(R1R2R3R4);
	R2R3= _unpklu4(R2R3R4F3);
	R4F3= _unpkhu4(R2R3R4F3);

	_mem4(image_Cr+4)=_packl4(_shru2(_add2(_pack2((_dotp2(S4H3,cst3cst2)+_dotp2(R4F3,cst1cst4)),(_dotp2(S3S4,cst3cst2)+_dotp2(R3R4,cst1cst4))),0x00200020),0x00060006),_shru2(_add2(_pack2((_dotp2(S2S3,cst3cst2)+_dotp2(R2R3,cst1cst4)),(_dotp2(S1S2,cst3cst2)+_dotp2(R1R2,cst1cst4))),0x00200020),0x00060006));

	//cr
	//Iteracion 3
	ptr_img_Cb += PicWidthSamples; 


	Q1Q2Q3Q4= _mem4(ptr_img_Cb );
	Q2Q3Q4D3= _mem4(ptr_img_Cb +1);
	Q1Q2 = _unpklu4(Q1Q2Q3Q4);
	Q3Q4 = _unpkhu4(Q1Q2Q3Q4);
	Q2Q3 = _unpklu4(Q2Q3Q4D3);
	Q4D3 = _unpkhu4(Q2Q3Q4D3);

	_mem4(image_Cb+8)=_packl4(_shru2(_add2(_pack2((_dotp2(P4B3,cst3cst2)+_dotp2(Q4D3,cst1cst4)),(_dotp2(P3P4,cst3cst2)+_dotp2(Q3Q4,cst1cst4))),0x00200020),0x00060006),_shru2(_add2(_pack2((_dotp2(P2P3,cst3cst2)+_dotp2(Q2Q3,cst1cst4)),(_dotp2(P1P2,cst3cst2)+_dotp2(Q1Q2,cst1cst4))),0x00200020),0x00060006));

	//cr
	ptr_img_Cr += PicWidthSamples;



	S1S2S3S4= _mem4(ptr_img_Cr);
	S2S3S4H3= _mem4(ptr_img_Cr+1);	
	S1S2 = _unpklu4(S1S2S3S4);
	S3S4 = _unpkhu4(S1S2S3S4);
	S2S3 = _unpklu4(S2S3S4H3);
	S4H3 = _unpkhu4(S2S3S4H3);

	_mem4(image_Cr+8)=_packl4(_shru2(_add2(_pack2((_dotp2(R4F3,cst3cst2)+_dotp2(S4H3,cst1cst4)),(_dotp2(R3R4,cst3cst2)+_dotp2(S3S4,cst1cst4))),0x00200020),0x00060006),_shru2(_add2(_pack2((_dotp2(R2R3,cst3cst2)+_dotp2(S2S3,cst1cst4)),(_dotp2(R1R2,cst3cst2)+_dotp2(S1S2,cst1cst4))),0x00200020),0x00060006));

	//cr

	//Iteracion 4
	ptr_img_Cb += PicWidthSamples; 
	P1P2P3P4 = _mem4(ptr_img_Cb);
	P2P3P4B3= _mem4(ptr_img_Cb + 1);
	P1P2= _unpklu4(P1P2P3P4);
	P3P4= _unpkhu4(P1P2P3P4);
	P2P3 = _unpklu4(P2P3P4B3);
	P4B3 = _unpkhu4(P2P3P4B3);

	_mem4(image_Cb+12)=_packl4(_shru2(_add2(_pack2((_dotp2(Q4D3,cst3cst2)+_dotp2(P4B3,cst1cst4)),(_dotp2(Q3Q4,cst3cst2)+_dotp2(P3P4,cst1cst4))),0x00200020),0x00060006),_shru2(_add2(_pack2((_dotp2(Q2Q3,cst3cst2)+_dotp2(P2P3,cst1cst4)),(_dotp2(Q1Q2,cst3cst2)+_dotp2(P1P2,cst1cst4))),0x00200020),0x00060006));

	//cr
	ptr_img_Cr += PicWidthSamples;


	R1R2R3R4= _mem4(ptr_img_Cr);
	R2R3R4F3= _mem4(ptr_img_Cr+1);
	R1R2= _unpklu4(R1R2R3R4);
	R3R4= _unpkhu4(R1R2R3R4);
	R2R3= _unpklu4(R2R3R4F3);
	R4F3= _unpkhu4(R2R3R4F3);


	_mem4(image_Cr+12)=_packl4(_shru2(_add2(_pack2((_dotp2(S4H3,cst3cst2)+_dotp2(R4F3,cst1cst4)),(_dotp2(S3S4,cst3cst2)+_dotp2(R3R4,cst1cst4))),0x00200020),0x00060006),_shru2(_add2(_pack2((_dotp2(S2S3,cst3cst2)+_dotp2(R2R3,cst1cst4)),(_dotp2(S1S2,cst3cst2)+_dotp2(R1R2,cst1cst4))),0x00200020),0x00060006));


}
#else

void chroma_SampleInterpolationMain8x8(unsigned char *image_Cb, unsigned char *image_Cr, unsigned char *refPicLXCb, 
									   unsigned char *refPicLXCr , short xFracl, short yFracl, short PicWidthSamples)
{


	short cst1 = xFracl * yFracl;
	short cst2 = cst1 - ((xFracl + yFracl) << 3) + 64;
	short cst3 = (xFracl << 3) - cst1;
	short cst4 = (yFracl << 3) - cst1;
	short j,i;
	unsigned char *ptr_img_Cb = refPicLXCb;
	unsigned char *ptr_img_Cr = refPicLXCr;


	for(j = 0; j < 4; j++)	{
		for(i = 0; i < 4; i++){
			unsigned char A = *ptr_img_Cb;
			unsigned char B = *(ptr_img_Cb + 1);
			unsigned char C = *(ptr_img_Cb + PicWidthSamples);
			unsigned char D = *(ptr_img_Cb + 1 + PicWidthSamples);
			unsigned char E = *ptr_img_Cr;
			unsigned char F = *(ptr_img_Cr + 1);
			unsigned char G = *(ptr_img_Cr + PicWidthSamples);
			unsigned char H = *(ptr_img_Cr + 1 + PicWidthSamples);

			image_Cb[(j << 2) + i] = (unsigned char) ((cst2*A + cst3*B + cst4*C + cst1*D + 32) >> 6);
			image_Cr[(j << 2) + i] = (unsigned char) ((cst2*E + cst3*F + cst4*G + cst1*H + 32) >> 6);
			*ptr_img_Cb++;
			*ptr_img_Cr++;
		}
		ptr_img_Cb += PicWidthSamples - 4;
		ptr_img_Cr += PicWidthSamples - 4;

	}	
}
#endif







/**
This function allows to get the prediction of an non IDR picture. 
//8.4.2.2


@param DpbLuma Table of the reference decoded picture buffer.
@param DpbCb Table of the reference decoded picture buffer.
@param DpbCr Table of the reference decoded picture buffer.
@param image Table of current frame.
@param image_Cb Table of current frame.
@param image_Cr Table of current frame.
@param mvL0 The motion vector are stocked for each 4x4 block of each macroblock.
@param PicWidthInPix Width in pixel of the current frame.
@param interpol Function pointer of 8x8 interpoaltion.
*/
void interpolation_main_8x8(unsigned char *DpbLuma, unsigned char *DpbCb,unsigned char *DpbCr, 
							unsigned char *image , unsigned char *image_Cb, unsigned char *image_Cr, 
							short mvL0[2], short PicWidthInPix, const interpol_8x8 * interpol)
{


	//Luminance interpolation 
	short xFracl = (short) mvL0[0]&3;
	short yFracl = (short) mvL0[1]&3;

	interpol[xFracl + (yFracl << 2)](image, DpbLuma, PicWidthInPix, 8);


	//Chrominance interpolation
	xFracl = (short)  mvL0[0]&7;
	yFracl = (short)  mvL0[1]&7;
	chroma_SampleInterpolationMain8x8(image_Cb,image_Cr, DpbCb, DpbCr, xFracl, yFracl, PicWidthInPix>>1);

}






/**
This function allows to get the prediction of an non IDR picture. 
//8.4.2.2


@param DpbLuma Table of the reference decoded picture buffer.
@param DpbCb Table of the reference decoded picture buffer.
@param DpbCr Table of the reference decoded picture buffer.
@param Luma_l0 Table of current frame.
@param Chroma_Cb_l0 Table of current frame.
@param Chroma_Cr_l0 Table of current frame.
@param mvL0 The motion vector are stocked for each 4x4 block of each macroblock.
@param ref_cache_l0 A cache table where the reference is stocked for each 4x4 block of each macroblock.
@param x X-coordinate.
@param y Y-coordinate.
@param PicWidthInPix Width in pixel of the current frame.
@param PicHeightInPix Height in pixel of the current frame.
@prama RefPicListL0 List l0 of reference image.
@prama RefPicListL1 List l1 of reference image.
@param interpol Function pointer of 8x8 interpoaltion.
*/
void SampleInterpolationMain8x8(unsigned char *DpbLuma, unsigned char *DpbCb,unsigned char *DpbCr, 
								unsigned char *Luma_l0 , unsigned char *Chroma_Cb_l0, unsigned char *Chroma_Cr_l0, 
								short mvL0[][2], short *ref_cache_l0,int x, int y, short PicWidthInPix, 
								short PicHeightInPix , const LIST_MMO * RefPicListL0, const interpol_8x8 *interpol)
{



	//Recovery of  the prediction
	if(ref_cache_l0[12] >= 0){
		int address = RefPicListL0[ref_cache_l0[12]] . MemoryAddress;	
		int xIntl = CLIP3(-11, PicWidthInPix - 27, x + (mvL0[12][0] >> 2));
		int yIntl = CLIP3(-11, PicHeightInPix - 27, y + (mvL0[12][1] >> 2));

		interpolation_main_8x8(&DpbLuma[address + xIntl + yIntl * PicWidthInPix], 
			&DpbCb[(address >> 2) + (xIntl >> 1) + (yIntl >> 1) * (PicWidthInPix >> 1)], 
			&DpbCr[(address >> 2) + (xIntl >> 1) + (yIntl >> 1) * (PicWidthInPix >> 1)], 
			Luma_l0, Chroma_Cb_l0, Chroma_Cr_l0, mvL0[12], PicWidthInPix, interpol);
	}


	if(ref_cache_l0[14] >= 0){
		int address = RefPicListL0[ref_cache_l0[14]] . MemoryAddress;	
		int xIntl = CLIP3(-11,PicWidthInPix - 27, x + 8 +(mvL0[14][0] >> 2));
		int yIntl = CLIP3(-11,PicHeightInPix - 27, y + (mvL0[14][1] >> 2));

		interpolation_main_8x8(&DpbLuma[address + xIntl + yIntl * PicWidthInPix], 
			&DpbCb[(address >> 2) + (xIntl >> 1) + (yIntl >> 1) * (PicWidthInPix >> 1)], 
			&DpbCr[(address >> 2) + (xIntl >> 1) + (yIntl >> 1) * (PicWidthInPix >> 1)], 
			&Luma_l0[1 << 6], &Chroma_Cb_l0[1 << 4], &Chroma_Cr_l0[1 << 4], mvL0[14], PicWidthInPix, interpol);

	}

	if(ref_cache_l0[28] >= 0){
		int address = RefPicListL0[ref_cache_l0[28]] . MemoryAddress;	
		int xIntl = CLIP3(-11,PicWidthInPix - 27, x + (mvL0[28][0] >> 2));
		int yIntl = CLIP3(-11,PicHeightInPix - 27,y + 8 + (mvL0[28][1] >> 2));

		interpolation_main_8x8(&DpbLuma[address + xIntl + yIntl * PicWidthInPix], 
			&DpbCb[(address >> 2) + (xIntl >> 1) + (yIntl >> 1) * (PicWidthInPix >> 1)], 
			&DpbCr[(address >> 2) + (xIntl >> 1) + (yIntl >> 1) * (PicWidthInPix >> 1)], 
			&Luma_l0[1 << 7], &Chroma_Cb_l0[1 << 5], &Chroma_Cr_l0[1 << 5], mvL0[28], PicWidthInPix, interpol);
	}

	if(ref_cache_l0[30] >= 0){
		int address = RefPicListL0[ref_cache_l0[30]] . MemoryAddress;	
		int xIntl = CLIP3(-11,PicWidthInPix - 27, x + 8 + (mvL0[30][0] >> 2));
		int yIntl = CLIP3(-11,PicHeightInPix - 27,y + 8 + (mvL0[30][1] >> 2));

		interpolation_main_8x8(&DpbLuma[address + xIntl + yIntl * PicWidthInPix], 
			&DpbCb[(address >> 2) + (xIntl >> 1) + (yIntl >> 1) * (PicWidthInPix >> 1)], 
			&DpbCr[(address >> 2) + (xIntl >> 1) + (yIntl >> 1) * (PicWidthInPix >> 1)], 
			&Luma_l0[(1 << 7) + (1 << 6)], &Chroma_Cb_l0[(1 << 5) + (1 << 4)], &Chroma_Cr_l0[(1 << 5) + (1 << 4)], mvL0[30], PicWidthInPix, interpol);
	}
}
