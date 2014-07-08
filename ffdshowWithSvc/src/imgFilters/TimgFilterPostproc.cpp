/*
 * Copyright (c) 2002-2006 Milan Cutka
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
#include "TimgFilterPostproc.h"
#include "TpostprocSettings.h"
#include "Tlibmplayer.h"
#include "TvideoCodec.h"
#include "IffdshowBase.h"
#include "IffdshowDecVideo.h"
#include "Tconfig.h"
#include "nic/nic_postProcess.h"
#include "libavcodec/avcodec.h"
#include "ffdebug.h"

//============================== TimgFilterPostprocBase =============================
const unsigned int TimgFilterPostprocBase::quantsDx=128*2;
const unsigned int TimgFilterPostprocBase::quantsDy= 96*2;

TimgFilterPostprocBase::TimgFilterPostprocBase(IffdshowBase *Ideci,Tfilters *Iparent,bool Ih264):
 TimgFilter(Ideci,Iparent),
 h264(Ih264),testh264(true),oldh264mode(-1),
 wasIP(false),
 oldDeblockStrength(UINT_MAX),
 quants((int8_t*)aligned_calloc(quantsDx,quantsDy))
{
}
TimgFilterPostprocBase::~TimgFilterPostprocBase()
{
 aligned_free(quants);
}

bool TimgFilterPostprocBase::pp_codec(int CodecID)
{
  if(raw_codec(CodecID))
    return true;
	switch (CodecID) {
		case CODEC_ID_MPEG1VIDEO:
		case CODEC_ID_MPEG2VIDEO:
		case CODEC_ID_LIBMPEG2:
		case CODEC_ID_MPEG4:
		case CODEC_ID_MSMPEG4V1:
		case CODEC_ID_MSMPEG4V2:
		case CODEC_ID_MSMPEG4V3:
		case CODEC_ID_H263:
		case CODEC_ID_SVQ1:
		case CODEC_ID_FLV1:
		case CODEC_ID_INDEO2:
		case CODEC_ID_INDEO3:
		case CODEC_ID_XVID4:
		case CODEC_ID_MJPEG:
		case CODEC_ID_MJPEGB:
		case CODEC_ID_MSVIDEO1:
		case CODEC_ID_CINEPAK:
		case CODEC_ID_VP5:
		case CODEC_ID_VP6:
		case CODEC_ID_VP6F:
			return true;
		default: return false;
	}
}

int TimgFilterPostprocBase::prepare(const TpostprocSettings *cfg,int maxquant,int frametype)
{
 const TvideoCodecDec *movie=NULL;
 if (!pp_codec(deci->getCurrentCodecId2()))
 return 0;
 currentq=0;
 if (!cfg->isCustom && cfg->autoq && cfg->qual)
  {
   currentq=deci->getParam2(IDFF_currentq);
   #if 1
   int cpuusage=deci->getCpuUsageForPP();
   #else
   int64_t late;deciV->getLate(&late);
   DPRINTF(_l("late:%I64i"),late);
   int cpuusage=late>100*10000LL?91:0;
   #endif
   if (cpuusage>90 && deciV->getQueuedCount()<8)
    {
     if (currentq>0) currentq--;
    }
   else
    {
     if (currentq<cfg->qual) currentq++;
    }
   deci->putParam(IDFF_currentq,currentq);
  }
 else
  deci->putParam(IDFF_currentq,currentq=cfg->qual);
 //DPRINTF("currentq: %i",currentq);
 int ppmode=Tlibmplayer::getPPmode(cfg,currentq);
 if (!ppmode) return ppmode;
 unsigned int deblockStrength=cfg->deblockStrength;
 if (!movie) deciV->getMovieSource(&movie);
 if (!movie || !movie->quants)
  {
   oldDeblockStrength=deblockStrength;
   unsigned char *quant=(unsigned char *)quants;
   for (unsigned int i=0;i<quantsDx*quantsDy;quant+=1,i++)
    {
     int q=((maxquant/2)*deblockStrength)>>8;
     *quant=(unsigned char)limit(q,1,maxquant);
    }
  }
 else
  if (!wasIP || (frametype&FRAME_TYPE::typemask)!=FRAME_TYPE::B)
   {
    wasIP=true;
    const unsigned char *quantsMovie=(const unsigned char*)movie->quants;
    unsigned int quantsDxMovie=movie->quantsDx;
    unsigned int quantsDyMovie=movie->quantsDy;
    unsigned int quantsStrideMovie=movie->quantsStride;
    unsigned int quantBytesMovie=movie->quantBytes;
    int8_t *quants2=quants;
    for (unsigned int y=0;y<quantsDyMovie;quantsMovie+=quantBytesMovie*quantsStrideMovie,quants2+=quantsDx,y++)
     {
      const unsigned char *quantsMovieln=quantsMovie;
      int8_t *quants2ln=quants2;
      for (unsigned int x=0;x<quantsDxMovie;quantsMovieln+=quantBytesMovie,quants2ln++,x++)
       {
        int q=(*quantsMovieln*deblockStrength)>>8;
        if (movie->quantType==FF_QSCALE_TYPE_MPEG2) q>>=1;
        *quants2ln=(unsigned char)limit(q,1,maxquant);
       }
     }
   }
 return ppmode;
}

void TimgFilterPostprocBase::onSeek(void)
{
 testh264=true;wasIP=false;
}

//============================ TimgFilterPostprocMplayer ============================
TimgFilterPostprocMplayer::TimgFilterPostprocMplayer(IffdshowBase *Ideci,Tfilters *Iparent):TimgFilterPostprocBase(Ideci,Iparent,false)
{
 libmplayer=NULL;
 pp_ctx=NULL;Tlibmplayer::pp_mode_defaults(pp_mode);
}
TimgFilterPostprocMplayer::~TimgFilterPostprocMplayer()
{
 if (libmplayer) libmplayer->Release();
}

void TimgFilterPostprocMplayer::done(void)
{
 if (pp_ctx) libmplayer->pp_free_context(pp_ctx);pp_ctx=NULL;
}
void TimgFilterPostprocMplayer::onSizeChange(void)
{
 done();
}

bool TimgFilterPostprocMplayer::is(const TffPictBase &pict,const TfilterSettingsVideo *cfg0)
{
 const TpostprocSettings *cfg=(const TpostprocSettings*)cfg0;
 if (super::is(pict,cfg) && Tlibmplayer::getPPmode(cfg,cfg->qual))
  {
   Trect r=pict.getRect(cfg->full,cfg->half);
   return pictRect.dx>=16 && pictRect.dy>=16;
  }
 else
  return false;
}

HRESULT TimgFilterPostprocMplayer::process(TfilterQueue::iterator it,TffPict &pict,const TfilterSettingsVideo *cfg0)
{
 const TpostprocSettings *cfg=(const TpostprocSettings*)cfg0;
 if (int ppmode=prepare(cfg,31,0))
  {
   init(pict,cfg->full,cfg->half);
   if (pictRect.dx>=16 && pictRect.dy>=16)
    {
     bool cspChanged=false;
     const unsigned char *tempPict1[4];
     cspChanged|=getCur(FF_CSPS_MASK_YUV_PLANAR,pict,cfg->full,tempPict1);
     unsigned char *tempPict2[4];
     cspChanged|=getNext(csp1,pict,cfg->full,tempPict2);
     if (cspChanged) done();

     if (!pp_ctx)
      {
       if (!libmplayer) deci->getPostproc(&libmplayer);
       pp_ctx=libmplayer->pp_get_context(dx1[0],dy1[0],Tlibmplayer::ppCpuCaps(csp1));
      }
     pp_mode.lumMode=ppmode&15;
     pp_mode.chromMode=(ppmode>>4)&15;
     if (cfg->deblockMplayerAccurate)
      {
       pp_mode.baseDcDiff=128;
       pp_mode.flatnessThreshold=7;
       if (pp_mode.lumMode&H_DEBLOCK) pp_mode.lumMode=(pp_mode.lumMode&~H_DEBLOCK)|H_A_DEBLOCK;
       if (pp_mode.lumMode&V_DEBLOCK) pp_mode.lumMode=(pp_mode.lumMode&~V_DEBLOCK)|V_A_DEBLOCK;
       if (pp_mode.chromMode&H_DEBLOCK) pp_mode.chromMode=(pp_mode.chromMode&~H_DEBLOCK)|H_A_DEBLOCK;
       if (pp_mode.chromMode&V_DEBLOCK) pp_mode.chromMode=(pp_mode.chromMode&~V_DEBLOCK)|V_A_DEBLOCK;
      }
     else
      {
       pp_mode.baseDcDiff=256/8;
       pp_mode.flatnessThreshold=56-16-1;
      }

     if (pp_mode.lumMode&LUM_LEVEL_FIX && cfg->fullYrange)
      {
       pp_mode.minAllowedY=0;
       pp_mode.maxAllowedY=255;
      }
     else
      {
       pp_mode.minAllowedY=16;
       pp_mode.maxAllowedY=234;
      }
     int frametype=pict.frametype&FRAME_TYPE::typemask; //3 = B
     libmplayer->pp_postprocess(tempPict1,stride1,
                                tempPict2,stride2,
                                dx1[0],dy1[0],
                                quants,quantsDx,
                                &pp_mode,pp_ctx,frametype);
    }
  }
 return parent->deliverSample(++it,pict);
}

//============================== TimgFilterPostprocSpp ===============================
#include "csimd.h"

extern "C" void simple_idct_mmx_P(short*);
void TimgFilterPostprocSpp::ff_fdct_mmx2(int16_t *block)
{
 static const int BITS_FRW_ACC=3; //; 2 or 3 for accuracy
 static const int SHIFT_FRW_COL=BITS_FRW_ACC;
 static const int SHIFT_FRW_ROW=(BITS_FRW_ACC + 17 - 3);
 static const int RND_FRW_ROW=(1 << (SHIFT_FRW_ROW-1));

 static __align8(const int16_t,tab_frw_01234567[]) = {  // forward_dct coeff table
   16384,   16384,   22725,   19266,
   16384,   16384,   12873,    4520,
   21407,    8867,   19266,   -4520,
   -8867,  -21407,  -22725,  -12873,
   16384,  -16384,   12873,  -22725,
  -16384,   16384,    4520,   19266,
    8867,  -21407,    4520,  -12873,
   21407,   -8867,   19266,  -22725,

   22725,   22725,   31521,   26722,
   22725,   22725,   17855,    6270,
   29692,   12299,   26722,   -6270,
  -12299,  -29692,  -31521,  -17855,
   22725,  -22725,   17855,  -31521,
  -22725,   22725,    6270,   26722,
   12299,  -29692,    6270,  -17855,
   29692,  -12299,   26722,  -31521,

   21407,   21407,   29692,   25172,
   21407,   21407,   16819,    5906,
   27969,   11585,   25172,   -5906,
  -11585,  -27969,  -29692,  -16819,
   21407,  -21407,   16819,  -29692,
  -21407,   21407,    5906,   25172,
   11585,  -27969,    5906,  -16819,
   27969,  -11585,   25172,  -29692,

   19266,   19266,   26722,   22654,
   19266,   19266,   15137,    5315,
   25172,   10426,   22654,   -5315,
  -10426,  -25172,  -26722,  -15137,
   19266,  -19266,   15137,  -26722,
  -19266,   19266,    5315,   22654,
   10426,  -25172,    5315,  -15137,
   25172,  -10426,   22654,  -26722,

   16384,   16384,   22725,   19266,
   16384,   16384,   12873,    4520,
   21407,    8867,   19266,   -4520,
   -8867,  -21407,  -22725,  -12873,
   16384,  -16384,   12873,  -22725,
  -16384,   16384,    4520,   19266,
    8867,  -21407,    4520,  -12873,
   21407,   -8867,   19266,  -22725,

   19266,   19266,   26722,   22654,
   19266,   19266,   15137,    5315,
   25172,   10426,   22654,   -5315,
  -10426,  -25172,  -26722,  -15137,
   19266,  -19266,   15137,  -26722,
  -19266,   19266,    5315,   22654,
   10426,  -25172,    5315,  -15137,
   25172,  -10426,   22654,  -26722,

   21407,   21407,   29692,   25172,
   21407,   21407,   16819,    5906,
   27969,   11585,   25172,   -5906,
  -11585,  -27969,  -29692,  -16819,
   21407,  -21407,   16819,  -29692,
  -21407,   21407,    5906,   25172,
   11585,  -27969,    5906,  -16819,
   27969,  -11585,   25172,  -29692,

   22725,   22725,   31521,   26722,
   22725,   22725,   17855,    6270,
   29692,   12299,   26722,   -6270,
  -12299,  -29692,  -31521,  -17855,
   22725,  -22725,   17855,  -31521,
  -22725,   22725,    6270,   26722,
   12299,  -29692,    6270,  -17855,
   29692,  -12299,   26722,  -31521,
  };
 static __align8(const int16_t,fdct_tg_all_16[]) = {
    13036, 13036, 13036, 13036,		// tg * (2<<16) + 0.5
    27146, 27146, 27146, 27146,		// tg * (2<<16) + 0.5
    -21746, -21746, -21746, -21746,	// tg * (2<<16) + 0.5
 };
 static const __int64 fdct_one_corr = 0x0001000100010001LL;
 static  __align8(const int16_t,ocos_4_16[4]) = {
    23170, 23170, 23170, 23170,	//cos * (2<<15) + 0.5
 };
 static __align8(const int32_t,fdct_r_row[2])={RND_FRW_ROW, RND_FRW_ROW };

 struct Tcol
  {
   static __forceinline void fdct_col(const int16_t *in, int16_t *out, int offset)
    {
       __m64 mm0,mm1,mm2,mm3,mm4,mm5,mm6,mm7;
       csimd::movq((in + offset + 1 * 8), mm0);
       csimd::movq((in + offset + 6 * 8), mm1);
       csimd::movq(mm0, mm2);
       csimd::movq((in + offset + 2 * 8), mm3);
       csimd::paddsw(mm1, mm0);
       csimd::movq((in + offset + 5 * 8), mm4);
       csimd::psllw(SHIFT_FRW_COL, mm0);
       csimd::movq((in + offset + 0 * 8), mm5);
       csimd::paddsw(mm3, mm4);
       csimd::paddsw((in + offset + 7 * 8), mm5);
       csimd::psllw(SHIFT_FRW_COL, mm4);
       csimd::movq(mm0, mm6);
       csimd::psubsw(mm1, mm2);
       csimd::movq((fdct_tg_all_16 + 4), mm1);
       csimd::psubsw(mm4, mm0);
       csimd::movq((in + offset + 3 * 8), mm7);
       csimd::pmulhw(mm0, mm1);
       csimd::paddsw((in + offset + 4 * 8), mm7);
       csimd::psllw(SHIFT_FRW_COL, mm5);
       csimd::paddsw(mm4, mm6);
       csimd::psllw(SHIFT_FRW_COL, mm7);
       csimd::movq(mm5, mm4);
       csimd::psubsw(mm7, mm5);
       csimd::paddsw(mm5, mm1);
       csimd::paddsw(mm7, mm4);
       csimd::por(fdct_one_corr, mm1);
       csimd::psllw(SHIFT_FRW_COL + 1, mm2);
       csimd::pmulhw((fdct_tg_all_16 + 4), mm5);
       csimd::movq(mm4, mm7);
       csimd::psubsw((in + offset + 5 * 8), mm3);
       csimd::psubsw(mm6, mm4);
       csimd::movq(mm1, (out + offset + 2 * 8));
       csimd::paddsw(mm6, mm7);
       csimd::movq((in + offset + 3 * 8), mm1);
       csimd::psllw(SHIFT_FRW_COL + 1, mm3);
       csimd::psubsw((in + offset + 4 * 8), mm1);
       csimd::movq(mm2, mm6);
       csimd::movq(mm4, (out + offset + 4 * 8));
       csimd::paddsw(mm3, mm2);
       csimd::pmulhw(ocos_4_16, mm2);
       csimd::psubsw(mm3, mm6);
       csimd::pmulhw(ocos_4_16, mm6);
       csimd::psubsw(mm0, mm5);
       csimd::por(fdct_one_corr, mm5);
       csimd::psllw(SHIFT_FRW_COL, mm1);
       csimd::por(fdct_one_corr, mm2);
       csimd::movq(mm1, mm4);
       csimd::movq((in + offset + 0 * 8), mm3);
       csimd::paddsw(mm6, mm1);
       csimd::psubsw((in + offset + 7 * 8), mm3);
       csimd::psubsw(mm6, mm4);
       csimd::movq((fdct_tg_all_16 + 0), mm0);
       csimd::psllw(SHIFT_FRW_COL, mm3);
       csimd::movq((fdct_tg_all_16 + 8), mm6);
       csimd::pmulhw(mm1, mm0);
       csimd::movq(mm7, (out + offset + 0 * 8));
       csimd::pmulhw(mm4, mm6);
       csimd::movq(mm5, (out + offset + 6 * 8));
       csimd::movq(mm3, mm7);
       csimd::movq((fdct_tg_all_16 + 8), mm5);
       csimd::psubsw(mm2, mm7);
       csimd::paddsw(mm2, mm3);
       csimd::pmulhw(mm7, mm5);
       csimd::paddsw(mm3, mm0);
       csimd::paddsw(mm4, mm6);
       csimd::pmulhw((fdct_tg_all_16 + 0), mm3);
       csimd::por(fdct_one_corr, mm0);
       csimd::paddsw(mm7, mm5);
       csimd::psubsw(mm6, mm7);
       csimd::movq(mm0, (out + offset + 1 * 8));
       csimd::paddsw(mm4, mm5);
       csimd::movq(mm7, (out + offset + 3 * 8));
       csimd::psubsw(mm1, mm3);
       csimd::movq(mm5, (out + offset + 5 * 8));
       csimd::movq(mm3, (out + offset + 7 * 8));
    }
  };
/*
 struct TrowMMX2
  {
   static __forceinline void fdct_row(const int16_t *in, int16_t *out, const int16_t *table)
    {
       __m64 mm0,mm1,mm2,mm3,mm4,mm5,mm6,mm7;
       mm5=_mm_shuffle_pi16(*(__m64*)(in+4),0x1b);// pshufw((in + 4), mm5, 0x1B);
       csimd::movq((in + 0), mm0);
       csimd::movq(mm0, mm1);
       csimd::paddsw(mm5, mm0);
       csimd::psubsw(mm5, mm1);
       csimd::movq(mm0, mm2);
       csimd::punpckldq(mm1, mm0);
       csimd::punpckhdq(mm1, mm2);
       csimd::movq((table + 0), mm1);
       csimd::movq((table + 4), mm3);
       csimd::movq((table + 8), mm4);
       csimd::movq((table + 12), mm5);
       csimd::movq((table + 16), mm6);
       csimd::movq((table + 20), mm7);
       csimd::pmaddwd(mm0, mm1);
       csimd::pmaddwd(mm2, mm3);
       csimd::pmaddwd(mm0, mm4);
       csimd::pmaddwd(mm2, mm5);
       csimd::pmaddwd(mm0, mm6);
       csimd::pmaddwd(mm2, mm7);
       csimd::pmaddwd((table + 24), mm0);
       csimd::pmaddwd((table + 28), mm2);
       csimd::paddd(mm1, mm3);
       csimd::paddd(mm4, mm5);
       csimd::paddd(mm6, mm7);
       csimd::paddd(mm0, mm2);
       csimd::movq(fdct_r_row, mm0);
       csimd::paddd(mm0, mm3);
       csimd::paddd(mm0, mm5);
       csimd::paddd(mm0, mm7);
       csimd::paddd(mm0, mm2);
       csimd::psrad(SHIFT_FRW_ROW, mm3);
       csimd::psrad(SHIFT_FRW_ROW, mm5);
       csimd::psrad(SHIFT_FRW_ROW, mm7);
       csimd::psrad(SHIFT_FRW_ROW, mm2);
       csimd::packssdw(mm5, mm3);
       csimd::packssdw(mm2, mm7);
       csimd::movq(mm3, (out + 0));
       csimd::movq(mm7, (out + 4));
    }
  };
*/
 struct TrowMMX
  {
   static __forceinline void fdct_row(const int16_t *in, int16_t *out, const int16_t *table)
    {
       __m64 mm0,mm1,mm2,mm3,mm4,mm5,mm6,mm7;
       csimd::movd((in + 6), mm1);
       csimd::punpcklwd((in + 4), mm1);
       csimd::movq(mm1, mm2);
       csimd::psrlq(0x20, mm1);
       csimd::movq((in + 0), mm0);
       csimd::punpcklwd(mm2, mm1);
       csimd::movq(mm0, mm5);
       csimd::paddsw(mm1, mm0);
       csimd::psubsw(mm1, mm5);
       csimd::movq(mm0, mm2);
       csimd::punpckldq(mm5, mm0);
       csimd::punpckhdq(mm5, mm2);
       csimd::movq((table + 0), mm1);
       csimd::movq((table + 4), mm3);
       csimd::movq((table + 8), mm4);
       csimd::movq((table + 12), mm5);
       csimd::movq((table + 16), mm6);
       csimd::movq((table + 20), mm7);
       csimd::pmaddwd(mm0, mm1);
       csimd::pmaddwd(mm2, mm3);
       csimd::pmaddwd(mm0, mm4);
       csimd::pmaddwd(mm2, mm5);
       csimd::pmaddwd(mm0, mm6);
       csimd::pmaddwd(mm2, mm7);
       csimd::pmaddwd((table + 24), mm0);
       csimd::pmaddwd((table + 28), mm2);
       csimd::paddd(mm1, mm3);
       csimd::paddd(mm4, mm5);
       csimd::paddd(mm6, mm7);
       csimd::paddd(mm0, mm2);
       csimd::movq(fdct_r_row, mm0);
       csimd::paddd(mm0, mm3);
       csimd::paddd(mm0, mm5);
       csimd::paddd(mm0, mm7);
       csimd::paddd(mm0, mm2);
       csimd::psrad(SHIFT_FRW_ROW, mm3);
       csimd::psrad(SHIFT_FRW_ROW, mm5);
       csimd::psrad(SHIFT_FRW_ROW, mm7);
       csimd::psrad(SHIFT_FRW_ROW, mm2);
       csimd::packssdw(mm5, mm3);
       csimd::packssdw(mm2, mm7);
       csimd::movq(mm3, (out + 0));
       csimd::movq(mm7, (out + 4));
   }
  };

    __align8(int64_t,align_tmp[16]);
    int16_t * const block_tmp= (int16_t*)align_tmp;
    int16_t *block1, *out;
    const int16_t *table;
    int i;

    block1 = block_tmp;
    Tcol::fdct_col(block, block1, 0);
    Tcol::fdct_col(block, block1, 4);

    block1 = block_tmp;
    table = tab_frw_01234567;
    out = block;
    for(i=8;i>0;i--) {
        TrowMMX::fdct_row(block1, out, table);
        block1 += 8;
        table += 32;
        out += 8;
    }
}

