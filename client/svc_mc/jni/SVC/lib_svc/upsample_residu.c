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

#include "string.h"


//AVC Files
#include "data.h"
#include "type.h"
#include "clip.h"
#include "SliceGroupMap.h"

//SVC Files
#include "svc_type.h"
#include "upsample_residu.h"
#include "mb_upsample_filter.h"
#include "ResidualProcessing.h"





//Luma residu upsample filters
void LumaHorizontalFilter(short *x16, short *k16, short *xp16,short *yp16, const NAL *Nal, 
						  const RESIDU *base_residu, short *luma_tmp, short *luma_in){

	short x, y;
	short p2,p3;

	// up to a scale factor of 4
	for(y = 0; y < 20; y ++){
		short q = CLIP3(0, Nal -> curr_Height, yp16[y - 2]);

		short *ptr2 = luma_tmp + q * (Nal -> curr_Width);
		short *ptr1 = luma_in  + q * (Nal -> base_Width + 32);

		for(x = 0; x < 16; x ++){
			short i1 = x16[x];
			short k = k16[x]; 
			short p = xp16[x];



			if( base_residu[(p >> 4) + (q >> 4) * (Nal -> base_Width >> 4)] . Transform8x8){
				p = p >> 3;
				p2 = ( i1 / 8 ) == p ? i1 : ( p * 8 );
				p3 = ( (i1 + 1) / 8 ) == p ? (i1 + 1) : ( p * 8 + (8 - 1) );
			}else{
				p = p >> 2;
				p2 = ( i1 / 4 ) == p ? i1 : ( p * 4 );
				p3 = ( (i1 + 1) / 4 ) == p ? (i1 + 1) : ( p * 4 + (4 - 1) );
			}

			ptr2[x] = (short) ((16 - k) * ptr1[p2] + k * ptr1[p3]);
		}
	}
}



void LumaVerticalFilter(short *y16, short *k16, short *xp16, short *yp16, const NAL *Nal, 
						const RESIDU *base_residu, short *luma_tmp, short *luma_out){


	short x,y;
	short p2,p3;
	int PicWidthInPix = Nal -> curr_Width + 32;

	for(x = 0; x < 16; x ++){
		short *ptr1 = luma_tmp + x;
		short *ptr2 = luma_out + x;

		for(y = 0; y < 16; y ++){
			short i1 = y16[y];
			short k = k16[y]; 
			short p = yp16[y];

			if( base_residu[(xp16[x] >> 4) + (yp16[y] >> 4) * (Nal -> base_Width >> 4)] . Transform8x8){
				p = p >> 3;
				p2 = ( i1 >> 3 ) == p ? i1 : ( p << 3 );
				p3 = ( (i1 + 1) >> 3 ) == p ? (i1 + 1) : ( (p << 3) + 7 );
			}else {
				p = p >> 2;
				p2 = ( i1 >> 2 ) == p ? i1 : ( p << 2 );
				p3 = ( (i1 + 1) >> 2 ) == p ? (i1 + 1) : (( p << 2) + 3);
			}


			ptr2[y * PicWidthInPix] = (short) (( (16 - k) * ptr1[Nal -> curr_Width * p2] + k * ptr1[Nal -> curr_Width * p3] + 128 ) >> 8);
		}
	}
}




//Chroma residu upsample filters
void ChromaHorizontalFilter(short *x16, short *k16, short *xp16,short *yp16, const NAL *Nal, 
							short *luma_tmp, short *luma_in){

	short x, y;


	for(y = 0; y < 10; y ++){
		short q = CLIP3(0, Nal -> curr_Height >> 1, yp16[y - 1]);
		short *ptr2 = luma_tmp + q * ((Nal -> curr_Width) >> 1);
		short *ptr1 = luma_in  + q * ((Nal -> base_Width + 32) >> 1);

		for(x = 0; x < 8; x ++){
			short i1 = x16[x];
			short k = k16[x]; 
			short p = xp16[x] >> 2;

			short p2 = ( i1 >> 2 ) == p ? i1 : ( p << 2 );
			short p3 = ( (i1 + 1) >> 2 ) == p ? (i1 + 1) : ( (p << 2) + (4 - 1) );

			ptr2[x] = (short) ((16 - k) * ptr1[p2] + k * ptr1[p3]);
		}
	}
}






void ChromaVerticalFilter(short *y16, short *k16, short *yp16, const NAL *Nal, short *luma_tmp, short *luma_out){


	short x,y;
	const int PicWidthInPix = (Nal -> curr_Width + 32) >> 1;
	const int ResWidthInPix = (Nal -> curr_Width) >> 1;

	for(x = 0; x < 8; x ++){
		short *ptr1 = luma_tmp + x;
		short *ptr2 = luma_out + x;

		for(y = 0; y < 8; y ++){
			short i1 = y16[y];
			short k = k16[y]; 
			short p = yp16[y] >> 2;

			short p2 = ( i1 >> 2 ) == p ? i1 : ( p << 2 );
			short p3 = ( (i1 + 1) >> 2 ) == p ? (i1 + 1) : (( p << 2) + 3);

			ptr2[y * PicWidthInPix] = (short) (( (16 - k) * ptr1[ResWidthInPix * p2] + k * ptr1[ResWidthInPix * p3] + 128 ) >> 8);
		}
	}
}







