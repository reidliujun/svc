
/*****************************************************************************
   *
   *  Open SVC Decoder developped in IETR image lab
   *
   *
   *
   *              Médéric BLESTEL <mblestel@insa-rennes.Fr>
   *              Mickael RAULET <mraulet@insa-rennes.Fr>
   *			  Maxime  PELCAT <mpelcat@insa-rennes.Fr>
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



#include "type.h"
#include "init_data.h"


/**
This function permits to initialize the table non_zero_count_cache with the value val. 

@param non_zero_count_cache Specifies the coeff_token of each blocks 4x4 of a macroblock.
@param val The value to initialise the table.

*/
void init_non_zero_count_cache ( unsigned char *non_zero_count_cache, unsigned char val )
{
    int i ;
    
    for ( i = 0 ; i < 48 ; i++ ) {
        *non_zero_count_cache++ = val ;
    }
}





/**
This function permits to initialize the table with -1. 

@param mode The table to initialise.

*/
void init_intra_pred( int * mode){

	int i;
	 for ( i = 0 ; i < 16 ; i++ ) {
		 *mode++ = -1;
	 }
}




/**
This function permits to initialize the structure. 

@param size The number of macroblcok to initialize.
@param intra The structure which contains the data for the current macroblock. 
*/
void init_data_block (const int ai_iSize, DATA ao_tstBlock[ ]){
	int i;

	for( i=0;i<ai_iSize;i++){
		init_block(&ao_tstBlock[i]);
	}
}

void init_data_residu (const int ai_iSize, RESIDU Block[ ]){
	int i;

	for( i=0;i<ai_iSize;i++){
		init_residu(&Block [i]);
	}
}

/**
This function permits to initialize the table picture_mb_type. 

@param size The number of macroblcok to initialize.
@param intra The tab to initialize.

*/
void init_int_tab (const int ai_iSize, int ao_tiPicture_mb_type [ ] )
{
    int i ;
    
    for ( i = 0 ; i < ai_iSize ; i++ ) {
        ao_tiPicture_mb_type [i] = -1 ;
    }
}

/**
This function permits to initialize a table 

@param size The number of macroblcok to initialize.

*/
void InitUcharTab (const int ai_iSize, unsigned char Table [ ] )
{
	 int i ;
    
    for ( i = 0 ; i < ai_iSize ; i++ ) {
        Table [i] = 255 ;
    }
}



/**
This function is used to initialize the MMO structure.

@param num_of_layers Number of layer to initialize.
@param mmo MMo structure.
*/
void init_mmo( int num_of_layers, MMO *mmo){


	int i, j;
		
	memset(mmo, 0, sizeof(MMO));
	memset(mmo -> Prev, -1, sizeof(LIST_MMO));
	mmo -> low_delay = 1;	

	for ( j = 0; j < num_of_layers; j++){
		mmo -> LayerMMO[j] . frame_num = -1;
		for (i = 0; i < 16; i++){
			mmo -> LayerMMO[j] . LongRef[i] . poc = MIN_POC;
		}
	}
}




void init_int ( int *a )
{
   *a = 0 ;
}


/**
This function permits to initialize the principal parameter of the structure slice 

@param Slice The slice structure contains all information used to decode the next slice
*/
void init_slice ( SLICE *Slice ){

	memset (Slice,'\0',sizeof(SLICE));
}




void reinit_slice ( SLICE *Slice )
{

	Slice -> mb_xy = 0;
	Slice -> slice_num = 0;
}



/**
This function is used to initialize the LIST_MMO structure.

@param num_of_layers Number of layer to initialize.
@param mmo MMo structure.
*/
void InitListMmo( LIST_MMO *mmo){

	memset(mmo, 0, sizeof(LIST_MMO));
}
