/*
 * Copyright (c) 2005,2006 Milan Cutka
 * uses code from mplayer 2xsai filter
 * http://elektron.its.tudelft.nl/~dalikifa/
 * and original 2xSai Copyright (c) Derek Liauw Kie Fa, 1999
 *
 * hq2x filter (C) 2003 MaxSt ( maxst@hiend3d.com )
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
#include "2xsai.h"

void T2xSaI::super(const uint8_t *src, stride_t src_pitch,
                   uint8_t *dst, stride_t dst_pitch,
                   uint32_t width, uint32_t height)
{
 unsigned int x, y;
 unsigned long color[16];

 const unsigned char *src_line[4];

 /* Point to the first 3 lines. */
 src_line[0] = src;
 src_line[1] = src;
 src_line[2] = src + src_pitch;
 src_line[3] = src + src_pitch * 2;

 x = 0, y = 0;

 unsigned long *lbp;
 lbp = (unsigned long*)src_line[0];
 color[0] = *lbp;       color[1] = color[0];   color[2] = color[0];    color[3] = color[0];
 color[4] = color[0];   color[5] = color[0];   color[6] = *(lbp + 1);  color[7] = *(lbp + 2);
 lbp = (unsigned long*)src_line[2];
 color[8] = *lbp;     color[9] = color[8];     color[10] = *(lbp + 1); color[11] = *(lbp + 2);
 lbp = (unsigned long*)src_line[3];
 color[12] = *lbp;    color[13] = color[12];   color[14] = *(lbp + 1); color[15] = *(lbp + 2);

 for (y = 0; y < height; y++)
  {
   unsigned char *dst_line[2];
   dst_line[0] = dst + dst_pitch*2*y;
   dst_line[1] = dst + dst_pitch*(2*y+1);

   /* Todo: x = width - 2, x = width - 1 */

   for (x = 0; x < width; x++)
    {
     unsigned long product1a, product1b, product2a, product2b;

//---------------------------------------  B0 B1 B2 B3    0  1  2  3
//                                         4  5* 6  S2 -> 4  5* 6  7
//                                         1  2  3  S1    8  9 10 11
//                                         A0 A1 A2 A3   12 13 14 15
//--------------------------------------
     if (color[9] == color[6] && color[5] != color[10])
      {
       product2b = color[9];
       product1b = product2b;
      }
     else if (color[5] == color[10] && color[9] != color[6])
      {
       product2b = color[5];
       product1b = product2b;
      }
     else if (color[5] == color[10] && color[9] == color[6])
      {
       int r = 0;

       r += GET_RESULT(color[6], color[5], color[8], color[13]);
       r += GET_RESULT(color[6], color[5], color[4], color[1]);
       r += GET_RESULT(color[6], color[5], color[14], color[11]);
       r += GET_RESULT(color[6], color[5], color[2], color[7]);

       if (r > 0)
               product1b = color[6];
       else if (r < 0)
               product1b = color[5];
       else
               product1b = INTERPOLATE(color[5], color[6]);

       product2b = product1b;
      }
     else
      {
       if (color[6] == color[10] && color[10] == color[13] && color[9] != color[14] && color[10] != color[12])
        product2b = Q_INTERPOLATE(color[10], color[10], color[10], color[9]);
       else if (color[5] == color[9] && color[9] == color[14] && color[13] != color[10] && color[9] != color[15])
        product2b = Q_INTERPOLATE(color[9], color[9], color[9], color[10]);
       else
        product2b = INTERPOLATE(color[9], color[10]);

       if (color[6] == color[10] && color[6] == color[1] && color[5] != color[2] && color[6] != color[0])
        product1b = Q_INTERPOLATE(color[6], color[6], color[6], color[5]);
       else if (color[5] == color[9] && color[5] == color[2] && color[1] != color[6] && color[5] != color[3])
        product1b = Q_INTERPOLATE(color[6], color[5], color[5], color[5]);
       else
        product1b = INTERPOLATE(color[5], color[6]);
      }

     if (color[5] == color[10] && color[9] != color[6] && color[4] == color[5] && color[5] != color[14])
      product2a = INTERPOLATE(color[9], color[5]);
     else if (color[5] == color[8] && color[6] == color[5] && color[4] != color[9] && color[5] != color[12])
      product2a = INTERPOLATE(color[9], color[5]);
     else
      product2a = color[9];

     if (color[9] == color[6] && color[5] != color[10] && color[8] == color[9] && color[9] != color[2])
      product1a = INTERPOLATE(color[9], color[5]);
     else if (color[4] == color[9] && color[10] == color[9] && color[8] != color[5] && color[9] != color[0])
      product1a = INTERPOLATE(color[9], color[5]);
     else
      product1a = color[5];

     *((unsigned long *) (&dst_line[0][x * 8])) = product1a;
     *((unsigned long *) (&dst_line[0][x * 8 + 4])) = product1b;
     *((unsigned long *) (&dst_line[1][x * 8])) = product2a;
     *((unsigned long *) (&dst_line[1][x * 8 + 4])) = product2b;

     /* Move color matrix forward */
     color[0] = color[1]; color[4] = color[5]; color[8] = color[9];   color[12] = color[13];
     color[1] = color[2]; color[5] = color[6]; color[9] = color[10];  color[13] = color[14];
     color[2] = color[3]; color[6] = color[7]; color[10] = color[11]; color[14] = color[15];

     if (x < width - 3)
      {
       x += 3;
       color[3] = *(((unsigned long*)src_line[0]) + x);
       color[7] = *(((unsigned long*)src_line[1]) + x);
       color[11] = *(((unsigned long*)src_line[2]) + x);
       color[15] = *(((unsigned long*)src_line[3]) + x);
       x -= 3;
      }
    }

   /* We're done with one line, so we shift the source lines up */
   src_line[0] = src_line[1];
   src_line[1] = src_line[2];
   src_line[2] = src_line[3];

   /* Read next line */
   if (y + 3 >= height)
    src_line[3] = src_line[2];
   else
    src_line[3] = src_line[2] + src_pitch;

   /* Then shift the color matrix up */
   lbp = (unsigned long*)src_line[0];
   color[0] = *lbp;     color[1] = color[0];    color[2] = *(lbp + 1);  color[3] = *(lbp + 2);
   lbp = (unsigned long*)src_line[1];
   color[4] = *lbp;     color[5] = color[4];    color[6] = *(lbp + 1);  color[7] = *(lbp + 2);
   lbp = (unsigned long*)src_line[2];
   color[8] = *lbp;     color[9] = color[9];    color[10] = *(lbp + 1); color[11] = *(lbp + 2);
   lbp = (unsigned long*)src_line[3];
   color[12] = *lbp;    color[13] = color[12];  color[14] = *(lbp + 1); color[15] = *(lbp + 2);
  } // y loop
}

int T2xSaI::GetResult1(uint32 A, uint32 B, uint32 C, uint32 D, uint32 E)
{
 int x = 0;
 int y = 0;
 int r = 0;
 if (A == C) x+=1; else if (B == C) y+=1;
 if (A == D) x+=1; else if (B == D) y+=1;
 if (x <= 1) r+=1;
 if (y <= 1) r-=1;
 return r;
}

int T2xSaI::GetResult2(uint32 A, uint32 B, uint32 C, uint32 D, uint32 E)
{
 int x = 0;
 int y = 0;
 int r = 0;
 if (A == C) x+=1; else if (B == C) y+=1;
 if (A == D) x+=1; else if (B == D) y+=1;
 if (x <= 1) r-=1;
 if (y <= 1) r+=1;
 return r;
}

