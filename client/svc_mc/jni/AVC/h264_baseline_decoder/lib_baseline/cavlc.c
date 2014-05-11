/* FFmpeg project
* libavcodec (h264.c)
* H.26L/H.264/AVC/JVT/14496-10/... encoder/decoder
* Copyright (c) 2003 Michael Niedermayer <michaelni@gmx.at>
*
* This file has been modified for opensvcdecoder.
*/

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
* but WITHOUT ANY WARRANTY; without even the implied warranty off
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

#include "cavlc.h"
#include "refill.h"
#include "motion_vector.h"
#include "init_data.h"
#include "bitstream.h"





//pour le level

#ifndef TI_OPTIM
const int    ietr_log2_tab [256] = { 0, 0, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7 };

const int    coeff_token_table_index [17] = { 0, 0, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3 };
#endif

const int    chroma_dc_coeff_token_len [4 * 5] = { 2, 0, 0, 0, 6, 1, 0, 0, 6, 6, 3, 0, 6, 7, 7, 6, 6, 8, 8, 7, };


const int    chroma_dc_coeff_token_bits [4 * 5] = { 1, 0, 0, 0, 7, 1, 0, 0, 4, 6, 1, 0, 3, 3, 2, 5, 2, 3, 2, 0, };


const int    coeff_token_len [4][4 * 17] = {{ 1, 0, 0, 0, 6, 2, 0, 0, 8, 6, 3, 0, 9, 8, 7, 5, 10, 9, 8, 6, 11, 10, 9, 7, 13, 11, 10, 8, 13, 13, 11, 9, 13
, 13, 13, 10, 14, 14, 13, 11, 14, 14, 14, 13, 15, 15, 14, 14, 15, 15, 15, 14, 16, 15, 15, 15, 16, 16, 16, 15, 16, 16, 16, 16, 16, 16, 16, 16, }
, { 2, 0, 0, 0, 6, 2, 0, 0, 6, 5, 3, 0, 7, 6, 6, 4, 8, 6, 6, 4, 8, 7, 7, 5, 9, 8, 8, 6, 11, 9, 9, 6, 11, 11, 11
, 7, 12, 11, 11, 9, 12, 12, 12, 11, 12, 12, 12, 11, 13, 13, 13, 12, 13, 13, 13, 13, 13, 14, 13, 13, 14, 14, 14, 13, 14, 14, 14, 14, }
, { 4, 0, 0, 0, 6, 4, 0, 0, 6, 5, 4, 0, 6, 5, 5, 4, 7, 5, 5, 4, 7, 5, 5, 4, 7, 6, 6, 4, 7, 6, 6, 4, 8, 7, 7, 5
, 8, 8, 7, 6, 9, 8, 8, 7, 9, 9, 8, 8, 9, 9, 9, 8, 10, 9, 9, 9, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, }
, { 6, 0, 0, 0, 6, 6, 0, 0, 6, 6, 6, 0, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6
, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, }};




const int    coeff_token_bits [4][4 * 17] = {{ 1, 0, 0, 0, 5, 1, 0, 0, 7, 4, 1, 0, 7, 6, 5, 3, 7, 6, 5, 3, 7, 6, 5, 4, 15, 6, 5, 4, 11, 14, 5, 4, 8, 10, 13, 4, 15
, 14, 9, 4, 11, 10, 13, 12, 15, 14, 9, 12, 11, 10, 13, 8, 15, 1, 9, 12, 11, 14, 13, 8, 7, 10, 9, 12, 4, 6, 5, 8, }
, { 3, 0, 0, 0, 11, 2, 0, 0, 7, 7, 3, 0, 7, 10, 9, 5, 7, 6, 5, 4, 4, 6, 5, 6, 7, 6, 5, 8, 15, 6, 5, 4, 11, 14
, 13, 4, 15, 10, 9, 4, 11, 14, 13, 12, 8, 10, 9, 8, 15, 14, 13, 12, 11, 10, 9, 12, 7, 11, 6, 8, 9, 8, 10, 1
, 7, 6, 5, 4, }
, { 15, 0, 0, 0, 15, 14, 0, 0, 11, 15, 13, 0, 8, 12, 14, 12, 15, 10, 11, 11, 11, 8, 9, 10, 9, 14, 13, 9, 8, 10
, 9, 8, 15, 14, 13, 13, 11, 14, 10, 12, 15, 10, 13, 12, 11, 14, 9, 12, 8, 10, 13, 8, 13, 7, 9, 12, 9, 12, 11
, 10, 5, 8, 7, 6, 1, 4, 3, 2, }
, { 3, 0, 0, 0, 0, 1, 0, 0, 4, 5, 6, 0, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26
, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53
, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, }}  ;

const int    total_zeros_len [16][16]  = {{ 1, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 9 }, { 3, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 6, 6, 6, 6 }
, { 4, 3, 3, 3, 4, 4, 3, 3, 4, 5, 5, 6, 5, 6 }, { 5, 3, 4, 4, 3, 3, 3, 4, 3, 4, 5, 5, 5 }
, { 4, 4, 4, 3, 3, 3, 3, 3, 4, 5, 4, 5 }, { 6, 5, 3, 3, 3, 3, 3, 3, 4, 3, 6 }, { 6, 5, 3, 3, 3, 2, 3, 4, 3, 6 }
, { 6, 4, 5, 3, 2, 2, 3, 3, 6 }, { 6, 6, 4, 2, 2, 3, 2, 5 }, { 5, 5, 3, 2, 2, 2, 4 }, { 4, 4, 3, 3, 1, 3 }
, { 4, 4, 2, 1, 3 }, { 3, 3, 1, 2 }, { 2, 2, 1 }, { 1, 1 }, } ;

