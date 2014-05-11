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




void edges_limit(  const NAL *Nal, int AvailMask, short x_mbPartIdx, short y_mbPartIdx, unsigned char tmp_luma[], unsigned char base_luma[], 
				 unsigned char tmp_Cb[], unsigned char base_Cb[], unsigned char tmp_Cr[], unsigned char base_Cr[])
{

	short base_L = Nal -> base_Width + 32;
	short base_C = (Nal -> base_Width + 32) >> 1;

	//Recover all the macroblock
	copy_2d(LumaBlockSize, LumaBlockSize, (Nal -> base_Width + 32), LumaStride, base_luma, &tmp_luma[TopBorder * LumaStride + LeftBorder]);
	copy_2d(ChromaBlockSize, ChromaBlockSize, base_C, ChromaStride, base_Cb, &tmp_Cb[TopBorder * ChromaStride + (LeftBorder >> 1)]);
	copy_2d(ChromaBlockSize, ChromaBlockSize, base_C, ChromaStride, base_Cr, &tmp_Cr[TopBorder * ChromaStride + (LeftBorder >> 1)]);

	//The padding is done switch the prediction location.
	//For instance, the left padding is done only if the prediction is based on the first edge of the base macroblock.
	if ( !x_mbPartIdx){
		//Get the left extension border
		BackLeftEdge(Nal, AvailMask, &tmp_luma[TopBorder * LumaStride], base_luma, &tmp_Cb[TopBorder * ChromaStride], 
			base_Cb, &tmp_Cr[TopBorder * ChromaStride], base_Cr);
		if (!y_mbPartIdx){
			//Get the top extension border
			BackTopEdge(Nal, AvailMask, &tmp_luma[LeftBorder], base_luma, &tmp_Cb[LeftBorder >> 1], base_Cb, &tmp_Cr[LeftBorder >> 1], base_Cr);

			//Get the top left extension border
			BackTopLeftEdge(Nal, AvailMask, tmp_luma, base_luma, tmp_Cb, base_Cb, tmp_Cr, base_Cr);
		}else{
			//Get the bottom extension border
			BackBottomEdge(Nal, AvailMask, &tmp_luma[LeftBorder + (TopBorder +  LumaBlockSize) * LumaStride], &base_luma[15 * base_L], 
				&tmp_Cb[(LeftBorder >> 1) + (TopBorder +  ChromaBlockSize) * ChromaStride], &base_Cb[7 * base_C], 
				&tmp_Cr[(LeftBorder >> 1) + (TopBorder +  ChromaBlockSize) * ChromaStride], &base_Cr[7 * base_C]);

			//Get the bottom left extension border
			BackBottomLeftEdge(Nal, AvailMask, &tmp_luma[(TopBorder +  LumaBlockSize) * LumaStride], &base_luma[15 * base_L], 
				&tmp_Cb[(TopBorder +  ChromaBlockSize) * ChromaStride], &base_Cb[7 * base_C], 
				&tmp_Cr[(TopBorder +  ChromaBlockSize) * ChromaStride], &base_Cr[7 * base_C]);
		}
	}else{
		//Get the right extension border
		BackRightEdge(Nal, AvailMask, &tmp_luma[(LeftBorder +  LumaBlockSize) + TopBorder * LumaStride], &base_luma[15], 
			&tmp_Cb[((LeftBorder >> 1) +  ChromaBlockSize) + TopBorder * ChromaStride], &base_Cb[7], 
			&tmp_Cr[((LeftBorder >> 1) +  ChromaBlockSize) + TopBorder * ChromaStride], &base_Cr[7]);
		if (!y_mbPartIdx){
			//Get the top extension border
			BackTopEdge(Nal, AvailMask, &tmp_luma[LeftBorder], base_luma, &tmp_Cb[LeftBorder >> 1], base_Cb, &tmp_Cr[LeftBorder >> 1], base_Cr);

			//Get the top right extension border
			BackTopRightEdge(Nal, AvailMask, &tmp_luma[LeftBorder + LumaBlockSize], &base_luma[15], 
				&tmp_Cb[(LeftBorder >> 1) + ChromaBlockSize], &base_Cb[7], 
				&tmp_Cr[(LeftBorder >> 1) + ChromaBlockSize], &base_Cr[7]);
		}else{
			//Get the bottom extension border
			BackBottomEdge(Nal, AvailMask, &tmp_luma[LeftBorder + (TopBorder + LumaBlockSize) * LumaStride], &base_luma[15 * base_L], 
				&tmp_Cb[(LeftBorder >> 1) + (TopBorder + ChromaBlockSize) * ChromaStride], &base_Cb[7 * base_C], 
				&tmp_Cr[(LeftBorder >> 1) + (TopBorder + ChromaBlockSize) * ChromaStride], &base_Cr[7 * base_C]);

			//Get the bottom right extension border
			BackBottomRightEdge(Nal, AvailMask, &tmp_luma[(LeftBorder + LumaBlockSize) + (TopBorder + LumaBlockSize) * LumaStride], &base_luma[15 + 15 * base_L], 
				&tmp_Cb[((LeftBorder >> 1) + ChromaBlockSize) + (TopBorder + ChromaBlockSize) * ChromaStride], &base_Cb[7 + 7 * base_C], 
				&tmp_Cr[((LeftBorder >> 1) + ChromaBlockSize) + (TopBorder + ChromaBlockSize) * ChromaStride], &base_Cr[7 + 7 * base_C]);
		}
	}
}
