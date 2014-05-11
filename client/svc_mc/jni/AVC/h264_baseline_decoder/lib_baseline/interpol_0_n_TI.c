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
#include "type.h"
#include "clip.h"
#include "interpol_0_n.h"





void luma_sample_interp_0_0_TI(unsigned char image [], unsigned char refPicLXl[],	const short PicWidthSamples, const short stride){



	unsigned int uiTmp1,uiTmp2,uiTmp3,uiTmp4;
	unsigned char* pRefImgPtr = refPicLXl;
	unsigned char* pImgPtr = image;

	uiTmp1 = _mem4 (pRefImgPtr);
	pRefImgPtr+=PicWidthSamples;
	uiTmp2 = _mem4 (pRefImgPtr);
	pRefImgPtr+=PicWidthSamples;
	uiTmp3 = _mem4 (pRefImgPtr);
	pRefImgPtr+=PicWidthSamples;
	uiTmp4 = _mem4 (pRefImgPtr);

	_amem4 (pImgPtr) = uiTmp1;
	pImgPtr+=stride;
	_amem4 (pImgPtr) = uiTmp2;
	pImgPtr+=stride;
	_amem4 (pImgPtr) = uiTmp3;
	pImgPtr+=stride;
	_amem4 (pImgPtr) = uiTmp4;

}


