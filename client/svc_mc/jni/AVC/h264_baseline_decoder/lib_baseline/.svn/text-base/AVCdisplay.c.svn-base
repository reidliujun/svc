
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

#include "string.h"

//AVC Files
#include "symbol.h"
#include "AVCdisplay.h"
#include "display_management.h"



/**
This function allows to display the right image according to it's poc.

@param Nb_img_display Number of decoded images to way before starting displaying.
@param sps_id SPS structure of the current video.
@param Current_pic LIST_MMO structure with contains information on the current picture.
@param address_pic Address of the image to display in the decoded picture buffer.
@param x_size Width of the image to display.
@param y_size Height of the image to display.
@parma ImgToDisplay Parameter which permits to know if there is an picture to display.
@param mmo MMO structure which contains all information about memory management of the video.
*/

void display_order (SPS *sps_id, LIST_MMO *Current_pic, int *address_pic, int *x_size, int *y_size, 
					int *Crop, int *ImgToDisplay, MMO *mmo)
{
	int j;

	//Store the current picture in the reordoring buffer
	StorePicture(&mmo -> poc_params[mmo -> nb_img_display - mmo -> num_free_poc_address], Current_pic, sps_id, 0);
	mmo -> num_free_poc_address --;
	mmo -> LayerMMO [0] . num_decoded_frame ++;
	
	//Display the first picture, and wait after for enough renderer
 	if ( (mmo ->  LayerMMO [0] . num_decoded_frame >= mmo -> nb_img_display) || (mmo -> LayerMMO [0] . num_decoded_frame == 1)){
		int BestPos;

		//Find the best picture with the correct poc
		POC_PARAMS *Frame = FindBestFrame(mmo, mmo -> poc_params, &BestPos, MIN_POC);
		mmo -> num_free_poc_address ++;

		//Remove from short or long ref the current picture
		CurrentDisplay(mmo, &mmo -> LayerMMO[0], Frame -> Picture, address_pic);
		//printf("%d \n", Frame -> Picture -> poc);

		*ImgToDisplay = 1;
		*x_size = Frame -> x_size;
		*y_size = Frame -> y_size;
		*Crop = Frame -> Crop;

		for (j = BestPos; j < mmo -> nb_img_display - mmo -> num_free_poc_address; j++){
			memmove(&mmo -> poc_params[j], &mmo -> poc_params[j + 1],  1 * sizeof(POC_PARAMS));
		}
	}else{
		*ImgToDisplay = 0;
		*x_size = (sps_id -> pic_width_in_mbs) << 4;
		*y_size = (sps_id -> pic_height_in_map_units) << 4;
	}
}


