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


#ifndef ICT_SVC
#define ICT_SVC

#ifdef TI_OPTIM
#define SVC4x4Add SVC4x4Add_TI
#else
#define SVC4x4Add SVC4x4Add_C
#endif




void base_4x4_chroma( const short * RESTRICT  block, short * RESTRICT  dst,const short PicWidthInPix);

#ifdef TI_OPTIM
static __inline void SVC4x4Add_TI ( short * RESTRICT  block, short *image,const int PicWidthInPix)
{
	unsigned int uiTabTmp0,uiTabTmp1,uiTabTmp2,uiTabTmp3,uiTabTmp4,uiTabTmp5,uiTabTmp6,uiTabTmp7;

	block [0] += 32;

	{
		unsigned int uiB1B0,uiB3B2,uiZ3Z0,uiZ2Z1,uiZ0Z1,uiZ3Z2;
		short * psBlock = block;

		// first 4 values
		uiB1B0 = _amem4(psBlock);
		psBlock += 2;
		uiB3B2 = _amem4(psBlock);
		psBlock += 2;

		uiZ3Z0 = _add2(uiB1B0,_extu(uiB3B2,16,16)+((((int)uiB3B2)>>1)&0xffff0000));
		uiZ2Z1 = _sub2(_extu(uiB1B0,16,16)+((((int)uiB1B0)>>1)&0xffff0000),uiB3B2);
		uiZ0Z1 = _pack2(uiZ3Z0,uiZ2Z1);
		uiZ3Z2 = _packh2(uiZ3Z0,uiZ2Z1);

		uiTabTmp0 = _add2(uiZ0Z1,uiZ3Z2);
		uiTabTmp0 = _packlh2(uiTabTmp0,uiTabTmp0);
		uiTabTmp1 = _sub2(uiZ0Z1,uiZ3Z2);

		// second 4 values
		uiB1B0 = _amem4(psBlock);
		psBlock += 2;
		uiB3B2 = _amem4(psBlock);
		psBlock += 2;

		uiZ3Z0 = _add2(uiB1B0,_extu(uiB3B2,16,16)+((((int)uiB3B2)>>1)&0xffff0000));
		uiZ2Z1 = _sub2(_extu(uiB1B0,16,16)+((((int)uiB1B0)>>1)&0xffff0000),uiB3B2);
		uiZ0Z1 = _pack2(uiZ3Z0,uiZ2Z1);
		uiZ3Z2 = _packh2(uiZ3Z0,uiZ2Z1);

		uiTabTmp2 = _add2(uiZ0Z1,uiZ3Z2);
		uiTabTmp2 = _packlh2(uiTabTmp2,uiTabTmp2);
		uiTabTmp3 = _sub2(uiZ0Z1,uiZ3Z2);

		// third 4 values
		uiB1B0 = _amem4(psBlock);
		psBlock += 2;
		uiB3B2 = _amem4(psBlock);
		psBlock += 2;

		uiZ3Z0 = _add2(uiB1B0,_extu(uiB3B2,16,16)+((((int)uiB3B2)>>1)&0xffff0000));
		uiZ2Z1 = _sub2(_extu(uiB1B0,16,16)+((((int)uiB1B0)>>1)&0xffff0000),uiB3B2);
		uiZ0Z1 = _pack2(uiZ3Z0,uiZ2Z1);
		uiZ3Z2 = _packh2(uiZ3Z0,uiZ2Z1);

		uiTabTmp4 = _add2(uiZ0Z1,uiZ3Z2);
		uiTabTmp4 = _packlh2(uiTabTmp4,uiTabTmp4);
		uiTabTmp5 = _sub2(uiZ0Z1,uiZ3Z2);

		// fourth 4 values
		uiB1B0 = _amem4(psBlock);
		psBlock += 2;
		uiB3B2 = _amem4(psBlock);
		psBlock += 2;

		uiZ3Z0 = _add2(uiB1B0,_extu(uiB3B2,16,16)+((((int)uiB3B2)>>1)&0xffff0000));
		uiZ2Z1 = _sub2(_extu(uiB1B0,16,16)+((((int)uiB1B0)>>1)&0xffff0000),uiB3B2);
		uiZ0Z1 = _pack2(uiZ3Z0,uiZ2Z1);
		uiZ3Z2 = _packh2(uiZ3Z0,uiZ2Z1);

		uiTabTmp6 = _add2(uiZ0Z1,uiZ3Z2);
		uiTabTmp6 = _packlh2(uiTabTmp6,uiTabTmp6);
		uiTabTmp7 = _sub2(uiZ0Z1,uiZ3Z2);
	}

	{
		unsigned int AH,AL,BH,BL2,CH,CL,DH,DL;

		int inc = PicWidthInPix>>1;
		unsigned int* pImg = ((unsigned int*)&image [0]);

		unsigned int uiZ0Z0,uiZ3Z3,uiZ1Z1,uiZ2Z2;
		unsigned int ATmp,BTmp,CTmp,DTmp;

		ATmp = _amem4(pImg+1); 
		BTmp = _amem4(pImg+1+inc); 
		CTmp = _amem4(pImg+1+2*inc); 
		DTmp = _amem4(pImg+1+3*inc); 

		uiZ0Z0 = _add2(uiTabTmp0,uiTabTmp4);
		uiZ3Z3 = _add2(uiTabTmp2,_shr2(uiTabTmp6,1));
		uiZ1Z1 = _sub2(uiTabTmp0,uiTabTmp4);
		uiZ2Z2 = _sub2(_shr2(uiTabTmp2,1),uiTabTmp6);

		AL = _shr2(_add2(uiZ0Z0,uiZ3Z3),6);
		BL2 = _shr2(_add2(uiZ1Z1,uiZ2Z2),6);
		CL = _shr2(_sub2(uiZ1Z1,uiZ2Z2),6);
		DL = _shr2(_sub2(uiZ0Z0,uiZ3Z3),6);


		AL = _add2(AL,ATmp);
		BL2 = _add2(BL2,BTmp);
		CL = _add2(CL,CTmp);
		DL = _add2(DL,DTmp);

		uiZ0Z0 = _add2(uiTabTmp1,uiTabTmp5);
		uiZ3Z3 = _add2(uiTabTmp3,_shr2(uiTabTmp7,1));
		uiZ1Z1 = _sub2(uiTabTmp1,uiTabTmp5);
		uiZ2Z2 = _sub2(_shr2(uiTabTmp3,1),uiTabTmp7);

		AH = _shr2(_add2(uiZ0Z0,uiZ3Z3),6);
		BH = _shr2(_add2(uiZ1Z1,uiZ2Z2),6);
		CH = _shr2(_sub2(uiZ1Z1,uiZ2Z2),6);
		DH = _shr2(_sub2(uiZ0Z0,uiZ3Z3),6);

		ATmp = _amem4(pImg); 
		BTmp = _amem4(pImg+inc); 
		CTmp = _amem4(pImg+2*inc); 
		DTmp = _amem4(pImg+3*inc);


		AH = _add2(AH,ATmp);
		BH = _add2(BH,BTmp);
		CH = _add2(CH,CTmp);
		DH = _add2(DH,DTmp);


		_amem4(pImg)=AL;
		_amem4(pImg+1)=AH;
		_amem4(pImg+inc)=BL2;
		_amem4(pImg+inc+1)=BH;
		_amem4(pImg+2*inc)=CL;
		_amem4(pImg+2*inc+1)=CH;
		_amem4(pImg+3*inc)=DL;
		_amem4(pImg+3*inc+1)=DH;
	}
}
#endif

