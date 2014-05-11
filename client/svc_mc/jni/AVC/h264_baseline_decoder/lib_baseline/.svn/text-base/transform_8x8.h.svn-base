/* FFmpeg project
 * libavcodec (h264.c)
 * H.26L/H.264/AVC/JVT/14496-10/... encoder/decoder
 * Copyright (c) 2003 Michael Niedermayer <michaelni@gmx.at>
 *
 * This file has been modified for opensvcdecoder.
 */
 
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

#ifndef TRANSFORM_8x8_H
#define TRANSFORM_8x8_H

#include "type.h"
#include "clip.h"




static __inline void ict_8x8(short *block, unsigned char *image, const int PicWidthInPix){
   
	int i;
	__declspec(align(64)) short BlockTemp[64];
	short *ptr = BlockTemp;
	block[0] += 32;

    for( i = 0; i < 8; i++ )
    {
        const short a0 =  block[0] + block[4];
        const short a2 =  block[0] - block[4];
        const short a4 = (block[2] >> 1) - block[6];
        const short a6 = (block[6] >> 1) + block[2];

        const short b0 = a0 + a6;
        const short b2 = a2 + a4;
        const short b4 = a2 - a4;
        const short b6 = a0 - a6;

        const short a1 = -block[3] + block[5] - block[7] - (block[7] >> 1);
        const short a3 =  block[1] + block[7] - block[3] - (block[3] >> 1);
        const short a5 = -block[1] + block[7] + block[5] + (block[5] >> 1);
        const short a7 =  block[3] + block[5] + block[1] + (block[1] >> 1);

        const short b1 = (a7 >> 2) + a1;
        const short b3 =  a3 + (a5 >> 2);
        const short b5 = (a3 >> 2) - a5;
        const short b7 =  a7 - (a1 >> 2);

        ptr[0] = b0 + b7;
        ptr[7] = b0 - b7;
        ptr[1] = b2 + b5;
        ptr[6] = b2 - b5;
        ptr[2] = b4 + b3;
        ptr[5] = b4 - b3;
        ptr[3] = b6 + b1;
        ptr[4] = b6 - b1;
		block += 8;
		ptr += 8;
    }


    for( i = 0; i < 8; i++, image++)
    {
        const short a0 =  BlockTemp[0 + i] + BlockTemp[4 * 8 + i];
        const short a2 =  BlockTemp[0 + i] - BlockTemp[4 * 8 + i];
        const short a4 = (BlockTemp[2 * 8 + i] >> 1) - BlockTemp[6 * 8 + i];
        const short a6 = (BlockTemp[6 * 8 + i] >> 1) + BlockTemp[2 * 8 + i];

        const short b0 = a0 + a6;
        const short b2 = a2 + a4;
        const short b4 = a2 - a4;
        const short b6 = a0 - a6;

        const short a1 = -BlockTemp[3*8 + i] + BlockTemp[5*8 + i] - BlockTemp[7*8 + i] - (BlockTemp[7*8 + i] >> 1);
        const short a3 =  BlockTemp[1*8 + i] + BlockTemp[7*8 + i] - BlockTemp[3*8 + i] - (BlockTemp[3*8 + i] >> 1);
        const short a5 = -BlockTemp[1*8 + i] + BlockTemp[7*8 + i] + BlockTemp[5*8 + i] + (BlockTemp[5*8 + i] >> 1);
        const short a7 =  BlockTemp[3*8 + i] + BlockTemp[5*8 + i] + BlockTemp[1*8 + i] + (BlockTemp[1*8 + i] >> 1);

        const short b1 = (a7 >> 2) + a1;
        const short b3 =  a3 + (a5 >> 2);
        const short b5 = (a3 >> 2) - a5;
        const short b7 =  a7 - (a1 >> 2);
 
        image [0]					 = (unsigned char ) CLIP255( image[0]					 + ((b0 + b7) >> 6) );
        image [PicWidthInPix]		 = (unsigned char ) CLIP255( image[PicWidthInPix]		 + ((b2 + b5) >> 6) );
        image [(PicWidthInPix << 1)] = (unsigned char ) CLIP255( image [(PicWidthInPix << 1)] + ((b4 + b3) >> 6) );
        image [3 * PicWidthInPix]	 = (unsigned char ) CLIP255( image [3 * PicWidthInPix]	 + ((b6 + b1) >> 6) );
        image [(PicWidthInPix << 2)] = (unsigned char ) CLIP255( image [(PicWidthInPix << 2)] + ((b6 - b1) >> 6) );
        image [5 * PicWidthInPix]	 = (unsigned char ) CLIP255( image [5 * PicWidthInPix]	 + ((b4 - b3) >> 6) );
        image [6 * PicWidthInPix]	 = (unsigned char ) CLIP255( image [6 * PicWidthInPix]	 + ((b2 - b5) >> 6) );
        image [7 * PicWidthInPix]	 = (unsigned char ) CLIP255( image [7 * PicWidthInPix]	 + ((b0 - b7) >> 6) );
    }
}
#endif
