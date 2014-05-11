/*****************************************************************************
*
*  Open SVC Decoder developped in IETR image lab
*
*
*
*              Médéric Blestel <mblestel@insa-rennes.Fr>
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



#include <stdio.h>

//AVC files
#include "type.h"
#include "data.h"
#include "clip.h"

//SVC files
#include "svc_type.h"
#include "svc_data.h"
#include "mb_padding.h"
#include "mb_upsample_filter.h"


#ifndef TI_OPTIM //ESB
//Constants
static const short filter16[16][4] = {{0,32,0,0},
{-1,32,2,-1},
{-2,31,4,-1},
{-3,30,6,-1},
{-3,28,8,-1},
{-4,26,11,-1},
{-4,24,14,-2},
{-3,22,16,-3},
{-3,19,19,-3},
{-3,16,22,-3},
{-2,14,24,-4},
{-1,11,26,-4},
{-1,8,28,-3},
{-1,6,30,-3},
{-1,4,31,-2},
{-1,2,32,-1}
};

static const short filter16_chroma[16][2] = { // bilinear
	{32,0},
	{30,2},
	{28,4},
	{26,6},
	{24,8},
	{22,10},
	{20,12},
	{18,14},
	{16,16},
	{14,18},
	{12,20},
	{10,22},
	{8,24},
	{6,26},
	{4,28},
	{2,30}
};



/**
Upsample a macroblock's luminance sample.
*/
void upsample_mb_luninance(const SPS *sps, const NAL *Nal, unsigned char *Y, unsigned char *aio_tucImage, 
						   const short x_base, const short y_base, const short x, const short y)
{


	int i, j;
	short    px [32];
	short    py [32];
	short    temp [25 * 32];//24*32
	const short base_PicWidthInPix = Nal -> down_Width;
	const short Curr_PicWidthInPix = Nal -> up_Width;
	const short base_PicHeightInPix = Nal -> down_Height;
	const short Curr_PicHeightInPix = Nal -> up_Height;


	const short shiftX = ShiftCalculation(sps -> level_idc, base_PicWidthInPix);
	const short shiftY = ShiftCalculation(sps -> level_idc, base_PicHeightInPix);
	const int Xdeltaa = ((base_PicWidthInPix << shiftX) + (Curr_PicWidthInPix >> 1)) / Curr_PicWidthInPix;
	const int Ydeltaa = ((base_PicHeightInPix << shiftY) + (Curr_PicHeightInPix >> 1)) / Curr_PicHeightInPix;
	const int Xdeltab = (((base_PicWidthInPix * 2) << (shiftX - 2)) + (Curr_PicWidthInPix >> 1)) / Curr_PicWidthInPix + (1 << (shiftX - 5));
	const int Ydeltab = (((base_PicHeightInPix * 2) << (shiftY - 2)) + (Curr_PicHeightInPix >> 1)) / Curr_PicHeightInPix + (1 << (shiftY - 5));

	for(i = 0; i <  32; i++){
		px[i] = (short) ((((i + x - (sps -> scaled_base_left_offset << 1)) * Xdeltaa + Xdeltab) >> (shiftX - 4)) - 8);
		py[i] = (short) ((((i + y - (sps -> scaled_base_top_offset << 1)) * Ydeltaa + Ydeltab) >> (shiftY - 4)) - 8);
	}




	for( j = 0; j < 24; j++ )   {
		unsigned char *ptr = &aio_tucImage[j * LumaStride];
		short *Tmp = &temp[j * 32];

		for( i = 0; i < 24; i++ ){
			const short pos16 = px[i] & 0x0f;
			const short pos = (px[i] >> 4) - 1 - x_base;
			const short *Filter = &filter16[pos16][0];

			const short m = CLIP3(-4, 31, pos); 
			Tmp[i] = Filter[0] * ptr[m] + Filter[1] * ptr[m + 1] + Filter[2] * ptr[m + 2] + Filter[3] * ptr[m + 3];
		}
	}


	for( i = 0; i < 16; i++ )   {
		short *Tmp = &temp[i + 4 * 32];
		unsigned char *ptr_img = &Y[i];
		for( j = 0; j < 16; j++ ){
			const short pos16 = py[j] & 0x0f;
			const short pos = (py[j] >> 4) - 1 - y_base;
			const short *Filter = &filter16[pos16][0];

			const short m = CLIP3(-4, 17, pos);
			int tmp_result = Filter[0] * Tmp[m  * 32] + Filter[1] * Tmp[(m + 1) * 32] + Filter[2] * Tmp[(m + 2) * 32] + Filter[3] * Tmp[(m  + 3) * 32];
			ptr_img[j * (Nal -> curr_Width + 32)] = (unsigned char) CLIP255((tmp_result + 512) >> 10);
		}
	}
}



