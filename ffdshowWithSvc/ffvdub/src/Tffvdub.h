#ifndef _TFFVDUB_H_
#define _TFFVDUB_H_

#include "TpropertyPageSite.h"

struct IffProc;
struct IPropertyPage;
class TpropertyPageSite;
struct Tffvdub
{
private:
 IffProc *proc;
 IPropertyPage *page;
 TpropertyPageSite *pageSite;
 unsigned char *buf;unsigned int buflen;
 unsigned int srcDx,srcDy;
 unsigned int dstDx,dstDy,dstStride;
 void initProc(void);
 static void shiftWindow(HWND h,int dx,int dy);
public:
 bool ok,outchanged;
 Tffvdub(void);
 Tffvdub(const Tffvdub* Iffvdub);
 ~Tffvdub();

 int config(FilterActivation *fa,HWND m_hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
 int run(const FilterActivation *fa);
 bool fss(char *dstbuf,int dstbuflen);
 long param(FilterActivation *fa);
 void scriptConfig(IScriptInterpreter *isi, CScriptValue *argv, int argc);
};

#endif
