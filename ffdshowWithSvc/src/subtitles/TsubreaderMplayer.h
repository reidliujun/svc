#ifndef _TSUBREADERMPLAYER_H_
#define _TSUBREADERMPLAYER_H_

#include "Tsubreader.h"
#include "TsubtitleText.h"
#include "rational.h"

class TsubtitleParserBase
{
private:
 template<class tchar> static TsubtitleParserBase* getParser0(int format,double fps,const TsubtitlesSettings *cfg,const Tconfig *ffcfg,Tsubreader *subreader,bool isEmbedded);
protected:
 static const int LINE_LEN=MAX_SUBTITLE_LENGTH; // Maximal length of line of a subtitle
 int format;
 double fps;
 REFERENCE_TIME frameToTime(int frame)
  {
   return hmsToTime(0,0,int(frame/fps),int(100*frame/fps)%100);
  }
 static REFERENCE_TIME hmsToTime(int h,int m,int s,int hunsec=0)
  {
   return REF_SECOND_MULT*h*60*60+REF_SECOND_MULT*m*60+REF_SECOND_MULT*s+REF_SECOND_MULT*hunsec/100;
  }
public:
 static TsubtitleParserBase* getParser(int format,double fps,const TsubtitlesSettings *cfg,const Tconfig *ffcfg,Tsubreader *Isubreader,bool utf8,bool isEmbedded);
 TsubtitleParserBase(int Iformat,double Ifps):format(Iformat),fps(Ifps) {}
 enum FLAGS {PARSETIME=1,SSA_NODIALOGUE=2};
 virtual Tsubtitle* parse(Tstream &fd, int flags=PARSETIME, REFERENCE_TIME start=REFTIME_INVALID, REFERENCE_TIME stop=REFTIME_INVALID)=0;
};

template<class tchar> class TsubtitleParser :public TsubtitleParserBase
{
private:
 Tsubreader *subreader;
 TsubtitlesSettings cfg;
 const Tconfig *ffcfg;
 TtextFix<tchar> textfix;
protected:
 TsubtitleFormat textformat;
 typedef typename tchar_traits<tchar>::ffstring ffstring;
 typedef typename tchar_traits<tchar>::strings strings;
 TsubtitleParser(int Iformat,double Ifps,const TsubtitlesSettings *Icfg,const Tconfig *Iffcfg,Tsubreader *Isubreader);
 static const tchar* sub_readtext(const tchar *source, TsubtitleTextBase<tchar> &sub);
 Tsubtitle* store(TsubtitleTextBase<tchar> &sub);
 static int eol(tchar p);
 static void trail_space(tchar *s);
public:
 virtual ~TsubtitleParser() {}
};

