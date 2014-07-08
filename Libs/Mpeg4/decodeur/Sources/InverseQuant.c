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
/* clause 7.4.4 Inverse quantisation */
/* on est dans le cas de blocks codés intra et on utilise la matrice par défaut */
#include "mpeg.h"
#include "stdio.h"

//#define ABS(X) ((X)<0 ? -(X) : (X))
const int   Wintra [64]
    = { 8, 17, 18, 19, 21, 23, 25, 27, 17, 18, 19, 21, 23, 25, 27, 28, 20, 21, 22, 23, 24, 26, 28, 30, 21, 22, 23, 24
        , 26, 28, 30, 32, 22, 23, 24, 26, 28, 30, 32, 35, 23, 24, 26, 28, 30, 32, 35, 38, 25, 26, 28, 30, 32, 35, 38, 41
        , 27, 28, 30, 32, 35, 38, 41, 45 };

void InverseQuantI ( short *RESTRICT QF, const struct_VOP *RESTRICT VOP, const int dc_scaler
    , const struct_VOLsimple *RESTRICT VOLsimple, short *RESTRICT F, short *RESTRICT BlkXn )
{
    int i ;
    
    if ( VOLsimple -> quant_type == 0 ) 
    // Quantification H_263
    {
        const int   quant_add = VOP -> quant & 1 ? VOP -> quant : VOP -> quant - 1 ;
#ifdef TI_OPTIM
        {
            const int   nb = 31 - (VOLsimple -> bits_per_pixel + 3);
            const int   quant_m_2 = VOP -> quant << 1 ;
            for ( i = 0 ; i < 64 ; i += 2 ) {
                int         b = _amem4(QF + i);
                const int   a = _abs2(b);
                int         tmp1 = _mpy(a, quant_m_2);
                int         tmp2 = _mpyhl(a, quant_m_2);
                if ( _ext(b, 16, 16) != 0 ) 
                    tmp1 += quant_add ;
                if ( _ext(b, 0, 16) != 0 ) 
                    tmp2 += quant_add ;
                if ( _ext(b, 16, 16) < 0 ) 
                    tmp1 = -tmp1 ;
                if ( _ext(b, 0, 16) < 0 ) 
                    tmp2 = -tmp2 ;
                tmp1 = _sshl(tmp1, nb) >> nb ;
                tmp2 = _sshl(tmp2, nb) >> nb ;
                _amem4(F + i) = _pack2(tmp2, tmp1);
            }
            F [0] = QF [0] * dc_scaler ;
            F [0] = _sshl(F [0], nb) >> nb ;
        }
#else 
        {
            const int   quant_m_2 = VOP -> quant << 1 ;
            const int   nb = 1 << (VOLsimple -> bits_per_pixel + 3) ;
            for ( i = 0 ; i < 64 ; i++ ) {
                if ( QF [i] == 0 ) 
                    F [i] = 0 ;
                else if ( QF [i] < 0 ) 
                    F [i] = (short) (quant_m_2 * QF [i] - quant_add) ;
                else 
                    F [i] = (short) (quant_m_2 * QF [i] + quant_add) ;
                if ( F [i] <= -nb ) 
                    F [i] = (short) -nb ;
                else if ( F [i] >= nb - 1 ) 
                    F [i] = (short) nb - 1 ;
            }
            F [0] = (short) (QF [0] * dc_scaler) ;
            if ( F [0] <= -nb ) 
                F [0] = (short) -nb ;
            else if ( F [0] >= nb - 1 ) 
                F [0] = (short) nb - 1 ;
        }
#endif
    }
    else 
    {
#ifdef C64LIB
        {
            int         sum = 0 ;
            const int   nb = 31 - (VOLsimple -> bits_per_pixel + 3);
            for ( i = 1 ; i < 64 ; i++ ) {
                if (QF [i]<0)
                    F [i] = -( (-QF [i]) * Wintra [i] * VOP -> quant >> 3) ;
                else
                    F [i] = ( (QF [i]) * Wintra [i] * VOP -> quant >> 3) ;

                F [i] = _sshl(F [i], nb) >> nb ;
            }
            F [0] = QF [0] * dc_scaler ;
            F [0] = _sshl(F [0], nb) >> nb ;
            sum += F [0];
        }
#else 
        {
            int         sum = 0 ;
            const int   nb = 2 << (VOLsimple -> bits_per_pixel + 2) ;
            for ( i = 1 ; i < 64 ; i++ ) {
                if (QF [i]<0)
                    F [i] = -( (-QF [i]) * Wintra [i] * VOP -> quant >> 3) ;
                else
                    F [i] = ( (QF [i]) * Wintra [i] * VOP -> quant >> 3) ;
                if ( F [i] <= -nb ) 
                    F [i] = (short) -nb ;
                else if ( F [i] >= nb - 1 ) 
                    F [i] = (short) nb - 1 ;
            }
            F [0] = (short) (QF [0] * dc_scaler) ;
            if ( F [0] <= -nb ) 
                F [0] = (short) -nb ;
            else if ( F [0] >= nb - 1 ) 
                F [0] = (short) nb - 1 ;
        }
#endif
    }
    for ( i = 0 ; i < 8 ; i++ ) {
        BlkXn [i + 7] = QF [i];
        BlkXn [i] = QF [i * 8];
    }
    BlkXn [0] = F [0];
    BlkXn [15] = (short) VOP -> quant ;
}

