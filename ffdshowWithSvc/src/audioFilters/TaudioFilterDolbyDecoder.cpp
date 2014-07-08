/*
 * Copyright (c) 2004-2006 Milan Cutka
 * based on mplayer HRTF plugin by ylai
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
#include "TaudioFilterDolbyDecoder.h"
#include "TfirSettings.h"

TaudioFilterDolbyDecoder2::TaudioFilterDolbyDecoder2(IffdshowBase *Ideci,Tfilters *Iparent):TaudioFilter(Ideci,Iparent)
{
 olddelay=-1;oldfreq=0;
 filter_coefs_lfe=NULL;
}

void TaudioFilterDolbyDecoder2::done(void)
{
 onSeek();
 if (filter_coefs_lfe) aligned_free(filter_coefs_lfe);filter_coefs_lfe=NULL;
}

bool TaudioFilterDolbyDecoder2::is(const TsampleFormat &fmt,const TfilterSettingsAudio *cfg)
{
 return super::is(fmt,cfg) && fmt.nchannels==2;
}
bool TaudioFilterDolbyDecoder2::getOutputFmt(TsampleFormat &fmt,const TfilterSettingsAudio *cfg)
{
 if (super::getOutputFmt(fmt,cfg))
  {
   fmt.setChannels(6);
   return true;
  }
 else
  return false;
}

float TaudioFilterDolbyDecoder2::passive_lock(float x)
{
 static const float MATAGCLOCK=0.2f;    /* AGC range (around 1) where the matrix behaves passively */
 const float x1 = x - 1;
 const float ax1s = fabs(x - 1) * (1.0f / MATAGCLOCK);
 return x1 - x1 / (1 + ax1s * ax1s) + 1;
}

