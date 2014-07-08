#ifndef _TSUBTITLESTEXTPIN_H_
#define _TSUBTITLESTEXTPIN_H_

#include "Tsubtitles.h"
#include "TsubtitleText.h"

struct TsubtitlesSettings;
struct TsubtitleVobsub;
class Tspudec;
class TsubtitlesTextpin :public Tsubtitles
{
protected:
 int type;
 TsubtitlesTextpin(int Itype,IffdshowBase *Ideci);
public:
 static TsubtitlesTextpin* create(int type,const unsigned char *extradata,unsigned int extradatalen,IffdshowBase *Ideci);
 virtual void addSubtitle(REFERENCE_TIME start,REFERENCE_TIME stop,const unsigned char *data,unsigned int datalen,const TsubtitlesSettings *cfg,bool utf8)=0;
 virtual void resetSubtitles(void);
 virtual bool ctlSubtitles(unsigned int id,const void *data,unsigned int datalen) {return false;}
};

#endif
