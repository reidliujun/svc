#ifndef _TSUBTITLESTEXTPINVOBSUB_H_
#define _TSUBTITLESTEXTPINVOBSUB_H_

#include "TsubtitlesTextpin.h"
#include "TsubtitleDVD.h"

struct TsubtitlesSettings;
struct TsubtitleVobsub;
class Tspudec;
class TsubtitlesTextpinVobsub :public TsubtitlesTextpin, public TsubtitleDVDparent
{
private:
 Tspudec *spu;
public:
 TsubtitlesTextpinVobsub(int Itype,IffdshowBase *Ideci,const unsigned char *vobsubdata,unsigned int vobsubdatalen);
 virtual void addSubtitle(REFERENCE_TIME start,REFERENCE_TIME stop,const unsigned char *data,unsigned int datalen,const TsubtitlesSettings *cfg,bool utf8);
 virtual void resetSubtitles(void);
};

#endif
