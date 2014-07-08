/*****************************************************************************
   *
   *  MPEG4DSP developped in IETR image lab
   *
   *
   *
   *            Copyright (C) 2005  Jean-Francois NEZAN <jnezan@insa-rennes.Fr>
   *            Copyright (C) 2005 Mickael Raulet <mraulet@insa-rennes.Fr>
   *              http://www.ietr.org/gro/IMA/th3/temp/index.htm
   *
   *  Based on the XviD MPEG-4 video codec
   *
   *			Copyright (C) 2002 Michael Militzer <isibaar@xvid.org>
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
/* ce module est la fonction macroblock() de la norme clause 6.2.6, dans le cas d'objets simples images I */
/* a noter que mb_type=5 dans le cas d'un mb de type stuffing */
/* pas pour des short_video_header */
/* a noter que l'on considère uniquement le cas de 4 blocs non-transparents, c'est logique pour des MB de 
   type mais je n'arrive pas a localiser la variable spécifiant si il y a 1 2 3 ou 4 blocs non-transparents dans le MB */
/* on fait le calcul de la valeur de Qp de la table 6.21 dans le cas ou on a la valeur du parametre de quantif DCT qui ne varie qu'avec les parametres "vop_quant" et "dquant" */
/* la variable "coef_dct" pourra etre re_utilisée dans la quantification ... */
#include <stdio.h>
#include "mpeg.h"

typedef struct {
    short            code ;
    unsigned char   len ;
}   VLC ;

#define VLC_ERROR -1

const VLC       mcbpc_intra_table [64]
    = {{ -1, 0 }, { 20, 6 }, { 36, 6 }, { 52, 6 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 19, 3 }, { 19, 3 }
        , { 19, 3 }, { 19, 3 }, { 19, 3 }, { 19, 3 }, { 19, 3 }, { 19, 3 }, { 35, 3 }, { 35, 3 }, { 35, 3 }, { 35, 3 }
        , { 35, 3 }, { 35, 3 }, { 35, 3 }, { 35, 3 }, { 51, 3 }, { 51, 3 }, { 51, 3 }, { 51, 3 }, { 51, 3 }, { 51, 3 }
        , { 51, 3 }, { 51, 3 }, { 3, 1 }, { 3, 1 }, { 3, 1 }, { 3, 1 }, { 3, 1 }, { 3, 1 }, { 3, 1 }, { 3, 1 }, { 3, 1 }
        , { 3, 1 }, { 3, 1 }, { 3, 1 }, { 3, 1 }, { 3, 1 }, { 3, 1 }, { 3, 1 }, { 3, 1 }, { 3, 1 }, { 3, 1 }, { 3, 1 }
        , { 3, 1 }, { 3, 1 }, { 3, 1 }, { 3, 1 }, { 3, 1 }, { 3, 1 }, { 3, 1 }, { 3, 1 }, { 3, 1 }, { 3, 1 }, { 3, 1 }
        , { 3, 1 }};
const VLC       cbpy_table [64]
    = {{ -1, 0 }, { -1, 0 }, { 6, 6 }, { 9, 6 }, { 8, 5 }, { 8, 5 }, { 4, 5 }, { 4, 5 }, { 2, 5 }, { 2, 5 }, { 1, 5 }
        , { 1, 5 }, { 0, 4 }, { 0, 4 }, { 0, 4 }, { 0, 4 }, { 12, 4 }, { 12, 4 }, { 12, 4 }, { 12, 4 }, { 10, 4 }
        , { 10, 4 }, { 10, 4 }, { 10, 4 }, { 14, 4 }, { 14, 4 }, { 14, 4 }, { 14, 4 }, { 5, 4 }, { 5, 4 }, { 5, 4 }
        , { 5, 4 }, { 13, 4 }, { 13, 4 }, { 13, 4 }, { 13, 4 }, { 3, 4 }, { 3, 4 }, { 3, 4 }, { 3, 4 }, { 11, 4 }
        , { 11, 4 }, { 11, 4 }, { 11, 4 }, { 7, 4 }, { 7, 4 }, { 7, 4 }, { 7, 4 }, { 15, 2 }, { 15, 2 }, { 15, 2 }
        , { 15, 2 }, { 15, 2 }, { 15, 2 }, { 15, 2 }, { 15, 2 }, { 15, 2 }, { 15, 2 }, { 15, 2 }, { 15, 2 }, { 15, 2 }
        , { 15, 2 }, { 15, 2 }, { 15, 2 }};
