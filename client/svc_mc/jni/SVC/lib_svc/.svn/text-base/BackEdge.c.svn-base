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


//AVC Files
#include "type.h"


//SVC Files
#include "svc_type.h"
#include "BackEdge.h"
#include "mb_padding.h"



void BackLeftEdge(const NAL *Nal, int AvailMask, unsigned char *tmp_luma, unsigned char *base_luma, 
				  unsigned char *tmp_Cb, unsigned char *base_Cb, unsigned char *tmp_Cr, unsigned char *base_Cr)
{


	int left =		   AvailMask & 0x01;
	int top =	      (AvailMask >> 1) & 0x01;
	int top_left =    (AvailMask >> 3) & 0x01;
	int bottom_left = (AvailMask >> 5) & 0x01;
	int bottom =	  (AvailMask >> 6) & 0x01;
	const short base_L = Nal -> base_Width + 32;
	const short base_C = (Nal -> base_Width + 32) >> 1;

	//Get the left extension border
	if (left){
		//In case of the left macroblock is available
		copy_2d(LumaBlockSize, LeftBorder, base_L, LumaStride, &base_luma[-8], tmp_luma);
		copy_2d(ChromaBlockSize, LeftBorder >> 1, base_C, ChromaStride, &base_Cb[-4], tmp_Cb);
		copy_2d(ChromaBlockSize, LeftBorder >> 1, base_C, ChromaStride, &base_Cr[-4], tmp_Cr);
	}else{
		//else it's not available
		if (top_left){
			merge(LumaBlockSize >> 1, LeftBorder, base_L, &base_luma[-1 - base_L], &tmp_luma[7], top, LumaStride, -1);
			merge(ChromaBlockSize >> 1, LeftBorder >> 1, base_C, &base_Cb[-1 - base_C], &tmp_Cb[3], top, ChromaStride, -1);
			merge(ChromaBlockSize >> 1, LeftBorder >> 1, base_C, &base_Cr[-1 - base_C], &tmp_Cr[3], top, ChromaStride, -1);
		}else{
			//Extension of the pixel of the line of the current macroblock
			copy_1d(LumaBlockSize >> 1, LeftBorder, base_L, LumaStride, 0, &base_luma[0], tmp_luma);
			copy_1d(ChromaBlockSize >> 1, LeftBorder >> 1, base_C, ChromaStride, 0, &base_Cb[0], tmp_Cb);
			copy_1d(ChromaBlockSize >> 1, LeftBorder >> 1, base_C, ChromaStride, 0, &base_Cr[0], tmp_Cr);
		}

		//Second sub macroblock of the left one
		if(bottom_left){
			merge(LumaBlockSize >> 1, LeftBorder, -base_L, &base_luma[-1 + 16 * base_L], &tmp_luma[7 + LumaStride * 15], bottom, -LumaStride, -1);
			merge(ChromaBlockSize >> 1, LeftBorder >> 1, -base_C, &base_Cb[-1 + 8 *base_C], &tmp_Cb[3 + ChromaStride * 7], bottom, -ChromaStride, -1);
			merge(ChromaBlockSize >> 1, LeftBorder >> 1, -base_C, &base_Cr[-1 + 8 *base_C], &tmp_Cr[3 + ChromaStride * 7], bottom, -ChromaStride, -1);
		}else{
			//Extension of the pixel of the line of the current macroblock
			copy_1d(LumaBlockSize >> 1, LeftBorder, base_L, LumaStride, 0, &base_luma[8 * base_L], &tmp_luma[8 * LumaStride]);
			copy_1d(ChromaBlockSize >> 1, LeftBorder >> 1, base_C, ChromaStride, 0, &base_Cb[4 * base_C], &tmp_Cb[4 * ChromaStride]);
			copy_1d(ChromaBlockSize >> 1, LeftBorder >> 1, base_C, ChromaStride, 0, &base_Cr[4 * base_C], &tmp_Cr[4 * ChromaStride]);
		}
	}
}





