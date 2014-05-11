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





#define SLICE_TYPE_P 0
#define SLICE_TYPE_B 1
#define SLICE_TYPE_I 2
#define SLICE_TYPE_SP 3
#define SLICE_TYPE_SI 4


#define INTRA_4x4 1
#define INTRA_8x8 2
#define INTRA_16x16 3
#define INTRA_PCM 4
#define Pred_L0 5
#define Pred_L1 6
#define P_8x8 7
#define P_8x8ref0 8
#define P_Skip 255
#define B_direct 9
#define B_L0 10
#define B_L1 11
#define B_Bi 12
#define B_L0_L0 13
#define B_L0_L1 14
#define B_L0_Bi 15
#define B_L1_L1 16
#define B_L1_L0 17
#define B_L1_Bi 18
#define B_Bi_L0 19
#define B_Bi_L1 20
#define B_Bi_Bi 21
#define B_8x8 22
#define B_Skip 254


//SVC
#define BL 26
#define I_BL 26
#define P_BL 27
#define B_BL 28
#define PB_BL 26


#define MIN_POC -(1 << 15)
#define MAX_POC (1 << 14)


#define NA 255
#define INIT_VLC_LE 2
#define ChromaFormatFactor 1.5

#define LIST_NOT_USED -1
#define PART_NOT_AVAILABLE -2

#define LUMA_DC_BLOCK_INDEX 26
#define CHROMA_DC_BLOCK_INDEX 27
#define CHROMA_DC_COEFF_TOKEN_VLC_BITS 8
#define COEFF_TOKEN_VLC_BITS 8
#define TOTAL_ZEROS_VLC_BITS 9
#define CHROMA_DC_TOTAL_ZEROS_VLC_BITS 3
#define RUN_VLC_BITS 3
#define RUN7_VLC_BITS 6

// Pour le prediction
#define INTRA_4x4_V 0
#define INTRA_4x4_H 1
#define INTRA_4x4_DC 2
#define INTRA_4x4_DDL 3
#define INTRA_4x4_DDR 4
#define INTRA_4x4_VR 5
#define INTRA_4x4_HD 6
#define INTRA_4x4_VL 7
#define INTRA_4x4_HU 8

#define INTRA_8x8_V 0
#define INTRA_8x8_H 1
#define INTRA_8x8_DC 2
#define INTRA_8x8_DDL 3
#define INTRA_8x8_DDR 4
#define INTRA_8x8_VR 5
#define INTRA_8x8_HD 6
#define INTRA_8x8_VL 7
#define INTRA_8x8_HU 8

#define INTRA_16x16_V 0
#define INTRA_16x16_H 1
#define INTRA_16x16_DC 2
#define INTRA_16x16_P 3
#define INTRA_16x16_DC_LEFT 4
#define INTRA_16x16_DC_TOP 5
#define INTRA_16x16_DC_128 7



#define INTRA_CHROMA_DC 0
#define INTRA_CHROMA_H 1
#define INTRA_CHROMA_V 2
#define INTRA_CHROMA_P 3
#define INTRA_CHROMA_DC_LEFT 5
#define INTRA_CHROMA_DC_TOP 6
#define INTRA_CHROMA_DC_128 7
 
#define IS_I(X)		(X > 0 && X < 5)
#define IS_NxN(X)   (X == 1 || X == 2)
#define IS_P(X)		(X > 4 && X < 9 || X == P_Skip || X == P_BL)
#define IS_B(X)		(X > 9 && X < 22 || X == B_Skip)

// 0 1 4 5
// 2 3 6 7
// 8 9 12 13
//10 11 14 15
#define LOCX(x) ((x & 1) << 2) + ((x & 4) << 1)
#define LOCY(x) ((x & 2) << 1) + (x & 8)

// 0 1 2 3
// 4 5 6 7
// 8 9 10 11
//12 13 14 15
#define POSX(x) ((x & 3) << 2)
#define POSY(x) ((x >> 2) << 2)

#define SIGN(X) (X > 0 ? 1: -1)
#define RESAMPLE_CAL(Y,Z) (((Y << 16) + (Z >> 1)) / Z)
#define MV_CAL(X,Y) ((X * Y + 32768) >> 16)



#define MaxChromaMode 3
#define MaxPSubMbType 3 
#define MaxBSubMbType 12