TimgFilterPostprocSpp::TimgFilterPostprocSpp(IffdshowBase *Ideci,Tfilters *Iparent):
 TimgFilterPostprocBase(Ideci,Iparent,false),
 temp(NULL),src(NULL),
 old_sppMode(-1)
{
}
void TimgFilterPostprocSpp::done(void)
{
 if (temp) aligned_free(temp);temp=NULL;
 if (src) aligned_free(src);src=NULL;
}
void TimgFilterPostprocSpp::onSizeChange(void)
{
 done();
}

template<> __align16(const uint8_t,TimgFilterPostprocSpp::TstoreSlice<Tmmx>::dither[8][8])=
{
 {  0,  48,  12,  60,   3,  51,  15,  63, },
 { 32,  16,  44,  28,  35,  19,  47,  31, },
 {  8,  56,   4,  52,  11,  59,   7,  55, },
 { 40,  24,  36,  20,  43,  27,  39,  23, },
 {  2,  50,  14,  62,   1,  49,  13,  61, },
 { 34,  18,  46,  30,  33,  17,  45,  29, },
 { 10,  58,   6,  54,   9,  57,   5,  53, },
 { 42,  26,  38,  22,  41,  25,  37,  21, },
};
#ifdef __SSE2__
template<> __align16(const uint8_t,TimgFilterPostprocSpp::TstoreSlice<Tsse2>::dither[8][16])=
{
 {  0,  48,  12,  60,   3,  51,  15,  63,   0,  48,  12,  60,   3,  51,  15,  63 },
 { 32,  16,  44,  28,  35,  19,  47,  31,  32,  16,  44,  28,  35,  19,  47,  31 },
 {  8,  56,   4,  52,  11,  59,   7,  55,   8,  56,   4,  52,  11,  59,   7,  55 },
 { 40,  24,  36,  20,  43,  27,  39,  23,  40,  24,  36,  20,  43,  27,  39,  23 },
 {  2,  50,  14,  62,   1,  49,  13,  61,   2,  50,  14,  62,   1,  49,  13,  61 },
 { 34,  18,  46,  30,  33,  17,  45,  29,  34,  18,  46,  30,  33,  17,  45,  29 },
 { 10,  58,   6,  54,   9,  57,   5,  53,  10,  58,   6,  54,   9,  57,   5,  53 },
 { 42,  26,  38,  22,  41,  25,  37,  21,  42,  26,  38,  22,  41,  25,  37,  21 },
};
#endif

