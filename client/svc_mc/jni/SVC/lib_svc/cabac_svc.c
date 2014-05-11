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


//AVC files
#include "type.h"
#include "main_data.h"
#include "cabac.h"
#include "macroblock_cabac.h"
#include "mb_cabac_partionning.h"


//SVC files
#include "svc_data.h"
#include "cavlc_svc.h"
#include "cabac_svc.h"
#include "Coordonates.h"
#include "CabacIMacroblock.h"
#include "CabacPMacroblock.h"
#include "CabacBMacroblock.h"


/**
Decode base mode flag.
*/
unsigned char CabacGetBaseModeFlag( CABACContext *c, unsigned char *state, RESIDU *CurrResidu,  unsigned char *SliceTable, int mb_stride) {


	int AvailMask = CurrResidu -> AvailMask;
	int ctx = 2;

	if( (AvailMask & 0x01) && SliceTable[0] == SliceTable[- 1] && IS_BL(CurrResidu [-1] . MbType)){
		ctx--;
	}

	if( ((AvailMask >> 1) & 0x01) && SliceTable[0] == SliceTable[- mb_stride] && IS_BL(CurrResidu [- mb_stride] . MbType)){
		ctx--;
	}

	return (unsigned char) get_cabac( c, &state[ctx] );
}









/**
Decode an Intra macroblock.
*/
char CabacIMacroblockLayer(CABACContext *cabac, unsigned char *cabac_state, const NAL *Nal, SLICE *Slice, 
						   RESIDU *TabResidu, RESIDU *BaseResidu, DATA  *Tab_Block, CABAC_NEIGH *Neigh, 
						   unsigned char *SliceTable, unsigned char NonZeroCountCache [], const short iMb_x, 
						   const short iMb_y, int ConsIntraPredFlag, int DctAllowed, unsigned char *LastQP, int *MbQpDelta )
{




	const int iCurrMbAddr = iMb_x + iMb_y * Slice -> mb_stride;
	DATA *CurrBlock = &Tab_Block[iCurrMbAddr];
	RESIDU *CurrentResidu = &TabResidu[iCurrMbAddr];


	unsigned char BaseModeFlag = 0;
	unsigned char InCropWindow = CurrentResidu -> InCropWindow;

	CurrentResidu -> Transform8x8 = 0;
	//TODO
	if( Slice -> AdaptiveBaseModeFlag && InCropWindow)  {
		CurrentResidu -> BaseModeFlag = BaseModeFlag = CabacGetBaseModeFlag(cabac, &cabac_state[460], 
			CurrentResidu, &SliceTable[iCurrMbAddr], Slice -> mb_stride);   
	}else if (Slice -> DefaultBaseModeFlag){
		CurrentResidu -> BaseModeFlag = BaseModeFlag = 1;
	}


	if(!BaseModeFlag){
		int mb_type = decode_cabac_intra_mb_type(cabac, &cabac_state[3], CurrentResidu, 1, Slice -> mb_stride);

		CurrBlock -> MbPartPredMode [0] = i_mb_type_info[mb_type].type ;
		CurrentResidu -> MbType = i_mb_type_info[mb_type].type ;
		CurrentResidu -> Cbp = i_mb_type_info[mb_type] . Cbp;
		CurrentResidu -> Intra16x16PredMode =  i_mb_type_info[mb_type].pred_mode;
		
		//Initialize base macroblock address
		GetBaseMbAddr(Nal, CurrBlock, iMb_x << 4, iMb_y << 4);
	}
	else{
		CurrBlock -> MbPartPredMode [0] = CurrentResidu -> MbType = GetBaseType(Nal, BaseResidu, CurrBlock, iMb_x, iMb_y);
	}

	return CabacIPartionning(cabac, cabac_state, Slice, Nal, CurrentResidu, BaseResidu, Tab_Block, Neigh, SliceTable, 
			NonZeroCountCache, iMb_x, iMb_y, ConsIntraPredFlag, DctAllowed, LastQP, MbQpDelta);
}




