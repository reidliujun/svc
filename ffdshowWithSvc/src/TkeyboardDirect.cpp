/*
 * Copyright (c) 2002-2006 Milan Cutka
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "stdafx.h"
#include "TkeyboardDirect.h"
#include "IffdshowBase.h"
#include "IffdshowDec.h"
#include "IffdshowDecVideo.h"
#include "ffdshow_constants.h"
#include "reg.h"
#include "ffdebug.h"
#include "Tconfig.h"

//============================ TdirectInput ============================
TdirectInput::TdirectInput(TintStrColl *Icoll,const char_t *Iname,const GUID &IdeviceId,const DIDATAFORMAT &IdeviceFormat,void *Istate,void *Istateprev,size_t Istatesize,IffdshowBase *Ideci):
 Toptions(Icoll),
 name(Iname),
 state(Istate),
 stateprev(Istateprev),
 statesize(Istatesize),
 deci(Ideci),
 deviceId(IdeviceId),deviceFormat(IdeviceFormat)
{
 tsnprintf_s(classname, countof(classname), _TRUNCATE, _l("ffdshow_%s_%i"), name, rand()%1000);
 di=NULL;did=NULL;event=NULL;h=NULL;thr=0;
 inExplorer=deci->inExplorer()==S_OK;
}

void TdirectInput::onChange(int id,int newval)
{
 if (is && !inExplorer)
  {
   int mode=deci->getParam2(IDFF_filterMode);
   if ((mode&IDFF_FILTERMODE_PLAYER) && !(mode&IDFF_FILTERMODE_VFW))
    {
     if (!di)
      hook();
    }
   else
    {
     if (di)
      unhook();
    }
  }
}

void TdirectInput::load(void)
{
 TregOpRegRead t(HKEY_CURRENT_USER,FFDSHOW_REG_PARENT _l("\\") FFDSHOWDECVIDEO);
 reg_op(t);
}
void TdirectInput::save(void)
{
 TregOpRegWrite t(HKEY_CURRENT_USER,FFDSHOW_REG_PARENT _l("\\") FFDSHOWDECVIDEO);
 reg_op(t);
}

void TdirectInput::hook(void)
{
 if (di) unhook();
 DPRINTF(_l("hook"));
 DirectInput8Create(0,DIRECTINPUT_VERSION,IID_IDirectInput8,(void**)&di,NULL);
 if (!di) return;
 di->CreateDevice(deviceId,&did,NULL);if (!did) return;

 h=createInvisibleWindow(0,classname,classname,DefWindowProc,0,NULL);
 if (!h) return;
 if (did->SetCooperativeLevel(h,DISCL_BACKGROUND|DISCL_NONEXCLUSIVE)!=S_OK) return;
 if (did->SetDataFormat(&deviceFormat)!=S_OK) return;
 char_t eventname[50];
 tsnprintf_s(eventname, countof(eventname), _TRUNCATE, _l("ffdshow %s event"), name);
 event=CreateEvent(NULL,FALSE,FALSE,eventname);if (!event) return;
 if (did->SetEventNotification(event)!=S_OK) return;
 if (did->Acquire()!=S_OK) return;
 terminate=false;
 thr=_beginthread(thread,16384,this);
}
void TdirectInput::unhook(void)
{
 if (!di) return;
 DPRINTF(_l("unhook"));
 if (thr)
  {
   terminate=true;
   while (terminate);
  }
 if (did)
  {
   did->Unacquire();
   did->SetEventNotification(NULL);
   if (event) CloseHandle(event);event=NULL;
   did->Release();
   did=NULL;
  }
 if (di) di->Release();di=NULL;
 if (h) DestroyWindow(h);
 HINSTANCE hi;deci->getInstance(&hi);
 UnregisterClass(classname,hi);
}
void TdirectInput::thread(void *self0)
{
 TdirectInput *self=(TdirectInput*)self0;
 setThreadName(DWORD(-1),text<char>(self->name));
 self->did->GetDeviceState((DWORD)self->statesize,self->stateprev);
 while (!self->terminate)
  {
   HRESULT res=WaitForSingleObject(self->event,100);
   if (res==WAIT_OBJECT_0 || res==WAIT_TIMEOUT)
    {
     self->did->GetDeviceState((DWORD)self->statesize,self->state);
     self->processState();
     memcpy(self->stateprev,self->state,self->statesize);
    }
  }
 self->terminate=false;
}

bool TdirectInput::windowActive(void)
{
 HWND tw=GetForegroundWindow();
 DWORD tpid=0;GetWindowThreadProcessId(tw,&tpid);
 DWORD cpid=GetCurrentProcessId();
 return cpid==tpid;
}

//============================== Tkeyboard =============================
void Tkeyboard::reg_op(TregOp &t)
{
 TdirectInput::reg_op(t);
 char_t pomS[256];
 for (TkeysParams::iterator i=keysParams.begin();i!=keysParams.end();i++)
  t._REG_OP_N(0,strncpyf(pomS, countof(pomS),_l("key %s"),i->descr),i->key,i->key);
}

void Tkeyboard::initKeysParam(void)
{
 keysParams.push_back(TkeyParam(_l("Activation key 1"),VK_CONTROL));i_key_act1=0;
 keysParams.push_back(TkeyParam(_l("Activation key 2"),VK_MENU));i_key_act2=1;
 keysParams.push_back(TkeyParam(_l("Second function") ,VK_SHIFT));i_key_mod=2;

 keysParams.push_back(TkeyParam(_l("Seek forward") ,VK_RIGHT,idff_forward ));
 keysParams.push_back(TkeyParam(_l("Seek backward"),VK_LEFT ,idff_backward));

 keysParams.push_back(TkeyParam(_l("Toggle OSD"),'O',IDFF_isOSD));
 keysParams.push_back(TkeyParam(_l("Toggle visualizations"),'V',IDFF_isVis));
 keysParams.push_back(TkeyParam(_l("Toggle crop/zoom"),'C',IDFF_isCropNzoom));
 keysParams.push_back(TkeyParam(_l("Toggle deinterlace"),'D',IDFF_isDeinterlace));
 keysParams.push_back(TkeyParam(_l("Toggle levels"),'L',IDFF_isLevels));
 keysParams.push_back(TkeyParam(_l("Toggle noise"),'N',IDFF_isNoise));
 keysParams.push_back(TkeyParam(_l("Toggle picture properties"),'I',IDFF_isPictProp));
 keysParams.push_back(TkeyParam(_l("Toggle postprocessing"),'P',IDFF_isPostproc));
 keysParams.push_back(TkeyParam(_l("Toggle blur"),'B',IDFF_isBlur));
 keysParams.push_back(TkeyParam(_l("Toggle resize"),'R',IDFF_isResize));
 keysParams.push_back(TkeyParam(_l("Toggle subtitles"),'S',IDFF_isSubtitles));
 keysParams.push_back(TkeyParam(_l("Toggle sharpen"),'H',IDFF_isSharpen));
 keysParams.push_back(TkeyParam(_l("Toggle flip"),'F',IDFF_flip));
 keysParams.push_back(TkeyParam(_l("Toggle avisynth"),'A',IDFF_isAvisynth));
 keysParams.push_back(TkeyParam(_l("Toggle warpsharp"),'W',IDFF_isWarpsharp));

 keysParams.push_back(TkeyParam(_l("Grab frame"),'G',idff_grab));

 keysParams.push_back(TkeyParam(_l("Subtitles delay/size decrease"),0x6d,idff_subdelaySub));
 keysParams.push_back(TkeyParam(_l("Subtitles delay/size increase"),0x6b,idff_subdelayAdd));
 keysParams.push_back(TkeyParam(_l("Subtitles position decrease"),0x21,idff_subposSub));
 keysParams.push_back(TkeyParam(_l("Subtitles position increase"),0x22,idff_subposAdd));

 keysParams.push_back(TkeyParam(_l("Decrease video delay"),0xbc,idff_videodelaySub));
 keysParams.push_back(TkeyParam(_l("Increase video delay"),0xbe,idff_videodelayAdd));

 keysParams.push_back(TkeyParam(_l("Previous preset"),0xdb,idff_presetPrev));
 keysParams.push_back(TkeyParam(_l("Next preset"),0xdd,idff_presetNext));
}
void Tkeyboard::keyProc(int code,bool remote)
{
 dkeys[DIK_LCONTROL]|=dkeys[DIK_RCONTROL];
 dkeys[DIK_LALT]|=dkeys[DIK_RALT];
 dkeys[DIK_LSHIFT]|=dkeys[DIK_RSHIFT];
 dkeys[DIK_LMENU]|=dkeys[DIK_RMENU];
 if (code==DIK_RCONTROL) code=DIK_LCONTROL;
 else if (code==DIK_RALT) code=DIK_LALT;
 else if (code==DIK_RSHIFT) code=DIK_LSHIFT;
 else if (code==DIK_RMENU) code=DIK_LMENU;

 DPRINTF(_l("keyProc:%x"),code);
 if ((keysParams[i_key_act1].key && !dkeys[vk2dik[keysParams[i_key_act1].key]]) ||
     (keysParams[i_key_act2].key && !dkeys[vk2dik[keysParams[i_key_act2].key]])) return;
 if (!always && !remote && !windowActive())
  return;
 clock_t t1=clock();
 bool isPress=t1-prevT>CLOCKS_PER_SEC/5;
 DPRINTF(_l("prev:%i, now:%i, isPress:%i"),prevT,t1,isPress);
 clock_t prevT0=prevT;
 prevT=t1;
 if (isPress)
  {
   comptrQ<IffdshowDec> deciD(deci);comptrQ<IffdshowDecVideo> deciV(deci);
   bool isMod=keysParams[i_key_mod].key && dkeys[vk2dik[keysParams[i_key_mod].key]];
   for (TkeysParams::const_iterator i=keysParams.begin();i!=keysParams.end();i++)
    if (code==vk2dik[i->key])
     if (i->idff>0 && !deci->getCfgDlgHwnd())
      {
       switch (i->idff)
        {
         case IDFF_isOSD:
          if (isMod)
           {
            if (deci->getParam2(IDFF_isOSD))
             deciV->cycleOSDpresets();
            return;
           }
          break;
         case IDFF_isSubtitles:
          if (isMod)
           {
            if (deciV->cycleSubLanguages(+1)==S_OK && shortosd)
             {
              const char_t *lang;
              deciV->getSubtitleLanguageDesc(deci->getParam2(IDFF_subCurLang),&lang);
              if (lang && lang[0])
               {
                char_t msg[256];
                tsnprintf_s(msg, countof(msg), _TRUNCATE, _l("subtitles language: %s"), lang);
                deciV->shortOSDmessage(msg,30);
               }
             }
            return;
           }
          break;
        }
       deci->invParam(i->idff);
       deci->saveGlobalSettings();
       deciD->saveActivePreset(NULL);
       if (shortosd)
        {
         char_t msg[200];
         tsnprintf_s(msg, countof(msg), _TRUNCATE, _l("%s %s"), i->descr,deci->getParam2(i->idff) ? _l("on") : _l("off"));
         deciV->shortOSDmessage(msg,30);
        }
       return;
      }
     else
      switch (i->idff)
       {
        case idff_forward:
        case idff_backward:
         {
          if (deci->getState2()==State_Paused)
           deciV->frameStep((i->idff==idff_backward?-1:1)*(isMod?10:1));
          else
           {
            int pos;deciD->tell(&pos);
            if (pos!=-1)
             {
              int sec=isMod?seek2:seek1;if (i->idff==idff_backward) sec*=-1;
              if (SUCCEEDED(deciD->seek(pos+sec)) && shortosd)
               {
                char_t msg[100];
                tsnprintf_s(msg, countof(msg), _TRUNCATE, _l("fast %s %i seconds"), sec < 0 ? _l("backward") : _l("forward"), abs(sec));
                deciV->shortOSDmessage(msg,30);
               }
             }
           }
          return;
         }
        case idff_grab:
         deciV->grabNow();
         return;
        case idff_subdelaySub:
         {
          int idff=isMod?(deci->getParam2(IDFF_fontAutosize)?IDFF_fontSizeA:IDFF_fontSizeP):IDFF_subDelay;
          deci->putParam(idff,deci->getParam2(idff)-(isMod?2:100));
          if (shortosd)
           {
            char_t msg[256];
            tsnprintf_s(msg, countof(msg), _TRUNCATE, _l("subtitles %s: %i"), isMod ? _l("size") : _l("delay"), deci->getParam2(idff));
            deciV->shortOSDmessage(msg,30);
           }
          return;
         }
        case idff_subdelayAdd:
         {
          int idff=isMod?(deci->getParam2(IDFF_fontAutosize)?IDFF_fontSizeA:IDFF_fontSizeP):IDFF_subDelay;
          deci->putParam(idff,deci->getParam2(idff)+(isMod?2:100));
          if (shortosd)
           {
            char_t msg[256];
            tsnprintf_s(msg, countof(msg), _TRUNCATE, _l("subtitles %s: %i"), isMod ? _l("size") : _l("delay"), deci->getParam2(idff));
            deciV->shortOSDmessage(msg,30);
           }
          return;
         }
        case idff_subposSub:
         {
          int idff=isMod?IDFF_subPosX:IDFF_subPosY;
          deci->putParam(idff,deci->getParam2(idff)-5);
          if (shortosd)
           {
            char_t msg[256];
            tsnprintf_s(msg, countof(msg), _TRUNCATE, _l("subtitles %s position : %i"), isMod ? _l("horizontal") : _l("vertical"), deci->getParam2(idff));
            deciV->shortOSDmessage(msg,30);
           }
          return;
         }
        case idff_subposAdd:
         {
          int idff=isMod?IDFF_subPosX:IDFF_subPosY;
          deci->putParam(idff,deci->getParam2(idff)+5);
          if (shortosd)
           {
            char_t msg[256];
            tsnprintf_s(msg, countof(msg), _TRUNCATE, _l("subtitles %s position : %i"), isMod ? _l("horizontal") : _l("vertical"), deci->getParam2(idff));
            deciV->shortOSDmessage(msg,30);
           }
          return;
         }
        case idff_videodelayAdd:
        case idff_videodelaySub:
         {
          //deci->putParam(IDFF_isVideoDelayEnd,0);
          int delay=deci->getParam2(IDFF_videoDelay)+(i->idff==idff_videodelayAdd?20:-20);
          deci->putParam(IDFF_videoDelay,delay);
          if (shortosd)
           {
            char_t msg[256];
            tsnprintf_s(msg, countof(msg), _TRUNCATE, _l("video delay: %i ms"), delay);
            deciV->shortOSDmessage(msg,30);
           }
          return;
         }
        case idff_presetPrev:
        case idff_presetNext:
         {
          deciD->cyclePresets(i->idff==idff_presetNext?1:-1);
          if (shortosd)
           {
            char_t msg[256],preset[256];
            deciD->getActivePresetName(preset,256);
            tsnprintf_s(msg, countof(msg), _TRUNCATE, _l("preset: %s"), preset);
            deciV->shortOSDmessage(msg,30);
           }
          return;
         }
       }
  }
 prevT=prevT0;
}
void Tkeyboard::processState(void)
{
 for (int k=0;k<256;k++)
  if (dprevkeys[k]!=dkeys[k])
   if (dkeys[k])
    keydown=k;
   else
    if (k==keydown) keydown=-1;
 if (keydown!=-1) keyProc(keydown,false);
}

Tkeyboard::Tkeyboard(TintStrColl *Icoll,IffdshowBase *Ideci):TdirectInput(Icoll,_l("keys"),GUID_SysKeyboard,c_dfDIKeyboard,dkeys,dprevkeys,sizeof(dkeys),Ideci)
{
 static const TintOptionT<Tkeyboard> iopts[]=
  {
   IDFF_isKeys       ,&Tkeyboard::is       ,0,0,_l(""),0,
     _l("keyboard"),0,
   IDFF_keysAlways   ,&Tkeyboard::always   ,0,0,_l(""),0,
     _l("keysAlways"),0,
   IDFF_keysShortOsd ,&Tkeyboard::shortosd ,0,0,_l(""),0,
     _l("showKeysMessages"),1,
   IDFF_keysSeek1    ,&Tkeyboard::seek1    ,1,10000,_l(""),0,
     _l("keysSeek1"),15,
   IDFF_keysSeek2    ,&Tkeyboard::seek2    ,1,10000,_l(""),0,
     _l("keysSeek2"),60,
   0
  };
 addOptions(iopts);
 setOnChange(IDFF_isKeys,this,&Tkeyboard::onChange);

 prevT=0;
 initKeysParam();
 load();
 memset(dkeys,0,sizeof(dkeys));memset(dprevkeys,0,sizeof(dprevkeys));
}
Tkeyboard::~Tkeyboard()
{
 unhook();
}

void Tkeyboard::reset(void)
{
 always=0;
 shortosd=1;
 seek1=15;
 seek2=60;
 keysParams.clear();
 initKeysParam();
}
void Tkeyboard::hook(void)
{
 keydown=-1;
 TdirectInput::hook();
}

void Tkeyboard::keyDown(int key)
{
 dkeys[vk2dik[key&255]]=1;
 keyProc(vk2dik[key&255],true);
}
void Tkeyboard::keyUp(int key)
{
 dkeys[vk2dik[key&255]]=0;
}

void Tkeyboard::exportGMLkey(int &id,FILE *f,TkeysParams::const_iterator k,const char *second)
{
 fprintf(f,"    <Command Name=\"%s\" Identifier=\"%i\" Enabled=\"TRUE\">\n",second?second:(const char*)text<char>(k->descr),id++);
 fprintf(f,"      <StateCount>1</StateCount>\n");
 fprintf(f,"      <StateBegin>1</StateBegin>\n");
 fprintf(f,"      <StateCurrent>1</StateCurrent>\n");
 fprintf(f,"      <StateInvert>FALSE</StateInvert>\n");
 fprintf(f,"      <OSD>FALSE</OSD>\n");
 fprintf(f,"      <Target_class>ffdshow_remote_class</Target_class>\n");
 fprintf(f,"      <Submatch>FALSE</Submatch>\n");
 fprintf(f,"      <Topmost>FALSE</Topmost>\n");
 fprintf(f,"      <MatchBy>2</MatchBy>\n");
 fprintf(f,"      <OneMatch>FALSE</OneMatch>\n");
 fprintf(f,"      <UseVar>TRUE</UseVar>\n");
 fprintf(f,"      <MatchHidden>TRUE</MatchHidden>\n");
 fprintf(f,"      <MatchNum>1</MatchNum>\n");
 fprintf(f,"      <ActionType>6</ActionType>\n");
 fprintf(f,"      <ActionSubType>1</ActionSubType>\n");
 if (keyInfo[k->key&255].girderok)
  fprintf(f,"      <sValue1>%s</sValue1>\n",(const char*)text<char>(keyInfo[k->key&255].keyname));
 else
  fprintf(f,"      <iValue1>%i</iValue1>\n",k->key);
 fprintf(f,"      <sValue2>FALSE</sValue2>\n");
 fprintf(f,"      <bValue1>TRUE</bValue1>\n");
 fprintf(f,"      <bValue2>%s</bValue2>\n",second?"TRUE":"FALSE");
 fprintf(f,"      <bValue3>TRUE</bValue3>\n");
 fprintf(f,"    </Command>\n");
}
bool Tkeyboard::exportToGML(const char_t *flnm)
{
 FILE *f=fopen(flnm,_l("wt"));if (!f) return false;
 fprintf(f,"<?xml version=\"1.0\" encoding=\"utf-8\"?>\n");
 fprintf(f,"<Girder>\n");
 int id=2;
 fprintf(f,"  <Group Name=\"ffdshow\" Identifier=\"%i\" Enabled=\"TRUE\">\n",id++);
 for (TkeysParams::const_iterator k=keysParams.begin()+3;k!=keysParams.end();k++)
  {
   exportGMLkey(id,f,k);
   switch(k->idff)
    {
     case idff_forward:exportGMLkey(id,f,k,"Long seek forward");break;
     case idff_backward:exportGMLkey(id,f,k,"Long seek backward");break;
     case IDFF_isOSD:exportGMLkey(id,f,k,"Cycle OSD presets");break;
     case IDFF_isSubtitles:exportGMLkey(id,f,k,"Cycle subtitle languages");break;
     case idff_subdelaySub:exportGMLkey(id,f,k,"Descrease subtitles font size");break;
     case idff_subdelayAdd:exportGMLkey(id,f,k,"Increase subtitles font size");break;
     case idff_subposSub:exportGMLkey(id,f,k,"Subtitles horizontal position decrease");break;
     case idff_subposAdd:exportGMLkey(id,f,k,"Subtitles horizontal position increase");break;
    }
  }
 fprintf(f,"  </Group>\n");
 fprintf(f,"</Girder>\n");
 fclose(f);
 return true;
}

const Tkeyboard::TkeyInfo Tkeyboard::keyInfo[256]=
{
false,/*00*/_l("none"),
false,/*01*/_l("VK_LBUTTON"),
false,/*02*/_l("VK_RBUTTON"),
false,/*03*/_l("VK_CANCEL"),
false,/*04*/_l("VK_MBUTTON"),
false,/*05*/_l("VK_XBUTTON1"),
false,/*06*/_l("VK_XBUTTON2"),
false,/*07*/_l(""),
false,/*08*/_l("VK_BACK"),
false,/*09*/_l("tab"),
false,/*0A*/_l(""),
false,/*0B*/_l(""),
false,/*0C*/_l("VK_CLEAR"),
false,/*0D*/_l("enter"),
false,/*0E*/_l(""),
false,/*0F*/_l(""),
false,/*10*/_l("shift"),
false,/*11*/_l("ctrl"),
false,/*12*/_l("alt"),
false,/*13*/_l("VK_PAUSE"),
false,/*14*/_l("caps lock"),
false,/*15*/_l("VK_HANGUL"),
false,/*16*/_l(""),
false,/*17*/_l("VK_JUNJA"),
false,/*18*/_l("VK_FINAL"),
false,/*19*/_l("VK_HANJA"),
false,/*1A*/_l(""),
false,/*1B*/_l("esc"),
false,/*1C*/_l("VK_CONVERT"),
false,/*1D*/_l("VK_NONCONVERT"),
false,/*1E*/_l("VK_ACCEPT"),
false,/*1F*/_l("VK_MODECHANGE"),
false,/*20*/_l("space"),
false,/*21*/_l("page up"),
false,/*22*/_l("page down"),
false,/*23*/_l("end"),
false,/*24*/_l("home"),
false,/*25*/_l("left arrow"),
false,/*26*/_l("up arrow"),
false,/*27*/_l("right arrow"),
false,/*28*/_l("down arrow"),
false,/*29*/_l("VK_SELECT"),
false,/*2A*/_l("VK_PRINT"),
false,/*2B*/_l("VK_EXECUTE"),
false,/*2C*/_l("VK_SNAPSHOT"),
false,/*2D*/_l("insert"),
false,/*2E*/_l("delete"),
false,/*2F*/_l("VK_HELP"),
true ,/*30*/_l("0"),
true ,/*31*/_l("1"),
true ,/*32*/_l("2"),
true ,/*33*/_l("3"),
true ,/*34*/_l("4"),
true ,/*35*/_l("5"),
true ,/*36*/_l("6"),
true ,/*37*/_l("7"),
true ,/*38*/_l("8"),
true ,/*39*/_l("9"),
false,/*3A*/_l(""),
false,/*3B*/_l(""),
false,/*3C*/_l(""),
false,/*3D*/_l(""),
false,/*3E*/_l(""),
false,/*3F*/_l(""),
false,/*40*/_l(""),
true ,/*41*/_l("A"),
true ,/*42*/_l("B"),
true ,/*43*/_l("C"),
true ,/*44*/_l("D"),
true ,/*45*/_l("E"),
true ,/*46*/_l("F"),
true ,/*47*/_l("G"),
true ,/*48*/_l("H"),
true ,/*49*/_l("I"),
true ,/*4A*/_l("J"),
true ,/*4B*/_l("K"),
true ,/*4C*/_l("L"),
true ,/*4D*/_l("M"),
true ,/*4E*/_l("N"),
true ,/*4F*/_l("O"),
true ,/*50*/_l("P"),
true ,/*51*/_l("Q"),
true ,/*52*/_l("R"),
true ,/*53*/_l("S"),
true ,/*54*/_l("T"),
true ,/*55*/_l("U"),
true ,/*56*/_l("V"),
true ,/*57*/_l("W"),
true ,/*58*/_l("X"),
true ,/*59*/_l("Y"),
true ,/*5A*/_l("Z"),
false,/*5B*/_l("left win"),
false,/*5C*/_l("right win"),
false,/*5D*/_l("menu key"),
false,/*5E*/_l(""),
false,/*5F*/_l("VK_SLEEP"),
false,/*60*/_l("numpad 0"),
false,/*61*/_l("numpad 1"),
false,/*62*/_l("numpad 2"),
false,/*63*/_l("numpad 3"),
false,/*64*/_l("numpad 4"),
false,/*65*/_l("numpad 5"),
false,/*66*/_l("numpad 6"),
false,/*67*/_l("numpad 7"),
false,/*68*/_l("numpad 8"),
false,/*69*/_l("numpad 9"),
false,/*6A*/_l("numpad *"),
false,/*6B*/_l("numpad +"),
false,/*6C*/_l("VK_SEPARATOR"),
false,/*6D*/_l("numpad -"),
false,/*6E*/_l("VK_DECIMAL"),
false,/*6F*/_l("numpad /"),
false,/*70*/_l("F1"),
false,/*71*/_l("F2"),
false,/*72*/_l("F3"),
false,/*73*/_l("F4"),
false,/*74*/_l("F5"),
false,/*75*/_l("F6"),
false,/*76*/_l("F7"),
false,/*77*/_l("F8"),
false,/*78*/_l("F9"),
false,/*79*/_l("F10"),
false,/*7A*/_l("F11"),
false,/*7B*/_l("F12"),
false,/*7C*/_l("F13"),
false,/*7D*/_l("F14"),
false,/*7E*/_l("F15"),
false,/*7F*/_l("F16"),
false,/*80*/_l("F17"),
false,/*81*/_l("F18"),
false,/*82*/_l("F19"),
false,/*83*/_l("F20"),
false,/*84*/_l("F21"),
false,/*85*/_l("F22"),
false,/*86*/_l("F23"),
false,/*87*/_l("F24"),
false,/*88*/_l(""),
false,/*89*/_l(""),
false,/*8A*/_l(""),
false,/*8B*/_l(""),
false,/*8C*/_l(""),
false,/*8D*/_l(""),
false,/*8E*/_l(""),
false,/*8F*/_l(""),
false,/*90*/_l("numlock"),
false,/*91*/_l("VK_SCROLL"),
false,/*92*/_l("VK_OEM_NEC_EQUAL"),
false,/*93*/_l(""),
false,/*94*/_l(""),
false,/*95*/_l(""),
false,/*96*/_l(""),
false,/*97*/_l(""),
false,/*98*/_l(""),
false,/*99*/_l(""),
false,/*9A*/_l(""),
false,/*9B*/_l(""),
false,/*9C*/_l(""),
false,/*9D*/_l(""),
false,/*9E*/_l(""),
false,/*9F*/_l(""),
false,/*A0*/_l("VK_LSHIFT"),
false,/*A1*/_l("VK_RSHIFT"),
false,/*A2*/_l("VK_LCONTROL"),
false,/*A3*/_l("VK_RCONTROL"),
false,/*A4*/_l("VK_LMENU"),
false,/*A5*/_l("VK_RMENU"),
false,/*A6*/_l("VK_BROWSER_BACK"),
false,/*A7*/_l("VK_BROWSER_FORWARD"),
false,/*A8*/_l("VK_BROWSER_REFRESH"),
false,/*A9*/_l("VK_BROWSER_STOP"),
false,/*AA*/_l("VK_BROWSER_SEARCH"),
false,/*AB*/_l("VK_BROWSER_FAVORITES"),
false,/*AC*/_l("VK_BROWSER_HOME"),
false,/*AD*/_l("VK_VOLUME_MUTE"),
false,/*AE*/_l("VK_VOLUME_DOWN"),
false,/*AF*/_l("VK_VOLUME_UP"),
false,/*B0*/_l("VK_MEDIA_NEXT_TRACK"),
false,/*B1*/_l("VK_MEDIA_PREV_TRACK"),
false,/*B2*/_l("VK_MEDIA_STOP"),
false,/*B3*/_l("VK_MEDIA_PLAY_PAUSE"),
false,/*B4*/_l("VK_LAUNCH_MAIL"),
false,/*B5*/_l("VK_LAUNCH_MEDIA_SELECT"),
false,/*B6*/_l("VK_LAUNCH_APP1"),
false,/*B7*/_l("VK_LAUNCH_APP2"),
false,/*B8*/_l(""),
false,/*B9*/_l(""),
true ,/*BA*/_l(";"),
true ,/*BB*/_l("="),
true ,/*BC*/_l(","),
true ,/*BD*/_l("-"),
true ,/*BE*/_l("."),
true ,/*BF*/_l("/"),
true ,/*C0*/_l("`"),
false,/*C1*/_l(""),
false,/*C2*/_l(""),
false,/*C3*/_l(""),
false,/*C4*/_l(""),
false,/*C5*/_l(""),
false,/*C6*/_l(""),
false,/*C7*/_l(""),
false,/*C8*/_l(""),
false,/*C9*/_l(""),
false,/*CA*/_l(""),
false,/*CB*/_l(""),
false,/*CC*/_l(""),
false,/*CD*/_l(""),
false,/*CE*/_l(""),
false,/*CF*/_l(""),
false,/*D0*/_l(""),
false,/*D1*/_l(""),
false,/*D2*/_l(""),
false,/*D3*/_l(""),
false,/*D4*/_l(""),
false,/*D5*/_l(""),
false,/*D6*/_l(""),
false,/*D7*/_l(""),
false,/*D8*/_l(""),
false,/*D9*/_l(""),
false,/*DA*/_l(""),
true ,/*DB*/_l("["),
true ,/*DC*/_l("\\"),
true ,/*DD*/_l("]"),
true ,/*DE*/_l("'"),
false,/*DF*/_l("VK_OEM_8"),
false,/*E0*/_l(""),
false,/*E1*/_l("VK_OEM_AX"),
false,/*E2*/_l("VK_OEM_102"),
false,/*E3*/_l("VK_ICO_HELP"),
false,/*E4*/_l("VK_ICO_00"),
false,/*E5*/_l("VK_PROCESSKEY"),
false,/*E6*/_l("VK_ICO_CLEAR"),
false,/*E7*/_l("VK_PACKET"),
false,/*E8*/_l(""),
false,/*E9*/_l("VK_OEM_RESET"),
false,/*EA*/_l("VK_OEM_JUMP"),
false,/*EB*/_l("VK_OEM_PA1"),
false,/*EC*/_l("VK_OEM_PA2"),
false,/*ED*/_l("VK_OEM_PA3"),
false,/*EE*/_l("VK_OEM_WSCTRL"),
false,/*EF*/_l("VK_OEM_CUSEL"),
false,/*F0*/_l("VK_OEM_ATTN"),
false,/*F1*/_l("VK_OEM_FINISH"),
false,/*F2*/_l("VK_OEM_COPY"),
false,/*F3*/_l("VK_OEM_AUTO"),
false,/*F4*/_l("VK_OEM_ENLW"),
false,/*F5*/_l("VK_OEM_BACKTAB"),
false,/*F6*/_l("VK_ATTN"),
false,/*F7*/_l("VK_CRSEL"),
false,/*F8*/_l("VK_EXSEL"),
false,/*F9*/_l("VK_EREOF"),
false,/*FA*/_l("VK_PLAY"),
false,/*FB*/_l("VK_ZOOM"),
false,/*FC*/_l("VK_NONAME"),
false,/*FD*/_l("VK_PA1"),
false,/*FE*/_l("VK_OEM_CLEAR"),
false,/*FF*/_l("")
};

