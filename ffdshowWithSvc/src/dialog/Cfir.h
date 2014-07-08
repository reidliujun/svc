#ifndef _CFIRPAGE_H_
#define _CFIRPAGE_H_

#include "TconfPageDecAudio.h"

struct IaudioFilterFir;
class TfirPage :public TconfPageDecAudio
{
private:
 HWND hg;
 struct
  {
   BITMAPINFOHEADER bmiHeader;
   RGBQUAD bmiColors[256];
  } bmpFIR,bmpFFT;
 unsigned char *bitsFIR,*bitsFFT;
 unsigned int gx,gy;int stride;
 void drawFir(void),drawFir2(void);unsigned int *fir;
 void type2dlg(void),window2dlg(void),maxfreq2dlg(void);
 bool isFFT;
 unsigned int getFirFreq(void);
 void onUserDisplayMaxFreq(void),onFirType(void),onFirWindow(void);
 IaudioFilterFir *filter;
protected:
 virtual INT_PTR msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
public:
 TfirPage(TffdshowPageDec *Iparent,const TfilterIDFF *idff);
 virtual void init(void);
 virtual void cfg2dlg(void);
 virtual void translate(void);
 virtual void onFrame(void);
};

#endif

