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
#include "interpol_n_0.h"



/**
This function allows to get the luminance prediction of a non IDR picture when xFracl = 1 and yFracl = 0.


@param image Table of current frame.
@param refPicLXl Table of the reference decoded picture buffer.
@param PicWidthSamples Stride of the reference buffer.
@param stride Stride of the current image.
*/
void luma_sample_interp_1_0_TI(unsigned char image [RESTRICT], unsigned char refPicLXl[RESTRICT],	const short PicWidthSamples, const short stride)
{


	unsigned char* pRefImgPtr;
	unsigned char* pImgPtr;
	unsigned int A,B,C,input1,input2,input3,input4;
	unsigned int tmpl1,tmpl2,tmpl3,tmpl4,tmpm1,tmpm2,tmpm3,tmpm4,tmpr4;
	unsigned int tmpl12,tmpl34,tmpm12,tmpm34,tmp12,tmp34,tmpend1,tmpend2,tmpend3,tmpend4;

	// first line
	pRefImgPtr = refPicLXl-2;
	pImgPtr = image;

	A = _mem4(pRefImgPtr);
	B = _mem4(pRefImgPtr+4);

	// We prepare the input value for half pel interfpolation at the end
	input1 = _extu(A,0,16) + _extu(B,16,0);

	C = _mem4(pRefImgPtr+8);

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

	// rounding and shifting
	tmp12 = _sadd2(tmpl12,tmpm12);
	tmp12 = _shr2(_sadd2(tmp12,0x00100010),5);
	tmp34 = _sadd2(tmpl34,tmpm34);
	tmp34 = _sadd2(tmp34,tmpr4);
	tmp34 = _shr2(_sadd2(tmp34,0x00100010),5);

	// all values are packed in one unsigned int
	tmpend1 = _spacku4(tmp34,tmp12);
	tmpend1 = _swap4(tmpend1);

	_amem4(pImgPtr) =  _avgu4(tmpend1,input1);

	// second line
	pRefImgPtr+=PicWidthSamples;
	pImgPtr+=stride;

	A = _mem4(pRefImgPtr);
	B = _mem4(pRefImgPtr+4);
	input2 = _extu(A,0,16) + _extu(B,16,0);

	C = _mem4(pRefImgPtr+8);
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
	tmp12 = _shr2(_sadd2(tmp12,0x00100010),5);
	tmp34 = _sadd2(tmpl34,tmpm34);
	tmp34 = _sadd2(tmp34,tmpr4);
	tmp34 = _shr2(_sadd2(tmp34,0x00100010),5);

	tmpend2 = _spacku4(tmp34,tmp12);
	tmpend2 = _swap4(tmpend2);

	_amem4(pImgPtr) = _avgu4(tmpend2,input2);

	// third line
	pRefImgPtr+=PicWidthSamples;
	pImgPtr+=stride;

	A = _mem4(pRefImgPtr);
	B = _mem4(pRefImgPtr+4);
	
	input3 = _extu(A,0,16) + _extu(B,16,0);

	C = _mem4(pRefImgPtr+8);
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
	tmp12 = _shr2(_sadd2(tmp12,0x00100010),5);
	tmp34 = _sadd2(tmpl34,tmpm34);
	tmp34 = _sadd2(tmp34,tmpr4);
	tmp34 = _shr2(_sadd2(tmp34,0x00100010),5);

	tmpend3 = _spacku4(tmp34,tmp12);
	tmpend3 = _swap4(tmpend3);

	_amem4(pImgPtr) = _avgu4(tmpend3,input3);

	// fourth line
	pRefImgPtr+=PicWidthSamples;
	pImgPtr+=stride;

	A = _mem4(pRefImgPtr);
	B = _mem4(pRefImgPtr+4);

	input4 = _extu(A,0,16) + _extu(B,16,0);


	C = _mem4(pRefImgPtr+8);

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
	tmp12 = _shr2(_sadd2(tmp12,0x00100010),5);
	tmp34 = _sadd2(tmpl34,tmpm34);
	tmp34 = _sadd2(tmp34,tmpr4);
	tmp34 = _shr2(_sadd2(tmp34,0x00100010),5);

	tmpend4 = _spacku4(tmp34,tmp12);
	tmpend4 = _swap4(tmpend4);

	_amem4(pImgPtr) = _avgu4(tmpend4,input4);




}

