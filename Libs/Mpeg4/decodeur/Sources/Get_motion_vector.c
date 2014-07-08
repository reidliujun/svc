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
#include "mpeg.h"


typedef struct {
    short            code ;
    unsigned char   len ;
}   VLC ;

const VLC   TMNMVtab0 []
    = {{ 3, 4 }, { -3, 4 }, { 2, 3 }, { 2, 3 }, { -2, 3 }, { -2, 3 }, { 1, 2 }, { 1, 2 }, { 1, 2 }, { 1, 2 }, { -1, 2 }
        , { -1, 2 }, { -1, 2 }, { -1, 2 }};
const VLC   TMNMVtab1 []
    = {{ 12, 10 }, { -12, 10 }, { 11, 10 }, { -11, 10 }, { 10, 9 }, { 10, 9 }, { -10, 9 }, { -10, 9 }, { 9, 9 }
        , { 9, 9 }, { -9, 9 }, { -9, 9 }, { 8, 9 }, { 8, 9 }, { -8, 9 }, { -8, 9 }, { 7, 7 }, { 7, 7 }, { 7, 7 }
        , { 7, 7 }, { 7, 7 }, { 7, 7 }, { 7, 7 }, { 7, 7 }, { -7, 7 }, { -7, 7 }, { -7, 7 }, { -7, 7 }, { -7, 7 }
        , { -7, 7 }, { -7, 7 }, { -7, 7 }, { 6, 7 }, { 6, 7 }, { 6, 7 }, { 6, 7 }, { 6, 7 }, { 6, 7 }, { 6, 7 }
        , { 6, 7 }, { -6, 7 }, { -6, 7 }, { -6, 7 }, { -6, 7 }, { -6, 7 }, { -6, 7 }, { -6, 7 }, { -6, 7 }, { 5, 7 }
        , { 5, 7 }, { 5, 7 }, { 5, 7 }, { 5, 7 }, { 5, 7 }, { 5, 7 }, { 5, 7 }, { -5, 7 }, { -5, 7 }, { -5, 7 }
        , { -5, 7 }, { -5, 7 }, { -5, 7 }, { -5, 7 }, { -5, 7 }, { 4, 6 }, { 4, 6 }, { 4, 6 }, { 4, 6 }, { 4, 6 }
        , { 4, 6 }, { 4, 6 }, { 4, 6 }, { 4, 6 }, { 4, 6 }, { 4, 6 }, { 4, 6 }, { 4, 6 }, { 4, 6 }, { 4, 6 }, { 4, 6 }
        , { -4, 6 }, { -4, 6 }, { -4, 6 }, { -4, 6 }, { -4, 6 }, { -4, 6 }, { -4, 6 }, { -4, 6 }, { -4, 6 }, { -4, 6 }
        , { -4, 6 }, { -4, 6 }, { -4, 6 }, { -4, 6 }, { -4, 6 }, { -4, 6 }};
const VLC   TMNMVtab2 []
    = {{ 32, 12 }, { -32, 12 }, { 31, 12 }, { -31, 12 }, { 30, 11 }, { 30, 11 }, { -30, 11 }, { -30, 11 }, { 29, 11 }
        , { 29, 11 }, { -29, 11 }, { -29, 11 }, { 28, 11 }, { 28, 11 }, { -28, 11 }, { -28, 11 }, { 27, 11 }, { 27, 11 }
        , { -27, 11 }, { -27, 11 }, { 26, 11 }, { 26, 11 }, { -26, 11 }, { -26, 11 }, { 25, 11 }, { 25, 11 }
        , { -25, 11 }, { -25, 11 }, { 24, 10 }, { 24, 10 }, { 24, 10 }, { 24, 10 }, { -24, 10 }, { -24, 10 }
        , { -24, 10 }, { -24, 10 }, { 23, 10 }, { 23, 10 }, { 23, 10 }, { 23, 10 }, { -23, 10 }, { -23, 10 }
        , { -23, 10 }, { -23, 10 }, { 22, 10 }, { 22, 10 }, { 22, 10 }, { 22, 10 }, { -22, 10 }, { -22, 10 }
        , { -22, 10 }, { -22, 10 }, { 21, 10 }, { 21, 10 }, { 21, 10 }, { 21, 10 }, { -21, 10 }, { -21, 10 }
        , { -21, 10 }, { -21, 10 }, { 20, 10 }, { 20, 10 }, { 20, 10 }, { 20, 10 }, { -20, 10 }, { -20, 10 }
        , { -20, 10 }, { -20, 10 }, { 19, 10 }, { 19, 10 }, { 19, 10 }, { 19, 10 }, { -19, 10 }, { -19, 10 }
        , { -19, 10 }, { -19, 10 }, { 18, 10 }, { 18, 10 }, { 18, 10 }, { 18, 10 }, { -18, 10 }, { -18, 10 }
        , { -18, 10 }, { -18, 10 }, { 17, 10 }, { 17, 10 }, { 17, 10 }, { 17, 10 }, { -17, 10 }, { -17, 10 }
        , { -17, 10 }, { -17, 10 }, { 16, 10 }, { 16, 10 }, { 16, 10 }, { 16, 10 }, { -16, 10 }, { -16, 10 }
        , { -16, 10 }, { -16, 10 }, { 15, 10 }, { 15, 10 }, { 15, 10 }, { 15, 10 }, { -15, 10 }, { -15, 10 }
        , { -15, 10 }, { -15, 10 }, { 14, 10 }, { 14, 10 }, { 14, 10 }, { 14, 10 }, { -14, 10 }, { -14, 10 }
        , { -14, 10 }, { -14, 10 }, { 13, 10 }, { 13, 10 }, { 13, 10 }, { 13, 10 }, { -13, 10 }, { -13, 10 }
        , { -13, 10 }, { -13, 10 }};

