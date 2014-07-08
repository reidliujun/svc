/*
 * Copyright (c) 2003-2006 Milan Cutka
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
#include "ffdshowRemoteAPIimpl.h"
#include "IffdshowBase.h"
#include "IffdshowDec.h"
#include "IffdshowDecVideo.h"
#include "TkeyboardDirect.h"
#include "TsubtitlesFile.h"
#include "reg.h"
#include "Tpresets.h"
#include "TpresetSettings.h"
#include "TffDecoderVideo.h"
#include "dsutil.h"
#include "strmif.h"
#include "ffdebug.h"

Tremote::Tremote(TintStrColl *Icoll,IffdshowBase *Ideci):deci(Ideci),Toptions(Icoll)
{
 static const TintOptionT<Tremote> iopts[]=
  {
   IDFF_isRemote           ,&Tremote::is           ,0,0,_l(""),0,
     _l("isRemote"),0,
   IDFF_remoteMessageMode  ,&Tremote::messageMode  ,0,1,_l(""),0,
     _l("remoteMessageMode"),0,
   IDFF_remoteMessageUser  ,&Tremote::messageUser  ,1,1,_l(""),0,
     _l("remoteMessageUser"),WM_APP+18,
   IDFF_remoteAcceptKeys   ,&Tremote::acceptKeys   ,1,1,_l(""),0,
     _l("remoteAcceptKeys"),1,
   0
  };
 addOptions(iopts);
 setOnChange(IDFF_isRemote,this,&Tremote::onChange);
 load();

 h=NULL;
 pdwROT = 0; // Initializes the running object table reference
 keys=NULL;hThread=NULL;
 fThread=NULL;
 fEvent=NULL;
 fMode=1; // Default mode : fast forward
 fSeconds=10; // Default step : 10 seconds
 inExplorer=deci->inExplorer()==S_OK;
 OSDPositionX=0; // Horizontal position of the DrawOSD (default : Left of screen)
 OSDPositionY=10; // Vertical position of the DrawOSD (default : Top + 10px)
 currentSubtitleStream=0, currentAudioStream=0;
 streamsLoaded=false;
 foundHaali=false;
}
Tremote::~Tremote()
{
 stop();
 if (keys) delete keys;
}

void Tremote::load(void)
{
 TregOpRegRead t(HKEY_CURRENT_USER,FFDSHOW_REG_PARENT _l("\\") FFDSHOWDECVIDEO);
 reg_op(t);
}
void Tremote::save(void)
{
 TregOpRegWrite t(HKEY_CURRENT_USER,FFDSHOW_REG_PARENT _l("\\") FFDSHOWDECVIDEO);
 reg_op(t);
}

void Tremote::onChange(int id,int val)
{
 int filtermode;
 if (is && !inExplorer && ((filtermode=deci->getParam2(IDFF_filterMode))&IDFF_FILTERMODE_PLAYER) && !(filtermode&IDFF_FILTERMODE_VFW))
  start();
 else
  stop();
}

void Tremote::start(void)
{
 if (hThread) return;
 deciD=deci;
 deciV=deci;
 remotemsg=messageMode==0?RegisterWindowMessage(_l(FFDSHOW_REMOTE_MESSAGE)):messageUser;
 paramid=0;subtitleIdx=0;
 unsigned threadID;
 hThread=(HANDLE)_beginthreadex(NULL,65536,threadProc,this,NULL,&threadID);
}
void Tremote::stop(void)
{
if (fThread)
{
	SetEvent(fEvent);
	WaitForSingleObject(fThread, 3000);
	CloseHandle(fEvent);
	CloseHandle(fThread);
	fThread = NULL; fEvent = NULL;
 }

if (h)
  {
   SendMessage(h,WM_CLOSE,0,0);
   WaitForSingleObject(hThread,INFINITE);
   hThread=NULL;
  }
 deciD=NULL;deciV=NULL;
}
unsigned int __stdcall Tremote::threadProc(void *self0)
{
 randomize();
 setThreadName(DWORD(-1),"remote");

 Tremote *self=(Tremote*)self0;
 HINSTANCE hi=self->deci->getInstance2();
 char_t windowName[80];
 tsnprintf_s(windowName, countof(windowName), _TRUNCATE, _l("%s_window%i"), FFDSHOW_REMOTE_CLASS, rand());
 ATOM at=NULL;
 self->h=createInvisibleWindow(hi,_l(FFDSHOW_REMOTE_CLASS),windowName,remoteWndProc0,self,&at);
 if (self->h)
  {
   SetWindowLongPtr(self->h,GWLP_USERDATA,LONG_PTR(self));
   MSG msg;
   while(GetMessage(&msg, NULL, 0, 0))
    {
     TranslateMessage(&msg);
     DispatchMessage(&msg);
    }
  }
 UnregisterClass(_l(FFDSHOW_REMOTE_CLASS),hi);
 if (self->pdwROT)
 {
	comptr<IRunningObjectTable> pROT;
	if (SUCCEEDED(GetRunningObjectTable(0,&pROT)))
		pROT->Revoke(self->pdwROT);
	self->pdwROT=0;
 }
 self->h=NULL;
 _endthreadex(0);
 return 0;
}

unsigned int __stdcall Tremote::ffwdThreadProc(void *self0)
{
 randomize();
 setThreadName(DWORD(-1),"remote fast forward");
 Tremote *self=(Tremote*)self0;
 HANDLE fEvent=self->fEvent;
 IFilterGraph    *m_pGraph = NULL;
 self->deci->getGraph(&m_pGraph); // Graph we belong to
 comptrQ<IMediaControl> _pMC=m_pGraph;
 if (self->deci != NULL && _pMC != NULL)
 {
	_pMC->Run();
	int seconds = self->fSeconds;
	int mode = self->fMode;
	seconds *= mode;
	seconds /= 5;
	if (seconds == 0)
		seconds = 1;
	int pos;
	int duration = self->deci->getParam2(IDFF_movieDuration);
	int hh, mm, ss;
	char_t msg[100];
	char_t duration_str[30];
	hh = duration/3600;
	mm = (duration - hh*3600)/60;
	ss = duration - hh*3600 - mm*60;
	tsprintf(duration_str,_l("%02i:%02i:%02i"), hh, mm, ss);

	self->deci->tell(&pos);
	DWORD currentTime, elapsedTime;
	if (pos!=-1 && duration >0)
	 while(WaitForSingleObject(fEvent, 0) != WAIT_OBJECT_0)
	 {
		currentTime = GetTickCount();
		pos+=seconds;
		if (pos<0 || pos >= duration)// || self->deci->getState2()!=State_Running)
			break;
		if (!SUCCEEDED(self->deci->seek(pos)))
			break;
		hh = pos/3600;
		mm = (pos - hh*3600)/60;
		ss = pos - hh*3600 - mm*60;
		tsnprintf_s(msg, countof(msg), _TRUNCATE, _l("%s %02i:%02i:%02i / %s"),mode<0?_l("<<"):_l(">>"), hh, mm, ss, duration_str);
		self->deciV->resetOSD();
		self->deciV->drawOSD(self->OSDPositionX, self->OSDPositionY, msg);
		//self->deciV->shortOSDmessage(msg,30);
		elapsedTime = GetTickCount() - currentTime;
		if (elapsedTime < 200 && elapsedTime > 0)
			Sleep(200 - elapsedTime);
		else
			Sleep(100);
	 }
	 self->deciV->drawOSD(self->OSDPositionX, self->OSDPositionY, _l(""));
 }

 //self->fThread=NULL;
 /* Albain FIX : when using _endthreadex in a subthread created inside a thread then the class is not
    unloaded */
 //_endthreadex(0);
 return 0;
}

