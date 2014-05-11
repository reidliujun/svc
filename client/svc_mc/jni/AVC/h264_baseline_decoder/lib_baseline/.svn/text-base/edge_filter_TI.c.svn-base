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

/***********************************************************************************************************/
/******** VERTICAL LUMA ********/





/**
This function allows to execute vertical boundry 4-tap linear filter. 


@param pix Table of current frame.
@param stride Width of the current frame.
@param bS Boundary parameter strength.
@param alpha A offset threshold.
@param beta B offset threshold.

*/
void strong_vertical_luma_TI ( unsigned char pix[], const unsigned int stride,	 const unsigned char alpha, const unsigned char beta)
{

	int d;
	const unsigned int cui_BetAlpBet =_packl4(beta,beta)+(alpha<<8);// [0,beta,alpha,beta]
	const unsigned int cui_AlpDiv2Add2 = ( alpha >> 2 ) + 2;
	unsigned int ui_temp;
	unsigned int cui_tst, p0, p1, q0,q1;
	#pragma MUST_ITERATE(16,16,16)
    for( d = 0; d < 16; d++ ) {
		const unsigned int cui_P0123	= _amem4(pix-4);
        const unsigned int cui_Q3210	= _amem4(pix);
		{
			const unsigned int cui_q1q0p0p1	= _packlh2(cui_Q3210,cui_P0123); 	//q1_q0_p0_p1
			const unsigned int ui_q0p0p1	= _extu(cui_q1q0p0p1,8,8); 			//q0_p0_p1
			const unsigned int ui_q1q0p0	= _extu(cui_q1q0p0p1,0,8);			//q1_q0_p0
			const unsigned int cui_cmpgArg	= _subabs4(ui_q0p0p1,ui_q1q0p0);
			cui_tst		= _cmpgtu4( cui_BetAlpBet,cui_cmpgArg);
			p0 =  _extu(ui_q1q0p0,24,24);
        	p1 =  _extu(ui_q0p0p1,24,24);
        	q0 =  _extu(cui_Q3210,24,24);
        	q1 =  _extu(cui_Q3210,16,24);
		}
        if( cui_tst == 7 )	// (8-333) p.147
        {
            if(_subabs4( p0 , q0 ) < cui_AlpDiv2Add2) // (8-347)
            {
				const unsigned int p2 = _extu(cui_P0123,16,24);
				const unsigned int q2 = _extu(cui_Q3210,8,24);
				unsigned int ui_tmp = (_subabs4( q2,q0)<<16) + _subabs4(p2,p0);	//[0|abs(q2-q0)|0|abs(p2-p0)]
				const unsigned int cui_tst = _cmpgtu4( cui_BetAlpBet ,ui_tmp);

    	        if( cui_tst & 0x01)				// (8-347)
                {/* p0', p1', p2' */
					ui_temp =  p1 + p0 + q0 + 2;
					pix[-3] = ( _dotpu4(0x01010302,cui_P0123) + q0 + 4 ) >> 3;	// p'2 = ( 2*p3 + 3*p2 + p1 + p0 + q0 + 4 ) >> 3;
					pix[-2] = (ui_temp + p2)>> 2;									// p'1 = ( p2 + p1 + p0 + q0 + 2 ) >> 2; //4
					pix[-1] = (((ui_temp<<1)+ q1+p2 )>> 3);          	// p'0 = ( p2 + 2*p1 + 2*p0 + 2*q0 + q1 + 4 ) >> 3;//8
                } else {
                    pix[-1] = ( (p1<<1) + p0 + q1 + 2 ) >> 2; /* p0' */
                }
                if( cui_tst & 0x04)			// (8-354)
                {/* q0', q1', q2' */
					ui_temp =  p0 + q0 + q1 + 2;
					pix[2] = ( _dotpu4(0x02030101,cui_Q3210) + p0 + 4 ) >> 3;	// q'2 = ( 2*q3 + 3*q2 + q1 + q0 + p0 + 4 ) >> 3;
                    pix[1] = ( ui_temp + q2 ) >> 2;								// q'1 = ( p0 + q0 + q1 + q2 + 2 ) >> 2;
					pix[0] = ( p1 + (ui_temp<<1) + q2 ) >> 3;					// q'0 = ( p1 + 2*p0 + 2*q0 + 2*q1 + q2 + 4 ) >> 3;
				} else {
                	pix[0] = ( (q1<<1) + q0 + p1 + 2 ) >> 2; /* q0' */
                }
            }
            else
            {/* p0', q0' */
				ui_temp =  p1 + q1 + 2;
          		pix[-1] = ( p1 + p0 + ui_temp ) >> 2; // p0' = ( 2*p1 + p0 + q1 + 2 ) >> 2;
            	pix[0]  = ( q1 + q0 + ui_temp ) >> 2; // q0' = ( 2*q1 + q0 + p1 + 2 ) >> 2;
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

void weak_vertical_luma_TI(unsigned char pix[], int ystride,const unsigned char alpha	, const unsigned char beta, const unsigned char tc0)
{

	const unsigned int cui_BetAlpBet = _packl4(beta,beta)+(alpha<<8) ;//(beta<<16)+(alpha<<8)+beta;
	unsigned char d;
	#pragma MUST_ITERATE(4,4,4)
	for( d = 4; d !=0 ; d--)
	{	
		const unsigned int P0123 = _amem4(pix-4);
		const unsigned int Q3210 = _amem4(pix);
		const unsigned int ui_Q10P01= _packlh2 (Q3210,P0123); // [ q1,q0 | p0,p1 ]
		const unsigned int ui_tstArg1 = _extu(ui_Q10P01,8,8);	// q0_p0_p1
		const unsigned int ui_tstArg2 = _extu(ui_Q10P01,0,8);	// q1_q0_p0
		const unsigned int ui_comp = _cmpgtu4(cui_BetAlpBet, _subabs4(ui_tstArg1,ui_tstArg2)); //[ 0,|q0-q1|<beta,|p0-q0|<alpha,|p1-p0|<beta]
	   
	    if( ui_comp==7){// (8-333) p.146
	        unsigned int tc = tc0,ui_tmp;
			short us_tmp;
			const unsigned char p0 = _extu(P0123,0,24);
			const unsigned char p1 = ui_Q10P01;
			const unsigned char q0 = Q3210;
			const unsigned char q1 = _extu(Q3210,16,24);
			const unsigned char avg_p0q0 = _avg2(p0,q0);
			const unsigned char q2 = _extu(Q3210,8,24);
			const unsigned char p2 = _extu(P0123,16,24);
			const short ci_A = ((( p2 + avg_p0q0 ) >> 1) - p1);	//Its the better place for this code even if it's not use
			const short ci_B = ((( q2 + avg_p0q0 ) >> 1) - q1); //by the if section it'll be calculate in parallel

			ui_tmp	= (_subabs4( q2,q0)<<16) + _subabs4(p2,p0);	//ui_QxxPxx = [0|0|abs(q2-q0)|abs(p2-p0) ]
			ui_tmp	= _cmpgtu4( cui_BetAlpBet ,ui_tmp);
			
	        if( ui_tmp & 0x01 ){
	            pix[-2] = p1 + CLIP3_OPTIM(-tc0,tc0,ci_A);
	            tc++;}

	        if( ui_tmp & 0x04 ) {
	            pix[ 1] = q1 + CLIP3_OPTIM(-tc0,tc0,ci_B);
	            tc++;}
	        us_tmp = CLIP3_OPTIM( -tc, tc, (_dotpsu4(0xFF04FC01,ui_Q10P01)+ 4 ) >> 3 );
	        pix[-1] = _spacku4 (0, p0 + us_tmp );    /* p0' */
	        pix[0]  = _spacku4 (0, q0 - us_tmp );    /* q0' */
	    }
	    pix += ystride;
	} 
}




/***********************************************************************************************************/
/******** HORIZONTAL LUMA ********/


/**
This function allows to execute horizontal boundry 4-tap linear filter. 

@param pix Table of current frame.
@param xstride Width of the current frame.
@param alpha A offset threshold.
@param beta B offset threshold.
@param tc0

*/

void weak_horizontal_luma_TI(unsigned char pix[], int xstride, const unsigned char alpha, const unsigned char beta, const unsigned char tc0){

	const unsigned int ci_2tc	=  _packl4(tc0,tc0);	// [ 0 | tc0 | 0 | tc0 ]
	const int ci_2tcNeg =  _packl4((-tc0),(-tc0));		// [ 0 | -tc0 | 0 | -tc0 ]
	const unsigned int xstrideBy2 = xstride<<1;
	unsigned int  ui_Q0delta,ui_P0delta, ui_CompResult, ui_CompResult2, ui_CompResult3, ui_tc4;
	int i_P1, i_Q1, i_deltaba, i_deltadc;
	int i_tmpA,i_tmpB, i_tmpC, i_tmpD,i_tmpE;
 // Load the pixel values for 4 columns
	const unsigned int ui_4p2 = _mem4(pix - 3*xstride);		// p2d | p2c | p2b | p2a
	const unsigned int ui_4p1 = _mem4(pix - xstrideBy2);	// p1d | p1c | p1b | p1a
	const unsigned int ui_4p0 = _mem4(pix - xstride);		// p0d | p0c | p0b | p0a
	const unsigned int ui_4q0 = _mem4(pix);					// q0d | q0c | q0b | q0a
	const unsigned int ui_4q1 = _mem4(pix + xstride);		// q1d | q1c | q1b | q1a
	const unsigned int ui_4q2 = _mem4(pix + xstrideBy2);	// q2d | q2c | q2b | q2a
//---------- Masks -----------
{
	unsigned int ui_NormP0P1, ui_NormQ0Q1, ui_NormP0Q0,ui_NormP2P0,ui_NormQ2Q0;
	unsigned int ui_4Beta,ui_4Alpha;
	i_tmpA  = _packl4(beta,beta);
	ui_4Beta = i_tmpA | (i_tmpA<<8);// [ beta | beta | beta | beta ]
	i_tmpA  = _packl4(alpha,alpha);
	ui_4Alpha = i_tmpA | (i_tmpA<<8);// [ alpha | alpha | alpha | alpha ]

	ui_NormP0P1 = _subabs4(ui_4p1,ui_4p0);	// |p0d-p1d |p0c-p1c|p0b-p1b|p0a-p1a
	ui_NormP2P0 = _subabs4(ui_4p2,ui_4p0);	// |p2d-p0d |p2c-p0c|p2b-p0b|p2a-p0a
	ui_NormQ0Q1 = _subabs4(ui_4q0,ui_4q1);	// |q0d-q1d |q0c-q1c|q0b-q1b|q0a-q1a
	ui_NormP0Q0 = _subabs4(ui_4p0,ui_4q0);	// |p0d-q0d |p0c-q0c|p0b-q0b|p0a-q1a
	ui_NormQ2Q0 = _subabs4(ui_4q2,ui_4q0);	// |q2d-q0d |q2c-q0c|q2b-q0b|q2a-q1a
//      if( abs( p0 - q0 ) < alpha && abs( p1 - p0 ) < beta &&  abs( q1 - q0 ) < beta ) for the four columns
	i_tmpA = _cmpgtu4(ui_4Beta, ui_NormP0P1);	// [ (p0d-p1d)<B | (p0c-p1c)<B | (p0b-p1b)<B | (p0a-p1a)<B ]
	i_tmpB = _cmpgtu4(ui_4Alpha,ui_NormP0Q0);	// [ (p0d-q0d)<A | (p0c-q1c)<A | (p0b-q0b)<A | (p0a-q1a)<A ]
	i_tmpC = _cmpgtu4(ui_4Beta, ui_NormQ0Q1);	// [ (q0d-q1d)<B | (q0c-q1c)<B | (q0b-q1b)<B | (p0a-q1a)<B ]
	ui_CompResult = _xpnd4(i_tmpA & i_tmpB & i_tmpC);
//      if( abs( p2 - p0 ) < beta )
	i_tmpC = _cmpgtu4(ui_4Beta,ui_NormP2P0);
	ui_CompResult2 = _xpnd4(i_tmpA & i_tmpC);
//      if( abs( q2 - q0 ) < beta )
	i_tmpD = _cmpgtu4(ui_4Beta,ui_NormQ2Q0);
	ui_CompResult3 = _xpnd4(i_tmpA & i_tmpD);
//		tc
	ui_tc4 = _add4( (ui_CompResult2 & 0x01010101),(ui_CompResult3 & 0x01010101));
}
//-- unpack --
{
	unsigned int ui_Avgba,ui_Avgdc;

	const unsigned int ui_p0ba = _unpklu4(ui_4p0);	// [ 0 | p0b | 0 | p0a ]
	const unsigned int ui_p0dc = _unpkhu4(ui_4p0);	// [ 0 | p0d | 0 | p0c ]
	const unsigned int ui_p1ba = _unpklu4(ui_4p1);	// [ 0 | p1b | 0 | p1a ]
	const unsigned int ui_p1dc = _unpkhu4(ui_4p1);	// [ 0 | p1d | 0 | p1c ]
	const unsigned int ui_p2ba = _unpklu4(ui_4p2);	// [ 0 | p2b | 0 | p2a ]
	const unsigned int ui_p2dc = _unpkhu4(ui_4p2);	// [ 0 | p2d | 0 | p2c ]
	const unsigned int ui_q0ba = _unpklu4(ui_4q0);	// [ 0 | q0b | 0 | q0a ]
	const unsigned int ui_q0dc = _unpkhu4(ui_4q0);	// [ 0 | q0d | 0 | q0c ]
	const unsigned int ui_q1ba = _unpklu4(ui_4q1);	// [ 0 | q1b | 0 | q1a ]
	const unsigned int ui_q1dc = _unpkhu4(ui_4q1);	// [ 0 | q1d | 0 | q1c ]
	const unsigned int ui_q2ba = _unpklu4(ui_4q2);	// [ 0 | q2b | 0 | q1a ]
	const unsigned int ui_q2dc = _unpkhu4(ui_4q2);	// [ 0 | q2d | 0 | q2c ]
//-------------
	i_tmpA = _avgu4(ui_4p0,ui_4q0);// [ (p0d+q0d+1)>>1 | (p0c+q1c+1)>>1 | (p0b+q0b+1)>>1 | (p0a+q1a+1)>>1 ]
	ui_Avgba = _unpklu4(i_tmpA);	// [ 0 | (p0b+q0b+1)>>1 | 0 | (p0a+q1a+1)>>1 ]
	ui_Avgdc = _unpkhu4(i_tmpA);	// [ 0 | (p0d+q0d+1)>>1 | 0 | (p0c+q1c+1)>>1 ]
//---- P1 ----
	i_tmpA = _shr2(_add2(ui_Avgba,ui_p2ba),1);	// [ 0 | ((p0b+q0b+1)>>1+p2b)>>1 | 0 | ((p0a+q1a+1)>>1+p2a)>>1 ]
	i_tmpB = _shr2(_add2(ui_Avgdc,ui_p2dc),1);	// [ 0 | ((p0d+q0d+1)>>1+p2d)>>1 | 0 | ((p0c+q0c+1)>>1+p2c)>>1 ]

	i_tmpC = _sub2(i_tmpA,ui_p1ba);	// [ 0 | ((p0b+q0b+1)>>1+p2b)>>1-p1b | 0 | ((p0a+q1a+1)>>1+p2a)>>1-p1a ]
	i_tmpD = _sub2(i_tmpB,ui_p1dc);	// [ 0 | ((p0d+q0d+1)>>1+p2d)>>1-p1d | 0 | ((p0c+q0c+1)>>1+p2c)>>1-p1c ]
	//Clip3 ba
	i_tmpA = _max2(ci_2tcNeg,i_tmpC );
	i_tmpC = _min2(ci_2tc,i_tmpA);
	//Clip3 dc
	i_tmpA = _max2(ci_2tcNeg,i_tmpD );
	i_tmpD = _min2(ci_2tc,i_tmpA);
	// pack the data and add 4p1
	i_tmpB = _packl4(i_tmpD,i_tmpC);	// [ ((p0d+q0d+1)>>1+p2d)>>1-p1d |  ((p0c+q0c+1)>>1+p2c)>>1-p1c | ((p0b+q0b+1)>>1+p2b)>>1-p1b |((p0a+q1a+1)>>1+p2a)>>1-p1a ]
	i_P1 = _add4(i_tmpB,ui_4p1);
//---- Q1 ----
	i_tmpA = _shr2(_add2(ui_Avgba,ui_q2ba),1);	// [ 0 | ((p0b+q0b+1)>>1+q2b)>>1 | 0 | ((p0a+q1a+1)>>1+q2a)>>1 ]
	i_tmpB = _shr2(_add2(ui_Avgdc,ui_q2dc),1);	// [ 0 | ((p0d+q0d+1)>>1+q2d)>>1 | 0 | ((p0c+q0c+1)>>1+q2c)>>1 ]
	i_tmpC = _sub2(i_tmpA,ui_q1ba);	// [ 0 | ((p0b+q0b+1)>>1+p2b)>>1-p1b | 0 | ((p0a+q1a+1)>>1+p2a)>>1-p1a ]
	i_tmpD = _sub2(i_tmpB,ui_q1dc);	// [ 0 | ((p0d+q0d+1)>>1+p2d)>>1-p1d | 0 | ((p0c+q0c+1)>>1+p2c)>>1-p1c ]
	//Clip3 ba
	i_tmpA = _max2(ci_2tcNeg,i_tmpC );
	i_tmpC = _min2(ci_2tc,i_tmpA);
	//Clip3 dc
	i_tmpA = _max2(ci_2tcNeg,i_tmpD );
	i_tmpD = _min2(ci_2tc,i_tmpA);
	// pack the data and add 4p1
    i_tmpB = _packl4(i_tmpD,i_tmpC);	// [ ((p0d+q0d+1)>>1+p2d)>>1-p1d |  ((p0c+q0c+1)>>1+p2c)>>1-p1c | ((p0b+q0b+1)>>1+p2b)>>1-p1b |((p0a+q1a+1)>>1+p2a)>>1-p1a ]
	i_Q1 = _add4(i_tmpB,ui_4q1);
//-- Delta --     (((q0 - p0 ) << 2) + (p1 - q1) + 4) >> 3 
	i_tmpA = ui_q0ba<<2;
	i_tmpB = ui_p0ba<<2;
	i_tmpC = _sub2(i_tmpA,i_tmpB);		// [  0 | 4*(q0b-p0b) |  0 | 4*(q0a-p0a) ]
	i_tmpD = _sub2(ui_p1ba,ui_q1ba);	// [  0 | p1b-q1b |  0 | p1a-q1a ]
	i_tmpE = _add2(i_tmpD,0x00040004);	// [  0 | p1b-q1b+4 |  0 | p1a-q1a+4 ]
	i_tmpD = _add2(i_tmpC,i_tmpE);		// [  0 | 4*(q0b-p0b)+p1b-q1b+4) |  0 | 4*(q0a-p0a)+p1a-q1a+4)]
	i_deltaba = _shr2(i_tmpD,3);		// [  0 | (4*(q0b-p0b)+p1b-q1b+4))>>3 |  0 | (4*(q0a-p0a)+p1a-q1a+4))>>3]

	i_tmpA = ui_q0dc<<2;
	i_tmpB = ui_p0dc<<2;
	i_tmpC = _sub2(i_tmpA,i_tmpB);		// [  0 | 4*(q0d-p0d) |  0 | 4*(q0c-p0c) ]
	i_tmpD = _sub2(ui_p1dc,ui_q1dc);	// [  0 | p1b-q1b |  0 | p1a-q1a ]
	i_tmpE = _add2(i_tmpD,0x00040004);	// [  0 | p1b-q1b+4 |  0 | p1a-q1a+4 ]
	i_tmpD = _add2(i_tmpC,i_tmpE);		// [  0 | 4*(q0b-p0b)+p1b-q1b+4) |  0 | 4*(q0a-p0a)+p1a-q1a+4)]
	i_deltadc = _shr2(i_tmpD,3);		// [  0 | (4*(q0b-p0b)+p1b-q1b+4))>>3 |  0 | (4*(q0a-p0a)+p1a-q1a+4))>>3]

/*-- Clip3 Delta and tc --*/
	i_tmpA = _unpklu4(ui_tc4);
	i_tmpB = _add2(i_tmpA,ci_2tc);	// tc++
	i_tmpC = _sub2(0,i_tmpB); 		// i_tmpC = _neg2(i_tmpB)
	i_tmpA = _max2(i_tmpC,i_deltaba );
	i_deltaba = _min2(i_tmpB,i_tmpA);
	i_tmpD = _add2(ui_p0ba,i_deltaba);	// p0_ba + delta_ba 
	i_tmpE = _sub2(ui_q0ba,i_deltaba);	// q0_ba - delta_ba 

	i_tmpA = _unpkhu4(ui_tc4);
	i_tmpB = _add2(i_tmpA,ci_2tc);
	i_tmpC = _sub2(0,i_tmpB); //_neg2
	i_tmpA = _max2(i_tmpC,i_deltadc );
	i_deltadc = _min2(i_tmpB,i_tmpA);
	i_tmpA = _add2(i_deltadc,ui_p0dc);	// p0_dc + delta_dc 
	i_tmpB = _sub2(ui_q0dc,i_deltadc);	// q0_dc - delta_dc 
	ui_P0delta = _spacku4(i_tmpA,i_tmpD);
	ui_Q0delta = _spacku4(i_tmpB,i_tmpE);
}
/*---------- store part-----------*/
{
	const unsigned int cui_mask2 = ui_CompResult & ui_CompResult2;
	const unsigned int cui_mask3 = ui_CompResult & ui_CompResult3;

	_mem4(pix-2*xstride)= (cui_mask2 & i_P1) | (~cui_mask2 & ui_4p1);
	_mem4(pix-xstride)  = (ui_CompResult & ui_P0delta) | (~ui_CompResult & ui_4p0);
	_mem4(pix)			= (ui_CompResult & ui_Q0delta) | (~ui_CompResult & ui_4q0);
	_mem4(pix+xstride)	= (cui_mask3 & i_Q1) | (~cui_mask3 & ui_4q1);
 
}
}




/**
This function allows to execute horizontal boundry 4-tap linear filter. 

@param pix Table of current frame.
@param xstride Width of the current frame.
@param alpha A offset threshold.
@param beta B offset threshold.
*/

void strong_horizontal_luma_TI(unsigned char pix[], const int stride, const unsigned char alpha, const unsigned char beta)
{

	const unsigned int strideBy2 = stride<<1;
	const unsigned int cui_4Beta  = 0x01010101*beta;
	const unsigned int cui_4Alpha = 0x01010101*alpha;
	const unsigned int cuc_AlphDiv4 = ( alpha >> 2 ) + 2;
	const unsigned int cui_AlphaDiv2 = _packl4(cuc_AlphDiv4,cuc_AlphDiv4)|((unsigned int)(_packl4(cuc_AlphDiv4,cuc_AlphDiv4)<<8));
	unsigned int d;
	unsigned int ui_4p1dc ,ui_4p1ba;
	unsigned int ui_4p0dc ,ui_4p0ba;
	unsigned int ui_4q0dc ,ui_4q0ba;
	unsigned int ui_4q1dc ,ui_4q1ba;
	unsigned int ui_4p2dc ,ui_4p2ba;
	unsigned int ui_4p3dc ,ui_4p3ba; 
	unsigned int ui_4q2dc ,ui_4q2ba;
	unsigned int ui_4q3dc ,ui_4q3ba;
	unsigned int ui_CompResult, ui_CompResult2,ui_CompResult3;
	unsigned int ui_tempA,ui_tempB,ui_tempC;
	unsigned int ui_P0,ui_P0e,ui_P1,ui_P2,ui_Q0,ui_Q0e,ui_Q1,ui_Q2;

#pragma MUST_ITERATE(4,4,4);
	for( d = 4; d !=0; d-- )
	{
		const unsigned int ui_4p3 = _amem4(pix - (strideBy2<<1));// p3d | p3c | p3b | p3a
		const unsigned int ui_4p2 = _amem4(pix - (3*stride));	// p2d | p2c | p2b | p2a
		const unsigned int ui_4p1 = _amem4(pix - strideBy2);		// p1d | p1c | p1b | p1a
		const unsigned int ui_4p0 = _amem4(pix - stride);		// p0d | p0c | p0b | p0a
		const unsigned int ui_4q0 = _amem4(pix);					// q0d | q0c | q0b | q0a
		const unsigned int ui_4q1 = _amem4(pix + stride);		// q1d | q1c | q1b | q1a
		const unsigned int ui_4q2 = _amem4(pix + strideBy2);		// q2d | q2c | q2b | q2a
		const unsigned int ui_4q3 = _amem4(pix + 3*stride);		// q3d | q3c | q3b | q3a

		ui_4p0dc = _unpkhu4(ui_4p0);			// 0 | p0d | 0 | p0c
		ui_4p0ba = _unpklu4(ui_4p0);			// 0 | p0b | 0 | p0a
		ui_4p1dc = _unpkhu4(ui_4p1);			// 0 | p1d | 0 | p1c
		ui_4p1ba = _unpklu4(ui_4p1);			// 0 | p1b | 0 | p1a 
		ui_4p2dc = _unpkhu4(ui_4p2);			// 0 | p2d | 0 | p2c
		ui_4p2ba = _unpklu4(ui_4p2);			// 0 | p2b | 0 | p2a
		ui_4p3dc = _unpkhu4(ui_4p3);			// 0 | p3d | 0 | p3c
		ui_4p3ba = _unpklu4(ui_4p3);			// 0 | p3b | 0 | p3a
		ui_4q0dc = _unpkhu4(ui_4q0);			// 0 | q0d | 0 | q0c
		ui_4q0ba = _unpklu4(ui_4q0);			// 0 | q0b | 0 | q0a
		ui_4q1dc = _unpkhu4(ui_4q1);			// 0 | q1d | 0 | q1c
		ui_4q1ba = _unpklu4(ui_4q1);			// 0 | q1b | 0 | q1a
		ui_4q2dc = _unpkhu4(ui_4q2);			// 0 | q2d | 0 | q2c
		ui_4q2ba = _unpklu4(ui_4q2);			// 0 | q2b | 0 | q2a
		ui_4q3dc = _unpkhu4(ui_4q3);			// 0 | q3d | 0 | q3c
		ui_4q3ba = _unpklu4(ui_4q3);			// 0 | q3b | 0 | q3a
		{
			/*---------- Masks -----------*/
			unsigned int ui_NormP0P1, ui_NormQ0Q1, ui_NormP0Q0,ui_NormP2P0,ui_NormQ2Q0;
			ui_NormP0P1 = _subabs4(ui_4p1,ui_4p0);	// |p0d-p1d |p0c-p1c|p0b-p1b|p0a-p1a
			ui_NormP2P0 = _subabs4(ui_4p2,ui_4p0);	// |p2d-p0d |p2c-p0c|p2b-p0b|p2a-p0a
			ui_NormQ0Q1 = _subabs4(ui_4q0,ui_4q1);	// |q0d-q1d |q0c-q1c|q0b-q1b|q0a-q1a
			ui_NormP0Q0 = _subabs4(ui_4p0,ui_4q0);	// |p0d-q0d |p0c-q0c|p0b-q0b|p0a-q1a
			ui_NormQ2Q0 = _subabs4(ui_4q2,ui_4q0);	// |q2d-q0d |q2c-q0c|q2b-q0b|q2a-q1a
			//      if( abs( p0 - q0 ) < alpha && abs( p1 - p0 ) < beta &&  abs( q1 - q0 ) < beta ) for the four columns
			ui_tempA = _cmpgtu4(cui_4Beta, ui_NormP0P1);
			ui_tempB = _cmpgtu4(cui_4Alpha,ui_NormP0Q0);
			ui_tempC = _cmpgtu4(cui_4Beta, ui_NormQ0Q1);
			ui_CompResult = _xpnd4(ui_tempA & ui_tempB & ui_tempC);
			//      if(abs( p0-q0 ) < (( alpha >> 2 ) + 2 ))&& ( abs( p2-p0 ) < beta)
			ui_tempA = _cmpgtu4(cui_AlphaDiv2, ui_NormP0Q0);
			ui_tempB = _cmpgtu4(cui_4Beta,ui_NormP2P0);
			ui_CompResult2 = _xpnd4(ui_tempA & ui_tempB);
			//      if(abs( p0-q0 ) < (( alpha >> 2 ) + 2 ))&&  ( abs( q2-q0 ) < beta)
			ui_tempB = _cmpgtu4(cui_4Beta,ui_NormQ2Q0);
			ui_CompResult3 = _xpnd4(ui_tempA & ui_tempB);
		}
		{
			unsigned int ui_p0q0, ui_p1p0q0,ui_p2p1p0q0,ui_p0q0q1q2,ui_p0q0q1,ui_p1p0q0q1;
			unsigned int ui_P0low,ui_P0high,ui_P0lowe,ui_P0highe;
			unsigned int ui_P1low,ui_P1high;
			unsigned int ui_P2low,ui_P2high;
			unsigned int ui_Q0low,ui_Q0high,ui_Q0lowe,ui_Q0highe;
			unsigned int ui_Q1low,ui_Q1high;
			unsigned int ui_Q2low,ui_Q2high;
			/*---------- 16 bits low -----------*/
			// p'0 low
			ui_p0q0 	= _add2(ui_4p0ba,ui_4q0ba);		// = [ p0+q0 | p0+q0]
			ui_p1p0q0 	= _add2(ui_4p1ba,ui_p0q0);		// = [ p1+p0+q0 | p1+p0+q0]
			ui_p2p1p0q0 = _add2(ui_4p2ba,ui_p1p0q0);
			ui_p1p0q0q1 = _add2(ui_p1p0q0,ui_4q1ba);	// = [ p1+p0+q0 | p1+p0+q0]
			ui_tempA = _add2(ui_p2p1p0q0,ui_p1p0q0q1);	// = [ p2+2p1+2p0+2q0+q1 | p2+2p1+2p0+2q0+q1] 						
			ui_P0low = _add2(ui_tempA,0x00040004)>>3;	// = [ (p2+2p1+2p0+2q0+q1+4)>>3 | (p2+2p1+2p0+2q0+q1+4)>>3]
			// p'0 low else
			ui_tempA = _sub2(ui_p1p0q0q1,ui_4q0ba);
			ui_tempB = _add2(ui_tempA,ui_4p1ba);
			ui_P0lowe= _add2(ui_tempB,0x00020002)>>2;
			// p'1 low
			ui_P1low = _add2(ui_p2p1p0q0,0x00020002)>>2;// = [ (p2+p1+p0+q0+2)>>2 | (p2+p1+p0+q0+2)>>2]
			// p'2 low
			ui_tempB = _add2(ui_4p2ba,ui_4p3ba)<<1;		// = [ 2p2+2p3 | 2p2+2p3]
			ui_tempA = _add2(ui_tempB,ui_p2p1p0q0);		// = [ 3p2+2p3+p1+p0+q0||3p2+2p3+p1+p0+q0]
			ui_P2low = _add2(ui_tempA,0x00040004)>>3;	// = [ (p2+2p1+2p0+2q0+q1+4)>>3 | (p2+2p1+2p0+2q0+q1+4)>>3]
			// q'0 low
			ui_p0q0q1	= _add2(ui_p0q0,ui_4q1ba);		// = [ p0+q0+q1 | p0+q0+q1]
			ui_p0q0q1q2 = _add2(ui_p0q0q1,ui_4q2ba);	// = [ p0+q0+q1+q2 | p0+q0+q1+q2]
			ui_tempA = _add2(ui_p1p0q0q1,ui_p0q0q1q2);	// = [ p1+2p0+2q0+2q1+q2 | p1+2p0+2q0+2q1+q2]
			ui_Q0low = _add2(ui_tempA,0x00040004)>>3;
			// q'0 low else
			ui_tempA = _sub2(ui_p1p0q0q1,ui_4p0ba);
			ui_tempB = _add2(ui_tempA,ui_4q1ba);
			ui_Q0lowe= _add2(ui_tempB,0x00020002)>>2;
			// q'1 low
			ui_Q1low = _add2(ui_p0q0q1q2,0x00020002)>>2;	// = [ (p0+q0+q1+q2+2)>>2 | (p0+q0+q1+q2+2)>>2 ]
			// q'2 low
			ui_tempA = _add2(ui_4q2ba,ui_4q3ba)<<1;			// = [ 2q2+2q3 | 2q2+2q3]
			ui_tempB = _add2(ui_tempA,ui_p0q0q1q2);			// = [ p0+q0+q1+3q2+2q3||p0+q0+q1+3q2+2q3]
			ui_Q2low = _add2(ui_tempB,0x00040004)>>3;		// = [ (p0+q0+q1+3q2+2q3+2)>>2 | (p0+q0+q1+3q2+2q3+2)>>2 ]
			/*---------- 16 bits high -----------*/
			// p'0 high
			ui_p0q0 	= _add2(ui_4p0dc,ui_4q0dc);			// = [ p0+q0 | p0+q0]
			ui_p1p0q0 	= _add2(ui_4p1dc,ui_p0q0);			// = [ p1+p0+q0 | p1+p0+q0]
			ui_p2p1p0q0 = _add2(ui_4p2dc,ui_p1p0q0);
			ui_p1p0q0q1 = _add2(ui_p1p0q0,ui_4q1dc);		// = [ p1+p0+q0 | p1+p0+q0]
			ui_tempA	= _add2(ui_p2p1p0q0,ui_p1p0q0q1);	// = [ p2+2p1+2p0+2q0+q1 | p2+2p1+2p0+2q0+q1] 						
			ui_P0high	= _add2(ui_tempA,0x00040004)>>3;	// = [ (p2+2p1+2p0+2q0+q1+4)>>3 | (p2+2p1+2p0+2q0+q1+4)>>3]
			// p'0 high else
			ui_tempA	= _sub2(ui_p1p0q0q1,ui_4q0dc);
			ui_tempB	= _add2(ui_tempA,ui_4p1dc);
			ui_P0highe	= _add2(ui_tempB,0x00020002)>>2;
			// p'1 high
			ui_P1high	= _add2(ui_p2p1p0q0,0x00020002)>>2;	// = [ (p2+p1+p0+q0+2)>>2 | (p2+p1+p0+q0+2)>>2]
			// p'2 high
			ui_tempB	= _add2(ui_4p2dc,ui_4p3dc)<<1;		// = [ 2p2+2p3 | 2p2+2p3]
			ui_tempA	= _add2(ui_tempB,ui_p2p1p0q0);		// = [ 3p2+2p3+p1+p0+q0||3p2+2p3+p1+p0+q0]
			ui_P2high	= _add2(ui_tempA,0x00040004)>>3;	// = [ (3p2+2p3+p1+p0+q0+4)>>3 | (3p2+2p3+p1+p0+q0+4)>>3]
			// q'0 high
			ui_p0q0q1	= _add2(ui_p0q0,ui_4q1dc);			// = [ p0+q0+q1 | p0+q0+q1]
			ui_p0q0q1q2	= _add2(ui_p0q0q1,ui_4q2dc);
			ui_tempA	= _add2(ui_p1p0q0q1,ui_p0q0q1q2);	// = [ p1+2p0+2q0+2q1+q2 | p1+2p0+2q0+2q1+q2]
			ui_Q0high	= _add2(ui_tempA,0x00040004)>>3;	// = [ (p0+q0+q1+q2+2)>>2 | (p0+q0+q1+q2+2)>>2 ]
			// q'0 high else
			ui_tempA	= _sub2(ui_p1p0q0q1,ui_4p0dc);
			ui_tempB	= _add2(ui_tempA,ui_4q1dc);
			ui_Q0highe	= _add2(ui_tempB,0x00020002)>>2;
			// q'1 high
			ui_Q1high	= _add2(ui_p0q0q1q2,0x00020002)>>2;	// ui_Q0high = [ (p0+q0+q1+q2+2)>>2 | (p0+q0+q1+q2+2)>>2 ]
			// q'2 high
			ui_tempA	= _add2(ui_4q2dc,ui_4q3dc)<<1;		// = [ 2q2+2q3 | 2q2+2q3]
			ui_tempB	= _add2(ui_tempA,ui_p0q0q1q2);		// = [ p0+q0+q1+3q2+2q3||p0+q0+q1+3q2+2q3]
			ui_Q2high	= _add2(ui_tempB,0x00040004)>>3;	// = [ (p0+q0+q1+3q2+2q3+2)>>2 | (p0+q0+q1+3q2+2q3+2)>>2 ]
			/*---------- both: 16 bits low and high parts-----------*/
			ui_P0 = _packl4(ui_P0high, ui_P0low);
			ui_P0e= _packl4(ui_P0highe,ui_P0lowe);
			ui_P1 = _packl4(ui_P1high, ui_P1low);
			ui_P2 = _packl4(ui_P2high, ui_P2low);
			ui_Q0 = _packl4(ui_Q0high, ui_Q0low);
			ui_Q0e= _packl4(ui_Q0highe,ui_Q0lowe);
			ui_Q1 = _packl4(ui_Q1high, ui_Q1low);
			ui_Q2 = _packl4(ui_Q2high, ui_Q2low);
		}
		/*---------- store part-----------*/
		{
			ui_tempA = ui_CompResult & ui_CompResult2;
			ui_tempB = ui_CompResult & (~ui_CompResult2);
			ui_tempC = (~ui_tempA)	 & (~ui_tempB);

			_amem4(pix-3*stride) = (ui_tempA & ui_P2) | ((~ui_tempA) & ui_4p2);
			_amem4(pix-strideBy2)= (ui_tempA & ui_P1) | ((~ui_tempA) & ui_4p1);
			_amem4(pix-stride)   = (ui_tempA & ui_P0) | ( ui_tempC & ui_4p0)| (ui_tempB & ui_P0e);

			ui_tempA = ui_CompResult & ui_CompResult3;
			ui_tempB = ui_CompResult & (~ui_CompResult3);
			ui_tempC = (~ui_tempA)	 & (~ui_tempB);
			_amem4(pix)			= (ui_tempA & ui_Q0) | (ui_tempC & ui_4q0) | (ui_tempB & ui_Q0e);
			_amem4(pix+stride)   = (ui_tempA & ui_Q1) | ((~ui_tempA) & ui_4q1);
			_amem4(pix+strideBy2)= (ui_tempA & ui_Q2) | ((~ui_tempA) & ui_4q2);
			pix +=4;
		}
	}			
}


/***********************************************************************************************************/
/********  HORIZONTAL CHROMA *********/




/**

This function allow to execute a 4-tap linear filter. 

@param pix Table of current frame.
@param xstride horizontal incrementation. 
@param alpha Left treshold.						 
@param beta Right treshold.
@param tc0.

*/ 

void weak_horizontal_chroma_TI(unsigned char pix[], const int xstride, const unsigned char alpha	, const unsigned char beta, const unsigned char tc0)
{

	const unsigned int cui_2Beta  = _mpyu(beta ,0x0101);	// [ 0 | 0 | beta| beta ]
	const unsigned int cui_2Alpha = _mpyu(alpha,0x0101);	// [ 0 | 0 |alpha | alpha ]
	const unsigned int ci_2tc	= _packl4(tc0,tc0);
	const int ci_2tcNeg = _packl4((-tc0),(-tc0));

	int ui_tmpA, ui_tmpB , ui_tmpC,ui_tmpD,ui_tmpE,ui_tmpF,ui_CompResult,ui_delta;
	int ui_2p1ab,ui_2p0ab,ui_2q0ab,ui_2q1ab;
	unsigned int ui_NormP0P1,ui_NormQ0Q1,ui_NormP0Q0;
  
 	const unsigned int ui_2p1 = _amem2_const(pix - (xstride<<1));// [ 0 | 0 | p1b | p1a ]
 	const unsigned int ui_2p0 = _amem2_const(pix - xstride);		// [ 0 | 0 | p0b | p0a ]
 	const unsigned int ui_2q0 = _amem2_const(pix);				// [ 0 | 0 | q0b | q0a ]
	const unsigned int ui_2q1 = _amem2_const(pix + xstride);		// [ 0 | 0 | q1b | q1a ]
//  if( abs(p0-q0)< alpha && abs(p1-p0)< beta && abs(q1-q0)< beta ) for the four columns
	ui_NormP0P1 = _subabs4(ui_2p1,ui_2p0);	// [ 0 | 0 | p0b-p1b | p0a-p1a ]
	ui_NormQ0Q1 = _subabs4(ui_2q0,ui_2q1);	// [ 0 | 0 | q0b-q1b | q0a-q1a ]
	ui_NormP0Q0 = _subabs4(ui_2p0,ui_2q0);	// [ 0 | 0 | p0b-q0b | p0a-q1a ]
	ui_tmpA = _cmpgtu4(cui_2Beta, ui_NormP0P1);
	ui_tmpB = _cmpgtu4(cui_2Alpha,ui_NormP0Q0);
	ui_tmpC = _cmpgtu4(cui_2Beta, ui_NormQ0Q1);
	ui_CompResult = _xpnd4(ui_tmpA & ui_tmpB & ui_tmpC);
/*-- unpack --*/
	ui_2p1ab = _unpklu4(ui_2p1);	// [ 0 | p1b | 0 | p1a ]
	ui_2p0ab = _unpklu4(ui_2p0);	// [ 0 | p0b | 0 | p0a ]
	ui_2q0ab = _unpklu4(ui_2q0);	// [ 0 | q0b | 0 | q0a ]
	ui_2q1ab = _unpklu4(ui_2q1);	// [ 0 | q1b | 0 | q1a ]
/*-------------*/
	ui_tmpA = ui_2q0ab<<2;
	ui_tmpB = ui_2p0ab<<2;
	ui_tmpC = _sub2(ui_tmpA,ui_tmpB);		// [  0 | 4*(q0d-p0d) |  0 | 4*(q0c-p0c) ]
	ui_tmpD = _sub2(ui_2p1ab,ui_2q1ab);	// [  0 | p1b-q1b |  0 | p1a-q1a ]
	ui_tmpE = _add2(ui_tmpD,0x00040004);// [  0 | p1b-q1b+4 |  0 | p1a-q1a+4 ]
	ui_tmpD = _add2(ui_tmpC,ui_tmpE);	// [  0 | 4*(q0b-p0b)+p1b-q1b+4) |  0 | 4*(q0a-p0a)+p1a-q1a+4)]
	ui_tmpD = _shr2(ui_tmpD,3);			// [  0 | (4*(q0b-p0b)+p1b-q1b+4))>>3 |  0 | (4*(q0a-p0a)+p1a-q1a+4))>>3]
/*-- Clip3 --*/
	ui_tmpF = _max2( ci_2tcNeg,ui_tmpD );
	ui_delta = _min2(ci_2tc,ui_tmpF);
/*-- p0+delta, q0-delta --*/
	ui_tmpA = _add2(ui_2p0ab, ui_delta);
	ui_tmpB = _sub2(ui_2q0ab, ui_delta);
	ui_tmpC = _spacku4(0,ui_tmpA);
	ui_tmpD = _spacku4(0,ui_tmpB);
/*-- store --*/
	_amem2(pix-xstride)= ((ui_CompResult & ui_tmpC) | (~ui_CompResult & ui_2p0));
	_amem2(pix) = 		((ui_CompResult & ui_tmpD) | (~ui_CompResult & ui_2q0));
}




/**

This function allow to execute a 4-tap linear filter. 

@param pix Table of current frame.
@param xstride horizontal incrementation. 
@param alpha Left treshold.						 
@param beta Right treshold.

*/ 
void strong_horizontal_chroma_TI(unsigned char pix[], const int xstride	, const unsigned char alpha, const unsigned char beta)
{

	const unsigned int cui_01010101 = 0x01010101;
	const unsigned int cui_00020002 = 0x00020002;

	const unsigned int cui_4Beta  = beta*cui_01010101;
	const unsigned int cui_4Alpha = alpha*cui_01010101;
	unsigned int ui_4p0, ui_4p1,ui_4q0, ui_4q1;
	unsigned int Pix,Qix;
	unsigned int ui_NormP0P1,ui_NormQ0Q1,ui_NormP0Q0;
	unsigned int ui_AddP1Q1,ui_AddP1P0,ui_AddQ1Q0;
	unsigned int ui_4p1ba,ui_4p1dc,ui_4p0ba,ui_4p0dc,ui_4q0ba,ui_4q0dc,ui_4q1ba,ui_4q1dc ;
	unsigned int ui_CompResult;
	unsigned int ui_tmpA, ui_tmpB,ui_tmpC;
	unsigned char d;

	#pragma UNROLL(2)
    for( d = 2; d !=0; d-- )
    {
		ui_4p1	 = _amem4(pix -(xstride<<1));	// p1d | p1c | p1b | p1a
		ui_4p0	 = _amem4(pix-xstride);			// p0d | p0c | p0b | p0a
		ui_4q0	 = _amem4(pix);					// q0d | q0c | q0b | q0a
		ui_4q1	 = _amem4( pix + xstride);		// q1d | q1c | q1b | q1a

		ui_4p1dc = _unpkhu4(ui_4p1);			// 0 | p1d | 0 | p1c
		ui_4p1ba = _unpklu4(ui_4p1);			// 0 | p1b | 0 | p1a

		ui_4p0dc = _unpkhu4(ui_4p0);			// 0 | p0d | 0 | p0c
		ui_4p0ba = _unpklu4(ui_4p0);			// 0 | p0b | 0 | p0a

		ui_4q0dc = _unpkhu4(ui_4q0);			// 0 | q0d | 0 | q0c
		ui_4q0ba = _unpklu4(ui_4q0);			// 0 | q0b | 0 | q0a

		ui_4q1dc = _unpkhu4(ui_4q1);			// 0 | q1d | 0 | q1c
		ui_4q1ba = _unpklu4(ui_4q1);			// 0 | q1b | 0 | q1a

		ui_NormP0P1 = _subabs4(ui_4p1,ui_4p0);	// |p0d-p1d |p0c-p1c|p0b-p1b|p0a-p1a
		ui_NormQ0Q1 = _subabs4(ui_4q0,ui_4q1);	// |q0d-q1d |q0c-q1c|q0b-q1b|q0a-q1a
		ui_NormP0Q0 = _subabs4(ui_4p0,ui_4q0);	// |p0d-q0d |p0c-q0c|p0b-q0b|p0a-q1a
//      if( abs( p0 - q0 ) < alpha && abs( p1 - p0 ) < beta &&  abs( q1 - q0 ) < beta ) for the four columns
		ui_tmpA = _cmpgtu4(cui_4Beta, ui_NormP0P1);
		ui_tmpB = _cmpgtu4(cui_4Alpha,ui_NormP0Q0);
		ui_tmpC = _cmpgtu4(cui_4Beta, ui_NormQ0Q1);
		ui_CompResult = ui_tmpA & ui_tmpB & ui_tmpC;
		ui_CompResult = _xpnd4(ui_CompResult);
// Low 16 bits
		ui_AddP1Q1 = _add2(ui_4p1ba,ui_4q1ba); // [ p1b+q1b | p1a+q1a ]
		ui_AddP1P0 = _add2(ui_4p1ba,ui_4p0ba); // [ p1b+p0b | p1a+p0a ]
		ui_AddQ1Q0 = _add2(ui_4q1ba,ui_4q0ba); // [ p1b+q0b | q1a+q0a ]

		ui_AddP1Q1 = _add2(ui_AddP1Q1,cui_00020002);	// [ p1b+q1b+2 | p1a+q1a+2 ]

		ui_tmpA = _add2(ui_AddP1P0,ui_AddP1Q1)>>2;	// [ p1b+p0b+p1b+q1b+2 | p1a+p0a+p1a+q1a+2 ]
		ui_tmpB = _add2(ui_AddQ1Q0,ui_AddP1Q1)>>2;	// [ p1b+q0b+p1b+q1b+2 | q1a+q0a+p1a+q1a+2 ]

		Pix = _packl4(0, ui_tmpA); //[0|0| p1b+p0b+p1b+q1b+2 | p1a+p0a+p1a+q1a+2 ]
		Qix = _packl4(0, ui_tmpB); //[0|0| p1b+q0b+p1b+q1b+2 | q1a+q0a+p1a+q1a+2 ]
// hight 16 bits
		ui_AddP1Q1 = _add2(ui_4p1dc,ui_4q1dc); // [ p1d+q1d | p1c+q1c ]
		ui_AddP1P0 = _add2(ui_4p1dc,ui_4p0dc); // [ p1d+p0d | p1c+p0c ]
		ui_AddQ1Q0 = _add2(ui_4q1dc,ui_4q0dc); // [ p1d+q0d | q1c+q0c ]

		ui_AddP1Q1 = _add2(ui_AddP1Q1,cui_00020002);	// [ p1d+q1d+2 | p1c+q1c+2 ]

		ui_tmpA = _add2(ui_AddP1P0,ui_AddP1Q1)>>2;	// [ p1d+p0d+p1d+q1d+2 | p1c+p0c+p1c+q1c+2 ]
		ui_tmpB = _add2(ui_AddQ1Q0,ui_AddP1Q1)>>2;	// [ p1d+q0d+p1d+q1d+2 | q1c+q0c+p1c+q1c+2 ]

		ui_tmpA = _packl4(ui_tmpA,0); //[p1d+p0d+p1d+q1d+2 | p1c+p0c+p1c+q1c+2|0|0]
		ui_tmpB = _packl4(ui_tmpB,0); //[ p1d+q0d+p1d+q1d+2 | q1c+q0c+p1c+q1c+2 |0|0]

		Pix += ui_tmpA;
		Qix += ui_tmpB;

		_amem4(pix-xstride) = ((ui_CompResult & Pix) | (~ui_CompResult & ui_4p0));
		_amem4(pix)			= ((ui_CompResult & Qix) | (~ui_CompResult & ui_4q0));
      	pix +=4;
    }
}



/***********************************************************************************************************/
/********  VERTICAL CHROMA ********/



/**
This function allow to execute a 4-tap linear filter. 

@param pix Table of current frame.			
@param ystride Vertical incremetation.
@param alpha A treshold.						 
@param beta B treshold.
@param tc0.

	[p1, p0 || q0, q1] 
*/			   

void weak_vertical_chroma_TI(unsigned char pix[], const int ystride, const unsigned char alpha, const unsigned char beta, const unsigned char tc0)
{

	const unsigned int cui_BetAlpBet = _packl4(beta,beta)+(alpha<<8) ;//(beta<<16)+(alpha<<8)+beta;
	unsigned int ui_Q10P01, ui_tstArg1, ui_tstArg2, ui_comp,ui_mask ,p0 ,q0 ,i;
	int delta;
	unsigned short us_tmpB;
	#pragma UNROLL(2)
    for( i = 2; i !=0; i-- ) {
		ui_Q10P01= _mem4 (pix-2);
		p0 = _extu(ui_Q10P01,16,24);
		q0 = _extu(ui_Q10P01,8,24);
		ui_tstArg1 = _extu(ui_Q10P01,8,8);	// q0_p0_p1
		ui_tstArg2 = _extu(ui_Q10P01,0,8);	// q1_q0_p0
		ui_comp = _cmpgtu4(cui_BetAlpBet, _subabs4(ui_tstArg1,ui_tstArg2)); //[ 0|(q0-q1)<B| (p0-q0)<A| (p1-p0)<B]
		ui_mask = _xpnd4(ui_comp==7);

		delta = CLIP3_OPTIM( -tc0, tc0, (_dotpsu4(0xFF04FC01,ui_Q10P01) + 4) >> 3 );

		ui_tstArg1  = p0 + ( char)(ui_mask &  delta);
        ui_tstArg2  = q0 - ( char)(ui_mask &  delta); 
		us_tmpB = _spacku4(0,_packl4(ui_tstArg2,ui_tstArg1)); // [0,0,q0,p0]

		_mem2(pix-1) = us_tmpB;
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
void strong_vertical_chroma(unsigned char pix[],const unsigned int ystride, const unsigned char alpha, const unsigned char beta)
{

/* Works fine but is longer
	const unsigned int cui_BetAlpBet = _packl4(beta,beta)+(alpha<<8) ;//(beta<<16)+(alpha<<8)+beta;
	unsigned int ui_Q10P01,	ui_tstArg1, ui_tstArg2, ui_comp,ui_mask;
	unsigned short us_tmpA;
	unsigned char p0, q0, d;
	#pragma MUST_ITERATE(8,8, )
    for( d = 8; d !=0; d-- ) {
		ui_Q10P01= _mem4 (pix-2);
		p0 = (ui_Q10P01>>8);
		q0 = (ui_Q10P01>>16);

		ui_tstArg1 = _extu(ui_Q10P01,8,8);	// q0_p0_p1
		ui_tstArg2 = _extu(ui_Q10P01,0,8);	// q1_q0_p0
		ui_comp = _cmpgtu4(cui_BetAlpBet, _subabs4(ui_tstArg1,ui_tstArg2)); //[ 0|q0-q1| p0-q0| p1-p0|]
		ui_mask = _xpnd4(ui_comp==7);
		
		us_tmpA = ((ui_mask & ((_dotpu4(ui_Q10P01,0x01000102)+2) >> 2)) | (~ui_mask & p0));   	// p0' 
        us_tmpA +=((ui_mask & ((_dotpu4(ui_Q10P01,0x02010001)+2) >> 2)) | (~ui_mask & q0))<<8;  // q0' 
		_mem2(pix-1) = us_tmpA;
        pix += ystride;
    } */
    unsigned char d;
	const unsigned int cui_BetAlpBet = _packl4(beta,beta)+(alpha<<8) ;//(beta<<16)+(alpha<<8)+beta;
	#pragma MUST_ITERATE(8,8,8)
    for( d = 8; d !=0; d-- )
    {
		const unsigned int ui_Q10P01  = _mem4 (pix-2);
		const unsigned int ui_tstArg1 = _extu(ui_Q10P01,8,8);	// q0_p0_p1
		const unsigned int ui_tstArg2 = _extu(ui_Q10P01,0,8);	// q1_q0_p0

        if( _cmpgtu4(cui_BetAlpBet, _subabs4(ui_tstArg1,ui_tstArg2))==7 )
        {
            pix[-1] = (_dotpu4(ui_Q10P01,0x01000102)+2) >> 2;
            pix[0] 	= (_dotpu4(ui_Q10P01,0x02010001)+2) >> 2; 
        }
        pix += ystride;
    }
}


#endif