__inline int get_mv_data ( int *RESTRICT pos, const unsigned int code_lu, int *RESTRICT incr )
{
    int index ;
    
    *pos += 1 ;
    if ( code_lu >> 12 ) 
        /*lecture et test du premier bit*/
        return 0 ;
    index = code_lu ; /* si premier bit nul alors index = code_lu */ 
    if ( index >> 9 ) {
        
        /* index >= 512*/
        index = (index >> 8) - 2 ;
        *incr = TMNMVtab0 [index].len ;
        return TMNMVtab0 [index].code ;
    }
    if ( index >> 7 ) {
        
        /* index >= 128*/
        index = (index >> 2) - 32 ;
        *incr = TMNMVtab1 [index].len ;
        return TMNMVtab1 [index].code ;
    }
    
    /* index < 128 */
    index -= 4 ;
    *incr = TMNMVtab2 [index].len ;
    return TMNMVtab2 [index].code ;
}

__inline int get_mv ( const unsigned char *RESTRICT bs, int *RESTRICT pos, const int fcode )
{
    int             data ;
    int             incr = 0 ;
    int             res ;
    int             mv ;
    const int       temp = fcode - 1 ;
    int             scale_fac = 1 << temp ;
    unsigned int    code_lu = showNbits(bs, *pos, 24);  /* lecture de l'index (13 bits) et de res (max 11 bits)*/ 
    
    data = get_mv_data(pos, code_lu >> 11, &incr); /* récupération de la donnée et de la longueur*/ 
    *pos += incr ;
    if ( !temp ) 
        return data ;
    if ( !data ) 
        return 0 ;
    mv = (ABS(data) - 1) * scale_fac + 1 ;
    res = code_lu << (9 + incr) >> (32 - temp) ; /* lecture de res de longueur fcode - 1 */ 
    *pos += temp ; /* mise à jour de la position du pointeur dans le flux */ 
    mv += res ;
    return data < 0 ? -mv : mv ;
}

/*****************************************************************************
   * Inlined functions
   ****************************************************************************/
/*
   * MODE_INTER, vm18 page 48
   * MODE_INTER4V vm18 page 51
   *
   *   (x,y-1)      (x+1,y-1)
   *   [   |   ]    [   |   ]
   *   [ 2 | 3 ]    [ 2 |   ]
   *
   *   (x-1,y)       (x,y)        (x+1,y)
   *   [   | 1 ]    [ 0 | 1 ]    [ 0 |   ]
   *   [   | 3 ]    [ 2 | 3 ]    [   |   ]
   */
