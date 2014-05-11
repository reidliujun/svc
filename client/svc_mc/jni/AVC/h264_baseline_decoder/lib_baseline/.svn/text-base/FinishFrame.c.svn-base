
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
#include "type.h"
#include "filter.h"
#include "pic_list.h"
#include "AVCdisplay.h"
#include "FinishFrame.h"
#include "display_management.h"




/**
Used when error has been detected into a slice, to execute the MMO operation and the display management
*/
void FinishFrameError(SPS *sps_id, LIST_MMO *Current_pic, int *Crop, int *ImgToDisplay, int *AdressPic, 
				 MMO *Mmo, int *xsize, int *ysize){

	//Short and long term management
	execute_ref_pic_marking(Current_pic, Mmo, 0);

	//Display managment.
	display_order(sps_id, Current_pic, AdressPic, xsize, ysize, Crop, ImgToDisplay, Mmo);
}



/**
When all slice are decoded, we execute the MMO operation and the loop filter, and the display management.
*/
void FinishFrame(SPS *Sps, PPS *Pps, LIST_MMO *Current_pic, SLICE *Slice, int EndOfSlice, unsigned char *SliceTab, DATA *TabBlbock, 
				 RESIDU *Residu, short *MvL0, short *MvL1, short *RefL0, short *RefL1, int *Crop, int *ImgToDisplay, int *AdressPic, 
				 MMO *Mmo, unsigned char *RefY, unsigned char *RefU, unsigned char *RefV, int *xsize, int *ysize)
{


	if (!EndOfSlice){
		unsigned int PicWidthInPix = (Sps -> pic_width_in_mbs + 2) << 4   ;
		unsigned int PicHeightInPix = (Sps -> pic_height_in_map_units ) << 4 ;
		unsigned char *Y = &RefY[Current_pic -> MemoryAddress];
		unsigned char *U = &RefU[Current_pic -> MemoryAddress >> 2];
		unsigned char *V = &RefV[Current_pic -> MemoryAddress >> 2];

		//Short and long term management
		execute_ref_pic_marking(Current_pic, Mmo, 0);

		if( Slice -> disable_deblocking_filter_idc != 1){
			//Deblocking filter if necessary
			filter_P(Sps, Pps, Slice, SliceTab, MvL0, RefL0, Residu, Y, U, V);
		}

		//Padding for motion interpolation
		ImageSetEdges(Y, U, V, PicWidthInPix, PicHeightInPix);

		//Display managment.
		display_order(Sps, Current_pic, AdressPic, xsize, ysize, Crop, ImgToDisplay, Mmo);
	}
}
