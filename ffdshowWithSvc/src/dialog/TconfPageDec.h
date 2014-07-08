#ifndef _TCONFPAGEDEC_H_
#define _TCONFPAGEDEC_H_

#include "TconfPageBase.h"
#include "IffdshowDec.h"

class TffdshowPageDec;
struct TfilterIDFF;
class TconfPageDec :public TconfPageBase
{
protected:
 TconfPageDec(TffdshowPageDec *Iparent,const TfilterIDFF *idff,int IfilterPageId);
 int idffInter,idffFull,idffHalf,idffOrder,idffShow;int resInter;
 void cfgInv(unsigned int i);
 comptrQ<IffdshowDec> deciD;
 virtual INT_PTR msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
 TffdshowPageDec *parent;
public:
 virtual int  getInter(void);
 virtual bool invInter(void);
 virtual int  getShow(void);
 virtual void setShow(int s);
 virtual int  getProcessFull(void);
 virtual void setProcessFull(int full);
 virtual int  getProcessHalf(void);
 virtual void setProcessHalf(int half);
 virtual void interDlg(void);
 static const int maxOrder=10000;
 virtual void setOrder(int o);
 virtual int  getOrder(void);
 virtual bool hasReset(void);
 virtual bool reset(bool testonly=false) {return false;}
 virtual void onNewFilter(void) {}
};

#endif