const unsigned char Tkeyboard::vk2dik[256]=
{
/*00                        */ 0,
/*01 VK_LBUTTON             */ 0,
/*02 VK_RBUTTON             */ 0,
/*03 VK_CANCEL              */ 0,
/*04 VK_MBUTTON             */ 0,
/*05 VK_XBUTTON1            */ 0,
/*06 VK_XBUTTON2            */ 0,
/*07                        */ 0,
/*08 VK_BACK                */ DIK_BACK,
/*09 VK_TAB                 */ DIK_TAB,
/*0A                        */ 0,
/*0B                        */ 0,
/*0C VK_CLEAR               */ 0,
/*0D VK_RETURN              */ DIK_RETURN,
/*0E                        */ 0,
/*0F                        */ 0,
/*10 VK_SHIFT               */ DIK_LSHIFT,
/*11 VK_CONTROL             */ DIK_LCONTROL,
/*12 VK_MENU                */ DIK_LMENU,
/*13 VK_PAUSE               */ DIK_PAUSE,
/*14 VK_CAPITAL             */ DIK_CAPITAL,
/*15 VK_HANGUL              */ 0,
/*16                        */ 0,
/*17 VK_JUNJA               */ 0,
/*18 VK_FINAL               */ 0,
/*19 VK_HANJA               */ 0,
/*1A                        */ 0,
/*1B VK_ESCAPE              */ DIK_ESCAPE,
/*1C VK_CONVERT             */ DIK_CONVERT,
/*1D VK_NONCONVERT          */ DIK_NOCONVERT,
/*1E VK_ACCEPT              */ 0,
/*1F VK_MODECHANGE          */ 0,
/*20 VK_SPACE               */ DIK_SPACE,
/*21 VK_PRIOR               */ DIK_PRIOR,
/*22 VK_NEXT                */ DIK_NEXT,
/*23 VK_END                 */ DIK_END,
/*24 VK_HOME                */ DIK_HOME,
/*25 VK_LEFT                */ DIK_LEFT,
/*26 VK_UP                  */ DIK_UP,
/*27 VK_RIGHT               */ DIK_RIGHT,
/*28 VK_DOWN                */ DIK_DOWN,
/*29 VK_SELECT              */ 0,
/*2A VK_PRINT               */ 0,
/*2B VK_EXECUTE             */ 0,
/*2C VK_SNAPSHOT            */ 0,
/*2D VK_INSERT              */ DIK_INSERT,
/*2E VK_DELETE              */ DIK_DELETE,
/*2F VK_HELP                */ 0,
/*30 0                      */ DIK_0,
/*31 1                      */ DIK_1,
/*32 2                      */ DIK_2,
/*33 3                      */ DIK_3,
/*34 4                      */ DIK_4,
/*35 5                      */ DIK_5,
/*36 6                      */ DIK_6,
/*37 7                      */ DIK_7,
/*38 8                      */ DIK_8,
/*39 9                      */ DIK_9,
/*3A                        */ 0,
/*3B                        */ 0,
/*3C                        */ 0,
/*3D                        */ 0,
/*3E                        */ 0,
/*3F                        */ 0,
/*40                        */ 0,
/*41 A                      */ DIK_A,
/*42 B                      */ DIK_B,
/*43 C                      */ DIK_C,
/*44 D                      */ DIK_D,
/*45 E                      */ DIK_E,
/*46 F                      */ DIK_F,
/*47 G                      */ DIK_G,
/*48 H                      */ DIK_H,
/*49 I                      */ DIK_I,
/*4A J                      */ DIK_J,
/*4B K                      */ DIK_K,
/*4C L                      */ DIK_L,
/*4D M                      */ DIK_M,
/*4E N                      */ DIK_N,
/*4F O                      */ DIK_O,
/*50 P                      */ DIK_P,
/*51 Q                      */ DIK_Q,
/*52 R                      */ DIK_R,
/*53 S                      */ DIK_S,
/*54 T                      */ DIK_T,
/*55 U                      */ DIK_U,
/*56 V                      */ DIK_V,
/*57 W                      */ DIK_W,
/*58 X                      */ DIK_X,
/*59 Y                      */ DIK_Y,
/*5A Z                      */ DIK_Z,
/*5B VK_LWIN                */ DIK_LWIN,
/*5C VK_RWIN                */ DIK_RWIN,
/*5D VK_APPS                */ DIK_APPS,
/*5E                        */ 0,
/*5F VK_SLEEP               */ DIK_SLEEP,
/*60 VK_NUMPAD0             */ DIK_NUMPAD0,
/*61 VK_NUMPAD1             */ DIK_NUMPAD1,
/*62 VK_NUMPAD2             */ DIK_NUMPAD2,
/*63 VK_NUMPAD3             */ DIK_NUMPAD3,
/*64 VK_NUMPAD4             */ DIK_NUMPAD4,
/*65 VK_NUMPAD5             */ DIK_NUMPAD5,
/*66 VK_NUMPAD6             */ DIK_NUMPAD6,
/*67 VK_NUMPAD7             */ DIK_NUMPAD7,
/*68 VK_NUMPAD8             */ DIK_NUMPAD8,
/*69 VK_NUMPAD9             */ DIK_NUMPAD9,
/*6A VK_MULTIPLY            */ DIK_MULTIPLY,
/*6B VK_ADD                 */ DIK_ADD,
/*6C VK_SEPARATOR           */ 0,
/*6D VK_SUBTRACT            */ DIK_SUBTRACT,
/*6E VK_DECIMAL             */ DIK_DECIMAL,
/*6F VK_DIVIDE              */ DIK_DIVIDE,
/*70 VK_F1                  */ DIK_F1,
/*71 VK_F2                  */ DIK_F2,
/*72 VK_F3                  */ DIK_F3,
/*73 VK_F4                  */ DIK_F4,
/*74 VK_F5                  */ DIK_F5,
/*75 VK_F6                  */ DIK_F6,
/*76 VK_F7                  */ DIK_F7,
/*77 VK_F8                  */ DIK_F8,
/*78 VK_F9                  */ DIK_F9,
/*79 VK_F10                 */ DIK_F10,
/*7A VK_F11                 */ DIK_F11,
/*7B VK_F12                 */ DIK_F12,
/*7C VK_F13                 */ DIK_F13,
/*7D VK_F14                 */ DIK_F14,
/*7E VK_F15                 */ DIK_F15,
/*7F VK_F16                 */ 0,
/*80 VK_F17                 */ 0,
/*81 VK_F18                 */ 0,
/*82 VK_F19                 */ 0,
/*83 VK_F20                 */ 0,
/*84 VK_F21                 */ 0,
/*85 VK_F22                 */ 0,
/*86 VK_F23                 */ 0,
/*87 VK_F24                 */ 0,
/*88                        */ 0,
/*89                        */ 0,
/*8A                        */ 0,
/*8B                        */ 0,
/*8C                        */ 0,
/*8D                        */ 0,
/*8E                        */ 0,
/*8F                        */ 0,
/*90 VK_NUMLOCK             */ DIK_NUMLOCK,
/*91 VK_SCROLL              */ DIK_SCROLL,
/*92 VK_OEM_FJ_JISHO        */ 0,
/*93 VK_OEM_FJ_MASSHOU      */ 0,
/*94 VK_OEM_FJ_TOUROKU      */ 0,
/*95 VK_OEM_FJ_LOYA         */ 0,
/*96 VK_OEM_FJ_ROYA         */ 0,
/*97                        */ 0,
/*98                        */ 0,
/*99                        */ 0,
/*9A                        */ 0,
/*9B                        */ 0,
/*9C                        */ 0,
/*9D                        */ 0,
/*9E                        */ 0,
/*9F                        */ 0,
/*A0 VK_LSHIFT              */ DIK_LSHIFT,
/*A1 VK_RSHIFT              */ DIK_RSHIFT,
/*A2 VK_LCONTROL            */ DIK_LCONTROL,
/*A3 VK_RCONTROL            */ DIK_RCONTROL,
/*A4 VK_LMENU               */ DIK_LMENU,
/*A5 VK_RMENU               */ DIK_RMENU,
/*A6 K_BROWSER_BACK         */ DIK_WEBBACK,
/*A7 K_BROWSER_FORWARD      */ DIK_WEBFORWARD,
/*A8 K_BROWSER_REFRESH      */ DIK_WEBREFRESH,
/*A9 K_BROWSER_STOP         */ DIK_WEBSTOP,
/*AA K_BROWSER_SEARCH       */ DIK_WEBSEARCH,
/*AB,K_BROWSER_FAVORITES    */ DIK_WEBFAVORITES,
/*AC K_BROWSER_HOME         */ DIK_WEBHOME,
/*AD VK_VOLUME_MUTE         */ DIK_MUTE,
/*AE VK_VOLUME_DOWN         */ DIK_VOLUMEDOWN,
/*AF VK_VOLUME_UP           */ DIK_VOLUMEUP,
/*B0 VK_MEDIA_NEXT_TRACK    */ DIK_NEXTTRACK,
/*B1 VK_MEDIA_PREV_TRACK    */ DIK_PREVTRACK,
/*B2 VK_MEDIA_STOP          */ DIK_MEDIASTOP,
/*B3 VK_MEDIA_PLAY_PAUSE    */ DIK_PLAYPAUSE,
/*B4 VK_LAUNCH_MAIL         */ DIK_MAIL,
/*B5 VK_LAUNCH_MEDIA_SELECT */ DIK_MEDIASELECT,
/*B6 VK_LAUNCH_APP1         */ 0,
/*B7 VK_LAUNCH_APP2         */ 0,
/*B8                        */ 0,
/*B9                        */ 0,
/*BA VK_OEM_1               */ DIK_SEMICOLON,
/*BB VK_OEM_PLUS            */ DIK_EQUALS,
/*BC VK_OEM_COMMA           */ DIK_COMMA,
/*BD VK_OEM_MINUS           */ DIK_MINUS,
/*BE VK_OEM_PERIOD          */ DIK_PERIOD,
/*BF VK_OEM_2               */ DIK_SLASH,
/*C0 VK_OEM_3               */ DIK_GRAVE,
/*C1                        */ 0,
/*C2                        */ 0,
/*C3                        */ 0,
/*C4                        */ 0,
/*C5                        */ 0,
/*C6                        */ 0,
/*C7                        */ 0,
/*C8                        */ 0,
/*C9                        */ 0,
/*CA                        */ 0,
/*CB                        */ 0,
/*CC                        */ 0,
/*CD                        */ 0,
/*CE                        */ 0,
/*CF                        */ 0,
/*D0                        */ 0,
/*D1                        */ 0,
/*D2                        */ 0,
/*D3                        */ 0,
/*D4                        */ 0,
/*D5                        */ 0,
/*D6                        */ 0,
/*D7                        */ 0,
/*D8                        */ 0,
/*D9                        */ 0,
/*DA                        */ 0,
/*DB VK_OEM_4               */ DIK_LBRACKET,
/*DC VK_OEM_5               */ 0,
/*DD VK_OEM_6               */ DIK_RBRACKET,
/*DE VK_OEM_7               */ DIK_APOSTROPHE,
/*DF VK_OEM_8               */ 0,
/*E0                        */ 0,
/*E1 VK_OEM_AX              */ 0,
/*E2 VK_OEM_102             */ 0,
/*E3 VK_ICO_HELP            */ 0,
/*E4 VK_ICO_00              */ 0,
/*E5 VK_PROCESSKEY          */ 0,
/*E6 VK_ICO_CLEAR           */ 0,
/*E7 VK_PACKET              */ 0,
/*E8                        */ 0,
/*E9 VK_OEM_RESET           */ 0,
/*EA VK_OEM_JUMP            */ 0,
/*EB VK_OEM_PA1             */ 0,
/*EC VK_OEM_PA2             */ 0,
/*ED VK_OEM_PA3             */ 0,
/*EE VK_OEM_WSCTRL          */ 0,
/*EF VK_OEM_CUSEL           */ 0,
/*F0 VK_OEM_ATTN            */ 0,
/*F1 VK_OEM_FINISH          */ 0,
/*F2 VK_OEM_COPY            */ 0,
/*F3 VK_OEM_AUTO            */ 0,
/*F4 VK_OEM_ENLW            */ 0,
/*F5 VK_OEM_BACKTAB         */ 0,
/*F6 VK_ATTN                */ 0,
/*F7 VK_CRSEL               */ 0,
/*F8 VK_EXSEL               */ 0,
/*F9 VK_EREOF               */ 0,
/*FA VK_PLAY                */ 0,
/*FB VK_ZOOM                */ 0,
/*FC VK_NONAME              */ 0,
/*FD VK_PA1                 */ 0,
/*FE VK_OEM_CLEAR           */ 0,
/*FF                        */ 0
};