void T2xSaI::_2xSaI(const uint8 *srcPtr, stride_t srcPitch, uint8 *dstBitmap, int width, int height,stride_t dstPitch)
{
 uint32 line;
 uint32 x_offset;
 line = 0;//(dstBitmap->h - height * 2) >> 1;

 x_offset = 0;//(width*2 - width * 2);

 const uint16 *bp[4] =
  {
   (uint16*)(srcPtr/*-srcPitch*/),
   (uint16*) srcPtr,
   (uint16*)(srcPtr+srcPitch),
   (uint16*)(srcPtr+2*srcPitch)
  };

 for (; height; height-=1)
  {
   uint8_t *dP = dstBitmap+line*dstPitch+x_offset; /*bmp_write_line (dstBitmap, line) + x_offset*/;
   for (int x = 0; x<width; x++ )
    {
     register uint32 colorA, colorB;
     uint32 colorC, colorD,
            colorE, colorF, colorG, colorH,
            colorI, colorJ, colorK, colorL,
            colorM, colorN, colorO, colorP;
     uint32 product, product1, product2;


     //---------------------------------------
     // Map of the pixels:                    I|E F|J
     //                                       G|A B|K
     //                                       H|C D|L
     //                                       M|N O|P
     int a1=x>0?-1:0;
     int a2=x<width-1?1:0;
     int a3=x<width-2?2:1;
     colorI = bp[0][a1+x];//*(bP- Nextline - 1);
     colorE = bp[0][ 0+x];//*(bP- Nextline);
     colorF = bp[0][a2+x];//*(bP- Nextline + 1);
     colorJ = bp[0][a3+x];//*(bP- Nextline + 2);

     colorG = bp[1][a1+x];//*(bP - 1);
     colorA = bp[1][ 0+x];//*(bP);
     colorB = bp[1][a2+x];//*(bP + 1);
     colorK = bp[1][a3+x];//*(bP + 2);

     colorH = bp[2][a1+x];//*(bP + Nextline - 1);
     colorC = bp[2][ 0+x];//*(bP + Nextline);
     colorD = bp[2][a2+x];//*(bP + Nextline + 1);
     colorL = bp[2][a3+x];//*(bP + Nextline + 2);

     colorM = bp[3][a1+x];//*(bP + Nextline + Nextline - 1);
     colorN = bp[3][ 0+x];//*(bP + Nextline + Nextline);
     colorO = bp[3][a2+x];//*(bP + Nextline + Nextline + 1);
     colorP = bp[3][a3+x];//*(bP + Nextline + Nextline + 2);

     if ((colorA == colorD) && (colorB != colorC))
     {
        if ( ((colorA == colorE) && (colorB == colorL)) ||
             ((colorA == colorC) && (colorA == colorF) && (colorB != colorE) && (colorB == colorJ)) )
        {
           product = colorA;
        }
        else
        {
           product = INTERPOLATE16(colorA, colorB);
        }

        if (((colorA == colorG) && (colorC == colorO)) ||
            ((colorA == colorB) && (colorA == colorH) && (colorG != colorC) && (colorC == colorM)) )
        {
           product1 = colorA;
        }
        else
        {
           product1 = INTERPOLATE16(colorA, colorC);
        }
        product2 = colorA;
     }
     else if ((colorB == colorC) && (colorA != colorD))
     {
        if (((colorB == colorF) && (colorA == colorH)) ||
            ((colorB == colorE) && (colorB == colorD) && (colorA != colorF) && (colorA == colorI)) )
        {
           product = colorB;
        }
        else
        {
           product = INTERPOLATE16(colorA, colorB);
        }

        if (((colorC == colorH) && (colorA == colorF)) ||
            ((colorC == colorG) && (colorC == colorD) && (colorA != colorH) && (colorA == colorI)) )
        {
           product1 = colorC;
        }
        else
        {
           product1 = INTERPOLATE16(colorA, colorC);
        }
        product2 = colorB;
     }
     else if ((colorA == colorD) && (colorB == colorC))
     {
        if (colorA == colorB)
        {
           product = colorA;
           product1 = colorA;
           product2 = colorA;
        }
        else
        {
           register int r = 0;
           product1 = INTERPOLATE16(colorA, colorC);
           product = INTERPOLATE16(colorA, colorB);

           r += GetResult1 (colorA, colorB, colorG, colorE, colorI);
           r += GetResult2 (colorB, colorA, colorK, colorF, colorJ);
           r += GetResult2 (colorB, colorA, colorH, colorN, colorM);
           r += GetResult1 (colorA, colorB, colorL, colorO, colorP);

           if (r > 0)
               product2 = colorA;
           else if (r < 0)
               product2 = colorB;
           else
           {
               product2 = Q_INTERPOLATE16(colorA, colorB, colorC, colorD);
           }
        }
     }
     else
     {
        product2 = Q_INTERPOLATE16(colorA, colorB, colorC, colorD);

        if ((colorA == colorC) && (colorA == colorF) && (colorB != colorE) && (colorB == colorJ))
        {
           product = colorA;
        }
        else if ((colorB == colorE) && (colorB == colorD) && (colorA != colorF) && (colorA == colorI))
        {
           product = colorB;
        }
        else
        {
           product = INTERPOLATE16(colorA, colorB);
        }

        if ((colorA == colorB) && (colorA == colorH) && (colorG != colorC) && (colorC == colorM))
        {
           product1 = colorA;
        }
        else if ((colorC == colorG) && (colorC == colorD) && (colorA != colorH) && (colorA == colorI))
        {
           product1 = colorC;
        }
        else
        {
           product1 = INTERPOLATE16(colorA, colorC);
        }
     }

     product = colorA | (product << 16);
     product1 = product1 | (product2 << 16);
     *(uint32_t*)dP=product;
     *(uint32_t*)(dP+dstPitch)=product1;

     dP += 4;
    }//end of for ( finish= width etc..)
   bp[0]=bp[1];bp[1]=bp[2];bp[2]=bp[3];
   if (height>3)
    bp[3]+=srcPitch/2;
   line += 2;
   //srcPtr += srcPitch;
  } //endof: for (height; height; height--)
  //memcpy(dstBitmap+(line-1)*dstPitch,dstBitmap+(line-2)*dstPitch,width*4);
}

//=====================================================================================
int Thq2x::LUT16to32[65536];
__align8(int,Thq2x::RGBtoYUV[65536]);

void Thq2x::init(void)
{
 int i, j, k, r, g, b, Y, u, v;

 for (i=0; i<65536; i++)
   LUT16to32[i] = ((i & 0xF800) << 8) + ((i & 0x07E0) << 5) + ((i & 0x001F) << 3);

 for (i=0; i<32; i++)
 for (j=0; j<64; j++)
 for (k=0; k<32; k++)
  {
    r = i << 3;
    g = j << 2;
    b = k << 3;
    Y = (r + g + b) >> 2;
    u = 128 + ((r - b) >> 2);
    v = 128 + ((-r + 2*g -b)>>3);
    RGBtoYUV[ (i << 11) + (j << 5) + k ] = (Y<<16) + (u<<8) + v;
  }
}

const __int64 Thq2x::reg_blank = 0;
const __int64 Thq2x::const3    = 0x0000000300030003LL;
const __int64 Thq2x::const5    = 0x0000000500050005LL;
const __int64 Thq2x::const6    = 0x0000000600060006LL;
const __int64 Thq2x::const14   = 0x0000000E000E000ELL;

__forceinline void Thq2x::Interp1(unsigned char * pc, int c1, int c2)
{
  *((int*)pc) = (c1*3+c2) >> 2;
}

__forceinline void Thq2x::Interp2(unsigned char * pc, int c1, int c2, int c3)
{
  *((int*)pc) = (c1*2+c2+c3) >> 2;
}

__forceinline void Thq2x::Interp5(unsigned char * pc, int c1, int c2)
{
  *((int*)pc) = (c1+c2) >> 1;
}

__forceinline void Thq2x::Interp6(unsigned char * pc, int c1, int c2, int c3)
{
  //*((int*)pc) = (c1*5+c2*2+c3)/8;
  unsigned char *eax= pc;
  __m64 mm1,mm2,mm3;
  movd      ( mm1, c1);
  movd      ( mm2, c2);
  movd      ( mm3, c3);
  punpcklbw ( mm1, reg_blank);
  punpcklbw ( mm2, reg_blank);
  punpcklbw ( mm3, reg_blank);
  pmullw    ( mm1, const5);
  psllw     ( mm2, 1);
  paddw     ( mm1, mm3);
  paddw     ( mm1, mm2);
  psrlw     ( mm1, 3);
  packuswb  ( mm1, reg_blank);
  movd(       eax, mm1);
}

__forceinline void Thq2x::Interp7(unsigned char * pc, int c1, int c2, int c3)
{
  //*((int*)pc) = (c1*6+c2+c3)/8;
  unsigned char *eax= pc;
  __m64 mm1,mm2,mm3;
  movd      ( mm1, c1);
  movd      ( mm2, c2);
  movd      ( mm3, c3);
  punpcklbw ( mm1, reg_blank);
  punpcklbw ( mm2, reg_blank);
  punpcklbw ( mm3, reg_blank);
  pmullw    ( mm1, const6);
  paddw     ( mm2, mm3);
  paddw     ( mm1, mm2);
  psrlw     ( mm1, 3);
  packuswb  ( mm1, reg_blank);
  movd      ( eax, mm1);
}

__forceinline void Thq2x::Interp9(unsigned char * pc, int c1, int c2, int c3)
{
  //*((int*)pc) = (c1*2+(c2+c3)*3)/8;
  unsigned char *eax= pc;
  __m64 mm1,mm2,mm3;
  movd       (mm1, c1);
  movd       (mm2, c2);
  movd       (mm3, c3);
  punpcklbw  (mm1, reg_blank);
  punpcklbw  (mm2, reg_blank);
  punpcklbw  (mm3, reg_blank);
  psllw      (mm1, 1);
  paddw      (mm2, mm3);
  pmullw     (mm2, const3);
  paddw      (mm1, mm2);
  psrlw      (mm1, 3);
  packuswb   (mm1, reg_blank);
  movd       (eax, mm1);
}

