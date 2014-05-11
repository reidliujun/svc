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
#include "interpol_2_n.h"



/**
This function allows to get the luminance prediction of a non IDR picture when xFracl = 2 and yFracl = 1.


@param image Table of current frame.
@param refPicLXl Table of the reference decoded picture buffer.
@param PicWidthSamples Stride of the reference buffer.
@param stride Stride of the current image.
*/
void luma_sample_interp_2_1_TI(unsigned char image [RESTRICT], unsigned char refPicLXl[RESTRICT],	const short PicWidthSamples, const short stride)
{
	/* Vertical & horizontal interpolation */

	unsigned char* pImgPtr;
	unsigned char* pRefImgPtr;
	unsigned int A,B,C;
	unsigned int tmpl1,tmpl2,tmpl3,tmpl4,tmpm1,tmpm2,tmpm3,tmpm4,tmpr4;
	unsigned int tmpl12,tmpl34,tmpm12,tmpm34,tmp12,tmp34;
	unsigned int l1_1,l1_2,l2_1,l2_2,l3_1,l3_2,l4_1,l4_2,l5_1,l5_2,l6_1,l6_2,l7_1,l7_2,l8_1,l8_2,l9_1,l9_2;
	unsigned int AplF,BplE,CplD;
	int spl1mpy,spl2mpy;
	unsigned int res12_1,res34_1,res12_2,res34_2,res12_3,res34_3,res12_4,res34_4;
	unsigned int res_1,res_2,res_3,res_4;
	unsigned int input1,input2,input3,input4,input5,inputtmp;

	// first line
	pRefImgPtr = refPicLXl-((PicWidthSamples+1)<<1);

	A = _mem4(pRefImgPtr);
	B = _mem4(pRefImgPtr+4);
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

	tmp12 = _sadd2(tmpl12,tmpm12);
	tmp34 = _sadd2(tmpl34,tmpm34);
	tmp34 = _sadd2(tmp34,tmpr4);

	l1_1 = tmp12;
	l1_2 = tmp34;

	// second line
	pRefImgPtr+=PicWidthSamples;

	A = _mem4(pRefImgPtr);
	B = _mem4(pRefImgPtr+4);
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
	tmp34 = _sadd2(tmpl34,tmpm34);
	tmp34 = _sadd2(tmp34,tmpr4);

	l2_1 = tmp12;
	l2_2 = tmp34;

	// third line
	pRefImgPtr+=PicWidthSamples;

	A = _mem4(pRefImgPtr);
	B = _mem4(pRefImgPtr+4);
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
	tmp34 = _sadd2(tmpl34,tmpm34);
	tmp34 = _sadd2(tmp34,tmpr4);

	l3_1 = tmp12;
	l3_2 = tmp34;

	input1 = _shr2(_sadd2(tmp12,0x00100010),5);
	inputtmp = _shr2(_sadd2(tmp34,0x00100010),5);
	input1 = _spacku4(inputtmp,input1);
	input1 = _swap4(input1);

	// fourth line
	pRefImgPtr+=PicWidthSamples;

	A = _mem4(pRefImgPtr);
	B = _mem4(pRefImgPtr+4);
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
	tmp34 = _sadd2(tmpl34,tmpm34);
	tmp34 = _sadd2(tmp34,tmpr4);

	l4_1 = tmp12;
	l4_2 = tmp34;

	input2 = _shr2(_sadd2(tmp12,0x00100010),5);
	inputtmp = _shr2(_sadd2(tmp34,0x00100010),5);
	input2 = _spacku4(inputtmp,input2);
	input2 = _swap4(input2);

	// fifth line
	pRefImgPtr+=PicWidthSamples;

	A = _mem4(pRefImgPtr);
	B = _mem4(pRefImgPtr+4);
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
	tmp34 = _sadd2(tmpl34,tmpm34);
	tmp34 = _sadd2(tmp34,tmpr4);

	l5_1 = tmp12;
	l5_2 = tmp34;

	input3 = _shr2(_sadd2(tmp12,0x00100010),5);
	inputtmp = _shr2(_sadd2(tmp34,0x00100010),5);
	input3 = _spacku4(inputtmp,input3);
	input3 = _swap4(input3);

	// sixth line
	pRefImgPtr+=PicWidthSamples;

	A = _mem4(pRefImgPtr);
	B = _mem4(pRefImgPtr+4);
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
	tmp34 = _sadd2(tmpl34,tmpm34);
	tmp34 = _sadd2(tmp34,tmpr4);

	l6_1 = tmp12;
	l6_2 = tmp34;

	input4 = _shr2(_sadd2(tmp12,0x00100010),5);
	inputtmp = _shr2(_sadd2(tmp34,0x00100010),5);
	input4 = _spacku4(inputtmp,input4);
	input4 = _swap4(input4);

	// seventh line
	pRefImgPtr+=PicWidthSamples;

	A = _mem4(pRefImgPtr);
	B = _mem4(pRefImgPtr+4);
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
	tmp34 = _sadd2(tmpl34,tmpm34);
	tmp34 = _sadd2(tmp34,tmpr4);

	l7_1 = tmp12;
	l7_2 = tmp34;

	input5 = _shr2(_sadd2(tmp12,0x00100010),5);
	inputtmp = _shr2(_sadd2(tmp34,0x00100010),5);
	input5 = _spacku4(inputtmp,input5);
	input5 = _swap4(input5);


	// eigth line
	pRefImgPtr+=PicWidthSamples;

	A = _mem4(pRefImgPtr);
	B = _mem4(pRefImgPtr+4);
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
	tmp34 = _sadd2(tmpl34,tmpm34);
	tmp34 = _sadd2(tmp34,tmpr4);

	l8_1 = tmp12;
	l8_2 = tmp34;

	// neigth line
	pRefImgPtr+=PicWidthSamples;

	A = _mem4(pRefImgPtr);
	B = _mem4(pRefImgPtr+4);
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
	tmp34 = _sadd2(tmpl34,tmpm34);
	tmp34 = _sadd2(tmp34,tmpr4);

	l9_1 = tmp12;
	l9_2 = tmp34;

	// The intermediary results are in li_i

	// First line
	AplF = _sadd2(l1_1,l6_1);
	AplF = _sadd2(0x02000200,AplF);
	BplE = _sadd2(l2_1,l5_1);
	CplD = _sadd2(l3_1,l4_1);

	spl1mpy = _packh2(BplE,CplD);
	spl1mpy = _dotp2(0xFFFB0014,spl1mpy);
	spl1mpy += _ext(AplF,0,16);
	spl1mpy >>= 10;

	spl2mpy = _pack2(BplE,CplD);
	spl2mpy = _dotp2(0xFFFB0014,spl2mpy);
	spl2mpy += _ext(AplF,16,16);
	spl2mpy >>= 10;

	res12_1 = _pack2(spl1mpy,spl2mpy);

	AplF = _sadd2(l1_2,l6_2);
	AplF = _sadd2(0x02000200,AplF);
	BplE = _sadd2(l2_2,l5_2);
	CplD = _sadd2(l3_2,l4_2);

	spl1mpy = _packh2(BplE,CplD);
	spl1mpy = _dotp2(0xFFFB0014,spl1mpy);
	spl1mpy += _ext(AplF,0,16);
	spl1mpy >>= 10;

	spl2mpy = _pack2(BplE,CplD);
	spl2mpy = _dotp2(0xFFFB0014,spl2mpy);
	spl2mpy += _ext(AplF,16,16);
	spl2mpy >>= 10;

	res34_1 = _pack2(spl1mpy,spl2mpy);

	res_1 = _spacku4(res34_1,res12_1);
	res_1 = _swap4(res_1);

	pImgPtr = image;
	_amem4(pImgPtr) = _avgu4(res_1,input1);

	// Second line
	AplF = _sadd2(l2_1,l7_1);
	AplF = _sadd2(0x02000200,AplF);
	BplE = _sadd2(l3_1,l6_1);
	CplD = _sadd2(l4_1,l5_1);

	spl1mpy = _packh2(BplE,CplD);
	spl1mpy = _dotp2(0xFFFB0014,spl1mpy);
	spl1mpy += _ext(AplF,0,16);
	spl1mpy >>= 10;

	spl2mpy = _pack2(BplE,CplD);
	spl2mpy = _dotp2(0xFFFB0014,spl2mpy);
	spl2mpy += _ext(AplF,16,16);
	spl2mpy >>= 10;

	res12_2 = _pack2(spl1mpy,spl2mpy);

	AplF = _sadd2(l2_2,l7_2);
	AplF = _sadd2(0x02000200,AplF);
	BplE = _sadd2(l3_2,l6_2);
	CplD = _sadd2(l4_2,l5_2);

	spl1mpy = _packh2(BplE,CplD);
	spl1mpy = _dotp2(0xFFFB0014,spl1mpy);
	spl1mpy += _ext(AplF,0,16);
	spl1mpy >>= 10;

	spl2mpy = _pack2(BplE,CplD);
	spl2mpy = _dotp2(0xFFFB0014,spl2mpy);
	spl2mpy += _ext(AplF,16,16);
	spl2mpy >>= 10;

	res34_2 = _pack2(spl1mpy,spl2mpy);


	res_2 = _spacku4(res34_2,res12_2);
	res_2 = _swap4(res_2);

	pImgPtr += stride;
	_amem4(pImgPtr) = _avgu4(res_2,input2);

	// Third line
	AplF = _sadd2(l3_1,l8_1);
	AplF = _sadd2(0x02000200,AplF);
	BplE = _sadd2(l4_1,l7_1);
	CplD = _sadd2(l5_1,l6_1);

	spl1mpy = _packh2(BplE,CplD);
	spl1mpy = _dotp2(0xFFFB0014,spl1mpy);
	spl1mpy += _ext(AplF,0,16);
	spl1mpy >>= 10;

	spl2mpy = _pack2(BplE,CplD);
	spl2mpy = _dotp2(0xFFFB0014,spl2mpy);
	spl2mpy += _ext(AplF,16,16);
	spl2mpy >>= 10;

	res12_3 = _pack2(spl1mpy,spl2mpy);

	AplF = _sadd2(l3_2,l8_2);
	AplF = _sadd2(0x02000200,AplF);
	BplE = _sadd2(l4_2,l7_2);
	CplD = _sadd2(l5_2,l6_2);

	spl1mpy = _packh2(BplE,CplD);
	spl1mpy = _dotp2(0xFFFB0014,spl1mpy);
	spl1mpy += _ext(AplF,0,16);
	spl1mpy >>= 10;

	spl2mpy = _pack2(BplE,CplD);
	spl2mpy = _dotp2(0xFFFB0014,spl2mpy);
	spl2mpy += _ext(AplF,16,16);
	spl2mpy >>= 10;

	res34_3 = _pack2(spl1mpy,spl2mpy);

	res_3 = _spacku4(res34_3,res12_3);
	res_3 = _swap4(res_3);

	pImgPtr += stride;
	_amem4(pImgPtr) = _avgu4(res_3,input3);

	// Fourth line
	AplF = _sadd2(l4_1,l9_1);
	AplF = _sadd2(0x02000200,AplF);
	BplE = _sadd2(l5_1,l8_1);
	CplD = _sadd2(l6_1,l7_1);

	spl1mpy = _packh2(BplE,CplD);
	spl1mpy = _dotp2(0xFFFB0014,spl1mpy);
	spl1mpy += _ext(AplF,0,16);
	spl1mpy >>= 10;

	spl2mpy = _pack2(BplE,CplD);
	spl2mpy = _dotp2(0xFFFB0014,spl2mpy);
	spl2mpy += _ext(AplF,16,16);
	spl2mpy >>= 10;

	res12_4 = _pack2(spl1mpy,spl2mpy);

	AplF = _sadd2(l4_2,l9_2);
	AplF = _sadd2(0x02000200,AplF);
	BplE = _sadd2(l5_2,l8_2);
	CplD = _sadd2(l6_2,l7_2);

	spl1mpy = _packh2(BplE,CplD);
	spl1mpy = _dotp2(0xFFFB0014,spl1mpy);
	spl1mpy += _ext(AplF,0,16);
	spl1mpy >>= 10;

	spl2mpy = _pack2(BplE,CplD);
	spl2mpy = _dotp2(0xFFFB0014,spl2mpy);
	spl2mpy += _ext(AplF,16,16);
	spl2mpy >>= 10;

	res34_4 = _pack2(spl1mpy,spl2mpy);

	res_4 = _spacku4(res34_4,res12_4);
	res_4 = _swap4(res_4);

	pImgPtr += stride;
	_amem4(pImgPtr) = _avgu4(res_4,input4);

}





