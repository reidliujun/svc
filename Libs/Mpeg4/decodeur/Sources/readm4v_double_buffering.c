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
#include <stdio.h>
#ifdef QDMA_6416
#include <QDMA.h>
#endif

void readm4v_double_buffering ( const int nb_octets_to_read, const int pos_octet, const int new_buff_i
    , const unsigned char *buffer_new, const unsigned char *buffer_state, unsigned char *buffer_out, int *position_o )
{
#ifdef QDMA_6416
    
    if ( new_buff_i == 1 ) {
        int pos = pos_octet - nb_octets_to_read >> 2 << 2 ;
        if ( nb_octets_to_read - pos != 0 ) 
            QDMA32copy(buffer_out + pos, buffer_state + nb_octets_to_read + pos, nb_octets_to_read - pos);
        QDMA32copy(buffer_out + nb_octets_to_read, buffer_new, nb_octets_to_read);
        *position_o = (pos_octet - nb_octets_to_read) * 8 ;
    } else {
        QDMA32copy(buffer_out, buffer_state, 2 * nb_octets_to_read);
        /*for ( i = 0 ; i < 2*nb_octets_to_read ; i++ ) 
           buffer_out [i] = buffer_state [i];*/
        *position_o = pos_octet * 8 ;
    }
#else 
    
    int i ;
    
    if ( new_buff_i == 1 ) {
        for ( i = pos_octet - nb_octets_to_read ; i < nb_octets_to_read ; i++ ) 
            buffer_out [i] = buffer_state [i + nb_octets_to_read];
        for ( i = 0 ; i < nb_octets_to_read ; i++ ) 
            buffer_out [i + nb_octets_to_read] = buffer_new [i];
        *position_o = (pos_octet - nb_octets_to_read) * 8 ;
    } else {
        for ( i = 0 ; i < 2 * nb_octets_to_read ; i++ ) 
            buffer_out [i] = buffer_state [i];
        *position_o = pos_octet * 8 ;
    }
#endif
}

void New_buffer ( const int nb_octets_to_read, const int pos, int *new_buff_o, int *pos_octet )
{
    if ( pos % 8 == 0 ) 
        *pos_octet = pos / 8 ;
    else 
        *pos_octet = pos / 8 + 1 ;
    if ( *pos_octet >= nb_octets_to_read ) {
        *new_buff_o = 1 ;
    } else {
        *new_buff_o = 0 ;
    }
}