static __inline void SVC4x4Add_C ( short * RESTRICT  block, short *image,const int PicWidthInPix)
{

	__declspec(align(16)) short BlockTemp[16];

	int     i, j = 0 ;

	block [0] += 32;


	for ( i = 0 ; i < 4 ; i++ ) {
		const short   z0 = block [j] + block [2 + j];
		const short   z1 = block [j] - block [2 + j];
		const short   z2 = (block [1 + j] >> 1) - block [3 + j];
		const short   z3 = block [1 + j] + (block [3 + j] >> 1);
		BlockTemp [j + 0] = z0 + z3 ;
		BlockTemp [j + 1] = z1 + z2 ;
		BlockTemp [j + 2] = z1 - z2 ;
		BlockTemp [j + 3] = z0 - z3 ;
		j += 4 ;
	}

	for ( i = 0 ; i < 4 ; i++ ) {
		const short   z0 = BlockTemp [i] + BlockTemp [8 + i];
		const short   z1 = BlockTemp [i] - BlockTemp [8 + i];
		const short   z2 = (BlockTemp [4 + i] >> 1) - BlockTemp [12 + i];
		const short   z3 = BlockTemp [4 + i] + (BlockTemp [12 + i] >> 1);
		image [i] += ((z0 + z3) >> 6);
		image [PicWidthInPix + i] += ((z1 + z2) >> 6);
		image [2*PicWidthInPix + i] += ((z1 - z2) >> 6);
		image [3*PicWidthInPix + i] += ((z0 - z3) >> 6);
	}
}




