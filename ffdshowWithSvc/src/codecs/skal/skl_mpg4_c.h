/********************************************************
 * Some code. Copyright (C) 2003 by Pascal Massimino.   *
 * All Rights Reserved.      (http://skal.planet-d.net) *
 * For Educational/Academic use ONLY. See 'LICENSE.TXT'.*
 ********************************************************/
/*
 * skl_mpg4_c.h
 *
 *   Plain-C main MPEG4 standalone header
 ************************************************/

#ifndef _SKL_MPG4_C_H_
#define _SKL_MPG4_C_H_

//#include <stdlib.h>

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

#ifndef SKL_C_API_ONLY

/** @file */
//////////////////////////////////////////////////////////
// Some special types and #defines
//////////////////////////////////////////////////////////

typedef short SKL_MV[2];

typedef struct SKL_MP4_ENC SKL_MP4_ENC;
typedef struct SKL_MP4_DEC SKL_MP4_DEC;
typedef struct SKL_MP4_PIC SKL_MP4_PIC;         // visible YUV picture
typedef struct SKL_MP4_MAP SKL_MP4_MAP;         // opaque

typedef struct SKL_YUV_DSP SKL_YUV_DSP;

#ifdef _WINDOWS
typedef __int64 SKL_INT64;
typedef unsigned __int64 SKL_UINT64;
#define SKL_EXPORT __declspec(dllexport)
#else
typedef long long int SKL_INT64;
typedef unsigned long long int SKL_UINT64;
#define SKL_EXPORT
#endif

//////////////////////////////////////////////////////////
// CPU features
//////////////////////////////////////////////////////////

typedef enum {

  SKL_CPU_C    = 0,
  SKL_CPU_X86  = 1,
  SKL_CPU_MMX  = 2,
  SKL_CPU_SSE  = 3,
  SKL_CPU_SSE2 = 4,
  SKL_CPU_REF  = 5,
  SKL_CPU_ALT  = 6,
  SKL_CPU_LAST = 7,

  SKL_CPU_DETECT = 7

} SKL_CPU_FEATURE;


//////////////////////////////////////////////////////////
// SKL_MP4_SLICER

  /* @see SKL_MP4_DEC, SKL_MP4_ENC */
typedef void (*SKL_MP4_SLICER)(const void *Pic, int y, int Height, void *Data);

//////////////////////////////////////////////////////////
// SKL_MP4_PIC

  /* C-wrapper for C++ API. @see SKL_MP4_DEC, SKL_MP4_ENC */
struct SKL_MP4_PIC
{
  unsigned char *Y;
  unsigned char *U;
  unsigned char *V;

  int Coding;
  int Width;
  int Height;
  int BpS;
  SKL_MV   *MV;
  double Time;
  SKL_UINT64 Time_Ticks;
  SKL_MP4_MAP *Map;
  void        *Data;
};

    /* proxies */

typedef SKL_MP4_ENC *(*SKL_MP4_NEW_ENC)();
typedef void (*SKL_MP4_DELETE_ENC)(SKL_MP4_ENC *);
typedef SKL_MP4_DEC *(*SKL_MP4_NEW_DEC)();
typedef void (*SKL_MP4_DELETE_DEC)(SKL_MP4_DEC *);

extern SKL_EXPORT SKL_MP4_ENC *Skl_MP4_New_Encoder();
extern SKL_EXPORT void Skl_MP4_Delete_Encoder(SKL_MP4_ENC *);

extern SKL_EXPORT SKL_MP4_DEC *Skl_MP4_New_Decoder();
extern SKL_EXPORT void Skl_MP4_Delete_Decoder(SKL_MP4_DEC *);

#endif    /* SKL_C_API_ONLY */

//////////////////////////////////////////////////////////
// C-interface for SKL_MP4_DEC (decoder class)
//////////////////////////////////////////////////////////

extern SKL_EXPORT int  Skl_MP4_Dec_Decode(SKL_MP4_DEC * const Dec, const char *Buf, int Len);        /**< C-wrapper for C++ API. @see SKL_MP4_DEC */
extern SKL_EXPORT int  Skl_MP4_Dec_Decode_MPEG12(SKL_MP4_DEC * const Dec, const char *Buf, int Len); /**< C-wrapper for C++ API. @see SKL_MP4_DEC */
extern SKL_EXPORT int  Skl_MP4_Dec_Get_Frame_Number(const SKL_MP4_DEC * const Dec);                  /**< C-wrapper for C++ API. @see SKL_MP4_DEC */
extern SKL_EXPORT int  Skl_MP4_Dec_Is_Frame_Ready(const SKL_MP4_DEC * const Dec);                    /**< C-wrapper for C++ API. @see SKL_MP4_DEC */
extern SKL_EXPORT void Skl_MP4_Dec_Consume_Frame(SKL_MP4_DEC * const Dec, SKL_MP4_PIC *Pic);         /**< C-wrapper for C++ API. @see SKL_MP4_DEC */
extern SKL_EXPORT void Skl_MP4_Dec_Set_CPU(SKL_MP4_DEC * const Dec, SKL_CPU_FEATURE Cpu);            /**< C-wrapper for C++ API. @see SKL_MP4_DEC */
extern SKL_EXPORT void Skl_MP4_Dec_Set_Slicer(SKL_MP4_DEC * const Dec, SKL_MP4_SLICER Slicer);       /**< C-wrapper for C++ API. @see SKL_MP4_DEC */

//////////////////////////////////////////////////////////
//  C-interface for SKL_MP4_ENC (encoder class)
//////////////////////////////////////////////////////////

