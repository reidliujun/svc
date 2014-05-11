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





/**
This function allows to get the luminance prediction of a non IDR picture when xFracl = 1 and yFracl = 1.


@param image Table of current frame.
@param refPicLXl Table of the reference decoded picture buffer.
@param PicWidthSamples Stride of the reference buffer.
@param stride Stride of the current image.
*/
void luma_sample_interp_1_1_TI(unsigned char image [RESTRICT],	unsigned char refPicLXl[RESTRICT],	const short PicWidthSamples	, const short stride){
	



	unsigned int l1,l2,l3,l4;
	unsigned int A1,A2,A3,A4;

	{
		unsigned char* pRefImgPtr;
		unsigned int A,B,C;
		unsigned int tmpl1,tmpl2,tmpl3,tmpl4,tmpm1,tmpm2,tmpm3,tmpm4,tmpr4;
		unsigned int tmpl12,tmpl34,tmpm12,tmpm34,tmp12,tmp34;
		unsigned int l1_1,l1_2,l2_1,l2_2,l3_1,l3_2,l4_1,l4_2;

		// first line
		pRefImgPtr = refPicLXl-2;

		A = _mem4(pRefImgPtr);
		B = _mem4(pRefImgPtr+4);
		C = _mem4(pRefImgPtr+8);

		A1 = _packlh2(B,A);


		// values are all worked on swapped
		tmpl1 = _dotpsu4(0x1414FB01,A); // Mpy by coeffs 20 20 -5 1
		tmpl2 = _dotpsu4(0x14FB0100,A); // Mpy by coeffs 20 -5 1 0
		tmpl3 = _dotpsu4(0xFB010000,A); // Mpy by coeffs -5 1 0 0
		tmpl4 = _extu(A,0,24);//_dotpsu4(0x01000000,A);// Mpy by coeffs 1 0 0 0
		tmpm1 = _dotpsu4(0x000001FB,B); // Mpy by coeffs 0 0 1 -5
		tmpm2 = _dotpsu4(0x0001FB14,B); // Mpy by coeffs 0 1 -5 20
		tmpm3 = _dotpsu4(0x01FB1414,B); // Mpy by coeffs 1 -5 20 20
		tmpm4 = _dotpsu4(0xFB1414FB,B); // Mpy by coeffs -5 20 20 -5
		tmpr4 = _extu(C,24,24);//_dotpsu4(0x00000001,C);// Mpy by coeffs 0 0 0 1

		// coeffs mpy results are packed in 16 bits half registers
		tmpl12 = _spack2(tmpl1,tmpl2);
		tmpl34 = _spack2(tmpl3,tmpl4);
		tmpm12 = _spack2(tmpm1,tmpm2);
		tmpm34 = _spack2(tmpm3,tmpm4);

		tmp12 = _sadd2(tmpl12,tmpm12);
		tmp34 = _sadd2(tmpl34,tmpm34);
		tmp34 = _sadd2(tmp34,tmpr4);

		l1_1 = _shr2(_sadd2(tmp12,0x00100010),5);
		l1_2 =  _shr2(_sadd2(tmp34,0x00100010),5);
		l1 = _spacku4(l1_2,l1_1);
		l1 = _swap4(l1);

		// second line
		pRefImgPtr+=PicWidthSamples;

		A = _mem4(pRefImgPtr);
		B = _mem4(pRefImgPtr+4);
		C = _mem4(pRefImgPtr+8);

		A2 = _packlh2(B,A);


		tmpl1 = _dotpsu4(0x1414FB01,A);
		tmpl2 = _dotpsu4(0x14FB0100,A);
		tmpl3 = _dotpsu4(0xFB010000,A);
		tmpl4 = _extu(A,0,24);//_dotpsu4(0x01000000,A);
		tmpm1 = _dotpsu4(0x000001FB,B);
		tmpm2 = _dotpsu4(0x0001FB14,B);
		tmpm3 = _dotpsu4(0x01FB1414,B);
		tmpm4 = _dotpsu4(0xFB1414FB,B);
		tmpr4 = _extu(C,24,24);//_dotpsu4(0x00000001,C);

		tmpl12 = _spack2(tmpl1,tmpl2);
		tmpl34 = _spack2(tmpl3,tmpl4);
		tmpm12 = _spack2(tmpm1,tmpm2);
		tmpm34 = _spack2(tmpm3,tmpm4);

		tmp12 = _sadd2(tmpl12,tmpm12);
		tmp34 = _sadd2(tmpl34,tmpm34);
		tmp34 = _sadd2(tmp34,tmpr4);

		l2_1 = _shr2(_sadd2(tmp12,0x00100010),5);
		l2_2 = _shr2(_sadd2(tmp34,0x00100010),5);
		l2 = _spacku4(l2_2,l2_1);
		l2 = _swap4(l2);

		// third line
		pRefImgPtr+=PicWidthSamples;

		A = _mem4(pRefImgPtr);
		B = _mem4(pRefImgPtr+4);
		C = _mem4(pRefImgPtr+8);

		A3 = _packlh2(B,A);


		tmpl1 = _dotpsu4(0x1414FB01,A);
		tmpl2 = _dotpsu4(0x14FB0100,A);
		tmpl3 = _dotpsu4(0xFB010000,A);
		tmpl4 = _extu(A,0,24);//_dotpsu4(0x01000000,A);
		tmpm1 = _dotpsu4(0x000001FB,B);
		tmpm2 = _dotpsu4(0x0001FB14,B);
		tmpm3 = _dotpsu4(0x01FB1414,B);
		tmpm4 = _dotpsu4(0xFB1414FB,B);
		tmpr4 = _extu(C,24,24);//_dotpsu4(0x00000001,C);

		tmpl12 = _spack2(tmpl1,tmpl2);
		tmpl34 = _spack2(tmpl3,tmpl4);
		tmpm12 = _spack2(tmpm1,tmpm2);
		tmpm34 = _spack2(tmpm3,tmpm4);

		tmp12 = _sadd2(tmpl12,tmpm12);
		tmp34 = _sadd2(tmpl34,tmpm34);
		tmp34 = _sadd2(tmp34,tmpr4);

		l3_1 = _shr2(_sadd2(tmp12,0x00100010),5);
		l3_2 = _shr2(_sadd2(tmp34,0x00100010),5);
		l3 = _spacku4(l3_2,l3_1);
		l3 = _swap4(l3);

		// fourth line
		pRefImgPtr+=PicWidthSamples;

		A = _mem4(pRefImgPtr);
		B = _mem4(pRefImgPtr+4);
		C = _mem4(pRefImgPtr+8);

		A4 = _packlh2(B,A);

		tmpl1 = _dotpsu4(0x1414FB01,A);
		tmpl2 = _dotpsu4(0x14FB0100,A);
		tmpl3 = _dotpsu4(0xFB010000,A);
		tmpl4 = _extu(A,0,24);//_dotpsu4(0x01000000,A);
		tmpm1 = _dotpsu4(0x000001FB,B);
		tmpm2 = _dotpsu4(0x0001FB14,B);
		tmpm3 = _dotpsu4(0x01FB1414,B);
		tmpm4 = _dotpsu4(0xFB1414FB,B);
		tmpr4 = _extu(C,24,24);//_dotpsu4(0x00000001,C);

		tmpl12 = _spack2(tmpl1,tmpl2);
		tmpl34 = _spack2(tmpl3,tmpl4);
		tmpm12 = _spack2(tmpm1,tmpm2);
		tmpm34 = _spack2(tmpm3,tmpm4);

		tmp12 = _sadd2(tmpl12,tmpm12);
		tmp34 = _sadd2(tmpl34,tmpm34);
		tmp34 = _sadd2(tmp34,tmpr4);

		l4_1 = _shr2(_sadd2(tmp12,0x00100010),5);
		l4_2 = _shr2(_sadd2(tmp34,0x00100010),5);
		l4 = _spacku4(l4_2,l4_1);
		l4 = _swap4(l4);
	}

	{
		unsigned int uiLine1,uiLine2,uiLine3,uiLine4,uiLine5,uiLine6,uiLine7,uiLine8,uiLine9;
		unsigned int uiTmpLine12_h,uiTmpLine34_h,uiTmpLine12_l,uiTmpLine34_l,uiTmpLine1234_4,uiTmpLine1234_2,uiTmpLine1234_3,uiTmpLine1234_1;
		unsigned int uiTmpLine56_h,uiTmpLine78_h,uiTmpLine56_l,uiTmpLine78_l,uiTmpLine5678_4,uiTmpLine5678_2,uiTmpLine5678_3,uiTmpLine5678_1;
		unsigned int tmpc1_1,tmpc1_2,tmpc2_1,tmpc2_2,tmpc1,tmpc2;
		unsigned int tmpc3_1,tmpc3_2,tmpc4_1,tmpc4_2,tmpc3,tmpc4;
		unsigned int tmp12,tmp34;
		unsigned int tmpl9l,tmpl9h;
		unsigned char* pRefImgPtr;
		unsigned char* pImgPtr;
		unsigned int l1p,l2p,l3p,l4p;

		pRefImgPtr = refPicLXl - 2 * PicWidthSamples;

		uiLine1 = _mem4(pRefImgPtr);
		pRefImgPtr += PicWidthSamples;
		uiLine2 = _mem4(pRefImgPtr);

	
			uiLine3 = A1;
			uiLine4 = A2;
			uiLine5 = A3;
			uiLine6 = A4;
			pRefImgPtr += 5*PicWidthSamples;
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

		l1p = _spacku4(tmp34,tmp12);
		l1p = _swap4(l1p);

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

		l2p = _spacku4(tmp34,tmp12);
		l2p = _swap4(l2p);

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

		l3p = _spacku4(tmp34,tmp12);
		l3p = _swap4(l3p);

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

		l4p = _spacku4(tmp34,tmp12);
		l4p = _swap4(l4p);

		pImgPtr = image;
		_amem4(pImgPtr) = _avgu4(l1,l1p);
		pImgPtr += stride;
		_amem4(pImgPtr) = _avgu4(l2,l2p);
		pImgPtr += stride;
		_amem4(pImgPtr) = _avgu4(l3,l3p);
		pImgPtr += stride;
		_amem4(pImgPtr) = _avgu4(l4,l4p);
		pImgPtr += stride;
	}
}






