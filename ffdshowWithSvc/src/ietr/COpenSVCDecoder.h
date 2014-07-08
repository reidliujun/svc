#ifndef _OPENSVCDECODER_H_
#define _OPENSVCDECODER_H_

#include "TconfPageDec.h"

class TinfoBase;
class TInfoOpenSVCDecoder :public TconfPageDec
{
private:
 HWND hlv;
 TinfoBase *info;
 char_t pszTextBuf[1024];
 struct Titem
  {
   Titem(void):val(NULL),wasChange(1) {}
   int id,index;
   const char_t *name,*translatedName,*val;
   int wasChange;
  };
 struct TsortItem
  {
  private:
   const int *infos;
  public:
   TsortItem(const int *Iinfos):infos(Iinfos) {}
   bool operator()(const Titem &i1,const Titem &i2) {return i1.index<i2.index;}
  };
 typedef std::vector<Titem> Titems;
 Titems infoitems;
 void onAllowDPRINTF(void);
protected:
 virtual const int* getInfos(void)=0;
 virtual INT_PTR msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
 virtual void onFrame(void);
public:
 TInfoOpenSVCDecoder(TffdshowPageDec *Iparent,TinfoBase *Iinfo);
 virtual ~TInfoOpenSVCDecoder();
 virtual void cfg2dlg(void);
 virtual void init(void);
 virtual void translate(void);
};

class TInfoOpenSVCDecoderVideo :public TInfoOpenSVCDecoder
{
protected:
 virtual const int* getInfos(void)
  {
   static const int infos[]=
    {
    #ifdef OSDTIMETABALE
     IDFF_OSDtype_timetable,
    #endif
     IDD_IETR,
	 IDFF_OpenSVCDecoderInfo, 0};
   return infos;
  }
public:
 TInfoOpenSVCDecoderVideo(TffdshowPageDec *Iparent);
};

class TInfoOpenSVCDecoderAudio :public TInfoOpenSVCDecoder
{
protected:
 virtual const int* getInfos(void)
  {
   static const int infos[]={IDFF_OSDtype_sourceFlnm,IDFF_OSDtype_inCodecString,IDFF_OSDtype_outCodecString,IDFF_OSDtype_outSpeakersConfig,IDFF_OSDtype_movieSource,IDFF_OSDtype_bps,IDFF_OSDtype_audioJitter,0};
   return infos;
  }
public:
 TInfoOpenSVCDecoderAudio(TffdshowPageDec *Iparent);
};

#endif

