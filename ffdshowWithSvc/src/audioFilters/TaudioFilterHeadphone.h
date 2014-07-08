#ifndef _TAUDIOFILTERHEADPHONE_H_
#define _TAUDIOFILTERHEADPHONE_H_

#include "TaudioFilter.h"
#include "TsampleFormat.h"

struct TmixerSettings;
DECLARE_FILTER(TaudioFilterHeadphone,public,TaudioFilter)
private:
 TsampleFormat oldfmt;int olddim;
 Tbuffer buf;
 typedef uint8_t byte_t;
 struct atomic_operation_t
  {
   int i_source_channel_offset;
   int i_dest_channel_offset;
   unsigned int i_delay;/* in sample unit */
   double d_amplitude_factor;
  };
 struct aout_filter_sys_t
  {
  private:
   void ComputeChannelOperations (unsigned int i_rate , unsigned int i_next_atomic_operation
                                  , int i_source_channel_offset , double d_x , double d_z
                                  , double d_channel_amplitude_factor );
  public:
   size_t i_overflow_buffer_size;/* in bytes */
   byte_t * p_overflow_buffer;
   unsigned int i_nb_atomic_operations;
   atomic_operation_t * p_atomic_operations;
   int Init(const TsampleFormat &fmt,const TmixerSettings *cfg);
  } *p_sys;
 bool inited;
protected:
 virtual int getSupportedFormats(const TfilterSettingsAudio *cfg,bool *honourPreferred,const TsampleFormat &sf) const {return TsampleFormat::SF_FLOAT32;}
public:
 TaudioFilterHeadphone(IffdshowBase *Ideci,Tfilters *Iparent);
 virtual bool getOutputFmt(TsampleFormat &fmt,const TfilterSettingsAudio *cfg);
 virtual HRESULT process(TfilterQueue::iterator it,TsampleFormat &fmt,void *samples,size_t numsamples,const TfilterSettingsAudio *cfg0);
 virtual void done(void);
 virtual void onSeek(void);
};

DECLARE_FILTER(TaudioFilterHeadphone2,public,TaudioFilter)
private:
 TsampleFormat oldfmt;
 int indexes[9];
 float in[9];
 Tbuffer buf;

 struct af_hrtf_s
  {
  public:
   af_hrtf_s(const TsampleFormat &fmt);
   ~af_hrtf_s();
   void update_ch(const float *in, const int k);

   /* Lengths */
   int dlbuflen, hrflen;
   unsigned int basslen;
   /* L, C, R, Ls, Rs channels */
   std::vector<float> lf,rf,lr,rr,cf,cr,ra,la;
   const float *cf_ir, *af_ir, *of_ir, *ar_ir, *or_ir, *cr_ir;
   int cf_o, af_o, of_o, ar_o, or_o, cr_o;
   /* Bass */
   std::vector<float> ba_l,ba_r;
   float *ba_ir;
   /* Cyclic position on the ring buffer */
   int cyc_pos;
  } *s;

 static float passive_lock(float x);
 static float conv(const int nx, const int nk, const float *sx, const float *sk,const int offset);

 static const int DELAYBUFLEN=1024;	/* Length of the delay buffer */
 static const int HRTFFILTLEN=64;	/* HRTF filter length */
 static const int BASSFILTFREQ=180;	/* Bass compensation filter cut (Hz) */
 static const int BASSFILTLEN=193;	/* Bass compensation filter length */
 static const float BASSGAIN;       	/* Bass compensation gain */
 static const float BASSCROSS;    	/* Bass cross talk */

 static const float IRTHRESH;	/* Impulse response pruning thresh. */

 static const float M17_0DB;
 static const float M9_03DB;
 static const float M6_99DB;
 static const float M4_77DB;
 static const float M3_01DB;
 static const float M1_76DB;

 static const int CFECHODELAY=360;	/* Center front echo delay (samples) */

 static const float STEXPAND2;	/* Stereo expansion / 2 */

 static const float cf_filt[128],af_filt[128],of_filt[128],ar_filt[128],or_filt[128],cr_filt[128];

 static int pulse_detect(const float *sx);
protected:
 virtual int getSupportedFormats(const TfilterSettingsAudio *cfg,bool *honourPreferred,const TsampleFormat &sf) const {return TsampleFormat::SF_FLOAT32;}
public:
 TaudioFilterHeadphone2(IffdshowBase *Ideci,Tfilters *Iparent);
 virtual bool getOutputFmt(TsampleFormat &fmt,const TfilterSettingsAudio *cfg);
 virtual HRESULT process(TfilterQueue::iterator it,TsampleFormat &fmt,void *samples,size_t numsamples,const TfilterSettingsAudio *cfg0);
 virtual void done(void);
};

#endif
