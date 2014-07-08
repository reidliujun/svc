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
#include "mpeg.h"
#ifndef POCKET_PC
#include <string.h>
#endif

void                        StockBlocksLum_init (const struct_VOLsimple *RESTRICT VOLsimple, short *RESTRICT BuffX3, short *RESTRICT BuffX4) ;
void                        VideoPacketHeaderP (const unsigned char *RESTRICT data, int pos_i
    , const struct_VOLsimple *RESTRICT VOLsimple, struct_VOP *RESTRICT VOP, const int *RESTRICT MB_courant
    , struct_VOP *RESTRICT newVOP, int *RESTRICT pos_o, int *RESTRICT resync_marker, int *RESTRICT MB_num) ;
void                        Param_MB_P (const int pos_i, const unsigned char *RESTRICT data
    , const struct_VOP *RESTRICT VOP, const int *RESTRICT MB_pos, struct_VOP *RESTRICT NewVOP, int *RESTRICT pos_o
    , int *RESTRICT Mb_not_coded, int *RESTRICT Mb_type, unsigned char *RESTRICT stock_mb_type_P) ;
void                        StockBlocksLum (const int pos_i, const int resync_marker
    , const short *RESTRICT BuffX2, const short *RESTRICT BuffX3, const short *RESTRICT BuffX4
    , const struct_VOLsimple *RESTRICT VOLsimple, short *RESTRICT BuffA, short *RESTRICT BuffB, short *RESTRICT BuffC
    , short *RESTRICT BuffD, short *RESTRICT BuffE) ;
void                        StockBlocksCb (const int MBpos, const short *RESTRICT BuffX
    , const struct_VOLsimple *RESTRICT VOLsimple, const int resync_marker, short *RESTRICT BuffA
    , short *RESTRICT BuffB, short *RESTRICT BuffC) ;
void                        StockBlocksCr (const int MBpos, const short *RESTRICT BuffX
    , const struct_VOLsimple *RESTRICT VOLsimple, const int resync_marker, short *RESTRICT BuffA
    , short *RESTRICT BuffB, short *RESTRICT BuffC) ;
void                        decode_bloc_intra (const int k, const unsigned char *RESTRICT data
    , const struct_VOP *RESTRICT VOP, const int pos_i, const REVERSE_EVENT *RESTRICT DCT3D_I
    , const struct_VOLsimple *RESTRICT VOLsimple, const int MB_pos, const short *RESTRICT Blk_A
    , const short *RESTRICT Blk_B, const short *RESTRICT Blk_C, short *RESTRICT InverseQuant_Blk_X
    , unsigned char *RESTRICT block_sat_X, int *RESTRICT pos_fin_vlc) ;
void                        Stock_block_in_pict (const int width, const int dep_X, const unsigned char *RESTRICT src
    , unsigned char *RESTRICT dst) ;
void                        image_setedges (unsigned char *ref_y, unsigned char *ref_u
    , unsigned char *ref_v, unsigned char *y, unsigned char *u, unsigned char *v, const unsigned int edged_width
    , const unsigned int height) ;
void                        get_motion_vector (const int k, const vector *RESTRICT save_vector
    , const struct_VOLsimple *RESTRICT VOLsimple, const unsigned char *RESTRICT bs, const int pos_i
    , const int *RESTRICT MBpos, const struct_VOP *RESTRICT VOP, vector *RESTRICT current_mv, int *RESTRICT mvpos) ;
void                        decode_bloc_inter (const int k, const unsigned char *RESTRICT data
    , const struct_VOP *RESTRICT VOP, const int pos_i, const REVERSE_EVENT *RESTRICT DCT3D_P
    , const struct_VOLsimple *RESTRICT VOLsimple, const int width, const int height, const int width_edged, const int x_pos_inter
    , const int y_pos_inter, const int dx, const int dy, const unsigned char *RESTRICT Refn
    , unsigned char *RESTRICT block_X, int *RESTRICT pos_fin_vlc) ;