LRESULT CALLBACK Tremote::remoteWndProc0(HWND hwnd, UINT msg, WPARAM wprm, LPARAM lprm)
{
 Tremote *self=(Tremote*)GetWindowLongPtr(hwnd,GWLP_USERDATA);
 //DPRINTF("got remote message %i",msg);
 switch (msg)
  {
   case WM_DESTROY:
    PostQuitMessage(0);
    break;
  }
 return self?self->remoteWndProc(hwnd,msg,wprm,lprm):DefWindowProc(hwnd,msg,wprm,lprm);
}

LRESULT CALLBACK Tremote::remoteWndProc(HWND hwnd, UINT msg, WPARAM wprm, LPARAM lprm)
{
 if (msg==remotemsg)
  switch (wprm)
   {
    case WPRM_SETPARAM_ID:
     paramid=(int)lprm;
     return TRUE;
    case WPRM_GETPARAM:
     return deci->getParam2(paramid);
    case WPRM_GETPARAM2:
     return deci->getParam2((int)lprm);
    case WPRM_PUTPARAM:
     return SUCCEEDED(deci->putParam(paramid,(int)lprm))?TRUE:FALSE;
    case WPRM_STOP:
     return SUCCEEDED(deciD->stop())?TRUE:FALSE;
    case WPRM_RUN:
     return SUCCEEDED(deciD->run())?TRUE:FALSE;
    case WPRM_GETSTATE:
		if (fThread)
			return 3;
		else
			return deciD->getState2();
    case WPRM_GETDURATION:
     return deci->getParam2(IDFF_movieDuration);
    case WPRM_GETCURTIME:
     return deciD->getCurTime2();
    case WPRM_PREVPRESET:
     return SUCCEEDED(deciD->cyclePresets(-1))?TRUE:FALSE;
    case WPRM_NEXTPRESET:
     return SUCCEEDED(deciD->cyclePresets(+1))?TRUE:FALSE;
	case WPRM_SETCURTIME:
     return SUCCEEDED(deciD->seek((int)lprm))?TRUE:FALSE;
	case WPRM_SETADDTOROT:
		if ((int)lprm == 1) // 1 = Register to running object table (ROT) 0 = unregister
		{
			if (!pdwROT)
			{
				comptr<IRunningObjectTable> pROT;
				if (SUCCEEDED(GetRunningObjectTable(0,&pROT)))
				{
					IFilterGraph *pGraph = NULL;
					deci->getGraph(&pGraph);
					WCHAR entryName[256];
					wsprintfW(entryName, L"FilterGraph %08p pid %08x (ffdshow)", (DWORD_PTR)pGraph,GetCurrentProcessId());
					comptr<IMoniker> pMoniker;
					if (SUCCEEDED(CreateItemMoniker(L"!",entryName,&pMoniker)))
					{
						pROT->Register(ROTFLAGS_REGISTRATIONKEEPSALIVE && ROTFLAGS_ALLOWANYCLIENT,(IUnknown*)pGraph,pMoniker,&pdwROT);
						//pMoniker->Release();
						return TRUE;
					}
					pROT->Release();
				}
			}
			return FALSE;
		}
		else
		{
			if (pdwROT)
			{
				comptr<IRunningObjectTable> pROT;
				if (SUCCEEDED(GetRunningObjectTable(0,&pROT)))
				{
					pROT->Revoke(pdwROT);
					pROT->Release();
				}
				pdwROT=0;
				return TRUE;
			}
			return FALSE;
		}
	case WPRM_FASTFORWARD:
	case WPRM_FASTREWIND:
		fMode=(wprm==WPRM_FASTFORWARD)?1:-1;
		fSeconds = (int) lprm; // Update the step size in seconds
		if (fThread != NULL)
		{
			SetEvent(fEvent);
			WaitForSingleObject(fThread, 3000);

			CloseHandle(fEvent);
			CloseHandle(fThread);
			fThread = NULL; fEvent = NULL;
			deciV->drawOSD(OSDPositionX, OSDPositionY, _l(""));
		}
		if (fSeconds != 0)
		{
			unsigned threadID;
			// Create a manual-reset nonsignaled unnamed event
			fEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
			fThread=(HANDLE)_beginthreadex(NULL,65536,ffwdThreadProc,this,NULL,&threadID);
		}
		return TRUE;
	case WPRM_GET_FASTFORWARD:
		if (fThread == NULL)
			return 0;
		else
			return fSeconds*fMode;
	case WPRM_CAPTUREIMAGE:
		if (deciV != NULL)
		{
			deciV->grabNow();
			return TRUE;
		}
		return FALSE;
	case WPRM_SET_OSDX:
		OSDPositionX=(int)lprm;
		return TRUE;
	case WPRM_SET_OSDY:
		OSDPositionY=(int)lprm;
		return TRUE;
	case WPRM_SET_STREAM:
		getStreams(false);
		setStream((long)lprm);
		return TRUE;
	case WPRM_GET_CURRENT_AUDIO_STREAM:
		getStreams(true);
		return currentAudioStream;
	case WPRM_GET_CURRENT_SUBTITLE_STREAM:
		getStreams(true);
		return currentSubtitleStream;
	case WPRM_GET_FRAMERATE:
		if (deciV != NULL)
		{
			unsigned int fps1000 = 0;
			deciV->getAVIfps(&fps1000);
			return fps1000;
		}
		return FALSE;
 }

 switch(msg)
 {
	 case MSG_GETPARAMSTR:
		{
			COPYDATASTRUCT cd;
			cd.dwData = (int)lprm;
			const char_t *paramStr = deci->getParamStr2((unsigned int)lprm);
			if (paramStr == NULL)
				return FALSE;
			cd.lpData = alloca(sizeof(char_t)*(strlen(paramStr)+1));
			text<char_t>(paramStr, (char_t*)cd.lpData);
			cd.cbData = sizeof(char_t)*(strlen(paramStr)+1);
			DWORD_PTR ret = 0;
			SendMessageTimeout((HWND)wprm, WM_COPYDATA, lprm, (LPARAM)&cd, 
				SMTO_ABORTIFHUNG, 1500, &ret);
			return TRUE;
		}
	case MSG_GET_CURRENT_SUBTITLES:
		{
			COPYDATASTRUCT cd;
			cd.dwData = (int)MSG_GET_CURRENT_SUBTITLES;
			if (!deciV) return FALSE;
			const char_t *paramStr = deciV->getCurrentSubFlnm();
			if (paramStr == NULL)
				return FALSE;
			cd.lpData = alloca(sizeof(char_t)*(strlen(paramStr)+1));
			text<char_t>(paramStr, (char_t*)cd.lpData);
			cd.cbData = sizeof(char_t)*(strlen(paramStr)+1);
			DWORD_PTR ret = 0;
			SendMessageTimeout((HWND)wprm, WM_COPYDATA, MSG_GET_CURRENT_SUBTITLES, (LPARAM)&cd, 
				SMTO_ABORTIFHUNG, 1500, &ret);
			return TRUE;
		}
	case MSG_GET_PRESETLIST:
		{
			COPYDATASTRUCT cd;
			cd.dwData = MSG_GET_PRESETLIST;
			Tpresets *presets;
			deciD->getPresetsPtr(&presets);
			int presetsNum = presets->size();
			size_t string_size = 2048;
			char_t *presetList = (char_t*)alloca(sizeof(char_t)*string_size);
			strcpy(presetList, _l(""));
			for (int i=0; i<presetsNum; i++)
			{
				Tpreset *preset = presets->at(i);
				const char_t *presetName = preset->presetName;
				// Resize the string if needed
				if (strlen(presetList)+strlen(presetName)+ 10 >= string_size)
				{
					string_size += 2048;
					char_t *tmpStr = (char_t*)alloca(sizeof(char_t)*string_size);
					strcpy(tmpStr, presetList);
					presetList = tmpStr;
				}
				strcat(presetList, presetName);
				if (i != presetsNum - 1)
					strcat(presetList, _l(";"));
			}
			cd.lpData = alloca(sizeof(char_t)*(strlen(presetList)+1));
			text<char_t>(presetList, (char_t*)cd.lpData);
			cd.cbData = sizeof(char_t)*(strlen(presetList)+1);
			DWORD_PTR ret = 0;
			SendMessageTimeout((HWND)wprm, WM_COPYDATA, MSG_GET_PRESETLIST, (LPARAM)&cd, 
				SMTO_ABORTIFHUNG, 1500, &ret);
			return TRUE;
		}
	case MSG_GET_SOURCEFILE:
	  {
		COPYDATASTRUCT cd;
		cd.dwData = MSG_GET_SOURCEFILE;
		const char_t *fileName = deci->getSourceName();
		cd.lpData = alloca(sizeof(char_t)*(strlen(fileName)+1));
		text<char_t>(fileName, (char_t*)cd.lpData);
		cd.cbData = sizeof(char_t)*(strlen(fileName)+1);
		DWORD_PTR ret = 0;
		SendMessageTimeout((HWND)wprm, WM_COPYDATA, MSG_GET_SOURCEFILE, (LPARAM)&cd, 
			SMTO_ABORTIFHUNG, 2000, &ret);
		return TRUE;
	  }
	case MSG_GET_SUBTITLEFILESLIST:
	  {
		COPYDATASTRUCT cd;
		cd.dwData = MSG_GET_SUBTITLEFILESLIST;
		if (!deciV) return FALSE;
        strings files;
        TsubtitlesFile::findPossibleSubtitles(deci->getSourceName(),deci->getParamStr2(IDFF_subSearchDir),files);
        if (files.size() == 0)
		{
			return FALSE;
		}
		else
        {
			size_t string_size = 2048;
			char_t *filesList = (char_t*)alloca(sizeof(char_t)*string_size);
			text<char_t>(_l(""), filesList);
			for (UINT i=0; i<files.size(); i++)
			{
				const char_t *fileName = files[i].c_str();
				// Resize the string if needed
				if (strlen(filesList)+strlen(fileName)+ 10 >= string_size)
				{
					string_size += 2048;
					char_t *tmpStr = (char_t*)alloca(sizeof(char_t)*string_size);
					strcpy(tmpStr, filesList);
					filesList = tmpStr;
				}
				strcat(filesList, fileName);
				if (i != files.size() - 1)
					strcat(filesList, _l(";"));
			}
			cd.lpData = alloca(sizeof(char_t)*(strlen(filesList)+1));
			text<char_t>(filesList, (char_t*)cd.lpData);
			cd.cbData = sizeof(char_t)*(strlen(filesList)+1);
			DWORD_PTR ret = 0;
			SendMessageTimeout((HWND)wprm, WM_COPYDATA, MSG_GET_SUBTITLEFILESLIST, (LPARAM)&cd, 
				SMTO_ABORTIFHUNG, 1500, &ret);
			//SendMessage((HWND)wprm, WM_COPYDATA, MSG_GET_SUBTITLEFILESLIST, (LPARAM)&cd);
        }
		return TRUE;
	  }
		case MSG_GET_CHAPTERSLIST:
		{
			COPYDATASTRUCT cd;
			cd.dwData = MSG_GET_CHAPTERSLIST;
			size_t string_size = 2048;
			char_t *stringList = (char_t*)alloca(sizeof(char_t)*string_size);
			strcpy(stringList, _l(""));
			std::vector<std::pair<long, ffstring> > *pChaptersList = NULL;
			deciV->getChaptersList((void**)&pChaptersList);
			
			for (long l = 0; l<(long) pChaptersList->size(); l++)
			{
				long markerTime = (*pChaptersList)[l].first;
				char_t tmpStr[40];
				wsprintf(tmpStr, _l("%ld"), markerTime);
				ffstring chapterString = _l("<chapter><time>") + ffstring(tmpStr)+_l("</time><name>")+ffstring((*pChaptersList)[l].second)
					+_l("</name></chapter>");

				// Resize the string if needed
				if (strlen(stringList)+strlen(chapterString.c_str())+ 10 >= string_size)
				{
					string_size += 2048;
					char_t *tmpStr = (char_t*)alloca(sizeof(char_t)*string_size);
					strcpy(tmpStr, stringList);
					stringList = tmpStr;
				}
				strcat(stringList, chapterString.c_str());
			}
			cd.lpData = alloca(sizeof(char_t)*(strlen(stringList)+1));
			text<char_t>(stringList, (char_t*)cd.lpData);
			cd.cbData = sizeof(char_t)*(strlen(stringList)+1);
			DWORD_PTR ret = 0;
			SendMessageTimeout((HWND)wprm, WM_COPYDATA, MSG_GET_CHAPTERSLIST, (LPARAM)&cd, 
				SMTO_ABORTIFHUNG, 1500, &ret);
			return TRUE;
		}
		case MSG_GET_AUDIOSTREAMSLIST:
		{
			COPYDATASTRUCT cd;
			cd.dwData = MSG_GET_AUDIOSTREAMSLIST;
			size_t string_size = 2048;
			char_t *stringList = (char_t*)alloca(sizeof(char_t)*string_size);
			strcpy(stringList, _l(""));
			getStreams(false);
			
			for (long l = 0; l<(long)AudioStreamsNames.size(); l++)
			{
				long streamNb = AudioStreamsNames[l].first;
				char_t tmpStr[40];
				tsnprintf_s(tmpStr, countof(tmpStr), _TRUNCATE, _l("%ld"), streamNb);
				ffstring xmlString = _l("<stream><id>") + ffstring(tmpStr)+_l("</id><name>")+ffstring(AudioStreamsNames[l].second)
					+_l("</name><language_name>")+ffstring(AudioStreamsLanguageNames[l].second)+_l("</language_name></stream>");

				// Resize the string if needed
				if (strlen(stringList)+strlen(xmlString.c_str())+ 10 >= string_size)
				{
					string_size += 2048;
					char_t *tmpStr = (char_t*)alloca(sizeof(char_t)*string_size);
					strcpy(tmpStr, stringList);
					stringList = tmpStr;
				}
				strcat(stringList, xmlString.c_str());
			}
			cd.lpData = alloca(sizeof(char_t)*(strlen(stringList)+1));
			text<char_t>(stringList, (char_t*)cd.lpData);
			cd.cbData = sizeof(char_t)*(strlen(stringList)+1);
			DWORD_PTR ret = 0;
			DPRINTF(_l("Sending : %s"), stringList);
			SendMessageTimeout((HWND)wprm, WM_COPYDATA, MSG_GET_AUDIOSTREAMSLIST, (LPARAM)&cd, 
				SMTO_ABORTIFHUNG, 1500, &ret);
			return TRUE;
		}
		case MSG_GET_SUBTITLESTREAMSLIST:
		{
			COPYDATASTRUCT cd;
			cd.dwData = MSG_GET_SUBTITLESTREAMSLIST;
			size_t string_size = 2048;
			char_t *stringList = (char_t*)alloca(sizeof(char_t)*string_size);
			strcpy(stringList, _l(""));
			getStreams(false);
			
			for (long l = 0; l<(long)SubtitleStreamsNames.size(); l++)
			{
				long streamNb = SubtitleStreamsNames[l].first;
				char_t tmpStr[40];
				tsnprintf_s(tmpStr, countof(tmpStr), _TRUNCATE, _l("%ld"), streamNb);
				ffstring xmlString = _l("<stream><id>") + ffstring(tmpStr)+_l("</id><name>")+ffstring(SubtitleStreamsNames[l].second)
					+_l("</name><language_name>")+ffstring(SubtitleStreamsLanguageNames[l].second)+_l("</language_name></stream>");

				// Resize the string if needed
				if (strlen(stringList)+strlen(xmlString.c_str())+ 10 >= string_size)
				{
					string_size += 2048;
					char_t *tmpStr = (char_t*)alloca(sizeof(char_t)*string_size);
					strcpy(tmpStr, stringList);
					stringList = tmpStr;
				}
				strcat(stringList, xmlString.c_str());
			}
			cd.lpData = alloca(sizeof(char_t)*(strlen(stringList)+1));
			text<char_t>(stringList, (char_t*)cd.lpData);
			cd.cbData = sizeof(char_t)*(strlen(stringList)+1);
			DWORD_PTR ret = 0;
			DPRINTF(_l("Sending : %s"), stringList);
			SendMessageTimeout((HWND)wprm, WM_COPYDATA, MSG_GET_SUBTITLESTREAMSLIST, (LPARAM)&cd, 
				SMTO_ABORTIFHUNG, 1500, &ret);
			return TRUE;
		}
 }

 if (acceptKeys && (msg==WM_SYSKEYDOWN || msg==WM_SYSKEYUP || msg==WM_KEYDOWN || msg==WM_KEYUP))
  {
   if (!keys)
    {
     keys=new Tkeyboard(new TintStrColl,deci);
     keys->load();
    }
   switch (msg)
    {
     case WM_SYSKEYDOWN:
     case WM_KEYDOWN:
      keys->keyDown((int)wprm);
      break;
     case WM_SYSKEYUP:
     case WM_KEYUP:
      keys->keyUp((int)wprm);
      break;
    }
  }
 else if (msg==WM_COPYDATA)
  {
   const COPYDATASTRUCT *cds=(const COPYDATASTRUCT*)lprm;
   switch (cds->dwData)
    {
     case COPY_PUTPARAMSTR:
      return SUCCEEDED(deci->putParamStr(paramid,text<char_t>((const char_t*)cds->lpData)))?TRUE:FALSE;
     case COPY_GETPARAMSTR:
      {
       if (!cds->cbData) return false;
       char_t *buft=(char_t*)alloca(cds->cbData*sizeof(char_t));
       if (SUCCEEDED(deci->getParamStr(paramid,buft,cds->cbData)))
        {
         text<char_t>(buft, cds->cbData, (char_t*)cds->lpData, cds->cbData);
		 ((COPYDATASTRUCT*)lprm)->dwData = paramid;
		 SendMessage((HWND)wprm, WM_COPYDATA, paramid, lprm);
         return TRUE;
        }
       return FALSE;
       //return cds->cbData && SUCCEEDED(deci->getParamStr(paramid,(char*)cds->lpData,cds->cbData))?TRUE:FALSE;
      }
     case COPY_SETACTIVEPRESET:
      return SUCCEEDED(deciD->setActivePreset((const char_t*)cds->lpData,false))?TRUE:FALSE;
     case COPY_AVAILABLESUBTITLE_FIRST:
      subtitleIdx=0;
     case COPY_AVAILABLESUBTITLE_NEXT:
      {
       if (!deciV || cds->cbData==0) return FALSE;
       strings files;
       TsubtitlesFile::findPossibleSubtitles(deci->getSourceName(),deci->getParamStr2(IDFF_subSearchDir),files);
       if (subtitleIdx>=files.size())
        ((char_t*)cds->lpData)[0]='\0';
       else
        {
         ff_strncpy((char_t*)cds->lpData, (const char_t *)text<char_t>(files[subtitleIdx].c_str()),cds->cbData);
         ((char_t*)cds->lpData)[cds->cbData-1]='\0';
         subtitleIdx++;
        }
	   SendMessage((HWND)wprm, WM_COPYDATA, paramid, lprm);
       return TRUE;
      }
	  case COPY_SET_SHORTOSD_MSG:
		  return SUCCEEDED(deciV->shortOSDmessage(text<char_t>((const char_t*)cds->lpData), 25))?TRUE:FALSE;
	  case COPY_SET_OSD_MSG:
		  deciV->resetOSD();
		  return SUCCEEDED(deciV->drawOSD(OSDPositionX, OSDPositionY, text<char_t>((const char_t*)cds->lpData)))?TRUE:FALSE;
    }
  }
 return DefWindowProc(hwnd,msg,wprm,lprm);
}


