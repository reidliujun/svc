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



#ifndef SLICE_DATA_H
#define SLICE_DATA_H

/**
Return true when problems occurs during the decoding process.
*/
static __inline int EndOfSliceManagement(SLICE *Slice, int PicSizeInMbs, char ErrorDetection)
{
	// why End of slice is set to one when ErrorDetection is equal to 2
	//-> in case of slice and error
	if ( Slice -> mb_xy < PicSizeInMbs || ErrorDetection == 2){
		return 1 ;
	}
	return 0;
}




//prototypes.

int MoreRbsp( const unsigned char *ai_pcData, const int *aio_piPosition, const int *ai_piNalBytesInNalunit, 
			 int iCurrMbAddr, int PicSizeInMbs, char *ErrorDetected);
int more_rbsp_data ( const unsigned char *data, const int *aio_piPosition, const int *NalBytesInNalunit);

char slice_data_I_cavlc(RESIDU *picture_residu, const unsigned char *ai_pcData, const int *ai_piNalBytesInNalunit, 
						const SPS *Sps, const PPS *Pps, DATA *aio_tstBlock, SLICE *Slice, 
						int *aio_piPosition, const VLC_TABLES *Vlc, unsigned char *SliceTable,  
						const unsigned char *aio_pstSliceGroupMap, int *ao_piEndOfSlice);

char slice_data_P_cavlc(RESIDU *picture_residu, const unsigned char *ai_pcData,  const int *ai_piNalBytesInNalunit,
						const SPS *Sps, const PPS *Pps,DATA *aio_tstBlock, SLICE *Slice, 
						int *aio_piPosition, const VLC_TABLES *Vlc, unsigned char *SliceTable,
						const unsigned char *aio_pstSliceGroupMap, int *ao_piEndOfSlice, short *mvl0_io, short *refl0_io);




#endif
