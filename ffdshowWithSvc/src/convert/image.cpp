/**************************************************************************
 *
 *  XVID MPEG-4 VIDEO CODEC
 *  image stuff
 *
 *  This program is an implementation of a part of one or more MPEG-4
 *  Video tools as specified in ISO/IEC 14496-2 standard.  Those intending
 *  to use this software module in hardware or software products are
 *  advised that its use may infringe existing patents or copyrights, and
 *  any such use would be at such party's own risk.  The original
 *  developer of this software module and his/her company, and subsequent
 *  editors and their companies, will have no liability for use of this
 *  software or modifications or derivatives thereof.
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
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 *************************************************************************/

#include "stdafx.h"
#include "image.h"
#include "ffImgfmt.h"
#include "Tconfig.h"
#include "simd.h"
#include "ffdebug.h"

typedef void (packedFunc)(uint8_t * x_ptr,
                          stride_t x_stride,
                          uint8_t * y_src,
                          uint8_t * v_src,
                          uint8_t * u_src,
                          stride_t y_stride,
                          stride_t uv_stride,
                          int width,
                          int height);
typedef packedFunc *packedFuncPtr;

static packedFuncPtr yuyv_to_yv12;
static packedFuncPtr uyvy_to_yv12;

static packedFuncPtr yuyvi_to_yv12;
static packedFuncPtr uyvyi_to_yv12;

static packedFuncPtr yv12_to_yuyv;
static packedFuncPtr yv12_to_uyvy;

static packedFuncPtr yv12_to_yuyvi;
static packedFuncPtr yv12_to_uyvyi;

typedef void (planarFunc) (uint8_t * y_dst, uint8_t * u_dst, uint8_t * v_dst,
                           stride_t y_dst_stride, stride_t uv_dst_stride,
                           const uint8_t * y_src, const uint8_t * u_src, const uint8_t * v_src,
                           stride_t y_src_stride, stride_t uv_src_stride,
                           int width, int height);
typedef planarFunc *planarFuncPtr;

static planarFuncPtr yv12_to_yv12;

/* yv12 to yv12 copy function */
static void yv12_to_yv12_c(uint8_t * y_dst, uint8_t * u_dst, uint8_t * v_dst,
				stride_t y_dst_stride, stride_t uv_dst_stride,
				const uint8_t * y_src, const uint8_t * u_src, const uint8_t * v_src,
				stride_t y_src_stride, stride_t uv_src_stride,
				int width, int height)
{
	int width2 = width / 2;
	int height2 = height / 2;
	int y;

    for (y = height; y; y--)    {
		memcpy(y_dst, y_src, width);
		y_src += y_src_stride;
		y_dst += y_dst_stride;
    }

	for (y = height2; y; y--) {
		memcpy(u_dst, u_src, width2);
		u_src += uv_src_stride;
		u_dst += uv_dst_stride;
    }

	for (y = height2; y; y--) {
		memcpy(v_dst, v_src, width2);
		v_src += uv_src_stride;
		v_dst += uv_dst_stride;
    }
}

enum {CCIR=1,JPEG=2};
template<int CCIR> struct YUV_RGB_DATA
{
 static int32_t RGB_Y_tab[256];
 static int32_t B_U_tab[256];
 static int32_t G_U_tab[256];
 static int32_t G_V_tab[256];
 static int32_t R_V_tab[256];

 static const double Y_R_IN,Y_G_IN,Y_B_IN;static const int Y_ADD_IN;
 static const double U_R_IN,U_G_IN,U_B_IN;static const int U_ADD_IN;
 static const double V_R_IN,V_G_IN,V_B_IN;static const int V_ADD_IN;

 static const double RGB_Y_OUT,B_U_OUT;static const int Y_ADD_OUT;
 static const double G_U_OUT,G_V_OUT;static const int U_ADD_OUT;
 static const double R_V_OUT;static const int V_ADD_OUT;

 static const int Y_ADD,U_ADD,V_ADD;

 static const __int64 Y_SUB,U_SUB,V_SUB,Y_MUL,UG_MUL,VG_MUL,UB_MUL,VR_MUL,y_mul,u_mul,v_mul;
};

/*  rgb -> yuv def's

    this following constants are "official spec"
    Video Demystified" (ISBN 1-878707-09-4)

    rgb<->yuv _is_ lossy, since most programs do the conversion differently

    SCALEBITS/FIX taken from  ffmpeg
*/

#define MAKE_M64_16(d,c,b,a) uint64_t(short(d))+(uint64_t(short(c))<<16)+(uint64_t(short(b))<<32)+(uint64_t(short(a))<<48)
#define MAKE_M64_16_1(a) MAKE_M64_16(a,a,a,a)
#define FIX(a) int((a)*256+0.5)
#define FIX64(a) int((a)*64+0.5)
//-------------------- ccir --------------------
template<> const int YUV_RGB_DATA<CCIR>::Y_ADD=16;
template<> const int YUV_RGB_DATA<CCIR>::U_ADD=128;
template<> const int YUV_RGB_DATA<CCIR>::V_ADD=128;

template<> int32_t YUV_RGB_DATA<CCIR>::RGB_Y_tab[256]={};
template<> int32_t YUV_RGB_DATA<CCIR>::B_U_tab[256]={};
template<> int32_t YUV_RGB_DATA<CCIR>::G_U_tab[256]={};
template<> int32_t YUV_RGB_DATA<CCIR>::G_V_tab[256]={};
template<> int32_t YUV_RGB_DATA<CCIR>::R_V_tab[256]={};

template<> const double YUV_RGB_DATA<CCIR>::Y_R_IN=0.299*219.0/255.0;
template<> const double YUV_RGB_DATA<CCIR>::Y_G_IN=0.587*219.0/255.0;
template<> const double YUV_RGB_DATA<CCIR>::Y_B_IN=0.114*219.0/255.0;
template<> const int YUV_RGB_DATA<CCIR>::Y_ADD_IN=16;

template<> const double YUV_RGB_DATA<CCIR>::U_R_IN=0.16874*224.0/255.0;
template<> const double YUV_RGB_DATA<CCIR>::U_G_IN=0.33126*224.0/255.0;
template<> const double YUV_RGB_DATA<CCIR>::U_B_IN=0.50000*224.0/255.0;
template<> const int YUV_RGB_DATA<CCIR>::U_ADD_IN=128;

template<> const double YUV_RGB_DATA<CCIR>::V_R_IN=0.50000*224.0/255.0;
template<> const double YUV_RGB_DATA<CCIR>::V_G_IN=0.41869*224.0/255.0;
template<> const double YUV_RGB_DATA<CCIR>::V_B_IN=0.08131*224.0/255.0;
template<> const int YUV_RGB_DATA<CCIR>::V_ADD_IN=128;

template<> const double YUV_RGB_DATA<CCIR>::RGB_Y_OUT=1.164;
template<> const double YUV_RGB_DATA<CCIR>::B_U_OUT  =1.77200*255.0/224.0;
template<> const int YUV_RGB_DATA<CCIR>::Y_ADD_OUT   =16;

template<> const double YUV_RGB_DATA<CCIR>::G_U_OUT  =0.34414*255.0/224.0;
template<> const double YUV_RGB_DATA<CCIR>::G_V_OUT  =0.71414*255.0/224.0;
template<> const int YUV_RGB_DATA<CCIR>::U_ADD_OUT   =128;

template<> const double YUV_RGB_DATA<CCIR>::R_V_OUT  =1.40200*255.0/224.0;
template<> const int YUV_RGB_DATA<CCIR>::V_ADD_OUT   =128;

template<> const __int64 YUV_RGB_DATA<CCIR>::Y_SUB=  MAKE_M64_16_1(Y_ADD);
template<> const __int64 YUV_RGB_DATA<CCIR>::U_SUB=  MAKE_M64_16_1(U_ADD);
template<> const __int64 YUV_RGB_DATA<CCIR>::V_SUB=  MAKE_M64_16_1(V_ADD);

template<> const __int64 YUV_RGB_DATA<CCIR>::Y_MUL=  MAKE_M64_16_1(FIX64(RGB_Y_OUT));

template<> const __int64 YUV_RGB_DATA<CCIR>::UG_MUL= MAKE_M64_16_1(FIX64(G_U_OUT));
template<> const __int64 YUV_RGB_DATA<CCIR>::VG_MUL= MAKE_M64_16_1(FIX64(G_V_OUT));

template<> const __int64 YUV_RGB_DATA<CCIR>::UB_MUL= MAKE_M64_16_1(FIX64(B_U_OUT));
template<> const __int64 YUV_RGB_DATA<CCIR>::VR_MUL= MAKE_M64_16_1(FIX64(R_V_OUT));
//                                                                 FIX(Y_B            FIX(Y_G)            FIX(Y_R)
template<> const __int64 YUV_RGB_DATA<CCIR>::y_mul=  MAKE_M64_16( FIX(Y_B_IN)/* 25*/, FIX(Y_G_IN)/*129*/, FIX(Y_R_IN)/* 66*/, 0);
template<> const __int64 YUV_RGB_DATA<CCIR>::u_mul=  MAKE_M64_16( FIX(U_B_IN)/*112*/,-FIX(U_G_IN)/*-74*/,-FIX(U_R_IN)/*-38*/, 0);
template<> const __int64 YUV_RGB_DATA<CCIR>::v_mul=  MAKE_M64_16(-FIX(V_B_IN)/*-18*/,-FIX(V_G_IN)/*-94*/, FIX(V_R_IN)/*112*/, 0);

//-------------------- jpeg --------------------
template<> const int YUV_RGB_DATA<JPEG>::Y_ADD=0;
template<> const int YUV_RGB_DATA<JPEG>::U_ADD=128;
template<> const int YUV_RGB_DATA<JPEG>::V_ADD=128;

template<> int32_t YUV_RGB_DATA<JPEG>::RGB_Y_tab[256]={};
template<> int32_t YUV_RGB_DATA<JPEG>::B_U_tab[256]={};
template<> int32_t YUV_RGB_DATA<JPEG>::G_U_tab[256]={};
template<> int32_t YUV_RGB_DATA<JPEG>::G_V_tab[256]={};
template<> int32_t YUV_RGB_DATA<JPEG>::R_V_tab[256]={};

template<> const double YUV_RGB_DATA<JPEG>::Y_R_IN=0.299;
template<> const double YUV_RGB_DATA<JPEG>::Y_G_IN=0.587;
template<> const double YUV_RGB_DATA<JPEG>::Y_B_IN=0.114;
template<> const int YUV_RGB_DATA<JPEG>::Y_ADD_IN=0;

template<> const double YUV_RGB_DATA<JPEG>::U_R_IN=0.16874;
template<> const double YUV_RGB_DATA<JPEG>::U_G_IN=0.33126;
template<> const double YUV_RGB_DATA<JPEG>::U_B_IN=0.50000;
template<> const int YUV_RGB_DATA<JPEG>::U_ADD_IN=128;

template<> const double YUV_RGB_DATA<JPEG>::V_R_IN=0.50000;
template<> const double YUV_RGB_DATA<JPEG>::V_G_IN=0.41869;
template<> const double YUV_RGB_DATA<JPEG>::V_B_IN=0.08131;
template<> const int YUV_RGB_DATA<JPEG>::V_ADD_IN=128;

template<> const double YUV_RGB_DATA<JPEG>::RGB_Y_OUT=1;
template<> const double YUV_RGB_DATA<JPEG>::B_U_OUT  =1.77200;
template<> const int YUV_RGB_DATA<JPEG>::Y_ADD_OUT   =0;

template<> const double YUV_RGB_DATA<JPEG>::G_U_OUT  =0.34414;
template<> const double YUV_RGB_DATA<JPEG>::G_V_OUT  =0.71414;
template<> const int YUV_RGB_DATA<JPEG>::U_ADD_OUT   =128;

template<> const double YUV_RGB_DATA<JPEG>::R_V_OUT  =1.40200;
template<> const int YUV_RGB_DATA<JPEG>::V_ADD_OUT   =128;

template<> const __int64 YUV_RGB_DATA<JPEG>::Y_SUB=  MAKE_M64_16_1(Y_ADD);
template<> const __int64 YUV_RGB_DATA<JPEG>::U_SUB=  MAKE_M64_16_1(U_ADD);
template<> const __int64 YUV_RGB_DATA<JPEG>::V_SUB=  MAKE_M64_16_1(V_ADD);

template<> const __int64 YUV_RGB_DATA<JPEG>::Y_MUL=  MAKE_M64_16_1(FIX64(RGB_Y_OUT));

template<> const __int64 YUV_RGB_DATA<JPEG>::UG_MUL= MAKE_M64_16_1(FIX64(G_U_OUT));
template<> const __int64 YUV_RGB_DATA<JPEG>::VG_MUL= MAKE_M64_16_1(FIX64(G_V_OUT));

template<> const __int64 YUV_RGB_DATA<JPEG>::UB_MUL= MAKE_M64_16_1(FIX64(B_U_OUT));
template<> const __int64 YUV_RGB_DATA<JPEG>::VR_MUL= MAKE_M64_16_1(FIX64(R_V_OUT));
//                                                                 FIX(Y_B     FIX(Y_G)     FIX(Y_R)
template<> const __int64 YUV_RGB_DATA<JPEG>::y_mul=  MAKE_M64_16( FIX(Y_B_IN), FIX(Y_G_IN), FIX(Y_R_IN), 0);
template<> const __int64 YUV_RGB_DATA<JPEG>::u_mul=  MAKE_M64_16( FIX(U_B_IN),-FIX(U_G_IN),-FIX(U_R_IN), 0);
template<> const __int64 YUV_RGB_DATA<JPEG>::v_mul=  MAKE_M64_16(-FIX(V_B_IN),-FIX(V_G_IN), FIX(V_R_IN), 0);

#undef FIX
#undef FIX64
#undef MAKE_M64_16_1
#undef MAKE_M64_16

struct RGB555
{
 static __forceinline uint16_t MK(int R,int G,int B)
  {
   return (uint16_t)(((limit_uint8(R) << 7) & 0x7c00) |
                     ((limit_uint8(G) << 2) & 0x03e0) |
                     ((limit_uint8(B) >> 3) & 0x001f));
  }
 static __forceinline uint32_t B(uint32_t RGB) {return (RGB << 3) & 0xf8;}
 static __forceinline uint32_t G(uint32_t RGB) {return (RGB >> 2) & 0xf8;}
 static __forceinline uint32_t R(uint32_t RGB) {return (RGB >> 7) & 0xf8;}
};

struct RGB565
{
 static __forceinline uint16_t MK(int R,int G,int B)
  {
   return (uint16_t)(((limit_uint8(R) << 8) & 0xf800) |
                     ((limit_uint8(G) << 3) & 0x07e0) |
                     ((limit_uint8(B) >> 3) & 0x001f));
  }
 static __forceinline uint32_t B(uint32_t RGB) {return (RGB << 3) & 0xf8;}
 static __forceinline uint32_t G(uint32_t RGB) {return (RGB >> 3) & 0xfc;}
 static __forceinline uint32_t R(uint32_t RGB) {return (RGB >> 8) & 0xf8;}
};

static const int SCALEBITS_OUT=13;
static const int SCALEBITS_IN=8;
#define FIX_IN(x) ((uint16_t) ((x) * (1L<<SCALEBITS_IN) + 0.5))

