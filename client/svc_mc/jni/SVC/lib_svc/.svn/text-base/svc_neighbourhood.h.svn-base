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

#ifndef SVC_NEIGHBOUR_H
#define SVC_NEIGHBOUR_H


void get_base_neighbourhood(short *Mask,  short Addr,  const short iAddr_x, const short PicWidthInMbs, 
							const RESIDU *residu, const short PicSizeInMbs, const int IntraConstrained);


/**
This function tests if the macroblock address is correct.
*/
static __inline int svc_neighbour_avail ( const int ai_iMbAddr, const int PicSizeInMbs)
{
    if ( ai_iMbAddr < 0 || ai_iMbAddr >= PicSizeInMbs ) 
		return 0 ;

    return 1 ;
}
#endif
