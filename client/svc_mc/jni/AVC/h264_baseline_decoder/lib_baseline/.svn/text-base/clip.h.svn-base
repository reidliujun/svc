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


#ifndef CLIP_H
#define CLIP_H

#define MIN(X,Y) (((X)>(Y)) ? Y:X)
#define MAX(X,Y) (((X)>(Y)) ? X:Y)
#define ADD(X,Y) ((X)+(Y))
#define SUB(X,Y) ((X)-(Y))

#ifdef TI_OPTIM
#define MIN16(X,Y) _min2(X,Y)
#define MAX16(X,Y) _max2(X,Y)
#define CLIP3(X,Y,Z) _min2(Y,_max2(X,Z))
#define CLIP255(x) _spacku4 (0,x)
#else
#define MIN16(X,Y) ((((short)(X))>((short)(Y))) ? ((short)(Y)):((short)(X)))
#define MAX16(X,Y) ((((short)(X))>((short)(Y))) ? ((short)(X)):((short)(Y)))
#define CLIP255(X) MIN(255,MAX(0,X))
#define CLIP3(X,Y,Z) MIN(Y,MAX(X,Z))
#endif




#define CLIP3_16(X,Y,Z) MIN16(Y,MAX16(X,Z))
#define CLIP255_16(X) MIN16(255,MAX16(0,X))


#endif
