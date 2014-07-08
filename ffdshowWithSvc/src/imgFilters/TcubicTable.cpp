/*
 * Copyright (c) 2002-2006 Milan Cutka
 * cubic function by Avery Lee, implementation based on Warpsharp filter for AviSynth by ...
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "stdafx.h"
#include "TcubicTable.h"

TcubicTable::TcubicTable(double A)
{
 for (int i=0; i<256; i++)
  {
   double d = (double)i / 256.0;
   int y1, y2, y3, y4, ydiff;

   // Coefficients for all four pixels *must* add up to 1.0 for
   // consistent unity gain.
   //
   // Two good values for A are -1.0 (original VirtualDub bicubic filter)
   // and -0.75 (closely matches Photoshop).

   y1 = (int)floor(0.5 + (        +     A*d -       2.0*A*d*d +       A*d*d*d) * 16384.0);
   y2 = (int)floor(0.5 + (+ 1.0             -     (A+3.0)*d*d + (A+2.0)*d*d*d) * 16384.0);
   y3 = (int)floor(0.5 + (        -     A*d + (2.0*A+3.0)*d*d - (A+2.0)*d*d*d) * 16384.0);
   y4 = (int)floor(0.5 + (                  +           A*d*d -       A*d*d*d) * 16384.0);

   // Normalize y's so they add up to 16384.

   ydiff = (16384 - y1 - y2 - y3 - y4)/4;
   assert(ydiff > -16 && ydiff < 16);

   y1 += ydiff;
   y2 += ydiff;
   y3 += ydiff;
   y4 += ydiff;

   table[i*4 + 0] = (y2<<16) | (y1 & 0xffff);
   table[i*4 + 1] = (y4<<16) | (y3 & 0xffff);
   table[i*4 + 2] = 0;
   table[i*4 + 3] = 0;
  }
}
