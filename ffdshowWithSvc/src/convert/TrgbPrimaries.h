#ifndef _TRGBPRIMARIES_H_
#define _TRGBPRIMARIES_H_

#include "interfaces.h"

struct ToutputVideoSettings;
typedef struct 
{
 int64_t cybgr_64;
 int64_t fpix_mul;
 int fraction;
 int y1y2_mult;
 int sub_32;
} Tmmx_ConvertRGBtoYUY2matrix;

class TrgbPrimaries
{
private:
 short avisynthMmxMatrix[36];
 int32_t swscaleTable[6];
 Tmmx_ConvertRGBtoYUY2matrix mmx_ConvertRGBtoYUY2matrix;

 void reset(void);
 IffdshowDecVideo* deciV;
 bool wasJpeg;
protected:
 int cspOptionsIturBt, cspOptionsBlackCutoff, cspOptionsWhiteCutoff, cspOptionsChromaCutoff;
public:
 TrgbPrimaries(IffdshowBase *deci);
 TrgbPrimaries();
 void UpdateSettings(void);
 void writeToXvidYuv2RgbMatrix(short *asmData);
 void writeToXvidRgb2YuvMatrix(short *asmData);
 const unsigned char* getAvisynthYuv2RgbMatrix(void);
 const Tmmx_ConvertRGBtoYUY2matrix* getAvisynthRgb2YuvMatrix(void);
 const void initXvid(void);
 const int32_t* toSwscaleTable(void);
 void setJpeg(bool isjpeg);
 enum
  {
   ITUR_BT601,
   ITUR_BT709
  };
 enum
  {
   RecYCbCr,
   PcYCbCr,
   CutomYCbCr
  };
};

#endif
