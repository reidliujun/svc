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
#include "interpol_n_2.h"


/**
This function allows to get the luminance prediction of a non IDR picture when xFracl = 1 and yFracl = 2.


@param image Table of current frame.
@param refPicLXl Table of the reference decoded picture buffer.
@param PicWidthSamples Stride of the reference buffer.
@param stride Stride of the current image.
*/
void luma_sample_interp_1_2_TI(unsigned char image [RESTRICT], unsigned char refPicLXl[RESTRICT],	const short PicWidthSamples, const short stride)
{ 



	unsigned int input1,input2,input3,input4;
	unsigned int tmpl1_1,tmpl1_2,tmpl1_3,tmpl1_4,tmpl1_5,tmpl2_1,tmpl2_2,tmpl2_3,tmpl2_4,tmpl2_5;
	unsigned int tmpl3_1,tmpl3_2,tmpl3_3,tmpl3_4,tmpl3_5,tmpl4_1,tmpl4_2,tmpl4_3,tmpl4_4,tmpl4_5;

	{

		unsigned int uiLine1_1,uiLine2_1,uiLine3_1,uiLine4_1,uiLine5_1,uiLine6_1,uiLine7_1,uiLine8_1,uiLine9_1;
		unsigned int uiLine1_2,uiLine2_2,uiLine3_2,uiLine4_2,uiLine5_2,uiLine6_2,uiLine7_2,uiLine8_2,uiLine9_2;
		unsigned int uiLine1_3,uiLine2_3,uiLine3_3,uiLine4_3,uiLine5_3,uiLine6_3,uiLine7_3,uiLine8_3,uiLine9_3;

		unsigned int uiTmpLine12_h,uiTmpLine34_h,uiTmpLine12_l,uiTmpLine34_l,uiTmpLine1234_4,uiTmpLine1234_2,uiTmpLine1234_3,uiTmpLine1234_1;
		unsigned int uiTmpLine56_h,uiTmpLine78_h,uiTmpLine56_l,uiTmpLine78_l,uiTmpLine5678_4,uiTmpLine5678_2,uiTmpLine5678_3,uiTmpLine5678_1;
		unsigned int uiTmpLine1234_5,uiTmpLine1234_6,uiTmpLine1234_7,uiTmpLine1234_8,uiTmpLine1234_9;
		unsigned int uiTmpLine5678_5,uiTmpLine5678_6,uiTmpLine5678_7,uiTmpLine5678_8,uiTmpLine5678_9;

		unsigned int tmp12,tmp34,tmp56,tmp78,tmp9;

		unsigned int tmpc1_1,tmpc1_2,tmpc2_1,tmpc2_2,tmpc1,tmpc2;
		unsigned int tmpc3_1,tmpc3_2,tmpc4_1,tmpc4_2,tmpc3,tmpc4;
		unsigned int tmpc5_1,tmpc5_2,tmpc6_1,tmpc6_2,tmpc5,tmpc6;
		unsigned int tmpc7_1,tmpc7_2,tmpc8_1,tmpc8_2,tmpc7,tmpc8;
		unsigned int tmpc9_1,tmpc9_2;
		unsigned int tmpl9l,tmpl9h;

		unsigned char* pRefImgPtr;

		pRefImgPtr = refPicLXl-2-(PicWidthSamples<<1);

		uiLine1_1 = _mem4(pRefImgPtr);
		uiLine1_2 = _mem4(pRefImgPtr+4);
		uiLine1_3 = _mem4(pRefImgPtr+8);
		pRefImgPtr += PicWidthSamples;

		uiLine2_1 = _mem4(pRefImgPtr);
		uiLine2_2 = _mem4(pRefImgPtr+4);
		uiLine2_3 = _mem4(pRefImgPtr+8);
		pRefImgPtr += PicWidthSamples;

		uiLine3_1 = _mem4(pRefImgPtr);
		uiLine3_2 = _mem4(pRefImgPtr+4);
		uiLine3_3 = _mem4(pRefImgPtr+8);
		pRefImgPtr += PicWidthSamples;

		uiLine4_1 = _mem4(pRefImgPtr);
		uiLine4_2 = _mem4(pRefImgPtr+4);
		uiLine4_3 = _mem4(pRefImgPtr+8);
		pRefImgPtr += PicWidthSamples;

		uiLine5_1 = _mem4(pRefImgPtr);
		uiLine5_2 = _mem4(pRefImgPtr+4);
		uiLine5_3 = _mem4(pRefImgPtr+8);
		pRefImgPtr += PicWidthSamples;

		uiLine6_1 = _mem4(pRefImgPtr);
		uiLine6_2 = _mem4(pRefImgPtr+4);
		uiLine6_3 = _mem4(pRefImgPtr+8);
		pRefImgPtr += PicWidthSamples;

		uiLine7_1 = _mem4(pRefImgPtr);
		uiLine7_2 = _mem4(pRefImgPtr+4);
		uiLine7_3 = _mem4(pRefImgPtr+8);
		pRefImgPtr += PicWidthSamples;

		uiLine8_1 = _mem4(pRefImgPtr);
		uiLine8_2 = _mem4(pRefImgPtr+4);
		uiLine8_3 = _mem4(pRefImgPtr+8);
		pRefImgPtr += PicWidthSamples;

		uiLine9_1 = _mem4(pRefImgPtr);
		uiLine9_2 = _mem4(pRefImgPtr+4);
		uiLine9_3 = _mem4(pRefImgPtr+8);

		uiTmpLine12_h = _packh4(uiLine1_1,uiLine2_1);
		uiTmpLine34_h = _packh4(uiLine3_1,uiLine4_1);
		uiTmpLine12_l = _packl4(uiLine1_1,uiLine2_1);
		uiTmpLine34_l = _packl4(uiLine3_1,uiLine4_1);
		uiTmpLine1234_4 = _packh4(uiTmpLine12_h,uiTmpLine34_h);
		uiTmpLine1234_2 = _packl4(uiTmpLine12_h,uiTmpLine34_h);
		uiTmpLine1234_3 = _packh4(uiTmpLine12_l,uiTmpLine34_l);
		uiTmpLine1234_1 = _packl4(uiTmpLine12_l,uiTmpLine34_l);

		uiTmpLine56_h = _packh4(uiLine5_1,uiLine6_1);
		uiTmpLine78_h = _packh4(uiLine7_1,uiLine8_1);
		uiTmpLine56_l = _packl4(uiLine5_1,uiLine6_1);
		uiTmpLine78_l = _packl4(uiLine7_1,uiLine8_1);
		uiTmpLine5678_4 = _packh4(uiTmpLine56_h,uiTmpLine78_h);
		uiTmpLine5678_2 = _packl4(uiTmpLine56_h,uiTmpLine78_h);
		uiTmpLine5678_3 = _packh4(uiTmpLine56_l,uiTmpLine78_l);
		uiTmpLine5678_1 = _packl4(uiTmpLine56_l,uiTmpLine78_l);

		uiTmpLine12_h = _packh4(uiLine1_2,uiLine2_2);
		uiTmpLine34_h = _packh4(uiLine3_2,uiLine4_2);
		uiTmpLine12_l = _packl4(uiLine1_2,uiLine2_2);
		uiTmpLine34_l = _packl4(uiLine3_2,uiLine4_2);
		uiTmpLine1234_8 = _packh4(uiTmpLine12_h,uiTmpLine34_h);
		uiTmpLine1234_6 = _packl4(uiTmpLine12_h,uiTmpLine34_h);
		uiTmpLine1234_7 = _packh4(uiTmpLine12_l,uiTmpLine34_l);
		uiTmpLine1234_5 = _packl4(uiTmpLine12_l,uiTmpLine34_l);

		uiTmpLine56_h = _packh4(uiLine5_2,uiLine6_2);
		uiTmpLine78_h = _packh4(uiLine7_2,uiLine8_2);
		uiTmpLine56_l = _packl4(uiLine5_2,uiLine6_2);
		uiTmpLine78_l = _packl4(uiLine7_2,uiLine8_2);
		uiTmpLine5678_8 = _packh4(uiTmpLine56_h,uiTmpLine78_h);
		uiTmpLine5678_6 = _packl4(uiTmpLine56_h,uiTmpLine78_h);
		uiTmpLine5678_7 = _packh4(uiTmpLine56_l,uiTmpLine78_l);
		uiTmpLine5678_5 = _packl4(uiTmpLine56_l,uiTmpLine78_l);


		uiTmpLine12_l = _packl4(uiLine1_3,uiLine2_3);
		uiTmpLine34_l = _packl4(uiLine3_3,uiLine4_3);
		uiTmpLine1234_9 = _packl4(uiTmpLine12_l,uiTmpLine34_l);

		uiTmpLine56_l = _packl4(uiLine5_3,uiLine6_3);
		uiTmpLine78_l = _packl4(uiLine7_3,uiLine8_3);
		uiTmpLine5678_9 = _packl4(uiTmpLine56_l,uiTmpLine78_l);

		// first line
		tmpc1_1 = _dotpsu4(0x01FB1414,uiTmpLine1234_1);
		tmpc1_2 = _dotpsu4(0xFB010000,uiTmpLine5678_1);

		tmpc2_1 = _dotpsu4(0x01FB1414,uiTmpLine1234_2);
		tmpc2_2 = _dotpsu4(0xFB010000,uiTmpLine5678_2);

		tmpc1 = _spack2(tmpc1_1,tmpc2_1);
		tmpc2 = _spack2(tmpc1_2,tmpc2_2);

		tmp12 = _sadd2(tmpc1,tmpc2);

		tmpc3_1 = _dotpsu4(0x01FB1414,uiTmpLine1234_3);
		tmpc3_2 = _dotpsu4(0xFB010000,uiTmpLine5678_3);

		tmpc4_1 = _dotpsu4(0x01FB1414,uiTmpLine1234_4);
		tmpc4_2 = _dotpsu4(0xFB010000,uiTmpLine5678_4);

		tmpc3 = _spack2(tmpc3_1,tmpc4_1);
		tmpc4 = _spack2(tmpc3_2,tmpc4_2);

		tmp34 = _sadd2(tmpc3,tmpc4);

		tmpc5_1 = _dotpsu4(0x01FB1414,uiTmpLine1234_5);
		tmpc5_2 = _dotpsu4(0xFB010000,uiTmpLine5678_5);

		tmpc6_1 = _dotpsu4(0x01FB1414,uiTmpLine1234_6);
		tmpc6_2 = _dotpsu4(0xFB010000,uiTmpLine5678_6);

		tmpc5 = _spack2(tmpc5_1,tmpc6_1);
		tmpc6 = _spack2(tmpc5_2,tmpc6_2);

		tmp56 = _sadd2(tmpc5,tmpc6);

		tmpc7_1 = _dotpsu4(0x01FB1414,uiTmpLine1234_7);
		tmpc7_2 = _dotpsu4(0xFB010000,uiTmpLine5678_7);

		tmpc8_1 = _dotpsu4(0x01FB1414,uiTmpLine1234_8);
		tmpc8_2 = _dotpsu4(0xFB010000,uiTmpLine5678_8);

		tmpc7 = _spack2(tmpc7_1,tmpc8_1);
		tmpc8 = _spack2(tmpc7_2,tmpc8_2);

		tmp78 = _sadd2(tmpc7,tmpc8);

		tmpc9_1 = _dotpsu4(0x01FB1414,uiTmpLine1234_9);
		tmpc9_2 = _dotpsu4(0xFB010000,uiTmpLine5678_9);

		tmp9 = (tmpc9_1 + tmpc9_2) << 16;

		tmpl1_1 = tmp12;
		tmpl1_2 = tmp34;
		tmpl1_3 = tmp56;
		tmpl1_4 = tmp78;
		tmpl1_5 = tmp9;

		// second line
		tmpc1_1 = _dotpsu4(0x0001FB14,uiTmpLine1234_1);
		tmpc1_2 = _dotpsu4(0x14FB0100,uiTmpLine5678_1);

		tmpc2_1 = _dotpsu4(0x0001FB14,uiTmpLine1234_2);
		tmpc2_2 = _dotpsu4(0x14FB0100,uiTmpLine5678_2);

		tmpc1 = _spack2(tmpc1_1,tmpc2_1);
		tmpc2 = _spack2(tmpc1_2,tmpc2_2);

		tmp12 = _sadd2(tmpc1,tmpc2);

		tmpc3_1 = _dotpsu4(0x0001FB14,uiTmpLine1234_3);
		tmpc3_2 = _dotpsu4(0x14FB0100,uiTmpLine5678_3);

		tmpc4_1 = _dotpsu4(0x0001FB14,uiTmpLine1234_4);
		tmpc4_2 = _dotpsu4(0x14FB0100,uiTmpLine5678_4);

		tmpc3 = _spack2(tmpc3_1,tmpc4_1);
		tmpc4 = _spack2(tmpc3_2,tmpc4_2);

		tmp34 = _sadd2(tmpc3,tmpc4);

		tmpc5_1 = _dotpsu4(0x0001FB14,uiTmpLine1234_5);
		tmpc5_2 = _dotpsu4(0x14FB0100,uiTmpLine5678_5);

		tmpc6_1 = _dotpsu4(0x0001FB14,uiTmpLine1234_6);
		tmpc6_2 = _dotpsu4(0x14FB0100,uiTmpLine5678_6);

		tmpc5 = _spack2(tmpc5_1,tmpc6_1);
		tmpc6 = _spack2(tmpc5_2,tmpc6_2);

		tmp56 = _sadd2(tmpc5,tmpc6);

		tmpc7_1 = _dotpsu4(0x0001FB14,uiTmpLine1234_7);
		tmpc7_2 = _dotpsu4(0x14FB0100,uiTmpLine5678_7);

		tmpc8_1 = _dotpsu4(0x0001FB14,uiTmpLine1234_8);
		tmpc8_2 = _dotpsu4(0x14FB0100,uiTmpLine5678_8);

		tmpc7 = _spack2(tmpc7_1,tmpc8_1);
		tmpc8 = _spack2(tmpc7_2,tmpc8_2);

		tmp78 = _sadd2(tmpc7,tmpc8);

		tmpc9_1 = _dotpsu4(0x0001FB14,uiTmpLine1234_9);
		tmpc9_2 = _dotpsu4(0x14FB0100,uiTmpLine5678_9);

		tmp9 = (tmpc9_1 + tmpc9_2) << 16;

		tmpl2_1 = tmp12;
		tmpl2_2 = tmp34;
		tmpl2_3 = tmp56;
		tmpl2_4 = tmp78;
		tmpl2_5 = tmp9;

		// third line
		tmpc1_1 = _dotpsu4(0x000001FB,uiTmpLine1234_1);
		tmpc1_2 = _dotpsu4(0x1414FB01,uiTmpLine5678_1);

		tmpc2_1 = _dotpsu4(0x000001FB,uiTmpLine1234_2);
		tmpc2_2 = _dotpsu4(0x1414FB01,uiTmpLine5678_2);

		tmpc1 = _spack2(tmpc1_1,tmpc2_1);
		tmpc2 = _spack2(tmpc1_2,tmpc2_2);

		tmp12 = _sadd2(tmpc1,tmpc2);

		tmpc3_1 = _dotpsu4(0x000001FB,uiTmpLine1234_3);
		tmpc3_2 = _dotpsu4(0x1414FB01,uiTmpLine5678_3);

		tmpc4_1 = _dotpsu4(0x000001FB,uiTmpLine1234_4);
		tmpc4_2 = _dotpsu4(0x1414FB01,uiTmpLine5678_4);

		tmpc3 = _spack2(tmpc3_1,tmpc4_1);
		tmpc4 = _spack2(tmpc3_2,tmpc4_2);

		tmp34 = _sadd2(tmpc3,tmpc4);

		tmpc5_1 = _dotpsu4(0x000001FB,uiTmpLine1234_5);
		tmpc5_2 = _dotpsu4(0x1414FB01,uiTmpLine5678_5);

		tmpc6_1 = _dotpsu4(0x000001FB,uiTmpLine1234_6);
		tmpc6_2 = _dotpsu4(0x1414FB01,uiTmpLine5678_6);

		tmpc5 = _spack2(tmpc5_1,tmpc6_1);
		tmpc6 = _spack2(tmpc5_2,tmpc6_2);

		tmp56 = _sadd2(tmpc5,tmpc6);

		tmpc7_1 = _dotpsu4(0x000001FB,uiTmpLine1234_7);
		tmpc7_2 = _dotpsu4(0x1414FB01,uiTmpLine5678_7);

		tmpc8_1 = _dotpsu4(0x000001FB,uiTmpLine1234_8);
		tmpc8_2 = _dotpsu4(0x1414FB01,uiTmpLine5678_8);

		tmpc7 = _spack2(tmpc7_1,tmpc8_1);
		tmpc8 = _spack2(tmpc7_2,tmpc8_2);

		tmp78 = _sadd2(tmpc7,tmpc8);

		tmpc9_1 = _dotpsu4(0x000001FB,uiTmpLine1234_9);
		tmpc9_2 = _dotpsu4(0x1414FB01,uiTmpLine5678_9);

		tmp9 = (tmpc9_1 + tmpc9_2) << 16;

		tmpl3_1 = tmp12;
		tmpl3_2 = tmp34;
		tmpl3_3 = tmp56;
		tmpl3_4 = tmp78;
		tmpl3_5 = tmp9;



		// fourth line
		tmpc1_1 = _dotpsu4(0x00000001,uiTmpLine1234_1);
		tmpc1_2 = _dotpsu4(0xFB1414FB,uiTmpLine5678_1);

		tmpc2_1 = _dotpsu4(0x00000001,uiTmpLine1234_2);
		tmpc2_2 = _dotpsu4(0xFB1414FB,uiTmpLine5678_2);

		uiLine9_1 = _swap4(uiLine9_1);
		tmpl9h = _unpkhu4 (uiLine9_1);
		tmpl9l = _unpklu4 (uiLine9_1);

		tmpc1 = _spack2(tmpc1_1,tmpc2_1);
		tmpc2 = _spack2(tmpc1_2,tmpc2_2);

		tmp12 = _sadd2(tmpc1,tmpc2);
		tmp12 = _sadd2(tmp12,tmpl9l);

		tmpc3_1 = _dotpsu4(0x00000001,uiTmpLine1234_3);
		tmpc3_2 = _dotpsu4(0xFB1414FB,uiTmpLine5678_3);

		tmpc4_1 = _dotpsu4(0x00000001,uiTmpLine1234_4);
		tmpc4_2 = _dotpsu4(0xFB1414FB,uiTmpLine5678_4);

		tmpc3 = _spack2(tmpc3_1,tmpc4_1);
		tmpc4 = _spack2(tmpc3_2,tmpc4_2);

		tmp34 = _sadd2(tmpc3,tmpc4);
		tmp34 = _sadd2(tmp34,tmpl9h);


		uiLine9_2 = _swap4(uiLine9_2);
		tmpl9h = _unpkhu4 (uiLine9_2);
		tmpl9l = _unpklu4 (uiLine9_2);

		tmpc5_1 = _dotpsu4(0x00000001,uiTmpLine1234_5);
		tmpc5_2 = _dotpsu4(0xFB1414FB,uiTmpLine5678_5);

		tmpc6_1 = _dotpsu4(0x00000001,uiTmpLine1234_6);
		tmpc6_2 = _dotpsu4(0xFB1414FB,uiTmpLine5678_6);

		tmpc5 = _spack2(tmpc5_1,tmpc6_1);
		tmpc6 = _spack2(tmpc5_2,tmpc6_2);

		tmp56 = _sadd2(tmpc5,tmpc6);
		tmp56 = _sadd2(tmp56,tmpl9l);

		tmpc7_1 = _dotpsu4(0x00000001,uiTmpLine1234_7);
		tmpc7_2 = _dotpsu4(0xFB1414FB,uiTmpLine5678_7);

		tmpc8_1 = _dotpsu4(0x00000001,uiTmpLine1234_8);
		tmpc8_2 = _dotpsu4(0xFB1414FB,uiTmpLine5678_8);

		tmpc7 = _spack2(tmpc7_1,tmpc8_1);
		tmpc8 = _spack2(tmpc7_2,tmpc8_2);

		tmp78 = _sadd2(tmpc7,tmpc8);
		tmp78 = _sadd2(tmp78,tmpl9h);

		tmpc9_1 = _dotpsu4(0x00000001,uiTmpLine1234_9);
		tmpc9_2 = _dotpsu4(0xFB1414FB,uiTmpLine5678_9);

		tmp9 = (tmpc9_1 + tmpc9_2 + _extu(uiLine9_3,24,24)) << 16;

		tmpl4_1 = tmp12;
		tmpl4_2 = tmp34;
		tmpl4_3 = tmp56;
		tmpl4_4 = tmp78;
		tmpl4_5 = tmp9;

		// Input for last avgu4

		{
			unsigned int avgtmp1 = _shr2(_sadd2(tmpl1_2,0x00100010),5);
			unsigned int avgtmp2 = _shr2(_sadd2(tmpl1_3,0x00100010),5);
			input1 = _spacku4(avgtmp2,avgtmp1);
			input1 = _swap4(input1);

			avgtmp1 = _shr2(_sadd2(tmpl2_2,0x00100010),5);
			avgtmp2 = _shr2(_sadd2(tmpl2_3,0x00100010),5);
			input2 = _spacku4(avgtmp2,avgtmp1);
			input2 = _swap4(input2);

			avgtmp1 = _shr2(_sadd2(tmpl3_2,0x00100010),5);
			avgtmp2 = _shr2(_sadd2(tmpl3_3,0x00100010),5);
			input3 = _spacku4(avgtmp2,avgtmp1);
			input3 = _swap4(input3);

			avgtmp1 = _shr2(_sadd2(tmpl4_2,0x00100010),5);
			avgtmp2 = _shr2(_sadd2(tmpl4_3,0x00100010),5);
			input4 = _spacku4(avgtmp2,avgtmp1);
			input4 = _swap4(input4);

		}

		// intermediary results are stored in tmpli_i registers

		{
			unsigned int BC,ED,AAplFF,BCplED,res1_12,res1_34,spl1mpy,spl2mpy,spl3mpy,spl4mpy,tmpend1,tmpend2,tmpend3,tmpend4;
			unsigned int res2_12,res2_34,res3_12,res3_34,res4_12,res4_34;
			unsigned char* pImgPtr;

			// First line
			// first value
			BC = _packlh2(tmpl1_1,tmpl1_2);
			ED = _packhl2(tmpl1_3,tmpl1_2);
			BCplED = _sadd2(BC,ED);
			spl1mpy = _dotp2(0xFFFB0014,BCplED);


			AAplFF = _sadd2(tmpl1_1,_packlh2(tmpl1_3,tmpl1_4));
			AAplFF = _sadd2(0x02000200,AAplFF);
			spl1mpy +=  _ext(AAplFF,0,16);
			spl1mpy >>= 10;

			// second value
			BC = tmpl1_2;
			ED = _packlh2(tmpl1_3,tmpl1_3);
			BCplED = _sadd2(BC,ED);
			spl2mpy = _dotp2(0xFFFB0014,BCplED);

			spl2mpy +=  _ext(AAplFF,16,16);
			spl2mpy >>= 10;

			res1_12 = _pack2(spl1mpy,spl2mpy);
			// third value
			BC = _packlh2(tmpl1_2,tmpl1_3);
			ED = _packhl2(tmpl1_4,tmpl1_3);
			BCplED = _sadd2(BC,ED);
			spl3mpy = _dotp2(0xFFFB0014,BCplED);


			AAplFF = _sadd2(tmpl1_2,_packlh2(tmpl1_4,tmpl1_5));
			AAplFF = _sadd2(0x02000200,AAplFF);
			spl3mpy +=  _ext(AAplFF,0,16);
			spl3mpy >>= 10;

			// fourth value
			BC = tmpl1_3;
			ED = _packlh2(tmpl1_4,tmpl1_4);
			BCplED = _sadd2(BC,ED);
			spl4mpy = _dotp2(0xFFFB0014,BCplED);

			spl4mpy +=  _ext(AAplFF,16,16);
			spl4mpy >>= 10;

			res1_34 = _pack2(spl3mpy,spl4mpy);

			tmpend1 = _spacku4(res1_34,res1_12);
			tmpend1 = _swap4(tmpend1);


			pImgPtr = image;
			_amem4(pImgPtr) = _avgu4(tmpend1,input1);

			// Second line
			// first value
			BC = _packlh2(tmpl2_1,tmpl2_2);
			ED = _packhl2(tmpl2_3,tmpl2_2);
			BCplED = _sadd2(BC,ED);
			spl1mpy = _dotp2(0xFFFB0014,BCplED);


			AAplFF = _sadd2(tmpl2_1,_packlh2(tmpl2_3,tmpl2_4));
			AAplFF = _sadd2(0x02000200,AAplFF);
			spl1mpy +=  _ext(AAplFF,0,16);
			spl1mpy >>= 10;

			// second value
			BC = tmpl2_2;
			ED = _packlh2(tmpl2_3,tmpl2_3);
			BCplED = _sadd2(BC,ED);
			spl2mpy = _dotp2(0xFFFB0014,BCplED);

			spl2mpy +=  _ext(AAplFF,16,16);
			spl2mpy >>= 10;

			res2_12 = _pack2(spl1mpy,spl2mpy);
			// third value
			BC = _packlh2(tmpl2_2,tmpl2_3);
			ED = _packhl2(tmpl2_4,tmpl2_3);
			BCplED = _sadd2(BC,ED);
			spl3mpy = _dotp2(0xFFFB0014,BCplED);


			AAplFF = _sadd2(tmpl2_2,_packlh2(tmpl2_4,tmpl2_5));
			AAplFF = _sadd2(0x02000200,AAplFF);
			spl3mpy +=  _ext(AAplFF,0,16);
			spl3mpy >>= 10;

			// fourth value
			BC = tmpl2_3;
			ED = _packlh2(tmpl2_4,tmpl2_4);
			BCplED = _sadd2(BC,ED);
			spl4mpy = _dotp2(0xFFFB0014,BCplED);

			spl4mpy +=  _ext(AAplFF,16,16);
			spl4mpy >>= 10;

			res2_34 = _pack2(spl3mpy,spl4mpy);

			tmpend2 = _spacku4(res2_34,res2_12);
			tmpend2 = _swap4(tmpend2);

			pImgPtr += stride;
			_amem4(pImgPtr) =  _avgu4(tmpend2,input2);

			// Third line
			// first value
			BC = _packlh2(tmpl3_1,tmpl3_2);
			ED = _packhl2(tmpl3_3,tmpl3_2);
			BCplED = _sadd2(BC,ED);
			spl1mpy = _dotp2(0xFFFB0014,BCplED);


			AAplFF = _sadd2(tmpl3_1,_packlh2(tmpl3_3,tmpl3_4));
			AAplFF = _sadd2(0x02000200,AAplFF);
			spl1mpy +=  _ext(AAplFF,0,16);
			spl1mpy >>= 10;

			// second value
			BC = tmpl3_2;
			ED = _packlh2(tmpl3_3,tmpl3_3);
			BCplED = _sadd2(BC,ED);
			spl2mpy = _dotp2(0xFFFB0014,BCplED);

			spl2mpy +=  _ext(AAplFF,16,16);
			spl2mpy >>= 10;

			res3_12 = _pack2(spl1mpy,spl2mpy);
			// third value
			BC = _packlh2(tmpl3_2,tmpl3_3);
			ED = _packhl2(tmpl3_4,tmpl3_3);
			BCplED = _sadd2(BC,ED);
			spl3mpy = _dotp2(0xFFFB0014,BCplED);


			AAplFF = _sadd2(tmpl3_2,_packlh2(tmpl3_4,tmpl3_5));
			AAplFF = _sadd2(0x02000200,AAplFF);
			spl3mpy +=  _ext(AAplFF,0,16);
			spl3mpy >>= 10;

			// fourth value
			BC = tmpl3_3;
			ED = _packlh2(tmpl3_4,tmpl3_4);
			BCplED = _sadd2(BC,ED);
			spl4mpy = _dotp2(0xFFFB0014,BCplED);

			spl4mpy +=  _ext(AAplFF,16,16);
			spl4mpy >>= 10;

			res3_34 = _pack2(spl3mpy,spl4mpy);

			tmpend3 = _spacku4(res3_34,res3_12);
			tmpend3 = _swap4(tmpend3);

			pImgPtr += stride;
			_amem4(pImgPtr) =  _avgu4(tmpend3,input3);

			// Fourth line
			// first value
			BC = _packlh2(tmpl4_1,tmpl4_2);
			ED = _packhl2(tmpl4_3,tmpl4_2);
			BCplED = _sadd2(BC,ED);
			spl1mpy = _dotp2(0xFFFB0014,BCplED);


			AAplFF = _sadd2(tmpl4_1,_packlh2(tmpl4_3,tmpl4_4));
			AAplFF = _sadd2(0x02000200,AAplFF);
			spl1mpy +=  _ext(AAplFF,0,16);
			spl1mpy >>= 10;

			// second value
			BC = tmpl4_2;
			ED = _packlh2(tmpl4_3,tmpl4_3);
			BCplED = _sadd2(BC,ED);
			spl2mpy = _dotp2(0xFFFB0014,BCplED);

			spl2mpy +=  _ext(AAplFF,16,16);
			spl2mpy >>= 10;

			res4_12 = _pack2(spl1mpy,spl2mpy);
			// third value
			BC = _packlh2(tmpl4_2,tmpl4_3);
			ED = _packhl2(tmpl4_4,tmpl4_3);
			BCplED = _sadd2(BC,ED);
			spl3mpy = _dotp2(0xFFFB0014,BCplED);


			AAplFF = _sadd2(tmpl4_2,_packlh2(tmpl4_4,tmpl4_5));
			AAplFF = _sadd2(0x02000200,AAplFF);
			spl3mpy +=  _ext(AAplFF,0,16);
			spl3mpy >>= 10;

			// fourth value
			BC = tmpl4_3;
			ED = _packlh2(tmpl4_4,tmpl4_4);
			BCplED = _sadd2(BC,ED);
			spl4mpy = _dotp2(0xFFFB0014,BCplED);

			spl4mpy +=  _ext(AAplFF,16,16);
			spl4mpy >>= 10;

			res4_34 = _pack2(spl3mpy,spl4mpy);

			tmpend4 = _spacku4(res4_34,res4_12);
			tmpend4 = _swap4(tmpend4);

			pImgPtr += stride;
			_amem4(pImgPtr) = _avgu4(tmpend4,input4);

		}
	}

}

