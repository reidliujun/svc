/*****************************************************************************
   *
   *  H264DSP developped in IETR image lab
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


#include "stdio.h"

#define WRITE_YUV_


#ifdef WRITE_YUV
static int OpenFile = 0;
FILE *pf;
#endif


#define ExtractPicture_C extract_picture

#ifdef MMX
#undef extract_picture
#ifdef SSE2
#define ExtractPicture_SSE2 extract_picture 
#else
#define ExtractPicture_MMX extract_picture
#endif
#ifdef POCKET_PC
#include <mmintrin.h>
#include <cmnintrin.h> 
#else
#include <emmintrin.h>
#include <mmintrin.h>
#endif
#endif

#ifdef WRITE_YUV
void WriteOutputVideo(int xsize, int ysize, unsigned char *img_luma_out, unsigned char *img_Cb_out, unsigned char *img_Cr_out)
{
	if(!OpenFile){
		char *argv[3];
		argv[1] = "-out";
		argv[2] = "out.yuv";
		WriteYUVInit(3, argv);
		OpenFile = 1;
	}
	WriteYUVCrop(xsize, ysize, img_luma_out, img_Cb_out, img_Cr_out, pf);
}
#endif



/**
This function allows to remove the padding of the image.

@param xsize Width of the picture.
@param ysize Heigth of the picture.
@param egde Edge of the picture.
@param img_luma_in Decoded picture Buffer.
@param img_Cb_in Decoded picture Buffer Cb.
@param img_Cr_in Decoded picture Buffer Cr.
@param address_pic Position of the current frame in the dpb.
@param img_luma_out picture unpadded.
@param img_Cr_out Chroma Cb unpadded.
@param img_Cr_out Chroma Cr unpadded.
*/
#ifndef SSE2
#ifndef MMX
void ExtractPicture_C(int xsize, int ysize, int edge, int Crop, unsigned char img_luma_in[], unsigned char img_Cb_in[], 
					  unsigned char img_Cr_in[], int address_pic, unsigned char img_luma_out[],
					  unsigned char img_Cb_out[], unsigned char img_Cr_out[])
{


	int i,j;
	
	int Right = Crop & 0xff;
	int Left  = (Crop >> 8) & 0xff;
	int Bottom  = (Crop >> 16) & 0xff;
	int Top  = (Crop >> 24) & 0xff;
	int Xsize = xsize + 2 * edge + Right + Left;

	const int offset_L = (Xsize * (16 + Top) + Left + edge + address_pic);
	const int offset_C = Xsize * (4 + Top / 4) + (Left / 2) + (edge / 2) + (address_pic >> 2);

	unsigned int *im_in = (unsigned int *) (&img_luma_in[offset_L]);
	unsigned int *im_in_Cb = (unsigned int *) (&img_Cb_in[offset_C]);
	unsigned int *im_in_Cr = (unsigned int *) (&img_Cr_in[offset_C]); 
	
	int y_limit = ysize;
	int x_limit = xsize >> 2;
	int stride = (Xsize - Right - Left);
	Xsize = Xsize >> 2;

	for (i = 0; i < y_limit; i++, im_in += Xsize){
		unsigned int *im_out = (unsigned int *) &img_luma_out[i * stride];
		for (j = 0; j < x_limit; j++){
			im_out[j] = im_in[j];
		}
	}


	Xsize = Xsize >> 1;
	stride = stride >> 1;
	y_limit = y_limit >> 1;
	x_limit = (x_limit + 1) >> 1;

//printf("%d %d\n",x_limit,y_limit);
	for (i = 0; i < y_limit; i++){
		unsigned int *im_out_Cb = (unsigned int *) &img_Cb_out[i * stride];
		unsigned int *im_out_Cr = (unsigned int *) &img_Cr_out[i * stride];
		for (j = 0; j < x_limit; j++){
			im_out_Cb[j] = im_in_Cb[i * Xsize + j];
			im_out_Cr[j] = im_in_Cr[i * Xsize + j];
		}
	}

#ifdef WRITE_YUV
	WriteOutputVideo(xsize, ysize, img_luma_out, img_Cb_out, img_Cr_out);
#endif
}
#endif


#ifdef MMX

