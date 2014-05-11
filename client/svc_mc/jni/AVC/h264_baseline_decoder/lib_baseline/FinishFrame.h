
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


#ifndef FINSIH_FRAME_H
#define FINSIH_FRAME_H

void FinishFrameError(SPS *sps_id, LIST_MMO *Current_pic, int *Crop, int *ImgToDisplay, int *AdressPic, 
				 MMO *Mmo, int *xsize, int *ysize);

void FinishFrame(SPS *Sps, PPS *Pps, LIST_MMO *Current_pic, SLICE *Slice, int EndOfSlice, unsigned char *SliceTab, DATA *TabBlbock, 
				 RESIDU *Residu, short *MvL0, short *MvL1, short *RefL0, short *RefL1, int *Crop, int *ImgToDisplay, int *AdressPic, 
				 MMO *Mmo, unsigned char *RefY, unsigned char *RefU, unsigned char *RefV, int *xsize, int *ysize);

#endif