/*
@param image Table of current frame.
@param refPicLXl Table of the reference decoded picture buffer.
@param PicWidthSamples Stride of the reference buffer.
@param stride Stride of the current image.
*/
void luma_sample_interp_3_2_TI(unsigned char image [RESTRICT], unsigned char refPicLXl[RESTRICT],	const short PicWidthSamples, const short stride)
{ 

	unsigned int input1,input2,input3,input4;
	unsigned int tmpl1_1,tmpl1_2,tmpl1_3,tmpl1_4,tmpl1_5,tmpl2_1,tmpl2_2,tmpl2_3,tmpl2_4,tmpl2_5;
	unsigned int tmpl3_1,tmpl3_2,tmpl3_3,tmpl3_4,tmpl3_5,tmpl4_1,tmpl4_2,tmpl4_3,tmpl4_4,tmpl4_5;

	{

		unsigned int uiLine1_1,uiLine2_1,uiLine3_1,uiLine4_1,uiLine5_1,uiLine6_1,uiLine7_1,uiLine8_1,uiLine9_1;
		unsigned int uiLine1_2,uiLine2_2,uiLine3_2,uiLine4_2,uiLine5_2,uiLine6_2,uiLine7_2,uiLine8_2,uiLine9_2;
		unsigned int uiLine1_3,uiLine2_3,uiLine3_3,uiLine4_3,uiLine5_3,uiLine6_3,uiLine7_3,uiLine8_3,uiLine9_3;

		unsigned int uiTmpLine12_h,uiTmpLine34_h,uiTmpLine12_l,uiTmpLine34_l,uiTmpLine1234_4,uiTmpLine1234_2,uiTmpLine1234_3,uiTmpLine1234_1;
		unsigned int uiTmpLine56_h,uiTmpLine78_h,uiTmpLine56_l,uiTmpLine78_l,uiTmpLine5678_4,uiTmpLine5678_2,uiTmpLine5678_3,uiTmpLine5678_1;
		unsigned int uiTmpLine1234_5,uiTmpLine1234_6,uiTmpLine1234_7,uiTmpLine1234_8,uiTmpLine1234_9;
		unsigned int uiTmpLine5678_5,uiTmpLine5678_6,uiTmpLine5678_7,uiTmpLine5678_8,uiTmpLine5678_9;

		unsigned int tmp12,tmp34,tmp56,tmp78,tmp9;

		unsigned int tmpc1_1,tmpc1_2,tmpc2_1,tmpc2_2,tmpc1,tmpc2;
		unsigned int tmpc3_1,tmpc3_2,tmpc4_1,tmpc4_2,tmpc3,tmpc4;
		unsigned int tmpc5_1,tmpc5_2,tmpc6_1,tmpc6_2,tmpc5,tmpc6;
		unsigned int tmpc7_1,tmpc7_2,tmpc8_1,tmpc8_2,tmpc7,tmpc8;
		unsigned int tmpc9_1,tmpc9_2;
		unsigned int tmpl9l,tmpl9h;

		unsigned char* pRefImgPtr;

		pRefImgPtr = refPicLXl-2-(PicWidthSamples<<1);

		uiLine1_1 = _mem4(pRefImgPtr);
		uiLine1_2 = _mem4(pRefImgPtr+4);
		uiLine1_3 = _mem4(pRefImgPtr+8);
		pRefImgPtr += PicWidthSamples;

		uiLine2_1 = _mem4(pRefImgPtr);
		uiLine2_2 = _mem4(pRefImgPtr+4);
		uiLine2_3 = _mem4(pRefImgPtr+8);
		pRefImgPtr += PicWidthSamples;

		uiLine3_1 = _mem4(pRefImgPtr);
		uiLine3_2 = _mem4(pRefImgPtr+4);
		uiLine3_3 = _mem4(pRefImgPtr+8);
		pRefImgPtr += PicWidthSamples;

		uiLine4_1 = _mem4(pRefImgPtr);
		uiLine4_2 = _mem4(pRefImgPtr+4);
		uiLine4_3 = _mem4(pRefImgPtr+8);
		pRefImgPtr += PicWidthSamples;

		uiLine5_1 = _mem4(pRefImgPtr);
		uiLine5_2 = _mem4(pRefImgPtr+4);
		uiLine5_3 = _mem4(pRefImgPtr+8);
		pRefImgPtr += PicWidthSamples;

		uiLine6_1 = _mem4(pRefImgPtr);
		uiLine6_2 = _mem4(pRefImgPtr+4);
		uiLine6_3 = _mem4(pRefImgPtr+8);
		pRefImgPtr += PicWidthSamples;

		uiLine7_1 = _mem4(pRefImgPtr);
		uiLine7_2 = _mem4(pRefImgPtr+4);
		uiLine7_3 = _mem4(pRefImgPtr+8);
		pRefImgPtr += PicWidthSamples;

		uiLine8_1 = _mem4(pRefImgPtr);
		uiLine8_2 = _mem4(pRefImgPtr+4);
		uiLine8_3 = _mem4(pRefImgPtr+8);
		pRefImgPtr += PicWidthSamples;

		uiLine9_1 = _mem4(pRefImgPtr);
		uiLine9_2 = _mem4(pRefImgPtr+4);
		uiLine9_3 = _mem4(pRefImgPtr+8);

		uiTmpLine12_h = _packh4(uiLine1_1,uiLine2_1);
		uiTmpLine34_h = _packh4(uiLine3_1,uiLine4_1);
		uiTmpLine12_l = _packl4(uiLine1_1,uiLine2_1);
		uiTmpLine34_l = _packl4(uiLine3_1,uiLine4_1);
		uiTmpLine1234_4 = _packh4(uiTmpLine12_h,uiTmpLine34_h);
		uiTmpLine1234_2 = _packl4(uiTmpLine12_h,uiTmpLine34_h);
		uiTmpLine1234_3 = _packh4(uiTmpLine12_l,uiTmpLine34_l);
		uiTmpLine1234_1 = _packl4(uiTmpLine12_l,uiTmpLine34_l);

		uiTmpLine56_h = _packh4(uiLine5_1,uiLine6_1);
		uiTmpLine78_h = _packh4(uiLine7_1,uiLine8_1);
		uiTmpLine56_l = _packl4(uiLine5_1,uiLine6_1);
		uiTmpLine78_l = _packl4(uiLine7_1,uiLine8_1);
		uiTmpLine5678_4 = _packh4(uiTmpLine56_h,uiTmpLine78_h);
		uiTmpLine5678_2 = _packl4(uiTmpLine56_h,uiTmpLine78_h);
		uiTmpLine5678_3 = _packh4(uiTmpLine56_l,uiTmpLine78_l);
		uiTmpLine5678_1 = _packl4(uiTmpLine56_l,uiTmpLine78_l);

		uiTmpLine12_h = _packh4(uiLine1_2,uiLine2_2);
		uiTmpLine34_h = _packh4(uiLine3_2,uiLine4_2);
		uiTmpLine12_l = _packl4(uiLine1_2,uiLine2_2);
		uiTmpLine34_l = _packl4(uiLine3_2,uiLine4_2);
		uiTmpLine1234_8 = _packh4(uiTmpLine12_h,uiTmpLine34_h);
		uiTmpLine1234_6 = _packl4(uiTmpLine12_h,uiTmpLine34_h);
		uiTmpLine1234_7 = _packh4(uiTmpLine12_l,uiTmpLine34_l);
		uiTmpLine1234_5 = _packl4(uiTmpLine12_l,uiTmpLine34_l);

		uiTmpLine56_h = _packh4(uiLine5_2,uiLine6_2);
		uiTmpLine78_h = _packh4(uiLine7_2,uiLine8_2);
		uiTmpLine56_l = _packl4(uiLine5_2,uiLine6_2);
		uiTmpLine78_l = _packl4(uiLine7_2,uiLine8_2);
		uiTmpLine5678_8 = _packh4(uiTmpLine56_h,uiTmpLine78_h);
		uiTmpLine5678_6 = _packl4(uiTmpLine56_h,uiTmpLine78_h);
		uiTmpLine5678_7 = _packh4(uiTmpLine56_l,uiTmpLine78_l);
		uiTmpLine5678_5 = _packl4(uiTmpLine56_l,uiTmpLine78_l);


		uiTmpLine12_l = _packl4(uiLine1_3,uiLine2_3);
		uiTmpLine34_l = _packl4(uiLine3_3,uiLine4_3);
		uiTmpLine1234_9 = _packl4(uiTmpLine12_l,uiTmpLine34_l);

		uiTmpLine56_l = _packl4(uiLine5_3,uiLine6_3);
		uiTmpLine78_l = _packl4(uiLine7_3,uiLine8_3);
		uiTmpLine5678_9 = _packl4(uiTmpLine56_l,uiTmpLine78_l);

		// first line
		tmpc1_1 = _dotpsu4(0x01FB1414,uiTmpLine1234_1);
		tmpc1_2 = _dotpsu4(0xFB010000,uiTmpLine5678_1);

		tmpc2_1 = _dotpsu4(0x01FB1414,uiTmpLine1234_2);
		tmpc2_2 = _dotpsu4(0xFB010000,uiTmpLine5678_2);

		tmpc1 = _spack2(tmpc1_1,tmpc2_1);
		tmpc2 = _spack2(tmpc1_2,tmpc2_2);

		tmp12 = _sadd2(tmpc1,tmpc2);

		tmpc3_1 = _dotpsu4(0x01FB1414,uiTmpLine1234_3);
		tmpc3_2 = _dotpsu4(0xFB010000,uiTmpLine5678_3);

		tmpc4_1 = _dotpsu4(0x01FB1414,uiTmpLine1234_4);
		tmpc4_2 = _dotpsu4(0xFB010000,uiTmpLine5678_4);

		tmpc3 = _spack2(tmpc3_1,tmpc4_1);
		tmpc4 = _spack2(tmpc3_2,tmpc4_2);

		tmp34 = _sadd2(tmpc3,tmpc4);

		tmpc5_1 = _dotpsu4(0x01FB1414,uiTmpLine1234_5);
		tmpc5_2 = _dotpsu4(0xFB010000,uiTmpLine5678_5);

		tmpc6_1 = _dotpsu4(0x01FB1414,uiTmpLine1234_6);
		tmpc6_2 = _dotpsu4(0xFB010000,uiTmpLine5678_6);

		tmpc5 = _spack2(tmpc5_1,tmpc6_1);
		tmpc6 = _spack2(tmpc5_2,tmpc6_2);

		tmp56 = _sadd2(tmpc5,tmpc6);

		tmpc7_1 = _dotpsu4(0x01FB1414,uiTmpLine1234_7);
		tmpc7_2 = _dotpsu4(0xFB010000,uiTmpLine5678_7);

		tmpc8_1 = _dotpsu4(0x01FB1414,uiTmpLine1234_8);
		tmpc8_2 = _dotpsu4(0xFB010000,uiTmpLine5678_8);

		tmpc7 = _spack2(tmpc7_1,tmpc8_1);
		tmpc8 = _spack2(tmpc7_2,tmpc8_2);

		tmp78 = _sadd2(tmpc7,tmpc8);

		tmpc9_1 = _dotpsu4(0x01FB1414,uiTmpLine1234_9);
		tmpc9_2 = _dotpsu4(0xFB010000,uiTmpLine5678_9);

		tmp9 = (tmpc9_1 + tmpc9_2) << 16;

		tmpl1_1 = tmp12;
		tmpl1_2 = tmp34;
		tmpl1_3 = tmp56;
		tmpl1_4 = tmp78;
		tmpl1_5 = tmp9;

		// second line
		tmpc1_1 = _dotpsu4(0x0001FB14,uiTmpLine1234_1);
		tmpc1_2 = _dotpsu4(0x14FB0100,uiTmpLine5678_1);

		tmpc2_1 = _dotpsu4(0x0001FB14,uiTmpLine1234_2);
		tmpc2_2 = _dotpsu4(0x14FB0100,uiTmpLine5678_2);

		tmpc1 = _spack2(tmpc1_1,tmpc2_1);
		tmpc2 = _spack2(tmpc1_2,tmpc2_2);

		tmp12 = _sadd2(tmpc1,tmpc2);

		tmpc3_1 = _dotpsu4(0x0001FB14,uiTmpLine1234_3);
		tmpc3_2 = _dotpsu4(0x14FB0100,uiTmpLine5678_3);

		tmpc4_1 = _dotpsu4(0x0001FB14,uiTmpLine1234_4);
		tmpc4_2 = _dotpsu4(0x14FB0100,uiTmpLine5678_4);

		tmpc3 = _spack2(tmpc3_1,tmpc4_1);
		tmpc4 = _spack2(tmpc3_2,tmpc4_2);

		tmp34 = _sadd2(tmpc3,tmpc4);

		tmpc5_1 = _dotpsu4(0x0001FB14,uiTmpLine1234_5);
		tmpc5_2 = _dotpsu4(0x14FB0100,uiTmpLine5678_5);

		tmpc6_1 = _dotpsu4(0x0001FB14,uiTmpLine1234_6);
		tmpc6_2 = _dotpsu4(0x14FB0100,uiTmpLine5678_6);

		tmpc5 = _spack2(tmpc5_1,tmpc6_1);
		tmpc6 = _spack2(tmpc5_2,tmpc6_2);

		tmp56 = _sadd2(tmpc5,tmpc6);

		tmpc7_1 = _dotpsu4(0x0001FB14,uiTmpLine1234_7);
		tmpc7_2 = _dotpsu4(0x14FB0100,uiTmpLine5678_7);

		tmpc8_1 = _dotpsu4(0x0001FB14,uiTmpLine1234_8);
		tmpc8_2 = _dotpsu4(0x14FB0100,uiTmpLine5678_8);

		tmpc7 = _spack2(tmpc7_1,tmpc8_1);
		tmpc8 = _spack2(tmpc7_2,tmpc8_2);

		tmp78 = _sadd2(tmpc7,tmpc8);

		tmpc9_1 = _dotpsu4(0x0001FB14,uiTmpLine1234_9);
		tmpc9_2 = _dotpsu4(0x14FB0100,uiTmpLine5678_9);

		tmp9 = (tmpc9_1 + tmpc9_2) << 16;

		tmpl2_1 = tmp12;
		tmpl2_2 = tmp34;
		tmpl2_3 = tmp56;
		tmpl2_4 = tmp78;
		tmpl2_5 = tmp9;

		// third line
		tmpc1_1 = _dotpsu4(0x000001FB,uiTmpLine1234_1);
		tmpc1_2 = _dotpsu4(0x1414FB01,uiTmpLine5678_1);

		tmpc2_1 = _dotpsu4(0x000001FB,uiTmpLine1234_2);
		tmpc2_2 = _dotpsu4(0x1414FB01,uiTmpLine5678_2);

		tmpc1 = _spack2(tmpc1_1,tmpc2_1);
		tmpc2 = _spack2(tmpc1_2,tmpc2_2);

		tmp12 = _sadd2(tmpc1,tmpc2);

		tmpc3_1 = _dotpsu4(0x000001FB,uiTmpLine1234_3);
		tmpc3_2 = _dotpsu4(0x1414FB01,uiTmpLine5678_3);

		tmpc4_1 = _dotpsu4(0x000001FB,uiTmpLine1234_4);
		tmpc4_2 = _dotpsu4(0x1414FB01,uiTmpLine5678_4);

		tmpc3 = _spack2(tmpc3_1,tmpc4_1);
		tmpc4 = _spack2(tmpc3_2,tmpc4_2);

		tmp34 = _sadd2(tmpc3,tmpc4);

		tmpc5_1 = _dotpsu4(0x000001FB,uiTmpLine1234_5);
		tmpc5_2 = _dotpsu4(0x1414FB01,uiTmpLine5678_5);

		tmpc6_1 = _dotpsu4(0x000001FB,uiTmpLine1234_6);
		tmpc6_2 = _dotpsu4(0x1414FB01,uiTmpLine5678_6);

		tmpc5 = _spack2(tmpc5_1,tmpc6_1);
		tmpc6 = _spack2(tmpc5_2,tmpc6_2);

		tmp56 = _sadd2(tmpc5,tmpc6);

		tmpc7_1 = _dotpsu4(0x000001FB,uiTmpLine1234_7);
		tmpc7_2 = _dotpsu4(0x1414FB01,uiTmpLine5678_7);

		tmpc8_1 = _dotpsu4(0x000001FB,uiTmpLine1234_8);
		tmpc8_2 = _dotpsu4(0x1414FB01,uiTmpLine5678_8);

		tmpc7 = _spack2(tmpc7_1,tmpc8_1);
		tmpc8 = _spack2(tmpc7_2,tmpc8_2);

		tmp78 = _sadd2(tmpc7,tmpc8);

		tmpc9_1 = _dotpsu4(0x000001FB,uiTmpLine1234_9);
		tmpc9_2 = _dotpsu4(0x1414FB01,uiTmpLine5678_9);

		tmp9 = (tmpc9_1 + tmpc9_2) << 16;

		tmpl3_1 = tmp12;
		tmpl3_2 = tmp34;
		tmpl3_3 = tmp56;
		tmpl3_4 = tmp78;
		tmpl3_5 = tmp9;



		// fourth line
		tmpc1_1 = _dotpsu4(0x00000001,uiTmpLine1234_1);
		tmpc1_2 = _dotpsu4(0xFB1414FB,uiTmpLine5678_1);

		tmpc2_1 = _dotpsu4(0x00000001,uiTmpLine1234_2);
		tmpc2_2 = _dotpsu4(0xFB1414FB,uiTmpLine5678_2);

		uiLine9_1 = _swap4(uiLine9_1);
		tmpl9h = _unpkhu4 (uiLine9_1);
		tmpl9l = _unpklu4 (uiLine9_1);

		tmpc1 = _spack2(tmpc1_1,tmpc2_1);
		tmpc2 = _spack2(tmpc1_2,tmpc2_2);

		tmp12 = _sadd2(tmpc1,tmpc2);
		tmp12 = _sadd2(tmp12,tmpl9l);

		tmpc3_1 = _dotpsu4(0x00000001,uiTmpLine1234_3);
		tmpc3_2 = _dotpsu4(0xFB1414FB,uiTmpLine5678_3);

		tmpc4_1 = _dotpsu4(0x00000001,uiTmpLine1234_4);
		tmpc4_2 = _dotpsu4(0xFB1414FB,uiTmpLine5678_4);

		tmpc3 = _spack2(tmpc3_1,tmpc4_1);
		tmpc4 = _spack2(tmpc3_2,tmpc4_2);

		tmp34 = _sadd2(tmpc3,tmpc4);
		tmp34 = _sadd2(tmp34,tmpl9h);


		uiLine9_2 = _swap4(uiLine9_2);
		tmpl9h = _unpkhu4 (uiLine9_2);
		tmpl9l = _unpklu4 (uiLine9_2);

		tmpc5_1 = _dotpsu4(0x00000001,uiTmpLine1234_5);
		tmpc5_2 = _dotpsu4(0xFB1414FB,uiTmpLine5678_5);

		tmpc6_1 = _dotpsu4(0x00000001,uiTmpLine1234_6);
		tmpc6_2 = _dotpsu4(0xFB1414FB,uiTmpLine5678_6);

		tmpc5 = _spack2(tmpc5_1,tmpc6_1);
		tmpc6 = _spack2(tmpc5_2,tmpc6_2);

		tmp56 = _sadd2(tmpc5,tmpc6);
		tmp56 = _sadd2(tmp56,tmpl9l);

		tmpc7_1 = _dotpsu4(0x00000001,uiTmpLine1234_7);
		tmpc7_2 = _dotpsu4(0xFB1414FB,uiTmpLine5678_7);

		tmpc8_1 = _dotpsu4(0x00000001,uiTmpLine1234_8);
		tmpc8_2 = _dotpsu4(0xFB1414FB,uiTmpLine5678_8);

		tmpc7 = _spack2(tmpc7_1,tmpc8_1);
		tmpc8 = _spack2(tmpc7_2,tmpc8_2);

		tmp78 = _sadd2(tmpc7,tmpc8);
		tmp78 = _sadd2(tmp78,tmpl9h);

		tmpc9_1 = _dotpsu4(0x00000001,uiTmpLine1234_9);
		tmpc9_2 = _dotpsu4(0xFB1414FB,uiTmpLine5678_9);

		tmp9 = (tmpc9_1 + tmpc9_2 + _extu(uiLine9_3,24,24)) << 16;

		tmpl4_1 = tmp12;
		tmpl4_2 = tmp34;
		tmpl4_3 = tmp56;
		tmpl4_4 = tmp78;
		tmpl4_5 = tmp9;
	}
		// Input for last avgu4
	{
	
	
		unsigned int avgtmp1 = _shr2(_sadd2(_packlh2(tmpl1_2,tmpl1_3),0x00100010),5);
		unsigned int avgtmp2 = _shr2(_sadd2(_packlh2(tmpl1_3,tmpl1_4),0x00100010),5);
		input1 = _spacku4(avgtmp2,avgtmp1);
		input1 = _swap4(input1);

		avgtmp1 = _shr2(_sadd2(_packlh2(tmpl2_2,tmpl2_3),0x00100010),5);
		avgtmp2 = _shr2(_sadd2(_packlh2(tmpl2_3,tmpl2_4),0x00100010),5);
		input2 = _spacku4(avgtmp2,avgtmp1);
		input2 = _swap4(input2);

		avgtmp1 = _shr2(_sadd2(_packlh2(tmpl3_2,tmpl3_3),0x00100010),5);
		avgtmp2 = _shr2(_sadd2(_packlh2(tmpl3_3,tmpl3_4),0x00100010),5);
		input3 = _spacku4(avgtmp2,avgtmp1);
		input3 = _swap4(input3);

		avgtmp1 = _shr2(_sadd2(_packlh2(tmpl4_2,tmpl4_3),0x00100010),5);
		avgtmp2 = _shr2(_sadd2(_packlh2(tmpl4_3,tmpl4_4),0x00100010),5);
		input4 = _spacku4(avgtmp2,avgtmp1);
		input4 = _swap4(input4);
	}

	// intermediary results are stored in tmpli_i registers

	{
		unsigned int BC,ED,AAplFF,BCplED,res1_12,res1_34,spl1mpy,spl2mpy,spl3mpy,spl4mpy,tmpend1,tmpend2,tmpend3,tmpend4;
		unsigned int res2_12,res2_34,res3_12,res3_34,res4_12,res4_34;
		unsigned char* pImgPtr;

		// First line
		// first value
		BC = _packlh2(tmpl1_1,tmpl1_2);
		ED = _packhl2(tmpl1_3,tmpl1_2);
		BCplED = _sadd2(BC,ED);
		spl1mpy = _dotp2(0xFFFB0014,BCplED);


		AAplFF = _sadd2(tmpl1_1,_packlh2(tmpl1_3,tmpl1_4));
		AAplFF = _sadd2(0x02000200,AAplFF);
		spl1mpy +=  _ext(AAplFF,0,16);
		spl1mpy >>= 10;

		// second value
		BC = tmpl1_2;
		ED = _packlh2(tmpl1_3,tmpl1_3);
		BCplED = _sadd2(BC,ED);
		spl2mpy = _dotp2(0xFFFB0014,BCplED);

		spl2mpy +=  _ext(AAplFF,16,16);
		spl2mpy >>= 10;

		res1_12 = _pack2(spl1mpy,spl2mpy);
		// third value
		BC = _packlh2(tmpl1_2,tmpl1_3);
		ED = _packhl2(tmpl1_4,tmpl1_3);
		BCplED = _sadd2(BC,ED);
		spl3mpy = _dotp2(0xFFFB0014,BCplED);


		AAplFF = _sadd2(tmpl1_2,_packlh2(tmpl1_4,tmpl1_5));
		AAplFF = _sadd2(0x02000200,AAplFF);
		spl3mpy +=  _ext(AAplFF,0,16);
		spl3mpy >>= 10;

		// fourth value
		BC = tmpl1_3;
		ED = _packlh2(tmpl1_4,tmpl1_4);
		BCplED = _sadd2(BC,ED);
		spl4mpy = _dotp2(0xFFFB0014,BCplED);

		spl4mpy +=  _ext(AAplFF,16,16);
		spl4mpy >>= 10;

		res1_34 = _pack2(spl3mpy,spl4mpy);

		tmpend1 = _spacku4(res1_34,res1_12);
		tmpend1 = _swap4(tmpend1);


		pImgPtr = image;
		_amem4(pImgPtr) = _avgu4(tmpend1,input1);

		// Second line
		// first value
		BC = _packlh2(tmpl2_1,tmpl2_2);
		ED = _packhl2(tmpl2_3,tmpl2_2);
		BCplED = _sadd2(BC,ED);
		spl1mpy = _dotp2(0xFFFB0014,BCplED);


		AAplFF = _sadd2(tmpl2_1,_packlh2(tmpl2_3,tmpl2_4));
		AAplFF = _sadd2(0x02000200,AAplFF);
		spl1mpy +=  _ext(AAplFF,0,16);
		spl1mpy >>= 10;

		// second value
		BC = tmpl2_2;
		ED = _packlh2(tmpl2_3,tmpl2_3);
		BCplED = _sadd2(BC,ED);
		spl2mpy = _dotp2(0xFFFB0014,BCplED);

		spl2mpy +=  _ext(AAplFF,16,16);
		spl2mpy >>= 10;

		res2_12 = _pack2(spl1mpy,spl2mpy);
		// third value
		BC = _packlh2(tmpl2_2,tmpl2_3);
		ED = _packhl2(tmpl2_4,tmpl2_3);
		BCplED = _sadd2(BC,ED);
		spl3mpy = _dotp2(0xFFFB0014,BCplED);


		AAplFF = _sadd2(tmpl2_2,_packlh2(tmpl2_4,tmpl2_5));
		AAplFF = _sadd2(0x02000200,AAplFF);
		spl3mpy +=  _ext(AAplFF,0,16);
		spl3mpy >>= 10;

		// fourth value
		BC = tmpl2_3;
		ED = _packlh2(tmpl2_4,tmpl2_4);
		BCplED = _sadd2(BC,ED);
		spl4mpy = _dotp2(0xFFFB0014,BCplED);

		spl4mpy +=  _ext(AAplFF,16,16);
		spl4mpy >>= 10;

		res2_34 = _pack2(spl3mpy,spl4mpy);

		tmpend2 = _spacku4(res2_34,res2_12);
		tmpend2 = _swap4(tmpend2);

		pImgPtr += stride;
		_amem4(pImgPtr) =  _avgu4(tmpend2,input2);

		// Third line
		// first value
		BC = _packlh2(tmpl3_1,tmpl3_2);
		ED = _packhl2(tmpl3_3,tmpl3_2);
		BCplED = _sadd2(BC,ED);
		spl1mpy = _dotp2(0xFFFB0014,BCplED);


		AAplFF = _sadd2(tmpl3_1,_packlh2(tmpl3_3,tmpl3_4));
		AAplFF = _sadd2(0x02000200,AAplFF);
		spl1mpy +=  _ext(AAplFF,0,16);
		spl1mpy >>= 10;

		// second value
		BC = tmpl3_2;
		ED = _packlh2(tmpl3_3,tmpl3_3);
		BCplED = _sadd2(BC,ED);
		spl2mpy = _dotp2(0xFFFB0014,BCplED);

		spl2mpy +=  _ext(AAplFF,16,16);
		spl2mpy >>= 10;

		res3_12 = _pack2(spl1mpy,spl2mpy);
		// third value
		BC = _packlh2(tmpl3_2,tmpl3_3);
		ED = _packhl2(tmpl3_4,tmpl3_3);
		BCplED = _sadd2(BC,ED);
		spl3mpy = _dotp2(0xFFFB0014,BCplED);


		AAplFF = _sadd2(tmpl3_2,_packlh2(tmpl3_4,tmpl3_5));
		AAplFF = _sadd2(0x02000200,AAplFF);
		spl3mpy +=  _ext(AAplFF,0,16);
		spl3mpy >>= 10;

		// fourth value
		BC = tmpl3_3;
		ED = _packlh2(tmpl3_4,tmpl3_4);
		BCplED = _sadd2(BC,ED);
		spl4mpy = _dotp2(0xFFFB0014,BCplED);

		spl4mpy +=  _ext(AAplFF,16,16);
		spl4mpy >>= 10;

		res3_34 = _pack2(spl3mpy,spl4mpy);

		tmpend3 = _spacku4(res3_34,res3_12);
		tmpend3 = _swap4(tmpend3);

		pImgPtr += stride;
		_amem4(pImgPtr) = _avgu4(tmpend3,input3);

		// Fourth line
		// first value
		BC = _packlh2(tmpl4_1,tmpl4_2);
		ED = _packhl2(tmpl4_3,tmpl4_2);
		BCplED = _sadd2(BC,ED);
		spl1mpy = _dotp2(0xFFFB0014,BCplED);


		AAplFF = _sadd2(tmpl4_1,_packlh2(tmpl4_3,tmpl4_4));
		AAplFF = _sadd2(0x02000200,AAplFF);
		spl1mpy +=  _ext(AAplFF,0,16);
		spl1mpy >>= 10;

		// second value
		BC = tmpl4_2;
		ED = _packlh2(tmpl4_3,tmpl4_3);
		BCplED = _sadd2(BC,ED);
		spl2mpy = _dotp2(0xFFFB0014,BCplED);

		spl2mpy +=  _ext(AAplFF,16,16);
		spl2mpy >>= 10;

		res4_12 = _pack2(spl1mpy,spl2mpy);
		// third value
		BC = _packlh2(tmpl4_2,tmpl4_3);
		ED = _packhl2(tmpl4_4,tmpl4_3);
		BCplED = _sadd2(BC,ED);
		spl3mpy = _dotp2(0xFFFB0014,BCplED);


		AAplFF = _sadd2(tmpl4_2,_packlh2(tmpl4_4,tmpl4_5));
		AAplFF = _sadd2(0x02000200,AAplFF);
		spl3mpy +=  _ext(AAplFF,0,16);
		spl3mpy >>= 10;

		// fourth value
		BC = tmpl4_3;
		ED = _packlh2(tmpl4_4,tmpl4_4);
		BCplED = _sadd2(BC,ED);
		spl4mpy = _dotp2(0xFFFB0014,BCplED);

		spl4mpy +=  _ext(AAplFF,16,16);
		spl4mpy >>= 10;

		res4_34 = _pack2(spl3mpy,spl4mpy);

		tmpend4 = _spacku4(res4_34,res4_12);
		tmpend4 = _swap4(tmpend4);

		pImgPtr += stride;
		_amem4(pImgPtr) = _avgu4(tmpend4,input4);
		
	}
	
}


#endif
