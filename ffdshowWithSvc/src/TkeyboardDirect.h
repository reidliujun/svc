#ifndef _TKEYBOARDDIRECT_H_
#define _TKEYBOARDDIRECT_H_

#include "Toptions.h"
#include "interfaces.h"

struct IDirectInput;
struct IDirectInputDevice;
class TdirectInput :public Toptions
{
private:
 const char_t *name;
 const GUID &deviceId;const DIDATAFORMAT &deviceFormat;
 IDirectInput8 *di;
 IDirectInputDevice8 *did; 
 char_t classname[40];
 HWND h;
 HANDLE event;
 uintptr_t thr;
 volatile bool terminate;
 static void thread(void *self);
 void *state,*stateprev;size_t statesize;
 bool inExplorer;
protected:
 TdirectInput(TintStrColl *Icoll,const char_t *Iname,const GUID &IdeviceId,const DIDATAFORMAT &IdeviceFormat,void *Istate,void *Istateprev,size_t Istatesize,IffdshowBase *Ideci);
 int is,always;
 void onChange(int id,int newval);
 IffdshowBase* deci;
 virtual void processState(void)=0;
 static bool windowActive(void);
public:
 virtual void hook(void);
 void unhook(void);
 void load(void),save(void);
};

struct TregOp;
class Tkeyboard :public TdirectInput
{
public:
 struct TkeyParam
  {
   TkeyParam(const char_t *Idescr,int Ikey,int Iidff=0):descr(Idescr),key(Ikey),idff(Iidff) {}
   const char_t *descr;
   int key;
   int idff;
  };
 typedef std::vector<TkeyParam> TkeysParams;
 TkeysParams keysParams;
private:
 enum
 {
  idff_forward      =-1,
  idff_backward     =-2,
  idff_grab         =-3,
  idff_subdelaySub  =-4,
  idff_subdelayAdd  =-5,
  idff_subposSub    =-6,
  idff_subposAdd    =-7,
  idff_videodelaySub=-8,
  idff_videodelayAdd=-9,
  idff_presetPrev   =-10,
  idff_presetNext   =-11
 };
 virtual void reg_op(TregOp &t);
 void keyProc(int code,bool remote);
 struct TkeyInfo
  {
   bool girderok;
   const char_t *keyname;
  };
 static const TkeyInfo keyInfo[256];
 static const unsigned char vk2dik[256];
 clock_t prevT;
 uint8_t dkeys[256],dprevkeys[256];
 int keydown;
 void exportGMLkey(int &id,FILE *f,TkeysParams::const_iterator k,const char *second=NULL);
protected:
 virtual void processState(void);
public:
 Tkeyboard(TintStrColl *Icoll,IffdshowBase *Ideci);
 ~Tkeyboard();
 virtual void hook(void);
 int shortosd;
 int i_key_act1,i_key_act2;
 int i_key_mod;
 int seek1,seek2;
 void initKeysParam(void);
 void reset(void);
 bool exportToGML(const char_t *flnm);
 void keyDown(int key),keyUp(int key);
 static const char_t* getKeyName(int key);
};

class Tmouse :public TdirectInput
{
private:
 DIMOUSESTATE state,prevstate;
protected:
 virtual void processState(void);
public:
 Tmouse(TintStrColl *Icoll,IffdshowBase *Ideci);
 ~Tmouse();
};

#endif
