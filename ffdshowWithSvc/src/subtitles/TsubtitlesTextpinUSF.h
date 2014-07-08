#ifndef _TSUBTITLESTEXTPINUSF_H_
#define _TSUBTITLESTEXTPINUSF_H_

#include "TsubtitlesTextpin.h"

class TsubtitlesTextpinUSF :public TsubtitlesTextpin
{
public:
 TsubtitlesTextpinUSF(int Itype,IffdshowBase *Ideci,const unsigned char *extradata,unsigned int extradatalen);
 virtual ~TsubtitlesTextpinUSF();
 virtual void addSubtitle(REFERENCE_TIME start,REFERENCE_TIME stop,const unsigned char *data,unsigned int datalen,const TsubtitlesSettings *cfg,bool utf8);
 virtual void resetSubtitles(void);
};

#endif
