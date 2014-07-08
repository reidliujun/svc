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

void    decode_bloc_intra (const int k, const unsigned char *RESTRICT data, const struct_VOP *RESTRICT VOP
    , const int pos_i, const REVERSE_EVENT *RESTRICT DCT3D_I, const struct_VOLsimple *RESTRICT VOLsimple
    , const int MB_pos, const short *RESTRICT Blk_A, const short *RESTRICT Blk_B, const short *RESTRICT Blk_C
    , short *RESTRICT InverseQuant_Blk_X, unsigned char *RESTRICT block_sat_X, int *RESTRICT pos_fin_vlc) ;
void    decode_bloc_inter (const int k, const unsigned char *RESTRICT data, const struct_VOP *RESTRICT VOP
    , const int pos_i, const REVERSE_EVENT *RESTRICT DCT3D_P, const struct_VOLsimple *RESTRICT VOLsimple
    , const int width, const int height, const int width_edged, const int x_pos_inter, const int y_pos_inter, const int dx, const int dy
    , const unsigned char *RESTRICT Refn, unsigned char *RESTRICT block_X
    , int *RESTRICT pos_fin_vlc) ;
void    SDX_Stock_block_from_prev_pict_in_block (const int k, const int width, const int *MB_pos
    , const unsigned char *RESTRICT src, unsigned char *RESTRICT dst) ;

/*!
   SynDEx intra-macroblock processing.
   
   	\param[in] data 				: bitstream (double buffer output)
   	\param[in] VOP 					: input VOP
   	\param[in] pos		 			: position in bitstream
   	\param[in] DCT3D_I 				: inverse VLC intra coefficients
   	\param[in] VOLsimple			: input VOL
   	\param[in] MB_pos				: current macroblock number
   	\param[in] StockBlockLum_BuffA	: luminance blocks neighboring block 'A'
   	\param[in] StockBlockLum_BuffB	: luminance blocks neighboring block 'B'
   	\param[in] StockBlockLum_BuffC	: luminance blocks neighboring block 'C'
   	\param[in] StockBlockLum_BuffD	: luminance blocks neighboring block 'D'
   	\param[in] StockBlockLum_BuffE	: luminance blocks neighboring block 'E'
   	\param[in] StockBlockLum_BuffA	: chrominance U block neighboring block 'A'
   	\param[in] StockBlocksCb_BuffA	: chrominance U block neighboring block 'B'
   	\param[in] StockBlocksCb_BuffB	: chrominance U block neighboring block 'C'
   	\param[in] StockBlocksCb_BuffC	: chrominance V block neighboring block 'A'
   	\param[in] StockBlocksCr_BuffA	: chrominance V block neighboring block 'B'
   	\param[in] StockBlocksCr_BuffC	: chrominance V block neighboring block 'C'
   	\param[out] X1					: processed X1 output block
   	\param[out] X2					: processed X2 output block
   	\param[out] X3					: processed X3 output block
   	\param[out] X4					: processed X4 output block
   	\param[out] XCb					: processed XCb output block
   	\param[out] XCr					: processed XCr output block 
   	\param[out] Blk_X2				: X2 block inverse quantization
   	\param[out] Blk_X3				: X3 block inverse quantization
   	\param[out] Blk_X4				: X4 block inverse quantization
   	\param[out] Blk_XCb				: XCb block inverse quantization
   	\param[out] Blk_XCr				: XCr block inverse quantization
   	\param[out] pos_fin_vlc			: position in bitstream after macroblock processing
   */
