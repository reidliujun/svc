#ifndef _TPRESETS_H_
#define _TPRESETS_H_

#include "interfaces.h"

struct Tpreset;
struct TautoPresetProps;
class Tpresets :public std::vector<Tpreset*>
{
private:
 const char_t *presetext;
 iterator findPreset(const char_t *presetName);
 void listRegKeys(strings &l);
protected:
 const char_t *reg_child;
 Tpresets(const char_t *Ireg_child,const char_t *Ipresetext):reg_child(Ireg_child),presetext(Ipresetext) {}
 Tpreset* getAutoPreset0(TautoPresetProps &aprops,bool filefirst);
public:
 virtual ~Tpresets();
 virtual Tpresets* newPresets(void)=0;
 void init(void);
 void done(void);
 virtual Tpreset* newPreset(const char_t *presetName=NULL)=0;
 virtual Tpreset* getPreset(const char_t *presetName,bool create);
 virtual Tpreset* getAutoPreset(IffdshowBase *deci,bool filefirst);
 void savePreset(Tpreset *preset,const char_t *presetName);
 bool savePresetFile(Tpreset *preset,const char_t *flnm);
 void storePreset(Tpreset *preset);
 bool removePreset(const char_t *presetName);
 void saveRegAll(void);
 void nextUniqueName(Tpreset *preset);
 void nextUniqueName(char_t *presetName, size_t buflen);
};

struct TpresetVideo;
struct TvideoAutoPresetProps;
class TpresetsVideo :public Tpresets
{
protected:
 int filtermode;
 TpresetsVideo(const char_t *Ireg_child, int mode):Tpresets(Ireg_child,_l("ffpreset")) {filtermode=mode;}
public:
 virtual Tpresets* newPresets(void) {return new TpresetsVideo(reg_child, filtermode);}
 virtual Tpreset* getAutoPreset(IffdshowBase *deci,bool filefirst);
 virtual Tpreset* newPreset(const char_t *presetName=NULL);
};

class TpresetsVideoProc :public TpresetsVideo
{
public:
 TpresetsVideoProc(int mode):TpresetsVideo(FFDSHOWDECVIDEO, mode) {}
 virtual Tpresets* newPresets(void) {return new TpresetsVideoProc(filtermode);}
};

class TpresetsVideoPlayer :public TpresetsVideo
{
public:
 TpresetsVideoPlayer(int mode):TpresetsVideo(FFDSHOWDECVIDEO, mode) {}
 virtual Tpresets* newPresets(void) {return new TpresetsVideoPlayer(filtermode);}
 virtual Tpreset* newPreset(const char_t *presetName=NULL);
};


class TpresetsVideoVFW :public TpresetsVideo
{
public:
 TpresetsVideoVFW(int mode):TpresetsVideo(FFDSHOWDECVIDEOVFW, mode) {}
 virtual Tpresets* newPresets(void) {return new TpresetsVideoVFW(filtermode);}
};

struct TaudioAutoPresetProps;
class TpresetsAudio :public Tpresets
{
public:
 TpresetsAudio(const char_t *IregChild=FFDSHOWDECAUDIO):Tpresets(IregChild,_l("ffApreset")) {}
 virtual Tpreset* getAutoPreset(IffdshowBase *deci,bool filefirst);
 virtual Tpresets* newPresets(void) {return new TpresetsAudio;}
 virtual Tpreset* newPreset(const char_t *presetName=NULL);
};

class TpresetsAudioRaw :public TpresetsAudio
{
public:
 TpresetsAudioRaw(void):TpresetsAudio(FFDSHOWDECAUDIORAW) {}
};

#endif
