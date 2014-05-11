/*****************************************************************************
*
*  Open SVC Decoder developped in IETR image lab
*
*
*
*              Médéric BLESTEL <mblestel@insa-rennes.Fr>
*              Mickael RAULET <mraulet@insa-rennes.Fr>
*			  Maxime  PELCAT <mpelcat@insa-rennes.Fr>
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
* but WITHOUT ANY WARRANTY; without even the implied warranty off
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




#ifndef ERRORDETECTION_H
#define ERRORDETECTION_H


#include "type.h"
#include "symbol.h"

#ifdef ERROR_DETECTION






/**
Mb Type detection
Ne need to change the mb type, it wiil not be decoded
*/
static __inline int ErrorsCheckIMbType(int MbType)
{
	if(MbType > INTRA_PCM && MbType != I_BL){
		//mb type error
		return 1;
	}
	return 0;
}

/**
Mb Type detection
Ne need to change the mb type, it wiil not be decoded
*/
static __inline int ErrorsCheckPMbType(int MbType)
{
	if((MbType < INTRA_PCM || MbType > P_8x8ref0) && MbType != P_BL){
		//mb type error
		return 1;
	}
	return 0;
}

/**
Mb Type detection
No need to change the mb type, it will not be decoded
*/
static __inline int ErrorsCheckBMbType(int MbType)
{
	if((MbType < P_8x8ref0 || MbType > B_8x8) && MbType != B_BL){
		//mb type error
		return 1;
	}
	return 0;
}

/**
Mb Type detection
No need to change the mb type, it will not be decoded
*/
static __inline int ErrorsCheckMbType(int MbType, int Limit)
{
	if(MbType > Limit){
		//mb type error
		return 1;
	}
	return 0;
}



/**
Intra prediction mode error detection for chrominance.
The mode should be changed because the macroblock will be decoded.
*/
static __inline int ErrorsCheckIntraChromaPredMode(unsigned char *Mode)
{
	if ( *Mode > MaxChromaMode){
		//Error found on chroma prediction mode
		*Mode = 0;
		return 1;
	}
	return 0;
}




/**
sub-macroblock type error detection.
Types should be changed.
//Ok for B and P macroblock
*/
static __inline int ErrorsCheckSubMbType(unsigned char *SubMbType, int MaxSubMbType)
{
	if(SubMbType [0] > MaxSubMbType || SubMbType [1] > MaxSubMbType ||
		SubMbType [2] > MaxSubMbType || SubMbType [3] > MaxSubMbType){
			SubMbType [0] = SubMbType [1] = SubMbType [2] = SubMbType [3] = 0;
			return 1;
	}
	return 0;
}




/**
sub-macroblock reference lists error detection.
Types should be changed.
//Ok for B and P macroblock
*/
static __inline int ErrorsCheckSubRefLx(char *RefLx, int NumRefLx)
{
	if(RefLx [0] > NumRefLx || RefLx [1] > NumRefLx	|| 
		RefLx [2] > NumRefLx || RefLx [3] > NumRefLx){
			RefLx [0] = RefLx [1] = RefLx [2] = RefLx [3] = 0;
			return 1;
	}
	return 0;
}

/**
Macroblock reference lists error detection.
Types should be changed.
//Ok for B and P macroblock
*/
static __inline int ErrorsCheckRefLx(char *RefLx, int NumRefLx)
{
	if(RefLx [0] > NumRefLx || RefLx [1] > NumRefLx){
		RefLx [0] = RefLx [1] = 0;
		return 1;
	}
	return 0;
}


#endif
#endif
