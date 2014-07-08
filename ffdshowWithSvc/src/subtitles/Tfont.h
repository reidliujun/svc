#ifndef _TFONT_H_
#define _TFONT_H_

#include "interfaces.h"
#include "ffImgfmt.h"
#include "TsubtitleProps.h"
#include "rational.h"

enum
{
 ALIGN_FFDSHOW=0,
 ALIGN_LEFT=1,
 ALIGN_CENTER=2,
 ALIGN_RIGHT=3
};

class TrenderedSubtitleLine;
class TfontManager;
struct Tconfig;
class TrenderedSubtitleLines: public std::vector<TrenderedSubtitleLine*>
{
public:
 struct TprintPrefs
  {
   TprintPrefs(IffdshowBase *Ideci):
    deci(Ideci),
    config(NULL),
    sizeDx(0),
    sizeDy(0),
    posXpix(0),
    fontchangesplit(false),
    textBorderLR(0),
    tabsize(8),
    dvd(false),
    blur(false),
    outlineBlur(false),
    clipdy(0),
    sar(1,1),
    opaqueBox(false),
    subformat(-1),
    isOSD(false),
    rtStart(REFTIME_INVALID)
    {
    }
   unsigned char **dst;
   const stride_t *stride;
   const unsigned int *shiftX,*shiftY;
   unsigned int dx,dy,clipdy;
   bool isOSD;
   int xpos,ypos;
   int align,alignSSA;
   int linespacing;
   unsigned int sizeDx,sizeDy;
   int posXpix;
   bool vobchangeposition;int vobscale,vobaamode,vobaagauss;
   bool fontchangesplit,fontsplit;
   int textBorderLR;
   int tabsize;
   bool dvd;
   IffdshowBase *deci;
   const Tconfig *config;
   int shadowMode, shadowAlpha; // Subtitles shadow
   double shadowSize;
   bool blur,outlineBlur;
   int csp,cspBpp;
   double outlineWidth;
   Rational sar;
   bool opaqueBox;
   int subformat;
   REFERENCE_TIME rtStart;
  };
 TrenderedSubtitleLines(void) {}
 TrenderedSubtitleLines(TrenderedSubtitleLine *ln) {push_back(ln);}
 void add(TrenderedSubtitleLine *ln,unsigned int *height);
 void clear(void);
 using std::vector<value_type>::empty;
 void print(const TprintPrefs &prefs);

private:
 void printASS(const TprintPrefs &prefs);
 class ParagraphKey
  {
   public:
    short alignment;
    short marginTop,marginBottom;
    short marginL,marginR;
    short isPos;
    short posx,posy;

    ParagraphKey(): alignment(-1), marginTop(-1), marginBottom(-1), marginL(-1), marginR(-1), isPos(false), posx(-1),posy(-1){};
  };
 friend bool operator < (const TrenderedSubtitleLines::ParagraphKey &a, const TrenderedSubtitleLines::ParagraphKey &b);

 class ParagraphValue
  {
   public:
    double topOverhang,bottomOverhang;
    double height,y;
    bool firstuse;

    ParagraphValue(): topOverhang(0), bottomOverhang(0), height(0), y(0), firstuse(true) {};
  };
 void prepareKey(const_iterator i,ParagraphKey &pkey,unsigned int prefsdx,unsigned int prefsdy);
};

bool operator < (const TrenderedSubtitleLines::ParagraphKey &a, const TrenderedSubtitleLines::ParagraphKey &b);

class TrenderedTextSubtitleWord;

class TcharsChache
{
private:
 typedef std::hash_map<int,TrenderedTextSubtitleWord*> Tchars;
 Tchars chars;
 HDC hdc;
 YUVcolorA yuv,outlineYUV,shadowYUV;
 int xscale,yscale;
 IffdshowBase *deci;
public:
 const YUVcolorA& getBodyYUV(void) const {return yuv;}
 const YUVcolorA& getOutlineYUV(void) const {return outlineYUV;}
 const YUVcolorA& getShadowYUV(void) const {return shadowYUV;}
 TcharsChache(HDC Ihdc,const YUVcolorA &Iyuv,const YUVcolorA &Ioutline,const YUVcolorA &Ishadow,int Ixscale,int Iyscale,IffdshowBase *Ideci);
 template<class tchar> const TrenderedTextSubtitleWord *getChar(tchar *s,const TrenderedSubtitleLines::TprintPrefs &prefs,const LOGFONT &lf,TSubtitleProps props);
 ~TcharsChache();
};