VECTOR get_pmv2 ( const vector *RESTRICT save_vector, const int mb_width, const int bound, const int x, const int y
    , const int block )
{
    static const VECTOR zeroMV = { 0, 0 };
    int                 lx, ly, lz ; /* left */ 
    int                 tx, ty, tz ; /* top */ 
    int                 rx, ry, rz ; /* top-right */ 
    int                 lpos, tpos, rpos ;
    int                 num_cand, last_cand ;
    VECTOR              pmv [4];     /* left neighbour, top neighbour, top-right neighbour */ 
    
    switch ( block ) {
        case 0 : 
            lx = x - 1 ;
            ly = y ;
            lz = 1 ;
            tx = x ;
            ty = y - 1 ;
            tz = 2 ;
            rx = x + 1 ;
            ry = y - 1 ;
            rz = 2 ;
            break ;
        case 1 : 
            lx = x ;
            ly = y ;
            lz = 0 ;
            tx = x ;
            ty = y - 1 ;
            tz = 3 ;
            rx = x + 1 ;
            ry = y - 1 ;
            rz = 2 ;
            break ;
        case 2 : 
            lx = x - 1 ;
            ly = y ;
            lz = 3 ;
            tx = x ;
            ty = y ;
            tz = 0 ;
            rx = x ;
            ry = y ;
            rz = 1 ;
            break ;
        default : 
            lx = x ;
            ly = y ;
            lz = 2 ;
            tx = x ;
            ty = y ;
            tz = 0 ;
            rx = x ;
            ry = y ;
            rz = 1 ;
    }
    lpos = lx + ly * mb_width ;
    rpos = rx + ry * mb_width ;
    tpos = tx + ty * mb_width ;
    last_cand = num_cand = 0 ;
    if ( lpos >= bound && lx >= 0 ) {
        num_cand++ ;
        last_cand = 1 ;
        pmv [1] = save_vector [lpos].mvs [lz];
    } else {
        pmv [1] = zeroMV ;
    }
    if ( tpos >= bound ) {
        num_cand++ ;
        last_cand = 2 ;
        pmv [2] = save_vector [tpos].mvs [tz];
    } else {
        pmv [2] = zeroMV ;
    }
    if ( rpos >= bound && rx < mb_width ) {
        num_cand++ ;
        last_cand = 3 ;
        pmv [3] = save_vector [rpos].mvs [rz];
    } else {
        pmv [3] = zeroMV ;
    }
    /*
       * If there're more than one candidate, we return the median vector
       * edgomez : the special case "no candidates" is handled the same way
       *           because all vectors are set to zero. So the median vector
       *           is {0,0}, and this is exactly the vector we must return
       *           according to the mpeg4 specs.
       */
    if ( num_cand != 1 ) {
        
        /* set median */
        pmv [0].x = MIN(MAX(pmv [1].x, pmv [2].x), MIN(MAX(pmv [2].x, pmv [3].x), MAX(pmv [1].x, pmv [3].x)));
        pmv [0].y = MIN(MAX(pmv [1].y, pmv [2].y), MIN(MAX(pmv [2].y, pmv [3].y), MAX(pmv [1].y, pmv [3].y)));
        return pmv [0];
    }
    return pmv [last_cand]; /* no point calculating median mv */ 
}

void get_motion_vector ( const int k, const vector *RESTRICT save_vector, const struct_VOLsimple *RESTRICT VOLsimple
    , const unsigned char *RESTRICT bs, const int pos_i, const int *RESTRICT MBpos, const struct_VOP *RESTRICT VOP
    , vector *RESTRICT current_mv, int *RESTRICT mvpos )
{
    int     fcode = VOP -> fcode_forward ;
    int     bound = VOP -> bound ;
    int     scale_fac = 1 << (fcode - 1) ;
    int     high = 32 * scale_fac - 1 ;
    int     low = -32 * scale_fac ;
    int     range = 64 * scale_fac ;
    VECTOR  pmv ;
    int     mv_x, mv_y ;
    
    *mvpos = pos_i ;
    pmv = get_pmv2(save_vector, VOLsimple -> video_object_layer_width / 16, bound
        , *MBpos % (VOLsimple -> video_object_layer_width / 16), *MBpos / (VOLsimple -> video_object_layer_width / 16)
        , k);
    mv_x = get_mv(bs, mvpos, fcode);
    mv_y = get_mv(bs, mvpos, fcode);
    mv_x += pmv.x ;
    mv_y += pmv.y ;
    if ( mv_x < low ) {
        mv_x += range ;
    } else if ( mv_x > high ) {
        mv_x -= range ;
    }
    if ( mv_y < low ) {
        mv_y += range ;
    } else if ( mv_y > high ) {
        mv_y -= range ;
    }
    current_mv -> mvs [k].x = mv_x ;
    current_mv -> mvs [k].y = mv_y ;
}

/* decode B-frame motion vector */
void get_b_motion_vector ( const unsigned char VOP_fcode, const VECTOR *RESTRICT pmv, const unsigned char *RESTRICT bs
    , const int pos_i, VECTOR *RESTRICT mv, int *RESTRICT mvpos )
{
    int fcode = VOP_fcode ;
    int scale_fac = 1 << (fcode - 1) ;
    int high = 32 * scale_fac - 1 ;
    int low = -32 * scale_fac ;
    int range = 64 * scale_fac ;
    int mv_x, mv_y ;
    
    *mvpos = pos_i ;
    mv_x = get_mv(bs, mvpos, fcode);
    mv_y = get_mv(bs, mvpos, fcode);
    mv_x += pmv -> x ;
    mv_y += pmv -> y ;
    if ( mv_x < low ) {
        mv_x += range ;
    } else if ( mv_x > high ) {
        mv_x -= range ;
    }
    if ( mv_y < low ) {
        mv_y += range ;
    } else if ( mv_y > high ) {
        mv_y -= range ;
    }
    mv -> x = mv_x ;
    mv -> y = mv_y ;
}
