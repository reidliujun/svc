/**************************************************************************
* This software module was originally developed by contributors to the 
* course of development of the ISO/IEC 23002-2 (Fixed-point 8x8 IDCT and DCT) 
* standard for reference purposes and its performance may not have been optimized. 
* This software module is an implementation of one or more tools as specified by 
* the ISO/IEC 23002-2 standard.
*
* ISO/IEC gives You a royalty-free, worldwide, non-exclusive, copyright
* license to copy, distribute, and make derivative works of this software
* module or modifications thereof for use in implementations of the ISO/IEC
* 23002-2 standard in products that satisfy conformance criteria (if any).
*
* Those intending to use this software module in products are advised that
* its use may infringe existing patents. ISO/IEC have no liability for use
* of this software module or modifications thereof.
*
* Copyright is not released for products that do not conform to audiovisual
* and image-coding related ITU Recommendations and/or ISO/IEC International
* Standards.
*
******** Section to be removed when the standard is published ************
*
* Assurance that the originally developed software module can be used
* (1) in the ISO/IEC 23002-2 standard once the ISO/IEC 23002-2 standard
*     has been adopted; and
* (2) to develop the ISO/IEC 23002-2 standard:
*
* The original contributors grant ISO/IEC all rights necessary to include 
* the originally developed software module or modifications thereof in the 
* ISO/IEC 23002-2 standard and to permit ISO/IEC to offer You a royalty-free, 
* worldwide, non-exclusive, copyright license to copy, distribute, and make 
* derivative works for use in implementations of the ISO/IEC 23002-2 standard 
* in products that satisfy conformance criteria (if any), and to the extent 
* that such originally developed software module or portions of it are included 
* in the ISO/IEC 23002-2 standard. To the extent that the original contributors 
* own patent rights that would be required to make, use, or sell the originally 
* developed software module or portions thereof included in the ISO/IEC 23002-2 
* standard in a conforming product, the original contributors will assure the 
* ISO/IEC that they are willing to negotiate licenses under reasonable and non-
* discriminatory terms and conditions with applicants throughout the world.
*
* ISO/IEC gives You a free license to this software module or modifications
* thereof for the sole  purpose of developing the ISO/IEC 23002-2 standard.
*
******** end of section to be removed when the standard is published *****
*
* The original contributors retain full rights to modify and use the code for 
* their own purposes, assign or donate the code to a third party and to inhibit
* third parties from using the code for products that do not conform to
* MPEG-related ITU Recommendations and/or ISO/IEC International Standards.
*
* This copyright notice must be included in all copies or derivative works.
* Copyright (c) ISO/IEC 2007.
**********************************************************************/

/*
* Compute products:
*   y ~ y * 113/128;
*   z ~ y * 719/4096: 
*/
#define pmul_1(y,z)      \
  do {                   \
    int y2, y3;          \
    y2 = (y >> 3) - (y >> 7); \
    y3 = y2 - (y >> 11); \
    z  = y2 + (y3 >> 1); \
    y  = y - y2;         \
  } while (0)

/*
* Compute products:
*   y ~ y * 1533/2048;
*   z ~ y * 1/2;
*/
#define pmul_2(y,z)      \
  do {                   \
    int y2;              \
    y2 = (y >> 9) - y;   \
    z  = y >> 1;         \
    y  = (y2 >> 2) - y2; \
  } while (0)

/*
* Compute products:
*   y ~ y * 41/128;
*   z ~ y * 99/128;
*/
#define pmul_3(y,z)      \
  do {                   \
    int y2, y3;          \
    y2 = y + (y >> 5);   \
    y3 = y2 >> 2;        \
    y  = y3 + (y >> 4);  \
    z  = y2 - y3;        \
  } while (0)

/*
* Fast scaled 1D IDCT:
*  in - pointer to a row of scaled coeffs
*  out - pointer to a column where to store the results of IDCT
*/
static void scaled_1d_idct (int *in, int *out)
{
  register int x0, x1, x2, x3, x4, x5, x6, x7;
  register int xa, xb;

  /* odd coeffs: */
  x1 = in[1];
  x3 = in[3];
  x5 = in[5];
  x7 = in[7];

  /* 4th stage: */
  xa = x1 + x7; /* x1 */
  xb = x1 - x7; /* x7 */

  /* 3rd stage: */
  x1 = xa + x3;
  x3 = xa - x3;
  x7 = xb + x5;
  x5 = xb - x5;

  /* 2nd stage: */
  pmul_1(x3, xa);
  pmul_1(x5, xb);
  x3 = x3 - xb;
  x5 = x5 + xa;

  pmul_2(x1, xa);
  pmul_2(x7, xb);
  x1 = x1 + xb;
  x7 = x7 - xa;

  /* even coeffs: */
  x0 = in[0];
  x2 = in[2];
  x4 = in[4];
  x6 = in[6];

  /* 3rd stage: */
  pmul_3(x2, xa);
  pmul_3(x6, xb);
  x2 = x2 - xb;
  x6 = x6 + xa;

  xa = x0 + x4; /* x0 */
  xb = x0 - x4; /* x4 */

  /* 2nd stage: */
  x0 = xa + x6;
  x6 = xa - x6;
  x4 = xb + x2;
  x2 = xb - x2;

  /* 1st stage: */
  out[0*8] = x0 + x1;
  out[1*8] = x4 + x5;
  out[2*8] = x2 + x3;
  out[3*8] = x6 + x7;
  out[4*8] = x6 - x7;
  out[5*8] = x2 - x3;
  out[6*8] = x4 - x5;
  out[7*8] = x0 - x1;
}

