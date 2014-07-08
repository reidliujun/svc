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

short   iDCT_data [64];
short   InvDCT_f [128];
void    VLCinverseI (const int N, const int posMB, const int pos_i, const unsigned char *RESTRICT data
    , const REVERSE_EVENT *RESTRICT DCT3D_I, const short *RESTRICT BlkA, const short *RESTRICT BlkB
    , const short *RESTRICT BlkC, const struct_VOP *RESTRICT VOP, int *RESTRICT pos_fin_vlc, short *RESTRICT PQF
    , short *RESTRICT QFpred, short *RESTRICT F00pred, short *RESTRICT QPpred, int *RESTRICT prediction_direction) ;
void    InverseACDCpred (const int type, const short *RESTRICT PQF, const short *RESTRICT QFpred, const short F00pred
    , const short QPpred, const struct_VOP *RESTRICT VOP, const int prediction_direction, short *RESTRICT QF
    , int *RESTRICT dc_scaler) ;
void    InverseQuantI (short *RESTRICT QF, const struct_VOP *RESTRICT VOP, const int dc_scaler
    , const struct_VOLsimple *RESTRICT VOLsimple, short *RESTRICT F, short *RESTRICT BlkXn) ;
void    InverseDCT (const short *RESTRICT F_in, short *RESTRICT f_out) ;
void    interpolate8x8_switch (const int width, const int height, const int edge, const int stride_src, const int x, const int y, const int dx
    , const int dy, const struct_VOP *RESTRICT VOP, const unsigned char *RESTRICT refn, unsigned char *RESTRICT cur) ;
void    VLCinverseP (const REVERSE_EVENT *RESTRICT DCT3D_P, const int pos_i, const unsigned char *RESTRICT data
    , int *RESTRICT pos_fin_vlc, short *RESTRICT PQF) ;
void    InverseQuantP (const short *RESTRICT QF, const struct_VOP *RESTRICT VOP
    , const struct_VOLsimple *RESTRICT VOLsimple, short *RESTRICT F) ;
void    block_inter (short *RESTRICT src1, unsigned char *RESTRICT src2, short *RESTRICT dst) ;
void 	InverseDCT_optim (short *RESTRICT F);
void block_sat ( const short *RESTRICT src, unsigned char *RESTRICT dst );


/*!
   intra-block processing.
   
   	\param[in] k	 				: block number in macroblock
   	\param[in] data 				: bitstream (double buffer output)
   	\param[in] VOP 					: input VOP
   	\param[in] pos_i		 			: position in bitstream
   	\param[in] DCT3D_I 				: inverse VLC intra coefficients
   	\param[in] VOLsimple			: input VOL
   	\param[in] MB_pos				: current macroblock number
   	\param[in] Blk_A				: neighboring block 'A'
   	\param[in] Blk_B				: neighboring block 'B'
   	\param[in] Blk_C				: neighboring block 'C'
   	\param[out] InverseQuant_Blk_X	: block inverse quantization
   	\param[out] block_sat_X			: processed output block 
   	\param[out] pos_fin_vlc			: position in bitstream after block processing
   */
void decode_bloc_intra ( const int k, const unsigned char *RESTRICT data, const struct_VOP *RESTRICT VOP
    , const int pos_i, const REVERSE_EVENT *RESTRICT DCT3D_I, const struct_VOLsimple *RESTRICT VOLsimple
    , const int MB_pos, const short *RESTRICT Blk_A, const short *RESTRICT Blk_B, const short *RESTRICT Blk_C
    , short *RESTRICT InverseQuant_Blk_X, unsigned char *RESTRICT block_sat_X, int *RESTRICT pos_fin_vlc )
{
    int     DCpred_prediction_direction [1];
    short   DCpred_F00pred [1];
    short   DCpred_QPpred [1];
    short   DCpred_QFpred [7];
    int     InverseACDCpred_dc_scaler [1];
    
    VLCinverseI(5 - k, MB_pos, pos_i, data, DCT3D_I, Blk_A, Blk_B, Blk_C, VOP, pos_fin_vlc, InvDCT_f, DCpred_QFpred
        , DCpred_F00pred, DCpred_QPpred, DCpred_prediction_direction);
    InverseACDCpred(k, InvDCT_f, DCpred_QFpred, DCpred_F00pred [0], DCpred_QPpred [0], VOP
        , DCpred_prediction_direction [0], iDCT_data, InverseACDCpred_dc_scaler);
    InverseQuantI(iDCT_data, VOP, InverseACDCpred_dc_scaler [0], VOLsimple, InvDCT_f, InverseQuant_Blk_X);
    InverseDCT_optim(InvDCT_f);
    block_sat(InvDCT_f, block_sat_X);
}

