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

#ifndef INTERPOL_N_2_H
#define INTERPOL_N_2_H

void luma_sample_interp_1_2(unsigned char *image, unsigned char *refPicLXl,	const short PicWidthSamples, const short stride);

void luma_sample_interp_3_2(unsigned char *image, unsigned char *refPicLXl,	const short PicWidthSamples, const short stride);
void luma_sample_interp_1_2_v(short tmp_res [RESTRICT], unsigned char refPicLXl[RESTRICT], const short PicWidthSamples);
static void luma_sample_interp_1_2_2pass(short tmp_res [RESTRICT], unsigned char ptr_imag[RESTRICT], const short stride);
void luma_sample_interp_1_2_h(short tmp_res [RESTRICT], unsigned char image[RESTRICT], const short stride);


#ifdef TI_OPTIM
void luma_sample_interp_1_2_TI(unsigned char *image, unsigned char *refPicLXl,	const short PicWidthSamples, const short stride);
void luma_sample_interp_3_2_TI(unsigned char *image, unsigned char *refPicLXl,	const short PicWidthSamples, const short stride);
#endif


#ifdef MMXi_
void luma_sample_interp_1_2_MMX(unsigned char *image, unsigned char *refPicLXl,	const short PicWidthSamples, const short stride);
void luma_sample_interp_3_2_MMX(unsigned char *image, unsigned char *refPicLXl,	const short PicWidthSamples, const short stride);
#endif
#endif