void BackRightEdge(const NAL *Nal, int AvailMask, unsigned char *tmp_luma, unsigned char *base_luma, 
				   unsigned char tmp_Cb[], unsigned char base_Cb[], unsigned char tmp_Cr[], unsigned char base_Cr[])
{



	int top =	       (AvailMask >> 1) & 0x01;
	int top_right =    (AvailMask >> 2) & 0x01;
	int right =		   (AvailMask >> 4) & 0x01;
	int bottom =	   (AvailMask >> 6) & 0x01;
	int bottom_right = (AvailMask >> 7) & 0x01;
	const short base_L = Nal -> base_Width + 32;
	const short base_C = (Nal -> base_Width + 32) >> 1;

	if (right){
		//In case of the left macroblock is available
		copy_2d(LumaBlockSize, RightBorder, base_L, LumaStride, &base_luma[1], tmp_luma);
		copy_2d(ChromaBlockSize, RightBorder >> 1, base_C, ChromaStride, &base_Cb[1], tmp_Cb);
		copy_2d(ChromaBlockSize, RightBorder >> 1, base_C, ChromaStride, &base_Cr[1], tmp_Cr);
	}else{
		//First Part of the macroblock
		if (top_right){
			merge(LumaBlockSize >> 1, RightBorder, base_L, &base_luma[1 - base_L], tmp_luma, top, LumaStride,1);
			merge(ChromaBlockSize >> 1, RightBorder >> 1, base_C, &base_Cb[1 - base_C], tmp_Cb, top, ChromaStride,1);
			merge(ChromaBlockSize >> 1, RightBorder >> 1, base_C, &base_Cr[1 - base_C], tmp_Cr, top, ChromaStride,1);
		}else{
			//Extension of the pixel of the line of the current macroblock
			copy_1d(LumaBlockSize >> 1, RightBorder, base_L, LumaStride, 0, base_luma, tmp_luma);
			copy_1d(ChromaBlockSize >> 1, RightBorder >> 1, base_C, ChromaStride, 0, base_Cb, tmp_Cb);
			copy_1d(ChromaBlockSize >> 1, RightBorder >> 1, base_C, ChromaStride, 0, base_Cr, tmp_Cr);
		}

		//Second Part of the macroblock
		if (bottom_right){
			merge(LumaBlockSize >> 1, RightBorder, -base_L, &base_luma[1 + 16 * base_L], &tmp_luma[LumaStride * 15], bottom, -LumaStride,1);
			merge(ChromaBlockSize >> 1, RightBorder >> 1, -base_C, &base_Cb[1 + 8 * base_C], &tmp_Cb[ChromaStride * 7], bottom, -ChromaStride,1);
			merge(ChromaBlockSize >> 1, RightBorder >> 1, -base_C, &base_Cr[1 + 8 * base_C], &tmp_Cr[ChromaStride * 7], bottom, -ChromaStride,1);
		}else{
			//Extension of the pixel of the line of the current macroblock
			copy_1d(LumaBlockSize >> 1, RightBorder, base_L, LumaStride, 0, &base_luma[8 * base_L], &tmp_luma[LumaStride * 8]);
			copy_1d(ChromaBlockSize >> 1, RightBorder >> 1, base_C, ChromaStride, 0, &base_Cb[4 * base_C], &tmp_Cb[ChromaStride * 4]);
			copy_1d(ChromaBlockSize >> 1, RightBorder >> 1, base_C, ChromaStride, 0, &base_Cr[4 * base_C], &tmp_Cr[ChromaStride * 4]);
		}
	}
}






