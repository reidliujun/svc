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
/* 7.4.3.2 adapatative DC coefficient prediction */
/* 7.4.3.3 adapatative ac coefficient prediction */
/* 7.4.3.4 saturation of QF[] */
#include "mpeg.h"

#ifdef C64LIB
//#define NUM_NORM 16 // num range is -2048 2047 (11 bits + sign)

// Qpred range is 1 - 31 (5 bits)
#define NUM_NORM 10 // num range is -2047 2047 (10 bits + sign)
static  inline int divide ( int num, int den )
{
    int i, sign ;
    
    sign = num >> 31 ; // test the sign of numerator 
    num = _abs(num);
    num += den >> 1 ;
    den <<= NUM_NORM ; // align denominator 
    for ( i = 0 ; i <= NUM_NORM ; i++ ) 
        num = _subc(num, den);
    num = _extu(num, 31 - NUM_NORM, 31 - NUM_NORM);
    
    /* unsigned extraction */
    if ( sign ) 
        num = -num ;
    return (num);
}
#endif

#define DIV_DIV(A, B) ( (A) > 0 ? ((A)+((B)>>1))/(B) : ((A)-((B)>>1))/(B) )

void InverseACDCpred ( const int type, const short *RESTRICT PQF, const short *RESTRICT QFpred, const short F00pred
    , const short QPpred, const struct_VOP *RESTRICT VOP, const int prediction_direction, short *RESTRICT QF
    , int *RESTRICT dc_scaler )
{
    /***********************/
    /*  Declare variables  */
    /***********************/
    //int dc_scaler,i;
    int i ;
    
    /***********************/
    /*  Get input values   */
    /***********************/
    // dc_scaler = cal_dc_scaler_lum (VOP->quant);
    if ( type < 4 ) {
        if ( VOP -> quant > 0 && VOP -> quant < 5 ) 
            *dc_scaler = 8 ;
        else if ( VOP -> quant > 4 && VOP -> quant < 9 ) 
            *dc_scaler = 2 * VOP -> quant ;
        else if ( VOP -> quant > 8 && VOP -> quant < 25 ) 
            *dc_scaler = VOP -> quant + 8 ;
        else 
            *dc_scaler = 2 * VOP -> quant - 16 ;
    } else {
        if ( VOP -> quant > 0 && VOP -> quant < 5 ) 
            *dc_scaler = 8 ;
        else if ( VOP -> quant > 4 && VOP -> quant < 25 ) 
            *dc_scaler = (VOP -> quant + 13) / 2 ;
        else 
            *dc_scaler = VOP -> quant - 6 ;
    }
    
    /* adaptative AC coefficient prediction clause 7.4.3.3 */
#ifdef C64LIB
	for( i=0 ; i < 64 ; i+=4 )
		_mem8(QF+i) = _mem8(PQF+i);

#else
    for ( i = 0 ; i < 64 ; i++ ) 
        QF [i] = PQF [i];
#endif
   
    /* adaptative DC coefficient Prediction clause 7.4.3.2 */
    if ( VOP -> ac_pred_flag != 0 && prediction_direction == 1 ) {
        
        /* prediction de la première colonne */
#ifdef C64LIB
        for ( i = 1 ; i < 8 ; i++ ) {
            QF [i * 8] = PQF [i * 8] + divide((QFpred [i - 1] * QPpred), VOP -> quant);
            if ( QF [i] <= -2048 ) 
                QF [i] = -2048 ;
            else if ( QF [i] >= 2047 ) 
                QF [i] = 2047 ;
        }
#else 
        for ( i = 1 ; i < 8 ; i++ ) {
            if ( QFpred [i - 1] == 0 ) 
                QF [i * 8] = PQF [i * 8];
            else 
                QF [i * 8] = PQF [i * 8] + DIV_DIV((QFpred [i - 1] * QPpred), (short) VOP -> quant);
            if ( QF [i] <= -2048 ) 
                QF [i] = -2048 ;
            else if ( QF [i] >= 2047 ) 
                QF [i] = 2047 ;
        }
#endif
    } else if ( VOP -> ac_pred_flag != 0 && prediction_direction == 0 ) {
        
        /* prediction de la première ligne */
#ifdef C64LIB
        for ( i = 1 ; i < 8 ; i++ ) {
            QF [i] = PQF [i] + divide((QFpred [i - 1] * QPpred), VOP -> quant);
            if ( QF [i] <= -2048 ) 
                QF [i] = -2048 ;
            else if ( QF [i] >= 2047 ) 
                QF [i] = 2047 ;
        }
#else 
        for ( i = 1 ; i < 8 ; i++ ) {
            if ( QFpred [i - 1] == 0 ) 
                QF [i] = PQF [i];
            else 
                QF [i] = PQF [i] + DIV_DIV((QFpred [i - 1] * QPpred),(short) VOP -> quant);
            if ( QF [i] <= -2048 ) 
                QF [i] = -2048 ;
            else if ( QF [i] >= 2047 ) 
                QF [i] = 2047 ;
        }
#endif
    }
#ifdef C64LIB
    QF [0] = PQF [0] + divide(F00pred, *dc_scaler);
#else 
    QF [0] = PQF [0] + DIV_DIV(F00pred, (short) *dc_scaler);
#endif
    
    /* saturation of QF clause 7.4.3.4 */
    if ( QF [0] <= -2048 ) 
        QF [0] = -2048 ;
    else if ( QF [0] >= 2047 ) 
        QF [0] = 2047 ;
}