const char_t* Tkeyboard::getKeyName(int key)
{
 return keyInfo[key&255].keyname;
}

//=============================== Tmouse ===============================
Tmouse::Tmouse(TintStrColl *Icoll,IffdshowBase *Ideci):TdirectInput(Icoll,_l("mouse"),GUID_SysMouse,c_dfDIMouse,&state,&prevstate,sizeof(state),Ideci)
{
 static const TintOptionT<Tmouse> iopts[]=
  {
   IDFF_isMouse     ,&Tmouse::is      ,0,0,_l(""),0,
     _l("mouse"),0,
   IDFF_mouseAlways ,&Tmouse::always  ,0,0,_l(""),0,
     _l("mouseAlways"),0,
   0
  };
 addOptions(iopts);
 setOnChange(IDFF_isMouse,this,&Tmouse::onChange);

 load();
}
Tmouse::~Tmouse()
{
 unhook();
}
void Tmouse::processState(void)
{
 if (!state.lX && !state.lY && !state.lZ) return;
 if (!always && !windowActive())
  return;
 DPRINTF(_l("%i %i %i  %i %i %i %i"),state.lX,state.lY,state.lZ,(int)state.rgbButtons[0],(int)state.rgbButtons[1],(int)state.rgbButtons[2],(int)state.rgbButtons[3]);
}
