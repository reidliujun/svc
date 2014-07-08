#ifndef _TSUBTITLESTEXTPINTEXT_H_
#define _TSUBTITLESTEXTPINTEXT_H_

#include "TsubtitlesTextpin.h"

struct TsubtitlesSettings;
struct TsubtitleVobsub;
class TsubtitleParserBase;
class Tspudec;
struct Tstream;
class TsubtitlesTextpinText :public TsubtitlesTextpin
{
private:
 enum
  {
   __GAB1_LANGUAGE_UNICODE__=2,
   __GAB1_RAWTEXTSUBTITLE__ =4
  };
 TsubtitleParserBase *parser;
 Tbuffer initdata;
public:
 TsubtitlesTextpinText(int Itype,IffdshowBase *Ideci,const unsigned char *extradata,unsigned int extradatalen);
 virtual ~TsubtitlesTextpinText();
 virtual void addSubtitle(REFERENCE_TIME start,REFERENCE_TIME stop,const unsigned char *data,unsigned int datalen,const TsubtitlesSettings *cfg,bool utf8);
 virtual void resetSubtitles(void);
};

#endif
