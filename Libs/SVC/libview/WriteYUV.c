/*****************************************************************************
   *
   *  H264DSP developped in IETR image lab
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



#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static FILE *ptfile ;
static char *ARG_INPUTFILE=0;


/**
Initialization.
*/
void WriteYUVInit (int argc, char * argv[])
{
	int i;
	for ( i = 1 ; i < argc ; i++ ) 
	{
		if (strcmp("-out", argv[i]) == 0 && i < argc - 1 ) 
		{
			i++;
			ARG_INPUTFILE = argv[i];
			break;
		}
	}
	if ( ARG_INPUTFILE == NULL || (ptfile = fopen(ARG_INPUTFILE, "wb")) == NULL) 
	{
		printf("Cannot open yuv_file concatenated input file '%s' for reading\n", ARG_INPUTFILE);
		exit( -1);
	}
}


/**
Write into a file YUV components.
*/
void WriteYUVCrop(int PicWidth, int PicHeight, unsigned char *Y, unsigned char *U, unsigned char *V)
{
	int i;
	int XDIM = PicWidth;
	int YDIM = PicHeight;

	for(i = 0; i < YDIM; i++)
		fwrite(Y + i*(XDIM + 32), 1, XDIM, ptfile);

	for(i=0; i<(YDIM>>1); i++)
		fwrite(U + i*((XDIM + 32)>>1), 1, (XDIM>>1), ptfile);

	for(i=0; i<(YDIM>>1); i++)
		fwrite(V + i*((XDIM + 32)>>1), 1, (XDIM>>1), ptfile); 
}