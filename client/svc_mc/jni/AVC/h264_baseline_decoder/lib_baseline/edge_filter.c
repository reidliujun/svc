/* FFmpeg project
 * libavcodec (h264.c)
 * H.26L/H.264/AVC/JVT/14496-10/... encoder/decoder
 * Copyright (c) 2003 Michael Niedermayer <michaelni@gmx.at>
 *
 * This file has been modified for opensvcdecoder.
 */
 
 /*****************************************************************************
   *
   *  Open SVC Decoder developped in IETR image lab
   *
   *
   *
   *              Médéric BLESTEL <mblestel@insa-rennes.Fr>
   *              Mickael RAULET <mraulet@insa-rennes.Fr>
   *			  Alain Maccari <amaccari@ens.insa-rennes.fr>               
   *              http://www.ietr.org/
   *
   *
   *
   *
   *
   * This library is free software; you can redistribute it and/or
   * modify it under the terms of the GNU Lesser General Public
   * License as published by the Free Software Foundation; either
   * version 2 of the License, or (at your option) any later version.
   *
   * This library is distributed in the hope that it will be useful,
   * but WITHOUT ANY WARRANTY; without even the implied warranty of
   * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   * Lesser General Public License for more details.
   *
   * You should have received a copy of the GNU Lesser General Public   
   * License along with this not, write to the Free Software
   * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
   *
   *
   * $Id$
   *
   **************************************************************************/

#include "edge_filter.h"
#include "clip.h"
#include <math.h>






#ifdef TI_OPTIM
#define CLIP3_OPTIM(X,Y,Z) (_min2(Y,_max2( (X),(Z) )))
#endif


//LOG_Handle	trace;
/* Deblocking filter */
const unsigned char alpha_table[52] = {
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  4,  4,  5,  6,
     7,  8,  9, 10, 12, 13, 15, 17, 20, 22,
    25, 28, 32, 36, 40, 45, 50, 56, 63, 71,
    80, 90,101,113,127,144,162,182,203,226,
    255, 255
};

const unsigned char beta_table[52] = {
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  2,  2,  2,  3,
     3,  3,  3,  4,  4,  4,  6,  6,  7,  7,
     8,  8,  9,  9, 10, 10, 11, 11, 12, 12,
    13, 13, 14, 14, 15, 15, 16, 16, 17, 17,
    18, 18
};

const unsigned char tc0_table[52][3] = {
    { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 },
    { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 },
    { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 1 },
    { 0, 0, 1 }, { 0, 0, 1 }, { 0, 0, 1 }, { 0, 1, 1 }, { 0, 1, 1 }, { 1, 1, 1 },
    { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 2 }, { 1, 1, 2 }, { 1, 1, 2 },
    { 1, 1, 2 }, { 1, 2, 3 }, { 1, 2, 3 }, { 2, 2, 3 }, { 2, 2, 4 }, { 2, 3, 4 },
    { 2, 3, 4 }, { 3, 3, 5 }, { 3, 4, 6 }, { 3, 4, 6 }, { 4, 5, 7 }, { 4, 5, 8 },
    { 4, 6, 9 }, { 5, 7,10 }, { 6, 8,11 }, { 6, 8,13 }, { 7,10,14 }, { 8,11,16 },
    { 9,12,18 }, {10,13,20 }, {11,15,23 }, {13,17,25 }
};




/***********************************************************************************************************/
/******** VERTICAL LUMA ********/



/**
These functions allow to execute the vertical boundary filter for the luminance. 

@param pix Table of current frame.
@param stride Width of the current frame.
@param bS Boundary parameter strength.
@param index_a Variable used to determine the offset threshold.
@param index_b Variable used to determine the offset threshold.

*/
void filter_mb_edgev(unsigned char pix[],const  int stride, char bS[4], int index_a, int index_b )
{
    const unsigned char alpha = alpha_table[index_a];
	const unsigned char beta  = beta_table[index_b]; //beta_table[clip( qp + Slice -> beta_offset, 0, 51 )];
	const unsigned int strideBy4 = stride << 2; 
	const unsigned int strideBy8 = stride << 3;
	const unsigned int strideBy12 = strideBy8 + strideBy4;

    if( bS[0] < 4 ) {
		if (bS[0]) weak_vertical_luma(pix			 , stride, alpha, beta, (tc0_table[index_a][bS[0] - 1]));
		if (bS[1]) weak_vertical_luma(pix + strideBy4, stride, alpha, beta, (tc0_table[index_a][bS[1] - 1]));
		if (bS[2]) weak_vertical_luma(pix + strideBy8, stride, alpha, beta, (tc0_table[index_a][bS[2] - 1]));
		if (bS[3]) weak_vertical_luma(pix + strideBy12, stride, alpha, beta, (tc0_table[index_a][bS[3] - 1]));
    } else {
        /* 16px edge length, because bS=4 is triggered by being at
         * the edge of an intra MB, so all 4 bS are the same */
		strong_vertical_luma(pix, stride, alpha, beta);
	}
}




