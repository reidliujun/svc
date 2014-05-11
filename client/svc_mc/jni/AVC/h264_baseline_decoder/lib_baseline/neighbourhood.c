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
#define C_NEIGHBOURHOOD



#include "symbol.h"
#include "type.h"
#include "data.h"
#include "neighbourhood.h"
#include "prediction_mode_8x8.h"





/**
This function allows to do an intra 8x8 interpolation.

@param predict_sample Sample to initialize according to the mode.
@param PicWidthInPix Width of the sample to interpolate.
@param macro Specifies the availability of the neighbouring macroblock.
@param locx Position in abciss in the current macroblock.
@param locy Position in ordonne in the current macroblock.
@param mode Mode of the interpolation.
*/
void mbAddrN_8x8(unsigned char * predict_sample, const short PicWidthInPix, int Mask, const short locx, const short locy,  const int mode)
{
	NEIGHBOUR       left [1];
	NEIGHBOUR       top [1];
	NEIGHBOUR       top_right [1];
	NEIGHBOUR       top_left [1];
	int Avail;

	// For macroblocks A, B C & D
	left -> xN = top_left -> xN = locx - 1;
	top -> xN = locx ; 
	left -> yN = locy;
	top_right -> xN = locx + 8 ;
	top -> yN = top_left -> yN = top_right -> yN = locy - 1;


	//Retrieving neighbourhood
	neighbouring_locations_luma(Mask, top_left);
	neighbouring_locations_luma(Mask, top_right);

	//Faster to it here than only in the DC case
	neighbouring_locations_luma(Mask, top);
	neighbouring_locations_luma(Mask, left);


	Avail =  (left -> Avail << 3) + (top -> Avail << 2) + (top_right -> Avail << 1) + top_left -> Avail;

	switch(mode){
		case 0:
			predict_8x8_v(predict_sample,PicWidthInPix, Avail);	break;
		case 1:
			predict_8x8_h (predict_sample,PicWidthInPix, Avail); break;
		case 2:	
			predict_8x8_dc (predict_sample, PicWidthInPix, Avail); break;
		case 3:
			predict_8x8_ddl (predict_sample, PicWidthInPix, Avail);	break;
		case 4:
			predict_8x8_ddr (predict_sample, PicWidthInPix, Avail);	break;
		case 5:
			predict_8x8_vr (predict_sample, PicWidthInPix, Avail);	break;
		case 6:
			predict_8x8_hd (predict_sample, PicWidthInPix, Avail); break;
		case 7:
			predict_8x8_vl(predict_sample, PicWidthInPix, Avail); break;
		case 8:
			predict_8x8_hu (predict_sample, PicWidthInPix, Avail);	break;
	}
}




