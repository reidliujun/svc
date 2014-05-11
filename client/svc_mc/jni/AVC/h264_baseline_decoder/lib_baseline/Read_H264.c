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


int			RemainBytes = 0;

#ifndef ARM

#define READ_NO_END_


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <jni.h>
#include <android/log.h>
#include <pthread.h>

//AVC Files
#include "help.h"
#include "clip.h"
#include "Read_h264.h"

#define  LOG_TAG    "libsvc"
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)


static FILE *ptfile ;
/*
static char* StreamBuffer = NULL;
static int StreamBufferPos = 0;

static int  file_size ;
*/


char* StreamBuffer;
int StreamBufferPos;
int currentBufferPos;
int file_size = 0;


static int	nb_repeat = 0;
int         nb_octet_already_read = 0;
int         Filesize (FILE *f) ;
static char *ARG_INPUTFILE=0;
double t16,t15, t0,t1;

extern pthread_mutex_t BufferReallocMutex;
extern pthread_mutex_t DataSizeMutex;


char** getStreamBufferPtr(){return &StreamBuffer;}
int* getStreamBufferPosPtr(){return &StreamBufferPos;}
int* getFileSizePtr(){return &file_size;}

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
		else if (strcmp("-fn", argv[i]) == 0 && i < argc - 1 )	//RAL: specify N� of frames to decode
		{ 														//RAL
			*frame_num= atoi(argv[++i]);						//RAL
		} 														//RAL
	}
	if ( ARG_INPUTFILE == NULL || (ptfile = fopen(ARG_INPUTFILE, "rb")) == NULL) 
	{
		printf("Input file doesn't exit\n");
		exit( 10);
	}

	//file_size = Filesize(ptfile);
	//fseek(ptfile, nb_octet_already_read, 0);
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
	currentBufferPos = 0;
	int i;

	for ( i = 1 ; i < argc ; i++ ) 
	{
		if (strcmp("-h264", argv[i]) == 0 && i < argc - 1 ) 
		{
			i++;
			//ARG_INPUTFILE = argv[i];
		}
	}

	//if ( ARG_INPUTFILE == NULL || (ptfile = fopen(ARG_INPUTFILE, "rb")) == NULL)
	//{
	//	DisplayDecoderOptions();
	//	exit(10);
	//}

	//file_size = Filesize(ptfile);
	//fseek(ptfile, nb_octet_already_read, 0);
	return file_size ;
}
#endif


