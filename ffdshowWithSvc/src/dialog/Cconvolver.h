#ifndef _CCONVOLVERPAGE_H_
#define _CCONVOLVERPAGE_H_

#include "TconfPageDecAudio.h"

class TconvolverPage :public TconfPageDecAudio
{
private:
 void onFile(void),onManualFileSet(void),onManualFileClear(void);
 void format2dlg(const char_t *flnm),adjust2dlg(void),mapping2dlg(void);
 void enableMapping(bool is);
 HWND hlv;
 static const int speakersIdffs[];
 ffstring speakerFormats[6];
 ffstring selectFile(int idff,int idc=0);
 ffstring getWavDesc(const char_t *flnm,int idc);
protected:
 virtual INT_PTR msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
public:
 TconvolverPage(TffdshowPageDec *Iparent,const TfilterIDFF *idff);
 virtual void init(void);
 virtual void cfg2dlg(void);
 virtual void translate(void);
};

#endif