class TrenderedSubtitleWordBase
{
private:
 bool own;
public:
 TrenderedSubtitleWordBase(bool Iown):own(Iown) 
  {
   for (int i=0;i<3;i++)
    {
     bmp[i]=NULL;msk[i]=NULL;
     outline[i]=NULL;shadow[i]=NULL;
    }
  }
 ~TrenderedSubtitleWordBase();
 unsigned int dx[3],dy[3];
 unsigned int dxCharY,dyCharY;
 unsigned char *bmp[3],*msk[3];stride_t bmpmskstride[3];
 unsigned char *outline[3],*shadow[3];
 virtual void print(int startx, int starty, unsigned int dx[3],int dy[3],unsigned char *dstLn[3],const stride_t stride[3],const unsigned char *bmp[3],const unsigned char *msk[3],REFERENCE_TIME rtStart=REFTIME_INVALID) const =0;
 int csp;
 virtual int get_ascent64() const {return dy[0]*8;}
 virtual int get_descent64() const {return 0;}
 virtual int get_baseline() const {return dy[0];}
 virtual int get_topOverhang() const {return 0;}
 virtual int get_bottomOverhang() const {return 0;}
 virtual int get_leftOverhang() const {return 0;}
 virtual int get_rightOverhang() const {return 0;}
};

class TrenderedVobsubWord : public TrenderedSubtitleWordBase
{
private:
 bool shiftChroma;
public:
 TrenderedVobsubWord(bool IshiftChroma=true):shiftChroma(IshiftChroma),TrenderedSubtitleWordBase(false) {}
 virtual void print(int startx, int starty, unsigned int dx[3],int dy[3],unsigned char *dstLn[3],const stride_t stride[3],const unsigned char *bmp[3],const unsigned char *msk[3],REFERENCE_TIME rtStart=REFTIME_INVALID) const;
};

class TrenderedTextSubtitleWord : public TrenderedSubtitleWordBase
{
private:
 TrenderedTextSubtitleWord *secondaryColoredWord;
 TrenderedSubtitleLines::TprintPrefs prefs;
 YUVcolorA m_bodyYUV,m_outlineYUV,m_shadowYUV;
 int baseline;
 int topOverhang,bottomOverhang,leftOverhang,rightOverhang;
 int m_outlineWidth,m_shadowSize,m_shadowMode;
 int dstOffset;
 mutable int oldFader;
 mutable unsigned int oldBodyYUVa,oldOutlineYUVa;
 void drawShadow(       HDC hdc,
                        HBITMAP hbmp,
                        unsigned char *bmp16,
                        HGDIOBJ old,
                        double xscale,
                        const SIZE &sz
                       );
 void updateMask(int fader = 1 << 16, int create = 1) const; // create: 0=update, 1=new, 2=update after copy (karaoke)
 unsigned char* blur(unsigned char *src,stride_t Idx,stride_t Idy,int startx,int starty,int endx, int endy, bool mild);
 unsigned int getShadowSize(LONG fontHeight);
 unsigned int getBottomOverhang(void);
 unsigned int getRightOverhang(void);
 unsigned int getTopOverhang(void);
 unsigned int getLeftOverhang(void);
public:
 TSubtitleProps props;
 // full rendering
 template<class tchar> TrenderedTextSubtitleWord(
                        HDC hdc,
                        const tchar *s,
                        size_t strlens,
                        const YUVcolorA &YUV,
                        const YUVcolorA &outlineYUV,
                        const YUVcolorA &shadowYUV,
                        const TrenderedSubtitleLines::TprintPrefs &prefs,
                        LOGFONT lf,
                        double xscale,
                        TSubtitleProps Iprops);
 // secondary (for karaoke)
 TrenderedTextSubtitleWord(
                        const TrenderedTextSubtitleWord &parent,
                        bool senondaryColor                       // to distinguish from default copy constructor
                        );
 // fast rendering
 template<class tchar> TrenderedTextSubtitleWord(
                        TcharsChache *chars,
                        const tchar *s,size_t strlens,
                        const TrenderedSubtitleLines::TprintPrefs &prefs,
                        const LOGFONT &lf,
                        TSubtitleProps Iprops);
 ~TrenderedTextSubtitleWord();
 virtual void print(int startx, int starty, unsigned int dx[3],int dy[3],unsigned char *dstLn[3],const stride_t stride[3],const unsigned char *bmp[3],const unsigned char *msk[3],REFERENCE_TIME rtStart=REFTIME_INVALID) const;
 unsigned int alignXsize;
 void* (__cdecl *TtextSubtitlePrintY)  (const unsigned char* bmp,const unsigned char* outline,const unsigned char* shadow,const unsigned short *colortbl,const unsigned char* dst,const unsigned char* msk);
 void* (__cdecl *TtextSubtitlePrintUV) (const unsigned char* bmp,const unsigned char* outline,const unsigned char* shadow,const unsigned short *colortbl,const unsigned char* dstU,const unsigned char* dstV);
 void* (__cdecl *YV12_lum2chr_min)(const unsigned char* lum0,const unsigned char* lum1,unsigned char* chr);
 void* (__cdecl *YV12_lum2chr_max)(const unsigned char* lum0,const unsigned char* lum1,unsigned char* chr);
 virtual int get_ascent64() const;
 virtual int get_descent64() const;
 virtual int get_baseline() const;
 virtual int get_topOverhang() const {return topOverhang;}
 virtual int get_bottomOverhang() const {return bottomOverhang;}
 virtual int get_leftOverhang() const {return leftOverhang;}
 virtual int get_rightOverhang() const {return rightOverhang;}
};