void TimgFilterPostprocSpp::store_slice_c(uint8_t *dst, const int16_t *src, int dst_stride, int src_stride, unsigned int width, unsigned int height, int log2_scale)
{
 for (unsigned int y=0;y<height;y++)
  {
   const uint8_t *d=TstoreSlice<Tmmx>::dither[y];
   for (unsigned int x=0;x<width;x+=8)
    {
     store(0,src,x,y,dst_stride,src_stride,log2_scale,d,dst);
     store(1,src,x,y,dst_stride,src_stride,log2_scale,d,dst);
     store(2,src,x,y,dst_stride,src_stride,log2_scale,d,dst);
     store(3,src,x,y,dst_stride,src_stride,log2_scale,d,dst);
     store(4,src,x,y,dst_stride,src_stride,log2_scale,d,dst);
     store(5,src,x,y,dst_stride,src_stride,log2_scale,d,dst);
     store(6,src,x,y,dst_stride,src_stride,log2_scale,d,dst);
     store(7,src,x,y,dst_stride,src_stride,log2_scale,d,dst);
    }
  }
}
template<class _mm> void TimgFilterPostprocSpp::TstoreSlice<_mm>::store_slice(uint8_t *dst0, const int16_t *src0, int dst_stride, int src_stride, unsigned int width, unsigned int height, int log2_scale)
{
 const int16_t *src=src0;unsigned char *dst=dst0;
 if (_mm::align && (intptr_t(src)&15 || intptr_t(dst)&15 || src_stride&15 || dst_stride&15))
  {
   TstoreSlice<Tmmx>::store_slice(dst,src,dst_stride,src_stride,width,height,log2_scale);
   return;
  }
 unsigned int mmxw=width&~(_mm::size-1);
 for(unsigned int y=0; y<height; y++)
  {
   typename _mm::__m mm3,mm4,mm0,mm1;
   movq (mm3,dither[y]);
   movq (mm4,dither[y]);
   pxor (mm0, mm0);
   punpcklbw (mm3,mm0);
   punpckhbw (mm4,mm0);
   psraw (mm3,log2_scale);
   psraw (mm4,log2_scale);
   uint8_t *dst1=dst;
   const uint8_t *src1=(const uint8_t*)src;
   const uint8_t *dst1end=dst+width-_mm::size+1;
   for (;dst1<dst1end;src1+=_mm::size*2,dst1+=_mm::size)
    {
     movq (mm0,src1);
     movq (mm1,src1+_mm::size);
     paddw (mm0,mm3);
     paddw (mm1,mm4);
     psraw (mm0,6-log2_scale);
     psraw (mm1,6-log2_scale);
     packuswb(mm0,mm1);
     movq (dst1,mm0);
    }
   src+=src_stride;
   dst+=dst_stride;
  }
 if(width != mmxw)
  store_slice_c(dst0 + mmxw, src0 + mmxw, dst_stride, src_stride, width - mmxw, height,log2_scale);
}
__forceinline void TimgFilterPostprocSpp::requant_core_soft(unsigned char *dst0,unsigned char *dst1,unsigned char *dst2,unsigned char *dst3,const unsigned char *src0,const unsigned char *src1,const unsigned char *src2,const unsigned char *src3,const __m64 &mm4,const __m64 &mm5)
{
 __m64 mm0,mm1,mm6,mm7,mm2,mm3;
 movq  (mm0,src0);
 movq  (mm1,src1);
 pxor    (mm6,mm6 );
 pxor    (mm7,mm7 );
 pcmpgtw (mm6,mm0 );
 pcmpgtw (mm7,mm1 );
 pxor    (mm0,mm6 );
 pxor    (mm1,mm7 );
 psubusw (mm0,mm4 );
 psubusw (mm1,mm4 );
 pxor    (mm0,mm6 );
 pxor    (mm1,mm7 );
 movq    (mm2,src2);
 movq    (mm3,src3);
 pxor    (mm6,mm6);
 pxor    (mm7,mm7);
 pcmpgtw (mm6,mm2);
 pcmpgtw (mm7,mm3);
 pxor    (mm2,mm6);
 pxor    (mm3,mm7);
 psubusw (mm2,mm4);
 psubusw (mm3,mm4);
 pxor    (mm2,mm6);
 pxor    (mm3,mm7);

 paddsw (mm0,mm5 );
 paddsw (mm1,mm5 );
 paddsw (mm2,mm5 );
 paddsw (mm3,mm5 );
 psraw  (mm0, 3  );
 psraw  (mm1, 3  );
 psraw  (mm2, 3  );
 psraw  (mm3, 3  );

 movq      (mm7,mm0);
 punpcklwd (mm0,mm2);
 punpckhwd (mm7,mm2);
 movq      (mm2,mm1);
 punpcklwd (mm1,mm3);
 punpckhwd (mm2,mm3);
 movq      (mm3,mm0);
 punpcklwd (mm0,mm1);
 punpckhwd (mm3,mm7);
 punpcklwd (mm7,mm2);
 punpckhwd (mm1,mm2);

 movq (dst0,mm0);
 movq (dst1,mm7);
 movq (dst2,mm3);
 movq (dst3,mm1);
}
void TimgFilterPostprocSpp::softthresh_mmx(DCTELEM dst0[64], const DCTELEM src0[64], int qp)
{
 int bias= 0; //FIXME
 unsigned int threshold1;
 threshold1= qp*((1<<4) - bias) - 1;

 __m64 mm4,mm5;
 movd (mm4,threshold1);
 movd (mm5,4);
 packssdw( mm4, mm4);
 packssdw( mm5, mm5);
 packssdw( mm4, mm4);
 packssdw( mm5, mm5);
 const unsigned char *src=(const unsigned char*)src0;
 unsigned char *dst=(unsigned char*)dst0;
 requant_core_soft(   dst,  8+dst, 16+dst, 24+dst,   src, 8+src, 64+src, 72+src,mm4,mm5);
 requant_core_soft(32+dst, 40+dst, 48+dst, 56+dst,16+src,24+src, 48+src, 56+src,mm4,mm5);
 requant_core_soft(64+dst, 72+dst, 80+dst, 88+dst,32+src,40+src, 96+src,104+src,mm4,mm5);
 requant_core_soft(96+dst,104+dst,112+dst,120+dst,80+src,88+src,112+src,120+src,mm4,mm5);
 dst0[0]=DCTELEM((src0[0]+4)>>3);
}
__forceinline void TimgFilterPostprocSpp::requant_core_hard(unsigned char *dst0,unsigned char *dst1,unsigned char *dst2,unsigned char *dst3,const unsigned char *src0,const unsigned char *src1,const unsigned char *src2,const unsigned char *src3,const __m64 &mm4,const __m64 &mm5,const __m64 &mm6)
{
 __m64 mm0,mm1,mm2,mm3;
 movq( mm0,src0);
 movq( mm1,src1);
 movq( mm2,src2);
 movq( mm3,src3);

 psubw( mm0, mm4);
 psubw( mm1, mm4);
 psubw( mm2, mm4);
 psubw( mm3, mm4);

 paddusw( mm0,mm5);
 paddusw( mm1,mm5);
 paddusw( mm2,mm5);
 paddusw( mm3,mm5);

 paddw( mm0,mm6);
 paddw( mm1,mm6);
 paddw( mm2,mm6);
 paddw( mm3,mm6);

 psubusw( mm0,mm6);
 psubusw( mm1,mm6);
 psubusw( mm2,mm6);
 psubusw( mm3,mm6);

 psraw( mm0,3);
 psraw( mm1,3);
 psraw( mm2,3);
 psraw( mm3,3);
 __m64 mm7;
 movq      (mm7,mm0);
 punpcklwd (mm0,mm2);
 punpckhwd (mm7,mm2);
 movq      (mm2,mm1);
 punpcklwd (mm1,mm3);
 punpckhwd (mm2,mm3);
 movq      (mm3,mm0);
 punpcklwd (mm0,mm1);
 punpckhwd (mm3,mm7);
 punpcklwd (mm7,mm2);
 punpckhwd (mm1,mm2);

 movq (dst0,mm0);
 movq (dst1,mm7);
 movq (dst2,mm3);
 movq (dst3,mm1);
}

void TimgFilterPostprocSpp::hardthresh_mmx(DCTELEM dst0[64], const DCTELEM src0[64], int qp)
{
 int bias= 0; //FIXME
 unsigned int threshold1;

 threshold1= qp*((1<<4) - bias) - 1;

 __m64 mm4,mm5,mm6;
 movd( mm4,threshold1+1);
 movd( mm5,threshold1+5);
 movd( mm6,threshold1-4);
 packssdw( mm4,mm4);
 packssdw( mm5,mm5);
 packssdw( mm6,mm6);
 packssdw( mm4,mm4);
 packssdw( mm5,mm5);
 packssdw( mm6,mm6);
 const unsigned char *src=(const unsigned char*)src0;
 unsigned char *dst=(unsigned char*)dst0;
 requant_core_hard(   dst,  8+dst, 16+dst, 24+dst,   src, 8+src, 64+src, 72+src,mm4,mm5,mm6);
 requant_core_hard(32+dst, 40+dst, 48+dst, 56+dst,16+src,24+src, 48+src, 56+src,mm4,mm5,mm6);
 requant_core_hard(64+dst, 72+dst, 80+dst, 88+dst,32+src,40+src, 96+src,104+src,mm4,mm5,mm6);
 requant_core_hard(96+dst,104+dst,112+dst,120+dst,80+src,88+src,112+src,120+src,mm4,mm5,mm6);

 dst0[0]=DCTELEM((src0[0]+4)>>3);
}

const uint8_t TimgFilterPostprocSpp::offset[127][2]=
{
 {0,0},
 {0,0}, {4,4},
 {0,0}, {2,2}, {6,4}, {4,6},
 {0,0}, {5,1}, {2,2}, {7,3}, {4,4}, {1,5}, {6,6}, {3,7},

 {0,0}, {4,0}, {1,1}, {5,1}, {3,2}, {7,2}, {2,3}, {6,3},
 {0,4}, {4,4}, {1,5}, {5,5}, {3,6}, {7,6}, {2,7}, {6,7},

 {0,0}, {0,2}, {0,4}, {0,6}, {1,1}, {1,3}, {1,5}, {1,7},
 {2,0}, {2,2}, {2,4}, {2,6}, {3,1}, {3,3}, {3,5}, {3,7},
 {4,0}, {4,2}, {4,4}, {4,6}, {5,1}, {5,3}, {5,5}, {5,7},
 {6,0}, {6,2}, {6,4}, {6,6}, {7,1}, {7,3}, {7,5}, {7,7},

 {0,0}, {4,4}, {0,4}, {4,0}, {2,2}, {6,6}, {2,6}, {6,2},
 {0,2}, {4,6}, {0,6}, {4,2}, {2,0}, {6,4}, {2,4}, {6,0},
 {1,1}, {5,5}, {1,5}, {5,1}, {3,3}, {7,7}, {3,7}, {7,3},
 {1,3}, {5,7}, {1,7}, {5,3}, {3,1}, {7,5}, {3,5}, {7,1},
 {0,1}, {4,5}, {0,5}, {4,1}, {2,3}, {6,7}, {2,7}, {6,3},
 {0,3}, {4,7}, {0,7}, {4,3}, {2,1}, {6,5}, {2,5}, {6,1},
 {1,0}, {5,4}, {1,4}, {5,0}, {3,2}, {7,6}, {3,6}, {7,2},
 {1,2}, {5,6}, {1,6}, {5,2}, {3,0}, {7,4}, {3,4}, {7,0},
};

inline void TimgFilterPostprocSpp::add_block(int16_t *dst, int stride, DCTELEM block[64])
{
 for (int y=0;y<8;y++)
  {
   paddw(*(__m64*)&dst[0+y*stride],*(__m64*)&block[0+y*8]);
   paddw(*(__m64*)&dst[4+y*stride],*(__m64*)&block[4+y*8]);
  }
}

inline void TimgFilterPostprocSpp::get_pixels(DCTELEM *block, const uint8_t *pixels, int line_size)
{
 __m64 mm7=_mm_setzero_si64(),mm0,mm2,mm1,mm3;
 for (int REG_a=-64;REG_a;pixels+=2*line_size,REG_a+=16)
  {
   movq (mm0,pixels);
   movq (mm2,pixels+line_size);
   movq (mm1,mm0);
   movq (mm3,mm2);
   punpcklbw  (mm0,mm7);
   punpckhbw  (mm1,mm7);
   punpcklbw  (mm2,mm7);
   punpckhbw  (mm3,mm7);
   movq ( 0+block+64+REG_a,mm0);
   movq ( 4+block+64+REG_a,mm1);
   movq ( 8+block+64+REG_a,mm2);
   movq (12+block+64+REG_a,mm3);
  }
}

#ifndef WIN64
void TimgFilterPostprocSpp::filter(uint8_t *dst, const uint8_t *src0, int dst_stride, int src_stride, unsigned int width, unsigned int height, const int8_t *qp_store, int qp_stride, bool is_luma)
{
 const int count=1<<currentq;
 const int stride=is_luma?temp_stride:((width+16+15)&(~15));
 uint64_t block_align1[32*2],*block_align=(uint64_t*)(((intptr_t)block_align1+16)&(~15));
 DCTELEM *block =(DCTELEM *)block_align;
 DCTELEM *block2=(DCTELEM *)(block_align+16);
 unsigned int y;
 for (y=0;y<height;y++)
  {
   int index=8+8*stride+y*stride;
   memcpy(src+index,src0+y*src_stride,width);
   for(unsigned int x=0;x<8;x++)
    {
     src[index      -x-1]=src[index+      x  ];
     src[index+width+x  ]=src[index+width-x-1];
    }
  }
 for (y=0;y<8;y++)
  {
   memcpy(src+(       7-y)*stride,src+(       y+8)*stride,stride);
   memcpy(src+(height+8+y)*stride,src+(height-y+7)*stride,stride);
  }
 //FIXME (try edge emu)
 for (y=0;y<height+8;y+=8)
  {
   memset(temp+(8+y)*stride,0,8*stride*sizeof(int16_t));
   for (unsigned int x=0;x<width+8;x+=8)
    {
     const int qps=3+is_luma;
     int qp;

     qp=qp_store[(std::min(x,width-1)>>qps)+(std::min(y,height-1)>>qps)*qp_stride];
     //if(p->mpeg2) qp>>=1;
     for (int i=0;i<count;i++)
      {
       const int x1=x+offset[i+count-1][0];
       const int y1=y+offset[i+count-1][1];
       const int index= x1 + y1*stride;
       get_pixels(block,src+index,stride);
       ff_fdct_mmx2(block);
       //dsp->fdct(block);
       requantize(block2,block,qp);
       simple_idct_mmx_P(block2);
       //dsp->idct(block2);
       add_block(temp+index,stride,block2);
      }
    }
   if (y)
    store_slice(dst+(y-8)*dst_stride,temp+8+y*stride,dst_stride,stride,width,std::min(8U,height+8-y),6-currentq);
  }
}
#endif

