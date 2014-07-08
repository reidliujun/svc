#ifndef _CINPAGE_H_
#define _CINPAGE_H_

#include "TconfPageEnc.h"

class TffProcVideo;
class TinPage :public TconfPageEnc
{
private:
 TffProcVideo *ffproc;
 void in2dlg(void),incsp2dlg(void),interlaced2dlg(void),lavcnr2dlg(void);
 void onProcConfig(void);
public:
 TinPage(TffdshowPageEnc *Iparent);
 virtual void init(void);
 virtual void cfg2dlg(void);
 virtual void translate(void);
};

#endif

