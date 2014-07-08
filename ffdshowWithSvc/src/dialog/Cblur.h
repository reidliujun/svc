#ifndef _CBLURPAGE_H_
#define _CBLURPAGE_H_

#include "TconfPageDecVideo.h"

class TblurPage :public TconfPageDecVideo
{
private:
 void tempsmooth2dlg(void),denoise3d2dlg(void),mplayer2dlg(void),swscaler2dlg(void),gradual2dlg(void),smooth2dlg(void),soften2dlg(void);
public:
 TblurPage(TffdshowPageDec *Iparent,const TfilterIDFF *idff);
 virtual void init(void);
 virtual void cfg2dlg(void);
};

#endif