bool TimgFilterPostprocSpp::is(const TffPictBase &pict,const TfilterSettingsVideo *cfg0)
{
 const TpostprocSettings *cfg=(const TpostprocSettings*)cfg0;
 if (super::is(pict,cfg) && cfg->qual)
  {
   Trect r=pict.getRect(cfg->full,cfg->half);
   return pictRect.dx>=16 && pictRect.dy>=16;
  }
 else
  return false;
}

HRESULT TimgFilterPostprocSpp::process(TfilterQueue::iterator it,TffPict &pict,const TfilterSettingsVideo *cfg0)
{
 const TpostprocSettings *cfg=(const TpostprocSettings*)cfg0;
#ifndef WIN64
 if (prepare(cfg,31,pict.frametype) && currentq)
  {
   init(pict,cfg->full,cfg->half);
   if (pictRect.dx>=16 && pictRect.dy>=16)
    {
     bool cspChanged=false;
     const unsigned char *tempPict1[4];
     cspChanged|=getCur(FF_CSPS_MASK_YUV_PLANAR,pict,cfg->full,tempPict1);
     unsigned char *tempPict2[4];
     cspChanged|=getNext(csp1,pict,cfg->full,tempPict2);
     if (cspChanged || old_sppMode!=cfg->sppMode)
      {
       old_sppMode=cfg->sppMode;
       done();
      }

     if (!temp)
      {
       switch (cfg->sppMode)
        {
         case 0:requantize=hardthresh_mmx;break;
         case 1:requantize=softthresh_mmx;break;
        }
      #ifdef __SSE2__
       if (Tconfig::cpu_flags&FF_CPU_SSE2)
        store_slice=TstoreSlice<Tsse2>::store_slice;
       else
      #endif
       if (Tconfig::cpu_flags&FF_CPU_MMX)
        store_slice=TstoreSlice<Tmmx>::store_slice;
       else
        store_slice=store_slice_c;

       unsigned int h=(dy1[0]+16+15)&(~15);
       temp_stride=(dx1[0]+16+15)&(~15);
       temp=(int16_t*)aligned_malloc(temp_stride*h*sizeof(int16_t));
       src=(uint8_t*)aligned_malloc(temp_stride*h*sizeof(uint8_t));
      }

     for (unsigned int i=0;i<pict.cspInfo.numPlanes;i++)
      filter(tempPict2[i],tempPict1[i],stride2[i],stride1[i],dx1[i],dy1[i],quants,quantsDx,i==0);
     if (Tconfig::cpu_flags&FF_CPU_MMX) _mm_empty();
     if (Tconfig::cpu_flags&FF_CPU_MMXEXT) _mm_sfence();
    }
  }
#endif
 return parent->deliverSample(++it,pict);
}

//============================== TimgFilterPostprocNic ===============================
// postprocessing routines by Nic (http://nic.dnsalias.com/)
TimgFilterPostprocNic::TimgFilterPostprocNic(IffdshowBase *Ideci,Tfilters *Iparent):TimgFilterPostprocBase(Ideci,Iparent,false)
{
 nic_dering=getNic_dering();
}

bool TimgFilterPostprocNic::is(const TffPictBase &pict,const TfilterSettingsVideo *cfg0)
{
 const TpostprocSettings *cfg=(const TpostprocSettings*)cfg0;
 if (super::is(pict,cfg) && Tlibmplayer::getPPmode(cfg,cfg->qual)&~(LUM_LEVEL_FIX|CHROM_LEVEL_FIX))
  {
   Trect r=pict.getRect(cfg->full,cfg->half);
   return pictRect.dx>=16 && pictRect.dy>=16;
  }
 else
  return false;
}

HRESULT TimgFilterPostprocNic::process(TfilterQueue::iterator it,TffPict &pict,const TfilterSettingsVideo *cfg0)
{
 const TpostprocSettings *cfg=(const TpostprocSettings*)cfg0;
 if (int ppmode=prepare(cfg,31,pict.frametype))
  {
   init(pict,cfg->full,cfg->half);
   if (pictRect.dx>=16 && pictRect.dy>=16)
    {
     unsigned char *tempPict[4];
     getCurNext(/*FF_CSPS_MASK_YUV_PLANAR*/FF_CSP_420P,pict,cfg->full,COPYMODE_DEF,tempPict);
     nic_postprocess(tempPict,stride2,
                     pict.cspInfo.shiftX[1],pict.cspInfo.shiftY[1],
                     dx1[0],dy1[0],
                     quants,quantsDx,ppmode,cfg->nicXthresh,cfg->nicYthresh);
     if (ppmode&LUM_DERING)
      nic_dering(tempPict[0],dx1[0],dy1[0],stride2[0],quants,quantsDx,pict.cspInfo.shiftX[0],pict.cspInfo.shiftY[0]);
     if (ppmode&CHROM_DERING)
      {
       nic_dering(tempPict[1],dx1[1],dy1[1],stride2[1],quants,quantsDx,pict.cspInfo.shiftX[1],pict.cspInfo.shiftY[1]);
       nic_dering(tempPict[2],dx1[2],dy1[2],stride2[2],quants,quantsDx,pict.cspInfo.shiftX[2],pict.cspInfo.shiftY[2]);
      }
    }
  }
 return parent->deliverSample(++it,pict);
}

//============================ TimgFilterPostprocFspp ===============================
void TimgFilterPostprocFspp::filter(uint8_t *dst, const uint8_t *src0,
		            stride_t dst_stride, stride_t src_stride,
		            int width, int height0,
		            int8_t *qp_store, int qp_stride, int is_luma)
{
    int x, x0, y, es, qy, t;
    const int stride= is_luma ? temp_stride : (width+16);//((width+16+15)&(~15))
    const int step=6-log2_count;
    const int qps= 3 + is_luma;
    int32_t __attribute__((aligned(32))) block_align[4*8*BLOCKSZ+ 4*8*BLOCKSZ];
    DCTELEM *block= (DCTELEM *)block_align;
    DCTELEM *block3=(DCTELEM *)(block_align+4*8*BLOCKSZ);

    memset(block3, 0, 4*8*BLOCKSZ);

    const int height = height0 & ~(step - 1);
    //src=src-src_stride*8-8;//!
    if (!src0 || !dst) return; // HACK avoid crash for Y8 colourspace
    for(y = 0 ; y < height0 ; y++){
        int index= 8 + 8 * stride + y * stride;
        memcpy(src + index, src0 + y * src_stride, width);//this line can be avoided by using DR & user fr.buffers
        for(x=0; x<8; x++){
            src[index         - x - 1]= src[index +         x    ];
            src[index + width + x    ]= src[index + width - x - 1];
        }
    }
    for(y = 0 ; y < 8 ; y++){
        memcpy(src + (          7 - y) * stride, src + (          y + 8) * stride, stride);
        memcpy(src + (height0 + 8 + y) * stride, src + (height0 - y + 7) * stride, stride);
    }
    //FIXME (try edge emu)

    for(y=8; y<24; y++)
        memset(temp+ 8 +y*stride, 0,width*sizeof(int16_t));

    for(y=step; y<height+8; y+=step){    //step= 1,2
        qy=y-4;
        if (qy>height-1) qy=height-1;
        if (qy<0) qy=0;
        qy=(qy>>qps)*qp_stride;
        row_fdct_s(block, src + y*stride +2-(y&1), stride, 2);
        for(x0=0; x0<width+8-8*(BLOCKSZ-1); x0+=8*(BLOCKSZ-1)){
            row_fdct_s(block+8*8, src + y*stride+8+x0 +2-(y&1), stride, 2*(BLOCKSZ-1));
            if(qp)
                column_fidct_s((int16_t*)(&threshold_mtx[0]), block+0*8, block3+0*8, 8*(BLOCKSZ-1)); //yes, this is a HOTSPOT
            else
                for (x=0; x<8*(BLOCKSZ-1); x+=8) {
                    t=x+x0-2; //correct t=x+x0-2-(y&1), but its the same
                    if (t<0) t=0;//t always < width-2
                    t=qp_store[qy+(t>>qps)];
                    //if(mpeg2) t>>=1; //copy mpeg2,prev_q to locals?
                    if (t!=prev_q) prev_q=t, mul_thrmat_s(t);
                    column_fidct_s((int16_t*)(&threshold_mtx[0]), block+x*8, block3+x*8, 8); //yes, this is a HOTSPOT
                }
            row_idct_s(block3+0*8, temp + (y&15)*stride+x0+2-(y&1), stride, 2*(BLOCKSZ-1));
            memcpy(block, block+(BLOCKSZ-1)*64, 8*8*sizeof(DCTELEM)); //cycling
            memcpy(block3, block3+(BLOCKSZ-1)*64, 6*8*sizeof(DCTELEM));
        }
        //
        es=width+8-x0; //  8, ...
        if (es>8)
            row_fdct_s(block+8*8, src + y*stride+8+x0 +2-(y&1), stride, (es-4)>>2);
        column_fidct_s((int16_t*)(&threshold_mtx[0]), block, block3, es-0);
        row_idct_s(block3+0*8, temp + (y&15)*stride+x0+2-(y&1), stride, es>>2);
        {const int y1=y-8+step;//l5-7  l4-6
            if (!(y1&7) && y1) {
                if (y1&8) store_slice_s(dst + (y1-8)*dst_stride, temp+ 8 +8*stride,
                                        dst_stride, stride, width, 8, 5-log2_count);
                else store_slice2_s(dst + (y1-8)*dst_stride, temp+ 8 +0*stride,
                                    dst_stride, stride, width, 8, 5-log2_count);
            } }
    }

    if (y&7) {  // == height & 7
        if (y&8) store_slice_s(dst + ((y-8)&~7)*dst_stride, temp+ 8 +8*stride,
                               dst_stride, stride, width, y&7, 5-log2_count);
        else store_slice2_s(dst + ((y-8)&~7)*dst_stride, temp+ 8 +0*stride,
                            dst_stride, stride, width, y&7, 5-log2_count);
    }

    for(; y < height0 + 8 ; y++){
        memcpy(dst+ (y - 8) * dst_stride, src0 + (y-8) * src_stride, width);
    }
}

//This func reads from 1 slice, 1 and clears 0 & 1
void TimgFilterPostprocFspp::store_slice_s(uint8_t *dst, int16_t *src_, stride_t dst_stride, stride_t src_stride, long width, long height, long log2_scale)
{
 uint8_t *src=(uint8_t*)src_;
    const uint8_t *od=&TimgFilterPostprocSpp::TstoreSlice<Tmmx>::dither[0][0];
    const uint8_t *end=&TimgFilterPostprocSpp::TstoreSlice<Tmmx>::dither[height][0];
    width = (width+7)&~7;
    dst_stride-=width;
    //src_stride=(src_stride-width)*2;
        int REG_d_=log2_scale;
        uint8_t *REG_S=src;
        uint8_t *REG_D=dst;
        stride_t REG_a=src_stride;
        __m64 mm5;
        csimd::movd( REG_d_, mm5             );
        REG_d_^=-1;
        stride_t REG_c=REG_a;
        REG_d_+=7;
        REG_a=-REG_a;
        REG_c-=width;
        REG_c+= REG_c;
        __m64 mm2;
        csimd::movd( REG_d_, mm2);
        src_stride=REG_c;
        const uint8_t *REG_d=od;
        REG_a<<=4;
        __m64 mm3,mm4,mm7,mm0,mm1;
       label2:
        csimd::movq (REG_d, mm3);
        csimd::movq (mm3, mm4 );
        csimd::pxor (mm7, mm7);
        csimd::punpcklbw( mm7, mm3 );
        csimd::punpckhbw( mm7, mm4);
        REG_c=width;
        csimd::psraw (mm5, mm3);
        csimd::psraw (mm5, mm4);
       label1:
        csimd::movq (mm7, REG_S+REG_a);
        csimd::movq (REG_S, mm0  );
        csimd::movq (8+REG_S, mm1);

        csimd::movq (mm7, 8+REG_S+REG_a);
        csimd::paddw (mm3, mm0);
        csimd::paddw (mm4, mm1);

        csimd::movq (mm7, REG_S);
        csimd::psraw (mm2, mm0);
        csimd::psraw (mm2, mm1);

        csimd::movq (mm7, 8+REG_S);
        csimd::packuswb (mm1, mm0);
        REG_S+=16;

        csimd::movq (mm0, REG_D);
        REG_D+=8;
        REG_c-=8;
        if (REG_c>0) //jg
         goto label1;//1b
        REG_S+=src_stride;
        REG_d+=8;
        REG_D+=dst_stride;
        if (end>REG_d)// cmp end, REG_d
         goto label2;//jl 2b
}

