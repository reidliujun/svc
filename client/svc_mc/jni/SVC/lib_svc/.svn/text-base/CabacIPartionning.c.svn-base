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
* but WITHOUT ANY WARRANTY; without even the implied warranty off
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
//Files
#include "type.h"
#include "symbol.h"
#include "svc_data.h"
#include "svc_type.h"
#include "main_data.h"
#include "vlc_cabac.h"
#include "cabac_svc.h"
#include "VlcCabacSvc.h"
#include "fill_caches.h"
#include "fill_caches_svc.h"
#include "CabacIMacroblock.h"
#include "fill_caches_cabac.h"
#include "macroblock_partitionning.h"


/**
Macroblock I decoding process with CABAC entropy.
*/
char CabacIPartionning (CABACContext *cabac, unsigned char *cabac_state, SLICE *slice, const NAL *Nal, RESIDU *CurrentResidu, 
					   RESIDU *BaseResidu, DATA *TabBlock, CABAC_NEIGH *Neigh, unsigned char *SliceTable,
					   unsigned char NonZeroCountCache [], const short iMb_x, const short iMb_y, 
					   int constrained_intra_pred_flag, int DctAllowed, unsigned char *LastQP, int *MbQpDelta)
{  



	int    coded_block_pattern ;
	const int iCurrMbAddr = iMb_x + iMb_y * slice -> mb_stride;
	short intra4x4_pred_mode_cache[40];

	DATA *MbBlock = &TabBlock[iCurrMbAddr];

#ifdef ERROR_DETECTION
	//Error detection
	if(ErrorsCheckIMbType(CurrentResidu -> MbType)){
		return 1;
	}
#endif

	//Updating the slice table in order to save in which slice each macroblock belong to
	if ( CurrentResidu -> MbType == INTRA_PCM ) {
		unsigned char *ptr = cabac -> bytestream;
		int Position = 0;
		if (cabac -> low & 0x1) {
			ptr -= CABAC_BITS >> 3;
		}
		ParseIPCM(ptr, &Position, CurrentResidu, NonZeroCountCache);
		init_cabac_decoder(cabac, &ptr[384], (int)(cabac -> bytestream_end - ptr));
		CurrentResidu -> Cbp = 0x1ef;
		*MbQpDelta = 0;
	} else {
		//Updating the parameter in order to decode the VLC
		fill_caches_svc(slice, CurrentResidu, BaseResidu, 1, NonZeroCountCache, SliceTable, 
			MbBlock, intra4x4_pred_mode_cache, iMb_x, iMb_y, constrained_intra_pred_flag, Nal -> TCoeffPrediction);
		fill_caches_cabac (slice, Neigh, SliceTable, iCurrMbAddr, *MbQpDelta, CurrentResidu );

		//Recovery of the motion vectors for the sub_macroblock 
		if ( DctAllowed && CurrentResidu -> MbType == INTRA_4x4 && 
			decode_cabac_mb_transform_size(cabac, cabac_state, TabBlock, CurrentResidu -> AvailMask, iCurrMbAddr, slice -> mb_stride)){
				CurrentResidu -> MbType = CurrentResidu -> Transform8x8 = MbBlock -> Transform8x8 = MbBlock -> MbPartPredMode [0] = INTRA_8x8;
		}

		//Recovery of the prediction mode and the motion vectors for the macroblock 
		if ( MbBlock -> MbPartPredMode [0] == INTRA_4x4){
			mb_pred_intra_cabac(cabac, cabac_state, CurrentResidu -> Intra16x16DCLevel, intra4x4_pred_mode_cache);
			CurrentResidu -> IntraChromaPredMode = decode_cabac_mb_chroma_pre_mode( cabac, cabac_state, CurrentResidu -> AvailMask, Neigh);
		}else if (MbBlock -> Transform8x8){
			mb_pred_intra_8x8_cabac(cabac, cabac_state, CurrentResidu -> Intra16x16DCLevel, intra4x4_pred_mode_cache);
			CurrentResidu -> IntraChromaPredMode = decode_cabac_mb_chroma_pre_mode( cabac, cabac_state, CurrentResidu -> AvailMask, Neigh);
		}else if ( CurrentResidu -> MbType != I_BL){
			CurrentResidu -> IntraChromaPredMode = decode_cabac_mb_chroma_pre_mode( cabac, cabac_state, CurrentResidu -> AvailMask,  Neigh);
		}

#ifdef ERROR_DETECTION
		//Error detection
		if(ErrorsCheckIntraChromaPredMode(&CurrentResidu -> IntraChromaPredMode)){
			return 1;
		}
#endif


		if ( MbBlock -> MbPartPredMode[0] != INTRA_16x16) {
			CurrentResidu -> Cbp = decode_cabac_mb_cbp_luma(cabac, cabac_state, CurrentResidu -> AvailMask, Neigh, iMb_x, iMb_y) ;
			CurrentResidu -> Cbp |= (decode_cabac_mb_cbp_chroma(cabac, cabac_state, Neigh)) << 4;
		}

		coded_block_pattern = CurrentResidu -> Cbp;

		if ( CurrentResidu -> Cbp > 0 || MbBlock -> MbPartPredMode[0] == INTRA_16x16 ) {
			if (CurrentResidu -> Cbp & 0x0f && DctAllowed && CurrentResidu -> MbType == 26 && 
				decode_cabac_mb_transform_size(cabac, cabac_state, TabBlock, CurrentResidu -> AvailMask, iCurrMbAddr, slice -> mb_stride)){
					CurrentResidu -> Transform8x8 = MbBlock -> Transform8x8 = MbBlock -> MbPartPredMode [0] = INTRA_8x8;
			}
			*MbQpDelta = decode_cabac_mb_dqp( cabac, cabac_state, Neigh, CurrentResidu[- 1] . MbType);
#ifdef TI_OPTIM
			//Initialization of the macroblock Neighbourhood
			*LastQP = CurrentResidu -> Qp = divide(*last_QP + *MbQpDelta + 52, 52) >> 8;
#else
			*LastQP = CurrentResidu -> Qp = (*LastQP + *MbQpDelta + 52) % 52;
#endif

			//Decoding process of the VLC 
			residual_cabac(cabac, cabac_state, CurrentResidu, Neigh, NonZeroCountCache, &coded_block_pattern);
			CurrentResidu -> Cbp = coded_block_pattern;
		} else {
			//In case of there is no error in the prediction
			init_mb_skip(NonZeroCountCache);
			CurrentResidu -> Qp = *LastQP;
		}
	}
	return 0;
}
