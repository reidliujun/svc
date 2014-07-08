#ifndef _TOPTIONS_H_
#define _TOPTIONS_H_

#include "FastDelegate.h"

struct TffdshowParamInfo;
class TintStrColl;
struct TregOp;
struct TcreateParamListBase;
struct Toptions
{
private:
 TintStrColl *coll;
public:
 typedef int Toptions::*TintVal;
 typedef fastdelegate::FastDelegate2<int,int> TsendOnChange;
 template<class T> struct TintOptionT
  {
   typedef int Tval;
   int id;
   int T::*val;
   int min,max;//meaning
   //-------------------
   //   0   0    on/off switch
   //  -1  -1    readonly
   //  -2  -2    not accessible
   //  -3  -3    dynamic min,max
   // if min==max && min>0 - any value allowed
   typedef fastdelegate::FastDelegate2<int,Tval> TonChangeBind;
   const char_t *name;
   bool inPreset;
   const char_t *regname; //if NULL, don't perform registry operations
   static const int DEF_DYN=13245364;
   Tval def; //HACK: if DYN_DEF, call getDefault
  };
 struct TintOption : TintOptionT<Toptions>
  {
   bool get(Toptions *self,Tval *valPtr) const;
   void set(Toptions *self,Tval val,const TsendOnChange &sendOnChange,const TonChangeBind &onChangeBind) const;
   bool inv(Toptions *self,const TsendOnChange &sendOnChange,const TonChangeBind &onChangeBind) const;
   void getInfo(Toptions *self,TffdshowParamInfo *info) const;
   void reg_op(Toptions *self,TregOp &t) const;
   void reset(Toptions *self) const;
   void reset(Toptions *self,const TsendOnChange &sendOnChange,const TonChangeBind &onChangeBind) const;
  };
 typedef char_t Toptions::*TstrVal;
 struct TstrOption
  {
   typedef const char_t* Tval;
   int id;
   TstrVal val;
   size_t buflen; //if 0, readonly
   typedef fastdelegate::FastDelegate2<int,Tval> TonChangeBind;
   bool multipleLines;
   const char_t *name;
   bool inPreset;
   const char_t *regname; //if NULL, don't perform registry operations
   Tval def; //if NULL, call getDefaultStr
   bool get(Toptions *self,Tval *valPtr) const;
   void set(Toptions *self,Tval val,const TsendOnChange &sendOnChange,const TonChangeBind &onChangeBind) const;
   void getInfo(Toptions *self,TffdshowParamInfo *info) const;
   void reg_op(Toptions *self,TregOp &t) const;
   void reset(Toptions *self) const;
   void reset(Toptions *self,const TsendOnChange &sendOnChange,const TonChangeBind &onChangeBind) const;
  };
 template<class Topt> void getInfo(const Topt *self,TffdshowParamInfo *info);
protected:
 Toptions& operator=(const Toptions &src)
  {
   return *this;
  }
 template<class T> void addOptions(const TintOptionT<T> *iopts) {addOptions((const TintOption*)iopts);}void addOptions(const TintOption *iopts);
 void addOptions(const TstrOption *sopts);
 template<class Tobj,class Tfunc> void setOnChange(int id,Tobj *obj,Tfunc func) const;
 bool notifyChange(int id,int val);
 bool notifyChange(int id,const char_t *val);
 virtual void getMinMax(int id,int &min,int &max) {}
 virtual int getDefault(int id) {return 0;}
 virtual void getDefaultStr(int id,char_t *buf,size_t buflen) {}
 void resetValues(const int *ids);
 void setParamList(int id,const TcreateParamListBase *createParamList);
public:
 Toptions(TintStrColl *Icoll);
 virtual ~Toptions();
 virtual void reg_op(TregOp &t);
};

typedef std::vector< std::pair<int,const char_t*> > TparamListItems;
struct TcreateParamListBase
 {
  virtual TparamListItems* create() const =0;
 };
struct TcreateParamList1 : TcreateParamListBase
{
private:
 const char_t* *items;
public:
 TcreateParamList1(const char_t* *Iitems):items(Iitems) {}
 virtual TparamListItems* create(void) const
  {
   TparamListItems *it=new TparamListItems;
   for (int i=0;items[i];i++)
    it->push_back(std::make_pair(i,items[i]));
   return it;
  }
};
template<class T> struct TcreateParamList2 : TcreateParamListBase
{
private:
 const T *self;
 const char_t* T::*ls;
public:
 TcreateParamList2(const T *Iself,const char_t* T::*Ils):self(Iself),ls(Ils) {}
 virtual TparamListItems* create(void) const
  {
   TparamListItems *it=new TparamListItems;
   for (int i=0;(self+i)->*ls;i++)
    it->push_back(std::make_pair(i,(self+i)->*ls));
   return it;
  }
};
template<class T> struct TcreateParamList3 : TcreateParamListBase
{
private:
 const T *self;
 const int T::*li;
 const char_t* T::*ls;
public:
 TcreateParamList3(const T *Iself,const int T::*Ili,const char_t* T::*Ils):self(Iself),li(Ili),ls(Ils) {}
 virtual TparamListItems* create(void) const
  {
   TparamListItems *it=new TparamListItems;
   for (int i=0;(self+i)->*ls;i++)
    it->push_back(std::make_pair((self+i)->*li,(self+i)->*ls));
   std::sort(it->begin(),it->end());
   return it;
  }
};

