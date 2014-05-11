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




#ifndef MAIN_DATA_H
#define MAIN_DATA_H
#include "type.h"


#define CABAC_BITS 8
#define CABAC_MASK ((1<<CABAC_BITS)-1)



typedef struct CABACContext{
    int low;
    int range;
    int outstanding_count;  //>> ne sert a rien, a verifier
    unsigned char lps_range[2*65][4];   ///< rangeTabLPS
    unsigned char lps_state[2*64];      ///< transIdxLPS
    unsigned char mps_state[2*64];      ///< transIdxMPS
    unsigned char *bytestream_start;
    unsigned char *bytestream;
    unsigned char *bytestream_end;

}CABACContext;

typedef struct {

    int intra_chroma_pred_mode_left ;
    int intra_chroma_pred_mode_top ;
	int Cbp_a;
	int Cbp_b;
	int last_cbp;
	int last_qp_diff;

}CABAC_NEIGH;




typedef struct {
	STRUCT_PF		    baseline_vectors[1];
} MAIN_STRUCT_PF;


#endif


