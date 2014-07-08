#ifndef _CPRESETSPAGE_H_
#define _CPRESETSPAGE_H_

#include "TconfPageDec.h"

class Tpresets;
class TpresetAutoloadDlgBase;
class TpresetsPage :public TconfPageDec
{
private:
 const char_t *presetfilter,*presetext;
 bool applying;
 HWND hlv;
 HFONT italicFont,boldFont;
 void changePresetState(void);
 bool nochange;
 void autopreset2dlg(void);
 void onAutopreset(void),onNewBt(void),onNewMenu(void),onNew(int menuCmd),onReadFromFile(void),onSaveToFile(void),onRemove(void),onRename(int menuCmd),onRenameBt(void),onRenameMenu(void);
 void addNewPreset(Tpreset *newPreset);
 void renamePreset(const char_t *presetName);
protected:
 TpresetAutoloadDlgBase* (*autoDlgCreate)(IffdshowBase *Ideci,HWND parent,int x,int y);
 virtual INT_PTR msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
public:
 TpresetsPage(TffdshowPageDec *Iparent,const char_t *Ipresetfilter,const char_t *Ipresetext);
 virtual void init(void);
 virtual bool enabled(void);
 virtual void cfg2dlg(void);
 virtual void translate(void);
};

class TffdshowPageDecVideo;
class TpresetsPageVideo :public TpresetsPage
{
public:
 TpresetsPageVideo(TffdshowPageDec *Iparent);
};

class TffdshowPageDecAudio;
class TpresetsPageAudio :public TpresetsPage
{
public:
 TpresetsPageAudio(TffdshowPageDec *Iparent);
};

#endif

