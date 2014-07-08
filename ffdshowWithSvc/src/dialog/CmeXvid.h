#ifndef _CMEXVIDPAGE_H_
#define _CMEXVIDPAGE_H_

#include "TconfPageEnc.h"
#include "ffcodecs.h"

class TmeXvidPage :public TconfPageEnc
{
private:
 void me2dlg(void),vhq2dlg(void);
 static const int idCustomVHQs[];
protected:
 virtual INT_PTR msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
public:
 TmeXvidPage(TffdshowPageEnc *Iparent);
 virtual void cfg2dlg(void);
 virtual bool forCodec(int codecId) {return xvid_codec(codecId);}
 virtual bool enabled(void);
 virtual void translate(void);
};

#endif