/**
This function permits to read from the bitstream a buffer.

@param nb_octets_to_read_temp :		The size in octets to read from the bistream.
@param NalRbspBytes :				Size of the last Nal.
@param nb_octets_already_read : 	The size in octets already read from the bistream.
@param buffer :						The buffer where will be stocked the data read from the bistream.
*/
void readh264 ( const int nb_octets_to_read_temp, int NalRbspBytes, int *nb_octets_already_read, unsigned char *buffer)
{
	unsigned int nb_octects = 0 ;
//	LOGE("READ264 : BEFORE nb_octets_to_read_temp = %d, NalRbspBytes = %d, *nb_octets_already_read = %d", nb_octets_to_read_temp, NalRbspBytes, *nb_octets_already_read);

	if (*nb_octets_already_read - nb_octets_to_read_temp + NalRbspBytes >= file_size)
	{ // file_size is not enough
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
	}

	else
	{
#else
		*nb_octets_already_read = 0;
		NalRbspBytes = nb_octets_to_read_temp;
	}
	{
#endif
        //---- shift residue data ----//
		//---- first time, (nb_octets_to_read_temp - NalRbspBytes) = 0 ----//
//		LOGE("test_buffer : memcpy(buffer, buffer[%d], %d) in readh264", NalRbspBytes, nb_octets_to_read_temp - NalRbspBytes);
		memcpy(buffer, &buffer[NalRbspBytes], (nb_octets_to_read_temp - NalRbspBytes ) * sizeof(unsigned char));

		if ( *nb_octets_already_read + NalRbspBytes <= file_size ) {
			//fseek(ptfile, *nb_octets_already_read, 0);
			//fread(&buffer[nb_octets_to_read_temp - NalRbspBytes], sizeof(char), NalRbspBytes, ptfile);
			
//			pthread_mutex_lock(&BufferReallocMutex);
//			{
//				LOGE("test_buffer : memcpy1(buffer[%d], StreamBuffer[%d], %d) in readh264", nb_octets_to_read_temp - NalRbspBytes, *nb_octets_already_read, NalRbspBytes);
				memcpy(&buffer[nb_octets_to_read_temp - NalRbspBytes], &StreamBuffer[*nb_octets_already_read], NalRbspBytes * sizeof(char));
//			}
//			pthread_mutex_unlock(&BufferReallocMutex);
			
		} else if (file_size > *nb_octets_already_read - 4){
			//fseek(ptfile, *nb_octets_already_read, 0);
			nb_octects =  file_size - *nb_octets_already_read ;
			//fread(&buffer[nb_octets_to_read_temp - NalRbspBytes], sizeof(char), nb_octects, ptfile);
			
//			pthread_mutex_lock(&BufferReallocMutex);
//			{
//				LOGE("test_buffer : memcpy2(buffer[%d], StreamBuffer[%d], %d) in readh264", nb_octets_to_read_temp - NalRbspBytes, *nb_octets_already_read, nb_octects);
				memcpy(&buffer[nb_octets_to_read_temp - NalRbspBytes], &StreamBuffer[*nb_octets_already_read], nb_octects * sizeof(char));
//			}
//			pthread_mutex_unlock(&BufferReallocMutex);
			
			buffer [nb_octets_to_read_temp - NalRbspBytes + nb_octects ] = 0 ;
			buffer [nb_octets_to_read_temp - NalRbspBytes + nb_octects + 1] = 0 ;
			buffer [nb_octets_to_read_temp - NalRbspBytes + nb_octects + 2] = 0 ;
			buffer [nb_octets_to_read_temp - NalRbspBytes + nb_octects + 3] = 1 ;
		}
	}

	RemainBytes = file_size - (*nb_octets_already_read - nb_octets_to_read_temp + NalRbspBytes);;
	*nb_octets_already_read += NalRbspBytes;

	if (*nb_octets_already_read > currentBufferPos)
		currentBufferPos = *nb_octets_already_read;
}


/*
 * Read data only if necessary
 *
 * @param NbBytesToRead:	copy the amount of bytes from StreamBuffer to buffer in each run.
 * @param *buffer:			the buffer store the data will be decoded.
 * @param ReadNewBytes:		get the amount of bytes in this run.
 * @param *NbBytesTreated:	how many bytes already be got by decoder.
 */