void SDX_decode_MB_intra ( const unsigned char *RESTRICT data, const struct_VOP *VOP, const int pos
    , const REVERSE_EVENT *RESTRICT DCT3D_I, const struct_VOLsimple *RESTRICT VOLsimple, const int *RESTRICT MB_pos
    , const short *RESTRICT StockBlockLum_BuffA, const short *RESTRICT StockBlockLum_BuffB
    , const short *RESTRICT StockBlockLum_BuffC, const short *RESTRICT StockBlockLum_BuffD
    , const short *RESTRICT StockBlockLum_BuffE, const short *RESTRICT StockBlocksCb_BuffA
    , const short *RESTRICT StockBlocksCb_BuffB, const short *RESTRICT StockBlocksCb_BuffC
    , const short *RESTRICT StockBlocksCr_BuffA, const short *RESTRICT StockBlocksCr_BuffB
    , const short *RESTRICT StockBlocksCr_BuffC, unsigned char *RESTRICT X1, unsigned char *RESTRICT X2
    , unsigned char *RESTRICT X3, unsigned char *RESTRICT X4, unsigned char *RESTRICT XCb, unsigned char *RESTRICT XCr
    , short *RESTRICT Blk_X2, short *RESTRICT Blk_X3, short *RESTRICT Blk_X4, short *RESTRICT Blk_XCb
    , short *RESTRICT Blk_XCr, int *pos_fin_vlc )
{
    int             k ;
    short           Blk_X1 [16];
    int             VLCinverseXi_pos [1];
    int             VLCinverseXi_pos_prec [1];
    const short     *DCpred_buffA ;
    const short     *DCpred_buffB ;
    const short     *DCpred_buffC ;
    short           *Blk_Xi ;
    unsigned char   *Xi ;
    
    VLCinverseXi_pos_prec [0] = pos ;
    DCpred_buffA = StockBlockLum_BuffA ;
    DCpred_buffB = StockBlockLum_BuffB ;
    DCpred_buffC = StockBlockLum_BuffC ;
    Blk_Xi = Blk_X1 ;
    Xi = X1 ;
    for ( k = 0 ; k < 6 ; k++ ) {
        decode_bloc_intra(k, data, VOP, VLCinverseXi_pos_prec [0], DCT3D_I, VOLsimple, MB_pos [0], DCpred_buffA
            , DCpred_buffB, DCpred_buffC, Blk_Xi, Xi, VLCinverseXi_pos);
        switch ( k ) {
            case 0 : 
                
                // pour X2
                DCpred_buffA = Blk_X1 ;
                DCpred_buffB = StockBlockLum_BuffC ;
                DCpred_buffC = StockBlockLum_BuffD ;
                Blk_Xi = Blk_X2 ;
                Xi = X2 ;
                break ;
            case 1 : 
                
                // pour X3
                DCpred_buffA = StockBlockLum_BuffE ;
                DCpred_buffB = StockBlockLum_BuffA ;
                DCpred_buffC = Blk_X1 ;
                Blk_Xi = Blk_X3 ;
                Xi = X3 ;
                break ;
            case 2 : 
                
                // pour X4
                DCpred_buffA = Blk_X3 ;
                DCpred_buffB = Blk_X1 ;
                DCpred_buffC = Blk_X2 ;
                Blk_Xi = Blk_X4 ;
                Xi = X4 ;
                break ;
            case 3 : 
                
                // pour XCb
                DCpred_buffA = StockBlocksCb_BuffA ;
                DCpred_buffB = StockBlocksCb_BuffB ;
                DCpred_buffC = StockBlocksCb_BuffC ;
                Blk_Xi = Blk_XCb ;
                Xi = XCb ;
                break ;
            case 4 : 
                
                // pour XCr
                DCpred_buffA = StockBlocksCr_BuffA ;
                DCpred_buffB = StockBlocksCr_BuffB ;
                DCpred_buffC = StockBlocksCr_BuffC ;
                Blk_Xi = Blk_XCr ;
                Xi = XCr ;
                break ;
            default : break ;
        }
        VLCinverseXi_pos_prec [0] = VLCinverseXi_pos [0];
    } // for Xi
    *pos_fin_vlc = VLCinverseXi_pos [0];
}

