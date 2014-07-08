#ifndef _TFILTERSETTINGS_H_
#define _TFILTERSETTINGS_H_

#include "reg.h"
#include "resource.h"
#include "ffdshow_constants.h"
#include "Toptions.h"
#include "Tfilters.h"
#include "interfaces.h"

struct TfilterIDFF
{
 const char_t *name;
 int id;
 int is;
 int order;
 int show;
 int full,half;
 int dlgId;
};

struct TfilterIDFF;
struct TfilterSettings;
struct TfilterTriple
{
 TfilterTriple(const TfilterIDFF *Iidff,TfilterSettings *Icfg,int IorderDef):idff(Iidff),cfg(Icfg),orderDef(IorderDef) {}
 const TfilterIDFF *idff;
 TfilterSettings *cfg;
 int orderDef;
};

struct TfilterIDFFs : std::vector<TfilterTriple>
{
private:
 int orderDef;
public:
 TfilterIDFFs(int min_order):orderDef(min_order) {}
 void copy(const TfilterIDFFs *orig);
 void add(const TfilterIDFF *idff,TfilterSettings *filter,bool incOrder=true)
  {
   resize(size()+1,TfilterTriple(idff,filter,orderDef));
   if (incOrder) orderDef++;
  }
 int maxOrder(void) const {return orderDef-1;}
};


class TimgFilters;
struct TfilterQueue;
class TffdshowPageDec;
struct TfilterSettings :public Toptions
{
private:
 size_t sizeofthisAll,sizeofthis;
protected:
 TfilterSettings(size_t IsizeofthisAll,size_t Isizeofthis,TintStrColl *Icoll,TfilterIDFFs *filters,const TfilterIDFF *idff,bool incOrder=true):
  sizeofthisAll(IsizeofthisAll),
  sizeofthis(Isizeofthis),
  Toptions(Icoll),
  deepcopy(false)
  {
   if (filters)
    filters->add(idff,this,incOrder);
  }
 template<class Tf> Tf* queueFilter(size_t index,Tfilters *filters,TfilterQueue &queue) const
  {
   Tf *f=filters->getFilter<Tf>();
   queue.add(f,sizeofthisAll,index);
   return f;
  }
 template<class Tf> Tf* queueFilter(Tfilters *filters,TfilterQueue &queue) const
  {
   Tf *f=filters->getFilter<Tf>();
   queue.add(f,this);
   return f;
  }
 void idffOnChange(const TfilterIDFF &idffs,Tfilters *filters,bool temporary) const;
 virtual const int *getResets(unsigned int pageId) {return NULL;}
public:
 bool deepcopy;
 TfilterSettings& operator=(const TfilterSettings &src)
  {
   memcpy(((uint8_t*)this)+sizeof(Toptions),((uint8_t*)&src)+sizeof(Toptions),sizeofthisAll-sizeof(Toptions));
   return *this;
  }
 virtual void copy(const TfilterSettings *src) {}

 int is,order,show;
 virtual void createFilters(size_t filtersorder,Tfilters *filters,TfilterQueue &queue) const =0;
 virtual void createPages(TffdshowPageDec *parent) const =0;
 bool hasReset(unsigned int pageId=0);
 virtual bool reset(unsigned int pageId=0);
 virtual bool getTip(unsigned int pageId,char_t *buf,size_t buflen) {return false;}
 virtual ~TfilterSettings() {}
 inline bool equal(const TfilterSettings &second) const
  {
   uint8_t *p1=(uint8_t*)this+sizeofthis;
   uint8_t *p2=(uint8_t*)&second+sizeofthis;
   size_t size=sizeofthisAll-sizeofthis;
   return memcmp(p1,p2,size)==0;
  }
};

struct TfilterSettingsVideo :public TfilterSettings
{
 TfilterSettingsVideo(size_t IsizeofthisAll,TintStrColl *Icoll,TfilterIDFFs *filters=NULL,const TfilterIDFF *idff=NULL,bool incOrder=true):TfilterSettings(IsizeofthisAll,sizeof(TfilterSettingsVideo),Icoll,filters,idff,incOrder) {}
 int full,half;
};

struct TfilterSettingsAudio :public TfilterSettings
{
 TfilterSettingsAudio(size_t IsizeofthisAll,TintStrColl *Icoll,TfilterIDFFs *filters=NULL,const TfilterIDFF *idff=NULL,bool incOrder=true):TfilterSettings(IsizeofthisAll,sizeof(TfilterSettingsAudio),Icoll,filters,idff,incOrder) {}
};

#endif