/*
* 2D scaling: 
*/
#define A   1024 
#define B   1138
#define C   1730 
#define D   1609 
#define E   1264 
#define F   1922 
#define G   1788 
#define H   2923 
#define I   2718 
#define J   2528

/* 2D scale-factor matrix: */
static int scale [8*8] = {
  A, B, C, D, A, D, C, B,
  B, E, F, G, B, G, F, E,
  C, F, H, I, C, I, H, F,
  D, G, I, J, D, J, I, G,
  A, B, C, D, A, D, C, B,
  D, G, I, J, D, J, I, G,
  C, F, H, I, C, I, H, F,
  B, E, F, G, B, G, F, E
};

/*************************
* 2D 8x8 IDCT:  rows first
*/
void idct_23002_2 (short *P)
{
  int block[8*8], block2[8*8];
  register int i;

  /* multiplier-based scaling:
  *  - can be moved outside the transform, executed for non-zero coeffs only, 
  *    or absorbed in quantization step. */
  for (i=0; i<64; i++)
    block[i] = scale[i] * P[i];
  block[0] += 1 << 12;           /* bias DC for proper rounding */

  /* perform  scaled 1D IDCT for rows and columns: */
  for (i=0; i<8; i++)
    scaled_1d_idct (block + i*8, block2 + i);
  for (i=0; i<8; i++)
    scaled_1d_idct (block2 + i*8, block + i);

  /* downshift and store the results: */
  for (i=0; i<64; i++)
    P[i] = block[i] >> 13;
}

/***************** FDCT ************************/

/*
 * Fast scaled 1D FDCT:
 *  in - pointer to a column of scaled coeffs
 *  out - pointer to a row where to store the results of IDCT
 */
static void scaled_1d_fdct (int *in, int *out)
{
  register int x0, x1, x2, x3, x4, x5, x6, x7;
  register int xa, xb;

  /* 1st stage: */
  x0 = in[0*8] + in[7*8];
  x1 = in[0*8] - in[7*8];
  x4 = in[1*8] + in[6*8];
  x5 = in[1*8] - in[6*8];
  x2 = in[2*8] + in[5*8];
  x3 = in[2*8] - in[5*8];
  x6 = in[3*8] + in[4*8];
  x7 = in[3*8] - in[4*8];

  /* odd part: */
  pmul_1(x3, xa);  
  pmul_1(x5, xb);  
  x3 = x3 + xb;
  x5 = x5 - xa; 

  pmul_2(x1, xa);  
  pmul_2(x7, xb);  
  x1 = x1 - xb; 
  x7 = x7 + xa;

  xa = x1 + x3; 
  x3 = x1 - x3;
  xb = x7 + x5;
  x5 = x7 - x5;

  x1 = xa + xb; 
  x7 = xa - xb; 

  /* even part: */
  xa = x0 + x6;
  x6 = x0 - x6;
  xb = x4 + x2;
  x2 = x4 - x2;

  x0 = xa + xb; 
  x4 = xa - xb; 

  pmul_3(x2, xa);
  pmul_3(x6, xb);
  x2 = xb + x2;
  x6 = x6 - xa;

  out[0] = x0;
  out[1] = x1;
  out[2] = x2;
  out[3] = x3;
  out[4] = x4;
  out[5] = x5;
  out[6] = x6;
  out[7] = x7;
}

/*************************
 * 2D 8x8 FDCT:
 */
void fdct_23002_2 (short *P)
{
  int block[8*8], block2[8*8];
  register int i;

  /* load and scale: */
  for (i=0; i<64; i++)
    block[i] = P[i] << 7;

  /* perform  scaled 1D IDCT for rows and columns: */
  for (i=0; i<8; i++)
    scaled_1d_fdct (block + i, block2 + i*8);
  for (i=0; i<8; i++)
    scaled_1d_fdct (block2 + i, block + i*8);

  /* descale, downshift and store the results: */
  for (i=0; i<64; i++) 
    P[i] = (block[i] * scale[i] + 0x7FFFF - (block[i]>>31)) >> 20;
}