/**
Upsample a macroblock's chrominance sample.
*/
void upsample_mb_chroma(const SPS *sps,const NAL *Nal, unsigned char *Y, unsigned char *aio_tucImage, 
						const short x_base, const short y_base, const short x, const short y)
{

	short i, j;
	short px [16];
	short py [16];
	short temp [12 * 16];

	const short base_PicWidthInPix = Nal -> down_Width >> 1;
	const short Curr_PicWidthInPix = Nal -> up_Width >> 1;
	const short base_PicHeightInPix = Nal -> down_Height >> 1;
	const short Curr_PicHeightInPix = Nal -> up_Height >> 1;

	const short shiftX = ShiftCalculation(sps -> level_idc, base_PicWidthInPix);
	const short shiftY = ShiftCalculation(sps -> level_idc, base_PicHeightInPix);

	const int Xdeltaa = ((base_PicWidthInPix << shiftX) + (Curr_PicWidthInPix >> 1)) / Curr_PicWidthInPix;
	const int Ydeltaa = ((base_PicHeightInPix << shiftY) + (Curr_PicHeightInPix >> 1)) / Curr_PicHeightInPix;
	const int Xdeltab = (((base_PicWidthInPix * (2 + sps -> chroma_phase_x)) << (shiftX - 2)) + (Curr_PicWidthInPix >> 1)) / Curr_PicWidthInPix + (1 << (shiftX - 5));
	const int Ydeltab = (((base_PicHeightInPix * (2 + sps -> chroma_phase_y)) << (shiftY - 2)) + (Curr_PicHeightInPix >> 1)) / Curr_PicHeightInPix + (1 << (shiftY - 5));

	for(i = 0; i < 16; i++){
		px[i] = (short) ((((i + x - sps -> scaled_base_left_offset) * Xdeltaa + Xdeltab) >> (shiftX - 4)) - 4 * (2 + sps -> chroma_phase_x));
		py[i] = (short) ((((i + y - sps -> scaled_base_top_offset) * Ydeltaa + Ydeltab) >> (shiftY - 4)) - 4 * (2 + sps -> chroma_phase_y));
	}





	for( j = 0; j < 11; j++ )   {
		unsigned char *ptr = &aio_tucImage[j * ChromaStride];
		short *tmp = &temp[j * 16];

		for( i = 0; i < 16; i++ ){
			const short pos16 = px[i] & 0x0f;
			const short pos = (px[i] >> 4) - x_base;
			const short m = CLIP3(-4, 15, pos );
			const short *filter = &filter16_chroma[pos16][0];
			tmp[i] = filter[0] * ptr[m] + filter[1] * ptr[m + 1];
		}
	}

	for( i = 0; i < 8; i++ )   {
		short*  tmp = &temp[i + 4 * 16];
		unsigned char *ptr_img = &Y[i];
		for( j = 0; j < 8; j++ ){
			const short pos16 = py[j] & 0x0f;
			const short pos = (py[j] >> 4) - y_base;
			const short m = CLIP3(-4, 15, pos);
			const short *filter = &filter16_chroma[pos16][0];
			const int tmp_result = filter[0] * tmp[m  * 16] + filter[1] * tmp[m * 16 + 16];

			ptr_img[j * ((Nal -> curr_Width + 32) >> 1)] = (unsigned char) CLIP255((tmp_result + 512) >> 10);
		}
	}
}

#else


//esb
static int filter16_optim[16][2] = {{0x00000020,0},
{0xFFFF0020,0x0002FFFF},
{0xFFFE001F,0x0004FFFF},
{0xFFFD001E,0x0006FFFF},
{0xFFFD001C,0x0008FFFF},
{0xFFFC001A,0x000BFFFF},
{0xFFFC0018,0x000EFFFE},
{0xFFFD0016,0x0010FFFD},
{0xFFFD0013,0x0013FFFD},
{0xFFFD0010,0x0016FFFD},
{0xFFFE000E,0x0018FFFC},
{0xFFFF000B,0x001AFFFC},
{0xFFFF0008,0x001CFFFD},
{0xFFFF0006,0x001EFFFD},
{0xFFFF0004,0x001FFFFE},
{0xFFFF0002,0x0020FFFF}
};