void ExtractPicture_MMX(int xsize, int ysize, int edge, int Crop, unsigned char img_luma_in[], unsigned char img_Cb_in[], 
						unsigned char img_Cr_in[], int address_pic, unsigned char img_luma_out[],
						unsigned char img_Cb_out[], unsigned char img_Cr_out[])
{


	int i,j;
	
	int Right = Crop & 0xff;
	int Left  = (Crop >> 8) & 0xff;
	int Bottom  = (Crop >> 16) & 0xff;
	int Top  = (Crop >> 24) & 0xff;
	int Xsize = xsize + 2 * edge + Right + Left;

	const int offset_L = (Xsize * (16 + Top) + Left + edge + address_pic);
	const int offset_C = Xsize * (4 + Top / 4) + (Left / 2) + (edge / 2) + (address_pic >> 2);

	__m64 *im_in = (__m64 *) (&img_luma_in[offset_L]);
	__m64 *im_in_Cb = (__m64 *) (&img_Cb_in[offset_C]);
	__m64 *im_in_Cr = (__m64 *) (&img_Cr_in[offset_C]);

	int y_limit = ysize;
	int x_limit = (xsize + 7) >> 3;
	int stride = Xsize - Right - Left;
	Xsize = Xsize >> 3;

	for (i = 0; i < y_limit; i++, im_in += Xsize){
		__m64 *im_out = (__m64 *) &img_luma_out[i * stride];
		memcpy(im_out, im_in, x_limit * sizeof(__m64));
	}


	Xsize = (Xsize + 1) >> 1;
	stride = stride >> 1;
	y_limit >>= 1;
	x_limit = (x_limit + 1) >> 1;

	for (i = 0; i < y_limit; i++){
		__m64 *im_out_Cb = (__m64 *) &img_Cb_out[i * stride];
		__m64 *im_out_Cr = (__m64 *) &img_Cr_out[i * stride];
		memcpy(im_out_Cb, &im_in_Cb[i * Xsize], x_limit * sizeof(__m64));
		memcpy(im_out_Cr, &im_in_Cr[i * Xsize], x_limit * sizeof(__m64));
	}

#ifdef WRITE_YUV
	WriteOutputVideo(xsize, ysize, img_luma_out, img_Cb_out, img_Cr_out);
#endif
#ifndef POCKET_PC
	_mm_empty();    
#endif  
}
#endif
#endif //SSE2


void extract_picture_preesm(int *xsize, int *ysize, int edge, int Crop, unsigned char img_luma_in[], unsigned char img_Cb_in[], 
							unsigned char img_Cr_in[], int *address_pic, unsigned char *display_image)
{
	int XDIM = ((int *) display_image)[0] = *xsize;
	int YDIM = ((int *) display_image)[1] = *ysize;
	unsigned char *Y = display_image + 8;
	unsigned char *U = Y + (XDIM + 32) * YDIM;
	unsigned char *V = U + (XDIM + 32) * YDIM/4;

	extract_picture(XDIM,YDIM, edge, Crop, img_luma_in, img_Cb_in, img_Cr_in, address_pic[0], Y,U,V);
}



#ifdef SSE2

void ExtractPicture_SSE2(int xsize, int ysize, int edge, int Crop, unsigned char img_luma_in[], unsigned char img_Cb_in[], 
						 unsigned char img_Cr_in[], int address_pic, unsigned char img_luma_out[],
						 unsigned char img_Cb_out[], unsigned char img_Cr_out[])
{


	int i,j;
	
	int Right = Crop & 0xff;
	int Left  = (Crop >> 8) & 0xff;
	int Bottom  = (Crop >> 16) & 0xff;
	int Top  = (Crop >> 24) & 0xff;
	int Xsize = xsize + 2 * edge + Right + Left;

	const int offset_L = (Xsize * (16 + Top) + Left + edge + address_pic);
	const int offset_C = Xsize * (4 + Top / 4) + (Left / 2) + (edge / 2) + (address_pic >> 2);


	//problem with the player

	const int y_limit = ysize;
	const int x_limit = (xsize + 15) >> 4;
	int stride = Xsize - Right - Left;

	__m128i *im_in = (__m128i *) (&img_luma_in[offset_L]);
	__m128i *im_in_Cb = (__m128i *) (&img_Cb_in[offset_C]);
	__m128i *im_in_Cr = (__m128i *) (&img_Cr_in[offset_C]);

	const int W2 = Xsize >> 5;
	
	//No problems, Xsize is always / 16
	Xsize = Xsize >> 4;

	for (i = 0; i < y_limit; i++, im_in += Xsize){
		__m128i *im_out = (__m128i *) &img_luma_out[i * stride];
		for (j = 0; j < x_limit; j++){
			_mm_storeu_si128(&im_out[j], im_in[j]);
		}
	}


	for (i = 0; i < y_limit >> 1; i++, im_in_Cb += W2){
		__m128i *im_out_Cb = (__m128i *) &img_Cb_out[i * W2];
		__m128i *im_out_Cr = (__m128i *) &img_Cr_out[i * W2];
		for (j = 0; j < x_limit >> 1; j++){
			_mm_storeu_si128(&im_out_Cb[j], im_in_Cb[j]);
			_mm_storeu_si128(&im_out_Cr[j], im_in_Cr[i * W2 + j]);
			
		}
	}

#ifdef WRITE_YUV
	WriteOutputVideo(xsize, ysize, img_luma_out, img_Cb_out, img_Cr_out);
#endif
}


#endif

