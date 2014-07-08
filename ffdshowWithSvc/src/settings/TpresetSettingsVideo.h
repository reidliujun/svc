#ifndef _TPRESETSETTINGSVIDEO_H_
#define _TPRESETSETTINGSVIDEO_H_

#include "TpresetSettings.h"

struct TvideoAutoPresetProps :TautoPresetProps
{
private:
 comptrQ<IffdshowDecVideo> deciV;
 bool wasResolution;unsigned int dx,dy;
 char_t fourcc[5];
 char_t previousfourcc[5];
 double SAR,DAR;
 double fps;
 static const char_t aspectSAR,aspectDAR;
public:
 TvideoAutoPresetProps(IffdshowBase *Ideci);
 virtual void getSourceResolution(unsigned int *dx,unsigned int *dy);
 const char_t* getFOURCC(void);
 const char_t* getPreviousFOURCC(void);
 static const char_t* getFOURCCitem(IffdshowDec *deciD,unsigned int index);
 const char_t *getSAR(void),*getDAR(void);
 const char_t *getFps(void);
 bool aspectMatch(const char_t *mask,const char_t *flnm);
 bool fpsMatch(const char_t *mask,const char_t *flnm);
};

struct TpostprocSettings;
struct TsubtitlesSettings;
struct TlevelsSettings;
struct TvisSettings;
struct TgrabSettings;
struct ToutputVideoSettings;
struct TresizeAspectSettings;
struct TpresetVideo :public Tpreset
{
private:
 int needOutcspsFix,needGlobalFix;
protected:
 virtual void reg_op(TregOp &t);
 virtual int getDefault(int id);
public:
 TpresetVideo(const char_t *Ireg_child, const char_t *IpresetName, int filtermode);
 virtual ~TpresetVideo() {}
 TpresetVideo& operator=(const TpresetVideo &src)
  {
   Tpreset::operator =(src);
   needOutcspsFix=src.needOutcspsFix;
   needGlobalFix=src.needGlobalFix;

   autoloadSize=src.autoloadSize;
   autoloadSizeXmin=src.autoloadSizeXmin;
   autoloadSizeXmax=src.autoloadSizeXmax;
   autoloadSizeCond=src.autoloadSizeCond;
   autoloadSizeYmin=src.autoloadSizeYmin;
   autoloadSizeYmax=src.autoloadSizeYmax;

   videoDelay=src.videoDelay;isVideoDelayEnd=src.isVideoDelayEnd;videoDelayEnd=src.videoDelayEnd;
   idct=src.idct;
   workaroundBugs=src.workaroundBugs;errorRecognition=src.errorRecognition;errorConcealment=src.errorConcealment;
   lavcDecThreads=src.lavcDecThreads;
   grayscale=src.grayscale;
   multiThread=src.multiThread;
   dontQueueInWMP=src.dontQueueInWMP;
   useQueueOnlyIn=src.useQueueOnlyIn;
   queueCount=src.queueCount;
   queueVMR9YV12=src.queueVMR9YV12;
   dropOnDelay=src.dropOnDelay;
   dropDelayTime=src.dropDelayTime;
   h264skipOnDelay=src.h264skipOnDelay;
   h264skipDelayTime=src.h264skipDelayTime;
   bordersBrightness=src.bordersBrightness;
   ff_strncpy(useQueueOnlyInList, src.useQueueOnlyInList, countof(useQueueOnlyInList));

   isDyInterlaced=src.isDyInterlaced;dyInterlaced=src.dyInterlaced;
   return *this;
  }

 virtual Tpreset* copy(void) {return new_copy(this);}
 virtual void loadReg(void);

 int autoloadSize,autoloadSizeXmin,autoloadSizeXmax,autoloadSizeCond,autoloadSizeYmin,autoloadSizeYmax;
 virtual bool autoloadSizeMatch(int AVIdx,int AVIdy) const;
 virtual bool is_autoloadSize(void) const {return !!autoloadSize;}

 int videoDelay,isVideoDelayEnd,videoDelayEnd;
 int idct;
 int workaroundBugs,errorRecognition,errorConcealment;
 int lavcDecThreads;
 int grayscale;
 int multiThread;
 int dontQueueInWMP,useQueueOnlyIn,queueCount,queueVMR9YV12;
 int dropOnDelay, dropDelayTime, dropDelayTimeReal;
 int h264skipOnDelay, h264skipDelayTime;
 char_t useQueueOnlyInList[256];

 int isDyInterlaced,dyInterlaced;
 int bordersBrightness;

 TpostprocSettings *postproc;
 TsubtitlesSettings *subtitles;
 TlevelsSettings *levels;
 TresizeAspectSettings *resize;
 TvisSettings *vis;
 TgrabSettings *grab;
 ToutputVideoSettings *output;
};

class TpresetVideoPlayer : public TpresetVideo
{
public:
 TpresetVideoPlayer(const char_t *Ireg_child,const char_t *IpresetName, int filtermode);
 virtual Tpreset* copy(void) {return new_copy(this);}
};

#endif
