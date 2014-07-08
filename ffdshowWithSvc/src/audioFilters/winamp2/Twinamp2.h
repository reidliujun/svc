#ifndef _TWINAMP2_H_
#define _TWINAMP2_H_

struct winampDSPModule;
class Twinamp2dsp;
class Tdll;
struct winampDSPHeader;

class Twinamp2dspDll
{
private:
 unsigned int refcount;
 Tdll *dll;
 winampDSPHeader *hdr;
 winampDSPHeader* (*winampDSPGetHeaderType)(void);
public:
 Twinamp2dspDll(const ffstring &flnm);
 bool isMultichannelAllowed(const char_t *compList) const;
 ~Twinamp2dspDll();
 void addref(void);
 void release(void);
 ffstring descr;
 ffstring dllFileName;
 typedef std::vector<Twinamp2dsp*> Tfilters;
 Tfilters filters;
};

class Twinamp2dsp
{
private:
 int inited;
 Twinamp2dspDll *dll;
 winampDSPModule *mod;
 HANDLE hThread;
 static unsigned int __stdcall threadProc(void *self0);
 volatile HWND h;
 static LRESULT CALLBACK wndProc(HWND hwnd, UINT msg, WPARAM wprm, LPARAM lprm);
public:
 Twinamp2dsp(Twinamp2dspDll *Idll,winampDSPModule *Imod);
 ~Twinamp2dsp();
 void addref(void);
 void release(void);
 ffstring descr;
 int init(void);
 void config(HWND parent);
 size_t process(int16_t *samples,size_t numsamples,int bps,int nch,int srate);
 void done(void);
 bool isMultichannelAllowed(const char_t *compList) const {return dll?dll->isMultichannelAllowed(compList):false;}
};

struct Twinamp2settings;
class Twinamp2
{
public:
 Twinamp2(const char_t *Iwinampdir);
 ~Twinamp2();
 char_t winampdir[MAX_PATH];
 typedef std::vector<Twinamp2dspDll*> Twinamp2dspDlls;
 Twinamp2dspDlls dsps;
 Twinamp2dspDll* getFilter(const char_t *flnm);
 Twinamp2dsp* getFilter(const Twinamp2settings *cfg,int nchannels);
};

#endif
