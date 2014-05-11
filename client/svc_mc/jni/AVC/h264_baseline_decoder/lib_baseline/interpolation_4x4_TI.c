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


#ifdef TI_OPTIM

#include "interpolation.h"
#include "clip.h"
#include "symbol.h"
#include "data.h"


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


void chroma_sample_interpolation_TI(unsigned char image_Cb [RESTRICT],	 unsigned char image_Cr [RESTRICT]
					, unsigned char refPicLXCb[RESTRICT], unsigned char refPicLXCr[RESTRICT]
					, const short xFracl, const short yFracl,  const short PicWidthSamples,const short stride)
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

