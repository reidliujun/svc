#ifndef _TSAMPLEFORMAT_H_
#define _TSAMPLEFORMAT_H_

struct VORBISFORMAT;
struct VORBISFORMAT2;
struct VORBISFORMATILL;
struct TsampleFormat
{
protected:
 void init(const WAVEFORMATEX &wfex,bool wfextcheck,const GUID *subtype);
 void init(const WAVEFORMATEXTENSIBLE &wfexten,const GUID *subtype);
 void init(const VORBISFORMAT &vf);
 void init(const VORBISFORMAT2 &vf2);
 void init(const VORBISFORMATILL &vfIll);
public:
 TsampleFormat(void):
  channelmask(0),
  nchannels(0),
  dolby(DOLBY_NO),
  freq(0),
  sf(SF_NULL),
  pcm_be(false)
  {
  }
 TsampleFormat(const WAVEFORMATEX &wfex,bool wfextcheck=true,const GUID *subtype=NULL);
 TsampleFormat(const WAVEFORMATEXTENSIBLE &wfexten,const GUID *subtype=NULL);
 TsampleFormat(const VORBISFORMAT &vf);
 TsampleFormat(const VORBISFORMAT2 &vf2);
 TsampleFormat(const VORBISFORMATILL &vfIll);
 TsampleFormat(const AM_MEDIA_TYPE &mt);
 TsampleFormat(int Isf,int Ifreq,int Inumchannels,int Ichannelmask=0):sf(Isf),freq(Ifreq)
  {
   setChannels(Inumchannels,Ichannelmask);
  }
 void reset(void)
  {
   channelmask=0;
   nchannels=0;
   freq=0;
   sf=0;
   dolby=DOLBY_NO;
  }
 CMediaType toCMediaType(bool alwaysextensible=true) const;
 static CMediaType createMediaTypeSPDIF(unsigned int frequency);
 WAVEFORMATEXTENSIBLE toWAVEFORMATEXTENSIBLE(bool alwaysextensible=true) const;

 enum
  {
   SF_NULL   =0,
   SF_PCM16  =1,
   SF_PCM24  =2,
   SF_PCM32  =4,
   SF_FLOAT32=8,

   SF_ALL     =SF_PCM16|SF_PCM24|SF_PCM32|SF_FLOAT32,
   SF_ALLINT  =SF_PCM16|SF_PCM24|SF_PCM32,
   SF_ALL_24  =SF_PCM16|SF_PCM32|SF_FLOAT32,

   SF_AC3     =16,
   SF_PCM8    =32,
   SF_LPCM16  =64,
   SF_LPCM20  =128,
   SF_FLOAT64 =65536,

   SF_ALLFLOAT=SF_FLOAT32|SF_FLOAT64
  };

 enum
  {
   DOLBY_NO         =0,  // Stream is not Dolby-encoded
   DOLBY_SURROUND   =1,  // Dolby Surround
   DOLBY_PROLOGIC   =2,  // Dolby Pro Logic
   DOLBY_PROLOGICII =3,  // Dolby Pro Logic II
   DOLBY_LFE        =4   // LFE presence flag
  };

 int sf;
 unsigned int freq;
 unsigned int nchannels;
 unsigned int channelmask;
 int speakers[8];
 int dolby;
 bool pcm_be;

