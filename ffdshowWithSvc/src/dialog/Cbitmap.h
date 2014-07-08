#ifndef _CBITMAPPAGE_H_
#define _CBITMAPPAGE_H_

#include "TconfPageDecVideo.h"

class TbitmapPage :public TconfPageDecVideo
{
private:
 void pos2dlg(void),opacity2dlg(void);
 void onFlnm(void);
protected:
 virtual INT_PTR msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
public:
 TbitmapPage(TffdshowPageDec *Iparent,const TfilterIDFF *idff);
 virtual void init(void);
 virtual void cfg2dlg(void);
 virtual void translate(void);
};

#endif