/**
This function allows to get the luminance prediction of a non IDR picture when xFracl = 1 and yFracl = 3.


@param image Table of current frame.
@param refPicLXl Table of the reference decoded picture buffer.
@param PicWidthSamples Stride of the reference buffer.
@param stride Stride of the current image.
*/
void luma_sample_interp_1_3_TI(unsigned char image [RESTRICT],	unsigned char refPicLXl[RESTRICT],	const short PicWidthSamples, const short stride){

	

	unsigned int l1,l2,l3,l4;
	unsigned int A1,A2,A3,A4;

	{
		unsigned char* pRefImgPtr;
		unsigned int A,B,C;
		unsigned int tmpl1,tmpl2,tmpl3,tmpl4,tmpm1,tmpm2,tmpm3,tmpm4,tmpr4;
		unsigned int tmpl12,tmpl34,tmpm12,tmpm34,tmp12,tmp34;
		unsigned int l1_1,l1_2,l2_1,l2_2,l3_1,l3_2,l4_1,l4_2;

		// first line
		pRefImgPtr = refPicLXl+ PicWidthSamples -2;

		A = _mem4(pRefImgPtr);
		B = _mem4(pRefImgPtr+4);
		C = _mem4(pRefImgPtr+8);

		A1 = _packlh2(B,A);


		// values are all worked on swapped
		tmpl1 = _dotpsu4(0x1414FB01,A); // Mpy by coeffs 20 20 -5 1
		tmpl2 = _dotpsu4(0x14FB0100,A); // Mpy by coeffs 20 -5 1 0
		tmpl3 = _dotpsu4(0xFB010000,A); // Mpy by coeffs -5 1 0 0
		tmpl4 = _extu(A,0,24);//_dotpsu4(0x01000000,A);// Mpy by coeffs 1 0 0 0
		tmpm1 = _dotpsu4(0x000001FB,B); // Mpy by coeffs 0 0 1 -5
		tmpm2 = _dotpsu4(0x0001FB14,B); // Mpy by coeffs 0 1 -5 20
		tmpm3 = _dotpsu4(0x01FB1414,B); // Mpy by coeffs 1 -5 20 20
		tmpm4 = _dotpsu4(0xFB1414FB,B); // Mpy by coeffs -5 20 20 -5
		tmpr4 = _extu(C,24,24);//_dotpsu4(0x00000001,C);// Mpy by coeffs 0 0 0 1

		// coeffs mpy results are packed in 16 bits half registers
		tmpl12 = _spack2(tmpl1,tmpl2);
		tmpl34 = _spack2(tmpl3,tmpl4);
		tmpm12 = _spack2(tmpm1,tmpm2);
		tmpm34 = _spack2(tmpm3,tmpm4);

		tmp12 = _sadd2(tmpl12,tmpm12);
		tmp34 = _sadd2(tmpl34,tmpm34);
		tmp34 = _sadd2(tmp34,tmpr4);

		l1_1 = _shr2(_sadd2(tmp12,0x00100010),5);
		l1_2 =  _shr2(_sadd2(tmp34,0x00100010),5);
		l1 = _spacku4(l1_2,l1_1);
		l1 = _swap4(l1);

		// second line
		pRefImgPtr+=PicWidthSamples;

		A = _mem4(pRefImgPtr);
		B = _mem4(pRefImgPtr+4);
		C = _mem4(pRefImgPtr+8);

		A2 = _packlh2(B,A);


		tmpl1 = _dotpsu4(0x1414FB01,A);
		tmpl2 = _dotpsu4(0x14FB0100,A);
		tmpl3 = _dotpsu4(0xFB010000,A);
		tmpl4 = _extu(A,0,24);//_dotpsu4(0x01000000,A);
		tmpm1 = _dotpsu4(0x000001FB,B);
		tmpm2 = _dotpsu4(0x0001FB14,B);
		tmpm3 = _dotpsu4(0x01FB1414,B);
		tmpm4 = _dotpsu4(0xFB1414FB,B);
		tmpr4 = _extu(C,24,24);//_dotpsu4(0x00000001,C);

		tmpl12 = _spack2(tmpl1,tmpl2);
		tmpl34 = _spack2(tmpl3,tmpl4);
		tmpm12 = _spack2(tmpm1,tmpm2);
		tmpm34 = _spack2(tmpm3,tmpm4);

		tmp12 = _sadd2(tmpl12,tmpm12);
		tmp34 = _sadd2(tmpl34,tmpm34);
		tmp34 = _sadd2(tmp34,tmpr4);

		l2_1 = _shr2(_sadd2(tmp12,0x00100010),5);
		l2_2 = _shr2(_sadd2(tmp34,0x00100010),5);
		l2 = _spacku4(l2_2,l2_1);
		l2 = _swap4(l2);

		// third line
		pRefImgPtr+=PicWidthSamples;

		A = _mem4(pRefImgPtr);
		B = _mem4(pRefImgPtr+4);
		C = _mem4(pRefImgPtr+8);

		A3 = _packlh2(B,A);


		tmpl1 = _dotpsu4(0x1414FB01,A);
		tmpl2 = _dotpsu4(0x14FB0100,A);
		tmpl3 = _dotpsu4(0xFB010000,A);
		tmpl4 = _extu(A,0,24);//_dotpsu4(0x01000000,A);
		tmpm1 = _dotpsu4(0x000001FB,B);
		tmpm2 = _dotpsu4(0x0001FB14,B);
		tmpm3 = _dotpsu4(0x01FB1414,B);
		tmpm4 = _dotpsu4(0xFB1414FB,B);
		tmpr4 = _extu(C,24,24);//_dotpsu4(0x00000001,C);

		tmpl12 = _spack2(tmpl1,tmpl2);
		tmpl34 = _spack2(tmpl3,tmpl4);
		tmpm12 = _spack2(tmpm1,tmpm2);
		tmpm34 = _spack2(tmpm3,tmpm4);

		tmp12 = _sadd2(tmpl12,tmpm12);
		tmp34 = _sadd2(tmpl34,tmpm34);
		tmp34 = _sadd2(tmp34,tmpr4);

		l3_1 = _shr2(_sadd2(tmp12,0x00100010),5);
		l3_2 = _shr2(_sadd2(tmp34,0x00100010),5);
		l3 = _spacku4(l3_2,l3_1);
		l3 = _swap4(l3);

		// fourth line
		pRefImgPtr+=PicWidthSamples;

		A = _mem4(pRefImgPtr);
		B = _mem4(pRefImgPtr+4);
		C = _mem4(pRefImgPtr+8);

		A4 = _packlh2(B,A);


		tmpl1 = _dotpsu4(0x1414FB01,A);
		tmpl2 = _dotpsu4(0x14FB0100,A);
		tmpl3 = _dotpsu4(0xFB010000,A);
		tmpl4 = _extu(A,0,24);//_dotpsu4(0x01000000,A);
		tmpm1 = _dotpsu4(0x000001FB,B);
		tmpm2 = _dotpsu4(0x0001FB14,B);
		tmpm3 = _dotpsu4(0x01FB1414,B);
		tmpm4 = _dotpsu4(0xFB1414FB,B);
		tmpr4 = _extu(C,24,24);//_dotpsu4(0x00000001,C);

		tmpl12 = _spack2(tmpl1,tmpl2);
		tmpl34 = _spack2(tmpl3,tmpl4);
		tmpm12 = _spack2(tmpm1,tmpm2);
		tmpm34 = _spack2(tmpm3,tmpm4);

		tmp12 = _sadd2(tmpl12,tmpm12);
		tmp34 = _sadd2(tmpl34,tmpm34);
		tmp34 = _sadd2(tmp34,tmpr4);

		l4_1 = _shr2(_sadd2(tmp12,0x00100010),5);
		l4_2 = _shr2(_sadd2(tmp34,0x00100010),5);
		l4 = _spacku4(l4_2,l4_1);
		l4 = _swap4(l4);
	}

	{
		unsigned int uiLine1,uiLine2,uiLine3,uiLine4,uiLine5,uiLine6,uiLine7,uiLine8,uiLine9;
		unsigned int uiTmpLine12_h,uiTmpLine34_h,uiTmpLine12_l,uiTmpLine34_l,uiTmpLine1234_4,uiTmpLine1234_2,uiTmpLine1234_3,uiTmpLine1234_1;
		unsigned int uiTmpLine56_h,uiTmpLine78_h,uiTmpLine56_l,uiTmpLine78_l,uiTmpLine5678_4,uiTmpLine5678_2,uiTmpLine5678_3,uiTmpLine5678_1;
		unsigned int tmpc1_1,tmpc1_2,tmpc2_1,tmpc2_2,tmpc1,tmpc2;
		unsigned int tmpc3_1,tmpc3_2,tmpc4_1,tmpc4_2,tmpc3,tmpc4;
		unsigned int tmp12,tmp34;
		unsigned int tmpl9l,tmpl9h;
		unsigned char* pRefImgPtr;
		unsigned char* pImgPtr;
		unsigned int l1p,l2p,l3p,l4p;

		pRefImgPtr = refPicLXl - 2 * PicWidthSamples;

		uiLine1 = _mem4(pRefImgPtr);
		pRefImgPtr += PicWidthSamples;
		uiLine2 = _mem4(pRefImgPtr);

	
			pRefImgPtr += PicWidthSamples;
			uiLine3 = _mem4(pRefImgPtr);
			uiLine4 = A1;
			uiLine5 = A2;
			uiLine6 = A3;
			uiLine7 = A4;
			pRefImgPtr += 5*PicWidthSamples;


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

		l1p = _spacku4(tmp34,tmp12);
		l1p = _swap4(l1p);

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

		l2p = _spacku4(tmp34,tmp12);
		l2p = _swap4(l2p);

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

		l3p = _spacku4(tmp34,tmp12);
		l3p = _swap4(l3p);

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

		l4p = _spacku4(tmp34,tmp12);
		l4p = _swap4(l4p);

		pImgPtr = image;
		_amem4(pImgPtr) = _avgu4(l1,l1p);
		pImgPtr += stride;
		_amem4(pImgPtr) = _avgu4(l2,l2p);
		pImgPtr += stride;
		_amem4(pImgPtr) = _avgu4(l3,l3p);
		pImgPtr += stride;
		_amem4(pImgPtr) = _avgu4(l4,l4p);
		pImgPtr += stride;
	}
}





