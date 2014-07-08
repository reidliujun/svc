#ifndef _CRESIZESETTINGSPAGE_H_
#define _CRESIZESETTINGSPAGE_H_

#include "TconfPageDecVideo.h"

class TresizeSettingsPage :public TconfPageDecVideo
{
private:
 struct TparamsIDs
  {
   int idff_method;
   int idff_bicubicParam,idff_gaussParam,idff_lanczosParam;
   int idc_lbl,idc_tbr;
  };
 static const TparamsIDs idsLuma,idsChroma;
 void paramLuma2dlg(void),paramChroma2dlg(void),param2dlg(const TparamsIDs &ids),blurSharpen2dlg(void),warp2dlg(void);
 INT_PTR setParams(int pos,const TparamsIDs &ids);
protected:
 virtual INT_PTR msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
public:
 TresizeSettingsPage(TffdshowPageDec *Iparent,const TfilterIDFF *idff);
 virtual void init(void);
 virtual void cfg2dlg(void);
 virtual void translate(void);
};

#endif

