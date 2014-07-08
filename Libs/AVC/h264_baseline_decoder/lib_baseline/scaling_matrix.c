/* FFmpeg project
 * libavcodec (h264.c)
 * H.26L/H.264/AVC/JVT/14496-10/... encoder/decoder
 * Copyright (c) 2003 Michael Niedermayer <michaelni@gmx.at>
 *
 * This file has been modified for opensvcdecoder.
 

*****************************************************************************
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

#include <string.h>
#include "type.h"
#include "data.h"
#include "bitstream.h"


// Tables declaration
static const short default_scaling4[2][16] = {
{   6,13,20,28,
   13,20,28,32,
   20,28,32,37,
   28,32,37,42
},{
   10,14,20,24,
   14,20,24,27,
   20,24,27,30,
   24,27,30,34
}};

static const short default_scaling8[2][64]={
{   6,10,13,16,18,23,25,27,
   10,11,16,18,23,25,27,29,
   13,16,18,23,25,27,29,31,
   16,18,23,25,27,29,31,33,
   18,23,25,27,29,31,33,36,
   23,25,27,29,31,33,36,38,
   25,27,29,31,33,36,38,40,
   27,29,31,33,36,38,40,42
},{
    9,13,15,17,19,21,22,24,
   13,13,17,19,21,22,24,25,
   15,17,19,21,22,24,25,27,
   17,19,21,22,24,25,27,28,
   19,21,22,24,25,27,28,30,
   21,22,24,25,27,28,30,32,
   22,24,25,27,28,30,32,33,
   24,25,27,28,30,32,33,35
}};




/**


This function permits to get back from the bitstream the scaling matrix.

@param data The NAL unit.
@param aio_piPosition The current aio_piPosition in the NAL.
@param factors Table to initialize.
@param size Size of the table.
@param jvt_list Default scaling table.
@param fallback_list 
*/

void decode_scaling_list(unsigned char *data, int *aio_piPosition,short *factors, int size, const short *jvt_list , const short *fallback_list){

	int index;
	unsigned int uiScanTableH = 0xf9be4190;
	unsigned int uiScanTableL = 0xef91b904;
	short i, last = 8, next = 8;

	if(getNbits(data, aio_piPosition, 1)){ // matrix not written, we use the preset one 
		if (size == 16){

			for(i=0;i<size;i++){
				index = ( (((uiScanTableH >> (i<<1))<<2) + ((uiScanTableL >> (i<<1)) & 3)) & 0xf);
				if(next)
					next = (last + read_se(data, aio_piPosition)) & 0xff;
				if(!i && !next){ // matrix not written, we use the preset one 
					memcpy(factors, jvt_list, size*sizeof(short));
					break;
				}
				last = factors[index] = next ? next : last;
			}
		}else{
			for(i=0;i<size;i++){
				index = zigzag_scan8x8[i];
				if(next)
					next = (last + read_se(data, aio_piPosition) ) & 0xff;
				if(!i && !next){ // matrix not written, we use the preset one 
					memcpy(factors, jvt_list, size*sizeof(short));
					break;
				}
				last = factors[index] = next ? next : last;
			}
		}
	}else
		memcpy(factors, fallback_list, size*sizeof(short));
}




/**
This function permits to get back from the bitstream the scaling matrix.

@param data The NAL unit.
@param aio_piPosition The current aio_piPosition in the NAL.
@param sps SPS structure of the current video.
@param is_sps TO know if the calling funciton is sequence parameter set.
@param scaling_matrix4 Default scaling matrix.
@param scaling_matrix8 Default scaling matrix.
*/
void decode_scaling_matrices(unsigned char *data, int *aio_piPosition,  SPS *sps,int is_sps, short (*scaling_matrix4)[16], short (*scaling_matrix8)[64]){    
	 short *fallback[4];

	 if (!is_sps && sps -> scaling_matrix_present){
		 fallback[0] = sps -> scaling_matrix4[0];
		 fallback[1] = sps -> scaling_matrix4[3];
		 fallback[2] = sps -> scaling_matrix8[0];
		 fallback[3] = sps -> scaling_matrix8[1];
	 }else {
		 fallback[0] = (short *)default_scaling4[0];
		 fallback[1] = (short *)default_scaling4[1];
		 fallback[2] = (short *)default_scaling8[0];
		 fallback[3] = (short *)default_scaling8[1];
	 }

    if(getNbits(data, aio_piPosition, 1)){
        sps -> scaling_matrix_present |= is_sps;
        decode_scaling_list(data, aio_piPosition, scaling_matrix4[0],16,default_scaling4[0],fallback[0]); // Intra, Y
        decode_scaling_list(data, aio_piPosition,scaling_matrix4[1],16,default_scaling4[0],scaling_matrix4[0]); // Intra, Cr
		decode_scaling_list(data, aio_piPosition,scaling_matrix4[2],16,default_scaling4[0],scaling_matrix4[1]); // Intra, Cb
		decode_scaling_list(data, aio_piPosition,scaling_matrix4[3],16,default_scaling4[1],fallback[1]); // Inter, Y
		decode_scaling_list(data, aio_piPosition,scaling_matrix4[4],16,default_scaling4[1],scaling_matrix4[3]); // Inter, Cr
		decode_scaling_list(data, aio_piPosition,scaling_matrix4[5],16,default_scaling4[1],scaling_matrix4[4]); // Inter, Cb
		decode_scaling_list(data, aio_piPosition,scaling_matrix8[0],64,default_scaling8[0],fallback[2]);  // Intra, Y
		decode_scaling_list(data, aio_piPosition,scaling_matrix8[1],64,default_scaling8[1],fallback[3]);  // Inter, Y

	} else if(!is_sps && sps -> scaling_matrix_present) {
		memcpy(scaling_matrix4, sps -> scaling_matrix4, 6*16*sizeof(short));
		memcpy(scaling_matrix8, sps -> scaling_matrix8, 2*64*sizeof(short));
    }
}
