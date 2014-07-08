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

void    StockBlocksLum_init (const struct_VOLsimple *RESTRICT VOLsimple, short *RESTRICT BuffX3, short *RESTRICT BuffX4) ;
void    VideoPacketHeaderI (const unsigned char *RESTRICT data, const int pos_i
    , const struct_VOLsimple *RESTRICT VOLsimple, struct_VOP *RESTRICT VOP, const int *RESTRICT MB_courant
    , struct_VOP *RESTRICT newVOP, int *RESTRICT pos_o, int *RESTRICT resync_marker, int *RESTRICT MB_num) ;
void    Param_MB_I (int pos_i, const unsigned char *RESTRICT data, const struct_VOP *RESTRICT VOP
    , struct_VOP *RESTRICT NewVOP, int *RESTRICT pos_o) ;
void    StockBlocksLum (const int pos_i, const int resync_marker, const short *RESTRICT BuffX2
    , const short *RESTRICT BuffX3, const short *RESTRICT BuffX4, const struct_VOLsimple *RESTRICT VOLsimple
    , short *RESTRICT BuffA, short *RESTRICT BuffB, short *RESTRICT BuffC, short *RESTRICT BuffD, short *RESTRICT BuffE) ;
void    StockBlocksCb (const int MBpos, const short *RESTRICT BuffX, const struct_VOLsimple *RESTRICT VOLsimple
    , const int resync_marker, short *RESTRICT BuffA, short *RESTRICT BuffB, short *RESTRICT BuffC) ;
void    StockBlocksCr (const int MBpos, const short *RESTRICT BuffX, const struct_VOLsimple *RESTRICT VOLsimple
    , const int resync_marker, short *RESTRICT BuffA, short *RESTRICT BuffB, short *RESTRICT BuffC) ;
void    decode_bloc_intra (const int k, const unsigned char *RESTRICT data, const struct_VOP *RESTRICT VOP
    , const int pos_i, const REVERSE_EVENT *RESTRICT DCT3D_I, const struct_VOLsimple *RESTRICT VOLsimple
    , const int MB_pos, const short *RESTRICT Blk_A, const short *RESTRICT Blk_B, const short *RESTRICT Blk_C
    , short *RESTRICT InverseQuant_Blk_X, unsigned char *RESTRICT block_sat_X, int *RESTRICT pos_fin_vlc) ;
void    image_setedges (unsigned char *ref_y, unsigned char *ref_u, unsigned char *ref_v
    , unsigned char *y, unsigned char *u, unsigned char *v, const unsigned int edged_width, const unsigned int height) ;
void Stock_block_in_pict ( const int width, const int dep_X, const unsigned char *RESTRICT src
    , unsigned char *RESTRICT dst );


/*!
   I-frame processing.
   
   	\param[in] data 			: bitstream (double buffer output)
   	\param[in] VOLsimple		: input VOL
   	\param[in] pos_i 		: position in bitstream
   	\param[in] VOP 				: input VOP
   	\param[in] DCT3D_I 			: inverse VLC intra coefficients
   	\param[out] pos_o		: position in bitstream after frame processing
    \param[out] address		: address in the image buffer to display
   	\param[inout] Lum				: image luminance Y output
   	\param[inout] Cb				: image chrominance U output
   	\param[inout] Cr				: image chrominance V output
   	\param[inout] keyframes				: if it is the first keyframes

   */
