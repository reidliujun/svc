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

#ifndef CABAC_SVC_H
#define CABAC_SVC_H


char CabacIMacroblockLayer(CABACContext *cabac, unsigned char *cabac_state, const NAL *Nal, SLICE *Slice, 
						   RESIDU *TabResidu, RESIDU *BaseResidu, DATA  *Tab_Block, CABAC_NEIGH *Neigh, 
						    unsigned char *SliceTable, unsigned char NonZeroCountCache [], const short iMb_x, 
						   const short iMb_y, int ConsIntraPredFlag, int DctAllowed, unsigned char *LastQP, int *MbQpDelta );


char CabacPMacroblockLayer(CABACContext *cabac, unsigned char *cabac_state, const NAL *Nal, SLICE *Slice, 
						  RESIDU *TabResidu, RESIDU *BaseResidu, DATA  *Tab_Block, CABAC_NEIGH *Neigh, 
						   unsigned char *SliceTable, unsigned char NonZeroCountCache [], short mv_cache_l0[][2], 
						  short *ref_cache_l0, const short iMb_x , const short iMb_y, 
						  int ConsIntraPredFlag, int DctAllowed, unsigned char *LastQP, int *MbQpDelta);

char CabacBMacroblockLayer(CABACContext *cabac, unsigned char *cabac_state, const NAL *Nal, SLICE *Slice, RESIDU *TabResidu, 
						   RESIDU *BaseResidu, DATA  *Tab_Block, CABAC_NEIGH *Neigh,  unsigned char *SliceTable, unsigned char NonZeroCountCache [], 
						   short mv_cache_l0[][2], short *ref_cache_l0, short mv_cache_l1[][2], short *ref_cache_l1, const short iMb_x, 
						   const short iMb_y, int ConsIntraPredFlag, int DctAllowed, int Direct8x8InferFlag,  unsigned char *LastQP, int *MbQpDelta);





static __inline unsigned char CabacGetResPredFlag(CABACContext *cabac, unsigned char *cabac_state, int IsBl){

	return (unsigned char) get_cabac( cabac, &cabac_state[465 + !IsBl]);
}
#endif
