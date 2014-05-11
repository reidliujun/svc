/*****************************************************************************
*
*  Open SVC Decoder developped in IETR image lab
*
*
*
*              Médéric BLESTEL <mblestel@insa-rennes.Fr>
*              Mickael RAULET <mraulet@insa-rennes.Fr>
*			  Maxime  PELCAT <mpelcat@insa-rennes.Fr>
*			  Alain  MACCARI <alain.maccari@ens.insa-rennes.fr>
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


#include <stdio.h>
#include <string.h>

#include "neighbourhood.h"
#include "prediction_mode.h"



/**
These functions are called by the pf: Allpf_struct -> Ptr_mb4x4_mode [mode] in decode_I_Intra4x4_lum()
It's used in DC luminance prediction.


@param predict_sample The sample to be decoded
@param PicWidthInPix Width in pixels.
@param macro Specifies the availability of the neighbouring macroblock.
@param locx Location in x in the current macroblock.
@param locy Location in y in the current macroblock.
*/
void predict_4x4_dci( unsigned char * RESTRICT predict_sample,const short PicWidthInPix, int Mask, const short locx, const short locy)
{
	NEIGHBOUR       left [1];
	NEIGHBOUR       top [1];
	// For macroblocks A, B C & D
	left -> xN = locx - 1;
	left -> yN = locy;
	top -> xN = locx ;
	top -> yN =  locy - 1;

	neighbouring_locations_luma(Mask, left);
	neighbouring_locations_luma(Mask, top);
	predict_4x4_dc (predict_sample, left, top, PicWidthInPix);
}




/**
These functions are called by the pf: Allpf_struct -> Ptr_mb4x4_mode [mode] in decode_I_Intra4x4_lum()
It's used in down left luminance prediction.

@param predict_sample The sample to be decoded
@param PicWidthInPix Width in pixels.
@param macro Specifies the availability of the neighbouring macroblock.
@param locx Location in x in the current macroblock.
@param locy Location in y in the current macroblock.
*/
void predict_4x4_ddli ( unsigned char * RESTRICT predict_sample,const short PicWidthInPix , int Mask, const short locx, const short locy)
{
	NEIGHBOUR       top_right [1];

	// For macroblocks A, B C & D
	top_right -> xN = locx + 4 ;
	top_right -> yN = locy - 1;

	neighbouring_locations_luma(Mask, top_right);
	top_right -> Avail = top_right -> Avail && !((locx == 4 || locx == 12) && (locy == 4 || locy == 12));
	predict_4x4_ddl (predict_sample, top_right, PicWidthInPix);
}



/**
These functions are called by the pf: Allpf_struct -> Ptr_mb4x4_mode [mode] in decode_I_Intra4x4_lum()
It's used in verticval left luminance prediction.

@param predict_sample The sample to be decoded
@param PicWidthInPix Width in pixels.
@param macro Specifies the availability of the neighbouring macroblock.
@param locx Location in x in the current macroblock.
@param locy Location in y in the current macroblock.
*/
void predict_4x4_vli ( unsigned char * RESTRICT predict_sample,const short PicWidthInPix , int Mask, const short locx, const short locy)
{
	NEIGHBOUR       top_right [1];

	// For macroblocks A, B C & D
	top_right -> xN = locx + 4 ;
	top_right -> yN = locy - 1;

	neighbouring_locations_luma(Mask, top_right);
	top_right -> Avail = top_right -> Avail && !((locx == 4 || locx == 12) && (locy == 4 || locy == 12));
	predict_4x4_vl (predict_sample, top_right, PicWidthInPix);
}



/**
These functions are called by the pf: Allpf_struct -> Ptr_mbChroma_mode_pf [mode] in decode_I_Intra4x4_lum()
It's used in DC chrominance predicition.

@param predict_sample The sample to be decoded
@param PicWidthInPix Width in pixels.
@param macro Specifies the availability of the neighbouring macroblock.
*/
void predict_chr_dci( unsigned char * predict_sample, const short PicWidthInPix, int Mask){

	switch(Mask & 0x03)
	{

	case 1 :
		predict_chr_dc_left ( predict_sample, PicWidthInPix);
		break;
	case 2 : 
		predict_chr_dc_top ( predict_sample, PicWidthInPix);
		break;
	case 3 :
		predict_chr_dc	( predict_sample, PicWidthInPix);
		break;
	default:
		predict_chr_dc_128 ( predict_sample, PicWidthInPix);
		break;
	}
}

/**
These functions are called by the pf: Allpf_struct -> Ptr_mb4x4_mode [mode]	in decode_I_Intra4x4_lum()
Derivation process for neighbouring 16x16 DC.

@param predict_sample The sample to be decoded
@param PicWidthInPix Width in pixels.
@param macro Specifies the availability of the neighbouring macroblock.
*/
void predict_16x16_dci ( unsigned char * RESTRICT predict_sample, const short PicWidthInPix, int Mask)
{


	switch(Mask & 0x03)
	{
	case 1 :
		predict_16x16_dc_left ( predict_sample, PicWidthInPix);
		break;
	case 2 : 
		predict_16x16_dc_top ( predict_sample, PicWidthInPix); 
		break;
	case 3 :
		predict_16x16_dc ( predict_sample, PicWidthInPix); 
		break;
	default:
		predict_16x16_dc_128 ( predict_sample, PicWidthInPix); 
		break;
	}
}