/**
Decode an P macroblock.
*/
char CabacPMacroblockLayer(CABACContext *cabac, unsigned char *cabac_state, const NAL *Nal, SLICE *Slice, 
						  RESIDU *TabResidu, RESIDU *BaseResidu, DATA  *Tab_Block, CABAC_NEIGH *Neigh, 
						  unsigned char *SliceTable, unsigned char NonZeroCountCache [], short mv_cache_l0[][2], 
						  short *ref_cache_l0, const short iMb_x, const short iMb_y, 
						  int ConsIntraPredFlag, int DctAllowed, unsigned char *LastQP, int *MbQpDelta)
{




	const int iCurrMbAddr = iMb_x + iMb_y * Slice -> mb_stride;
	DATA *CurrBlock = &Tab_Block[iCurrMbAddr];
	RESIDU *CurrentResidu = &TabResidu[iCurrMbAddr];


	unsigned char BaseModeFlag = 0;
	unsigned char	InCropWindow = CurrentResidu -> InCropWindow;

	CurrentResidu -> Transform8x8 = 0;
	//TODO
	if( Slice -> AdaptiveBaseModeFlag && InCropWindow)  {
		CurrentResidu -> BaseModeFlag = BaseModeFlag = CabacGetBaseModeFlag(cabac, &cabac_state[460], 
			CurrentResidu, &SliceTable[iCurrMbAddr], Slice -> mb_stride);   
	}else if (Slice -> DefaultBaseModeFlag){
		CurrentResidu -> BaseModeFlag = BaseModeFlag  = 1;
	}


	if(!BaseModeFlag){
		int mb_type = decode_cabac_mb_type(cabac, cabac_state, TabResidu, SliceTable, iCurrMbAddr, 
			Slice -> slice_type, Slice -> mb_stride);
#ifdef ERROR_DETECTION
		//Error detection
		if(ErrorsCheckMbType(mb_type, 5 + 27)){
			return 1;
		}
#endif

		if (mb_type < 5){
			CurrBlock -> NumMbPart = p_mb_type_info[mb_type] . partcount ;
			CurrBlock -> MbPartPredMode [0] = p_mb_type_info[mb_type] . type_0 ;
			CurrBlock -> MbPartPredMode [1] = p_mb_type_info[mb_type] . type_1 ;
			CurrentResidu -> MbType = p_mb_type_info[mb_type] . name ;
			CurrentResidu -> Mode = p_mb_type_info[mb_type] . Mode ;
		} else{
			mb_type -= 5; 
			CurrBlock -> MbPartPredMode [0] = i_mb_type_info[mb_type] . type ;
			CurrentResidu -> MbType = i_mb_type_info[mb_type] . type ;
			CurrentResidu -> Cbp = i_mb_type_info[mb_type] . Cbp;
			CurrentResidu -> Intra16x16PredMode =  i_mb_type_info[mb_type] . pred_mode;	
		}
		//Initialize base macroblock address
		GetBaseMbAddr(Nal, CurrBlock, iMb_x << 4, iMb_y << 4);
	}else{
		CurrBlock -> MbPartPredMode [0] = CurrentResidu -> MbType = GetBaseType(Nal, BaseResidu, CurrBlock, iMb_x, iMb_y);
	}

	if (IS_I(CurrentResidu -> MbType)){
		return 	CabacIPartionning(cabac, cabac_state, Slice, Nal, CurrentResidu, BaseResidu, Tab_Block, Neigh, SliceTable, 
			NonZeroCountCache, iMb_x, iMb_y, ConsIntraPredFlag, DctAllowed, LastQP, MbQpDelta);
	} else {
		return CabacPPartionning(cabac, cabac_state, Slice, CurrentResidu, Tab_Block, Neigh, SliceTable, 
			NonZeroCountCache, mv_cache_l0, ref_cache_l0, iMb_x, iMb_y, DctAllowed, LastQP, MbQpDelta);
	}
}





