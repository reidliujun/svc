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


extern const int zigzag_scan8x8_cavlc[64];



#ifdef MMX

/**
This function permits to decode the VLC on 4 values blocks. 
All the parameters decoded will be stored in differents structures or tables.


@param ao_piCoeffLevel Contains the AC luma level of a 16x16 decoded macroblock.
@param position The current position in the NAL.
@param vlc The VLC tables in order to decode the Nal Unit.
*/


void residual_block_cavlc_4_MMX ( short * ao_piCoeffLevel, const unsigned char * ai_poData, int * position , const VLC_TABLES *vlc)
{

    int             i ;
    int             coeff_token ;
    int             tmp ;
    int             tmp4 ;
    int				coeff_token_TotalCoeff;
	int				coeff_token_TrailingOnes;
    int             levelCode ;
    int    			coeffNum ;

    unsigned int    level_prefix ;
    unsigned int    suffixLength ;
    unsigned int    zerosLeft ;
	unsigned int    tmp2 ;
    unsigned int    tmp3 ; 

    __m64           level;const VLC_TYPE *pstTable;
	VLC_TYPE		pstTmp;

   	

	tmp = (int)showNbits(ai_poData, *position, 24);
	// gets the 8 first bits
	tmp2 = ((unsigned int)tmp) >> 16;

	// deduces coeff token
	coeff_token = vlc -> Chroma_dc_coeff_token_vlc[tmp2] . code;

	// tmp2 contains the shifting for next value
	tmp2 = vlc -> Chroma_dc_coeff_token_vlc [tmp2] . len;
    (*position) += tmp2;

		
	coeff_token_TotalCoeff = TotalCoeff(coeff_token);

	//Recovery of the values of the ao_piCoeffLevel
    if ( coeff_token_TotalCoeff > 0 )
	{
		const __m64 cst1 = _mm_set_pi32 (0x00010001,0x00010001);

			// Getting the trailing ones sign flag
		coeff_token_TrailingOnes = TrailingOnes(coeff_token);

		tmp =( ((unsigned int)tmp) >> (19 - tmp2));
		level = _mm_setr_pi16 ((short)(tmp >> 3), (short)(tmp >> 2),(short)(tmp >> 1), (short)tmp);
		level = _mm_and_si64 (level, _mm_slli_si64(cst1,1));
		level = _mm_sub_pi16 (cst1, level);

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
			else 
			{
				levelCode = 30 + getNbits(ai_poData, position, 12); //part
			}

			levelCode += ((int)( coeff_token_TrailingOnes < 3 ) <<1) ;

			suffixLength = (int)( levelCode > 5 ) + 1;

			tmp = -(levelCode & 1);
			tmp = (((2 + levelCode) >> 1) ^ tmp) - tmp;

//mbl compil
#ifdef INTEL
			level = _mm_insert_pi16(level,tmp, coeff_token_TrailingOnes);
#else
			level.m64_i16[coeff_token_TrailingOnes] = tmp;
#endif


			tmp2 = 6 * (1 << (suffixLength - 1));

            //remaining coefficients have suffix_length > 0
            for (i = coeff_token_TrailingOnes + 1; i < coeff_token_TotalCoeff ; i++ ) 
            {
                level_prefix = read_levelprefix(ai_poData, position, &tmp3);
				tmp = ( level_prefix == 15 )? 12 : suffixLength;

				tmp3 >>= (32 - tmp);
				(*position) += tmp;
                levelCode = (level_prefix << suffixLength) + tmp3;

				tmp4 = -(levelCode & 1);
				tmp4 = (((2 + levelCode) >> 1) ^ tmp4) - tmp4;
//mbl compil
#ifdef INTEL
				level = _mm_insert_pi16(level,tmp4, i);
#else
				level.m64_i16[i] = tmp4;
#endif

				if (levelCode > (int)(tmp2 - 1)) 
				{
					tmp2 <<= 1;
                    suffixLength++ ;
				}
            }
        }

		//Decoding of the number of zéro in the block
        if ( coeff_token_TotalCoeff < 4 ) 
        {
			zerosLeft = read_ceMax_depth1(ai_poData, position, &vlc -> Chroma_dc_total_zeros_vlc [8 * (coeff_token_TotalCoeff - 1)],
                    	  CHROMA_DC_TOTAL_ZEROS_VLC_BITS);
		} else zerosLeft = 0 ;

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
					ullRun[i] += pstTmp.code;
					pstTable -= (pstTmp.code << 3);
					zerosLeft -= pstTmp.code;
					*position += pstTmp . len;
					tmp2 -= pstTmp . len;
				}
			}

			ullRun[i] += zerosLeft;
	       
			coeffNum = -1 ;

			//Recording the level using the zig zag scan.
			for ( i = coeff_token_TotalCoeff - 1 ; i >= 0 ; i-- ) {
				coeffNum += (int)((ullRun[i]) + 1);
//mbl pb with intel
#ifdef INTEL
				ao_piCoeffLevel [coeffNum] = (short) _mm_extract_pi16(level,i);
#else
				ao_piCoeffLevel [coeffNum] = level.m64_i16 [i];
#endif
			}
		}
	}

	empty();    
}



