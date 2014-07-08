/*****************************************************************************
   *
   *  MPEG4DSP developped in IETR image lab
   *
   *
   *
   *              Alexis Brumaud <alexis.brumaud@ens.insa-rennes.fr>
   *			  Jean-Francois NEZAN <jnezan@insa-rennes.Fr>
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
/* clause 6.2.7 de la norme, short_video_header nul */
/* on ne traite que les pixels de profondeur ne dépassant pas 8 ... */
/* pour chaque MB on remplit le tableau dct_dc_differential[block_count] */
/* on doit pouvoir traiter les reverse_VLC dans les objets simples */
/* toutes les MB sont codés en intra */
/*#include <math.h>*/
#include "stdio.h"
#include "mpeg.h"
#include "VLCtables.h"

/************************** init_vlc_tables_I **********************************************************/
void init_vlc_tables_I ( REVERSE_EVENT *RESTRICT DCT3D_I )
{
    int i, j ;
    
    for ( i = 0 ; i < 4096 ; i++ ) 
        (*(DCT3D_I + i)).event.level = 0 ;
    for ( i = 0 ; i < 102 ; i++ ) {
        for ( j = 0 ; j < (1 << (12 - coeff_tab [1][i].vlc.len)) ; j++ ) {
            (*(DCT3D_I + (coeff_tab [1][i].vlc.code << (12 - coeff_tab [1][i].vlc.len) | j))).len
                = coeff_tab [1][i].vlc.len ;
            (*(DCT3D_I + (coeff_tab [1][i].vlc.code << (12 - coeff_tab [1][i].vlc.len) | j))).event.last
                = coeff_tab [1][i].event.last ;
            (*(DCT3D_I + (coeff_tab [1][i].vlc.code << (12 - coeff_tab [1][i].vlc.len) | j))).event.level
                = coeff_tab [1][i].event.level ;
            (*(DCT3D_I + (coeff_tab [1][i].vlc.code << (12 - coeff_tab [1][i].vlc.len) | j))).event.run
                = coeff_tab [1][i].event.run ;
        }
    }
}

/************************** init_vlc_tables_P **********************************************************/
void init_vlc_tables_P ( REVERSE_EVENT *RESTRICT DCT3D_P )
{
    int i, j ;
    
    for ( i = 0 ; i < 4096 ; i++ ) 
        (*(DCT3D_P + i)).event.level = 0 ;
    for ( i = 0 ; i < 102 ; i++ ) {
        for ( j = 0 ; j < (1 << (12 - coeff_tab [0][i].vlc.len)) ; j++ ) {
            (*(DCT3D_P + (coeff_tab [0][i].vlc.code << (12 - coeff_tab [0][i].vlc.len) | j))).len
                = coeff_tab [0][i].vlc.len ;
            (*(DCT3D_P + (coeff_tab [1][i].vlc.code << (12 - coeff_tab [0][i].vlc.len) | j))).event.level
                = coeff_tab [0][i].event.level ;
            (*(DCT3D_P + (coeff_tab [1][i].vlc.code << (12 - coeff_tab [0][i].vlc.len) | j))).event.last
                = coeff_tab [0][i].event.last ;
            (*(DCT3D_P + (coeff_tab [1][i].vlc.code << (12 - coeff_tab [0][i].vlc.len) | j))).event.run
                = coeff_tab [0][i].event.run ;
        }
    }
}

