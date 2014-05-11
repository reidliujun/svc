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

#ifndef NAL_UNIT_H
#define NAL_UNIT_H


#include "svc_type.h"

void init_nal_struct(NAL *Nal, unsigned char MaxNumLayer);
void IsPicToDecode(NAL *Nal);
void SetDefRecPic(NAL *Nal, int *do_dec_ref_pic_marking_base);
void SetPicToDisplay(NAL *Nal);
void CheckSkipPicToDisplay(NAL *Nal, int EndOfSlice);
void NextNalDqId(ConfigSVC *StreamType, NAL *Nal);
void NextNalDqIdPlayer(ConfigSVC *StreamType, NAL *Nal, int *CommandLayer);
void PrefixNalUnit(unsigned char *Data, int *NalInRbsp, NAL *Nal, MMO *Mmo, SPS *Sps, int *EndOfSlice);
void NalUnitSVC(unsigned char *data_in, int *nal_unit_type, int *nal_ref_idc, NAL *Nal);
void subset_sps (unsigned char *data, int *NalInRbsp, SPS *sps, NAL *Nal);
void NalUnitHeader (unsigned char *Data, int *position, NAL *Nal, int *EndOfSlice);
void NalUnitData(unsigned char *Data, int *position, NAL *Nal, int *EndOfSlice);
int IsAuComplete(unsigned char *NalInRbsp, int NalInRBSP, PPS *Pps, SPS *Sps, NAL *Nal, 
				 unsigned char NalCount, short *Poc, unsigned char *FrameCount);
#endif