/**
This function allows to get the luminance prediction of a non IDR picture when xFracl = 2 and yFracl = 0.


@param image Table of current frame.
@param refPicLXl Table of the reference decoded picture buffer.
@param PicWidthSamples Stride of the reference buffer.
@param stride Stride of the current image.
*/
void luma_sample_interp_2_0_TI(unsigned char image [RESTRICT], unsigned char refPicLXl[RESTRICT],	const short PicWidthSamples, const short stride)
{


	unsigned char* pRefImgPtr;
	unsigned char* pImgPtr;
	unsigned int A,B,C;
	unsigned int tmpl1,tmpl2,tmpl3,tmpl4,tmpm1,tmpm2,tmpm3,tmpm4,tmpr4;
	unsigned int tmpl12,tmpl34,tmpm12,tmpm34,tmp12,tmp34,tmpend1,tmpend2,tmpend3,tmpend4;

	// first line
	pRefImgPtr = refPicLXl-2;
	pImgPtr = image;

	A = _mem4(pRefImgPtr);
	B = _mem4(pRefImgPtr+4);

	// We prepare the input value for half pel interfpolation at the end
	//input1 = _extu(A,0,16) + _extu(B,16,0);

	C = _mem4(pRefImgPtr+8);

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

	// rounding and shifting
	tmp12 = _sadd2(tmpl12,tmpm12);
	tmp12 = _shr2(_sadd2(tmp12,0x00100010),5);
	tmp34 = _sadd2(tmpl34,tmpm34);
	tmp34 = _sadd2(tmp34,tmpr4);
	tmp34 = _shr2(_sadd2(tmp34,0x00100010),5);

	// all values are packed in one unsigned int
	tmpend1 = _spacku4(tmp34,tmp12);
	tmpend1 = _swap4(tmpend1);

	_amem4(pImgPtr) = tmpend1;

	// second line
	pRefImgPtr+=PicWidthSamples;
	pImgPtr+=stride;

	A = _mem4(pRefImgPtr);
	B = _mem4(pRefImgPtr+4);
//	input2 = _extu(A,0,16) + _extu(B,16,0);
	
	C = _mem4(pRefImgPtr+8);
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
	tmp12 = _shr2(_sadd2(tmp12,0x00100010),5);
	tmp34 = _sadd2(tmpl34,tmpm34);
	tmp34 = _sadd2(tmp34,tmpr4);
	tmp34 = _shr2(_sadd2(tmp34,0x00100010),5);

	tmpend2 = _spacku4(tmp34,tmp12);
	tmpend2 = _swap4(tmpend2);

	_amem4(pImgPtr) = tmpend2;

	// third line
	pRefImgPtr+=PicWidthSamples;
	pImgPtr+=stride;

	A = _mem4(pRefImgPtr);
	B = _mem4(pRefImgPtr+4);

//	input3 = _extu(A,0,24) + _extu(B,8,0);
	C = _mem4(pRefImgPtr+8);
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
	tmp12 = _shr2(_sadd2(tmp12,0x00100010),5);
	tmp34 = _sadd2(tmpl34,tmpm34);
	tmp34 = _sadd2(tmp34,tmpr4);
	tmp34 = _shr2(_sadd2(tmp34,0x00100010),5);

	tmpend3 = _spacku4(tmp34,tmp12);
	tmpend3 = _swap4(tmpend3);

	_amem4(pImgPtr) = tmpend3;

	// fourth line
	pRefImgPtr+=PicWidthSamples;
	pImgPtr+=stride;

	A = _mem4(pRefImgPtr);
	B = _mem4(pRefImgPtr+4);
	
//	input4 = _extu(A,0,16) + _extu(B,16,0);

	C = _mem4(pRefImgPtr+8);

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
	tmp12 = _shr2(_sadd2(tmp12,0x00100010),5);
	tmp34 = _sadd2(tmpl34,tmpm34);
	tmp34 = _sadd2(tmp34,tmpr4);
	tmp34 = _shr2(_sadd2(tmp34,0x00100010),5);

	tmpend4 = _spacku4(tmp34,tmp12);
	tmpend4 = _swap4(tmpend4);

	_amem4(pImgPtr) = tmpend4;

}

