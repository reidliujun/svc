/*
 * Copyright (c) 2007 h.yamagata
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "stdafx.h"
#include "IffdshowBase.h"
#include "IffdshowDecVideo.h"
#include "ffdshow_constants.h"
#include "TrgbPrimaries.h"
#include "ToutputVideoSettings.h"
#include "image.h"

//===================================== TrgbPrimaries ====================================
TrgbPrimaries::TrgbPrimaries():
 deciV(NULL)
{
 reset();
}

TrgbPrimaries::TrgbPrimaries(IffdshowBase *deci):
 deciV(NULL)
{
 deciV = comptrQ<IffdshowDecVideo>(deci);
 if (deciV)
  UpdateSettings();
 else
  reset();
}

void TrgbPrimaries::UpdateSettings(void)
{
 if (deciV)
  {
   const ToutputVideoSettings *outcfg = deciV->getToutputVideoSettings(); // This pointer may change during playback.
   cspOptionsIturBt = outcfg->cspOptionsIturBt;
   cspOptionsBlackCutoff = outcfg->get_cspOptionsBlackCutoff();
   cspOptionsWhiteCutoff = outcfg->get_cspOptionsWhiteCutoff();
   cspOptionsChromaCutoff = outcfg->get_cspOptionsChromaCutoff();
  }
 wasJpeg = false;
}

void TrgbPrimaries::reset(void)
{
 cspOptionsIturBt = ITUR_BT601;
 cspOptionsBlackCutoff = 16;
 cspOptionsWhiteCutoff = 235;
 cspOptionsChromaCutoff = 16;
 wasJpeg = false;
}

void TrgbPrimaries::setJpeg(bool isjpeg)
{
 // force BT.601 PC-YUV for MJPEG.
 if (isjpeg)
  {
   cspOptionsIturBt = ITUR_BT601;
   cspOptionsBlackCutoff = 0;
   cspOptionsWhiteCutoff = 255;
   cspOptionsChromaCutoff = 1;
   if (!wasJpeg)
    initXvid();
   wasJpeg = true;
  }
}

void TrgbPrimaries::writeToXvidRgb2YuvMatrix(short *asmData)
{
#define toRgb2YuvDataI                                                         \
 double Kr,Kg,Kb;                                                              \
 if (cspOptionsIturBt == ITUR_BT601)                                           \
  {                                                                            \
   Kr = 0.299;                                                                 \
   Kg = 0.587;                                                                 \
   Kb = 0.114;                                                                 \
  }                                                                            \
 else                                                                          \
  {                                                                            \
   Kr = 0.2125;                                                                \
   Kg = 0.7154;                                                                \
   Kb = 0.0721;                                                                \
  }                                                                            \
                                                                               \
 int whiteCutOff = cspOptionsWhiteCutoff >= 255 ? 255 : cspOptionsWhiteCutoff; \
 int blackCutOff = cspOptionsBlackCutoff <= 0 ? 1 : cspOptionsBlackCutoff;     \
 double y_range   = whiteCutOff - blackCutOff;                                 \
 double chr_range = 128 - cspOptionsChromaCutoff;

 toRgb2YuvDataI
 asmData[ 0] = asmData[ 4] = short((Kb * y_range / 255.0) * 256.0 + 0.5);
 asmData[ 1] = asmData[ 5] = short((Kg * y_range / 255.0) * 256.0 + 0.5);
 asmData[ 2] = asmData[ 6] = short((Kr * y_range / 255.0) * 256.0 + 0.5);

 asmData[ 8] = asmData[12] =
 asmData[18] = asmData[22] = short((chr_range / 255.0) * 256.0 + 0.5);
 asmData[ 9] = asmData[13] = short(-((Kg * chr_range / 255.0 / (1 - Kb)) * 256.0 + 0.5));
 asmData[10] = asmData[14] = short(-((Kr * chr_range / 255.0 / (1 - Kb)) * 256.0 + 0.5));

 asmData[16] = asmData[20] = short(-((Kb * chr_range / 255.0 / (1 - Kr)) * 256.0 + 0.5));
 asmData[17] = asmData[21] = short(-((Kg * chr_range / 255.0 / (1 - Kr)) * 256.0 + 0.5));

 asmData[24] = short(blackCutOff);

 asmData[ 3] = asmData[ 7] =
 asmData[11] = asmData[15] =
 asmData[19] = asmData[23] =
 asmData[25] = 0;
}

void TrgbPrimaries::writeToXvidYuv2RgbMatrix(short *asmData)
{
#define toYuv2RgbDataI                                              \
 double Kr,Kg,Kb;                                                   \
 if (cspOptionsIturBt == ITUR_BT601)                                \
  {                                                                 \
   Kr = 0.299;                                                      \
   Kg = 0.587;                                                      \
   Kb = 0.114;                                                      \
  }                                                                 \
 else                                                               \
  {                                                                 \
   Kr = 0.2125;                                                     \
   Kg = 0.7154;                                                     \
   Kb = 0.0721;                                                     \
  }                                                                 \
                                                                    \
 double y_range   = cspOptionsWhiteCutoff - cspOptionsBlackCutoff;  \
 double chr_range = 128 - cspOptionsChromaCutoff;                   \
                                                                    \
 double y_mul =255.0 / y_range;                                     \
 double vr_mul=(255 / chr_range) * (1.0 - Kr);                      \
 double ug_mul=(255 / chr_range) * (1.0 - Kb) * Kb / Kg;            \
 double vg_mul=(255 / chr_range) * (1.0 - Kr) * Kr / Kg;            \
 double ub_mul=(255 / chr_range) * (1.0 - Kb);

 toYuv2RgbDataI
 short Y_MUL =short(y_mul  * 64 + 0.4);
 short UG_MUL=short(ug_mul * 64 + 0.5);
 short VG_MUL=short(vg_mul * 64 + 0.5);
 short UB_MUL=short(ub_mul * 64 + 0.5);
 short VR_MUL=short(vr_mul * 64 + 0.5);
 for (int i=0 ; i<8 ; i++)
  {
   asmData[i]   = (short)cspOptionsBlackCutoff;  // YSUB
   asmData[i+8] =128;                   // U_SUB
   asmData[i+16]=128;                   // V_SUB
   asmData[i+24]=Y_MUL;
   asmData[i+32]=UG_MUL;
   asmData[i+40]=VG_MUL;
   asmData[i+48]=UB_MUL;
   asmData[i+56]=VR_MUL;
  }
}

const void TrgbPrimaries::initXvid(void)
{
 toYuv2RgbDataI
 xvid_colorspace_init(y_mul,ub_mul,ug_mul,vg_mul,vr_mul,cspOptionsBlackCutoff);
}

const Tmmx_ConvertRGBtoYUY2matrix* TrgbPrimaries::getAvisynthRgb2YuvMatrix(void)
{
 toRgb2YuvDataI
 mmx_ConvertRGBtoYUY2matrix.cybgr_64 = (int64_t(Kr * y_range / 255.0 * 32768.0) << 32) + (int64_t(Kg * y_range / 255.0 * 32768.0) << 16) + int64_t(Kb * y_range / 255.0 * 32768.0);
 mmx_ConvertRGBtoYUY2matrix.fpix_mul = (int64_t(112.0 / ((1-Kr) * 255.0) * 32768.0 + 0.5) << 32) + int64_t(112.0 / ((1-Kb) * 255.0) * 32768.0 + 0.5);
 mmx_ConvertRGBtoYUY2matrix.fraction = int((cspOptionsBlackCutoff + 0.5) * 32768.0);
 mmx_ConvertRGBtoYUY2matrix.sub_32 = -cspOptionsBlackCutoff * 2;
 mmx_ConvertRGBtoYUY2matrix.y1y2_mult = int(255.0 / y_range * 16384.0);
 return &mmx_ConvertRGBtoYUY2matrix;
}

const unsigned char* TrgbPrimaries::getAvisynthYuv2RgbMatrix(void)
{
 static const int64_t avisynthMmxMatrixConstants[4]=
  {
   0x00080008000800080LL,
   0x000FF00FF00FF00FFLL,
   0x00000200000002000LL,
   0x0FF000000FF000000LL
  };

 toYuv2RgbDataI
 // Avisynth YUY2->RGB
 int cy =short(y_mul * 16384 + 0.5);
 short crv=short(vr_mul * 8192 + 0.5);
 short cgu=short(-ug_mul * 8192 - 0.5);
 short cgv=short(-vg_mul * 8192 - 0.5);
 short cbu=short(ub_mul * 8192 + 0.5);
 memcpy(&avisynthMmxMatrix[4], avisynthMmxMatrixConstants, 32); // common part
 int *avisynthMmxMatrixInt = (int*)avisynthMmxMatrix;
 avisynthMmxMatrix[0] = avisynthMmxMatrix[1] = short(cspOptionsBlackCutoff);
 avisynthMmxMatrixInt[1] = 0;
 avisynthMmxMatrixInt[10] = avisynthMmxMatrixInt[11] = cy;
 avisynthMmxMatrixInt[12] = avisynthMmxMatrixInt[13] = crv << 16;
 avisynthMmxMatrix[29] = avisynthMmxMatrix[31] = cgv;
 avisynthMmxMatrix[28] = avisynthMmxMatrix[30] = cgu;
 avisynthMmxMatrixInt[16] = avisynthMmxMatrixInt[17] = cbu;
 return (const unsigned char*)avisynthMmxMatrix;
}

const int32_t* TrgbPrimaries::toSwscaleTable(void)
{
 toYuv2RgbDataI

 swscaleTable[0] = int32_t(vr_mul * 65536 + 0.5);
 swscaleTable[1] = int32_t(ub_mul * 65536 + 0.5);
 swscaleTable[2] = int32_t(ug_mul * 65536 + 0.5);
 swscaleTable[3] = int32_t(vg_mul * 65536 + 0.5);
 swscaleTable[4] = int32_t(y_mul  * 65536 + 0.5);
 swscaleTable[5] = int32_t(cspOptionsBlackCutoff * 65536);
 return swscaleTable;
}
