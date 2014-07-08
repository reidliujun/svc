#ifndef _CLOGOAWAYPAGE_H_
#define _CLOGOAWAYPAGE_H_

#include "TconfPageDecVideo.h"

#pragma warning(push)
#pragma warning(disable:4201)

class TlogoawayPage :public TconfPageDecVideo
{
private:
 void mode2dlg(void),borders2dlg(void),points2dlg(void);
 struct Tcbx
  {
   int id;
   int idff;
  };
 static union Tcbxs
  {
   Tcbx cbxs[8];
   struct {Tcbx borders[4],points[4];};
  } const cbxs;
 void onParamFlnm(void);
protected:
 virtual INT_PTR msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
public:
 TlogoawayPage(TffdshowPageDec *Iparent,const TfilterIDFF *idff);
 virtual void init(void);
 virtual void cfg2dlg(void);
 virtual void translate(void);
};

#pragma warning(pop)

#endif

