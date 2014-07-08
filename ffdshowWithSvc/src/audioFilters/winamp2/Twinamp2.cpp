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
#include "Twinamp2.h"
#include "Tdll.h"
#include "DSP.H"
#include "Twinamp2settings.h"
#include "ffdebug.h"

//============================== Twinamp2 ====================================
Twinamp2::Twinamp2(const char_t *Iwinampdir)
{
 ff_strncpy(winampdir,Iwinampdir,countof(winampdir));
 strings files;
 char_t mask[MAX_PATH];
 _makepath_s(mask,MAX_PATH,NULL,winampdir,_l("plugins\\dsp*"),_l("dll"));
 findFiles(mask,files);
 for (strings::const_iterator flnm=files.begin();flnm!=files.end();flnm++)
  {
   Twinamp2dspDll *dsp=new Twinamp2dspDll(*flnm);
   if (!dsp->filters.empty())
    dsps.push_back(dsp);
   else
    delete dsp;
  }
}
Twinamp2::~Twinamp2()
{
 for (Twinamp2dspDlls::iterator d=dsps.begin();d!=dsps.end();d++)
  (*d)->release();
}

Twinamp2dspDll* Twinamp2::getFilter(const char_t *flnm)
{
 for (Twinamp2dspDlls::iterator d=dsps.begin();d!=dsps.end();d++)
  if ((*d)->descr==flnm)
   return *d;
 return NULL;
}

Twinamp2dsp* Twinamp2::getFilter(const Twinamp2settings *cfg, int nchannels)
{
 Twinamp2dspDll *d=getFilter(cfg->flnm);
 if (!d) return NULL;
 if (nchannels>2 && !d->isMultichannelAllowed(cfg->allowMultichannelOnlyIn))
   return NULL;
 for (Twinamp2dspDll::Tfilters::iterator f=d->filters.begin();f!=d->filters.end();f++)
  if (strcmp((*f)->descr.c_str(),cfg->modulename)==0)
   {
    return *f;
   }
 return NULL;
}

//=========================== Twinamp2dspDll =================================
Twinamp2dspDll::Twinamp2dspDll(const ffstring &flnm):refcount(1)
{
 dll=NULL;
 winampDSPGetHeaderType=NULL;
 hdr=NULL;
 char_t filename[MAX_PATH],name[MAX_PATH],ext[MAX_PATH];
 _splitpath_s(flnm.c_str(),NULL,0,NULL,0,name,MAX_PATH,ext,MAX_PATH);
 _makepath_s(filename,MAX_PATH,NULL,NULL,name,ext);
 // DSP stacker, Adapt-X and Vst host are not compatible with ffdshow currently. Maybe ffdshow's bug, but I can't help...
 if (   _strnicmp(_l("dsp_stacker.dll"),filename,16)==0
     || _strnicmp(_l("dsp_adaptx.dll"),filename,15)==0
     || _strnicmp(_l("dsp_vst.dll"),filename,12)==0
    )
  return;
 dll=new Tdll(flnm.c_str(),NULL);
 dll->loadFunction(winampDSPGetHeaderType,"winampDSPGetHeader2");
 if (dll->ok)
  {
   hdr=winampDSPGetHeaderType();
   if (hdr->version!=DSP_HDRVER)
    {
     hdr=NULL;
     return;
    }
   descr=hdr->description;
   dllFileName=filename;
  }
 if (hdr)
  for (int i=0;;i++)
   {
    winampDSPModule *flt=hdr->getModule(i);
    if (!flt) break;
    flt->hDllInstance=dll->hdll;
    filters.push_back(new Twinamp2dsp(this,flt));
   }
}
Twinamp2dspDll::~Twinamp2dspDll()
{
 for (Tfilters::iterator f=filters.begin();f!=filters.end();f++)
  delete *f;
 delete dll;
}
void Twinamp2dspDll::addref(void)
{
 refcount++;
 DPRINTFA("Twinamp2dspDll: %s: %i",hdr->description,refcount);
}
void Twinamp2dspDll::release(void)
{
 refcount--;
 DPRINTFA("Twinamp2dspDll: %s: %i",hdr->description,refcount);
 if (refcount==0)
  {
   DPRINTFA("Twinamp2dspDll: deleting: %s",hdr->description);
   delete this;
  }
}

bool Twinamp2dspDll::isMultichannelAllowed(const char_t *compList) const
{
 strings multichannelDlls;
 strtok(compList,_l(";"),multichannelDlls);
 for (strings::const_iterator b=multichannelDlls.begin();b!=multichannelDlls.end();b++)
  if (DwStrcasecmp(*b,dllFileName)==0)
   return true;
 return false;
}
//============================= Twinamp2dsp ==================================
Twinamp2dsp::Twinamp2dsp(Twinamp2dspDll *Idll,winampDSPModule *Imod):mod(Imod),dll(Idll),h(NULL),hThread(NULL)
{
 descr=mod->description;
 inited=0;
}
Twinamp2dsp::~Twinamp2dsp()
{
}
int Twinamp2dsp::init(void)
{
 if (!inited++ && mod->Init)
  {
   addref();
   unsigned threadID;
   hThread=(HANDLE)_beginthreadex(NULL,65536,threadProc,this,NULL,&threadID);
   while (!h) Sleep(20);
   return (h!=(HWND)-1)?1:(h=NULL,0);
  }
 else
  return 0;
}
void Twinamp2dsp::config(HWND parent)
{
 if (mod->Config)
  {
   mod->hwndParent=parent;
   mod->Config(mod);
  }
}
void Twinamp2dsp::done(void)
{
 if (!inited) return;
 inited--;
 if (inited==0 && h && hThread)
  {
   SendMessage(h,WM_CLOSE,0,0);
   WaitForSingleObject(hThread,INFINITE);
   hThread = NULL;
   h = NULL;
   release();
  }
}
void Twinamp2dsp::addref(void)
{
 dll->addref();
}
void Twinamp2dsp::release(void)
{
 dll->release();
}

size_t Twinamp2dsp::process(int16_t *samples,size_t numsamples,int bps,int nch,int srate)
{
 return mod->ModifySamples?mod->ModifySamples(mod,samples,(int)numsamples,bps,nch,srate):0;
}

LRESULT CALLBACK Twinamp2dsp::wndProc(HWND hwnd, UINT msg, WPARAM wprm, LPARAM lprm)
{
 switch (msg)
  {
   case WM_DESTROY:
    PostQuitMessage(0);
    break;
  }
 return DefWindowProc(hwnd,msg,wprm,lprm);
}

unsigned int __stdcall Twinamp2dsp::threadProc(void *self0)
{
 Twinamp2dsp *self=(Twinamp2dsp*)self0;
 static const char_t *FFDSHOW_WINAMP_CLASS=_l("ffdshow_winamp_class");
 randomize();
 setThreadName(DWORD(-1),"winamp2");

 HINSTANCE hi=self->mod->hDllInstance;
 char_t windowName[80];
 tsnprintf_s(windowName, countof(windowName), _TRUNCATE, _l("%s_window%i"), FFDSHOW_WINAMP_CLASS, rand());
 HWND h=createInvisibleWindow(hi,FFDSHOW_WINAMP_CLASS,windowName,wndProc,self,NULL);
 self->mod->hwndParent=h;
 if (self->mod->Init(self->mod)==0)
  {
   self->h=h;
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
  }
 else
  {
   self->h=(HWND)-1;
   DestroyWindow(h);
  }
 self->mod->Quit(self->mod);
 UnregisterClass(FFDSHOW_WINAMP_CLASS,hi);
 _endthreadex(0);
 return 0;
}
