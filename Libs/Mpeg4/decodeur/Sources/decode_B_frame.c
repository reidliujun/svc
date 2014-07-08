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

static const unsigned int   roundtab [16] = { 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2 };
static const VECTOR         zeromv = { 0, 0 };
void                        StockBlocksLum_init (const struct_VOLsimple *RESTRICT VOLsimple, short *RESTRICT BuffX3, short *RESTRICT BuffX4) ;
void                        VideoPacketHeaderB (const unsigned char *RESTRICT data, const int pos_i
    , const struct_VOLsimple *RESTRICT VOLsimple, struct_VOP *RESTRICT VOP, const int *RESTRICT MB_courant
    , struct_VOP *RESTRICT newVOP, int *RESTRICT pos_o, int *RESTRICT resync_marker, int *RESTRICT MB_num) ;
void                        Param_MB_B (int pos_i, const unsigned char *RESTRICT data, const struct_VOP *RESTRICT VOP
    , const int *RESTRICT MB_pos, const unsigned char *RESTRICT stock_mb_type_P, struct_VOP *RESTRICT NewVOP
    , int *RESTRICT pos_o) ;

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
void                        get_motion_vector (const int k, const vector *RESTRICT save_vector
    , const struct_VOLsimple *RESTRICT VOLsimple, const unsigned char *RESTRICT bs, const int pos_i
    , const int *RESTRICT MBpos, const struct_VOP *RESTRICT VOP, vector *RESTRICT current_mv, int *RESTRICT mvpos) ;
void                        decode_bloc_inter (const int k, const unsigned char *RESTRICT data
    , const struct_VOP *RESTRICT VOP, const int pos_i, const REVERSE_EVENT *RESTRICT DCT3D_P
    , const struct_VOLsimple *RESTRICT VOLsimple, const int width, const int height, const int width_edged, const int x_pos_inter
    , const int y_pos_inter, const int dx, const int dy, const unsigned char *RESTRICT Refn
    , unsigned char *RESTRICT block_X, int *RESTRICT pos_fin_vlc) ;
void                        get_b_motion_vector (const int VOP_fcode, const VECTOR *RESTRICT pmv
    , const unsigned char *RESTRICT bs, const int pos_i, VECTOR *RESTRICT mv, int *RESTRICT mvpos) ;
void                        decode_bf_interpolate_bloc_inter (const int k, const unsigned char *RESTRICT data
    , const struct_VOP *RESTRICT VOP, const int pos_i, const REVERSE_EVENT *RESTRICT DCT3D_P
    , const struct_VOLsimple *RESTRICT VOLsimple, const int width, const int height, const int width_edged, const int x_pos_inter
    , const int y_pos_inter, const int dx, const int dy, const int dx_b, const int dy_b
    , const unsigned char *RESTRICT Refn_first, const unsigned char *RESTRICT Refn_last
    , unsigned char *RESTRICT block_X, int *RESTRICT pos_fin_vlc) ;
void Stock_block_in_pict ( const int width, const int dep_X, const unsigned char *RESTRICT src
    , unsigned char *RESTRICT dst );


/*!
   B-frame processing. (TODO):MAJ
   
   	\param[in] data 			: bitstream (double buffer output)
   	\param[in] VOLsimple		: input VOL
   	\param[in] position 		: position in bitstream
   	\param[in] VOP 				: input VOP
   	\param[in] DCT3D_I 			: inverse VLC intra coefficients
   	\param[in] DCT3D_P 			: inverse VLC inter coefficients
    \param[in] stock_mb_type_P	: mb type
    \param[in] mv_P         	: P mv vector
    \param[out] pos_fin_vlc		: position in bitstream after frame processing
    \param[out] address		: address in the buffer of luminance of the picture to display
    \param[inout] Lum 		: buffer of luminance Y
   	\param[inout] Cb 		: buffer of chrominance U
   	\param[inout] Cr 		: buffer of chrominance V
    \param[inout] keyframes 		: where is the next key frames
   */