 void setChannels(int Inchannels,int IchannelMask=0)
  {
   nchannels=std::min(8,Inchannels);
   channelmask=IchannelMask;
   if (channelmask==0)
    {
     static const int speakersPresets[8][8]=
      {
       SPEAKER_FRONT_CENTER,0,0,0,0,0,0,0,
       SPEAKER_FRONT_LEFT,SPEAKER_FRONT_RIGHT,0,0,0,0,0,0,
       SPEAKER_FRONT_LEFT,SPEAKER_FRONT_RIGHT,SPEAKER_BACK_CENTER,0,0,0,0,0,
       SPEAKER_FRONT_LEFT,SPEAKER_FRONT_RIGHT,SPEAKER_BACK_LEFT,SPEAKER_BACK_RIGHT,0,0,0,0,
       SPEAKER_FRONT_LEFT,SPEAKER_FRONT_RIGHT,SPEAKER_FRONT_CENTER,SPEAKER_BACK_LEFT,SPEAKER_BACK_RIGHT,0,0,0,
       SPEAKER_FRONT_LEFT,SPEAKER_FRONT_RIGHT,SPEAKER_FRONT_CENTER,SPEAKER_LOW_FREQUENCY,SPEAKER_BACK_LEFT,SPEAKER_BACK_RIGHT,0,0,
       SPEAKER_FRONT_LEFT,SPEAKER_FRONT_RIGHT,SPEAKER_FRONT_CENTER,SPEAKER_BACK_LEFT,SPEAKER_BACK_RIGHT,SPEAKER_SIDE_LEFT,SPEAKER_SIDE_RIGHT,0,
       SPEAKER_FRONT_LEFT,SPEAKER_FRONT_RIGHT,SPEAKER_FRONT_CENTER,SPEAKER_LOW_FREQUENCY,SPEAKER_BACK_LEFT,SPEAKER_BACK_RIGHT,SPEAKER_SIDE_LEFT,SPEAKER_SIDE_RIGHT,
      };
     memcpy(speakers,speakersPresets[nchannels-1],sizeof(int)*8);
    }
   else
    {
     static const int speakersOrder[]={SPEAKER_FRONT_LEFT,SPEAKER_FRONT_RIGHT,SPEAKER_FRONT_CENTER,SPEAKER_LOW_FREQUENCY,SPEAKER_BACK_LEFT,SPEAKER_BACK_RIGHT,SPEAKER_FRONT_LEFT_OF_CENTER,SPEAKER_FRONT_RIGHT_OF_CENTER,SPEAKER_BACK_CENTER,SPEAKER_SIDE_LEFT,SPEAKER_SIDE_RIGHT,SPEAKER_TOP_CENTER,SPEAKER_TOP_FRONT_LEFT,SPEAKER_TOP_FRONT_CENTER,SPEAKER_TOP_FRONT_RIGHT,SPEAKER_TOP_BACK_LEFT,SPEAKER_TOP_BACK_CENTER,SPEAKER_TOP_BACK_RIGHT};
     for (unsigned int i=0,c=0;i<sizeof(speakersOrder) && c<nchannels;i++)
      if (channelmask&speakersOrder[i])
       speakers[c++]=speakersOrder[i];
    }
  }
 int findSpeaker(int spk) const
  {
   for (unsigned int i=0;i<nchannels;i++)
    if (speakers[i]==spk)
     return i;
   return -1;
  }
 int makeChannelMask(void) const
  {
   int mask=0;
   for (unsigned int i=0;i<nchannels;i++)
    mask|=speakers[i];
   return mask;
  }
 static const int standardChannelMasks[];
 int makeChannelMask2(void) const
  {
   if (channelmask)
    return channelmask;
   else
    return standardChannelMasks[nchannels-1];
  }
 int nfront(void) const
  {
   return countbits(makeChannelMask2()&(SPEAKER_FRONT_LEFT|SPEAKER_FRONT_RIGHT|SPEAKER_FRONT_CENTER|SPEAKER_FRONT_LEFT_OF_CENTER|SPEAKER_FRONT_RIGHT_OF_CENTER));
  }
 int nrear(void) const
  {
   return countbits(makeChannelMask2()&(SPEAKER_BACK_LEFT|SPEAKER_BACK_CENTER|SPEAKER_BACK_RIGHT));
  }
 int nside(void) const
  {
   return countbits(makeChannelMask2()&(SPEAKER_SIDE_LEFT|SPEAKER_SIDE_RIGHT));
  }

