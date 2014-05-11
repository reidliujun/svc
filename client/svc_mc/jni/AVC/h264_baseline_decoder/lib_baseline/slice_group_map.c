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


#include "clip.h"
#include "SliceGroupMap.h"



/**
This function permits to determine in which Slice, each macroblock belong to according to the image structure. 

@param Pps The pps structure.
@param ao_tiMapUnitToSliceGroupMap A table which contains the aio_piPosition of each macroblock in a Slice according to the image structure. 
@param ai_islice_group_change_cycle It's used to derive the number of Slice group map units.
@param ai_iPicWidthInMbs The width in macroblock of the image.
@param PicHeightInMapUnits The height in macroblock of the image.
@param PicSizeInMapUnits The size in macroblock of the image.
@param ai_slice_group_id Table for determine the slice decoding map.
@param ai_bottom_right_tab Table used for determine the slice decoding map.
@param ai_top_left_tab Table for used determine the slice decoding map.
*/
void mapUnitToSlice (const SPS *Sps, const PPS *Pps,  unsigned char ao_tiMapUnitToSliceGroupMap [ ], 
					 const int ai_islice_group_change_cycle, unsigned char *ai_Slice_group_id)
{
	int i, j, k, x, y ;
	int iGroup ;
	int	sizeOfUpperLeftGroup ;
	int	MapUnitsInSliceGroup0;
	int leftBound;
	int topBound;
	int rightBound;
	int bottomBound;
	int xDir;
	int yDir;
	int mapUnitVacant;

	short	PicWidthInMbs = Sps -> pic_width_in_mbs;
	short	PicHeightInMapUnits = Sps -> pic_height_in_map_units;
	short PicSizeInMapUnits = PicWidthInMbs * PicHeightInMapUnits;
	const short *TopLeftTab = Pps -> top_left_tab;
	const short *BottomRightTab = Pps -> bottom_right_tab;



	int SliceGroupChangeRate = Pps -> slice_group_change_rate_minus1;

	if (ai_islice_group_change_cycle * SliceGroupChangeRate < PicSizeInMapUnits){
		MapUnitsInSliceGroup0 = ai_islice_group_change_cycle * SliceGroupChangeRate;
	}
	else{
		MapUnitsInSliceGroup0 = PicSizeInMapUnits;
	}

	sizeOfUpperLeftGroup = Pps -> slice_group_change_direction_flag ? (PicSizeInMapUnits - MapUnitsInSliceGroup0)  : MapUnitsInSliceGroup0;


	//Initialization of the macroblock map units
	if ( Pps -> num_slice_groups_minus1 == 0 ) {
		for ( i = 0 ; i < PicSizeInMapUnits ; i++ )  {
			ao_tiMapUnitToSliceGroupMap [i] = 0 ;
		}
	}
	else {
		switch(Pps -> slice_group_map_type){
			case 0:
				i = 0 ;
				do 	{
					for ( iGroup = 0 ; iGroup <= Pps -> num_slice_groups_minus1 && i < PicSizeInMapUnits ; 
						i += Pps -> run_length_minus1 [iGroup++] + 1 ) 	{
						for ( j = 0 ; j <= Pps -> run_length_minus1 [iGroup] && i + j < PicSizeInMapUnits ; j++ ) 	{
							ao_tiMapUnitToSliceGroupMap [i + j] = iGroup ;
						}
					}
				} while ( i < PicSizeInMapUnits );
				break;

			case 1:          
				for ( i = 0 ; i < PicSizeInMapUnits ; i++ ) {
					ao_tiMapUnitToSliceGroupMap [i]	= (i % PicWidthInMbs + ((i / PicWidthInMbs * (Pps -> num_slice_groups_minus1 + 1)) >> 1))
						% (Pps -> num_slice_groups_minus1 + 1);
				}
				break;

			case 2:
				for ( i = 0 ; i < PicSizeInMapUnits ; i++ ) {
					ao_tiMapUnitToSliceGroupMap [i] = Pps -> num_slice_groups_minus1 ;
				}

				for ( iGroup = Pps -> num_slice_groups_minus1 - 1 ; iGroup >= 0 ; iGroup-- ) 		{
					short yTopLeft, xTopLeft, yBottomRight, xBottomRight;
					GetMbPosition(TopLeftTab [iGroup], PicWidthInMbs, &xTopLeft, &yTopLeft);
					GetMbPosition(BottomRightTab [iGroup], PicWidthInMbs, &xBottomRight, &yBottomRight);

					for ( y = yTopLeft ; y <= yBottomRight ; y++ ) {
						for ( x = xTopLeft ; x <= xBottomRight ; x++ ) {
							ao_tiMapUnitToSliceGroupMap [y * PicWidthInMbs + x] = iGroup ;
						}
					}
				}
				break;

			case 3:
				for( i = 0; i < PicSizeInMapUnits; i++ ){
					ao_tiMapUnitToSliceGroupMap[ i ] = 1;
				}

				leftBound = rightBound = x = (PicWidthInMbs - Pps -> slice_group_change_direction_flag ) >> 1;
				topBound = bottomBound = y = ( PicHeightInMapUnits - Pps -> slice_group_change_direction_flag ) >> 1;
				xDir = Pps -> slice_group_change_direction_flag - 1;
				yDir = Pps -> slice_group_change_direction_flag ;

				for( k = 0; k < MapUnitsInSliceGroup0; k += mapUnitVacant ) 	{
					mapUnitVacant = ( ao_tiMapUnitToSliceGroupMap[ y * PicWidthInMbs + x ]  ==  1 );
					if( mapUnitVacant )	{
						ao_tiMapUnitToSliceGroupMap[ y * PicWidthInMbs + x ] = 0;
					}

					if( xDir  ==  -1  &&  x  ==  leftBound ) 	{
						leftBound = MAX( leftBound - 1, 0 );
						x = leftBound;
						xDir = 0;
						yDir = 2 * Pps -> slice_group_change_direction_flag - 1;
					} 
					else if( xDir  ==  1  &&  x  ==  rightBound )	{
						rightBound = MIN( rightBound + 1, PicWidthInMbs - 1 );
						x = rightBound;
						xDir = 0;
						yDir = 1 - 2 * Pps -> slice_group_change_direction_flag ;
					} 
					else if( yDir  ==  -1  &&  y  ==  topBound ) {
						topBound = MAX( topBound - 1, 0 );
						y = topBound;
						xDir = 1 - 2 * Pps -> slice_group_change_direction_flag;
						yDir = 0;
					} 
					else if( yDir  ==  1  &&  y  ==  bottomBound ) {
						bottomBound  =  MIN( bottomBound + 1, PicHeightInMapUnits - 1 );
						y = bottomBound;
						xDir = 2 * Pps -> slice_group_change_direction_flag - 1;
						yDir = 0;
					} 
					else	{
						x = x + xDir;
						y = y + yDir;
					}
				}
				break;

			case 4:
				for ( i = 0 ; i < sizeOfUpperLeftGroup ; i++ ) {
					ao_tiMapUnitToSliceGroupMap [i] = Pps -> slice_group_change_direction_flag ;
				}
				for ( ; i < PicSizeInMapUnits ; i++ ) {
					ao_tiMapUnitToSliceGroupMap [i] = 1 - Pps -> slice_group_change_direction_flag ;
				}
				break;

			case 5:
				k = 0;
				for( j = 0; j < PicWidthInMbs; j++ ){
					for( i = 0; i < PicHeightInMapUnits; i++ ){
						if( k++ < sizeOfUpperLeftGroup ){
							ao_tiMapUnitToSliceGroupMap[ i * PicWidthInMbs + j ] = Pps -> slice_group_change_direction_flag;
						}
						else	{
							ao_tiMapUnitToSliceGroupMap[ i * PicWidthInMbs + j ] = 1 - Pps -> slice_group_change_direction_flag;
						}
					}
				}
				break;

			case 6:
				for ( i = 0 ; i < PicSizeInMapUnits ; i++ ) {
					ao_tiMapUnitToSliceGroupMap [i] = ai_Slice_group_id[i] ;
				}
				break;
		}
	}
}

