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


#include "clip.h"
#include "data.h"
#include "symbol.h"
#include "interpolation.h"



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
@param stride Stride of the current image.
*/

#ifdef TI_OPTIM
void chroma_sample_interpolation_TI(unsigned char image_Cb [RESTRICT],unsigned char image_Cr [RESTRICT],
									unsigned char refPicLXCb[RESTRICT], unsigned char refPicLXCr[RESTRICT],
									const short xFracl, const short yFracl, const short PicWidthSamples, const short stride)
{




	unsigned char* pucCbPtrA = refPicLXCb;
	unsigned char* pucCbPtrB = refPicLXCb + 1;
	unsigned char* pucCbPtrC = refPicLXCb + PicWidthSamples;
	unsigned char* pucCbPtrD = refPicLXCb + PicWidthSamples + 1;

	unsigned char* pucCrPtrE = refPicLXCr;
	unsigned char* pucCrPtrF = refPicLXCr + 1;
	unsigned char* pucCrPtrG = refPicLXCr + PicWidthSamples;
	unsigned char* pucCrPtrH = refPicLXCr + PicWidthSamples + 1;

	unsigned char* pucOutputCbPtr = image_Cb;
	unsigned char* pucOutputCrPtr = image_Cr;

	unsigned int uiTmp1,uiTmp2;
	unsigned int ui1_1,ui1_2,ui2_1,ui2_2,res_1,res_2,res_3,res_4;
	unsigned int tmpend1_1,tmpend1_2,tmpend2_1,tmpend2_2;

	unsigned int uiA,uiB,uiC,uiD;
	unsigned int uiE,uiF,uiG,uiH;

	unsigned int uicst = xFracl * yFracl;

	uiTmp1 = _pack2(uicst,uicst);
	uiTmp2 = (_pack2(xFracl,yFracl)) << 3;
	uiTmp2 = _sub2(uiTmp2,uiTmp1);
	uiTmp1 = (uicst) + ((uicst - ((xFracl + yFracl) <<3) + 64) << 16);
	uicst = _packh2(uiTmp1,uiTmp2); // cst2 cst3
	uiTmp1 = _pack2(uiTmp2,uiTmp1); // cst4 cst1
	uicst =  _spacku4(uicst,uiTmp1);

	uiA = _mem2(pucCbPtrA);
	uiB = _mem2(pucCbPtrB);
	uiC = _mem2(pucCbPtrC);
	uiD = _mem2(pucCbPtrD);

	uiE = _mem2(pucCrPtrE);
	uiF = _mem2(pucCrPtrF);
	uiG = _mem2(pucCrPtrG);
	uiH = _mem2(pucCrPtrH);

	pucCbPtrA += PicWidthSamples;
	pucCbPtrB += PicWidthSamples;
	pucCbPtrC += PicWidthSamples;
	pucCbPtrD += PicWidthSamples;

	pucCrPtrE += PicWidthSamples;
	pucCrPtrF += PicWidthSamples;
	pucCrPtrG += PicWidthSamples;
	pucCrPtrH += PicWidthSamples;

	uiA += (_mem2(pucCbPtrA) << 16);
	uiB += (_mem2(pucCbPtrB) << 16);
	uiC += (_mem2(pucCbPtrC) << 16);
	uiD += (_mem2(pucCbPtrD) << 16);

	uiE += (_mem2(pucCrPtrE) << 16);
	uiF += (_mem2(pucCrPtrF) << 16);
	uiG += (_mem2(pucCrPtrG) << 16);
	uiH += (_mem2(pucCrPtrH) << 16);

	uiTmp1 = _packh4(uiA,uiB);
	uiTmp2 = _packh4(uiC,uiD);
	ui1_1 = _packh4(uiTmp1,uiTmp2);
	ui2_1 = _packl4(uiTmp1,uiTmp2);
	uiTmp1 = _packl4(uiA,uiB);
	uiTmp2 = _packl4(uiC,uiD);
	ui1_2 = _packh4(uiTmp1,uiTmp2);
	ui2_2 = _packl4(uiTmp1,uiTmp2);

	tmpend1_1 = _dotpu4(uicst,ui1_1);
	tmpend1_2 = _dotpu4(uicst,ui1_2);
	tmpend2_1 = _dotpu4(uicst,ui2_1);
	tmpend2_2 = _dotpu4(uicst,ui2_2);

	res_1 = _pack2(tmpend1_1,tmpend1_2);
	res_2 = _pack2(tmpend2_1,tmpend2_2);
	res_1 = _shr2(_sadd2(res_1,0x00200020),6);
	res_2 = _shr2(_sadd2(res_2,0x00200020),6);

	res_1 = _spacku4(0x00000000,res_1);
	res_2 = _spacku4(0x00000000,res_2);

	_mem2(pucOutputCbPtr) = res_2;
	pucOutputCbPtr += stride;
	_mem2(pucOutputCbPtr) = res_1;

	uiTmp1 = _packh4(uiE,uiF);
	uiTmp2 = _packh4(uiG,uiH);
	ui1_1 = _packh4(uiTmp1,uiTmp2);
	ui2_1 = _packl4(uiTmp1,uiTmp2);
	uiTmp1 = _packl4(uiE,uiF);
	uiTmp2 = _packl4(uiG,uiH);
	ui1_2 = _packh4(uiTmp1,uiTmp2);
	ui2_2 = _packl4(uiTmp1,uiTmp2);

	tmpend1_1 = _dotpu4(uicst,ui1_1);
	tmpend1_2 = _dotpu4(uicst,ui1_2);
	tmpend2_1 = _dotpu4(uicst,ui2_1);
	tmpend2_2 = _dotpu4(uicst,ui2_2);

	res_3 = _pack2(tmpend1_1,tmpend1_2);
	res_4 = _pack2(tmpend2_1,tmpend2_2);
	res_3 = _shr2(_sadd2(res_3,0x00200020),6);
	res_4 = _shr2(_sadd2(res_4,0x00200020),6);

	res_3 = _spacku4(0x00000000,res_3);
	res_4 = _spacku4(0x00000000,res_4);

	_mem2(pucOutputCrPtr) = res_4;
	pucOutputCrPtr += stride;
	_mem2(pucOutputCrPtr) = res_3;

}