/**
This function allows to get the luminance prediction of a non IDR picture when xFracl = 2 and yFracl = 2.


@param image Table of current frame.
@param refPicLXl Table of the reference decoded picture buffer.
@param PicWidthSamples Stride of the reference buffer.
@param stride Stride of the current image.
*/
void luma_sample_interp_2_2_TI(unsigned char image [RESTRICT], unsigned char refPicLXl[RESTRICT],	const short PicWidthSamples, const short stride)
{
	/* Vertical & horizontal interpolation */

	unsigned char* pImgPtr;
	unsigned char* pRefImgPtr;
	unsigned int A,B,C;
	unsigned int tmpl1,tmpl2,tmpl3,tmpl4,tmpm1,tmpm2,tmpm3,tmpm4,tmpr4;
	unsigned int tmpl12,tmpl34,tmpm12,tmpm34,tmp12,tmp34;
	unsigned int l1_1,l1_2,l2_1,l2_2,l3_1,l3_2,l4_1,l4_2,l5_1,l5_2,l6_1,l6_2,l7_1,l7_2,l8_1,l8_2,l9_1,l9_2;
	unsigned int AplF,BplE,CplD;
	int spl1mpy,spl2mpy;
	unsigned int res12_1,res34_1,res12_2,res34_2,res12_3,res34_3,res12_4,res34_4;
	unsigned int res_1,res_2,res_3,res_4;
	unsigned int input1,input2,input3,input4,input5,inputtmp;

	// first line
	pRefImgPtr = refPicLXl-((PicWidthSamples+1)<<1);

	A = _mem4(pRefImgPtr);
	B = _mem4(pRefImgPtr+4);
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

	tmp12 = _sadd2(tmpl12,tmpm12);
	tmp34 = _sadd2(tmpl34,tmpm34);
	tmp34 = _sadd2(tmp34,tmpr4);

	l1_1 = tmp12;
	l1_2 = tmp34;

	// second line
	pRefImgPtr+=PicWidthSamples;

	A = _mem4(pRefImgPtr);
	B = _mem4(pRefImgPtr+4);
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
	tmp34 = _sadd2(tmpl34,tmpm34);
	tmp34 = _sadd2(tmp34,tmpr4);

	l2_1 = tmp12;
	l2_2 = tmp34;

	// third line
	pRefImgPtr+=PicWidthSamples;

	A = _mem4(pRefImgPtr);
	B = _mem4(pRefImgPtr+4);
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
	tmp34 = _sadd2(tmpl34,tmpm34);
	tmp34 = _sadd2(tmp34,tmpr4);

	l3_1 = tmp12;
	l3_2 = tmp34;

	input1 = _shr2(_sadd2(tmp12,0x00100010),5);
	inputtmp = _shr2(_sadd2(tmp34,0x00100010),5);
	input1 = _spacku4(inputtmp,input1);
	input1 = _swap4(input1);

	// fourth line
	pRefImgPtr+=PicWidthSamples;

	A = _mem4(pRefImgPtr);
	B = _mem4(pRefImgPtr+4);
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
	tmp34 = _sadd2(tmpl34,tmpm34);
	tmp34 = _sadd2(tmp34,tmpr4);

	l4_1 = tmp12;
	l4_2 = tmp34;

	input2 = _shr2(_sadd2(tmp12,0x00100010),5);
	inputtmp = _shr2(_sadd2(tmp34,0x00100010),5);
	input2 = _spacku4(inputtmp,input2);
	input2 = _swap4(input2);

	// fifth line
	pRefImgPtr+=PicWidthSamples;

	A = _mem4(pRefImgPtr);
	B = _mem4(pRefImgPtr+4);
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
	tmp34 = _sadd2(tmpl34,tmpm34);
	tmp34 = _sadd2(tmp34,tmpr4);

	l5_1 = tmp12;
	l5_2 = tmp34;

	input3 = _shr2(_sadd2(tmp12,0x00100010),5);
	inputtmp = _shr2(_sadd2(tmp34,0x00100010),5);
	input3 = _spacku4(inputtmp,input3);
	input3 = _swap4(input3);

	// sixth line
	pRefImgPtr+=PicWidthSamples;

	A = _mem4(pRefImgPtr);
	B = _mem4(pRefImgPtr+4);
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
	tmp34 = _sadd2(tmpl34,tmpm34);
	tmp34 = _sadd2(tmp34,tmpr4);

	l6_1 = tmp12;
	l6_2 = tmp34;

	input4 = _shr2(_sadd2(tmp12,0x00100010),5);
	inputtmp = _shr2(_sadd2(tmp34,0x00100010),5);
	input4 = _spacku4(inputtmp,input4);
	input4 = _swap4(input4);

	// seventh line
	pRefImgPtr+=PicWidthSamples;

	A = _mem4(pRefImgPtr);
	B = _mem4(pRefImgPtr+4);
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
	tmp34 = _sadd2(tmpl34,tmpm34);
	tmp34 = _sadd2(tmp34,tmpr4);

	l7_1 = tmp12;
	l7_2 = tmp34;

	input5 = _shr2(_sadd2(tmp12,0x00100010),5);
	inputtmp = _shr2(_sadd2(tmp34,0x00100010),5);
	input5 = _spacku4(inputtmp,input5);
	input5 = _swap4(input5);



	// eigth line
	pRefImgPtr+=PicWidthSamples;

	A = _mem4(pRefImgPtr);
	B = _mem4(pRefImgPtr+4);
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
	tmp34 = _sadd2(tmpl34,tmpm34);
	tmp34 = _sadd2(tmp34,tmpr4);

	l8_1 = tmp12;
	l8_2 = tmp34;

	// neigth line
	pRefImgPtr+=PicWidthSamples;

	A = _mem4(pRefImgPtr);
	B = _mem4(pRefImgPtr+4);
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
	tmp34 = _sadd2(tmpl34,tmpm34);
	tmp34 = _sadd2(tmp34,tmpr4);

	l9_1 = tmp12;
	l9_2 = tmp34;

	// The intermediary results are in li_i

	// First line
	AplF = _sadd2(l1_1,l6_1);
	AplF = _sadd2(0x02000200,AplF);
	BplE = _sadd2(l2_1,l5_1);
	CplD = _sadd2(l3_1,l4_1);

	spl1mpy = _packh2(BplE,CplD);
	spl1mpy = _dotp2(0xFFFB0014,spl1mpy);
	spl1mpy += _ext(AplF,0,16);
	spl1mpy >>= 10;

	spl2mpy = _pack2(BplE,CplD);
	spl2mpy = _dotp2(0xFFFB0014,spl2mpy);
	spl2mpy += _ext(AplF,16,16);
	spl2mpy >>= 10;

	res12_1 = _pack2(spl1mpy,spl2mpy);

	AplF = _sadd2(l1_2,l6_2);
	AplF = _sadd2(0x02000200,AplF);
	BplE = _sadd2(l2_2,l5_2);
	CplD = _sadd2(l3_2,l4_2);

	spl1mpy = _packh2(BplE,CplD);
	spl1mpy = _dotp2(0xFFFB0014,spl1mpy);
	spl1mpy += _ext(AplF,0,16);
	spl1mpy >>= 10;

	spl2mpy = _pack2(BplE,CplD);
	spl2mpy = _dotp2(0xFFFB0014,spl2mpy);
	spl2mpy += _ext(AplF,16,16);
	spl2mpy >>= 10;

	res34_1 = _pack2(spl1mpy,spl2mpy);

	res_1 = _spacku4(res34_1,res12_1);
	res_1 = _swap4(res_1);

	pImgPtr = image;
	_amem4(pImgPtr) = res_1;

	// Second line
	AplF = _sadd2(l2_1,l7_1);
	AplF = _sadd2(0x02000200,AplF);
	BplE = _sadd2(l3_1,l6_1);
	CplD = _sadd2(l4_1,l5_1);

	spl1mpy = _packh2(BplE,CplD);
	spl1mpy = _dotp2(0xFFFB0014,spl1mpy);
	spl1mpy += _ext(AplF,0,16);
	spl1mpy >>= 10;

	spl2mpy = _pack2(BplE,CplD);
	spl2mpy = _dotp2(0xFFFB0014,spl2mpy);
	spl2mpy += _ext(AplF,16,16);
	spl2mpy >>= 10;

	res12_2 = _pack2(spl1mpy,spl2mpy);

	AplF = _sadd2(l2_2,l7_2);
	AplF = _sadd2(0x02000200,AplF);
	BplE = _sadd2(l3_2,l6_2);
	CplD = _sadd2(l4_2,l5_2);

	spl1mpy = _packh2(BplE,CplD);
	spl1mpy = _dotp2(0xFFFB0014,spl1mpy);
	spl1mpy += _ext(AplF,0,16);
	spl1mpy >>= 10;

	spl2mpy = _pack2(BplE,CplD);
	spl2mpy = _dotp2(0xFFFB0014,spl2mpy);
	spl2mpy += _ext(AplF,16,16);
	spl2mpy >>= 10;

	res34_2 = _pack2(spl1mpy,spl2mpy);


	res_2 = _spacku4(res34_2,res12_2);
	res_2 = _swap4(res_2);

	pImgPtr += stride;
	_amem4(pImgPtr) = res_2;

	// Third line
	AplF = _sadd2(l3_1,l8_1);
	AplF = _sadd2(0x02000200,AplF);
	BplE = _sadd2(l4_1,l7_1);
	CplD = _sadd2(l5_1,l6_1);

	spl1mpy = _packh2(BplE,CplD);
	spl1mpy = _dotp2(0xFFFB0014,spl1mpy);
	spl1mpy += _ext(AplF,0,16);
	spl1mpy >>= 10;

	spl2mpy = _pack2(BplE,CplD);
	spl2mpy = _dotp2(0xFFFB0014,spl2mpy);
	spl2mpy += _ext(AplF,16,16);
	spl2mpy >>= 10;

	res12_3 = _pack2(spl1mpy,spl2mpy);

	AplF = _sadd2(l3_2,l8_2);
	AplF = _sadd2(0x02000200,AplF);
	BplE = _sadd2(l4_2,l7_2);
	CplD = _sadd2(l5_2,l6_2);

	spl1mpy = _packh2(BplE,CplD);
	spl1mpy = _dotp2(0xFFFB0014,spl1mpy);
	spl1mpy += _ext(AplF,0,16);
	spl1mpy >>= 10;

	spl2mpy = _pack2(BplE,CplD);
	spl2mpy = _dotp2(0xFFFB0014,spl2mpy);
	spl2mpy += _ext(AplF,16,16);
	spl2mpy >>= 10;

	res34_3 = _pack2(spl1mpy,spl2mpy);

	res_3 = _spacku4(res34_3,res12_3);
	res_3 = _swap4(res_3);

	pImgPtr += stride;
	_amem4(pImgPtr) = res_3;

	// Fourth line
	AplF = _sadd2(l4_1,l9_1);
	AplF = _sadd2(0x02000200,AplF);
	BplE = _sadd2(l5_1,l8_1);
	CplD = _sadd2(l6_1,l7_1);

	spl1mpy = _packh2(BplE,CplD);
	spl1mpy = _dotp2(0xFFFB0014,spl1mpy);
	spl1mpy += _ext(AplF,0,16);
	spl1mpy >>= 10;

	spl2mpy = _pack2(BplE,CplD);
	spl2mpy = _dotp2(0xFFFB0014,spl2mpy);
	spl2mpy += _ext(AplF,16,16);
	spl2mpy >>= 10;

	res12_4 = _pack2(spl1mpy,spl2mpy);

	AplF = _sadd2(l4_2,l9_2);
	AplF = _sadd2(0x02000200,AplF);
	BplE = _sadd2(l5_2,l8_2);
	CplD = _sadd2(l6_2,l7_2);

	spl1mpy = _packh2(BplE,CplD);
	spl1mpy = _dotp2(0xFFFB0014,spl1mpy);
	spl1mpy += _ext(AplF,0,16);
	spl1mpy >>= 10;

	spl2mpy = _pack2(BplE,CplD);
	spl2mpy = _dotp2(0xFFFB0014,spl2mpy);
	spl2mpy += _ext(AplF,16,16);
	spl2mpy >>= 10;

	res34_4 = _pack2(spl1mpy,spl2mpy);

	res_4 = _spacku4(res34_4,res12_4);
	res_4 = _swap4(res_4);

	pImgPtr += stride;
	_amem4(pImgPtr) = res_4;
	
}







