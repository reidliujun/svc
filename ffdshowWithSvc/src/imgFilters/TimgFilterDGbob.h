#ifndef _TIMGFILTERDGBOB_H_
#define _TIMGFILTERDGBOB_H_

#include "TimgFilter.h"

DECLARE_FILTER(TimgFilterDGbob,public,TimgFilter)
private:
 struct TtempPict
  {
   void done(void) {buf.clear();}
   TffPict p;Tbuffer buf;
  } *picts0[5],**picts;
 enum
  {
   PRVPRV=-2,
   PRV=-1,
   SRC=0,
   NXT=1,
   NXTNXT=2
  };
 int n,order;
 int do_deinterlace;
protected:
 virtual int getSupportedInputColorspaces(const TfilterSettingsVideo *cfg) const {return FF_CSP_420P|FF_CSP_YUY2|FF_CSP_RGB32;}
 virtual void onSizeChange(void);
public:
 TimgFilterDGbob(IffdshowBase *Ideci,Tfilters *Iparent);
 ~TimgFilterDGbob();
 virtual void done(void);
 virtual HRESULT process(TfilterQueue::iterator it,TffPict &pict,const TfilterSettingsVideo *cfg0);
 virtual void onSeek(void);
};

#endif