/**
This function allows to get the luminance prediction of a non IDR picture when xFracl = 3 and yFracl = 1.


@param image Table of current frame.
@param refPicLXl Table of the reference decoded picture buffer.
@param PicWidthSamples Stride of the reference buffer.
@param stride Stride of the current image.
*/
void luma_sample_interp_3_1_TI(unsigned char image [RESTRICT],	unsigned char refPicLXl[RESTRICT],	const short PicWidthSamples, const short stride){

	


	unsigned int l1,l2,l3,l4;
	unsigned int A1,A2,A3,A4;

	{
		unsigned char* pRefImgPtr;
		unsigned int A,B,C;
		unsigned int tmpl1,tmpl2,tmpl3,tmpl4,tmpm1,tmpm2,tmpm3,tmpm4,tmpr4;
		unsigned int tmpl12,tmpl34,tmpm12,tmpm34,tmp12,tmp34;
		unsigned int l1_1,l1_2,l2_1,l2_2,l3_1,l3_2,l4_1,l4_2;

		// first line
		pRefImgPtr = refPicLXl - 2;

		A = _mem4(pRefImgPtr);
		B = _mem4(pRefImgPtr+4);
		C = _mem4(pRefImgPtr+8);

		A1 = _extu(B,8,0) + _extu(A,0,24);

		// values are all worked on swapped
		tmpl1 = _dotpsu4(0x1414FB01,A); // Mpy by coeffs 20 20 -5 1
		tmpl2 = _dotpsu4(0x14FB0100,A); // Mpy by coeffs 20 -5 1 0
		tmpl3 = _dotpsu4(0xFB010000,A); // Mpy by coeffs -5 1 0 0
		tmpl4 = _extu(A,0,24);//_dotpsu4(0x01000000,A);// Mpy by coeffs 1 0 0 0
		tmpm1 = _dotpsu4(0x000001FB,B); // Mpy by coeffs 0 0 1 -5
		tmpm2 = _dotpsu4(0x0001FB14,B); // Mpy by coeffs 0 1 -5 20
		tmpm3 = _dotpsu4(0x01FB1414,B); // Mpy by coeffs 1 -5 20 20
		tmpm4 = _dotpsu4(0xFB1414FB,B); // Mpy by coeffs -5 20 20 -5
		tmpr4 = _extu(C,24,24);//_dotpsu4(0x00000001,C);// Mpy by coeffs 0 0 0 1

		// coeffs mpy results are packed in 16 bits half registers
		tmpl12 = _spack2(tmpl1,tmpl2);
		tmpl34 = _spack2(tmpl3,tmpl4);
		tmpm12 = _spack2(tmpm1,tmpm2);
		tmpm34 = _spack2(tmpm3,tmpm4);

		tmp12 = _sadd2(tmpl12,tmpm12);
		tmp34 = _sadd2(tmpl34,tmpm34);
		tmp34 = _sadd2(tmp34,tmpr4);

		l1_1 = _shr2(_sadd2(tmp12,0x00100010),5);
		l1_2 =  _shr2(_sadd2(tmp34,0x00100010),5);
		l1 = _spacku4(l1_2,l1_1);
		l1 = _swap4(l1);

		// second line
		pRefImgPtr+=PicWidthSamples;

		A = _mem4(pRefImgPtr);
		B = _mem4(pRefImgPtr+4);
		C = _mem4(pRefImgPtr+8);

		A2 = _extu(B,8,0) + _extu(A,0,24);

		tmpl1 = _dotpsu4(0x1414FB01,A);
		tmpl2 = _dotpsu4(0x14FB0100,A);
		tmpl3 = _dotpsu4(0xFB010000,A);
		tmpl4 = _extu(A,0,24);//_dotpsu4(0x01000000,A);
		tmpm1 = _dotpsu4(0x000001FB,B);
		tmpm2 = _dotpsu4(0x0001FB14,B);
		tmpm3 = _dotpsu4(0x01FB1414,B);
		tmpm4 = _dotpsu4(0xFB1414FB,B);
		tmpr4 = _extu(C,24,24);//_dotpsu4(0x00000001,C);

		tmpl12 = _spack2(tmpl1,tmpl2);
		tmpl34 = _spack2(tmpl3,tmpl4);
		tmpm12 = _spack2(tmpm1,tmpm2);
		tmpm34 = _spack2(tmpm3,tmpm4);

		tmp12 = _sadd2(tmpl12,tmpm12);
		tmp34 = _sadd2(tmpl34,tmpm34);
		tmp34 = _sadd2(tmp34,tmpr4);

		l2_1 = _shr2(_sadd2(tmp12,0x00100010),5);
		l2_2 = _shr2(_sadd2(tmp34,0x00100010),5);
		l2 = _spacku4(l2_2,l2_1);
		l2 = _swap4(l2);

		// third line
		pRefImgPtr+=PicWidthSamples;

		A = _mem4(pRefImgPtr);
		B = _mem4(pRefImgPtr+4);
		C = _mem4(pRefImgPtr+8);

		A3 = _extu(B,8,0) + _extu(A,0,24);

		tmpl1 = _dotpsu4(0x1414FB01,A);
		tmpl2 = _dotpsu4(0x14FB0100,A);
		tmpl3 = _dotpsu4(0xFB010000,A);
		tmpl4 = _extu(A,0,24);//_dotpsu4(0x01000000,A);
		tmpm1 = _dotpsu4(0x000001FB,B);
		tmpm2 = _dotpsu4(0x0001FB14,B);
		tmpm3 = _dotpsu4(0x01FB1414,B);
		tmpm4 = _dotpsu4(0xFB1414FB,B);
		tmpr4 = _extu(C,24,24);//_dotpsu4(0x00000001,C);

		tmpl12 = _spack2(tmpl1,tmpl2);
		tmpl34 = _spack2(tmpl3,tmpl4);
		tmpm12 = _spack2(tmpm1,tmpm2);
		tmpm34 = _spack2(tmpm3,tmpm4);

		tmp12 = _sadd2(tmpl12,tmpm12);
		tmp34 = _sadd2(tmpl34,tmpm34);
		tmp34 = _sadd2(tmp34,tmpr4);

		l3_1 = _shr2(_sadd2(tmp12,0x00100010),5);
		l3_2 = _shr2(_sadd2(tmp34,0x00100010),5);
		l3 = _spacku4(l3_2,l3_1);
		l3 = _swap4(l3);

		// fourth line
		pRefImgPtr+=PicWidthSamples;

		A = _mem4(pRefImgPtr);
		B = _mem4(pRefImgPtr+4);
		C = _mem4(pRefImgPtr+8);

		A4 = _extu(B,8,0) + _extu(A,0,24);

		tmpl1 = _dotpsu4(0x1414FB01,A);
		tmpl2 = _dotpsu4(0x14FB0100,A);
		tmpl3 = _dotpsu4(0xFB010000,A);
		tmpl4 = _extu(A,0,24);//_dotpsu4(0x01000000,A);
		tmpm1 = _dotpsu4(0x000001FB,B);
		tmpm2 = _dotpsu4(0x0001FB14,B);
		tmpm3 = _dotpsu4(0x01FB1414,B);
		tmpm4 = _dotpsu4(0xFB1414FB,B);
		tmpr4 = _extu(C,24,24);//_dotpsu4(0x00000001,C);

		tmpl12 = _spack2(tmpl1,tmpl2);
		tmpl34 = _spack2(tmpl3,tmpl4);
		tmpm12 = _spack2(tmpm1,tmpm2);
		tmpm34 = _spack2(tmpm3,tmpm4);

		tmp12 = _sadd2(tmpl12,tmpm12);
		tmp34 = _sadd2(tmpl34,tmpm34);
		tmp34 = _sadd2(tmp34,tmpr4);

		l4_1 = _shr2(_sadd2(tmp12,0x00100010),5);
		l4_2 = _shr2(_sadd2(tmp34,0x00100010),5);
		l4 = _spacku4(l4_2,l4_1);
		l4 = _swap4(l4);
	}

	{
		unsigned int uiLine1,uiLine2,uiLine3,uiLine4,uiLine5,uiLine6,uiLine7,uiLine8,uiLine9;
		unsigned int uiTmpLine12_h,uiTmpLine34_h,uiTmpLine12_l,uiTmpLine34_l,uiTmpLine1234_4,uiTmpLine1234_2,uiTmpLine1234_3,uiTmpLine1234_1;
		unsigned int uiTmpLine56_h,uiTmpLine78_h,uiTmpLine56_l,uiTmpLine78_l,uiTmpLine5678_4,uiTmpLine5678_2,uiTmpLine5678_3,uiTmpLine5678_1;
		unsigned int tmpc1_1,tmpc1_2,tmpc2_1,tmpc2_2,tmpc1,tmpc2;
		unsigned int tmpc3_1,tmpc3_2,tmpc4_1,tmpc4_2,tmpc3,tmpc4;
		unsigned int tmp12,tmp34;
		unsigned int tmpl9l,tmpl9h;
		unsigned char* pRefImgPtr;
		unsigned char* pImgPtr;
		unsigned int l1p,l2p,l3p,l4p;

		pRefImgPtr = refPicLXl+ 1 - 2 * PicWidthSamples;

		uiLine1 = _mem4(pRefImgPtr);
		pRefImgPtr += PicWidthSamples;
		uiLine2 = _mem4(pRefImgPtr);


			uiLine3 = A1;
			uiLine4 = A2;
			uiLine5 = A3;
			uiLine6 = A4;
			pRefImgPtr += 5*PicWidthSamples;
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

		l1p = _spacku4(tmp34,tmp12);
		l1p = _swap4(l1p);

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

		l2p = _spacku4(tmp34,tmp12);
		l2p = _swap4(l2p);

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

		l3p = _spacku4(tmp34,tmp12);
		l3p = _swap4(l3p);

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

		l4p = _spacku4(tmp34,tmp12);
		l4p = _swap4(l4p);

		pImgPtr = image;
		_amem4(pImgPtr) = _avgu4(l1,l1p);
		pImgPtr += stride;
		_amem4(pImgPtr) = _avgu4(l2,l2p);
		pImgPtr += stride;
		_amem4(pImgPtr) = _avgu4(l3,l3p);
		pImgPtr += stride;
		_amem4(pImgPtr) = _avgu4(l4,l4p);
		pImgPtr += stride;
	}

}