__forceinline void Thq2x::Interp10(unsigned char * pc, int c1, int c2, int c3)
{
  //*((int*)pc) = (c1*14+c2+c3)/16;
  unsigned char *eax= pc;
  __m64 mm1,mm2,mm3;
  movd       (mm1, c1);
  movd       (mm2, c2);
  movd       (mm3, c3);
  punpcklbw  (mm1, reg_blank);
  punpcklbw  (mm2, reg_blank);
  punpcklbw  (mm3, reg_blank);
  pmullw     (mm1, const14);
  paddw      (mm2, mm3);
  paddw      (mm1, mm2);
  psrlw      (mm1, 4);
  packuswb   (mm1, reg_blank);
  movd       (eax, mm1);
}

#define PIXEL00_0     *((int*)(pOut)) = c[5];
#define PIXEL00_10    Interp1(pOut, c[5], c[1]);
#define PIXEL00_11    Interp1(pOut, c[5], c[4]);
#define PIXEL00_12    Interp1(pOut, c[5], c[2]);
#define PIXEL00_20    Interp2(pOut, c[5], c[4], c[2]);
#define PIXEL00_21    Interp2(pOut, c[5], c[1], c[2]);
#define PIXEL00_22    Interp2(pOut, c[5], c[1], c[4]);
#define PIXEL00_60    Interp6(pOut, c[5], c[2], c[4]);
#define PIXEL00_61    Interp6(pOut, c[5], c[4], c[2]);
#define PIXEL00_70    Interp7(pOut, c[5], c[4], c[2]);
#define PIXEL00_90    Interp9(pOut, c[5], c[4], c[2]);
#define PIXEL00_100   Interp10(pOut, c[5], c[4], c[2]);
#define PIXEL01_0     *((int*)(pOut+4)) = c[5];
#define PIXEL01_10    Interp1(pOut+4, c[5], c[3]);
#define PIXEL01_11    Interp1(pOut+4, c[5], c[2]);
#define PIXEL01_12    Interp1(pOut+4, c[5], c[6]);
#define PIXEL01_20    Interp2(pOut+4, c[5], c[2], c[6]);
#define PIXEL01_21    Interp2(pOut+4, c[5], c[3], c[6]);
#define PIXEL01_22    Interp2(pOut+4, c[5], c[3], c[2]);
#define PIXEL01_60    Interp6(pOut+4, c[5], c[6], c[2]);
#define PIXEL01_61    Interp6(pOut+4, c[5], c[2], c[6]);
#define PIXEL01_70    Interp7(pOut+4, c[5], c[2], c[6]);
#define PIXEL01_90    Interp9(pOut+4, c[5], c[2], c[6]);
#define PIXEL01_100   Interp10(pOut+4, c[5], c[2], c[6]);
#define PIXEL10_0     *((int*)(pOut+dstBpL)) = c[5];
#define PIXEL10_10    Interp1(pOut+dstBpL, c[5], c[7]);
#define PIXEL10_11    Interp1(pOut+dstBpL, c[5], c[8]);
#define PIXEL10_12    Interp1(pOut+dstBpL, c[5], c[4]);
#define PIXEL10_20    Interp2(pOut+dstBpL, c[5], c[8], c[4]);
#define PIXEL10_21    Interp2(pOut+dstBpL, c[5], c[7], c[4]);
#define PIXEL10_22    Interp2(pOut+dstBpL, c[5], c[7], c[8]);
#define PIXEL10_60    Interp6(pOut+dstBpL, c[5], c[4], c[8]);
#define PIXEL10_61    Interp6(pOut+dstBpL, c[5], c[8], c[4]);
#define PIXEL10_70    Interp7(pOut+dstBpL, c[5], c[8], c[4]);
#define PIXEL10_90    Interp9(pOut+dstBpL, c[5], c[8], c[4]);
#define PIXEL10_100   Interp10(pOut+dstBpL, c[5], c[8], c[4]);
#define PIXEL11_0     *((int*)(pOut+dstBpL+4)) = c[5];
#define PIXEL11_10    Interp1(pOut+dstBpL+4, c[5], c[9]);
#define PIXEL11_11    Interp1(pOut+dstBpL+4, c[5], c[6]);
#define PIXEL11_12    Interp1(pOut+dstBpL+4, c[5], c[8]);
#define PIXEL11_20    Interp2(pOut+dstBpL+4, c[5], c[6], c[8]);
#define PIXEL11_21    Interp2(pOut+dstBpL+4, c[5], c[9], c[8]);
#define PIXEL11_22    Interp2(pOut+dstBpL+4, c[5], c[9], c[6]);
#define PIXEL11_60    Interp6(pOut+dstBpL+4, c[5], c[8], c[6]);
#define PIXEL11_61    Interp6(pOut+dstBpL+4, c[5], c[6], c[8]);
#define PIXEL11_70    Interp7(pOut+dstBpL+4, c[5], c[6], c[8]);
#define PIXEL11_90    Interp9(pOut+dstBpL+4, c[5], c[6], c[8]);
#define PIXEL11_100   Interp10(pOut+dstBpL+4, c[5], c[6], c[8]);

__forceinline int Thq2x::Diff(unsigned int w5, unsigned int w1)
{
 static const __int64 treshold=0x0000000000300706LL;
 int eax=0;;
 unsigned int ebx=w5;
 unsigned int edx=w1;
 if (ebx==edx) return eax;
 const unsigned char *ecx=(const unsigned char*)RGBtoYUV;
 __m64 mm1,mm5,mm2;
 movd    (mm1,ecx + ebx*4);
 movq    (mm5,mm1);
 movd    (mm2,ecx + edx*4);
 psubusb (mm1,mm2);
 psubusb (mm2,mm5);
 por     (mm1,mm2);
 psubusb (mm1,treshold);
 movd    (eax,mm1);
 return eax;
}
// returns result in eax register

