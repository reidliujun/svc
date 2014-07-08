#ifndef _COUTSFSPAGE_H_
#define _COUTSFSPAGE_H_

#include "TconfPageDecAudio.h"
#include "ToutputAudioSettings.h"

class ToutsfsPage :public TconfPageDecAudio
{
private:
 void ac32dlg(int &outsfs),connect2dlg(void);
 static const int ac3bitrates[];
 TdevicesList *devicesList;
protected:
 virtual INT_PTR msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
public:
 ToutsfsPage(TffdshowPageDec *Iparent);
 void init(void);
 virtual bool reset(bool testonly=false);
 virtual void cfg2dlg(void);
 virtual void translate(void);
 virtual void applySettings(void);
 virtual void multichanneldevice2dlg(void);
};

#endif