void residual_block_cavlc_16_MMX( short *   ao_piCoeffLevel, const int ai_iMaxNumCoeff, const unsigned char *ai_poData, int *   position
							 , const int ai_iN,	const VLC_TABLES *vlc, unsigned char NonZeroCountCache [  ])
{

	
    int             i,tmp4, tmp5 ;
   	int				coeff_token_TotalCoeff;
	int				coeff_token_TrailingOnes;  
    int             levelCode ;
    int    			coeffNum ;
    unsigned int    level_prefix ;
    unsigned int    suffixLength ;
    unsigned int    zerosLeft ;
	unsigned int    tmp3 ;

	__m64           level [4];




	//Decode the coeff_token
	short flag2 = -(int)( ai_iN != LUMA_DC_BLOCK_INDEX );
	short n = ai_iN & flag2; 

    int tmp1 = pred_non_zero_count(n, NonZeroCountCache);
	const VLC_TYPE*  pstTable = &vlc -> Coeff_token_vlc [vlc -> Coeff_token_vlc_adress [getCoeffTokenIndex(tmp1)]];

    int tmp2 = tmp4 = showNbits (ai_poData, *position, 24);
	int index = tmp2 >> (24 - COEFF_TOKEN_VLC_BITS);

	int coeff_token = pstTable [index] . code ;
	int tmp = pstTable [index] . len ;

	int flag = -((int)(tmp < 0));

	tmp2 = tmp2 & ((1 <<(24 - COEFF_TOKEN_VLC_BITS)) - 1);
	index = ((index & (~flag)) | (((tmp2 >> (24 - COEFF_TOKEN_VLC_BITS + tmp)) + coeff_token) & flag) );

	tmp5 = COEFF_TOKEN_VLC_BITS & flag;

	coeff_token = pstTable [index] . code ;
	tmp = pstTable [index] . len ;

	tmp5 += tmp;

	*position += tmp5 ;

	coeff_token_TotalCoeff = coeff_token >> 2;

	tmp = flag2 & SCAN8(ai_iN);
	NonZeroCountCache [tmp] = coeff_token_TotalCoeff; //((NonZeroCountCache [tmp] & (~flag2)) | ((coeff_token_TotalCoeff) & (flag2)) );

	//Recovery of the values of the ao_piCoeffLevel
	if ( coeff_token_TotalCoeff > 0 )
	{
		const __m64 cst1 = _mm_set_pi32 (0x00010001,0x00010001);
		__m64 tmp64;

			// Getting the trailing ones sign flag
		coeff_token_TrailingOnes = coeff_token & 3;

		suffixLength =  (unsigned int)((coeff_token_TotalCoeff > 10) && (coeff_token_TrailingOnes < 3)); 

		tmp =( ((unsigned int)tmp4) >> (19 - tmp5));
		tmp64 = _mm_setr_pi16 ((short)(tmp >> 3), (short)(tmp >> 2),(short)(tmp >> 1), (short)tmp);
		tmp64 = _mm_and_si64 (tmp64, _mm_slli_si64(cst1,1));
		tmp64 = _mm_sub_pi16 (cst1, tmp64);
		*level = tmp64;

		(*position) += coeff_token_TrailingOnes;

        if ( coeff_token_TrailingOnes < coeff_token_TotalCoeff ) 
        {
            level_prefix = read_levelprefix(ai_poData, position, &tmp);
            //first coefficient has suffix_length equal to 0 or 1

			levelCode = level_prefix;

			if((suffixLength != 0) || (level_prefix >= 14))
			{
				if(suffixLength == 0)
				{
					if ( level_prefix == 14 ) 
					{
						levelCode = 14 + showNbits_9max(ai_poData, *position, 4); //part
						(*position) += 4;
					} 
					else 
					{
						levelCode = 30 + getNbits(ai_poData, position, 12); //part
					}
				}
				else
				{
					if ( level_prefix < 15 ) 
					{
						levelCode = (level_prefix << 1) + showNbits_9max(ai_poData, *position, 1); //part
						(*position) += 1;
					}
					else 
					{
						levelCode = (level_prefix << 1) + getNbits(ai_poData, position, 12); //part
					}
				}
			}

            levelCode += ((int)( coeff_token_TrailingOnes < 3 )) << 1 ;

			suffixLength = ( levelCode > 5 )? 2 : 1;

            *(((short*)level) + coeff_token_TrailingOnes) = MapUnsignedToShort(levelCode);
			tmp2 = 6*(1<<(suffixLength-1));

            //remaining coefficients have suffix_length > 0
            for (i = coeff_token_TrailingOnes+1; i < coeff_token_TotalCoeff ; i++ ) 
            {
                level_prefix = read_levelprefix(ai_poData, position, &tmp3);
				tmp = ( level_prefix == 15 )? 12 : suffixLength;

				tmp3 >>= (32-tmp);
				(*position) +=tmp;
                levelCode = (level_prefix << suffixLength) + tmp3;

                *(((short*)level) + i) = MapUnsignedToShort(levelCode);

				tmp3 = (levelCode > (tmp2 - 1)) && (suffixLength < 6);
				
				tmp2 <<= tmp3;
                suffixLength += tmp3;
            }
        }

		//Decoding of the number of zéro in the block
        if ( coeff_token_TotalCoeff < ai_iMaxNumCoeff ) 
        {
			zerosLeft = read_ceMax_depth1(ai_poData, position, &vlc -> Total_zeros_vlc[(coeff_token_TotalCoeff - 1) << 9], TOTAL_ZEROS_VLC_BITS);
		} 
		else 
		{
            zerosLeft = 0 ;
		}

		//Decoding the number of zéro between two non-zero coefficients
		{
			short run [16];
			const __m64 cstScanTable64 = _mm_set_pi32 (0xfeb7adc9,0x63258410);
			int iIndex;

			memset(run,0, 16 * sizeof(short));
			
			for(i = 0;(i < coeff_token_TotalCoeff - 1) && ( zerosLeft >= 7 ); i++)
			{
				*(run + i) = read_ceMax_depth2(ai_poData, position, vlc -> Run7_vlc);
				zerosLeft -= *(run + i);
			}

			// we load all values in one showNbits
			tmp64 = showNbits_64 (ai_poData, *position, 56);
			tmp = 56 - RUN_VLC_BITS;
			pstTable = &vlc -> Run_vlc [8 * (zerosLeft - 1)];

			for(; (i < coeff_token_TotalCoeff - 1) && (zerosLeft > 0); i++)
			{
				index = (_mm_cvtsi64_si32(_mm_srli_si64(tmp64,tmp) )) & 7;
				tmp -= pstTable [index].len ;

				*position += pstTable [index].len ;
				tmp2 = *(run + i) = pstTable [index].code;
				zerosLeft -= tmp2;
				pstTable -= (tmp2 << 3);
			}


			*(((short*)run) + coeff_token_TotalCoeff - 1) = zerosLeft ;
       
			coeffNum = ( ai_iMaxNumCoeff == 15 )? 0 : -4; 

			//Recording the level using the zig zag scan.
			for ( i = coeff_token_TotalCoeff - 1 ; i >= 0 ; i-- ) 
			{
				coeffNum += (*(run + i) + 1) << 2 ;
				iIndex = _mm_cvtsi64_si32(_mm_srli_si64(cstScanTable64,coeffNum)) & 0xf;

				ao_piCoeffLevel [iIndex] = *(((short*)level) + i);

			}

		}
	}
	empty();    
}


#endif
