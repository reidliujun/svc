#ifndef _CGRAPHPAGE_H_
#define _CGRAPHPAGE_H_

#include "TconfPageEnc.h"

class TgraphPage :public TconfPageEnc
{
private:
 static const int IDC_TMR_GRAPH=101;
 struct Tframe
  {
   void set(WPARAM wParam,LPARAM lParam)
    {
     size=(unsigned int)wParam;
     quant=(unsigned int)(lParam>>20);
     frametype=(unsigned int)(lParam&((1<<20)-1));
    }
   unsigned int frametype;
   unsigned int size;
   unsigned int quant;
  } *frames;
 void clearFrames(void);
 struct
  {
   BITMAPINFOHEADER bmiHeader;
   RGBQUAD bmiColors[256];
  } graphBmp;
 unsigned char *graphBits;
 HWND hg;
 int gx,gy,stride;
 void drawCompleteGraph(void);
 void putPixel(int x,int y,unsigned char c);
 void lineY(int x,int y1,int y2,unsigned char c),lineX(int x1,int x2,int y,unsigned char c);
 int shift;
 void shiftLeft(void);
 void shiftGraph(Tframe &frame);
 unsigned int oldframescount;clock_t t1;
 unsigned int oldmaxframesize;
 void debug2dlg(void),graph2dlg(void);
 void onDebugoutputfile(void);
protected:
 virtual INT_PTR msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
public:
 TgraphPage(TffdshowPageEnc *Iparent);
 virtual void init(void);
 virtual void cfg2dlg(void);
};

#endif

