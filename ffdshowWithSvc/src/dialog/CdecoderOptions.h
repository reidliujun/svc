#ifndef _CMISCPAGE_H_
#define _CMISCPAGE_H_

#include "TconfPageDecVideo.h"

class TmiscPage :public TconfPageDecVideo
{
private:
 bool islavc;
 struct Tworkaround
  {
   int ff_bug;
   int idc_chb;
  };
 static const Tworkaround workarounds[];
 void onMatrixExport(void);
protected:
 virtual INT_PTR msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
public:
 TmiscPage(TffdshowPageDec *Iparent);
 virtual void init(void);
 virtual void cfg2dlg(void);
 virtual void getTip(char_t *tipS,size_t len);
 virtual bool reset(bool testonly=false);
 virtual void translate(void);
};

#endif

