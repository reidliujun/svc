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

#ifndef SLICE_HEADER_EXT_H
#define SLICE_HEADER_EXT_H


//AVC Files
#include "type.h"

/**
Search into reference picture the one with the correct poc number.
*/
static __inline int search_picture_address(int poc, const LAYER_MMO *LayerMmo, unsigned char QualityId)
{
    int i;

	for(i = 0; i < LayerMmo -> ShortRefCount; i++)  {
		if (LayerMmo -> ShortRef[i] . poc == poc){
            return (LayerMmo -> ShortRef[i] . MemoryAddress);
		}
    }
	 for(i = 0; i < 16; i++)  {
		 if (LayerMmo -> LongRef [i] . poc == poc){
            return (LayerMmo -> LongRef [i] . MemoryAddress);
		}
    }
	 if(QualityId){
		 return search_picture_address(poc, &LayerMmo[-1], QualityId - 1);
	 }
    return -1;
}






static __inline int search_mv_address(int poc, const LAYER_MMO *LayerMmo)
{
    int i;

	for(i = 0; i < LayerMmo -> ShortRefCount; i++)  {
		if (LayerMmo -> ShortRef[i] . poc == poc){
            return (LayerMmo -> ShortRef[i] . MvMemoryAddress);
		}
    }
	 for(i = 0; i < 16; i++)  {
		if (LayerMmo -> LongRef[i] . poc == poc){
            return (LayerMmo -> LongRef[i] . MvMemoryAddress);
		}
    }
    return (0);
}



int SliceHeaderCut(unsigned char *data, SPS *sps, PPS *pps, NAL *Nal, int *position);
int SpatialScalabilityType(SPS *sps, NAL *Nal );
void UpdateReferenceListAddress(LAYER_MMO *LayerMmo, LIST_MMO *RefListL0, LIST_MMO *RefListL1, unsigned char QualityId);
void NalParamInit(NAL *Nal, PPS *pps, SPS *sps, SPS *pt_sps_id);
void SetSpatialUsed(NAL *Nal);
void SvcReadDefaultParameters(unsigned char *data, int *position, NAL *Nal, SLICE *Slice, SPS *pt_sps_id);
void SvcReadLayerPrediction(unsigned char *data, int *position, NAL *Nal, SPS *pt_sps_id, PPS *pt_pps_id, SLICE *Slice, 
							LIST_MMO *Current_pic, MMO *Mmo, int EndOfSlice);
void SvcReadReferenceListProcessing(unsigned char *data, int *position, NAL *Nal, SLICE *Slice, SPS *pt_sps_id, 
								PPS *pt_pps_id, LAYER_MMO *LayerMmo, LIST_MMO *Current_pic, MMO *Mmo, 
								LIST_MMO *RefPicListL0, LIST_MMO *RefPicListL1);
#endif