/**
Process to get the neighbouring and their availability

@param aio_pstMacro Macro the availability of the neighbouring macroblock.
@param iAddr_x Abciss of the current macroblock in the current picture.
@param PicWidthInPix Width of the sample to interpolate.
@param SliceTable Table which contains for each macroblock the number of the slice they belong to.
@param aio_tstBlock Contains all parameters of each macroblock of the current picture.
@param constained_pred_flag Parameter to determine the intra prediction constrained or not.
*/
void GetNeighbour(short Addr, short iAddr_x, const short PicWidthInMbs, const short PicSizeInMbs, 
				  const unsigned char *SliceTable, RESIDU *CurrResidu, const int constained_pred_flag)
{


	const int iAddr_xp1 = (iAddr_x + 1) != PicWidthInMbs;

	int Mask = 0;
	//6.4.4
	int mbAddrA = Addr - 1 ;
	int mbAddrE = Addr + 1;
	int mbAddrD = Addr - PicWidthInMbs - 1;
	int mbAddrB = Addr - PicWidthInMbs;
	int mbAddrC = Addr - PicWidthInMbs + 1;
	int mbAddrF = Addr + PicWidthInMbs - 1;
	int mbAddrG = Addr + PicWidthInMbs;
	int mbAddrH = Addr + PicWidthInMbs + 1 ;


	if (constained_pred_flag){
		if(mbAddrA >= 0) Mask =   IS_I( CurrResidu [mbAddrA] . MbType) && (iAddr_x != 0)   && NeighbourAvail(mbAddrA, PicSizeInMbs, SliceTable[mbAddrA], SliceTable[Addr]);
		if(mbAddrB >= 0) Mask += (IS_I( CurrResidu [mbAddrB] . MbType) &&					  NeighbourAvail(mbAddrB, PicSizeInMbs, SliceTable[mbAddrB], SliceTable[Addr])) << 1;
		if(mbAddrC >= 0) Mask += (IS_I( CurrResidu [mbAddrC] . MbType) && (iAddr_xp1 != 0) && NeighbourAvail(mbAddrC, PicSizeInMbs, SliceTable[mbAddrC], SliceTable[Addr])) << 2;
		if(mbAddrD >= 0) Mask += (IS_I( CurrResidu [mbAddrD] . MbType) && (iAddr_x != 0)   && NeighbourAvail(mbAddrD, PicSizeInMbs, SliceTable[mbAddrD], SliceTable[Addr])) << 3;

		//For SVC
		//Working only with on slice per frame
		if(mbAddrE >= 0) Mask += (IS_I( CurrResidu [mbAddrE] . MbType) && (iAddr_xp1 != 0) && NeighbourAvail(mbAddrE, PicSizeInMbs, SliceTable[mbAddrE], SliceTable[Addr])) << 4;
		if(mbAddrF >= 0) Mask += (IS_I( CurrResidu [mbAddrF] . MbType) && (iAddr_x != 0)   && NeighbourAvail(mbAddrF, PicSizeInMbs, SliceTable[mbAddrF], SliceTable[Addr])) << 5;
		if(mbAddrG >= 0) Mask += (IS_I( CurrResidu [mbAddrG] . MbType)					   && NeighbourAvail(mbAddrG, PicSizeInMbs, SliceTable[mbAddrG], SliceTable[Addr])) << 6;
		if(mbAddrH >= 0) Mask += (IS_I( CurrResidu [mbAddrH] . MbType) && (iAddr_xp1 != 0) && NeighbourAvail(mbAddrH, PicSizeInMbs, SliceTable[mbAddrH], SliceTable[Addr])) << 7;
	}else{
		if(mbAddrA >= 0) Mask =  ((iAddr_x != 0)   && NeighbourAvail(mbAddrA, PicSizeInMbs, SliceTable[mbAddrA], SliceTable[Addr]));
		if(mbAddrB >= 0) Mask +=	(				  NeighbourAvail(mbAddrB, PicSizeInMbs, SliceTable[mbAddrB], SliceTable[Addr])) << 1;
		if(mbAddrC >= 0) Mask += ((iAddr_xp1 != 0) && NeighbourAvail(mbAddrC, PicSizeInMbs, SliceTable[mbAddrC], SliceTable[Addr])) << 2;
		if(mbAddrD >= 0) Mask += ((iAddr_x != 0)   && NeighbourAvail(mbAddrD, PicSizeInMbs, SliceTable[mbAddrD], SliceTable[Addr])) << 3;
		if(mbAddrE >= 0) Mask += ((iAddr_xp1 != 0) && NeighbourAvail(mbAddrE, PicSizeInMbs, SliceTable[mbAddrE], SliceTable[Addr])) << 4;
		if(mbAddrF >= 0) Mask += ((iAddr_x != 0)   && NeighbourAvail(mbAddrF, PicSizeInMbs, SliceTable[mbAddrF], SliceTable[Addr])) << 5;
		if(mbAddrG >= 0) Mask +=	(			      NeighbourAvail(mbAddrG, PicSizeInMbs, SliceTable[mbAddrG], SliceTable[Addr])) << 6;
		if(mbAddrH >= 0) Mask += ((iAddr_xp1 != 0) && NeighbourAvail(mbAddrH, PicSizeInMbs, SliceTable[mbAddrH], SliceTable[Addr])) << 7;
	}

	CurrResidu [Addr] . AvailMask = Mask;
}								