/********************************** get_coeff **********************************************************/
__inline int get_coeff ( const REVERSE_EVENT *RESTRICT DCT3D, const unsigned char *RESTRICT bs, int *RESTRICT pos
    , int *RESTRICT run, int *RESTRICT last, const unsigned char *RESTRICT max_run
    , const unsigned char *RESTRICT max_level )
{
#ifdef TI_OPTIM
    
    int             len, level ;
    unsigned int    reverse_event ;
    unsigned int    temp = (int)showNbits(bs, *pos, 13);
    
    if ( temp >> 6 != ESCAPE ) {
        reverse_event = _mem4((void *)(DCT3D + (temp >> 1))); /* chargement d'une structure REVERSE_EVENT dans 32bits*/ 
        len = _extu(reverse_event, 24, 24); /*reverse_event -> len*/ 
        *last = _extu(reverse_event, 16, 24); /*reverse_event -> event.last*/ 
        *run = _extu(reverse_event, 8, 24); /*reverse_event -> event.run*/ 
        level = _extu(reverse_event, 0, 24); /*reverse_event -> event.level*/ 
        *pos += len + 1 ;
        if ( temp << 19 + len >> 31 ) 
            /* lecture du bit donnant le signe du level */
            level = -level ;
        if ( level == 0 ) {
            *run = VLC_ERROR ; /* level = 0 => erreur dans le flux */ 
            level = 0 ;
        }
        return level ;
    }
    *pos += 7 ;
    temp = showNbits(bs, *pos, 15);
    if ( temp >> 14 == 0 ) {
        
        /* first escape mode, level is offset */
        reverse_event = _mem4((void *)(DCT3D + (temp << 18 >> 20)));
        len = _extu(reverse_event, 24, 24); /*reverse_event -> len*/ 
        *last = _extu(reverse_event, 16, 24); /*reverse_event -> event.last*/ 
        *run = _extu(reverse_event, 8, 24); /*reverse_event -> event.run*/ 
        level = _extu(reverse_event, 0, 24); /*reverse_event -> event.level*/ 
        level += *(max_level + *last * 64 + *run);
        *pos += len + 2 ;
        if ( temp << len + 18 >> 31 ) 
            level = -level ;
        if ( level == 0 ) {
            *run = VLC_ERROR ; /* level = 0 => erreur dans le flux */ 
            level = 0 ;
        }
        return level ;
    }
    if ( temp >> 13 == 2 ) {
        
        /* second escape mode, run is offset */
        reverse_event = _mem4((void *)(DCT3D + (temp << 19 >> 20)));
        *last = _extu(reverse_event, 16, 24); /*reverse_event -> event.last*/ 
        *run = _extu(reverse_event, 8, 24); /*reverse_event -> event.run*/ 
        level = _extu(reverse_event, 0, 24); /*reverse_event -> event.level*/ 
        len = _extu(reverse_event, 24, 24); /*reverse_event -> len*/ 
        *run += *(max_run + *last * 64 + level) + 1 ;
        *pos += len + 3 ;
        if ( temp << len + 19 >> 31 ) 
            level = -level ;
        if ( level == 0 ) {
            *run = VLC_ERROR ; /* level = 0 => erreur dans le flux */ 
            level = 0 ;
        }
        return level ;
    }
    
    /* third escape mode - fixed length codes */
    *pos += 2 ;
    temp = showNbits(bs, *pos, 20); //récupère le LAST,RUN,LEVEL après le ESCAPE
    *last = temp >> 19 ;
    *run = _extu(temp, 13, 26);
    *pos += 21 ;
    level = temp ;
    return level << 20 >> 20 ;
#else 
    
    REVERSE_EVENT   *reverse_event ;
    int             result, level ;
    unsigned int    val ;
    unsigned int    temp = (int)showNbits(bs, *pos, 13);
    
    if ( temp >> 6 != ESCAPE ) {
        reverse_event = (REVERSE_EVENT *)(DCT3D + (temp >> 1));
        level = reverse_event -> event.level ;
        *last = reverse_event -> event.last ;
        *run = reverse_event -> event.run ;
        *pos += reverse_event -> len + 1 ;
        result = temp << (19 + reverse_event -> len) >> 31 ? -level : level ;
        if ( level == 0 ) {
            *run = VLC_ERROR ;
            result = 0 ;
        }
        return result ;
    }
    *pos += 7 ;
    temp = showNbits(bs, *pos, 15);
    if ( temp >> 14 == 0 ) {
        reverse_event = (REVERSE_EVENT *)(DCT3D + (temp << 18 >> 20));
        level = reverse_event -> event.level ;
        if ( level == 0 ) {
            *run = VLC_ERROR ;
            return 0 ;
        }
        *last = reverse_event -> event.last ;
        *run = reverse_event -> event.run ;
        *pos += reverse_event -> len + 2 ;
        level += *(max_level + *last * 64 + *run);
        result = temp << (reverse_event -> len + 18) >> 31 ? -level : level ;
        return result ;
    }
    if ( temp >> 13 == 2 ) {
        reverse_event = (REVERSE_EVENT *)(DCT3D + (temp << 19 >> 20));
        if ( (level = reverse_event -> event.level) == 0 ) {
            *run = VLC_ERROR ;
            return 0 ;
        }
        *last = reverse_event -> event.last ;
        *run = reverse_event -> event.run ;
        *pos += reverse_event -> len + 3 ;
        *run += *(max_run + *last * 64 + level) + 1 ;
        result = (temp << (reverse_event -> len + 19)) >> 31 ? -level : level ;
        return result ;
    }
    
    /* third escape mode - fixed length codes */
    *pos += 2 ;
    val = showNbits(bs, *pos, 20); //récupère le LAST,RUN,LEVEL après le ESCAPE
    *last = val >> 19 ;
    *run = val << 13 >> 26 ;
    *pos += 21 ;
    level = val ;
    return level << 20 >> 20 ;
#endif
}