template<class tchar> class TsubtitleParserMicrodvd :public TsubtitleParser<tchar>
{
public:
 TsubtitleParserMicrodvd(int Iformat,double Ifps,const TsubtitlesSettings *Icfg,const Tconfig *Iffcfg,Tsubreader *Isubreader):TsubtitleParser<tchar>(Iformat,Ifps,Icfg,Iffcfg,Isubreader) {}
 virtual Tsubtitle* parse(Tstream &fd,int flags=TsubtitleParser<tchar>::PARSETIME, REFERENCE_TIME start=REFTIME_INVALID, REFERENCE_TIME stop=REFTIME_INVALID);
};
template<class tchar> class TsubtitleParserSubrip :public TsubtitleParser<tchar>
{
public:
 TsubtitleParserSubrip(int Iformat,double Ifps,const TsubtitlesSettings *Icfg,const Tconfig *Iffcfg,Tsubreader *Isubreader):TsubtitleParser<tchar>(Iformat,Ifps,Icfg,Iffcfg,Isubreader) {}
 virtual Tsubtitle* parse(Tstream &fd,int flags=TsubtitleParser<tchar>::PARSETIME, REFERENCE_TIME start=REFTIME_INVALID, REFERENCE_TIME stop=REFTIME_INVALID);
};
template<class tchar> class TsubtitleParserSubviewer :public TsubtitleParser<tchar>
{
public:
 TsubtitleParserSubviewer(int Iformat,double Ifps,const TsubtitlesSettings *Icfg,const Tconfig *Iffcfg,Tsubreader *Isubreader):TsubtitleParser<tchar>(Iformat,Ifps,Icfg,Iffcfg,Isubreader) {}
 virtual Tsubtitle* parse(Tstream &fd,int flags=TsubtitleParser<tchar>::PARSETIME, REFERENCE_TIME start=REFTIME_INVALID, REFERENCE_TIME stop=REFTIME_INVALID);
};
template<class tchar> class TsubtitleParserSami :public TsubtitleParser<tchar>
{
private:
 int sub_slacktime;
 bool sub_no_text_pp;
 tchar line[TsubtitleParser<tchar>::LINE_LEN+1],*s;
 const tchar *slacktime_s;
public:
 TsubtitleParserSami(int Iformat,double Ifps,const TsubtitlesSettings *Icfg,const Tconfig *Iffcfg,Tsubreader *Isubreader):TsubtitleParser<tchar>(Iformat,Ifps,Icfg,Iffcfg,Isubreader),s(NULL),sub_slacktime(2000),sub_no_text_pp(false) {} // 20 seconds
 virtual Tsubtitle* parse(Tstream &fd,int flags=TsubtitleParser<tchar>::PARSETIME, REFERENCE_TIME start=REFTIME_INVALID, REFERENCE_TIME stop=REFTIME_INVALID);
};
template<class tchar> class TsubtitleParserVplayer :public TsubtitleParser<tchar>
{
public:
 TsubtitleParserVplayer(int Iformat,double Ifps,const TsubtitlesSettings *Icfg,const Tconfig *Iffcfg,Tsubreader *Isubreader):TsubtitleParser<tchar>(Iformat,Ifps,Icfg,Iffcfg,Isubreader) {}
 virtual Tsubtitle* parse(Tstream &fd,int flags=TsubtitleParser<tchar>::PARSETIME, REFERENCE_TIME start=REFTIME_INVALID, REFERENCE_TIME stop=REFTIME_INVALID);
};
template<class tchar> class TsubtitleParserRt :public TsubtitleParser<tchar>
{
public:
 TsubtitleParserRt(int Iformat,double Ifps,const TsubtitlesSettings *Icfg,const Tconfig *Iffcfg,Tsubreader *Isubreader):TsubtitleParser<tchar>(Iformat,Ifps,Icfg,Iffcfg,Isubreader) {}
 virtual Tsubtitle* parse(Tstream &fd,int flags=TsubtitleParser<tchar>::PARSETIME, REFERENCE_TIME start=REFTIME_INVALID, REFERENCE_TIME stop=REFTIME_INVALID);
};
template<class tchar> class TsubtitleParserSSA :public TsubtitleParser<tchar>
{
private:
 uint8_t inV4styles,inEvents,inInfo;
 int playResX,playResY,wrapStyle;
 Rational timer;
 bool isEmbedded;
 typedef typename tchar_traits<tchar>::ffstring ffstring;
 typedef typename tchar_traits<tchar>::strings strings;
 static void strToInt(const ffstring &str,int *i);
 static void strToIntMargin(const ffstring &str,int *i);
 static void strToDouble(const ffstring &str,double *d);
 struct Tstyle
  {
  private:
   int version;
  public:
   Tstyle(int playResX,int playResY,int version,int wrapStyle):props(playResX,playResY,wrapStyle) {this->version=version;props.version=version;}
   ffstring name,fontname,fontsize,primaryColour,bold,italic,underline,strikeout,encoding,spacing,fontScaleX,fontScaleY;
   ffstring secondaryColour,tertiaryColour,outlineColour,backgroundColour,alignment;
   ffstring angleZ,borderStyle,outlineWidth,shadowDepth,marginLeft,marginRight,marginV,marginTop,marginBottom,alpha,relativeTo;
   TSubtitleProps props;
   void toProps(void);
   bool toCOLORREF(const ffstring &colourStr,COLORREF &colour,int &alpha);
  };
 struct Tstyles : std::map<ffstring,Tstyle,ffstring_iless>
  {
   const TSubtitleProps* getProps(const ffstring &style);
   void add(Tstyle &style);
  };
 Tstyles styles;
 typedef std::vector<ffstring Tstyle::*> TstyleFormat;
 TstyleFormat styleFormat;

 struct Tevent
  {
   ffstring dummy;
   ffstring start,end,style,text;
   ffstring marked,readorder,layer,actor,marginT,marginB,name,marginL,marginR,marginV,effect;
  };
 typedef std::vector<ffstring Tevent::*> TeventFormat;
 TeventFormat eventFormat;
 TSubtitleProps defprops;
public:
 TsubtitleParserSSA(int Iformat,double Ifps,const TsubtitlesSettings *Icfg,const Tconfig *Iffcfg,Tsubreader *Isubreader,bool isEmbedded0);
 virtual Tsubtitle* parse(Tstream &fd, int flags=TsubtitleParser<tchar>::PARSETIME, REFERENCE_TIME start=REFTIME_INVALID, REFERENCE_TIME stop=REFTIME_INVALID);
 enum
  {
   SSA=4,ASS=5,ASS2=6
  } version;
};
template<class tchar> class TsubtitleParserDunnowhat :public TsubtitleParser<tchar>
{
public:
 TsubtitleParserDunnowhat(int Iformat,double Ifps,const TsubtitlesSettings *Icfg,const Tconfig *Iffcfg,Tsubreader *Isubreader):TsubtitleParser<tchar>(Iformat,Ifps,Icfg,Iffcfg,Isubreader) {}
 virtual Tsubtitle* parse(Tstream &fd,int flags=TsubtitleParser<tchar>::PARSETIME, REFERENCE_TIME start=REFTIME_INVALID, REFERENCE_TIME stop=REFTIME_INVALID);
};
template<class tchar> class TsubtitleParserMPsub :public TsubtitleParser<tchar>
{
private:
 double mpsub_position;
 bool sub_uses_time;
public:
 TsubtitleParserMPsub(int Iformat,double Ifps,const TsubtitlesSettings *Icfg,const Tconfig *Iffcfg,Tsubreader *Isubreader):TsubtitleParser<tchar>(Iformat,Ifps,Icfg,Iffcfg,Isubreader),mpsub_position(0),sub_uses_time(Iformat&Tsubreader::SUB_USESTIME?true:false) {}
 virtual Tsubtitle* parse(Tstream &fd,int flags=TsubtitleParser<tchar>::PARSETIME, REFERENCE_TIME start=REFTIME_INVALID, REFERENCE_TIME stop=REFTIME_INVALID);
};
template<class tchar> class TsubtitleParserAqt :public TsubtitleParser<tchar>
{
private:
 Tsubtitle *previous;
public:
 TsubtitleParserAqt(int Iformat,double Ifps,const TsubtitlesSettings *Icfg,const Tconfig *Iffcfg,Tsubreader *Isubreader):TsubtitleParser<tchar>(Iformat,Ifps,Icfg,Iffcfg,Isubreader),previous(NULL) {}
 virtual Tsubtitle* parse(Tstream &fd,int flags=TsubtitleParser<tchar>::PARSETIME, REFERENCE_TIME start=REFTIME_INVALID, REFERENCE_TIME stop=REFTIME_INVALID);
};
template<class tchar> class TsubtitleParserSubviewer2 :public TsubtitleParser<tchar>
{
public:
 TsubtitleParserSubviewer2(int Iformat,double Ifps,const TsubtitlesSettings *Icfg,const Tconfig *Iffcfg,Tsubreader *Isubreader):TsubtitleParser<tchar>(Iformat,Ifps,Icfg,Iffcfg,Isubreader) {}
 virtual Tsubtitle* parse(Tstream &fd,int flags=TsubtitleParser<tchar>::PARSETIME, REFERENCE_TIME start=REFTIME_INVALID, REFERENCE_TIME stop=REFTIME_INVALID);
};
template<class tchar> class TsubtitleParserSubrip09 :public TsubtitleParser<tchar>
{
private:
 Tsubtitle *previous;
public:
 TsubtitleParserSubrip09(int Iformat,double Ifps,const TsubtitlesSettings *Icfg,const Tconfig *Iffcfg,Tsubreader *Isubreader):TsubtitleParser<tchar>(Iformat,Ifps,Icfg,Iffcfg,Isubreader),previous(NULL) {}
 virtual Tsubtitle* parse(Tstream &fd,int flags=TsubtitleParser<tchar>::PARSETIME, REFERENCE_TIME start=REFTIME_INVALID, REFERENCE_TIME stop=REFTIME_INVALID);
};
template<class tchar> class TsubtitleParserMPL2 :public TsubtitleParser<tchar>
{
public:
 TsubtitleParserMPL2(int Iformat,double Ifps,const TsubtitlesSettings *Icfg,const Tconfig *Iffcfg,Tsubreader *Isubreader):TsubtitleParser<tchar>(Iformat,Ifps,Icfg,Iffcfg,Isubreader) {}
 virtual Tsubtitle* parse(Tstream &fd,int flags=TsubtitleParser<tchar>::PARSETIME, REFERENCE_TIME start=REFTIME_INVALID, REFERENCE_TIME stop=REFTIME_INVALID);
};

struct TsubreaderMplayer :Tsubreader
{
public:
 TsubreaderMplayer(Tstream &f,int Isub_format,double Ifps,const TsubtitlesSettings *Icfg,const Tconfig *Iffcfg,bool isEmbedded);
};

#endif