const int    total_zeros_bits [16][16] = {{ 1, 3, 2, 3, 2, 3, 2, 3, 2, 3, 2, 3, 2, 3, 2, 1 }
, { 7, 6, 5, 4, 3, 5, 4, 3, 2, 3, 2, 3, 2, 1, 0 }
, { 5, 7, 6, 5, 4, 3, 4, 3, 2, 3, 2, 1, 1, 0 }
, { 3, 7, 5, 4, 6, 5, 4, 3, 3, 2, 2, 1, 0 }
, { 5, 4, 3, 7, 6, 5, 4, 3, 2, 1, 1, 0 }
, { 1, 1, 7, 6, 5, 4, 3, 2, 1, 1, 0 }
, { 1, 1, 5, 4, 3, 3, 2, 1, 1, 0 }
, { 1, 1, 1, 3, 3, 2, 2, 1, 0 }
, { 1, 0, 1, 3, 2, 1, 1, 1 }
, { 1, 0, 1, 3, 2, 1, 1 }
, { 0, 1, 1, 2, 1, 3 }
, { 0, 1, 1, 1, 1 }
, { 0, 1, 1, 1 }
, { 0, 1, 1 }
, { 0, 1 }, }  ;


const int    chroma_dc_total_zeros_len [3][4] = {{ 1, 2, 3, 3, }, { 1, 2, 2, 0, }, { 1, 1, 0, 0, }, } ;

const int    chroma_dc_total_zeros_bits [3][4]  = {{ 1, 1, 1, 0, }, { 1, 1, 0, 0, }, { 1, 0, 0, 0, }, } ;

const int    run_len [7][16]  = {{ 1, 1 }, { 1, 2, 2 }, { 2, 2, 2, 2 }, { 2, 2, 2, 3, 3 }, { 2, 2, 3, 3, 3, 3 }, { 2, 3, 3, 3, 3, 3, 3 } , { 3, 3, 3, 3, 3, 3, 3, 4, 5, 6, 7, 8, 9, 10, 11 }, };

