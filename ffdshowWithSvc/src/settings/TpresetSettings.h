#ifndef _TPRESETSETTINGS_H_
#define _TPRESETSETTINGS_H_

#include "Toptions.h"
#include "IffdshowBase.h"
#include "interfaces.h"

extern const char_t *FFPRESET_DEFAULT;

struct TautoPresetProps
{
private:
 ffstring sourceFullFlnm,sourceNameExt,sourceName;
 char_t volumeName[MAX_PATH],volumeSerial[10];
 BOOL wasVolume;
 void getSourceFlnm(void),getVolume(void);
 const char_t *decoder;
 IFilterGraph *graphPtr;strings filtersnames;
protected:
 comptr<IffdshowBase> deci;comptrQ<IffdshowDec> deciD;
public:
 TautoPresetProps(IffdshowBase *Ideci);
 ~TautoPresetProps();
 const char_t *presetName;
 template<int idff> const char_t* getParamStr(void)
  {
   return deci->getParamStr2(idff);
  }
 const char_t *getSourceFullFlnm(void),*getSourceName(void),*getSourceNameExt(void),*getExeflnm(void),*getVolumeName(void),*getVolumeSerial(void),*getPresetName(void),*getDecoder(void),*getDSfilterName(void);
 static const char_t *getExeItem(IffdshowDec*,unsigned int index),*getDecoderItem(IffdshowDec *deciD,unsigned int index);
 bool presetNameMatch(const char_t *mask,const char_t *flnm);
 bool wildcardmatch(const char_t *mask,const char_t *flnm);
 bool stricoll(const char_t *s1,const char_t *s2);
 bool dsfilterMatch(const char_t *f1,const char_t *f2);
 virtual void getSourceResolution(unsigned int *dx,unsigned int *dy){if (dx) *dx=0;if (dy) *dy=0;}
};

struct TregOp;
struct TfilterSettings;
struct TfilterIDFF;
struct TfilterIDFFs;
class Tfilters;
struct TfilterQueue;
class TffdshowPageDec;
struct Tpreset :public Toptions
{
private:
 int min_order;
 enum
  {
   ORDER=1,CFG=2,ORDERDEF=3
  };
 typedef std::tuple<size_t,TfilterSettings*,int> Torder; //order in filters,cfg,orderDef
 struct Torders :public std::vector<Torder>
  {
  private:
   static bool orderSort(const Torder &o1,const Torder &o2);
  public:
   void sort(void);
  };
 void getOrders(Torders &orders,bool all=false) const;
 void fixOrder(void);
public:
 static void normalizePresetName(char_t *dst, const char_t *src, size_t bufsize);
 static void normalizePresetName(ffstring &dst,const char_t *src);
 static bool isValidPresetName(const char_t *presetName);
 virtual bool autoloadSizeMatch(int AVIdx,int AVIdy) const {return false;};
 virtual bool is_autoloadSize(void) const {return false;}
protected:
 Tpreset(const char_t *Ireg_child, const char_t *IpresetName, int Imin_order, int Ifiltermode);
 const char_t *reg_child;
 friend class TffdshowDec;
 Tpreset& operator =(const Tpreset &src);
 TintStrColl *options;
 TfilterIDFFs *filters;
 int filtermode;
 template<class T> static T* new_copy(T *self)
  {
   T *newpreset=new T(_l(""),_l(""),self->filtermode);
   *newpreset=*self;
   return newpreset;
  }
public:
 virtual ~Tpreset();
 virtual Tpreset* copy(void)=0;
 char_t presetName[MAX_PATH];
 void loadDefault(void);
 virtual void loadReg(void);
 void saveReg(void);
 bool loadFile(const char_t *flnm),saveFile(const char_t *flnm);
 virtual void reg_op(TregOp &t);

 struct TautoPresetItemDef
  {
   const char_t *desc,*hint;
   const char_t *regIs;int defIs;
   const char_t *regVal;const char_t *defVal;
   typedef bool (TautoPresetProps::*TcompareFc)(const char_t *str1,const char_t *str2);
   TcompareFc compareFc;
   typedef const char_t* (TautoPresetProps::*TgetValFc)(void);
   TgetValFc getVal;
   const char_t* (*getListItem)(IffdshowDec *deciD,unsigned int index);
   const char_t *help;
  };
 void addAutoPresetItems(const TautoPresetItemDef *IautoPresetItems);
 int autoLoadedFromFile;
 int autoLoadLogic;

 int autoloadExtsNeedFix;
 virtual bool isAutoPreset(TautoPresetProps &props) const;

 const TfilterIDFFs *getFilters(void);
 TfilterSettings *getSettings(int filterID) const;
 const TfilterSettings *indexGetSettings(size_t index) const;
 void addFilter(TfilterSettings *filter);
 bool setFilterOrder(unsigned int filterID,unsigned int newOrder);
 bool resetOrder(void);
 void createFilters(Tfilters *filters,TfilterQueue &queue) const;
 void createPages(TffdshowPageDec *pages) const;
private:
 struct TautoPresetItem
  {
  private:
   int is;
   strings vals;
  public:
   TautoPresetItem(const TautoPresetItemDef *Iitem);
   const TautoPresetItemDef *item;
   void reg_op(TregOp &t);
   void addWild(void);
   bool getIs() const;
   bool match(TautoPresetProps &props) const;
   void get(const char_t* *name,const char_t* *hint,int *allowWildcard,int *is,int *isVal,char_t *val,size_t vallen,int *isList,int *isHelp) const;
   void set(int is,const char_t *val);
   const char_t* getList(IffdshowDec *deciD,unsigned int index);
  };
 typedef std::vector<TautoPresetItem> TautoPresetItems;
 TautoPresetItems autoPresetItems;
public:
 int getMinOrder(void) const,getMaxOrder(void) const;
 size_t getAutoPresetItemsCount(void) const {return autoPresetItems.size();}
 void getAutoPresetItemInfo(unsigned int index,const char_t* *name,const char_t* *hint,int *allowWildcard,int *is,int *isVal,char_t *val,size_t vallen,int *isList,int *isHelp) const {return autoPresetItems[index].get(name,hint,allowWildcard,is,isVal,val,vallen,isList,isHelp);}
 void setAutoPresetItem(unsigned int index,int is,const char_t *val) {autoPresetItems[index].set(is,val);}
 const char_t* getAutoPresetItemList(IffdshowDec *deciD,unsigned int paramIndex,unsigned int listIndex) {return autoPresetItems[paramIndex].getList(deciD,listIndex);}
 void getAutoPresetItemHelp(unsigned int index,const char_t* *helpPtr) {*helpPtr=autoPresetItems[index].item->help;}
};

#endif