template<int SIZE,int PIXELS,int VPIXELS,class C1,int C2,int C3,int C4,int CCIR> struct TMAKE_COLORSPACE
{

 template<int ROW> static __forceinline void WRITE_RGB16(uint8_t *x_ptr,stride_t x_stride,
                                                         uint8_t *y_ptr,stride_t y_stride,
                                                         int r[2],int g[2],int b[2],
                                                         const int b_u,const int g_uv,const int r_v)
  {
   int rgb_y = YUV_RGB_DATA<CCIR>::RGB_Y_tab[ y_ptr[y_stride*(ROW) + 0] ];
   b[ROW] = (b[ROW] & 0x7) + ((rgb_y + b_u) >> SCALEBITS_OUT);
   g[ROW] = (g[ROW] & 0x7) + ((rgb_y - g_uv) >> SCALEBITS_OUT);
   r[ROW] = (r[ROW] & 0x7) + ((rgb_y + r_v) >> SCALEBITS_OUT);
   *(uint16_t *) (x_ptr+((ROW)*x_stride)+0) = C1::MK(r[ROW], g[ROW], b[ROW]);
   rgb_y = YUV_RGB_DATA<CCIR>::RGB_Y_tab[ y_ptr[y_stride*(ROW) + 1] ];
   b[ROW] = (b[ROW] & 0x7) + ((rgb_y + b_u) >> SCALEBITS_OUT);
   g[ROW] = (g[ROW] & 0x7) + ((rgb_y - g_uv) >> SCALEBITS_OUT);
   r[ROW] = (r[ROW] & 0x7) + ((rgb_y + r_v) >> SCALEBITS_OUT);
   *(uint16_t *) (x_ptr+((ROW)*x_stride)+2) = C1::MK(r[ROW], g[ROW], b[ROW]);
  }

 template<int ROW> static __forceinline void READ_RGB16_Y(uint8_t *x_ptr,stride_t x_stride,
                                                          uint8_t *y_ptr,stride_t y_stride,
                                                          uint32_t &r,uint32_t &g,uint32_t &b,uint32_t &r0,uint32_t &g0,uint32_t &b0)
  {
   uint32_t rgb = *(uint16_t *) (x_ptr + ((ROW)*x_stride) + 0);
   b0 += b = C1::B(rgb);
   g0 += g = C1::G(rgb);
   r0 += r = C1::R(rgb);
   y_ptr[(ROW)*y_stride+0] =
           (uint8_t)((uint8_t) ((FIX_IN(YUV_RGB_DATA<CCIR>::Y_R_IN) * r + FIX_IN(YUV_RGB_DATA<CCIR>::Y_G_IN) * g +
                                   FIX_IN(YUV_RGB_DATA<CCIR>::Y_B_IN) * b) >> SCALEBITS_IN) + YUV_RGB_DATA<CCIR>::Y_ADD_IN);
   rgb = *(uint16_t *) (x_ptr + ((ROW)*x_stride) + 2);
   b0 += b = C1::B(rgb);
   g0 += g = C1::G(rgb);
   r0 += r = C1::R(rgb);
   y_ptr[(ROW)*y_stride+1] =
           (uint8_t)((uint8_t) ((FIX_IN(YUV_RGB_DATA<CCIR>::Y_R_IN) * r + FIX_IN(YUV_RGB_DATA<CCIR>::Y_G_IN) * g +
                                   FIX_IN(YUV_RGB_DATA<CCIR>::Y_B_IN) * b) >> SCALEBITS_IN) + YUV_RGB_DATA<CCIR>::Y_ADD_IN);
  }
 template<int ROW,int UV_ROW> static __forceinline void READ_RGB16_UV(uint8_t *u_ptr,uint8_t *v_ptr,stride_t uv_stride,
                                                           uint32_t &r,uint32_t &g,uint32_t &b,uint32_t &r0,uint32_t &g0,uint32_t &b0)
  {
   u_ptr[(UV_ROW)*uv_stride] =
           (uint8_t)((uint8_t) ((-FIX_IN(YUV_RGB_DATA<CCIR>::U_R_IN) * r0 - FIX_IN(YUV_RGB_DATA<CCIR>::U_G_IN) * g0 +
                                   FIX_IN(YUV_RGB_DATA<CCIR>::U_B_IN) * b0) >> (SCALEBITS_IN + 2)) + YUV_RGB_DATA<CCIR>::U_ADD_IN);
   v_ptr[(UV_ROW)*uv_stride] =
           (uint8_t)((uint8_t) ((FIX_IN(YUV_RGB_DATA<CCIR>::V_R_IN) * r0 - FIX_IN(YUV_RGB_DATA<CCIR>::V_G_IN) * g0 -
                                   FIX_IN(YUV_RGB_DATA<CCIR>::V_B_IN) * b0) >> (SCALEBITS_IN + 2)) + YUV_RGB_DATA<CCIR>::V_ADD_IN);
  }

 template<int ROW> static __forceinline void WRITE_RGB(uint8_t *x_ptr,stride_t x_stride,
                                                       uint8_t *y_ptr,stride_t y_stride,
                                                       int r[2],int g[2],int b[2],
                                                       const int b_u,const int g_uv,const int r_v)
  {
   int rgb_y = YUV_RGB_DATA<CCIR>::RGB_Y_tab[ y_ptr[(ROW)*y_stride + 0] ];
   x_ptr[(ROW)*x_stride+(C3)] = limit_uint8((rgb_y + b_u) >> SCALEBITS_OUT);
   x_ptr[(ROW)*x_stride+(C2)] = limit_uint8((rgb_y - g_uv) >> SCALEBITS_OUT);
   x_ptr[(ROW)*x_stride+(C1::value)] = limit_uint8((rgb_y + r_v) >> SCALEBITS_OUT);
   if ((SIZE)>3) x_ptr[(ROW)*x_stride+(C4)] = 0;
   rgb_y = YUV_RGB_DATA<CCIR>::RGB_Y_tab[ y_ptr[(ROW)*y_stride + 1] ];
   x_ptr[(ROW)*x_stride+(SIZE)+(C3)] = limit_uint8((rgb_y + b_u) >> SCALEBITS_OUT);
   x_ptr[(ROW)*x_stride+(SIZE)+(C2)] = limit_uint8((rgb_y - g_uv) >> SCALEBITS_OUT);
   x_ptr[(ROW)*x_stride+(SIZE)+(C1::value)] = limit_uint8((rgb_y + r_v) >> SCALEBITS_OUT);
   if ((SIZE)>3) x_ptr[(ROW)*x_stride+(SIZE)+(C4)] = 0;
  }

 template<int ROW> static __forceinline void READ_RGB_Y(uint8_t *x_ptr,stride_t x_stride,
                                                          uint8_t *y_ptr,stride_t y_stride,
                                                          uint32_t &r,uint32_t &g,uint32_t &b,uint32_t &r0,uint32_t &g0,uint32_t &b0)
  {
   r0 += r = x_ptr[(ROW)*x_stride+(C1::value)];
   g0 += g = x_ptr[(ROW)*x_stride+(C2)];
   b0 += b = x_ptr[(ROW)*x_stride+(C3)];
   y_ptr[(ROW)*y_stride+0] =
           (uint8_t)((uint8_t) ((FIX_IN(YUV_RGB_DATA<CCIR>::Y_R_IN) * r + FIX_IN(YUV_RGB_DATA<CCIR>::Y_G_IN) * g +
                                   FIX_IN(YUV_RGB_DATA<CCIR>::Y_B_IN) * b) >> SCALEBITS_IN) + YUV_RGB_DATA<CCIR>::Y_ADD_IN);
   r0 += r = x_ptr[(ROW)*x_stride+(SIZE)+(C1::value)];
   g0 += g = x_ptr[(ROW)*x_stride+(SIZE)+(C2)];
   b0 += b = x_ptr[(ROW)*x_stride+(SIZE)+(C3)];
   y_ptr[(ROW)*y_stride+1] =
           (uint8_t)((uint8_t) ((FIX_IN(YUV_RGB_DATA<CCIR>::Y_R_IN) * r + FIX_IN(YUV_RGB_DATA<CCIR>::Y_G_IN) * g +
                   FIX_IN(YUV_RGB_DATA<CCIR>::Y_B_IN) * b) >> SCALEBITS_IN) + YUV_RGB_DATA<CCIR>::Y_ADD_IN);
  }

 template<int ROW,int UV_ROW> static __forceinline void READ_RGB_UV(uint8_t *u_ptr,uint8_t *v_ptr,stride_t uv_stride,
                                                           uint32_t &r,uint32_t &g,uint32_t &b,uint32_t &r0,uint32_t &g0,uint32_t &b0)
  {
        u_ptr[(UV_ROW)*uv_stride] =
                (uint8_t)((uint8_t) ((-FIX_IN(YUV_RGB_DATA<CCIR>::U_R_IN) * r0 - FIX_IN(YUV_RGB_DATA<CCIR>::U_G_IN) * g0 +
                                        FIX_IN(YUV_RGB_DATA<CCIR>::U_B_IN) * b0) >> (SCALEBITS_IN + 2)) + YUV_RGB_DATA<CCIR>::U_ADD_IN);
        v_ptr[(UV_ROW)*uv_stride] =
                (uint8_t)((uint8_t) ((FIX_IN(YUV_RGB_DATA<CCIR>::V_R_IN) * r0 - FIX_IN(YUV_RGB_DATA<CCIR>::V_G_IN) * g0 -
                                        FIX_IN(YUV_RGB_DATA<CCIR>::V_B_IN) * b0) >> (SCALEBITS_IN + 2)) + YUV_RGB_DATA<CCIR>::V_ADD_IN);
  }

 struct YV12_TO_RGB16
  {
   static __forceinline void ROW(int r[4],int g[4],int b[4])
    {
     r[0] = r[1] = g[0] = g[1] = b[0] = b[1] = 0;
    }
   static __forceinline void PROCESS(uint8_t *x_ptr,stride_t x_stride,
                                     uint8_t *y_ptr,stride_t y_stride,
                                     uint8_t *u_ptr,uint8_t *v_ptr,stride_t uv_stride,
                                     int r[4],int g[4],int b[4],
                                     bool full=true)
    {
     int b_u = YUV_RGB_DATA<CCIR>::B_U_tab[ u_ptr[0] ];
     int g_uv = YUV_RGB_DATA<CCIR>::G_U_tab[ u_ptr[0] ] + YUV_RGB_DATA<CCIR>::G_V_tab[ v_ptr[0] ];
     int r_v = YUV_RGB_DATA<CCIR>::R_V_tab[ v_ptr[0] ];
     WRITE_RGB16<0>(x_ptr,x_stride,y_ptr,y_stride,r,g,b,b_u,g_uv,r_v);
     WRITE_RGB16<1>(x_ptr,x_stride,y_ptr,y_stride,r,g,b,b_u,g_uv,r_v);
    }
  };

 struct YV12_TO_RGB16I
  {
   static __forceinline void ROW(int r[4],int g[4],int b[4])
    {
     r[0] = r[1] = r[2] = r[3] = 0;
     g[0] = g[1] = g[2] = g[3] = 0;
     b[0] = b[1] = b[2] = b[3] = 0;
    }
   static __forceinline void PROCESS(uint8_t *x_ptr,stride_t x_stride,
                                     uint8_t *y_ptr,stride_t y_stride,
                                     uint8_t *u_ptr,uint8_t *v_ptr,stride_t uv_stride,
                                     int r[4],int g[4],int b[4],
                                     bool full=true)
    {
     int b_u0 = YUV_RGB_DATA<CCIR>::B_U_tab[ u_ptr[0] ];
     int g_uv0 = YUV_RGB_DATA<CCIR>::G_U_tab[ u_ptr[0] ] + YUV_RGB_DATA<CCIR>::G_V_tab[ v_ptr[0] ];
     int r_v0 = YUV_RGB_DATA<CCIR>::R_V_tab[ v_ptr[0] ];
     int b_u1=0,g_uv1=0,r_v1=0;
     WRITE_RGB16<0>(x_ptr,x_stride,y_ptr,y_stride,r,g,b,b_u0,g_uv0,r_v0);
     if(full)
      {
       b_u1 = YUV_RGB_DATA<CCIR>::B_U_tab[ u_ptr[uv_stride] ];
       g_uv1 = YUV_RGB_DATA<CCIR>::G_U_tab[ u_ptr[uv_stride] ] + YUV_RGB_DATA<CCIR>::G_V_tab[ v_ptr[uv_stride] ];
       r_v1 = YUV_RGB_DATA<CCIR>::R_V_tab[ v_ptr[uv_stride] ];
       WRITE_RGB16<1>(x_ptr,x_stride,y_ptr,y_stride,r,g,b,b_u1,g_uv1,r_v1);
       WRITE_RGB16<2>(x_ptr,x_stride,y_ptr,y_stride,r,g,b,b_u0,g_uv0,r_v0);
       WRITE_RGB16<3>(x_ptr,x_stride,y_ptr,y_stride,r,g,b,b_u1,g_uv1,r_v1);
      }
     else
      WRITE_RGB16<1>(x_ptr,x_stride,y_ptr,y_stride,r,g,b,b_u0,g_uv0,r_v0);
    }
  };

 struct YV12_TO_RGB
  {
   static __forceinline void ROW(int r[4],int g[4],int b[4])
    {
    }
   static __forceinline void PROCESS(uint8_t *x_ptr,stride_t x_stride,
                                     uint8_t *y_ptr,stride_t y_stride,
                                     uint8_t *u_ptr,uint8_t *v_ptr,stride_t uv_stride,
                                     int r[4],int g[4],int b[4],
                                     bool full=true)
     {
      int b_u = YUV_RGB_DATA<CCIR>::B_U_tab[ u_ptr[0] ];
      int g_uv = YUV_RGB_DATA<CCIR>::G_U_tab[ u_ptr[0] ] + YUV_RGB_DATA<CCIR>::G_V_tab[ v_ptr[0] ];
      int r_v= YUV_RGB_DATA<CCIR>::R_V_tab[ v_ptr[0] ];
      WRITE_RGB<0>(x_ptr,x_stride,y_ptr,y_stride,r,g,b,b_u,g_uv,r_v);
      WRITE_RGB<1>(x_ptr,x_stride,y_ptr,y_stride,r,g,b,b_u,g_uv,r_v);
     }
  };

 struct YV12_TO_RGBI
  {
   static __forceinline void ROW(int r[4],int g[4],int b[4])
    {
    }
   static __forceinline void PROCESS(uint8_t *x_ptr,stride_t x_stride,
                                     uint8_t *y_ptr,stride_t y_stride,
                                     uint8_t *u_ptr,uint8_t *v_ptr,stride_t uv_stride,
                                     int r[4],int g[4],int b[4],
                                     bool full=true)
    {
     int b_u0 = YUV_RGB_DATA<CCIR>::B_U_tab[ u_ptr[0] ];
     int g_uv0 = YUV_RGB_DATA<CCIR>::G_U_tab[ u_ptr[0] ] + YUV_RGB_DATA<CCIR>::G_V_tab[ v_ptr[0] ];
     int r_v0= YUV_RGB_DATA<CCIR>::R_V_tab[ v_ptr[0] ];
     int b_u1=0,g_uv1=0,r_v1=0;

     WRITE_RGB<0>(x_ptr,x_stride,y_ptr,y_stride,r,g,b,b_u0,g_uv0,r_v0);
     if(full) // Without flag(full), it crashes when input size is not multiple of 4.
      {
       b_u1 = YUV_RGB_DATA<CCIR>::B_U_tab[ u_ptr[uv_stride] ];
       g_uv1 = YUV_RGB_DATA<CCIR>::G_U_tab[ u_ptr[uv_stride] ] + YUV_RGB_DATA<CCIR>::G_V_tab[ v_ptr[uv_stride] ];
       r_v1= YUV_RGB_DATA<CCIR>::R_V_tab[ v_ptr[uv_stride] ];
       WRITE_RGB<1>(x_ptr,x_stride,y_ptr,y_stride,r,g,b,b_u1,g_uv1,r_v1);
       WRITE_RGB<2>(x_ptr,x_stride,y_ptr,y_stride,r,g,b,b_u0,g_uv0,r_v0);
       WRITE_RGB<3>(x_ptr,x_stride,y_ptr,y_stride,r,g,b,b_u1,g_uv1,r_v1);
      }
     else
      WRITE_RGB<1>(x_ptr,x_stride,y_ptr,y_stride,r,g,b,b_u0,g_uv0,r_v0);
    }
  };

 template<int ROW,int UV_ROW> static __forceinline void WRITE_YUYV(uint8_t *x_ptr,stride_t x_stride,uint8_t *y_ptr,stride_t y_stride,uint8_t *u_ptr,uint8_t *v_ptr,stride_t uv_stride)
  {
   x_ptr[(ROW)*x_stride+(C1::value)] = y_ptr[   (ROW)*y_stride +0];
   x_ptr[(ROW)*x_stride+(C2)] = u_ptr[(UV_ROW)*uv_stride+0];
   x_ptr[(ROW)*x_stride+(C3)] = y_ptr[   (ROW)*y_stride +1];
   x_ptr[(ROW)*x_stride+(C4)] = v_ptr[(UV_ROW)*uv_stride+0];
  }

 struct YV12_TO_YUYV
  {
   static __forceinline void ROW(int r[4],int g[4],int b[4])
    {
    }
   static __forceinline void PROCESS(uint8_t *x_ptr,stride_t x_stride,
                                     uint8_t *y_ptr,stride_t y_stride,
                                     uint8_t *u_ptr,uint8_t *v_ptr,stride_t uv_stride,
                                     int r[4],int g[4],int b[4],
                                     bool full=true)
    {
     WRITE_YUYV<0, 0>(x_ptr,x_stride,y_ptr,y_stride,u_ptr,v_ptr,uv_stride);
     WRITE_YUYV<1, 0>(x_ptr,x_stride,y_ptr,y_stride,u_ptr,v_ptr,uv_stride);
    }
  };

 template<int ROW> static __forceinline void READ_YUYV_Y(uint8_t *y_ptr,stride_t y_stride,uint8_t *x_ptr,stride_t x_stride)
  {
   y_ptr[(ROW)*y_stride+0] = x_ptr[(ROW)*x_stride+(C1::value)];
   y_ptr[(ROW)*y_stride+1] = x_ptr[(ROW)*x_stride+(C3)];
  }
 template<int UV_ROW,int ROW1,int ROW2> static __forceinline void READ_YUYV_UV(uint8_t *x_ptr,stride_t x_stride,uint8_t *u_ptr,uint8_t *v_ptr,stride_t uv_stride)
  {
   u_ptr[(UV_ROW)*uv_stride] = (uint8_t)((x_ptr[(ROW1)*x_stride+(C2)] + x_ptr[(ROW2)*x_stride+(C2)] + 1) / 2);
   v_ptr[(UV_ROW)*uv_stride] = (uint8_t)((x_ptr[(ROW1)*x_stride+(C4)] + x_ptr[(ROW2)*x_stride+(C4)] + 1) / 2);
  }

 struct YV12_TO_YUYVI
  {
   static __forceinline void ROW(int r[4],int g[4],int b[4])
    {
    }
   static __forceinline void PROCESS(uint8_t *x_ptr,stride_t x_stride,
                                     uint8_t *y_ptr,stride_t y_stride,
                                     uint8_t *u_ptr,uint8_t *v_ptr,stride_t uv_stride,
                                     int r[4],int g[4],int b[4],
                                     bool full=true)
    {
     WRITE_YUYV<0, 0>(x_ptr,x_stride,y_ptr,y_stride,u_ptr,v_ptr,uv_stride);
     WRITE_YUYV<1, 1>(x_ptr,x_stride,y_ptr,y_stride,u_ptr,v_ptr,uv_stride);
     WRITE_YUYV<2, 0>(x_ptr,x_stride,y_ptr,y_stride,u_ptr,v_ptr,uv_stride);
     WRITE_YUYV<3, 1>(x_ptr,x_stride,y_ptr,y_stride,u_ptr,v_ptr,uv_stride);
    }
  };

 struct YUYV_TO_YV12
  {
   static __forceinline void ROW(int r[4],int g[4],int b[4])
    {
    }
   static __forceinline void PROCESS(uint8_t *x_ptr,stride_t x_stride,
                                     uint8_t *y_ptr,stride_t y_stride,
                                     uint8_t *u_ptr,uint8_t *v_ptr,stride_t uv_stride,
                                     int r[4],int g[4],int b[4],
                                     bool full=true)
    {
        READ_YUYV_Y<0>(y_ptr,y_stride,x_ptr,x_stride);
        READ_YUYV_Y<1>(y_ptr,y_stride,x_ptr,x_stride);
        READ_YUYV_UV<0, 0,1>(x_ptr,x_stride,u_ptr,v_ptr,uv_stride);
    }
  };
 struct YUYVI_TO_YV12
  {
   static __forceinline void ROW(int r[4],int g[4],int b[4])
    {
    }
   static __forceinline void PROCESS(uint8_t *x_ptr,stride_t x_stride,
                                     uint8_t *y_ptr,stride_t y_stride,
                                     uint8_t *u_ptr,uint8_t *v_ptr,stride_t uv_stride,
                                     int r[4],int g[4],int b[4],
                                     bool full=true)
    {
     READ_YUYV_Y<0>(y_ptr,y_stride,x_ptr,x_stride);
     READ_YUYV_Y<1>(y_ptr,y_stride,x_ptr,x_stride);
     READ_YUYV_Y<2>(y_ptr,y_stride,x_ptr,x_stride);
     READ_YUYV_Y<3>(y_ptr,y_stride,x_ptr,x_stride);
     READ_YUYV_UV<0, 0,2>(x_ptr,x_stride,u_ptr,v_ptr,uv_stride);
     READ_YUYV_UV<1, 1,3>(x_ptr,x_stride,u_ptr,v_ptr,uv_stride);
    }
  };

 struct RGB16_TO_YV12
  {
   static __forceinline void ROW(int r[4],int g[4],int b[4])
    {
    }
   static __forceinline void PROCESS(uint8_t *x_ptr,stride_t x_stride,
                                     uint8_t *y_ptr,stride_t y_stride,
                                     uint8_t *u_ptr,uint8_t *v_ptr,stride_t uv_stride,
                                     int [4],int [4],int [4],
                                     bool full=true)
    {
     uint32_t r, g, b, r0, g0, b0;
     r0 = g0 = b0 = 0;
     READ_RGB16_Y <0>(x_ptr,x_stride,y_ptr,y_stride,r,g,b,r0,g0,b0);
     READ_RGB16_Y <1>(x_ptr,x_stride,y_ptr,y_stride,r,g,b,r0,g0,b0);
     READ_RGB16_UV<0,0>(u_ptr,v_ptr,uv_stride,r,g,b,r0,g0,b0);
    }
  };

 struct RGB16I_TO_YV12
  {
   static __forceinline void ROW(int r[4],int g[4],int b[4])
    {
    }
   static __forceinline void PROCESS(uint8_t *x_ptr,stride_t x_stride,
                                     uint8_t *y_ptr,stride_t y_stride,
                                     uint8_t *u_ptr,uint8_t *v_ptr,stride_t uv_stride,
                                     int [4],int [4],int [4],
                                     bool full=true)
    {
     uint32_t r, g, b, r0, g0, b0, r1, g1, b1;
     r0 = g0 = b0 = r1 = g1 = b1 = 0;
     READ_RGB16_Y<0>(x_ptr,x_stride,y_ptr,y_stride,r,g,b,r0,g0,b0);
     READ_RGB16_Y<1>(x_ptr,x_stride,y_ptr,y_stride,r,g,b,r1,g1,b1);
     READ_RGB16_Y<2>(x_ptr,x_stride,y_ptr,y_stride,r,g,b,r0,g0,b0);
     READ_RGB16_Y<3>(x_ptr,x_stride,y_ptr,y_stride,r,g,b,r1,g1,b1);
     READ_RGB16_UV<0, 0>(u_ptr,v_ptr,uv_stride,r,g,b,r0,g0,b0);
     READ_RGB16_UV<1, 1>(u_ptr,v_ptr,uv_stride,r,g,b,r0,g0,b0);
    }
  };

 struct RGB_TO_YV12
  {
   static __forceinline void ROW(int r[4],int g[4],int b[4])
    {
    }
   static __forceinline void PROCESS(uint8_t *x_ptr,stride_t x_stride,
                                     uint8_t *y_ptr,stride_t y_stride,
                                     uint8_t *u_ptr,uint8_t *v_ptr,stride_t uv_stride,
                                     int [4],int [4],int [4],
                                     bool full=true)
    {
     uint32_t r, g, b, r0, g0, b0;
     r0 = g0 = b0 = 0;
     READ_RGB_Y<0>(x_ptr,x_stride,y_ptr,y_stride,r,g,b,r0,g0,b0);
     READ_RGB_Y<1>(x_ptr,x_stride,y_ptr,y_stride,r,g,b,r0,g0,b0);
     READ_RGB_UV<0,0>(u_ptr,v_ptr,uv_stride,r,g,b,r0,g0,b0);
    }
  };

 struct RGBI_TO_YV12
  {
   static __forceinline void ROW(int r[4],int g[4],int b[4])
    {
    }
   static __forceinline void PROCESS(uint8_t *x_ptr,stride_t x_stride,
                                     uint8_t *y_ptr,stride_t y_stride,
                                     uint8_t *u_ptr,uint8_t *v_ptr,stride_t uv_stride,
                                     int [4],int [4],int [4],
                                     bool full=true)
    {
     uint32_t r, g, b, r0, g0, b0, r1, g1, b1;
     r0 = g0 = b0 = r1 = g1 = b1 = 0;
     READ_RGB_Y<0>(x_ptr,x_stride,y_ptr,y_stride,r,g,b,r0,g0,b0);
     READ_RGB_Y<1>(x_ptr,x_stride,y_ptr,y_stride,r,g,b,r1,g1,b1);
     READ_RGB_Y<2>(x_ptr,x_stride,y_ptr,y_stride,r,g,b,r0,g0,b0);
     READ_RGB_Y<3>(x_ptr,x_stride,y_ptr,y_stride,r,g,b,r1,g1,b1);
     READ_RGB_UV<0, 0>(u_ptr,v_ptr,uv_stride,r,g,b,r0,g0,b0);
     READ_RGB_UV<1, 1>(u_ptr,v_ptr,uv_stride,r,g,b,r0,g0,b0);
    }
  };

 template<class TFUNC> static __forceinline void MAKE_COLORSPACE(uint8_t * x_ptr, stride_t x_stride,
                                   uint8_t * y_ptr, uint8_t * u_ptr, uint8_t * v_ptr,
                                   stride_t y_stride, stride_t uv_stride,
                                   int width, int height,const TFUNC &FUNC)
  {
        int fixed_width = (width + 1) & ~1;
        stride_t x_dif = x_stride - (SIZE)*fixed_width;
        stride_t y_dif = y_stride - fixed_width;
        stride_t uv_dif = uv_stride - (fixed_width / 2);
        for (int y = 0; y < height; y+=(VPIXELS)) {
                int r[4], g[4], b[4];
                FUNC.ROW(r,g,b);
                for (int x = 0; x < fixed_width; x+=(PIXELS)) {
                        FUNC.PROCESS(x_ptr,x_stride,y_ptr,y_stride,u_ptr,v_ptr,uv_stride,r,g,b,y+VPIXELS<=height);
                        x_ptr += (PIXELS)*(SIZE);
                        y_ptr += (PIXELS);
                        u_ptr += (PIXELS)/2;
                        v_ptr += (PIXELS)/2;
                }
                x_ptr += x_dif + (VPIXELS-1)*x_stride;
                y_ptr += y_dif + (VPIXELS-1)*y_stride;
                u_ptr += uv_dif + ((VPIXELS/2)-1)*uv_stride;
                v_ptr += uv_dif + ((VPIXELS/2)-1)*uv_stride;
        }
  }
};