extern SKL_EXPORT const SKL_MP4_PIC *Skl_MP4_Enc_Prepare_Next_Frame(SKL_MP4_ENC * const Enc, int Width, int Height);                       /**< C-wrapper for C++ API. @see SKL_MP4_ENC */
extern SKL_EXPORT const SKL_MP4_PIC *Skl_MP4_Enc_Get_Next_Frame(const SKL_MP4_ENC * const Enc);                                            /**< C-wrapper for C++ API. @see SKL_MP4_ENC */
extern SKL_EXPORT const SKL_MP4_PIC *Skl_MP4_Enc_Get_Last_Coded_Frame(const SKL_MP4_ENC * const Enc);                                      /**< C-wrapper for C++ API. @see SKL_MP4_ENC */
extern SKL_EXPORT int                Skl_MP4_Enc_Encode(SKL_MP4_ENC * const Enc);                                                          /**< C-wrapper for C++ API. @see SKL_MP4_ENC */
extern SKL_EXPORT int                Skl_MP4_Enc_Finish_Encoding(SKL_MP4_ENC * const Enc);                                                 /**< C-wrapper for C++ API. @see SKL_MP4_ENC */
extern SKL_EXPORT const char *       Skl_MP4_Enc_Get_Bits(const SKL_MP4_ENC * const Enc);                                                  /**< C-wrapper for C++ API. @see SKL_MP4_ENC */
extern SKL_EXPORT int                Skl_MP4_Enc_Get_Bits_Length(const SKL_MP4_ENC * const Enc);                                           /**< C-wrapper for C++ API. @see SKL_MP4_ENC */
extern SKL_EXPORT void               Skl_MP4_Enc_Set_CPU(SKL_MP4_ENC * const Enc, SKL_CPU_FEATURE Cpu);                                    /**< C-wrapper for C++ API. @see SKL_MP4_ENC */
extern SKL_EXPORT void               Skl_MP4_Enc_Set_Custom_Matrix(SKL_MP4_ENC * const Enc, int Intra, const char *M);                     /**< C-wrapper for C++ API. @see SKL_MP4_ENC */
extern SKL_EXPORT void               Skl_MP4_Enc_Set_Slicer(SKL_MP4_ENC * const Enc, SKL_MP4_SLICER Slicer, void *Slicer_Data);            /**< C-wrapper for C++ API. @see SKL_MP4_ENC */
extern SKL_EXPORT void               Skl_MP4_Enc_Get_All_Frames(const SKL_MP4_ENC * const Enc, SKL_MP4_PIC *Pic);                          /**< C-wrapper for C++ API. @see SKL_MP4_ENC */
extern SKL_EXPORT int                Skl_MP4_Enc_Ioctl(SKL_MP4_ENC * const Enc, const char * const Param);                                 /**< C-wrapper for C++ API. @see SKL_MP4_ENC */
extern SKL_EXPORT int                Skl_MP4_Enc_Set_Analyzer_Param_I(const SKL_MP4_ENC * const Enc, const char * const Param, int Value);     /**< C-wrapper for C++ API. @see SKL_MP4_ENC */
extern SKL_EXPORT int                Skl_MP4_Enc_Get_Analyzer_Param_I(const SKL_MP4_ENC * const Enc, const char * const Param, int *Value);    /**< C-wrapper for C++ API. @see SKL_MP4_ENC */
extern SKL_EXPORT int                Skl_MP4_Enc_Set_Analyzer_Param_F(const SKL_MP4_ENC * const Enc, const char * const Param, float Value);   /**< C-wrapper for C++ API. @see SKL_MP4_ENC */
extern SKL_EXPORT int                Skl_MP4_Enc_Get_Analyzer_Param_F(const SKL_MP4_ENC * const Enc, const char * const Param, float *Value);  /**< C-wrapper for C++ API. @see SKL_MP4_ENC */
extern SKL_EXPORT int                Skl_MP4_Enc_Set_Analyzer_Param_S(const SKL_MP4_ENC * const Enc, const char * const Param, const char * const Value);   /**< C-wrapper for C++ API. @see SKL_MP4_ENC */
extern SKL_EXPORT int                Skl_MP4_Enc_Get_Analyzer_Param_S(const SKL_MP4_ENC * const Enc, const char * const Param, const char ** const Value);  /**< C-wrapper for C++ API. @see SKL_MP4_ENC */
extern SKL_EXPORT const int *        Skl_MP4_Enc_Get_Analyzer_Param_P(const SKL_MP4_ENC * const Enc, const char * const Param);            /**< C-wrapper for C++ API. @see SKL_MP4_ENC */

//////////////////////////////////////////////////////////
// SKL_YUV_DSP : YUV<->RGB conversion operations
//////////////////////////////////////////////////////////

#ifndef SKL_C_API_ONLY

typedef void (*SKL_DSP_SWITCH)();
typedef void (*SKL_TO_RGB_FUNC)(unsigned char *RGB,
                                const int Dst_BpS,
                                const unsigned char *Y,
                                const unsigned char *U,
                                const unsigned char *V,
                                const int Src_BpS,
                                const int Width, const int Height);
typedef void (*SKL_TO_YUV_FUNC)(unsigned char *Y, unsigned char *U, unsigned char *V,
                                const int Dst_BpS,
                                const unsigned char *Src,
                                const int Src_BpS,
                                const int Width, const int Height);

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

#endif

extern SKL_EXPORT int Skl_Init_YUV_DSP(SKL_YUV_DSP *Dsp, SKL_CPU_FEATURE Cpu /* = SKL_CPU_DETECT */ );

//////////////////////////////////////////////////////////

#if defined(__cplusplus) || defined(c_plusplus)
}     /* extern "C" */
#endif

#endif   /* _SKL_MPG4_C_H_ */
