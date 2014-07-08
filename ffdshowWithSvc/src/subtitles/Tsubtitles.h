#ifndef _TSUBTITLES_H_
#define _TSUBTITLES_H_

#include "interfaces.h"
#include "Tsubreader.h"

struct Tsubtitle;
struct TsubtitlesSettings;
struct Tconfig;
class Tsubtitles :public safe_bool<Tsubtitles>
{
private:
 const Tsubtitle *oldsub;
 unsigned int current_sub;
 REFERENCE_TIME nosub_range_start,nosub_range_end;
protected:
 IffdshowBase *deci;
 const Tconfig *ffcfg;
 Tsubreader *subs;
 int sub_format;
 virtual void checkChange(const TsubtitlesSettings *cfg,bool *forceChange) {}
public:
 void init(void);
 Tsubtitles(IffdshowBase *Ideci);
 virtual ~Tsubtitles();
 virtual void done(void);
 bool boolean_test() const {return subs!=NULL;}
 virtual const Tsubtitle* getSubtitle(const TsubtitlesSettings *cfg,REFERENCE_TIME time,bool *forceChange=NULL);
 void setModified(void) {subs->IsProcessOverlapDone=false;};
 bool IsProcessOverlapDone(void) {return subs->IsProcessOverlapDone;};
 void processOverlap(void);
};

#endif