static __inline void SVC4x4Store (short * RESTRICT  block, short *image,const int PicWidthInPix)
{
	__declspec(align(16)) short BlockTemp[16];

	int     i, j = 0 ;

	block [0] += 32;


	for ( i = 0 ; i < 4 ; i++ ) {
		const short   z0 = block [j] + block [2 + j];
		const short   z1 = block [j] - block [2 + j];
		const short   z2 = (block [1 + j] >> 1) - block [3 + j];
		const short   z3 = block [1 + j] + (block [3 + j] >> 1);
		BlockTemp [j + 0] = z0 + z3 ;
		BlockTemp [j + 1] = z1 + z2 ;
		BlockTemp [j + 2] = z1 - z2 ;
		BlockTemp [j + 3] = z0 - z3 ;
		j += 4 ;
	}

	for ( i = 0 ; i < 4 ; i++ ) {
		const short   z0 = BlockTemp [i] + BlockTemp [8 + i];
		const short   z1 = BlockTemp [i] - BlockTemp [8 + i];
		const short   z2 = (BlockTemp [4 + i] >> 1) - BlockTemp [12 + i];
		const short   z3 = BlockTemp [4 + i] + (BlockTemp [12 + i] >> 1);
		image [i]					= ((z0 + z3) >> 6);
		image [PicWidthInPix + i]	= ((z1 + z2) >> 6);
		image [2*PicWidthInPix + i] = ((z1 - z2) >> 6);
		image [3*PicWidthInPix + i] = ((z0 - z3) >> 6);
	}
}





static __inline void SVC8x8Add(short *block, short *image, const int PicWidthInPix){
	int i,j = 0;


	block[0] += 32;

	for( i = 0; i < 8; i++ )
	{
		const short a0 =  block[j] + block[j + 4];
		const short a2 =  block[j] - block[j + 4];
		const short a4 = (block[j + 2]>>1) - block[j + 6];
		const short a6 = (block[j + 6]>>1) + block[j + 2];

		const short b0 = a0 + a6;
		const short b2 = a2 + a4;
		const short b4 = a2 - a4;
		const short b6 = a0 - a6;

		const short a1 = -block[j + 3] + block[j + 5] - block[j + 7] - (block[j + 7]>>1);
		const short a3 =  block[j + 1] + block[j + 7] - block[j + 3] - (block[j + 3]>>1);
		const short a5 = -block[j + 1] + block[j + 7] + block[j + 5] + (block[j + 5]>>1);
		const short a7 =  block[j + 3] + block[j + 5] + block[j + 1] + (block[j + 1]>>1);

		const short b1 = (a7>>2) + a1;
		const short b3 =  a3 + (a5>>2);
		const short b5 = (a3>>2) - a5;
		const short b7 =  a7 - (a1>>2);

		block[j + 0] = b0 + b7;
		block[j + 7] = b0 - b7;
		block[j + 1] = b2 + b5;
		block[j + 6] = b2 - b5;
		block[j + 2] = b4 + b3;
		block[j + 5] = b4 - b3;
		block[j + 3] = b6 + b1;
		block[j + 4] = b6 - b1;

		j += 8;
	}


	for( i = 0; i < 8; i++ )
	{
		const short a0 =  block[0 + i] + block[4*8 + i];
		const short a2 =  block[0 + i] - block[4*8 + i];
		const short a4 = (block[2*8 + i]>>1) - block[6*8 + i];
		const short a6 = (block[6*8 + i]>>1) + block[2*8 + i];

		const short b0 = a0 + a6;
		const short b2 = a2 + a4;
		const short b4 = a2 - a4;
		const short b6 = a0 - a6;

		const short a1 = -block[3*8 + i] + block[5*8 + i] - block[7*8 + i] - (block[7*8 + i]>>1);
		const short a3 =  block[1*8 + i] + block[7*8 + i] - block[3*8 + i] - (block[3*8 + i]>>1);
		const short a5 = -block[1*8 + i] + block[7*8 + i] + block[5*8 + i] + (block[5*8 + i]>>1);
		const short a7 =  block[3*8 + i] + block[5*8 + i] + block[1*8 + i] + (block[1*8 + i]>>1);

		const short b1 = (a7>>2) + a1;
		const short b3 =  a3 + (a5>>2);
		const short b5 = (a3>>2) - a5;
		const short b7 =  a7 - (a1>>2);

		image [i]						 += ((b0 + b7) >> 6);
		image [PicWidthInPix + i]		 += ((b2 + b5) >> 6);
		image [(PicWidthInPix << 1) + i] += ((b4 + b3) >> 6);
		image [3 * PicWidthInPix + i]	 += ((b6 + b1) >> 6);
		image [(PicWidthInPix << 2) + i] += ((b6 - b1) >> 6);
		image [5 * PicWidthInPix + i]	 += ((b4 - b3) >> 6);
		image [6 * PicWidthInPix + i]	 += ((b2 - b5) >> 6);
		image [7 * PicWidthInPix + i]	 += ((b0 - b7) >> 6);
	}
}





