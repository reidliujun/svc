#ifndef _TSUBTITLESSETTINGS_H_
#define _TSUBTITLESSETTINGS_H_

#include "TfilterSettings.h"
#include "TfontSettings.h"

class Ttranslate;
class Twindow;
struct TsubtitlesSettings :TfilterSettingsVideo
{
private:
 static const TfilterIDFF idffs;
protected:
 virtual const int *getResets(unsigned int pageId);
public:
 TsubtitlesSettings(TintStrColl *Icoll=NULL,TfilterIDFFs *filters=NULL, int filtermode=0);
 virtual void copy(const TfilterSettings *src);

 char_t flnm[MAX_PATH],tempflnm[MAX_PATH];
 int autoFlnm;
 int posX,posY;
 int align;
 static const char_t *alignments[];
 static void getPosHoriz(int x, char_t *s, Twindow *w, int id, size_t len),getPosVert(int x, char_t *s, Twindow *w, int id, size_t len);
 int isExpand,expandCode;
 void getExpand(int *x,int *y) const;
 static void getExpand(int is,int code,int *x,int *y);
 int delay,speed,speed2;
 int stereoscopic,stereoscopicPar;
 int deflang,deflang2;
 struct Tlang
  {
   const char_t *desc;
   const char_t *lang,*isolang;
  };
 static const Tlang  langs[];
 static const char_t *getLangDescr(const char_t *lang),*getLangDescrIso(const char_t *isolang);
 int vobsub;
 int vobsubAA;static const char_t *vobsubAAs[];int vobsubAAswgauss;
 int vobsubChangePosition,vobsubScale;
 int linespacing;
 int timeoverlap;
 int isMinDuration,minDurationType,minDurationSubtitle,minDurationLine,minDurationChar;
 static const char_t *durations[];
 int fix,fixLang;char_t fixDict[60];
 int wordWrap;
 static const char_t *fixIls[];
 static const char_t *wordWraps[];
 int opacity;
 int splitBorder;
 int cc;
 int filtermode;
 int extendedTags;
 static const int delayDef=0,speedDef=1000;
 virtual void createFilters(size_t filtersorder,Tfilters *filters,TfilterQueue &queue) const;
 virtual void createPages(TffdshowPageDec *parent) const;
 virtual bool getTip(unsigned int pageId,char_t *buf,size_t buflen);
 virtual void reg_op(TregOp &t);
 TfontSettingsSub font; //HACK: must be last
};

#endif