const VLC       mcbpc_inter_table [257]
    = {{ VLC_ERROR, 0 }, { 255, 9 }, { 52, 9 }, { 36, 9 }, { 20, 9 }, { 49, 9 }, { 35, 8 }, { 35, 8 }, { 19, 8 }
        , { 19, 8 }, { 50, 8 }, { 50, 8 }, { 51, 7 }, { 51, 7 }, { 51, 7 }, { 51, 7 }, { 34, 7 }, { 34, 7 }, { 34, 7 }
        , { 34, 7 }, { 18, 7 }, { 18, 7 }, { 18, 7 }, { 18, 7 }, { 33, 7 }, { 33, 7 }, { 33, 7 }, { 33, 7 }, { 17, 7 }
        , { 17, 7 }, { 17, 7 }, { 17, 7 }, { 4, 6 }, { 4, 6 }, { 4, 6 }, { 4, 6 }, { 4, 6 }, { 4, 6 }, { 4, 6 }
        , { 4, 6 }, { 48, 6 }, { 48, 6 }, { 48, 6 }, { 48, 6 }, { 48, 6 }, { 48, 6 }, { 48, 6 }, { 48, 6 }, { 3, 5 }
        , { 3, 5 }, { 3, 5 }, { 3, 5 }, { 3, 5 }, { 3, 5 }, { 3, 5 }, { 3, 5 }, { 3, 5 }, { 3, 5 }, { 3, 5 }, { 3, 5 }
        , { 3, 5 }, { 3, 5 }, { 3, 5 }, { 3, 5 }, { 32, 4 }, { 32, 4 }, { 32, 4 }, { 32, 4 }, { 32, 4 }, { 32, 4 }
        , { 32, 4 }, { 32, 4 }, { 32, 4 }, { 32, 4 }, { 32, 4 }, { 32, 4 }, { 32, 4 }, { 32, 4 }, { 32, 4 }, { 32, 4 }
        , { 32, 4 }, { 32, 4 }, { 32, 4 }, { 32, 4 }, { 32, 4 }, { 32, 4 }, { 32, 4 }, { 32, 4 }, { 32, 4 }, { 32, 4 }
        , { 32, 4 }, { 32, 4 }, { 32, 4 }, { 32, 4 }, { 32, 4 }, { 32, 4 }, { 16, 4 }, { 16, 4 }, { 16, 4 }, { 16, 4 }
        , { 16, 4 }, { 16, 4 }, { 16, 4 }, { 16, 4 }, { 16, 4 }, { 16, 4 }, { 16, 4 }, { 16, 4 }, { 16, 4 }, { 16, 4 }
        , { 16, 4 }, { 16, 4 }, { 16, 4 }, { 16, 4 }, { 16, 4 }, { 16, 4 }, { 16, 4 }, { 16, 4 }, { 16, 4 }, { 16, 4 }
        , { 16, 4 }, { 16, 4 }, { 16, 4 }, { 16, 4 }, { 16, 4 }, { 16, 4 }, { 16, 4 }, { 16, 4 }, { 2, 3 }, { 2, 3 }
        , { 2, 3 }, { 2, 3 }, { 2, 3 }, { 2, 3 }, { 2, 3 }, { 2, 3 }, { 2, 3 }, { 2, 3 }, { 2, 3 }, { 2, 3 }, { 2, 3 }
        , { 2, 3 }, { 2, 3 }, { 2, 3 }, { 2, 3 }, { 2, 3 }, { 2, 3 }, { 2, 3 }, { 2, 3 }, { 2, 3 }, { 2, 3 }, { 2, 3 }
        , { 2, 3 }, { 2, 3 }, { 2, 3 }, { 2, 3 }, { 2, 3 }, { 2, 3 }, { 2, 3 }, { 2, 3 }, { 2, 3 }, { 2, 3 }, { 2, 3 }
        , { 2, 3 }, { 2, 3 }, { 2, 3 }, { 2, 3 }, { 2, 3 }, { 2, 3 }, { 2, 3 }, { 2, 3 }, { 2, 3 }, { 2, 3 }, { 2, 3 }
        , { 2, 3 }, { 2, 3 }, { 2, 3 }, { 2, 3 }, { 2, 3 }, { 2, 3 }, { 2, 3 }, { 2, 3 }, { 2, 3 }, { 2, 3 }, { 2, 3 }
        , { 2, 3 }, { 2, 3 }, { 2, 3 }, { 2, 3 }, { 2, 3 }, { 2, 3 }, { 2, 3 }, { 1, 3 }, { 1, 3 }, { 1, 3 }, { 1, 3 }
        , { 1, 3 }, { 1, 3 }, { 1, 3 }, { 1, 3 }, { 1, 3 }, { 1, 3 }, { 1, 3 }, { 1, 3 }, { 1, 3 }, { 1, 3 }, { 1, 3 }
        , { 1, 3 }, { 1, 3 }, { 1, 3 }, { 1, 3 }, { 1, 3 }, { 1, 3 }, { 1, 3 }, { 1, 3 }, { 1, 3 }, { 1, 3 }, { 1, 3 }
        , { 1, 3 }, { 1, 3 }, { 1, 3 }, { 1, 3 }, { 1, 3 }, { 1, 3 }, { 1, 3 }, { 1, 3 }, { 1, 3 }, { 1, 3 }, { 1, 3 }
        , { 1, 3 }, { 1, 3 }, { 1, 3 }, { 1, 3 }, { 1, 3 }, { 1, 3 }, { 1, 3 }, { 1, 3 }, { 1, 3 }, { 1, 3 }, { 1, 3 }
        , { 1, 3 }, { 1, 3 }, { 1, 3 }, { 1, 3 }, { 1, 3 }, { 1, 3 }, { 1, 3 }, { 1, 3 }, { 1, 3 }, { 1, 3 }, { 1, 3 }
        , { 1, 3 }, { 1, 3 }, { 1, 3 }, { 1, 3 }, { 1, 3 }, { 0, 1 }};