//This func reads from 2 slices, 0 & 2  and clears 2-nd
void TimgFilterPostprocFspp::store_slice2_s(uint8_t *dst, int16_t *src_, stride_t dst_stride, stride_t src_stride, long width, long height, long log2_scale)
{
 uint8_t *src=(uint8_t*)src_;
    const uint8_t *od=&TimgFilterPostprocSpp::TstoreSlice<Tmmx>::dither[0][0];
    const uint8_t *end=&TimgFilterPostprocSpp::TstoreSlice<Tmmx>::dither[height][0];
    width = (width+7)&~7;
    dst_stride-=width;
    //src_stride=(src_stride-width)*2;
        int REG_d_= log2_scale;
        uint8_t*REG_S= src;
        uint8_t*REG_D= dst;
        stride_t REG_a= src_stride;
        __m64 mm5;
        csimd::movd( REG_d_, mm5);
        REG_d_^=-1;
        stride_t REG_c=REG_a;
        REG_d_+=7;
        REG_c-=width;
        REG_c+=REG_c;
        __m64 mm2;
        csimd::movd (REG_d_, mm2);
        src_stride=REG_c;
        const uint8_t *REG_d= od;
        REG_a<<=5;
        __m64 mm3,mm4,mm7,mm0,mm1,mm6;

       label2:
        csimd::movq (REG_d, mm3);
        csimd::movq (mm3, mm4);
        csimd::pxor (mm7, mm7);
        csimd::punpcklbw (mm7, mm3);
        csimd::punpckhbw (mm7, mm4);
        REG_c=width;
        csimd::psraw (mm5, mm3);
        csimd::psraw (mm5, mm4);
       label1:
        csimd::movq (REG_S, mm0);
        csimd::movq (8+REG_S, mm1);
        csimd::paddw (mm3, mm0);

        csimd::paddw (REG_S+REG_a, mm0);
        csimd::paddw (mm4, mm1);
        csimd::movq (8+REG_S+REG_a, mm6);

        csimd::movq (mm7, REG_S+REG_a);
        csimd::psraw (mm2, mm0);
        csimd::paddw (mm6, mm1);

        csimd::movq (mm7, 8+REG_S+REG_a);
        csimd::psraw (mm2, mm1);
        csimd::packuswb (mm1, mm0);

        csimd::movq (mm0, REG_D);
        REG_S+=16;
        REG_D+=8;
        REG_c-=8;
        if (REG_c>0) //jg 1b
          goto label1;
        REG_S+=src_stride;
        REG_d+=8;
        REG_D+=dst_stride;
        if (end>REG_d)
          goto label2;//jl 2b
}

void TimgFilterPostprocFspp::mul_thrmat_s(int q)
{
    const uint64_t *adr=&threshold_mtx_noq[0];
        uint8_t *REG_D=(uint8_t*)adr;
        uint8_t *REG_S=(uint8_t*)adr;
	__m64 mm0,mm7,mm1,mm2,mm3,mm4,mm5,mm6;
	csimd::movd (q, mm7);
	REG_D+= 8*8*2;
        csimd::movq (0*8+REG_S, mm0);
        csimd::punpcklwd( mm7, mm7);
        csimd::movq (1*8+REG_S, mm1);
        csimd::punpckldq( mm7, mm7);
        csimd::pmullw (mm7, mm0);

        csimd::movq (2*8+REG_S, mm2);
        csimd::pmullw( mm7, mm1);

        csimd::movq (3*8+REG_S, mm3);
        csimd::pmullw (mm7, mm2);

        csimd::movq (mm0, 0*8+REG_D);
        csimd::movq (4*8+REG_S, mm4);
        csimd::pmullw (mm7, mm3);

        csimd::movq (mm1, 1*8+REG_D);
        csimd::movq (5*8+REG_S, mm5);
        csimd::pmullw( mm7, mm4);

        csimd::movq (mm2, 2*8+REG_D);
        csimd::movq (6*8+REG_S, mm6);
        csimd::pmullw( mm7, mm5);

        csimd::movq (mm3, 3*8+REG_D);
        csimd::movq (7*8+0*8+REG_S, mm0);
        csimd::pmullw(mm7, mm6 );

        csimd::movq (mm4, 4*8+REG_D);
        csimd::movq (7*8+1*8+REG_S, mm1);
        csimd::pmullw( mm7, mm0);

        csimd::movq (mm5, 5*8+REG_D);
        csimd::movq (7*8+2*8+REG_S, mm2);
        csimd::pmullw( mm7, mm1);

        csimd::movq (mm6, 6*8+REG_D);
        csimd::movq (7*8+3*8+REG_S, mm3);
        csimd::pmullw( mm7, mm2);

        csimd::movq (mm0, 7*8+0*8+REG_D);
        csimd::movq (7*8+4*8+REG_S, mm4);
        csimd::pmullw( mm7, mm3);

        csimd::movq (mm1, 7*8+1*8+REG_D);
        csimd::movq (7*8+5*8+REG_S, mm5);
        csimd::pmullw( mm7, mm4);

        csimd::movq (mm2, 7*8+2*8+REG_D);
        csimd::movq (7*8+6*8+REG_S, mm6 );
        csimd::pmullw( mm7, mm5);

        csimd::movq (mm3, 7*8+3*8+REG_D);
        csimd::movq (14*8+0*8+REG_S, mm0);
        csimd::pmullw( mm7, mm6);

        csimd::movq (mm4, 7*8+4*8+REG_D);
        csimd::movq (14*8+1*8+REG_S, mm1);
        csimd::pmullw (mm7, mm0);

        csimd::movq (mm5, 7*8+5*8+REG_D);
        csimd::pmullw (mm7, mm1);

        csimd::movq (mm6, 7*8+6*8+REG_D);
        csimd::movq (mm0, 14*8+0*8+REG_D);
        csimd::movq (mm1, 14*8+1*8+REG_D);
}

#define FIX(x,s)  ((int) ((x) * (1<<s) + 0.5)&0xffff)
#define C64(x)    ((uint64_t)((x)|(x)<<16))<<32 | (uint64_t)(x) | (uint64_t)(x)<<16
#define FIX64(x,s)  C64(FIX(x,s))

const __int64 TimgFilterPostprocFspp::MM_FIX_0_382683433=FIX64(0.382683433, 14);
const __int64 TimgFilterPostprocFspp::MM_FIX_0_541196100=FIX64(0.541196100, 14);
const __int64 TimgFilterPostprocFspp::MM_FIX_0_707106781=FIX64(0.707106781, 14);
const __int64 TimgFilterPostprocFspp::MM_FIX_1_306562965=FIX64(1.306562965, 14);

const __int64 TimgFilterPostprocFspp::MM_FIX_1_414213562_A=FIX64(1.414213562, 14);

const __int64 TimgFilterPostprocFspp::MM_FIX_1_847759065=FIX64(1.847759065, 13);
const __int64 TimgFilterPostprocFspp::MM_FIX_2_613125930=FIX64(-2.613125930, 13); //-
const __int64 TimgFilterPostprocFspp::MM_FIX_1_414213562=FIX64(1.414213562, 13);
const __int64 TimgFilterPostprocFspp::MM_FIX_1_082392200=FIX64(1.082392200, 13);
const __int64 TimgFilterPostprocFspp::MM_FIX_0_847759065=FIX64(0.847759065, 14);
const __int64 TimgFilterPostprocFspp::MM_FIX_0_566454497=FIX64(0.566454497, 14);
const __int64 TimgFilterPostprocFspp::MM_FIX_0_198912367=FIX64(0.198912367, 14);

const __int64 TimgFilterPostprocFspp::MM_DESCALE_RND={C64(4)};
const __int64 TimgFilterPostprocFspp::MM_2={C64(2)};

#undef FIX
#undef C64
#undef FIX64

