/*
 * Copyright (c) 2004,2005 Milan Cutka
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <mmsystem.h>
#include <mmreg.h>
#include <objbase.h>
#include <initguid.h>
#define _STLP_NEW_PLATFORM_SDK
#include "PODtypes.h"
#if defined(UCLIBCPP) && (defined(__INTEL_COMPILER) || defined(__GNUC__) || (_MSC_VER>=1300))
 #include "uClibc++/algorithm"
 #include "uClibc++/vector"
#else
 #include <algorithm>
 #include <vector>
#endif
#include "imgFilters/avisynth/avisynth_c.h"
#include "inttypes.h"
#include "imgFilters/IffProc.h"
#include "imgFilters/ffImgfmt.h"
#include "audioFilters/IffProcAudio.h"

struct Tavisynth
{
private:
 HMODULE hdll;
 template<class T> void loadFunction(T &fnc,const char *name)
  {
   fnc=hdll?(T)GetProcAddress(hdll,name):NULL;
   ok&=(fnc!=NULL);
  }
public:
 bool ok;
 Tavisynth(void)
  {
   hdll=LoadLibrary("avisynth.dll");
   ok=(hdll!=NULL);
   loadFunction(avs_add_function,"avs_add_function");
   loadFunction(avs_release_clip,"avs_release_clip");
   loadFunction(avs_new_c_filter,"avs_new_c_filter");
   loadFunction(avs_set_to_clip,"avs_set_to_clip");
   loadFunction(avs_release_video_frame,"avs_release_video_frame");
   loadFunction(avs_get_frame,"avs_get_frame");
   loadFunction(avs_new_video_frame_a,"avs_new_video_frame_a");
   loadFunction(avs_get_audio,"avs_get_audio");
  }
 ~Tavisynth()
  {
   if (hdll)
    FreeLibrary(hdll);
  }

 int (AVSC_CC *avs_add_function)(AVS_ScriptEnvironment *,const char * name, const char * params, AVS_ApplyFunc apply, void * user_data);
 void (AVSC_CC *avs_release_clip)(AVS_Clip *);
 struct AVS_Clip* (AVSC_CC *avs_new_c_filter)(AVS_ScriptEnvironment * e,AVS_FilterInfo * * fi,AVS_Value child, int store_child);
 void (AVSC_CC *avs_set_to_clip)(AVS_Value *, AVS_Clip *);
 void (AVSC_CC *avs_release_video_frame)(AVS_VideoFrame *);
 struct AVS_VideoFrame* (AVSC_CC *avs_get_frame)(AVS_Clip *, int n);
 struct AVS_VideoFrame* (AVSC_CC *avs_new_video_frame_a)(AVS_ScriptEnvironment *, const AVS_VideoInfo * vi, int align);
 int (AVSC_CC *avs_get_audio)(AVS_Clip *, void * buf, INT64 start, INT64 count);

 inline AVS_Value avs_new_value_clip(AVS_Clip * v0) { AVS_Value v; avs_set_to_clip(&v, v0); return v; }
 inline AVS_VideoFrame* avs_new_video_frame(AVS_ScriptEnvironment * env,  const AVS_VideoInfo * vi) {return avs_new_video_frame_a(env,vi,AVS_FRAME_ALIGN);}
} *avisynth=NULL;

class TffavisynthVideo
{
private:
 ~TffavisynthVideo()
  {
   if (proc)
    {
     proc->end();
     proc->Release();
    }
   CoUninitialize();
  }
 IffProc *proc;
 int incsp;
public:
 TffavisynthVideo(const char *preset,const char *options,AVS_VideoInfo &vi,const char* *error)
  {
   proc=NULL;
   CoInitialize(NULL);
   *error=NULL;
   if (CoCreateInstance(CLSID_TFFPROC,NULL,CLSCTX_INPROC_SERVER,IID_IffProc7,(void**)&proc)==S_OK)
    {
     proc->loadPreset("ffavisynth");
     if (preset && preset[0]!='\0')
      proc->setActivePreset(preset);
     if (options && options[0]!='\0')
      proc->putStringParam(options,',');

     proc->begin(vi.width,vi.height,vi.fps_numerator,vi.fps_denominator);
     if (avs_is_rgb24(&vi))
      incsp=FF_CSP_RGB24;
     else if (avs_is_rgb32(&vi))
      incsp=FF_CSP_RGB32;
     else if (avs_is_yuy2(&vi))
      incsp=FF_CSP_YUY2;
     else if (avs_is_yv12(&vi))
      incsp=FF_CSP_420P;
     else
      *error="ffdshow: unknown colorspace";

     unsigned int outDx,outDy;
     proc->getNewSize(vi.width,vi.height,&outDx,&outDy);
     vi.width=outDx;vi.height=outDy;
    }
   else
    *error="ffdshow: can't initialize IffProc7";
  }
 static void AVSC_CC free_filter(AVS_FilterInfo *fi)
  {
   delete (TffavisynthVideo*)fi->user_data;
  }
 static AVS_VideoFrame* AVSC_CC get_frame(AVS_FilterInfo *fi, int n)
  {
   return ((TffavisynthVideo*)fi->user_data)->GetFrame(fi,n);
  }

 AVS_VideoFrame* GetFrame(AVS_FilterInfo *p,int frame)
  {
   AVS_VideoFrame *srcframe=avisynth->avs_get_frame(p->child,frame);
   AVS_VideoFrame *dstframe=avisynth->avs_new_video_frame(p->env,&p->vi);
   const unsigned char *src[4];stride_t srcStride[4];
   unsigned char *dst[4];stride_t dstStride[4];
   if (avs_is_planar(&p->vi))
    {
     src[0]=avs_get_read_ptr_p(srcframe,AVS_PLANAR_Y);src[1]=avs_get_read_ptr_p(srcframe,AVS_PLANAR_U);src[2]=avs_get_read_ptr_p(srcframe,AVS_PLANAR_V);
     srcStride[0]=avs_get_pitch_p(srcframe,AVS_PLANAR_Y);srcStride[1]=avs_get_pitch_p(srcframe,AVS_PLANAR_U);srcStride[2]=avs_get_pitch_p(srcframe,AVS_PLANAR_V);
     dst[0]=avs_get_write_ptr_p(dstframe,AVS_PLANAR_Y);dst[1]=avs_get_write_ptr_p(dstframe,AVS_PLANAR_U);dst[2]=avs_get_write_ptr_p(dstframe,AVS_PLANAR_V);
     dstStride[0]=avs_get_pitch_p(dstframe,AVS_PLANAR_Y);dstStride[1]=avs_get_pitch_p(dstframe,AVS_PLANAR_U);dstStride[2]=avs_get_pitch_p(dstframe,AVS_PLANAR_V);
    }
   else
    {
     src[0]=avs_get_read_ptr(srcframe);
     srcStride[0]=avs_get_pitch(srcframe);
     dst[0]=avs_get_write_ptr(dstframe);
     dstStride[0]=avs_get_pitch(dstframe);
    }
   proc->processTime(frame,0,0,incsp,src,srcStride,true,incsp,dst,dstStride);
   avisynth->avs_release_video_frame(srcframe);
   return dstframe;
  }
};

class TffavisynthAudio
{
private:
 ~TffavisynthAudio()
  {
   if (proc)
    {
     proc->end();
     proc->Release();
    }
   if (tempbuffer) free(tempbuffer);
   CoUninitialize();
  }
 static void vi2wfmt(const AVS_VideoInfo &vi,WAVEFORMATEX &wfx)
  {
   wfx.nChannels=(WORD)avs_audio_channels(&vi);
   wfx.nSamplesPerSec=avs_samples_per_second(&vi);
   switch (avs_sample_type(&vi))
    {
     case AVS_SAMPLE_INT8 :wfx.wFormatTag=WAVE_FORMAT_PCM;wfx.wBitsPerSample= 8;break;
     case AVS_SAMPLE_INT16:wfx.wFormatTag=WAVE_FORMAT_PCM;wfx.wBitsPerSample=16;break;
     case AVS_SAMPLE_INT24:wfx.wFormatTag=WAVE_FORMAT_PCM;wfx.wBitsPerSample=24;break;
     case AVS_SAMPLE_INT32:wfx.wFormatTag=WAVE_FORMAT_PCM;wfx.wBitsPerSample=32;break;
     case AVS_SAMPLE_FLOAT:wfx.wFormatTag=WAVE_FORMAT_IEEE_FLOAT;wfx.wBitsPerSample=32;break;
    }
  }
 static void wfmt2vi(const WAVEFORMATEX &wfx,AVS_VideoInfo &vi)
  {
   vi.nchannels=wfx.nChannels;
   vi.audio_samples_per_second=wfx.nSamplesPerSec;
   if (wfx.wFormatTag==WAVE_FORMAT_PCM)
    switch (wfx.wBitsPerSample)
     {
      case  8:vi.sample_type=AVS_SAMPLE_INT8 ;break;
      case 16:vi.sample_type=AVS_SAMPLE_INT16;break;
      case 24:vi.sample_type=AVS_SAMPLE_INT24;break;
      case 32:vi.sample_type=AVS_SAMPLE_INT32;break;
     }
   else
    vi.sample_type=AVS_SAMPLE_FLOAT;
  }
 AVS_VideoInfo viIn;WAVEFORMATEX wfIn;
 IffProcAudio *proc;
 uint8_t *tempbuffer;size_t tempbuffer_size;
public:
 TffavisynthAudio(const char *preset,const char *options,AVS_VideoInfo &vi,const char* *error)
  {
   *error=NULL;
   proc=NULL;tempbuffer=NULL;tempbuffer_size=0;
   CoInitialize(NULL);
   if (CoCreateInstance(CLSID_TFFPROCAUDIO,NULL,CLSCTX_INPROC_SERVER,IID_IffProcAudio,(void**)&proc)==S_OK)
    {
     proc->loadPreset("ffavisynth");
     if (preset && preset[0]!='\0')
      proc->setActivePreset(preset);
     if (options && options[0]!='\0')
      proc->putStringParam(options,',');
     WAVEFORMATEX wfOut;
     viIn=vi;
     vi2wfmt(vi,wfIn);
     proc->begin(&wfIn,&wfOut);
     wfmt2vi(wfOut,vi);
    }
   else
    *error="ffdshowAudio: can't initialize IffProcAudio";
  }
 int __stdcall GetAudio(AVS_FilterInfo *fi,void* buf, __int64 start, __int64 count)
  {
   if (tempbuffer_size<(size_t)count)
    tempbuffer=(uint8_t*)realloc(tempbuffer,(tempbuffer_size=(size_t)count)*avs_bytes_per_audio_sample(&viIn));
   avisynth->avs_get_audio(fi->child,tempbuffer,start,count);
   size_t outcount;WAVEFORMATEX wfOut;
   void *outsamples=tempbuffer;
   proc->process(&wfIn,(unsigned int)count,&wfOut,&outcount,&outsamples);
   memcpy(buf,outsamples,outcount*avs_bytes_per_audio_sample(&fi->vi));
   return 0;
  }
 static int AVSC_CC get_audio(AVS_FilterInfo *fi, void * buf, INT64 start, INT64 count)
  {
   return ((TffavisynthAudio*)fi->user_data)->GetAudio(fi,buf,start,count);
  }
 static void AVSC_CC free_filter(AVS_FilterInfo *fi)
  {
   delete (TffavisynthAudio*)fi->user_data;
  }
};

static AVS_Value AVSC_CC create_video(AVS_ScriptEnvironment * env, AVS_Value args, void *)
{
 AVS_FilterInfo *fi;
 AVS_Clip *new_clip = avisynth->avs_new_c_filter(env, &fi, avs_array_elt(args, 0), 1);
 fi->get_frame=TffavisynthVideo::get_frame;
 fi->free_filter=TffavisynthVideo::free_filter;
 const char *error=NULL;
 fi->user_data=new TffavisynthVideo(avs_as_string(avs_array_elt(args,1)),avs_as_string(avs_array_elt(args,2)),fi->vi,&error);
 if (!error)
  {
   AVS_Value v=avisynth->avs_new_value_clip(new_clip);
   avisynth->avs_release_clip(new_clip);
   return v;
  }
 else
  return avs_new_value_error(error);
}

static AVS_Value AVSC_CC create_audio(AVS_ScriptEnvironment * env, AVS_Value args, void *)
{
 AVS_FilterInfo *fi;
 AVS_Clip *new_clip = avisynth->avs_new_c_filter(env, &fi, avs_array_elt(args, 0), 1);
 fi->get_audio=TffavisynthAudio::get_audio;
 fi->free_filter=TffavisynthAudio::free_filter;
 const char *error=NULL;
 fi->user_data=new TffavisynthAudio(avs_as_string(avs_array_elt(args,1)),avs_as_string(avs_array_elt(args,2)),fi->vi,&error);
 if (!error)
  {
   AVS_Value v=avisynth->avs_new_value_clip(new_clip);
   avisynth->avs_release_clip(new_clip);
   return v;
  }
 else
  return avs_new_value_error(error);
}

extern "C" __declspec(dllexport) const char* __stdcall avisynth_c_plugin_init(AVS_ScriptEnvironment * env)
{
 if (!avisynth)
  avisynth=new Tavisynth;
 if (avisynth->ok)
  {
   avisynth->avs_add_function(env,"ffdshow","c[preset]s[options]s",create_video,0);
   avisynth->avs_add_function(env,"ffdshowAudio","c[preset]s[options]s",create_audio,0);
  }
 return "";
}

extern "C" BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
 static int count=0;
 switch (dwReason)
  {
   case DLL_PROCESS_ATTACH:
    count++;
    break;
   case DLL_PROCESS_DETACH:
    count--;
    if (count<=0 && avisynth)
     delete avisynth;
    break;
  }
 return TRUE;
}
