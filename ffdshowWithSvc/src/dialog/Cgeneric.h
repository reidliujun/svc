#ifndef _CGENERICPAGE_H_
#define _CGENERICPAGE_H_

#include "TconfPageEnc.h"

class TgenericPage :public TconfPageEnc
{
private:
 void kf2dlg(void);
 HWND hlv;
 typedef std::tuple<const char_t* /*name*/,int/*idff*/,int/*val*/,bool/*repaint*/> Tflag;
 enum {NAME=1,IDFF=2,VAL=3,REPAINT=4};
 typedef array_vector<Tflag,64> Tflags;
 Tflags flags;
 bool nostate;
protected:
 virtual INT_PTR msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
 virtual void b2dlg(void);
public:
 TgenericPage(TffdshowPageEnc *Iparent);
 virtual void init(void);
 virtual void cfg2dlg(void);
};

class TgenericX264page :public TgenericPage
  {
  protected:
   virtual void b2dlg(void);
  public:
   TgenericX264page(TffdshowPageEnc *Iparent);
   virtual bool forCodec(int codecId) {return x264_codec(codecId);}
   virtual void cfg2dlg(void);
  };

#endif