void TimgFilterPostprocFspp::column_fidct_s(int16_t* thr_adr_,  DCTELEM *data_,  DCTELEM *output_,  int cnt)
{
__m64 mm0,mm1,mm2,mm3,mm4,mm5,mm6,mm7;

uint8_t *REG_S=(uint8_t*)data_;
uint8_t *REG_D=(uint8_t*)output_;
int REG_c=cnt;
uint8_t *REG_d=(uint8_t*)thr_adr_;

__m64 temps[4];
int REG_a;

     label1:
        csimd::movq( DCTSIZE_S*0*2+REG_S, mm1 );
        //
        csimd::movq( DCTSIZE_S*3*2+REG_S, mm7 );
        csimd::movq( mm1, mm0             );

        csimd::paddw( DCTSIZE_S*7*2+REG_S, mm1);  //t0
        csimd::movq( mm7, mm3             );

        csimd::paddw( DCTSIZE_S*4*2+REG_S, mm7);  //t3
        csimd::movq( mm1, mm5             );

        csimd::movq( DCTSIZE_S*1*2+REG_S, mm6 );
        csimd::psubw (mm7, mm1 );            //t13

        csimd::movq( DCTSIZE_S*2*2+REG_S, mm2 );
        csimd::movq( mm6, mm4             );

        csimd::paddw( DCTSIZE_S*6*2+REG_S, mm6 ); //t1
        csimd::paddw( mm7, mm5           );  //t10

        csimd::paddw( DCTSIZE_S*5*2+REG_S, mm2);  //t2
        csimd::movq( mm6, mm7 );

        csimd::paddw (mm2, mm6);             //t11
        csimd::psubw( mm2, mm7);             //t12

        csimd::movq( mm5, mm2 );
        csimd::paddw( mm6, mm5);             //d0
        // i0 t13 t12 i3 i1 d0 - d4
        csimd::psubw( mm6, mm2);             //d4
        csimd::paddw( mm1, mm7);

        csimd::movq(  4*16+REG_d, mm6);
        csimd::psllw (2, mm7 );

        csimd::psubw (0*16+REG_d, mm5      );
        csimd::psubw (mm6, mm2            );

        csimd::paddusw (0*16+REG_d, mm5    );
        csimd::paddusw (mm6, mm2          );

        csimd::pmulhw (MM_FIX_0_707106781, mm7 );
        //
        csimd::paddw (0*16+REG_d, mm5);
        csimd::paddw (mm6, mm2            );

        csimd::psubusw( 0*16+REG_d, mm5    );
        csimd::psubusw( mm6, mm2          );

//This func is totally compute-bound,  operates at huge speed. So,  DC shortcut
// at this place isn't worthwhile due to BTB miss penalty (checked on Pent. 3).
//However,  typical numbers: nondc - 29,  dc - 46,  zero - 25. All <> 0 case is very rare.
        csimd::paddw (MM_2, mm5 );
        csimd::movq( mm2, mm6  );

        csimd::paddw( mm5, mm2);
        csimd::psubw( mm6, mm5 );

        csimd::movq( mm1, mm6);
        csimd::paddw (mm7, mm1 );            //d2

        csimd::psubw (2*16+REG_d, mm1);
        csimd::psubw (mm7, mm6       );      //d6

        csimd::movq( 6*16+REG_d, mm7  );
        csimd::psraw (2, mm5           );

        csimd::paddusw(2*16+REG_d, mm1);
        csimd::psubw(mm7, mm6            );
        // t7 d2 /t11 t4 t6 - d6 /t10

        csimd::paddw(2*16+REG_d, mm1      );
        csimd::paddusw(mm7, mm6          );

        csimd::psubusw(2*16+REG_d, mm1    );
        csimd::paddw(mm7, mm6            );

        csimd::psubw(DCTSIZE_S*4*2+REG_S, mm3 );
        csimd::psubusw(mm7, mm6          );

        //csimd::movq( [edi+DCTSIZE_S*2*2], mm1
        //csimd::movq( [edi+DCTSIZE_S*6*2], mm6
        csimd::movq( mm1, mm7             );
        csimd::psraw (2, mm2              );

        csimd::psubw(DCTSIZE_S*6*2+REG_S, mm4 );
        csimd::psubw(mm6, mm1            );

        csimd::psubw(DCTSIZE_S*7*2+REG_S, mm0 );
        csimd::paddw(mm7, mm6            ); //'t13

        csimd::psraw (2, mm6              ); //csimd::paddw(mm6, MM_2 !!    ---
        csimd::movq( mm2, mm7              );

        csimd::pmulhw (MM_FIX_1_414213562_A, mm1 );
        csimd::paddw(mm6, mm2        );     //'t0

        csimd::movq( mm2, temps[0]  );      //!
        csimd::psubw(mm6, mm7          );   //'t3

        csimd::movq( DCTSIZE_S*2*2+REG_S, mm2 );
        csimd::psubw(mm6, mm1         );    //'t12

        csimd::psubw(DCTSIZE_S*5*2+REG_S, mm2);  //t5
        csimd::movq( mm5, mm6             );

        csimd::movq( mm7, temps[3]       );
        csimd::paddw(mm2, mm3            ); //t10

        csimd::paddw(mm4, mm2            ); //t11
        csimd::paddw(mm0, mm4             );//t12

        csimd::movq( mm3, mm7             );
        csimd::psubw(mm4, mm3            );

        csimd::psllw(2, mm3              );
        csimd::psllw(2, mm7              ); //opt for P6

        csimd::pmulhw (MM_FIX_0_382683433, mm3 );
        csimd::psllw(2, mm4              );

        csimd::pmulhw (MM_FIX_0_541196100, mm7 );
        csimd::psllw(2, mm2              );

        csimd::pmulhw (MM_FIX_1_306562965, mm4 );
        csimd::paddw(mm1, mm5         );    //'t1

        csimd::pmulhw (MM_FIX_0_707106781, mm2 );
        csimd::psubw(mm1, mm6           );  //'t2
        // t7 't12 't11 t4 t6 - 't13 't10   ---

        csimd::paddw(mm3, mm7     );        //z2

        csimd::movq( mm5, temps[1]);
        csimd::paddw(mm3, mm4        );     //z4

        csimd::movq( 3*16+REG_d, mm3   );
        csimd::movq( mm0, mm1          );

        csimd::movq( mm6, temps[2]  );
        csimd::psubw(mm2, mm1           );  //z13

//===
        csimd::paddw(mm2, mm0           );  //z11
        csimd::movq( mm1, mm5             );

        csimd::movq( 5*16+REG_d, mm2       );
        csimd::psubw(mm7, mm1             );//d3

        csimd::paddw(mm7, mm5             );//d5
        csimd::psubw(mm3, mm1              );

        csimd::movq( 1*16+REG_d, mm7         );
        csimd::psubw(mm2, mm5            );

        csimd::movq( mm0, mm6             );
        csimd::paddw(mm4, mm0             );//d1

        csimd::paddusw(mm3, mm1        );
        csimd::psubw(mm4, mm6           );  //d7

        // d1 d3 - - - d5 d7 -
        csimd::movq( 7*16+REG_d, mm4  );
        csimd::psubw(mm7, mm0         );

        csimd::psubw(mm4, mm6       );
        csimd::paddusw(mm2, mm5    );

        csimd::paddusw(mm4, mm6  );
        csimd::paddw(mm3, mm1   );

        csimd::paddw(mm2, mm5  );
        csimd::paddw(mm4, mm6  );

        csimd::psubusw(mm3, mm1 );
        csimd::psubusw(mm2, mm5  );

        csimd::psubusw(mm4, mm6);
        csimd::movq( mm1, mm4 );

        csimd::por( mm5, mm4  );
        csimd::paddusw(mm7, mm0 );

        csimd::por( mm6, mm4   );
        csimd::paddw(mm7, mm0  );

        csimd::packssdw(mm4, mm4);
        csimd::psubusw(mm7, mm0 );

        csimd::movd (mm4, REG_a             );
        if (REG_a)    //or REG_a, REG_a
         goto label2; //jnz 2f


        //csimd::movq( [edi+DCTSIZE_S*3*2], mm1
        //csimd::movq( [edi+DCTSIZE_S*5*2], mm5
        //csimd::movq( [edi+DCTSIZE_S*1*2], mm0
        //csimd::movq( [edi+DCTSIZE_S*7*2], mm6
        // t4 t5 - - - t6 t7 -
        //--- t4 (mm0) may be <>0; mm1, mm5, mm6 == 0
//Typical numbers: nondc - 19,  dc - 26,  zero - 55. zero case alone isn't worthwhile
        csimd::movq( temps[0], mm4);
        csimd::movq( mm0, mm1       );

        csimd::pmulhw (MM_FIX_0_847759065, mm0);  //tmp6
        csimd::movq( mm1, mm2      );

        csimd::movq( DCTSIZE_S*0*2+REG_D, mm5);
        csimd::movq( mm2, mm3        );

        csimd::pmulhw (MM_FIX_0_566454497, mm1 ); //tmp5
        csimd::paddw(mm4, mm5               );

        csimd::movq( temps[1], mm6 );
        //csimd::paddw(mm3, MM_2
        csimd::psraw( 2, mm3 );              //tmp7

        csimd::pmulhw (MM_FIX_0_198912367, mm2 ); //-tmp4
        csimd::psubw(mm3, mm4            );

        csimd::movq( DCTSIZE_S*1*2+REG_D, mm7);
        csimd::paddw(mm3, mm5              );

        csimd::movq( mm4, DCTSIZE_S*7*2+REG_D );
        csimd::paddw(mm6, mm7               );

        csimd::movq( temps[2], mm3    );
        csimd::psubw(mm0, mm6            );

        csimd::movq( DCTSIZE_S*2*2+REG_D, mm4 );
        csimd::paddw(mm0, mm7                );

        csimd::movq( mm5, DCTSIZE_S*0*2+REG_D );
        csimd::paddw(mm3, mm4                );

        csimd::movq( mm6, DCTSIZE_S*6*2+REG_D );
        csimd::psubw(mm1, mm3                );

        csimd::movq( DCTSIZE_S*5*2+REG_D, mm5 );
        csimd::paddw(mm1, mm4                 );

        csimd::movq( DCTSIZE_S*3*2+REG_D, mm6 );
        csimd::paddw(mm3, mm5                 );

        csimd::movq( temps[3], mm0    );
        REG_S+=8;

        csimd::movq( mm7, DCTSIZE_S*1*2+REG_D );
        csimd::paddw(mm0, mm6           );

        csimd::movq( mm4, DCTSIZE_S*2*2+REG_D );
        csimd::psubw(mm2, mm0             );

        csimd::movq( DCTSIZE_S*4*2+REG_D, mm7);
        csimd::paddw(mm2, mm6            );

        csimd::movq( mm5, DCTSIZE_S*5*2+REG_D);
        csimd::paddw(mm0, mm7         );

        csimd::movq( mm6, DCTSIZE_S*3*2+REG_D);

        csimd::movq( mm7, DCTSIZE_S*4*2+REG_D);
        REG_D               +=8;
        goto label4;

       label2:
        //--- non DC2
        //csimd::psraw mm1, 2 w/o it -> offset. thr1, thr1, thr1  (actually thr1, thr1, thr1-1)
        //csimd::psraw mm5, 2
        //csimd::psraw mm0, 2
        //csimd::psraw mm6, 2
        csimd::movq( mm5, mm3 );
        csimd::psubw(mm1, mm5 );

        csimd::psllw(1, mm5   );            //'z10
        csimd::paddw(mm1, mm3 );            //'z13

        csimd::movq( mm0, mm2 );
        csimd::psubw(mm6, mm0 );

        csimd::movq( mm5, mm1  );
        csimd::psllw(1, mm0   );            //'z12

        csimd::pmulhw (MM_FIX_2_613125930, mm1);  //-
        csimd::paddw(mm0, mm5 );

        csimd::pmulhw (MM_FIX_1_847759065, mm5);  //'z5
        csimd::paddw(mm6, mm2   );          //'z11

        csimd::pmulhw (MM_FIX_1_082392200, mm0 );
        csimd::movq( mm2, mm7 );

        //---
        csimd::movq( temps[0], mm4 );
        csimd::psubw(mm3, mm2 );

        csimd::psllw(1, mm2  );
        csimd::paddw(mm3, mm7);             //'t7

        csimd::pmulhw (MM_FIX_1_414213562, mm2);  //'t11
        csimd::movq( mm4, mm6    );
        //csimd::paddw(mm7, MM_2
        csimd::psraw (2, mm7  );

        csimd::paddw(DCTSIZE_S*0*2+REG_D, mm4 );
        csimd::psubw(mm7, mm6 );

        csimd::movq( temps[1], mm3 );
        csimd::paddw(mm7, mm4   );

        csimd::movq( mm6, DCTSIZE_S*7*2+REG_D );
        csimd::paddw(mm5, mm1        );     //'t12

        csimd::movq( mm4, DCTSIZE_S*0*2+REG_D  );
        csimd::psubw(mm7, mm1    );         //'t6

        csimd::movq( temps[2], mm7);
        csimd::psubw(mm5, mm0      );       //'t10

        csimd::movq( temps[3], mm6 );
        csimd::movq( mm3, mm5     );

        csimd::paddw(DCTSIZE_S*1*2+REG_D, mm3);
        csimd::psubw(mm1, mm5 );

        csimd::psubw(mm1, mm2 );            //'t5
        csimd::paddw(mm1, mm3);

        csimd::movq( mm5, DCTSIZE_S*6*2+REG_D);
        csimd::movq( mm7, mm4  );

        csimd::paddw(DCTSIZE_S*2*2+REG_D, mm7);
        csimd::psubw(mm2, mm4  );

        csimd::paddw(DCTSIZE_S*5*2+REG_D, mm4 );
        csimd::paddw(mm2, mm7  );

        csimd::movq( mm3, DCTSIZE_S*1*2+REG_D);
        csimd::paddw(mm2, mm0 );            //'t4

        // 't4 't6 't5 - - - - 't7
        csimd::movq( mm7, DCTSIZE_S*2*2+REG_D );
        csimd::movq( mm6, mm1               );

        csimd::paddw(DCTSIZE_S*4*2+REG_D, mm6);
        csimd::psubw(mm0, mm1                );

        csimd::paddw(DCTSIZE_S*3*2+REG_D, mm1);
        csimd::paddw(mm0, mm6                );

        csimd::movq( mm4, DCTSIZE_S*5*2+REG_D);
        REG_S               +=8;

        csimd::movq( mm6, DCTSIZE_S*4*2+REG_D );

        csimd::movq( mm1, DCTSIZE_S*3*2+REG_D );
        REG_D               +=8;

       label4:
//=part 2 (the same)===========================================================
        csimd::movq( DCTSIZE_S*0*2+REG_S, mm1);
        //
        csimd::movq( DCTSIZE_S*3*2+REG_S, mm7 );
        csimd::movq( mm1, mm0 );

        csimd::paddw(DCTSIZE_S*7*2+REG_S, mm1 ); //t0
        csimd::movq( mm7, mm3 );

        csimd::paddw(DCTSIZE_S*4*2+REG_S, mm7);  //t3
        csimd::movq( mm1, mm5);

        csimd::movq( DCTSIZE_S*1*2+REG_S, mm6 );
        csimd::psubw(mm7, mm1);             //t13

        csimd::movq( DCTSIZE_S*2*2+REG_S, mm2);
        csimd::movq( mm6, mm4  );

        csimd::paddw(DCTSIZE_S*6*2+REG_S, mm6 ); //t1
        csimd::paddw(mm7, mm5  );           //t10

        csimd::paddw(DCTSIZE_S*5*2+REG_S, mm2);  //t2
        csimd::movq( mm6, mm7);

        csimd::paddw(mm2, mm6);             //t11
        csimd::psubw(mm2, mm7 );            //t12

        csimd::movq( mm5, mm2 );
        csimd::paddw(mm6, mm5 );            //d0
        // i0 t13 t12 i3 i1 d0 - d4
        csimd::psubw(mm6, mm2);             //d4
        csimd::paddw(mm1, mm7);

        csimd::movq(  1*8+4*16+REG_d, mm6);
        csimd::psllw(2, mm7 );

        csimd::psubw(1*8+0*16+REG_d, mm5 );
        csimd::psubw(mm6, mm2 );

        csimd::paddusw(1*8+0*16+REG_d, mm5 );
        csimd::paddusw(mm6, mm2);

        csimd::pmulhw (MM_FIX_0_707106781, mm7 );
        //
        csimd::paddw(1*8+0*16+REG_d, mm5 );
        csimd::paddw(mm6, mm2   );

        csimd::psubusw(1*8+0*16+REG_d, mm5);
        csimd::psubusw(mm6, mm2);

//This func is totally compute-bound,  operates at huge speed. So,  DC shortcut
// at this place isn't worthwhile due to BTB miss penalty (checked on Pent. 3).
//However,  typical numbers: nondc - 29,  dc - 46,  zero - 25. All <> 0 case is very rare.
        csimd::paddw((MM_2), mm5  );
        csimd::movq( mm2, mm6);

        csimd::paddw(mm5, mm2);
        csimd::psubw(mm6, mm5 );

        csimd::movq( mm1, mm6 );
        csimd::paddw(mm7, mm1 );            //d2

        csimd::psubw(1*8+2*16+REG_d, mm1);
        csimd::psubw(mm7, mm6);             //d6

        csimd::movq( 1*8+6*16+REG_d, mm7);
        csimd::psraw (2, mm5 );

        csimd::paddusw(1*8+2*16+REG_d, mm1);
        csimd::psubw(mm7, mm6);
        // t7 d2 /t11 t4 t6 - d6 /t10

        csimd::paddw(1*8+2*16+REG_d, mm1);
        csimd::paddusw(mm7, mm6 );

        csimd::psubusw(1*8+2*16+REG_d, mm1 );
        csimd::paddw(mm7, mm6  );

        csimd::psubw(DCTSIZE_S*4*2+REG_S, mm3);
        csimd::psubusw(mm7, mm6 );

        //csimd::movq( [edi+DCTSIZE_S*2*2], mm1
        //csimd::movq( [edi+DCTSIZE_S*6*2], mm6
        csimd::movq( mm1, mm7  );
        csimd::psraw (2, mm2 );

        csimd::psubw(DCTSIZE_S*6*2+REG_S, mm4 );
        csimd::psubw(mm6, mm1            );

        csimd::psubw(DCTSIZE_S*7*2+REG_S, mm0 );
        csimd::paddw(mm7, mm6  );           //'t13

        csimd::psraw (2, mm6 );              //csimd::paddw(mm6, MM_2 !!    ---
        csimd::movq( mm2, mm7);

        csimd::pmulhw (MM_FIX_1_414213562_A, mm1 );
        csimd::paddw(mm6, mm2 );            //'t0

        csimd::movq( mm2, temps[0] );       //!
        csimd::psubw(mm6, mm7     );        //'t3

        csimd::movq( DCTSIZE_S*2*2+REG_S, mm2);
        csimd::psubw(mm6, mm1 );            //'t12

        csimd::psubw(DCTSIZE_S*5*2+REG_S, mm2);  //t5
        csimd::movq( mm5, mm6   );

        csimd::movq( mm7, temps[3]  );
        csimd::paddw(mm2, mm3);             //t10

        csimd::paddw(mm4, mm2);             //t11
        csimd::paddw(mm0, mm4);             //t12

        csimd::movq( mm3, mm7);
        csimd::psubw(mm4, mm3);

        csimd::psllw(2, mm3);
        csimd::psllw(2, mm7);               //opt for P6

        csimd::pmulhw (MM_FIX_0_382683433, mm3 );
        csimd::psllw(2, mm4  );

        csimd::pmulhw (MM_FIX_0_541196100, mm7 );
        csimd::psllw(2, mm2);

        csimd::pmulhw (MM_FIX_1_306562965, mm4 );
        csimd::paddw(mm1, mm5);             //'t1

        csimd::pmulhw (MM_FIX_0_707106781, mm2 );
        csimd::psubw(mm1, mm6 );            //'t2
        // t7 't12 't11 t4 t6 - 't13 't10   ---

        csimd::paddw(mm3, mm7 );            //z2

        csimd::movq( mm5, temps[1]       );
        csimd::paddw(mm3, mm4 );            //z4

        csimd::movq( 1*8+3*16+REG_d, mm3 );
        csimd::movq( mm0, mm1 );

        csimd::movq( mm6, temps[2]);
        csimd::psubw(mm2, mm1  );           //z13

//===
        csimd::paddw(mm2, mm0 );            //z11
        csimd::movq( mm1, mm5);

        csimd::movq( 1*8+5*16+REG_d, mm2);
        csimd::psubw(mm7, mm1);             //d3

        csimd::paddw(mm7, mm5);             //d5
        csimd::psubw(mm3, mm1);

        csimd::movq( 1*8+1*16+REG_d, mm7);
        csimd::psubw(mm2, mm5);

        csimd::movq( mm0, mm6);
        csimd::paddw(mm4, mm0);             //d1

        csimd::paddusw(mm3, mm1 );
        csimd::psubw(mm4, mm6);             //d7

        // d1 d3 - - - d5 d7 -
        csimd::movq( 1*8+7*16+REG_d, mm4);
        csimd::psubw(mm7, mm0);

        csimd::psubw(mm4, mm6);
        csimd::paddusw(mm2, mm5 );

        csimd::paddusw(mm4, mm6 );
        csimd::paddw(mm3, mm1);

        csimd::paddw(mm2, mm5);
        csimd::paddw(mm4, mm6 );

        csimd::psubusw(mm3, mm1 );
        csimd::psubusw(mm2, mm5 );

        csimd::psubusw(mm4, mm6 );
        csimd::movq( mm1, mm4);

        csimd::por( mm5, mm4 );
        csimd::paddusw(mm7, mm0 );

        csimd::por( mm6, mm4 );
        csimd::paddw(mm7, mm0 );

        csimd::packssdw(mm4, mm4 );
        csimd::psubusw(mm7, mm0);

        csimd::movd (mm4, REG_a);
        if (REG_a) //or REG_a, REG_a
          goto label3;//jnz 3f
        //csimd::movq( [edi+DCTSIZE_S*3*2], mm1
        //csimd::movq( [edi+DCTSIZE_S*5*2], mm5
        //csimd::movq( [edi+DCTSIZE_S*1*2], mm0
        //csimd::movq( [edi+DCTSIZE_S*7*2], mm6
        // t4 t5 - - - t6 t7 -
        //--- t4 (mm0) may be <>0; mm1, mm5, mm6 == 0
//Typical numbers: nondc - 19,  dc - 26,  zero - 55. zero case alone isn't worthwhile
        csimd::movq( temps[0], mm4);
        csimd::movq( mm0, mm1   );

        csimd::pmulhw (MM_FIX_0_847759065, mm0);  //tmp6
        csimd::movq( mm1, mm2 );

        csimd::movq( DCTSIZE_S*0*2+REG_D, mm5);
        csimd::movq( mm2, mm3);

        csimd::pmulhw (MM_FIX_0_566454497, mm1);  //tmp5
        csimd::paddw(mm4, mm5);

        csimd::movq( temps[1], mm6);
        //csimd::paddw(mm3, MM_2
        csimd::psraw (2, mm3 );              //tmp7

        csimd::pmulhw (MM_FIX_0_198912367, mm2);  //-tmp4
        csimd::psubw(mm3, mm4  );

        csimd::movq( DCTSIZE_S*1*2+REG_D, mm7 );
        csimd::paddw(mm3, mm5       );

        csimd::movq( mm4, DCTSIZE_S*7*2+REG_D   );
        csimd::paddw(mm6, mm7             );

        csimd::movq( temps[2], mm3 );
        csimd::psubw(mm0, mm6    );

        csimd::movq( DCTSIZE_S*2*2+REG_D, mm4 );
        csimd::paddw(mm0, mm7   );

        csimd::movq( mm5, DCTSIZE_S*0*2+REG_D );
        csimd::paddw(mm3, mm4              );

        csimd::movq( mm6, DCTSIZE_S*6*2+REG_D );
        csimd::psubw(mm1, mm3                );

        csimd::movq( DCTSIZE_S*5*2+REG_D, mm5 );
        csimd::paddw(mm1, mm4              );

        csimd::movq( DCTSIZE_S*3*2+REG_D, mm6 );
        csimd::paddw(mm3, mm5          );

        csimd::movq( temps[3], mm0 );
        REG_S+=24;

        csimd::movq( mm7, DCTSIZE_S*1*2+REG_D  );
        csimd::paddw(mm0, mm6  );

        csimd::movq( mm4, DCTSIZE_S*2*2+REG_D  );
        csimd::psubw(mm2, mm0   );

        csimd::movq( DCTSIZE_S*4*2+REG_D, mm7);
        csimd::paddw(mm2, mm6            );

        csimd::movq( mm5, DCTSIZE_S*5*2+REG_D);
        csimd::paddw(mm0, mm7         );

        csimd::movq( mm6, DCTSIZE_S*3*2+REG_D );

        csimd::movq( mm7, DCTSIZE_S*4*2+REG_D);
        REG_D+=24;
        REG_c-=2;
        if (REG_c) goto label1;//jnz 1b
        goto label5;

       label3:
        //--- non DC2
        //csimd::psraw mm1, 2 w/o it -> offset. thr1, thr1, thr1  (actually thr1, thr1, thr1-1)
        //csimd::psraw mm5, 2
        //csimd::psraw mm0, 2
        //csimd::psraw mm6, 2
        csimd::movq( mm5, mm3 );
        csimd::psubw(mm1, mm5);

        csimd::psllw(1, mm5 );              //'z10
        csimd::paddw(mm1, mm3 );            //'z13

        csimd::movq( mm0, mm2);
        csimd::psubw(mm6, mm0 );

        csimd::movq( mm5, mm1);
        csimd::psllw(1, mm0 );              //'z12

        csimd::pmulhw (MM_FIX_2_613125930, mm1);  //-
        csimd::paddw(mm0, mm5  );

        csimd::pmulhw (MM_FIX_1_847759065, mm5);  //'z5
        csimd::paddw(mm6, mm2  );           //'z11

        csimd::pmulhw (MM_FIX_1_082392200, mm0 );
        csimd::movq( mm2, mm7 );

        //---
        csimd::movq( temps[0], mm4 );
        csimd::psubw(mm3, mm2 );

        csimd::psllw(1, mm2  );
        csimd::paddw(mm3, mm7 );            //'t7

        csimd::pmulhw (MM_FIX_1_414213562, mm2);  //'t11
        csimd::movq( mm4, mm6 );
        //csimd::paddw(mm7, MM_2
        csimd::psraw( 2, mm7 );

        csimd::paddw(DCTSIZE_S*0*2+REG_D, mm4);
        csimd::psubw(mm7, mm6     );

        csimd::movq( temps[1], mm3);
        csimd::paddw(mm7, mm4     );

        csimd::movq( mm6, DCTSIZE_S*7*2+REG_D );
        csimd::paddw(mm5, mm1          );   //'t12

        csimd::movq( mm4, DCTSIZE_S*0*2+REG_D );
        csimd::psubw(mm7, mm1         );    //'t6

        csimd::movq( temps[2], mm7);
        csimd::psubw(mm5, mm0      );       //'t10

        csimd::movq( temps[3], mm6 );
        csimd::movq( mm3, mm5       );

        csimd::paddw(DCTSIZE_S*1*2+REG_D, mm3 );
        csimd::psubw(mm1, mm5    );

        csimd::psubw(mm1, mm2  );           //'t5
        csimd::paddw(mm1, mm3 );

        csimd::movq( mm5, DCTSIZE_S*6*2+REG_D );
        csimd::movq( mm7, mm4             );

        csimd::paddw(DCTSIZE_S*2*2+REG_D, mm7);
        csimd::psubw(mm2, mm4               );

        csimd::paddw(DCTSIZE_S*5*2+REG_D, mm4);
        csimd::paddw(mm2, mm7                );

        csimd::movq( mm3, DCTSIZE_S*1*2+REG_D );
        csimd::paddw(mm2, mm0         );    //'t4

        // 't4 't6 't5 - - - - 't7
        csimd::movq( mm7, DCTSIZE_S*2*2+REG_D);
        csimd::movq( mm6, mm1              );

        csimd::paddw(DCTSIZE_S*4*2+REG_D, mm6);
        csimd::psubw(mm0, mm1               );

        csimd::paddw(DCTSIZE_S*3*2+REG_D, mm1);
        csimd::paddw(mm0, mm6               );

        csimd::movq( mm4, DCTSIZE_S*5*2+REG_D);
        REG_S+=24;

        csimd::movq( mm6, DCTSIZE_S*4*2+REG_D);

        csimd::movq( mm1, DCTSIZE_S*3*2+REG_D);
        REG_D+=24;
        REG_c-=2;
        if (REG_c) goto label1;// jnz 1b
       label5:;
}

