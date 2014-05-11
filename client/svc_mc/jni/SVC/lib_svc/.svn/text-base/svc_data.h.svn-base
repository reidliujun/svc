
/*****************************************************************************
   *
   *  Open SVC Decoder developped in IETR image lab
   *
   *
   *
   *              Médéric BLESTEL <mblestel@insa-rennes.Fr>
   *              Mickael RAULET <mraulet@insa-rennes.Fr>
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
   * License along with this library; if not, write to the Free Software
   * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
   *
   *
   * $Id$
   *
   **************************************************************************/

#include "symbol.h"



#define EP 0
#define EB 1
#define EI 2

#define IS_BL(X) (X == I_BL || X == P_BL || X == B_BL)
#define IS_INTER_BL(X) (X == P_BL || X == B_BL)
#define IS_I_SVC(X) (X > 0 && X < 5 || X == I_BL)
#define IS_P_SVC(X) (X > 4 && X < 9 || X == P_Skip || X == P_BL)
#define IS_B_SVC(X) (X > 8 && X < 20 || X == B_Skip || X == P_BL)
#define IS_B_L0(X,Y) (X == 10 || X == 12 || X == 13 || X == 15 || X == 19 || X == 21 || (Y && (X == 18 || X == 17)) || (!Y && (X == 14 || X == 20)))
#define IS_B_L1(X,Y) (X == 11 || X == 12 || X == 16 || X == 18 || X == 20 || X == 21 || (Y && (X == 14 || X == 15)) || (!Y && (X == 17 || X == 19)))

#define BLK_4x4 0
#define BLK_4x8 1
#define BLK_8x4 2
#define BLK_8x8 3

#define MBLK_INTRA_4x4 4
#define MBLK_16x16 3
#define MBLK_16x8 2
#define MBLK_8x16 1
#define MBLK_8x8 0

#define RESCALE_POSITION(W,X,Y,Z) (((W * X) + Y)/Z)
