#ifndef _TINFO_H_
#define _TINFO_H_

#include "interfaces.h"
#include "Ttranslate.h"
#include "IffdshowBase.h"

#ifndef OSDTIMETABALE
//#define OSDTIMETABALE // OSD debug item "Time table" to reserch multithread time table. if you don't need this item comment out.
#endif

class TinfoBase
{
protected:
 IffdshowBase *deci;
 struct TinfoItem
  {
   int type;
   const char_t *name,*shortcut;
  };
 struct TinfoValueBase
  {
  private:
   IffdshowBase *deci;
  protected:
   Ttranslate *trans;
   char_t s[512],olds[512];int oldVal;
   virtual const char_t *getVal0(bool &wasChange,bool &splitline);
  public:
   const TinfoItem *item;
   TinfoValueBase(const TinfoItem *Iitem,IffdshowBase *Ideci):item(Iitem),deci(Ideci)
    {
     s[0]=olds[0]='\0';
     oldVal=-1;
     Ideci->getTranslator(&trans);
    }
   virtual ~TinfoValueBase()
    {
     if (trans)
      trans->release();
    }
   const char_t *getVal(bool &wasChange,bool &splitline);
  };
 virtual TinfoValueBase* createNew(const TinfoItem *item)
  {
   return new TinfoValueBase(item,deci);
  }
 void addItems(const TinfoItem *Iitems);
private:
 struct Tvalues : std::hash_map<int,TinfoValueBase*>
  {
   ~Tvalues()
    {
     for (iterator i=begin();i!=end();i++)
      delete i->second;
    }
  } values;
public:
 TinfoBase(IffdshowBase *Ideci);
 virtual ~TinfoBase() {}
 bool getInfo(unsigned int index,int *id,const char_t* *name);
 const char_t* getName(int id);
 const char_t* getShortcut(int id);
 int getInfoShortcutItem(const char_t *s,int *toklen);
 const char_t* getVal(int id,int *wasChange,int *splitline);
};

class TinfoDec : public TinfoBase
{
protected:
 IffdshowDec *deciD;
 virtual TinfoValueBase* createNew(const TinfoItem *item)
  {
   return new TinfoValueDec(item,deci,deciD);
  }
 struct TinfoValueDec : TinfoValueBase
  {
  private:
   IffdshowDec *deciD;
  protected:
   virtual const char_t *getVal0(bool &wasChange,bool &splitline);
  public:
   TinfoValueDec(const TinfoItem *Iitem,IffdshowBase *Ideci,IffdshowDec *IdeciD):TinfoValueBase(Iitem,Ideci),deciD(IdeciD) {}
  };
public:
 TinfoDec(IffdshowBase *Ideci);
};

class TinfoDecVideo : public TinfoDec
{
protected:
 IffdshowDecVideo *deciV;
 virtual TinfoValueBase* createNew(const TinfoItem *item)
  {
   return new TinfoValueDecVideo(item,deci,deciD,deciV);
  }
 struct TinfoValueDecVideo : TinfoValueDec
  {
  private:
   IffdshowDecVideo *deciV;
   char_t sizeStr[256],aspectStr[256];
   unsigned int oldDx,oldDy,oldSar1,oldSar2,oldDar1,oldDar2;
   const char_t *getInputSize(char_t *s,bool &wasChange);
   const char_t *getInputAspect(char_t *s, bool &wasChange, size_t buflen);
  protected:
   virtual const char_t *getVal0(bool &wasChange,bool &splitline);
  public:
   TinfoValueDecVideo(const TinfoItem *Iitem,IffdshowBase *Ideci,IffdshowDec *IdeciD,IffdshowDecVideo *IdeciV):TinfoValueDec(Iitem,Ideci,IdeciD),deciV(IdeciV),oldDx(0),oldDy(0),oldSar1(0),oldSar2(0),oldDar1(0),oldDar2(0) {}
  };
public:
 TinfoDecVideo(IffdshowBase *Ideci);
};

class TinfoDecVideoPict : public TinfoDecVideo
{
public:
 TinfoDecVideoPict(IffdshowBase *Ideci);
};

class TinfoDecAudio : public TinfoDec
{
protected:
 IffdshowDecAudio *deciA;
 virtual TinfoValueBase* createNew(const TinfoItem *item)
  {
   return new TinfoValueDecAudio(item,deci,deciD,deciA);
  }
 struct TinfoValueDecAudio : TinfoValueDec
  {
  private:
   IffdshowDecAudio *deciA;
   const char_t *oldSampleFormat;
   unsigned int oldfreq,oldnchannels;
  protected:
   virtual const char_t *getVal0(bool &wasChange,bool &splitline);
  public:
   TinfoValueDecAudio(const TinfoItem *Iitem,IffdshowBase *Ideci,IffdshowDec *IdeciD,IffdshowDecAudio *IdeciA):TinfoValueDec(Iitem,Ideci,IdeciD),deciA(IdeciA),oldSampleFormat(NULL),oldfreq(0),oldnchannels(0) {}
  };
public:
 TinfoDecAudio(IffdshowBase *Ideci);
};

#endif