/**
This function allows to execute vertical boundry 4-tap linear filter. 


@param pix Table of current frame.
@param stride Width of the current frame.
@param bS Boundary parameter strength.
@param alpha A offset threshold.
@param beta B offset threshold.

*/
void strong_vertical_luma_C ( unsigned char pix[], const unsigned int stride,	 const unsigned char alpha, const unsigned char beta)
{

	int d;
	for( d = 0; d < 16; d++ ) {
		const short p0 = pix[-1];
		const short p1 = pix[-2];
		const short q0 = pix[0];
		const short q1 = pix[1];


		if( abs( p0 - q0 ) < alpha && abs( p1 - p0 ) < beta &&	abs( q1 - q0 ) < beta ) {
			const short p2 = pix[-3];
			const short q2 = pix[2];

			if(abs( p0 - q0 ) < (( alpha >> 2 ) + 2 )){
				if( abs( p2 - p0 ) < beta)	{
					const short p3 = pix[-4];
					/* p0', p1', p2' */
					pix[-1] = ( p2 + 2*p1 + 2*p0 + 2*q0 + q1 + 4) >> 3;
					pix[-2] = ( p2 + p1 + p0 + q0 + 2 ) >> 2;
					pix[-3] = ( 2*p3 + 3*p2 + p1 + p0 + q0 + 4 ) >> 3;
				} else {
					/* p0' */
					pix[-1] = ( 2*p1 + p0 + q1 + 2 ) >> 2;
				}
				if( abs( q2 - q0 ) < beta)
				{
					const int q3 = pix[3];
					/* q0', q1', q2' */
					pix[0] = ( p1 + 2*p0 + 2*q0 + 2*q1 + q2 + 4 ) >> 3;
					pix[1] = ( p0 + q0 + q1 + q2 + 2 ) >> 2;
					pix[2] = ( 2*q3 + 3*q2 + q1 + q0 + p0 + 4 ) >> 3;
				} else {
					/* q0' */
					pix[0] = ( 2*q1 + q0 + p1 + 2 ) >> 2;
				}
			}else{
				/* p0', q0' */
				pix[-1] = ( 2*p1 + p0 + q1 + 2 ) >> 2;
				pix[ 0] = ( 2*q1 + q0 + p1 + 2 ) >> 2;
			}
		}
		pix += stride;
	}
}



/**
This function allows to execute vertical boundry 4-tap linear filter. 


@param pix Table of current frame.
@param ystride Stride of the incremental.
@param bS Boundary parameter strength.
@param alpha A offset threshold.
@param beta B offset threshold.

*/

void weak_vertical_luma_C(unsigned char pix[], const int ystride,const unsigned char alpha	, const unsigned char beta, const unsigned char tc0)
{
    int d;
    for( d = 0; d < 4; d++ ) {
	    const unsigned char p1 = pix[-2];
        const unsigned char p0 = pix[-1];
        const unsigned char q0 = pix[0];
        const unsigned char q1 = pix[1];
       
        if( abs(p0 - q0) < alpha && abs(p1 - p0) < beta && abs(q1 - q0) < beta )
        {
           
            int i_delta;
			int inter_1;
			int inter_2;
			int tc = tc0;
			const unsigned char p2 = pix[-3];
			const unsigned char q2 = pix[2];
			const short addp0_q0 = (p0 + q0 + 1) >> 1;
			const short addp2 = (p2 + addp0_q0) >> 1;
			const short addq2 = (q2 + addp0_q0) >> 1;

            if( abs( p2 - p0 ) < beta ) {
                pix[-2] = p1 + CLIP3(- tc0, tc0, addp2 - p1);
                tc++;
            }

            if( abs( q2 - q0 ) < beta ) {
                pix[1] = q1 + CLIP3(-tc0, tc0, addq2 - q1);
                tc++;
            }

			inter_1 = (q0 - p0 ) << 2;
			inter_2 = (p1 - q1) + 4;
            i_delta = CLIP3(  -tc, tc, (inter_1 + inter_2) >> 3 );
            pix[-1] = (unsigned char ) CLIP255( p0 + i_delta );    /* p0' */
            pix[0]  = (unsigned char ) CLIP255( q0 - i_delta );    /* q0' */
        }
        pix += ystride;
    } 
}




