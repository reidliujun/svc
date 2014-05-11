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



#ifndef FILL_CABAC
#define FILL_CABAC

#include "type.h"
#include "main_data.h"


void fill_caches_cabac ( SLICE *slice, CABAC_NEIGH *neigh, unsigned char *SliceTable, int mb_xy, int diff, RESIDU *residu );

void fill_caches_motion_vector_B(SLICE *slice, int b_stride, int b8_stride, short ref_cache_l0 [], short ref_cache_l1 [], 
								 short mv_cache_l0[][2], short  mv_cache_l1[][2], unsigned char *SliceTable, RESIDU *CurrResidu, 
								 short *mv_l0, short mv_l1[], short ref_l0[], short ref_l1[], int iCurrMbAddr, int ai_iX);

		
void init_mb_skip_mv_ref(short mv[], short ref[], int b_stride, int b8_stride);


void reset_ref_index( int size, int ref_l0[], int ref_l1[]); 
void write_back_motion_cache_B(const int ai_iB_stride, const int ai_iB8_stride, const int ai_iMb_xy, 
							   short ai_tiMv_l0[ ], short ai_tiMv_l1[ ], short  ai_tiMv_cache_l0[ ][2], 
							   short ai_tiMv_cache_l1[ ][2], short ai_tiRef_l0[ ], short ai_tiRef_l1[ ], 
							   short aio_tiRef_cache_l0[ ], short aio_tiRef_cache_l1[ ], int mb_stride);


void write_back_main_interpolation(const int ai_iB_stride, const int ai_iB8_stride, short ai_tiMv_l0[ ], short ai_tiMv_l1[ ], 
								   short  ai_tiMv_cache_l0[ ][2], short  ai_tiMv_cache_l1[ ][2], short ai_tiRef_l0[ ], short ai_tiRef_l1[ ], 
								   short aio_tiRef_cache_l0[ ], short aio_tiRef_cache_l1[ ]);

void write_back_main_interpolation_8x8(const int ai_iB_stride, const int ai_iB8_stride, short ai_tiMv_l0[ ], short ai_tiMv_l1[ ], 
									   short ai_tiMv_cache_l0[ ][2], short  ai_tiMv_cache_l1[ ][2], short ai_tiRef_l0[ ], 
									   short ai_tiRef_l1[ ], short aio_tiRef_cache_l0[ ], short aio_tiRef_cache_l1[ ]);


#endif
