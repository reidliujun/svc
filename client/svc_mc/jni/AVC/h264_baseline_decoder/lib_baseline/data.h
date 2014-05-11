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

#ifndef DATA_H
#define DATA_H
		


#include "type.h"
#include "symbol.h"






/**
This function permits to divide a number by an another one.
// divides num by den and returns remainder (num >> 16) and result (num & 7fff)

@param num The operande.
@param den The divisor.
*/

static __inline int divide ( int num, int den ) // num and den ranges are 15 bits
{
#ifdef TI_OPTIM

	den = den << 7 ; // align denominator 
    num = _subc(num, den);
    num = _subc(num, den);
    num = _subc(num, den);
    num = _subc(num, den);
    num = _subc(num, den);
    num = _subc(num, den);
    num = _subc(num, den);
    num = _subc(num, den);

   

#else
	num = (num/den) + ((num%den) << 16);
#endif
 	return (num);
}


/**
Compute the x and y position of the macroblock into the frame.
*/
static __inline void GetMbPosition ( short iCurrMbAddr, short PicWidthInMbs, short *iMb_x, short *iMb_y )
{
#ifdef TI_OPTIM
	*iMb_y = divide(iCurrMbAddr, PicWidthInMbs);
	*iMb_x = (*iMb_y >> 8);
	*iMb_y = (*iMb_y & 0x7f);
#else
	*iMb_x = iCurrMbAddr % PicWidthInMbs ;
	*iMb_y = iCurrMbAddr / PicWidthInMbs ;
#endif

}

static const short zigzag_scan [16 + 2 * 4] = { 4 + 1 * 8, 5 + 1 * 8, 4 + 2 * 8, 5 + 2 * 8, 6 + 1 * 8, 7 + 1 * 8,  6 + 2 * 8, 7 + 2 * 8
										 , 4 + 3 * 8, 5 + 3 * 8, 4 + 4 * 8, 5 + 4 * 8, 6 + 3 * 8, 7 + 3 * 8,  6 + 4 * 8, 7 + 4 * 8
										 , 1 + 1 * 8, 2 + 1 * 8, 1 + 2 * 8, 2 + 2 * 8, 1 + 4 * 8, 2 + 4 * 8,  1 + 5 * 8, 2 + 5 * 8};
#define SCAN8(x) zigzag_scan[x]

/*
#ifdef TI_OPTIM
static const short zigzag_scan [16 + 2 * 4] = { 4 + 1 * 8, 5 + 1 * 8, 4 + 2 * 8, 5 + 2 * 8, 6 + 1 * 8, 7 + 1 * 8,  6 + 2 * 8, 7 + 2 * 8
										 , 4 + 3 * 8, 5 + 3 * 8, 4 + 4 * 8, 5 + 4 * 8, 6 + 3 * 8, 7 + 3 * 8,  6 + 4 * 8, 7 + 4 * 8
										 , 1 + 1 * 8, 2 + 1 * 8, 1 + 2 * 8, 2 + 2 * 8, 1 + 4 * 8, 2 + 4 * 8,  1 + 5 * 8, 2 + 5 * 8};



#define SCAN8(x) zigzag_scan[x]
#else

static __inline short Scan8 (const short i)
{
		short ni = ~i;
		short i2 = (i & 4) >> 2;
		short i3 = (i & 8) >> 3;
		short i4 = (i & 16) >> 4;
		return ((((i & 2) << i3) + (ni&(i3 << 1))) << 3) + ((ni & 2) << 2) + ((ni & 16) >> 2) + (i2 << 1) + i4 + (i&1) + 22 - (22 &((int)(i4 & i2) - 1));
}


#define SCAN8(x) Scan8(x)


#endif*/




static const int zigzag_scan8x8[64]={ 0 + 0 * 8, 1 + 0 * 8, 0 + 1 * 8, 0 + 2 * 8, 1 + 1 * 8, 2 + 0 * 8, 3 + 0 * 8, 2 + 1 * 8
									, 1 + 2 * 8, 0 + 3 * 8, 0 + 4 * 8, 1 + 3 * 8, 2 + 2 * 8, 3 + 1 * 8, 4 + 0 * 8, 5 + 0 * 8
									, 4 + 1 * 8, 3 + 2 * 8, 2 + 3 * 8, 1 + 4 * 8, 0 + 5 * 8, 0 + 6 * 8, 1 + 5 * 8, 2 + 4 * 8
									, 3 + 3 * 8, 4 + 2 * 8, 5 + 1 * 8, 6 + 0 * 8, 7 + 0 * 8, 6 + 1 * 8, 5 + 2 * 8, 4 + 3 * 8
									, 3 + 4 * 8, 2 + 5 * 8, 1 + 6 * 8, 0 + 7 * 8, 1 + 7 * 8, 2 + 6 * 8, 3 + 5 * 8, 4 + 4 * 8
									, 5 + 3 * 8, 6 + 2 * 8, 7 + 1 * 8, 7 + 2 * 8, 6 + 3 * 8, 5 + 4 * 8, 4 + 5 * 8, 3 + 6 * 8
									, 2 + 7 * 8, 3 + 7 * 8, 4 + 6 * 8, 5 + 5 * 8, 6 + 4 * 8, 7 + 3 * 8, 7 + 4 * 8, 6 + 5 * 8
									, 5 + 6 * 8, 4 + 7 * 8, 5 + 7 * 8, 6 + 6 * 8, 7 + 5 * 8, 7 + 6 * 8, 6 + 7 * 8, 7 + 7 * 8};