/***********************************************************************************************************/
/******** HORIZONTAL LUMA ********/

/**
These functions allow to execute the horizontal filter for the luminance. 

@param pix Table of current frame.
@param stride Width of the current frame.
@param bS Boundary parameter strength.
@param index_a Variable used to determine the offset threshold.
@param index_b Variable used to determine the offset threshold.

*/
void filter_mb_edgeh( unsigned char pix[], const int stride, char bS[4], int index_a, int index_b) {
    const unsigned char alpha = alpha_table[index_a];
	const unsigned char beta  = beta_table[index_b];

    if( bS[0] < 4 )  {
		if (bS[0]) weak_horizontal_luma(pix, stride, alpha, beta, tc0_table[index_a][bS[0] - 1]);
		if (bS[1]) weak_horizontal_luma(pix + 4, stride, alpha, beta, tc0_table[index_a][bS[1] - 1]);
		if (bS[2]) weak_horizontal_luma(pix + 8, stride, alpha, beta, tc0_table[index_a][bS[2] - 1]);
		if (bS[3]) weak_horizontal_luma(pix + 12, stride, alpha, beta, tc0_table[index_a][bS[3] - 1]);

    } else {    /* 16px edge length, see filter_mb_edgev */
		strong_horizontal_luma(pix, stride, alpha,beta );
    }
}





/**
This function allows to execute horizontal boundry 4-tap linear filter. 

@param pix Table of current frame.
@param xstride Width of the current frame.
@param alpha A offset threshold.
@param beta B offset threshold.
@param tc0

*/

void weak_horizontal_luma_C(unsigned char pix[], const int xstride, const unsigned char alpha, const unsigned char beta, const unsigned char tc0){


	int d;
	for( d = 0; d < 4; d++ ) {
	    const short p1 = pix[-2*xstride];
        const short p0 = pix[-xstride];
        const short q0 = pix[0];
        const short q1 = pix[xstride];
       
        if( (abs(p0 - q0) < alpha) && (abs(p1 - p0) < beta) && (abs(q1 - q0) < beta) )     {
            
			int i_delta, inter_1, inter_2;
			int tc = tc0;
			const short p2 = pix[-3*xstride];
			const short q2 = pix[2*xstride];
			const short addp0_q0 = (p0 + q0 + 1) >> 1;
			const short addp2 = (p2 + addp0_q0) >> 1;
			const short addq2 = (q2 + addp0_q0) >> 1;

           if( abs( p2 - p0 ) < beta ) {
                pix[-2*xstride] = p1 + CLIP3(-tc0, tc0, addp2 - p1 );
                tc++;
            }

            if( abs( q2 - q0 ) < beta ) {
                pix[xstride] = q1 + CLIP3(-tc0, tc0, addq2 - q1 );
                tc++;
            }

			inter_1 = (q0 - p0 ) << 2;
			inter_2 = (p1 - q1) + 4;
            i_delta = CLIP3(  -tc, tc, (inter_1 +  inter_2) >> 3 );
            pix[-xstride] = (unsigned char ) CLIP255( p0 + i_delta );    /* p0' */
            pix[0]        = (unsigned char ) CLIP255( q0 - i_delta );    /* q0' */
        }
        pix += 1;
    }
}





/**
This function allows to execute horizontal boundry 4-tap linear filter. 

@param pix Table of current frame.
@param xstride Width of the current frame.
@param alpha A offset threshold.
@param beta B offset threshold.
*/

