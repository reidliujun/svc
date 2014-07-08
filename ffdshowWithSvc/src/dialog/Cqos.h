#ifndef _CMISCPAGE_H_
#define _CMISCPAGE_H_

#include "TconfPageDecVideo.h"

class TqosPage :public TconfPageDecVideo
{
private:
 bool islavc;
protected:
 virtual INT_PTR msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
public:
 TqosPage(TffdshowPageDec *Iparent);
 virtual void init(void);
 virtual void cfg2dlg(void);
 virtual void getTip(char_t *tipS,size_t len);
 virtual bool reset(bool testonly=false);
 virtual void translate(void);
};

#endif