const int       dquant_table [4] = { -1, -2, 1, 2 };
__inline int    get_B_mbtype (const unsigned int temp, int *RESTRICT incr) ;
__inline int    get_dbquant (const unsigned int temp, int *RESTRICT incr) ;

void Param_MB_I ( int pos_i, const unsigned char *RESTRICT data, const struct_VOP *RESTRICT VOP
    , struct_VOP *RESTRICT NewVOP, int *RESTRICT pos_o )
{
    
    /* mes variables */
    int             cbpc ;
    int             mcbpc ;
    int             quant ;
    int             cbpy ;
    unsigned int    temp ;
    int             incr = 0 ;
    
    /*sauvegarde des entrées*/
    *NewVOP = *VOP ;
    quant = VOP -> quant ;
    *pos_o = pos_i ;
    
    /* lecture du flux */
    temp = showNbits(data, *pos_o, 19);
    
    /* recup de mcbpc tableau B-6 de la norme */
    mcbpc = mcbpc_intra_table [temp >> 13].code ;
    incr = mcbpc_intra_table [temp >> 13].len ;
    NewVOP -> mb_type = mcbpc & 7 ;
    cbpc = mcbpc >> 4 ;
    NewVOP -> ac_pred_flag = (temp << (13 + incr)) >> 31 ; /*1*/ 
    
    /* recup de cbpy */
    cbpy = cbpy_table [(temp << (14 + incr)) >> 26].code ;
    incr += cbpy_table [(temp << (14 + incr)) >> 26].len ;
    NewVOP -> cbp = cbpy << 2 | cbpc ;
    NewVOP -> prev_quant = quant ;
    
    /* incrémentation de la position dans le flux */
    *pos_o += incr + 1 ;
    if ( NewVOP -> mb_type == MODE_INTRA_Q ) {
        
        /* MB intra_Q */
        quant += dquant_table [(temp << (14 + incr)) >> 30]; /*2*/ 
        *pos_o += 2 ;
        if ( quant >> 5 ) 
            /* quant > 31 */
            quant = 31 ;
        if ( quant < 1 ) 
            quant = 1 ;
    }
    NewVOP -> quant = quant ;
}

void Param_MB_P ( int pos_i, const unsigned char *RESTRICT data, const struct_VOP *RESTRICT VOP
    , const int *RESTRICT MB_pos, struct_VOP *RESTRICT NewVOP, int *RESTRICT pos_o, int *RESTRICT Mb_not_coded
    , int *RESTRICT Mb_type, unsigned char *RESTRICT stock_mb_type_P )
{
    
    /* mes variables */
    int             cbpc ;
    int             mcbpc ;
    int             quant ;
    int             cbpy ;
    unsigned int    temp ;
    int             index, incr = 0 ;
    
    /*sauvegarde des entrées*/
    *NewVOP = *VOP ;
    quant = VOP -> quant ;
    *pos_o = pos_i ;
    
    /* lecture du flux */
    temp = showNbits(data, *pos_o, 19);
    NewVOP -> not_coded = temp >> 18 ; /*1*/ 
    *pos_o += 1 ;
    if ( !(temp >> 18) ) 
    /* coded */
    {
        unsigned int    intra ;
        
        /* recup de mcbpc tableau B-6 de la norme */
        if ( (index = temp << 14 >> 23) >> 8 ) 
            /* si index > 256 alors index = 256*/
            index = 256 ;
        mcbpc = mcbpc_inter_table [index].code ;
        incr += mcbpc_inter_table [index].len ;
        NewVOP -> mb_type = mcbpc & 7 ;
        cbpc = mcbpc >> 4 ;
        NewVOP -> ac_pred_flag = 0 ;
        intra = NewVOP -> mb_type == MODE_INTRA || NewVOP -> mb_type == MODE_INTRA_Q ;
        if ( intra ) {
            NewVOP -> ac_pred_flag = (temp << (14 + incr)) >> 31 ;
            cbpy = cbpy_table [(temp << (15 + incr)) >> 26].code ;
            incr += 1 + cbpy_table [(temp << (15 + incr)) >> 26].len ;
        } else {
            cbpy = cbpy_table [(temp << (14 + incr)) >> 26].code ;
            cbpy = 15 - cbpy ;
            incr += cbpy_table [(temp << (14 + incr)) >> 26].len ;
        }
        NewVOP -> cbp = cbpy << 2 | cbpc ;
        
        /* store the previous macrobloc quantification */
        NewVOP -> prev_quant = quant ;
        if ( NewVOP -> mb_type == MODE_INTER_Q || NewVOP -> mb_type == MODE_INTRA_Q ) {
            quant += dquant_table [(temp << (14 + incr)) >> 30]; /*2*/ 
            incr += 2 ;
            if ( quant >> 5 ) {
                
                /* quant > 31*/
                quant = 31 ;
            } else if ( quant < 1 ) {
                quant = 1 ;
            }
        }
        NewVOP -> quant = quant ;
        *pos_o += incr ;
        
        /* for B-frames */
        stock_mb_type_P [*MB_pos] = (unsigned char) NewVOP -> mb_type ;
    }
    else 
    /* not_coded */
    {
        
        /* for B-frames */
        stock_mb_type_P [*MB_pos] = MODE_NOT_CODED ;
    }
    *Mb_not_coded = (int)(NewVOP -> not_coded);
    *Mb_type = (int)(NewVOP -> mb_type);
}

