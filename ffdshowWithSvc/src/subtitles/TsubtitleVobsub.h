#ifndef _TSUBTITLEVOBSUB_H_
#define _TSUBTITLEVOBSUB_H_

#include "Tsubtitle.h"
#include "Tfont.h"

class Tspudec;
class Tvobsub;
struct TsubtitleDVD;
struct TsubtitleVobsub :public Tsubtitle
{
private:
 mutable int oldtimer;
 Tspudec *spu;
 Tvobsub *vobsub;
 mutable TsubtitleDVD *dvdsub;
 static void drawalpha(int x0,int y0, unsigned int w,unsigned int h, const unsigned char* srcY, const unsigned char *srcaY, int strideY,const unsigned char* srcUV, const unsigned char *srcaUV, int strideUV,const TrenderedSubtitleLines::TprintPrefs &prefs);
public:
 TsubtitleVobsub(Tspudec *Ispu,Tvobsub *Ivobsub);
 virtual ~TsubtitleVobsub();
 virtual void print(REFERENCE_TIME time,bool wasseek,Tfont &f,bool forceChange,TrenderedSubtitleLines::TprintPrefs &prefs) const;
 virtual Tsubtitle* copy(void);
 virtual Tsubtitle* create(void) {return new TsubtitleVobsub(NULL,NULL);}
};

#endif
