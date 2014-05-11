/*****************************************************************************
*
*  Open SVC Decoder developped in IETR image lab
*
*
*
*              M�d�ric BLESTEL <mblestel@insa-rennes.Fr>
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

#include <string.h> 


//AVC Files
#include "type.h"
#include "nal.h"
#include "bitstream.h"
#include "slice_data_vlc.h"


//SVC Files
#include "svc_type.h"
#include "set_layer.h"
#include "SwitchAdress.h"
#include "pic_list_svc.h"
#include "nal_unit_header_svc_extension.h"

#include <android/log.h>
#define  LOG_TAG    "libsvc"
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

extern int	RemainBytes ;

int RemoveEmulationBytes(unsigned char *data, unsigned char *rbsp_byte, int *BytesRead)
{
	int i;
	int offset;
	int NalInRbsp = 0;
	int NalLenght = search_start_code(data, &offset); //Get start code size

	//Loop over the bytes until next start code
	for ( i = offset ; i < NalLenght + offset; i++ ) {
		rbsp_byte [NalInRbsp++] = data [i];
		if ( i + 2 < (NalLenght + offset) && show3Bytes_Nal(data, i) == 3 ) {
			rbsp_byte [NalInRbsp++] = data [i++] ;
			i ++ ;
		}
	}

	rbsp_byte [NalInRbsp] = data [i];
	*BytesRead = NalLenght + offset;
	return NalInRbsp;
}






/**
This function permits to get the length of the next NAL.
This function search from the data read from the bistream the next start_code.

@param data The buffer read from the bistream.
@param rbsp_byte Buffer containing the data to decode.
@param NumBytesInRBSP Size of the NAL to decode.
*/
int GetNalBytesInNalunit (unsigned char *data, unsigned char *rbsp_byte, int *NumBytesInRBSP, ConfigSVC *StreamType)
{

	int i;
	int offset;
	int nal_unit_type;
	unsigned char *NextNal;
	int NBInRBSP = 0;
	int NalInRBSP = search_start_code(data, &offset);

	for ( i = offset ; i < NalInRBSP + offset; i++ ) {
		rbsp_byte [NBInRBSP++] = data [i];
		if ( i + 2 < (NalInRBSP + offset) && show3Bytes_Nal(data, i) == 3 ) {
			rbsp_byte [NBInRBSP++] = data [i++] ;
			i ++ ;
		}
	}

	rbsp_byte [NBInRBSP] = data [i];
	*NumBytesInRBSP =  NalInRBSP + offset;
//	LOGE("READXX264 : NumBytesInRBSP = %d, RemainBytes = %d", *NumBytesInRBSP, RemainBytes);

	//Get the next Nal to know its type
	if(*NumBytesInRBSP < RemainBytes){
		NalInRBSP = 0;

		//determination of the length of the start code ( 3 or 4 )
		for (offset = 0 ; NalInRBSP != 1 ; offset++ ) 
			NalInRBSP = show3Bytes_Nal(data, offset);

		offset += 2;

		//Get the next nal unit type
		NextNal = &data[i + offset];
		nal_unit_type = NextNal[0] & 0x1F;

		//Get the dqid if it's a SVC nal
		if ( nal_unit_type == 14 || nal_unit_type == 20){
			int Position = 8;
			int ReservedOneBits = getNbits(NextNal, &Position, 9) ;// u(1)  reserved_one_bits =
			int DependencyId = getNbits(NextNal, &Position, 3) ;// u(3)
			int QualityId = getNbits(NextNal, &Position, 4) ;// u(4) quality_id = 
			StreamType -> NextDqId = (DependencyId << 4 ) + QualityId;
		}
	}

	return NBInRBSP ;
}




/**
This function move the beginning of the Rbsp buffer to the next NAL.
*/
void SwapRbspBuffer(unsigned char *rbsp_byte, NALSTREAMER *NalStreamer)
{
	int LastNalSize = NalStreamer -> NalSize[NalStreamer -> NalToStream - 1];
	NalStreamer -> TotalAuSize -= LastNalSize;
	memmove(rbsp_byte, &rbsp_byte[LastNalSize], NalStreamer -> TotalAuSize * sizeof(char));
}