void decode_B_frame ( const unsigned char *RESTRICT data, const struct_VOLsimple *RESTRICT VOLsimple, const int pos_i
    , struct_VOP *RESTRICT VOP, const REVERSE_EVENT *RESTRICT DCT3D_P, const unsigned char *RESTRICT stock_mb_type_P
    , const vector *RESTRICT mv_P, int *RESTRICT pos_fin_vlc, int *RESTRICT address
    , unsigned char *RESTRICT frame_Y, unsigned char *RESTRICT frame_U, unsigned char *RESTRICT frame_V
    , int *RESTRICT keyframes 
     )
{
    int             i, j, k, l ;
    int             sum ;
    int             MB_courant ;
    int             MB_number ;
    int             pos_X [1];
    int             tab_pos_X [4];
    struct_VOP      new_VOP [1];
    int             pos [1];
    int             pos2 [1];
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
    unsigned char    *display [6];
    unsigned char    *display_img_first [6];
    unsigned char    *display_img_last [6];
    int             width, height ;
    vector          current_mv [1];
    vector          current_b_mv [1];
    VECTOR          p_fmv [1];
    VECTOR          p_bmv [1];
    VECTOR          mv [1];
    int             uv_dx, uv_dy, b_uv_dx, b_uv_dy ;
    unsigned char   *imgLum_first ;
    unsigned char   *imgCb_first ;
    unsigned char   *imgCr_first ;
    unsigned char   *imgLum_last ;
    unsigned char   *imgCb_last ;
    unsigned char   *imgCr_last ;
    int             dx, dy, dx_b, dy_b ;
    int             x_pos_inter, y_pos_inter ;
    int             tab_x_pos_inter [4] = { 0, 8, 0, 8 };
    int             tab_y_pos_inter [4] = { 0, 0, 8, 8 };
    const int       edge_size2 = EDGE_SIZE >> 1 ;
    const int       stride = VOLsimple -> video_object_layer_width + 2 * EDGE_SIZE ;
    const int       picture_size = stride * (VOLsimple -> video_object_layer_height + 2 * EDGE_SIZE);
    //display
    display [0] = frame_Y + 2 * picture_size;
    display [1] = frame_Y + 2 * picture_size;
    display [2] = frame_Y + 2 * picture_size;
    display [3] = frame_Y + 2 * picture_size;
    display [4] = frame_U + picture_size / 2;
    display [5] = frame_V + picture_size / 2;

    if (keyframes[0]==1){
    //display_img_first
        imgLum_first = frame_Y + picture_size + EDGE_SIZE + EDGE_SIZE * (VOLsimple -> video_object_layer_width + 2 * EDGE_SIZE);
        imgCb_first = frame_U + picture_size / 4 + EDGE_SIZE / 2 + EDGE_SIZE / 2 * (VOLsimple -> video_object_layer_width + 2 * EDGE_SIZE) / 2 ;
        imgCr_first = frame_V + picture_size / 4 + EDGE_SIZE / 2 + EDGE_SIZE / 2 * (VOLsimple -> video_object_layer_width + 2 * EDGE_SIZE) / 2 ;
        imgLum_last = frame_Y + EDGE_SIZE + EDGE_SIZE * (VOLsimple -> video_object_layer_width + 2 * EDGE_SIZE);
        imgCb_last = frame_U + EDGE_SIZE / 2 + EDGE_SIZE / 2 * (VOLsimple -> video_object_layer_width + 2 * EDGE_SIZE) / 2 ;
        imgCr_last = frame_V + EDGE_SIZE / 2 + EDGE_SIZE / 2 * (VOLsimple -> video_object_layer_width + 2 * EDGE_SIZE) / 2 ;
    }
    else
    {
        imgLum_first = frame_Y + EDGE_SIZE + EDGE_SIZE * (VOLsimple -> video_object_layer_width + 2 * EDGE_SIZE);
        imgCb_first = frame_U + EDGE_SIZE / 2 + EDGE_SIZE / 2 * (VOLsimple -> video_object_layer_width + 2 * EDGE_SIZE) / 2 ;
        imgCr_first = frame_V + EDGE_SIZE / 2 + EDGE_SIZE / 2 * (VOLsimple -> video_object_layer_width + 2 * EDGE_SIZE) / 2 ;
        imgLum_last = frame_Y + picture_size + EDGE_SIZE + EDGE_SIZE * (VOLsimple -> video_object_layer_width + 2 * EDGE_SIZE);
        imgCb_last = frame_U + picture_size / 4 + EDGE_SIZE / 2 + EDGE_SIZE / 2 * (VOLsimple -> video_object_layer_width + 2 * EDGE_SIZE) / 2 ;
        imgCr_last = frame_V + picture_size / 4 + EDGE_SIZE / 2 + EDGE_SIZE / 2 * (VOLsimple -> video_object_layer_width + 2 * EDGE_SIZE) / 2 ;
    }
    display_img_first [0] = imgLum_first;
    display_img_first [1] =  imgLum_first;
    display_img_first [2] =  imgLum_first;
    display_img_first [3] =  imgLum_first;
    display_img_first [4] =  imgCb_first;
    display_img_first [5] =  imgCr_first;
    //display_img_first
    display_img_last [0] =  imgLum_last ;
    display_img_last [1] =  imgLum_last ;
    display_img_last [2] =  imgLum_last ;
    display_img_last [3] =  imgLum_last ;
    display_img_last [4] =  imgCb_last ;
    display_img_last [5] =  imgCr_last ;
    *address = 2 * picture_size;

    //tab_pos_X
    tab_pos_X [0] = 0 ;
    tab_pos_X [1] = 8 ;
    tab_pos_X [2] = 8 * stride ;
    tab_pos_X [3] = 8 * stride + 8 ;
    *pos_fin_vlc = pos_i ;
    StockBlocksLum_init(VOLsimple, InverseQuant_BlkXn + 2 * 16, InverseQuant_BlkXn + 3 * 16);
    for ( MB_courant = 0 ; 
            MB_courant < VOLsimple -> video_object_layer_width * VOLsimple -> video_object_layer_height / 256 ; 
            MB_courant++ ) {
        if ( MB_courant % (VOLsimple -> video_object_layer_width / 16) == 0 ) {
            *p_fmv = *p_bmv = zeromv ;
        }
        VideoPacketHeaderB(data, *pos_fin_vlc, VOLsimple, VOP, &MB_courant, new_VOP, pos
            , VideoPacketHeader_resync_marker, &MB_number);
        mv [0] = current_b_mv -> mvs [0] = current_b_mv -> mvs [1] = current_b_mv -> mvs [2] = current_b_mv -> mvs [3]
            = current_mv -> mvs [0] = current_mv -> mvs [1] = current_mv -> mvs [2] = current_mv -> mvs [3] = zeromv ;
        Param_MB_B(pos [0], data, new_VOP, &MB_courant, stock_mb_type_P, VOP, pos2);
        MB_courant = MB_number ;
        i = MB_courant % (VOLsimple -> video_object_layer_width / 16);
        j = MB_courant / (VOLsimple -> video_object_layer_width / 16);
        if ( stock_mb_type_P [MB_courant] == MODE_NOT_CODED ) {
            uv_dx = current_mv -> mvs [0].x ;
            uv_dy = current_mv -> mvs [0].y ;
            uv_dx = uv_dx & 3 ? uv_dx >> 1 | 1 : uv_dx / 2 ;
            uv_dy = uv_dy & 3 ? uv_dy >> 1 | 1 : uv_dy / 2 ;
            VOP -> rounding = 0 ;
            StockBlocksLum(i, VideoPacketHeader_resync_marker [0], InverseQuant_BlkXn + 1 * 16
                , InverseQuant_BlkXn + 2 * 16, InverseQuant_BlkXn + 3 * 16, VOLsimple, StockBlockLum_BuffA
                , StockBlockLum_BuffB, StockBlockLum_BuffC, StockBlockLum_BuffD, StockBlockLum_BuffE);
            StockBlocksCb(MB_courant, InverseQuant_BlkXn + 4 * 16, VOLsimple, VideoPacketHeader_resync_marker [0]
                , StockBlocksCb_BuffA, StockBlocksCb_BuffB, StockBlocksCb_BuffC);
            StockBlocksCr(MB_courant, InverseQuant_BlkXn + 5 * 16, VOLsimple, VideoPacketHeader_resync_marker [0]
                , StockBlocksCr_BuffA, StockBlocksCr_BuffB, StockBlocksCr_BuffC);
            VLCinverseXi_pos_prec [0] = pos2 [0];
            
            // display = Lum ; //+ EDGE_SIZE + EDGE_SIZE * (VOLsimple->video_object_layer_width + 2 * EDGE_SIZE);
            // display_img_first = imgLum_first ;
            width = VOLsimple -> video_object_layer_width ;
            height = VOLsimple -> video_object_layer_height ;
            x_pos_inter = 16 * i ;
            y_pos_inter = 16 * j ;
            pos_X [0] = ((i + j * stride) << 4) + EDGE_SIZE + EDGE_SIZE * stride ;
            for ( k = 0 ; k < 4 ; k++ ) {
                dx = current_mv -> mvs [k].x ;
                dy = current_mv -> mvs [k].y ;
                decode_bloc_inter(k, data, VOP, VLCinverseXi_pos_prec [0], DCT3D_P, VOLsimple, width, height, stride
                    , x_pos_inter + tab_x_pos_inter [k], y_pos_inter + tab_y_pos_inter [k], dx, dy
                    ,  display_img_first [k], block_8x8, VLCinverseXi_pos);
                Stock_block_in_pict(stride, pos_X [0] + tab_pos_X [k], block_8x8,  display [k]);
                VLCinverseXi_pos_prec [0] = VLCinverseXi_pos [0];
            } // for Xi
            x_pos_inter = 8 * i ;
            y_pos_inter = 8 * j ;
            dx = uv_dx ;
            dy = uv_dy ;
            width = VOLsimple -> video_object_layer_width / 2 ;
            height = VOLsimple -> video_object_layer_height / 2 ;
            pos_X [0] = ((i + j * (stride >> 1)) << 3) + edge_size2 + edge_size2 * (stride >> 1);
            for ( k = 4 ; k < 6 ; k++ ) {
                decode_bloc_inter(k, data, VOP, VLCinverseXi_pos_prec [0], DCT3D_P, VOLsimple, width, height, stride >> 1
                    , x_pos_inter, y_pos_inter, dx, dy,  display_img_first [k]
                    , block_8x8, VLCinverseXi_pos);
                Stock_block_in_pict(stride >> 1, pos_X [0], block_8x8,  display [k]);
                VLCinverseXi_pos_prec [0] = VLCinverseXi_pos [0];
            } // for Xi
            *pos_fin_vlc = VLCinverseXi_pos [0];
            for ( k = 1 ; k < 6 ; k++ ) {
                memset(InverseQuant_BlkXn + k * 16, 0, 16 * sizeof(short));
                *(InverseQuant_BlkXn + k * 16) = 1024 ;
            }
        } //fin last_MB_P MODE_NOT_CODED
        else {
            switch ( VOP -> mb_type ) {
                case MODE_DIRECT : 
                    get_b_motion_vector(1, &zeromv, data, pos2 [0], mv, pos);
                    *pos2 = *pos ;
                case MODE_DIRECT_NONE_MV : 
                    for ( l = 0 ; l < 4 ; l++ ) {
                        current_mv -> mvs [l].x
                            = mv_P [MB_courant].mvs [l].x * VOP -> time_bp / VOP -> time_pp + mv -> x ;
                        current_mv -> mvs [l].y
                            = mv_P [MB_courant].mvs [l].y * VOP -> time_bp / VOP -> time_pp + mv -> y ;
                        current_b_mv -> mvs [l].x
                            = mv -> x ? 
                                   current_mv -> mvs [l].x - mv_P [MB_courant].mvs [l].x
                                : mv_P [MB_courant].mvs [l].x * (VOP -> time_bp - VOP -> time_pp) / VOP -> time_pp ;
                        current_b_mv -> mvs [l].y
                            = mv -> y ? 
                                   current_mv -> mvs [l].y - mv_P [MB_courant].mvs [l].y
                                : mv_P [MB_courant].mvs [l].y * (VOP -> time_bp - VOP -> time_pp) / VOP -> time_pp ;
                    }
                    sum = current_mv -> mvs [0].x + current_mv -> mvs [1].x + current_mv -> mvs [2].x
                        + current_mv -> mvs [3].x ;
                    uv_dx = sum == 0 ? 0 : SIGN(sum) * (roundtab [ABS(sum) % 16] + ABS(sum) / 16 * 2);
                    sum = current_mv -> mvs [0].y + current_mv -> mvs [1].y + current_mv -> mvs [2].y
                        + current_mv -> mvs [3].y ;
                    uv_dy = sum == 0 ? 0 : SIGN(sum) * (roundtab [ABS(sum) % 16] + ABS(sum) / 16 * 2);
                    sum = current_b_mv -> mvs [0].x + current_b_mv -> mvs [1].x + current_b_mv -> mvs [2].x
                        + current_b_mv -> mvs [3].x ;
                    b_uv_dx = sum == 0 ? 0 : SIGN(sum) * (roundtab [ABS(sum) % 16] + ABS(sum) / 16 * 2);
                    sum = current_b_mv -> mvs [0].y + current_b_mv -> mvs [1].y + current_b_mv -> mvs [2].y
                        + current_b_mv -> mvs [3].y ;
                    b_uv_dy = sum == 0 ? 0 : SIGN(sum) * (roundtab [ABS(sum) % 16] + ABS(sum) / 16 * 2);
                    VOP -> rounding = 0 ;
                    StockBlocksLum(i, VideoPacketHeader_resync_marker [0], InverseQuant_BlkXn + 1 * 16
                        , InverseQuant_BlkXn + 2 * 16, InverseQuant_BlkXn + 3 * 16, VOLsimple, StockBlockLum_BuffA
                        , StockBlockLum_BuffB, StockBlockLum_BuffC, StockBlockLum_BuffD, StockBlockLum_BuffE);
                    StockBlocksCb(MB_courant, InverseQuant_BlkXn + 4 * 16, VOLsimple
                        , VideoPacketHeader_resync_marker [0], StockBlocksCb_BuffA, StockBlocksCb_BuffB
                        , StockBlocksCb_BuffC);
                    StockBlocksCr(MB_courant, InverseQuant_BlkXn + 5 * 16, VOLsimple
                        , VideoPacketHeader_resync_marker [0], StockBlocksCr_BuffA, StockBlocksCr_BuffB
                        , StockBlocksCr_BuffC);
                    VLCinverseXi_pos_prec [0] = pos2 [0];
                    
                    // display = Lum ; // + EDGE_SIZE + EDGE_SIZE * (VOLsimple->video_object_layer_width + 2 * EDGE_SIZE);;
                    // display_img_first = imgLum_first ;
                    // display_img_last = imgLum_last ;
                    width = VOLsimple -> video_object_layer_width ;
                    height = VOLsimple -> video_object_layer_height ;
                    x_pos_inter = 16 * i ;
                    y_pos_inter = 16 * j ;
                    pos_X [0] = ((i + j * stride) << 4) + EDGE_SIZE + EDGE_SIZE * stride ;
                    for ( k = 0 ; k < 4 ; k++ ) {
                        dx = current_mv -> mvs [k].x ;
                        dy = current_mv -> mvs [k].y ;
                        dx_b = current_b_mv -> mvs [k].x ;
                        dy_b = current_b_mv -> mvs [k].y ;
                        decode_bf_interpolate_bloc_inter(k, data, VOP, VLCinverseXi_pos_prec [0], DCT3D_P, VOLsimple
                            , width, height, stride, x_pos_inter + tab_x_pos_inter [k], y_pos_inter + tab_y_pos_inter [k], dx
                            , dy, dx_b, dy_b,  display_img_first [k]
                            ,  display_img_last [k], block_8x8
                            , VLCinverseXi_pos);
                        Stock_block_in_pict(stride, pos_X [0] + tab_pos_X [k], block_8x8,  display [k]);
                        VLCinverseXi_pos_prec [0] = VLCinverseXi_pos [0];
                    } // for Xi
                    x_pos_inter = 8 * i ;
                    y_pos_inter = 8 * j ;
                    dx = uv_dx ;
                    dy = uv_dy ;
                    dx_b = b_uv_dx ;
                    dy_b = b_uv_dy ;
                    width = VOLsimple -> video_object_layer_width / 2 ;
                    height = VOLsimple -> video_object_layer_height / 2 ;
                    pos_X [0] = ((i + j * (stride >> 1)) << 3) + edge_size2 + edge_size2 * (stride >> 1);
                    for ( k = 4 ; k < 6 ; k++ ) {
                        decode_bf_interpolate_bloc_inter(k, data, VOP, VLCinverseXi_pos_prec [0], DCT3D_P, VOLsimple
                            , width, height, stride >> 1, x_pos_inter, y_pos_inter, dx, dy, dx_b, dy_b
                            ,  display_img_first [k],  display_img_last [k]
                            , block_8x8, VLCinverseXi_pos);
                        Stock_block_in_pict(stride >> 1, pos_X [0], block_8x8,  display [k]);
                        VLCinverseXi_pos_prec [0] = VLCinverseXi_pos [0];
                    } // for Xi
                    *pos_fin_vlc = VLCinverseXi_pos [0];
                    for ( k = 1 ; k < 6 ; k++ ) {
                        memset(InverseQuant_BlkXn + k * 16, 0, 16 * sizeof(short));
                        *(InverseQuant_BlkXn + k * 16) = 1024 ;
                    }
                    break ;
                case MODE_INTERPOLATE : 
                    get_b_motion_vector(VOP -> fcode_forward, p_fmv, data, pos2 [0], &current_mv -> mvs [0], pos);
                    *p_fmv = current_mv -> mvs [1] = current_mv -> mvs [2] = current_mv -> mvs [3]
                        = current_mv -> mvs [0];
                    get_b_motion_vector(VOP -> fcode_backward, p_bmv, data, pos [0], &current_b_mv -> mvs [0], pos2);
                    *p_bmv = current_b_mv -> mvs [1] = current_b_mv -> mvs [2] = current_b_mv -> mvs [3]
                        = current_b_mv -> mvs [0];
                    uv_dx = current_mv -> mvs [0].x ;
                    uv_dy = current_mv -> mvs [0].y ;
                    uv_dx = uv_dx & 3 ? uv_dx >> 1 | 1 : uv_dx / 2 ;
                    uv_dy = uv_dy & 3 ? uv_dy >> 1 | 1 : uv_dy / 2 ;
                    b_uv_dx = current_b_mv -> mvs [0].x ;
                    b_uv_dy = current_b_mv -> mvs [0].y ;
                    b_uv_dx = b_uv_dx & 3 ? b_uv_dx >> 1 | 1 : b_uv_dx / 2 ;
                    b_uv_dy = b_uv_dy & 3 ? b_uv_dy >> 1 | 1 : b_uv_dy / 2 ;
                    VOP -> rounding = 0 ;
                    StockBlocksLum(i, VideoPacketHeader_resync_marker [0], InverseQuant_BlkXn + 1 * 16
                        , InverseQuant_BlkXn + 2 * 16, InverseQuant_BlkXn + 3 * 16, VOLsimple, StockBlockLum_BuffA
                        , StockBlockLum_BuffB, StockBlockLum_BuffC, StockBlockLum_BuffD, StockBlockLum_BuffE);
                    StockBlocksCb(MB_courant, InverseQuant_BlkXn + 4 * 16, VOLsimple
                        , VideoPacketHeader_resync_marker [0], StockBlocksCb_BuffA, StockBlocksCb_BuffB
                        , StockBlocksCb_BuffC);
                    StockBlocksCr(MB_courant, InverseQuant_BlkXn + 5 * 16, VOLsimple
                        , VideoPacketHeader_resync_marker [0], StockBlocksCr_BuffA, StockBlocksCr_BuffB
                        , StockBlocksCr_BuffC);
                    VLCinverseXi_pos_prec [0] = pos2 [0];
                    
                    // display = Lum ; // + EDGE_SIZE + EDGE_SIZE * (VOLsimple->video_object_layer_width + 2 * EDGE_SIZE);
                    // display_img_first = imgLum_first ;
                    // display_img_last = imgLum_last ;
                    width = VOLsimple -> video_object_layer_width ;
                    height = VOLsimple -> video_object_layer_height ;
                    x_pos_inter = 16 * i ;
                    y_pos_inter = 16 * j ;
                    pos_X [0] = ((i + j * stride) << 4) + EDGE_SIZE + EDGE_SIZE * stride ;
                    for ( k = 0 ; k < 4 ; k++ ) {
                        dx = current_mv -> mvs [k].x ;
                        dy = current_mv -> mvs [k].y ;
                        dx_b = current_b_mv -> mvs [k].x ;
                        dy_b = current_b_mv -> mvs [k].y ;
                        decode_bf_interpolate_bloc_inter(k, data, VOP, VLCinverseXi_pos_prec [0], DCT3D_P, VOLsimple
                            , width, height, stride, x_pos_inter + tab_x_pos_inter [k], y_pos_inter + tab_y_pos_inter [k], dx
                            , dy, dx_b, dy_b,  display_img_first [k]
                            ,  display_img_last [k], block_8x8
                            , VLCinverseXi_pos);
                        Stock_block_in_pict(stride, pos_X [0] + tab_pos_X [k], block_8x8,  display [k]);
                        VLCinverseXi_pos_prec [0] = VLCinverseXi_pos [0];
                    } // for Xi
                    x_pos_inter = 8 * i ;
                    y_pos_inter = 8 * j ;
                    dx = uv_dx ;
                    dy = uv_dy ;
                    dx_b = b_uv_dx ;
                    dy_b = b_uv_dy ;
                    width = VOLsimple -> video_object_layer_width / 2 ;
                    height = VOLsimple -> video_object_layer_height / 2 ;
                    pos_X [0] = ((i + j * (stride >> 1)) << 3) + edge_size2 + edge_size2 * (stride >> 1);
                    for ( k = 4 ; k < 6 ; k++ ) {
                        decode_bf_interpolate_bloc_inter(k, data, VOP, VLCinverseXi_pos_prec [0], DCT3D_P, VOLsimple
                            , width, height, stride >> 1, x_pos_inter, y_pos_inter, dx, dy, dx_b, dy_b
                            ,  display_img_first [k],  display_img_last [k]
                            , block_8x8, VLCinverseXi_pos);
                        Stock_block_in_pict(stride >> 1, pos_X [0], block_8x8,  display [k]);
                        VLCinverseXi_pos_prec [0] = VLCinverseXi_pos [0];
                    } // for Xi
                    *pos_fin_vlc = VLCinverseXi_pos [0];
                    for ( k = 1 ; k < 6 ; k++ ) {
                        memset(InverseQuant_BlkXn + k * 16, 0, 16 * sizeof(short));
                        *(InverseQuant_BlkXn + k * 16) = 1024 ;
                    }
                    break ;
                case MODE_BACKWARD : 
                    get_b_motion_vector(VOP -> fcode_backward, p_bmv, data, pos2 [0], &current_mv -> mvs [0], pos);
                    *p_bmv = current_mv -> mvs [1] = current_mv -> mvs [2] = current_mv -> mvs [3]
                        = current_mv -> mvs [0];
                    uv_dx = current_mv -> mvs [0].x ;
                    uv_dy = current_mv -> mvs [0].y ;
                    uv_dx = uv_dx & 3 ? uv_dx >> 1 | 1 : uv_dx / 2 ;
                    uv_dy = uv_dy & 3 ? uv_dy >> 1 | 1 : uv_dy / 2 ;
                    VOP -> rounding = 0 ;
                    StockBlocksLum(i, VideoPacketHeader_resync_marker [0], InverseQuant_BlkXn + 1 * 16
                        , InverseQuant_BlkXn + 2 * 16, InverseQuant_BlkXn + 3 * 16, VOLsimple, StockBlockLum_BuffA
                        , StockBlockLum_BuffB, StockBlockLum_BuffC, StockBlockLum_BuffD, StockBlockLum_BuffE);
                    StockBlocksCb(MB_courant, InverseQuant_BlkXn + 4 * 16, VOLsimple
                        , VideoPacketHeader_resync_marker [0], StockBlocksCb_BuffA, StockBlocksCb_BuffB
                        , StockBlocksCb_BuffC);
                    StockBlocksCr(MB_courant, InverseQuant_BlkXn + 5 * 16, VOLsimple
                        , VideoPacketHeader_resync_marker [0], StockBlocksCr_BuffA, StockBlocksCr_BuffB
                        , StockBlocksCr_BuffC);
                    VLCinverseXi_pos_prec [0] = pos [0];
                    
                    // display = Lum ; //  + EDGE_SIZE + EDGE_SIZE * (VOLsimple->video_object_layer_width + 2 * EDGE_SIZE);
                    // display_img_last = imgLum_last ;
                    width = VOLsimple -> video_object_layer_width ;
                    height = VOLsimple -> video_object_layer_height ;
                    x_pos_inter = 16 * i ;
                    y_pos_inter = 16 * j ;
                    pos_X [0] = ((i + j * stride) << 4) + EDGE_SIZE + EDGE_SIZE * stride ;
                    for ( k = 0 ; k < 4 ; k++ ) {
                        dx = current_mv -> mvs [k].x ;
                        dy = current_mv -> mvs [k].y ;
                        decode_bloc_inter(k, data, VOP, VLCinverseXi_pos_prec [0], DCT3D_P, VOLsimple, width, height, stride
                            , x_pos_inter + tab_x_pos_inter [k], y_pos_inter + tab_y_pos_inter [k], dx, dy
                            ,  display_img_last [k], block_8x8
                            , VLCinverseXi_pos);
                        Stock_block_in_pict(stride, pos_X [0] + tab_pos_X [k], block_8x8,  display [k]);
                        VLCinverseXi_pos_prec [0] = VLCinverseXi_pos [0];
                    } // for Xi
                    x_pos_inter = 8 * i ;
                    y_pos_inter = 8 * j ;
                    dx = uv_dx ;
                    dy = uv_dy ;
                    width = VOLsimple -> video_object_layer_width / 2 ;
                    height = VOLsimple -> video_object_layer_height / 2 ;
                    pos_X [0] = ((i + j * (stride >> 1)) << 3) + edge_size2 + edge_size2 * (stride >> 1);
                    for ( k = 4 ; k < 6 ; k++ ) {
                        decode_bloc_inter(k, data, VOP, VLCinverseXi_pos_prec [0], DCT3D_P, VOLsimple, width, height
                            , stride >> 1, x_pos_inter, y_pos_inter, dx, dy,  display_img_last [k]
                            , block_8x8, VLCinverseXi_pos);
                        Stock_block_in_pict(stride >> 1, pos_X [0], block_8x8,  display [k]);
                        VLCinverseXi_pos_prec [0] = VLCinverseXi_pos [0];
                    } // for Xi
                    *pos_fin_vlc = VLCinverseXi_pos [0];
                    for ( k = 1 ; k < 6 ; k++ ) {
                        memset(InverseQuant_BlkXn + k * 16, 0, 16 * sizeof(short));
                        *(InverseQuant_BlkXn + k * 16) = 1024 ;
                    }
                    break ;
                case MODE_FORWARD : 
                    get_b_motion_vector(VOP -> fcode_forward, p_fmv, data, pos2 [0], &current_mv -> mvs [0], pos);
                    *p_fmv = current_mv -> mvs [1] = current_mv -> mvs [2] = current_mv -> mvs [3]
                        = current_mv -> mvs [0];
                    uv_dx = current_mv -> mvs [0].x ;
                    uv_dy = current_mv -> mvs [0].y ;
                    uv_dx = uv_dx & 3 ? uv_dx >> 1 | 1 : uv_dx / 2 ;
                    uv_dy = uv_dy & 3 ? uv_dy >> 1 | 1 : uv_dy / 2 ;
                    VOP -> rounding = 0 ;
                    StockBlocksLum(i, VideoPacketHeader_resync_marker [0], InverseQuant_BlkXn + 1 * 16
                        , InverseQuant_BlkXn + 2 * 16, InverseQuant_BlkXn + 3 * 16, VOLsimple, StockBlockLum_BuffA
                        , StockBlockLum_BuffB, StockBlockLum_BuffC, StockBlockLum_BuffD, StockBlockLum_BuffE);
                    StockBlocksCb(MB_courant, InverseQuant_BlkXn + 4 * 16, VOLsimple
                        , VideoPacketHeader_resync_marker [0], StockBlocksCb_BuffA, StockBlocksCb_BuffB
                        , StockBlocksCb_BuffC);
                    StockBlocksCr(MB_courant, InverseQuant_BlkXn + 5 * 16, VOLsimple
                        , VideoPacketHeader_resync_marker [0], StockBlocksCr_BuffA, StockBlocksCr_BuffB
                        , StockBlocksCr_BuffC);
                    VLCinverseXi_pos_prec [0] = pos [0];
                    
                    // display = Lum ; // + EDGE_SIZE + EDGE_SIZE * (VOLsimple->video_object_layer_width + 2 * EDGE_SIZE);
                    // display_img_first = imgLum_first ;
                    width = VOLsimple -> video_object_layer_width ;
                    height = VOLsimple -> video_object_layer_height ;
                    x_pos_inter = 16 * i ;
                    y_pos_inter = 16 * j ;
                    pos_X [0] = ((i + j * stride) << 4) + EDGE_SIZE + EDGE_SIZE * stride ;
                    for ( k = 0 ; k < 4 ; k++ ) {
                        dx = current_mv -> mvs [k].x ;
                        dy = current_mv -> mvs [k].y ;
                        decode_bloc_inter(k, data, VOP, VLCinverseXi_pos_prec [0], DCT3D_P, VOLsimple, width, height, stride
                            , x_pos_inter + tab_x_pos_inter [k], y_pos_inter + tab_y_pos_inter [k], dx, dy
                            ,  display_img_first [k], block_8x8
                            , VLCinverseXi_pos);
                        Stock_block_in_pict(stride, pos_X [0] + tab_pos_X [k], block_8x8,  display [k]);
                        VLCinverseXi_pos_prec [0] = VLCinverseXi_pos [0];
                    } // for Xi
                    x_pos_inter = 8 * i ;
                    y_pos_inter = 8 * j ;
                    dx = uv_dx ;
                    dy = uv_dy ;
                    width = VOLsimple -> video_object_layer_width / 2 ;
                    height = VOLsimple -> video_object_layer_height / 2 ;
                    pos_X [0] = ((i + j * (stride >> 1)) << 3) + edge_size2 + edge_size2 * (stride >> 1);
                    for ( k = 4 ; k < 6 ; k++ ) {
                        decode_bloc_inter(k, data, VOP, VLCinverseXi_pos_prec [0], DCT3D_P, VOLsimple, width, height
                            , stride >> 1, x_pos_inter, y_pos_inter, dx, dy,  display_img_first [k]
                            , block_8x8, VLCinverseXi_pos);
                        Stock_block_in_pict(stride >> 1, pos_X [0], block_8x8,  display [k]);
                        VLCinverseXi_pos_prec [0] = VLCinverseXi_pos [0];
                    } // for Xi
                    *pos_fin_vlc = VLCinverseXi_pos [0];
                    for ( k = 1 ; k < 6 ; k++ ) {
                        memset(InverseQuant_BlkXn + k * 16, 0, 16 * sizeof(short));
                        *(InverseQuant_BlkXn + k * 16) = 1024 ;
                    }
                    break ;
                default : break ;
            }
        } //fin if last_MB_P
    } //for MB
}