void strong_horizontal_luma_C(unsigned char pix[], const int stride, const unsigned char alpha, const unsigned char beta)
{
    int d;

    /* 16px edge length, see filter_mb_edgev */
    for( d = 0; d < 16; d++ )  {
        const int p0 = pix[-stride];
        const int p1 = pix[-2*stride];
        const int q0 = pix[0];
        const int q1 = pix[stride];

        if( abs( p0 - q0 ) < alpha &&
            abs( p1 - p0 ) < beta &&
            abs( q1 - q0 ) < beta )     {

			const int p2 = pix[-3*stride];
			const int q2 = pix[stride << 1];
            const int p3 = pix[-4*stride];
            const int q3 = pix[ 3*stride];

            if(abs( p0 - q0 ) < (( alpha >> 2 ) + 2 )){
                if( abs( p2 - p0 ) < beta) {
                    /* p0', p1', p2' */
                    pix[-stride] = (unsigned char ) (( p2 + 2*p1 + 2*p0 + 2*q0 + q1 + 4 ) >> 3);
                    pix[-2*stride] = (unsigned char ) (( p2 + p1 + p0 + q0 + 2 ) >> 2);
                    pix[-3*stride] = (unsigned char ) (( 2*p3 + 3*p2 + p1 + p0 + q0 + 4 ) >> 3);
                } else {
                    /* p0' */
                    pix[-1*stride] = (unsigned char ) (( 2*p1 + p0 + q1 + 2 ) >> 2);
                }
                if( abs( q2 - q0 ) < beta) {
                    /* q0', q1', q2' */
                    pix[0] = (unsigned char ) (( p1 + 2*p0 + 2*q0 + 2*q1 + q2 + 4 ) >> 3);
                    pix[stride] = (unsigned char ) (( p0 + q0 + q1 + q2 + 2 ) >> 2);
                    pix[stride << 1] = (unsigned char ) (( 2*q3 + 3*q2 + q1 + q0 + p0 + 4 ) >> 3);
                } else {
                    /* q0' */
                    pix[0] = (unsigned char ) (( 2*q1 + q0 + p1 + 2 ) >> 2);
                }
            }else{
                /* p0', q0' */
                pix[-stride] = (unsigned char ) (( 2*p1 + p0 + q1 + 2 ) >> 2);
                pix[0] = (unsigned char ) (( 2*q1 + q0 + p1 + 2 ) >> 2);
            }
        }
        pix++;
    }
}




/***********************************************************************************************************/
/********  HORIZONTAL CHROMA *********/



/**

This function allow to execute a 4-tap linear filter. 

@param pix Table of current frame.					
@param alpha Left treshold						 
@param beta Right treshold
@param bS Boundary parameter strength.


		[p1 
		 p0 
		 q0 
		 q1]
*/ 

void filter_mb_edgech(unsigned char pix[],  int index_a, int index_b, const int stride, char bS[4])
{
    const unsigned char alpha = alpha_table[index_a];
	const unsigned char beta  = beta_table[index_b]; //beta_table[clip( qp + Slice -> beta_offset, 0, 51 )];

    if( bS[0] < 4 )  {
		if (bS[0]) weak_horizontal_chroma(pix, stride, alpha, beta, tc0_table[index_a][bS[0] - 1] + 1);
		if (bS[1]) weak_horizontal_chroma(pix+2, stride,alpha,beta, tc0_table[index_a][bS[1] - 1] + 1);
		if (bS[2]) weak_horizontal_chroma(pix+4, stride,alpha,beta, tc0_table[index_a][bS[2] - 1] + 1);
		if (bS[3]) weak_horizontal_chroma(pix+6, stride,alpha,beta, tc0_table[index_a][bS[3] - 1] + 1);
    } else {
		strong_horizontal_chroma(pix,stride, alpha, beta);
    }
}





/**

This function allow to execute a 4-tap linear filter. 

@param pix Table of current frame.
@param xstride horizontal incrementation. 
@param alpha Left treshold.						 
@param beta Right treshold.
@param tc0.

*/ 

void weak_horizontal_chroma_C(unsigned char pix[], const int xstride, const unsigned char alpha	, const unsigned char beta, const unsigned char tc0)
{

    int d;
    const int tc = tc0;
    for( d = 0; d < 2; d++ ) {
        const int p0 = pix[-1*xstride];	const int p1 = pix[-2*xstride];
        const int q0 = pix[0];			const int q1 = pix[1*xstride];

        if( abs(p0 - q0) < alpha && abs(p1 - p0) < beta && abs(q1 - q0) < beta )      {
            int delta = CLIP3(  -tc, tc, (((q0 - p0 ) << 2) + (p1 - q1) + 4) >> 3 );
            pix[-xstride] = (unsigned char ) CLIP255( p0 + delta );    /* p0' */
            pix[0]        = (unsigned char ) CLIP255( q0 - delta );    /* q0' */
        }
        pix ++;
    }

}