/*!
   SynDEx inter-macroblock processing.
   
   	\param[in] data 			: bitstream (double buffer output)
   	\param[in] VOP 				: input VOP
   	\param[in] pos				: position in bitstream
   	\param[in] DCT3D_P 			: inverse VLC inter coefficients
   	\param[in] VOLsimple		: input VOL
   	\param[in] MB_pos			: current macroblock number
   	\param[in] current_mv		: luminance blocks motion vectors
   	\param[in] uv_dx			: chrominance blocks motion vector x direction
   	\param[in] uv_dy			: chrominance blocks motion vector y direction
   	\param[in] refLum 			: image luminance Y recopy with edges
   	\param[in] refCb 			: image chrominance U recopy with edges
   	\param[in] refCr 			: image chrominance V recopy with edges
   	\param[out] X1				: processed X1 output block
   	\param[out] X2				: processed X2 output block
   	\param[out] X3				: processed X3 output block
   	\param[out] X4				: processed X4 output block
   	\param[out] XCb				: processed XCb output block
   	\param[out] XCr				: processed XCr output block 
   	\param[out] Blk_X2			: X2 block inverse quantization
   	\param[out] Blk_X3			: X3 block inverse quantization
   	\param[out] Blk_X4			: X4 block inverse quantization
   	\param[out] Blk_XCb			: XCb block inverse quantization
   	\param[out] Blk_XCr			: XCr block inverse quantization
   	\param[out] pos_fin_vlc		: position in bitstream after macroblock processing
   */
void SDX_decode_MB_inter ( const unsigned char *RESTRICT data, const struct_VOP *RESTRICT VOP, const int pos
    , const REVERSE_EVENT *RESTRICT DCT3D_P, const struct_VOLsimple *RESTRICT VOLsimple, const int *RESTRICT MB_pos
    , const vector *current_mv, const int *uv_dx, const int *uv_dy, const unsigned char *RESTRICT refLum
    , const unsigned char *RESTRICT refCb, const unsigned char *RESTRICT refCr, unsigned char *RESTRICT X1
    , unsigned char *RESTRICT X2, unsigned char *RESTRICT X3, unsigned char *RESTRICT X4, unsigned char *RESTRICT XCb
    , unsigned char *RESTRICT XCr, short *RESTRICT Blk_X2, short *RESTRICT Blk_X3, short *RESTRICT Blk_X4
    , short *RESTRICT Blk_XCb, short *RESTRICT Blk_XCr, int *pos_fin_vlc )
{
    int                 k ;
    int                 x_pos [1];
    int                 y_pos [1];
    short               Blk_X1 [16];
    int                 VLCinverseXi_pos [1];
    int                 VLCinverseXi_pos_prec [1];
    short               *Blk_Xi ;
    unsigned char       *Xi ;
    const unsigned char *imgRef ;
    int                 x_pos_inter, y_pos_inter, dx, dy, width, height, width_edged ;
    
    *x_pos = MB_pos [0] % (VOLsimple -> video_object_layer_width / 16);
    *y_pos = MB_pos [0] / (VOLsimple -> video_object_layer_width / 16);
    VLCinverseXi_pos_prec [0] = pos ;
    width = VOLsimple -> video_object_layer_width ;
    height = VOLsimple ->video_object_layer_height;
    width_edged = VOLsimple -> video_object_layer_width + 2 * EDGE_SIZE ;
    imgRef = refLum + EDGE_SIZE + EDGE_SIZE * width_edged ;
    x_pos_inter = 16 * x_pos [0];
    y_pos_inter = 16 * y_pos [0];
    dx = current_mv -> mvs [0].x ;
    dy = current_mv -> mvs [0].y ;
    Blk_Xi = Blk_X1 ;
    Xi = X1 ;
    for ( k = 0 ; k < 6 ; k++ ) {
        decode_bloc_inter(k, data, VOP, VLCinverseXi_pos_prec [0], DCT3D_P, VOLsimple, width, height, width_edged, x_pos_inter
            , y_pos_inter, dx, dy, imgRef, Xi, VLCinverseXi_pos);
        memset(Blk_Xi, 0, 16 * sizeof(short));
        *(Blk_Xi) = 1024 ;
        switch ( k ) {
            case 0 : 
                
                // pour X2
                x_pos_inter = 16 * x_pos [0] + 8 ;
                dx = current_mv -> mvs [1].x ;
                dy = current_mv -> mvs [1].y ;
                Blk_Xi = Blk_X2 ;
                Xi = X2 ;
                break ;
            case 1 : 
                
                // pour X3
                x_pos_inter = 16 * x_pos [0];
                y_pos_inter = 16 * y_pos [0] + 8 ;
                dx = current_mv -> mvs [2].x ;
                dy = current_mv -> mvs [2].y ;
                Blk_Xi = Blk_X3 ;
                Xi = X3 ;
                break ;
            case 2 : 
                
                // pour X4
                x_pos_inter = 16 * x_pos [0] + 8 ;
                dx = current_mv -> mvs [3].x ;
                dy = current_mv -> mvs [3].y ;
                Blk_Xi = Blk_X4 ;
                Xi = X4 ;
                break ;
            case 3 : 
                
                // pour XCb
                width = VOLsimple -> video_object_layer_width / 2 ;
                height = VOLsimple ->video_object_layer_height / 2 ;
                width_edged = (VOLsimple -> video_object_layer_width + 2 * EDGE_SIZE) / 2 ;
                imgRef = refCb + EDGE_SIZE / 2 + EDGE_SIZE / 2 * width_edged ;
                x_pos_inter = 8 * x_pos [0];
                y_pos_inter = 8 * y_pos [0];
                dx = uv_dx [0];
                dy = uv_dy [0];
                Blk_Xi = Blk_XCb ;
                Xi = XCb ;
                break ;
            case 4 : 
                
                // pour XCr
                imgRef = refCr + EDGE_SIZE / 2 + EDGE_SIZE / 2 * width_edged ;
                Blk_Xi = Blk_XCr ;
                Xi = XCr ;
                break ;
            default : break ;
        }
        VLCinverseXi_pos_prec [0] = VLCinverseXi_pos [0];
    } // for Xi
    *pos_fin_vlc = VLCinverseXi_pos [0];
}

