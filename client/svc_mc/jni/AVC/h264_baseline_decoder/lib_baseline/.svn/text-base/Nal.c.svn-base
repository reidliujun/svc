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

#ifndef CHIP_6416
#ifndef POCKET_PC
//#include <windows.h>
#endif
#endif
#include <stdlib.h>
#include <stdio.h>


#include "nal.h"




/**
This function permits to saerch from a buffer the next start_code.

@param data The NAL unit.
@param offset Size of the start code.
*/
int search_start_code ( unsigned char *data, int *offset)
{
    int i;
    int c = 0 ;
	
    //determination of the length of the start code ( 3 or 4 )
    for (*offset = 0 ; c != 1 ; (*offset)++ ) 
        c = show3Bytes_Nal(data, *offset);

	*offset += 2;
	c = 0 ;
	 //determination of the length of the Nal unit
	for (i = 0 ; c != 1 ; i++ ) 
        c = show3Bytes_Nal(data, (*offset) + i);



	//return of the right size in case of there is not code for the end of the Nal unit
	if ( data[i - 2 + (*offset)] == 0)
		return i - 2;
	else 
		return i - 1;
}





/**
This function permits to get the length of the next NAL.
This function search from the data read from the bistream the next start_code.

@param data The buffer read from the bistream.
@param rbsp_byte Buffer containing the data to decode.
@param NumBytesInRBSP Size of the NAL to decode.
*/
 int get_NalBytesInNalunit ( unsigned char *data, unsigned char *rbsp_byte, int *NumBytesInRBSP)
{

	int i;
	int offset;
	int NBInRBSP = 0;
    int NalInRBSP = search_start_code(data, &offset);

	for ( i = offset ; i < NalInRBSP + offset  ; i++ ) {
        rbsp_byte [NBInRBSP++] = data [i];
		if ( i + 2 < (NalInRBSP + offset) && show3Bytes_Nal(data,  i) == 3 ) {
            rbsp_byte [NBInRBSP++] = data [i++] ;
            i ++ ;
		}
    }

	rbsp_byte [NBInRBSP] = data [i];
	*NumBytesInRBSP =  NalInRBSP + offset;
	return NBInRBSP ;
}



 /**
This function permits to get the length of the next NAL.
This function search from the data read from the bistream the next start_code.

@param data The buffer read from the bistream.
@param rbsp_byte Buffer containing the data to decode.
@param NumBytesInRBSP Size of the NAL to decode.
*/
 int get_NalBytesInNalunit_wo_emulation_start_code ( unsigned char *data,  unsigned char *rbsp_byte, int *NumBytesInRBSP)
{

	int i;
	int offset;
	int NBInRBSP = 0;
    int NalInRBSP = search_start_code(data,&offset);

	for ( i = offset ; i < NalInRBSP + offset  ; i++ ) {
        rbsp_byte [NBInRBSP++] = data [i];
    }

	rbsp_byte [NBInRBSP] = data [i];
	*NumBytesInRBSP =  NalInRBSP + offset;
	return NBInRBSP ;
}



 /**
This function permits to get the length of the next NAL when there is no start code.
This function search from the data read from the bistream the next start_code.

@param data The buffer read from the bistream.
@param length Size of the NAL to decode.
@param rbsp_byte Buffer containing the data to decode.

*/
int readnal_without_start_code(unsigned char *data, const int length, unsigned char *rbsp_byte)
{

	int i;
	int NBInRBSP = 0;

	for ( i = 0 ; i < length  ; i++ ) {
        rbsp_byte [NBInRBSP++] = data [i];
			if ( i + 2 < length && show3Bytes_Nal(data,  i) == 3 ) {
            rbsp_byte [NBInRBSP++] = data [i++] ;
            i ++ ;
		}
    }
	rbsp_byte [NBInRBSP] = data [i];
	
	// Remove 0x00 bytes at the end of NALU
    while (rbsp_byte[NBInRBSP - 1] == 0x00)
    {
      NBInRBSP--;
    }

    return NBInRBSP;
}



/**
This function permits to get the type of the current NAL.

@param data_size number of element to read from the bistream.
@param data_in The buffer read from the bistream.
@param data_out copy the buffer read from the bistream.
@param nal_unit_type The type of the NAL unit.
@param nal_ref_idc Specifies if the current picture is a reference one.
@param end_of_slice Specifies if the current picture is totally decoded
*/
void Nal_unit (unsigned char *data_in, int *nal_unit_type, int *nal_ref_idc)
{

	//Read from the buffer the NAL unit type
	int tmp = data_in[0];
	*nal_ref_idc = tmp >> 5 ;
	*nal_unit_type = tmp & 0x1F ;
}


/**
This function allows to test if the size determinated by the generated code is not bigger than the one fixed in type.h 

@param pic_width Width determined by the code generator.
@param pic_height Height determined by the code generator.
*/
void decoderh264_init(int pic_width, int pic_height){


	if(pic_width > PIC_WIDTH){
		printf("The picture size of the generated code is too large \n");
		printf("The maximum width is %d\n", PIC_WIDTH);
	//	system("pause");
		exit(12);
	}

	if(pic_height > PIC_HEIGHT){
		printf("The picture size of the generated code is too large \n");
		printf("The maximum height is %d\n", PIC_HEIGHT);
	//	system("pause");
		exit(13);
	}
}