void decode_I_frame ( const unsigned char *RESTRICT data, const struct_VOLsimple *RESTRICT VOLsimple, int pos_i
    , struct_VOP *RESTRICT VOP, const REVERSE_EVENT *RESTRICT DCT3D_I, int *RESTRICT pos_o, int *RESTRICT address, unsigned char *RESTRICT Lum
    , unsigned char *RESTRICT Cb, unsigned char *RESTRICT Cr, int *RESTRICT keyframes )
{
    int             i, j, k ;
    int             MB_courant = 0 ;
    int             MB_number ;
    int             pos_X [1];
    int             tab_pos_X [4];
    struct_VOP      new_VOP [1];
    int             VideoPacketHeader_pos [1];
    int             MacroblockI_pos [1];
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
    unsigned char    *display [6];
    int             width ;
    const int       edge_size2 = EDGE_SIZE >> 1 ;
    const int       stride = VOLsimple -> video_object_layer_width + 2 * EDGE_SIZE ;
 
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
        *address = stride * (VOLsimple -> video_object_layer_height + 2 * EDGE_SIZE) ;
        display [0] = Lum ;
        display [1] = Lum ;
        display [2] = Lum ;
        display [3] = Lum ;
        display [4] = Cb ;
        display [5] = Cr ;
        keyframes[0]=1;
        keyframes[1]=0;
    }
    else
    {
        *address = stride * (VOLsimple -> video_object_layer_height + 2 * EDGE_SIZE) ;
        display [0] = Lum + *address;
        display [1] = Lum + *address;
        display [2] = Lum + *address;
        display [3] = Lum + *address;
        display [4] = Cb + *address / 4;
        display [5] = Cr + *address / 4;
        keyframes[0]=0;
        keyframes[1]=1;
        *address = 0;
    }

    *pos_o = pos_i ;
    StockBlocksLum_init(VOLsimple, InverseQuant_BlkXn + 2 * 16, InverseQuant_BlkXn + 3 * 16);
    for ( MB_courant = 0 ; 
        MB_courant < VOLsimple -> video_object_layer_width * VOLsimple -> video_object_layer_height / 256 ; 
        MB_courant++ ) {
			if ( MB_courant == 342)
				MB_courant += 0;
            VideoPacketHeaderI(data, *pos_o, VOLsimple, VOP, &MB_courant, new_VOP, VideoPacketHeader_pos
                , VideoPacketHeader_resync_marker, &MB_number);
            MB_courant = MB_number ;
            i = MB_courant % (VOLsimple -> video_object_layer_width / 16);
            j = MB_courant / (VOLsimple -> video_object_layer_width / 16); 
            Param_MB_I(VideoPacketHeader_pos [0], data, new_VOP, VOP, MacroblockI_pos);
            StockBlocksLum(i, VideoPacketHeader_resync_marker [0], InverseQuant_BlkXn + 1 * 16, InverseQuant_BlkXn + 2 * 16
                , InverseQuant_BlkXn + 3 * 16, VOLsimple, StockBlockLum_BuffA, StockBlockLum_BuffB, StockBlockLum_BuffC
                , StockBlockLum_BuffD, StockBlockLum_BuffE);
            StockBlocksCb(MB_courant, InverseQuant_BlkXn + 4 * 16, VOLsimple, VideoPacketHeader_resync_marker [0]
            , StockBlocksCb_BuffA, StockBlocksCb_BuffB, StockBlocksCb_BuffC);
            StockBlocksCr(MB_courant, InverseQuant_BlkXn + 5 * 16, VOLsimple, VideoPacketHeader_resync_marker [0]
            , StockBlocksCr_BuffA, StockBlocksCr_BuffB, StockBlocksCr_BuffC);
            VLCinverseXi_pos_prec [0] = MacroblockI_pos [0];
            width = VOLsimple -> video_object_layer_width ;
            pos_X [0] = ((i + j * stride) << 4) + EDGE_SIZE + EDGE_SIZE * stride ;
            for ( k = 0 ; k < 4 ; k++ ) {         
                decode_bloc_intra(k, data, VOP, VLCinverseXi_pos_prec [0], DCT3D_I, VOLsimple, MB_courant
                    , DCpred_buffA [k], DCpred_buffB [k], DCpred_buffC [k]
                , InverseQuant_BlkXn + k * 16, block_8x8, VLCinverseXi_pos);
                Stock_block_in_pict(width + 2 * EDGE_SIZE, pos_X [0] + tab_pos_X [k], block_8x8
                    , display [0]);
                VLCinverseXi_pos_prec [0] = VLCinverseXi_pos [0];
            }
            width = VOLsimple -> video_object_layer_width >> 1 ;
            pos_X [0] = ((i + j * (stride >> 1)) << 3) + edge_size2 + edge_size2 * (stride >> 1);
            for ( k = 4 ; k < 6 ; k++ ) {
                decode_bloc_intra(k, data, VOP, VLCinverseXi_pos_prec [0], DCT3D_I, VOLsimple, MB_courant
                    , DCpred_buffA [k], DCpred_buffB [k], DCpred_buffC [k]
                , InverseQuant_BlkXn + k * 16, block_8x8, VLCinverseXi_pos);
                Stock_block_in_pict(width + 2 * edge_size2, pos_X [0], block_8x8, display [k]);
                VLCinverseXi_pos_prec [0] = VLCinverseXi_pos [0];
            }
            *pos_o = VLCinverseXi_pos [0];
    }
    image_setedges( (display [0]),  (display [4]),  (display [5])
    	,  (display [0]),  (display [4]),  (display [5]), VOLsimple -> video_object_layer_width + 2 * EDGE_SIZE
        , VOLsimple -> video_object_layer_height);
}