void BackTopEdge(const NAL *Nal, int AvailMask, unsigned char *tmp_luma, unsigned char *base_luma, 
				 unsigned char *tmp_Cb, unsigned char *base_Cb, unsigned char *tmp_Cr, unsigned char *base_Cr)
{

	int left =		   AvailMask & 0x01;
	int top =	      (AvailMask >> 1) & 0x01;
	int top_right =   (AvailMask >> 2) & 0x01;
	int top_left =    (AvailMask >> 3) & 0x01;
	int right =		  (AvailMask >> 4) & 0x01;
	const short base_L = Nal -> base_Width + 32;
	const short base_C = (Nal -> base_Width + 32) >> 1;


	if(top){
		copy_2d(TopBorder, LumaBlockSize, base_L, LumaStride, &base_luma[- 4 * base_L],  tmp_luma);
		copy_2d(TopBorder, ChromaBlockSize, base_C, ChromaStride, &base_Cb[-4 * base_C], tmp_Cb);
		copy_2d(TopBorder, ChromaBlockSize, base_C, ChromaStride, &base_Cr[-4 * base_C], tmp_Cr);
	}else{
		if(top_left){
			merge(TopBorder, LumaBlockSize >> 1, -base_L, &base_luma[0], &tmp_luma[LumaStride * 3], left, -LumaStride, 1);
			merge(TopBorder, ChromaBlockSize >> 1, -base_C, &base_Cb[0], &tmp_Cb[ChromaStride * 3], left, -ChromaStride, 1);
			merge(TopBorder, ChromaBlockSize >> 1, -base_C, &base_Cr[0], &tmp_Cr[ChromaStride * 3], left, -ChromaStride, 1);
		}else{
			copy_1d(TopBorder, LumaBlockSize >> 1, 0, LumaStride, 1, &base_luma[0], tmp_luma);
			copy_1d(TopBorder, ChromaBlockSize >> 1, 0, ChromaStride, 1, &base_Cb[0], tmp_Cb);
			copy_1d(TopBorder, ChromaBlockSize >> 1, 0, ChromaStride, 1, &base_Cr[0], tmp_Cr);
		}

		if(top_right){
			merge(TopBorder, LumaBlockSize >> 1, - base_L, &base_luma[15], &tmp_luma[LumaStride * 3 + 15], right, -LumaStride, -1);
			merge(TopBorder, ChromaBlockSize >> 1, - base_C, &base_Cb[7], &tmp_Cb[ChromaStride * 3 + 7], right, -ChromaStride, -1);
			merge(TopBorder, ChromaBlockSize >> 1, - base_C, &base_Cr[7], &tmp_Cr[ChromaStride * 3 + 7], right, -ChromaStride, -1);
		}else{
			copy_1d(TopBorder, LumaBlockSize >> 1, 0, LumaStride, 1, &base_luma[8], &tmp_luma[8]);
			copy_1d(TopBorder, ChromaBlockSize >> 1, 0, ChromaStride, 1, &base_Cb[4], &tmp_Cb[4]);
			copy_1d(TopBorder, ChromaBlockSize >> 1, 0, ChromaStride, 1, &base_Cr[4], &tmp_Cr[4]);
		}
	}
}





void BackTopLeftEdge(const NAL *Nal, int AvailMask, unsigned char *tmp_luma, unsigned char *base_luma, 
					 unsigned char *tmp_Cb, unsigned char *base_Cb, unsigned char *tmp_Cr, unsigned char *base_Cr)
{

	int left =	    AvailMask & 0x01;
	int top =	   (AvailMask >> 1) & 0x01;
	int top_left = (AvailMask >> 3) & 0x01;
	const short base_L = Nal -> base_Width + 32;
	const short base_C = (Nal -> base_Width + 32) >> 1;


	if(top_left){
		copy_2d(TopBorder, LumaBlockSize >> 1, base_L, LumaStride, &base_luma[-8 - 4 * base_L], tmp_luma);
		copy_2d(TopBorder, ChromaBlockSize >> 1, base_C, ChromaStride, &base_Cb[-4 -4 * base_C], tmp_Cb);
		copy_2d(TopBorder, ChromaBlockSize >> 1, base_C, ChromaStride, &base_Cr[-4 -4 * base_C], tmp_Cr);
	}else{
		if(left){
			if(top){
				merge(TopBorder, LumaBlockSize >> 1, - base_L, &base_luma[- 1], &tmp_luma[7 + 3 * LumaStride], 1, -LumaStride, -1);
				merge(TopBorder, ChromaBlockSize >> 1, - base_C, &base_Cb[- 1], &tmp_Cb[3 + 3*ChromaStride], 1, -ChromaStride, -1);
				merge(TopBorder, ChromaBlockSize >> 1, - base_C, &base_Cr[- 1], &tmp_Cr[3 + 3*ChromaStride], 1, -ChromaStride, -1);
			}else{
				copy_1d(TopBorder, LumaBlockSize >> 1, 0, LumaStride, 1, &base_luma[-8], tmp_luma);
				copy_1d(TopBorder, ChromaBlockSize >> 1, 0, ChromaStride, 1, &base_Cb[-4], tmp_Cb);
				copy_1d(TopBorder, ChromaBlockSize >> 1, 0, ChromaStride, 1, &base_Cr[-4], tmp_Cr);
			}
		}else {
			//moyen d'enlever le test, car soit l'expansion a ete faite soit le macro est available
			if(top){
				//Extension of the pixel of the line of the current macroblock
				copy_1d(TopBorder, LumaBlockSize >> 1, base_L, LumaStride, 0, &base_luma[- 4 * base_L], tmp_luma);
				copy_1d(TopBorder, ChromaBlockSize >> 1, base_C, ChromaStride, 0, &base_Cb[- 4 * base_C], tmp_Cb);
				copy_1d(TopBorder, ChromaBlockSize >> 1, base_C, ChromaStride, 0, &base_Cr[- 4 * base_C], tmp_Cr);
			}else{
				copy_1d(TopBorder, LumaBlockSize >> 1, 0, LumaStride, 0, &base_luma[0], tmp_luma);
				copy_1d(TopBorder, ChromaBlockSize >> 1, 0, ChromaStride, 0, &base_Cb[0], tmp_Cb);
				copy_1d(TopBorder, ChromaBlockSize >> 1, 0, ChromaStride, 0, &base_Cr[0], tmp_Cr);
			}
		}
	}
}


