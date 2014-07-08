#ifndef _TFILTER_H_
#define _TFILTER_H_

#include "interfaces.h"

class Tfilter
{
protected:
 comptr<IffdshowBase> deci;comptrQ<IffdshowDec> deciD;
public:
 Tfilter(IffdshowBase *Ideci);
 virtual ~Tfilter();
 virtual HRESULT queryInterface(const IID &iid,void **ptr) const {return E_NOINTERFACE;}
 virtual void done(void) {}
 virtual void onSeek(void) {}
 virtual void onStop(void) {}
 virtual void onFlush(void) {}
 virtual void onDisconnect(PIN_DIRECTION dir) {}
};

#define _DECLARE_FILTER(filter,parent) \
{\
public:\
 static const char* id(void) {static const char *name=#filter;return name;}\
protected:\
 typedef parent super;\
private:

#define DECLARE_FILTER(filter,access,parent) \
class filter:access parent \
_DECLARE_FILTER(filter,parent)

struct TfilterSettings;
struct Tpreset;
struct TfilterQueueItem
{
private:
 TbyteBuffer owncfg;size_t index;
 const TfilterSettings *cfg;
public:
 TfilterQueueItem(Tfilter *Ifilter,size_t cfgsize,size_t Iindex):filter(Ifilter),owncfg(cfgsize),index(Iindex) {}
 TfilterQueueItem(Tfilter *Ifilter,const TfilterSettings *Icfg):filter(Ifilter),cfg(Icfg) {}
 Tfilter *filter;
 void copyCfg(const Tpreset *cfgsrc);
 const TfilterSettings* getCfg(void) const {return owncfg.size()?(const TfilterSettings*)&*owncfg.begin():cfg;}
};

struct TfilterQueue :public std::list<TfilterQueueItem>
{
private:
 TfilterQueue& operator=(const TfilterQueue&);
public:
 TfilterQueue(bool Itemporary=false):temporary(Itemporary) {}
 bool temporary;
 void add(Tfilter *f,size_t cfgsize,size_t index) {push_back(TfilterQueueItem(f,cfgsize,index));}
 void add(Tfilter *f,const TfilterSettings *cfg) {push_back(TfilterQueueItem(f,cfg));}
 void copyCfg(const Tpreset *cfg);
};

#endif