#undef FIX_IN

//===================================== YUY2 ============================
//MAKE_COLORSPACE(yv12_to_yuyv_c,    2,2,2, YV12_TO_YUYV,   0,1,2,3,CCIR)
static void yv12_to_yuyv_c(uint8_t * x_ptr,stride_t x_stride,uint8_t * y_ptr,uint8_t * u_ptr,uint8_t * v_ptr,stride_t y_stride,stride_t uv_stride,int width,int height)
{
 typedef TMAKE_COLORSPACE<2,2,2, intToVal<0>, 1,2,3,CCIR> TMAKE_COLORSPACE_yv12_to_yuyv_c;
 TMAKE_COLORSPACE_yv12_to_yuyv_c::MAKE_COLORSPACE(x_ptr,x_stride,y_ptr,u_ptr,v_ptr,y_stride,uv_stride,width,height,TMAKE_COLORSPACE_yv12_to_yuyv_c::YV12_TO_YUYV());
}
//MAKE_COLORSPACE(yv12_to_uyvy_c,    2,2,2, YV12_TO_YUYV,   1,0,3,2,CCIR)
static void yv12_to_uyvy_c(uint8_t * x_ptr,stride_t x_stride,uint8_t * y_ptr,uint8_t * u_ptr,uint8_t * v_ptr,stride_t y_stride,stride_t uv_stride,int width,int height)
{
 typedef TMAKE_COLORSPACE<2,2,2, intToVal<1>, 0,3,2,CCIR> TMAKE_COLORSPACE_yv12_to_uyvy_c;
 TMAKE_COLORSPACE_yv12_to_uyvy_c::MAKE_COLORSPACE(x_ptr,x_stride,y_ptr,u_ptr,v_ptr,y_stride,uv_stride,width,height,TMAKE_COLORSPACE_yv12_to_uyvy_c::YV12_TO_YUYV());
}
//MAKE_COLORSPACE(yv12_to_yuyvi_c,   2,2,4, YV12_TO_YUYVI,  0,1,2,3,CCIR)
static void yv12_to_yuyvi_c(uint8_t * x_ptr,stride_t x_stride,uint8_t * y_ptr,uint8_t * u_ptr,uint8_t * v_ptr,stride_t y_stride,stride_t uv_stride,int width,int height)
{
 typedef TMAKE_COLORSPACE<2,2,4, intToVal<0>, 1,2,3,CCIR> TMAKE_COLORSPACE_yv12_to_yuyvi_c;
 TMAKE_COLORSPACE_yv12_to_yuyvi_c::MAKE_COLORSPACE(x_ptr,x_stride,y_ptr,u_ptr,v_ptr,y_stride,uv_stride,width,height,TMAKE_COLORSPACE_yv12_to_yuyvi_c::YV12_TO_YUYVI());
}
//MAKE_COLORSPACE(yv12_to_uyvyi_c,   2,2,4, YV12_TO_YUYVI,  1,0,3,2,CCIR)
static void yv12_to_uyvyi_c(uint8_t * x_ptr,stride_t x_stride,uint8_t * y_ptr,uint8_t * u_ptr,uint8_t * v_ptr,stride_t y_stride,stride_t uv_stride,int width,int height)
{
 typedef TMAKE_COLORSPACE<2,2,4, intToVal<1>, 0,3,2,CCIR> TMAKE_COLORSPACE_yv12_to_uyvyi_c;
 TMAKE_COLORSPACE_yv12_to_uyvyi_c::MAKE_COLORSPACE(x_ptr,x_stride,y_ptr,u_ptr,v_ptr,y_stride,uv_stride,width,height,TMAKE_COLORSPACE_yv12_to_uyvyi_c::YV12_TO_YUYVI());
}

//---------------------------------------------
//MAKE_COLORSPACE(yuyv_to_yv12_c,    2,2,2, YUYV_TO_YV12,   0,1,2,3,CCIR)
static void yuyv_to_yv12_c(uint8_t * x_ptr,stride_t x_stride,uint8_t * y_ptr,uint8_t * u_ptr,uint8_t * v_ptr,stride_t y_stride,stride_t uv_stride,int width,int height)
{
 typedef TMAKE_COLORSPACE<2,2,2, intToVal<0>, 1,2,3,CCIR> TMAKE_COLORSPACE_yuyv_to_yv12_c;
 TMAKE_COLORSPACE_yuyv_to_yv12_c::MAKE_COLORSPACE(x_ptr,x_stride,y_ptr,u_ptr,v_ptr,y_stride,uv_stride,width,height,TMAKE_COLORSPACE_yuyv_to_yv12_c::YUYV_TO_YV12());
}
//MAKE_COLORSPACE(uyvy_to_yv12_c,    2,2,2, YUYV_TO_YV12,   1,0,3,2,CCIR)
static void uyvy_to_yv12_c(uint8_t * x_ptr,stride_t x_stride,uint8_t * y_ptr,uint8_t * u_ptr,uint8_t * v_ptr,stride_t y_stride,stride_t uv_stride,int width,int height)
{
 typedef TMAKE_COLORSPACE<2,2,2, intToVal<1>, 0,3,2,CCIR> TMAKE_COLORSPACE_uyvy_to_yv12_c;
 TMAKE_COLORSPACE_uyvy_to_yv12_c::MAKE_COLORSPACE(x_ptr,x_stride,y_ptr,u_ptr,v_ptr,y_stride,uv_stride,width,height,TMAKE_COLORSPACE_uyvy_to_yv12_c::YUYV_TO_YV12());
}
//MAKE_COLORSPACE(yuyvi_to_yv12_c,   2,2,4, YUYVI_TO_YV12,  0,1,2,3,CCIR)
static void yuyvi_to_yv12_c(uint8_t * x_ptr,stride_t x_stride,uint8_t * y_ptr,uint8_t * u_ptr,uint8_t * v_ptr,stride_t y_stride,stride_t uv_stride,int width,int height)
{
 typedef TMAKE_COLORSPACE<2,2,4, intToVal<0>, 1,2,3,CCIR> TMAKE_COLORSPACE_yuyvi_to_yv12_c;
 TMAKE_COLORSPACE_yuyvi_to_yv12_c::MAKE_COLORSPACE(x_ptr,x_stride,y_ptr,u_ptr,v_ptr,y_stride,uv_stride,width,height,TMAKE_COLORSPACE_yuyvi_to_yv12_c::YUYVI_TO_YV12());
}
//MAKE_COLORSPACE(uyvyi_to_yv12_c,    2,2,2, YUYV_TO_YV12,   1,0,3,2,CCIR)
static void uyvyi_to_yv12_c(uint8_t * x_ptr,stride_t x_stride,uint8_t * y_ptr,uint8_t * u_ptr,uint8_t * v_ptr,stride_t y_stride,stride_t uv_stride,int width,int height)
{
 typedef TMAKE_COLORSPACE<2,2,4, intToVal<1>, 0,3,2,CCIR> TMAKE_COLORSPACE_uyvyi_to_yv12_c;
 TMAKE_COLORSPACE_uyvyi_to_yv12_c::MAKE_COLORSPACE(x_ptr,x_stride,y_ptr,u_ptr,v_ptr,y_stride,uv_stride,width,height,TMAKE_COLORSPACE_uyvyi_to_yv12_c::YUYVI_TO_YV12());
}

//============================================ MMX =============================================
template<class Tsimd> static void yv12_to_yv12_simd(uint8_t * y_dst, uint8_t * u_dst, uint8_t * v_dst,
 				      stride_t y_dst_stride, stride_t uv_dst_stride,
 				      const uint8_t * y_src, const uint8_t * u_src, const uint8_t * v_src,
 				      stride_t y_src_stride, stride_t uv_src_stride,
 				      int width, int height)
{
 struct TplaneCopy
  {
   static __forceinline void PLANE_COPY(uint8_t *DST,stride_t DST_DIF,const uint8_t *SRC,stride_t SRC_DIF,int WIDTH,int HEIGHT)
    {
        int eax= WIDTH  ;
        int ebp= HEIGHT ;// $ebp$ = height
        int ecx;//=0;
        const unsigned char *esi= SRC;
        unsigned char *edi= DST;

        int ebx= eax;
        eax>>=6;//                      ; $eax$ = width / 64
        ebx&=63;//; remainder = width % 64
        int edx= ebx;
        ebx>>=4 ;//               ; $ebx$ = remainder / 16
        edx&= 15;//                     ; $edx$ = remainder % 16
        __m64 mm0,mm1,mm2,mm3,mm4,mm5,mm6,mm7;
loop64_start:
        if (eax==0)
         goto loop16_start;
        ecx= eax;//            ; width64
loop64:
        Tsimd::prefetchnta (esi + 64);//  ; non temporal prefetch
        Tsimd::prefetchnta (esi + 96);

        movq (mm1, esi);//         ; read from src
        movq (mm2, esi + 8);
        movq (mm3, esi + 16);
        movq (mm4, esi + 24 );
        movq (mm5, esi + 32);
        movq (mm6, esi + 40);
        movq (mm7, esi + 48);
        movq (mm0, esi + 56);

        Tsimd::movntq (edi, mm1);//               ; write to y_out
        Tsimd::movntq (edi + 8, mm2 );
        Tsimd::movntq (edi + 16, mm3 );
        Tsimd::movntq (edi + 24, mm4 );
        Tsimd::movntq (edi + 32, mm5 );
        Tsimd::movntq (edi + 40, mm6 );
        Tsimd::movntq (edi + 48, mm7 );
        Tsimd::movntq (edi + 56, mm0 );

        esi+= 64;
        edi+= 64;
        ecx--;
        if (ecx)
         goto loop64;


loop16_start:
        if (ebx==0)
         goto loop1_start;
        ecx= ebx;//            ; width16
loop16:
        movq (mm1, esi);
        movq (mm2, esi + 8);
        Tsimd::movntq (edi, mm1);
        Tsimd::movntq (edi + 8, mm2 );

        esi+= 16;
        edi+= 16 ;
        ecx--;
        if (ecx)
         goto loop16;


loop1_start:
        ecx=edx;
        while (ecx)
         {
          *edi++=*esi++;
          ecx--;
         }
        //memcpy(edi,esi,edx);
        //mov ecx, edx
        //rep movsb

        esi+= SRC_DIF;
        edi+= DST_DIF;
        ebp--;
        if (ebp)
         goto loop64_start;
    }
  };
 int  width2	=width>>1;
 int  height2	=height>>1;
 stride_t  y_src_dif	=y_src_stride-width;;
 stride_t  y_dst_dif	=y_dst_stride-width;
 stride_t uv_src_dif	=uv_src_stride-width2;
 stride_t uv_dst_dif	=uv_dst_stride-width2;

 TplaneCopy::PLANE_COPY     ( y_dst, y_dst_dif,  y_src, y_src_dif,  width,  height);
 TplaneCopy::PLANE_COPY     ( u_dst, uv_dst_dif, u_src, uv_src_dif, width2, height2);
 TplaneCopy::PLANE_COPY     ( v_dst, uv_dst_dif, v_src, uv_src_dif, width2, height2);
}