void BackTopRightEdge(const NAL *Nal, int AvailMask, unsigned char *tmp_luma, unsigned char *base_luma, 
					  unsigned char *tmp_Cb, unsigned char *base_Cb, unsigned char *tmp_Cr, unsigned char *base_Cr)
{

	int top =	      (AvailMask >> 1) & 0x01;
	int top_right =   (AvailMask >> 2) & 0x01;
	int right =		  (AvailMask >> 4) & 0x01;
	const short base_L = Nal -> base_Width + 32;
	const short base_C = (Nal -> base_Width + 32) >> 1;

	if(top_right){
		copy_2d(TopBorder, RightBorder, base_L, LumaStride, &base_luma[1 - 4 * base_L], tmp_luma);
		copy_2d(TopBorder, RightBorder >> 1, base_C, ChromaStride, &base_Cb[1 - 4 * base_C], tmp_Cb);
		copy_2d(TopBorder, RightBorder >> 1, base_C, ChromaStride, &base_Cr[1 - 4 * base_C], tmp_Cr);
	}else{
		if(right){
			if(top){
				merge(TopBorder, RightBorder, -base_L, &base_luma[1], &tmp_luma[LumaStride*3], 1, -LumaStride, 1);
				merge(TopBorder, RightBorder >> 1, - base_C, &base_Cb[1], &tmp_Cb[ChromaStride * 3], 1, -ChromaStride, 1);
				merge(TopBorder, RightBorder >> 1, - base_C, &base_Cr[1], &tmp_Cr[ChromaStride * 3], 1, -ChromaStride, 1);
			}else{
				copy_1d(TopBorder, RightBorder, 0, LumaStride, 1 ,&base_luma[1], tmp_luma);
				copy_1d(TopBorder, RightBorder >> 1, 0, ChromaStride, 1, &base_Cb[1], tmp_Cb);
				copy_1d(TopBorder, RightBorder >> 1, 0, ChromaStride, 1, &base_Cr[1], tmp_Cr);
			}
		}else{
			//moyen d'enlever le test, car soit l'expansion a ete faite soit le macro est available
			if(top){
				copy_1d(TopBorder, RightBorder, base_L, LumaStride, 0, &base_luma[- 4 * base_L], tmp_luma);
				copy_1d(TopBorder, RightBorder >> 1, base_C, ChromaStride, 0, &base_Cb[- 4 * base_C], tmp_Cb);
				copy_1d(TopBorder, RightBorder >> 1, base_C, ChromaStride, 0, &base_Cr[- 4 * base_C], tmp_Cr);
			}else{
				copy_1d(TopBorder, RightBorder, 0, LumaStride, 0, base_luma, tmp_luma);
				copy_1d(TopBorder, RightBorder >> 1, 0, ChromaStride, 0, base_Cb, tmp_Cb);
				copy_1d(TopBorder, RightBorder >> 1, 0, ChromaStride, 0, base_Cr, tmp_Cr);
			}
		}
	}
}