 unsigned int bitsPerSample(void) const
  {
   switch (sf)
    {
     case SF_PCM8:return 8;
     default:
     case SF_LPCM16:
     case SF_PCM16:return 16;
     case SF_PCM24:return 24;
     case SF_PCM32:
     case SF_FLOAT32:return 32;
     case SF_FLOAT64:return 64;
    }
  }
 unsigned int avgBytesPerSec(void) const
  {
   return freq*blockAlign();
  }
 unsigned int blockAlign(void) const
  {
   return nchannels*bitsPerSample()/8;
  }

 bool operator !(void) const
  {
   return !nchannels;
  }
 void description(char_t *buf,size_t buflen) const
  {
   const char_t *chnls=_l("");
   switch (nchannels)
    {
     case 1:chnls=_l("mono");break;
     case 2:chnls=_l("stereo");break;
     default:
      tsnprintf_s(buf, buflen, _TRUNCATE, _l("%u Hz, %u channels"), freq, nchannels);
      buf[buflen-1]='\0';
      return;
    }
   tsnprintf_s(buf, buflen, _TRUNCATE, _l("%u Hz, %s"), freq, chnls);
   buf[buflen-1]='\0';
  }
 static const char_t *descriptionPCM(int sf,bool pcm_be=false)
  {
   switch (sf)
    {
     case SF_PCM8:return _l("8-bit integer");
     case SF_PCM16:return pcm_be?_l("16-bit integer BE"):_l("16-bit integer");
     case SF_PCM24:return pcm_be?_l("24-bit integer BE"):_l("24-bit integer");
     case SF_PCM32:return pcm_be?_l("32-bit integer BE"):_l("32-bit integer");
     case SF_FLOAT32:return pcm_be?_l("32-bit float BE"):_l("32-bit float");
     case SF_FLOAT64:return pcm_be?_l("64-bit float BE"):_l("64-bit float");
     case SF_AC3:return _l("ac3");
     case SF_LPCM16:return _l("16-bit LPCM");
     case SF_LPCM20:return _l("20-bit LPCM");
     default:return _l("unknown");
    }
  }
 const char_t *descriptionPCM(void) const
  {
   return descriptionPCM(sf,pcm_be);
  }
 void descriptionPCM(char_t *buf,size_t buflen) const
  {
   if (freq==0)
    ff_strncpy(buf,_l("none"),buflen);
   else
    {
     char_t *buf2=(char_t*)_alloca(buflen*sizeof(char_t));
     description(buf2,buflen);
     tsnprintf_s(buf, buflen, _TRUNCATE, _l("%s %s"), buf2, descriptionPCM());
    }
   buf[buflen-1]='\0';
  }
 static const char_t *getSpeakerName(int speaker,bool shrt=false);
 void getSpeakersDescr(char_t *buf,size_t buflen,bool shrt=false) const;

 static int sf_bestMatch(int sfIn,int wantedSFS);
 static DWORD getPCMformat(const CMediaType &mtIn,DWORD def=0);

 bool operator !=(const TsampleFormat &sf2) const
  {
    return memcmp(this,&sf2,sizeof(*this)+sizeof(bool)-sizeof(int))!=0;
  }
};

#pragma pack(push,1)
struct int24_t
{
private:
 uint8_t v[3];
 void init(int32_t x)
  {
   v[0]=((uint8_t*)&x)[0];
   v[1]=((uint8_t*)&x)[1];
   v[2]=((uint8_t*)&x)[2];
  }
public:
 int24_t(void) {}
 int24_t(int32_t x)
  {
   init(x);
  }
 int24_t(int64_t x)
  {
   init(int32_t(x));
  }
 int24_t(float x)
  {
   init(int32_t(x));
  }
 operator int32_t(void) const
  {
   int32_t i32;
   ((uint8_t*)&i32)[0]=0;
   ((uint8_t*)&i32)[1]=v[0];
   ((uint8_t*)&i32)[2]=v[1];
   ((uint8_t*)&i32)[3]=v[2];
   return i32>>8;
  }
} __attribute__((packed));
#pragma pack(pop)

