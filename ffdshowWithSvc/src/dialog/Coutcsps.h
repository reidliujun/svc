#ifndef _COUTCSPSPAGE_H_
#define _COUTCSPSPAGE_H_

#include "TconfPageDecVideo.h"

class ToutcspsPage :public TconfPageDecVideo
{
private:
 static const int idcs[],idffs[];

 void dv2dlg(void),overlay2dlg(void),csp2dlg(void),dfc2dlg(void);
 virtual INT_PTR msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
 int *backupDV;
public:
 ToutcspsPage(TffdshowPageDec *Iparent);
 ~ToutcspsPage()
  {
   if(backupDV)
    free(backupDV);
  }
 virtual void init(void);
 virtual void cfg2dlg(void);
 virtual void getTip(char_t *tipS,size_t len);
 virtual void translate(void);
};

#endif