static short	 qP_tab[58]={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,29,30,31,32,32,33,34,34,35,35,36,36,37,37,37,38,38,38,39,39,39,39,39,39,39,39,39,39};


static   const IMbInfo i_mb_type_info [27]= { 
	{INTRA_4x4,0,0},
	{INTRA_16x16, 0,0},
	{INTRA_16x16, 1,0},
	{INTRA_16x16, 2,0},
	{INTRA_16x16, 3,0},
	{INTRA_16x16, 0,16},
	{INTRA_16x16, 1,16},
	{INTRA_16x16, 2,16},
	{INTRA_16x16, 3,16},
	{INTRA_16x16, 0,32},
	{INTRA_16x16, 1,32},
	{INTRA_16x16, 2,32},
	{INTRA_16x16, 3,32},
	{INTRA_16x16, 0,0+15},
	{INTRA_16x16, 1,0+15},
	{INTRA_16x16, 2,0+15},
	{INTRA_16x16, 3,0+15},
	{INTRA_16x16, 0,16+15},
	{INTRA_16x16, 1,16+15},
	{INTRA_16x16, 2,16+15},
	{INTRA_16x16, 3,16+15},
	{INTRA_16x16, 0,32+15},
	{INTRA_16x16, 1,32+15},
	{INTRA_16x16, 2,32+15},
	{INTRA_16x16, 3,32+15},
	{INTRA_PCM, 0,0},
    {I_BL,6,NA}
	};



static const PMbInfo p_mb_type_info[5]={{Pred_L0   , Pred_L0 , NA      , 1, 3},
										{Pred_L0   , Pred_L0 , Pred_L0 , 2, 2},
										{Pred_L0   , Pred_L0 , Pred_L0 , 2, 1},
										{P_8x8     , Pred_L0 , NA      , 4, 0},
										{P_8x8ref0 , Pred_L0 , NA      , 4, 0}};





static const int sub_num_p_part[5]={1, 2, 2, 4, 0};
static const int sub_num_b_part[13]={4, 1, 1, 1, 2, 2, 2, 2, 2, 2, 4, 4, 4};



static const PMbInfo b_mb_type_info[23]={
{B_direct   , B_direct  , NA	  , NA, 0},
{B_L0	    , Pred_L0   , Pred_L0 ,  1, 3},
{B_L1       , Pred_L1	, Pred_L1 ,  1, 3},
{B_Bi       , B_Bi		, B_Bi	  ,  1, 3},
{B_L0_L0    , Pred_L0	, Pred_L0 ,  2, 2},
{B_L0_L0    , Pred_L0	, Pred_L0 ,  2, 1},
{B_L1_L1    , Pred_L1	, Pred_L1 ,  2, 2},
{B_L1_L1    , Pred_L1	, Pred_L1 ,  2, 1},
{B_L0_L1    , Pred_L0	, Pred_L1 ,  2, 2},
{B_L0_L1    , Pred_L0	, Pred_L1 ,  2, 1},
{B_L1_L0    , Pred_L1	, Pred_L0 ,  2, 2},
{B_L1_L0    , Pred_L1	, Pred_L0 ,  2, 1},
{B_L0_Bi    , Pred_L0	, B_Bi    ,  2, 2},
{B_L0_Bi    , Pred_L0	, B_Bi    ,  2, 1},
{B_L1_Bi    , Pred_L1	, B_Bi    ,  2, 2},
{B_L1_Bi    , Pred_L1	, B_Bi    ,  2, 1},
{B_Bi_L0    , B_Bi	    , Pred_L0 ,  2, 2},
{B_Bi_L0    , B_Bi	    , Pred_L0 ,  2, 1},
{B_Bi_L1    , B_Bi	    , Pred_L1 ,  2, 2},
{B_Bi_L1    , B_Bi	    , Pred_L1 ,  2, 1},
{B_Bi_Bi    , B_Bi	    , B_Bi    ,  2, 2},
{B_Bi_Bi    , B_Bi	    , B_Bi    ,  2, 1},
{B_8x8      , Pred_L0	, NA	  ,  4, 0}
};



#endif
