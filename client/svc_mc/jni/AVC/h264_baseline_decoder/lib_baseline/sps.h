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

#ifndef SPS_H
#define SPS_H


void init_sps ( SPS *sps );
void hrd_parameters ( unsigned char *data, int *aio_piPosition, SPS *sps );
void vui_parameters ( unsigned char *data, int *aio_piPosition, SPS *sps );
void seq_parameter_set( unsigned char *data, SPS *sps);
void get_max_dpb( SPS * sps_id);
void seq_parameter_set_data ( unsigned char *data, int *aio_piPosition,  SPS *sps);
void pic_parameter_set  ( unsigned char *data, int *ao_aio_pstSlice_group_id,  PPS *pps, SPS *sps,  int NalBytesInNalunit );
void PPSCut (unsigned char *data, PPS *pps, SPS *sps, int NalBytesInNalunit );
#endif
