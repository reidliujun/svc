#ifndef _TFFPROCBASE_H_
#define _TFFPROCBASE_H_

#include "interfaces.h"

struct TregOpIDstreamWrite;
struct IffdshowParamsEnum;
class TffProcBase
{
private:
 int filtermode;
protected:
 comptrQ<IffdshowBase> deci;comptrQ<IffdshowDec> deciD;
 TffProcBase(int Ifiltermode);
 void initDeci(IUnknown *unk);

 STDMETHODIMP setTempPreset(const char *tempPresetName);
 STDMETHODIMP setActivePreset(const char *presetName);
 STDMETHODIMP loadPreset(const char *presetName);
 STDMETHODIMP saveBytestreamConfig(void *buf,size_t len);
 STDMETHODIMP loadBytestreamConfig(const void *buf,size_t len);
 STDMETHODIMP config(HWND owner);
 STDMETHODIMP getIffDecoder_(/*IffdshowBase*/void* *deciPtr) {return E_NOTIMPL;}
 STDMETHODIMP putStringParam(const char *param,char sep);
 STDMETHODIMP getParamsEnum(IffdshowParamsEnum* *enumPtr);
 STDMETHODIMP notifyParamsChanged(void);
 STDMETHODIMP saveActivePreset(const char *name);
 STDMETHODIMP putParam(unsigned int paramID,int val);
 STDMETHODIMP setBasePageSite(IPropertyPage *page);
public:
 virtual ~TffProcBase();

 void saveConfig(TregOpIDstreamWrite &t);
 void loadConfig(bool notreg,const unsigned char *buf,size_t len);
};

#endif