const int    run_bits [7][16] = {{ 1, 0 }, { 1, 1, 0 }, { 3, 2, 1, 0 }, { 3, 2, 1, 1, 0 }, { 3, 2, 3, 2, 1, 0 }, { 3, 0, 1, 3, 2, 5, 4 } , { 7, 6, 5, 4, 3, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, };


const int zigzag_scan8x8_cavlc[64]={ 0+0*8, 1+1*8, 1+2*8, 2+2*8, 4+1*8, 0+5*8, 3+3*8, 7+0*8, 3+4*8, 1+7*8, 5+3*8, 6+3*8, 2+7*8, 6+4*8, 5+6*8, 7+5*8,
1+0*8, 2+0*8, 0+3*8, 3+1*8, 3+2*8, 0+6*8, 4+2*8, 6+1*8, 2+5*8, 2+6*8, 6+2*8, 5+4*8, 3+7*8, 7+3*8, 4+7*8, 7+6*8,
0+1*8, 3+0*8, 0+4*8, 4+0*8, 2+3*8, 1+5*8, 5+1*8, 5+2*8, 1+6*8, 3+5*8, 7+1*8, 4+5*8, 4+6*8, 7+4*8, 5+7*8, 6+7*8,
0+2*8, 2+1*8, 1+3*8, 5+0*8, 1+4*8, 2+4*8, 6+0*8, 4+3*8, 0+7*8, 4+4*8, 7+2*8, 3+6*8, 5+5*8, 6+5*8, 6+6*8, 7+7*8,};


const char golomb_to_intra4x4 [48] = { 47, 31, 15, 0, 23, 27, 29, 30, 7, 11, 13, 14, 39, 43, 45, 46, 16, 3, 5, 10, 12, 19, 21, 26, 28, 35, 37, 42, 44
, 1, 2, 4, 8, 17, 18, 20, 24, 6, 9, 22, 25, 32, 33, 34, 36, 40, 38, 41 };

const char golomb_to_inter [48]  = { 0, 16, 1, 2, 4, 8, 32, 3, 5, 10, 12, 15, 47, 7, 11, 13, 14, 6, 9, 31, 35, 37, 42, 44, 33, 34, 36, 40, 39, 43
, 45, 46, 17, 18, 20, 24, 19, 21, 26, 28, 23, 27, 29, 30, 22, 25, 38, 41 };






/**
This function permits to decode the VLC on 16 values blocks. 
All the parameters decoded will be stored in differents structures or tables.


@param ao_piCoeffLevel Contains the AC luma level of a 16x16 decoded macroblock.
@param ai_iMaxNumCoeff Number of coefficient to decode.
@param ai_poData The NAL unit.
@param position The current position in the NAL.
@param ai_iN Index of the current block.
@param vlc The VLC tables in order to decode the Nal Unit.
@param NonZeroCountCache Specifies the coeff_token of each blocks 4x4 of a macroblock.
*/
void residual_block_cavlc_16_C( short *ao_piCoeffLevel, const int ai_iMaxNumCoeff, const unsigned char *RESTRICT ai_poData, int *position, 
							   const int ai_iN,	const VLC_TABLES *vlc, unsigned char NonZeroCountCache [ RESTRICT ])
{

	int i ;
	int	coeff_token_TotalCoeff;
	int coeff_token_TrailingOnes;   
	int coeffNum ;
	int tmp2;

	unsigned int    level_prefix ;
	unsigned int    suffixLength ;
	unsigned int    zerosLeft ;
	unsigned int    tmp3 ;
	short           level [16]; 

	//Decode the coeff_token
	short n = ( ai_iN == LUMA_DC_BLOCK_INDEX )? 0 : ai_iN;
	int tmp = pred_non_zero_count(n, NonZeroCountCache);
	const VLC_TYPE* pstTable = &vlc -> Coeff_token_vlc [vlc -> Coeff_token_vlc_adress [getCoeffTokenIndex(tmp)]];

	int tmp1 = (int)showNbits(ai_poData, *position, 24);
	int index = tmp1 >> (24 - COEFF_TOKEN_VLC_BITS);

	int coeff_token = pstTable [index] . code;
	tmp = pstTable [index] . len ;

	if ( tmp < 0 ) 
	{
		tmp2 = tmp1 & ((1 << (24 - COEFF_TOKEN_VLC_BITS)) - 1);
		index = (tmp2 >> (24 - COEFF_TOKEN_VLC_BITS + tmp)) + coeff_token;
		*position += COEFF_TOKEN_VLC_BITS ;

		coeff_token = pstTable [index] . code ;
		tmp = pstTable [index] . len ;
	}

	*position += tmp ;

	coeff_token_TotalCoeff = TotalCoeff(coeff_token);

	if(ai_iN != LUMA_DC_BLOCK_INDEX){
		NonZeroCountCache [SCAN8(ai_iN)] = coeff_token_TotalCoeff;
	}else{
		NonZeroCountCache [0] = coeff_token_TotalCoeff;
	}


	//Recovery of the values of the ao_piCoeffLevel
	if ( coeff_token_TotalCoeff > 0 )
	{

		tmp = (int) showNbits(ai_poData, *position, 24);
		level [0] = 1 - ((tmp >> (22)) & 2);
		level [1] = 1 - ((tmp >> (21)) & 2);
		level [2] = 1 - ((tmp >> (20)) & 2);
		level [3] = 1 - ((tmp >> (19)) & 2);

		coeff_token_TrailingOnes = TrailingOnes(coeff_token);
		(*position) += coeff_token_TrailingOnes;
		suffixLength =  ((coeff_token_TotalCoeff > 10) && (coeff_token_TrailingOnes < 3)) ? 1 : 0; 


		if ( coeff_token_TrailingOnes < coeff_token_TotalCoeff )    {
			//first coefficient has suffix_length equal to 0 or 1
			int levelCode ;
			level_prefix = read_levelprefix(ai_poData, position, &tmp);


			if(suffixLength == 0){
				if ( level_prefix < 14 )	{
					levelCode = level_prefix; //part
				} 
				else if ( level_prefix == 14 ) 	{
					//levelSuffixSize == 12
					levelCode = 14 + getNbits(ai_poData, position, 4); //part
				} 
				else {
					//level_prefix == 15
					levelCode = 30 + getNbits(ai_poData, position, 12); //part
				}
			}
			else{
				if ( level_prefix < 15 ) {
					levelCode = (level_prefix << 1) + getNbits(ai_poData, position, 1); //part
				}
				else {
					//level_prefix == 15
					//levelSuffixSize == level_prefix - 3
					levelCode = 30 + getNbits(ai_poData, position, 12); //part
				}
			}



			if ( coeff_token_TrailingOnes < 3 ) {
				levelCode += 2 ;
			}

			suffixLength = ( levelCode > 5 )? 2 : 1;

			level [coeff_token_TrailingOnes] = MapUnsignedToShort(levelCode);
			tmp2 = 6 * (1 << (suffixLength - 1));

			//remaining coefficients have suffix_length > 0
			for (i = coeff_token_TrailingOnes + 1; i < coeff_token_TotalCoeff ; i++ ) 
			{
				level_prefix = read_levelprefix(ai_poData, position,(int *) &tmp3);
				tmp = ( level_prefix == 15 )? 12 : suffixLength;

				tmp3 >>= (32 - tmp);
				(*position) += tmp;
				levelCode = (level_prefix << suffixLength) + tmp3;

				level [i] = MapUnsignedToShort(levelCode);

				if ( (levelCode > (tmp2 - 1)) && (suffixLength < 6)) 
				{
					tmp2 <<= 1;
					suffixLength++ ;
				}
			}
		}

		//Decoding of the number of zéro in the block
		if ( coeff_token_TotalCoeff < ai_iMaxNumCoeff ) 
		{
			int cra = (coeff_token_TotalCoeff - 1) << 9;
			zerosLeft = read_ceMax_depth1(ai_poData, position, &vlc -> Total_zeros_vlc [cra], TOTAL_ZEROS_VLC_BITS);
		} 
		else 
		{
			zerosLeft = 0 ;
		}

		//Decoding the number of zéro between two non-zero coefficients
		{

			unsigned int uiScanTableH = 0xf9be4190;
			unsigned int uiScanTableL = 0xef91b904;
			int iIndex;
			short run [16];

			for ( i = 0 ; i < 16 ; i++ ){
				run [i] = 0 ;
			}

			for(i = 0; (i < coeff_token_TotalCoeff - 1) && ( zerosLeft >= 7 ); i++)
			{
				run [i] = read_ceMax_depth2(ai_poData, position, vlc -> Run7_vlc);
				zerosLeft -= run [i];
			}

			if(coeff_token_TotalCoeff < 11)
			{
				// we load 8 values in one showNbits
				tmp2 = (int)showNbits(ai_poData, *position, 24);
				tmp = 24 - RUN_VLC_BITS;
				pstTable = &vlc -> Run_vlc [8 * (zerosLeft - 1)];

				for(; (i < coeff_token_TotalCoeff - 1) && (zerosLeft > 0); i++)
				{
					index = (tmp2 >> tmp) & 7;
					tmp -= pstTable [index] . len ;

					*position += pstTable [index] . len ;
					run [i] = pstTable [index] . code;
					zerosLeft -= run [i];
					pstTable -= (run [i] << 3);
				}
			}
			else
			{
				for (;(i < coeff_token_TotalCoeff - 1) && ( zerosLeft > 0 );i++) 
				{
					run [i] = read_ceMax_depth1(ai_poData, position, &vlc -> Run_vlc [8 * (zerosLeft - 1)], RUN_VLC_BITS);
					zerosLeft -= run [i];
				}
			}


			run [coeff_token_TotalCoeff - 1] = zerosLeft ;

			coeffNum = ( ai_iMaxNumCoeff == 15 )? 0 : -2; 

#ifdef TI_OPTIM
#pragma MUST_ITERATE (1,16,1)
#endif
			//Recording the level using the zig zag scan.
			for ( i = coeff_token_TotalCoeff - 1 ; i >= 0 ; i-- ) 
			{
				coeffNum += (run [i] + 1) << 1 ;
				iIndex = ((((uiScanTableH >> coeffNum) << 2) + ((uiScanTableL >> coeffNum) & 3)) & 0xf);

				ao_piCoeffLevel [iIndex] = level [i];

			}
		}
	}
}




/**
This function permits to decode the VLC on 64 values blocks. 
All the parameters decoded will be stored in differents structures or tables.


@param ao_piCoeffLevel Contains the AC luma level of a 16x16 decoded macroblock.
@param scantable The table which determines the index.
@param ai_poData The NAL unit.
@param position The current position in the NAL.
@param ai_iN Index of the current block.
@param vlc The VLC tables in order to decode the Nal Unit.
@param NonZeroCountCache Specifies the coeff_token of each blocks 4x4 of a macroblock.
*/
void residual_block_cavlc_64( short *   ao_piCoeffLevel, const int scantable[], const unsigned char * RESTRICT ai_poData, int * position, 
							 const int n, const VLC_TABLES *vlc, unsigned char NonZeroCountCache [ RESTRICT ])
{
	int             i ;
	int				coeff_token_TotalCoeff;
	int				coeff_token_TrailingOnes;  
	int    			coeffNum ;
	unsigned int    level_prefix ;
	unsigned int    suffixLength ;
	unsigned int    zerosLeft ;
	unsigned int    tmp3 ;

	short           level [16];

	int tmp5 = (int)showNbits(ai_poData, *position, 24);

	int tmp = pred_non_zero_count(n, NonZeroCountCache);
	const VLC_TYPE* pstTable = &vlc -> Coeff_token_vlc [vlc -> Coeff_token_vlc_adress [getCoeffTokenIndex(tmp)]];

	int index = tmp5 >> (24 - COEFF_TOKEN_VLC_BITS);
	int tmp2 = tmp5 &((1 << (24 - COEFF_TOKEN_VLC_BITS)) - 1);

	int coeff_token = pstTable [index] . code ;
	tmp = pstTable [index] . len ;

	if ( tmp < 0 ) 
	{
		*position += COEFF_TOKEN_VLC_BITS ;
		index = (tmp2 >> (24 - COEFF_TOKEN_VLC_BITS + tmp)) + coeff_token ;

		coeff_token = pstTable [index] . code;
		tmp = pstTable [index] . len;
	}

	*position += tmp ;

	coeff_token_TotalCoeff = TotalCoeff(coeff_token);
	NonZeroCountCache [SCAN8(n)] = coeff_token_TotalCoeff;

	//Recovery of the values of the ao_piCoeffLevel
	if ( coeff_token_TotalCoeff > 0 )
	{
		coeff_token_TrailingOnes = TrailingOnes(coeff_token);

		suffixLength =  ((coeff_token_TotalCoeff > 10) && (coeff_token_TrailingOnes < 3)) ? 1 : 0; 

		tmp = (int)showNbits(ai_poData, *position, 24);
		tmp2 = 22;

		for ( i = 0 ; i < 4 ; i++ ) {
			// Getting the trailing ones sign flag
			level [i] = 1 - ((tmp >> (tmp2)) & 2);
			tmp2--;		
		}

		(*position) += coeff_token_TrailingOnes;

		if ( coeff_token_TrailingOnes < coeff_token_TotalCoeff ) {
			//first coefficient has suffix_length equal to 0 or 1
			int levelCode ;
			level_prefix = read_levelprefix(ai_poData, position, &tmp);


			if(suffixLength == 0){
				if ( level_prefix < 14 )	{
					levelCode = level_prefix; //part
				} 
				else if ( level_prefix == 14 ) 	{
					//levelSuffixSize == 12
					levelCode = 14 + getNbits(ai_poData, position, 4); //part
				} 
				else {
					//level_prefix == 15
					levelCode = 30 + getNbits(ai_poData, position, 12); //part
				}
			}
			else{
				if ( level_prefix < 15 ) {
					levelCode = (level_prefix << 1) + getNbits(ai_poData, position, 1); //part
				}
				else {
					//level_prefix == 15
					//levelSuffixSize == level_prefix - 3
					levelCode = 30 + getNbits(ai_poData, position, 12); //part
				}
			}


			if ( coeff_token_TrailingOnes < 3 ) 
			{
				levelCode += 2 ;
			}

			suffixLength = ( levelCode > 5 )? 2 : 1;

			level [coeff_token_TrailingOnes] = MapUnsignedToShort(levelCode);
			tmp2 = 6 * (1 << (suffixLength - 1));




			//remaining coefficients have suffix_length > 0
			for (i = coeff_token_TrailingOnes + 1; i < coeff_token_TotalCoeff ; i++ ) 
			{
				level_prefix = read_levelprefix(ai_poData, position, (int *)&tmp3);
				tmp = ( level_prefix == 15 )? 12 : suffixLength;

				tmp3 >>= (32 - tmp);
				(*position) += tmp;
				levelCode = (level_prefix << suffixLength) + tmp3;

				level [i] = MapUnsignedToShort(levelCode);

				if ( (levelCode > (tmp2 - 1)) && (suffixLength < 6)) 
				{
					tmp2 <<= 1;
					suffixLength++ ;
				}
			}
		}

		//Decoding of the number of zéro in the block
		if ( coeff_token_TotalCoeff < 16 )  {
			int cra ;
			cra = coeff_token_TotalCoeff - 1 ;
			zerosLeft = read_ceMax_depth1(ai_poData, position, &vlc -> Total_zeros_vlc [cra * 512], TOTAL_ZEROS_VLC_BITS);
		} 
		else{
			zerosLeft = 0 ;
		}

		//Decoding the number of zéro between two non-zero coefficients
		{
			short run [16];

			for ( i = 0 ; i < 16 ; i++ ) {
				run [i] = 0 ;
			}

			for(i = 0;(i < coeff_token_TotalCoeff - 1) && ( zerosLeft >= 7 ); i++){
				run [i] = read_ceMax_depth2(ai_poData, position, vlc -> Run7_vlc);
				zerosLeft -= run [i];
			}

			if(coeff_token_TotalCoeff < 11)
			{
				// we load 8 values in one showNbits
				tmp2 = (int)showNbits(ai_poData, *position, 24);
				tmp = 24 - RUN_VLC_BITS;
				pstTable = &vlc -> Run_vlc [8 * (zerosLeft - 1)];

				for(; (i < coeff_token_TotalCoeff - 1) && (zerosLeft > 0); i++)
				{
					index = (tmp2>>tmp)&7;
					tmp -= pstTable [index].len ;

					*position += pstTable [index].len ;
					run [i] = pstTable [index].code;
					zerosLeft -= run [i];
					pstTable -= (run [i] << 3);
				}
			}
			else
			{
				for (;(i < coeff_token_TotalCoeff - 1) && ( zerosLeft > 0 ); i++) 
				{
					run [i] = read_ceMax_depth1(ai_poData, position, &vlc -> Run_vlc [8 * (zerosLeft - 1)], RUN_VLC_BITS);
					zerosLeft -= run [i];
				}
			}


			run [coeff_token_TotalCoeff - 1] = zerosLeft ;

			coeffNum = -1; 

#ifdef TI_OPTIM
#pragma MUST_ITERATE (1,16,1)
#endif
			//Recording the level using the zig zag scan.
			for ( i = coeff_token_TotalCoeff - 1 ; i >= 0 ; i-- ) 
			{
				coeffNum += (run [i] + 1);
				ao_piCoeffLevel [scantable[coeffNum]] = level [i];

			}
		}
	}
#ifdef MMX
	empty();
#endif
}



/**
This function permits to decode the VLC on 4 values blocks. 
All the parameters decoded will be stored in differents structures or tables.


@param ao_piCoeffLevel Contains the AC luma level of a 16x16 decoded macroblock.
@param position The current position in the NAL.
@param vlc The VLC tables in order to decode the Nal Unit.
*/
void residual_block_cavlc_4_C ( short * ao_piCoeffLevel, const unsigned char * RESTRICT ai_poData, int * position , const VLC_TABLES *vlc)
{

#ifdef TI_OPTIM

	int             i ;
	int             coeff_token ;
	int             tmp ;
	int				coeff_token_TotalCoeff;
	int				coeff_token_TrailingOnes;
	int             levelCode ;
	int    			coeffNum ;

	unsigned int    level_prefix ;
	unsigned int    suffixLength ;
	unsigned int    zerosLeft ;
	unsigned int    tmp2 ;
	unsigned int             tmp3 ; 

	short           level [4];
	const VLC_TYPE *pstTable;
	VLC_TYPE		pstTmp;


	tmp = (int)showNbits(ai_poData, *position, 24);
	// gets the 8 first bits
	tmp2 = ((unsigned int)tmp) >> 16;

	// deduces coeff token
	coeff_token = vlc -> Chroma_dc_coeff_token_vlc [tmp2] . code;

	// tmp2 contains the shifting for next value
	tmp2 = vlc -> Chroma_dc_coeff_token_vlc [tmp2] . len;
	(*position) += tmp2;


	coeff_token_TotalCoeff = TotalCoeff(coeff_token);

	//Recovery of the values of the ao_piCoeffLevel
	if ( coeff_token_TotalCoeff > 0 ){
		coeff_token_TrailingOnes = TrailingOnes(coeff_token);

		tmp2 = 22 - tmp2;

		// gets one by one the coeff_token_TrailingOnes next bits
		// values over coeff_token_TrailingOnes will be erased later
		for ( i = 0 ; i < 4 ; i++ ) 
		{
			// Getting the trailing ones sign flag
			level [i] = 1 - ((tmp >> (tmp2)) & 2);
			tmp2--;
		}

		(*position) += coeff_token_TrailingOnes;

		if ( coeff_token_TrailingOnes < coeff_token_TotalCoeff ) 
		{
			level_prefix = read_levelprefix(ai_poData, position, &tmp);
			//first coefficient has suffix_length equal to 0

			if ( level_prefix < 14 ) 
			{
				levelCode = level_prefix; //part
			} 
			else if ( level_prefix == 14 ) 
			{
				levelCode = 14 + getNbits(ai_poData, position, 4); //part
			} 
			else if ( level_prefix == 15 ) 
			{
				levelCode = 30 + getNbits(ai_poData, position, 12); //part
			}

			if ( coeff_token_TrailingOnes < 3 ) 
			{
				levelCode += 2;
			}

			suffixLength = ( levelCode > 5 )? 2 : 1;

			level [coeff_token_TrailingOnes] = MapUnsignedToShort(levelCode);
			tmp2 = 6 * (1 << (suffixLength - 1));

			//remaining coefficients have suffix_length > 0
			for (i = coeff_token_TrailingOnes + 1; i < coeff_token_TotalCoeff ; i++ ) 
			{
				level_prefix = read_levelprefix(ai_poData, position,(int *) &tmp3);
				tmp = ( level_prefix == 15 )? 12 : suffixLength;

				tmp3 >>= (32 - tmp);
				(*position) += tmp;
				levelCode = (level_prefix << suffixLength) + tmp3;

				level [i] = MapUnsignedToShort(levelCode);

				if (levelCode > (int)(tmp2 - 1)) 
				{
					tmp2 <<= 1;
					suffixLength++;
				}
			}
		}

		//Decoding of the number of zéro in the block
		if ( coeff_token_TotalCoeff < 4 ) 
		{
			zerosLeft = read_ceMax_depth1(ai_poData, position, &vlc -> Chroma_dc_total_zeros_vlc [8 * (coeff_token_TotalCoeff - 1)], CHROMA_DC_TOTAL_ZEROS_VLC_BITS);
		} else 
			zerosLeft = 0 ;

		//Decoding the number of zéro between two non-zero coefficients

		pstTable = &vlc -> Run_vlc [8 * (zerosLeft - 1)];
		tmp = (coeff_token_TotalCoeff > 1)? showNbits_9max(ai_poData, *position, (coeff_token_TotalCoeff - 1) * RUN_VLC_BITS) : 0;
		tmp2 = (coeff_token_TotalCoeff - 2) * RUN_VLC_BITS;

		{
			UINT64 ullRun = 0;
			for(i = 0; i < coeff_token_TotalCoeff - 1; i++)
			{
				if(zerosLeft)
				{
					pstTmp = pstTable [(tmp >> tmp2) & 7];
					ullRun += pstTmp . code;
					pstTable -= (pstTmp . code << 3);
					zerosLeft -= pstTmp . code;
					*position += pstTmp . len;
					tmp2 -= pstTmp . len;
				}
				ullRun <<= 16;
			}

			ullRun += zerosLeft;

			coeffNum = -1 ;

#pragma MUST_ITERATE (1,4,1)
			//Recording the level using the zig zag scan.
			for ( i = coeff_token_TotalCoeff - 1 ; i >= 0 ; i-- ) 
			{
				coeffNum += (int)((ullRun&0xffff) + 1) ;
				ao_piCoeffLevel [coeffNum] = level [i];

				ullRun >>= 16;
			}
		}
	}

#else

	int             i ;
	int				coeff_token_TotalCoeff;
	int				coeff_token_TrailingOnes;
	int    			coeffNum ;
	unsigned int    suffixLength ;
	unsigned int    zerosLeft ;
	unsigned int    tmp3 ; 

	short           level [4];
	const VLC_TYPE *pstTable;
	VLC_TYPE		pstTmp;


	int tmp = (int)showNbits(ai_poData, *position, 24);
	// gets the 8 first bits
	int tmp5 = ((unsigned int)tmp) >> 16;

	// deduces coeff token
	int coeff_token = vlc -> Chroma_dc_coeff_token_vlc [tmp5] . code;

	// tmp2 contains the shifting for next value
	int tmp2 = vlc -> Chroma_dc_coeff_token_vlc [tmp5] . len;
	(*position) += tmp2;


	coeff_token_TotalCoeff = TotalCoeff(coeff_token);

	//Recovery of the values of the ao_piCoeffLevel
	if ( coeff_token_TotalCoeff > 0 )
	{
		coeff_token_TrailingOnes = TrailingOnes(coeff_token);

		tmp2 = 22 - tmp2;

		// gets one by one the coeff_token_TrailingOnes next bits
		// values over coeff_token_TrailingOnes will be erased later
		for ( i = 0 ; i < 4 ; i++ ) 
		{
			// Getting the trailing ones sign flag
			level [i] = 1 - ((tmp >> (tmp2)) & 2);
			tmp2--;
		}

		(*position) += coeff_token_TrailingOnes;

		if ( coeff_token_TrailingOnes < coeff_token_TotalCoeff )   {
			//first coefficient has suffix_length equal to 0
			int   levelCode ;
			int level_prefix = read_levelprefix(ai_poData, position, &tmp);


			if ( level_prefix < 14 ) 
			{
				levelCode = level_prefix; //part
			} 
			else if ( level_prefix == 14 ) 
			{
				levelCode = 14 + getNbits(ai_poData, position, 4); //part
			} 
			else{
				//level_prefix == 15
				levelCode = 30 + getNbits(ai_poData, position, 12); //part
			}

			if ( coeff_token_TrailingOnes < 3 ) 
			{
				levelCode += 2;
			}

			suffixLength = ( levelCode > 5 )? 2 : 1;

			level [coeff_token_TrailingOnes] = MapUnsignedToShort(levelCode);
			tmp2 = 6 * (1 << (suffixLength - 1));

			//remaining coefficients have suffix_length > 0
			for (i = coeff_token_TrailingOnes + 1; i < coeff_token_TotalCoeff ; i++ ) 
			{
				level_prefix = read_levelprefix(ai_poData, position,(int *) &tmp3);
				tmp = ( level_prefix == 15 )? 12 : suffixLength;

				tmp3 >>= (32 - tmp);
				(*position) += tmp;
				levelCode = (level_prefix << suffixLength) + tmp3;

				level [i] = MapUnsignedToShort(levelCode);

				if (levelCode > (int)(tmp2 - 1)) 
				{
					tmp2 <<= 1;
					suffixLength++;
				}
			}
		}

		//Decoding of the number of zéro in the block
		if ( coeff_token_TotalCoeff < 4 )     {
			zerosLeft = read_ceMax_depth1(ai_poData, position, &vlc -> Chroma_dc_total_zeros_vlc [8 * (coeff_token_TotalCoeff - 1)], CHROMA_DC_TOTAL_ZEROS_VLC_BITS);
		} else
			zerosLeft = 0 ;

		//Decoding the number of zéro between two non-zero coefficients

		pstTable = &vlc -> Run_vlc [8 * (zerosLeft - 1)];
		tmp = (coeff_token_TotalCoeff > 1)? showNbits_9max(ai_poData, *position, (coeff_token_TotalCoeff - 1) * RUN_VLC_BITS) : 0;
		tmp2 = (coeff_token_TotalCoeff - 2) * RUN_VLC_BITS;

		{
			int ullRun[4] = {0,0,0,0};
			for(i = 0; i < coeff_token_TotalCoeff - 1; i++)
			{
				if(zerosLeft)
				{
					pstTmp = pstTable [(tmp >> tmp2) & 7];
					ullRun[i] += pstTmp . code;
					pstTable -= (pstTmp . code << 3);
					zerosLeft -= pstTmp . code;
					*position += pstTmp . len;
					tmp2 -= pstTmp . len;
				}
			}

			ullRun[i] += zerosLeft;

			coeffNum = -1 ;

			//Recording the level using the zig zag scan.
			for ( i = coeff_token_TotalCoeff - 1 ; i >= 0 ; i-- ) 
			{
				coeffNum += (int)((ullRun[i]) + 1) ;
				ao_piCoeffLevel [coeffNum] = level [i];
			}

		}
	}

#endif
}







/**
This function permits to decode the VLC. 
All the parameters decoded will be stored in differents structures or tables.

@param ai_pcData The NAL unit.
@param position The current position in the NAL.
@param aio_pstMacroblock Contains all parameters of the current macroblock.
@param vlc The VLC tables in order to decode the Nal Unit.
@param NonZeroCountCache Specifies the coeff_token of each blocks 4x4 of a macroblock.
@param cbp THe coded block pattern.
*/
void residual (const unsigned char *ai_pcData, int *position, RESIDU *Residu, const VLC_TABLES *Vlc,  unsigned char NonZeroCountCache [ ]){

	int i, i8x8, i4x4,ind ;


	//Recovery of the Intra 16x16 luminance DC
	if ( Residu -> MbType == INTRA_16x16 ){
		residual_block_cavlc_16(Residu -> Intra16x16DCLevel, 16, ai_pcData, position, 26, Vlc, NonZeroCountCache);

		if( Residu -> Cbp & 15 ) {
			for( i = 0; i < 16; i++ ) {
				residual_block_cavlc_16(&Residu -> LumaLevel [i * 16], 15, ai_pcData, position, i, Vlc, NonZeroCountCache);
			}
		}else {

			//Updating the non_zero_count_cache table for the neighbourhood
			NonZeroCountCache [12]	= NonZeroCountCache [13] = NonZeroCountCache [14] = NonZeroCountCache [15] = 
				NonZeroCountCache [20] = NonZeroCountCache [21] = NonZeroCountCache [22] = NonZeroCountCache [23] = 
				NonZeroCountCache [28] = NonZeroCountCache [29] = NonZeroCountCache [30] = NonZeroCountCache [31] = 
				NonZeroCountCache [36] = NonZeroCountCache [37] = NonZeroCountCache [38] = NonZeroCountCache [39] = 0;		
		}
	}
	else if (Residu -> Transform8x8 == INTRA_8x8 ){
		for ( i8x8 = 0 ; i8x8 < 4 ; i8x8++ ) {	
			if ( Residu -> Cbp & 1 << i8x8 ) {
				// each luma 8x8 block
				for ( i4x4 = 0 ; i4x4 < 4 ; i4x4++ ) {
					// each 4x4 sub-block of block 
					const int   index = i4x4 + (i8x8 << 2);
					//Recovery of the Intra 4x4 luminance AC
					residual_block_cavlc_64(&Residu -> LumaLevel [i8x8 * 64], zigzag_scan8x8_cavlc + 16 * i4x4, ai_pcData, position, index, Vlc, NonZeroCountCache);

				}
				ind = SCAN8(i8x8 << 2);
				NonZeroCountCache[ind] |= NonZeroCountCache[ind + 1] | 
					NonZeroCountCache[ind + 8] |  NonZeroCountCache[ind + 9];
			} 
			else {
				const int index = SCAN8(i8x8 << 2);
				//Updating the non_zero_count_cache table for the neighbourhood
				NonZeroCountCache [index] = NonZeroCountCache [index + 1] = 
					NonZeroCountCache [index + 8] = NonZeroCountCache [index + 9] = 0 ;

			}
		}
	}
	else
	{
		//Recovery of the luminance AC
		for ( i8x8 = 0 ; i8x8 < 4 ; i8x8++ ){			
			if ( Residu -> Cbp & 1 << i8x8 ) {
				// each luma 8x8 block 
				for ( i4x4 = 0 ; i4x4 < 4 ; i4x4++ ) {
					// each 4x4 sub-block of block 
					const int   index = i4x4 + 4 * i8x8;
					//Recovery of the Intra 4x4 luminance AC
					residual_block_cavlc_16(&Residu -> LumaLevel [index << 4], 16, ai_pcData, position, index, Vlc, NonZeroCountCache);
				}
			} 
			else 
			{
				const int index = SCAN8(i8x8 << 2);
				//Updating the non_zero_count_cache table for the neighbourhood
				NonZeroCountCache [index] = NonZeroCountCache [index + 1]	= 
				NonZeroCountCache [index + 8] = NonZeroCountCache [index + 9] = 0 ;
			}
		}
	}

	//Recovery of the Chrominance DC
	if ( Residu -> Cbp & 0x30 ) {
		//Chrominance Cb
		residual_block_cavlc_4(Residu -> ChromaDCLevel_Cb, ai_pcData, position, Vlc);

		//Chrominance Cr
		residual_block_cavlc_4(Residu -> ChromaDCLevel_Cr, ai_pcData, position, Vlc);
	}


	//Recovery of the Chrominance AC
	if ( Residu -> Cbp & 0x20 )     {
		//Chrominance Cb
		for ( i4x4 = 0 ; i4x4 < 4 ; i4x4++ ) {
			const int   index = 16 + i4x4;		
			residual_block_cavlc_16(&Residu -> ChromaACLevel_Cb [i4x4 * 16], 15, ai_pcData, position, index, Vlc , NonZeroCountCache);
		}

		//Chrominance Cr
		for ( i4x4 = 0 ; i4x4 < 4 ; i4x4++ ) {
			const int   index = 16 + 4 + i4x4 ;            
			residual_block_cavlc_16(&Residu -> ChromaACLevel_Cr [i4x4 << 4], 15, ai_pcData, position, index, Vlc, NonZeroCountCache);
		}
	} 
	else 
	{
		//Updating the non_zero_count_cache table for the neighbourhood
		NonZeroCountCache [9 + 0] = NonZeroCountCache [9 + 1] = 
			NonZeroCountCache [9 + 8] = NonZeroCountCache [9 + 9] = 
			NonZeroCountCache [33 + 0] = NonZeroCountCache [33 + 1] = 
			NonZeroCountCache [33 + 8] = NonZeroCountCache [33 + 9] = 0 ;
	}
}










//initialization
/**
This function permits to fill the VLC table adress. 

@param ao_tiCoeff_token_vlc_adress The adress of the vls table.

*/
void init_coeff_token_vlc_adress ( int ao_tiCoeff_token_vlc_adress [ ] )
{
	ao_tiCoeff_token_vlc_adress [0] = 0 ;
	ao_tiCoeff_token_vlc_adress [1] = 520 ;
	ao_tiCoeff_token_vlc_adress [2] = 852 ;
	ao_tiCoeff_token_vlc_adress [3] = 1132 ;
}




/**
This function permits to fill a VLC table. 

*/
int build_table ( VLC_TYPE *vlc, int table_nb_bits, int nb_codes, const int *bits, const int *codes, int code_prefix, int n_prefix, int position, int *table )
{
	int i, j, k, n, table_size, table_index, nb, n1, index, code_prefix2 ;
	int code ;

	//    VLC_TYPE (*table)[2];
	table_index = table_size = 1 << table_nb_bits ;
	*table = table_index + position ;

	for ( i = 0 ; i < table_size ; i++ ) {
		vlc [i + position].len = 0 ; //bits
		vlc [i + position].code = -1 ; //codes
	}

	/* first pass: map codes and compute auxillary table sizes */
	for ( i = 0 ; i < nb_codes ; i++ ) {
		n = bits[i]; 
		code = codes[i]; 

		/* we accept tables with holes */
		if ( n <= 0 ) 
			continue ;

		/* if code matches the prefix, it is in the table */
		n -= n_prefix ;

		code_prefix2 = code >> n ;

		if ( n > 0 && code_prefix2 == code_prefix ) {
			if ( n <= table_nb_bits ) {

				/* no need to add another table */
				j = code << (table_nb_bits - n) & (table_size - 1) ;
				nb = 1 << (table_nb_bits - n) ;
				for ( k = 0 ; k < nb ; k++ ) {
					if ( 1 & INIT_VLC_LE ) 
						j = (code >> n_prefix) + (k << n);
					if ( vlc [j + position].len /*bits*/ != 0 ) 
						return -1 ;
					vlc [j + position].len = n ; //bits
					vlc [j + position].code = i ; //code
					j++ ;
				}
			} else {
				n -= table_nb_bits ;
				j = code >> (1 & INIT_VLC_LE ? n_prefix : n) & (1 << table_nb_bits) - 1 ;

				/* compute table size */
				n1 = -vlc [j + position].len ; //bits
				if ( n > n1 ) 
					n1 = n ;
				vlc [j + position].len = -n1 ; //bits
			}
		}
	}

	/* second pass : fill auxillary tables recursively */
	for ( i = 0 ; i < table_size ; i++ ) {
		n = vlc [i + position].len ; //bits
		if ( n < 0 ) {
			n = -n ;
			if ( n > table_nb_bits ) {
				n = table_nb_bits ;
				vlc [i + position].len = -n ; //bits
			}
			index = build_table(vlc, n, nb_codes, bits, codes, code_prefix << table_nb_bits | i , n_prefix + table_nb_bits,*table, table);
			if ( index < 0 ) 
				return -1 ;

			/* note: realloc has been done, so reload tables */
			/*  vlc = &vlc -> vlc[table_index];*/
			vlc [i].code = *table - index ; //code
		}
	}
	return table_index ;
}




/**
This function permits to build the VLC  table. 

*/
int init_vlc ( VLC_TYPE *vlc, int nb_bits, int nb_codes, const int *bits, const int *codes)
{
	int    position = 0 ;
	int    table = 0 ;

	if ( build_table(vlc, nb_bits, nb_codes, bits, codes, 0, 0, position, &table) < 0 ) 
		return -1 ;
	return 0 ;
}




/**
This function permits to initialize the VLC tables. 

@param Vlc The structure which contains all the table needed for decoding the vlc
*/
void decode_init_vlc(VLC_TABLES *Vlc){



	int i ;

	init_coeff_token_vlc_adress(Vlc -> Coeff_token_vlc_adress);

	init_vlc(Vlc -> Chroma_dc_coeff_token_vlc, CHROMA_DC_COEFF_TOKEN_VLC_BITS, 4 * 5, &chroma_dc_coeff_token_len [0], &chroma_dc_coeff_token_bits [0]);

	for ( i = 0 ; i < 4 ; i++ ) {
		init_vlc(&Vlc -> Coeff_token_vlc [Vlc -> Coeff_token_vlc_adress [i]], COEFF_TOKEN_VLC_BITS, 4 * 17, &coeff_token_len [i][0], &coeff_token_bits [i][0]);
	}

	for ( i = 0 ; i < 3 ; i++ ) {
		init_vlc(&Vlc -> Chroma_dc_total_zeros_vlc [8 * i], CHROMA_DC_TOTAL_ZEROS_VLC_BITS, 4, &chroma_dc_total_zeros_len [i][0], &chroma_dc_total_zeros_bits [i][0]);
	}

	for ( i = 0 ; i < 15 ; i++ ) {
		init_vlc(&Vlc -> Total_zeros_vlc [i * 512], TOTAL_ZEROS_VLC_BITS, 16, &total_zeros_len [i][0], &total_zeros_bits [i][0]);
	}

	for ( i = 0 ; i < 6 ; i++ ) {
		init_vlc(&Vlc -> Run_vlc [8 * i], RUN_VLC_BITS, 7, &run_len [i][0], &run_bits [i][0]);
	}

	init_vlc(Vlc -> Run7_vlc, RUN7_VLC_BITS, 16, &run_len [6][0], &run_bits [6][0]);
}





/**
This function permits to decode an Exp-Golomb-coded syntax;

ue(v): unsigned integer Exp-Golomb-coded syntax element with the left bit first.
@param tab The NAL unit.
@param position The current aio_piPosition in the NAL.
*/
/*static __inline*/ int read_ue_C ( const unsigned char *tab,  int *pos )
{
	int i;
	int r_value;
	int buf = showNbits(tab, *pos, 24);
	int leadingzeroBits = 0;
	int test = 1 << 23;

	for ( i = 0; i < 24 && leadingzeroBits == 0; i++, test >>= 1)
		leadingzeroBits = buf & test;

	leadingzeroBits = i - 1;

	if(!leadingzeroBits){
		*pos += 1;
		return 0;
	}
	else if(leadingzeroBits < 12){
		*pos += (leadingzeroBits << 1) + 1;
		buf >>= (23 - (leadingzeroBits << 1));

		r_value = (buf - 1);

	}
	else	{
		// we calculate another time leadingzeroBits this time up to 32 bits
		int b = 0 ;
		int leadingzeroBits = 0 ;
		b = getNbits(tab, pos, 1);

		for (  ; !b ; leadingzeroBits++ ) {
			b = getNbits(tab, pos, 1);
		}

		r_value = leadingzeroBits ? (1 << leadingzeroBits) - 1 + getNbits(tab, pos, leadingzeroBits) : 0;
	}

	if(r_value < 0) {
		r_value = 0;
	}
	return r_value;
}


