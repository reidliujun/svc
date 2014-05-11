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

#ifndef INTERPOL_N_N_H
#define INTERPOL_N_N_H

void luma_sample_interp_1_1(unsigned char *image, unsigned char *refPicLXl,	const short PicWidthSamples, const short stride);
void luma_sample_interp_1_3(unsigned char *image, unsigned char *refPicLXl,	const short PicWidthSamples, const short stride);
void luma_sample_interp_3_1(unsigned char *image, unsigned char *refPicLXl,	const short PicWidthSamples, const short stride);
void luma_sample_interp_3_3(unsigned char *image, unsigned char *refPicLXl,	const short PicWidthSamples, const short stride);


#ifdef TI_OPTIM
void luma_sample_interp_1_1_TI(unsigned char *image, unsigned char *refPicLXl,	const short PicWidthSamples, const short stride);
void luma_sample_interp_1_3_TI(unsigned char *image, unsigned char *refPicLXl,	const short PicWidthSamples, const short stride);
void luma_sample_interp_3_1_TI(unsigned char *image, unsigned char *refPicLXl,	const short PicWidthSamples, const short stride);
void luma_sample_interp_3_3_TI(unsigned char *image, unsigned char *refPicLXl,	const short PicWidthSamples, const short stride);
#endif


#ifdef MMXi_
void luma_sample_interp_1_1_MMX(unsigned char *image, unsigned char *refPicLXl,	const short PicWidthSamples, const short stride);
void luma_sample_interp_1_3_MMX(unsigned char *image, unsigned char *refPicLXl,	const short PicWidthSamples, const short stride);
void luma_sample_interp_3_1_MMX(unsigned char *image, unsigned char *refPicLXl,	const short PicWidthSamples, const short stride);
void luma_sample_interp_3_3_MMX(unsigned char *image, unsigned char *refPicLXl,	const short PicWidthSamples, const short stride);
#endif
#endif
