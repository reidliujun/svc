#ifndef _FFDSHOWREMOTEAPIIMPL_H_
#define _FFDSHOWREMOTEAPIIMPL_H_

#include "Toptions.h"
#include "ffdshowRemoteAPI.h"
#include "interfaces.h"

class Tkeyboard;
class Tremote : public Toptions
{
private:
 int is,messageMode,messageUser,acceptKeys;
 IffdshowBase *deci;comptrQ<IffdshowDec> deciD;comptrQ<IffdshowDecVideo> deciV;
 UINT remotemsg;
 static unsigned int __stdcall threadProc(void*);
 static unsigned int __stdcall ffwdThreadProc(void*); // Thread method for fast forward/rewind
 HWND h;
 LRESULT CALLBACK remoteWndProc(HWND wnd, UINT msg, WPARAM wprm, LPARAM lprm);
 static LRESULT CALLBACK remoteWndProc0(HWND hwnd, UINT msg, WPARAM wprm, LPARAM lprm);
 int paramid;
 HANDLE hThread;
 HANDLE fThread; // Thread used to perform fast forward/rewind
 HANDLE fEvent; // Manual reset event for terminating the thread
 int fSeconds; // Step in seconds to perform fast forward/rewind
 int fMode; // Fast forward/rewind mode
 Tkeyboard *keys;
 unsigned int subtitleIdx;
 void start(void),stop(void);
 bool inExplorer;
 DWORD pdwROT;
 int OSDPositionX, OSDPositionY;
 std::vector<std::pair<long, ffstring> > AudioStreamsNames, AudioStreamsLanguageNames,
	 SubtitleStreamsNames, SubtitleStreamsLanguageNames;
 long currentSubtitleStream, currentAudioStream;
 bool streamsLoaded;
 bool foundHaali;
public:
 Tremote(TintStrColl *Icoll,IffdshowBase *Ideci);
 ~Tremote();
 void load(void),save(void);
 void onChange(int id,int val);
 void getStreams(bool reload);
 void setStream(long streamNb);
};

#endif
