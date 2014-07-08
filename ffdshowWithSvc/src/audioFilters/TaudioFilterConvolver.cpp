/*
 * Copyright (c) 2005,2006 Milan Cutka
 * based on jack_convolve by Florian Schmidt
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
#include "TaudioFilterConvolver.h"
#include "fftsg.h"
#include "ffdebug.h"
#include "resample/resample2.h"

TaudioFilterConvolver::TaudioFilterConvolver(IffdshowBase *Ideci,Tfilters *Iparent):TaudioFilter(Ideci,Iparent)
{
}

//==================== TaudioFilterConvolver::Tconvolver::fft_response_t ========================
void TaudioFilterConvolver::Tconvolver::fft_response_t::init(const TwavReader<float> &response,const TconvolverSettings *cfg,unsigned int chunk_length,float normalization_factor,Tfftforward &fft_plan_forward,unsigned int procchannel)
{
 float levelAdjust=cfg->levelAdjustAuto?response.getAdjust():db2value((float)cfg->levelAdjustDB,10);
 // create new response struct
 length=response[0].size();
 // make channel_data array big enough to hold pointers for all channels
 size_t number_of_response_channels=procchannel!=INT32_MAX?1:response.size();
 channel_data.resize(number_of_response_channels);
 // how many chunks do we need for this response?
 number_of_chunks = (unsigned int)ceil((response[0].size())/(float)(chunk_length));
 // allocate per channel data buffer)
 // no need for double size, as we use r2c/c2r
 for (unsigned int index2 = 0; index2 < number_of_response_channels; ++index2)
  channel_data[index2].resize( (chunk_length + 1) * number_of_chunks);
 // for each  channel
 std::vector<float> fft_real(2*chunk_length);
 std::vector<complex> fft_complex(chunk_length+1);
 for (unsigned int index2 = 0; index2 < number_of_response_channels; ++index2)
  {
   unsigned int respchannel=procchannel!=INT32_MAX?std::min(response.nchannels-1,procchannel):index2;
   // for each chunk
   for (unsigned int index3 = 0; index3 < number_of_chunks; ++index3)
    {
     // copy original chunk to fft_real
     for (unsigned int index4 = 0; index4 < chunk_length; ++index4)
      {
       if (index4 + index3 * chunk_length < length)
        fft_real[index4] = response[respchannel][index4+index3*chunk_length]*levelAdjust;
       else
        fft_real[index4] = 0;
       // and zero pad the second half of the chunk at the same time
       fft_real[index4+chunk_length] = 0;
      }
     // do the fft
     fft_plan_forward.execute(&fft_real[0],&fft_complex[0]);
     // copy fft output to our private chunk store :)
     for (unsigned int index4 = 0; index4 < chunk_length + 1; ++index4)
      channel_data[index2][index4+index3 * (chunk_length + 1)]= complex(fft_complex[index4].real() / normalization_factor, fft_complex[index4].imag() / normalization_factor);
    }
  }
}

static unsigned int getNextPower2(unsigned int l)
{
 int num=1<<(av_log2(l)+2);
 return num;
}
//============================== TaudioFilterConvolver::Tconvolver ==============================
TaudioFilterConvolver::Tconvolver::Tconvolver(const TsampleFormat &infmt,const TwavReader<float> &response,const TconvolverSettings *cfg,unsigned int procchannel):
 chunk_length(getNextPower2((unsigned int)response[0].size())),
 number_of_responses(1),
 fft_plan_forward(2*getNextPower2((unsigned int)response[0].size())),
 fft_plan_backward(2*getNextPower2((unsigned int)response[0].size())),
 fft_real(2*getNextPower2((unsigned int)response[0].size())),
 fft_complex(getNextPower2((unsigned int)response[0].size())+1),
 fft_responses(1),
 input_chunk_ringbuffers(1)
{
 number_of_response_channels=procchannel!=INT32_MAX?1:response.nchannels;
 normalization_factor=sqrt(chunk_length * 2.0f);
 // --------------  chunk, pad, fft and store the responses
 // allocate array of response pointers
 // process input responses to padded, chunked, fft'ed internal representation
 //for (unsigned int index=0;index<number_of_responses;++index)
 fft_responses[0].init(response,cfg,chunk_length,normalization_factor,fft_plan_forward,procchannel);

 //  ------------- setup ringbuffers for storing FFT'ed input..
 //                the chunks are size chunk_length + 1 thus
 //                we need number_of_chunks * (chunk_length + 1)
 //                frames
 // one for each response
 for (unsigned int index = 0; index < number_of_responses; ++index)
  {
   input_chunk_ringbuffers[index].resize(fft_responses[index].number_of_chunks * (chunk_length + 1));
   // zero out
   for (unsigned int index2 = 0; index2 < (chunk_length + 1) * fft_responses[index].number_of_chunks; ++index2)
    input_chunk_ringbuffers[index][index2]=0;
  }
 // allocate input_chunk_ringbuffer_index arrays
 input_chunk_ringbuffer_indexes.resize(number_of_responses);
 // zero out
 for (unsigned int index = 0; index < number_of_responses; ++index)
  input_chunk_ringbuffer_indexes[index] = 0;
 // ---------------- setup overlap buffers
 //                  these need only be chunk_length sized
 overlap_buffers.resize(number_of_response_channels);
 for (unsigned int index = 0; index < number_of_response_channels; ++index)
  {
   overlap_buffers[index].resize(chunk_length);
   // zero out
   for (unsigned int index2 = 0; index2 < chunk_length; ++index2)
    overlap_buffers[index][index2] = 0;
  }
 if (procchannel!=INT32_MAX)
  {
   in_channels=out_channels=infmt.nchannels;
   in_channel[0]=out_channel[0]=procchannel;
  }
 else
  {
   in_channels=infmt.nchannels;
   for (unsigned int i=0;i<number_of_response_channels;i++)
    in_channel[i]=number_of_response_channels%in_channels;
   out_channels=number_of_response_channels;
   for (unsigned int i=0;i<number_of_response_channels;i++)
    out_channel[i]=i;
  }
}

int TaudioFilterConvolver::Tconvolver::process(const float * const in_data,float *out_data,size_t numsamples,const TconvolverSettings *cfg)
{
 float mixingStrength=cfg->mixingStrength/100.0f,mixingStrengthInv=1.0f-mixingStrength;

 int samplesPtr=0;
 for (;samplesPtr<int(numsamples-chunk_length);samplesPtr+=chunk_length)
  {
   for (unsigned int index = 0; index < number_of_responses; ++index)
    {
     // copy input chunk into fft_real
     for (unsigned int index2 = 0; index2 < chunk_length; ++index2)
      {
       fft_real[index2] = in_data[in_channel[0]+in_channels*(index2+samplesPtr)];//in_data[index][index2];
       // and pad at the same time
       fft_real[index2+chunk_length] = 0;
      }
     // do the fft
     fft_plan_forward.execute(&fft_real[0],&fft_complex[0]);
     // copy output into input_chunks ringbuffers
     for (unsigned int index2 = 0; index2 < chunk_length + 1; ++index2)
      input_chunk_ringbuffers[index][index2 + input_chunk_ringbuffer_indexes[index]]=complex(fft_complex[index2].real() / normalization_factor, fft_complex[index2].imag() / normalization_factor);
    }

   // do the complex multiplications for all response input channels
   for (unsigned int index = 0; index < number_of_response_channels; ++index)
    {
     // zero our the reverse fft input buffer
     for (unsigned int index2 = 0; index2 < chunk_length + 1; ++index2)
      fft_complex[index2]=0;
     // for all responses (of this output channel)
     for (unsigned int index2 = 0; index2 < number_of_responses; ++index2)
      {
       // for all chunks
       for (unsigned int index3 = 0; index3 < fft_responses[index2].number_of_chunks; ++index3)
        {
         // we go backward in time (from current chunk in input_chunnks_ringbuffers[] to oldest)
         int tmp_rb_chunk_index = input_chunk_ringbuffer_indexes[index2]
                                  - (index3 * (chunk_length + 1))
                                  + ((chunk_length + 1) * fft_responses[index2].number_of_chunks);
         // constraint to the actual data length ("%")
         tmp_rb_chunk_index %= (chunk_length + 1) * fft_responses[index2].number_of_chunks;
         complex_mul(&input_chunk_ringbuffers[index2][tmp_rb_chunk_index],
                     &fft_responses[index2].channel_data[index][index3 * (chunk_length + 1)],
                     &fft_complex[0],
                     chunk_length + 1);
        } // chunks
      } // responses
     // reverse fft the results
     fft_plan_backward.execute(&fft_complex[0],&fft_real[0]);
     // copy to out_buffers, save overlap and add previous overlap
     if (cfg->mixingStrength==100)
      for (unsigned int index2 = 0; index2 < chunk_length; ++index2)
       {
        float out=fft_real[index2] / normalization_factor + overlap_buffers[index][index2];
        out_data[out_channel[index]+out_channels*(index2+samplesPtr)] = out;
        overlap_buffers[index][index2] = fft_real[index2 + chunk_length] / normalization_factor;
       }
     else
      for (unsigned int index2 = 0; index2 < chunk_length; ++index2)
       {
        float out=fft_real[index2] / normalization_factor + overlap_buffers[index][index2];
        out_data[out_channel[index]+out_channels*(index2+samplesPtr)] = mix(in_data[in_channel[index]+in_channels*(index2+samplesPtr)],out,mixingStrength,mixingStrengthInv);
        overlap_buffers[index][index2] = fft_real[index2 + chunk_length] / normalization_factor;
       }
    }
   // advance input_chunk_ringbuffer_indexes
   for (unsigned int index = 0; index < number_of_responses; ++index)
    {
     input_chunk_ringbuffer_indexes[index] += chunk_length + 1;
     input_chunk_ringbuffer_indexes[index] %= fft_responses[index].number_of_chunks * (chunk_length + 1);
    }
  }
 return samplesPtr;
}

//==================================== TaudioFilterConvolver ====================================
void TaudioFilterConvolver::Tconvolver::Tfftforward::execute(const Tin *in,Tout *out)
{
 for (int i=0;i<length/2;i++)
  out[i]=complex(in[2*i+0],in[2*i+1]);
 rdft(length,1,(float*)out,&ip[0],&w[0]);
 out[length/2]=complex(out[0].imag(),0);
 out[0].imag()=0.0f;
}
void TaudioFilterConvolver::Tconvolver::Tfftbackward::execute(const Tin *in,Tout *out)
{
 for (int i=0;i<length/2;i++)
  {
   out[2*i+0]=in[i].real();
   out[2*i+1]=in[i].imag();
  }
 out[1]=in[length/2].real();
 rdft(length,-1,(float*)out,&ip[0],&w[0]);
 for (int i=0;i<length;i++)
  out[i]*=2;
}

void TaudioFilterConvolver::done(void)
{
 buffer.clear();
 convolvers.clear();
}

bool TaudioFilterConvolver::is(const TsampleFormat &fmt,const TfilterSettingsAudio *cfg0)
{
 const TconvolverSettings *cfg=(const TconvolverSettings*)cfg0;
 return super::is(fmt,cfg)/* && (cfg->mappingMode!=0 || !TwavReader<float>(cfg->file,true).empty())*/;
}

