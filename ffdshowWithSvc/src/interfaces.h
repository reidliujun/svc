#ifndef _INTERFACES_H_
#define _INTERFACES_H_

#include "char_t.h"

struct IffdshowBaseA;
struct IffdshowBaseW;
struct IffdshowDecA;
struct IffdshowDecW;
struct IffdshowDecVideoA;
struct IffdshowDecVideoW;
struct IffdshowDecAudioA;
struct IffdshowDecAudioW;

template<class tchar> struct interfaces;

template<> struct interfaces<char>
{
 typedef IffdshowBaseA IffdshowBase;
 typedef IffdshowDecA IffdshowDec;
 typedef IffdshowDecVideoA IffdshowDecVideo;
 typedef IffdshowDecAudioA IffdshowDecAudio;
};

template<> struct interfaces<wchar_t>
{
 typedef IffdshowBaseW IffdshowBase;
 typedef IffdshowDecW IffdshowDec;
 typedef IffdshowDecVideoW IffdshowDecVideo;
 typedef IffdshowDecAudioW IffdshowDecAudio;
};

typedef interfaces<char_t>::IffdshowBase IffdshowBase;
typedef interfaces<char_t>::IffdshowDec IffdshowDec;
typedef interfaces<char_t>::IffdshowDecVideo IffdshowDecVideo;
typedef interfaces<char_t>::IffdshowDecAudio IffdshowDecAudio;

#endif