/**

This function allow to execute a 4-tap linear filter. 

@param pix Table of current frame.
@param xstride horizontal incrementation. 
@param alpha Left treshold.						 
@param beta Right treshold.

*/ 
void strong_horizontal_chroma_C(unsigned char pix[], const int xstride	, const unsigned char alpha, const unsigned char beta)
{
	int d;
    for( d = 0; d < 8; d++ ) {
	    const unsigned short p1 = pix[-2*xstride];
		const unsigned short p0 = pix[-1*xstride];
        const unsigned short q0 = pix[0]; 
		const unsigned short q1 = pix[1*xstride];

        if( abs(p0 - q0) < alpha && abs(p1 - p0) < beta && abs(q1 - q0) < beta ) {
            pix[-xstride] = (unsigned char ) (( 2*p1 + p0 + q1 + 2 ) >> 2);   /* p0' */
            pix[0]        = (unsigned char ) (( 2*q1 + q0 + p1 + 2 ) >> 2);   /* q0' */
        }
        pix ++;
    }
}



/***********************************************************************************************************/
/********  VERTICAL CHROMA ********/



/**
This function allow to execute a 4-tap linear filter. 

@param pix Table of current frame.			
@param index_a Variable used to determine the offset threshold.
@param index_b Variable used to determine the offset threshold.
@param stride Width of the current frame.		 
@param bS Boundary parameter strength.

	[p1, p0 || q0, q1] 
*/			   

void filter_mb_edgecv(unsigned char pix[],  int index_a, int index_b, const int stride, char bS[4])
{
    const unsigned char alpha = alpha_table[index_a];
	const unsigned char beta  = beta_table[index_b];
	const unsigned int strideBy2 = stride << 1;
	const unsigned int strideBy4 = stride << 2 ;
	const unsigned int strideBy6 = strideBy2 + strideBy4 ;

	if( bS[0] < 4 ){
		if (bS[0]) weak_vertical_chroma(pix			   , stride,alpha,beta,tc0_table[index_a][bS[0] - 1] + 1);
		if (bS[1]) weak_vertical_chroma(pix + strideBy2, stride,alpha,beta,tc0_table[index_a][bS[1] - 1] + 1);
		if (bS[2]) weak_vertical_chroma(pix + strideBy4, stride,alpha,beta,tc0_table[index_a][bS[2] - 1] + 1);
		if (bS[3]) weak_vertical_chroma(pix + strideBy6, stride,alpha,beta,tc0_table[index_a][bS[3] - 1] + 1);
	} else {
		strong_vertical_chroma(pix, stride, alpha, beta);    
	}

}




/**
This function allow to execute a 4-tap linear filter. 

@param pix Table of current frame.			
@param ystride Vertical incremetation.
@param alpha A treshold.						 
@param beta B treshold.
@param tc0.

	[p1, p0 || q0, q1] 
*/			   

void weak_vertical_chroma_C(unsigned char pix[], const int ystride, const unsigned char alpha, const unsigned char beta, const unsigned char tc0)
{
    int d;
    for( d = 0; d < 2; d++ ) {
        const short p0 = pix[-1];
        const short p1 = pix[-2];
        const short q0 = pix[0];
        const short q1 = pix[1];

        if( abs( p0 - q0 ) < alpha &&
            abs( p1 - p0 ) < beta &&
            abs( q1 - q0 ) < beta ) {

            short delta = CLIP3(  -tc0, tc0, (((q0 - p0 ) << 2) + (p1 - q1) + 4) >> 3 );

            pix[-1] = (unsigned char) CLIP255( p0 + delta );    /* p0' */
            pix[0]  = (unsigned char) CLIP255( q0 - delta );    /* q0' */
        }
        pix += ystride;
    }

}


/**
This function allow to execute a 4-tap linear filter. 

@param pix Table of current frame.			
@param ystride Vertical incremetation.
@param alpha A treshold.						 
@param beta B treshold.


	[p1, p0 || q0, q1] 
*/		
void strong_vertical_chroma_C(unsigned char pix[],const unsigned int ystride, const unsigned char alpha, const unsigned char beta)
{
    int d;
    for( d = 0; d < 8; d++ ) {
        const int p0 = pix[-1]; const int p1 = pix[-2];
        const int q0 = pix[0]; 	const int q1 = pix[1];

        if( abs(p0-q0)< alpha && abs(p1-p0)< beta && abs(q1-q0)< beta )       {
            pix[-1] = (unsigned char) (( 2*p1 + p0 + q1 + 2 ) >> 2);   /* p0' */
            pix[0] 	= (unsigned char) (( 2*q1 + q0 + p1 + 2 ) >> 2);   /* q0' */
        }
        pix += ystride;
    }
}
