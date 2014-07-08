#ifndef _CPICTPROPPAGE_H_
#define _CPICTPROPPAGE_H_

#include "TconfPageDecVideo.h"

class TpictPropPage :public TconfPageDecVideo
{
private:
 void lumGain2dlg(void),lumOffset2dlg(void),gamma2dlg(void),gammaRGB2dlg(void),hue2dlg(void),saturation2dlg(void),colorize2dlg(void),levelfix2dlg(void);
 static const char_t *scanlines[];
protected:
 virtual INT_PTR msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
public:
 TpictPropPage(TffdshowPageDec *Iparent,const TfilterIDFF *idff);
 virtual void init(void);
 virtual void cfg2dlg(void);
 virtual void translate(void);
};

#endif

