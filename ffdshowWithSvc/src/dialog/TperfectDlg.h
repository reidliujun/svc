#ifndef _TPERFECTDLG_H_
#define _TPERFECTDLG_H_

#include "Twindow.h"
#include "Tperfect.h"

class TsecondPassPage;
struct TdialogSettingsEnc;
struct IffdshowEnc;
class TperfectDlg :public TdlgWindow
{
private:
 const char_t *_drawgraphframe,*_quantpercentage,*_quantcount;
 comptrQ<IffdshowEnc> deciE;
 Tperfect perfect;
 char_t oldStats1flnm[MAX_PATH];
 TxvidStats *statsRead;
 void initStatsRead(void);
 void init(void);

 void drawGraph(void);
 void onAnalyze(void),onSimulate(void);
 struct
 {
  BITMAPINFOHEADER bmiHeader;
  RGBQUAD bmiColors[256];
 } graphBmp,qpBmp,qcntBmp;
 unsigned char *graphBits,*qpBits,*qcntBits;
 HWND hg,hqp,hqcnt;
 HWND hTipgraph,hTipqp,hTipqcnt;

 int gx,gy,stride,qpy,qcnty;
 void lineY(unsigned char *bits,int x,int y1,int y2,unsigned char c),lineX(unsigned char *bits,int x1,int x2,int y,unsigned char c);

 void analyze(void),simulate(void);
protected:
 virtual INT_PTR msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
public:
 HWND hwndMain;
 TsecondPassPage *parent;
 TperfectDlg(TsecondPassPage *Iparent);
 ~TperfectDlg();
 void show(void);
};

#endif
