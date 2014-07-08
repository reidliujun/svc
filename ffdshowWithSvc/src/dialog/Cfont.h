#ifndef _CFONTPAGE_H_
#define _CFONTPAGE_H_

#include "TconfPageDecVideo.h"

class TfontPage :public TconfPageDecVideo
{
private:
 void size2dlg(void);
 void selectCharset(int ii);
 static int CALLBACK EnumFamCallBackCharsets(CONST LOGFONT *lpelf,CONST TEXTMETRIC *lpntm,DWORD FontType,LPARAM lParam);
 static int CALLBACK EnumFamCallBackFonts(CONST LOGFONT *lpelf,CONST TEXTMETRIC *lpntm,DWORD FontType,LPARAM lParam);
 HWND cbxCharset;
 HFONT boldFont;int validCharsets[256];
protected:
 void font2dlg(void),spacingxscale2dlg(void);
 virtual void shadow2dlg(void);
 virtual void shadowSize2dlg(void); 
 virtual void shadowAlpha2dlg(void);
 void fillCharsets(void);
 TfontPage(TffdshowPageDec *Iparent,const TfilterIDFF *idff=NULL,int IfilterPageId=0);
 virtual Twidget* createDlgItem(int id,HWND h);
 virtual int getTbrIdff(int id,const TbindTrackbars bind);
 virtual INT_PTR msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
 int idff_fontcharset,idff_fontname,idff_fontautosize,idff_fontautosizevideowindow,idff_fontsizep,idff_fontsizea,idff_fontspacing,idff_fontoutlinewidth,idff_fontweight,idff_fontcolor,idff_fontxscale,idff_fontyscale,idff_fontfast,idff_fontoverridescale,idff_fontaspectauto;
 int idff_fontoutlinecolor,idff_fontshadowcolor,idff_fontbodyalpha,idff_fontoutlinealpha;
 int idff_fontshadowalpha,idff_fontshadowsize,idff_fontshadowmode;
 int idff_fontopaquebox,idff_fontblur;
public:
 virtual void init(void);
 virtual void cfg2dlg(void);
 virtual void translate(void);
 virtual bool reset(bool testonly=false);
};

class TfontPageSubtitles :public TfontPage
{
protected:
public:
 TfontPageSubtitles(TffdshowPageDec *Iparent,const TfilterIDFF *idff);
};

class TfontPageOSD :public TfontPage
{
protected:
public:
 TfontPageOSD(TffdshowPageDec *Iparent);
};

#endif

