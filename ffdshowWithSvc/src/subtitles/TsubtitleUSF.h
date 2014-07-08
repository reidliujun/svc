#ifndef _TSUBTITLEUSF_H_
#define _TSUBTITLEUSF_H_

#include "Tsubtitle.h"

struct TsubreaderUSF2;
struct TsubtitleUSF2 : public Tsubtitle
{
private:
 TsubreaderUSF2 *subs;
 int idx;
 mutable TrenderedVobsubWord wrd;
 mutable bool ok,first;
public:
 TsubtitleUSF2(TsubreaderUSF2 *Isubs,int Iidx,REFERENCE_TIME start,REFERENCE_TIME stop);
 virtual void print(REFERENCE_TIME time,bool wasseek,Tfont &f,bool forceChange,TrenderedSubtitleLines::TprintPrefs &prefs) const;
 virtual Tsubtitle* create(void) {return new TsubtitleUSF2(subs,idx,start,stop);}
 virtual Tsubtitle* copy(void) {return new TsubtitleUSF2(subs,idx,start,stop);}
};

#endif