void BackBottomEdge(const NAL *Nal, int AvailMask, unsigned char *tmp_luma, unsigned char *base_luma, 
					unsigned char *tmp_Cb, unsigned char *base_Cb, unsigned char *tmp_Cr, unsigned char *base_Cr)
{

	int left =		   AvailMask & 0x01;
	int right =		   (AvailMask >> 4) & 0x01;
	int bottom_left =  (AvailMask >> 5) & 0x01;
	int bottom =	   (AvailMask >> 6) & 0x01;
	int bottom_right = (AvailMask >> 7) & 0x01;
	const short base_L = Nal -> base_Width + 32;
	const short base_C = (Nal -> base_Width + 32) >> 1;

	if(bottom){
		copy_2d(BottomBorder, LumaBlockSize, base_L, LumaStride, &base_luma[base_L], tmp_luma);
		copy_2d(BottomBorder >> 1, ChromaBlockSize, base_C, ChromaStride, &base_Cb[base_C], tmp_Cb);
		copy_2d(BottomBorder >> 1, ChromaBlockSize, base_C, ChromaStride, &base_Cr[base_C], tmp_Cr);
	}else{
		if(bottom_left){
			merge(4, LumaBlockSize >> 1, base_L, base_luma, tmp_luma, left, LumaStride, 1);//mbl check why not BottomBorder
			merge(4, ChromaBlockSize >> 1, base_C, base_Cb, tmp_Cb, left, ChromaStride, 1);
			merge(4, ChromaBlockSize >> 1, base_C, base_Cr, tmp_Cr, left, ChromaStride, 1);
		}else{
			copy_1d(BottomBorder, LumaBlockSize >> 1, 0, LumaStride, 1, base_luma, tmp_luma);
			copy_1d(BottomBorder >> 1, ChromaBlockSize >> 1, 0, ChromaStride, 1, base_Cb, tmp_Cb);
			copy_1d(BottomBorder >> 1, ChromaBlockSize >> 1, 0, ChromaStride, 1, base_Cr, tmp_Cr);
		}

		if(bottom_right){
			merge(8, LumaBlockSize >> 1, base_L, &base_luma[15], &tmp_luma[15], right, LumaStride, -1);//mbl check why not BottomBorder
			merge(BottomBorder >> 1, ChromaBlockSize >> 1, base_C, &base_Cb[7], &tmp_Cb[7], right, ChromaStride, -1);
			merge(BottomBorder >> 1, ChromaBlockSize >> 1, base_C, &base_Cr[7], &tmp_Cr[7], right, ChromaStride, -1);
		}else{
			copy_1d(BottomBorder, LumaBlockSize >> 1, 0, LumaStride, 1, &base_luma[8], &tmp_luma[8]);
			copy_1d(BottomBorder >> 1, ChromaBlockSize >> 1, 0, ChromaStride, 1, &base_Cb[4], &tmp_Cb[4]);
			copy_1d(BottomBorder >> 1, ChromaBlockSize >> 1, 0, ChromaStride, 1, &base_Cr[4], &tmp_Cr[4]);
		}
	}
}


void BackBottomLeftEdge(const NAL *Nal, int AvailMask, unsigned char *tmp_luma, unsigned char *base_luma, 
						unsigned char *tmp_Cb, unsigned char *base_Cb, unsigned char *tmp_Cr, unsigned char *base_Cr)
{


	int left =		   AvailMask & 0x01;
	int bottom_left = (AvailMask >> 5) & 0x01;
	int bottom =	  (AvailMask >> 6) & 0x01;
	const short base_L = Nal -> base_Width + 32;
	const short base_C = (Nal -> base_Width + 32) >> 1;

	if(bottom_left){
		copy_2d(BottomBorder, LeftBorder, base_L, LumaStride, &base_luma[-8 + base_L], tmp_luma);
		copy_2d(BottomBorder >> 1, LeftBorder >> 1, base_C, ChromaStride, &base_Cb[-4 + base_C], tmp_Cb);
		copy_2d(BottomBorder >> 1, LeftBorder >> 1, base_C, ChromaStride, &base_Cr[-4 + base_C], tmp_Cr);
	}else{
		if(left){
			if(bottom){
				merge(8, LeftBorder, base_L, &base_luma[-1], &tmp_luma[7], 1, LumaStride, -1);//mbl check why not BottomBorder
				merge(4, LeftBorder >> 1, base_C, &base_Cb[-1], &tmp_Cb[3], 1, ChromaStride, -1);
				merge(4, LeftBorder >> 1, base_C, &base_Cr[-1], &tmp_Cr[3], 1, ChromaStride, -1);
			}else{
				copy_1d(BottomBorder, LeftBorder, 0, LumaStride, 1, &base_luma[-8], tmp_luma);
				copy_1d(BottomBorder >> 1, LeftBorder >> 1, 0, ChromaStride, 1, &base_Cb[-4], tmp_Cb);
				copy_1d(BottomBorder >> 1, LeftBorder >> 1, 0, ChromaStride, 1, &base_Cr[-4], tmp_Cr);
			}
		}else{
			if(bottom){
				copy_1d(BottomBorder, LeftBorder, base_L, LumaStride, 0, &base_luma[base_L], tmp_luma);
				copy_1d(BottomBorder >> 1, LeftBorder >> 1, base_C, ChromaStride, 0, &base_Cb[base_C], tmp_Cb);
				copy_1d(BottomBorder >> 1, LeftBorder >> 1, base_C, ChromaStride, 0, &base_Cr[base_C], tmp_Cr);
			}else{
				copy_1d(BottomBorder, LeftBorder, 0, LumaStride, 0, base_luma, tmp_luma);
				copy_1d(BottomBorder >> 1, LeftBorder >> 1, 0, ChromaStride, 0, base_Cb, tmp_Cb);
				copy_1d(BottomBorder >> 1, LeftBorder >> 1, 0, ChromaStride, 0, base_Cr, tmp_Cr);
			}
		}
	}
}



