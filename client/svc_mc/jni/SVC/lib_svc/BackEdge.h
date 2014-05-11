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

#ifndef BACK_EDGE_H
#define BACK_EDGE_H



void BackLeftEdge		(const NAL *Nal, int AvailMask, unsigned char tmp_luma[], unsigned char base_luma[], unsigned char tmp_Cb[], unsigned char base_Cb[], unsigned char tmp_Cr[], unsigned char base_Cr[]);
void BackRightEdge		(const NAL *Nal, int AvailMask, unsigned char tmp_luma[], unsigned char base_luma[], unsigned char tmp_Cb[], unsigned char base_Cb[], unsigned char tmp_Cr[], unsigned char base_Cr[]);
void BackTopEdge		(const NAL *Nal, int AvailMask, unsigned char tmp_luma[], unsigned char base_luma[], unsigned char tmp_Cb[], unsigned char base_Cb[], unsigned char tmp_Cr[], unsigned char base_Cr[]);
void BackTopLeftEdge    (const NAL *Nal, int AvailMask, unsigned char tmp_luma[], unsigned char base_luma[], unsigned char tmp_Cb[], unsigned char base_Cb[], unsigned char tmp_Cr[], unsigned char base_Cr[]);
void BackTopRightEdge   (const NAL *Nal, int AvailMask, unsigned char tmp_luma[], unsigned char base_luma[], unsigned char tmp_Cb[], unsigned char base_Cb[], unsigned char tmp_Cr[], unsigned char base_Cr[]);
void BackBottomEdge     (const NAL *Nal, int AvailMask, unsigned char tmp_luma[], unsigned char base_luma[], unsigned char tmp_Cb[], unsigned char base_Cb[], unsigned char tmp_Cr[], unsigned char base_Cr[]);
void BackBottomLeftEdge (const NAL *Nal, int AvailMask, unsigned char tmp_luma[], unsigned char base_luma[], unsigned char tmp_Cb[], unsigned char base_Cb[], unsigned char tmp_Cr[], unsigned char base_Cr[]);
void BackBottomRightEdge(const NAL *Nal, int AvailMask, unsigned char tmp_luma[], unsigned char base_luma[], unsigned char tmp_Cb[], unsigned char base_Cb[], unsigned char tmp_Cr[], unsigned char base_Cr[]);
#endif
