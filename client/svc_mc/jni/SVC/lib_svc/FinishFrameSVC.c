
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
//#include "type.h"
#include "symbol.h"
#include "pic_list.h"
#include "display_management.h"


//SVC Files
#include "svc_type.h"
#include "set_layer.h"
#include "SVCDisplay.h"
#include "pic_list_svc.h"
#include "Loop_filter_ext.h"





/**
Used when error has been detected into a slice, to execute the MMO operation and the display management
*/
void FinishFrameSVCError(int Execute, NAL *Nal, SPS *StructSPS, PPS *Pps, LIST_MMO *Current_pic, int *Crop, 
						 int *ImgToDisplay, int *AdressPic, MMO *Mmo, int *xsize, int *ysize)
{

	int pic_parameter_set_id = Nal -> pic_parameter_id [Nal -> LayerId];
	SPS *sps_id = &StructSPS[Pps[pic_parameter_set_id] . seq_parameter_set_id + (Nal -> LayerId ? 16: 0)];


	//Sps must be defined
	if(Execute){
		//Short and long term management
		execute_ref_pic_marking(Current_pic, Mmo, Nal -> LayerId);
	}

	//Display managment.
	DisplayOrderSvc(sps_id, Current_pic, Nal, AdressPic, xsize, ysize, Crop, ImgToDisplay, Mmo);
}




/**
This function applies the deblocking filter if necessary and stores the decoded pictures.
*/
void FinishFrameSVC(const int NbMacro, NAL *Nal, SPS *Sps, PPS *Pps, LIST_MMO *Current_pic, SLICE *Slice, int EndOfSlice, 
					unsigned char *SliceTab, DATA *TabBlbock, RESIDU *Residu, short *MvL0, short *MvL1, short *RefL0, 
					short *RefL1, int *Crop, int *ImgToDisplay, int *AdressPic, MMO *Mmo,  
					unsigned char *RefY, unsigned char *RefU, unsigned char *RefV, int *xsize, int *ysize)
{


	if (!EndOfSlice && Mmo -> MemoryAllocation){
		//Short and long term management
		execute_ref_pic_marking(Current_pic, Mmo, Nal -> LayerId);

		if ( Nal -> PicToDisplay){
			const int PicWidthInPix = (Sps -> pic_width_in_mbs + 2) << 4;
			const int PicHeightInPix = (Sps -> pic_height_in_map_units ) << 4;
			short *mvL0 = &MvL0[Current_pic -> MvMemoryAddress];
			short *mvL1 = &MvL1[Current_pic -> MvMemoryAddress];
			short *refL0 = &RefL0[Current_pic -> MvMemoryAddress >> 1];
			short *refL1 = &RefL1[Current_pic -> MvMemoryAddress >> 1];
			unsigned char *Y = &RefY[Current_pic -> MemoryAddress];
			unsigned char *U = &RefU[Current_pic -> MemoryAddress >> 2];
			unsigned char *V = &RefV[Current_pic -> MemoryAddress >> 2];

			//Deblocking filter
			if (!Nal -> DqId){
				Loop_filter_avc(NbMacro, 0, Sps, Pps, Slice, Current_pic, SliceTab, TabBlbock, mvL0, mvL1, refL0, refL1, Residu, Y, U, V);
			}else {
				Loop_filter_svc(NbMacro, Nal, Sps, Pps, Slice, Current_pic, SliceTab, mvL0, mvL1, refL0, refL1, Residu, Y, U, V);
			}

			//Padding for motion interpolation
			ImageSetEdges(Y, U, V, PicWidthInPix, PicHeightInPix);
		}

		//Display managment.
		DisplayOrderSvc(Sps, Current_pic, Nal, AdressPic, xsize, ysize, Crop, ImgToDisplay, Mmo);
	}else {
		Nal -> LastDqIdMax = Nal -> DqIdMax;
	}
}
