#ifndef _TFILTERS_H_
#define _TFILTERS_H_

#include "Tfilter.h"
#include "interfaces.h"

struct Tconfig;
class Tfilters :protected std::map<const char*,Tfilter*>
{
protected:
 comptrQ<IffdshowDec> deciD;
 TfilterQueue queue;
 volatile LONG queueChanged;
 virtual void makeQueue(const Tpreset *cfg,TfilterQueue &queue);
public:
 Tfilters(IffdshowBase *Ideci);
 virtual ~Tfilters();
 comptr<IffdshowBase> deci;
 bool isdvdproc;
 template<class Tkey> Tkey* getFilter(void)
  {
   const char* key=Tkey::id();
   iterator f=find(key);
   if (f==end())
    {
     Tkey *filter=new Tkey(deci,this);
     insert(std::make_pair(key,filter));
     return filter;
    }
   else
    return (Tkey*)f->second;
  }
 void onSeek(void),onFlush(void),onStop(void),onDisconnect(PIN_DIRECTION dir);
 const Tconfig *config;
 void onQueueChange(int id,int val);
 HRESULT queryFilterInterface(const IID &iid,void **ptr) const;
};

#endif