/**
Decode an B macroblock.
*/
char CabacBMacroblockLayer(CABACContext *cabac, unsigned char *cabac_state, const NAL *Nal, SLICE *Slice, RESIDU *TabResidu, 
						   RESIDU *BaseResidu, DATA *Tab_Block, CABAC_NEIGH *Neigh, unsigned char *SliceTable, unsigned char NonZeroCountCache [], 
						   short mv_cache_l0[][2], short *ref_cache_l0, short mv_cache_l1[][2], short *ref_cache_l1, const short iMb_x, 
						   const short iMb_y, int ConsIntraPredFlag, int DctAllowed, int Direct8x8InferFlag, unsigned char *LastQP, int *MbQpDelta)
{




	const int iCurrMbAddr = iMb_x + iMb_y * Slice -> mb_stride;
	DATA *CurrBlock = &Tab_Block[iCurrMbAddr];
	RESIDU *CurrentResidu = &TabResidu[iCurrMbAddr];

	unsigned char BaseModeFlag = 0;
	unsigned char	InCropWindow = CurrentResidu -> InCropWindow;

	CurrentResidu -> Transform8x8 = 0;
	//TODO
	if( Slice -> AdaptiveBaseModeFlag && InCropWindow)  {
		CurrentResidu -> BaseModeFlag = BaseModeFlag = CabacGetBaseModeFlag(cabac, &cabac_state[460], CurrentResidu, &SliceTable[iCurrMbAddr], Slice -> mb_stride);   
	}else if (Slice -> DefaultBaseModeFlag){
		CurrentResidu -> BaseModeFlag = BaseModeFlag  = 1;
	}


	if(!BaseModeFlag){
		int mb_type = decode_cabac_mb_type(cabac, cabac_state, CurrentResidu, SliceTable, iCurrMbAddr, Slice -> slice_type, Slice -> mb_stride);
#ifdef ERROR_DETECTION
		//Error detection
		if(ErrorsCheckMbType(mb_type, 23 + 27)){
			return 1;
		}
#endif
		if (mb_type < 23){
			CurrBlock -> NumMbPart = b_mb_type_info[mb_type] . partcount;
			CurrBlock -> MbPartPredMode [0] = b_mb_type_info[mb_type] . type_0;
			CurrBlock -> MbPartPredMode [1] = b_mb_type_info[mb_type] . type_1;
			CurrentResidu -> MbType = b_mb_type_info[mb_type] . name;
			CurrentResidu -> Mode = b_mb_type_info[mb_type] . Mode;
			//SVC correction 
			if ( CurrentResidu -> MbType == B_direct){
				CurrentResidu -> Mode = 4;
			}
		} else{
			mb_type -= 23; 
			CurrBlock -> MbPartPredMode [0] = i_mb_type_info[mb_type] . type ;
			CurrentResidu -> MbType = i_mb_type_info[mb_type] . type ;
			CurrentResidu -> Cbp = i_mb_type_info[mb_type] . Cbp;
			CurrentResidu -> Intra16x16PredMode =  i_mb_type_info[mb_type] . pred_mode;	
		}
		//Initialize base macroblock address
		GetBaseMbAddr(Nal, CurrBlock, iMb_x << 4, iMb_y << 4);
	}else{
		CurrBlock -> MbPartPredMode [0] = CurrentResidu -> MbType = GetBaseType(Nal, BaseResidu, CurrBlock, iMb_x, iMb_y);
	}

	if (IS_I(CurrentResidu -> MbType)){
		return CabacIPartionning(cabac, cabac_state, Slice, Nal, CurrentResidu, BaseResidu, Tab_Block, Neigh, SliceTable, 
			NonZeroCountCache, iMb_x, iMb_y, ConsIntraPredFlag, DctAllowed, LastQP, MbQpDelta);
	} else {
		return CabacBPartionning(cabac, cabac_state, Slice, CurrentResidu, Tab_Block, Neigh, SliceTable, NonZeroCountCache, 
			mv_cache_l0, ref_cache_l0, mv_cache_l1, ref_cache_l1, iMb_x, iMb_y, DctAllowed, Direct8x8InferFlag, LastQP, MbQpDelta);
	}
}
