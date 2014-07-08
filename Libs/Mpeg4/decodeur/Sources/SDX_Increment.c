/*****************************************************************************
   *
   *  MPEG4DSP developped in IETR image lab
   *
   *
   *
   *              Jean-Francois NEZAN <jnezan@insa-rennes.Fr>
   *              Mickael Raulet <mraulet@insa-rennes.Fr>
   *              http://www.ietr.org/gro/IMA/th3/temp/index.htm
   *
   *
   *
   *
   *  This program is free software; you can redistribute it and/or modify
   *  it under the terms of the GNU General Public License as published by
   *  the Free Software Foundation; either version 2 of the License, or
   *  (at your option) any later version.
   *
   *  This program is distributed in the hope that it will be useful,
   *  but WITHOUT ANY WARRANTY; without even the implied warranty of
   *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   *  GNU General Public License for more details.
   *
   *  You should have received a copy of the GNU General Public License
   *  along with this program; if not, write to the Free Software
   *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
   *
   * $Id$
   *
   **************************************************************************/
#include "./include/mpeg.h"

void Increment_I ( struct_VOLsimple *RESTRICT VOLsimple, const int *RESTRICT MB_pos_courant
    , int *RESTRICT MB_pos_suivant, int *RESTRICT pos_X1, int *RESTRICT pos_X2, int *RESTRICT pos_X3
    , int *RESTRICT pos_X4, int *RESTRICT pos_XCb, int *RESTRICT pos_XCr )
{
    int x_pos, y_pos ;
    printf("*MB_pos_courant %d\n",*MB_pos_courant);
    *MB_pos_suivant = *MB_pos_courant ;
    x_pos = *MB_pos_courant % (VOLsimple -> video_object_layer_width / 16);
    y_pos = *MB_pos_courant / (VOLsimple -> video_object_layer_width / 16);
    *pos_X1 = x_pos * 16 + 16 * y_pos * VOLsimple -> video_object_layer_width ;
    *pos_X2 = x_pos * 16 + 8 + 16 * y_pos * VOLsimple -> video_object_layer_width ;
    *pos_X3 = x_pos * 16 + (16 * y_pos + 8) * VOLsimple -> video_object_layer_width ;
    *pos_X4 = x_pos * 16 + 8 + (16 * y_pos + 8) * VOLsimple -> video_object_layer_width ;
    *pos_XCb = x_pos * 8 + 8 * y_pos * VOLsimple -> video_object_layer_width / 2 ;
    *pos_XCr = x_pos * 8 + 8 * y_pos * VOLsimple -> video_object_layer_width / 2 ;
}

void Increment_P ( const int NB_MB, struct_VOLsimple *RESTRICT VOLsimple, const int *RESTRICT MB_pos_courant
    , const vector *RESTRICT current_mv, const vector *RESTRICT save_pmv_prec, vector *RESTRICT save_pmv
    , int *RESTRICT MB_pos_suivant, int *RESTRICT pos_X1, int *RESTRICT pos_X2, int *RESTRICT pos_X3
    , int *RESTRICT pos_X4, int *RESTRICT pos_XCb, int *RESTRICT pos_XCr )
{
    int i, x_pos, y_pos ;
    
    *MB_pos_suivant = *MB_pos_courant ;
    printf("*MB_pos_courant %d\n",*MB_pos_courant);

    x_pos = *MB_pos_courant % (VOLsimple -> video_object_layer_width / 16);
    y_pos = *MB_pos_courant / (VOLsimple -> video_object_layer_width / 16);
    *pos_X1 = x_pos * 16 + 16 * y_pos * VOLsimple -> video_object_layer_width ;
    *pos_X2 = x_pos * 16 + 8 + 16 * y_pos * VOLsimple -> video_object_layer_width ;
    *pos_X3 = x_pos * 16 + (16 * y_pos + 8) * VOLsimple -> video_object_layer_width ;
    *pos_X4 = x_pos * 16 + 8 + (16 * y_pos + 8) * VOLsimple -> video_object_layer_width ;
    *pos_XCb = x_pos * 8 + 8 * y_pos * VOLsimple -> video_object_layer_width / 2 ;
    *pos_XCr = x_pos * 8 + 8 * y_pos * VOLsimple -> video_object_layer_width / 2 ;
    //save motion vector
    memcpy(save_pmv, save_pmv_prec, NB_MB * sizeof(vector));
    for ( i = 0 ; i < 4 ; i++ ) {
        save_pmv [*MB_pos_courant].mvs [i].x = current_mv -> mvs [i].x ;
        save_pmv [*MB_pos_courant].mvs [i].y = current_mv -> mvs [i].y ;
    }
}