template<class Tdst,class Tsrc> inline Tdst convertSample(const Tsrc src)
{
 return Tdst(src);
}
// from mppdec by Frank Klemm
/*
template<> inline int16_t convertSample(const float x) //TODO: really faster?
{
 float floattmp = x + (int32_t)0x00FD8000L;
 return int16_t(*(int32_t*)(&floattmp) - (int32_t)0x4B7D8000L);
}
template<> inline int32_t convertSample(const float x)
{
 double doubletmp = double(x) + 0x001FFFFD80000000LL;
 return int32_t(*(int64_t*)(&doubletmp) - 0x433FFFFD80000000LL);
}
*/
template<typename T> struct TsampleFormatInfo
{
 static inline T min(void);
 static inline T max(void);
 static inline unsigned int bps(void);
};
template<> struct TsampleFormatInfo<int16_t>
{
 static inline int16_t min(void) {return SHRT_MIN;}
 static inline int16_t max(void) {return SHRT_MAX;}
 static inline unsigned int bps(void) {return 16;}
 static inline int sf(void) {return TsampleFormat::SF_PCM16;}
 template<class Tv> static inline int16_t limit(Tv v)
  {
   if (v<min())
    return min();
   else if (v>max())
    return max();
   else
    return convertSample<int16_t,Tv>(v);
  }
 typedef int32_t helper_t;
};
template<> inline int16_t TsampleFormatInfo<int16_t>::limit<int32_t>(int32_t v)
{
 v+=0x8000;
 if ((uint32_t)v>=0x10000)
  v=~v>>31;
 return int16_t(v-0x8000);
}

template<> struct TsampleFormatInfo<int24_t>
{
 static inline int24_t min(void) {return -(1<<23);}
 static inline int24_t max(void) {return (1<<23)-1;}
 static inline unsigned int bps(void) {return 24;}
 static inline int sf(void) {return TsampleFormat::SF_PCM24;}
 template<class Tv> static inline int24_t limit(Tv v)
  {
   if (v<min())
    return min();
   else if (v>max())
    return max();
   else
    return convertSample<int24_t,Tv>(v);
  }
 typedef int64_t helper_t;
};
template<> struct TsampleFormatInfo<int32_t>
{
 static inline int32_t min(void) {return INT32_MIN;}
 static inline int32_t max(void) {return INT32_MAX;}
 static inline unsigned int bps(void) {return 32;}
 static inline int sf(void) {return TsampleFormat::SF_PCM32;}
 template<class Tv> static inline int32_t limit(Tv v)
  {
   if (v<min())
    return min();
   else if (v>max())
    return max();
   else
    return convertSample<int32_t,Tv>(v);
  }
/*
 template<> static int32_t limit<int64_t>(int64_t v)
  {
   v+=int64_t(0x80000000);
   if ((uint64_t)v>=int64_t(0x100000000))
    v=~v>>63;
   return int32_t(v-int64_t(0x80000000));
  };*/
 typedef int64_t helper_t;
};
template<> struct TsampleFormatInfo<float>
{
 static inline float min(void) {return -1.0f;}
 static inline float max(void) {return 1.0f;}
 static inline unsigned int bps(void) {return 32;}
 static inline int sf(void) {return TsampleFormat::SF_FLOAT32;}
 template<class Tv> static inline float limit(Tv v)
  {
   if (v<min())
    return min();
   else if (v>max())
    return max();
   else
    return convertSample<float,Tv>(v);
  }
 typedef float helper_t;
};