void TaudioFilterDolbyDecoder2::matrix_decode(const float *in, const int k, const int il,
                                              const int ir, bool decode_rear,
                                              const int dlbuflen,
                                              float l_fwr, float r_fwr,
                                              float lpr_fwr, float lmr_fwr,
                                              float *adapt_l_gain, float *adapt_r_gain,
                                              float *adapt_lpr_gain, float *adapt_lmr_gain,
                                              float *lf, float *rf, float *lr,
                                              float *rr, float *cf)
{
 static const float M9_03DB=0.3535533906f;
 static const float MATAGCTRIG=8.0f;    /* (Fuzzy) AGC trigger */
 static const float MATAGCDECAY=1.0f;   /* AGC baseline decay rate (1/samp.) */
 static const float MATCOMPGAIN=0.37f;  /* Cross talk compensation gain,  0.50 - 0.55 is full cancellation. */

 const int kr = (k + olddelay) % dlbuflen;
 float l_gain = (l_fwr + r_fwr) / (1 + l_fwr + l_fwr);
 float r_gain = (l_fwr + r_fwr) / (1 + r_fwr + r_fwr);
 /* The 2nd axis has strong gain fluctuations, and therefore require
    limits.  The factor corresponds to the 1 / amplification of (Lt
    - Rt) when (Lt, Rt) is strongly correlated. (e.g. during
    dialogues).  It should be bigger than -12 dB to prevent
    distortion. */
 float lmr_lim_fwr = lmr_fwr > M9_03DB * lpr_fwr ? lmr_fwr : M9_03DB * lpr_fwr;
 float lpr_gain = (lpr_fwr + lmr_lim_fwr) / (1 + lpr_fwr + lpr_fwr);
 float lmr_gain = (lpr_fwr + lmr_lim_fwr) / (1 + lmr_lim_fwr + lmr_lim_fwr);
 float lmr_unlim_gain = (lpr_fwr + lmr_fwr) / (1 + lmr_fwr + lmr_fwr);
 float lpr, lmr;
 float l_agc, r_agc, lpr_agc, lmr_agc;
 float f, d_gain, c_gain, c_agc_cfk;

 /*** AXIS NO. 1: (Lt, Rt) -> (C, Ls, Rs) ***/
 /* AGC adaption */
 d_gain = (fabs(l_gain - *adapt_l_gain) + fabs(r_gain - *adapt_r_gain)) * 0.5f;
 f = d_gain * (1.0f / MATAGCTRIG);
 f = MATAGCDECAY - MATAGCDECAY / (1 + f * f);
 *adapt_l_gain = (1 - f) * *adapt_l_gain + f * l_gain;
 *adapt_r_gain = (1 - f) * *adapt_r_gain + f * r_gain;
 /* Matrix */
 l_agc = in[il] * passive_lock(*adapt_l_gain);
 r_agc = in[ir] * passive_lock(*adapt_r_gain);
 cf[k] = (l_agc + r_agc) * (float)M_SQRT1_2;
 if (decode_rear)
  {
   lr[kr] = rr[kr] = (l_agc - r_agc) * (float)M_SQRT1_2;
   /* Stereo rear channel is steered with the same AGC steering as
      the decoding matrix. Note this requires a fast updating AGC
      at the order of 20 ms (which is the case here). */
   lr[kr] *= (l_fwr + l_fwr) / (1 + l_fwr + r_fwr);
   rr[kr] *= (r_fwr + r_fwr) / (1 + l_fwr + r_fwr);
  }

 /*** AXIS NO. 2: (Lt + Rt, Lt - Rt) -> (L, R) ***/
 lpr = (in[il] + in[ir]) * (float)M_SQRT1_2;
 lmr = (in[il] - in[ir]) * (float)M_SQRT1_2;
 /* AGC adaption */
 d_gain = fabs(lmr_unlim_gain - *adapt_lmr_gain);
 f = d_gain * (1.0f / MATAGCTRIG);
 f = MATAGCDECAY - MATAGCDECAY / (1 + f * f);
 *adapt_lpr_gain = (1 - f) * *adapt_lpr_gain + f * lpr_gain;
 *adapt_lmr_gain = (1 - f) * *adapt_lmr_gain + f * lmr_gain;
 /* Matrix */
 lpr_agc = lpr * passive_lock(*adapt_lpr_gain);
 lmr_agc = lmr * passive_lock(*adapt_lmr_gain);
 lf[k] = (lpr_agc + lmr_agc) * (float)M_SQRT1_2;
 rf[k] = (lpr_agc - lmr_agc) * (float)M_SQRT1_2;

 /*** CENTER FRONT CANCELLATION ***/
 /* A heuristic approach exploits that Lt + Rt gain contains the
    information about Lt, Rt correlation.  This effectively reshapes
    the front and rear "cones" to concentrate Lt + Rt to C and
    introduce Lt - Rt in L, R. */
 /* 0.67677 is the empirical lower bound for lpr_gain. */
 c_gain = 8 * (*adapt_lpr_gain - 0.67677f);
 c_gain = c_gain > 0 ? c_gain : 0;
 /* c_gain should not be too high, not even reaching full
    cancellation (~ 0.50 - 0.55 at current AGC implementation), or
    the center will sound too narrow. */
 c_gain = MATCOMPGAIN / (1 + c_gain * c_gain);
 c_agc_cfk = c_gain * cf[k];
 lf[k] -= c_agc_cfk;
 rf[k] -= c_agc_cfk;
 cf[k] += c_agc_cfk + c_agc_cfk;
}

TfirFilter::_ftype_t* TaudioFilterDolbyDecoder2::calc_coefficients_125Hz_lowpass(int rate)
{
 len125=256;
 TfirFilter::_ftype_t f=125.0f/(rate/2);
 TfirFilter::_ftype_t *coeffs=TfirFilter::design_fir(&len125,&f,TfirSettings::LOWPASS,TfirSettings::WINDOW_HAMMING,0);
 static const float M3_01DB=0.7071067812f;
 for (unsigned int i=0;i<len125;i++) coeffs[i]*=M3_01DB;
 return coeffs;
}

