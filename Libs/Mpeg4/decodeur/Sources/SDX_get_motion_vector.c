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
   *  Based on the XviD MPEG-4 video codec
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

void                        get_motion_vector (const int k, const vector *RESTRICT save_vector
    , const struct_VOLsimple *RESTRICT VOLsimple, const unsigned char *RESTRICT bs, const int pos_i
    , const int *RESTRICT MBpos, const struct_VOP *RESTRICT VOP, vector *RESTRICT current_mv, int *RESTRICT mvpos) ;
static const unsigned int   roundtab [16] = { 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2 };

void SDX_get_motion_vector ( const int mb_not_coded, const int mb_type, vector *RESTRICT save_mv
    , const struct_VOLsimple *RESTRICT VOLsimple, const unsigned char *RESTRICT data, const int pos
    , const int *RESTRICT MBpos, const struct_VOP *RESTRICT VOP, vector *RESTRICT current_mv, int *RESTRICT uv_dx
    , int *RESTRICT uv_dy, int *RESTRICT mvpos )
{
    int pos1 [1];
    int pos2 [1];
    int sum ;
    
    *mvpos = pos ;
    pos1 [0] = pos ;
    if ( !mb_not_coded ) {
        if ( mb_type == 0 || mb_type == 1 ) {
            
            // INTER, INTER_Q
            get_motion_vector(0, save_mv, VOLsimple, data, pos1 [0], MBpos, VOP, current_mv, mvpos);
            current_mv -> mvs [1].x = current_mv -> mvs [2].x = current_mv -> mvs [3].x = current_mv -> mvs [0].x ;
            current_mv -> mvs [1].y = current_mv -> mvs [2].y = current_mv -> mvs [3].y = current_mv -> mvs [0].y ;
            *uv_dx = current_mv -> mvs [0].x ;
            *uv_dy = current_mv -> mvs [0].y ;
            *uv_dx = *uv_dx & 3 ? *uv_dx >> 1 | 1 : *uv_dx / 2 ;
            *uv_dy = *uv_dy & 3 ? *uv_dy >> 1 | 1 : *uv_dy / 2 ;
        } else if ( mb_type == 2 ) {
            
            // INTER4V
            get_motion_vector(0, save_mv, VOLsimple, data, pos1 [0], MBpos, VOP, current_mv, pos2);
            //save motion vector
            save_mv [*MBpos].mvs [0].x = current_mv -> mvs [0].x ;
            save_mv [*MBpos].mvs [0].y = current_mv -> mvs [0].y ;
            get_motion_vector(1, save_mv, VOLsimple, data, pos2 [0], MBpos, VOP, current_mv, pos1);
            //save motion vector
            save_mv [*MBpos].mvs [1].x = current_mv -> mvs [1].x ;
            save_mv [*MBpos].mvs [1].y = current_mv -> mvs [1].y ;
            get_motion_vector(2, save_mv, VOLsimple, data, pos1 [0], MBpos, VOP, current_mv, pos2);
            //save motion vector
            save_mv [*MBpos].mvs [2].x = current_mv -> mvs [2].x ;
            save_mv [*MBpos].mvs [2].y = current_mv -> mvs [2].y ;
            get_motion_vector(3, save_mv, VOLsimple, data, pos2 [0], MBpos, VOP, current_mv, mvpos);
            sum = current_mv -> mvs [0].x + current_mv -> mvs [1].x + current_mv -> mvs [2].x + current_mv -> mvs [3].x ;
            *uv_dx = sum == 0 ? 0 : SIGN(sum) * (roundtab [ABS(sum) % 16] + ABS(sum) / 16 * 2);
            sum = current_mv -> mvs [0].y + current_mv -> mvs [1].y + current_mv -> mvs [2].y + current_mv -> mvs [3].y ;
            *uv_dy = sum == 0 ? 0 : SIGN(sum) * (roundtab [ABS(sum) % 16] + ABS(sum) / 16 * 2);
        } else {
            
            // INTRA, INTRAQ
            current_mv -> mvs [0].x = current_mv -> mvs [1].x = current_mv -> mvs [2].x = current_mv -> mvs [3].x = 0 ;
            current_mv -> mvs [0].y = current_mv -> mvs [1].y = current_mv -> mvs [2].y = current_mv -> mvs [3].y = 0 ;
        }
    } else {
        
        // NOT CODED
        current_mv -> mvs [0].x = current_mv -> mvs [1].x = current_mv -> mvs [2].x = current_mv -> mvs [3].x = 0 ;
        current_mv -> mvs [0].y = current_mv -> mvs [1].y = current_mv -> mvs [2].y = current_mv -> mvs [3].y = 0 ;
    }
}