void Tremote::getStreams(bool reload)
{
	if (streamsLoaded && !reload)
		return;
	
	AudioStreamsNames.clear();
	AudioStreamsLanguageNames.clear();
	SubtitleStreamsNames.clear();
	SubtitleStreamsLanguageNames.clear();
	currentAudioStream = 0;
	currentSubtitleStream = 0;

	comptr<IEnumFilters> eff;
	IFilterGraph    *m_pGraph = NULL;
	deci->getGraph(&m_pGraph); // Graph we belong to
	if (m_pGraph->EnumFilters(&eff)==S_OK)
	{
		eff->Reset();
		const char_t *fileName = deci->getSourceName();
		for (comptr<IBaseFilter> bff;eff->Next(1,&bff,NULL)==S_OK;bff=NULL)
		{
			char_t name[MAX_PATH],filtername[MAX_PATH];
			getFilterName(bff,name,filtername,countof(filtername));
			FILTER_INFO filterinfo;
			bff->QueryFilterInfo(&filterinfo);
			if (filterinfo.pGraph)
				filterinfo.pGraph->Release();

			/*if (!strcmp(_l("ffdshow video decoder"), filtername)
				|| !strcmp(_l("ffdshow raw video filter"), filtername)
				|| !strcmp(_l("ffdshow VFW decoder helper"), filtername)
				|| !strcmp(_l("ffdshow subtitles filter"), filtername)
				|| !strcmp(_l("ffdshow Audio Decoder"), filtername))
				continue;*/
			IAMStreamSelect *pAMStreamSelect = NULL;
			bff->QueryInterface(IID_IAMStreamSelect, (void**) &pAMStreamSelect);
			if (pAMStreamSelect == NULL)
				continue;
			
			foundHaali = false;

			// Haali splitter
			if (!strcmp(filtername, fileName))
			{
				AudioStreamsNames.clear();
				AudioStreamsLanguageNames.clear();
				SubtitleStreamsNames.clear();
				SubtitleStreamsLanguageNames.clear();
				currentAudioStream = 0;
				currentSubtitleStream = 0;
				foundHaali = true;
			}

			DWORD cStreams = 0;
			pAMStreamSelect->Count(&cStreams);
			for (long streamNb=0; streamNb<(long)cStreams; streamNb++)
			{
				DWORD streamSelect = 0;
				LCID streamLanguageId = 0;
				DWORD streamGroup = 0;
				WCHAR *pstreamName = NULL;
				HRESULT hr = pAMStreamSelect->Info(streamNb, NULL, &streamSelect, 
					&streamLanguageId, &streamGroup, &pstreamName, NULL, NULL);

				if (hr != S_OK)
					continue;

				// Not audio or subtitles
				if (streamGroup != 1 && streamGroup != 2 && streamGroup != 6590033)
				{
					if (pstreamName != NULL)
						CoTaskMemFree(pstreamName);
					continue;
				}

				// Get language name
				char_t languageName[256];
				if (streamLanguageId == 0 || GetLocaleInfo(streamLanguageId, LOCALE_SLANGUAGE, languageName, 255) == 0)
				{
					tsnprintf_s(languageName, countof(languageName), _TRUNCATE, _l("Undetermined (%ld)"), streamNb);
				}

				char_t streamName[256];
				if (pstreamName != NULL)
				{
					text<char_t>(pstreamName, -1, streamName, 255);
					//wcsncpy(streamName, pstreamName, 255);
				}
				else
					tsnprintf_s(streamName, countof(streamName), _TRUNCATE, _l("Undetermined (%ld)"), streamNb);

				if (streamGroup == 1) // Audio
				{
					if (streamSelect == AMSTREAMSELECTINFO_ENABLED ||
						streamSelect == AMSTREAMSELECTINFO_EXCLUSIVE || 
						streamSelect == (AMSTREAMSELECTINFO_ENABLED | AMSTREAMSELECTINFO_EXCLUSIVE))
						currentAudioStream = streamNb;
					std::pair<long, ffstring> pair = std::make_pair<long, ffstring>(streamNb, streamName);
					AudioStreamsNames.push_back(pair);
					std::pair<long, ffstring> pair2 = std::make_pair<long, ffstring>(streamNb, languageName);
					AudioStreamsLanguageNames.push_back(pair2);
				}
				else // Subtitles
				{
					if (streamSelect == AMSTREAMSELECTINFO_ENABLED ||
						streamSelect == AMSTREAMSELECTINFO_EXCLUSIVE || 
						streamSelect == (AMSTREAMSELECTINFO_ENABLED | AMSTREAMSELECTINFO_EXCLUSIVE))
						currentSubtitleStream = streamNb;
					std::pair<long, ffstring> pair = std::make_pair<long, ffstring>(streamNb, streamName);
					SubtitleStreamsNames.push_back(pair);
					std::pair<long, ffstring> pair2 = std::make_pair<long, ffstring>(streamNb, languageName);
					SubtitleStreamsLanguageNames.push_back(pair2);
				}
				if (pstreamName != NULL)
					CoTaskMemFree(pstreamName);
			}
			
			pAMStreamSelect->Release();
			if (foundHaali)
				break;
		}
	}
	streamsLoaded = true;
}

void Tremote::setStream(long streamNb)
{

	comptr<IEnumFilters> eff;
	IFilterGraph    *m_pGraph = NULL;
	deci->getGraph(&m_pGraph); // Graph we belong to
	if (m_pGraph->EnumFilters(&eff)==S_OK)
	{
		eff->Reset();
		const char_t *fileName = deci->getSourceName();
		for (comptr<IBaseFilter> bff;eff->Next(1,&bff,NULL)==S_OK;bff=NULL)
		{
			char_t name[MAX_PATH],filtername[MAX_PATH];
			getFilterName(bff,name,filtername,countof(filtername));

			IAMStreamSelect *pAMStreamSelect = NULL;
			bff->QueryInterface(IID_IAMStreamSelect, (void**) &pAMStreamSelect);
			if (pAMStreamSelect == NULL)
				continue;
			
			if (foundHaali && strcmp(filtername, fileName))
			{
				pAMStreamSelect->Release();
				continue;
			}
			pAMStreamSelect->Enable(streamNb, AMSTREAMSELECTENABLE_ENABLE);
			pAMStreamSelect->Release();
			break;
		}
	}
}