bool TaudioFilterConvolver::getOutputFmt(TsampleFormat &fmt,const TfilterSettingsAudio *cfg0)
{
 if (super::getOutputFmt(fmt,cfg0))
  {
   const TconvolverSettings *cfg=(const TconvolverSettings*)cfg0;
   if (cfg->mappingMode==0)
    {
     TwavReader<float> impulse(cfg->file,true);
     if (fmt.nchannels==1 && !(fmt.nchannels==impulse.nchannels || impulse.nchannels==1))
      fmt.setChannels(impulse.nchannels);
    }
   return true;
  }
 else
  return false;
}

void TaudioFilterConvolver::resampleImpulse(TwavReader<float> &impulse,int dstfreq)
{
 size_t lenout=impulse[0].size()*dstfreq/impulse.freq;
 std::vector<float> buffOut(lenout*2);
 for (unsigned int i=0;i<impulse.nchannels;i++)
  {
   TreSampleContext<float> resampler(1,dstfreq,impulse.freq,int(16*2.2),10,1,1.0,0);
   std::vector<float> bufIn(impulse[i]);bufIn.resize(bufIn.size()*2,impulse[i].back());
   int ret=resampler.audio_resample(&buffOut[0],&bufIn[0],(int)bufIn.size());
   impulse[i]=buffOut;impulse[i].resize(lenout);
  }
 impulse.freq=dstfreq;
}
HRESULT TaudioFilterConvolver::process(TfilterQueue::iterator it,TsampleFormat &fmt,void *samples0,size_t numsamples,const TfilterSettingsAudio *cfg0)
{
 const TconvolverSettings *cfg=(const TconvolverSettings*)cfg0;
 if (is(fmt,cfg) && cfg->mixingStrength)
  {
   if (!cfg->equal(oldcfg))
    {
     oldcfg=*cfg;
     done();

     if (cfg->mappingMode==0)
      {
       TwavReader<float> impulse(cfg->file);
       if (!impulse.empty())
        {
         if (impulse.freq!=fmt.freq)
          resampleImpulse(impulse,fmt.freq);
         if (fmt.nchannels==1)
          {
           convolvers.push_back(Tconvolver(fmt,impulse,cfg));
           outchannels=convolvers[0].number_of_response_channels;
          }
         else if (fmt.nchannels==impulse.nchannels || impulse.nchannels==1)
          {
           for (unsigned int i=0;i<fmt.nchannels;i++)
            convolvers.push_back(Tconvolver(fmt,impulse,cfg,i));
           outchannels=fmt.nchannels;
          }
        }
      }
     else
      {
       size_t oldsize=0;
       TwavReader<float> *impulses[8];memset(impulses,0,sizeof(impulses));
       for (unsigned int i=0;i<fmt.nchannels;i++)
        {
         const char_t *file=NULL;
         if (fmt.speakers[i]&SPEAKER_FRONT_LEFT)
          file=cfg->fileL;
         else if (fmt.speakers[i]&SPEAKER_FRONT_RIGHT)
          file=cfg->fileR;
         else if (fmt.speakers[i]&SPEAKER_FRONT_CENTER)
          file=cfg->fileC;
         else if (fmt.speakers[i]&SPEAKER_LOW_FREQUENCY)
          file=cfg->fileLFE;
         else if (fmt.speakers[i]&(SPEAKER_BACK_LEFT|SPEAKER_BACK_CENTER))
          file=cfg->fileSL;
         else if (fmt.speakers[i]&SPEAKER_BACK_RIGHT)
          file=cfg->fileSR;
         if (file)
          {
           impulses[i]=new TwavReader<float>(file);
           if (!impulses[i]->empty() && (impulses[i]->nchannels!=1 || (oldsize && impulses[i]->at(0).size()!=oldsize)))
            goto endManual;
           if (!impulses[i]->empty())
            {
             if (impulses[i]->freq!=fmt.freq)
              resampleImpulse(*impulses[i],fmt.freq);
             oldsize=impulses[i]->at(0).size();
            }
          }
        }
       if (oldsize)
        {
         for (unsigned int i=0;i<fmt.nchannels;i++)
          convolvers.push_back(Tconvolver(fmt,(!impulses[i] || impulses[i]->empty())?TwavReader<float>(fmt.freq,1,oldsize,0.0f):*impulses[i],cfg,i));
         outchannels=fmt.nchannels;
        }
      endManual:
       for (unsigned int i=0;i<fmt.nchannels;i++)
        if (impulses[i])
         delete impulses[i];
      }
    }

   if (!convolvers.empty())
    {
     float *samples=(float*)init(cfg,fmt,samples0,numsamples);
     unsigned int inBlockAlign=fmt.blockAlign();
     buffer.append(samples,numsamples*inBlockAlign);
     const float * const in_data=(const float* const)&*buffer.begin();
     numsamples=buffer.size()/inBlockAlign;
     fmt.setChannels(outchannels);
     float *out_data=(float*)alloc_buffer(fmt,numsamples,buf);
     int samplesProcessed=0;
     for (Tconvolvers::iterator convolver=convolvers.begin();convolver!=convolvers.end();convolver++)
      samplesProcessed=convolver->process(in_data,out_data,numsamples,cfg);
     buffer.erase(buffer.begin(),buffer.begin()+samplesProcessed*inBlockAlign);
     numsamples=samplesProcessed;
     samples0=out_data;
    }
  }
 return parent->deliverSamples(++it,fmt,samples0,numsamples);
}

void TaudioFilterConvolver::onSeek(void)
{
 buffer.clear();
}
