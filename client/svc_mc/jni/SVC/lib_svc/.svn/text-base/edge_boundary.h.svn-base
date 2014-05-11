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


#ifndef EDGE_BOUNDARY_H
#define EDGE_BOUNDARY_H


void external_edge(const int mb_type, const int mbn_type, const int mb_xy, const int mbn_xy, char bS[4], 
				   unsigned char ProfileIdc, int dir, int edge,int slice_type, int spatial_scalability,RESIDU *residu, 
				   short mv_cache_l0[][2], short *ref_cache_l0, short poc_l0[], 
				   short mv_cache_l1[][2], short *ref_cache_l1, short poc_l1[] );


void internal_edge(const int mb_type, char bS[4], int dir, int edge, int slice_type, int spatial_scalability,
				   unsigned char ProfileIdc, RESIDU *residu, short mv_cache_l0[][2], short *ref_cache_l0, 
				   short poc_l0[], short mv_cache_l1[][2], short *ref_cache_l1, short poc_l1[]);

void InterExternalEdge(const int mb_type, const int mbn_type, const int mb_xy, const int mbn_xy, char bS[4], int dir, 
					   int edge, int spatial_scalability, int DeblockingFilterIdc, const RESIDU *residu );
void InterInternalEdge(const int mb_type, char bS[4], int dir, int edge, int spatial_scalability, const RESIDU *residu);
#endif