HRESULT TaudioFilterDolbyDecoder2::process(TfilterQueue::iterator it,TsampleFormat &fmt,void *samples,size_t numsamples,const TfilterSettingsAudio *cfg0)
{
 static const unsigned int FWRDURATION=240;      /* FWR average duration (samples) */

 const TdolbyDecoderSettings *cfg=(const TdolbyDecoderSettings*)cfg0;

 if (is(fmt,cfg))
  {
   if (olddelay!=cfg->delay || oldfreq!=fmt.freq)
    {
     done();
     olddelay=cfg->delay;oldfreq=fmt.freq;
     dlbuflen=std::max(FWRDURATION,(fmt.freq*cfg->delay/1000));//+(len7000-1);
     cyc_pos=dlbuflen-1;
     fwrbuf_l.resize(dlbuflen);
     fwrbuf_r.resize(dlbuflen);
     lf.resize(dlbuflen);rf.resize(dlbuflen);lr.resize(dlbuflen);rr.resize(dlbuflen);cf.resize(dlbuflen);cr.resize(dlbuflen);
     filter_coefs_lfe=calc_coefficients_125Hz_lowpass(fmt.freq);
     lfe_pos=0;memset(LFE_buf,0,sizeof(LFE_buf));
    }

   float *in=(float*)init(cfg,fmt,samples,numsamples); // Input audio data
   float *end=in+numsamples*fmt.nchannels; // Loop end
   fmt.setChannels(6);
   float *out=(float*)(samples=alloc_buffer(fmt,numsamples,buf));
   while(in < end)
    {
     const int k = cyc_pos;

     const int fwr_pos = (k + FWRDURATION) % dlbuflen;
     /* Update the full wave rectified total amplitude */
     /* Input matrix decoder */
     l_fwr += fabs(in[0]) - fabs(fwrbuf_l[fwr_pos]);
     r_fwr += fabs(in[1]) - fabs(fwrbuf_r[fwr_pos]);
     lpr_fwr += fabs(in[0] + in[1]) - fabs(fwrbuf_l[fwr_pos] + fwrbuf_r[fwr_pos]);
     lmr_fwr += fabs(in[0] - in[1]) - fabs(fwrbuf_l[fwr_pos] - fwrbuf_r[fwr_pos]);

     /* Matrix encoded 2 channel sources */
     fwrbuf_l[k] = in[0];
     fwrbuf_r[k] = in[1];
     matrix_decode(in, k, 0, 1, true, dlbuflen,
                   l_fwr, r_fwr,
                   lpr_fwr, lmr_fwr,
                   &adapt_l_gain, &adapt_r_gain,
                   &adapt_lpr_gain, &adapt_lmr_gain,
                   &lf[0], &rf[0], &lr[0], &rr[0], &cf[0]);

     out[0]=lf[k];
     out[1]=rf[k];
     out[2]=cf[k];
     LFE_buf[lfe_pos]=(out[0]+out[1])/2;
     out[3]=TfirFilter::firfilter(LFE_buf,lfe_pos,len125,len125,filter_coefs_lfe);
     lfe_pos++;if (lfe_pos==len125) lfe_pos=0;
     out[4]=lr[k];
     out[5]=rr[k];
     // Next sample...
     in += 2;
     out += fmt.nchannels;
     cyc_pos--;
     if (cyc_pos < 0)
      cyc_pos += dlbuflen;
    }
  }
 return parent->deliverSamples(++it,fmt,samples,numsamples);
}

void TaudioFilterDolbyDecoder2::onSeek(void)
{
 l_fwr=r_fwr=lpr_fwr=lmr_fwr=0;
 std::fill(fwrbuf_l.begin(),fwrbuf_l.end(),0.0f);std::fill(fwrbuf_r.begin(),fwrbuf_r.end(),0.0f);
 adapt_l_gain=adapt_r_gain=adapt_lpr_gain=adapt_lmr_gain=0;
 std::fill(lf.begin(),lf.end(),0.0f);
 std::fill(rf.begin(),rf.end(),0.0f);
 std::fill(lr.begin(),lr.end(),0.0f);
 std::fill(rr.begin(),rr.end(),0.0f);
 std::fill(cf.begin(),cf.end(),0.0f);
 std::fill(cr.begin(),cr.end(),0.0f);
 lfe_pos=0;memset(LFE_buf,0,sizeof(LFE_buf));
}