/************************** get_dc_size **********************************************************/
short get_dc_size ( const int block, int *RESTRICT pos, unsigned int code
    , int *RESTRICT incr )
{
    int result;
    
#ifdef TI_OPTIM
    if ( block >> 1 ) {
        
        /* luminance */
        code >>= 1 ;
        result = _norm(code) - 18 ;
        code >>= 8 ;
        if ( result >= 4 ) {
            *incr = result - 1 ;
            *pos += *incr ;
            return result ;
        } else {
            *incr = dc_lum_tab [code].len ;
            *pos += *incr ;
            return dc_lum_tab [code].code ;
        }
    } else {
        
        /* chrominance */
        if ( code >> 11 ) {
            result = _extu(~code, 20, 30);
            *incr = 2 ;
            *pos += *incr ;
            return result ;
        } else {
            result = _norm(code) - 18 ;
            *incr = result ;
            *pos += *incr ;
            return result ;
        }
    }
#else 
    {
        int i,temp ;
        if ( block >> 1 ) {
            
            /* luminance */
            code >>= 1 ;
            temp = code >> 8 ;
            if ( temp ) {
                *incr = dc_lum_tab [temp].len ;
                *pos += *incr ;
                return dc_lum_tab [temp].code ;
            } else {
                for ( i = 11 ; i > 3 && code != 1 ; i-- ) {
                    code >>= 1 ;
                }
                *incr = i ;
                *pos += *incr ;
                return (short)(i + 1) ;
            }
        } else {
            
            /* chrominance */
            if ( code >> 11 ) {
                result = ~code << 20 >> 30 ;
                *incr = 2 ;
                *pos += *incr ;
                return (short)result ;
            }
            temp = code ;
            for ( i = 12 ; i > 2 && temp != 1 ; i-- ) 
                temp >>= 1 ;
            if ( temp == 1 ) {
                *incr = i ;
                *pos += *incr ;
                return (short) i ;
            }
            else
                return (short) 0;
        }
    }
#endif
}

/*********************************** get_coeff_DC *****************************************************/
__inline int get_coeff_DC ( const int block, const unsigned char *RESTRICT bs, int *RESTRICT pos )
{
    unsigned int    code = showNbits(bs, *pos, 24);
    int             incr = 0 ;
    const int       dct_dc_size = get_dc_size(block, pos, code >> 12, &incr);
    
    if ( dct_dc_size == 0 ) 
        return 0 ;
    code = (code << (incr + 8)) >> (32 - dct_dc_size) ;
    *pos += dct_dc_size ;
    if ( code >> (dct_dc_size - 1) ) 
        return code ;
    return -1 * ((int)code ^ (1 << dct_dc_size) - 1);
}

/************************** VLCinverseI **********************************************************/
const unsigned int  alternate_horizontal_tables []
    = { 0, 1, 2, 3, 8, 9, 16, 17, 10, 11, 4, 5, 6, 7, 15, 14, 13, 12, 19, 18, 24, 25, 32, 33, 26, 27, 20, 21, 22, 23
        , 28, 29, 30, 31, 34, 35, 40, 41, 48, 49, 42, 43, 36, 37, 38, 39, 44, 45, 46, 47, 50, 51, 56, 57, 58, 59, 52, 53
        , 54, 55, 60, 61, 62, 63 };
const unsigned int  alternate_vertical_tables []
    = { 0, 8, 16, 24, 1, 9, 2, 10, 17, 25, 32, 40, 48, 56, 57, 49, 41, 33, 26, 18, 3, 11, 4, 12, 19, 27, 34, 42, 50, 58
        , 35, 43, 51, 59, 20, 28, 5, 13, 6, 14, 21, 29, 36, 44, 52, 60, 37, 45, 53, 61, 22, 30, 7, 15, 23, 31, 38, 46
        , 54, 62, 39, 47, 55, 63 };
const unsigned int  zigzag_tables []
    = { 0, 1, 8, 16, 9, 2, 3, 10, 17, 24, 32, 25, 18, 11, 4, 5, 12, 19, 26, 33, 40, 48, 41, 34, 27, 20, 13, 6, 7, 14
        , 21, 28, 35, 42, 49, 56, 57, 50, 43, 36, 29, 22, 15, 23, 30, 37, 44, 51, 58, 59, 52, 45, 38, 31, 39, 46, 53, 60
        , 61, 54, 47, 55, 62, 63 };

/*!
    VLCinverseI processing.
    
    	\param[in] prediction_direction	: prediction direction
    	\param[in] block	 			: block number in macroblock
    	\param[in] DCT3D_I 				: inverse VLC inter coefficients   	
    	\param[in] pos_i	 			: position in bitstream   	
    	\param[in] data 				: bitstream (double buffer output)
    	\param[in] VOP 					: input VOP
    	\param[out] pos_fin_vlc			: position in bitstream after block processing
   \param[out] PQF					: inverse scan output
   */
