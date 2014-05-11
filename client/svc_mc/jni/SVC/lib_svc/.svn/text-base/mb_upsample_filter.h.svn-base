/*****************************************************************************
   *
   *  Open SVC Decoder developped in IETR image lab
   *
   *
   *
   *              Médéric Blestel <mblestel@insa-rennes.Fr>
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


#ifndef MB_UP_FILTER_H
#define MB_UP_FILTER_H

#ifdef INTEL
#pragma warning(disable:177)
#endif


void upsample_mb_luninance(const SPS *sps, const NAL *Nal, unsigned char *Y	, unsigned char *aio_tucImage, 
						   const short x_base, const short y_base, const short x, const short y);
void upsample_mb_chroma(const SPS *sps,const NAL *Nal, unsigned char *Y, unsigned char *aio_tucImage, 
						const short x_base,const  short y_base, const short x,const short y);


static __inline short ShiftCalculation(int level_idc, short PicWidthInPix){
	

	if (level_idc > 30){
		int t = PicWidthInPix - 1;
		short shiftX = 0;
		while(t > 0) {
			shiftX++;
			t >>= 1;
		}        
		return 31 - shiftX;

	}else {
		return 16;
	}

}
#endif