/**
This function allows to get the luminance prediction of a non IDR picture when xFracl = 0 and yFracl = 1.


@param image Table of current frame.
@param refPicLXl Table of the reference decoded picture buffer.
@param PicWidthSamples Stride of the reference buffer.
@param stride Stride of the current image.
*/
void luma_sample_interp_0_1_TI(unsigned char image [], unsigned char refPicLXl[],	const short PicWidthSamples, const short stride){

	/* No horizontal interpolation */


	unsigned int uiLine1,uiLine2,uiLine3,uiLine4,uiLine5,uiLine6,uiLine7,uiLine8,uiLine9;
	unsigned int uiTmpLine12_h,uiTmpLine34_h,uiTmpLine12_l,uiTmpLine34_l,uiTmpLine1234_4,uiTmpLine1234_2,uiTmpLine1234_3,uiTmpLine1234_1;
	unsigned int uiTmpLine56_h,uiTmpLine78_h,uiTmpLine56_l,uiTmpLine78_l,uiTmpLine5678_4,uiTmpLine5678_2,uiTmpLine5678_3,uiTmpLine5678_1;
	unsigned int tmpc1_1,tmpc1_2,tmpc2_1,tmpc2_2,tmpc1,tmpc2;
	unsigned int tmpc3_1,tmpc3_2,tmpc4_1,tmpc4_2,tmpc3,tmpc4;
	unsigned int tmp12,tmp34,tmpend1,tmpend2,tmpend3,tmpend4;
	unsigned int tmpl9l,tmpl9h;
	unsigned int input1,input2,input3,input4;
	unsigned char* pRefImgPtr;
	unsigned char* pImgPtr;

	pRefImgPtr = refPicLXl-(PicWidthSamples<<1);
	pImgPtr = image;

	uiLine1 = _mem4(pRefImgPtr);
	pRefImgPtr += PicWidthSamples;
	uiLine2 = _mem4(pRefImgPtr);
	pRefImgPtr += PicWidthSamples;
	uiLine3 = _mem4(pRefImgPtr);
	pRefImgPtr += PicWidthSamples;
	uiLine4 = _mem4(pRefImgPtr);
	pRefImgPtr += PicWidthSamples;
	uiLine5 = _mem4(pRefImgPtr);
	pRefImgPtr += PicWidthSamples;
	uiLine6 = _mem4(pRefImgPtr);
	pRefImgPtr += PicWidthSamples;
	uiLine7 = _mem4(pRefImgPtr);
	pRefImgPtr += PicWidthSamples;
	uiLine8 = _mem4(pRefImgPtr);
	pRefImgPtr += PicWidthSamples;
	uiLine9 = _mem4(pRefImgPtr);


	input1 = uiLine3;
	input2 = uiLine4;
	input3 = uiLine5;
	input4 = uiLine6;


	uiTmpLine12_h = _packh4(uiLine1,uiLine2);
	uiTmpLine34_h = _packh4(uiLine3,uiLine4);
	uiTmpLine12_l = _packl4(uiLine1,uiLine2);
	uiTmpLine34_l = _packl4(uiLine3,uiLine4);
	uiTmpLine1234_4 = _packh4(uiTmpLine12_h,uiTmpLine34_h);
	uiTmpLine1234_2 = _packl4(uiTmpLine12_h,uiTmpLine34_h);
	uiTmpLine1234_3 = _packh4(uiTmpLine12_l,uiTmpLine34_l);
	uiTmpLine1234_1 = _packl4(uiTmpLine12_l,uiTmpLine34_l);

	uiTmpLine56_h = _packh4(uiLine5,uiLine6);
	uiTmpLine78_h = _packh4(uiLine7,uiLine8);
	uiTmpLine56_l = _packl4(uiLine5,uiLine6);
	uiTmpLine78_l = _packl4(uiLine7,uiLine8);
	uiTmpLine5678_4 = _packh4(uiTmpLine56_h,uiTmpLine78_h);
	uiTmpLine5678_2 = _packl4(uiTmpLine56_h,uiTmpLine78_h);
	uiTmpLine5678_3 = _packh4(uiTmpLine56_l,uiTmpLine78_l);
	uiTmpLine5678_1 = _packl4(uiTmpLine56_l,uiTmpLine78_l);

	tmpc1_1 = _dotpsu4(0x01FB1414,uiTmpLine1234_1);
	tmpc1_2 = _dotpsu4(0xFB010000,uiTmpLine5678_1);

	tmpc2_1 = _dotpsu4(0x01FB1414,uiTmpLine1234_2);
	tmpc2_2 = _dotpsu4(0xFB010000,uiTmpLine5678_2);

	tmpc1 = _spack2(tmpc1_1,tmpc2_1);
	tmpc2 = _spack2(tmpc1_2,tmpc2_2);

	tmp12 = _sadd2(tmpc1,tmpc2);
	tmp12 = _shr2(_sadd2(tmp12,0x00100010),5);

	tmpc3_1 = _dotpsu4(0x01FB1414,uiTmpLine1234_3);
	tmpc3_2 = _dotpsu4(0xFB010000,uiTmpLine5678_3);

	tmpc4_1 = _dotpsu4(0x01FB1414,uiTmpLine1234_4);
	tmpc4_2 = _dotpsu4(0xFB010000,uiTmpLine5678_4);

	tmpc3 = _spack2(tmpc3_1,tmpc4_1);
	tmpc4 = _spack2(tmpc3_2,tmpc4_2);

	tmp34 = _sadd2(tmpc3,tmpc4);
	tmp34 = _shr2(_sadd2(tmp34,0x00100010),5);

	tmpend1 = _spacku4(tmp34,tmp12);
	tmpend1 = _swap4(tmpend1);

	_amem4(pImgPtr) = _avgu4(tmpend1,input1);
	pImgPtr += stride;

	tmpc1_1 = _dotpsu4(0x0001FB14,uiTmpLine1234_1);
	tmpc1_2 = _dotpsu4(0x14FB0100,uiTmpLine5678_1);

	tmpc2_1 = _dotpsu4(0x0001FB14,uiTmpLine1234_2);
	tmpc2_2 = _dotpsu4(0x14FB0100,uiTmpLine5678_2);

	tmpc1 = _spack2(tmpc1_1,tmpc2_1);
	tmpc2 = _spack2(tmpc1_2,tmpc2_2);

	tmp12 = _sadd2(tmpc1,tmpc2);
	tmp12 = _shr2(_sadd2(tmp12,0x00100010),5);

	tmpc3_1 = _dotpsu4(0x0001FB14,uiTmpLine1234_3);
	tmpc3_2 = _dotpsu4(0x14FB0100,uiTmpLine5678_3);

	tmpc4_1 = _dotpsu4(0x0001FB14,uiTmpLine1234_4);
	tmpc4_2 = _dotpsu4(0x14FB0100,uiTmpLine5678_4);

	tmpc3 = _spack2(tmpc3_1,tmpc4_1);
	tmpc4 = _spack2(tmpc3_2,tmpc4_2);

	tmp34 = _sadd2(tmpc3,tmpc4);
	tmp34 = _shr2(_sadd2(tmp34,0x00100010),5);

	tmpend2 = _spacku4(tmp34,tmp12);
	tmpend2 = _swap4(tmpend2);

	_amem4(pImgPtr) = _avgu4(tmpend2,input2);
	pImgPtr += stride;

	tmpc1_1 = _dotpsu4(0x000001FB,uiTmpLine1234_1);
	tmpc1_2 = _dotpsu4(0x1414FB01,uiTmpLine5678_1);

	tmpc2_1 = _dotpsu4(0x000001FB,uiTmpLine1234_2);
	tmpc2_2 = _dotpsu4(0x1414FB01,uiTmpLine5678_2);

	tmpc1 = _spack2(tmpc1_1,tmpc2_1);
	tmpc2 = _spack2(tmpc1_2,tmpc2_2);

	tmp12 = _sadd2(tmpc1,tmpc2);
	tmp12 = _shr2(_sadd2(tmp12,0x00100010),5);

	tmpc3_1 = _dotpsu4(0x000001FB,uiTmpLine1234_3);
	tmpc3_2 = _dotpsu4(0x1414FB01,uiTmpLine5678_3);

	tmpc4_1 = _dotpsu4(0x000001FB,uiTmpLine1234_4);
	tmpc4_2 = _dotpsu4(0x1414FB01,uiTmpLine5678_4); 

	tmpc3 = _spack2(tmpc3_1,tmpc4_1);
	tmpc4 = _spack2(tmpc3_2,tmpc4_2);

	tmp34 = _sadd2(tmpc3,tmpc4);
	tmp34 = _shr2(_sadd2(tmp34,0x00100010),5);

	tmpend3 = _spacku4(tmp34,tmp12);
	tmpend3 = _swap4(tmpend3);

	_amem4(pImgPtr) = _avgu4(tmpend3,input3);
	pImgPtr += stride;

	uiLine9 = _swap4(uiLine9);
	tmpl9h = _unpkhu4 (uiLine9);
	tmpl9l = _unpklu4 (uiLine9);

	tmpc1_1 = _extu(uiTmpLine1234_1,24,24);//_dotpsu4(0x00000001,uiTmpLine1234_1);
	tmpc1_2 = _dotpsu4(0xFB1414FB,uiTmpLine5678_1);

	tmpc2_1 = _extu(uiTmpLine1234_2,24,24);//_dotpsu4(0x00000001,uiTmpLine1234_2);
	tmpc2_2 = _dotpsu4(0xFB1414FB,uiTmpLine5678_2);

	tmpc1 = _spack2(tmpc1_1,tmpc2_1);
	tmpc2 = _spack2(tmpc1_2,tmpc2_2);

	tmp12 = _sadd2(tmpc1,tmpc2);
	tmp12 = _sadd2(tmp12,tmpl9l);
	tmp12 = _shr2(_sadd2(tmp12,0x00100010),5);

	tmpc3_1 = _extu(uiTmpLine1234_3,24,24);//_dotpsu4(0x00000001,uiTmpLine1234_3);
	tmpc3_2 = _dotpsu4(0xFB1414FB,uiTmpLine5678_3);

	tmpc4_1 = _extu(uiTmpLine1234_4,24,24);//_dotpsu4(0x00000001,uiTmpLine1234_4);
	tmpc4_2 = _dotpsu4(0xFB1414FB,uiTmpLine5678_4);

	tmpc3 = _spack2(tmpc3_1,tmpc4_1);
	tmpc4 = _spack2(tmpc3_2,tmpc4_2);

	tmp34 = _sadd2(tmpc3,tmpc4);
	tmp34 = _sadd2(tmp34,tmpl9h);
	tmp34 = _shr2(_sadd2(tmp34,0x00100010),5);

	tmpend4 = _spacku4(tmp34,tmp12);
	tmpend4 = _swap4(tmpend4);

	_amem4(pImgPtr) = _avgu4(tmpend4,input4);

}

