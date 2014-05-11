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


//AVC Files
#include "type.h"

//SVC Files
#include "svc_type.h"
#include "set_layer.h"
#include "SwitchAdress.h"



/**
This function allows to change of layer on an IDR frame.
*/
void switch_layer(NAL *Nal)
{
	
	if(Nal -> IdrFlag == 1 && (Nal -> ComDqId != Nal -> DqIdWanted)){
		//IDR frame and a valid command to change of layer
		int i ;
		int test = 0;
		int SaveDqId = Nal -> ComDqId;

		for ( i = 0; i < 8; i++){
			if (SaveDqId == Nal -> layer_id_table[i]){
				//we find it
				break;
			} else if(i < 7 && SaveDqId > Nal -> layer_id_table[i] && Nal -> layer_id_table[i + 1] == -1){
				//we did not find it, but we should wait
				test = 1;
				break;
			}else if (i < 7 && SaveDqId > Nal -> layer_id_table[i] && SaveDqId < Nal -> layer_id_table[i + 1]){
				//we did not find it, but the value must be wrong, so we choose another one
				test = 2;
				break;
			}
		}
		
		
		if ( test != 1){
			if (test == 2){
				//The DQId has not been found.
				//we know that the DQId wanted, is between the layer_id_table [i] and layer_id_table [i + 1]
				Nal -> DqIdWanted = Nal -> layer_id_table [i + 1];
			}
			else {
				int WantedLayer = GetLayerNumber(Nal, SaveDqId);
				//we have to update the WantedDqId parameter to decode correctly the base layer if it's the one wanted 
				if (WantedLayer < Nal -> MaxNumLayer){
					Nal -> DqIdWanted = Nal -> layer_id_table[WantedLayer];
				}else {
					Nal -> DqIdWanted = Nal -> layer_id_table[Nal -> MaxNumLayer - 1];
				}
			}
			SaveDqId = Nal -> DqIdToDisplay = Nal -> DqIdWanted;
			Nal -> FlushRenderer = 1;
		}else {
			Nal -> DqIdToDisplay = Nal -> DqIdWanted = SaveDqId;
			Nal -> FlushRenderer = 1;
		}
	}else {
		Nal -> DqIdToDisplay = Nal -> DqIdWanted;
	}

	//In case of AVC Streams
	if(Nal -> layer_id_table[0] == -1){
		Nal -> layer_id_table [0] = 0;
	}
}