#endif


#ifdef MMX_____
void chroma_sample_interpolation_MMX(unsigned char image_Cb [RESTRICT],	unsigned char image_Cr [RESTRICT], 
									 unsigned char refPicLXCb[RESTRICT], unsigned char refPicLXCr[RESTRICT],
									 const short xFracl, const short yFracl,  const short PicWidthSamples,const short stride)
{




	const short cst1 = xFracl * yFracl;
	__m64 mcst = _mm_set_pi16( cst1, (yFracl << 3) - cst1, (xFracl << 3) - cst1, cst1 - ((xFracl + yFracl) << 3) + 64);

	__m64 mPCb0 = _mm_set_pi8(refPicLXCb[2 + (PicWidthSamples << 1)], refPicLXCb[1 + (PicWidthSamples << 1)],refPicLXCb[PicWidthSamples << 1],refPicLXCb[2 + PicWidthSamples],refPicLXCb[1 + PicWidthSamples],  refPicLXCb[PicWidthSamples], refPicLXCb[2],refPicLXCb[1]);
	__m64 mPCr0 = _mm_set_pi8(refPicLXCr[2 + (PicWidthSamples << 1)], refPicLXCr[1 + (PicWidthSamples << 1)],refPicLXCr[PicWidthSamples << 1],refPicLXCr[2 + PicWidthSamples],refPicLXCr[1 + PicWidthSamples],  refPicLXCr[PicWidthSamples], refPicLXCr[2],refPicLXCr[1]);


	image_Cb[0] = (refPicLXCb[0] * mcst .m64_u16[0] + mPCb0 . m64_u8[0] * mcst .m64_u16[1] + mPCb0 . m64_u8[2] * mcst .m64_u16[2] + mPCb0 . m64_u8[3] * mcst .m64_u16[3] + 32) >> 6;
	image_Cb[1] = (mPCb0 . m64_u8[0] * mcst .m64_u16[0] + mPCb0 . m64_u8[1] * mcst .m64_u16[1] + mPCb0 . m64_u8[3] * mcst .m64_u16[2] + mPCb0 . m64_u8[4] * mcst .m64_u16[3] + 32) >> 6;
	image_Cr[0] = (refPicLXCr[0] * mcst .m64_u16[0] + mPCr0 . m64_u8[0] * mcst .m64_u16[1] + mPCr0 . m64_u8[2] * mcst .m64_u16[2] + mPCr0 . m64_u8[3] * mcst .m64_u16[3] + 32) >> 6;
	image_Cr[1] = (mPCr0 . m64_u8[0] * mcst .m64_u16[0] + mPCr0 . m64_u8[1] * mcst .m64_u16[1] + mPCr0 . m64_u8[3] * mcst .m64_u16[2] + mPCr0 . m64_u8[4] * mcst .m64_u16[3] + 32) >> 6;

	image_Cb[stride]	 = (mPCb0 . m64_u8[2] * mcst .m64_u16[0] + mPCb0 . m64_u8[3] * mcst .m64_u16[1] + mPCb0 . m64_u8[5] * mcst .m64_u16[2] + mPCb0 . m64_u8[6] * mcst .m64_u16[3] + 32) >> 6;
	image_Cb[1 + stride] = (mPCb0 . m64_u8[3] * mcst .m64_u16[0] + mPCb0 . m64_u8[4] * mcst .m64_u16[1] + mPCb0 . m64_u8[6] * mcst .m64_u16[2] + mPCb0 . m64_u8[7] * mcst .m64_u16[3] + 32) >> 6;

	image_Cr[stride]	 = (mPCr0 . m64_u8[2] * mcst .m64_u16[0] + mPCr0 . m64_u8[3] * mcst .m64_u16[1] + mPCr0 . m64_u8[5] * mcst .m64_u16[2] + mPCr0 . m64_u8[6] * mcst .m64_u16[3] + 32) >> 6;
	image_Cr[1 + stride] = (mPCr0 . m64_u8[3] * mcst .m64_u16[0] + mPCr0 . m64_u8[4] * mcst .m64_u16[1] + mPCr0 . m64_u8[6] * mcst .m64_u16[2] + mPCr0 . m64_u8[7] * mcst .m64_u16[3] + 32) >> 6;

	empty();  
}

