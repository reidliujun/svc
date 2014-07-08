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

#include "string.h"

//AVC Files
#include "type.h"
#include "nal.h"
#include "sps.h"
#include "Read_h264.h"
#include "pic_list_svc.h"

//SVC Files
#include "svc_type.h"
#include "SliceHeaderExtension.h"
#include "nal_unit_header_svc_extension.h"




/**
Finds a change into NAL header.
*/
int IsNalHeaderChanged(NAL *Nal)
{
	if(Nal -> TemporalId != Nal -> LastTemporalId){
		return 1;
	}
	if(Nal -> DqId < Nal -> LastDqId){
		return 1;
	}
	return 0;
}


/**
This functions tries to detect the end of AU.
*/
int IsAuComplete(unsigned char *NalInRbsp, int NalInRBSP, PPS *Pps, SPS *Sps, NAL *Nal, 
				 unsigned char NalCount, short *Poc, unsigned char *FrameCount)
{
	int position = 8, OldPoc;
	int NalRefIdc, NalUnitType, AuNotFinished = 0, EndOfSlice = 0;
	NalUnitSVC(NalInRbsp, &NalUnitType, &NalRefIdc, Nal);

	switch(NalUnitType)
	{
	case 1:
	case 5:
		OldPoc = *Poc;
		*Poc = SliceHeaderCut(NalInRbsp, Sps, Pps, Nal, &position);
		if(OldPoc != *Poc && (*FrameCount)){
			AuNotFinished = 1;
		}
		(*FrameCount)++;
		break;
	case 7:
		seq_parameter_set(NalInRbsp, Sps);
		break;
	case 8:
		PPSCut(NalInRbsp, Pps, Sps, NalInRBSP);
		break;
	case 11:
		//Stream finished
		AuNotFinished = 2;
		break;
	case 14:
		Nal -> LastLayerId = Nal -> LayerId;
		Nal -> LastDqId = Nal -> DqId;
		Nal -> LastTemporalId = Nal -> TemporalId;
		Nal -> layer_id_table[0] = 0;
		NalUnitData(NalInRbsp, &position, Nal, &EndOfSlice);
		if(NalCount){
			AuNotFinished = IsNalHeaderChanged(Nal);
		}
		break;
	case 15:
		subset_sps(NalInRbsp, &NalInRBSP, Sps, Nal);
		break;
	case 20:
		Nal -> LastLayerId = Nal -> LayerId;
		Nal -> LastDqId = Nal -> DqId;
		Nal -> LastTemporalId = Nal -> TemporalId;
		NalUnitHeader(NalInRbsp, &position, Nal, &EndOfSlice);
		if(NalCount){
			AuNotFinished = IsNalHeaderChanged(Nal);
		}
		if(!AuNotFinished){
			OldPoc = *Poc;
			*Poc = SliceHeaderCut(NalInRbsp, Sps, Pps, Nal, &position);
			if(OldPoc != *Poc && (*FrameCount)){
				AuNotFinished = 1;
			}
			(*FrameCount)++;
		}
		break;
	}
	return AuNotFinished;
}



/**
Parse the first AU to detect stream type.
*/
void ParseFirstAU(int StreamSize, unsigned char *NalInRbsp, ConfigSVC *StreamType){

	int PosFile = 0;
	int pos_o_1 = StreamSize;

	int offset = 4;
	int AuNotFinished = 0;
	int NalInRBSP;
	NAL Nal[1];
	SPS Sps[SPS_BUF_SIZE];
	PPS Pps[PPS_BUF_SIZE];
	unsigned char NalCount = 0;
	unsigned char FrameCount =0;
	short Poc = 0;
	init_nal_struct(Nal, 8);
	readh264(StreamSize, pos_o_1, &PosFile, NalInRbsp);
	NalInRBSP = search_start_code(NalInRbsp, &offset);
	NalInRbsp += 4;


	while (!AuNotFinished){
		AuNotFinished = IsAuComplete(NalInRbsp, NalInRBSP, Pps, Sps, Nal, NalCount, &Poc, &FrameCount);
		NalInRbsp += NalInRBSP;
		NalInRBSP = search_start_code(NalInRbsp, &offset);
		NalInRbsp += offset;
		NalCount++;
	}

	StreamType -> DqIdMax = Nal -> DqIdMax;
	memcpy(StreamType -> layer_id_table, Nal -> layer_id_table, 8 * sizeof(char));
	memcpy(StreamType -> UsedSpatial, Nal -> UsedSpatial, 64 * sizeof(char));
}