template<class Tsimd,int BYTES,int PIXELS,int VPIXELS,int ARG1,int ARG2,int CCIR> struct TMAKE_COLORSPACE_SIMD
{
 struct YUYV_TO_YV12
  {
   static __forceinline void INIT(__m64 &mm7)
    {
     static const __int64 yuyv_mask=0x00ff00ff00ff00ffLL;
     movq (mm7, yuyv_mask);
    }
   static __forceinline void PROCESS(__m64 &mm7,unsigned char *edi,stride_t edx,unsigned char *ebx,unsigned char *ecx,unsigned char *esi,stride_t eax,stride_t uv_stride)
    {
     static const __int64 mmx_one=0x0001000100010001LL;//    dw 1, 1, 1, 1

     __m64 mm0,mm1,mm2,mm3,mm4,mm5,mm6;
     movq (mm0, edi);                               // x_ptr[0]
     movq (mm1, edi + 8);                   // x_ptr[8]
     movq (mm2, edi + edx);         // x_ptr[x_stride + 0]
     movq (mm3, edi + edx + 8);     // x_ptr[x_stride + 8]

            // average uv-components
    //---[ plain mmx ]----------------------------------------------------
    if (ARG2==0)           // if (ARG2 eq "0")
     {
                    movq (mm4, mm0);
                    movq (mm5, mm2);
      if (ARG1 == 0)                   // yuyv
       {
                    psrlw (mm4, 8);
                    psrlw (mm5, 8);
       }
                    pand (mm4, mm7);
                    pand (mm5, mm7);
                    paddw (mm4, mm5);

                    movq (mm5, mm1);
                    movq (mm6, mm3);
      if (ARG1 == 0)                   // yuyv
       {
                    psrlw (mm5, 8);
                    psrlw (mm6, 8);
       }
                    pand (mm5, mm7);
                    pand (mm6, mm7);
                    paddw( mm5, mm6);
                    paddw( mm4, mmx_one);          // +1 rounding
                    paddw( mm5, mmx_one);          //
                    psrlw( mm4, 1);
                    psrlw( mm5, 1);
     }
    else //---[ 3dnow/xmm ]----------------------------------------------------
     {
                    movq (mm4, mm0);
                    movq (mm5, mm1);
                    pavgb(mm4, mm2);                   //pavgb/pavgusb mm4, mm2
                    pavgb(mm5, mm3);                   //pavgb/pavgusb mm5, mm3

                    ////movq mm6, mm0             // 0 rounding
                    ////pxor mm6, mm2             //
                    ////psubb mm4, mm6            //
                    ////movq mm6, mm1             //
                    ////pxor mm6, mm3             //
                    ////psubb mm5, mm5            //

     if (ARG1 == 0)                   // yuyv
      {
                    psrlw (mm4, 8);
                    psrlw (mm5, 8);
      }
                    pand (mm4, mm7);
                    pand (mm5, mm7);
     }
    //--------------------------------------------------------------------

            // write y-component
    if (ARG1 == 1)                   // uyvy
     {
                    psrlw (mm0, 8);
                    psrlw (mm1, 8);
                    psrlw (mm2, 8);
                    psrlw (mm3, 8);
     }
                    pand (mm0, mm7);
                    pand (mm1, mm7);
                    pand (mm2, mm7);
                    pand (mm3, mm7);
                    packuswb (mm0, mm1);
                    packuswb (mm2, mm3);

                    Tsimd::movntq (esi, mm0);
                    Tsimd::movntq (esi+eax, mm2);

            // write uv-components
                    packuswb (mm4, mm5);
                    movq (mm5, mm4);
                    psrlq (mm4, 8);
                    pand (mm5, mm7);
                    pand (mm4, mm7);
                    packuswb (mm5,mm5);
                    packuswb (mm4,mm4);
                    movd (ebx,mm5);
                    movd (ecx,mm4);

    }
  };

 struct YV12_TO_YUYV
  {
   static __forceinline void INIT(__m64 &mm7)
    {
    }
   static __forceinline void PROCESS(__m64 &mm7,unsigned char *edi,stride_t edx,unsigned char *ebx,unsigned char *ecx,unsigned char *esi,stride_t eax,stride_t uv_stride)
    {
     __m64 mm4,mm5,mm0,mm1,mm2,mm3,mm6;
                  movd (mm4, ebx);                         // [    |uuuu]
                  movd (mm5, ecx);                         // [    |vvvv]
                  movq (mm0, esi);                         // [yyyy|yyyy] // y row 0
                  movq (mm1, esi+eax);                     // [yyyy|yyyy] // y row 1
                  punpcklbw (mm4, mm5);                      // [vuvu|vuvu] // uv row 0

    if (ARG1 == 0)             // YUYV
     {
                  movq (mm2, mm0);
                  movq (mm3, mm1);
                  punpcklbw (mm0, mm4);                      // vyuy|vyuy // y row 0 + 0
                  punpckhbw (mm2, mm4);                      // vyuy|vyuy // y row 0 + 8
                  punpcklbw (mm1, mm4);                      // vyuy|vyuy // y row 1 + 0
                  punpckhbw (mm3, mm4);                      // vyuy|vyuy // y row 1 + 8
                  movq (edi, mm0     );
                  movq (edi+8, mm2   );
                  movq (edi+edx, mm1 );
                  movq (edi+edx+8, mm3);
     } else {                  // UYVY
                  movq (mm5, mm4     );
                  movq (mm6, mm4     );
                  movq (mm7, mm4     );
                  punpcklbw (mm4, mm0);                      // yvyu|yvyu   // y row 0 + 0
                  punpckhbw (mm5, mm0);                      // yvyu|yvyu   // y row 0 + 8
                  punpcklbw (mm6, mm1);                      // yvyu|yvyu   // y row 1 + 0
                  punpckhbw (mm7, mm1);                      // yvyu|yvyu   // y row 1 + 8
                  movq (edi, mm4     );
                  movq (edi+8, mm5   );
                  movq (edi+edx, mm6  );
                  movq (edi+edx+8, mm7 );
     }
    }
  };

 struct YV12_TO_YUYVI
  {
   static __forceinline void INIT(__m64 &mm7)
    {
    }
   static __forceinline void PROCESS(__m64 &mm7,unsigned char *edi,stride_t edx,unsigned char *ebx,unsigned char *ecx,unsigned char *esi,stride_t eax,stride_t uv_stride)
    {
                  __m64 mm0,mm1,mm2,mm3,mm4,mm5,mm6;
                  movd (mm0, ebx);                         // [    |uuuu]
                  movd (mm1, ebx+uv_stride);                     // [    |uuuu]
                  punpcklbw (mm0, ecx);            // [vuvu|vuvu] // uv row 0
                  punpcklbw (mm1, ecx+uv_stride);        // [vuvu|vuvu] // uv row 1

     if (ARG1 == 0) {             // YUYV
                  movq (mm4, esi);                         // [yyyy|yyyy] // y row 0
                  movq (mm6, esi+eax);                     // [yyyy|yyyy] // y row 1
                  movq (mm5, mm4);
                  movq (mm7, mm6);
                  punpcklbw (mm4, mm0);                      // [yuyv|yuyv] // y row 0 + 0
                  punpckhbw (mm5, mm0);                      // [yuyv|yuyv] // y row 0 + 8
                  punpcklbw (mm6, mm1);                      // [yuyv|yuyv] // y row 1 + 0
                  punpckhbw (mm7, mm1);                      // [yuyv|yuyv] // y row 1 + 8
                  movq (edi, mm4);
                  movq (edi+8, mm5);
                  movq (edi+edx, mm6);
                  movq (edi+edx+8, mm7);

                  //push esi
                  //push edi
                  esi+= eax;
                  edi+= edx;
                  movq (mm4, esi+eax);                     // [yyyy|yyyy] // y row 2
                  movq (mm6, esi+2*eax);           // [yyyy|yyyy] // y row 3
                  movq (mm5, mm4);
                  movq (mm7, mm6);
                  punpcklbw (mm4, mm0);                      // [yuyv|yuyv] // y row 2 + 0
                  punpckhbw (mm5, mm0 );                     // [yuyv|yuyv] // y row 2 + 8
                  punpcklbw (mm6, mm1 );                     // [yuyv|yuyv] // y row 3 + 0
                  punpckhbw (mm7, mm1 );                     // [yuyv|yuyv] // y row 3 + 8
                  movq (edi+edx, mm4);
                  movq (edi+edx+8, mm5);
                  movq (edi+2*edx, mm6);
                  movq (edi+2*edx+8, mm7);
                  //pop edi
                  //pop esi
     }else{                   // UYVY
                  movq (mm2, esi);                         // [yyyy|yyyy] // y row 0
                  movq (mm3, esi+eax);                     // [yyyy|yyyy] // y row 1
                  movq (mm4, mm0);
                  movq (mm5, mm0);
                  movq (mm6, mm1);
                  movq (mm7, mm1);
                  punpcklbw( mm4, mm2);                      // [uyvy|uyvy] // y row 0 + 0
                  punpckhbw( mm5, mm2);                      // [uyvy|uyvy] // y row 0 + 8
                  punpcklbw( mm6, mm3);                      // [uyvy|uyvy] // y row 1 + 0
                  punpckhbw( mm7, mm3);                      // [uyvy|uyvy] // y row 1 + 8
                  movq (edi, mm4);
                  movq (edi+8, mm5);
                  movq (edi+edx, mm6);
                  movq (edi+edx+8, mm7);

                  //push esi
                  //push edi
                  esi+= eax;
                  edi+= edx;
                  movq (mm2, esi+eax);                     // [yyyy|yyyy] // y row 2
                  movq (mm3, esi+2*eax);           // [yyyy|yyyy] // y row 3
                  movq (mm4, mm0);
                  movq (mm5, mm0);
                  movq (mm6, mm1);
                  movq (mm7, mm1);
                  punpcklbw (mm4, mm2);                      // [uyvy|uyvy] // y row 2 + 0
                  punpckhbw (mm5, mm2);                      // [uyvy|uyvy] // y row 2 + 8
                  punpcklbw (mm6, mm3);                      // [uyvy|uyvy] // y row 3 + 0
                  punpckhbw (mm7, mm3);                      // [uyvy|uyvy] // y row 3 + 8
                  movq (edi+edx, mm4);
                  movq (edi+edx+8, mm5);
                  movq (edi+2*edx, mm6);
                  movq (edi+2*edx+8, mm7);
                  //pop edi
                  //pop esi
      }
    }
  };

 struct BGR_TO_YV12
  {
   static __forceinline void INIT(__m64 &mm7)
    {
     movq (mm7, YUV_RGB_DATA<CCIR>::y_mul);
    }
   static __forceinline void PROCESS(__m64 &mm7,unsigned char *edi,stride_t edx1,unsigned char *ebx,unsigned char *ecx,unsigned char *esi,stride_t eax,stride_t uv_stride)
    {
                  // y_out
                  __m64 mm4,mm5,mm0,mm2,mm6,mm1,mm3;
                  pxor (mm4, mm4);
                  pxor (mm5, mm5);
                  movd (mm0, edi);                 // x_ptr[0...]
                  movd (mm2, edi+edx1);             // x_ptr[x_stride...]
                  punpcklbw (mm0, mm4);              // [  |b |g |r ]
                  punpcklbw (mm2, mm5);              // [  |b |g |r ]
                  movq (mm6, mm0     );              // = [  |b4|g4|r4]
                  paddw (mm6, mm2    );              // +[  |b4|g4|r4]
                  pmaddwd (mm0, mm7  );              // *= Y_MUL
                  pmaddwd (mm2, mm7  );              // *= Y_MUL
                  movq (mm4, mm0     );              // [r]
                  movq (mm5, mm2     );              // [r]
                  psrlq (mm4, 32     );              // +[g]
                  psrlq (mm5, 32     );              // +[g]
                  paddd (mm0, mm4    );              // +[b]
                  paddd (mm2, mm5    );              // +[b]

                  pxor (mm4, mm4);
                  pxor (mm5, mm5);
                  movd (mm1, edi+ARG1);              // src[%1...]
                  movd (mm3, edi+edx1+ARG1);  // src[x_stride+%1...]
                  punpcklbw (mm1, mm4);              // [  |b |g |r ]
                  punpcklbw (mm3, mm5);              // [  |b |g |r ]
                  paddw (mm6, mm1    );              // +[  |b4|g4|r4]
                  paddw (mm6, mm3    );              // +[  |b4|g4|r4]
                  pmaddwd (mm1, mm7  );              // *= Y_MUL
                  pmaddwd (mm3, mm7  );              // *= Y_MUL
                  movq (mm4, mm1     );              // [r]
                  movq (mm5, mm3     );              // [r]
                  psrlq (mm4, 32     );              // +[g]
                  psrlq (mm5, 32     );              // +[g]
                  paddd (mm1, mm4    );              // +[b]
                  paddd (mm3, mm5    );              // +[b]

                  //push    edx
                  unsigned int edx;
                  movd ((int*)&edx, mm0);
                  edx>>= 8;
                  edx+=YUV_RGB_DATA<CCIR>::Y_ADD;
                  *esi=(unsigned char)edx;                   // y_ptr[0]

                  movd ((int*)&edx, mm1);
                  edx>>= 8;
                  edx+=YUV_RGB_DATA<CCIR>::Y_ADD;
                  *(esi + 1)=(unsigned char)edx;               // y_ptr[1]

                  movd ((int*)&edx, mm2);
                  edx>>= 8;
                  edx+=YUV_RGB_DATA<CCIR>::Y_ADD;
                  *(esi + eax + 0)=(unsigned char)edx;                 // y_ptr[y_stride + 0]

                  movd ((int*)&edx, mm3);
                  edx>>= 8;
                  edx+=YUV_RGB_DATA<CCIR>::Y_ADD;
                  *(esi + eax + 1)=(unsigned char)edx;                 // y_ptr[y_stride + 1]

                  // u_ptr, v_ptr
                  movq (mm0, mm6);                   // = [  |b4|g4|r4]
                  pmaddwd (mm6, YUV_RGB_DATA<CCIR>::v_mul);             // *= V_MUL
                  pmaddwd (mm0, YUV_RGB_DATA<CCIR>::u_mul);             // *= U_MUL
                  movq (mm1, mm0);
                  movq (mm2, mm6);
                  psrlq (mm1, 32);
                  psrlq (mm2, 32);
                  paddd (mm0, mm1);
                  paddd (mm2, mm6);

                  movd ((int*)&edx, mm0);
                  edx>>= 10;
                  edx+= YUV_RGB_DATA<CCIR>::U_ADD;
                  *ebx=(unsigned char)edx;

                  movd ((int*)&edx, mm2);
                  edx>>= 10;
                  edx+= YUV_RGB_DATA<CCIR>::V_ADD;
                  *ecx=(unsigned char)edx;
    }
  };

 struct YV12_TO_BGR
  {
   static __forceinline void INIT(__m64 &mm7)
    {
     pxor (mm7, mm7);
    }
   static __forceinline void PROCESS(__m64 &mm7,unsigned char *edi,stride_t edx,unsigned char *ebx,unsigned char *ecx,unsigned char *esi,stride_t eax,stride_t uv_stride)
    {
     __m64 TEMP_Y1;
     __m64 TEMP_Y2;
     __m64 TEMP_G1;
     __m64 TEMP_G2;
     __m64 TEMP_B1;
     __m64 TEMP_B2;
     static const int SCALEBITS=6;
          __m64 mm2,mm3,mm4,mm5,mm6,mm0,mm1;
          movd (mm2, ebx);         // u_ptr[0]
          movd (mm3, ecx);         // v_ptr[0]
          punpcklbw (mm2, mm7);              // u3u2u1u0 -> mm2
          punpcklbw (mm3, mm7);              // v3v2v1v0 -> mm3
          psubsw (mm2, YUV_RGB_DATA<CCIR>::U_SUB);              // U - 128
          psubsw (mm3, YUV_RGB_DATA<CCIR>::V_SUB);              // V - 128
          movq (mm4, mm2);
          movq (mm5, mm3);
          pmullw (mm2, YUV_RGB_DATA<CCIR>::UG_MUL);
          pmullw (mm3, YUV_RGB_DATA<CCIR>::VG_MUL);
          movq (mm6, mm2);                   // u3u2u1u0 -> mm6
          punpckhwd (mm2, mm2);              // u3u3u2u2 -> mm2
          punpcklwd (mm6, mm6);              // u1u1u0u0 -> mm6
          pmullw (mm4, YUV_RGB_DATA<CCIR>::UB_MUL);             // B_ADD -> mm4
          movq (mm0, mm3);
          punpckhwd (mm3, mm3);              // v3v3v2v2 -> mm2
          punpcklwd (mm0, mm0);              // v1v1v0v0 -> mm6
          paddsw (mm2, mm3);
          paddsw (mm6, mm0);
          pmullw (mm5, YUV_RGB_DATA<CCIR>::VR_MUL);             // R_ADD -> mm5
          movq (mm0, esi);                 // y7y6y5y4y3y2y1y0 -> mm0
          movq (mm1, mm0);
          punpckhbw (mm1, mm7);              // y7y6y5y4 -> mm1
          punpcklbw (mm0, mm7);              // y3y2y1y0 -> mm0
          psubsw (mm0, YUV_RGB_DATA<CCIR>::Y_SUB);              // Y - Y_SUB
          psubsw (mm1, YUV_RGB_DATA<CCIR>::Y_SUB);              // Y - Y_SUB
          pmullw (mm1, YUV_RGB_DATA<CCIR>::Y_MUL);
          pmullw (mm0, YUV_RGB_DATA<CCIR>::Y_MUL);
          movq (TEMP_Y2, mm1);             // y7y6y5y4 -> mm3
          movq (TEMP_Y1, mm0);             // y3y2y1y0 -> mm7
          psubsw (mm1, mm2);                 // g7g6g5g4 -> mm1
          psubsw (mm0, mm6);                 // g3g2g1g0 -> mm0
          psraw (mm1, SCALEBITS);
          psraw (mm0, SCALEBITS);
          packuswb (mm0, mm1);               //g7g6g5g4g3g2g1g0 -> mm0
          movq (TEMP_G1, mm0);
          movq (mm0, esi+eax);                     // y7y6y5y4y3y2y1y0 -> mm0
          movq (mm1, mm0);
          punpckhbw( mm1, mm7);              // y7y6y5y4 -> mm1
          punpcklbw( mm0, mm7);              // y3y2y1y0 -> mm0
          psubsw (mm0, YUV_RGB_DATA<CCIR>::Y_SUB);              // Y - Y_SUB
          psubsw (mm1, YUV_RGB_DATA<CCIR>::Y_SUB);              // Y - Y_SUB
          pmullw (mm1, YUV_RGB_DATA<CCIR>::Y_MUL);
          pmullw (mm0, YUV_RGB_DATA<CCIR>::Y_MUL);
          movq (mm3, mm1);
          psubsw (mm1, mm2);                 // g7g6g5g4 -> mm1
          movq (mm2, mm0);
          psubsw (mm0, mm6);                 // g3g2g1g0 -> mm0
          psraw (mm1, SCALEBITS);
          psraw (mm0, SCALEBITS);
          packuswb (mm0, mm1);               // g7g6g5g4g3g2g1g0 -> mm0
          movq (TEMP_G2, mm0);
          movq (mm0, mm4);
          punpckhwd (mm4, mm4);              // u3u3u2u2 -> mm2
          punpcklwd (mm0, mm0);              // u1u1u0u0 -> mm6
          movq (mm1, mm3);                   // y7y6y5y4 -> mm1
          paddsw (mm3, mm4);                 // b7b6b5b4 -> mm3
          movq (mm7, mm2);                   // y3y2y1y0 -> mm7
          paddsw (mm2, mm0 );                // b3b2b1b0 -> mm2
          psraw (mm3, SCALEBITS);
          psraw (mm2, SCALEBITS );
          packuswb (mm2, mm3 );              // b7b6b5b4b3b2b1b0 -> mm2
          movq (TEMP_B2, mm2);
          movq (mm3, TEMP_Y2);
          movq (mm2, TEMP_Y1);
          movq (mm6, mm3);                   // TEMP_Y2 -> mm6
          paddsw (mm3, mm4);                 // b7b6b5b4 -> mm3
          movq (mm4, mm2);                   // TEMP_Y1 -> mm4
          paddsw (mm2, mm0);                 // b3b2b1b0 -> mm2
          psraw (mm3, SCALEBITS);
          psraw (mm2, SCALEBITS);
          packuswb (mm2, mm3);               // b7b6b5b4b3b2b1b0 -> mm2
          movq (TEMP_B1, mm2);
          movq (mm0, mm5);
          punpckhwd (mm5, mm5);              // v3v3v2v2 -> mm5
          punpcklwd (mm0, mm0);              // v1v1v0v0 -> mm0
          paddsw (mm1, mm5);                 // r7r6r5r4 -> mm1
          paddsw (mm7, mm0 );                // r3r2r1r0 -> mm7
          psraw (mm1, SCALEBITS);
          psraw (mm7, SCALEBITS);
          packuswb (mm7, mm1);               // r7r6r5r4r3r2r1r0 -> mm7 (TEMP_R2)
          paddsw (mm6, mm5);                 // r7r6r5r4 -> mm6
          paddsw (mm4, mm0);                 // r3r2r1r0 -> mm4
          psraw (mm6, SCALEBITS);
          psraw (mm4, SCALEBITS);
          packuswb (mm4, mm6);               // r7r6r5r4r3r2r1r0 -> mm4 (TEMP_R1)
          movq (mm0, TEMP_B1);
          movq (mm1, TEMP_G1);
          movq (mm6, mm7);
          movq (mm2, mm0);
          punpcklbw (mm2, mm4);              // r3b3r2b2r1b1r0b0 -> mm2
          punpckhbw (mm0, mm4);              // r7b7r6b6r5b5r4b4 -> mm0
          pxor (mm7, mm7);
          movq (mm3, mm1);
          punpcklbw (mm1, mm7);              // 0g30g20g10g0 -> mm1
          punpckhbw (mm3, mm7);              // 0g70g60g50g4 -> mm3
          movq (mm4, mm2);
          punpcklbw (mm2, mm1);              // 0r1g1b10r0g0b0 -> mm2
          punpckhbw (mm4, mm1);              // 0r3g3b30r2g2b2 -> mm4
          movq (mm5, mm0);
          punpcklbw( mm0, mm3);              // 0r5g5b50r4g4b4 -> mm0
          punpckhbw (mm5, mm3);              // 0r7g7b70r6g6b6 -> mm5
       if (ARG1 == 3)             // BGR (24-bit)
        {
          movd (edi, mm2);
          psrlq (mm2, 32);
          movd (edi + 3, mm2);
          movd (edi + 6, mm4);
          psrlq (mm4, 32);
          movd (edi + 9, mm4);
          movd (edi + 12, mm0);
          psrlq (mm0, 32);
          movd (edi + 15, mm0);
          movq (mm2, mm5);
          psrlq (mm0, 8);              // 000000r5g5 -> mm0
          psllq (mm2, 32);             // 0r6g6b60000 -> mm2
          psrlq (mm5, 32);             // 00000r7g7b7 -> mm5
          psrlq (mm2, 16);             // 000r6g6b600 -> mm2
          por (mm0, mm2);              // 000r6g6b6r5g5 -> mm0
          psllq (mm5, 40);             // r7g7b700000 -> mm5
          por (mm5, mm0);              // r7g7b7r6g6b6r5g5 -> mm5
          movq (edi + 16, mm5);
          movq (mm0, TEMP_B2);
          movq (mm1, TEMP_G2);
          movq (mm2, mm0);
          punpcklbw (mm2, mm6);              // r3b3r2b2r1b1r0b0 -> mm2
          punpckhbw (mm0, mm6);              // r7b7r6b6r5b5r4b4 -> mm0
          movq (mm3, mm1     );
          punpcklbw (mm1, mm7);              // 0g30g20g10g0 -> mm1
          punpckhbw (mm3, mm7);              // 0g70g60g50g4 -> mm3
          movq (mm4, mm2     );
          punpcklbw (mm2, mm1);              // 0r1g1b10r0g0b0 -> mm2
          punpckhbw (mm4, mm1);              // 0r3g3b30r2g2b2 -> mm4
          movq (mm5, mm0     );
          punpcklbw (mm0, mm3);              // 0r5g5b50r4g4b4 -> mm0
          punpckhbw (mm5, mm3);              // 0r7g7b70r6g6b6 -> mm5
          movd (edi+edx, mm2 );
          psrlq (mm2, 32     );
          movd (edi+edx + 3, mm2);
          movd (edi+edx + 6, mm4);
          psrlq (mm4, 32        );
          movd (edi+edx + 9, mm4);
          movd (edi+edx + 12, mm0);
          psrlq (mm0, 32         );
          movd (edi+edx + 15, mm0);
          movq (mm2, mm5         );
          psrlq (mm0, 8 );             // 000000r5g5 -> mm0
          psllq (mm2, 32);             // 0r6g6b60000 -> mm2
          psrlq (mm5, 32);             // 00000r7g7b7 -> mm5
          psrlq (mm2, 16);             // 000r6g6b600 -> mm2
          por (mm0, mm2 );             // 000r6g6b6r5g5 -> mm0
          psllq (mm5, 40);             // r7g7b700000 -> mm5
          por (mm5, mm0 );             // r7g7b7r6g6b6r5g5 -> mm5
          movq (edi + edx + 16, mm5);

        }else{           // BGRA (32-bit)
          movq (edi, mm2);
          movq (edi + 8, mm4);
          movq (edi + 16, mm0);
          movq (edi + 24, mm5);
          movq (mm0, TEMP_B2);
          movq (mm1, TEMP_G2);
          movq (mm2, mm0);
          punpcklbw (mm2, mm6);              // r3b3r2b2r1b1r0b0 -> mm2
          punpckhbw (mm0, mm6);              // r7b7r6b6r5b5r4b4 -> mm0
          movq (mm3, mm1     );
          punpcklbw (mm1, mm7);              // 0g30g20g10g0 -> mm1
          punpckhbw (mm3, mm7);              // 0g70g60g50g4 -> mm3
          movq (mm4, mm2     );
          punpcklbw (mm2, mm1);              // 0r1g1b10r0g0b0 -> mm2
          punpckhbw (mm4, mm1);              // 0r3g3b30r2g2b2 -> mm4
          movq (mm5, mm0     );
          punpcklbw (mm0, mm3);              // 0r5g5b50r4g4b4 -> mm0
          punpckhbw (mm5, mm3);              // 0r7g7b70r6g6b6 -> mm5
          movq (edi + edx, mm2);
          movq (edi + edx + 8, mm4);
          movq (edi + edx + 16, mm0);
          movq (edi + edx + 24, mm5);
        }
    }
  };

 template<class TFUNC> static __forceinline void MAKE_COLORSPACE(uint8_t * x_ptr,
                               stride_t x_stride,
                               uint8_t * y_ptr,
                               uint8_t * v_ptr,
                               uint8_t * u_ptr,
                               stride_t y_stride,
                               stride_t uv_stride,
                               int width,
                               int height,
                               const TFUNC &FUNC)
  {
  //------------------------------------------------------------------------------
  //
  // MAKE_COLORSPACE(NAME,STACK, BYTES,PIXELS,ROWS, FUNC, ARG1, ARG2, CCIR)
  //
  // This macro provides a assembler width/height scroll loop
  // NAME         function name
  // STACK                additional stack bytes required by FUNC
  // BYTES                bytes-per-pixel for the given colorspace
  // PIXELS       pixels (columns) operated on per FUNC call
  // VPIXELS      vpixels (rows) operated on per FUNC call
  // FUNC         conversion macro name// we expect to find FUNC_INIT and FUNC macros
  // ARG1         argument passed to FUNC
  //
  // throughout the FUNC the registers mean:
  // eax          y_stride
  // ebx          u_ptr
  // ecx          v_ptr
  // edx          x_stride
  // esi          y_ptr
  // edi          x_ptr
  // ebp          width
  //
  //------------------------------------------------------------------------------
   stride_t x_dif;
   stride_t y_dif;
   stride_t uv_dif;
   stride_t fixed_width;


   stride_t eax= width;
   eax+=15;
   eax&=~15;
   fixed_width=eax;

   stride_t ebx1=x_stride;
   for (int i=0;i<BYTES;i++)
    ebx1-= eax;

   x_dif= ebx1;                        // x_dif = x_stride - BYTES*fixed_width

   ebx1= y_stride;
   ebx1-= eax;
   y_dif= ebx1;                        // y_dif = y_stride - fixed_width

   ebx1=uv_stride;
   stride_t ecx1= eax;
   ecx1>>=1;
   ebx1-= ecx1;
   uv_dif= ebx1;                       // uv_dif = uv_stride - fixed_width/2

   unsigned char *esi= y_ptr;                // $esi$ = y_ptr
   unsigned char *edi= x_ptr;                // $edi$ = x_ptr
   stride_t edx= x_stride;             // $edx$ = x_stride
   stride_t ebp= height;               // $ebp$ = height

   //       ; --- begin loop ---

   eax= y_stride;             //$eax$ = y_stride
   unsigned char *ebx= u_ptr;                //$ebx$ = u_ptr
   unsigned char *ecx= v_ptr;                //$ecx$ = v_ptr
                  __m64 mm7;
                  FUNC.INIT (mm7);          //call FUNC_INIT
   for (;ebp>0;ebp-= VPIXELS)
    {
                  stride_t tmp_height= ebp;
                  ebp= fixed_width;
     for (;ebp>0;ebp-= PIXELS)
      {
                  FUNC.PROCESS (mm7,edi,edx,ebx,ecx,esi,eax,uv_stride);

                  edi+= BYTES*PIXELS;               //x_ptr += BYTES*PIXELS
                  esi+= PIXELS;                             // y_ptr += PIXELS
                  ebx+= PIXELS/2;                   //u_ptr += PIXELS/2
                  ecx+= PIXELS/2;                   //v_ptr += PIXELS/2

      }

                  ebp= tmp_height;
                  edi+= x_dif;                //x_ptr += x_dif + (VPIXELS-1)*x_stride
                  esi+= y_dif;                //y_ptr += y_dif + (VPIXELS-1)*y_stride
                  for (int i=0;i<VPIXELS-1;i++)
                   {
                    edi+= edx;
                    esi+= eax;
                   }

                  ebx+= uv_dif;               //u_ptr += uv_dif + ((VPIXELS/2)-1)*uv_stride
                  ecx+= uv_dif;               //v_ptr += uv_dif + ((VPIXELS/2)-1)*uv_stride
                  for (int i=0;i<(VPIXELS/2)-1;i++)
                   {
                    ebx+= uv_stride;
                    ecx+= uv_stride;
                   }
    }

  }
};