/**
This function allows to get the luminance prediction of a non IDR picture when xFracl = 2 and yFracl = 3.

@param image Table of current frame.
@param refPicLXl Table of the reference decoded picture buffer.
@param PicWidthSamples Stride of the reference buffer.
@param stride Stride of the current image.
*/
void luma_sample_interp_2_3_TI(unsigned char image [RESTRICT], unsigned char refPicLXl[RESTRICT],	const short PicWidthSamples, const short stride)
{

	/* Vertical & horizontal interpolation */

	unsigned char* pImgPtr;
	unsigned char* pRefImgPtr;
	unsigned int A,B,C;
	unsigned int tmpl1,tmpl2,tmpl3,tmpl4,tmpm1,tmpm2,tmpm3,tmpm4,tmpr4;
	unsigned int tmpl12,tmpl34,tmpm12,tmpm34,tmp12,tmp34;
	unsigned int l1_1,l1_2,l2_1,l2_2,l3_1,l3_2,l4_1,l4_2,l5_1,l5_2,l6_1,l6_2,l7_1,l7_2,l8_1,l8_2,l9_1,l9_2;
	unsigned int AplF,BplE,CplD;
	int spl1mpy,spl2mpy;
	unsigned int res12_1,res34_1,res12_2,res34_2,res12_3,res34_3,res12_4,res34_4;
	unsigned int res_1,res_2,res_3,res_4;
	unsigned int input1,input2,input3,input4,input5,inputtmp;

	// first line
	pRefImgPtr = refPicLXl-((PicWidthSamples+1)<<1);

	A = _mem4(pRefImgPtr);
	B = _mem4(pRefImgPtr+4);
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

	tmp12 = _sadd2(tmpl12,tmpm12);
	tmp34 = _sadd2(tmpl34,tmpm34);
	tmp34 = _sadd2(tmp34,tmpr4);

	l1_1 = tmp12;
	l1_2 = tmp34;

	// second line
	pRefImgPtr+=PicWidthSamples;

	A = _mem4(pRefImgPtr);
	B = _mem4(pRefImgPtr+4);
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
	tmp34 = _sadd2(tmpl34,tmpm34);
	tmp34 = _sadd2(tmp34,tmpr4);

	l2_1 = tmp12;
	l2_2 = tmp34;

	// third line
	pRefImgPtr+=PicWidthSamples;

	A = _mem4(pRefImgPtr);
	B = _mem4(pRefImgPtr+4);
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
	tmp34 = _sadd2(tmpl34,tmpm34);
	tmp34 = _sadd2(tmp34,tmpr4);

	l3_1 = tmp12;
	l3_2 = tmp34;

	input1 = _shr2(_sadd2(tmp12,0x00100010),5);
	inputtmp = _shr2(_sadd2(tmp34,0x00100010),5);
	input1 = _spacku4(inputtmp,input1);
	input1 = _swap4(input1);

	// fourth line
	pRefImgPtr+=PicWidthSamples;

	A = _mem4(pRefImgPtr);
	B = _mem4(pRefImgPtr+4);
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
	tmp34 = _sadd2(tmpl34,tmpm34);
	tmp34 = _sadd2(tmp34,tmpr4);

	l4_1 = tmp12;
	l4_2 = tmp34;

	input2 = _shr2(_sadd2(tmp12,0x00100010),5);
	inputtmp = _shr2(_sadd2(tmp34,0x00100010),5);
	input2 = _spacku4(inputtmp,input2);
	input2 = _swap4(input2);

	// fifth line
	pRefImgPtr+=PicWidthSamples;

	A = _mem4(pRefImgPtr);
	B = _mem4(pRefImgPtr+4);
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
	tmp34 = _sadd2(tmpl34,tmpm34);
	tmp34 = _sadd2(tmp34,tmpr4);

	l5_1 = tmp12;
	l5_2 = tmp34;

	input3 = _shr2(_sadd2(tmp12,0x00100010),5);
	inputtmp = _shr2(_sadd2(tmp34,0x00100010),5);
	input3 = _spacku4(inputtmp,input3);
	input3 = _swap4(input3);

	// sixth line
	pRefImgPtr+=PicWidthSamples;

	A = _mem4(pRefImgPtr);
	B = _mem4(pRefImgPtr+4);
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
	tmp34 = _sadd2(tmpl34,tmpm34);
	tmp34 = _sadd2(tmp34,tmpr4);

	l6_1 = tmp12;
	l6_2 = tmp34;

	input4 = _shr2(_sadd2(tmp12,0x00100010),5);
	inputtmp = _shr2(_sadd2(tmp34,0x00100010),5);
	input4 = _spacku4(inputtmp,input4);
	input4 = _swap4(input4);

	// seventh line
	pRefImgPtr+=PicWidthSamples;

	A = _mem4(pRefImgPtr);
	B = _mem4(pRefImgPtr+4);
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
	tmp34 = _sadd2(tmpl34,tmpm34);
	tmp34 = _sadd2(tmp34,tmpr4);

	l7_1 = tmp12;
	l7_2 = tmp34;

	input5 = _shr2(_sadd2(tmp12,0x00100010),5);
	inputtmp = _shr2(_sadd2(tmp34,0x00100010),5);
	input5 = _spacku4(inputtmp,input5);
	input5 = _swap4(input5);


	input1 = input2;
	input2 = input3;
	input3 = input4;
	input4 = input5;


	// eigth line
	pRefImgPtr+=PicWidthSamples;

	A = _mem4(pRefImgPtr);
	B = _mem4(pRefImgPtr+4);
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
	tmp34 = _sadd2(tmpl34,tmpm34);
	tmp34 = _sadd2(tmp34,tmpr4);

	l8_1 = tmp12;
	l8_2 = tmp34;

	// neigth line
	pRefImgPtr+=PicWidthSamples;

	A = _mem4(pRefImgPtr);
	B = _mem4(pRefImgPtr+4);
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
	tmp34 = _sadd2(tmpl34,tmpm34);
	tmp34 = _sadd2(tmp34,tmpr4);

	l9_1 = tmp12;
	l9_2 = tmp34;

	// The intermediary results are in li_i

	// First line
	AplF = _sadd2(l1_1,l6_1);
	AplF = _sadd2(0x02000200,AplF);
	BplE = _sadd2(l2_1,l5_1);
	CplD = _sadd2(l3_1,l4_1);

	spl1mpy = _packh2(BplE,CplD);
	spl1mpy = _dotp2(0xFFFB0014,spl1mpy);
	spl1mpy += _ext(AplF,0,16);
	spl1mpy >>= 10;

	spl2mpy = _pack2(BplE,CplD);
	spl2mpy = _dotp2(0xFFFB0014,spl2mpy);
	spl2mpy += _ext(AplF,16,16);
	spl2mpy >>= 10;

	res12_1 = _pack2(spl1mpy,spl2mpy);

	AplF = _sadd2(l1_2,l6_2);
	AplF = _sadd2(0x02000200,AplF);
	BplE = _sadd2(l2_2,l5_2);
	CplD = _sadd2(l3_2,l4_2);

	spl1mpy = _packh2(BplE,CplD);
	spl1mpy = _dotp2(0xFFFB0014,spl1mpy);
	spl1mpy += _ext(AplF,0,16);
	spl1mpy >>= 10;

	spl2mpy = _pack2(BplE,CplD);
	spl2mpy = _dotp2(0xFFFB0014,spl2mpy);
	spl2mpy += _ext(AplF,16,16);
	spl2mpy >>= 10;

	res34_1 = _pack2(spl1mpy,spl2mpy);

	res_1 = _spacku4(res34_1,res12_1);
	res_1 = _swap4(res_1);

	pImgPtr = image;
	_amem4(pImgPtr) = _avgu4(res_1,input1);

	// Second line
	AplF = _sadd2(l2_1,l7_1);
	AplF = _sadd2(0x02000200,AplF);
	BplE = _sadd2(l3_1,l6_1);
	CplD = _sadd2(l4_1,l5_1);

	spl1mpy = _packh2(BplE,CplD);
	spl1mpy = _dotp2(0xFFFB0014,spl1mpy);
	spl1mpy += _ext(AplF,0,16);
	spl1mpy >>= 10;

	spl2mpy = _pack2(BplE,CplD);
	spl2mpy = _dotp2(0xFFFB0014,spl2mpy);
	spl2mpy += _ext(AplF,16,16);
	spl2mpy >>= 10;

	res12_2 = _pack2(spl1mpy,spl2mpy);

	AplF = _sadd2(l2_2,l7_2);
	AplF = _sadd2(0x02000200,AplF);
	BplE = _sadd2(l3_2,l6_2);
	CplD = _sadd2(l4_2,l5_2);

	spl1mpy = _packh2(BplE,CplD);
	spl1mpy = _dotp2(0xFFFB0014,spl1mpy);
	spl1mpy += _ext(AplF,0,16);
	spl1mpy >>= 10;

	spl2mpy = _pack2(BplE,CplD);
	spl2mpy = _dotp2(0xFFFB0014,spl2mpy);
	spl2mpy += _ext(AplF,16,16);
	spl2mpy >>= 10;

	res34_2 = _pack2(spl1mpy,spl2mpy);


	res_2 = _spacku4(res34_2,res12_2);
	res_2 = _swap4(res_2);

	pImgPtr += stride;
	_amem4(pImgPtr) = _avgu4(res_2,input2);

	// Third line
	AplF = _sadd2(l3_1,l8_1);
	AplF = _sadd2(0x02000200,AplF);
	BplE = _sadd2(l4_1,l7_1);
	CplD = _sadd2(l5_1,l6_1);

	spl1mpy = _packh2(BplE,CplD);
	spl1mpy = _dotp2(0xFFFB0014,spl1mpy);
	spl1mpy += _ext(AplF,0,16);
	spl1mpy >>= 10;

	spl2mpy = _pack2(BplE,CplD);
	spl2mpy = _dotp2(0xFFFB0014,spl2mpy);
	spl2mpy += _ext(AplF,16,16);
	spl2mpy >>= 10;

	res12_3 = _pack2(spl1mpy,spl2mpy);

	AplF = _sadd2(l3_2,l8_2);
	AplF = _sadd2(0x02000200,AplF);
	BplE = _sadd2(l4_2,l7_2);
	CplD = _sadd2(l5_2,l6_2);

	spl1mpy = _packh2(BplE,CplD);
	spl1mpy = _dotp2(0xFFFB0014,spl1mpy);
	spl1mpy += _ext(AplF,0,16);
	spl1mpy >>= 10;

	spl2mpy = _pack2(BplE,CplD);
	spl2mpy = _dotp2(0xFFFB0014,spl2mpy);
	spl2mpy += _ext(AplF,16,16);
	spl2mpy >>= 10;

	res34_3 = _pack2(spl1mpy,spl2mpy);

	res_3 = _spacku4(res34_3,res12_3);
	res_3 = _swap4(res_3);

	pImgPtr += stride;
	_amem4(pImgPtr) = _avgu4(res_3,input3);

	// Fourth line
	AplF = _sadd2(l4_1,l9_1);
	AplF = _sadd2(0x02000200,AplF);
	BplE = _sadd2(l5_1,l8_1);
	CplD = _sadd2(l6_1,l7_1);

	spl1mpy = _packh2(BplE,CplD);
	spl1mpy = _dotp2(0xFFFB0014,spl1mpy);
	spl1mpy += _ext(AplF,0,16);
	spl1mpy >>= 10;

	spl2mpy = _pack2(BplE,CplD);
	spl2mpy = _dotp2(0xFFFB0014,spl2mpy);
	spl2mpy += _ext(AplF,16,16);
	spl2mpy >>= 10;

	res12_4 = _pack2(spl1mpy,spl2mpy);

	AplF = _sadd2(l4_2,l9_2);
	AplF = _sadd2(0x02000200,AplF);
	BplE = _sadd2(l5_2,l8_2);
	CplD = _sadd2(l6_2,l7_2);

	spl1mpy = _packh2(BplE,CplD);
	spl1mpy = _dotp2(0xFFFB0014,spl1mpy);
	spl1mpy += _ext(AplF,0,16);
	spl1mpy >>= 10;

	spl2mpy = _pack2(BplE,CplD);
	spl2mpy = _dotp2(0xFFFB0014,spl2mpy);
	spl2mpy += _ext(AplF,16,16);
	spl2mpy >>= 10;

	res34_4 = _pack2(spl1mpy,spl2mpy);

	res_4 = _spacku4(res34_4,res12_4);
	res_4 = _swap4(res_4);

	pImgPtr += stride;
	_amem4(pImgPtr) = _avgu4(res_4,input4);

}


#endif