/**
This function reads a whole access unit and computes size of each NAL.
*/
int GetNalBytesAuSVC (unsigned char *data, unsigned char *rbsp_byte, int *ReadNewByte, ConfigSVC *StreamType, 
					  NALSTREAMER *NalStreamer, SPS *Sps, PPS *Pps)
{

	if(*ReadNewByte){
		int ByteRead = 0;
		int *NalSize = NalStreamer -> NalSize;
		char AuComplete = 0;
		unsigned char *NbNal = &NalStreamer -> NbNal;
		unsigned char *Rbsp = rbsp_byte;
		NAL Nal[1];

		init_nal_struct(Nal, 4); // mbl to correct (4 is not good, shall be a variable
		if(NalStreamer -> TotalAuSize){
			NalStreamer -> TotalAuSize = 0;
			NalStreamer -> NbNal = 0;
			NalStreamer -> NalToStream = 0;
			NalStreamer -> TotalDecodedBytes = 0;
			NalStreamer -> FrameCount = 0;
		}


		while(!AuComplete){
			NalSize [*NbNal] = RemoveEmulationBytes(data, Rbsp, &ByteRead);
			AuComplete = IsAuComplete(Rbsp, NalSize[*NbNal], Pps, Sps, Nal, *NbNal, &NalStreamer -> Poc, &NalStreamer -> FrameCount);
			data += ByteRead;
			Rbsp += NalSize[*NbNal];
			NalStreamer -> TotalAuSize += NalSize[*NbNal];
			if(!AuComplete){
				NalStreamer -> TotalDecodedBytes += ByteRead;
			}
			(*NbNal)++;
		}
		StreamType -> DqIdMax = Nal -> DqIdMax;
		*ReadNewByte = 0;
		if(AuComplete == 2){
			//End of stream
			NalStreamer -> NalSize[(*NbNal) - 1] = 0;
			NalStreamer -> NbNal++;
			NalStreamer -> EndOfSequence = NalStreamer -> NbNal;
		}
	}else {
		SwapRbspBuffer(rbsp_byte, NalStreamer);
	}

	if((NalStreamer -> NbNal <= NalStreamer -> NalToStream + 2) && !NalStreamer -> EndOfSequence){//Stream all NAL except the last one which is part from the next AU
		*ReadNewByte = NalStreamer -> TotalDecodedBytes;
		return NalStreamer -> NalSize[NalStreamer -> NalToStream++];
	}else if((NalStreamer -> EndOfSequence <= NalStreamer -> NalToStream + 2) && NalStreamer -> EndOfSequence){
		if(NalStreamer -> EndOfSequence == NalStreamer -> NalToStream + 2){
			return NalStreamer -> NalSize[NalStreamer -> NalToStream++];
		}else{
			return NalStreamer -> NalSize[NalStreamer -> NalToStream];
		}
	}
	return NalStreamer -> NalSize[NalStreamer -> NalToStream++];
}




/**
This function allows to initialize the NAL structure.

*/
void init_nal_struct(NAL *Nal, unsigned char MaxLayer){
	memset(Nal, '\0',sizeof(NAL));
	memset(&Nal -> layer_id_table, -1, 8 * sizeof(unsigned char));
	memset(&Nal -> UsedSpatial, -1, 64 * sizeof(unsigned char));
	Nal -> DqId = 0;
	Nal -> MaxNumLayer = MaxLayer;
	Nal -> TempToDisplay = 255;
}



/**
This function is sure to determine the Nal unit type of the current nal and to initilize the idr flag in case of AVC NAL.
*/
void NalUnitSVC(unsigned char *data_in, int *nal_unit_type, int *nal_ref_idc, NAL *Nal)
{

	//Read from the buffer the NAL unit type
	int tmp = data_in[0];
	Nal -> NalRefIdc = *nal_ref_idc = tmp >> 5 ;
	Nal -> NalUnitType = *nal_unit_type = tmp & 0x1F ;

	//To avoid AVC stream crash 
	Nal -> no_inter_layer_pred_flag = 1;
	if ( *nal_unit_type == 5){
		Nal -> IdrFlag = 1;
	}else {
		Nal -> IdrFlag = 0;
	}
	//Error detection
	//Due when NalUnitType == 14 lost.
	if(Nal -> NalUnitType < 6){
		Nal -> LayerId = 0;
	}
}


/**
This function updates every frames the temporal scalability to display.
*/
void UpdateTempTodisplay(NAL *Nal)
{
	if (Nal -> ComTempId){
		//Test if one command has been specified
		if (Nal -> ComTempId == 3){
			//Specific temporal id
			if(Nal -> TempWanted <= Nal -> MaxTemporalId){
				Nal -> TempToDisplay = Nal -> TempWanted;
			}
		}else if(Nal -> ComTempId == 2){
			//Asked to display an upper temporal scalability
			if(Nal -> TempToDisplay < Nal -> MaxTemporalId){
				Nal -> TempToDisplay++;
			}
		}else if(Nal -> ComTempId == 1){
			//Asked to display an lower temporal scalability
			if(Nal -> TempToDisplay > Nal -> MaxTemporalId){
				Nal -> TempToDisplay = Nal -> MaxTemporalId;
			}
			if(Nal -> TempToDisplay > 0){
				Nal -> TempToDisplay--;
			}
		}
		Nal -> ComTempId = 0;
	}
}


