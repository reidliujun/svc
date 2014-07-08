
#ifndef _CGENERICWMV9PAGE_H_
#define _CGENERICWMV9PAGE_H_

#include "TconfPageEnc.h"

struct TencoderWmv9;
class TgenericWmv9page :public TconfPageEnc
{
private:
 const TencoderWmv9 *enc;
protected:
 virtual INT_PTR msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
public:
 TgenericWmv9page(TffdshowPageEnc *Iparent);
 virtual bool enabled(void);
 virtual bool forCodec(int codecId) {return wmv9_codec(codecId);}
 virtual void cfg2dlg(void);
};

#endif

