#ifndef _TPRESETSETTINGSAUDIO_H_
#define _TPRESETSETTINGSAUDIO_H_

#include "TpresetSettings.h"

struct TaudioAutoPresetProps :TautoPresetProps
{
private:
 comptrQ<IffdshowDecAudio> deciA;
 char_t nchannels[10];
 double freq;
public:
 TaudioAutoPresetProps(IffdshowBase *Ideci);
 const char_t* getNchannels(void);
 const char_t *getFreq(void);
 bool freqMatch(const char_t *mask,const char_t *flnm);
};

struct ToutputAudioSettings;
struct TpresetAudio :public Tpreset
{
public:
 TpresetAudio(const char_t *IpresetName, const char_t *Ireg_child, int filtermode=0);
 virtual ~TpresetAudio() {}
 TpresetAudio& operator=(const TpresetAudio &src)
  {
   Tpreset::operator =(src);

   preferredsfs=src.preferredsfs;
   dithering=src.dithering;noiseShaping=src.noiseShaping;
   decoderDRC = src.decoderDRC;
   decoderDRCLevel = src.decoderDRCLevel;

   return *this;
  }
 virtual Tpreset* copy(void) {return new_copy(this);}

 int preferredsfs;
 int dithering,noiseShaping,decoderDRC,decoderDRCLevel;

 ToutputAudioSettings *output;
};

struct TpresetAudioRaw :public TpresetAudio
{
 TpresetAudioRaw(const char_t *IpresetName):TpresetAudio(IpresetName,FFDSHOWDECAUDIORAW) {}
};

#endif
