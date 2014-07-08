#ifndef _TIMGFILTERKERNELDEINT_H_
#define _TIMGFILTERKERNELDEINT_H_

#include "TimgFilter.h"
/*
DECLARE_FILTER(TimgFilterKernelDeint,public,TimgFilter)
private:
 TffPict prv;unsigned char *prvbuf;unsigned int prvbuflen;
 int n;
protected:
 virtual int getSupportedInputColorspaces(const TfilterSettingsVideo *cfg) const {return FF_CSP_420P|FF_CSP_YUY2|FF_CSP_RGB32;}
 virtual void onSizeChange(void);
public:
 TimgFilterKernelDeint(IffdshowBase *Ideci,Tfilters *Iparent);
 ~TimgFilterKernelDeint();
 virtual void done(void);
 virtual HRESULT process(TfilterQueue::iterator it,TffPict &pict,const TfilterSettingsVideo *cfg0);
 virtual void onSeek(void);
};
*/

#include "TdeinterlaceSettings.h"

class Tdll;
struct IkernelDeint;
DECLARE_FILTER(TimgFilterKernelDeint2,public,TimgFilter)
private:
 bool bob;
 Tdll *dll;
 IkernelDeint* (*createI)(bool IisYV12,unsigned int width,unsigned int height,unsigned int rowsize,int Iorder,int Ithreshold,bool Isharp,bool Itwoway,bool Ilinked,bool Imap,bool Ibob,int cpuflags);
 IkernelDeint *kernel;
 TdeinterlaceSettings oldcfg;
 int oldOrder;
protected:
 virtual bool is(const TffPictBase &pict,const TfilterSettingsVideo *cfg);
 virtual int getSupportedInputColorspaces(const TfilterSettingsVideo *cfg) const {return FF_CSP_420P/*|FF_CSP_YUY2*/;}
 virtual void onSizeChange(void);
public:
 static const char_t *dllname;
 TimgFilterKernelDeint2(IffdshowBase *Ideci,Tfilters *Iparent,bool Ibob=false);
 ~TimgFilterKernelDeint2();
 virtual void done(void);
 virtual HRESULT process(TfilterQueue::iterator it,TffPict &pict,const TfilterSettingsVideo *cfg0);
 virtual void onSeek(void);
};

DECLARE_FILTER(TimgFilterKernelBob,public,TimgFilterKernelDeint2)
public:
 TimgFilterKernelBob(IffdshowBase *Ideci,Tfilters *Iparent):TimgFilterKernelDeint2(Ideci,Iparent,true) {}
};

#endif