/**
This functions sets to actived or desactived NAL decoding process.
*/
void SetPicToDisplay(NAL *Nal)
{

	UpdateTempTodisplay(Nal);

	if (Nal -> TemporalId <= Nal -> TempToDisplay){
		//We want to display this layer
		Nal -> NalToSkip = 0;
	}else{
		Nal -> NalToSkip = 1;
	}


	if( (Nal -> DqId < Nal -> DqIdToDisplay && Nal -> DiscardableFlag) || (Nal -> DqId > Nal -> DqIdToDisplay) ){
		//DqId superior to the layer wanted, or inferior but can be discarded.
		//The layer is discarded, only its poc is calculated
		Nal -> PicToDisplay = 0;	//Reset parameters
		Nal -> NalToSkip = 1;		//Reset parameters
	}else if (Nal -> DqId < Nal -> DqIdToDisplay && Nal -> DqId != Nal -> DqIdMax){
		Nal -> PicToDisplay = 0;	//Reset parameters
	}else{
		Nal -> PicToDisplay = !Nal -> NalToSkip; //Display depends on the nal decoding
	}


	if(!Nal -> NalToSkip){
		Nal -> LastDecodedId = Nal -> LayerId;
	}
}



/**
Parses the NAL unit header.
*/
void NalUnitData(unsigned char *Data, int *position, NAL *Nal, int *EndOfSlice)
{

	getNbits(Data, position, 1) ;// u(1)  reserved_one_bits =
	Nal -> IdrFlag = getNbits(Data, position, 1) ;// u(1)  idr_flag =
	getNbits(Data, position, 6) ;// u(6)  priority_id = 
	Nal -> no_inter_layer_pred_flag = getNbits(Data, position, 1) ;// u(1) no_inter_layer_pred_flag
	Nal -> DependencyId = getNbits(Data, position, 3) ;// u(3)
	Nal -> QualityId = getNbits(Data, position, 4) ;// u(4) quality_id = 
	Nal -> DqId = ( Nal -> DependencyId << 4 ) + Nal -> QualityId;
	Nal -> TemporalId = getNbits(Data, position, 3) ;// u(3) temporal_id = 
	Nal -> UseRefBasePicFlag = getNbits(Data, position, 1) ;// u(1)
	Nal -> DiscardableFlag = getNbits(Data, position, 1) ;// u(1)   discardable_flag = 
	getNbits(Data, position, 1) ;//output_flag All u(1)
	getNbits(Data, position, 2) ;//reserved_three_2bits

	Nal -> LayerId = GetLayerNumber(Nal, Nal -> DqId);

	//Initialize the dependency Max if necessary
	if (Nal -> DependencyIdMax < Nal -> DependencyId){
		Nal -> DependencyIdMax = Nal -> DependencyId;
	}

	//Initialize the DqId Max if necessary
	if (Nal -> DqIdMax < Nal -> DqId){
		Nal -> DqIdMax = Nal -> DqId;
		Nal -> DqIdToDisplay = Nal -> DqIdWanted;
	}

	//Initialize the Temporal Id Max if necessary
	if (Nal -> MaxTemporalId < Nal -> TemporalId){
		Nal -> MaxTemporalId = Nal -> TemporalId;
	}

	Nal -> ErrorDetected = 0;
	if(Nal -> LastDqId != Nal -> DqId && *EndOfSlice){
		//*EndOfSlice = 0;
		Nal -> ErrorDetected = 1;
	}
}


/** 
This function is used before an AVC NAL, in order to describe to the decoder the Nal caracteristic.

*/
void PrefixNalUnit(unsigned char *Data, int *NalInRbsp, NAL *Nal, MMO *Mmo, SPS *Sps, int *EndOfSlice)
{

	int position = 8;


	//Get NAL header information
	NalUnitData(Data, &position, Nal, EndOfSlice);

	//Initialize the dependency Max if necessary
	if (Nal -> DqIdMax < Nal -> DqId){
		Nal -> DqIdMax = Nal -> DqId;
		Nal -> DqIdToDisplay = Nal -> DqIdWanted;
	}

	//Initialize the dependency Max if necessary
	if (Nal -> DependencyIdMax < Nal -> DependencyId){
		Nal -> DependencyIdMax = Nal -> DependencyId;
	}

	//Assume that prefix indicates DqId = 0
	Nal -> layer_id_table [0] = Nal -> DqId;

	if( Nal -> NalRefIdc != 0 ) {
		Nal -> StoreRefBasePicFlag = getNbits(Data, &position, 1) ;
		if( ( Nal -> UseRefBasePicFlag || Nal -> StoreRefBasePicFlag ) && !Nal -> IdrFlag){
			DecRefBasePicMarking(Data, &position, Mmo, Sps, Nal);
		}

		if( getNbits(Data, &position, 1) == 1 ){
			while( more_rbsp_data(Data, &position, NalInRbsp ) ){
				getNbits(Data, &position, 1);//prefix_nal_unit_extension_flag 2 u(1)
			}
		}
	}
}







