#ifndef _TIMGFILTERS_H_
#define _TIMGFILTERS_H_

#include "Tfilters.h"
#include "TexpandSettings.h"

struct TffPict;
DECLARE_INTERFACE(IprocVideoSink)
{
 STDMETHOD (deliverProcessedSample)(TffPict &pict) PURE;
};

struct TglobalSettingsDecVideo;
struct TpresetVideo;
class TimgFilterSubtitles;
class TimgFilterOutput;
struct TsubtitlesSettings;
struct TffPictBase;
struct IimgFilterGrab;
struct ToutputVideoSettings;
struct IOSDprovider;
class TimgFilters :public Tfilters
{
private:
 IprocVideoSink *sink;
 IimgFilterGrab *grab;
 TimgFilterOutput *output;
 bool firstprocess;
 bool stopAtSubtitles;
protected:
 TimgFilters(IffdshowBase *Ideci,IprocVideoSink *Isink);
 comptrQ<IffdshowDecVideo> deciV;
 virtual void onFirstProcess(const TpresetVideo *cfg) {}
 TimgFilterSubtitles *subtitles;
public:
 virtual ~TimgFilters();

 virtual void getOutputFmt(TffPictBase &pict,const TpresetVideo *cfg);
 virtual HRESULT process(TffPict &pict,const TpresetVideo *cfg);
 HRESULT deliverSample(TfilterQueue::iterator it,TffPict &pict);
 HRESULT convertOutputSample(const TffPict &pict,int dstcsp,unsigned char *dst[4],int dstStride[4],LONG &dstSize,const ToutputVideoSettings *cfg);
 bool isAnyActiveDownstreamFilter(TfilterQueue::iterator it);
 void adhocDVDsub(TfilterQueue::iterator it,TffPict &pict);
 virtual void onEndOfStream(void);
 bool pullImageFromSubtitlesFilter(TfilterQueue::iterator from);

 int dirtyBorder;
 REFERENCE_TIME subtitleResetTime;

 bool initSubtitles(int id,int type,const unsigned char *extradata,unsigned int extradatalen);
 void addSubtitle(int id,REFERENCE_TIME start,REFERENCE_TIME stop,const unsigned char *data,unsigned int datalen,const TsubtitlesSettings *cfg,bool utf8);
 void resetSubtitles(int id);
 bool ctlSubtitles(int id,int type,unsigned int ctl_id,const void *ctl_data,unsigned int ctl_datalen);
 const char_t *getCurrentSubFlnm(void) const;

 virtual HRESULT addClosedCaption(const wchar_t* line) {return E_NOTIMPL;}
 virtual HRESULT hideClosedCaptions(void) {return E_NOTIMPL;}
 virtual bool shortOSDmessage(const char_t *msg,unsigned int duration) {return false;}
 virtual HRESULT registerOSDprovider(IOSDprovider *provider,const char *name) {return E_NOTIMPL;}
 virtual HRESULT unregisterOSDprovider(IOSDprovider *provider) {return E_NOTIMPL;}
 void setStopAtSubtitles(bool stop) {stopAtSubtitles = stop;}
 bool getStopAtSubtitles(void) {return stopAtSubtitles;}

 void grabNow(void);
};

class TimgFilterOSD;
struct TfontSettingsOSD;
struct TresizeAspectSettings;
class TimgFiltersPlayer :public TimgFilters
{
private:
 const TglobalSettingsDecVideo *globalCfg;
 const TfontSettingsOSD &globalFontSettingsOSD;TimgFilterOSD *osd;
 const bool &allowOutChange;
 TexpandSettings expandSettings;Tfilter *expand;
 TresizeAspectSettings *resizeSettingsDV;Tfilter *resizeDV;
protected:
 virtual void onFirstProcess(const TpresetVideo *cfg);
 virtual void makeQueue(const Tpreset *cfg,TfilterQueue &queue);
public:
 TimgFiltersPlayer(IffdshowBase *Ideci,IprocVideoSink *Isink,const TfontSettingsOSD &IglobalFontSettingsOSD,bool &IallowOutChange);
 virtual ~TimgFiltersPlayer();

 virtual void getOutputFmt(TffPictBase &pict,const TpresetVideo *cfg);
 virtual HRESULT process(TffPict &pict,const TpresetVideo *cfg);

 TfontSettingsOSD *fontSettingsOSD;
 virtual bool shortOSDmessage(const char_t *msg,unsigned int duration);
 virtual HRESULT addClosedCaption(const wchar_t *line),hideClosedCaptions(void);
 virtual HRESULT registerOSDprovider(IOSDprovider *provider,const char *name);
 virtual HRESULT unregisterOSDprovider(IOSDprovider *provider);
};

#endif