#endif




/**
Chroma samples interpolation.
*/
void chroma_sample_interpolation_C(unsigned char image_Cb [RESTRICT],	unsigned char image_Cr [RESTRICT], 
								   unsigned char refPicLXCb[RESTRICT], unsigned char refPicLXCr[RESTRICT], 
								   const short xFracl, const short yFracl,  const short PicWidthSamples,const short stride)
{



	const short cst1 = xFracl * yFracl;
	const short cst2 = cst1 - ((xFracl + yFracl) << 3) + 64;
	const short cst3 = (xFracl << 3) - cst1;
	const short cst4 = (yFracl << 3) - cst1;

	unsigned char A1 = refPicLXCb[0];
	unsigned char B1 = refPicLXCb[1];
	unsigned char I1 = refPicLXCb[2];
	unsigned char C1 = refPicLXCb[0 + PicWidthSamples];
	unsigned char D1 = refPicLXCb[1 + PicWidthSamples];
	unsigned char J1 = refPicLXCb[2 + PicWidthSamples];

	unsigned char E1 = refPicLXCr[0];
	unsigned char F1 = refPicLXCr[1];
	unsigned char K1 = refPicLXCr[2];
	unsigned char G1 = refPicLXCr[0 + PicWidthSamples];
	unsigned char H1 = refPicLXCr[1 + PicWidthSamples];
	unsigned char L1 = refPicLXCr[2 + PicWidthSamples];

	{
		image_Cb[0] = (cst2*A1 + cst3*B1 + cst4*C1 + cst1*D1 + 32) >> 6;
		image_Cb[1] = (cst2*B1 + cst3*I1 + cst4*D1 + cst1*J1 + 32) >> 6;
		image_Cr[0] = (cst2*E1 + cst3*F1 + cst4*G1 + cst1*H1 + 32) >> 6;
		image_Cr[1] = (cst2*F1 + cst3*K1 + cst4*H1 + cst1*L1 + 32) >> 6;
	}

	{
		unsigned char C2 = refPicLXCb[0 + 2*PicWidthSamples];
		unsigned char D2 = refPicLXCb[1 + 2*PicWidthSamples];
		unsigned char J2 = refPicLXCb[2 + 2*PicWidthSamples];
		unsigned char G2 = refPicLXCr[0 + 2*PicWidthSamples];
		unsigned char H2 = refPicLXCr[1 + 2*PicWidthSamples];
		unsigned char L2 = refPicLXCr[2 + 2*PicWidthSamples];

		image_Cb[stride] = (cst2*C1 + cst3*D1 + cst4*C2 + cst1*D2 + 32) >> 6;
		image_Cb[1 + stride] = (cst2*D1 + cst3*J1 + cst4*D2 + cst1*J2 + 32) >> 6;
		image_Cr[stride] = (cst2*G1 + cst3*H1 + cst4*G2 + cst1*H2 + 32) >> 6;
		image_Cr[1 + stride] = (cst2*H1 + cst3*L1 + cst4*H2 + cst1*L2 + 32) >> 6;
	}
}






