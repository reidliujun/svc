#ifndef _TSUBTITLESTEXTPINDVD_H_
#define _TSUBTITLESTEXTPINDVD_H_

#include "TsubtitlesTextpin.h"
#include "TsubtitleDVD.h"

class TsubtitlesTextpinDVD :public TsubtitlesTextpin, public TsubtitleDVDparent
{
private:
 struct Tsubtitles :public Tsubtitle,public std::vector<Tsubtitle*>
  {
   virtual void print(REFERENCE_TIME time,bool wasseek,Tfont &f,bool forceChange,TrenderedSubtitleLines::TprintPrefs &prefs) const;
   virtual Tsubtitle* copy(void);
   virtual Tsubtitle* create(void) {return new Tsubtitles;}
  } subtitles;
public:
 TsubtitlesTextpinDVD(int Itype,IffdshowBase *Ideci);
 virtual void addSubtitle(REFERENCE_TIME start,REFERENCE_TIME stop,const unsigned char *data,unsigned int datalen,const TsubtitlesSettings *cfg,bool utf8);
 virtual void resetSubtitles(void);
 virtual bool ctlSubtitles(unsigned int id,const void *data,unsigned int datalen);
 virtual const Tsubtitle* getSubtitle(const TsubtitlesSettings *cfg,REFERENCE_TIME time,bool *forceChange=NULL);
};

#endif