static __inline void SVC8x8Store(short *block, short *image, const int PicWidthInPix){
	int i,j = 0;


	block[0] += 32;

	for( i = 0; i < 8; i++ )
	{
		const short a0 =  block[j] + block[j + 4];
		const short a2 =  block[j] - block[j + 4];
		const short a4 = (block[j + 2]>>1) - block[j + 6];
		const short a6 = (block[j + 6]>>1) + block[j + 2];

		const short b0 = a0 + a6;
		const short b2 = a2 + a4;
		const short b4 = a2 - a4;
		const short b6 = a0 - a6;

		const short a1 = -block[j + 3] + block[j + 5] - block[j + 7] - (block[j + 7]>>1);
		const short a3 =  block[j + 1] + block[j + 7] - block[j + 3] - (block[j + 3]>>1);
		const short a5 = -block[j + 1] + block[j + 7] + block[j + 5] + (block[j + 5]>>1);
		const short a7 =  block[j + 3] + block[j + 5] + block[j + 1] + (block[j + 1]>>1);

		const short b1 = (a7>>2) + a1;
		const short b3 =  a3 + (a5>>2);
		const short b5 = (a3>>2) - a5;
		const short b7 =  a7 - (a1>>2);

		block[j + 0] = b0 + b7;
		block[j + 7] = b0 - b7;
		block[j + 1] = b2 + b5;
		block[j + 6] = b2 - b5;
		block[j + 2] = b4 + b3;
		block[j + 5] = b4 - b3;
		block[j + 3] = b6 + b1;
		block[j + 4] = b6 - b1;

		j += 8;
	}
	for( i = 0; i < 8; i++ )
	{
		const short a0 =  block[0 + i] + block[4*8 + i];
		const short a2 =  block[0 + i] - block[4*8 + i];
		const short a4 = (block[2*8 + i]>>1) - block[6*8 + i];
		const short a6 = (block[6*8 + i]>>1) + block[2*8 + i];

		const short b0 = a0 + a6;
		const short b2 = a2 + a4;
		const short b4 = a2 - a4;
		const short b6 = a0 - a6;

		const short a1 = -block[3*8 + i] + block[5*8 + i] - block[7*8 + i] - (block[7*8 + i]>>1);
		const short a3 =  block[1*8 + i] + block[7*8 + i] - block[3*8 + i] - (block[3*8 + i]>>1);
		const short a5 = -block[1*8 + i] + block[7*8 + i] + block[5*8 + i] + (block[5*8 + i]>>1);
		const short a7 =  block[3*8 + i] + block[5*8 + i] + block[1*8 + i] + (block[1*8 + i]>>1);

		const short b1 = (a7>>2) + a1;
		const short b3 =  a3 + (a5>>2);
		const short b5 = (a3>>2) - a5;
		const short b7 =  a7 - (a1>>2);

		image [i]						 = ((b0 + b7) >> 6);
		image [PicWidthInPix + i]		 = ((b2 + b5) >> 6);
		image [(PicWidthInPix << 1) + i] = ((b4 + b3) >> 6);
		image [3 * PicWidthInPix + i]	 = ((b6 + b1) >> 6);
		image [(PicWidthInPix << 2) + i] = ((b6 - b1) >> 6);
		image [5 * PicWidthInPix + i]	 = ((b4 - b3) >> 6);
		image [6 * PicWidthInPix + i]	 = ((b2 - b5) >> 6);
		image [7 * PicWidthInPix + i]	 = ((b0 - b7) >> 6);
	}
}



#endif