/**
This function is used to know the layer's caracteristic as the layer ID, the temporal ID ...

@param data The NAL unit.
@param NalInRbsp NAL unit's Size.
@param
*/
void NalUnitHeader (unsigned char *Data, int *position, NAL *Nal, int *EndOfSlice)
{	

	int i, j;


	*position = 8;
	Nal -> LastLayerId = Nal -> LayerId;
	Nal -> LastDqId = Nal -> DqId;
	Nal -> LastTemporalId = Nal -> TemporalId;

	//Get NAL header information
	NalUnitData(Data, position, Nal, EndOfSlice);


	for ( i = 1; i < 8; i++){
		if ( Nal -> layer_id_table[i] == -1){
			Nal -> layer_id_table[i] = Nal -> DqId;
			Nal -> LayerId = i;
			break;
		}
		if(Nal -> layer_id_table[i] == Nal -> DqId){
			Nal -> LayerId = i;
			break;
		}
		if(Nal -> layer_id_table[i] > Nal -> DqId){
			Nal -> LayerId = i;
			for ( j = i; j < 7; j ++){
				Nal -> layer_id_table[i + 1] =  Nal -> layer_id_table[i];
			}
			Nal -> layer_id_table[i] = Nal -> DqId;
			break;
		}
	}

	//Player: ParseAu
	if (Nal -> DqIdMax == 255 && Nal -> LayerId > Nal -> MaxNumLayer - 1){
		Nal -> LayerId = Nal -> MaxNumLayer - 1;
	}
}





/**
This function try to determine the DqId of the next NAL if it's possible.
*/
void NextNalDqId(ConfigSVC *StreamType, NAL *Nal)
{

	//For the decoder, we can detect and calculate the nextDqId;
	Nal -> DqIdMax = StreamType -> DqIdMax;

	//Configure command
	Nal -> ComDqId = StreamType -> SetLayer;
	Nal -> ComTempId = StreamType -> TemporalCom;
	if( StreamType -> TemporalCom == 3){
		Nal -> TempWanted = StreamType -> TemporalId;
	}


	if (Nal -> layer_id_table [0] == -1){
		//Just done the first time
		memcpy(Nal -> layer_id_table, StreamType -> layer_id_table, 8 * sizeof(char));
		memcpy(Nal -> UsedSpatial, StreamType -> UsedSpatial, 64 * sizeof(char));
	}
}



/**
This function try to determine the DqId of the next NAL if it's possible.
Only used by players
*/
void NextNalDqIdPlayer(ConfigSVC *StreamType, NAL *Nal, int *CommandLayer)
{

	//For players, we know the DqIdMax
	if(Nal -> DqIdMax < CommandLayer[0]){
		//When missing layer occurs, DqIdToDisplay is changed to continue displaying a layer.
		//When the layer is back, we have to set back DqIdToDisplay to the old value
		Nal -> DqIdToDisplay = Nal -> DqIdWanted;
	}
	Nal -> DqIdMax = CommandLayer[0];
	//Configure command
	Nal -> ComDqId = CommandLayer[1];
	if(!Nal -> ComTempId){//No command already set
		Nal -> ComTempId = CommandLayer[2];
		CommandLayer[2] = 0;
	}
	if( Nal -> ComTempId == 3){
		Nal -> TempWanted = CommandLayer[3];
	}
}




/**
This function determines if the current layer has to be fully or partially decoded 
Nal -> PicToDisplay == 1 => Nal -> PicToDecode == 1  ==> fully decoded
Nal -> PicToDisplay == 0 and CUrrent layer needed for upper layer => Nal -> PicToDecode == 1  ==> partially decoded
*/
void IsPicToDecode(NAL *Nal){
	// if we want to display this layer, or we need for spatial enhencement.
	int WantedLayerNumber = GetLayerNumber (Nal, Nal -> DqIdToDisplay);
	Nal -> PicToDecode = Nal -> PicToDisplay || (Nal -> UsedSpatial [Nal -> LayerId] [WantedLayerNumber] > 0);
}
