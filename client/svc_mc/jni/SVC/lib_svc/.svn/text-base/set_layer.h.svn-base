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


#ifndef SETLAYER_H
#define SETLAYER_H

#include "svc_type.h"



#ifndef TI_OPTIM
#ifdef WIN32
#include <windows.h>
#endif
#endif


void get_layer(int argc, char **argv, ConfigSVC *Config);
void choose_layer (ConfigSVC *Config);



static __inline int GetLayerNumber(NAL *Nal, int DQId){
	
	int i;

	//If we know all DqId
	for (i = 0 ; i < 8; i++){
		if ( Nal -> layer_id_table[i] == DQId){
			return i;
		}
	}

	//In case of a wanted DqId but not a one which belongs to the sequence
	for (i = 0 ; i < 8; i++){
		if ( i < 7 && Nal -> layer_id_table[i] < DQId && Nal -> layer_id_table[i + 1] > DQId){
			break;
		}
	}


	//In case of nothing found, we prefer to send the layer 0
	if ( i == 8){
		return 0;
	}
	
	return i;//DQId >> 4;
}


#endif