/**
This function allows to get the luminance prediction of a non IDR picture when xFracl = 3 and yFracl = 0.


@param image Table of current frame.
@param refPicLXl Table of the reference decoded picture buffer.
@param PicWidthSamples Stride of the reference buffer.
@param stride Stride of the current image.
*/
void luma_sample_interp_3_0_TI(unsigned char image [RESTRICT], unsigned char refPicLXl[RESTRICT],	const short PicWidthSamples, const short stride){



	unsigned char* pRefImgPtr;
	unsigned char* pImgPtr;
	unsigned int A,B,C,input1,input2,input3,input4;
	unsigned int tmpl1,tmpl2,tmpl3,tmpl4,tmpm1,tmpm2,tmpm3,tmpm4,tmpr4;
	unsigned int tmpl12,tmpl34,tmpm12,tmpm34,tmp12,tmp34,tmpend1,tmpend2,tmpend3,tmpend4;

	// first line
	pRefImgPtr = refPicLXl-2;
	pImgPtr = image;

	A = _mem4(pRefImgPtr);
	B = _mem4(pRefImgPtr+4);

	// We prepare the input value for half pel interfpolation at the end
	input1 = _extu(A,0,24) + _extu(B,8,0);

	C = _mem4(pRefImgPtr+8);

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

	// rounding and shifting
	tmp12 = _sadd2(tmpl12,tmpm12);
	tmp12 = _shr2(_sadd2(tmp12,0x00100010),5);
	tmp34 = _sadd2(tmpl34,tmpm34);
	tmp34 = _sadd2(tmp34,tmpr4);
	tmp34 = _shr2(_sadd2(tmp34,0x00100010),5);

	// all values are packed in one unsigned int
	tmpend1 = _spacku4(tmp34,tmp12);
	tmpend1 = _swap4(tmpend1);

	_amem4(pImgPtr) =  _avgu4(tmpend1,input1);

	// second line
	pRefImgPtr+=PicWidthSamples;
	pImgPtr+=stride;

	A = _mem4(pRefImgPtr);
	B = _mem4(pRefImgPtr+4);
	input2 = _extu(A,0,24) + _extu(B,8,0);

	C = _mem4(pRefImgPtr+8);
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
	tmp12 = _shr2(_sadd2(tmp12,0x00100010),5);
	tmp34 = _sadd2(tmpl34,tmpm34);
	tmp34 = _sadd2(tmp34,tmpr4);
	tmp34 = _shr2(_sadd2(tmp34,0x00100010),5);

	tmpend2 = _spacku4(tmp34,tmp12);
	tmpend2 = _swap4(tmpend2);

	_amem4(pImgPtr) = _avgu4(tmpend2,input2);

	// third line
	pRefImgPtr+=PicWidthSamples;
	pImgPtr+=stride;

	A = _mem4(pRefImgPtr);
	B = _mem4(pRefImgPtr+4);
	
	input3 = _extu(A,0,24) + _extu(B,8,0);
	C = _mem4(pRefImgPtr+8);
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
	tmp12 = _shr2(_sadd2(tmp12,0x00100010),5);
	tmp34 = _sadd2(tmpl34,tmpm34);
	tmp34 = _sadd2(tmp34,tmpr4);
	tmp34 = _shr2(_sadd2(tmp34,0x00100010),5);

	tmpend3 = _spacku4(tmp34,tmp12);
	tmpend3 = _swap4(tmpend3);

	_amem4(pImgPtr) = _avgu4(tmpend3,input3);

	// fourth line
	pRefImgPtr+=PicWidthSamples;
	pImgPtr+=stride;

	A = _mem4(pRefImgPtr);
	B = _mem4(pRefImgPtr+4);
	
	input4 = _extu(A,0,24) + _extu(B,8,0);
	C = _mem4(pRefImgPtr+8);

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
	tmp12 = _shr2(_sadd2(tmp12,0x00100010),5);
	tmp34 = _sadd2(tmpl34,tmpm34);
	tmp34 = _sadd2(tmp34,tmpr4);
	tmp34 = _shr2(_sadd2(tmp34,0x00100010),5);

	tmpend4 = _spacku4(tmp34,tmp12);
	tmpend4 = _swap4(tmpend4);

	_amem4(pImgPtr) = _avgu4(tmpend4,input4);


}

#endif
