#ifndef _CSUBTITLESPAGE_H_
#define _CSUBTITLESPAGE_H_

#include "TconfPageDecVideo.h"

class TsubtitlesPage :public TconfPageDecVideo
{
private:
 bool autosubfirsttime;strings autosubfiles;
 void sub2dlg(void),auto2dlg(void),stereo2dlg(void),expand2dlg(void);
 void loadSubtitles(void);
 void onLoadfile(void),onSearchdir(void),onExpandClick(void);
protected:
 virtual INT_PTR msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
public:
 TsubtitlesPage(TffdshowPageDec *Iparent,const TfilterIDFF *idff);
 virtual void init(void);
 virtual void cfg2dlg(void);
 virtual void applySettings(void);
 virtual void translate(void);
};

#endif