void TimgFilterPostprocFspp::row_idct_s (DCTELEM* workspace_,  int16_t* output_adr_,  int output_stride,  int cnt)
{
int8_t *REG_S=(int8_t*)workspace_;
int8_t *REG_D=(int8_t*)output_adr_;
int REG_c=cnt;
int REG_a=output_stride*sizeof(short);
int REG_d=REG_a+REG_a*2;
        __m64 mm0,mm1,mm2,mm3,mm4,mm5,mm6,mm7;
__m64 temps[4];

       label1:
        csimd::movq( DCTSIZE_S*0*2+REG_S, mm0 );
        //

        csimd::movq( DCTSIZE_S*1*2+REG_S, mm1 );
        csimd::movq( mm0, mm4              );

        csimd::movq( DCTSIZE_S*2*2+REG_S, mm2   );
        csimd::punpcklwd( mm1, mm0          );

        csimd::movq( DCTSIZE_S*3*2+REG_S, mm3 );
        csimd::punpckhwd( mm1, mm4         );

        //transpose 4x4
        csimd::movq( mm2, mm7 );
        csimd::punpcklwd (mm3, mm2  );

        csimd::movq( mm0, mm6   );
        csimd::punpckldq (mm2, mm0);         //0

        csimd::punpckhdq (mm2, mm6 );        //1
        csimd::movq( mm0, mm5   );

        csimd::punpckhwd (mm3, mm7);
        csimd::psubw( mm6, mm0   );

        csimd::pmulhw( (MM_FIX_1_414213562_A), mm0 );
        csimd::movq( mm4, mm2    );

        csimd::punpckldq( mm7, mm4);         //2
        csimd::paddw( mm6, mm5  );

        csimd::punpckhdq( mm7, mm2);         //3
        csimd::movq( mm4, mm1 );

        csimd::psllw( 2, mm0  );
        csimd::paddw( mm2, mm4);             //t10

        csimd::movq( DCTSIZE_S*0*2+DCTSIZE_S+REG_S, mm3 );
        csimd::psubw( mm2, mm1 );            //t11

        csimd::movq( DCTSIZE_S*1*2+DCTSIZE_S+REG_S, mm2 );
        csimd::psubw( mm5, mm0);

        csimd::movq( mm4, mm6);
        csimd::paddw( mm5, mm4 );            //t0

        csimd::psubw( mm5, mm6 );            //t3
        csimd::movq( mm1, mm7 );

        csimd::movq( DCTSIZE_S*2*2+DCTSIZE_S+REG_S, mm5 );
        csimd::paddw( mm0, mm1   );          //t1

        csimd::movq( mm4, temps[0] );       //t0
        csimd::movq( mm3, mm4);

        csimd::movq( mm6, temps[1]  );      //t3
        csimd::punpcklwd( mm2, mm3  );

        //transpose 4x4
        csimd::movq( DCTSIZE_S*3*2+DCTSIZE_S+REG_S, mm6 );
        csimd::punpckhwd( mm2, mm4   );

        csimd::movq( mm5, mm2    );
        csimd::punpcklwd( mm6, mm5 );

        csimd::psubw( mm0, mm7 );            //t2
        csimd::punpckhwd( mm6, mm2 );

        csimd::movq( mm3, mm0 );
        csimd::punpckldq( mm5, mm3);         //4

        csimd::punpckhdq( mm5, mm0);         //5
        csimd::movq( mm4, mm5  );

        //
        csimd::movq( mm3, mm6  );
        csimd::punpckldq( mm2, mm4 );        //6

        csimd::psubw( mm0, mm3 );            //z10
        csimd::punpckhdq( mm2, mm5);         //7

        csimd::paddw( mm0, mm6 );            //z13
        csimd::movq( mm4, mm2  );

        csimd::movq( mm3, mm0 );
        csimd::psubw( mm5, mm4);             //z12

        csimd::pmulhw( (MM_FIX_2_613125930), mm0);  //-
        csimd::paddw( mm4, mm3 );

        csimd::pmulhw( (MM_FIX_1_847759065), mm3 ); //z5
        csimd::paddw( mm5, mm2 );            //z11  >

        csimd::pmulhw( (MM_FIX_1_082392200), mm4 );
        csimd::movq( mm2, mm5);

        csimd::psubw( mm6, mm2 );
        csimd::paddw( mm6, mm5 );            //t7

        csimd::pmulhw( (MM_FIX_1_414213562), mm2);  //t11
        csimd::paddw( mm3, mm0    );         //t12

        csimd::psllw (3, mm0  );
        csimd::psubw( mm3, mm4  );           //t10

        csimd::movq( temps[0], mm6  );
        csimd::movq( mm1, mm3 );

        csimd::psllw( 3, mm4);
        csimd::psubw( mm5, mm0);             //t6

        csimd::psllw( 3, mm2);
        csimd::paddw( mm0, mm1 );            //d1

        csimd::psubw( mm0, mm2 );            //t5
        csimd::psubw( mm0, mm3 );            //d6

        csimd::paddw( mm2, mm4 );            //t4
        csimd::movq( mm7, mm0 );

        csimd::paddw( mm2, mm7);             //d2
        csimd::psubw( mm2, mm0);             //d5

        csimd::movq( (MM_DESCALE_RND), mm2);    //4
        csimd::psubw( mm5, mm6     );        //d7

        csimd::paddw( temps[0], mm5 );      //d0
        csimd::paddw( mm2, mm1);

        csimd::paddw( mm2, mm5);
        csimd::psraw( 3, mm1);

        csimd::paddw( mm2, mm7  );
        csimd::psraw( 3, mm5  );

        csimd::paddw( REG_D, mm5 );
        csimd::psraw( 3, mm7  );

        csimd::paddw( (REG_D+REG_a), mm1);
        csimd::paddw( mm2, mm0           );

        csimd::paddw( (REG_D+REG_a*2), mm7 );
        csimd::paddw( mm2, mm3            );

        csimd::movq( mm5, REG_D  );
        csimd::paddw( mm2, mm6 );

        csimd::movq( mm1, REG_D+REG_a  );
        csimd::psraw( 3, mm0);

        csimd::movq( mm7, (REG_D+REG_a*2));
        REG_D += REG_d; //3*ls

        csimd::movq( temps[1], mm5 );       //t3
        csimd::psraw( 3, mm3);

        csimd::paddw( (REG_D+REG_a*2), mm0);
        csimd::psubw( mm4, mm5);             //d3

        csimd::paddw( (REG_D+REG_d), mm3);
        csimd::psraw (3, mm6  );

        csimd::paddw( temps[1], mm4  );     //d4
        csimd::paddw( mm2, mm5 );

        csimd::paddw( (REG_D+REG_a*4), mm6  );
        csimd::paddw( mm2, mm4  );

        csimd::movq( mm0, (REG_D+REG_a*2));
        csimd::psraw (3, mm5 );

        csimd::paddw( REG_D, mm5 );
        csimd::psraw (3, mm4  );

        csimd::paddw( (REG_D+REG_a), mm4);
        REG_S+= DCTSIZE_S*2*4;       //4 rows

        csimd::movq( mm3, (REG_D+REG_d) );
        csimd::movq( mm6, (REG_D+REG_a*4) );
        csimd::movq( mm5, REG_D         );
        csimd::movq( mm4, (REG_D+REG_a) );

        REG_D-=REG_d;
        REG_D+= 8;
        REG_c--;
        if (REG_c!=0)
         goto label1;//jnz 1b
}