/**
This function allows to get the luminance prediction of a non IDR picture when xFracl = 3 and yFracl = 3.


@param image Table of current frame.
@param refPicLXl Table of the reference decoded picture buffer.
@param PicWidthSamples Stride of the reference buffer.
@param stride Stride of the current image.
*/
void luma_sample_interp_3_3_TI(unsigned char image [RESTRICT],	unsigned char refPicLXl[RESTRICT],	const short PicWidthSamples, const short stride){

	

	unsigned int l1,l2,l3,l4;
	unsigned int A1,A2,A3,A4;

	{
		unsigned char* pRefImgPtr;
		unsigned int A,B,C;
		unsigned int tmpl1,tmpl2,tmpl3,tmpl4,tmpm1,tmpm2,tmpm3,tmpm4,tmpr4;
		unsigned int tmpl12,tmpl34,tmpm12,tmpm34,tmp12,tmp34;
		unsigned int l1_1,l1_2,l2_1,l2_2,l3_1,l3_2,l4_1,l4_2;

		// first line
		pRefImgPtr = refPicLXl+ PicWidthSamples -2;

		A = _mem4(pRefImgPtr);
		B = _mem4(pRefImgPtr+4);
		C = _mem4(pRefImgPtr+8);

		A1 = _extu(B,8,0) + _extu(A,0,24);

		// values are all worked on swapped
		tmpl1 = _dotpsu4(0x1414FB01,A); // Mpy by coeffs 20 20 -5 1
		tmpl2 = _dotpsu4(0x14FB0100,A); // Mpy by coeffs 20 -5 1 0
		tmpl3 = _dotpsu4(0xFB010000,A); // Mpy by coeffs -5 1 0 0
		tmpl4 = _extu(A,0,24);//_dotpsu4(0x01000000,A);// Mpy by coeffs 1 0 0 0
		tmpm1 = _dotpsu4(0x000001FB,B); // Mpy by coeffs 0 0 1 -5
		tmpm2 = _dotpsu4(0x0001FB14,B); // Mpy by coeffs 0 1 -5 20
		tmpm3 = _dotpsu4(0x01FB1414,B); // Mpy by coeffs 1 -5 20 20
		tmpm4 = _dotpsu4(0xFB1414FB,B); // Mpy by coeffs -5 20 20 -5
		tmpr4 = _extu(C,24,24);//_dotpsu4(0x00000001,C);// Mpy by coeffs 0 0 0 1

		// coeffs mpy results are packed in 16 bits half registers
		tmpl12 = _spack2(tmpl1,tmpl2);
		tmpl34 = _spack2(tmpl3,tmpl4);
		tmpm12 = _spack2(tmpm1,tmpm2);
		tmpm34 = _spack2(tmpm3,tmpm4);

		tmp12 = _sadd2(tmpl12,tmpm12);
		tmp34 = _sadd2(tmpl34,tmpm34);
		tmp34 = _sadd2(tmp34,tmpr4);

		l1_1 = _shr2(_sadd2(tmp12,0x00100010),5);
		l1_2 =  _shr2(_sadd2(tmp34,0x00100010),5);
		l1 = _spacku4(l1_2,l1_1);
		l1 = _swap4(l1);

		// second line
		pRefImgPtr+=PicWidthSamples;

		A = _mem4(pRefImgPtr);
		B = _mem4(pRefImgPtr+4);
		C = _mem4(pRefImgPtr+8);

		A2 = _extu(B,8,0) + _extu(A,0,24);

		tmpl1 = _dotpsu4(0x1414FB01,A);
		tmpl2 = _dotpsu4(0x14FB0100,A);
		tmpl3 = _dotpsu4(0xFB010000,A);
		tmpl4 = _extu(A,0,24);//_dotpsu4(0x01000000,A);
		tmpm1 = _dotpsu4(0x000001FB,B);
		tmpm2 = _dotpsu4(0x0001FB14,B);
		tmpm3 = _dotpsu4(0x01FB1414,B);
		tmpm4 = _dotpsu4(0xFB1414FB,B);
		tmpr4 = _extu(C,24,24);//_dotpsu4(0x00000001,C);

		tmpl12 = _spack2(tmpl1,tmpl2);
		tmpl34 = _spack2(tmpl3,tmpl4);
		tmpm12 = _spack2(tmpm1,tmpm2);
		tmpm34 = _spack2(tmpm3,tmpm4);

		tmp12 = _sadd2(tmpl12,tmpm12);
		tmp34 = _sadd2(tmpl34,tmpm34);
		tmp34 = _sadd2(tmp34,tmpr4);

		l2_1 = _shr2(_sadd2(tmp12,0x00100010),5);
		l2_2 = _shr2(_sadd2(tmp34,0x00100010),5);
		l2 = _spacku4(l2_2,l2_1);
		l2 = _swap4(l2);

		// third line
		pRefImgPtr+=PicWidthSamples;

		A = _mem4(pRefImgPtr);
		B = _mem4(pRefImgPtr+4);
		C = _mem4(pRefImgPtr+8);

		A3 = _extu(B,8,0) + _extu(A,0,24);

		tmpl1 = _dotpsu4(0x1414FB01,A);
		tmpl2 = _dotpsu4(0x14FB0100,A);
		tmpl3 = _dotpsu4(0xFB010000,A);
		tmpl4 = _extu(A,0,24);//_dotpsu4(0x01000000,A);
		tmpm1 = _dotpsu4(0x000001FB,B);
		tmpm2 = _dotpsu4(0x0001FB14,B);
		tmpm3 = _dotpsu4(0x01FB1414,B);
		tmpm4 = _dotpsu4(0xFB1414FB,B);
		tmpr4 = _extu(C,24,24);//_dotpsu4(0x00000001,C);

		tmpl12 = _spack2(tmpl1,tmpl2);
		tmpl34 = _spack2(tmpl3,tmpl4);
		tmpm12 = _spack2(tmpm1,tmpm2);
		tmpm34 = _spack2(tmpm3,tmpm4);

		tmp12 = _sadd2(tmpl12,tmpm12);
		tmp34 = _sadd2(tmpl34,tmpm34);
		tmp34 = _sadd2(tmp34,tmpr4);

		l3_1 = _shr2(_sadd2(tmp12,0x00100010),5);
		l3_2 = _shr2(_sadd2(tmp34,0x00100010),5);
		l3 = _spacku4(l3_2,l3_1);
		l3 = _swap4(l3);

		// fourth line
		pRefImgPtr+=PicWidthSamples;

		A = _mem4(pRefImgPtr);
		B = _mem4(pRefImgPtr+4);
		C = _mem4(pRefImgPtr+8);

		A4 = _extu(B,8,0) + _extu(A,0,24);

		tmpl1 = _dotpsu4(0x1414FB01,A);
		tmpl2 = _dotpsu4(0x14FB0100,A);
		tmpl3 = _dotpsu4(0xFB010000,A);
		tmpl4 = _extu(A,0,24);//_dotpsu4(0x01000000,A);
		tmpm1 = _dotpsu4(0x000001FB,B);
		tmpm2 = _dotpsu4(0x0001FB14,B);
		tmpm3 = _dotpsu4(0x01FB1414,B);
		tmpm4 = _dotpsu4(0xFB1414FB,B);
		tmpr4 = _extu(C,24,24);//_dotpsu4(0x00000001,C);

		tmpl12 = _spack2(tmpl1,tmpl2);
		tmpl34 = _spack2(tmpl3,tmpl4);
		tmpm12 = _spack2(tmpm1,tmpm2);
		tmpm34 = _spack2(tmpm3,tmpm4);

		tmp12 = _sadd2(tmpl12,tmpm12);
		tmp34 = _sadd2(tmpl34,tmpm34);
		tmp34 = _sadd2(tmp34,tmpr4);

		l4_1 = _shr2(_sadd2(tmp12,0x00100010),5);
		l4_2 = _shr2(_sadd2(tmp34,0x00100010),5);
		l4 = _spacku4(l4_2,l4_1);
		l4 = _swap4(l4);
	}

	{
		unsigned int uiLine1,uiLine2,uiLine3,uiLine4,uiLine5,uiLine6,uiLine7,uiLine8,uiLine9;
		unsigned int uiTmpLine12_h,uiTmpLine34_h,uiTmpLine12_l,uiTmpLine34_l,uiTmpLine1234_4,uiTmpLine1234_2,uiTmpLine1234_3,uiTmpLine1234_1;
		unsigned int uiTmpLine56_h,uiTmpLine78_h,uiTmpLine56_l,uiTmpLine78_l,uiTmpLine5678_4,uiTmpLine5678_2,uiTmpLine5678_3,uiTmpLine5678_1;
		unsigned int tmpc1_1,tmpc1_2,tmpc2_1,tmpc2_2,tmpc1,tmpc2;
		unsigned int tmpc3_1,tmpc3_2,tmpc4_1,tmpc4_2,tmpc3,tmpc4;
		unsigned int tmp12,tmp34;
		unsigned int tmpl9l,tmpl9h;
		unsigned char* pRefImgPtr;
		unsigned char* pImgPtr;
		unsigned int l1p,l2p,l3p,l4p;

		pRefImgPtr = refPicLXl+ 1 - 2 * PicWidthSamples;

		uiLine1 = _mem4(pRefImgPtr);
		pRefImgPtr += PicWidthSamples;
		uiLine2 = _mem4(pRefImgPtr);

	
			pRefImgPtr += PicWidthSamples;
			uiLine3 = _mem4(pRefImgPtr);
			uiLine4 = A1;
			uiLine5 = A2;
			uiLine6 = A3;
			uiLine7 = A4;
			pRefImgPtr += 5*PicWidthSamples;


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

		l1p = _spacku4(tmp34,tmp12);
		l1p = _swap4(l1p);

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

		l2p = _spacku4(tmp34,tmp12);
		l2p = _swap4(l2p);

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

		l3p = _spacku4(tmp34,tmp12);
		l3p = _swap4(l3p);

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

		l4p = _spacku4(tmp34,tmp12);
		l4p = _swap4(l4p);

		pImgPtr = image;
		_amem4(pImgPtr) = _avgu4(l1,l1p);
		pImgPtr += stride;
		_amem4(pImgPtr) = _avgu4(l2,l2p);
		pImgPtr += stride;
		_amem4(pImgPtr) = _avgu4(l3,l3p);
		pImgPtr += stride;
		_amem4(pImgPtr) = _avgu4(l4,l4p);
		pImgPtr += stride;
	}
}


#endif