const int   Winter [64]
    = { 16, 17, 18, 19, 20, 21, 22, 23, 17, 18, 19, 20, 21, 22, 23, 24, 18, 19, 20, 21, 22, 23, 24, 25, 19, 20, 21, 22
        , 23, 24, 26, 27, 20, 21, 22, 23, 25, 26, 27, 28, 21, 22, 23, 24, 26, 27, 28, 30, 22, 23, 24, 26, 27, 28, 30, 31
        , 23, 24, 25, 27, 28, 30, 31, 33 };

void InverseQuantP ( const short *RESTRICT QF, const struct_VOP *RESTRICT VOP
    , const struct_VOLsimple *RESTRICT VOLsimple, short *RESTRICT F )
{
    int i ;
    
    if ( VOLsimple -> quant_type == 0 ) 
    // Quantification H_263
    {
        const int   quant_add = VOP -> quant & 1 ? VOP -> quant : VOP -> quant - 1 ;
#ifdef C64LIB
        {
            const int   nb = 31 - (VOLsimple -> bits_per_pixel + 3);
            const int   quant_m_2 = VOP -> quant << 1 ;
            for ( i = 0 ; i < 64 ; i += 2 ) {
                int         b = _amem4((void *)(QF + i));
                const int   a = _abs2(b);
                int         tmp1 = _mpy(a, quant_m_2);
                int         tmp2 = _mpyhl(a, quant_m_2);
                if ( _ext(b, 16, 16) != 0 ) 
                    tmp1 += quant_add ;
                if ( _ext(b, 0, 16) != 0 ) 
                    tmp2 += quant_add ;
                if ( _ext(b, 16, 16) < 0 ) 
                    tmp1 = -tmp1 ;
                if ( _ext(b, 0, 16) < 0 ) 
                    tmp2 = -tmp2 ;
                tmp1 = _sshl(tmp1, nb) >> nb ;
                tmp2 = _sshl(tmp2, nb) >> nb ;
                _amem4(F + i) = _pack2(tmp2, tmp1);
            }
        }
#else 
        const int   quant_m_2 = VOP -> quant << 1 ;
        for ( i = 0 ; i < 64 ; i++ ) {
            if ( QF [i] < 0 ) 
                F [i] = (short) (quant_m_2 * QF [i] - quant_add) ;
            else 
                F [i] = (short) (quant_m_2 * QF [i] + quant_add) ;
        }
        for ( i = 0 ; i < 64 ; i++ ) {
            if ( QF [i] == 0 ) 
                F [i] = 0 ;
        }
        {
            const int   nb = 2 << (VOLsimple -> bits_per_pixel + 2) ;
            for ( i = 0 ; i < 64 ; i++ ) {
                if ( F [i] <= -nb ) 
                    F [i] = (short) -nb ;
                else if ( F [i] >= nb - 1 ) 
                    F [i] = (short) nb - 1 ;
            }
        }
#endif
    }
    else 
    {
        for ( i = 0 ; i < 64 ; i++ ) {
            if ( QF [i] >= 0 ) 
                F [i] = (short) ((2 * QF [i] + 1) * Winter [i] * VOP -> quant >> 4);
            else 
                F [i] = (short) -((2 * (-QF [i]) + 1) * Winter [i] * VOP -> quant >> 4);
        }
        for ( i = 0 ; i < 64 ; i++ ) {
            if ( QF [i] == 0 ) 
                F [i] = 0 ;
        }
#ifdef C64LIB
        {
            int         sum = 0 ;
            const int   nb = 2 << VOLsimple -> bits_per_pixel + 2 ;
            const short max = 0x7FFF - nb + 1 ;
            for ( i = 0 ; i < 64 ; i++ ) {
                if ( QF [i] == 0 ) F[i] = 0;
                else
                {
                    const short inc = max - 2 * max * _extu(F [i], 0, 31);
                    F [i] = _sadd2(F [i], inc) - inc ;
                }
                sum ^= F [i];
            }
            if ( (sum & 1) == 0 ) 
                F [63]^= 1 ;
        }
#else 
        {
            int         sum = 0 ;
            const int   nb = 2 << (VOLsimple -> bits_per_pixel + 2) ;
            for ( i = 0 ; i < 64 ; i++ ) {
                if ( F [i] <= -nb ) 
                    F [i] = (short) -nb ;
                else if ( F [i] >= nb - 1 ) 
                    F [i] = (short) nb - 1 ;
                sum ^= F [i];
            }
            if ( (sum & 1) == 0 ) 
                F [63]^= 1 ;
        }
#endif
    }
}
