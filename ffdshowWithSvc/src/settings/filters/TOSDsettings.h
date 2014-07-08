#ifndef _TOSDSETTINGS_H_
#define _TOSDSETTINGS_H_

#include "TfilterSettings.h"
#include "TfontSettings.h"

struct TOSDsettings :TfilterSettings
{
public:
 typedef std::pair<ffstring,ffstring> Tpreset;
 typedef std::vector<Tpreset> Tpresets;
private:
 TOSDsettings& operator =(const TOSDsettings&);
 char_t curPreset[40];void onCurPresetChange(int id,const char_t*val);
 const char_t *defPreset;
 Tpresets presets;
 mutable bool changed;mutable const char_t *oldformat;
protected:
 TOSDsettings(size_t IsizeofthisAll,TintStrColl *Icoll,TfilterIDFFs *filters,const char_t *IdefPreset);
public:
 size_t getSize(void) const {return presets.size();}
 const char_t* getPresetName(unsigned int i) const {return i>=presets.size()?NULL:presets[i].first.c_str();}
 HRESULT setPresetName(unsigned int i,const char_t *name);
 const char_t* getStartupFormat(int *duration) const;
 const char_t* getFormat(const char_t *presetName) const;
 const char_t* getFormat(void) const;
 void addPreset(const char_t *presetName,const char_t *format);
 bool setFormat(const char_t *presetName,const char_t *format);
 bool erase(const char_t *presetName);
 bool cycle(void);

 char_t startPreset[40];int startDuration;
 int isSave,saveOnly;char_t saveFlnm[MAX_PATH];

 void savePresets(const char_t *reg_child),loadPresets(const char_t *reg_child);
};

struct TOSDsettingsVideo : TOSDsettings
{
public:
 TOSDsettingsVideo(TintStrColl *Icoll=NULL,TfilterIDFFs *filters=NULL);

 TfontSettingsOSD font;
 char_t user[2048];int userPx,userPy;
 int linespace;
 int posX,posY;
 int userFormat;
 void resetLook(void);

 virtual void reg_op(TregOp &t);
 virtual void createFilters(size_t filtersorder,Tfilters *filters,TfilterQueue &queue) const;
 virtual void createPages(TffdshowPageDec *parent) const {}
};

struct TOSDsettingsAudio : TOSDsettings
{
public:
 TOSDsettingsAudio(TintStrColl *Icoll=NULL,TfilterIDFFs *filters=NULL);

 virtual void createFilters(size_t filtersorder,Tfilters *filters,TfilterQueue &queue) const;
 virtual void createPages(TffdshowPageDec *parent) const {}
};

#endif