void SDX_decode_MB_not_coded ( const int x_size, const int pos, const int *MB_pos, const unsigned char *RESTRICT Y_prec
    , const unsigned char *RESTRICT U_prec, const unsigned char *RESTRICT V_prec, unsigned char *RESTRICT X1
    , unsigned char *RESTRICT X2, unsigned char *RESTRICT X3, unsigned char *RESTRICT X4, unsigned char *RESTRICT XCb
    , unsigned char *RESTRICT XCr, short *RESTRICT Blk_X2, short *RESTRICT Blk_X3, short *RESTRICT Blk_X4
    , short *RESTRICT Blk_XCb, short *RESTRICT Blk_XCr, int *pos_fin_vlc )
{
    int                 k ;
    int               width ;
    short               Blk_X1 [16];
    const unsigned char *display_prec ;
    short               *Blk_Xi ;
    unsigned char       *Xi ;
    
    *pos_fin_vlc = pos ;
    display_prec = Y_prec ;
    width = x_size ;
    Blk_Xi = Blk_X1 ;
    Xi = X1 ;
    for ( k = 0 ; k < 6 ; k++ ) {
        SDX_Stock_block_from_prev_pict_in_block(k, width, MB_pos, display_prec, Xi);
        memset(Blk_Xi, 0, 16 * sizeof(short));
        *(Blk_Xi) = 1024 ;
        switch ( k ) {
            case 0 : 
                
                // pour X2
                Blk_Xi = Blk_X2 ;
                Xi = X2 ;
                break ;
            case 1 : 
                
                // pour X3
                Blk_Xi = Blk_X3 ;
                Xi = X3 ;
                break ;
            case 2 : 
                
                // pour X4
                Blk_Xi = Blk_X4 ;
                Xi = X4 ;
                break ;
            case 3 : 
                
                // pour XCb
                Blk_Xi = Blk_XCb ;
                Xi = XCb ;
                display_prec = U_prec ;
                width = x_size / 2 ;
                break ;
            case 4 : 
                
                // pour XCr
                Blk_Xi = Blk_XCr ;
                Xi = XCr ;
                display_prec = V_prec ;
                break ;
            default : break ;
        }
    } // for Xi
}