static void yuyv_to_yv12_mmx(uint8_t * x_ptr,stride_t x_stride,uint8_t * y_src,uint8_t * u_src,uint8_t * v_src,stride_t y_stride,stride_t uv_stride,int width,int height)
{
 typedef TMAKE_COLORSPACE_SIMD<Tmmx, 2,8,2, 0,0, CCIR> TMAKE_COLORSPACE_yuyv_to_yv12_mmx;
 TMAKE_COLORSPACE_yuyv_to_yv12_mmx::MAKE_COLORSPACE(x_ptr,x_stride,y_src,v_src,u_src,y_stride,uv_stride,width,height,TMAKE_COLORSPACE_yuyv_to_yv12_mmx::YUYV_TO_YV12());
}
static void yuyv_to_yv12_xmm(uint8_t * x_ptr,stride_t x_stride,uint8_t * y_src,uint8_t * u_src,uint8_t * v_src,stride_t y_stride,stride_t uv_stride,int width,int height)
{
 typedef TMAKE_COLORSPACE_SIMD<Tmmxext, 2,8,2, 0,1, CCIR> TMAKE_COLORSPACE_yuyv_to_yv12_mmxext;
 TMAKE_COLORSPACE_yuyv_to_yv12_mmxext::MAKE_COLORSPACE(x_ptr,x_stride,y_src,v_src,u_src,y_stride,uv_stride,width,height,TMAKE_COLORSPACE_yuyv_to_yv12_mmxext::YUYV_TO_YV12());
}
static void uyvy_to_yv12_mmx(uint8_t * x_ptr,stride_t x_stride,uint8_t * y_src,uint8_t * u_src,uint8_t * v_src,stride_t y_stride,stride_t uv_stride,int width,int height)
{
 typedef TMAKE_COLORSPACE_SIMD<Tmmx, 2,8,2, 1,0, CCIR> TMAKE_COLORSPACE_uyvy_to_yv12_mmx;
 TMAKE_COLORSPACE_uyvy_to_yv12_mmx::MAKE_COLORSPACE(x_ptr,x_stride,y_src,v_src,u_src,y_stride,uv_stride,width,height,TMAKE_COLORSPACE_uyvy_to_yv12_mmx::YUYV_TO_YV12());
}
static void uyvy_to_yv12_xmm(uint8_t * x_ptr,stride_t x_stride,uint8_t * y_src,uint8_t * u_src,uint8_t * v_src,stride_t y_stride,stride_t uv_stride,int width,int height)
{
 typedef TMAKE_COLORSPACE_SIMD<Tmmxext, 2,8,2, 1,1, CCIR> TMAKE_COLORSPACE_uyvy_to_yv12_mmxext;
 TMAKE_COLORSPACE_uyvy_to_yv12_mmxext::MAKE_COLORSPACE(x_ptr,x_stride,y_src,v_src,u_src,y_stride,uv_stride,width,height,TMAKE_COLORSPACE_uyvy_to_yv12_mmxext::YUYV_TO_YV12());
}

static void yv12_to_yuyv_mmx(uint8_t * x_ptr,stride_t x_stride,uint8_t * y_src,uint8_t * u_src,uint8_t * v_src,stride_t y_stride,stride_t uv_stride,int width,int height)
{
 typedef TMAKE_COLORSPACE_SIMD<Tmmx, 2,8,2, 0,-1, CCIR> TMAKE_COLORSPACE_yv12_to_yuyv_mmx;
 TMAKE_COLORSPACE_yv12_to_yuyv_mmx::MAKE_COLORSPACE(x_ptr,x_stride,y_src,v_src,u_src,y_stride,uv_stride,width,height,TMAKE_COLORSPACE_yv12_to_yuyv_mmx::YV12_TO_YUYV());
}
static void yv12_to_uyvy_mmx(uint8_t * x_ptr,stride_t x_stride,uint8_t * y_src,uint8_t * u_src,uint8_t * v_src,stride_t y_stride,stride_t uv_stride,int width,int height)
{
 typedef TMAKE_COLORSPACE_SIMD<Tmmx, 2,8,2, 1,-1, CCIR> TMAKE_COLORSPACE_yv12_to_uyvy_mmx;
 TMAKE_COLORSPACE_yv12_to_uyvy_mmx::MAKE_COLORSPACE(x_ptr,x_stride,y_src,v_src,u_src,y_stride,uv_stride,width,height,TMAKE_COLORSPACE_yv12_to_uyvy_mmx::YV12_TO_YUYV());
}

static void yv12_to_yuyvi_mmx(uint8_t * x_ptr,stride_t x_stride,uint8_t * y_src,uint8_t * u_src,uint8_t * v_src,stride_t y_stride,stride_t uv_stride,int width,int height)
{
 typedef TMAKE_COLORSPACE_SIMD<Tmmx, 2,8,4, 0,-1, CCIR> TMAKE_COLORSPACE_yv12_to_yuyvi_mmx;
 TMAKE_COLORSPACE_yv12_to_yuyvi_mmx::MAKE_COLORSPACE(x_ptr,x_stride,y_src,v_src,u_src,y_stride,uv_stride,width,height,TMAKE_COLORSPACE_yv12_to_yuyvi_mmx::YV12_TO_YUYVI());
}
static void yv12_to_uyvyi_mmx(uint8_t * x_ptr,stride_t x_stride,uint8_t * y_src,uint8_t * u_src,uint8_t * v_src,stride_t y_stride,stride_t uv_stride,int width,int height)
{
 typedef TMAKE_COLORSPACE_SIMD<Tmmx, 2,8,4, 1,-1, CCIR> TMAKE_COLORSPACE_yv12_to_uyvyi_mmx;
 TMAKE_COLORSPACE_yv12_to_uyvyi_mmx::MAKE_COLORSPACE(x_ptr,x_stride,y_src,v_src,u_src,y_stride,uv_stride,width,height,TMAKE_COLORSPACE_yv12_to_uyvyi_mmx::YV12_TO_YUYVI());
}