void Stock_block_from_prev_pict_in_pict ( const int width, const int pos_X, const unsigned char *RESTRICT src
    , unsigned char *RESTRICT dst );

static const unsigned int   roundtab [16] = { 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2 };

/*!
   P-frame processing. (TODO):MAJ
   
   	\param[in] data 			: bitstream (double buffer output)
   	\param[in] VOLsimple		: input VOL
   	\param[in] position 		: position in bitstream
   	\param[in] VOP 				: input VOP
   	\param[in] DCT3D_I 			: inverse VLC intra coefficients
   	\param[in] DCT3D_P 			: inverse VLC inter coefficients
    \param[out] stock_mb_type_P	: mb type
    \param[out] pos_fin_vlc		: position in bitstream after frame processing
    \param[out] address		: address in the buffer of luminance of the picture to display
    \param[inout] Lum 		: buffer of luminance Y
   	\param[inout] Cb 		: buffer of chrominance U
   	\param[inout] Cr 		: buffer of chrominance V
    \param[inout] keyframes 		: where is the next key frames
   */

void decode_P_frame ( const unsigned char *RESTRICT data, const struct_VOLsimple *RESTRICT VOLsimple, const int pos_i
    , struct_VOP *RESTRICT VOP, const REVERSE_EVENT *RESTRICT DCT3D_I, const REVERSE_EVENT *RESTRICT DCT3D_P
    , unsigned char *RESTRICT stock_mb_type_P, int *RESTRICT pos_fin_vlc, int *RESTRICT address
    , unsigned char *RESTRICT Lum, unsigned char *RESTRICT Cb, unsigned char *RESTRICT Cr
    , int *RESTRICT keyframes, vector *RESTRICT save_mv)
{
    int             i, j, k ;
    int             MB_courant ;
    int             MB_number ;
    int             pos_X [1];
    int             tab_pos_X [4];
    struct_VOP      new_VOP [1];
    int             pos [1];
    int             pos2 [1];
    int             mb_not_coded [1];
    int             mb_type [1];
    int             VideoPacketHeader_resync_marker [1];
    short           StockBlockLum_BuffA [16];
    short           StockBlockLum_BuffB [16];
    short           StockBlockLum_BuffC [16];
    short           StockBlockLum_BuffD [16];
    short           StockBlockLum_BuffE [16];
    short           StockBlocksCr_BuffA [16];
    short           StockBlocksCr_BuffB [16];
    short           StockBlocksCr_BuffC [16];
    short           StockBlocksCb_BuffA [16];
    short           StockBlocksCb_BuffB [16];
    short           StockBlocksCb_BuffC [16];
    int             VLCinverseXi_pos [1];
    int             VLCinverseXi_pos_prec [1];
    short           InverseQuant_BlkXn [6 * 16];
    unsigned char   block_8x8 [64];
    short    *DCpred_buffA [6];
    short    *DCpred_buffB [6];
    short    *DCpred_buffC [6];
    int           width, height ;
    int           width_edged ;
    vector          *current_vector ;
    int             uv_dx, uv_dy ;
    int             dx, dy ;
    int             x_pos_inter, y_pos_inter ;
    int             tab_x_pos_inter [4] = { 0, 8, 0, 8 };
    int             tab_y_pos_inter [4] = { 0, 0, 8, 8 };
    const int       edge_size2 = EDGE_SIZE >> 1 ;
    unsigned char    *display [6];
    unsigned char    *display_prec [6];
    unsigned int    mov_display_prec [6];
    const int       stride = VOLsimple -> video_object_layer_width + 2 * EDGE_SIZE ;

    *pos_fin_vlc = pos_i ;
    //DCpred_buffA
    DCpred_buffA [0] = StockBlockLum_BuffA ;
    DCpred_buffA [1] = InverseQuant_BlkXn ;
    DCpred_buffA [2] = (StockBlockLum_BuffE);
    DCpred_buffA [3] = (InverseQuant_BlkXn + 2 * 16);
    DCpred_buffA [4] = (StockBlocksCb_BuffA);
    DCpred_buffA [5] = (StockBlocksCr_BuffA);
    //DCpred_buffB
    DCpred_buffB [0] = StockBlockLum_BuffB ;
    DCpred_buffB [1] = StockBlockLum_BuffC ;
    DCpred_buffB [2] = (StockBlockLum_BuffA);
    DCpred_buffB [3] = (InverseQuant_BlkXn);
    DCpred_buffB [4] = (StockBlocksCb_BuffB);
    DCpred_buffB [5] = (StockBlocksCr_BuffB);
    //DCpred_buffC
    DCpred_buffC [0] = StockBlockLum_BuffC ;
    DCpred_buffC [1] = StockBlockLum_BuffD ;
    DCpred_buffC [2] = (InverseQuant_BlkXn);
    DCpred_buffC [3] = (InverseQuant_BlkXn + 1 * 16);
    DCpred_buffC [4] = (StockBlocksCb_BuffC);
    DCpred_buffC [5] = (StockBlocksCr_BuffC);
    //tab_pos_X
    tab_pos_X [0] = 0 ;
    tab_pos_X [1] = 8 ;
    tab_pos_X [2] = 8 * stride ;
    tab_pos_X [3] = 8 * stride + 8 ;
    //display
    if (keyframes[0]==0){
        *address = stride * (VOLsimple -> video_object_layer_height + 2 * EDGE_SIZE);
        display [0] = Lum ;
        display [1] = Lum ;
        display [2] = Lum ;
        display [3] = Lum ;
        display [4] = Cb ;
        display [5] = Cr ;
        display_prec [0] = Lum + *address;
        display_prec [1] = Lum + *address;
        display_prec [2] = Lum + *address;
        display_prec [3] = Lum + *address;
        display_prec [4] = Cb + *address / 4;
        display_prec [5] = Cr + *address / 4;
        keyframes[0]=1;
        keyframes[1]=0;
    }
    else
    {
        *address = stride * (VOLsimple -> video_object_layer_height + 2 * EDGE_SIZE);
        display [0] = (Lum + *address);
        display [1] = (Lum + *address);
        display [2] = (Lum + *address);
        display [3] = (Lum + *address);
        display [4] = (Cb + *address / 4);
        display [5] = (Cr + *address / 4);
        display_prec [0] = Lum ;
        display_prec [1] = Lum ;
        display_prec [2] = Lum ;
        display_prec [3] = Lum ;
        display_prec [4] = Cb ;
        display_prec [5] = Cr ;
        keyframes[0]=0;
        keyframes[1]=1;
        *address = 0;
    }

    //mov_display_prec
    mov_display_prec [0] = EDGE_SIZE + EDGE_SIZE * (stride);
    mov_display_prec [1] = EDGE_SIZE + EDGE_SIZE * (stride);
    mov_display_prec [2] = EDGE_SIZE + EDGE_SIZE * (stride);
    mov_display_prec [3] = EDGE_SIZE + EDGE_SIZE * (stride);
    mov_display_prec [4] = edge_size2 + edge_size2 * (stride >> 1);
    mov_display_prec [5] = edge_size2 + edge_size2 * (stride >> 1);
    StockBlocksLum_init(VOLsimple, InverseQuant_BlkXn + 2 * 16, InverseQuant_BlkXn + 3 * 16);
    for ( MB_courant = 0 ; 
        MB_courant < VOLsimple -> video_object_layer_width * VOLsimple -> video_object_layer_height / 256 ; 
        MB_courant++ ) {
			VideoPacketHeaderP(data, *pos_fin_vlc, VOLsimple, VOP, &MB_courant, new_VOP, pos
                , VideoPacketHeader_resync_marker, &MB_number);
            MB_courant = MB_number ;
            i = MB_courant % (VOLsimple -> video_object_layer_width / 16);
            j = MB_courant / (VOLsimple -> video_object_layer_width / 16);
            Param_MB_P(pos [0], data, new_VOP, &MB_courant, VOP, pos2, mb_not_coded, mb_type, stock_mb_type_P);
            current_vector = &(save_mv [MB_courant]);
            /* sauvegarde de 4 vecteur par MB, à optimiser */
            if ( !mb_not_coded [0] ) {

                /* CODED */
                if ( mb_type [0] == MODE_INTER || mb_type [0] == MODE_INTER_Q ) {

                    /* MODE INTER, INTER_Q */
                    get_motion_vector(0, save_mv, VOLsimple, data, pos2 [0], &MB_courant, VOP, current_vector, pos);
                    current_vector -> mvs [1].x = current_vector -> mvs [2].x = current_vector -> mvs [3].x
                        = current_vector -> mvs [0].x ;
                    current_vector -> mvs [1].y = current_vector -> mvs [2].y = current_vector -> mvs [3].y
                        = current_vector -> mvs [0].y ;
                    uv_dx = current_vector -> mvs [0].x ;
                    uv_dy = current_vector -> mvs [0].y ;
                    uv_dx = uv_dx & 3 ? uv_dx >> 1 | 1 : uv_dx / 2 ;
                    uv_dy = uv_dy & 3 ? uv_dy >> 1 | 1 : uv_dy / 2 ;
                    StockBlocksLum(i, VideoPacketHeader_resync_marker [0], InverseQuant_BlkXn + 1 * 16
                        , InverseQuant_BlkXn + 2 * 16, InverseQuant_BlkXn + 3 * 16, VOLsimple, StockBlockLum_BuffA
                        , StockBlockLum_BuffB, StockBlockLum_BuffC, StockBlockLum_BuffD, StockBlockLum_BuffE);
                    StockBlocksCb(MB_courant, InverseQuant_BlkXn + 4 * 16, VOLsimple, VideoPacketHeader_resync_marker [0]
                    , StockBlocksCb_BuffA, StockBlocksCb_BuffB, StockBlocksCb_BuffC);
                    StockBlocksCr(MB_courant, InverseQuant_BlkXn + 5 * 16, VOLsimple, VideoPacketHeader_resync_marker [0]
                    , StockBlocksCr_BuffA, StockBlocksCr_BuffB, StockBlocksCr_BuffC);
                    VLCinverseXi_pos_prec [0] = pos [0];
                    //display_img = imgLum ;
                    width = VOLsimple -> video_object_layer_width ;
                    height = VOLsimple -> video_object_layer_height ;
                    x_pos_inter = 16 * i ;
                    y_pos_inter = 16 * j ;
                    pos_X [0] = ((i + j * stride) << 4) + EDGE_SIZE + EDGE_SIZE * stride;
                    for ( k = 0 ; k < 4 ; k++ ) {
                        dx = current_vector -> mvs [k].x ;
                        dy = current_vector -> mvs [k].y ;
                        decode_bloc_inter(k, data, VOP, VLCinverseXi_pos_prec [0], DCT3D_P, VOLsimple, width, height, stride
                            , x_pos_inter + tab_x_pos_inter [k], y_pos_inter + tab_y_pos_inter [k], dx, dy
                            ,  (display_prec [k] + mov_display_prec [k])
                            , block_8x8, VLCinverseXi_pos);
                        Stock_block_in_pict(stride, pos_X [0] + tab_pos_X [k], block_8x8,  display [k]);
                        VLCinverseXi_pos_prec [0] = VLCinverseXi_pos [0];
                    }
                    pos_X [0] = ((i + j * (stride >> 1)) << 3) + edge_size2 + edge_size2 * (stride >> 1);
                    dx = uv_dx ;
                    dy = uv_dy ;
                    width = VOLsimple -> video_object_layer_width / 2 ;
                    height = VOLsimple -> video_object_layer_height / 2;
                    width_edged = stride >> 1 ;
                    x_pos_inter = 8 * i ;
                    y_pos_inter = 8 * j ;
                    for ( k = 4 ; k < 6 ; k++ ) {
                        decode_bloc_inter(k, data, VOP, VLCinverseXi_pos_prec [0], DCT3D_P, VOLsimple, width, height, width_edged
                            , x_pos_inter, y_pos_inter, dx, dy,  (display_prec [k] + mov_display_prec [k])
                            , block_8x8, VLCinverseXi_pos);
                        Stock_block_in_pict(stride >> 1, pos_X [0], block_8x8,  display [k]);
                        VLCinverseXi_pos_prec [0] = VLCinverseXi_pos [0];
                    }
                    *pos_fin_vlc = VLCinverseXi_pos [0];
                    for ( k = 1 ; k < 6 ; k++ ) {
                        memset(InverseQuant_BlkXn + k * 16, 0, 16 * sizeof(short));
                        *(InverseQuant_BlkXn + k * 16) = 1024 ;
                    }
                } //fin INTER MODE
                else if ( mb_type [0] == MODE_INTER4V ) {

                    /* MODE INTER4V */
                    int sum ;
                    get_motion_vector(0, save_mv, VOLsimple, data, pos2 [0], &MB_courant, VOP, current_vector, pos);
                    get_motion_vector(1, save_mv, VOLsimple, data, pos [0], &MB_courant, VOP, current_vector, pos2);
                    get_motion_vector(2, save_mv, VOLsimple, data, pos2 [0], &MB_courant, VOP, current_vector, pos);
                    get_motion_vector(3, save_mv, VOLsimple, data, pos [0], &MB_courant, VOP, current_vector, pos2);
                    sum = current_vector -> mvs [0].x + current_vector -> mvs [1].x + current_vector -> mvs [2].x
                        + current_vector -> mvs [3].x ;
                    uv_dx = sum == 0 ? 0 : SIGN(sum) * (roundtab [ABS(sum) % 16] + ABS(sum) / 16 * 2);
                    sum = current_vector -> mvs [0].y + current_vector -> mvs [1].y + current_vector -> mvs [2].y
                        + current_vector -> mvs [3].y ;
                    uv_dy = sum == 0 ? 0 : SIGN(sum) * (roundtab [ABS(sum) % 16] + ABS(sum) / 16 * 2);
                    StockBlocksLum(i, VideoPacketHeader_resync_marker [0], InverseQuant_BlkXn + 1 * 16
                        , InverseQuant_BlkXn + 2 * 16, InverseQuant_BlkXn + 3 * 16, VOLsimple, StockBlockLum_BuffA
                        , StockBlockLum_BuffB, StockBlockLum_BuffC, StockBlockLum_BuffD, StockBlockLum_BuffE);
                    StockBlocksCb(MB_courant, InverseQuant_BlkXn + 4 * 16, VOLsimple, VideoPacketHeader_resync_marker [0]
                    , StockBlocksCb_BuffA, StockBlocksCb_BuffB, StockBlocksCb_BuffC);
                    StockBlocksCr(MB_courant, InverseQuant_BlkXn + 5 * 16, VOLsimple, VideoPacketHeader_resync_marker [0]
                    , StockBlocksCr_BuffA, StockBlocksCr_BuffB, StockBlocksCr_BuffC);
                    VLCinverseXi_pos_prec [0] = pos2 [0];
                    //display_img = imgLum ;
                    width = VOLsimple -> video_object_layer_width ;
                    height = VOLsimple -> video_object_layer_height ;
                    width_edged = VOLsimple -> video_object_layer_width + 2 * EDGE_SIZE ;
                    pos_X [0] = ((i + j * stride) << 4) + EDGE_SIZE + EDGE_SIZE * stride;
                    for ( k = 0 ; k < 4 ; k++ ) {
                        dx = current_vector -> mvs [k].x ;
                        dy = current_vector -> mvs [k].y ;
                        x_pos_inter = 16 * i + tab_x_pos_inter [k];
                        y_pos_inter = 16 * j + tab_y_pos_inter [k];
                        decode_bloc_inter(k, data, VOP, VLCinverseXi_pos_prec [0], DCT3D_P, VOLsimple, width, height, width_edged
                            , x_pos_inter, y_pos_inter, dx, dy,  (display_prec [k] + mov_display_prec [k])
                            , block_8x8, VLCinverseXi_pos);
                        Stock_block_in_pict(stride, pos_X [0] + tab_pos_X [k], block_8x8,  display [k]);
                        VLCinverseXi_pos_prec [0] = VLCinverseXi_pos [0];
                    } // for Xi
                    x_pos_inter = 8 * i ;
                    y_pos_inter = 8 * j ;
                    dx = uv_dx ;
                    dy = uv_dy ;
                    width = VOLsimple -> video_object_layer_width / 2 ;
                    height = VOLsimple -> video_object_layer_height / 2;
                    width_edged = (VOLsimple -> video_object_layer_width + 2 * EDGE_SIZE) / 2 ;
                    pos_X [0] = ((i + j * (stride >> 1)) << 3) + edge_size2 + edge_size2 * (stride >> 1);
                    for ( k = 4 ; k < 6 ; k++ ) {
                        decode_bloc_inter(k, data, VOP, VLCinverseXi_pos_prec [0], DCT3D_P, VOLsimple, width, height, width_edged
                            , x_pos_inter, y_pos_inter, dx, dy,  (display_prec [k] + mov_display_prec [k])
                            , block_8x8, VLCinverseXi_pos);
                        Stock_block_in_pict(stride >> 1, pos_X [0], block_8x8,  display [k]);
                        VLCinverseXi_pos_prec [0] = VLCinverseXi_pos [0];
                    } // for Xi
                    *pos_fin_vlc = VLCinverseXi_pos [0];
                    for ( k = 1 ; k < 6 ; k++ ) {
                        memset(InverseQuant_BlkXn + k * 16, 0, 16 * sizeof(short));
                        *(InverseQuant_BlkXn + k * 16) = 1024 ;
                    }
                } //fin INTER4V MODE 
                else {

                    /* MODE_INTRA, MODE_INTRA_Q */
                    current_vector -> mvs [0].x = current_vector -> mvs [1].x = current_vector -> mvs [2].x
                        = current_vector -> mvs [3].x = 0 ;
                    current_vector -> mvs [0].y = current_vector -> mvs [1].y = current_vector -> mvs [2].y
                        = current_vector -> mvs [3].y = 0 ;
                    StockBlocksLum(i, VideoPacketHeader_resync_marker [0], InverseQuant_BlkXn + 1 * 16
                        , InverseQuant_BlkXn + 2 * 16, InverseQuant_BlkXn + 3 * 16, VOLsimple, StockBlockLum_BuffA
                        , StockBlockLum_BuffB, StockBlockLum_BuffC, StockBlockLum_BuffD, StockBlockLum_BuffE);
                    StockBlocksCb(MB_courant, InverseQuant_BlkXn + 4 * 16, VOLsimple, VideoPacketHeader_resync_marker [0]
                    , StockBlocksCb_BuffA, StockBlocksCb_BuffB, StockBlocksCb_BuffC);
                    StockBlocksCr(MB_courant, InverseQuant_BlkXn + 5 * 16, VOLsimple, VideoPacketHeader_resync_marker [0]
                    , StockBlocksCr_BuffA, StockBlocksCr_BuffB, StockBlocksCr_BuffC);
                    VLCinverseXi_pos_prec [0] = pos2 [0];
                    width = VOLsimple -> video_object_layer_width ;
                    pos_X [0] = ((i + j * stride) << 4) + EDGE_SIZE + EDGE_SIZE * stride;
                    //pos_X[0] = i * 16 + 16 * j * width ;
                    for ( k = 0 ; k < 4 ; k++ ) {
                        decode_bloc_intra(k, data, VOP, VLCinverseXi_pos_prec [0], DCT3D_I, VOLsimple, MB_courant
                            , DCpred_buffA [k], DCpred_buffB [k], DCpred_buffC [k]
                        , InverseQuant_BlkXn + k * 16, block_8x8, VLCinverseXi_pos);
                        Stock_block_in_pict(stride, pos_X [0] + tab_pos_X [k], block_8x8,  display [k]);
                        VLCinverseXi_pos_prec [0] = VLCinverseXi_pos [0];
                    }
                    width = VOLsimple -> video_object_layer_width >> 1 ;
                    pos_X [0] = ((i + j * (stride >> 1)) << 3) + edge_size2 + edge_size2 * (stride >> 1);
                    for ( k = 4 ; k < 6 ; k++ ) {
                        decode_bloc_intra(k, data, VOP, VLCinverseXi_pos_prec [0], DCT3D_I, VOLsimple, MB_courant
                            , DCpred_buffA [k], DCpred_buffB [k], DCpred_buffC [k]
                        , InverseQuant_BlkXn + k * 16, block_8x8, VLCinverseXi_pos);
                        Stock_block_in_pict(stride >> 1, pos_X [0], block_8x8,  display [k]);
                        VLCinverseXi_pos_prec [0] = VLCinverseXi_pos [0];
                    } // for Xi
                    *pos_fin_vlc = VLCinverseXi_pos [0];
                } //fin INTRA MODE
            } //fin CODED
            else {

                /* NOT_CODED */
                VOP -> mb_type = MODE_NOT_CODED ;
                current_vector -> mvs [0].x = current_vector -> mvs [1].x = current_vector -> mvs [2].x
                    = current_vector -> mvs [3].x = 0 ;
                current_vector -> mvs [0].y = current_vector -> mvs [1].y = current_vector -> mvs [2].y
                    = current_vector -> mvs [3].y = 0 ;
                *pos_fin_vlc = *pos2 ;
                StockBlocksLum(i, VideoPacketHeader_resync_marker [0], InverseQuant_BlkXn + 1 * 16
                    , InverseQuant_BlkXn + 2 * 16, InverseQuant_BlkXn + 3 * 16, VOLsimple, StockBlockLum_BuffA
                    , StockBlockLum_BuffB, StockBlockLum_BuffC, StockBlockLum_BuffD, StockBlockLum_BuffE);
                StockBlocksCb(MB_courant, InverseQuant_BlkXn + 4 * 16, VOLsimple, VideoPacketHeader_resync_marker [0]
                , StockBlocksCb_BuffA, StockBlocksCb_BuffB, StockBlocksCb_BuffC);
                StockBlocksCr(MB_courant, InverseQuant_BlkXn + 5 * 16, VOLsimple, VideoPacketHeader_resync_marker [0]
                , StockBlocksCr_BuffA, StockBlocksCr_BuffB, StockBlocksCr_BuffC);
                for ( k = 1 ; k < 6 ; k++ ) {
                    memset(InverseQuant_BlkXn + k * 16, 0, 16 * sizeof(short));
                    *(InverseQuant_BlkXn + k * 16) = 1024 ;
                }
                //pt_display_prec =  Y_prec ;
                width = VOLsimple -> video_object_layer_width ;
                pos_X [0] = ((i + j * stride) << 4) + EDGE_SIZE + EDGE_SIZE * stride;
                for ( k = 0 ; k < 4 ; k++ ) {
                    Stock_block_from_prev_pict_in_pict(stride, pos_X [0] + tab_pos_X [k],  display_prec [k]
                    ,  display [k]);
                }
                width = VOLsimple -> video_object_layer_width >> 1 ;
                pos_X [0] = ((i + j * (stride >> 1)) << 3) + edge_size2 + edge_size2 * (stride >> 1);
                for ( k = 4 ; k < 6 ; k++ ) {
                    Stock_block_from_prev_pict_in_pict(stride >> 1, pos_X [0],  display_prec [k]
                    ,  display [k]);
                }
            } //fin NOT_CODED
    } //for MB
    image_setedges( (display [0]),  (display [4]),  (display [5])
    	,  (display [0]),  (display [4]),  (display [5]), VOLsimple -> video_object_layer_width + 2 * EDGE_SIZE
        , VOLsimple -> video_object_layer_height);
    //for B-frames
}
