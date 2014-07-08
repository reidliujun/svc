#ifndef _CMEX264PAGE_H_
#define _CMEX264PAGE_H_

#include "TconfPageEnc.h"
#include "ffcodecs.h"

class TmeX264page :public TconfPageEnc
{
private:
 struct Trefinements
  {
   int id;
   const char_t *name;
  };
 static const Trefinements refinements[];
 static const char_t *predictions[];
 void method2dlg(void);
protected:
 virtual INT_PTR msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
public:
 TmeX264page(TffdshowPageEnc *Iparent);
 virtual void cfg2dlg(void);
 virtual bool forCodec(int codecId) {return x264_codec(codecId);}
 virtual void translate(void);
};

#endif

