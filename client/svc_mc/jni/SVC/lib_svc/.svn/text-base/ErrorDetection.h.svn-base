
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




void DetectSliceError(NAL *Nal, SPS *sps, PPS *pps, LIST_MMO *Current_pic, MMO *Mmo, 
					  int EndOfSlice, int PrevEndOfSlice, int *Crop, int *ImgToDisplay, 
					  int *AddressPic, int *xsize, int *ysize);

void DetecMissingTopLayer(NAL *Nal, SPS *sps, PPS *pps, LIST_MMO *Current_pic, MMO *Mmo,
						  int *Crop, int *ImgToDisplay, int *AddressPic, int *xsize, int *ysize);

void DetectPocProblem(NAL *Nal, MMO *Mmo, LIST_MMO *Current_pic, LAYER_MMO *LayerMmo, SPS *sps);
void RemoveDisplayError(MMO *Mmo, NAL *Nal);