void Param_MB_B ( int pos_i, const unsigned char *RESTRICT data, const struct_VOP *RESTRICT VOP
    , const int *RESTRICT MB_pos, const unsigned char *RESTRICT stock_mb_type_P, struct_VOP *RESTRICT NewVOP
    , int *RESTRICT pos_o )
{
    
    /* mes variables */
    int             quant ;
    int             modb1 ;
    int             modb2 ;
    unsigned int    temp ;
    int             incr = 0 ;
    
    /*sauvegarde des entrées*/
    *NewVOP = *VOP ;
    quant = VOP -> quant ;
    *pos_o = pos_i ;
    /*lecture du flux*/
    temp = showNbits(data, *pos_o, 16);
    if ( stock_mb_type_P [*MB_pos] == MODE_NOT_CODED ) {
        NewVOP -> cbp = 0 ;
        NewVOP -> mb_type = MODE_FORWARD ;
    } else {
        modb1 = temp >> 15 ;
        *pos_o += 1 ;
        if ( !modb1 ) 
        /* modb =='0' */
        {
            modb2 = temp << 17 >> 31 ;
            *pos_o += 1 ;
            NewVOP -> mb_type = get_B_mbtype(temp, &incr);
            if ( !modb2 ) 
            /* modb =='00' */
            {
                NewVOP -> cbp = (temp << (18 + incr)) >> 26 ;
                incr += 6 ;
            }
            else 
            {
                NewVOP -> cbp = 0 ;
            }
            
            /* store the previous macrobloc quantification */
            NewVOP -> prev_quant = quant ;
            if ( NewVOP -> mb_type && NewVOP -> cbp ) {
                quant += get_dbquant(temp, &incr);
                if ( quant > 31 ) 
                    quant = 31 ;
                else if ( quant < 1 ) 
                    quant = 1 ;
            }
            NewVOP -> quant = quant ;
        }
        else 
        {
            NewVOP -> mb_type = MODE_DIRECT_NONE_MV ;
            NewVOP -> cbp = 0 ;
        }
    }
    *pos_o += incr ;
}

/* for decode B-frame dbquant */
__inline int get_dbquant ( const unsigned int temp, int *RESTRICT incr )
{
    int   dbquant_bit1 ;
    int   dbquant_bit2 ;
    
    dbquant_bit1 = (temp << (18 + *incr)) >> 31 ;
    *incr += 1 ;
    if ( dbquant_bit1 ) {
        dbquant_bit2 = (temp << (18 + *incr)) >> 31 ;
        *incr += 1 ;
        if ( dbquant_bit2 ) 
            return 2 ;
        else 
            return -2 ;
    }
    return (0);
}

/*
   * decode B-frame mb_type
   * bit		ret_value
   * 1		0
   * 01		1
   * 001		2
   * 0001		3
   */
__inline int get_B_mbtype ( const unsigned int temp, int *RESTRICT incr )
{
    int mb_type ;
    
    for ( mb_type = 0 ; mb_type <= 3 ; mb_type++ ){ 
        if ( (temp << (18 + mb_type)) >> 31 ) {
            *incr = mb_type + 1 ;
            return mb_type ;
        }
    }
    return -1 ;
}