void Upsample_residu(const int layer_inc, short first_mb_in_slice, const NAL *Nal,const  SPS *sps_id, 
					 const RESIDU *current_residu, const RESIDU *base_residu, short *x16, short *y16, 
					 short *xk16, short *xp16, short *yk16, short *yp16, short *luma_tmp, short *luma_in, 
					 short *Cb_in, short *Cr_in, const unsigned char *ai_tiMbToSliceGroupMap)
{

	short i;
	short iMb_x, iMb_y, Coeff;
	const short PicSizeInMbs = (Nal -> curr_Height * Nal -> curr_Width) >> 8;
	const short PicWidthInMbs = Nal -> curr_Width >> 4;
	short base_PicWidthInPix = Nal -> down_Width;
	short Curr_PicWidthInPix = Nal -> up_Width;
	short base_PicHeightInPix = Nal -> down_Height;
	short Curr_PicHeightInPix = Nal -> up_Height;




	short shiftX = ShiftCalculation(sps_id -> level_idc, base_PicWidthInPix);
	int Scale = ((base_PicWidthInPix << shiftX) + (Curr_PicWidthInPix >> 1)) / Curr_PicWidthInPix;
	int Add = (((base_PicWidthInPix * 2) << (shiftX - 2)) + (Curr_PicWidthInPix >> 1)) / Curr_PicWidthInPix + (1 << (shiftX - 5));

	init_residu_luma_tab(Curr_PicWidthInPix, Scale, Add, shiftX, 8, &x16[sps_id -> scaled_base_left_offset * 2], &xk16[sps_id -> scaled_base_left_offset * 2], &xp16[sps_id -> scaled_base_left_offset * 2]);

	shiftX = ShiftCalculation(sps_id -> level_idc, base_PicHeightInPix);
	Scale = ((base_PicHeightInPix << shiftX) + (Curr_PicHeightInPix >> 1)) / Curr_PicHeightInPix;
	Add = (((base_PicHeightInPix * 2) << (shiftX - 2)) + (Curr_PicHeightInPix >> 1)) / Curr_PicHeightInPix + (1 << (shiftX - 5));

	init_residu_luma_tab(Curr_PicHeightInPix, Scale, Add, shiftX, 8, &y16[sps_id -> scaled_base_top_offset * 2], &yk16[sps_id -> scaled_base_top_offset * 2], &yp16[sps_id -> scaled_base_top_offset * 2]);

	
	//initialize macroblock position
	GetMbPosition(first_mb_in_slice, PicWidthInMbs, &iMb_x, &iMb_y);

	//Horizontal loop for luminance
	for( i = first_mb_in_slice; i < PicSizeInMbs;){

		if (current_residu[i] . ResidualPredictionFlag){
			short x_pos = iMb_x << 4;
			short y_pos = iMb_y << 4;
			LumaHorizontalFilter(&x16[x_pos], &xk16[x_pos], &xp16[x_pos], &yp16[y_pos], Nal, base_residu, &luma_tmp[x_pos], luma_in);
		}

		//Updating the macroblock address
		i = Next_MbAddress(i, PicSizeInMbs, PicWidthInMbs, ai_tiMbToSliceGroupMap, &iMb_x, &iMb_y);
	}


	//initialize macroblock position
	GetMbPosition(first_mb_in_slice, PicWidthInMbs, &iMb_x, &iMb_y);


	//Vertical loop
	for( i = first_mb_in_slice; i < PicSizeInMbs;){
		if (current_residu[i] . ResidualPredictionFlag){
			short x_pos = iMb_x << 4;
			short y_pos = iMb_y << 4;
			LumaVerticalFilter(&y16[y_pos], &yk16[y_pos],  &xp16[x_pos], &yp16[y_pos], Nal, base_residu, &luma_tmp[x_pos], &luma_in[layer_inc + x_pos + y_pos * (Nal -> curr_Width + 32)]);
		}
		//Updating the macroblock address
		i = Next_MbAddress(i, PicSizeInMbs,PicWidthInMbs, ai_tiMbToSliceGroupMap, &iMb_x, &iMb_y);
	}


	base_PicWidthInPix >>= 1;
	Curr_PicWidthInPix >>= 1;
	base_PicHeightInPix >>= 1;
	Curr_PicHeightInPix >>= 1;

	//Horizontal loop for chrominance Cb

	shiftX = ShiftCalculation(sps_id -> level_idc, base_PicWidthInPix);
	Scale = ((base_PicWidthInPix << shiftX) + (Curr_PicWidthInPix >> 1)) / Curr_PicWidthInPix;
	Coeff = 2 + sps_id -> chroma_phase_x;
	Add = (((base_PicWidthInPix * Coeff) << (shiftX - 2)) + (Curr_PicWidthInPix >> 1)) / Curr_PicWidthInPix + (1 << (shiftX - 5));

	init_residu_luma_tab(Curr_PicWidthInPix, Scale, Add, shiftX, Coeff << 2, &x16[sps_id -> scaled_base_left_offset], &xk16[sps_id -> scaled_base_left_offset], &xp16[sps_id -> scaled_base_left_offset]);

	shiftX = ShiftCalculation(sps_id -> level_idc, base_PicHeightInPix);
	Scale = ((base_PicHeightInPix << shiftX) + (Curr_PicHeightInPix >> 1)) / Curr_PicHeightInPix;
	Coeff = 2 + sps_id -> chroma_phase_y;
	Add = (((base_PicHeightInPix * Coeff) << (shiftX - 2)) + (Curr_PicHeightInPix >> 1)) / Curr_PicHeightInPix + (1 << (shiftX - 5));
	init_residu_luma_tab(Curr_PicHeightInPix, Scale, Add, shiftX, Coeff << 2, &y16[sps_id -> scaled_base_top_offset], &yk16[sps_id -> scaled_base_top_offset], &yp16[sps_id -> scaled_base_top_offset]);




	//initialize macroblock position
	GetMbPosition(first_mb_in_slice, PicWidthInMbs, &iMb_x, &iMb_y);


	for( i = first_mb_in_slice; i < PicSizeInMbs;){

		if (current_residu[i] . ResidualPredictionFlag){
			short x_pos = iMb_x << 3;
			short y_pos = iMb_y << 3;
			ChromaHorizontalFilter(&x16[x_pos], &xk16[x_pos], &xp16[x_pos], &yp16[y_pos], Nal, &luma_tmp[x_pos], Cb_in);
		}

		//Updating the macroblock address
		i = Next_MbAddress(i, PicSizeInMbs,PicWidthInMbs, ai_tiMbToSliceGroupMap, &iMb_x, &iMb_y);
	}


	//initialize macroblock position
	GetMbPosition(first_mb_in_slice, PicWidthInMbs, &iMb_x, &iMb_y);

	//Vertical loop
	for( i = first_mb_in_slice; i < PicSizeInMbs;){

		if (current_residu[i] . ResidualPredictionFlag){
			short x_pos = iMb_x << 3;
			short y_pos = iMb_y << 3;
			ChromaVerticalFilter(&y16[y_pos], &yk16[y_pos], &yp16[y_pos], Nal, &luma_tmp[x_pos], &Cb_in[(layer_inc >> 2) + x_pos + y_pos * ((Nal -> curr_Width + 32) >> 1)]);
		}

		//Updating the macroblock address
		i = Next_MbAddress(i, PicSizeInMbs,PicWidthInMbs, ai_tiMbToSliceGroupMap, &iMb_x, &iMb_y);
	}



	//initialize macroblock position
	GetMbPosition(first_mb_in_slice, PicWidthInMbs, &iMb_x, &iMb_y);


	//Horizontal loop for chrominance Cr
	for( i = first_mb_in_slice; i < PicSizeInMbs;){

		if (current_residu[i] . ResidualPredictionFlag){
			short x_pos = iMb_x << 3;
			short y_pos = iMb_y << 3;
			ChromaHorizontalFilter(&x16[x_pos], &xk16[x_pos], &xp16[x_pos], &yp16[y_pos],Nal, &luma_tmp[x_pos], Cr_in);
		}
		//Updating the macroblock address
		i = Next_MbAddress(i, PicSizeInMbs,PicWidthInMbs, ai_tiMbToSliceGroupMap, &iMb_x, &iMb_y);
	}

	//initialize macroblock position
	GetMbPosition(first_mb_in_slice, PicWidthInMbs, &iMb_x, &iMb_y);


	//Vertical loop
	for( i = first_mb_in_slice; i < PicSizeInMbs;){

		if (current_residu[i] . ResidualPredictionFlag){
			short x_pos = iMb_x << 3;
			short y_pos = iMb_y << 3;
			ChromaVerticalFilter(&y16[y_pos], &yk16[y_pos], &yp16[y_pos], Nal, &luma_tmp[x_pos], &Cr_in[(layer_inc >> 2) + x_pos + y_pos * ((Nal -> curr_Width + 32) >> 1)]);

		}
		//Updating the macroblock address
		i = Next_MbAddress(i, PicSizeInMbs,PicWidthInMbs, ai_tiMbToSliceGroupMap, &iMb_x, &iMb_y);
	}
}