void Thq2x::hq2x_32( const unsigned char * src, unsigned char * dst, int dx, int dy, stride_t srcBpL, stride_t dstBpL )
{
 static bool firsttime=true;
 if (firsttime)
  {
   firsttime=false;
   init();
  }

  int  i, j, k;
  stride_t  prevline, nextline;
  int  w[10];
  int  c[10];

  //   +----+----+----+
  //   |    |    |    |
  //   | w1 | w2 | w3 |
  //   +----+----+----+
  //   |    |    |    |
  //   | w4 | w5 | w6 |
  //   +----+----+----+
  //   |    |    |    |
  //   | w7 | w8 | w9 |
  //   +----+----+----+

  for (j=0; j<dy; j++)
  {
    if (j>0)    prevline = -srcBpL; else prevline = 0;
    if (j<dy-1) nextline =  srcBpL; else nextline = 0;
    const unsigned char *pIn=src+j*srcBpL;
    unsigned char *pOut=dst+(j*2)*dstBpL;
    for (i=0; i<dx; i++)
    {
      w[2] = *((unsigned short*)(pIn + prevline));
      w[5] = *((unsigned short*)pIn);
      w[8] = *((unsigned short*)(pIn + nextline));

      if (i>0)
      {
        w[1] = *((unsigned short*)(pIn + prevline - 2));
        w[4] = *((unsigned short*)(pIn - 2));
        w[7] = *((unsigned short*)(pIn + nextline - 2));
      }
      else
      {
        w[1] = w[2];
        w[4] = w[5];
        w[7] = w[8];
      }

      if (i<dx-1)
      {
        w[3] = *((unsigned short*)(pIn + prevline + 2));
        w[6] = *((unsigned short*)(pIn + 2));
        w[9] = *((unsigned short*)(pIn + nextline + 2));
      }
      else
      {
        w[3] = w[2];
        w[6] = w[5];
        w[9] = w[8];
      }

      int pattern = 0;

      if ( Diff(w[5],w[1]) ) pattern |= 0x0001;
      if ( Diff(w[5],w[2]) ) pattern |= 0x0002;
      if ( Diff(w[5],w[3]) ) pattern |= 0x0004;
      if ( Diff(w[5],w[4]) ) pattern |= 0x0008;
      if ( Diff(w[5],w[6]) ) pattern |= 0x0010;
      if ( Diff(w[5],w[7]) ) pattern |= 0x0020;
      if ( Diff(w[5],w[8]) ) pattern |= 0x0040;
      if ( Diff(w[5],w[9]) ) pattern |= 0x0080;

      for (k=1; k<=9; k++)
        c[k] = LUT16to32[w[k]];

      switch (pattern)
      {
        case 0:
        case 1:
        case 4:
        case 32:
        case 128:
        case 5:
        case 132:
        case 160:
        case 33:
        case 129:
        case 36:
        case 133:
        case 164:
        case 161:
        case 37:
        case 165:
        {
          PIXEL00_20
          PIXEL01_20
          PIXEL10_20
          PIXEL11_20
          break;
        }
        case 2:
        case 34:
        case 130:
        case 162:
        {
          PIXEL00_22
          PIXEL01_21
          PIXEL10_20
          PIXEL11_20
          break;
        }
        case 16:
        case 17:
        case 48:
        case 49:
        {
          PIXEL00_20
          PIXEL01_22
          PIXEL10_20
          PIXEL11_21
          break;
        }
        case 64:
        case 65:
        case 68:
        case 69:
        {
          PIXEL00_20
          PIXEL01_20
          PIXEL10_21
          PIXEL11_22
          break;
        }
        case 8:
        case 12:
        case 136:
        case 140:
        {
          PIXEL00_21
          PIXEL01_20
          PIXEL10_22
          PIXEL11_20
          break;
        }
        case 3:
        case 35:
        case 131:
        case 163:
        {
          PIXEL00_11
          PIXEL01_21
          PIXEL10_20
          PIXEL11_20
          break;
        }
        case 6:
        case 38:
        case 134:
        case 166:
        {
          PIXEL00_22
          PIXEL01_12
          PIXEL10_20
          PIXEL11_20
          break;
        }
        case 20:
        case 21:
        case 52:
        case 53:
        {
          PIXEL00_20
          PIXEL01_11
          PIXEL10_20
          PIXEL11_21
          break;
        }
        case 144:
        case 145:
        case 176:
        case 177:
        {
          PIXEL00_20
          PIXEL01_22
          PIXEL10_20
          PIXEL11_12
          break;
        }
        case 192:
        case 193:
        case 196:
        case 197:
        {
          PIXEL00_20
          PIXEL01_20
          PIXEL10_21
          PIXEL11_11
          break;
        }
        case 96:
        case 97:
        case 100:
        case 101:
        {
          PIXEL00_20
          PIXEL01_20
          PIXEL10_12
          PIXEL11_22
          break;
        }
        case 40:
        case 44:
        case 168:
        case 172:
        {
          PIXEL00_21
          PIXEL01_20
          PIXEL10_11
          PIXEL11_20
          break;
        }
        case 9:
        case 13:
        case 137:
        case 141:
        {
          PIXEL00_12
          PIXEL01_20
          PIXEL10_22
          PIXEL11_20
          break;
        }
        case 18:
        case 50:
        {
          PIXEL00_22
          if (Diff(w[2], w[6]))
          {
            PIXEL01_10
          }
          else
          {
            PIXEL01_20
          }
          PIXEL10_20
          PIXEL11_21
          break;
        }
        case 80:
        case 81:
        {
          PIXEL00_20
          PIXEL01_22
          PIXEL10_21
          if (Diff(w[6], w[8]))
          {
            PIXEL11_10
          }
          else
          {
            PIXEL11_20
          }
          break;
        }
        case 72:
        case 76:
        {
          PIXEL00_21
          PIXEL01_20
          if (Diff(w[8], w[4]))
          {
            PIXEL10_10
          }
          else
          {
            PIXEL10_20
          }
          PIXEL11_22
          break;
        }
        case 10:
        case 138:
        {
          if (Diff(w[4], w[2]))
          {
            PIXEL00_10
          }
          else
          {
            PIXEL00_20
          }
          PIXEL01_21
          PIXEL10_22
          PIXEL11_20
          break;
        }
        case 66:
        {
          PIXEL00_22
          PIXEL01_21
          PIXEL10_21
          PIXEL11_22
          break;
        }
        case 24:
        {
          PIXEL00_21
          PIXEL01_22
          PIXEL10_22
          PIXEL11_21
          break;
        }
        case 7:
        case 39:
        case 135:
        {
          PIXEL00_11
          PIXEL01_12
          PIXEL10_20
          PIXEL11_20
          break;
        }
        case 148:
        case 149:
        case 180:
        {
          PIXEL00_20
          PIXEL01_11
          PIXEL10_20
          PIXEL11_12
          break;
        }
        case 224:
        case 228:
        case 225:
        {
          PIXEL00_20
          PIXEL01_20
          PIXEL10_12
          PIXEL11_11
          break;
        }
        case 41:
        case 169:
        case 45:
        {
          PIXEL00_12
          PIXEL01_20
          PIXEL10_11
          PIXEL11_20
          break;
        }
        case 22:
        case 54:
        {
          PIXEL00_22
          if (Diff(w[2], w[6]))
          {
            PIXEL01_0
          }
          else
          {
            PIXEL01_20
          }
          PIXEL10_20
          PIXEL11_21
          break;
        }
        case 208:
        case 209:
        {
          PIXEL00_20
          PIXEL01_22
          PIXEL10_21
          if (Diff(w[6], w[8]))
          {
            PIXEL11_0
          }
          else
          {
            PIXEL11_20
          }
          break;
        }
        case 104:
        case 108:
        {
          PIXEL00_21
          PIXEL01_20
          if (Diff(w[8], w[4]))
          {
            PIXEL10_0
          }
          else
          {
            PIXEL10_20
          }
          PIXEL11_22
          break;
        }
        case 11:
        case 139:
        {
          if (Diff(w[4], w[2]))
          {
            PIXEL00_0
          }
          else
          {
            PIXEL00_20
          }
          PIXEL01_21
          PIXEL10_22
          PIXEL11_20
          break;
        }
        case 19:
        case 51:
        {
          if (Diff(w[2], w[6]))
          {
            PIXEL00_11
            PIXEL01_10
          }
          else
          {
            PIXEL00_60
            PIXEL01_90
          }
          PIXEL10_20
          PIXEL11_21
          break;
        }
        case 146:
        case 178:
        {
          PIXEL00_22
          if (Diff(w[2], w[6]))
          {
            PIXEL01_10
            PIXEL11_12
          }
          else
          {
            PIXEL01_90
            PIXEL11_61
          }
          PIXEL10_20
          break;
        }
        case 84:
        case 85:
        {
          PIXEL00_20
          if (Diff(w[6], w[8]))
          {
            PIXEL01_11
            PIXEL11_10
          }
          else
          {
            PIXEL01_60
            PIXEL11_90
          }
          PIXEL10_21
          break;
        }
        case 112:
        case 113:
        {
          PIXEL00_20
          PIXEL01_22
          if (Diff(w[6], w[8]))
          {
            PIXEL10_12
            PIXEL11_10
          }
          else
          {
            PIXEL10_61
            PIXEL11_90
          }
          break;
        }
        case 200:
        case 204:
        {
          PIXEL00_21
          PIXEL01_20
          if (Diff(w[8], w[4]))
          {
            PIXEL10_10
            PIXEL11_11
          }
          else
          {
            PIXEL10_90
            PIXEL11_60
          }
          break;
        }
        case 73:
        case 77:
        {
          if (Diff(w[8], w[4]))
          {
            PIXEL00_12
            PIXEL10_10
          }
          else
          {
            PIXEL00_61
            PIXEL10_90
          }
          PIXEL01_20
          PIXEL11_22
          break;
        }
        case 42:
        case 170:
        {
          if (Diff(w[4], w[2]))
          {
            PIXEL00_10
            PIXEL10_11
          }
          else
          {
            PIXEL00_90
            PIXEL10_60
          }
          PIXEL01_21
          PIXEL11_20
          break;
        }
        case 14:
        case 142:
        {
          if (Diff(w[4], w[2]))
          {
            PIXEL00_10
            PIXEL01_12
          }
          else
          {
            PIXEL00_90
            PIXEL01_61
          }
          PIXEL10_22
          PIXEL11_20
          break;
        }
        case 67:
        {
          PIXEL00_11
          PIXEL01_21
          PIXEL10_21
          PIXEL11_22
          break;
        }
        case 70:
        {
          PIXEL00_22
          PIXEL01_12
          PIXEL10_21
          PIXEL11_22
          break;
        }
        case 28:
        {
          PIXEL00_21
          PIXEL01_11
          PIXEL10_22
          PIXEL11_21
          break;
        }
        case 152:
        {
          PIXEL00_21
          PIXEL01_22
          PIXEL10_22
          PIXEL11_12
          break;
        }
        case 194:
        {
          PIXEL00_22
          PIXEL01_21
          PIXEL10_21
          PIXEL11_11
          break;
        }
        case 98:
        {
          PIXEL00_22
          PIXEL01_21
          PIXEL10_12
          PIXEL11_22
          break;
        }
        case 56:
        {
          PIXEL00_21
          PIXEL01_22
          PIXEL10_11
          PIXEL11_21
          break;
        }
        case 25:
        {
          PIXEL00_12
          PIXEL01_22
          PIXEL10_22
          PIXEL11_21
          break;
        }
        case 26:
        case 31:
        {
          if (Diff(w[4], w[2]))
          {
            PIXEL00_0
          }
          else
          {
            PIXEL00_20
          }
          if (Diff(w[2], w[6]))
          {
            PIXEL01_0
          }
          else
          {
            PIXEL01_20
          }
          PIXEL10_22
          PIXEL11_21
          break;
        }
        case 82:
        case 214:
        {
          PIXEL00_22
          if (Diff(w[2], w[6]))
          {
            PIXEL01_0
          }
          else
          {
            PIXEL01_20
          }
          PIXEL10_21
          if (Diff(w[6], w[8]))
          {
            PIXEL11_0
          }
          else
          {
            PIXEL11_20
          }
          break;
        }
        case 88:
        case 248:
        {
          PIXEL00_21
          PIXEL01_22
          if (Diff(w[8], w[4]))
          {
            PIXEL10_0
          }
          else
          {
            PIXEL10_20
          }
          if (Diff(w[6], w[8]))
          {
            PIXEL11_0
          }
          else
          {
            PIXEL11_20
          }
          break;
        }
        case 74:
        case 107:
        {
          if (Diff(w[4], w[2]))
          {
            PIXEL00_0
          }
          else
          {
            PIXEL00_20
          }
          PIXEL01_21
          if (Diff(w[8], w[4]))
          {
            PIXEL10_0
          }
          else
          {
            PIXEL10_20
          }
          PIXEL11_22
          break;
        }
        case 27:
        {
          if (Diff(w[4], w[2]))
          {
            PIXEL00_0
          }
          else
          {
            PIXEL00_20
          }
          PIXEL01_10
          PIXEL10_22
          PIXEL11_21
          break;
        }
        case 86:
        {
          PIXEL00_22
          if (Diff(w[2], w[6]))
          {
            PIXEL01_0
          }
          else
          {
            PIXEL01_20
          }
          PIXEL10_21
          PIXEL11_10
          break;
        }
        case 216:
        {
          PIXEL00_21
          PIXEL01_22
          PIXEL10_10
          if (Diff(w[6], w[8]))
          {
            PIXEL11_0
          }
          else
          {
            PIXEL11_20
          }
          break;
        }
        case 106:
        {
          PIXEL00_10
          PIXEL01_21
          if (Diff(w[8], w[4]))
          {
            PIXEL10_0
          }
          else
          {
            PIXEL10_20
          }
          PIXEL11_22
          break;
        }
        case 30:
        {
          PIXEL00_10
          if (Diff(w[2], w[6]))
          {
            PIXEL01_0
          }
          else
          {
            PIXEL01_20
          }
          PIXEL10_22
          PIXEL11_21
          break;
        }
        case 210:
        {
          PIXEL00_22
          PIXEL01_10
          PIXEL10_21
          if (Diff(w[6], w[8]))
          {
            PIXEL11_0
          }
          else
          {
            PIXEL11_20
          }
          break;
        }
        case 120:
        {
          PIXEL00_21
          PIXEL01_22
          if (Diff(w[8], w[4]))
          {
            PIXEL10_0
          }
          else
          {
            PIXEL10_20
          }
          PIXEL11_10
          break;
        }
        case 75:
        {
          if (Diff(w[4], w[2]))
          {
            PIXEL00_0
          }
          else
          {
            PIXEL00_20
          }
          PIXEL01_21
          PIXEL10_10
          PIXEL11_22
          break;
        }
        case 29:
        {
          PIXEL00_12
          PIXEL01_11
          PIXEL10_22
          PIXEL11_21
          break;
        }
        case 198:
        {
          PIXEL00_22
          PIXEL01_12
          PIXEL10_21
          PIXEL11_11
          break;
        }
        case 184:
        {
          PIXEL00_21
          PIXEL01_22
          PIXEL10_11
          PIXEL11_12
          break;
        }
        case 99:
        {
          PIXEL00_11
          PIXEL01_21
          PIXEL10_12
          PIXEL11_22
          break;
        }
        case 57:
        {
          PIXEL00_12
          PIXEL01_22
          PIXEL10_11
          PIXEL11_21
          break;
        }
        case 71:
        {
          PIXEL00_11
          PIXEL01_12
          PIXEL10_21
          PIXEL11_22
          break;
        }
        case 156:
        {
          PIXEL00_21
          PIXEL01_11
          PIXEL10_22
          PIXEL11_12
          break;
        }
        case 226:
        {
          PIXEL00_22
          PIXEL01_21
          PIXEL10_12
          PIXEL11_11
          break;
        }
        case 60:
        {
          PIXEL00_21
          PIXEL01_11
          PIXEL10_11
          PIXEL11_21
          break;
        }
        case 195:
        {
          PIXEL00_11
          PIXEL01_21
          PIXEL10_21
          PIXEL11_11
          break;
        }
        case 102:
        {
          PIXEL00_22
          PIXEL01_12
          PIXEL10_12
          PIXEL11_22
          break;
        }
        case 153:
        {
          PIXEL00_12
          PIXEL01_22
          PIXEL10_22
          PIXEL11_12
          break;
        }
        case 58:
        {
          if (Diff(w[4], w[2]))
          {
            PIXEL00_10
          }
          else
          {
            PIXEL00_70
          }
          if (Diff(w[2], w[6]))
          {
            PIXEL01_10
          }
          else
          {
            PIXEL01_70
          }
          PIXEL10_11
          PIXEL11_21
          break;
        }
        case 83:
        {
          PIXEL00_11
          if (Diff(w[2], w[6]))
          {
            PIXEL01_10
          }
          else
          {
            PIXEL01_70
          }
          PIXEL10_21
          if (Diff(w[6], w[8]))
          {
            PIXEL11_10
          }
          else
          {
            PIXEL11_70
          }
          break;
        }
        case 92:
        {
          PIXEL00_21
          PIXEL01_11
          if (Diff(w[8], w[4]))
          {
            PIXEL10_10
          }
          else
          {
            PIXEL10_70
          }
          if (Diff(w[6], w[8]))
          {
            PIXEL11_10
          }
          else
          {
            PIXEL11_70
          }
          break;
        }
        case 202:
        {
          if (Diff(w[4], w[2]))
          {
            PIXEL00_10
          }
          else
          {
            PIXEL00_70
          }
          PIXEL01_21
          if (Diff(w[8], w[4]))
          {
            PIXEL10_10
          }
          else
          {
            PIXEL10_70
          }
          PIXEL11_11
          break;
        }
        case 78:
        {
          if (Diff(w[4], w[2]))
          {
            PIXEL00_10
          }
          else
          {
            PIXEL00_70
          }
          PIXEL01_12
          if (Diff(w[8], w[4]))
          {
            PIXEL10_10
          }
          else
          {
            PIXEL10_70
          }
          PIXEL11_22
          break;
        }
        case 154:
        {
          if (Diff(w[4], w[2]))
          {
            PIXEL00_10
          }
          else
          {
            PIXEL00_70
          }
          if (Diff(w[2], w[6]))
          {
            PIXEL01_10
          }
          else
          {
            PIXEL01_70
          }
          PIXEL10_22
          PIXEL11_12
          break;
        }
        case 114:
        {
          PIXEL00_22
          if (Diff(w[2], w[6]))
          {
            PIXEL01_10
          }
          else
          {
            PIXEL01_70
          }
          PIXEL10_12
          if (Diff(w[6], w[8]))
          {
            PIXEL11_10
          }
          else
          {
            PIXEL11_70
          }
          break;
        }
        case 89:
        {
          PIXEL00_12
          PIXEL01_22
          if (Diff(w[8], w[4]))
          {
            PIXEL10_10
          }
          else
          {
            PIXEL10_70
          }
          if (Diff(w[6], w[8]))
          {
            PIXEL11_10
          }
          else
          {
            PIXEL11_70
          }
          break;
        }
        case 90:
        {
          if (Diff(w[4], w[2]))
          {
            PIXEL00_10
          }
          else
          {
            PIXEL00_70
          }
          if (Diff(w[2], w[6]))
          {
            PIXEL01_10
          }
          else
          {
            PIXEL01_70
          }
          if (Diff(w[8], w[4]))
          {
            PIXEL10_10
          }
          else
          {
            PIXEL10_70
          }
          if (Diff(w[6], w[8]))
          {
            PIXEL11_10
          }
          else
          {
            PIXEL11_70
          }
          break;
        }
        case 55:
        case 23:
        {
          if (Diff(w[2], w[6]))
          {
            PIXEL00_11
            PIXEL01_0
          }
          else
          {
            PIXEL00_60
            PIXEL01_90
          }
          PIXEL10_20
          PIXEL11_21
          break;
        }
        case 182:
        case 150:
        {
          PIXEL00_22
          if (Diff(w[2], w[6]))
          {
            PIXEL01_0
            PIXEL11_12
          }
          else
          {
            PIXEL01_90
            PIXEL11_61
          }
          PIXEL10_20
          break;
        }
        case 213:
        case 212:
        {
          PIXEL00_20
          if (Diff(w[6], w[8]))
          {
            PIXEL01_11
            PIXEL11_0
          }
          else
          {
            PIXEL01_60
            PIXEL11_90
          }
          PIXEL10_21
          break;
        }
        case 241:
        case 240:
        {
          PIXEL00_20
          PIXEL01_22
          if (Diff(w[6], w[8]))
          {
            PIXEL10_12
            PIXEL11_0
          }
          else
          {
            PIXEL10_61
            PIXEL11_90
          }
          break;
        }
        case 236:
        case 232:
        {
          PIXEL00_21
          PIXEL01_20
          if (Diff(w[8], w[4]))
          {
            PIXEL10_0
            PIXEL11_11
          }
          else
          {
            PIXEL10_90
            PIXEL11_60
          }
          break;
        }
        case 109:
        case 105:
        {
          if (Diff(w[8], w[4]))
          {
            PIXEL00_12
            PIXEL10_0
          }
          else
          {
            PIXEL00_61
            PIXEL10_90
          }
          PIXEL01_20
          PIXEL11_22
          break;
        }
        case 171:
        case 43:
        {
          if (Diff(w[4], w[2]))
          {
            PIXEL00_0
            PIXEL10_11
          }
          else
          {
            PIXEL00_90
            PIXEL10_60
          }
          PIXEL01_21
          PIXEL11_20
          break;
        }
        case 143:
        case 15:
        {
          if (Diff(w[4], w[2]))
          {
            PIXEL00_0
            PIXEL01_12
          }
          else
          {
            PIXEL00_90
            PIXEL01_61
          }
          PIXEL10_22
          PIXEL11_20
          break;
        }
        case 124:
        {
          PIXEL00_21
          PIXEL01_11
          if (Diff(w[8], w[4]))
          {
            PIXEL10_0
          }
          else
          {
            PIXEL10_20
          }
          PIXEL11_10
          break;
        }
        case 203:
        {
          if (Diff(w[4], w[2]))
          {
            PIXEL00_0
          }
          else
          {
            PIXEL00_20
          }
          PIXEL01_21
          PIXEL10_10
          PIXEL11_11
          break;
        }
        case 62:
        {
          PIXEL00_10
          if (Diff(w[2], w[6]))
          {
            PIXEL01_0
          }
          else
          {
            PIXEL01_20
          }
          PIXEL10_11
          PIXEL11_21
          break;
        }
        case 211:
        {
          PIXEL00_11
          PIXEL01_10
          PIXEL10_21
          if (Diff(w[6], w[8]))
          {
            PIXEL11_0
          }
          else
          {
            PIXEL11_20
          }
          break;
        }
        case 118:
        {
          PIXEL00_22
          if (Diff(w[2], w[6]))
          {
            PIXEL01_0
          }
          else
          {
            PIXEL01_20
          }
          PIXEL10_12
          PIXEL11_10
          break;
        }
        case 217:
        {
          PIXEL00_12
          PIXEL01_22
          PIXEL10_10
          if (Diff(w[6], w[8]))
          {
            PIXEL11_0
          }
          else
          {
            PIXEL11_20
          }
          break;
        }
        case 110:
        {
          PIXEL00_10
          PIXEL01_12
          if (Diff(w[8], w[4]))
          {
            PIXEL10_0
          }
          else
          {
            PIXEL10_20
          }
          PIXEL11_22
          break;
        }
        case 155:
        {
          if (Diff(w[4], w[2]))
          {
            PIXEL00_0
          }
          else
          {
            PIXEL00_20
          }
          PIXEL01_10
          PIXEL10_22
          PIXEL11_12
          break;
        }
        case 188:
        {
          PIXEL00_21
          PIXEL01_11
          PIXEL10_11
          PIXEL11_12
          break;
        }
        case 185:
        {
          PIXEL00_12
          PIXEL01_22
          PIXEL10_11
          PIXEL11_12
          break;
        }
        case 61:
        {
          PIXEL00_12
          PIXEL01_11
          PIXEL10_11
          PIXEL11_21
          break;
        }
        case 157:
        {
          PIXEL00_12
          PIXEL01_11
          PIXEL10_22
          PIXEL11_12
          break;
        }
        case 103:
        {
          PIXEL00_11
          PIXEL01_12
          PIXEL10_12
          PIXEL11_22
          break;
        }
        case 227:
        {
          PIXEL00_11
          PIXEL01_21
          PIXEL10_12
          PIXEL11_11
          break;
        }
        case 230:
        {
          PIXEL00_22
          PIXEL01_12
          PIXEL10_12
          PIXEL11_11
          break;
        }
        case 199:
        {
          PIXEL00_11
          PIXEL01_12
          PIXEL10_21
          PIXEL11_11
          break;
        }
        case 220:
        {
          PIXEL00_21
          PIXEL01_11
          if (Diff(w[8], w[4]))
          {
            PIXEL10_10
          }
          else
          {
            PIXEL10_70
          }
          if (Diff(w[6], w[8]))
          {
            PIXEL11_0
          }
          else
          {
            PIXEL11_20
          }
          break;
        }
        case 158:
        {
          if (Diff(w[4], w[2]))
          {
            PIXEL00_10
          }
          else
          {
            PIXEL00_70
          }
          if (Diff(w[2], w[6]))
          {
            PIXEL01_0
          }
          else
          {
            PIXEL01_20
          }
          PIXEL10_22
          PIXEL11_12
          break;
        }
        case 234:
        {
          if (Diff(w[4], w[2]))
          {
            PIXEL00_10
          }
          else
          {
            PIXEL00_70
          }
          PIXEL01_21
          if (Diff(w[8], w[4]))
          {
            PIXEL10_0
          }
          else
          {
            PIXEL10_20
          }
          PIXEL11_11
          break;
        }
        case 242:
        {
          PIXEL00_22
          if (Diff(w[2], w[6]))
          {
            PIXEL01_10
          }
          else
          {
            PIXEL01_70
          }
          PIXEL10_12
          if (Diff(w[6], w[8]))
          {
            PIXEL11_0
          }
          else
          {
            PIXEL11_20
          }
          break;
        }
        case 59:
        {
          if (Diff(w[4], w[2]))
          {
            PIXEL00_0
          }
          else
          {
            PIXEL00_20
          }
          if (Diff(w[2], w[6]))
          {
            PIXEL01_10
          }
          else
          {
            PIXEL01_70
          }
          PIXEL10_11
          PIXEL11_21
          break;
        }
        case 121:
        {
          PIXEL00_12
          PIXEL01_22
          if (Diff(w[8], w[4]))
          {
            PIXEL10_0
          }
          else
          {
            PIXEL10_20
          }
          if (Diff(w[6], w[8]))
          {
            PIXEL11_10
          }
          else
          {
            PIXEL11_70
          }
          break;
        }
        case 87:
        {
          PIXEL00_11
          if (Diff(w[2], w[6]))
          {
            PIXEL01_0
          }
          else
          {
            PIXEL01_20
          }
          PIXEL10_21
          if (Diff(w[6], w[8]))
          {
            PIXEL11_10
          }
          else
          {
            PIXEL11_70
          }
          break;
        }
        case 79:
        {
          if (Diff(w[4], w[2]))
          {
            PIXEL00_0
          }
          else
          {
            PIXEL00_20
          }
          PIXEL01_12
          if (Diff(w[8], w[4]))
          {
            PIXEL10_10
          }
          else
          {
            PIXEL10_70
          }
          PIXEL11_22
          break;
        }
        case 122:
        {
          if (Diff(w[4], w[2]))
          {
            PIXEL00_10
          }
          else
          {
            PIXEL00_70
          }
          if (Diff(w[2], w[6]))
          {
            PIXEL01_10
          }
          else
          {
            PIXEL01_70
          }
          if (Diff(w[8], w[4]))
          {
            PIXEL10_0
          }
          else
          {
            PIXEL10_20
          }
          if (Diff(w[6], w[8]))
          {
            PIXEL11_10
          }
          else
          {
            PIXEL11_70
          }
          break;
        }
        case 94:
        {
          if (Diff(w[4], w[2]))
          {
            PIXEL00_10
          }
          else
          {
            PIXEL00_70
          }
          if (Diff(w[2], w[6]))
          {
            PIXEL01_0
          }
          else
          {
            PIXEL01_20
          }
          if (Diff(w[8], w[4]))
          {
            PIXEL10_10
          }
          else
          {
            PIXEL10_70
          }
          if (Diff(w[6], w[8]))
          {
            PIXEL11_10
          }
          else
          {
            PIXEL11_70
          }
          break;
        }
        case 218:
        {
          if (Diff(w[4], w[2]))
          {
            PIXEL00_10
          }
          else
          {
            PIXEL00_70
          }
          if (Diff(w[2], w[6]))
          {
            PIXEL01_10
          }
          else
          {
            PIXEL01_70
          }
          if (Diff(w[8], w[4]))
          {
            PIXEL10_10
          }
          else
          {
            PIXEL10_70
          }
          if (Diff(w[6], w[8]))
          {
            PIXEL11_0
          }
          else
          {
            PIXEL11_20
          }
          break;
        }
        case 91:
        {
          if (Diff(w[4], w[2]))
          {
            PIXEL00_0
          }
          else
          {
            PIXEL00_20
          }
          if (Diff(w[2], w[6]))
          {
            PIXEL01_10
          }
          else
          {
            PIXEL01_70
          }
          if (Diff(w[8], w[4]))
          {
            PIXEL10_10
          }
          else
          {
            PIXEL10_70
          }
          if (Diff(w[6], w[8]))
          {
            PIXEL11_10
          }
          else
          {
            PIXEL11_70
          }
          break;
        }
        case 229:
        {
          PIXEL00_20
          PIXEL01_20
          PIXEL10_12
          PIXEL11_11
          break;
        }
        case 167:
        {
          PIXEL00_11
          PIXEL01_12
          PIXEL10_20
          PIXEL11_20
          break;
        }
        case 173:
        {
          PIXEL00_12
          PIXEL01_20
          PIXEL10_11
          PIXEL11_20
          break;
        }
        case 181:
        {
          PIXEL00_20
          PIXEL01_11
          PIXEL10_20
          PIXEL11_12
          break;
        }
        case 186:
        {
          if (Diff(w[4], w[2]))
          {
            PIXEL00_10
          }
          else
          {
            PIXEL00_70
          }
          if (Diff(w[2], w[6]))
          {
            PIXEL01_10
          }
          else
          {
            PIXEL01_70
          }
          PIXEL10_11
          PIXEL11_12
          break;
        }
        case 115:
        {
          PIXEL00_11
          if (Diff(w[2], w[6]))
          {
            PIXEL01_10
          }
          else
          {
            PIXEL01_70
          }
          PIXEL10_12
          if (Diff(w[6], w[8]))
          {
            PIXEL11_10
          }
          else
          {
            PIXEL11_70
          }
          break;
        }
        case 93:
        {
          PIXEL00_12
          PIXEL01_11
          if (Diff(w[8], w[4]))
          {
            PIXEL10_10
          }
          else
          {
            PIXEL10_70
          }
          if (Diff(w[6], w[8]))
          {
            PIXEL11_10
          }
          else
          {
            PIXEL11_70
          }
          break;
        }
        case 206:
        {
          if (Diff(w[4], w[2]))
          {
            PIXEL00_10
          }
          else
          {
            PIXEL00_70
          }
          PIXEL01_12
          if (Diff(w[8], w[4]))
          {
            PIXEL10_10
          }
          else
          {
            PIXEL10_70
          }
          PIXEL11_11
          break;
        }
        case 205:
        case 201:
        {
          PIXEL00_12
          PIXEL01_20
          if (Diff(w[8], w[4]))
          {
            PIXEL10_10
          }
          else
          {
            PIXEL10_70
          }
          PIXEL11_11
          break;
        }
        case 174:
        case 46:
        {
          if (Diff(w[4], w[2]))
          {
            PIXEL00_10
          }
          else
          {
            PIXEL00_70
          }
          PIXEL01_12
          PIXEL10_11
          PIXEL11_20
          break;
        }
        case 179:
        case 147:
        {
          PIXEL00_11
          if (Diff(w[2], w[6]))
          {
            PIXEL01_10
          }
          else
          {
            PIXEL01_70
          }
          PIXEL10_20
          PIXEL11_12
          break;
        }
        case 117:
        case 116:
        {
          PIXEL00_20
          PIXEL01_11
          PIXEL10_12
          if (Diff(w[6], w[8]))
          {
            PIXEL11_10
          }
          else
          {
            PIXEL11_70
          }
          break;
        }
        case 189:
        {
          PIXEL00_12
          PIXEL01_11
          PIXEL10_11
          PIXEL11_12
          break;
        }
        case 231:
        {
          PIXEL00_11
          PIXEL01_12
          PIXEL10_12
          PIXEL11_11
          break;
        }
        case 126:
        {
          PIXEL00_10
          if (Diff(w[2], w[6]))
          {
            PIXEL01_0
          }
          else
          {
            PIXEL01_20
          }
          if (Diff(w[8], w[4]))
          {
            PIXEL10_0
          }
          else
          {
            PIXEL10_20
          }
          PIXEL11_10
          break;
        }
        case 219:
        {
          if (Diff(w[4], w[2]))
          {
            PIXEL00_0
          }
          else
          {
            PIXEL00_20
          }
          PIXEL01_10
          PIXEL10_10
          if (Diff(w[6], w[8]))
          {
            PIXEL11_0
          }
          else
          {
            PIXEL11_20
          }
          break;
        }
        case 125:
        {
          if (Diff(w[8], w[4]))
          {
            PIXEL00_12
            PIXEL10_0
          }
          else
          {
            PIXEL00_61
            PIXEL10_90
          }
          PIXEL01_11
          PIXEL11_10
          break;
        }
        case 221:
        {
          PIXEL00_12
          if (Diff(w[6], w[8]))
          {
            PIXEL01_11
            PIXEL11_0
          }
          else
          {
            PIXEL01_60
            PIXEL11_90
          }
          PIXEL10_10
          break;
        }
        case 207:
        {
          if (Diff(w[4], w[2]))
          {
            PIXEL00_0
            PIXEL01_12
          }
          else
          {
            PIXEL00_90
            PIXEL01_61
          }
          PIXEL10_10
          PIXEL11_11
          break;
        }
        case 238:
        {
          PIXEL00_10
          PIXEL01_12
          if (Diff(w[8], w[4]))
          {
            PIXEL10_0
            PIXEL11_11
          }
          else
          {
            PIXEL10_90
            PIXEL11_60
          }
          break;
        }
        case 190:
        {
          PIXEL00_10
          if (Diff(w[2], w[6]))
          {
            PIXEL01_0
            PIXEL11_12
          }
          else
          {
            PIXEL01_90
            PIXEL11_61
          }
          PIXEL10_11
          break;
        }
        case 187:
        {
          if (Diff(w[4], w[2]))
          {
            PIXEL00_0
            PIXEL10_11
          }
          else
          {
            PIXEL00_90
            PIXEL10_60
          }
          PIXEL01_10
          PIXEL11_12
          break;
        }
        case 243:
        {
          PIXEL00_11
          PIXEL01_10
          if (Diff(w[6], w[8]))
          {
            PIXEL10_12
            PIXEL11_0
          }
          else
          {
            PIXEL10_61
            PIXEL11_90
          }
          break;
        }
        case 119:
        {
          if (Diff(w[2], w[6]))
          {
            PIXEL00_11
            PIXEL01_0
          }
          else
          {
            PIXEL00_60
            PIXEL01_90
          }
          PIXEL10_12
          PIXEL11_10
          break;
        }
        case 237:
        case 233:
        {
          PIXEL00_12
          PIXEL01_20
          if (Diff(w[8], w[4]))
          {
            PIXEL10_0
          }
          else
          {
            PIXEL10_100
          }
          PIXEL11_11
          break;
        }
        case 175:
        case 47:
        {
          if (Diff(w[4], w[2]))
          {
            PIXEL00_0
          }
          else
          {
            PIXEL00_100
          }
          PIXEL01_12
          PIXEL10_11
          PIXEL11_20
          break;
        }
        case 183:
        case 151:
        {
          PIXEL00_11
          if (Diff(w[2], w[6]))
          {
            PIXEL01_0
          }
          else
          {
            PIXEL01_100
          }
          PIXEL10_20
          PIXEL11_12
          break;
        }
        case 245:
        case 244:
        {
          PIXEL00_20
          PIXEL01_11
          PIXEL10_12
          if (Diff(w[6], w[8]))
          {
            PIXEL11_0
          }
          else
          {
            PIXEL11_100
          }
          break;
        }
        case 250:
        {
          PIXEL00_10
          PIXEL01_10
          if (Diff(w[8], w[4]))
          {
            PIXEL10_0
          }
          else
          {
            PIXEL10_20
          }
          if (Diff(w[6], w[8]))
          {
            PIXEL11_0
          }
          else
          {
            PIXEL11_20
          }
          break;
        }
        case 123:
        {
          if (Diff(w[4], w[2]))
          {
            PIXEL00_0
          }
          else
          {
            PIXEL00_20
          }
          PIXEL01_10
          if (Diff(w[8], w[4]))
          {
            PIXEL10_0
          }
          else
          {
            PIXEL10_20
          }
          PIXEL11_10
          break;
        }
        case 95:
        {
          if (Diff(w[4], w[2]))
          {
            PIXEL00_0
          }
          else
          {
            PIXEL00_20
          }
          if (Diff(w[2], w[6]))
          {
            PIXEL01_0
          }
          else
          {
            PIXEL01_20
          }
          PIXEL10_10
          PIXEL11_10
          break;
        }
        case 222:
        {
          PIXEL00_10
          if (Diff(w[2], w[6]))
          {
            PIXEL01_0
          }
          else
          {
            PIXEL01_20
          }
          PIXEL10_10
          if (Diff(w[6], w[8]))
          {
            PIXEL11_0
          }
          else
          {
            PIXEL11_20
          }
          break;
        }
        case 252:
        {
          PIXEL00_21
          PIXEL01_11
          if (Diff(w[8], w[4]))
          {
            PIXEL10_0
          }
          else
          {
            PIXEL10_20
          }
          if (Diff(w[6], w[8]))
          {
            PIXEL11_0
          }
          else
          {
            PIXEL11_100
          }
          break;
        }
        case 249:
        {
          PIXEL00_12
          PIXEL01_22
          if (Diff(w[8], w[4]))
          {
            PIXEL10_0
          }
          else
          {
            PIXEL10_100
          }
          if (Diff(w[6], w[8]))
          {
            PIXEL11_0
          }
          else
          {
            PIXEL11_20
          }
          break;
        }
        case 235:
        {
          if (Diff(w[4], w[2]))
          {
            PIXEL00_0
          }
          else
          {
            PIXEL00_20
          }
          PIXEL01_21
          if (Diff(w[8], w[4]))
          {
            PIXEL10_0
          }
          else
          {
            PIXEL10_100
          }
          PIXEL11_11
          break;
        }
        case 111:
        {
          if (Diff(w[4], w[2]))
          {
            PIXEL00_0
          }
          else
          {
            PIXEL00_100
          }
          PIXEL01_12
          if (Diff(w[8], w[4]))
          {
            PIXEL10_0
          }
          else
          {
            PIXEL10_20
          }
          PIXEL11_22
          break;
        }
        case 63:
        {
          if (Diff(w[4], w[2]))
          {
            PIXEL00_0
          }
          else
          {
            PIXEL00_100
          }
          if (Diff(w[2], w[6]))
          {
            PIXEL01_0
          }
          else
          {
            PIXEL01_20
          }
          PIXEL10_11
          PIXEL11_21
          break;
        }
        case 159:
        {
          if (Diff(w[4], w[2]))
          {
            PIXEL00_0
          }
          else
          {
            PIXEL00_20
          }
          if (Diff(w[2], w[6]))
          {
            PIXEL01_0
          }
          else
          {
            PIXEL01_100
          }
          PIXEL10_22
          PIXEL11_12
          break;
        }
        case 215:
        {
          PIXEL00_11
          if (Diff(w[2], w[6]))
          {
            PIXEL01_0
          }
          else
          {
            PIXEL01_100
          }
          PIXEL10_21
          if (Diff(w[6], w[8]))
          {
            PIXEL11_0
          }
          else
          {
            PIXEL11_20
          }
          break;
        }
        case 246:
        {
          PIXEL00_22
          if (Diff(w[2], w[6]))
          {
            PIXEL01_0
          }
          else
          {
            PIXEL01_20
          }
          PIXEL10_12
          if (Diff(w[6], w[8]))
          {
            PIXEL11_0
          }
          else
          {
            PIXEL11_100
          }
          break;
        }
        case 254:
        {
          PIXEL00_10
          if (Diff(w[2], w[6]))
          {
            PIXEL01_0
          }
          else
          {
            PIXEL01_20
          }
          if (Diff(w[8], w[4]))
          {
            PIXEL10_0
          }
          else
          {
            PIXEL10_20
          }
          if (Diff(w[6], w[8]))
          {
            PIXEL11_0
          }
          else
          {
            PIXEL11_100
          }
          break;
        }
        case 253:
        {
          PIXEL00_12
          PIXEL01_11
          if (Diff(w[8], w[4]))
          {
            PIXEL10_0
          }
          else
          {
            PIXEL10_100
          }
          if (Diff(w[6], w[8]))
          {
            PIXEL11_0
          }
          else
          {
            PIXEL11_100
          }
          break;
        }
        case 251:
        {
          if (Diff(w[4], w[2]))
          {
            PIXEL00_0
          }
          else
          {
            PIXEL00_20
          }
          PIXEL01_10
          if (Diff(w[8], w[4]))
          {
            PIXEL10_0
          }
          else
          {
            PIXEL10_100
          }
          if (Diff(w[6], w[8]))
          {
            PIXEL11_0
          }
          else
          {
            PIXEL11_20
          }
          break;
        }
        case 239:
        {
          if (Diff(w[4], w[2]))
          {
            PIXEL00_0
          }
          else
          {
            PIXEL00_100
          }
          PIXEL01_12
          if (Diff(w[8], w[4]))
          {
            PIXEL10_0
          }
          else
          {
            PIXEL10_100
          }
          PIXEL11_11
          break;
        }
        case 127:
        {
          if (Diff(w[4], w[2]))
          {
            PIXEL00_0
          }
          else
          {
            PIXEL00_100
          }
          if (Diff(w[2], w[6]))
          {
            PIXEL01_0
          }
          else
          {
            PIXEL01_20
          }
          if (Diff(w[8], w[4]))
          {
            PIXEL10_0
          }
          else
          {
            PIXEL10_20
          }
          PIXEL11_10
          break;
        }
        case 191:
        {
          if (Diff(w[4], w[2]))
          {
            PIXEL00_0
          }
          else
          {
            PIXEL00_100
          }
          if (Diff(w[2], w[6]))
          {
            PIXEL01_0
          }
          else
          {
            PIXEL01_100
          }
          PIXEL10_11
          PIXEL11_12
          break;
        }
        case 223:
        {
          if (Diff(w[4], w[2]))
          {
            PIXEL00_0
          }
          else
          {
            PIXEL00_20
          }
          if (Diff(w[2], w[6]))
          {
            PIXEL01_0
          }
          else
          {
            PIXEL01_100
          }
          PIXEL10_10
          if (Diff(w[6], w[8]))
          {
            PIXEL11_0
          }
          else
          {
            PIXEL11_20
          }
          break;
        }
        case 247:
        {
          PIXEL00_11
          if (Diff(w[2], w[6]))
          {
            PIXEL01_0
          }
          else
          {
            PIXEL01_100
          }
          PIXEL10_12
          if (Diff(w[6], w[8]))
          {
            PIXEL11_0
          }
          else
          {
            PIXEL11_100
          }
          break;
        }
        case 255:
        {
          if (Diff(w[4], w[2]))
          {
            PIXEL00_0
          }
          else
          {
            PIXEL00_100
          }
          if (Diff(w[2], w[6]))
          {
            PIXEL01_0
          }
          else
          {
            PIXEL01_100
          }
          if (Diff(w[8], w[4]))
          {
            PIXEL10_0
          }
          else
          {
            PIXEL10_100
          }
          if (Diff(w[6], w[8]))
          {
            PIXEL11_0
          }
          else
          {
            PIXEL11_100
          }
          break;
        }
      }
      pIn+=2;
      pOut+=8;
    }
  }
 _mm_empty();
}