void ReadAuH264 (int NbBytesToRead, unsigned char *buffer, int ReadNewBytes, int *NbBytesTreated)
{
	extern int data_size;
//	LOGE("READAu264 : BEFORE NbBytesToRead = %d, ReadNewBytes = %d, *NbBytesTreated = %d", NbBytesToRead, ReadNewBytes, *NbBytesTreated);

	//---- If we need to read data ----//
	if(ReadNewBytes)
	{
		unsigned int BytesToRead = ReadNewBytes;
		unsigned char AddEndSequence = 0;

//		if(*NbBytesTreated + ReadNewBytes >= file_size)
//		{ // check if the number of bytes decoded is superior to the file size
//
//			if(*NbBytesTreated < file_size) {
//				BytesToRead = 1; // just to read last bytes
//				AddEndSequence = 1;
//			}
//			else {
//				BytesToRead = 0;
//			}
//		}

		//---- In case of the file size is small ----//
//		else if(NbBytesToRead > file_size)l
//		{
//			BytesToRead = file_size - *NbBytesTreated;
//			AddEndSequence = 1;
//		}

//		LOGE("test_buffer : memcpy(buffer[0], buffer[%d], %d) in ReadAuH264", ReadNewBytes, NbBytesToRead - ReadNewBytes);
		memcpy(buffer, &buffer[ReadNewBytes], (NbBytesToRead - ReadNewBytes) * sizeof(unsigned char));
		if(BytesToRead)
		{
//			BytesToRead = CLIP3(0, ReadNewBytes, file_size - *NbBytesTreated);	// CLIP3(X, Y, Z) = MIN(Y, MAX(X, Z))
			// fseek(ptfile, *NbBytesTreated, 0);
			// fread(&buffer[NbBytesToRead - ReadNewBytes], sizeof(char), BytesToRead, ptfile);




			if(*NbBytesTreated + ReadNewBytes > file_size)
			{
				int first_part = file_size - *NbBytesTreated;

//				LOGE("SYNC : before BRM in ReadAuH264() out of size");
//				pthread_mutex_lock(&BufferReallocMutex);
//				{
//					LOGE("SYNC : IN CR at BRM in ReadAuH264()");

//					LOGE("test_buffer : memcpy1(buffer[%d], StreamBuffer[%d], %d) in ReadAuH264", NbBytesToRead - ReadNewBytes, *NbBytesTreated, first_part);
					memcpy(&buffer[NbBytesToRead - ReadNewBytes], &StreamBuffer[*NbBytesTreated], first_part * sizeof(char));

//					LOGE("test_buffer : memcpy2(buffer[%d], StreamBuffer[0], %d) in ReadAuH264", NbBytesToRead - ReadNewBytes + first_part, *NbBytesTreated, ReadNewBytes - first_part);
					memcpy(&buffer[NbBytesToRead - ReadNewBytes + first_part], StreamBuffer, (ReadNewBytes - first_part) * sizeof(char));

					currentBufferPos = *NbBytesTreated = (*NbBytesTreated + ReadNewBytes) % file_size;
//				}
//				pthread_mutex_unlock(&BufferReallocMutex);
//				LOGE("SYNC : EXIT BRM in ReadAuH264() with datasize = %d", data_size);
			}
			else
			{
//				LOGE("SYNC : before BRM in ReadAuH264() size is enough");
//				pthread_mutex_lock(&BufferReallocMutex);
//				{
//					LOGE("SYNC : IN CR at BRM in ReadAuH264()");

//					LOGE("test_buffer : memcpy(buffer[%d], StreamBuffer[%d], %d) in ReadAuH264", NbBytesToRead - ReadNewBytes, *NbBytesTreated, BytesToRead);
					memcpy(&buffer[NbBytesToRead - ReadNewBytes], &StreamBuffer[*NbBytesTreated], BytesToRead * sizeof(char));

					currentBufferPos = *NbBytesTreated = (*NbBytesTreated + ReadNewBytes) % file_size;
//				}
//				pthread_mutex_unlock(&BufferReallocMutex);
//				LOGE("SYNC : EXIT BRM in ReadAuH264() with datasize = %d", data_size);
			}


//			LOGE("SYNC : before DataSizeMutex in ReadAuH264()");

//				LOGE("SYNC : IN CR at DataSizeMutex in ReadAuH264()");
				if (StreamBufferPos >= currentBufferPos)
				{
					pthread_mutex_lock(&DataSizeMutex);
					{
						data_size = StreamBufferPos - currentBufferPos;
					}
					pthread_mutex_unlock(&DataSizeMutex);
//					LOGE("test_buffer : data size = %d with SBP >= CBP", data_size);
				}
				else
				{
					pthread_mutex_lock(&DataSizeMutex);
					{
						data_size = (file_size - 1) - (currentBufferPos - StreamBufferPos);
					}
					pthread_mutex_unlock(&DataSizeMutex);
//					LOGE("test_buffer : data size = %d with SBP < CBP", data_size);
				}
//			LOGE("SYNC : EXIT DataSizeMutex in ReadAuH264()");


//			pthread_mutex_lock(&BufferReallocMutex);
//			{
//				LOGE("READAu264 : memcpy(buffer[%d], buffer[%d], %d)", NbBytesToRead - ReadNewBytes, *NbBytesTreated, BytesToRead);
//				memcpy(&buffer[NbBytesToRead - ReadNewBytes], &StreamBuffer[*NbBytesTreated], BytesToRead * sizeof(char));
				// *NbBytesTreated += BytesToRead;
//				*NbBytesTreated = (*NbBytesTreated + ReadNewBytes) % file_size;
//			}
//			pthread_mutex_unlock(&BufferReallocMutex);

		}	

		if(AddEndSequence)
		{
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
			currentBufferPos = 0;
#endif
		}
	}
//	LOGE("READAu264 : AFTER StreamBufferPos = %d, currentBufferPos = %d", StreamBufferPos, currentBufferPos);
}
#endif
