#ifndef _TSUBTITLEPROPS_H_
#define _TSUBTITLEPROPS_H_
struct TfontSettings;
struct Rational;
class TfontManager;

struct TSubtitleProps
{
 TSubtitleProps(void) {reset();}
 TSubtitleProps(unsigned int IrefResX,unsigned int IrefResY, int IwrapStyle) {reset();refResX=IrefResX;refResY=IrefResY;wrapStyle=IwrapStyle;}
 TSubtitleProps(bool Iitalic, bool Iunderline) {reset();italic=Iitalic;underline=Iunderline;}
 int bold;
 bool italic,underline,strikeout,blur;
 bool isColor;COLORREF color,SecondaryColour, TertiaryColour, OutlineColour, ShadowColour;
 int colorA,SecondaryColourA, TertiaryColourA, OutlineColourA, ShadowColourA;
 bool isPos;int posx,posy;
 unsigned int refResX,refResY;
 int wrapStyle; // -1 = default
 int size;
 int scaleX,scaleY; //in percents, -1 = default
 char_t fontname[LF_FACESIZE];
 int encoding; // -1 = default
 int version;  // -1 = default
 int extendedTags; // 0 = default
 double spacing;  //INT_MIN = default
 void reset(void);
 HGDIOBJ toGdiFont(HDC hdc, LOGFONT &lf, const TfontSettings &fontSettings, unsigned int dx, unsigned int dy, unsigned int clipdy, const Rational& sar, TfontManager *fontManager) const;
 void toLOGFONT(LOGFONT &lf, const TfontSettings &fontSettings, unsigned int dx, unsigned int dy, unsigned int clipdy, const Rational& sar) const;
 void fix_size(LOGFONT &lf, HDC hdc, TfontManager *fontManager) const;

 // Alignment. This sets how text is "justified" within the Left/Right onscreen margins,
 // and also the vertical placing. Values may be 1=Left, 2=Centered, 3=Right.
 // Add 4 to the value for a "Toptitle". Add 8 to the value for a "Midtitle".
 // eg. 5 = left-justified toptitle]
 // -1 = default(center)
 int alignment;

 int marginR,marginL,marginV,marginTop,marginBottom; // -1 = default
 int borderStyle; // -1 = default
 double outlineWidth,shadowDepth; // -1 = default
 int get_spacing(unsigned int dy,unsigned int clipdy) const;
 int get_marginR(unsigned int screenWidth) const;
 int get_marginL(unsigned int screenWidth) const;
 int get_marginTop(unsigned int screenHeight) const;
 int get_marginBottom(unsigned int screenHeight) const;
 int get_xscale(int Ixscale,const Rational& sar,int aspectAuto,int overrideScale) const;
 int get_yscale(int Iyscale,const Rational& sar,int aspectAuto,int overrideScale) const;
 static int alignASS2SSA(int align);
 mutable int m_ascent64,m_descent64; // multiple 64 to lfHeight. Get TextMetrics and plus 4 to tmAscent or tmDescent and divid by 8 for vsfilter compatibility.
 int tmpFadT1,tmpFadT2;
 int isFad;
 int fadeA1,fadeA2,fadeA3;
 bool karaokeNewWord; // true if the word is top of karaoke sequence.
 REFERENCE_TIME tStart,tStop;
 REFERENCE_TIME fadeT1,fadeT2,fadeT3,fadeT4;
 REFERENCE_TIME karaokeDuration,karaokeStart;
 enum
  {
   KARAOKE_NONE,
   KARAOKE_k,
   KARAOKE_kf,
   KARAOKE_ko
  } karaokeMode;
};

#endif
