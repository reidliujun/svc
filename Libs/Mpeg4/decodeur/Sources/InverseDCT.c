/*****************************************************************************
     *
     *  MPEG4DSP developped in IETR image lab
     *
     *
     *
     *
     *  Based on the XviD MPEG-4 video codec
     *
   *  XVID MPEG-4 VIDEO CODEC
   *  - Inverse DCT  -
   *
   *  These routines are from Independent JPEG Group's free JPEG software
   *  Copyright (C) 1991-1998, Thomas G. Lane (see the file README.IJG)
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

#ifdef C64LIB
#include <IMG_idct_8x8.h>


void InverseDCT ( const short *RESTRICT F_in, short *RESTRICT f_out )
{
    int i ;
    
    for ( i = 0 ; i < 64 ; i++ ) 
        f_out [i] = F_in [i];
    IMG_idct_8x8(f_out, 1);
}

void InverseDCT_optim (short *RESTRICT F )
{
    IMG_idct_8x8(F, 1);
}
#else 

void idct_23002_2 (short *P);

void InverseDCT ( const short *RESTRICT F_in, short *RESTRICT f_out )
{
    int i ;
    
    for ( i = 0 ; i < 64 ; i++ ) 
        f_out [i] = F_in [i];
	idct_23002_2(f_out);
}

void InverseDCT_optim ( short *RESTRICT f_out )
{
	idct_23002_2(f_out);
}
#endif
