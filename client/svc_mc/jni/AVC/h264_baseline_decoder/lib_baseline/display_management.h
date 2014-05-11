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

#ifndef DISPLAY_MANAG_H
#define DISPLAY_MANAG_H

#include "type.h"




void StorePicture(POC_PARAMS *Frame, LIST_MMO *Current_pic, SPS *sps, int layer_number);
POC_PARAMS* FindBestFrame(MMO *mmo, POC_PARAMS *FrameList, int *BestPos, int MinPoc);
POC_PARAMS* FindBestFrameTCPMP(MMO *mmo, LAYER_MMO *Layer_Mmo, int pics, int *out_of_order, int *out_idx, LIST_MMO *Current_pic);
void CurrentDisplay(MMO *CurrMmo, LAYER_MMO *LayerMmo, LIST_MMO *Frame, int *address_pic);
void ImageSetEdges ( unsigned char *y,  unsigned char * u, unsigned char *v	, const unsigned int stride_dst, const unsigned int height );
void SetDisplayPicture(LAYER_MMO *LayerMmo, int address_pic, int best_poc);
char SetAsDisplayed(LAYER_MMO *LayerMmo, int best_poc, int address_pic);
#endif
