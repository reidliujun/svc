#ifndef _TSUBREADERVOBSUB_H_
#define _TSUBREADERVOBSUB_H_

#include "Tsubreader.h"
#include "interfaces.h"

class Tvobsub;
class Tspudec;
struct TsubreaderVobsub :Tsubreader
{
private:
 Tvobsub *vobsub;
 Tspudec *spu;
 comptr<IffdshowBase> deci;comptrQ<IffdshowDecVideo> deciV;
public:
 TsubreaderVobsub(Tstream &f,const char_t *flnm,IffdshowBase *deci,Tspudec* *spuPtr=NULL);
 virtual ~TsubreaderVobsub();
 virtual void setLang(int langid);
 virtual int findlang(int langname);
};

#endif