template<class Toption> struct TcollOption
{
 TcollOption():coll(NULL),option(NULL),createParamList(NULL) {}
 TcollOption(Toptions *Icoll,const Toption *Ioption):coll(Icoll),option(Ioption),createParamList(NULL) {}
 Toptions *coll;
 const Toption *option;
 typename Toption::TonChangeBind onChange;
 const TcreateParamListBase *createParamList;
};
template<class Toption,class Tbase=std::hash_map<int,TcollOption<Toption> > > class TcollOptions :public Tbase
{
private:
 template<class T> struct TonChangeCmp
  {
  private:
   T &o;
  public:
   TonChangeCmp(T &Io):o(Io) {}
   bool operator()(const T *o2)
    {
     return o==*o2;
    }
  };
 typedef typename Toption::Tval Tval;
public:
 bool get(int id,Tval *valPtr)
  {
   typename Tbase::const_iterator o=this->find(id);
   return o==this->end()?false:o->second.option->get(o->second.coll,valPtr);
  }
 bool set(int id,Tval val,const Toptions::TsendOnChange &sendOnChange)
  {
   typename Tbase::const_iterator o=this->find(id);
   return o==this->end()?false:(o->second.option->set(o->second.coll,val,sendOnChange,o->second.onChange),true);
  }
 bool reset(int id,const Toptions::TsendOnChange &sendOnChange)
  {
   typename Tbase::const_iterator o=this->find(id);
   return o==this->end()?false:(o->second.option->reset(o->second.coll,sendOnChange,o->second.onChange),true);
  }
 bool getInfo(int id,TffdshowParamInfo *info);
 void getInfoIDs(ints &infos);
 bool notify(int id,Tval val);
 TparamListItems* getParamList(int id);

 void addOptions(const Toption *opts,Toptions *coll);
 void removeColl(Toptions *coll);
 void setOnChange(int id,const typename Toption::TonChangeBind &bind);
 void notifyParamsChanged(void);
 void reg_op(TregOp &t,Toptions *coll);
 void reset(const int *idBegin,const int *idEnd,Toptions *coll);
 void setParamList(int id,const TcreateParamListBase *createParamList);
};
class TcollIntOptions :public TcollOptions<Toptions::TintOption>
{
public:
 bool inv(int id,const Toptions::TsendOnChange &sendOnChange)
  {
   const_iterator o=this->find(id);
   return o==this->end()?false:o->second.option->inv(o->second.coll,sendOnChange,o->second.onChange);
  }
};
class TcollStrOptions :public TcollOptions<Toptions::TstrOption>
{
};

class TintStrColl :protected std::pair<TcollIntOptions,TcollStrOptions>
{
private:
 Toptions::TsendOnChange sendOnChange;
public:
 void addOptions(const Toptions::TintOption *iopts,Toptions *coll);
 void addOptions(const Toptions::TstrOption *sopts,Toptions *coll);
 void removeColl(Toptions *coll);
 void setOnChange(int id,const Toptions::TintOption::TonChangeBind &bind);
 void setOnChange(int id,const Toptions::TstrOption::TonChangeBind &bind);
 void setSendOnChange(const Toptions::TsendOnChange &IsendOnChange);
 void notifyParamsChanged(void);
 void reg_op(TregOp &t,Toptions *coll);
 void reset(const int *ids,Toptions *coll);
 bool reset(int id);
 void setParamList(int id,const TcreateParamListBase *createParamList);

 bool get(int id,int *valPtr) {return first.get(id,valPtr);}
 bool set(int id,int val) {return first.set(id,val,sendOnChange);}
 bool inv(int id) {return first.inv(id,sendOnChange);}
 bool notifyParam(int id,int val);

 bool get(int id,const char_t* *valPtr) {return second.get(id,valPtr);}
 bool set(int id,const char_t *val) {return second.set(id,val,sendOnChange);}
 bool notifyParam(int id,const char_t *val);

 bool getInfo(int id,TffdshowParamInfo *info);
 void getInfoIDs(ints &infos);
 TparamListItems* getParamList(int id);
};

template<class Tobj,class Tfunc> inline void Toptions::setOnChange(int id,Tobj *obj,Tfunc func) const
{
 if (coll) coll->setOnChange(id,fastdelegate::MakeDelegate(obj,func));
}

#endif