template<int CCIR> struct TpackedFuncPtrRGB
{
 static packedFuncPtr rgb555_to_yv12;
 static packedFuncPtr rgb565_to_yv12;
 static packedFuncPtr bgr_to_yv12;
 static packedFuncPtr bgra_to_yv12;
 static packedFuncPtr abgr_to_yv12;
 static packedFuncPtr rgb_to_yv12;
 static packedFuncPtr rgba_to_yv12;
 static packedFuncPtr argb_to_yv12;

 static packedFuncPtr rgb555i_to_yv12;
 static packedFuncPtr rgb565i_to_yv12;
 static packedFuncPtr bgri_to_yv12;
 static packedFuncPtr bgrai_to_yv12;
 static packedFuncPtr abgri_to_yv12;
 static packedFuncPtr rgbi_to_yv12;
 static packedFuncPtr rgbai_to_yv12;
 static packedFuncPtr argbi_to_yv12;

 static packedFuncPtr yv12_to_rgb555;
 static packedFuncPtr yv12_to_rgb565;
 static packedFuncPtr yv12_to_bgr;
 static packedFuncPtr yv12_to_bgra;
 static packedFuncPtr yv12_to_abgr;
 static packedFuncPtr yv12_to_rgb;
 static packedFuncPtr yv12_to_rgba;
 static packedFuncPtr yv12_to_argb;

 static packedFuncPtr yv12_to_rgb555i;
 static packedFuncPtr yv12_to_rgb565i;
 static packedFuncPtr yv12_to_bgri;
 static packedFuncPtr yv12_to_bgrai;
 static packedFuncPtr yv12_to_abgri;
 static packedFuncPtr yv12_to_rgbi;
 static packedFuncPtr yv12_to_rgbai;
 static packedFuncPtr yv12_to_argbi;

 static void init(void)
  {
   rgb555_to_yv12 = rgb555_to_yv12_c;
   rgb565_to_yv12 = rgb565_to_yv12_c;
   bgr_to_yv12     = bgr_to_yv12_c;
   bgra_to_yv12    = bgra_to_yv12_c;
   abgr_to_yv12    = abgr_to_yv12_c;
   rgb_to_yv12     = rgb_to_yv12_c;
   rgba_to_yv12    = rgba_to_yv12_c;

   rgb555i_to_yv12 = rgb555i_to_yv12_c;
   rgb565i_to_yv12 = rgb565i_to_yv12_c;
   bgri_to_yv12    = bgri_to_yv12_c;
   bgrai_to_yv12   = bgrai_to_yv12_c;
   abgri_to_yv12   = abgri_to_yv12_c;
   rgbi_to_yv12    = rgbi_to_yv12_c;
   rgbai_to_yv12   = rgbai_to_yv12_c;

   yv12_to_rgb555 = yv12_to_rgb555_c;
   yv12_to_rgb565 = yv12_to_rgb565_c;
   yv12_to_bgr     = yv12_to_bgr_c;
   yv12_to_bgra    = yv12_to_bgra_c;
   yv12_to_abgr    = yv12_to_abgr_c;
   yv12_to_rgb     = yv12_to_rgb_c;
   yv12_to_rgba    = yv12_to_rgba_c;

   yv12_to_rgb555i = yv12_to_rgb555i_c;
   yv12_to_rgb565i = yv12_to_rgb565i_c;
   yv12_to_bgri    = yv12_to_bgri_c;
   yv12_to_bgrai   = yv12_to_bgrai_c;
   yv12_to_abgri   = yv12_to_abgri_c;
   yv12_to_rgbi    = yv12_to_rgbi_c;
   yv12_to_rgbai   = yv12_to_rgbai_c;
  }
 static void initMMX(void)
  {
   bgr_to_yv12   = bgr_to_yv12_mmx;
   bgra_to_yv12  = bgra_to_yv12_mmx;
   yv12_to_bgr   = yv12_to_bgr_mmx;
   yv12_to_bgra  = yv12_to_bgra_mmx;
  }

 //MAKE_COLORSPACE(yv12_to_rgb555_c,  2,2,2, YV12_TO_RGB16,  MK_RGB555, 0,0,0,CCIR)
 static void yv12_to_rgb555_c(uint8_t * x_ptr,stride_t x_stride,uint8_t * y_ptr,uint8_t * u_ptr,uint8_t * v_ptr,stride_t y_stride,stride_t uv_stride,int width,int height)
 {
  typedef TMAKE_COLORSPACE<2,2,2, RGB555, 0,0,0,CCIR> TMAKE_COLORSPACE_yv12_to_rgb555_c;
  TMAKE_COLORSPACE_yv12_to_rgb555_c::MAKE_COLORSPACE(x_ptr,x_stride,y_ptr,u_ptr,v_ptr,y_stride,uv_stride,width,height,typename TMAKE_COLORSPACE_yv12_to_rgb555_c::YV12_TO_RGB16());
 }
 //MAKE_COLORSPACE(yv12_to_rgb565_c,  2,2,2, YV12_TO_RGB16,  MK_RGB565, 0,0,0,CCIR)
 static void yv12_to_rgb565_c(uint8_t * x_ptr,stride_t x_stride,uint8_t * y_ptr,uint8_t * u_ptr,uint8_t * v_ptr,stride_t y_stride,stride_t uv_stride,int width,int height)
 {
  typedef TMAKE_COLORSPACE<2,2,2, RGB565, 0,0,0,CCIR> TMAKE_COLORSPACE_yv12_to_rgb565_c;
  TMAKE_COLORSPACE_yv12_to_rgb565_c::MAKE_COLORSPACE(x_ptr,x_stride,y_ptr,u_ptr,v_ptr,y_stride,uv_stride,width,height,typename TMAKE_COLORSPACE_yv12_to_rgb565_c::YV12_TO_RGB16());
 }
 //MAKE_COLORSPACE(yv12_to_bgr_c,     3,2,2, YV12_TO_RGB,    2,1,0, 0,CCIR)
 static void yv12_to_bgr_c(uint8_t * x_ptr,stride_t x_stride,uint8_t * y_ptr,uint8_t * u_ptr,uint8_t * v_ptr,stride_t y_stride,stride_t uv_stride,int width,int height)
 {
  typedef TMAKE_COLORSPACE<3,2,2, intToVal<2>, 1,0,0,CCIR> TMAKE_COLORSPACE_yv12_to_bgr_c;
  TMAKE_COLORSPACE_yv12_to_bgr_c::MAKE_COLORSPACE(x_ptr,x_stride,y_ptr,u_ptr,v_ptr,y_stride,uv_stride,width,height,typename TMAKE_COLORSPACE_yv12_to_bgr_c::YV12_TO_RGB());
 }
 //MAKE_COLORSPACE(yv12_to_bgra_c,    4,2,2, YV12_TO_RGB,    2,1,0,3,CCIR)
 static void yv12_to_bgra_c(uint8_t * x_ptr,stride_t x_stride,uint8_t * y_ptr,uint8_t * u_ptr,uint8_t * v_ptr,stride_t y_stride,stride_t uv_stride,int width,int height)
 {
  typedef TMAKE_COLORSPACE<4,2,2, intToVal<2>, 1,0,3,CCIR> TMAKE_COLORSPACE_yv12_to_bgra_c;
  TMAKE_COLORSPACE_yv12_to_bgra_c::MAKE_COLORSPACE(x_ptr,x_stride,y_ptr,u_ptr,v_ptr,y_stride,uv_stride,width,height,typename TMAKE_COLORSPACE_yv12_to_bgra_c::YV12_TO_RGB());
 }
 //MAKE_COLORSPACE(yv12_to_abgr_c,    4,2,2, YV12_TO_RGB,    3,2,1,0,CCIR)
 static void yv12_to_abgr_c(uint8_t * x_ptr,stride_t x_stride,uint8_t * y_ptr,uint8_t * u_ptr,uint8_t * v_ptr,stride_t y_stride,stride_t uv_stride,int width,int height)
 {
  typedef TMAKE_COLORSPACE<4,2,2, intToVal<3>, 2,1,0,CCIR> TMAKE_COLORSPACE_yv12_to_abgr_c;
  TMAKE_COLORSPACE_yv12_to_abgr_c::MAKE_COLORSPACE(x_ptr,x_stride,y_ptr,u_ptr,v_ptr,y_stride,uv_stride,width,height,typename TMAKE_COLORSPACE_yv12_to_abgr_c::YV12_TO_RGB());
 }
 //MAKE_COLORSPACE(yv12_to_rgb_c,     3,2,2, YV12_TO_RGB,    0,1,2, 0,CCIR)
 static void yv12_to_rgb_c(uint8_t * x_ptr,stride_t x_stride,uint8_t * y_ptr,uint8_t * u_ptr,uint8_t * v_ptr,stride_t y_stride,stride_t uv_stride,int width,int height)
 {
  typedef TMAKE_COLORSPACE<3,2,2, intToVal<0>, 1,2,0,CCIR> TMAKE_COLORSPACE_yv12_to_rgb_c;
  TMAKE_COLORSPACE_yv12_to_rgb_c::MAKE_COLORSPACE(x_ptr,x_stride,y_ptr,u_ptr,v_ptr,y_stride,uv_stride,width,height,typename TMAKE_COLORSPACE_yv12_to_rgb_c::YV12_TO_RGB());
 }
 //MAKE_COLORSPACE(yv12_to_rgba_c,    4,2,2, YV12_TO_RGB,    0,1,2,3,CCIR)
 static void yv12_to_rgba_c(uint8_t * x_ptr,stride_t x_stride,uint8_t * y_ptr,uint8_t * u_ptr,uint8_t * v_ptr,stride_t y_stride,stride_t uv_stride,int width,int height)
 {
  typedef TMAKE_COLORSPACE<4,2,2, intToVal<0>, 1,2,3,CCIR> TMAKE_COLORSPACE_yv12_to_rgba_c;
  TMAKE_COLORSPACE_yv12_to_rgba_c::MAKE_COLORSPACE(x_ptr,x_stride,y_ptr,u_ptr,v_ptr,y_stride,uv_stride,width,height,typename TMAKE_COLORSPACE_yv12_to_rgba_c::YV12_TO_RGB());
 }
 /*
 //MAKE_COLORSPACE(yv12_to_argb_c,    4,2,2, YV12_TO_RGB,    1,2,3,0,CCIR)
 static void yv12_to_argb_c(uint8_t * x_ptr,stride_t x_stride,uint8_t * y_ptr,uint8_t * u_ptr,uint8_t * v_ptr,stride_t y_stride,stride_t uv_stride,int width,int height)
 {
  typedef TMAKE_COLORSPACE<4,2,2, intToVal<1>, 2,3,0,CCIR> TMAKE_COLORSPACE_yv12_to_argb_c;
  TMAKE_COLORSPACE_yv12_to_argb_c::MAKE_COLORSPACE(x_ptr,x_stride,y_ptr,u_ptr,v_ptr,y_stride,uv_stride,width,height,typename TMAKE_COLORSPACE_yv12_to_argb_c::YV12_TO_RGB());
 }
 */

 //============================ interlaced ============================

 //MAKE_COLORSPACE(yv12_to_rgb555i_c, 2,2,4, YV12_TO_RGB16I, MK_RGB555, 0,0,0,CCIR)
 static void yv12_to_rgb555i_c(uint8_t * x_ptr,stride_t x_stride,uint8_t * y_ptr,uint8_t * u_ptr,uint8_t * v_ptr,stride_t y_stride,stride_t uv_stride,int width,int height)
 {
  typedef TMAKE_COLORSPACE<2,2,4, RGB555, 0,0,0,CCIR> TMAKE_COLORSPACE_yv12_to_rgb555i_c;
  TMAKE_COLORSPACE_yv12_to_rgb555i_c::MAKE_COLORSPACE(x_ptr,x_stride,y_ptr,u_ptr,v_ptr,y_stride,uv_stride,width,height,typename TMAKE_COLORSPACE_yv12_to_rgb555i_c::YV12_TO_RGB16I());
 }
 //MAKE_COLORSPACE(yv12_to_rgb565i_c, 2,2,4, YV12_TO_RGB16I, MK_RGB565, 0,0,0,CCIR)
 static void yv12_to_rgb565i_c(uint8_t * x_ptr,stride_t x_stride,uint8_t * y_ptr,uint8_t * u_ptr,uint8_t * v_ptr,stride_t y_stride,stride_t uv_stride,int width,int height)
 {
  typedef TMAKE_COLORSPACE<2,2,4, RGB565, 0,0,0,CCIR> TMAKE_COLORSPACE_yv12_to_rgb565i_c;
  TMAKE_COLORSPACE_yv12_to_rgb565i_c::MAKE_COLORSPACE(x_ptr,x_stride,y_ptr,u_ptr,v_ptr,y_stride,uv_stride,width,height,typename TMAKE_COLORSPACE_yv12_to_rgb565i_c::YV12_TO_RGB16I());
 }
 //MAKE_COLORSPACE(yv12_to_bgri_c,    3,2,4, YV12_TO_RGBI,   2,1,0,0,CCIR)
 static void yv12_to_bgri_c(uint8_t * x_ptr,stride_t x_stride,uint8_t * y_ptr,uint8_t * u_ptr,uint8_t * v_ptr,stride_t y_stride,stride_t uv_stride,int width,int height)
 {
  typedef TMAKE_COLORSPACE<3,2,4, intToVal<2>, 1,0,0,CCIR> TMAKE_COLORSPACE_yv12_to_bgri_c;
  TMAKE_COLORSPACE_yv12_to_bgri_c::MAKE_COLORSPACE(x_ptr,x_stride,y_ptr,u_ptr,v_ptr,y_stride,uv_stride,width,height,typename TMAKE_COLORSPACE_yv12_to_bgri_c::YV12_TO_RGBI());
 }
 //MAKE_COLORSPACE(yv12_to_bgrai_c,   4,2,4, YV12_TO_RGBI,   2,1,0,3,CCIR)
 static void yv12_to_bgrai_c(uint8_t * x_ptr,stride_t x_stride,uint8_t * y_ptr,uint8_t * u_ptr,uint8_t * v_ptr,stride_t y_stride,stride_t uv_stride,int width,int height)
 {
  typedef TMAKE_COLORSPACE<4,2,4, intToVal<2>, 1,0,3,CCIR> TMAKE_COLORSPACE_yv12_to_bgrai_c;
  TMAKE_COLORSPACE_yv12_to_bgrai_c::MAKE_COLORSPACE(x_ptr,x_stride,y_ptr,u_ptr,v_ptr,y_stride,uv_stride,width,height,typename TMAKE_COLORSPACE_yv12_to_bgrai_c::YV12_TO_RGBI());
 }
 //MAKE_COLORSPACE(yv12_to_abgri_c,   4,2,4, YV12_TO_RGBI,   3,2,1,0,CCIR)
 static void yv12_to_abgri_c(uint8_t * x_ptr,stride_t x_stride,uint8_t * y_ptr,uint8_t * u_ptr,uint8_t * v_ptr,stride_t y_stride,stride_t uv_stride,int width,int height)
 {
  typedef TMAKE_COLORSPACE<4,2,4, intToVal<3>, 2,1,0,CCIR> TMAKE_COLORSPACE_yv12_to_abgri_c;
  TMAKE_COLORSPACE_yv12_to_abgri_c::MAKE_COLORSPACE(x_ptr,x_stride,y_ptr,u_ptr,v_ptr,y_stride,uv_stride,width,height,typename TMAKE_COLORSPACE_yv12_to_abgri_c::YV12_TO_RGBI());
 }
 //MAKE_COLORSPACE(yv12_to_rgbi_c,    3,2,4, YV12_TO_RGBI,   0,1,2,0,CCIR)
 static void yv12_to_rgbi_c(uint8_t * x_ptr,stride_t x_stride,uint8_t * y_ptr,uint8_t * u_ptr,uint8_t * v_ptr,stride_t y_stride,stride_t uv_stride,int width,int height)
 {
  typedef TMAKE_COLORSPACE<3,2,4, intToVal<0>, 1,2,0,CCIR> TMAKE_COLORSPACE_yv12_to_rgbi_c;
  TMAKE_COLORSPACE_yv12_to_rgbi_c::MAKE_COLORSPACE(x_ptr,x_stride,y_ptr,u_ptr,v_ptr,y_stride,uv_stride,width,height,typename TMAKE_COLORSPACE_yv12_to_rgbi_c::YV12_TO_RGBI());
 }
 //MAKE_COLORSPACE(yv12_to_rgbai_c,   4,2,4, YV12_TO_RGBI,   0,1,2,3,CCIR)
 static void yv12_to_rgbai_c(uint8_t * x_ptr,stride_t x_stride,uint8_t * y_ptr,uint8_t * u_ptr,uint8_t * v_ptr,stride_t y_stride,stride_t uv_stride,int width,int height)
 {
  typedef TMAKE_COLORSPACE<4,2,4, intToVal<0>, 1,2,3,CCIR> TMAKE_COLORSPACE_yv12_to_rgbai_c;
  TMAKE_COLORSPACE_yv12_to_rgbai_c::MAKE_COLORSPACE(x_ptr,x_stride,y_ptr,u_ptr,v_ptr,y_stride,uv_stride,width,height,typename TMAKE_COLORSPACE_yv12_to_rgbai_c::YV12_TO_RGBI());
 }
 /*
 //MAKE_COLORSPACE(yv12_to_argbi_c,   4,2,4, YV12_TO_RGBI,   1,2,3,0,CCIR)
 static void yv12_to_argbi_c(uint8_t * x_ptr,stride_t x_stride,uint8_t * y_ptr,uint8_t * u_ptr,uint8_t * v_ptr,stride_t y_stride,stride_t uv_stride,int width,int height)
 {
  typedef TMAKE_COLORSPACE<4,2,4, intToVal<1>, 2,3,0,CCIR> TMAKE_COLORSPACE_yv12_to_argbi_c;
  TMAKE_COLORSPACE_yv12_to_argbi_c::MAKE_COLORSPACE(x_ptr,x_stride,y_ptr,u_ptr,v_ptr,y_stride,uv_stride,width,height,typename TMAKE_COLORSPACE_yv12_to_argbi_c::YV12_TO_RGBI());
 }
 */


 //=======================================================================================
 //MAKE_COLORSPACE(rgb555_to_yv12_c,  2,2,2, RGB16_TO_YV12,  MK_RGB555, 0,0,0,CCIR)
 static void rgb555_to_yv12_c(uint8_t * x_ptr,stride_t x_stride,uint8_t * y_ptr,uint8_t * u_ptr,uint8_t * v_ptr,stride_t y_stride,stride_t uv_stride,int width,int height)
 {
  typedef TMAKE_COLORSPACE<2,2,2, RGB555, 0,0,0,CCIR> TMAKE_COLORSPACE_rgb555_to_yv12_c;
  TMAKE_COLORSPACE_rgb555_to_yv12_c::MAKE_COLORSPACE(x_ptr,x_stride,y_ptr,u_ptr,v_ptr,y_stride,uv_stride,width,height,typename TMAKE_COLORSPACE_rgb555_to_yv12_c::RGB16_TO_YV12());
 }
 //MAKE_COLORSPACE(rgb565_to_yv12_c,  2,2,2, RGB16_TO_YV12,  MK_RGB565, 0,0,0,CCIR)
 static void rgb565_to_yv12_c(uint8_t * x_ptr,stride_t x_stride,uint8_t * y_ptr,uint8_t * u_ptr,uint8_t * v_ptr,stride_t y_stride,stride_t uv_stride,int width,int height)
 {
  typedef TMAKE_COLORSPACE<2,2,2, RGB565, 0,0,0,CCIR> TMAKE_COLORSPACE_rgb565_to_yv12_c;
  TMAKE_COLORSPACE_rgb565_to_yv12_c::MAKE_COLORSPACE(x_ptr,x_stride,y_ptr,u_ptr,v_ptr,y_stride,uv_stride,width,height,typename TMAKE_COLORSPACE_rgb565_to_yv12_c::RGB16_TO_YV12());
 }
 //MAKE_COLORSPACE(bgr_to_yv12_c,     3,2,2, RGB_TO_YV12,    2,1,0, 0,CCIR)
 static void bgr_to_yv12_c(uint8_t * x_ptr,stride_t x_stride,uint8_t * y_ptr,uint8_t * u_ptr,uint8_t * v_ptr,stride_t y_stride,stride_t uv_stride,int width,int height)
 {
  typedef TMAKE_COLORSPACE<3,2,2, intToVal<2>, 1,0,0,CCIR> TMAKE_COLORSPACE_bgr_to_yv12_c;
  TMAKE_COLORSPACE_bgr_to_yv12_c::MAKE_COLORSPACE(x_ptr,x_stride,y_ptr,u_ptr,v_ptr,y_stride,uv_stride,width,height,typename TMAKE_COLORSPACE_bgr_to_yv12_c::RGB_TO_YV12());
 }
 //MAKE_COLORSPACE(bgra_to_yv12_c,    4,2,2, RGB_TO_YV12,    2,1,0, 0,CCIR)
 static void bgra_to_yv12_c(uint8_t * x_ptr,stride_t x_stride,uint8_t * y_ptr,uint8_t * u_ptr,uint8_t * v_ptr,stride_t y_stride,stride_t uv_stride,int width,int height)
 {
  typedef TMAKE_COLORSPACE<4,2,2, intToVal<2>, 1,0,0,CCIR> TMAKE_COLORSPACE_bgra_to_yv12_c;
  TMAKE_COLORSPACE_bgra_to_yv12_c::MAKE_COLORSPACE(x_ptr,x_stride,y_ptr,u_ptr,v_ptr,y_stride,uv_stride,width,height,typename TMAKE_COLORSPACE_bgra_to_yv12_c::RGB_TO_YV12());
 }
 //MAKE_COLORSPACE(abgr_to_yv12_c,    4,2,2, RGB_TO_YV12,    3,2,1, 0,CCIR)
 static void abgr_to_yv12_c(uint8_t * x_ptr,stride_t x_stride,uint8_t * y_ptr,uint8_t * u_ptr,uint8_t * v_ptr,stride_t y_stride,stride_t uv_stride,int width,int height)
 {
  typedef TMAKE_COLORSPACE<4,2,2, intToVal<3>, 2,1,0,CCIR> TMAKE_COLORSPACE_abgr_to_yv12_c;
  TMAKE_COLORSPACE_abgr_to_yv12_c::MAKE_COLORSPACE(x_ptr,x_stride,y_ptr,u_ptr,v_ptr,y_stride,uv_stride,width,height,typename TMAKE_COLORSPACE_abgr_to_yv12_c::RGB_TO_YV12());
 }
  //MAKE_COLORSPACE(rgb_to_yv12_c,     3,2,2, RGB_TO_YV12,    0,1,2, 0,CCIR)
 static void rgb_to_yv12_c(uint8_t * x_ptr,stride_t x_stride,uint8_t * y_ptr,uint8_t * u_ptr,uint8_t * v_ptr,stride_t y_stride,stride_t uv_stride,int width,int height)
 {
  typedef TMAKE_COLORSPACE<3,2,2, intToVal<0>, 1,2,0,CCIR> TMAKE_COLORSPACE_rgb_to_yv12_c;
  TMAKE_COLORSPACE_rgb_to_yv12_c::MAKE_COLORSPACE(x_ptr,x_stride,y_ptr,u_ptr,v_ptr,y_stride,uv_stride,width,height,typename TMAKE_COLORSPACE_rgb_to_yv12_c::RGB_TO_YV12());
 }
 //MAKE_COLORSPACE(rgba_to_yv12_c,    4,2,2, RGB_TO_YV12,    0,1,2, 0,CCIR)
 static void rgba_to_yv12_c(uint8_t * x_ptr,stride_t x_stride,uint8_t * y_ptr,uint8_t * u_ptr,uint8_t * v_ptr,stride_t y_stride,stride_t uv_stride,int width,int height)
 {
  typedef TMAKE_COLORSPACE<4,2,2, intToVal<0>, 0,1,2,CCIR> TMAKE_COLORSPACE_rgba_to_yv12_c;
  TMAKE_COLORSPACE_rgba_to_yv12_c::MAKE_COLORSPACE(x_ptr,x_stride,y_ptr,u_ptr,v_ptr,y_stride,uv_stride,width,height,typename TMAKE_COLORSPACE_rgba_to_yv12_c::RGB_TO_YV12());
 }
 /*
 //MAKE_COLORSPACE(argb_to_yv12_c,    4,2,2, RGB_TO_YV12,    1,2,3, 0,CCIR)
 static void argb_to_yv12_c(uint8_t * x_ptr,stride_t x_stride,uint8_t * y_ptr,uint8_t * u_ptr,uint8_t * v_ptr,stride_t y_stride,stride_t uv_stride,int width,int height)
 {
  typedef TMAKE_COLORSPACE<4,2,2, intToVal<1>, 2,3,0,CCIR> TMAKE_COLORSPACE_argb_to_yv12_c;
  TMAKE_COLORSPACE_argb_to_yv12_c::MAKE_COLORSPACE(x_ptr,x_stride,y_ptr,u_ptr,v_ptr,y_stride,uv_stride,width,height,typename TMAKE_COLORSPACE_argb_to_yv12_c::RGB_TO_YV12());
 }
 */
 //MAKE_COLORSPACE(rgb555i_to_yv12_c, 2,2,4, RGB16I_TO_YV12, MK_RGB555, 0,0,0,CCIR)
 static void rgb555i_to_yv12_c(uint8_t * x_ptr,stride_t x_stride,uint8_t * y_ptr,uint8_t * u_ptr,uint8_t * v_ptr,stride_t y_stride,stride_t uv_stride,int width,int height)
 {
  typedef TMAKE_COLORSPACE<2,2,4, RGB555, 0,0,0,CCIR> TMAKE_COLORSPACE_rgb555i_to_yv12_c;
  TMAKE_COLORSPACE_rgb555i_to_yv12_c::MAKE_COLORSPACE(x_ptr,x_stride,y_ptr,u_ptr,v_ptr,y_stride,uv_stride,width,height,typename TMAKE_COLORSPACE_rgb555i_to_yv12_c::RGB16I_TO_YV12());
 }
 //MAKE_COLORSPACE(rgb565i_to_yv12_c, 2,2,4, RGB16I_TO_YV12, MK_RGB565, 0,0,0,CCIR)
 static void rgb565i_to_yv12_c(uint8_t * x_ptr,stride_t x_stride,uint8_t * y_ptr,uint8_t * u_ptr,uint8_t * v_ptr,stride_t y_stride,stride_t uv_stride,int width,int height)
 {
  typedef TMAKE_COLORSPACE<2,2,4, RGB565, 0,0,0,CCIR> TMAKE_COLORSPACE_rgb565i_to_yv12_c;
  TMAKE_COLORSPACE_rgb565i_to_yv12_c::MAKE_COLORSPACE(x_ptr,x_stride,y_ptr,u_ptr,v_ptr,y_stride,uv_stride,width,height,typename TMAKE_COLORSPACE_rgb565i_to_yv12_c::RGB16I_TO_YV12());
 }
 //MAKE_COLORSPACE(bgri_to_yv12_c,    3,2,4, RGBI_TO_YV12,   2,1,0, 0,CCIR)
 static void bgri_to_yv12_c(uint8_t * x_ptr,stride_t x_stride,uint8_t * y_ptr,uint8_t * u_ptr,uint8_t * v_ptr,stride_t y_stride,stride_t uv_stride,int width,int height)
 {
  typedef TMAKE_COLORSPACE<3,2,4, intToVal<2>, 1,0,0,CCIR> TMAKE_COLORSPACE_bgri_to_yv12_c;
  TMAKE_COLORSPACE_bgri_to_yv12_c::MAKE_COLORSPACE(x_ptr,x_stride,y_ptr,u_ptr,v_ptr,y_stride,uv_stride,width,height,typename TMAKE_COLORSPACE_bgri_to_yv12_c::RGBI_TO_YV12());
 }
 //MAKE_COLORSPACE(bgrai_to_yv12_c,   4,2,4, RGBI_TO_YV12,   2,1,0, 0,CCIR)
 static void bgrai_to_yv12_c(uint8_t * x_ptr,stride_t x_stride,uint8_t * y_ptr,uint8_t * u_ptr,uint8_t * v_ptr,stride_t y_stride,stride_t uv_stride,int width,int height)
 {
  typedef TMAKE_COLORSPACE<4,2,4, intToVal<2>, 1,0,0,CCIR> TMAKE_COLORSPACE_bgrai_to_yv12_c;
  TMAKE_COLORSPACE_bgrai_to_yv12_c::MAKE_COLORSPACE(x_ptr,x_stride,y_ptr,u_ptr,v_ptr,y_stride,uv_stride,width,height,typename TMAKE_COLORSPACE_bgrai_to_yv12_c::RGBI_TO_YV12());
 }
 //MAKE_COLORSPACE(abgri_to_yv12_c,   4,2,4, RGBI_TO_YV12,   3,2,1, 0,CCIR)
 static void abgri_to_yv12_c(uint8_t * x_ptr,stride_t x_stride,uint8_t * y_ptr,uint8_t * u_ptr,uint8_t * v_ptr,stride_t y_stride,stride_t uv_stride,int width,int height)
 {
  typedef TMAKE_COLORSPACE<4,2,4, intToVal<3>, 2,1,0,CCIR> TMAKE_COLORSPACE_abgri_to_yv12_c;
  TMAKE_COLORSPACE_abgri_to_yv12_c::MAKE_COLORSPACE(x_ptr,x_stride,y_ptr,u_ptr,v_ptr,y_stride,uv_stride,width,height,typename TMAKE_COLORSPACE_abgri_to_yv12_c::RGBI_TO_YV12());
 }
  //MAKE_COLORSPACE(rgbi_to_yv12_c,    3,2,4, RGBI_TO_YV12,   0,1,2, 0,CCIR)
 static void rgbi_to_yv12_c(uint8_t * x_ptr,stride_t x_stride,uint8_t * y_ptr,uint8_t * u_ptr,uint8_t * v_ptr,stride_t y_stride,stride_t uv_stride,int width,int height)
 {
  typedef TMAKE_COLORSPACE<3,2,4, intToVal<0>, 1,2,0,CCIR> TMAKE_COLORSPACE_rgbi_to_yv12_c;
  TMAKE_COLORSPACE_rgbi_to_yv12_c::MAKE_COLORSPACE(x_ptr,x_stride,y_ptr,u_ptr,v_ptr,y_stride,uv_stride,width,height,typename TMAKE_COLORSPACE_rgbi_to_yv12_c::RGBI_TO_YV12());
 }
 //MAKE_COLORSPACE(rgbai_to_yv12_c,   4,2,4, RGBI_TO_YV12,   0,1,2, 0,CCIR)
 static void rgbai_to_yv12_c(uint8_t * x_ptr,stride_t x_stride,uint8_t * y_ptr,uint8_t * u_ptr,uint8_t * v_ptr,stride_t y_stride,stride_t uv_stride,int width,int height)
 {
  typedef TMAKE_COLORSPACE<4,2,4, intToVal<0>, 1,2,0,CCIR> TMAKE_COLORSPACE_rgbai_to_yv12_c;
  TMAKE_COLORSPACE_rgbai_to_yv12_c::MAKE_COLORSPACE(x_ptr,x_stride,y_ptr,u_ptr,v_ptr,y_stride,uv_stride,width,height,typename TMAKE_COLORSPACE_rgbai_to_yv12_c::RGBI_TO_YV12());
 }
 /*
 //MAKE_COLORSPACE(argbi_to_yv12_c,   4,2,4, RGBI_TO_YV12,   1,2,3, 0,CCIR)
 static void argbi_to_yv12_c(uint8_t * x_ptr,stride_t x_stride,uint8_t * y_ptr,uint8_t * u_ptr,uint8_t * v_ptr,stride_t y_stride,stride_t uv_stride,int width,int height)
 {
  typedef TMAKE_COLORSPACE<4,2,4, intToVal<1>, 2,3,0,CCIR> TMAKE_COLORSPACE_argbi_to_yv12_c;
  TMAKE_COLORSPACE_argbi_to_yv12_c::MAKE_COLORSPACE(x_ptr,x_stride,y_ptr,u_ptr,v_ptr,y_stride,uv_stride,width,height,typename TMAKE_COLORSPACE_argbi_to_yv12_c::RGBI_TO_YV12());
 }
 */

 static void bgr_to_yv12_mmx(uint8_t * x_ptr,stride_t x_stride,uint8_t * y_src,uint8_t * u_src,uint8_t * v_src,stride_t y_stride,stride_t uv_stride,int width,int height)
 {
#ifndef WIN64
  bgr_to_yv12_mmx_asm(x_ptr,x_stride,y_src,u_src,v_src,y_stride,uv_stride,width,height,false);
#else
  bgr_to_yv12_win64_sse2(x_ptr,x_stride,y_src,u_src,v_src,y_stride,uv_stride,width,height,false);
#endif
 }
 static void bgra_to_yv12_mmx(uint8_t * x_ptr,stride_t x_stride,uint8_t * y_src,uint8_t * u_src,uint8_t * v_src,stride_t y_stride,stride_t uv_stride,int width,int height)
 {
#ifndef WIN64
  bgra_to_yv12_mmx_asm(x_ptr,x_stride,y_src,u_src,v_src,y_stride,uv_stride,width,height,false);
#else
  bgra_to_yv12_win64_sse2(x_ptr,x_stride,y_src,u_src,v_src,y_stride,uv_stride,width,height,false);
#endif
 }

 static void yv12_to_bgr_mmx(uint8_t * x_ptr,stride_t x_stride,uint8_t * y_src,uint8_t * u_src,uint8_t * v_src,stride_t y_stride,stride_t uv_stride,int width,int height)
 {
#ifndef WIN64
  // WIN32
  yv12_to_bgr_mmx_asm(x_ptr,x_stride,y_src,u_src,v_src,y_stride,uv_stride,width,height,false);
#else
  // WIN64
  if (((size_t)x_ptr&15) || ((size_t)x_stride&15) || ((size_t)y_src&15) || ((size_t)y_stride&15))
   yv12_to_bgr_win64_sse2u(x_ptr,x_stride,y_src,u_src,v_src,y_stride,uv_stride,width,height,false); // fail over (unaligned)
  else
   yv12_to_bgr_win64_sse2a(x_ptr,x_stride,y_src,u_src,v_src,y_stride,uv_stride,width,height,false); // faster
#endif
 }
 static void yv12_to_bgra_mmx(uint8_t * x_ptr,stride_t x_stride,uint8_t * y_src,uint8_t * u_src,uint8_t * v_src,stride_t y_stride,stride_t uv_stride,int width,int height)
 {
#ifndef WIN64
  // WIN32
  yv12_to_bgra_mmx_asm(x_ptr,x_stride,y_src,u_src,v_src,y_stride,uv_stride,width,height,false);
#else
  // WIN64
  if (((size_t)x_ptr&15) || ((size_t)x_stride&15) || ((size_t)y_src&15) || ((size_t)y_stride&15))
   yv12_to_bgra_win64_sse2u(x_ptr,x_stride,y_src,u_src,v_src,y_stride,uv_stride,width,height,false); // fail over (unaligned)
  else
   yv12_to_bgra_win64_sse2a(x_ptr,x_stride,y_src,u_src,v_src,y_stride,uv_stride,width,height,false); // faster
#endif
 }
};