class TrenderedSubtitleLine : protected std::vector<TrenderedSubtitleWordBase*>
{
 bool firstrun;
public:
 TrenderedSubtitleLine(void):firstrun(true) {props.reset();}
 TrenderedSubtitleLine(TSubtitleProps p):firstrun(true) {props=p;}
 TrenderedSubtitleLine(TrenderedSubtitleWordBase *w):firstrun(true) {push_back(w);props.reset();}
 unsigned int width(void) const;
 unsigned int height(void) const;
 double charHeight(void) const;
 unsigned int baselineHeight(void) const;
 int get_topOverhang(void) const;
 int get_bottomOverhang(void) const;
 int get_leftOverhang(void) const;
 int get_rightOverhang(void) const;
 void prepareKaraoke(void);
 using std::vector<value_type>::push_back;
 using std::vector<value_type>::empty;
 void clear(void);
 void print(int startx,int starty,const TrenderedSubtitleLines::TprintPrefs &prefs,unsigned int prefsdx,unsigned int prefsdy) const;
 TSubtitleProps props;
};

template<class tchar> struct TsubtitleWord;
struct TfontSettings;
struct Tsubtitle;
struct TfontSettings;
template<class tchar> struct TsubtitleTextBase;
class Tfont
{
private:
 IffdshowBase *deci;
 TfontManager *fontManager;
 TfontSettings *fontSettings;
 HDC hdc;HGDIOBJ oldFont;
 TrenderedSubtitleLines lines;
 unsigned int height;
 const Tsubtitle *oldsub;
 int oldCsp;
 YUVcolorA yuvcolor,outlineYUV,shadowYUV;
 short matrix[5][5];
 template<class tchar> void prepareC(const TsubtitleTextBase<tchar> *sub,const TrenderedSubtitleLines::TprintPrefs &prefs,bool forceChange);
 template<class tchar> int get_splitdx_for_new_line(const TsubtitleWord<tchar> &w,int splitdx,int dx) const;
 TcharsChache *charsCache;
 template<class tchar> TrenderedTextSubtitleWord* newWord(const tchar *s,size_t slen,TrenderedSubtitleLines::TprintPrefs prefs,const TsubtitleWord<tchar> *w,const LOGFONT &lf,bool trimRightSpaces=false);
public:
 Tfont(IffdshowBase *Ideci);
 ~Tfont();
 void init(const TfontSettings *IfontSettings);
 template<class tchar> void print(const TsubtitleTextBase<tchar> *sub,bool forceChange,const TrenderedSubtitleLines::TprintPrefs &prefs,unsigned int *y=NULL);
 void done(void);
};

extern "C" {
 void* (__cdecl TtextSubtitlePrintY_mmx)(const unsigned char* bmp,const unsigned char* outline,const unsigned char* shadow,const unsigned short* colortbl,const unsigned char* dst,const unsigned char* msk);
 void* (__cdecl TtextSubtitlePrintUV_mmx)(const unsigned char* bmp,const unsigned char* outline,const unsigned char* shadow,const unsigned short* colortbl,const unsigned char* dstU,const unsigned char* dstV);
 void* (__cdecl TtextSubtitlePrintY_sse2)(const unsigned char* bmp,const unsigned char* outline,const unsigned char* shadow,const unsigned short* colortbl,const unsigned char* dst,const unsigned char* msk);
 void* (__cdecl TtextSubtitlePrintUV_sse2)(const unsigned char* bmp,const unsigned char* outline,const unsigned char* shadow,const unsigned short* colortbl,const unsigned char* dstU,const unsigned char* dstV);
 void* (__cdecl YV12_lum2chr_min_mmx)(const unsigned char* lum0,const unsigned char* lum1,unsigned char* chr);
 void* (__cdecl YV12_lum2chr_max_mmx)(const unsigned char* lum0,const unsigned char* lum1,unsigned char* chr);
 void* (__cdecl YV12_lum2chr_min_mmx2)(const unsigned char* lum0,const unsigned char* lum1,unsigned char* chr);
 void* (__cdecl YV12_lum2chr_max_mmx2)(const unsigned char* lum0,const unsigned char* lum1,unsigned char* chr);
 void  __cdecl storeXmmRegs(unsigned char* buf);
 void  __cdecl restoreXmmRegs(unsigned char* buf);
 void __cdecl fontRGB32toBW_mmx(size_t count,unsigned char *ptr);
 unsigned int __cdecl fontPrepareOutline_sse2(const unsigned char *src,size_t srcStrideGap,const short *matrix,size_t matrixSizeH,size_t matrixSizeV);
 unsigned int __cdecl fontPrepareOutline_mmx (const unsigned char *src,size_t srcStrideGap,const short *matrix,size_t matrixSizeH,size_t matrixSizeV,size_t matrixGap);
}

#endif
