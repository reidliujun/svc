#ifndef _TMIXERSETTINGS_H_
#define _TMIXERSETTINGS_H_

#include "TfilterSettings.h"

struct TsampleFormat;
struct TmixerSettings :TfilterSettingsAudio
{
private:
 static const TfilterIDFF idffs;
protected:
 virtual const int *getResets(unsigned int pageId);
public:
 TmixerSettings(TintStrColl *Icoll=NULL,TfilterIDFFs *filters=NULL);
 struct TchConfig
  {
   int id;
   const char_t *name;int nameIndex;
   unsigned int nchannels,channelmask;
   int dolby;
  };
 static const TchConfig chConfigs[];

 int out;
 void setFormatOut(TsampleFormat &fmt) const;
 void setFormatOut(TsampleFormat &outfmt,const TsampleFormat &infmt) const;
 int normalizeMatrix,expandStereo,voiceControl;
 int customMatrix;
 int matrix00,matrix01,matrix02,matrix03,matrix04,matrix05,matrix08,matrix06,matrix07,
     matrix10,matrix11,matrix12,matrix13,matrix14,matrix15,matrix18,matrix16,matrix17,
     matrix20,matrix21,matrix22,matrix23,matrix24,matrix25,matrix28,matrix26,matrix27,
     matrix30,matrix31,matrix32,matrix33,matrix34,matrix35,matrix38,matrix36,matrix37,
     matrix40,matrix41,matrix42,matrix43,matrix44,matrix45,matrix48,matrix46,matrix47,
     matrix50,matrix51,matrix52,matrix53,matrix54,matrix55,matrix58,matrix56,matrix57,
     matrix80,matrix81,matrix82,matrix83,matrix84,matrix85,matrix88,matrix86,matrix87,
     matrix60,matrix61,matrix62,matrix63,matrix64,matrix65,matrix68,matrix66,matrix67,
     matrix70,matrix71,matrix72,matrix73,matrix74,matrix75,matrix78,matrix76,matrix77;
 int clev,slev,lfelev;
 int autogain;
 int headphone_dim;
 virtual void createFilters(size_t filtersorder,Tfilters *filters,TfilterQueue &queue) const;
 virtual void createPages(TffdshowPageDec *parent) const;
 virtual bool getTip(unsigned int pageId,char_t *buf,size_t buflen);
};

#endif
