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


int			RemainBytes = 0;

#ifndef ARM

#define READ_NO_END_


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


//AVC Files
#include "help.h"
#include "clip.h"
#include "Read_h264.h"


static FILE *ptfile ;

static int  file_size ;
static int	nb_repeat = 0;
int         nb_octet_already_read = 0;
int         Filesize (FILE *f) ;
static char *ARG_INPUTFILE=0;
double t16,t15, t0,t1;



/**
This function permits to determine the size in octet of the bistream. 

@param f The file where data are read. 
*/
int Filesize ( FILE *f )
{
	long    oldfp, fsize ;

	oldfp = ftell(f);
	if ( oldfp < 0L ) 
		return -1 ;
	if ( 0 != fseek(f, 0, SEEK_END) ) 
		return -1 ;
	fsize = ftell(f);
	if ( fsize < 0 ) 
		return -1 ;
	if ( 0 != fseek(f, oldfp, SEEK_SET) ) 
		return -1 ;
	return fsize ;
}


#ifdef TI_OPTIM 

/**
This function permits to open the file which is gone to be decoded. 

@param argc Number of argument passed by command.
@param argv Stream of each argument.
@param ofile Name of output file
@param frame_num number of frames to decode
*/
int readh264_init_TI ( int argc, char *argv [], FILE* ofile, unsigned int* frame_num) //RAL
{

	int i;
	for ( i = 1 ; i < argc ; i++ ) 
	{
		if (strcmp("-h264", argv[i]) == 0 && i < argc - 1 ) 
		{
			i++;
			ARG_INPUTFILE = argv[i];
		}
		else if (strcmp("-yuv", argv[i]) == 0 && i < argc - 1 )	//RAL: open output file
		{ 														//RAL
			ofile= fopen(argv[i], "wb"); 						//RAL
			fclose(ofile); 										//RAL
		} 														//RAL
		else if (strcmp("-fn", argv[i]) == 0 && i < argc - 1 )	//RAL: specify Nº of frames to decode
		{ 														//RAL
			*frame_num= atoi(argv[++i]);						//RAL
		} 														//RAL
	}
	if ( ARG_INPUTFILE == NULL || (ptfile = fopen(ARG_INPUTFILE, "rb")) == NULL) 
	{
		printf("Input file doesn't exit\n");
		exit( 10);
	}

	file_size = Filesize(ptfile);
	fseek(ptfile, nb_octet_already_read, 0);
	return file_size ;
}

#else 


/**
This function permits to open the file which is gone to be decoded. 

@param argc Number of argument passed by command.
@param argv Stream of each argument.
*/
int readh264_init ( int argc, char *argv [] )
{
	int i;

	for ( i = 1 ; i < argc ; i++ ) 
	{
		if (strcmp("-h264", argv[i]) == 0 && i < argc - 1 ) 
		{
			i++;
			ARG_INPUTFILE = argv[i];
		}
	}
	if ( ARG_INPUTFILE == NULL || (ptfile = fopen(ARG_INPUTFILE, "rb")) == NULL) 
	{
		DisplayDecoderOptions();
		exit( 10);
	}

	file_size = Filesize(ptfile);
	fseek(ptfile, nb_octet_already_read, 0);
	return file_size ;
}
#endif


