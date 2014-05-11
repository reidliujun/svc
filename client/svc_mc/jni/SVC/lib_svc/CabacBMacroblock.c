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


//AVC Files
#include "type.h"
#include "symbol.h"
#include "svc_data.h"
#include "svc_type.h"
#include "main_data.h"
#include "vlc_cabac.h"
#include "cabac_svc.h"
#include "VlcCabacSvc.h"
#include "fill_caches.h"
#include "CabacBMacroblock.h"
#include "fill_caches_cabac.h"






/**
B cabac decoding process.
*/
char CabacBPartionning (CABACContext *cabac, unsigned char *cabac_state, SLICE *Slice, RESIDU *CurrentResidu, DATA  *TabBlock, 
					   CABAC_NEIGH *Neigh,  unsigned char *SliceTable, unsigned char NonZeroCountCache [], short mv_cache_l0[][2], 
					   short *ref_cache_l0, short mv_cache_l1[][2], short *ref_cache_l1, const short iMb_x, const short iMb_y, 
					   int DctAllowed, int Direct8x8InferFlag, unsigned char *LastQP, int *MbQpDelta)
{

	const int iCurrMbAddr = iMb_x + iMb_y * Slice -> mb_stride;
	int coded_block_pattern;
	DATA *CurrBlock = &TabBlock[iCurrMbAddr];

	//Updating the parameter in order to decode the VLC
	fill_caches( Slice, CurrentResidu, 1,  NonZeroCountCache, CurrBlock, SliceTable, iMb_x, iMb_y);
	fill_caches_cabac (Slice, Neigh, SliceTable, iCurrMbAddr, *MbQpDelta, CurrentResidu);

	if (!IS_BL(CurrentResidu -> MbType)){
		//Recovery of the motion vectors for the sub_macroblock 
		if ( CurrBlock -> NumMbPart == 4 ) {
			if(SubMbBCabacSVC(&DctAllowed, Direct8x8InferFlag, cabac, cabac_state, Slice, CurrBlock, CurrentResidu, mv_cache_l0, ref_cache_l0, mv_cache_l1, ref_cache_l1)){
				return 1;
			}
		}else if (CurrentResidu -> MbType != B_direct){ 
			if(MbPredBCabacSvc(cabac, cabac_state, Slice, CurrentResidu, CurrBlock, mv_cache_l0, ref_cache_l0, mv_cache_l1, ref_cache_l1)){
				return 1;
			}
		}
	}


	if( Slice -> AdaptiveResidualPredictionFlag && (IS_BL( CurrentResidu -> MbType) || ( !IS_I( CurrentResidu -> MbType) && CurrentResidu -> InCropWindow ))){
		CurrentResidu -> ResidualPredictionFlag = CabacGetResPredFlag(cabac, cabac_state, IS_BL(CurrentResidu -> MbType));
	}else{
		CurrentResidu -> ResidualPredictionFlag = Slice -> DefaultResidualPredictionFlag;
	}

	if ( CurrBlock -> MbPartPredMode[0] != INTRA_16x16 ) {
		CurrentResidu -> Cbp = decode_cabac_mb_cbp_luma( cabac, cabac_state, CurrentResidu -> AvailMask, Neigh, iMb_x, iMb_y) ;
		CurrentResidu -> Cbp |= (decode_cabac_mb_cbp_chroma( cabac, cabac_state, Neigh)) << 4 ;

		if ( DctAllowed && (CurrentResidu -> Cbp & 15)){
			if( decode_cabac_mb_transform_size(cabac, cabac_state, TabBlock, CurrentResidu -> AvailMask, iCurrMbAddr, Slice -> mb_stride)){
				CurrentResidu -> Transform8x8 = CurrBlock -> Transform8x8 = INTRA_8x8;
			}
		}
	}

	coded_block_pattern = CurrentResidu -> Cbp;

	if ( CurrentResidu -> Cbp > 0 || CurrBlock -> MbPartPredMode[0] == INTRA_16x16 ) {
		*MbQpDelta = decode_cabac_mb_dqp( cabac, cabac_state, Neigh, CurrentResidu[- 1] . MbType);
#ifdef TI_OPTIM
		//Initialization of the macroblock neighbourhood
		*LastQP = CurrentResidu -> Qp = divide(*LastQP + *MbQpDelta + 52, 52) >> 8;
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
	return 0;
}