void TimgFilterPostprocFspp::row_fdct_s(DCTELEM *data_,  const uint8_t *pixels,  int line_size,  int cnt)
{
 const uint8_t *REG_S=pixels;
 uint8_t *REG_D=(uint8_t*)data_;
 int REG_c=cnt;
 int REG_a=line_size;
 int REG_d;

        REG_d = REG_a+REG_a*2;
__m64 temps[2];
        __m64 mm0,mm1,mm2,mm3,mm4,mm5,mm6,mm7;
       label6:
        csimd::movd( (REG_S), mm0           );
        csimd::pxor( mm7, mm7);

        csimd::movd( (REG_S+REG_a), mm1 );
        csimd::punpcklbw( mm7, mm0   );

        csimd::movd( (REG_S+REG_a*2), mm2  );
        csimd::punpcklbw( mm7, mm1  );

        csimd::punpcklbw( mm7, mm2 );
        REG_S+= REG_d;

        csimd::movq( mm0, mm5);
        //

        csimd::movd( (REG_S+REG_a*4), mm3 );    //7  ;prefetch!
        csimd::movq( mm1, mm6 );

        csimd::movd( (REG_S+REG_d), mm4);      //6
        csimd::punpcklbw( mm7, mm3);

        csimd::psubw( mm3, mm5        );
        csimd::punpcklbw( mm7, mm4    );

        csimd::paddw( mm3, mm0       );
        csimd::psubw( mm4, mm6    );

        csimd::movd( (REG_S+REG_a*2), mm3);     //5
        csimd::paddw( mm4, mm1  );

        csimd::movq( mm5, temps[0] );       //t7
        csimd::punpcklbw( mm7, mm3  );

        csimd::movq( mm6, temps[1]  );      //t6
        csimd::movq( mm2, mm4  );

        csimd::movd( (REG_S), mm5  );          //3
        csimd::paddw( mm3, mm2);

        csimd::movd( (REG_S+REG_a), mm6  );    //4
        csimd::punpcklbw( mm7, mm5  );

        csimd::psubw( mm3, mm4   );
        csimd::punpcklbw( mm7, mm6 );

        csimd::movq( mm5, mm3);
        csimd::paddw( mm6, mm5 );            //t3

        csimd::psubw( mm6, mm3);             //t4  ; t0 t1 t2 t4 t5 t3 - -
        csimd::movq( mm0, mm6);

        csimd::movq( mm1, mm7 );
        csimd::psubw( mm5, mm0 );            //t13

        csimd::psubw( mm2, mm1 );
        csimd::paddw( mm2, mm7 );            //t11

        csimd::paddw( mm0, mm1);
        csimd::movq( mm7, mm2 );

        csimd::psllw( 2, mm1 );
        csimd::paddw( mm5, mm6);             //t10

        csimd::pmulhw( (MM_FIX_0_707106781), mm1 );
        csimd::paddw( mm6, mm7);             //d2

        csimd::psubw( mm2, mm6 );            //d3
        csimd::movq( mm0, mm5);

        //transpose 4x4
        csimd::movq( mm7, mm2 );
        csimd::punpcklwd( mm6, mm7  );

        csimd::paddw( mm1, mm0 );            //d0
        csimd::punpckhwd( mm6, mm2 );

        csimd::psubw( mm1, mm5 );            //d1
        csimd::movq( mm0, mm6  );

        csimd::movq( temps[1], mm1  );
        csimd::punpcklwd( mm5, mm0 );

        csimd::punpckhwd( mm5, mm6);
        csimd::movq( mm0, mm5 );

        csimd::punpckldq( mm7, mm0 );        //0
        csimd::paddw( mm4, mm3  );

        csimd::punpckhdq( mm7, mm5 );        //1
        csimd::movq( mm6, mm7 );

        csimd::movq( mm0, DCTSIZE_S*0*2+REG_D);
        csimd::punpckldq( mm2, mm6  );       //2

        csimd::movq( mm5, DCTSIZE_S*1*2+REG_D);
        csimd::punpckhdq( mm2, mm7 );        //3

        csimd::movq( mm6, DCTSIZE_S*2*2+REG_D);
        csimd::paddw( mm1, mm4   );

        csimd::movq( mm7, DCTSIZE_S*3*2+REG_D);
        csimd::psllw( 2, mm3);               //t10

        csimd::movq( temps[0], mm2 );
        csimd::psllw( 2, mm4   );            //t11

        csimd::pmulhw( (MM_FIX_0_707106781), mm4 ); //z3
        csimd::paddw( mm2, mm1  );

        csimd::psllw( 2, mm1 );              //t12
        csimd::movq( mm3, mm0  );

        csimd::pmulhw( (MM_FIX_0_541196100), mm0 );
        csimd::psubw( mm1, mm3 );

        csimd::pmulhw( (MM_FIX_0_382683433), mm3 ); //z5
        csimd::movq( mm2, mm5  );

        csimd::pmulhw( (MM_FIX_1_306562965), mm1);
        csimd::psubw( mm4, mm2);             //z13

        csimd::paddw( mm4, mm5 );            //z11
        csimd::movq( mm2, mm6);

        csimd::paddw( mm3, mm0);             //z2
        csimd::movq( mm5, mm7);

        csimd::paddw( mm0, mm2 );            //d4
        csimd::psubw( mm0, mm6 );            //d5

        csimd::movq( mm2, mm4);
        csimd::paddw( mm3, mm1);             //z4

        //transpose 4x4
        csimd::punpcklwd( mm6, mm2 );
        csimd::paddw( mm1, mm5  );           //d6

        csimd::punpckhwd( mm6, mm4  );
        csimd::psubw( mm1, mm7 );            //d7

        csimd::movq( mm5, mm6   );
        csimd::punpcklwd( mm7, mm5  );

        csimd::punpckhwd( mm7, mm6 );
        csimd::movq( mm2, mm7    );

        csimd::punpckldq( mm5, mm2  );       //4
        REG_S-=REG_d;

        csimd::punpckhdq( mm5, mm7 );        //5
        csimd::movq( mm4, mm5  );

        csimd::movq( mm2, DCTSIZE_S*0*2+DCTSIZE_S+REG_D) ;
        csimd::punpckldq( mm6, mm4 );        //6

        csimd::movq( mm7, DCTSIZE_S*1*2+DCTSIZE_S+REG_D) ;
        csimd::punpckhdq( mm6, mm5 );        //7

        csimd::movq( mm4, DCTSIZE_S*2*2+DCTSIZE_S+REG_D) ;
        REG_S+=4;

        csimd::movq( mm5, DCTSIZE_S*3*2+DCTSIZE_S+REG_D) ;
        REG_D+= DCTSIZE_S*2*4;        //4 rows
        REG_c--;
        if (REG_c!=0)
         goto label6;//jnz 6b
}

TimgFilterPostprocFspp::TimgFilterPostprocFspp(IffdshowBase *Ideci,Tfilters *Iparent):
 TimgFilterPostprocBase(Ideci,Iparent,false),
 temp(NULL),src(NULL)
{
}

bool TimgFilterPostprocFspp::is(const TffPictBase &pict,const TfilterSettingsVideo *cfg0)
{
 const TpostprocSettings *cfg=(const TpostprocSettings*)cfg0;
 if (super::is(pict,cfg) && cfg->qual)
  {
   Trect r=pict.getRect(cfg->full,cfg->half);
   return pictRect.dx>=16 && pictRect.dy>=16;
  }
 else
  return false;
}

void TimgFilterPostprocFspp::done(void)
{
 if (temp) aligned_free(temp);temp=NULL;
 if (src) aligned_free(src);src=NULL;
}
void TimgFilterPostprocFspp::onSizeChange(void)
{
 done();
}

HRESULT TimgFilterPostprocFspp::process(TfilterQueue::iterator it,TffPict &pict,const TfilterSettingsVideo *cfg0)
{
 const TpostprocSettings *cfg=(const TpostprocSettings*)cfg0;
 if (prepare(cfg,31,pict.frametype) && currentq)
  {
   init(pict,cfg->full,cfg->half);
   if (pictRect.dx>=16 && pictRect.dy>=16)
    {
     bool cspChanged=false;
     const unsigned char *tempPict1[4];
     cspChanged|=getCur(FF_CSPS_MASK_YUV_PLANAR,pict,cfg->full,tempPict1);
     unsigned char *tempPict2[4];
     cspChanged|=getNext(csp1,pict,cfg->full,tempPict2);
     if (cspChanged)
      done();

     if (!temp)
      {

       //vf->priv->log2_count= *((unsigned int*)data);
       //if (vf->priv->log2_count < 4) vf->priv->log2_count=4;

       static const short custom_threshold[64]=
        // values (296) can't be too high
        // -it causes too big quant dependence
        // or maybe overflow(check), which results in some flashing
       { 71, 296, 295, 237,  71,  40,  38,  19,
         245, 193, 185, 121, 102,  73,  53,  27,
         158, 129, 141, 107,  97,  73,  50,  26,
         102, 116, 109,  98,  82,  66,  45,  23,
         71,  94,  95,  81,  70,  56,  38,  20,
         56,  77,  74,  66,  56,  44,  30,  15,
         38,  53,  50,  45,  38,  30,  21,  11,
         20,  27,  26,  23,  20,  15,  11,   5
       };
       int custom_threshold_m[64];
       int i1=-1;
       i1=limit(i1,-15,32);
       int bias= (1<<4)+i1; //regulable

       for(int i=0;i<64;i++) //FIXME: tune custom_threshold[] and remove this !
           custom_threshold_m[i]=(int)(custom_threshold[i]*(bias/71.)+ 0.5);
       for(int i=0;i<8;i++){
           threshold_mtx_noq[2*i]=(uint64_t)custom_threshold_m[i*8+2]
               |(((uint64_t)custom_threshold_m[i*8+6])<<16)
               |(((uint64_t)custom_threshold_m[i*8+0])<<32)
               |(((uint64_t)custom_threshold_m[i*8+4])<<48);
           threshold_mtx_noq[2*i+1]=(uint64_t)custom_threshold_m[i*8+5]
               |(((uint64_t)custom_threshold_m[i*8+3])<<16)
               |(((uint64_t)custom_threshold_m[i*8+1])<<32)
               |(((uint64_t)custom_threshold_m[i*8+7])<<48);
       }

       //if (vf->priv->qp) vf->priv->prev_q=vf->priv->qp, mul_thrmat_s(vf->priv, vf->priv->qp);

       int h= (dy1[0]+16+15)&(~15);
       temp_stride= (dx1[0]+16+15)&(~15);
       temp= (int16_t*)aligned_calloc(temp_stride,3*8*sizeof(int16_t));
       src = (uint8_t*)aligned_malloc(temp_stride*h*sizeof(uint8_t));
      }
     DPRINTFA("%i",int(quants[0]));
     for (unsigned int i=0;i<pict.cspInfo.numPlanes;i++)
      filter(tempPict2[i],tempPict1[i],stride2[i],stride1[i],dx1[i],dy1[i],quants,quantsDx,i==0);
     if (Tconfig::cpu_flags&FF_CPU_MMX) _mm_empty();
     if (Tconfig::cpu_flags&FF_CPU_MMXEXT) _mm_sfence();
    }
  }
 return parent->deliverSample(++it,pict);
}