template<> packedFuncPtr TpackedFuncPtrRGB<CCIR>::rgb555_to_yv12=NULL; template<> packedFuncPtr TpackedFuncPtrRGB<JPEG>::rgb555_to_yv12=NULL;
template<> packedFuncPtr TpackedFuncPtrRGB<CCIR>::rgb565_to_yv12=NULL; template<> packedFuncPtr TpackedFuncPtrRGB<JPEG>::rgb565_to_yv12=NULL;
template<> packedFuncPtr TpackedFuncPtrRGB<CCIR>::bgr_to_yv12=NULL;    template<> packedFuncPtr TpackedFuncPtrRGB<JPEG>::bgr_to_yv12=NULL;
template<> packedFuncPtr TpackedFuncPtrRGB<CCIR>::bgra_to_yv12=NULL;   template<> packedFuncPtr TpackedFuncPtrRGB<JPEG>::bgra_to_yv12=NULL;
template<> packedFuncPtr TpackedFuncPtrRGB<CCIR>::abgr_to_yv12=NULL;   template<> packedFuncPtr TpackedFuncPtrRGB<JPEG>::abgr_to_yv12=NULL;
template<> packedFuncPtr TpackedFuncPtrRGB<CCIR>::rgb_to_yv12=NULL;    template<> packedFuncPtr TpackedFuncPtrRGB<JPEG>::rgb_to_yv12=NULL;
template<> packedFuncPtr TpackedFuncPtrRGB<CCIR>::rgba_to_yv12=NULL;   template<> packedFuncPtr TpackedFuncPtrRGB<JPEG>::rgba_to_yv12=NULL;
template<> packedFuncPtr TpackedFuncPtrRGB<CCIR>::argb_to_yv12=NULL;   template<> packedFuncPtr TpackedFuncPtrRGB<JPEG>::argb_to_yv12=NULL;

template<> packedFuncPtr TpackedFuncPtrRGB<CCIR>::rgb555i_to_yv12=NULL;template<> packedFuncPtr TpackedFuncPtrRGB<JPEG>::rgb555i_to_yv12=NULL;
template<> packedFuncPtr TpackedFuncPtrRGB<CCIR>::rgb565i_to_yv12=NULL;template<> packedFuncPtr TpackedFuncPtrRGB<JPEG>::rgb565i_to_yv12=NULL;
template<> packedFuncPtr TpackedFuncPtrRGB<CCIR>::bgri_to_yv12=NULL;   template<> packedFuncPtr TpackedFuncPtrRGB<JPEG>::bgri_to_yv12=NULL;
template<> packedFuncPtr TpackedFuncPtrRGB<CCIR>::bgrai_to_yv12=NULL;  template<> packedFuncPtr TpackedFuncPtrRGB<JPEG>::bgrai_to_yv12=NULL;
template<> packedFuncPtr TpackedFuncPtrRGB<CCIR>::abgri_to_yv12=NULL;  template<> packedFuncPtr TpackedFuncPtrRGB<JPEG>::abgri_to_yv12=NULL;
template<> packedFuncPtr TpackedFuncPtrRGB<CCIR>::rgbi_to_yv12=NULL;   template<> packedFuncPtr TpackedFuncPtrRGB<JPEG>::rgbi_to_yv12=NULL;
template<> packedFuncPtr TpackedFuncPtrRGB<CCIR>::rgbai_to_yv12=NULL;  template<> packedFuncPtr TpackedFuncPtrRGB<JPEG>::rgbai_to_yv12=NULL;
template<> packedFuncPtr TpackedFuncPtrRGB<CCIR>::argbi_to_yv12=NULL;  template<> packedFuncPtr TpackedFuncPtrRGB<JPEG>::argbi_to_yv12=NULL;

template<> packedFuncPtr TpackedFuncPtrRGB<CCIR>::yv12_to_rgb555=NULL; template<> packedFuncPtr TpackedFuncPtrRGB<JPEG>::yv12_to_rgb555=NULL;
template<> packedFuncPtr TpackedFuncPtrRGB<CCIR>::yv12_to_rgb565=NULL; template<> packedFuncPtr TpackedFuncPtrRGB<JPEG>::yv12_to_rgb565=NULL;
template<> packedFuncPtr TpackedFuncPtrRGB<CCIR>::yv12_to_bgr=NULL;    template<> packedFuncPtr TpackedFuncPtrRGB<JPEG>::yv12_to_bgr=NULL;
template<> packedFuncPtr TpackedFuncPtrRGB<CCIR>::yv12_to_bgra=NULL;   template<> packedFuncPtr TpackedFuncPtrRGB<JPEG>::yv12_to_bgra=NULL;
template<> packedFuncPtr TpackedFuncPtrRGB<CCIR>::yv12_to_abgr=NULL;   template<> packedFuncPtr TpackedFuncPtrRGB<JPEG>::yv12_to_abgr=NULL;
template<> packedFuncPtr TpackedFuncPtrRGB<CCIR>::yv12_to_rgb=NULL;    template<> packedFuncPtr TpackedFuncPtrRGB<JPEG>::yv12_to_rgb=NULL;
template<> packedFuncPtr TpackedFuncPtrRGB<CCIR>::yv12_to_rgba=NULL;   template<> packedFuncPtr TpackedFuncPtrRGB<JPEG>::yv12_to_rgba=NULL;
template<> packedFuncPtr TpackedFuncPtrRGB<CCIR>::yv12_to_argb=NULL;   template<> packedFuncPtr TpackedFuncPtrRGB<JPEG>::yv12_to_argb=NULL;

template<> packedFuncPtr TpackedFuncPtrRGB<CCIR>::yv12_to_rgb555i=NULL;template<> packedFuncPtr TpackedFuncPtrRGB<JPEG>::yv12_to_rgb555i=NULL;
template<> packedFuncPtr TpackedFuncPtrRGB<CCIR>::yv12_to_rgb565i=NULL;template<> packedFuncPtr TpackedFuncPtrRGB<JPEG>::yv12_to_rgb565i=NULL;
template<> packedFuncPtr TpackedFuncPtrRGB<CCIR>::yv12_to_bgri=NULL;   template<> packedFuncPtr TpackedFuncPtrRGB<JPEG>::yv12_to_bgri=NULL;
template<> packedFuncPtr TpackedFuncPtrRGB<CCIR>::yv12_to_bgrai=NULL;  template<> packedFuncPtr TpackedFuncPtrRGB<JPEG>::yv12_to_bgrai=NULL;
template<> packedFuncPtr TpackedFuncPtrRGB<CCIR>::yv12_to_abgri=NULL;  template<> packedFuncPtr TpackedFuncPtrRGB<JPEG>::yv12_to_abgri=NULL;
template<> packedFuncPtr TpackedFuncPtrRGB<CCIR>::yv12_to_rgbi=NULL;   template<> packedFuncPtr TpackedFuncPtrRGB<JPEG>::yv12_to_rgbi=NULL;
template<> packedFuncPtr TpackedFuncPtrRGB<CCIR>::yv12_to_rgbai=NULL;  template<> packedFuncPtr TpackedFuncPtrRGB<JPEG>::yv12_to_rgbai=NULL;
template<> packedFuncPtr TpackedFuncPtrRGB<CCIR>::yv12_to_argbi=NULL;  template<> packedFuncPtr TpackedFuncPtrRGB<JPEG>::yv12_to_argbi=NULL;

/*
  perform safe packed colorspace conversion, by splitting
  the image up into an optimized area (pixel width divisible by 16),
  and two unoptimized/plain-c areas (pixel width divisible by 2)
*/

static void safe_packed_conv(uint8_t * x_ptr, stride_t x_stride,
                             uint8_t * y_ptr, uint8_t * u_ptr, uint8_t * v_ptr,
                             stride_t y_stride, stride_t uv_stride,
                             int width, int height,
                             packedFunc * func_opt, packedFunc func_c, int size)
{
        int width_opt, width_c;

        width_opt = width;
        width_c = 0;
        if (func_opt != func_c)
         {
          if (x_stride < size*((width+15)/16)*16)
           width_opt = width & (~15);

          if (size==3)//if ((width_opt/size)&3)
           width_opt = std::max(0, width_opt - 16);
          width_c = width - width_opt;
         }

        func_opt(x_ptr, x_stride,
                        y_ptr, u_ptr, v_ptr, y_stride, uv_stride,
                        width_opt, height&~1);
        if (width_c)
        {
                _mm_empty();
                func_c(x_ptr + size*width_opt, x_stride,
                        y_ptr + width_opt, u_ptr + width_opt/2, v_ptr + width_opt/2,
                        y_stride, uv_stride, width_c, height&~1);
        }
}