void BackBottomRightEdge(const NAL *Nal, int AvailMask, unsigned char *tmp_luma, unsigned char *base_luma, 
						 unsigned char *tmp_Cb, unsigned char *base_Cb, unsigned char *tmp_Cr, unsigned char *base_Cr)
{


	int right =		  (AvailMask >> 4) & 0x01;
	int bottom =	  (AvailMask >> 6) & 0x01;
	int bottom_right =(AvailMask >> 7) & 0x01;
	const short base_L = Nal -> base_Width + 32;
	const short base_C = (Nal -> base_Width + 32) >> 1;


	if(bottom_right){
		copy_2d(BottomBorder, RightBorder, base_L, LumaStride, &base_luma[1 + base_L], tmp_luma);
		copy_2d(BottomBorder >> 1, RightBorder >> 1, base_C, ChromaStride, &base_Cb[1 + base_C], tmp_Cb);
		copy_2d(BottomBorder >> 1, RightBorder >> 1, base_C, ChromaStride, &base_Cr[1 + base_C], tmp_Cr);
	}else{
		if(right){
			if(bottom){
				merge(8, RightBorder, base_L, &base_luma[1], tmp_luma, 1, LumaStride, 1);//mbl check why not BottomBorder
				merge(4, RightBorder >> 1, base_C, &base_Cb[1], tmp_Cb, 1, ChromaStride, 1);
				merge(4, RightBorder >> 1, base_C, &base_Cr[1], tmp_Cr, 1, ChromaStride, 1);
			}else{
				copy_1d(BottomBorder, RightBorder, 0, LumaStride, 1, &base_luma[1], tmp_luma);
				copy_1d(BottomBorder >> 1, RightBorder >> 1, 0, ChromaStride, 1, &base_Cb[1], tmp_Cb);
				copy_1d(BottomBorder >> 1, RightBorder >> 1, 0, ChromaStride, 1, &base_Cr[1], tmp_Cr);
			}
		}else{
			if(bottom){
				copy_1d(BottomBorder, RightBorder, base_L, LumaStride, 0, &base_luma[base_L], tmp_luma);
				copy_1d(BottomBorder >> 1, RightBorder >> 1, base_C, ChromaStride, 0, &base_Cb[base_C], tmp_Cb);
				copy_1d(BottomBorder >> 1, RightBorder >> 1, base_C, ChromaStride, 0, &base_Cr[base_C], tmp_Cr);
			}else{
				copy_1d(BottomBorder, RightBorder, 0, LumaStride, 0, base_luma, tmp_luma);
				copy_1d(BottomBorder >> 1, RightBorder >> 1, 0, ChromaStride, 0, base_Cb, tmp_Cb);
				copy_1d(BottomBorder >> 1, RightBorder >> 1, 0, ChromaStride, 0, base_Cr, tmp_Cr);
			}
		}
	}
}
