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

//#ifdef ERROR_CONCEALMENT
//AVC File
#include "type.h"
#include "data.h"
#include "init_data.h"

//SVC Files
#include "svc_type.h"
#include "Coordonates.h"
#include "WriteBackSVC.h"
#include "ErrorConcealment.h"


/**
This function sets all macroblock data to those given.
*/
void ErrorSetMbType(const SPS *Sps, SLICE *Slice, NAL *Nal, RESIDU *Residu, DATA *Macroblocks, unsigned char *SliceTable, unsigned char MbType)
{
	short iCurrMbAddr;
	short PicSizeInMbs = Sps -> PicSizeInMbs;
	short PicWidthInMbs = Sps -> pic_width_in_mbs;

	for (iCurrMbAddr = 0; iCurrMbAddr < PicSizeInMbs ; iCurrMbAddr ++) {
		if (SliceTable [iCurrMbAddr] == 255){
			init_residu(&Residu[iCurrMbAddr]);
			SliceTable [iCurrMbAddr] = Slice -> slice_num;
			Residu[iCurrMbAddr] . MbType = MbType;
			if( MbType == I_BL){
				//Base macroblock addresses have to be computed
				//initialize macroblock position
				short iMb_x, iMb_y;
				GetMbPosition(iCurrMbAddr, PicWidthInMbs, &iMb_x, &iMb_y);
				GetBaseMbAddr(Nal, &Macroblocks[iCurrMbAddr], iMb_x << 4, iMb_y << 4);
			}
		}
	}
}


/**
This function fills macrblock data for a P/B frame in an AVC layer.
*/
void AVCPBErrorConcealment(const SPS *Sps, SLICE *Slice, unsigned char *SliceTable, RESIDU *Residu,
						 unsigned char MbType, short *MvL0, short *MvL1, short *RefL0, short *RefL1)
{

	short iCurrMbAddr;
	const short PicWidthInMbs = Sps -> pic_width_in_mbs;
	const short PicSizeInMbs = Sps -> PicSizeInMbs;


	__declspec(align(64)) short RefCacheL0 [48];
	__declspec(align(64)) short MvCacheL0 [48][2];


	memset(RefCacheL0, 0, 48 * sizeof(short));
	init_mv_cache(MvCacheL0);


	for (iCurrMbAddr = 0; iCurrMbAddr < PicSizeInMbs ; iCurrMbAddr ++) {
		if (SliceTable [iCurrMbAddr] == 255){
			short iMb_x, iMb_y;
			GetMbPosition(iCurrMbAddr, PicWidthInMbs, &iMb_x, &iMb_y);

			init_residu(&Residu[iCurrMbAddr]);
			SliceTable [iCurrMbAddr] = Slice -> slice_num;
			Residu[iCurrMbAddr] . MbType = MbType;
			write_back_motion_full_ref( Slice -> b_stride, Slice -> b8_stride,
				&MvL0[(iMb_x << 3) + iMb_y * (Slice -> b_stride << 2)], MvCacheL0,
				&RefL0[(iMb_x << 2) + iMb_y * (Slice -> b_stride << 1)], RefCacheL0 );
			write_back_motion_full_ref( Slice -> b_stride, Slice -> b8_stride,
				&MvL1[(iMb_x << 3) + iMb_y * (Slice -> b_stride << 2)], MvCacheL0,
				&RefL1[(iMb_x << 2) + iMb_y * (Slice -> b_stride << 1)], RefCacheL0 );
		}
	}
}



/*
This function sets macroblock type when error detection is activated.
*/
void AVCErrorConcealment(NAL *Nal, const SPS *Sps, SLICE *Slice, unsigned char *SliceTable, RESIDU *Residu,
						 DATA *Macroblocks, short *MvL0, short *MvL1, short *RefL0, short *RefL1)
{
	if (Nal -> ErrorDetection && Slice -> mb_xy < Sps -> PicSizeInMbs){
		//AVC Error concealment
		//Contrary to the SVC case, as motion vector are compute in the same time that the VLD,
		// we have to set its to a value.
		if( Slice -> slice_type != SLICE_TYPE_I){
			//Mb type is determined according to the slice type.
			//For AVC layer, motion vector and reference index are set here.
			unsigned char MbType = P_Skip - Slice -> slice_type; 
			AVCPBErrorConcealment(Sps, Slice, SliceTable, Residu, MbType, MvL0, MvL1, RefL0, RefL1);
		}else {
			//For intra frame, Mb type is set to a 16x16 macroblock with DC prediction
			ErrorSetMbType(Sps, Slice, Nal, Residu, Macroblocks, SliceTable, INTRA_16x16); 
		}
	}
}


/*
This function sets macroblock type when error detection is activated.
*/
void SVCErrorConcealment(NAL *Nal, const SPS *Sps, SLICE *Slice, unsigned char *SliceTable, 
						 RESIDU *Residu, DATA *Macroblocks)
{
	if (Nal -> ErrorDetection && Slice -> mb_xy < Sps -> PicSizeInMbs){
		//SVC Error concealment
		if (Slice -> slice_type != SLICE_TYPE_I){
			unsigned char MbType = P_Skip - Slice -> slice_type; 
			ErrorSetMbType(Sps, Slice, Nal, Residu, Macroblocks, SliceTable, MbType); //Skip macroblock in all case
		}else if(Nal -> DqId){
			unsigned char MbType = I_BL; 
			ErrorSetMbType(Sps, Slice, Nal, Residu, Macroblocks, SliceTable, MbType); //I_BL for spatial and SNR enhancement
		}
	}
}
//#endif