/**
This function allows to get the luminance prediction of a non IDR picture when xFracl = 0 and yFracl = 2.


@param image Table of current frame.
@param refPicLXl Table of the reference decoded picture buffer.
@param PicWidthSamples Stride of the reference buffer.
@param stride Stride of the current image.
*/
void luma_sample_interp_0_2_TI(unsigned char image [RESTRICT], unsigned char refPicLXl[RESTRICT],	const short PicWidthSamples, const short stride)
{

	/* No horizontal interpolation */


	unsigned int uiLine1,uiLine2,uiLine3,uiLine4,uiLine5,uiLine6,uiLine7,uiLine8,uiLine9;
	unsigned int uiTmpLine12_h,uiTmpLine34_h,uiTmpLine12_l,uiTmpLine34_l,uiTmpLine1234_4,uiTmpLine1234_2,uiTmpLine1234_3,uiTmpLine1234_1;
	unsigned int uiTmpLine56_h,uiTmpLine78_h,uiTmpLine56_l,uiTmpLine78_l,uiTmpLine5678_4,uiTmpLine5678_2,uiTmpLine5678_3,uiTmpLine5678_1;
	unsigned int tmpc1_1,tmpc1_2,tmpc2_1,tmpc2_2,tmpc1,tmpc2;
	unsigned int tmpc3_1,tmpc3_2,tmpc4_1,tmpc4_2,tmpc3,tmpc4;
	unsigned int tmp12,tmp34,tmpend1,tmpend2,tmpend3,tmpend4;
	unsigned int tmpl9l,tmpl9h;
	unsigned char* pRefImgPtr;
	unsigned char* pImgPtr;

	pRefImgPtr = refPicLXl-(PicWidthSamples<<1);
	pImgPtr = image;

	uiLine1 = _mem4(pRefImgPtr);
	pRefImgPtr += PicWidthSamples;
	uiLine2 = _mem4(pRefImgPtr);
	pRefImgPtr += PicWidthSamples;
	uiLine3 = _mem4(pRefImgPtr);
	pRefImgPtr += PicWidthSamples;
	uiLine4 = _mem4(pRefImgPtr);
	pRefImgPtr += PicWidthSamples;
	uiLine5 = _mem4(pRefImgPtr);
	pRefImgPtr += PicWidthSamples;
	uiLine6 = _mem4(pRefImgPtr);
	pRefImgPtr += PicWidthSamples;
	uiLine7 = _mem4(pRefImgPtr);
	pRefImgPtr += PicWidthSamples;
	uiLine8 = _mem4(pRefImgPtr);
	pRefImgPtr += PicWidthSamples;
	uiLine9 = _mem4(pRefImgPtr);


	uiTmpLine12_h = _packh4(uiLine1,uiLine2);
	uiTmpLine34_h = _packh4(uiLine3,uiLine4);
	uiTmpLine12_l = _packl4(uiLine1,uiLine2);
	uiTmpLine34_l = _packl4(uiLine3,uiLine4);
	uiTmpLine1234_4 = _packh4(uiTmpLine12_h,uiTmpLine34_h);
	uiTmpLine1234_2 = _packl4(uiTmpLine12_h,uiTmpLine34_h);
	uiTmpLine1234_3 = _packh4(uiTmpLine12_l,uiTmpLine34_l);
	uiTmpLine1234_1 = _packl4(uiTmpLine12_l,uiTmpLine34_l);

	uiTmpLine56_h = _packh4(uiLine5,uiLine6);
	uiTmpLine78_h = _packh4(uiLine7,uiLine8);
	uiTmpLine56_l = _packl4(uiLine5,uiLine6);
	uiTmpLine78_l = _packl4(uiLine7,uiLine8);
	uiTmpLine5678_4 = _packh4(uiTmpLine56_h,uiTmpLine78_h);
	uiTmpLine5678_2 = _packl4(uiTmpLine56_h,uiTmpLine78_h);
	uiTmpLine5678_3 = _packh4(uiTmpLine56_l,uiTmpLine78_l);
	uiTmpLine5678_1 = _packl4(uiTmpLine56_l,uiTmpLine78_l);

	tmpc1_1 = _dotpsu4(0x01FB1414,uiTmpLine1234_1);
	tmpc1_2 = _dotpsu4(0xFB010000,uiTmpLine5678_1);

	tmpc2_1 = _dotpsu4(0x01FB1414,uiTmpLine1234_2);
	tmpc2_2 = _dotpsu4(0xFB010000,uiTmpLine5678_2);

	tmpc1 = _spack2(tmpc1_1,tmpc2_1);
	tmpc2 = _spack2(tmpc1_2,tmpc2_2);

	tmp12 = _sadd2(tmpc1,tmpc2);
	tmp12 = _shr2(_sadd2(tmp12,0x00100010),5);

	tmpc3_1 = _dotpsu4(0x01FB1414,uiTmpLine1234_3);
	tmpc3_2 = _dotpsu4(0xFB010000,uiTmpLine5678_3);

	tmpc4_1 = _dotpsu4(0x01FB1414,uiTmpLine1234_4);
	tmpc4_2 = _dotpsu4(0xFB010000,uiTmpLine5678_4);

	tmpc3 = _spack2(tmpc3_1,tmpc4_1);
	tmpc4 = _spack2(tmpc3_2,tmpc4_2);

	tmp34 = _sadd2(tmpc3,tmpc4);
	tmp34 = _shr2(_sadd2(tmp34,0x00100010),5);

	tmpend1 = _spacku4(tmp34,tmp12);
	tmpend1 = _swap4(tmpend1);

	_amem4(pImgPtr) = tmpend1;
	pImgPtr += stride;

	tmpc1_1 = _dotpsu4(0x0001FB14,uiTmpLine1234_1);
	tmpc1_2 = _dotpsu4(0x14FB0100,uiTmpLine5678_1);

	tmpc2_1 = _dotpsu4(0x0001FB14,uiTmpLine1234_2);
	tmpc2_2 = _dotpsu4(0x14FB0100,uiTmpLine5678_2);

	tmpc1 = _spack2(tmpc1_1,tmpc2_1);
	tmpc2 = _spack2(tmpc1_2,tmpc2_2);

	tmp12 = _sadd2(tmpc1,tmpc2);
	tmp12 = _shr2(_sadd2(tmp12,0x00100010),5);

	tmpc3_1 = _dotpsu4(0x0001FB14,uiTmpLine1234_3);
	tmpc3_2 = _dotpsu4(0x14FB0100,uiTmpLine5678_3);

	tmpc4_1 = _dotpsu4(0x0001FB14,uiTmpLine1234_4);
	tmpc4_2 = _dotpsu4(0x14FB0100,uiTmpLine5678_4);

	tmpc3 = _spack2(tmpc3_1,tmpc4_1);
	tmpc4 = _spack2(tmpc3_2,tmpc4_2);

	tmp34 = _sadd2(tmpc3,tmpc4);
	tmp34 = _shr2(_sadd2(tmp34,0x00100010),5);

	tmpend2 = _spacku4(tmp34,tmp12);
	tmpend2 = _swap4(tmpend2);

	_amem4(pImgPtr) = tmpend2;
	pImgPtr += stride;

	tmpc1_1 = _dotpsu4(0x000001FB,uiTmpLine1234_1);
	tmpc1_2 = _dotpsu4(0x1414FB01,uiTmpLine5678_1);

	tmpc2_1 = _dotpsu4(0x000001FB,uiTmpLine1234_2);
	tmpc2_2 = _dotpsu4(0x1414FB01,uiTmpLine5678_2);

	tmpc1 = _spack2(tmpc1_1,tmpc2_1);
	tmpc2 = _spack2(tmpc1_2,tmpc2_2);

	tmp12 = _sadd2(tmpc1,tmpc2);
	tmp12 = _shr2(_sadd2(tmp12,0x00100010),5);

	tmpc3_1 = _dotpsu4(0x000001FB,uiTmpLine1234_3);
	tmpc3_2 = _dotpsu4(0x1414FB01,uiTmpLine5678_3);

	tmpc4_1 = _dotpsu4(0x000001FB,uiTmpLine1234_4);
	tmpc4_2 = _dotpsu4(0x1414FB01,uiTmpLine5678_4); 

	tmpc3 = _spack2(tmpc3_1,tmpc4_1);
	tmpc4 = _spack2(tmpc3_2,tmpc4_2);

	tmp34 = _sadd2(tmpc3,tmpc4);
	tmp34 = _shr2(_sadd2(tmp34,0x00100010),5);

	tmpend3 = _spacku4(tmp34,tmp12);
	tmpend3 = _swap4(tmpend3);

	_amem4(pImgPtr) = tmpend3;
	pImgPtr += stride;

	uiLine9 = _swap4(uiLine9);
	tmpl9h = _unpkhu4 (uiLine9);
	tmpl9l = _unpklu4 (uiLine9);

	tmpc1_1 = _extu(uiTmpLine1234_1,24,24);//_dotpsu4(0x00000001,uiTmpLine1234_1);
	tmpc1_2 = _dotpsu4(0xFB1414FB,uiTmpLine5678_1);

	tmpc2_1 = _extu(uiTmpLine1234_2,24,24);//_dotpsu4(0x00000001,uiTmpLine1234_2);
	tmpc2_2 = _dotpsu4(0xFB1414FB,uiTmpLine5678_2);

	tmpc1 = _spack2(tmpc1_1,tmpc2_1);
	tmpc2 = _spack2(tmpc1_2,tmpc2_2);

	tmp12 = _sadd2(tmpc1,tmpc2);
	tmp12 = _sadd2(tmp12,tmpl9l);
	tmp12 = _shr2(_sadd2(tmp12,0x00100010),5);

	tmpc3_1 = _extu(uiTmpLine1234_3,24,24);//_dotpsu4(0x00000001,uiTmpLine1234_3);
	tmpc3_2 = _dotpsu4(0xFB1414FB,uiTmpLine5678_3);

	tmpc4_1 = _extu(uiTmpLine1234_4,24,24);//_dotpsu4(0x00000001,uiTmpLine1234_4);
	tmpc4_2 = _dotpsu4(0xFB1414FB,uiTmpLine5678_4);

	tmpc3 = _spack2(tmpc3_1,tmpc4_1);
	tmpc4 = _spack2(tmpc3_2,tmpc4_2);

	tmp34 = _sadd2(tmpc3,tmpc4);
	tmp34 = _sadd2(tmp34,tmpl9h);
	tmp34 = _shr2(_sadd2(tmp34,0x00100010),5);

	tmpend4 = _spacku4(tmp34,tmp12);
	tmpend4 = _swap4(tmpend4);

	_amem4(pImgPtr) = tmpend4;
}