int image_input(IMAGE * image,
                uint32_t width,
                int height,
                stride_t edged_width,stride_t edged_width2,
                const uint8_t * src,
                stride_t src_stride,
                int csp,
                int interlacing,int jpeg)
{

        switch (csp & FF_CSPS_MASK) {
        case FF_CSP_RGB15:
                safe_packed_conv(
                        (uint8_t*)src, src_stride, image->y, image->u, image->v,
                        edged_width, edged_width2, width, height,
                        interlacing?(jpeg?TpackedFuncPtrRGB<JPEG>::rgb555i_to_yv12:TpackedFuncPtrRGB<CCIR>::rgb555i_to_yv12)  :(jpeg?TpackedFuncPtrRGB<JPEG>::rgb555_to_yv12:TpackedFuncPtrRGB<CCIR>::rgb555_to_yv12),
                        interlacing?(jpeg?TpackedFuncPtrRGB<JPEG>::rgb555i_to_yv12_c:TpackedFuncPtrRGB<CCIR>::rgb555i_to_yv12_c):(jpeg?TpackedFuncPtrRGB<JPEG>::rgb555_to_yv12_c:TpackedFuncPtrRGB<CCIR>::rgb555_to_yv12_c), 2);
                break;

        case FF_CSP_RGB16:
                safe_packed_conv(
                        (uint8_t*)src, src_stride, image->y, image->u, image->v,
                        edged_width, edged_width2, width, height,
                        interlacing?(jpeg?TpackedFuncPtrRGB<JPEG>::rgb565i_to_yv12:TpackedFuncPtrRGB<CCIR>::rgb565i_to_yv12)  :(jpeg?TpackedFuncPtrRGB<JPEG>::rgb565_to_yv12:TpackedFuncPtrRGB<CCIR>::rgb565_to_yv12),
                        interlacing?(jpeg?TpackedFuncPtrRGB<JPEG>::rgb565i_to_yv12_c:TpackedFuncPtrRGB<CCIR>::rgb565i_to_yv12_c):(jpeg?TpackedFuncPtrRGB<JPEG>::rgb565_to_yv12_c:TpackedFuncPtrRGB<CCIR>::rgb565_to_yv12_c), 2);
                break;

        // RGB Values: the xvid library refers to the write order, FF_CSP_ enum refers to the "memory byte order",
        // which under x86 is reversed, see the comment above the FF_CSP_ enum definition.
        case FF_CSP_RGB24:
                safe_packed_conv(
                        (uint8_t*)src, src_stride, image->y, image->u, image->v,
                        edged_width, edged_width2, width, height,
                        interlacing?(jpeg?TpackedFuncPtrRGB<JPEG>::bgri_to_yv12:TpackedFuncPtrRGB<CCIR>::bgri_to_yv12)  :(jpeg?TpackedFuncPtrRGB<JPEG>::bgr_to_yv12:TpackedFuncPtrRGB<CCIR>::bgr_to_yv12),
                        interlacing?(jpeg?TpackedFuncPtrRGB<JPEG>::bgri_to_yv12_c:TpackedFuncPtrRGB<CCIR>::bgri_to_yv12_c):(jpeg?TpackedFuncPtrRGB<JPEG>::bgr_to_yv12_c:TpackedFuncPtrRGB<CCIR>::bgr_to_yv12_c), 3);
                break;

        // RGB Values: the xvid library refers to the write order, FF_CSP_ enum refers to the "memory byte order",
        // which under x86 is reversed, see the comment above the FF_CSP_ enum definition.
        case FF_CSP_RGB32:
                safe_packed_conv(
                        (uint8_t*)src, src_stride, image->y, image->u, image->v,
                        edged_width, edged_width2, width, height,
                        interlacing?(jpeg?TpackedFuncPtrRGB<JPEG>::bgrai_to_yv12:TpackedFuncPtrRGB<CCIR>::bgrai_to_yv12)  :(jpeg?TpackedFuncPtrRGB<JPEG>::bgra_to_yv12:TpackedFuncPtrRGB<CCIR>::bgra_to_yv12),
                        interlacing?(jpeg?TpackedFuncPtrRGB<JPEG>::bgrai_to_yv12_c:TpackedFuncPtrRGB<CCIR>::bgrai_to_yv12_c):(jpeg?TpackedFuncPtrRGB<JPEG>::bgra_to_yv12_c:TpackedFuncPtrRGB<CCIR>::bgra_to_yv12_c), 4);
                break;

        // RGB Values: the xvid library refers to the write order, FF_CSP_ enum refers to the "memory byte order",
        // which under x86 is reversed, see the comment above the FF_CSP_ enum definition.
        case FF_CSP_BGR24 :
                safe_packed_conv(
                        (uint8_t*)src, src_stride, image->y, image->u, image->v,
                        edged_width, edged_width2, width, height,
                        interlacing?(jpeg?TpackedFuncPtrRGB<JPEG>::rgbi_to_yv12:TpackedFuncPtrRGB<CCIR>::rgbi_to_yv12)  :(jpeg?TpackedFuncPtrRGB<JPEG>::rgb_to_yv12:TpackedFuncPtrRGB<CCIR>::rgb_to_yv12),
                        interlacing?(jpeg?TpackedFuncPtrRGB<JPEG>::rgbi_to_yv12_c:TpackedFuncPtrRGB<CCIR>::rgbi_to_yv12_c):(jpeg?TpackedFuncPtrRGB<JPEG>::rgb_to_yv12_c:TpackedFuncPtrRGB<CCIR>::rgb_to_yv12_c), 3);
                break;

        case FF_CSP_ABGR :
                safe_packed_conv(
                        (uint8_t*)src, src_stride, image->y, image->u, image->v,
                        edged_width, edged_width2, width, height,
                        interlacing?(jpeg?TpackedFuncPtrRGB<JPEG>::abgri_to_yv12:TpackedFuncPtrRGB<CCIR>::abgri_to_yv12)  :(jpeg?TpackedFuncPtrRGB<JPEG>::abgr_to_yv12:TpackedFuncPtrRGB<CCIR>::abgr_to_yv12),
                        interlacing?(jpeg?TpackedFuncPtrRGB<JPEG>::abgri_to_yv12_c:TpackedFuncPtrRGB<CCIR>::abgri_to_yv12_c):(jpeg?TpackedFuncPtrRGB<JPEG>::abgr_to_yv12_c:TpackedFuncPtrRGB<CCIR>::abgr_to_yv12_c), 4);
                break;

        case FF_CSP_RGBA :
                safe_packed_conv(
                        (uint8_t*)src, src_stride, image->y, image->u, image->v,
                        edged_width, edged_width2, width, height,
                        interlacing?(jpeg?TpackedFuncPtrRGB<JPEG>::rgbai_to_yv12:TpackedFuncPtrRGB<CCIR>::rgbai_to_yv12)  :(jpeg?TpackedFuncPtrRGB<JPEG>::rgba_to_yv12:TpackedFuncPtrRGB<CCIR>::rgba_to_yv12),
                        interlacing?(jpeg?TpackedFuncPtrRGB<JPEG>::rgbai_to_yv12_c:TpackedFuncPtrRGB<CCIR>::rgbai_to_yv12_c):(jpeg?TpackedFuncPtrRGB<JPEG>::rgba_to_yv12_c:TpackedFuncPtrRGB<CCIR>::rgba_to_yv12_c), 4);
                break;

        case FF_CSP_YUY2:
                safe_packed_conv(
                        (uint8_t*)src, src_stride, image->y, image->u, image->v,
                        edged_width, edged_width2, width, height,
                        interlacing?yuyvi_to_yv12  :yuyv_to_yv12,
                        interlacing?yuyvi_to_yv12_c:yuyv_to_yv12_c, 2);
                break;

        case FF_CSP_YVYU:               /* u/v swapped */
                safe_packed_conv(
                        (uint8_t*)src, src_stride, image->y, image->v, image->u,
                        edged_width, edged_width2, width, height,
                        interlacing?yuyvi_to_yv12  :yuyv_to_yv12,
                        interlacing?yuyvi_to_yv12_c:yuyv_to_yv12_c, 2);
                break;

        case FF_CSP_UYVY:
                safe_packed_conv(
                        (uint8_t*)src, src_stride, image->y, image->u, image->v,
                        edged_width, edged_width2, width, height,
                        interlacing?uyvyi_to_yv12  :uyvy_to_yv12,
                        interlacing?uyvyi_to_yv12_c:uyvy_to_yv12_c, 2);
                break;

        case FF_CSP_VYUY:   /* u/v swapped */
               safe_packed_conv(
                (uint8_t*)src, src_stride, image->y, image->v, image->u,
                edged_width, edged_width2, width, height,
                interlacing?uyvyi_to_yv12  :uyvy_to_yv12,
                interlacing?uyvyi_to_yv12_c:uyvy_to_yv12_c, 2);
               break;
/*
        case FF_CSP_I420:
                yv12_to_yv12(image->y, image->u, image->v, edged_width, edged_width2,
                        (uint8_t*)src, (uint8_t*)src + width*height, (uint8_t*)src + width*height + width2*height2,
                        width, width2, width, height, (csp & FF_CSP_VFLIP));
                break;
        case FF_CSP_420P:               // u/v swapped
                yv12_to_yv12(image->y, image->v, image->u, edged_width, edged_width2,
                        (uint8_t*)src, (uint8_t*)src + width*height, (uint8_t*)src + width*height + width2*height2,
                        width, width2, width, height, (csp & FF_CSP_VFLIP));
                break;
*/

        case FF_CSP_NULL:
                return 0;

        default :
                return -1;
        }
 _mm_empty();
 return 0;
}



int image_output(IMAGE * image,
                 uint32_t width,
                 int height,
                 stride_t edged_width[4],
                 uint8_t * dst[4],
                 stride_t dst_stride[4],
                 int csp,
                 int interlacing,int jpeg)
{

        switch (csp & FF_CSPS_MASK) {
        case FF_CSP_RGB15:
                safe_packed_conv(
                        dst[0], dst_stride[0], image->y, image->u, image->v,
                        edged_width[0], edged_width[1], width, height,
                        interlacing?(jpeg?TpackedFuncPtrRGB<JPEG>::yv12_to_rgb555i:TpackedFuncPtrRGB<CCIR>::yv12_to_rgb555i)  :(jpeg?TpackedFuncPtrRGB<JPEG>::yv12_to_rgb555:TpackedFuncPtrRGB<CCIR>::yv12_to_rgb555),
                        interlacing?(jpeg?TpackedFuncPtrRGB<JPEG>::yv12_to_rgb555i_c:TpackedFuncPtrRGB<CCIR>::yv12_to_rgb555i_c):(jpeg?TpackedFuncPtrRGB<JPEG>::yv12_to_rgb555_c:TpackedFuncPtrRGB<CCIR>::yv12_to_rgb555_c), 2);
                break;

        case FF_CSP_RGB16:
                safe_packed_conv(
                        dst[0], dst_stride[0], image->y, image->u, image->v,
                        edged_width[0], edged_width[1], width, height,
                        interlacing?(jpeg?TpackedFuncPtrRGB<JPEG>::yv12_to_rgb565i:TpackedFuncPtrRGB<CCIR>::yv12_to_rgb565i)  :(jpeg?TpackedFuncPtrRGB<JPEG>::yv12_to_rgb565:TpackedFuncPtrRGB<CCIR>::yv12_to_rgb565),
                        interlacing?(jpeg?TpackedFuncPtrRGB<JPEG>::yv12_to_rgb565i_c:TpackedFuncPtrRGB<CCIR>::yv12_to_rgb565i_c):(jpeg?TpackedFuncPtrRGB<JPEG>::yv12_to_rgb565_c:TpackedFuncPtrRGB<CCIR>::yv12_to_rgb565_c), 2);
                break;

        // RGB Values: the xvid library refers to the write order, FF_CSP_ enum refers to the "memory byte order",
        // which under x86 is reversed, see the comment above the FF_CSP_ enum definition.
        case FF_CSP_RGB24:
                safe_packed_conv(
                        dst[0], dst_stride[0], image->y, image->u, image->v,
                        edged_width[0], edged_width[1], width, height,
                        interlacing?(jpeg?TpackedFuncPtrRGB<JPEG>::yv12_to_bgri:TpackedFuncPtrRGB<CCIR>::yv12_to_bgri)  :(jpeg?TpackedFuncPtrRGB<JPEG>::yv12_to_bgr:TpackedFuncPtrRGB<CCIR>::yv12_to_bgr),
                        interlacing?(jpeg?TpackedFuncPtrRGB<JPEG>::yv12_to_bgri_c:TpackedFuncPtrRGB<CCIR>::yv12_to_bgri_c):(jpeg?TpackedFuncPtrRGB<JPEG>::yv12_to_bgr_c:TpackedFuncPtrRGB<CCIR>::yv12_to_bgr_c), 3);
                break;

        // RGB Values: the xvid library refers to the write order, FF_CSP_ enum refers to the "memory byte order",
        // which under x86 is reversed, see the comment above the FF_CSP_ enum definition.
        case FF_CSP_RGB32:
                safe_packed_conv(
                        dst[0], dst_stride[0], image->y, image->u, image->v,
                        edged_width[0], edged_width[1], width, height,
                        interlacing?(jpeg?TpackedFuncPtrRGB<JPEG>::yv12_to_bgrai:TpackedFuncPtrRGB<CCIR>::yv12_to_bgrai)  :(jpeg?TpackedFuncPtrRGB<JPEG>::yv12_to_bgra:TpackedFuncPtrRGB<CCIR>::yv12_to_bgra),
                        interlacing?(jpeg?TpackedFuncPtrRGB<JPEG>::yv12_to_bgrai_c:TpackedFuncPtrRGB<CCIR>::yv12_to_bgrai_c):(jpeg?TpackedFuncPtrRGB<JPEG>::yv12_to_bgra_c:TpackedFuncPtrRGB<CCIR>::yv12_to_bgra_c), 4);
                break;

        // RGB Values: the xvid library refers to the write order, FF_CSP_ enum refers to the "memory byte order",
        // which under x86 is reversed, see the comment above the FF_CSP_ enum definition.
        case FF_CSP_BGR24:
                safe_packed_conv(
                        dst[0], dst_stride[0], image->y, image->u, image->v,
                        edged_width[0], edged_width[1], width, height,
                        interlacing?(jpeg?TpackedFuncPtrRGB<JPEG>::yv12_to_rgbi:TpackedFuncPtrRGB<CCIR>::yv12_to_rgbi)  :(jpeg?TpackedFuncPtrRGB<JPEG>::yv12_to_rgb:TpackedFuncPtrRGB<CCIR>::yv12_to_rgb),
                        interlacing?(jpeg?TpackedFuncPtrRGB<JPEG>::yv12_to_rgbi_c:TpackedFuncPtrRGB<CCIR>::yv12_to_rgbi_c):(jpeg?TpackedFuncPtrRGB<JPEG>::yv12_to_rgb_c:TpackedFuncPtrRGB<CCIR>::yv12_to_rgb_c), 3);
                break;

        case FF_CSP_ABGR:
                safe_packed_conv(
                        dst[0], dst_stride[0], image->y, image->u, image->v,
                        edged_width[0], edged_width[1], width, height,
                        interlacing?(jpeg?TpackedFuncPtrRGB<JPEG>::yv12_to_abgri:TpackedFuncPtrRGB<CCIR>::yv12_to_abgri)  :(jpeg?TpackedFuncPtrRGB<JPEG>::yv12_to_abgr:TpackedFuncPtrRGB<CCIR>::yv12_to_abgr),
                        interlacing?(jpeg?TpackedFuncPtrRGB<JPEG>::yv12_to_abgri_c:TpackedFuncPtrRGB<CCIR>::yv12_to_abgri_c):(jpeg?TpackedFuncPtrRGB<JPEG>::yv12_to_abgr_c:TpackedFuncPtrRGB<CCIR>::yv12_to_abgr_c), 4);
                break;

        case FF_CSP_RGBA:
                safe_packed_conv(
                        dst[0], dst_stride[0], image->y, image->u, image->v,
                        edged_width[0], edged_width[1], width, height,
                        interlacing?(jpeg?TpackedFuncPtrRGB<JPEG>::yv12_to_rgbai:TpackedFuncPtrRGB<CCIR>::yv12_to_rgbai)  :(jpeg?TpackedFuncPtrRGB<JPEG>::yv12_to_rgba:TpackedFuncPtrRGB<CCIR>::yv12_to_rgba),
                        interlacing?(jpeg?TpackedFuncPtrRGB<JPEG>::yv12_to_rgbai_c:TpackedFuncPtrRGB<CCIR>::yv12_to_rgbai_c):(jpeg?TpackedFuncPtrRGB<JPEG>::yv12_to_rgba_c:TpackedFuncPtrRGB<CCIR>::yv12_to_rgba_c), 4);
                break;

        case FF_CSP_YUY2:
                safe_packed_conv(
                        dst[0], dst_stride[0], image->y, image->u, image->v,
                        edged_width[0], edged_width[1], width, height,
                        interlacing?yv12_to_yuyvi  :yv12_to_yuyv,
                        interlacing?yv12_to_yuyvi_c:yv12_to_yuyv_c, 2);
                break;

        case FF_CSP_YVYU:               // u,v swapped
                safe_packed_conv(
                        dst[0], dst_stride[0], image->y, image->v, image->u,
                        edged_width[0], edged_width[1], width, height,
                        interlacing?yv12_to_yuyvi  :yv12_to_yuyv,
                        interlacing?yv12_to_yuyvi_c:yv12_to_yuyv_c, 2);
                break;

        case FF_CSP_UYVY:
                safe_packed_conv(
                        dst[0], dst_stride[0], image->y, image->u, image->v,
                        edged_width[0], edged_width[1], width, height,
                        interlacing?yv12_to_uyvyi  :yv12_to_uyvy,
                        interlacing?yv12_to_uyvyi_c:yv12_to_uyvy_c, 2);
                break;

        case FF_CSP_VYUY:       // u,v swapped
                safe_packed_conv(
                        dst[0], dst_stride[0], image->y, image->v, image->u,
                        edged_width[0], edged_width[1], width, height,
                        interlacing?yv12_to_uyvyi  :yv12_to_uyvy,
                        interlacing?yv12_to_uyvyi_c:yv12_to_uyvy_c, 2);
                break;

        case FF_CSP_420P:
                yv12_to_yv12( dst[0], dst[2], dst[1],
                              dst_stride[0], dst_stride[1],
                              image->y, image->v, image->u,
                              edged_width[0], edged_width[1], width, height );

                break;
        case FF_CSP_NULL:
                return 0;
        default:
                return -1;

        }
        _mm_empty();
        return 0;
}

// initialize rgb lookup tables
template<class CCIR> static void colorspace_init(
                      double rgb_y_out,
                      double b_u_out,
                      double g_u_out,
                      double g_v_out,
                      double r_v_out,
                      int    y_add_out)
{
 #define FIX_OUT(x) ((uint16_t) ((x) * (1L<<SCALEBITS_OUT) + 0.5))
 for (int i = 0; i < 256; i++)
  {
   CCIR::RGB_Y_tab[i] = FIX_OUT(rgb_y_out) * (i - y_add_out);
   CCIR::B_U_tab[i] = FIX_OUT(b_u_out) * (i - CCIR::U_ADD_OUT);
   CCIR::G_U_tab[i] = FIX_OUT(g_u_out) * (i - CCIR::U_ADD_OUT);
   CCIR::G_V_tab[i] = FIX_OUT(g_v_out) * (i - CCIR::V_ADD_OUT);
   CCIR::R_V_tab[i] = FIX_OUT(r_v_out) * (i - CCIR::V_ADD_OUT);
  }
 #undef FIX_OUT
}

void xvid_colorspace_init(
      double rgb_y_out,
      double b_u_out,
      double g_u_out,
      double g_v_out,
      double r_v_out,
      int    y_add_out)
{
 /* Initialize internal colorspace transformation tables */
 colorspace_init< YUV_RGB_DATA<CCIR> >(rgb_y_out, b_u_out, g_u_out, g_v_out, r_v_out, y_add_out);
 colorspace_init< YUV_RGB_DATA<JPEG> >(rgb_y_out, b_u_out, g_u_out, g_v_out, r_v_out, y_add_out);

 /* All colorspace transformation functions User Format->YV12 */
 yv12_to_yv12    = yv12_to_yv12_c;
 TpackedFuncPtrRGB<CCIR>::init();
 TpackedFuncPtrRGB<JPEG>::init();

 yuyv_to_yv12 = yuyv_to_yv12_c;
 uyvy_to_yv12 = uyvy_to_yv12_c;

 yuyvi_to_yv12   = yuyvi_to_yv12_c;
 uyvyi_to_yv12   = uyvyi_to_yv12_c;


 /* All colorspace transformation functions YV12->User format */
 yv12_to_yuyv = yv12_to_yuyv_c;
 yv12_to_uyvy = yv12_to_uyvy_c;

 yv12_to_yuyvi   = yv12_to_yuyvi_c;
 yv12_to_uyvyi   = yv12_to_uyvyi_c;
#if 1
 if (Tconfig::cpu_flags & FF_CPU_MMX)
  {
   // image input xxx_to_yv12 related functions
   yv12_to_yv12  = &yv12_to_yv12_simd<Tmmx>;// yv12_to_yv12_mmx;

   TpackedFuncPtrRGB<CCIR>::initMMX();
   TpackedFuncPtrRGB<JPEG>::initMMX();

   yuyv_to_yv12 = yuyv_to_yv12_mmx;
   uyvy_to_yv12 = uyvy_to_yv12_mmx;

   // image output yv12_to_xxx related functions
   yv12_to_yuyv = yv12_to_yuyv_mmx;
   yv12_to_uyvy = yv12_to_uyvy_mmx;

   yv12_to_yuyvi = yv12_to_yuyvi_mmx;
   yv12_to_uyvyi = yv12_to_uyvyi_mmx;
  }

 // these 3dnow functions are faster than mmx, but slower than xmm.
 /*
 if (Tconfig::cpu_flags & FF_CPU_3DNOW)
  {
   yuyv_to_yv12  = yuyv_to_yv12_3dn;
   uyvy_to_yv12  = uyvy_to_yv12_3dn;
  }
 */

 if (Tconfig::cpu_flags & FF_CPU_MMXEXT)
  {
   // Colorspace transformation
   yv12_to_yv12  = &yv12_to_yv12_simd<Tmmxext>;//yv12_to_yv12_xmm;
   yuyv_to_yv12  = yuyv_to_yv12_xmm;
   uyvy_to_yv12  = uyvy_to_yv12_xmm;
  }
#endif
}
