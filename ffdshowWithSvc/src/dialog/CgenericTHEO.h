#ifndef _CGENERICTHEOPAGE_H_
#define _CGENERICTHEOPAGE_H_

#include "TconfPageEnc.h"
#include "ffcodecs.h"

class TgenericTHEOpage :public TconfPageEnc
{
private:
 void kf2dlg(void),scenechange2dlg(void);
protected:
 virtual INT_PTR msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
public:
 TgenericTHEOpage(TffdshowPageEnc *Iparent);
 virtual bool forCodec(int codecId) {return theora_codec(codecId);}
 virtual void init(void);
 virtual void cfg2dlg(void);
};

#endif