static int filter16_chroma_optim[16]= {
	0x00200000,
	0x001E0002,
	0x001C0004,
	0x001A0006,
	0x00180008,
	0x0016000A,
	0x0014000C,
	0x0012000E,
	0x00100010,
	0x000E0012,
	0x000C0014,
	0x000A0016,
	0x00080018,
	0x0006001A,
	0x0004001C,
	0x0002001E
};


/**
Upsample a macroblock's luminance sample.
*/
void upsample_mb_luninance(const SPS *sps, const NAL *Nal, unsigned char *Y, unsigned char *aio_tucImage, 
						   const short x_base, const short y_base, const short x, const short y)
{

	short    px [32];
	short    py [32];
	short    temp [25 * 32];//24*32
	const short base_PicWidthInPix = Nal -> down_Width;
	const short Curr_PicWidthInPix = Nal -> up_Width;
	const short base_PicHeightInPix = Nal -> down_Height;
	const short Curr_PicHeightInPix = Nal -> up_Height;
	int deltaa,i, j;

	int   valor_inter=0;//ESB
	int * dir_px_py= (int*)px;
	short nal_optim=(Nal -> curr_Width + 32);

	short shiftX = ShiftCalculation(sps -> level_idc, base_PicWidthInPix);
	short shiftX4=shiftX-4;



	deltaa = ((base_PicWidthInPix << shiftX) + (Curr_PicWidthInPix >> 1)) / Curr_PicWidthInPix;

	valor_inter=(x - (sps -> scaled_base_left_offset << 1)) * deltaa + ((((base_PicWidthInPix <<1) << (shiftX - 2)) + (Curr_PicWidthInPix >> 1)) / Curr_PicWidthInPix + (1 << (shiftX - 5)));//PRUEBA



	_mem8(dir_px_py)   = _itoll(_sub2(_pack2((short) ((valor_inter + (deltaa * 3)>> shiftX4)),(short) ((valor_inter + (deltaa<<1)>> shiftX4))),0x00080008),_sub2(_pack2((short) ((valor_inter + deltaa>> shiftX4)),(short) ((valor_inter >> shiftX4))),0x00080008));
	_mem8(dir_px_py+2) = _itoll(_sub2(_pack2((short) ((valor_inter + (deltaa * 7)>> shiftX4)),(short) ((valor_inter + (deltaa * 6)>> shiftX4))),0x00080008),_sub2(_pack2((short) ((valor_inter + (deltaa * 5)>> shiftX4)),(short) ((valor_inter + (deltaa<<2)>> shiftX4))),0x00080008));
	_mem8(dir_px_py+4) = _itoll(_sub2(_pack2((short) ((valor_inter + (deltaa * 11)>> shiftX4)),(short) ((valor_inter + (deltaa * 10)>> shiftX4))),0x00080008),_sub2(_pack2((short) ((valor_inter + (deltaa * 9)>> shiftX4)),(short) ((valor_inter + (deltaa <<3)>> shiftX4))),0x00080008));
	_mem8(dir_px_py+6) = _itoll(_sub2(_pack2((short) ((valor_inter + (deltaa * 15)>> shiftX4)),(short) ((valor_inter + (deltaa * 14)>> shiftX4))),0x00080008),_sub2(_pack2((short) ((valor_inter + (deltaa * 13)>> shiftX4)),(short) ((valor_inter + (deltaa * 12)>> shiftX4))),0x00080008));
	_mem8(dir_px_py+8) = _itoll(_sub2(_pack2((short) ((valor_inter + (deltaa * 19)>> shiftX4)),(short) ((valor_inter + (deltaa * 18)>> shiftX4))),0x00080008),_sub2(_pack2((short) ((valor_inter + (deltaa * 17)>> shiftX4)),(short) ((valor_inter + (deltaa <<4)>> shiftX4))),0x00080008));
	_mem8(dir_px_py+10)= _itoll(_sub2(_pack2((short) ((valor_inter + (deltaa * 23)>> shiftX4)),(short) ((valor_inter + (deltaa * 22)>> shiftX4))),0x00080008),_sub2(_pack2((short) ((valor_inter + (deltaa * 21)>> shiftX4)),(short) ((valor_inter + (deltaa * 20)>> shiftX4))),0x00080008));
	_mem8(dir_px_py+12)= _itoll(_sub2(_pack2((short) ((valor_inter + (deltaa * 27)>> shiftX4)),(short) ((valor_inter + (deltaa * 26)>> shiftX4))),0x00080008),_sub2(_pack2((short) ((valor_inter + (deltaa * 25)>> shiftX4)),(short) ((valor_inter + (deltaa * 24)>> shiftX4))),0x00080008));
	_mem8(dir_px_py+14)= _itoll(_sub2(_pack2((short) ((valor_inter + (deltaa * 31)>> shiftX4)),(short) ((valor_inter + (deltaa * 30)>> shiftX4))),0x00080008),_sub2(_pack2((short) ((valor_inter + (deltaa * 29)>> shiftX4)),(short) ((valor_inter + (deltaa * 28)>> shiftX4))),0x00080008));



	shiftX = ShiftCalculation(sps -> level_idc, base_PicHeightInPix);
	shiftX4= shiftX-4;
	deltaa = ((base_PicHeightInPix << shiftX) + (Curr_PicHeightInPix >> 1)) / Curr_PicHeightInPix;


	valor_inter= (y - (sps -> scaled_base_top_offset << 1)) * deltaa + ((((base_PicHeightInPix <<1) << (shiftX - 2)) + (Curr_PicHeightInPix >> 1)) / Curr_PicHeightInPix + (1 << (shiftX - 5)));



	dir_px_py= (int*)py;

	_mem8(dir_px_py)  = _itoll(_sub2(_pack2((short) ((valor_inter + (deltaa * 3)>> shiftX4)),(short) ((valor_inter + (deltaa<<1)>> shiftX4))),0x00080008),_sub2(_pack2((short) ((valor_inter + deltaa>> shiftX4)),(short) ((valor_inter >> shiftX4))),0x00080008));
	_mem8(dir_px_py+2)  = _itoll(_sub2(_pack2((short) ((valor_inter + (deltaa * 7)>> shiftX4)),(short) ((valor_inter + (deltaa * 6)>> shiftX4))),0x00080008),_sub2(_pack2((short) ((valor_inter + (deltaa * 5)>> shiftX4)),(short) ((valor_inter + (deltaa<<2)>> shiftX4))),0x00080008));
	_mem8(dir_px_py+4)  = _itoll(_sub2(_pack2((short) ((valor_inter + (deltaa * 11)>> shiftX4)),(short) ((valor_inter + (deltaa * 10)>> shiftX4))),0x00080008),_sub2(_pack2((short) ((valor_inter + (deltaa * 9)>> shiftX4)),(short) ((valor_inter + (deltaa <<3)>> shiftX4))),0x00080008));
	_mem8(dir_px_py+6)  = _itoll(_sub2(_pack2((short) ((valor_inter + (deltaa * 15)>> shiftX4)),(short) ((valor_inter + (deltaa * 14)>> shiftX4))),0x00080008),_sub2(_pack2((short) ((valor_inter + (deltaa * 13)>> shiftX4)),(short) ((valor_inter + (deltaa * 12)>> shiftX4))),0x00080008));
	_mem8(dir_px_py+8)  = _itoll(_sub2(_pack2((short) ((valor_inter + (deltaa * 19)>> shiftX4)),(short) ((valor_inter + (deltaa * 18)>> shiftX4))),0x00080008),_sub2(_pack2((short) ((valor_inter + (deltaa * 17)>> shiftX4)),(short) ((valor_inter + (deltaa <<4)>> shiftX4))),0x00080008));
	_mem8(dir_px_py+10)  = _itoll(_sub2(_pack2((short) ((valor_inter + (deltaa * 23)>> shiftX4)),(short) ((valor_inter + (deltaa * 22)>> shiftX4))),0x00080008),_sub2(_pack2((short) ((valor_inter + (deltaa * 21)>> shiftX4)),(short) ((valor_inter + (deltaa * 20)>> shiftX4))),0x00080008));
	_mem8(dir_px_py+12)  = _itoll(_sub2(_pack2((short) ((valor_inter + (deltaa * 27)>> shiftX4)),(short) ((valor_inter + (deltaa * 26)>> shiftX4))),0x00080008),_sub2(_pack2((short) ((valor_inter + (deltaa * 25)>> shiftX4)),(short) ((valor_inter + (deltaa * 24)>> shiftX4))),0x00080008));
	_mem8(dir_px_py+14)  = _itoll(_sub2(_pack2((short) ((valor_inter + (deltaa * 31)>> shiftX4)),(short) ((valor_inter + (deltaa * 30)>> shiftX4))),0x00080008),_sub2(_pack2((short) ((valor_inter + (deltaa * 29)>> shiftX4)),(short) ((valor_inter + (deltaa * 28)>> shiftX4))),0x00080008));


	for( j = 0; j < 737; j=j+32 )  
	{
		unsigned char *ptr = &aio_tucImage[j];
		short * restrict Tmp = &temp[j];
		dir_px_py= (int*)Tmp;


#pragma MUST_ITERATE(6,6,);
		for( i = 0; i < 24; i+=4 )
		{
			int *filtro= &filter16_optim[px[i] & 0x0f][0];

			short m = CLIP3(-4,31,(px[i] >> 4) - 1 - x_base);//ESB   

			int * filtro2= &filter16_optim[px[i+1] & 0x0f][0];
			short m2 = CLIP3(-4,31,(px[i+1] >> 4) - 1 - x_base);//ESB

			int *filtro3= &filter16_optim[px[i+2] & 0x0f][0];
			int *filtro4= &filter16_optim[px[i+3] & 0x0f][0];
			short m3= CLIP3(-4,31,(px[i+2] >> 4) - 1 - x_base);//ESB 
			short m4= CLIP3(-4,31,(px[i+3] >> 4) - 1 - x_base);//ESB


			_mem8(dir_px_py)  = _itoll(
				_pack2((_dotp2(* filtro4,_pack2(ptr[m4],ptr[m4+1]))
				+_dotp2(* (filtro4+1),_pack2(ptr[m4+2],ptr[m4+3]))),
				(_dotp2(* filtro3,_pack2(ptr[m3],ptr[m3+1]))
				+_dotp2(* (filtro3+1),_pack2(ptr[m3+2],ptr[m3+3])))),

				_pack2((_dotp2(* filtro2,_pack2(ptr[m2],ptr[m2+1]))
				+_dotp2(* (filtro2+1),_pack2(ptr[m2+2],ptr[m2+3]))),
				(_dotp2(* filtro,_pack2(ptr[m],ptr[m+1]))
				+_dotp2(* (filtro+1),_pack2(ptr[m+2],ptr[m+3])))));



			dir_px_py+=2;

		}


	}//fin for de 32 en 32


	for( i = 0; i < 16; i++ )  //DSM_CANDIDATO_OPTIM  
	{ 
		//short *Tmp = &temp[i + 4 * 32];
		short *Tmp = &temp[i + 128];//ESB
		unsigned char * restrict ptr_img = &Y[i];
		for( j = 0; j < 16; j+=2 ) 
		{

			int *filtro= &filter16_optim[py[j] & 0x0f][0];		

			short m = CLIP3(-4,17, (py[j] >> 4) - 1 - y_base);//ESB


			int tmp_result = _dotp2(* filtro,_pack2(Tmp[m  <<5],Tmp[(m + 1) <<5]))
				+_dotp2(* (filtro+1),_pack2(Tmp[(m + 2) <<5],Tmp[(m + 3) <<5]));


			ptr_img[j * (nal_optim)] = (unsigned char) CLIP255((tmp_result + 512) >> 10);

			//2
			filtro= &filter16_optim[py[j+1] & 0x0f][0];
			m = CLIP3(-4,17, (py[j+1] >> 4) - 1 - y_base);

			tmp_result = _dotp2(* filtro,_pack2(Tmp[m  <<5],Tmp[(m + 1) <<5]))
				+_dotp2(* (filtro+1),_pack2(Tmp[(m + 2) <<5],Tmp[(m + 3) <<5]));
			ptr_img[(j+1) * (nal_optim)] = (unsigned char) CLIP255((tmp_result + 512) >> 10);

		}
	}
}



