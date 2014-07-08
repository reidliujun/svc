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

#ifndef POCKET_PC
#include <stdio.h>
#ifndef ARM
#include <conio.h>
#endif

#define uchar unsigned char

void compare_YUV ( const int xpic, const int ypic, const uchar *Y1, const uchar *U1, const uchar *V1, const uchar *Y2
    , const uchar *U2, const uchar *V2 )
{
    {
        int i, error = 0 ;
        for ( i = 0 ; i < xpic * ypic ; i++ ) {
            if ( Y1 [i] != Y2 [i] ) {
                //printf("error ! Y1 : %d # Y2 : %d\n", Y1 [i], Y2 [i]);
                error++ ;
            } else {
                
                //   printf("OK !\n");
            }
        }
        if ( error != 0 ) {
            printf("diff Lum %d \n", error);
            _getch();
        }
    }
    {
        int i, error = 0 ;
        for ( i = 0 ; i < xpic * ypic / 4 ; i++ ) {
            if ( U1 [i] != U2 [i] ) {
                //printf("error ! U1 : %d # U2 : %d\n", U1 [i], U2 [i]);
                error++ ;
            } else {
                
                //   printf("OK !\n");
            }
        }
        if ( error != 0 ) {
            printf("diff Cb %d \n", error);
            _getch();
        }
    }
    {
        int i, error = 0 ;
        for ( i = 0 ; i < xpic * ypic / 4 ; i++ ) {
            if ( V1 [i] != V2 [i] ) {
                //printf("error ! V1 : %d # V2 : %d\n", V1 [i], V2 [i]);
                error++ ;
            } else {
                
                //  printf("OK !\n");
            }
        }
        if ( error != 0 ) {
            printf("diff Cr %d \n", error);
            _getch();
        }
    }
}
#endif