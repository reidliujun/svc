#ifndef _TIMGFILTERSUBTITLES_H_
#define _TIMGFILTERSUBTITLES_H_

#include "TimgFilter.h"
#include "Tfont.h"
#include "TsubtitlesFile.h"
#include "TsubtitlesSettings.h"
#include "TexpandSettings.h"

class TsubtitlesTextpin;
class TimgFilterSubtitleExpand;
struct TfontSettingsSub;
template<class tchar> struct TsubtitleTextBase;
DECLARE_FILTER(TimgFilterSubtitles,public,TimgFilter)

public:
 typedef enum{
  ADHOC_NORMAL,
  ADHOC_ADHOC_DRAW_DVD_SUB_ONLY,
  ADHOC_SECOND_DONT_DRAW_DVD_SUB
 } AdhocMode;

private:
 Trect oldRectClip;
 int isdvdproc;
 bool wasDiscontinuity;
 bool expandSizeChanged,fontSizeChanged;
 TsubtitlesFile subs;
 typedef std::hash_map<int,TsubtitlesTextpin*> Tembedded;
 Tembedded embedded;
 Tfont font,fontCC;
 TfontSettingsSub *oldFontCfg,*oldFontCCcfg;
 int oldstereo,oldsplitborder;
 double AVIfps;
 TimgFilterSubtitleExpand *expand;TexpandSettings expandSettings;Trect oldExpandRect;
 int oldExpandCode;
 unsigned int oldSizeDx,oldSizeDy;
 CCritSec csEmbedded,csCC;
 TfilterQueue::iterator prevIt;TffPict prevPict;const TfilterSettingsVideo *prevCfg;bool again;Tbuffer prevbuf;
 AdhocMode prevAdhocMode;
 int subFlnmChanged;
 const char_t* findAutoSubFlnm(const TsubtitlesSettings *cfg);

 bool sequenceEnded;

 struct TsubPrintPrefs : TrenderedSubtitleLines::TprintPrefs
  {
   TsubPrintPrefs(unsigned char *Idst[4],stride_t Istride[4],unsigned int Idx[4],unsigned int Idy[4],IffdshowBase *Ideci,const TsubtitlesSettings *cfg,const TffPict &pict,int Iclipdy,const Tconfig *Iconfig,bool Idvd);
  };

 TsubtitleTextBase<wchar_t> *cc;
 bool wasCCchange;
 bool everRGB;
 AdhocMode adhocMode; // 0: normal, 1: adhoc! process only DVD sub/menu, 2: after adhoc, second call. process none DVD sub (cc decoder, etc).
protected:
 virtual bool is(const TffPictBase &pict,const TfilterSettingsVideo *cfg);
 virtual int getSupportedInputColorspaces(const TfilterSettingsVideo *cfg) const {return FF_CSP_420P;}
 virtual void onSizeChange(void);
public:
 TimgFilterSubtitles(IffdshowBase *Ideci,Tfilters *Iparent);
 virtual ~TimgFilterSubtitles();
 virtual bool getOutputFmt(TffPictBase &pict,const TfilterSettingsVideo *cfg0);
 virtual HRESULT process(TfilterQueue::iterator it,TffPict &pict,const TfilterSettingsVideo *cfg0);
 virtual void onSeek(void);
 void onSubFlnmChange(int id,int),onSubFlnmChangeStr(int id,const char_t*);

 bool initSubtitles(int id,int type,const unsigned char *extradata,unsigned int extradatalen);
 void addSubtitle(int id,REFERENCE_TIME start,REFERENCE_TIME stop,const unsigned char *data,unsigned int datalen,const TsubtitlesSettings *cfg,bool utf8);
 void resetSubtitles(int id);
 bool ctlSubtitles(int id,int type,unsigned int ctl_id,const void *ctl_data,unsigned int ctl_datalen);

 const char_t *getCurrentFlnm(void) const;

 void addClosedCaption(const wchar_t *line),hideClosedCaptions(void);
 virtual int getImgFilterID(void) {return IMGFILTER_SUBTITLES;}
 bool enterAdhocMode(void);
};


#endif
