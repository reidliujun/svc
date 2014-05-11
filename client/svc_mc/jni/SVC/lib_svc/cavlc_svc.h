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


#ifndef CAVLC_SVC_H
#define CAVLC_SVC_H


#include "svc_type.h"
#include "ErrorsDetection.h"
#include "fill_caches_svc.h"


int sub_mb_pred_svc ( const unsigned char *ai_pcData,  int *aio_piPosition, const SLICE *ai_pstSlice, 
					  DATA *  aio_pstMacroblock, RESIDU *Current_residu);
int mb_pred_svc( const unsigned char *ai_pcData, int *aio_piPosition, const SLICE *ai_pstSlice, 
				DATA *aio_pstMacroblock, RESIDU *Current_residu , short * intra4x4_pred_mode_cache);

char macroblock_layer_in_scalable_extension(const NAL *Nal, const PPS *Pps, RESIDU *Current_residu, RESIDU *BaseResidu, 
										   const unsigned char *ai_pcData,   int *position,  const SLICE *ai_pstSlice, 
										   DATA *Tab_Block, const VLC_TABLES *vlc, unsigned char NonZeroCountCache [ ], 
										   unsigned char *SliceTable,  const short ai_iMb_x , const short ai_iMb_y, 
										   int direct_8x8_inference_flag, unsigned char *last_QP);

		
extern const char    golomb_to_intra4x4 [48];
extern const char    golomb_to_inter [48];




/**
This function permits to decode the coded block pattern. 
me(v): mapped integer Exp-Golomb-coded syntax element with the left bit first.

@param data The NAL unit.
@param aio_piPosition The current aio_piPosition in the NAL.
@param mode The prediction mode of the macroblock.
*/
static __inline char read_me_svc ( const unsigned char *tab, int *pos, int mode, RESIDU *CurrResidu, const int mb_xy, const SLICE *slice, const int x)
{
    int val = read_ue(tab, pos);

#ifdef ERROR_DETECTION
	if( val > 47){
		return 1;
	}
#endif

	if (slice -> notDefaultScanIdx){
		if(!val){
			//Predict the cbp with the left neighourh
			if ((x - 1) >= 0){ 
				CurrResidu -> Cbp = CurrResidu[-1] . Cbp;
				return 0;
			}

			if((mb_xy - slice -> mb_stride) >= 0){
				CurrResidu -> Cbp = CurrResidu[- slice -> mb_stride] . Cbp;
				return 0;
			}		
			
			CurrResidu -> Cbp = 0;
		}
		else if ( mode < 4) {
			//in case of an intra macroblock
			CurrResidu -> Cbp = golomb_to_intra4x4 [val - 1];
		}else{
			// in case of the codeNum is different of zero
			//this for inter macroblock or base layer predicted macroblock
			CurrResidu -> Cbp = golomb_to_inter [val - 1];	
		}			
	}else if ( mode < 4) {
        //in case of an intra macroblock
		CurrResidu -> Cbp = golomb_to_intra4x4 [val];
	}else{
        // in case of the codeNum is different of zero
		//this for inter macroblock or base layer predicted macroblock
		CurrResidu -> Cbp = golomb_to_inter [val];
	}
	return 0;
}




static __inline int ReadMotionPredSVC( const unsigned char *ai_pcData, int *position, char *MotionPreLx, DATA *Macroblock, int Pred_Lx){

	int mbPartIdx;
	//Recovery of of the image reference for the frame P
	for ( mbPartIdx = 0 ; mbPartIdx < Macroblock -> NumMbPart ; mbPartIdx++ ) {
		if (Macroblock -> MbPartPredMode [mbPartIdx] != Pred_Lx) {
			MotionPreLx [mbPartIdx] = (char ) getNbits(ai_pcData, position, 1);
		}
	}
#ifdef ERROR_DETECTION
	//Error Detection
	if(ErrorsCheckSubRefLx(MotionPreLx, 1)){
		return 1;
	}
#endif
	return 0;
}
#endif