/**
This function permits to r;ead from the bitstream a buffer. 

@param nb_octets_to_read_temp The size in octets to read from the bistream.
@param NalRbspBytes Size of the last Nal.
@param nb_octets_already_read The size in octets already read from the bistream.
@param buffer The buffer where will be stocked the data read from the bistream. 
*/
void readh264 ( const int nb_octets_to_read_temp, int NalRbspBytes, int *nb_octets_already_read, unsigned char *buffer)
{
	unsigned int nb_octects = 0 ;

	if (*nb_octets_already_read - nb_octets_to_read_temp + NalRbspBytes >= file_size){
#ifndef READ_NO_END
		nb_repeat ++;
		if( nb_repeat == 1)	{//Create an end of sequence Nal unit to flush the video buffer
			buffer[0] = 0;
			buffer[1] = 0;
			buffer[2] = 0;
			buffer[3] = 1;
			buffer[4] = 11;
			buffer[5] = 0;
			buffer[6] = 0;
			buffer[7] = 0;
			buffer[8] = 1;
		}
		if(nb_repeat == 17){
			exit(11);
		}
	}else {
#else
		*nb_octets_already_read = 0;
		NalRbspBytes = nb_octets_to_read_temp;
	}
	{
#endif
		memcpy(buffer, &buffer[NalRbspBytes], (nb_octets_to_read_temp - NalRbspBytes ) * sizeof(unsigned char));
		if ( *nb_octets_already_read + NalRbspBytes <= file_size ) {
			fseek(ptfile, *nb_octets_already_read, 0);
			fread(&buffer[nb_octets_to_read_temp - NalRbspBytes], sizeof(char), NalRbspBytes, ptfile);
		} else if (file_size > *nb_octets_already_read - 4){
			fseek(ptfile, *nb_octets_already_read, 0);
			nb_octects =  file_size - *nb_octets_already_read ;
			fread(&buffer[nb_octets_to_read_temp - NalRbspBytes], sizeof(char), nb_octects, ptfile);
			buffer [nb_octets_to_read_temp - NalRbspBytes + nb_octects ] = 0 ;
			buffer [nb_octets_to_read_temp - NalRbspBytes + nb_octects + 1] = 0 ;
			buffer [nb_octets_to_read_temp - NalRbspBytes + nb_octects + 2] = 0 ;
			buffer [nb_octets_to_read_temp - NalRbspBytes + nb_octects + 3] = 1 ;
		}
	}

	RemainBytes = file_size - (*nb_octets_already_read - nb_octets_to_read_temp + NalRbspBytes);
	*nb_octets_already_read += NalRbspBytes;
}


/**
Read data only if necessary
*/
void ReadAuH264 (int NbBytesToRead, unsigned char *buffer, int ReadNewBytes, int *NbBytesTreated)
{
	if(ReadNewBytes){
		//If we need to read data
		unsigned int BytesToRead = ReadNewBytes;
		unsigned char AddEndSequence = 0;

		if(*NbBytesTreated + ReadNewBytes >= file_size){
			//check if the number of bytes decoded is superior to the file size
			if(*NbBytesTreated < file_size){
				BytesToRead = 1; //just to read last bytes
				AddEndSequence = 1;
			}else {
				BytesToRead = 0;
			}
		}else if(NbBytesToRead > file_size){
			//In case of the file size is small
			BytesToRead = file_size - *NbBytesTreated;
			AddEndSequence = 1;
		}
		memcpy(buffer, &buffer[ReadNewBytes], (NbBytesToRead - ReadNewBytes) * sizeof(unsigned char));
		if(BytesToRead){
			BytesToRead = CLIP3(0, ReadNewBytes, file_size - *NbBytesTreated);
			fseek(ptfile, *NbBytesTreated, 0);
			fread(&buffer[NbBytesToRead - ReadNewBytes], sizeof(char), BytesToRead, ptfile);
			*NbBytesTreated += BytesToRead;
		}	

		if(AddEndSequence){
			//Add start code and end of stream nal unit
			buffer[NbBytesToRead - ReadNewBytes + BytesToRead + 0] = 0;
			buffer[NbBytesToRead - ReadNewBytes + BytesToRead + 1] = 0;
			buffer[NbBytesToRead - ReadNewBytes + BytesToRead + 2] = 0;
			buffer[NbBytesToRead - ReadNewBytes + BytesToRead + 3] = 1;
#ifndef READ_NO_END
			buffer[NbBytesToRead - ReadNewBytes + BytesToRead + 4] = 11;
			buffer[NbBytesToRead - ReadNewBytes + BytesToRead + 5] = 0;
			buffer[NbBytesToRead - ReadNewBytes + BytesToRead + 6] = 0;
			buffer[NbBytesToRead - ReadNewBytes + BytesToRead + 7] = 0;
			buffer[NbBytesToRead - ReadNewBytes + BytesToRead + 8] = 1;
#else
			*NbBytesTreated = 0;
#endif
		}
	}
}
#endif