#undef PIXEL00_0
#undef PIXEL00_10
#undef PIXEL00_11
#undef PIXEL00_12
#undef PIXEL00_20
#undef PIXEL00_21
#undef PIXEL00_22
#undef PIXEL00_60
#undef PIXEL00_61
#undef PIXEL00_70
#undef PIXEL00_90
#undef PIXEL00_100
#undef PIXEL01_0
#undef PIXEL01_10
#undef PIXEL01_11
#undef PIXEL01_12
#undef PIXEL01_20
#undef PIXEL01_21
#undef PIXEL01_22
#undef PIXEL01_60
#undef PIXEL01_61
#undef PIXEL01_70
#undef PIXEL01_90
#undef PIXEL01_100
#undef PIXEL10_0
#undef PIXEL10_10
#undef PIXEL10_11
#undef PIXEL10_12
#undef PIXEL10_20
#undef PIXEL10_21
#undef PIXEL10_22
#undef PIXEL10_60
#undef PIXEL10_61
#undef PIXEL10_70
#undef PIXEL10_90
#undef PIXEL10_100
#undef PIXEL11_0
#undef PIXEL11_10
#undef PIXEL11_11
#undef PIXEL11_12
#undef PIXEL11_20
#undef PIXEL11_21
#undef PIXEL11_22
#undef PIXEL11_60
#undef PIXEL11_61
#undef PIXEL11_70
#undef PIXEL11_90
#undef PIXEL11_100
