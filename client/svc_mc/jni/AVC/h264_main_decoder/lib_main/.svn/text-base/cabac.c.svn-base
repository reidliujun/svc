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



#include "cabac.h"






/**
This function permits to initialize the CABAC data according the CABAC table defined in the norm. 

@param c Contains the data concernig the CABAC for decoding the VLC.
@param lps_range Least probable symbol table
@param mps_state Most probable symbol table
@param lps_state Least probable symbol state
@param state_count
*/
void init_cabac_states(CABACContext *c, unsigned char (*lps_range)[4],unsigned char *mps_state, unsigned char *lps_state, int state_count){
	int i, j;

	for(i = 0; i < state_count; i++){
		for(j = 0; j < 4; j++){ //FIXME check if this is worth the 1 shift we save
#ifdef TI_OPTIM
			c -> lps_range[i][j] = lps_range[i][j];
#else
			c -> lps_range[(i << 1) + 0][j + 4] = c -> lps_range[(i << 1) + 1][j + 4] = lps_range[i][j];//ESB
#endif
		}
		c -> mps_state[(i << 1)+ 0] = mps_state[i] << 1;
		c -> mps_state[(i << 1)+ 1] = (mps_state[i] << 1) + 1;

		if( i ){
			c -> lps_state[(i << 1) + 0] = lps_state[i] << 1;
			c -> lps_state[(i << 1) + 1] = (lps_state[i] << 1) + 1;
		}else{
			c -> lps_state[(i << 1) + 0] = 1;
			c -> lps_state[(i << 1) + 1] = 0;
		}
	}
}


/**
This function permits to initialize the structure C in order to start decodingthe vlc.. 

@param c Contains the data concernig the CABAC for decoding the VLC.
@param buf Corresponds to the bitstream read in the file.
@param buf_size Size of the current bitstreeam.
*/
void init_cabac_decoder(CABACContext *c, unsigned char *buf, int buf_size){
	c -> bytestream_start = c -> bytestream = buf;
	c -> bytestream_end = buf + buf_size;

	c -> low =  (*c -> bytestream++) << 10;
	c -> low += ((*c -> bytestream++) << 2) + 2;
	c -> range = 0x1FE << (CABAC_BITS + 1);
}