/**
This function allows to get the luminance prediction of a non IDR picture when xFracl = 0 and yFracl = 3.


@param image Table of current frame.
@param refPicLXl Table of the reference decoded picture buffer.
@param PicWidthSamples Stride of the reference buffer.
@param stride Stride of the current image.
*/
void luma_sample_interp_0_3_TI(unsigned char image [RESTRICT], unsigned char refPicLXl[RESTRICT],	const short PicWidthSamples, const short stride)
{

	/* No horizontal interpolation */



	unsigned int uiLine1,uiLine2,uiLine3,uiLine4,uiLine5,uiLine6,uiLine7,uiLine8,uiLine9;
	unsigned int uiTmpLine12_h,uiTmpLine34_h,uiTmpLine12_l,uiTmpLine34_l,uiTmpLine1234_4,uiTmpLine1234_2,uiTmpLine1234_3,uiTmpLine1234_1;
	unsigned int uiTmpLine56_h,uiTmpLine78_h,uiTmpLine56_l,uiTmpLine78_l,uiTmpLine5678_4,uiTmpLine5678_2,uiTmpLine5678_3,uiTmpLine5678_1;
	unsigned int tmpc1_1,tmpc1_2,tmpc2_1,tmpc2_2,tmpc1,tmpc2;
	unsigned int tmpc3_1,tmpc3_2,tmpc4_1,tmpc4_2,tmpc3,tmpc4;
	unsigned int tmp12,tmp34,tmpend1,tmpend2,tmpend3,tmpend4;
	unsigned int tmpl9l,tmpl9h;
	unsigned int input1,input2,input3,input4;
	unsigned char* pRefImgPtr;
	unsigned char* pImgPtr;

	pRefImgPtr = refPicLXl-(PicWidthSamples<<1);
	pImgPtr = image;

	uiLine1 = _mem4(pRefImgPtr);
	pRefImgPtr += PicWidthSamples;
	uiLine2 = _mem4(pRefImgPtr);
	pRefImgPtr += PicWidthSamples;
	uiLine3 = _mem4(pRefImgPtr);
	pRefImgPtr += PicWidthSamples;
	uiLine4 = _mem4(pRefImgPtr);
	pRefImgPtr += PicWidthSamples;
	uiLine5 = _mem4(pRefImgPtr);
	pRefImgPtr += PicWidthSamples;
	uiLine6 = _mem4(pRefImgPtr);
	pRefImgPtr += PicWidthSamples;
	uiLine7 = _mem4(pRefImgPtr);
	pRefImgPtr += PicWidthSamples;
	uiLine8 = _mem4(pRefImgPtr);
	pRefImgPtr += PicWidthSamples;
	uiLine9 = _mem4(pRefImgPtr);


	input1 = uiLine4;
	input2 = uiLine5;
	input3 = uiLine6;
	input4 = uiLine7;


	uiTmpLine12_h = _packh4(uiLine1,uiLine2);
	uiTmpLine34_h = _packh4(uiLine3,uiLine4);
	uiTmpLine12_l = _packl4(uiLine1,uiLine2);
	uiTmpLine34_l = _packl4(uiLine3,uiLine4);
	uiTmpLine1234_4 = _packh4(uiTmpLine12_h,uiTmpLine34_h);
	uiTmpLine1234_2 = _packl4(uiTmpLine12_h,uiTmpLine34_h);
	uiTmpLine1234_3 = _packh4(uiTmpLine12_l,uiTmpLine34_l);
	uiTmpLine1234_1 = _packl4(uiTmpLine12_l,uiTmpLine34_l);

	uiTmpLine56_h = _packh4(uiLine5,uiLine6);
	uiTmpLine78_h = _packh4(uiLine7,uiLine8);
	uiTmpLine56_l = _packl4(uiLine5,uiLine6);
	uiTmpLine78_l = _packl4(uiLine7,uiLine8);
	uiTmpLine5678_4 = _packh4(uiTmpLine56_h,uiTmpLine78_h);
	uiTmpLine5678_2 = _packl4(uiTmpLine56_h,uiTmpLine78_h);
	uiTmpLine5678_3 = _packh4(uiTmpLine56_l,uiTmpLine78_l);
	uiTmpLine5678_1 = _packl4(uiTmpLine56_l,uiTmpLine78_l);

	tmpc1_1 = _dotpsu4(0x01FB1414,uiTmpLine1234_1);
	tmpc1_2 = _dotpsu4(0xFB010000,uiTmpLine5678_1);

	tmpc2_1 = _dotpsu4(0x01FB1414,uiTmpLine1234_2);
	tmpc2_2 = _dotpsu4(0xFB010000,uiTmpLine5678_2);

	tmpc1 = _spack2(tmpc1_1,tmpc2_1);
	tmpc2 = _spack2(tmpc1_2,tmpc2_2);

	tmp12 = _sadd2(tmpc1,tmpc2);
	tmp12 = _shr2(_sadd2(tmp12,0x00100010),5);

	tmpc3_1 = _dotpsu4(0x01FB1414,uiTmpLine1234_3);
	tmpc3_2 = _dotpsu4(0xFB010000,uiTmpLine5678_3);

	tmpc4_1 = _dotpsu4(0x01FB1414,uiTmpLine1234_4);
	tmpc4_2 = _dotpsu4(0xFB010000,uiTmpLine5678_4);

	tmpc3 = _spack2(tmpc3_1,tmpc4_1);
	tmpc4 = _spack2(tmpc3_2,tmpc4_2);

	tmp34 = _sadd2(tmpc3,tmpc4);
	tmp34 = _shr2(_sadd2(tmp34,0x00100010),5);

	tmpend1 = _spacku4(tmp34,tmp12);
	tmpend1 = _swap4(tmpend1);

	_amem4(pImgPtr) =  _avgu4(tmpend1,input1);
	pImgPtr += stride;

	tmpc1_1 = _dotpsu4(0x0001FB14,uiTmpLine1234_1);
	tmpc1_2 = _dotpsu4(0x14FB0100,uiTmpLine5678_1);

	tmpc2_1 = _dotpsu4(0x0001FB14,uiTmpLine1234_2);
	tmpc2_2 = _dotpsu4(0x14FB0100,uiTmpLine5678_2);

	tmpc1 = _spack2(tmpc1_1,tmpc2_1);
	tmpc2 = _spack2(tmpc1_2,tmpc2_2);

	tmp12 = _sadd2(tmpc1,tmpc2);
	tmp12 = _shr2(_sadd2(tmp12,0x00100010),5);

	tmpc3_1 = _dotpsu4(0x0001FB14,uiTmpLine1234_3);
	tmpc3_2 = _dotpsu4(0x14FB0100,uiTmpLine5678_3);

	tmpc4_1 = _dotpsu4(0x0001FB14,uiTmpLine1234_4);
	tmpc4_2 = _dotpsu4(0x14FB0100,uiTmpLine5678_4);

	tmpc3 = _spack2(tmpc3_1,tmpc4_1);
	tmpc4 = _spack2(tmpc3_2,tmpc4_2);

	tmp34 = _sadd2(tmpc3,tmpc4);
	tmp34 = _shr2(_sadd2(tmp34,0x00100010),5);

	tmpend2 = _spacku4(tmp34,tmp12);
	tmpend2 = _swap4(tmpend2);

	_amem4(pImgPtr) =  _avgu4(tmpend2,input2);
	pImgPtr += stride;

	tmpc1_1 = _dotpsu4(0x000001FB,uiTmpLine1234_1);
	tmpc1_2 = _dotpsu4(0x1414FB01,uiTmpLine5678_1);

	tmpc2_1 = _dotpsu4(0x000001FB,uiTmpLine1234_2);
	tmpc2_2 = _dotpsu4(0x1414FB01,uiTmpLine5678_2);

	tmpc1 = _spack2(tmpc1_1,tmpc2_1);
	tmpc2 = _spack2(tmpc1_2,tmpc2_2);

	tmp12 = _sadd2(tmpc1,tmpc2);
	tmp12 = _shr2(_sadd2(tmp12,0x00100010),5);

	tmpc3_1 = _dotpsu4(0x000001FB,uiTmpLine1234_3);
	tmpc3_2 = _dotpsu4(0x1414FB01,uiTmpLine5678_3);

	tmpc4_1 = _dotpsu4(0x000001FB,uiTmpLine1234_4);
	tmpc4_2 = _dotpsu4(0x1414FB01,uiTmpLine5678_4); 

	tmpc3 = _spack2(tmpc3_1,tmpc4_1);
	tmpc4 = _spack2(tmpc3_2,tmpc4_2);

	tmp34 = _sadd2(tmpc3,tmpc4);
	tmp34 = _shr2(_sadd2(tmp34,0x00100010),5);

	tmpend3 = _spacku4(tmp34,tmp12);
	tmpend3 = _swap4(tmpend3);

	_amem4(pImgPtr) = _avgu4(tmpend3,input3);
	pImgPtr += stride;

	uiLine9 = _swap4(uiLine9);
	tmpl9h = _unpkhu4 (uiLine9);
	tmpl9l = _unpklu4 (uiLine9);

	tmpc1_1 = _extu(uiTmpLine1234_1,24,24);//_dotpsu4(0x00000001,uiTmpLine1234_1);
	tmpc1_2 = _dotpsu4(0xFB1414FB,uiTmpLine5678_1);

	tmpc2_1 = _extu(uiTmpLine1234_2,24,24);//_dotpsu4(0x00000001,uiTmpLine1234_2);
	tmpc2_2 = _dotpsu4(0xFB1414FB,uiTmpLine5678_2);

	tmpc1 = _spack2(tmpc1_1,tmpc2_1);
	tmpc2 = _spack2(tmpc1_2,tmpc2_2);

	tmp12 = _sadd2(tmpc1,tmpc2);
	tmp12 = _sadd2(tmp12,tmpl9l);
	tmp12 = _shr2(_sadd2(tmp12,0x00100010),5);

	tmpc3_1 = _extu(uiTmpLine1234_3,24,24);//_dotpsu4(0x00000001,uiTmpLine1234_3);
	tmpc3_2 = _dotpsu4(0xFB1414FB,uiTmpLine5678_3);

	tmpc4_1 = _extu(uiTmpLine1234_4,24,24);//_dotpsu4(0x00000001,uiTmpLine1234_4);
	tmpc4_2 = _dotpsu4(0xFB1414FB,uiTmpLine5678_4);

	tmpc3 = _spack2(tmpc3_1,tmpc4_1);
	tmpc4 = _spack2(tmpc3_2,tmpc4_2);

	tmp34 = _sadd2(tmpc3,tmpc4);
	tmp34 = _sadd2(tmp34,tmpl9h);
	tmp34 = _shr2(_sadd2(tmp34,0x00100010),5);

	tmpend4 = _spacku4(tmp34,tmp12);
	tmpend4 = _swap4(tmpend4);

	_amem4(pImgPtr) = _avgu4(tmpend4,input4);
}



#endif
