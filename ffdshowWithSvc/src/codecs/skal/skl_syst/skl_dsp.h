/********************************************************
 * Some code. Copyright (C) 2003 by Pascal Massimino.   *
 * All Rights Reserved.      (http://skal.planet-d.net) *
 * For Educational/Academic use ONLY. See 'LICENSE.TXT'.*
 ********************************************************/
/*
 * skl_dsp.h
 *
 * Low-level data processing
 ********************************************************/

#ifndef _SKL_DSP_H_
#define _SKL_DSP_H_

#include "skl_syst/skl_cpu_specs.h"

extern "C" {

//////////////////////////////////////////////////////////
// SKL_YUV_DSP : YUV conversion operations
//////////////////////////////////////////////////////////

typedef void (*SKL_TO_YUV_FUNC)(SKL_BYTE *Y, SKL_BYTE *U, SKL_BYTE *V,
                                const SKL_INT32 Dst_BpS,
                                const SKL_BYTE *Src,
                                const SKL_INT32 Src_BpS,
                                const SKL_INT32 Width, const SKL_INT32 Height);
typedef void (*SKL_TO_RGB_FUNC)(SKL_BYTE *RGB,
                                const SKL_INT32 Dst_BpS,
                                const SKL_BYTE *Y, const SKL_BYTE *U, const SKL_BYTE *V,
                                const SKL_INT32 Src_BpS,
                                const SKL_INT32 Width, const SKL_INT32 Height);
struct SKL_YUV_DSP
{
  const char *Name;
  SKL_DSP_SWITCH Switch_Off;

  int (*Init)(int Transfer_Type); // type=[1..7]. default=1 (ITU-R Rec. 709 (1990))
  SKL_TO_YUV_FUNC RGB565_TO_YUV;
  SKL_TO_RGB_FUNC YUV_TO_RGB565;
  SKL_TO_YUV_FUNC RGB24_TO_YUV;
  SKL_TO_RGB_FUNC YUV_TO_RGB24;
  SKL_TO_YUV_FUNC RGB32_TO_YUV;
  SKL_TO_RGB_FUNC YUV_TO_RGB32;
};

extern SKL_EXPORT int Skl_Init_YUV_DSP(SKL_YUV_DSP *Dsp, SKL_CPU_FEATURE Cpu = SKL_CPU_DETECT );

//////////////////////////////////////////////////////////
// SKL_IMG_DSP : Image processing
//////////////////////////////////////////////////////////

struct SKL_IMG_DSP
{
  const char *Name;
  SKL_DSP_SWITCH Switch_Off;

    // Note: Src1 is supposed to be 16b-aligned in SAD_*!! (for SSE2)
  SKL_UINT32 (*SAD_4x4  )(const SKL_BYTE *Src1, const SKL_BYTE *Src2, SKL_INT32 BpS);
  SKL_UINT32 (*SAD_8x8  )(const SKL_BYTE *Src1, const SKL_BYTE *Src2, SKL_INT32 BpS);
  SKL_UINT32 (*SAD_16x8_Field)(const SKL_BYTE *Src1, const SKL_BYTE *Src2, SKL_INT32 BpS);
  SKL_UINT32 (*SAD_16x16)(const SKL_BYTE *Src1, const SKL_BYTE *Src2, SKL_INT32 BpS);
  SKL_UINT32 (*SAD_16x7_Self)(const SKL_BYTE *Src, SKL_INT32 BpS);

  SKL_UINT32 (*SSD_4x4  )(const SKL_BYTE *Src1, const SKL_BYTE *Src2, SKL_INT32 BpS);
  SKL_UINT32 (*SSD_8x8  )(const SKL_BYTE *Src1, const SKL_BYTE *Src2, SKL_INT32 BpS);
  SKL_UINT32 (*SSD_16x8_Field)(const SKL_BYTE *Src1, const SKL_BYTE *Src2, SKL_INT32 BpS);
  SKL_UINT32 (*SSD_16x16)(const SKL_BYTE *Src1, const SKL_BYTE *Src2, SKL_INT32 BpS);

  SKL_UINT32 (*Hadamard_SAD_4x4  )(const SKL_BYTE *Src1, const SKL_BYTE *Src2, SKL_INT32 BpS);
  SKL_UINT32 (*Hadamard_SAD_8x8  )(const SKL_BYTE *Src1, const SKL_BYTE *Src2, SKL_INT32 BpS);
  SKL_UINT32 (*Hadamard_SAD_16x8_Field)(const SKL_BYTE *Src1, const SKL_BYTE *Src2, SKL_INT32 BpS);
  SKL_UINT32 (*Hadamard_SAD_16x16)(const SKL_BYTE *Src1, const SKL_BYTE *Src2, SKL_INT32 BpS);

  SKL_UINT32 (*Mean_4x4)(const SKL_BYTE *Src, SKL_INT32 BpS);
  SKL_UINT32 (*Mean_8x8)(const SKL_BYTE *Src, SKL_INT32 BpS);
  SKL_UINT32 (*Mean_16x16)(const SKL_BYTE *Src, SKL_INT32 BpS);

  SKL_UINT32 (*Sqr_4x4)(const SKL_BYTE *Src, SKL_INT32 BpS);
  SKL_UINT32 (*Sqr_8x8)(const SKL_BYTE *Src, SKL_INT32 BpS);
  SKL_UINT32 (*Sqr_16x16)(const SKL_BYTE *Src, SKL_INT32 BpS);

  SKL_UINT32 (*Sqr_Dev_16x16)(const SKL_BYTE *Src, SKL_INT32 BpS);
  SKL_UINT32 (*Abs_Dev_16x16)(const SKL_BYTE *Src, SKL_INT32 BpS);
  SKL_UINT32 (*Hadamard_Dev_16x16)(const SKL_BYTE *Src, SKL_INT32 BpS);

  SKL_UINT32 (*SAD)(const SKL_BYTE *Src1, const SKL_BYTE *Src2, SKL_INT32 W, SKL_INT32 H, SKL_INT32 BpS);
  SKL_UINT32 (*Mean)(const SKL_BYTE *Src, SKL_INT32 W, SKL_INT32 H, SKL_INT32 BpS);
  SKL_UINT32 (*Square_Dev)(const SKL_BYTE *Src, SKL_INT32 W, SKL_INT32 H, SKL_INT32 BpS);
  SKL_UINT32 (*Abs_Dev)(const SKL_BYTE *Src, SKL_INT32 W, SKL_INT32 H, SKL_INT32 BpS);

  SKL_UINT32 (*SAD_Avrg_4x4  )(const SKL_BYTE *Dst, const SKL_BYTE *Src1, const SKL_BYTE *Src2, SKL_INT32 BpS);
  SKL_UINT32 (*SAD_Avrg_8x8  )(const SKL_BYTE *Dst, const SKL_BYTE *Src1, const SKL_BYTE *Src2, SKL_INT32 BpS);
  SKL_UINT32 (*SAD_Avrg_16x8 )(const SKL_BYTE *Dst, const SKL_BYTE *Src1, const SKL_BYTE *Src2, SKL_INT32 BpS);
  SKL_UINT32 (*SAD_Avrg_16x16)(const SKL_BYTE *Dst, const SKL_BYTE *Src1, const SKL_BYTE *Src2, SKL_INT32 BpS);

  void (*Smooth_18x18_To_8x8)(SKL_BYTE *Dst, int Dst_BpS, const SKL_BYTE *Src, int Src_BpS);
  void (*Gradx_18x18_To_8x8) (SKL_INT8 *Dst, int Dst_BpS, const SKL_BYTE *Src, int Src_BpS);
  void (*Grady_18x18_To_8x8) (SKL_INT8 *Dst, int Dst_BpS, const SKL_BYTE *Src, int Src_BpS);
  void (*Grad2_18x18_To_8x8) (SKL_BYTE *Dst, int Dst_BpS, const SKL_BYTE *Src, int Src_BpS);
};

extern int Skl_Init_Img_DSP(SKL_IMG_DSP *Dsp, SKL_CPU_FEATURE Cpu = SKL_CPU_DETECT);

//////////////////////////////////////////////////////////
// SKL_QUANT_DSP : various quantization/dequantization
//////////////////////////////////////////////////////////

// M[2][31][64] is the common workspace for all implementation. Hope
// it's enough so we won't go OO.
//   Indexes: M[ 0=Quant/1=Dequant ][ Q=0..31 ][ 0=Mult/1=Bias ][ i=0..64 ]

typedef struct SKL_QUANT_DSP SKL_QUANT_DSP;

typedef SKL_INT16 (*SKL_QUANTIZER)[31][2][64];

    // for comfort, let's define some common signatures
typedef void (*SKL_QUANT_INTRA_FUNC)(SKL_INT16 *Out, const SKL_INT16 *In,
                                     const SKL_QUANTIZER M,
                                     SKL_INT32 Scale, SKL_INT32 DC_Scale);
typedef SKL_INT32 (*SKL_QUANT_INTER_FUNC)(SKL_INT16 *Out, const SKL_INT16 *In,
                                          const SKL_QUANTIZER M,
                                          SKL_INT32 Scale);
typedef void (*SKL_DEQUANT_INTRA_FUNC)(SKL_INT16 *Out, const SKL_INT16 *In,
                                       const SKL_QUANTIZER M,
                                       SKL_INT32 Scale, SKL_INT32 DC_Scale);
typedef void (*SKL_DEQUANT_INTER_FUNC)(SKL_INT16 *Out, const SKL_INT16 *In,
                                       const SKL_QUANTIZER M,
                                       SKL_INT32 Scale, int Rows);

struct SKL_QUANT_DSP
{
  const char *Name;
  SKL_DSP_SWITCH Switch_Off;

  void (*Init_Quantizer)(SKL_QUANTIZER Q, const SKL_BYTE In[64],
                         const SKL_BYTE *Scale_Map, int For_Intra);

  SKL_QUANT_INTRA_FUNC   Quant_Intra;
  SKL_QUANT_INTER_FUNC   Quant_Inter;

  SKL_DEQUANT_INTRA_FUNC Dequant_Intra;
  SKL_DEQUANT_INTER_FUNC Dequant_Inter;

  void (*Zero)(SKL_INT16 C[64]); // resets coeffs to zero
  void (*Zero16)(SKL_INT16 C[16]); // resets coeffs to zero

  void (*Dct)(SKL_INT16 *);
  void (*IDct)(SKL_INT16 *);

  void (*IDct_Sparse)(SKL_INT16 *);
  void (*IDct_Put)(SKL_INT16 *, SKL_BYTE *Dst, int BpS);
  void (*IDct_Add)(SKL_INT16 *, SKL_BYTE *Dst, int BpS);

  void (*IDct_Sparse_8x4)(SKL_INT16 *);
  void (*IDct_Put_8x4)(SKL_INT16 *, SKL_BYTE *Dst, int BpS);
  void (*IDct_Add_8x4)(SKL_INT16 *, SKL_BYTE *Dst, int BpS);
};

extern int Skl_Init_Quant_DSP(SKL_QUANT_DSP *Dsp,
                              SKL_CPU_FEATURE Cpu = SKL_CPU_DETECT,
                              int Quant_Type=1  /* 0=H263, 1=MPEG4, 2=MPEG2 */ );

//////////////////////////////////////////////////////////
// SKL_MB_DSP : Macro-block operations
//////////////////////////////////////////////////////////

typedef void (*SKL_MB_FUNC)(SKL_BYTE *Dst,
                            const SKL_BYTE *Src,
                            const int BpS);

struct SKL_HV_FILTER      // horizontal/vertical filter of a 16x16 block
{
  void (*H_Pass)(SKL_BYTE *Dst, const SKL_BYTE *Src, const int BpS);
  void (*V_Pass)(SKL_BYTE *Dst, const SKL_BYTE *Src, const int BpS);
  void (*HV_Pass)(SKL_BYTE *Dst, const SKL_BYTE *Src, const int BpS);
};
  // some pre-defined averaging filters (2Taps).
extern const SKL_HV_FILTER Skl_Filter_2_C, Skl_Filter_2_MMX, Skl_Filter_2_SSE;


#define SKL_MB_QP_PASS_SIGNATURE(NAME)  \
  void (NAME)(SKL_BYTE *Dst, const SKL_BYTE *Src, int H, int BpS)

    //   Half-pixel motion of 16x8 and 8x8 blocks
    // Index [0..3] is computed with: i=(dx&1) | ((dy&1)<<1)
    // where dx/dy is the motion vector in 1/2 pel unit.
    // [0..3] <=> [Full/Full]  [Full/Half]  [Half/Full]  [Half/Half]
    // We put everything in a single struct so it can easily be passed
    // to prediction functions as a whole...

struct SKL_MB_FUNCS
{
  SKL_MB_FUNC    HP_16x8 [4]; // 16x8  hp-prediction
  SKL_MB_FUNC    HP_8x8  [4]; // 8x8   hp-prediction
  SKL_MB_FUNC    HP_8x4  [4]; // 8x4   hp-prediction (for fields)

    // filter for QPel 16x? MPEG4 prediction

  void (*H_Pass)(SKL_BYTE *Dst, const SKL_BYTE *Src, int H, int BpS);
  void (*H_Pass_Avrg)(SKL_BYTE *Dst, const SKL_BYTE *Src, int H, int BpS);
  void (*H_Pass_Avrg_Up)(SKL_BYTE *Dst, const SKL_BYTE *Src, int H, int BpS);
  void (*H_LowPass)(SKL_BYTE *Dst, const SKL_BYTE *Src, int H, int BpS);
  void (*H_LowPass_Avrg)(SKL_BYTE *Dst, const SKL_BYTE *Src, int H, int BpS);
  void (*H_LowPass_Avrg_Up)(SKL_BYTE *Dst, const SKL_BYTE *Src, int H, int BpS);
  void (*V_Pass)(SKL_BYTE *Dst, const SKL_BYTE *Src, int W, int BpS);
  void (*V_Pass_Avrg)(SKL_BYTE *Dst, const SKL_BYTE *Src, int H, int BpS);
  void (*V_Pass_Avrg_Up)(SKL_BYTE *Dst, const SKL_BYTE *Src, int H, int BpS);

    // filter for QPel 8x? MPEG4 prediction

  void (*H_Pass_8)(SKL_BYTE *Dst, const SKL_BYTE *Src, int H, int BpS);
  void (*H_Pass_Avrg_8)(SKL_BYTE *Dst, const SKL_BYTE *Src, int H, int BpS);
  void (*H_Pass_Avrg_Up_8)(SKL_BYTE *Dst, const SKL_BYTE *Src, int H, int BpS);
  void (*H_LowPass_8)(SKL_BYTE *Dst, const SKL_BYTE *Src, int H, int BpS);
  void (*H_LowPass_Avrg_8)(SKL_BYTE *Dst, const SKL_BYTE *Src, int H, int BpS);
  void (*H_LowPass_Avrg_Up_8)(SKL_BYTE *Dst, const SKL_BYTE *Src, int H, int BpS);
  void (*V_Pass_8)(SKL_BYTE *Dst, const SKL_BYTE *Src, int W, int BpS);
  void (*V_Pass_Avrg_8)(SKL_BYTE *Dst, const SKL_BYTE *Src, int H, int BpS);
  void (*V_Pass_Avrg_Up_8)(SKL_BYTE *Dst, const SKL_BYTE *Src, int H, int BpS);

  void (*SAD_HP_16x16)(const SKL_BYTE *Cur, const SKL_BYTE *Src, int BpS, SKL_UINT32 Sad[3]);
  void (*SAD_HP_8x8)(const SKL_BYTE *Cur, const SKL_BYTE *Src, int BpS, SKL_UINT32 Sad[3]);
};

//////////////////////////////////////////////////////////

struct SKL_MB_DSP
{
  const char *Name;
  SKL_DSP_SWITCH Switch_Off;

  void (*Init)();

    // 16b->8b transfer    (*Src is actually Src[][8][8])
  void (*Copy_Upsampled_8x8_16To8)(SKL_BYTE *Dst, const SKL_INT16 *Src, const int BpS);
  void (*Add_Upsampled_8x8_16To8)(SKL_BYTE *Dst, const SKL_INT16 *Src, const int BpS);

    // 8b->16b transfer
  void (*Copy_16x8_8To16)(SKL_INT16 *Dst, const SKL_BYTE *Src, const int BpS);
  void (*Copy_8x8_8To16) (SKL_INT16 *Dst, const SKL_BYTE *Src, const int BpS);
  void (*Diff_16x8_8To16)(SKL_INT16 *Dst, const SKL_BYTE *Src, const int BpS);
  void (*Diff_8x8_8To16) (SKL_INT16 *Dst, const SKL_BYTE *Src, const int BpS);

  void (*Diff_16x8_88To16)(SKL_INT16 *Dst, const SKL_BYTE *Src1, const SKL_BYTE *Src2, const int BpS);
  void (*Diff_8x8_88To16) (SKL_INT16 *Dst, const SKL_BYTE *Src1, const SKL_BYTE *Src2, const int BpS);

    // 8b->8b transfer

  const SKL_MB_FUNCS *Copy[2]; // index = rounding (0/1)
  const SKL_MB_FUNCS  *Add;    // Rounding is always 0 for addition (B-frames)

  SKL_UINT32 (*SAD_16x7_Frame)(const SKL_INT16 *Src); // self SAD on DCT coeffs
  SKL_UINT32 (*SAD_16x7_Field)(const SKL_INT16 *Src); // Src[4*64]
  void (*Reorder_Frame_16x16)(SKL_INT16 *Src);        // frame to field

    // 8b utilitary functions
        /* replicate 16/8 edge pixels of YUV2 picture */
  void (*Make_Edges)(SKL_BYTE * const * const YUV, const int Width, const int Height, const int BpS);
        /* horizontal/vertical (3,1)-Filter of 8-pixels blocks */
  void (*HFilter_31)(SKL_BYTE *Src1, SKL_BYTE *Src2, int Nb_Blks);
  void (*VFilter_31)(SKL_BYTE *Src1, SKL_BYTE *Src2, const int BpS, int Nb_Blks);

    // 8b->16b 18x18 -> 8x8 downsampling
  void (*Filter_18x18_To_8x8)(SKL_INT16 *Dst, const SKL_BYTE *Src, const int BpS);
  void (*Filter_Diff_18x18_To_8x8)(SKL_INT16 *Dst, const SKL_BYTE *Src, const int BpS);

  const SKL_HV_FILTER *Filter;
};

extern int Skl_Init_Mb_DSP(SKL_MB_DSP *Dsp, SKL_CPU_FEATURE Cpu = SKL_CPU_DETECT);

//////////////////////////////////////////////////////////
// Global Motion Compensation stuff

struct SKL_GMC_DSP
{
  const char *Name;
  SKL_DSP_SWITCH Switch_Off;
  void (*Predict_16x16)(const SKL_GMC_DSP * const This,
                        SKL_BYTE *Dst, const SKL_BYTE *Src, int BpS,
                        int x, int y, int Rounder);
  void (*Predict_8x8)(const SKL_GMC_DSP * const This,
                      SKL_BYTE *uDst, const SKL_BYTE *uSrc, SKL_SAFE_INT uv_Coloc,
                      int BpS, int x, int y, int Rounder);

  int Width, Height;
  int Nb_Pts;
  int Accuracy;

  int dU[2];    // gradient-converted warp points
  int dV[2];
  int dw[2];
  int Uo, Vo, Uco, Vco;

  void Setup(int Width, int Height, const int MVs[][2], int Nb, int Accuracy);
  void Get_Average_MV(int MV[2], int x, int y, int QPel) const;
};

extern int Skl_Init_GMC_DSP(SKL_GMC_DSP *Dsp, SKL_CPU_FEATURE Cpu = SKL_CPU_DETECT);

//////////////////////////////////////////////////////////
// useful table for clipping pixels.
// MPEG4's 6-tap FIR output values in [-112,367]
// H264's  6-tap FIR output values in [-80,335]
// B-VOP like mixing requires range   [-128,384]
// H264's hadamard transform ranges   [-512,768]

#define SKL_CLIP_MIN  (-512)
#define SKL_CLIP_MAX  ( 768)
extern "C" SKL_BYTE Skl_DSP_Clip[SKL_CLIP_MAX-SKL_CLIP_MIN];
extern "C" void Skl_Init_DSP_Clip();

#ifndef NDEBUG
#define SKL_DSP_CLIP(x) (SKL_ASSERT((x)>=SKL_CLIP_MIN && (x)<SKL_CLIP_MAX), Skl_DSP_Clip[(x)-SKL_CLIP_MIN])
#else
#define SKL_DSP_CLIP(x) (Skl_DSP_Clip[(x)-SKL_CLIP_MIN])
#endif

//////////////////////////////////////////////////////////
// General-purpose other function

  // In[Size], Out[2*Size]
extern void Skl_Generic_IDct_Ref(int Size, const float *In, float *Out);

//////////////////////////////////////////////////////////

} /* extern "C" */

#endif  /* _SKL_DSP_H_ */