template<class Tout,class Tbase=std::vector< std::vector<Tout> > > struct TwavReader : Tbase , TsampleFormat
{
private:
 HMMIO hmmioFile;
 MMCKINFO chunkDATA,chunkRIFF;
 template<class Tin> bool read(unsigned int nsamples)
  {
   typename TsampleFormatInfo<Tout>::helper_t min=TsampleFormatInfo<Tout>::max(),max=TsampleFormatInfo<Tout>::min();
   for (unsigned int sample=0;sample<nsamples;sample++)
    for (unsigned int ch=0;ch<nchannels;ch++)
     {
      Tin in;
      if (mmioRead(hmmioFile,(HPSTR)&in,sizeof(in))!=sizeof(in))
       return false;
      Tout out=convertSample<Tout>(in)/-Tout(TsampleFormatInfo<Tin>::min());
      if (out>max) max=out;
      else if (out<min) min=out;
      (*this)[ch][sample]=out;
     }
   typename TsampleFormatInfo<Tout>::helper_t range=TsampleFormatInfo<Tout>::max()-TsampleFormatInfo<Tout>::min();
   adjust=range/(max-min);
   return true;
  }
 Tout adjust;
public:
 TwavReader(unsigned int Ifreq,unsigned int Inchannels,size_t Isize,const Tout &val):Tbase(Inchannels),TsampleFormat(TsampleFormatInfo<Tout>::sf(),Ifreq,Inchannels)
  {
   for (typename Tbase::iterator ch=this->begin();ch!=this->end();ch++)
    ch->resize(Isize,val);
   adjust=1;
  }
 TwavReader(const char_t *flnm,bool parseOnly=false)
  {
   MMIOINFO mmi;
   memset(&mmi,0,sizeof mmi);
   static const LONG inputBufferSize=10000;
   mmi.cchBuffer=inputBufferSize;
   hmmioFile=mmioOpen(LPTSTR(flnm),&mmi,MMIO_READ|MMIO_ALLOCBUF);
   if (hmmioFile)
    {
     chunkRIFF.fccType=mmioFOURCC('W','A','V','E');
     if (mmioDescend(hmmioFile,&chunkRIFF,NULL,MMIO_FINDRIFF)==MMSYSERR_NOERROR)
      {
       chunkDATA.ckid=mmioFOURCC('f','m','t',' ');
       if (mmioDescend(hmmioFile,&chunkDATA,&chunkRIFF,MMIO_FINDCHUNK)==MMSYSERR_NOERROR)
        {
         WAVEFORMATEX *wfmt=(WAVEFORMATEX*)malloc(chunkDATA.cksize);
         if (mmioRead(hmmioFile,(char*)wfmt,chunkDATA.cksize)==LONG(chunkDATA.cksize) && (wfmt->wFormatTag==WAVE_FORMAT_IEEE_FLOAT || wfmt->wFormatTag==WAVE_FORMAT_PCM || wfmt->wFormatTag==WAVE_FORMAT_EXTENSIBLE))
          {
           init(*wfmt,true,NULL);
           if (mmioAscend(hmmioFile,&chunkDATA,0)==MMSYSERR_NOERROR)
            {
             chunkDATA.ckid=mmioFOURCC('d','a','t','a');
             if (mmioDescend(hmmioFile,&chunkDATA,&chunkRIFF,MMIO_FINDCHUNK)==MMSYSERR_NOERROR)
              {
               unsigned int nsamples=chunkDATA.cksize/blockAlign();
               this->resize(nchannels);
               for (unsigned int i=0;i<nchannels;i++)
                this->at(i).resize(nsamples);
               if (!parseOnly)
                {
                 bool ret=false;
                 switch (sf)
                  {
                   case TsampleFormat::SF_PCM16:ret=read<int16_t>(nsamples);break;
                   case TsampleFormat::SF_PCM32:ret=read<int32_t>(nsamples);break;
                   case TsampleFormat::SF_FLOAT32:ret=read<float>(nsamples);break;
                  }
                 if (!ret) this->clear();
                }
              }
            }
          }
         free(wfmt);
        }
      }
     mmioClose(hmmioFile,0);
    }
  }
 Tout getAdjust(void) const {return adjust;}
};

#endif
