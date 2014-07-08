#ifndef _TOSDPROVIDER_H_
#define _TOSDPROVIDER_H_

#include "IOSDprovider.h"
#include "interfaces.h"

struct TOSDsettings;
class TOSDprovider : public IOSDprovider
{
private:
 comptr<IffdshowBase> &_deci;comptrQ<IffdshowDec> &_deciD;
public:
 const TOSDsettings *tempcfg;

 TOSDprovider(comptr<IffdshowBase> &Ideci,comptrQ<IffdshowDec> &IdeciD):_deci(Ideci),_deciD(IdeciD),tempcfg(NULL) {}
 STDMETHODIMP_(const char_t*) getInfoItemName(int type);
 STDMETHODIMP getInfoItemValue(int id,const char_t* *value,int *wasChange,int *splitline);
 STDMETHODIMP_(const char_t*) getStartupFormat(int *duration);
 STDMETHODIMP_(const char_t*) getFormat(void);
 STDMETHODIMP_(bool) isOSD(void);
 STDMETHODIMP getSave(int *isSave,const char_t* *flnm);
 STDMETHODIMP_(int) getInfoShortcutItem(const char_t *s,int *toklen);
};

#endif