/*!
   inter-block processing.
   
   	\param[in] k	 				: block number in macroblock
   	\param[in] data 				: bitstream (double buffer output)
   	\param[in] VOP	 				: input VOP
   	\param[in] pos_i		 			: position in bitstream
   	\param[in] DCT3D_P 				: inverse VLC inter coefficients
   	\param[in] VOLsimple			: input VOL
   	\param[in] width				: image output x_size (Y, U or V)
   	\param[in] width_edged			: image output x_size with edges (Y, U or V)
   	\param[in] x_pos_inter			: block x position in image (Y, U or V)
   	\param[in] y_pos_inter			: block y position in image (Y, U or V)
   	\param[in] dx					: block motion vector x direction
   	\param[in] dy					: block motion vector y direction
   	\param[in] Refn					: pointer on image in image recopy with edges (Y, U or V)
   	\param[out] InverseQuant_Blk_X	: block inverse quantization
   	\param[out] block_X				: processed output block 
   	\param[out] pos_fin_vlc			: position in bitstream after block processing
   */
void decode_bloc_inter ( const int k, const unsigned char *RESTRICT data, const struct_VOP *RESTRICT VOP
    , const int pos_i, const REVERSE_EVENT *RESTRICT DCT3D_P, const struct_VOLsimple *RESTRICT VOLsimple
    , const int width, const int height, const int width_edged, const int x_pos_inter, const int y_pos_inter, const int dx, const int dy
    , const unsigned char *RESTRICT Refn, unsigned char *RESTRICT block_X
    , int *RESTRICT pos_fin_vlc )
{
    interpolate8x8_switch(width, height, (width_edged - width)/2, width_edged, x_pos_inter, y_pos_inter, dx, dy, VOP, Refn, block_X);


    if ( VOP -> cbp & (1 << (5 - k)) ) {
        VLCinverseP(DCT3D_P, pos_i, data, pos_fin_vlc, iDCT_data);
        InverseQuantP(iDCT_data, VOP, VOLsimple, InvDCT_f);
        InverseDCT_optim(InvDCT_f);
        block_inter(InvDCT_f, block_X, iDCT_data);
        block_sat(iDCT_data, block_X);
    } else {
        *pos_fin_vlc = pos_i ;
    }
}

void decode_bf_interpolate_bloc_inter ( const int k, const unsigned char *RESTRICT data, const struct_VOP *RESTRICT VOP
    , const int pos_i, const REVERSE_EVENT *RESTRICT DCT3D_P, const struct_VOLsimple *RESTRICT VOLsimple
    , const int width, const int height, const int width_edged, const int x_pos_inter, const int y_pos_inter, const int dx, const int dy
    , const int dx_b, const int dy_b, const unsigned char *RESTRICT Refn_first, const unsigned char *RESTRICT Refn_last
    , unsigned char *RESTRICT block_X, int *RESTRICT pos_fin_vlc )
{
    int             i ;
    unsigned char   block_forward [64];
    unsigned char   block_backward [64];
    
    interpolate8x8_switch(width, height,  (width_edged - width)/2, width_edged, x_pos_inter, y_pos_inter, dx, dy, VOP, Refn_first, block_forward);
    interpolate8x8_switch(width, height,  (width_edged - width)/2, width_edged, x_pos_inter, y_pos_inter, dx_b, dy_b, VOP, Refn_last, block_backward);
    for ( i = 0 ; i < 64 ; i++ ) {
        block_X [i] = (block_forward [i] + block_backward [i] + 1) / 2 ;
    }
    if ( VOP -> cbp & (1 << (5 - k)) ) {
        VLCinverseP(DCT3D_P, pos_i, data, pos_fin_vlc, iDCT_data);
        InverseQuantP(iDCT_data, VOP, VOLsimple, InvDCT_f);
        InverseDCT_optim( InvDCT_f);
        block_inter(InvDCT_f, block_X, iDCT_data);
        block_sat(iDCT_data, block_X);
    } else {
        *pos_fin_vlc = pos_i ;
    }
}
