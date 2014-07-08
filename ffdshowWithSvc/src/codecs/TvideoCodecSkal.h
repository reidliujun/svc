#ifndef _TVIDEOCODECSKAL_H_
#define _TVIDEOCODECSKAL_H_

#include "TvideoCodec.h"
#include "skal/skl_mpg4_c.h"

class Tdll;
class TvideoCodecSkal :public TvideoCodecEnc
{
private:
 Tdll *dll;
 SKL_MP4_ENC*       (*Skl_MP4_New_Encoder)(void);
 void               (*Skl_MP4_Delete_Encoder)(SKL_MP4_ENC *);
 const SKL_MP4_PIC* (*Skl_MP4_Enc_Prepare_Next_Frame)(SKL_MP4_ENC * const Enc, int Width, int Height);
 const SKL_MP4_PIC* (*Skl_MP4_Enc_Get_Next_Frame)(const SKL_MP4_ENC * const Enc);
 const SKL_MP4_PIC* (*Skl_MP4_Enc_Get_Last_Coded_Frame)(const SKL_MP4_ENC * const Enc);
 int                (*Skl_MP4_Enc_Encode)(SKL_MP4_ENC * const Enc);
 int                (*Skl_MP4_Enc_Finish_Encoding)(SKL_MP4_ENC * const Enc);
 const char *       (*Skl_MP4_Enc_Get_Bits)(const SKL_MP4_ENC * const Enc);
 int                (*Skl_MP4_Enc_Get_Bits_Length)(const SKL_MP4_ENC * const Enc);
 void               (*Skl_MP4_Enc_Set_CPU)(SKL_MP4_ENC * const Enc, SKL_CPU_FEATURE Cpu);
 void               (*Skl_MP4_Enc_Set_Custom_Matrix)(SKL_MP4_ENC * const Enc, int Intra, const uint8_t *M);
 void               (*Skl_MP4_Enc_Set_Slicer)(SKL_MP4_ENC * const Enc, SKL_MP4_SLICER Slicer, void *Slicer_Data);
 void               (*Skl_MP4_Enc_Get_All_Frames)(const SKL_MP4_ENC * const Enc, SKL_MP4_PIC *Pic);
 int                (*Skl_MP4_Enc_Ioctl)(SKL_MP4_ENC * const Enc, const char * const Param);
 int                (*Skl_MP4_Enc_Set_Analyzer_Param_I)(const SKL_MP4_ENC * const Enc, const char * const Param, int Value);
 int                (*Skl_MP4_Enc_Get_Analyzer_Param_I)(const SKL_MP4_ENC * const Enc, const char * const Param, int *Value);
 int                (*Skl_MP4_Enc_Set_Analyzer_Param_F)(const SKL_MP4_ENC * const Enc, const char * const Param, float Value);
 int                (*Skl_MP4_Enc_Get_Analyzer_Param_F)(const SKL_MP4_ENC * const Enc, const char * const Param, float *Value);
 int                (*Skl_MP4_Enc_Set_Analyzer_Param_S)(const SKL_MP4_ENC * const Enc, const char * const Param, const char * const Value);
 int                (*Skl_MP4_Enc_Get_Analyzer_Param_S)(const SKL_MP4_ENC * const Enc, const char * const Param, const char ** const Value);
 const int *        (*Skl_MP4_Enc_Get_Analyzer_Param_P)(const SKL_MP4_ENC * const Enc, const char * const Param);
public:
 TvideoCodecSkal(IffdshowBase *Ideci,IencVideoSink *IsinkE);
 virtual ~TvideoCodecSkal();
 static const char_t *dllname;
 static bool getVersion(const Tconfig *config,ffstring &vers,ffstring &license);
private:
 SKL_MP4_ENC *Enc;
 static void Sliced_PSNR(const SKL_MP4_PIC *Pic, int yo, int Height, void* Data);
 struct PSNR_INFOS
  {
   PSNR_INFOS(void);
   ~PSNR_INFOS();

   size_t Size;
   uint8_t *Y, *U, *V;

   static uint32_t SSD_16x16(const uint8_t *Dst, const uint8_t *Src, int32_t BpS);
   static uint32_t SSD_8x8(const uint8_t *Dst, const uint8_t *Src, int32_t BpS);

   void Store(uint8_t *Dst, const uint8_t *Src, int W, int H, int BpS);
   uint32_t Get_SSE(uint8_t *Ref, const uint8_t *Src, int W, int H, int BpS);
   void Check(int W, int H, int BpS);
   void Clear(void);

   TencFrameParams *params;
   void setParams(TencFrameParams *Iparams) {params=Iparams;}
  } PSNR_Infos;
protected:
 virtual LRESULT beginCompress(int cfgcomode,int csp,const Trect &r);
public:
 virtual int getType(void) const {return IDFF_MOVIE_SKAL;}
 virtual HRESULT compress(const TffPict &pict,TencFrameParams &params);
 virtual void end(void);
};

#endif