/**
Upsample a macroblock's chrominance sample.
*/
void upsample_mb_chroma(const SPS *sps,const NAL *Nal, unsigned char *Y, unsigned char *aio_tucImage, 
						const short x_base, const short y_base, const short x, const short y)
{


	short i, j;
	//short pos16, pos;
	short m;
	short px [16];
	short py [16];
	short temp [12 * 16];
	//short m2;
	int tmp_result;

	const short base_PicWidthInPix = Nal -> down_Width >> 1;
	const short Curr_PicWidthInPix = Nal -> up_Width >> 1;
	const short base_PicHeightInPix = Nal -> down_Height >> 1;
	const short Curr_PicHeightInPix = Nal -> up_Height >> 1;


	short shiftX = ShiftCalculation(sps -> level_idc, base_PicWidthInPix);
	short shiftX4 = shiftX-4;

	int deltaa = ((base_PicWidthInPix << shiftX) + (Curr_PicWidthInPix >> 1)) / Curr_PicWidthInPix;
	int deltab = (((base_PicWidthInPix * (2 + sps -> chroma_phase_x)) << (shiftX - 2)) + (Curr_PicWidthInPix >> 1)) / Curr_PicWidthInPix + (1 << (shiftX - 5));

	int phase_croma = _pack2((short)((2 + sps -> chroma_phase_x)<<2),((short)(2 + sps -> chroma_phase_x)<<2));
	int valor_intermedio =(x - sps -> scaled_base_left_offset) * deltaa + deltab; 
	int * dir_px_py= (int*)px;

	short nal_optim = ((Nal -> curr_Width + 32) >> 1);


	_mem8(dir_px_py)    = _itoll(_sub2(_pack2((short)(valor_intermedio+(deltaa * 3)  >> (shiftX4)),(short)(valor_intermedio+(deltaa <<1)  >> (shiftX4))),phase_croma),_sub2(_pack2((short)(valor_intermedio+ deltaa       >> (shiftX4)),(short)(valor_intermedio >> (shiftX4))),phase_croma));
	_mem8(dir_px_py+2)  = _itoll(_sub2(_pack2((short)(valor_intermedio+(deltaa * 7)  >> (shiftX4)),(short)(valor_intermedio+(deltaa * 6)  >> (shiftX4))),phase_croma),_sub2(_pack2((short)(valor_intermedio+(deltaa * 5)  >> (shiftX4)),(short)(valor_intermedio+(deltaa <<2)  >> (shiftX4))),phase_croma));
	_mem8(dir_px_py+4)  = _itoll(_sub2(_pack2((short)(valor_intermedio+(deltaa * 11) >> (shiftX4)),(short)(valor_intermedio+(deltaa * 10) >> (shiftX4))),phase_croma),_sub2(_pack2((short)(valor_intermedio+(deltaa * 9)  >> (shiftX4)),(short)(valor_intermedio+(deltaa <<3)  >> (shiftX4))),phase_croma));
	_mem8(dir_px_py+6)  = _itoll(_sub2(_pack2((short)(valor_intermedio+(deltaa * 15) >> (shiftX4)),(short)(valor_intermedio+(deltaa * 14) >> (shiftX4))),phase_croma),_sub2(_pack2((short)(valor_intermedio+(deltaa * 13) >> (shiftX4)),(short)(valor_intermedio+(deltaa * 12) >> (shiftX4))),phase_croma));

	shiftX = ShiftCalculation(sps -> level_idc, base_PicHeightInPix);
	shiftX4 = shiftX-4;

	deltaa = ((base_PicHeightInPix << shiftX) + (Curr_PicHeightInPix >> 1)) / Curr_PicHeightInPix;
	deltab = (((base_PicHeightInPix * (2 + sps -> chroma_phase_y)) << (shiftX - 2)) + (Curr_PicHeightInPix >> 1)) / Curr_PicHeightInPix + (1 << (shiftX - 5));

	phase_croma = _pack2((short)((2 + sps -> chroma_phase_y)<<2),((short)(2 + sps -> chroma_phase_y)<<2));
	valor_intermedio =(y - sps -> scaled_base_top_offset) * deltaa + deltab;

	dir_px_py= (int*)py;


	_mem8(dir_px_py)    = _itoll(_sub2(_pack2((short)(valor_intermedio+(deltaa * 3)  >> (shiftX4)),(short)(valor_intermedio+(deltaa <<1)  >> (shiftX4))),phase_croma),_sub2(_pack2((short)(valor_intermedio+ deltaa       >> (shiftX4)),(short)(valor_intermedio >> (shiftX4))),phase_croma));
	_mem8(dir_px_py+2)  = _itoll(_sub2(_pack2((short)(valor_intermedio+(deltaa * 7)  >> (shiftX4)),(short)(valor_intermedio+(deltaa * 6)  >> (shiftX4))),phase_croma),_sub2(_pack2((short)(valor_intermedio+(deltaa * 5)  >> (shiftX4)),(short)(valor_intermedio+(deltaa <<2)  >> (shiftX4))),phase_croma));
	_mem8(dir_px_py+4)  = _itoll(_sub2(_pack2((short)(valor_intermedio+(deltaa * 11) >> (shiftX4)),(short)(valor_intermedio+(deltaa * 10) >> (shiftX4))),phase_croma),_sub2(_pack2((short)(valor_intermedio+(deltaa * 9)  >> (shiftX4)),(short)(valor_intermedio+(deltaa <<3)  >> (shiftX4))),phase_croma));
	_mem8(dir_px_py+6)  = _itoll(_sub2(_pack2((short)(valor_intermedio+(deltaa * 15) >> (shiftX4)),(short)(valor_intermedio+(deltaa * 14) >> (shiftX4))),phase_croma),_sub2(_pack2((short)(valor_intermedio+(deltaa * 13) >> (shiftX4)),(short)(valor_intermedio+(deltaa * 12) >> (shiftX4))),phase_croma));



	for( j = 0; j <177 ; j+=16 )   
	{
		unsigned char *  ptr = &aio_tucImage[j];
		short * restrict tmp = &temp[j];

		dir_px_py= (int*)tmp;//OJO


		_mem8(dir_px_py)      = _itoll(_pack2(_dotp2(filter16_chroma_optim[px[3]  & 0x0f],_pack2(ptr[CLIP3(-4, 15,(px[3]  >> 4) - x_base)],ptr[CLIP3(-4, 15,(px[3]  >> 4) - x_base) + 1])),_dotp2(filter16_chroma_optim[px[2]  & 0x0f],_pack2(ptr[CLIP3(-4, 15,(px[2]  >> 4) - x_base)],ptr[CLIP3(-4, 15,(px[2]  >> 4) - x_base) + 1]))),_pack2(_dotp2(filter16_chroma_optim[px[1]  & 0x0f],_pack2(ptr[CLIP3(-4, 15,(px[1]  >> 4) - x_base)],ptr[CLIP3(-4, 15,(px[1]  >> 4) - x_base) + 1])),_dotp2(filter16_chroma_optim[px[0]  & 0x0f],_pack2(ptr[CLIP3(-4, 15,(px[0]  >> 4) - x_base)],ptr[CLIP3(-4, 15,(px[0]  >> 4) - x_base) + 1]))));
		_mem8(dir_px_py+2)    = _itoll(_pack2(_dotp2(filter16_chroma_optim[px[7]  & 0x0f],_pack2(ptr[CLIP3(-4, 15,(px[7]  >> 4) - x_base)],ptr[CLIP3(-4, 15,(px[7]  >> 4) - x_base) + 1])),_dotp2(filter16_chroma_optim[px[6]  & 0x0f],_pack2(ptr[CLIP3(-4, 15,(px[6]  >> 4) - x_base)],ptr[CLIP3(-4, 15,(px[6]  >> 4) - x_base) + 1]))),_pack2(_dotp2(filter16_chroma_optim[px[5]  & 0x0f],_pack2(ptr[CLIP3(-4, 15,(px[5]  >> 4) - x_base)],ptr[CLIP3(-4, 15,(px[5]  >> 4) - x_base) + 1])),_dotp2(filter16_chroma_optim[px[4]  & 0x0f],_pack2(ptr[CLIP3(-4, 15,(px[4]  >> 4) - x_base)],ptr[CLIP3(-4, 15,(px[4]  >> 4) - x_base) + 1]))));
		_mem8(dir_px_py+4)    = _itoll(_pack2(_dotp2(filter16_chroma_optim[px[11] & 0x0f],_pack2(ptr[CLIP3(-4, 15,(px[11] >> 4) - x_base)],ptr[CLIP3(-4, 15,(px[11] >> 4) - x_base) + 1])),_dotp2(filter16_chroma_optim[px[10] & 0x0f],_pack2(ptr[CLIP3(-4, 15,(px[10] >> 4) - x_base)],ptr[CLIP3(-4, 15,(px[10] >> 4) - x_base) + 1]))),_pack2(_dotp2(filter16_chroma_optim[px[9]  & 0x0f],_pack2(ptr[CLIP3(-4, 15,(px[9]  >> 4) - x_base)],ptr[CLIP3(-4, 15,(px[9]  >> 4) - x_base) + 1])),_dotp2(filter16_chroma_optim[px[8]  & 0x0f],_pack2(ptr[CLIP3(-4, 15,(px[8]  >> 4) - x_base)],ptr[CLIP3(-4, 15,(px[8]  >> 4) - x_base) + 1]))));
		_mem8(dir_px_py+6)    = _itoll(_pack2(_dotp2(filter16_chroma_optim[px[15] & 0x0f],_pack2(ptr[CLIP3(-4, 15,(px[15] >> 4) - x_base)],ptr[CLIP3(-4, 15,(px[15] >> 4) - x_base) + 1])),_dotp2(filter16_chroma_optim[px[14] & 0x0f],_pack2(ptr[CLIP3(-4, 15,(px[14] >> 4) - x_base)],ptr[CLIP3(-4, 15,(px[14] >> 4) - x_base) + 1]))),_pack2(_dotp2(filter16_chroma_optim[px[13] & 0x0f],_pack2(ptr[CLIP3(-4, 15,(px[13] >> 4) - x_base)],ptr[CLIP3(-4, 15,(px[13] >> 4) - x_base) + 1])),_dotp2(filter16_chroma_optim[px[12] & 0x0f],_pack2(ptr[CLIP3(-4, 15,(px[12] >> 4) - x_base)],ptr[CLIP3(-4, 15,(px[12] >> 4) - x_base) + 1]))));
	}

	for( i = 0; i < 8; i++ )   
	{

		short*  tmp = &temp[i + 64];
		unsigned char * restrict ptr_img = &Y[i];

		m = CLIP3(-4, 15,(py[0] >> 4) - y_base);
		tmp_result = _dotp2(filter16_chroma_optim[py[0] & 0x0f],_pack2(tmp[m<<4],tmp[(m<<4) + 16]));
		ptr_img[0] = (unsigned char) CLIP255((tmp_result + 512) >> 10);

		m = CLIP3(-4, 15,(py[1] >> 4) - y_base);
		tmp_result = _dotp2(filter16_chroma_optim[py[1] & 0x0f],_pack2(tmp[m<<4],tmp[(m<<4) + 16]));
		ptr_img[nal_optim] = (unsigned char) CLIP255((tmp_result + 512) >> 10);

		m = CLIP3(-4, 15,(py[2] >> 4) - y_base);
		tmp_result = _dotp2(filter16_chroma_optim[py[2] & 0x0f],_pack2(tmp[m<<4],tmp[(m<<4) + 16]));
		ptr_img[nal_optim<<1] = (unsigned char) CLIP255((tmp_result + 512) >> 10);

		m = CLIP3(-4, 15,(py[3] >> 4) - y_base);
		tmp_result = _dotp2(filter16_chroma_optim[py[3] & 0x0f],_pack2(tmp[m<<4],tmp[(m<<4) + 16]));
		ptr_img[3 * nal_optim] = (unsigned char) CLIP255((tmp_result + 512) >> 10);

		m = CLIP3(-4, 15,(py[4] >> 4) - y_base);
		tmp_result = _dotp2(filter16_chroma_optim[py[4] & 0x0f],_pack2(tmp[m<<4],tmp[(m<<4) + 16]));
		ptr_img[nal_optim<<2] = (unsigned char) CLIP255((tmp_result + 512) >> 10);

		m = CLIP3(-4, 15,(py[5] >> 4) - y_base);
		tmp_result = _dotp2(filter16_chroma_optim[py[5] & 0x0f],_pack2(tmp[m<<4],tmp[(m<<4) + 16]));
		ptr_img[5 * nal_optim] = (unsigned char) CLIP255((tmp_result + 512) >> 10);

		m = CLIP3(-4, 15,(py[6] >> 4) - y_base);
		tmp_result = _dotp2(filter16_chroma_optim[py[6] & 0x0f],_pack2(tmp[m<<4],tmp[(m<<4) + 16]));
		ptr_img[6 * nal_optim] = (unsigned char) CLIP255((tmp_result + 512) >> 10);

		m = CLIP3(-4, 15,(py[7] >> 4) - y_base);
		tmp_result = _dotp2(filter16_chroma_optim[py[7] & 0x0f],_pack2(tmp[m<<4],tmp[(m<<4) + 16]));   
		ptr_img[7 * nal_optim] = (unsigned char) CLIP255((tmp_result + 512) >> 10);
	}
}

#endif
