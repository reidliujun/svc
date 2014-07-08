#ifndef _TAVISYNTH_H_
#define _TAVISYNTH_H_

#include "Tdll.h"
#include "avisynth/avisynth.h"
#include "avisynth/avisynth_c.h"
#include "interfaces.h"

struct Tavisynth :public Tdll
{
 Tavisynth(void);
 static bool getVersion(const Tconfig *config,ffstring &vers,ffstring &license);
 static const char_t *dllname;

 IScriptEnvironment* (__stdcall *CreateScriptEnvironment)(int version);
 static const char *getScriptVideo(size_t hdrsize,const char *extradata,size_t extradatasize);
 static const char *getScriptAudio(size_t hdrsize,const char *extradata,size_t extradatasize,size_t d=0);
};

struct Tavisynth_c : public Tdll
{
public:
 struct AvisynthError
  {
   AvisynthError(const char *Imsg):msg(Imsg) {}
   const char *msg;
  };
private:
 AVS_ScriptEnvironment* (AVSC_CC *avs_create_script_environment)(int version);
protected:
 Tavisynth_c(void);

 struct AVSValue :public AVS_Value
  {
   AVSValue(const AVS_Value &src) {memcpy(this,&src,sizeof(src));}
   AVSValue() { type = 'v'; }
   //AVSValue(IClip* c) { type = 'c'; clip = c; if (c) c->AddRef(); }
   //AVSValue(const PClip& c) { type = 'c'; clip = c.GetPointerWithAddRef(); }
   AVSValue(bool b) { type = 'b'; d.boolean = b; }
   AVSValue(int i) { type = 'i'; d.integer = i; }
   AVSValue(float f) { type = 'f'; d.floating_pt = f; }
   AVSValue(double f) { type = 'f'; d.floating_pt = float(f); }
   AVSValue(const char* s) { type = 's'; d.string = s; }
   AVSValue(const AVSValue* a, int size) { type = 'a'; d.array = a; array_size = (short)size; }
   //~AVSValue() {avs_release_value(*this);}
   const char* AsString() const {return avs_as_string(*this);}
   int AsInt() const {return avs_as_int(*this);}
   int IsClip() const {return avs_is_clip(*this);}

   void Release() {avs_release_value(*this);}
  };

 struct IScriptEnvironment
  {
  private:
   AVS_ScriptEnvironment *env;
   Tavisynth_c *self;
  public:
   IScriptEnvironment(Tavisynth_c *Iself,AVS_ScriptEnvironment *Ienv):self(Iself),env(Ienv)
    {
     self->loadFunction(avs_invoke,"avs_invoke");
     self->loadFunction(avs_release_value,"avs_release_value");
     self->loadFunction(avs_take_clip,"avs_take_clip");
     self->loadFunction(avs_get_video_info,"avs_get_video_info");
     self->loadFunction(avs_get_frame,"avs_get_frame");
     self->loadFunction(avs_release_clip,"avs_release_clip");
     self->loadFunction(avs_release_video_frame,"avs_release_video_frame");
     self->loadFunction(avs_get_audio,"avs_get_audio");
     self->loadFunction(avs_clip_get_error,"avs_clip_get_error");
     self->loadFunction(avs_add_function,"avs_add_function");
     self->loadFunction(avs_new_c_filter,"avs_new_c_filter");
     self->loadFunction(avs_new_video_frame_a,"avs_new_video_frame_a");
     self->loadFunction(avs_set_to_clip,"avs_set_to_clip");
     self->loadFunction(avs_copy_video_frame,"avs_copy_video_frame");
     self->loadFunction(avs_get_var,"avs_get_var");
     self->loadFunction(avs_set_var,"avs_set_var");
     self->loadFunction(avs_set_global_var,"avs_set_global_var");
    }
   ~IScriptEnvironment()
    {
     struct Ta
      {
 	::IScriptEnvironment * env;
	const char * error;
      };
     delete ((Ta*)env)->env;
    }

   AVS_Value Invoke(const char* name, const AVS_Value args, const char** arg_names=0)
    {
     AVS_Value val=avs_invoke(env,name,args,arg_names);

     if (avs_is_error(val))
      throw AvisynthError(avs_as_error(val));

     return val;
    }

   int SetGlobalVar(const char* name, const AVS_Value arg)
    {
     return avs_set_global_var(env,name,arg);
    }

   AVSValue GetGlobalVar(const char* name)
    {
     return AVSValue(avs_get_var(env,name));
    }

   IScriptEnvironment* operator ->() {return this;}
   operator AVS_ScriptEnvironment*() {return env;}

   struct AVS_Value (AVSC_CC *avs_invoke)(AVS_ScriptEnvironment *, const char * name, AVS_Value args, const char** arg_names);
   struct AVS_Value (AVSC_CC *avs_release_value)(AVS_Value);
   AVS_Clip* (AVSC_CC *avs_take_clip)(AVS_Value, AVS_ScriptEnvironment *);
   const AVS_VideoInfo* (AVSC_CC *avs_get_video_info)(AVS_Clip *);
   AVS_VideoFrame* (AVSC_CC *avs_get_frame)(AVS_Clip *, int n);
   void (AVSC_CC *avs_release_clip)(AVS_Clip *);
   void (AVSC_CC *avs_release_video_frame)(AVS_VideoFrame *);
   const char* (AVSC_CC *avs_clip_get_error)(AVS_Clip *);
   int (AVSC_CC *avs_get_audio)(AVS_Clip *, void * buf, INT64 start, INT64 count);
   int (AVSC_CC *avs_add_function)(AVS_ScriptEnvironment *, const char * name, const char * params, AVS_ApplyFunc apply, void * user_data);
   AVS_Clip* (AVSC_CC *avs_new_c_filter)(AVS_ScriptEnvironment * e,AVS_FilterInfo * * fi,AVS_Value child, int store_child);
   void (AVSC_CC *avs_set_to_clip)(AVS_Value *, AVS_Clip *);
   AVS_VideoFrame* (AVSC_CC *avs_new_video_frame_a)(AVS_ScriptEnvironment *, const AVS_VideoInfo * vi, int align);
   AVS_VideoFrame* (AVSC_CC *avs_copy_video_frame)(AVS_VideoFrame *);

