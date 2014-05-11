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

#ifndef PIC_LIST_H
#define PIC_LIST_H


#include "type.h"





void fill_default_ref_list(LAYER_MMO *LayerMmo, LIST_MMO *refPicLXl0,LIST_MMO *refPicLXl1  ,LIST_MMO *current, int slice_type);
short PocComputation(SPS *sps, int IdrPicFlag, int nal_ref_idc, LAYER_MMO *LayerMmo);
void fill_frame_num_gap(MMO *mmo, LAYER_MMO *LayerMmo, int MaxFrameNum, int num_ref_frames, LIST_MMO RefPicListL0[], LIST_MMO RefPicListL1[]);
void pic_order_process(SPS *sps, int nal_unit_type, int nal_ref_idc,MMO *mmo, LAYER_MMO *LayerMmo, LIST_MMO *Current_pic  , LIST_MMO RefPicListL0[], LIST_MMO RefPicListL1[]);
void execute_ref_pic_marking(LIST_MMO *Curr, MMO *Mmo, int LayerId);
void direct_ref_list_init(SLICE *Slice, LIST_MMO RefPicListL0[], LIST_MMO RefPicListL1[], LIST_MMO *Current_pic, LAYER_MMO *LayerMmo);


#endif


