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
#include "data.h"
#include "symbol.h"
#include "neighbourhood.h"

//SVC Files
#include "svc_type.h"
#include "svc_data.h"
#include "svc_neighbourhood.h"








/**
This functions initializes macroblock neighbourhood.
*/
void get_base_neighbourhood( short *Mask, short Addr, const short iAddr_x, const short PicWidthInMbs, 
							const RESIDU *residu, const short PicSizeInMbs, const int IntraConstrained){

	const short iAddr_xp1 = (iAddr_x + 1) != PicWidthInMbs;

    //6.4.4
	short mbAddrA = Addr - 1 ;
	short mbAddrE = Addr + 1;
	short mbAddrD = Addr - PicWidthInMbs - 1;
	short mbAddrB = Addr - PicWidthInMbs;
	short mbAddrC = Addr - PicWidthInMbs + 1;
	short mbAddrF = Addr + PicWidthInMbs - 1;
	short mbAddrG = Addr + PicWidthInMbs;
	short mbAddrH = Addr + PicWidthInMbs + 1 ;

	*Mask = 0;

	if( IntraConstrained){
		if(mbAddrA >= 0) *Mask =  (short ) (IS_I_SVC(residu[mbAddrA] . MbType) && (iAddr_x != 0)   && NeighbourAvail(mbAddrA, PicSizeInMbs, residu[mbAddrA] . SliceNum, residu[Addr] . SliceNum));
		if(mbAddrB >= 0) *Mask += (short ) (IS_I_SVC(residu[mbAddrB] . MbType)					   && NeighbourAvail(mbAddrB, PicSizeInMbs, residu[mbAddrB] . SliceNum, residu[Addr] . SliceNum)) << 1;
		if(mbAddrC >= 0) *Mask += (short ) (IS_I_SVC(residu[mbAddrC] . MbType) && (iAddr_xp1 != 0) && NeighbourAvail(mbAddrC, PicSizeInMbs, residu[mbAddrC] . SliceNum, residu[Addr] . SliceNum)) << 2;
		if(mbAddrD >= 0) *Mask += (short ) (IS_I_SVC(residu[mbAddrD] . MbType) && (iAddr_x != 0)   && NeighbourAvail(mbAddrD, PicSizeInMbs, residu[mbAddrD] . SliceNum, residu[Addr] . SliceNum)) << 3;
		if(mbAddrE >= 0) *Mask += (short ) (IS_I_SVC(residu[mbAddrE] . MbType) && (iAddr_xp1 != 0) && NeighbourAvail(mbAddrE, PicSizeInMbs, residu[mbAddrE] . SliceNum, residu[Addr] . SliceNum)) << 4;
		if(mbAddrF >= 0) *Mask += (short ) (IS_I_SVC(residu[mbAddrF] . MbType) && (iAddr_x != 0)   && NeighbourAvail(mbAddrF, PicSizeInMbs, residu[mbAddrF] . SliceNum, residu[Addr] . SliceNum)) << 5;
		if(mbAddrG >= 0) *Mask += (short ) (IS_I_SVC(residu[mbAddrG] . MbType)					   && NeighbourAvail(mbAddrG, PicSizeInMbs, residu[mbAddrG] . SliceNum, residu[Addr] . SliceNum)) << 6;
		if(mbAddrH >= 0) *Mask += (short ) (IS_I_SVC(residu[mbAddrH] . MbType) && (iAddr_xp1 != 0) && NeighbourAvail(mbAddrH, PicSizeInMbs, residu[mbAddrH] . SliceNum, residu[Addr] . SliceNum)) << 7;
	}else {
		if(mbAddrA >= 0) *Mask =  (short ) (IS_I_SVC(residu[mbAddrA] . MbType) && (iAddr_x != 0)   && svc_neighbour_avail(mbAddrA, PicSizeInMbs));
		if(mbAddrB >= 0) *Mask += (short ) (IS_I_SVC(residu[mbAddrB] . MbType)					   && svc_neighbour_avail(mbAddrB, PicSizeInMbs)) << 1;
		if(mbAddrC >= 0) *Mask += (short ) (IS_I_SVC(residu[mbAddrC] . MbType) && (iAddr_xp1 != 0) && svc_neighbour_avail(mbAddrC, PicSizeInMbs)) << 2;
		if(mbAddrD >= 0) *Mask += (short ) (IS_I_SVC(residu[mbAddrD] . MbType) && (iAddr_x != 0)   && svc_neighbour_avail(mbAddrD, PicSizeInMbs)) << 3;
		if(mbAddrE >= 0) *Mask += (short ) (IS_I_SVC(residu[mbAddrE] . MbType) && (iAddr_xp1 != 0) && svc_neighbour_avail(mbAddrE, PicSizeInMbs)) << 4;
		if(mbAddrF >= 0) *Mask += (short ) (IS_I_SVC(residu[mbAddrF] . MbType) && (iAddr_x != 0)   && svc_neighbour_avail(mbAddrF, PicSizeInMbs)) << 5;
		if(mbAddrG >= 0) *Mask += (short ) (IS_I_SVC(residu[mbAddrG] . MbType)					   && svc_neighbour_avail(mbAddrG, PicSizeInMbs)) << 6;
		if(mbAddrH >= 0) *Mask += (short ) (IS_I_SVC(residu[mbAddrH] . MbType) && (iAddr_xp1 != 0) && svc_neighbour_avail(mbAddrH, PicSizeInMbs)) << 7;
	}
}
