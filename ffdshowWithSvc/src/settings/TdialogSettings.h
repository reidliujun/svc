#ifndef _TDIALOGSETTINGS_H_
#define _TDIALOGSETTINGS_H_

#include "Toptions.h"

struct TregOp;
struct Tconfig;
struct TdialogSettingsBase :Toptions
{
protected:
 const char_t *reg_child;
 virtual void reg_op(TregOp &t);
public:
 TdialogSettingsBase(const char_t *Ireg_child,TintStrColl *Icoll);
 virtual ~TdialogSettingsBase() {}
 bool loaded;
 int mainLastPage;
 int mainIsRestorePos;
 int mainPosX,mainPosY;
 int codecsLvWidth0,codecsLvWidth1,codecsLvWidth2;
 int codecsLastSel;
 int showHints;
 char_t lang[MAX_PATH];
 int translate;
 void load(const Tconfig *config),save(void);
};

struct TdialogSettingsDec :public TdialogSettingsBase
{
public:
 TdialogSettingsDec(const char_t *Ireg_child,TintStrColl *Icoll);
 int presetsLvWidth0;
 int keysLvWidth0,keysLvWidth1;
 int currentPage;
};

struct TdialogSettingsDecVideo :public TdialogSettingsDec
{
protected:
 virtual int getDefault(int id);
public:
 TdialogSettingsDecVideo(bool vfwdec,TintStrColl *Icoll);
 int dlgCustColor0,dlgCustColor1,dlgCustColor2,dlgCustColor3,dlgCustColor4,dlgCustColor5,dlgCustColor6,dlgCustColor7,dlgCustColor8,dlgCustColor9,dlgCustColor10,dlgCustColor11,dlgCustColor12,dlgCustColor13,dlgCustColor14,dlgCustColor15;
};

struct TdialogSettingsDecAudio :public TdialogSettingsDec
{
protected:
 virtual int getDefault(int id);
public:
 TdialogSettingsDecAudio(TintStrColl *Icoll,const char_t *Ireg_child=FFDSHOWDECAUDIO);
 int convolverLvWidth0,convolverLvWidth1,convolverLvWidth2,convolverLvSelected;
 int volumeDb;
};
struct TdialogSettingsDecAudioRaw :public TdialogSettingsDecAudio
{
 TdialogSettingsDecAudioRaw(TintStrColl *Icoll):TdialogSettingsDecAudio(Icoll,FFDSHOWDECAUDIORAW) {}
};

struct TdialogSettingsEnc :public TdialogSettingsBase
{
protected:
 virtual int getDefault(int id);
public:
 TdialogSettingsEnc(TintStrColl *Icoll);
 int showGraph;
 int perfectDlgX,perfectDlgY;
 int bpsFps1000;
 int bpsLen;
 int currentPage;
};

void getUILanguage(const char_t* pth, char_t* lang);
bool getpth(char_t* pth);
#endif