   struct AVS_Value (AVSC_CC *avs_get_var)(AVS_ScriptEnvironment *, const char* name);
   int (AVSC_CC *avs_set_var)(AVS_ScriptEnvironment *, const char* name, AVS_Value val);
   int (AVSC_CC *avs_set_global_var)(AVS_ScriptEnvironment *, const char* name, const AVS_Value val);

   int AddFunction(const char* name, const char* params, AVS_ApplyFunc apply, void* user_data) {return avs_add_function(env,name,params,apply,user_data);}
  };
 IScriptEnvironment* CreateScriptEnvironment(int version=AVISYNTH_INTERFACE_VERSION)
  {
   return new IScriptEnvironment(this,avs_create_script_environment(version));
  }

 struct VideoInfo :protected AVS_VideoInfo
  {
   VideoInfo() {}
   VideoInfo(const AVS_VideoInfo *src) {memcpy(this,src,sizeof(*src));}
   AVS_VideoInfo* operator&() {return this;}

   int HasVideo() const { return avs_has_video(this);}
   int HasAudio() const { return avs_has_audio(this);}
   int IsRGB() const { return avs_is_rgb(this);}
   int IsRGB24() const { return avs_is_rgb24(this);}
   int IsRGB32() const { return avs_is_rgb32(this);}
   int IsYUV() const { return avs_is_yuy(this);}
   int IsYUY2() const { return avs_is_yuy2(this);}
   int IsYV12() const { return avs_is_yv12(this);}
   int IsColorSpace(int c_space) const { return avs_is_color_space(this,c_space);}
   int Is(int property) const { return avs_is_property(this,property);}
   int IsPlanar() const { return avs_is_planar(this);}
   int IsFieldBased() const { return avs_is_field_based(this);}
   int IsParityKnown() const { return avs_is_parity_known(this);}
   int IsBFF() const { return avs_is_bff(this);}
   int IsTFF() const { return avs_is_tff(this);}
   int SampleType() const {return avs_sample_type(this);}
   int BytesPerAudioSample() const {return avs_bytes_per_audio_sample(this);}
   using AVS_VideoInfo::width;
   using AVS_VideoInfo::height;
   using AVS_VideoInfo::fps_numerator;
   using AVS_VideoInfo::fps_denominator;
   using AVS_VideoInfo::num_frames;
   using AVS_VideoInfo::pixel_type;
  };
public:
 struct PVideoFrame
  {
  private:
   AVS_VideoFrame *frame;
   IScriptEnvironment *env;
  public:
   PVideoFrame(IScriptEnvironment *Ienv,AVS_VideoFrame *Iframe):frame(Iframe),env(Ienv) {}
   ~PVideoFrame() {env->avs_release_video_frame(frame);}
   PVideoFrame* operator ->() {return this;}
   operator AVS_VideoFrame*()
    {
     AVS_VideoFrame *frame2=env->avs_copy_video_frame(frame);
     return frame2;
    }

   const BYTE* GetReadPtr() const {return avs_get_read_ptr(frame);}
   const BYTE* GetReadPtr(int plane) const {return avs_get_read_ptr_p(frame,plane);}
   BYTE* GetWritePtr() const {return avs_get_write_ptr(frame);}
   BYTE* GetWritePtr(int plane) const {return avs_get_write_ptr_p(frame,plane);}
   int GetHeight() const {return avs_get_height(frame);}
   int GetHeight(int plane) const {return avs_get_height_p(frame,plane);}
   int GetPitch() const {return avs_get_pitch(frame);}
   int GetPitch(int plane) const {return avs_get_pitch_p(frame,plane);}
   int GetRowSize() const {return avs_get_row_size(frame);}
   int GetRowSize(int plane) const {return avs_get_row_size_p(frame,plane);}
  };
protected:
 struct PClip
  {
  private:
   IScriptEnvironment *env;
   AVS_Clip *clip;
   VideoInfo info;
  public:
   PClip(AVSValue &val,IScriptEnvironment *Ienv):env(Ienv),clip(env->avs_take_clip(val,*Ienv))
    {
     if (!clip)
      throw AvisynthError("no clip");
     else if (const char *err=env->avs_clip_get_error(clip))
      throw AvisynthError(err);
     env->avs_release_value(val);
    }
   ~PClip() {env->avs_release_clip(clip);}
   PClip* operator ->() {return this;}
   const VideoInfo& GetVideoInfo(void) {return info=env->avs_get_video_info(clip);}
   PVideoFrame GetFrame(int n) {return PVideoFrame(env,env->avs_get_frame(clip,n));}
   int GetAudio( void * buf,INT64 start, INT64 count) {return env->avs_get_audio(clip,buf,start,count);}
  };
public:
 static bool getVersion(const Tconfig *config,ffstring &vers,ffstring &license);
 static const char_t *dllname;
};

class CMediaType;
struct TsampleFormat;
class TavisynthAudio : public Tavisynth_c
{
public:
 TavisynthAudio(const CMediaType &mt,TsampleFormat &fmt,IffdshowBase *deci,const char *scriptName);
 ~TavisynthAudio();
 IScriptEnvironment *env;
 const char *script;
 PClip *clip;
 const VideoInfo *vi;
};

#endif