/**
Luma and chroma sample interpolation.
*/
void Interpolate(int Ref, short Currentx, short Currenty, short *mvLX, const short OutStride, 
				 const short PicWidthInPix, const short PicHeightInPix, const LIST_MMO *RefPicListL0, 
				 unsigned char *Y, unsigned char*U, unsigned char *V, 
				 unsigned char *DpbLuma, unsigned char*DpbCb, unsigned char *DpbCr, const interpol_4x4 *interpol)
{

	const int address_L = RefPicListL0[Ref] . MemoryAddress; 
	const int address_C = address_L >> 2;

	const short currentMvx = mvLX[0];
	const short currentMvy = mvLX[1];

	const short xIntl = CLIP3_16(-8, PicWidthInPix - 24, Currentx + (currentMvx >> 2));
	const short yIntl = CLIP3_16(-8, PicHeightInPix - 24, Currenty + (currentMvy >> 2));

	unsigned char *ptr_dpb_Cb = &DpbCb[address_C + (xIntl >> 1) + ( yIntl >> 1 ) * (PicWidthInPix >> 1)];
	unsigned char *ptr_dpb_Cr = &DpbCr[address_C + (xIntl >> 1) + ( yIntl >> 1 ) * (PicWidthInPix >> 1)];

	//Luminance interpolation 
	short xFracl = currentMvx&3;
	short yFracl = currentMvy&3;
	interpol[xFracl + (yFracl << 2)](Y, &DpbLuma[address_L + yIntl * PicWidthInPix + xIntl], PicWidthInPix, OutStride);

	//Chrominance interpolation
	xFracl = currentMvx&7;
	yFracl = currentMvy&7;
	chroma_sample_interpolation(U, V, ptr_dpb_Cb, ptr_dpb_Cr, xFracl,  yFracl, PicWidthInPix >> 1, OutStride >> 1);
}




/**
This function allows to get the prediction of an non IDR picture. 
//8.4.2.2


@param DpbLuma Table of the reference decoded picture buffer.
@param DpbCb Table of the reference decoded picture buffer.
@param DpbCr Table of the reference decoded picture buffer.
@param image Table of current frame.
@param image_Cb Table of current frame.
@param image_Cr Table of current frame.
@param mvLX The motion vector are stocked for each 4x4 block of each macroblock.
@param ref_cache A cache table where the reference is stocked for each 4x4 block of each macroblock.
@param x X-coordinate.
@param y Y-coordinate.
@param PicWidthInPix Width in pixel of the current frame.
@param PicHeightInPix Height in pixel of the current frame.
@prama RefPicListL0 List l0 of reference image.
@param interpol Function pointer of 4x4 interpoaltion.
*/
void sample_interpolation(unsigned char *DpbLuma,unsigned char *DpbCb,	unsigned char *DpbCr, 
						  unsigned char *image, unsigned char *image_Cb, unsigned char *image_Cr, 
						  short mvLX[][2], short *ref_cache, const short x, const short y, 
						  const short PicWidthInPix, const short PicHeightInPix, 
						  const LIST_MMO *RefPicListL0, const interpol_4x4 *interpol)
{


	int i;

	for(i = 0; i < 16; i++)	{
		const short index8 = SCAN8(i);

		if (ref_cache[index8] > -1){
			const short Currentx = x + LOCX(i);
			const short Currenty = y + LOCY(i);
			unsigned char *U = &image_Cb[((LOCX(i)) >> 1) + ( (LOCY(i)) >> 1 ) * (PicWidthInPix >> 1)];
			unsigned char *V = &image_Cr[((LOCX(i)) >> 1) + ( (LOCY(i)) >> 1 ) * (PicWidthInPix >> 1)];
			unsigned char *Y = &image[(PicWidthInPix * (LOCY(i)) ) + LOCX(i)];

			Interpolate(ref_cache[index8], Currentx, Currenty, &mvLX[index8][0], PicWidthInPix, PicWidthInPix, PicHeightInPix, 
				RefPicListL0, Y, U, V, DpbLuma, DpbCb, DpbCr, interpol);
		}
	}	
}



