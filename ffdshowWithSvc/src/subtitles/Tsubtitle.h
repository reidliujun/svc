#ifndef _TSUBTITLE_H_
#define _TSUBTITLE_H_

#include "Tfont.h"

struct Tsubtitle
{
 virtual ~Tsubtitle() {}
 REFERENCE_TIME start,stop;
 Tsubtitle() {start = stop = REFTIME_INVALID;}
 virtual void print(REFERENCE_TIME time,bool wasseek,Tfont &f,bool forceChange,TrenderedSubtitleLines::TprintPrefs &prefs) const =0;
 virtual size_t numlines(void) const {return 1;}
 virtual size_t numchars(void) const {return 1;}
 virtual Tsubtitle* create(void)=0;
 virtual Tsubtitle* copy(void)=0;
 virtual bool copyLine(Tsubtitle *dst,size_t linenum) {return false;}
 virtual bool copyProps(Tsubtitle *dst) {return false;}
 virtual void append(const unsigned char *data,unsigned int datalen) {}
 virtual void addEmpty(void) {}
 virtual bool isText(void) const {return false;}
};

#endif
