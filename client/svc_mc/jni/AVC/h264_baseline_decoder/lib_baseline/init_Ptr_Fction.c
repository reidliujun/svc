/*****************************************************************************
   *
   *  Open SVC Decoder developped in IETR image lab
   *
   *
   *
   *              Médéric BLESTEL <mblestel@insa-rennes.Fr>
   *              Mickael RAULET <mraulet@insa-rennes.Fr>
   *
   *              http://www.ietr.org/
   *
   *
   *
   *
   * This library is free software; you can redistribute it and/or
   * modify it under the terms of the GNU Lesser General Public
   * License as published by the Free Software Foundation;  either
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

#include "init_Ptr_Fction.h"
#include "decode_mb_P.h"
#include "symbol.h"
#include "slice_data_vlc.h"

#include "prediction_mode.h"
#include "neighbourhood.h"
#include "interpolation.h"
#include "interpolation_8x8.h"
#include "interpol_n_0.h"
#include "interpol_0_n.h"
#include "interpol_2_n.h"
#include "interpol_n_2.h"
#include "interpol_n_n.h"

#include "interpolation_0_n.h"
#include "interpolation_n_0.h"
#include "interpolation_2_n.h"
#include "interpolation_n_2.h"
#include "interpolation_n_n.h"





/**
This function permits to initialize the function pointer. 

@param aio_pf The structure which contains all function pointer.
*/
void init_vectors(STRUCT_PF * aio_pf)
{
	/* pf used in decode_I_Intra4x4_lum functions*/
	aio_pf -> Ptr_mb4x4_mode [INTRA_4x4_V]	=	predict_4x4_v;
	aio_pf -> Ptr_mb4x4_mode [INTRA_4x4_H]	=	predict_4x4_h;
	aio_pf -> Ptr_mb4x4_mode [INTRA_4x4_DC]	=	predict_4x4_dci;	// Derivated
	aio_pf -> Ptr_mb4x4_mode [INTRA_4x4_DDL]	=	predict_4x4_ddli;	// Derivated
	aio_pf -> Ptr_mb4x4_mode [INTRA_4x4_DDR]	=	predict_4x4_ddr;
	aio_pf -> Ptr_mb4x4_mode [INTRA_4x4_VR]	=	predict_4x4_vr;
	aio_pf -> Ptr_mb4x4_mode [INTRA_4x4_HD]	=	predict_4x4_hd;
	aio_pf -> Ptr_mb4x4_mode [INTRA_4x4_VL]	=	predict_4x4_vli;	// Derivated
	aio_pf -> Ptr_mb4x4_mode [INTRA_4x4_HU]	=	predict_4x4_hu;


	/* pf used in decode_I_Intra16x16_lum functions*/
	aio_pf -> Ptr_mb16x16_mode [INTRA_16x16_V]	= predict_16x16_v	;
	aio_pf -> Ptr_mb16x16_mode [INTRA_16x16_H]	= predict_16x16_h	;
	aio_pf -> Ptr_mb16x16_mode [INTRA_16x16_DC]	= predict_16x16_dci	;	//  Derivated
	aio_pf -> Ptr_mb16x16_mode [INTRA_16x16_P]	= predict_16x16_p	;

	/* pf used in decode_I_Chroma Cr functions*/
	aio_pf -> Ptr_mbChroma_mode_pf[INTRA_CHROMA_DC] = predict_chr_dci;	//  Derivated
	aio_pf -> Ptr_mbChroma_mode_pf[INTRA_CHROMA_H] = predict_chr_h	;
	aio_pf -> Ptr_mbChroma_mode_pf[INTRA_CHROMA_V] = predict_chr_v	;
	aio_pf -> Ptr_mbChroma_mode_pf[INTRA_CHROMA_P] = predict_chr_p	;

#ifdef MMXi
	
	aio_pf -> Ptr_interpol_4x4[0] = luma_sample_interp_0_0_MMX;
	aio_pf -> Ptr_interpol_4x4[1] = luma_sample_interp_1_0;			//MMX seems to be slower
	aio_pf -> Ptr_interpol_4x4[2] = luma_sample_interp_2_0;			//MMX seems to be slower
	aio_pf -> Ptr_interpol_4x4[3] = luma_sample_interp_3_0;			//MMX seems to be slower

	aio_pf -> Ptr_interpol_4x4[4] = luma_sample_interp_0_1;			//MMX seems to be slower
	aio_pf -> Ptr_interpol_4x4[8] = luma_sample_interp_0_2;			//MMX seems to be slower
	aio_pf -> Ptr_interpol_4x4[12] = luma_sample_interp_0_3;		//MMX seems to be slower

	aio_pf -> Ptr_interpol_4x4[6] = luma_sample_interp_2_1;			//MMX seems to be slower
	aio_pf -> Ptr_interpol_4x4[10] = luma_sample_interp_2_2;		//MMX seems to be slower
	aio_pf -> Ptr_interpol_4x4[14] = luma_sample_interp_2_3;		//MMX seems to be slower

	//Seem to be slower
	aio_pf -> Ptr_interpol_4x4[9] = luma_sample_interp_1_2;			//MMX seems to be slower
	aio_pf -> Ptr_interpol_4x4[11] = luma_sample_interp_3_2;		//MMX seems to be slower

	aio_pf -> Ptr_interpol_4x4[5] = luma_sample_interp_1_1;			//MMX seems to be slower
	aio_pf -> Ptr_interpol_4x4[7] = luma_sample_interp_3_1;			//MMX seems to be slower
	aio_pf -> Ptr_interpol_4x4[13] = luma_sample_interp_1_3;		//MMX seems to be slower
	aio_pf -> Ptr_interpol_4x4[15] = luma_sample_interp_3_3;		//MMX seems to be slower
		

#elif TI_OPTIM
	aio_pf -> Ptr_interpol_4x4[0] = luma_sample_interp_0_0_TI;
	aio_pf -> Ptr_interpol_4x4[1] = luma_sample_interp_1_0_TI;
	aio_pf -> Ptr_interpol_4x4[2] = luma_sample_interp_2_0_TI;
	aio_pf -> Ptr_interpol_4x4[3] = luma_sample_interp_3_0_TI;

	aio_pf -> Ptr_interpol_4x4[4] = luma_sample_interp_0_1_TI;
	aio_pf -> Ptr_interpol_4x4[8] = luma_sample_interp_0_2_TI;
	aio_pf -> Ptr_interpol_4x4[12] = luma_sample_interp_0_3_TI;

	aio_pf -> Ptr_interpol_4x4[6] = luma_sample_interp_2_1_TI;
	aio_pf -> Ptr_interpol_4x4[10] = luma_sample_interp_2_2_TI;
	aio_pf -> Ptr_interpol_4x4[14] = luma_sample_interp_2_3_TI;

	aio_pf -> Ptr_interpol_4x4[9] = luma_sample_interp_1_2_TI;
	aio_pf -> Ptr_interpol_4x4[11] = luma_sample_interp_3_2_TI;

	aio_pf -> Ptr_interpol_4x4[5] = luma_sample_interp_1_1_TI;
	aio_pf -> Ptr_interpol_4x4[7] = luma_sample_interp_3_1_TI;
	aio_pf -> Ptr_interpol_4x4[13] = luma_sample_interp_1_3_TI;
	aio_pf -> Ptr_interpol_4x4[15] = luma_sample_interp_3_3_TI;

#else
		//Intepolation 4x4 initialization
	aio_pf -> Ptr_interpol_4x4[0] = luma_sample_interp_0_0;
	aio_pf -> Ptr_interpol_4x4[1] = luma_sample_interp_1_0;
	aio_pf -> Ptr_interpol_4x4[2] = luma_sample_interp_2_0;
	aio_pf -> Ptr_interpol_4x4[3] = luma_sample_interp_3_0;

	aio_pf -> Ptr_interpol_4x4[4] = luma_sample_interp_0_1;
	aio_pf -> Ptr_interpol_4x4[8] = luma_sample_interp_0_2;
	aio_pf -> Ptr_interpol_4x4[12] = luma_sample_interp_0_3;

	aio_pf -> Ptr_interpol_4x4[6] = luma_sample_interp_2_1;
	aio_pf -> Ptr_interpol_4x4[10] = luma_sample_interp_2_2;
	aio_pf -> Ptr_interpol_4x4[14] = luma_sample_interp_2_3;

	aio_pf -> Ptr_interpol_4x4[9] = luma_sample_interp_1_2;
	aio_pf -> Ptr_interpol_4x4[11] = luma_sample_interp_3_2;

	aio_pf -> Ptr_interpol_4x4[5] = luma_sample_interp_1_1;
	aio_pf -> Ptr_interpol_4x4[7] = luma_sample_interp_3_1;
	aio_pf -> Ptr_interpol_4x4[13] = luma_sample_interp_1_3;
	aio_pf -> Ptr_interpol_4x4[15] = luma_sample_interp_3_3;
#endif




#ifdef SSE2
	aio_pf -> Ptr_interpol_8x8[0] = luma_sample_interp_8x8_0_0_SSE2;

	aio_pf -> Ptr_interpol_8x8[1] = luma_sample_interp_8x8_1_0_SSE2;
	aio_pf -> Ptr_interpol_8x8[2] = luma_sample_interp_8x8_2_0_SSE2;
	aio_pf -> Ptr_interpol_8x8[3] = luma_sample_interp_8x8_3_0_SSE2;

	aio_pf -> Ptr_interpol_8x8[4] = luma_sample_interp_8x8_0_1_SSE2;
	aio_pf -> Ptr_interpol_8x8[8] = luma_sample_interp_8x8_0_2_SSE2;
	aio_pf -> Ptr_interpol_8x8[12] = luma_sample_interp_8x8_0_3_SSE2;

	aio_pf -> Ptr_interpol_8x8[6] = luma_sample_interp_8x8_2_1_SSE2; 
	aio_pf -> Ptr_interpol_8x8[10] = luma_sample_interp_8x8_2_2_SSE2; 
	aio_pf -> Ptr_interpol_8x8[14] = luma_sample_interp_8x8_2_3_SSE2;

	aio_pf -> Ptr_interpol_8x8[9] = luma_sample_interp_8x8_1_2_SSE2;
	aio_pf -> Ptr_interpol_8x8[11] = luma_sample_interp_8x8_3_2_SSE2;

	aio_pf -> Ptr_interpol_8x8[5] = luma_sample_interp_8x8_1_1_SSE2;
	aio_pf -> Ptr_interpol_8x8[7] = luma_sample_interp_8x8_3_1_SSE2;
	aio_pf -> Ptr_interpol_8x8[13] = luma_sample_interp_8x8_1_3_SSE2;
	aio_pf -> Ptr_interpol_8x8[15] = luma_sample_interp_8x8_3_3_SSE2;

#else
	aio_pf -> Ptr_interpol_8x8[0] = luma_sample_interp_8x8_0_0;

	aio_pf -> Ptr_interpol_8x8[1] = luma_sample_interp_8x8_1_0;
	aio_pf -> Ptr_interpol_8x8[2] = luma_sample_interp_8x8_2_0;
	aio_pf -> Ptr_interpol_8x8[3] = luma_sample_interp_8x8_3_0;

	aio_pf -> Ptr_interpol_8x8[4] = luma_sample_interp_8x8_0_1;
	aio_pf -> Ptr_interpol_8x8[8] = luma_sample_interp_8x8_0_2;
	aio_pf -> Ptr_interpol_8x8[12] = luma_sample_interp_8x8_0_3;

	aio_pf -> Ptr_interpol_8x8[6] = luma_sample_interp_8x8_2_1; 
	aio_pf -> Ptr_interpol_8x8[10] = luma_sample_interp_8x8_2_2; 
	aio_pf -> Ptr_interpol_8x8[14] = luma_sample_interp_8x8_2_3;

	aio_pf -> Ptr_interpol_8x8[9] = luma_sample_interp_8x8_1_2;
	aio_pf -> Ptr_interpol_8x8[11] = luma_sample_interp_8x8_3_2;

	aio_pf -> Ptr_interpol_8x8[5] = luma_sample_interp_8x8_1_1;
	aio_pf -> Ptr_interpol_8x8[7] = luma_sample_interp_8x8_3_1;
	aio_pf -> Ptr_interpol_8x8[13] = luma_sample_interp_8x8_1_3;
	aio_pf -> Ptr_interpol_8x8[15] = luma_sample_interp_8x8_3_3;

#endif

}

	