void VLCinverseI ( const int N, const int posMB, const int pos_i, const unsigned char *RESTRICT data
    , const REVERSE_EVENT *RESTRICT DCT3D_I, const short *RESTRICT BlkA, const short *RESTRICT BlkB
    , const short *RESTRICT BlkC, const struct_VOP *RESTRICT VOP, int *RESTRICT pos_fin_vlc, short *RESTRICT PQF
    , short *RESTRICT QFpred, short *RESTRICT F00pred, short *RESTRICT QPpred, int *RESTRICT prediction_direction )
{
    
    /* mes variables */
    int             i ;
    //int             error_flag = 0 ;
    int             var = 0 ;
    int             last ;
    unsigned int    *scan_table ;
    
    *pos_fin_vlc = pos_i ;
    //alternate_horizontal_scan = 0 ;
    //alternate_vertical_scan = 1 ;
    //zigzag_scan = 2 ;*/
#ifdef TI_OPTIM
    for ( i = 0 ; i < 64 ; i += 4 ) 
        _memd8((void *)(PQF + i)) = 0 ;
#else 
    for ( i = 0 ; i < 64 ; i++ ) 
        PQF [i] = 0 ;
#endif
    if ( N == 0 && posMB == 0 ) {
        scan_table = (unsigned int *)alternate_vertical_tables ;
        *prediction_direction = 1 ;
        *F00pred = BlkA [0];
        *QPpred = (short) VOP -> quant ;
        for ( i = 1 ; i < 8 ; i++ ) 
            QFpred [i - 1] = 0 ;
    } else if ( ABS(BlkA [0] - BlkB [0]) < ABS(BlkB [0] - BlkC [0]) ) {
        scan_table = (unsigned int *)alternate_horizontal_tables ;
        *prediction_direction = 0 ;
        *F00pred = BlkC [0];
        *QPpred = BlkC [15];
        for ( i = 1 ; i < 8 ; i++ ) 
            QFpred [i - 1] = BlkC [7 + i];
    } else {
        scan_table = (unsigned int *)alternate_vertical_tables ;
        *prediction_direction = 1 ;
        *F00pred = BlkA [0];
        *QPpred = BlkA [15];
        for ( i = 1 ; i < 8 ; i++ ) 
            QFpred [i - 1] = BlkA [i];
    }
    if ( VOP -> ac_pred_flag == 0 ) {
        scan_table = (unsigned int *)zigzag_tables ;
        *prediction_direction = 2 ;
    }
    if ( VOP -> prev_quant < VOP -> intra_dc_vlc_thr ) {
        PQF [0] = (short) get_coeff_DC(N, data, pos_fin_vlc);
        var = 1 ;
    }
    
    /* il n'y a pas forcément récupération des coefs AC : cf MoMuSyS */
    if ( VOP -> cbp & 1 << N ) {
        do {
            int         run ;
            const short level = (short) get_coeff(DCT3D_I, data, pos_fin_vlc, &run, &last, max_run_I, max_level_I);
            if ( !level ) {
                //error_flag = 1 ;
                break ;
            }
            var += run ;
            PQF [scan_table [var++]] = level ;
        } while ( !last );
    }
}

/************************** VLCinverseP **********************************************************/
/*!
     VLCinverseP processing.
     
     	\param[in] DCT3D_P 				: inverse VLC intra coefficients   	
     	\param[in] pos_i	 			: position in bitstream   	
     	\param[in] data 				: bitstream (double buffer output)
     	\param[out] pos_fin_vlc			: position in bitstream after block processing
   \param[out] PQF					: inverse scan output
   */
void VLCinverseP ( const REVERSE_EVENT *RESTRICT DCT3D_P, const int pos_i, const unsigned char *RESTRICT data
    , int *RESTRICT pos_fin_vlc, short *RESTRICT PQF )
{
    int i ;
    //int error_flag = 0 ;
    int last ;
    int var = 0 ;
    
#ifdef TI_OPTIM
    for ( i = 0 ; i < 64 ; i += 4 ) 
        _memd8((void *)(PQF + i)) = 0 ;
#else 
    for ( i = 0 ; i < 64 ; i++ ) 
        PQF [i] = 0 ;
#endif
    *pos_fin_vlc = pos_i ;
    do {
        int         run ;
        const short level = (short) get_coeff(DCT3D_P, data, pos_fin_vlc, &run, &last, max_run_P, max_level_P);
        if ( !level ) {
            //error_flag = 1 ;
            break ;
        }
        var += run ;
        PQF [zigzag_tables [var++]] = level ;
    } while ( !last );
}
