//
// asharp (version 0.95) - adaptive sharpenning filter.
//
// asharp engine implementation (C/MMX/ISSE)
//
// Copyright (C) 2002 Marc Fauconneau
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
//
//  Please contact me for any bugs or questions.
//  marc.fd@libertysurf.fr

//  Change log :
//         27 nov 2002 - ver 0.95  - First GPL release.

#ifndef _ASHARP_H_
#define _ASHARP_H_

typedef void (asharp_run_fct)(unsigned char *planeptr, int pitch,
                              int height, int width,
                              int T,int D, int B, int B2, int bf,unsigned char *lineptr);

asharp_run_fct* getAsharp(void);

#endif
